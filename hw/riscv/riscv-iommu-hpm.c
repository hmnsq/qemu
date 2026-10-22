/*
 * RISC-V IOMMU - Hardware Performance Monitor (HPM) helpers
 *
 * Copyright (C) 2022-2023 Rivos Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/timer.h"
#ಅನ್ತರ್ಭಾವ "cpu_bits.h"
#ಅನ್ತರ್ಭಾವ "riscv-iommu-hpm.h"
#ಅನ್ತರ್ಭಾವ "riscv-iommu.h"
#ಅನ್ತರ್ಭಾವ "riscv-iommu-bits.h"
#ಅನ್ತರ್ಭಾವ "trace.h"

/* For now we assume IOMMU HPM frequency to be 1GHz so 1-cycle is of 1-ns. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t get_ಚಕ್ರಗಳು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

uint64_t riscv_iommu_hpmcycle_read(RISCVIOMMUState *s)
{
    ಸ್ಥಿರಾಂಕ uint64_t ಚಕ್ರ = riscv_iommu_reg_get64(
        s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ಸ್ಥಿರಾಂಕ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    ಸ್ಥಿರಾಂಕ uint64_t ctr_prev = s->hpmcycle_prev;
    ಸ್ಥಿರಾಂಕ uint64_t ctr_val = s->hpmcycle_val;

    trace_riscv_iommu_hpm_read(ಚಕ್ರ, inhibit, ctr_prev, ctr_val);

    ಆದರೆ (get_ಕ್ಷೇತ್ರ(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Counter should not increment if inhibit bit is set. We can't really
         * stop the QEMU_CLOCK_VIRTUAL, so we just return the last updated
         * counter value to indicate that counter was not incremented.
         */
        ಹಿಂತಿರುಗಿ (ctr_val & RISCV_IOMMU_IOHPMCYCLES_COUNTER) |
               (ಚಕ್ರ & RISCV_IOMMU_IOHPMCYCLES_OVF);
    }

    ಹಿಂತಿರುಗಿ (ctr_val + get_ಚಕ್ರಗಳು() - ctr_prev) |
        (ಚಕ್ರ & RISCV_IOMMU_IOHPMCYCLES_OVF);
}

ಸ್ಥಿರ ಶೂನ್ಯ hpm_incr_ctr(RISCVIOMMUState *s, uint32_t ctr_idx)
{
    ಸ್ಥಿರಾಂಕ uint32_t off = ctr_idx << 3;
    uint64_t cntr_val;

    cntr_val = ldq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_ತಳ + off]);
    stq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_ತಳ + off], cntr_val + 1);

    trace_riscv_iommu_hpm_incr_ctr(cntr_val);

    /* Handle the overflow scenario. */
    ಆದರೆ (cntr_val == UINT64_MAX) {
        /*
         * Generate interrupt only if OF bit is clear. +1 to offset the cycle
         * register OF bit.
         */
        ಸ್ಥಿರಾಂಕ uint32_t ovf =
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF,
                                  ಬಿಟ್(ctr_idx + 1), 0);
        ಆದರೆ (!get_ಕ್ಷೇತ್ರ(ovf, ಬಿಟ್(ctr_idx + 1))) {
            riscv_iommu_reg_mod64(s,
                                  RISCV_IOMMU_REG_IOHPMEVT_ತಳ + off,
                                  RISCV_IOMMU_IOHPMEVT_ದ,
                                  0);
            riscv_iommu_notify(s, RISCV_IOMMU_INTR_PM);
        }
    }
}

