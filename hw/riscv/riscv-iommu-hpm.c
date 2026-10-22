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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/timer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cpu_bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu-hpm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu-bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "trace.h"

/* For now we assume IOMMU HPM frequency to be 1GHz so 1-cycle is of 1-ns. */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t get_ਚੱਕਰਾਂ(ਖਾਲੀ)
{
    ਵਾਪਸ qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

uint64_t riscv_iommu_hpmcycle_read(RISCVIOMMUState *s)
{
    ਅਚਲ uint64_t ਚੱਕਰ = riscv_iommu_reg_get64(
        s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ਅਚਲ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    ਅਚਲ uint64_t ctr_prev = s->hpmcycle_prev;
    ਅਚਲ uint64_t ctr_val = s->hpmcycle_val;

    trace_riscv_iommu_hpm_read(ਚੱਕਰ, inhibit, ctr_prev, ctr_val);

    ਜੇ (get_ਖੇਤਰ(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Counter should not increment if inhibit bit is set. We can't really
         * stop the QEMU_CLOCK_VIRTUAL, so we just return the last updated
         * counter value to indicate that counter was not incremented.
         */
        ਵਾਪਸ (ctr_val & RISCV_IOMMU_IOHPMCYCLES_COUNTER) |
               (ਚੱਕਰ & RISCV_IOMMU_IOHPMCYCLES_OVF);
    }

    ਵਾਪਸ (ctr_val + get_ਚੱਕਰਾਂ() - ctr_prev) |
        (ਚੱਕਰ & RISCV_IOMMU_IOHPMCYCLES_OVF);
}

ਸਥਿਰ ਖਾਲੀ hpm_incr_ctr(RISCVIOMMUState *s, uint32_t ctr_idx)
{
    ਅਚਲ uint32_t off = ctr_idx << 3;
    uint64_t cntr_val;

    cntr_val = ldq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_ਅਧਾਰ + off]);
    stq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_ਅਧਾਰ + off], cntr_val + 1);

    trace_riscv_iommu_hpm_incr_ctr(cntr_val);

    /* Handle the overflow scenario. */
    ਜੇ (cntr_val == UINT64_MAX) {
        /*
         * Generate interrupt only if OF bit is clear. +1 to offset the cycle
         * register OF bit.
         */
        ਅਚਲ uint32_t ovf =
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF,
                                  ਦੋਆਧਾਰੀ_ਅੰਕ(ctr_idx + 1), 0);
        ਜੇ (!get_ਖੇਤਰ(ovf, ਦੋਆਧਾਰੀ_ਅੰਕ(ctr_idx + 1))) {
            riscv_iommu_reg_mod64(s,
                                  RISCV_IOMMU_REG_IOHPMEVT_ਅਧਾਰ + off,
                                  RISCV_IOMMU_IOHPMEVT_ਦਾ,
                                  0);
            riscv_iommu_notify(s, RISCV_IOMMU_INTR_PM);
        }
    }
}

ਖਾਲੀ riscv_iommu_hpm_incr_ctr(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
                              ਬਿਨਾਨਿਸ਼ਾਨੀ event_id)
{
    ਅਚਲ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t did_gscid;
    uint32_t pid_pscid;
    uint32_t ctr_idx;
    gpointer ਮੁੱਲ;
    uint32_t ctrs;
    uint64_t evt;

    ਜੇ (!(s->cap & RISCV_IOMMU_CAP_HPM)) {
        ਵਾਪਸ;
    }

    ਮੁੱਲ = g_hash_ਸਾਰਣੀ_lookup(s->hpm_event_ctr_map,
                                GUINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(event_id));
    ਜੇ (ਮੁੱਲ == ਨਿਰਾ) {
        ਵਾਪਸ;
    }

    ਲਈ (ctrs = GPOINTER_ਮੰਜ਼ਿਲ_UINT(ਮੁੱਲ); ctrs != 0; ctrs &= ctrs - 1) {
        ctr_idx = ctz32(ctrs);
        ਜੇ (get_ਖੇਤਰ(inhibit, ਦੋਆਧਾਰੀ_ਅੰਕ(ctr_idx + 1))) {
            ਜਾਰੀ;
        }

        evt = riscv_iommu_reg_get64(s,
            RISCV_IOMMU_REG_IOHPMEVT_ਅਧਾਰ + (ctr_idx << 3));

        /*
         * It's quite possible that event ID has been changed in counter
         * but hashtable hasn't been updated yet. We don't want to increment
         * counter for the old event ID.
         */
        ਜੇ (event_id != get_ਖੇਤਰ(evt, RISCV_IOMMU_IOHPMEVT_EVENT_ID)) {
            ਜਾਰੀ;
        }

        ਜੇ (get_ਖੇਤਰ(evt, RISCV_IOMMU_IOHPMEVT_IDT)) {
            did_gscid = get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
            pid_pscid = get_ਖੇਤਰ(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        } ਨਹੀਂਤਾਂ {
            did_gscid = ctx->devid;
            pid_pscid = ctx->process_id;
        }

        ਜੇ (get_ਖੇਤਰ(evt, RISCV_IOMMU_IOHPMEVT_PV_PSCV)) {
            /*
             * If the transaction does not have a valid process_id, counter
             * increments if device_id matches DID_GSCID. If the transaction
             * has a valid process_id, counter increments if device_id
             * matches DID_GSCID and process_id matches PID_PSCID. See
             * IOMMU Specification, Chapter 5.23. Performance-monitoring
             * event selector.
             */
            ਜੇ (ctx->process_id &&
                get_ਖੇਤਰ(evt, RISCV_IOMMU_IOHPMEVT_PID_PSCID) != pid_pscid) {
                ਜਾਰੀ;
            }
        }

        ਜੇ (get_ਖੇਤਰ(evt, RISCV_IOMMU_IOHPMEVT_DV_GSCV)) {
            uint32_t ਨਕਾਬ = ~0;

            ਜੇ (get_ਖੇਤਰ(evt, RISCV_IOMMU_IOHPMEVT_DMASK)) {
                /*
                 * 1001 1011   mask = GSCID
                 * 0000 0111   mask = mask ^ (mask + 1)
                 * 1111 1000   mask = ~mask;
                 */
                ਨਕਾਬ = get_ਖੇਤਰ(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID);
                ਨਕਾਬ = ਨਕਾਬ ^ (ਨਕਾਬ + 1);
                ਨਕਾਬ = ~ਨਕਾਬ;
            }

            ਜੇ ((get_ਖੇਤਰ(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID) & ਨਕਾਬ) !=
                (did_gscid & ਨਕਾਬ)) {
                ਜਾਰੀ;
            }
        }

        hpm_incr_ctr(s, ctr_idx);
    }
}

/* Timer callback for cycle counter overflow. */
ਖਾਲੀ riscv_iommu_hpm_timer_cb(ਖਾਲੀ *priv)
{
    RISCVIOMMUState *s = priv;
    ਅਚਲ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t ovf;

    ਜੇ (get_ਖੇਤਰ(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        ਵਾਪਸ;
    }

    ਜੇ (s->irq_ਉਛਾਲ_ਖੱਬਾ > 0) {
        uint64_t irq_trigger_at =
            qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->irq_ਉਛਾਲ_ਖੱਬਾ;
        timer_mod_anticipate_ns(s->hpm_timer, irq_trigger_at);
        s->irq_ਉਛਾਲ_ਖੱਬਾ = 0;
        ਵਾਪਸ;
    }

    ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    ਜੇ (!get_ਖੇਤਰ(ovf, RISCV_IOMMU_IOCOUNTOVF_CY)) {
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

ਸਥਿਰ ਖਾਲੀ hpm_ਤਿਆਰੀ_timer(RISCVIOMMUState *s, uint64_t ਮੁੱਲ)
{
    ਅਚਲ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint64_t ਉਛਾਲ_at, ਉਛਾਲ_ns;

    ਜੇ (get_ਖੇਤਰ(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        ਵਾਪਸ;
    }

    /*
     * We are using INT64_MAX here instead to UINT64_MAX because cycle counter
     * has 63-bit precision and INT64_MAX is the maximum it can store.
     */
    ਜੇ (ਮੁੱਲ) {
        ਉਛਾਲ_ns = INT64_MAX - ਮੁੱਲ + 1;
    } ਨਹੀਂਤਾਂ {
        ਉਛਾਲ_ns = INT64_MAX;
    }

    ਉਛਾਲ_at = (uint64_t)qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + ਉਛਾਲ_ns;
    s->irq_ਉਛਾਲ_ਖੱਬਾ = 0;

    ਜੇ (ਉਛਾਲ_at > INT64_MAX) {
        s->irq_ਉਛਾਲ_ਖੱਬਾ = ਉਛਾਲ_at - INT64_MAX;
        ਉਛਾਲ_at = INT64_MAX;
    }

    timer_mod_anticipate_ns(s->hpm_timer, ਉਛਾਲ_at);
}

/* Updates the internal cycle counter state when iocntinh:CY is changed. */
ਖਾਲੀ riscv_iommu_process_iocntinh_cy(RISCVIOMMUState *s, bool prev_cy_inh)
{
    ਅਚਲ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);

    /* We only need to process CY bit toggle. */
    bool cy_inh = !!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY);
    ਜੇ (cy_inh == prev_cy_inh) {
        ਵਾਪਸ;
    }

    trace_riscv_iommu_hpm_iocntinh_cy(prev_cy_inh);

    ਜੇ (!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Cycle counter is enabled. Just start the timer again and update
         * the clock snapshot value to point to the current time to make
         * sure iohpmcycles read is correct.
         */
        s->hpmcycle_prev = get_ਚੱਕਰਾਂ();
        hpm_ਤਿਆਰੀ_timer(s, s->hpmcycle_val);
    } ਨਹੀਂਤਾਂ {
        /*
         * Cycle counter is disabled. Stop the timer and update the cycle
         * counter to record the current value which is last programmed
         * value + the cycles passed so far.
         */
        s->hpmcycle_val = s->hpmcycle_val + (get_ਚੱਕਰਾਂ() - s->hpmcycle_prev);
        timer_del(s->hpm_timer);
    }
}

ਖਾਲੀ riscv_iommu_process_hpmcycle_write(RISCVIOMMUState *s)
{
    ਅਚਲ uint64_t val = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ਅਚਲ uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);

    trace_riscv_iommu_hpm_ਚੱਕਰ_write(ovf, val);

    /*
     * Clear OF bit in IOCNTOVF if it's being cleared in IOHPMCYCLES register.
     */
    ਜੇ (get_ਖੇਤਰ(ovf, RISCV_IOMMU_IOCOUNTOVF_CY) &&
        !get_ਖੇਤਰ(val, RISCV_IOMMU_IOHPMCYCLES_OVF)) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF, 0,
            RISCV_IOMMU_IOCOUNTOVF_CY);
    }

    s->hpmcycle_val = val & ~RISCV_IOMMU_IOHPMCYCLES_OVF;
    s->hpmcycle_prev = get_ਚੱਕਰਾਂ();
    hpm_ਤਿਆਰੀ_timer(s, s->hpmcycle_val);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool check_valid_event_id(ਬਿਨਾਨਿਸ਼ਾਨੀ event_id)
{
    ਵਾਪਸ event_id > RISCV_IOMMU_HPMEVENT_INVALID &&
           event_id < RISCV_IOMMU_HPMEVENT_MAX;
}

ਸਥਿਰ gboolean hpm_event_equal(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ, gpointer udata)
{
    uint32_t *pair = udata;

    ਜੇ (GPOINTER_ਮੰਜ਼ਿਲ_UINT(ਮੁੱਲ) & (1 << pair[0])) {
        pair[1] = GPOINTER_ਮੰਜ਼ਿਲ_UINT(ਕੁੰਜੀ);
        ਵਾਪਸ true;
    }

    ਵਾਪਸ false;
}

/* Caller must check ctr_idx against hpm_ctrs to see if its supported or not. */
ਸਥਿਰ ਖਾਲੀ update_event_map(RISCVIOMMUState *s, uint64_t ਮੁੱਲ,
                             uint32_t ctr_idx)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ event_id = get_ਖੇਤਰ(ਮੁੱਲ, RISCV_IOMMU_IOHPMEVT_EVENT_ID);
    uint32_t pair[2] = { ctr_idx, RISCV_IOMMU_HPMEVENT_INVALID };
    uint32_t new_ਮੁੱਲ = 1 << ctr_idx;
    gpointer ਅੰਕੜੇ;

    /*
     * If EventID field is RISCV_IOMMU_HPMEVENT_INVALID
     * remove the current mapping.
     */
    ਜੇ (event_id == RISCV_IOMMU_HPMEVENT_INVALID) {
        ਅੰਕੜੇ = g_hash_ਸਾਰਣੀ_ਲੱਭੋ(s->hpm_event_ctr_map, hpm_event_equal, pair);

        new_ਮੁੱਲ = GPOINTER_ਮੰਜ਼ਿਲ_UINT(ਅੰਕੜੇ) & ~(new_ਮੁੱਲ);
        ਜੇ (new_ਮੁੱਲ != 0) {
            g_hash_ਸਾਰਣੀ_replace(s->hpm_event_ctr_map,
                                 GUINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(pair[1]),
                                 GUINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(new_ਮੁੱਲ));
        } ਨਹੀਂਤਾਂ {
            g_hash_ਸਾਰਣੀ_remove(s->hpm_event_ctr_map,
                                GUINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(pair[1]));
        }

        ਵਾਪਸ;
    }

    /* Update the counter mask if the event is already enabled. */
    ਜੇ (g_hash_ਸਾਰਣੀ_lookup_extended(s->hpm_event_ctr_map,
                                     GUINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(event_id),
                                     ਨਿਰਾ,
                                     &ਅੰਕੜੇ)) {
        new_ਮੁੱਲ |= GPOINTER_ਮੰਜ਼ਿਲ_UINT(ਅੰਕੜੇ);
    }

    g_hash_ਸਾਰਣੀ_ਪਾਓ(s->hpm_event_ctr_map,
                        GUINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(event_id),
                        GUINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(new_ਮੁੱਲ));
}