ಶೂನ್ಯ riscv_iommu_hpm_incr_ctr(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
                              ಚಿಹ್ನೆರಹಿತ event_id)
{
    ಸ್ಥಿರಾಂಕ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t did_gscid;
    uint32_t pid_pscid;
    uint32_t ctr_idx;
    gpointer ಮೌಲ್ಯ;
    uint32_t ctrs;
    uint64_t evt;

    ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_HPM)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಮೌಲ್ಯ = g_hash_ಕೋಷ್ಟಕ_lookup(s->hpm_event_ctr_map,
                                GUINT_ಗಮ್ಯ_ಸೂಚಕ(event_id));
    ಆದರೆ (ಮೌಲ್ಯ == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (ctrs = GPOINTER_ಗಮ್ಯ_UINT(ಮೌಲ್ಯ); ctrs != 0; ctrs &= ctrs - 1) {
        ctr_idx = ctz32(ctrs);
        ಆದರೆ (get_ಕ್ಷೇತ್ರ(inhibit, ಬಿಟ್(ctr_idx + 1))) {
            ಮುಂದುವರಿ;
        }

        evt = riscv_iommu_reg_get64(s,
            RISCV_IOMMU_REG_IOHPMEVT_ತಳ + (ctr_idx << 3));

        /*
         * It's quite possible that event ID has been changed in counter
         * but hashtable hasn't been updated yet. We don't want to increment
         * counter for the old event ID.
         */
        ಆದರೆ (event_id != get_ಕ್ಷೇತ್ರ(evt, RISCV_IOMMU_IOHPMEVT_EVENT_ID)) {
            ಮುಂದುವರಿ;
        }

        ಆದರೆ (get_ಕ್ಷೇತ್ರ(evt, RISCV_IOMMU_IOHPMEVT_IDT)) {
            did_gscid = get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
            pid_pscid = get_ಕ್ಷೇತ್ರ(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        } ಇಲ್ಲದಿದ್ದರೆ {
            did_gscid = ctx->devid;
            pid_pscid = ctx->process_id;
        }

        ಆದರೆ (get_ಕ್ಷೇತ್ರ(evt, RISCV_IOMMU_IOHPMEVT_PV_PSCV)) {
            /*
             * If the transaction does not have a valid process_id, counter
             * increments if device_id matches DID_GSCID. If the transaction
             * has a valid process_id, counter increments if device_id
             * matches DID_GSCID and process_id matches PID_PSCID. See
             * IOMMU Specification, Chapter 5.23. Performance-monitoring
             * event selector.
             */
            ಆದರೆ (ctx->process_id &&
                get_ಕ್ಷೇತ್ರ(evt, RISCV_IOMMU_IOHPMEVT_PID_PSCID) != pid_pscid) {
                ಮುಂದುವರಿ;
            }
        }

        ಆದರೆ (get_ಕ್ಷೇತ್ರ(evt, RISCV_IOMMU_IOHPMEVT_DV_GSCV)) {
            uint32_t ಮುಖವಾಡ = ~0;

            ಆದರೆ (get_ಕ್ಷೇತ್ರ(evt, RISCV_IOMMU_IOHPMEVT_DMASK)) {
                /*
                 * 1001 1011   mask = GSCID
                 * 0000 0111   mask = mask ^ (mask + 1)
                 * 1111 1000   mask = ~mask;
                 */
                ಮುಖವಾಡ = get_ಕ್ಷೇತ್ರ(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID);
                ಮುಖವಾಡ = ಮುಖವಾಡ ^ (ಮುಖವಾಡ + 1);
                ಮುಖವಾಡ = ~ಮುಖವಾಡ;
            }

            ಆದರೆ ((get_ಕ್ಷೇತ್ರ(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID) & ಮುಖವಾಡ) !=
                (did_gscid & ಮುಖವಾಡ)) {
                ಮುಂದುವರಿ;
            }
        }

        hpm_incr_ctr(s, ctr_idx);
    }
}

/* Timer callback for cycle counter overflow. */
ಶೂನ್ಯ riscv_iommu_hpm_timer_cb(ಶೂನ್ಯ *priv)
{
    RISCVIOMMUState *s = priv;
    ಸ್ಥಿರಾಂಕ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t ovf;

    ಆದರೆ (get_ಕ್ಷೇತ್ರ(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (s->irq_ಅತಿಪ್ರವಾಹ_ಎಡ > 0) {
        uint64_t irq_trigger_at =
            qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->irq_ಅತಿಪ್ರವಾಹ_ಎಡ;
        timer_mod_anticipate_ns(s->hpm_timer, irq_trigger_at);
        s->irq_ಅತಿಪ್ರವಾಹ_ಎಡ = 0;
        ಹಿಂತಿರುಗಿ;
    }

    ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    ಆದರೆ (!get_ಕ್ಷೇತ್ರ(ovf, RISCV_IOMMU_IOCOUNTOVF_CY)) {
        /*
         * We don't need to set hpmcycle_val to zero and update hpmcycle_prev to
         * current clock value. The way we calculate iohpmcycs will overflow
         * and return the correct value. This avoids the need to synchronize
         * timer callback and write callback.
         */
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF,
            RISCV_IOMMU_IOCOUNTOVF_CY, 0);
        riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_IOHPMCYCLES,
            RISCV_IOMMU_IOHPMCYCLES_OVF, 0);
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_PM);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ hpm_ಸಿದ್ಧತೆ_timer(RISCVIOMMUState *s, uint64_t ಮೌಲ್ಯ)
{
    ಸ್ಥಿರಾಂಕ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint64_t ಅತಿಪ್ರವಾಹ_at, ಅತಿಪ್ರವಾಹ_ns;

    ಆದರೆ (get_ಕ್ಷೇತ್ರ(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        ಹಿಂತಿರುಗಿ;
    }

    /*
     * We are using INT64_MAX here instead to UINT64_MAX because cycle counter
     * has 63-bit precision and INT64_MAX is the maximum it can store.
     */
    ಆದರೆ (ಮೌಲ್ಯ) {
        ಅತಿಪ್ರವಾಹ_ns = INT64_MAX - ಮೌಲ್ಯ + 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಅತಿಪ್ರವಾಹ_ns = INT64_MAX;
    }

    ಅತಿಪ್ರವಾಹ_at = (uint64_t)qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + ಅತಿಪ್ರವಾಹ_ns;
    s->irq_ಅತಿಪ್ರವಾಹ_ಎಡ = 0;

    ಆದರೆ (ಅತಿಪ್ರವಾಹ_at > INT64_MAX) {
        s->irq_ಅತಿಪ್ರವಾಹ_ಎಡ = ಅತಿಪ್ರವಾಹ_at - INT64_MAX;
        ಅತಿಪ್ರವಾಹ_at = INT64_MAX;
    }

    timer_mod_anticipate_ns(s->hpm_timer, ಅತಿಪ್ರವಾಹ_at);
}

/* Updates the internal cycle counter state when iocntinh:CY is changed. */
ಶೂನ್ಯ riscv_iommu_process_iocntinh_cy(RISCVIOMMUState *s, bool prev_cy_inh)
{
    ಸ್ಥಿರಾಂಕ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);

    /* We only need to process CY bit toggle. */
    bool cy_inh = !!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY);
    ಆದರೆ (cy_inh == prev_cy_inh) {
        ಹಿಂತಿರುಗಿ;
    }

    trace_riscv_iommu_hpm_iocntinh_cy(prev_cy_inh);

    ಆದರೆ (!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Cycle counter is enabled. Just start the timer again and update
         * the clock snapshot value to point to the current time to make
         * sure iohpmcycles read is correct.
         */
        s->hpmcycle_prev = get_ಚಕ್ರಗಳು();
        hpm_ಸಿದ್ಧತೆ_timer(s, s->hpmcycle_val);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * Cycle counter is disabled. Stop the timer and update the cycle
         * counter to record the current value which is last programmed
         * value + the cycles passed so far.
         */
        s->hpmcycle_val = s->hpmcycle_val + (get_ಚಕ್ರಗಳು() - s->hpmcycle_prev);
        timer_del(s->hpm_timer);
    }
}

ಶೂನ್ಯ riscv_iommu_process_hpmcycle_write(RISCVIOMMUState *s)
{
    ಸ್ಥಿರಾಂಕ uint64_t val = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ಸ್ಥಿರಾಂಕ uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);

    trace_riscv_iommu_hpm_ಚಕ್ರ_write(ovf, val);

    /*
     * Clear OF bit in IOCNTOVF if it's being cleared in IOHPMCYCLES register.
     */
    ಆದರೆ (get_ಕ್ಷೇತ್ರ(ovf, RISCV_IOMMU_IOCOUNTOVF_CY) &&
        !get_ಕ್ಷೇತ್ರ(val, RISCV_IOMMU_IOHPMCYCLES_OVF)) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF, 0,
            RISCV_IOMMU_IOCOUNTOVF_CY);
    }

    s->hpmcycle_val = val & ~RISCV_IOMMU_IOHPMCYCLES_OVF;
    s->hpmcycle_prev = get_ಚಕ್ರಗಳು();
    hpm_ಸಿದ್ಧತೆ_timer(s, s->hpmcycle_val);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool check_valid_event_id(ಚಿಹ್ನೆರಹಿತ event_id)
{
    ಹಿಂತಿರುಗಿ event_id > RISCV_IOMMU_HPMEVENT_INVALID &&
           event_id < RISCV_IOMMU_HPMEVENT_MAX;
}

ಸ್ಥಿರ gboolean hpm_event_equal(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ, gpointer udata)
{
    uint32_t *pair = udata;

    ಆದರೆ (GPOINTER_ಗಮ್ಯ_UINT(ಮೌಲ್ಯ) & (1 << pair[0])) {
        pair[1] = GPOINTER_ಗಮ್ಯ_UINT(ಕೀಲಿ);
        ಹಿಂತಿರುಗಿ true;
    }

    ಹಿಂತಿರುಗಿ false;
}

/* Caller must check ctr_idx against hpm_ctrs to see if its supported or not. */
ಸ್ಥಿರ ಶೂನ್ಯ update_event_map(RISCVIOMMUState *s, uint64_t ಮೌಲ್ಯ,
                             uint32_t ctr_idx)
{
    ಚಿಹ್ನೆರಹಿತ event_id = get_ಕ್ಷೇತ್ರ(ಮೌಲ್ಯ, RISCV_IOMMU_IOHPMEVT_EVENT_ID);
    uint32_t pair[2] = { ctr_idx, RISCV_IOMMU_HPMEVENT_INVALID };
    uint32_t new_ಮೌಲ್ಯ = 1 << ctr_idx;
    gpointer ದತ್ತಾಂಶ;

    /*
     * If EventID field is RISCV_IOMMU_HPMEVENT_INVALID
     * remove the current mapping.
     */
    ಆದರೆ (event_id == RISCV_IOMMU_HPMEVENT_INVALID) {
        ದತ್ತಾಂಶ = g_hash_ಕೋಷ್ಟಕ_ಕಂಡುಹಿಡಿ(s->hpm_event_ctr_map, hpm_event_equal, pair);

        new_ಮೌಲ್ಯ = GPOINTER_ಗಮ್ಯ_UINT(ದತ್ತಾಂಶ) & ~(new_ಮೌಲ್ಯ);
        ಆದರೆ (new_ಮೌಲ್ಯ != 0) {
            g_hash_ಕೋಷ್ಟಕ_replace(s->hpm_event_ctr_map,
                                 GUINT_ಗಮ್ಯ_ಸೂಚಕ(pair[1]),
                                 GUINT_ಗಮ್ಯ_ಸೂಚಕ(new_ಮೌಲ್ಯ));
        } ಇಲ್ಲದಿದ್ದರೆ {
            g_hash_ಕೋಷ್ಟಕ_remove(s->hpm_event_ctr_map,
                                GUINT_ಗಮ್ಯ_ಸೂಚಕ(pair[1]));
        }

        ಹಿಂತಿರುಗಿ;
    }

    /* Update the counter mask if the event is already enabled. */
    ಆದರೆ (g_hash_ಕೋಷ್ಟಕ_lookup_extended(s->hpm_event_ctr_map,
                                     GUINT_ಗಮ್ಯ_ಸೂಚಕ(event_id),
                                     ಖಾಲಿ,
                                     &ದತ್ತಾಂಶ)) {
        new_ಮೌಲ್ಯ |= GPOINTER_ಗಮ್ಯ_UINT(ದತ್ತಾಂಶ);
    }

    g_hash_ಕೋಷ್ಟಕ_ಸೇರಿಸು(s->hpm_event_ctr_map,
                        GUINT_ಗಮ್ಯ_ಸೂಚಕ(event_id),
                        GUINT_ಗಮ್ಯ_ಸೂಚಕ(new_ಮೌಲ್ಯ));
}