ਖਾਲੀ riscv_iommu_process_hpmevt_write(RISCVIOMMUState *s, uint32_t evt_reg)
{
    ਅਚਲ uint32_t ctr_idx = (evt_reg - RISCV_IOMMU_REG_IOHPMEVT_ਅਧਾਰ) >> 3;
    ਅਚਲ uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    uint64_t val = riscv_iommu_reg_get64(s, evt_reg);

    ਜੇ (ctr_idx >= s->hpm_cntrs) {
        ਵਾਪਸ;
    }

    trace_riscv_iommu_hpm_evt_write(ctr_idx, ovf, val);

    /* Clear OF bit in IOCNTOVF if it's being cleared in IOHPMEVT register. */
    ਜੇ (get_ਖੇਤਰ(ovf, ਦੋਆਧਾਰੀ_ਅੰਕ(ctr_idx + 1)) &&
        !get_ਖੇਤਰ(val, RISCV_IOMMU_IOHPMEVT_ਦਾ)) {
        /* +1 to offset CYCLE register OF bit. */
        riscv_iommu_reg_mod32(
            s, RISCV_IOMMU_REG_IOCOUNTOVF, 0, ਦੋਆਧਾਰੀ_ਅੰਕ(ctr_idx + 1));
    }

    ਜੇ (!check_valid_event_id(get_ਖੇਤਰ(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID))) {
        /* Reset EventID (WARL) field to invalid. */
        val = ਸੈੱਟ_ਖੇਤਰ(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID,
            RISCV_IOMMU_HPMEVENT_INVALID);
        riscv_iommu_reg_set64(s, evt_reg, val);
    }

    update_event_map(s, val, ctr_idx);
}