ಶೂನ್ಯ riscv_iommu_process_hpmevt_write(RISCVIOMMUState *s, uint32_t evt_reg)
{
    ಸ್ಥಿರಾಂಕ uint32_t ctr_idx = (evt_reg - RISCV_IOMMU_REG_IOHPMEVT_ತಳ) >> 3;
    ಸ್ಥಿರಾಂಕ uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    uint64_t val = riscv_iommu_reg_get64(s, evt_reg);

    ಆದರೆ (ctr_idx >= s->hpm_cntrs) {
        ಹಿಂತಿರುಗಿ;
    }

    trace_riscv_iommu_hpm_evt_write(ctr_idx, ovf, val);

    /* Clear OF bit in IOCNTOVF if it's being cleared in IOHPMEVT register. */
    ಆದರೆ (get_ಕ್ಷೇತ್ರ(ovf, ಬಿಟ್(ctr_idx + 1)) &&
        !get_ಕ್ಷೇತ್ರ(val, RISCV_IOMMU_IOHPMEVT_ದ)) {
        /* +1 to offset CYCLE register OF bit. */
        riscv_iommu_reg_mod32(
            s, RISCV_IOMMU_REG_IOCOUNTOVF, 0, ಬಿಟ್(ctr_idx + 1));
    }

    ಆದರೆ (!check_valid_event_id(get_ಕ್ಷೇತ್ರ(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID))) {
        /* Reset EventID (WARL) field to invalid. */
        val = ಗಣ_ಕ್ಷೇತ್ರ(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID,
            RISCV_IOMMU_HPMEVENT_INVALID);
        riscv_iommu_reg_set64(s, evt_reg, val);
    }

    update_event_map(s, val, ctr_idx);
}
