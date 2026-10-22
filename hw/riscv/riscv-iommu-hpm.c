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

#અન્તર્ભાવ "qemu/osdep.h"
#અન્તર્ભાવ "qemu/timer.h"
#અન્તર્ભાવ "cpu_bits.h"
#અન્તર્ભાવ "riscv-iommu-hpm.h"
#અન્તર્ભાવ "riscv-iommu.h"
#અન્તર્ભાવ "riscv-iommu-bits.h"
#અન્તર્ભાવ "trace.h"

/* For now we assume IOMMU HPM frequency to be 1GHz so 1-cycle is of 1-ns. */
સ્થિર અંતઃરેખ uint64_t get_cycles(શૂન્ય)
{
    પાછા qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

uint64_t riscv_iommu_hpmcycle_read(RISCVIOMMUState *s)
{
    અચલ uint64_t cycle = riscv_iommu_reg_get64(
        s, RISCV_IOMMU_REG_IOHPMCYCLES);
    અચલ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    અચલ uint64_t ctr_prev = s->hpmcycle_prev;
    અચલ uint64_t ctr_val = s->hpmcycle_val;

    trace_riscv_iommu_hpm_read(cycle, inhibit, ctr_prev, ctr_val);

    જો (get_field(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Counter should not increment if inhibit bit is set. We can't really
         * stop the QEMU_CLOCK_VIRTUAL, so we just return the last updated
         * counter value to indicate that counter was not incremented.
         */
        પાછા (ctr_val & RISCV_IOMMU_IOHPMCYCLES_COUNTER) |
               (cycle & RISCV_IOMMU_IOHPMCYCLES_OVF);
    }

    પાછા (ctr_val + get_cycles() - ctr_prev) |
        (cycle & RISCV_IOMMU_IOHPMCYCLES_OVF);
}

સ્થિર શૂન્ય hpm_incr_ctr(RISCVIOMMUState *s, uint32_t ctr_idx)
{
    અચલ uint32_t off = ctr_idx << 3;
    uint64_t cntr_val;

    cntr_val = ldq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_BASE + off]);
    stq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_BASE + off], cntr_val + 1);

    trace_riscv_iommu_hpm_incr_ctr(cntr_val);

    /* Handle the overflow scenario. */
    જો (cntr_val == UINT64_MAX) {
        /*
         * Generate interrupt only if OF bit is clear. +1 to offset the cycle
         * register OF bit.
         */
        અચલ uint32_t ovf =
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF,
                                  BIT(ctr_idx + 1), 0);
        જો (!get_field(ovf, BIT(ctr_idx + 1))) {
            riscv_iommu_reg_mod64(s,
                                  RISCV_IOMMU_REG_IOHPMEVT_BASE + off,
                                  RISCV_IOMMU_IOHPMEVT_OF,
                                  0);
            riscv_iommu_notify(s, RISCV_IOMMU_INTR_PM);
        }
    }
}

શૂન્ય riscv_iommu_hpm_incr_ctr(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
                              unsigned event_id)
{
    અચલ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t did_gscid;
    uint32_t pid_pscid;
    uint32_t ctr_idx;
    gpointer કિંમત;
    uint32_t ctrs;
    uint64_t evt;

    જો (!(s->cap & RISCV_IOMMU_CAP_HPM)) {
        પાછા;
    }

    કિંમત = g_hash_table_lookup(s->hpm_event_ctr_map,
                                GUINT_TO_POINTER(event_id));
    જો (કિંમત == ખાલી) {
        પાછા;
    }

    માટે (ctrs = GPOINTER_TO_UINT(કિંમત); ctrs != 0; ctrs &= ctrs - 1) {
        ctr_idx = ctz32(ctrs);
        જો (get_field(inhibit, BIT(ctr_idx + 1))) {
            ચાલુ;
        }

        evt = riscv_iommu_reg_get64(s,
            RISCV_IOMMU_REG_IOHPMEVT_BASE + (ctr_idx << 3));

        /*
         * It's quite possible that event ID has been changed in counter
         * but hashtable hasn't been updated yet. We don't want to increment
         * counter for the old event ID.
         */
        જો (event_id != get_field(evt, RISCV_IOMMU_IOHPMEVT_EVENT_ID)) {
            ચાલુ;
        }

        જો (get_field(evt, RISCV_IOMMU_IOHPMEVT_IDT)) {
            did_gscid = get_field(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
            pid_pscid = get_field(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        } નહીંતર {
            did_gscid = ctx->devid;
            pid_pscid = ctx->process_id;
        }

        જો (get_field(evt, RISCV_IOMMU_IOHPMEVT_PV_PSCV)) {
            /*
             * If the transaction does not have a valid process_id, counter
             * increments if device_id matches DID_GSCID. If the transaction
             * has a valid process_id, counter increments if device_id
             * matches DID_GSCID and process_id matches PID_PSCID. See
             * IOMMU Specification, Chapter 5.23. Performance-monitoring
             * event selector.
             */
            જો (ctx->process_id &&
                get_field(evt, RISCV_IOMMU_IOHPMEVT_PID_PSCID) != pid_pscid) {
                ચાલુ;
            }
        }

        જો (get_field(evt, RISCV_IOMMU_IOHPMEVT_DV_GSCV)) {
            uint32_t mask = ~0;

            જો (get_field(evt, RISCV_IOMMU_IOHPMEVT_DMASK)) {
                /*
                 * 1001 1011   mask = GSCID
                 * 0000 0111   mask = mask ^ (mask + 1)
                 * 1111 1000   mask = ~mask;
                 */
                mask = get_field(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID);
                mask = mask ^ (mask + 1);
                mask = ~mask;
            }

            જો ((get_field(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID) & mask) !=
                (did_gscid & mask)) {
                ચાલુ;
            }
        }

        hpm_incr_ctr(s, ctr_idx);
    }
}

/* Timer callback for cycle counter overflow. */
શૂન્ય riscv_iommu_hpm_timer_cb(શૂન્ય *priv)
{
    RISCVIOMMUState *s = priv;
    અચલ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t ovf;

    જો (get_field(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        પાછા;
    }

    જો (s->irq_overflow_ડાબું > 0) {
        uint64_t irq_trigger_at =
            qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->irq_overflow_ડાબું;
        timer_mod_anticipate_ns(s->hpm_timer, irq_trigger_at);
        s->irq_overflow_ડાબું = 0;
        પાછા;
    }

    ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    જો (!get_field(ovf, RISCV_IOMMU_IOCOUNTOVF_CY)) {
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

સ્થિર શૂન્ય hpm_setup_timer(RISCVIOMMUState *s, uint64_t કિંમત)
{
    અચલ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint64_t overflow_at, overflow_ns;

    જો (get_field(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        પાછા;
    }

    /*
     * We are using INT64_MAX here instead to UINT64_MAX because cycle counter
     * has 63-bit precision and INT64_MAX is the maximum it can store.
     */
    જો (કિંમત) {
        overflow_ns = INT64_MAX - કિંમત + 1;
    } નહીંતર {
        overflow_ns = INT64_MAX;
    }

    overflow_at = (uint64_t)qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + overflow_ns;
    s->irq_overflow_ડાબું = 0;

    જો (overflow_at > INT64_MAX) {
        s->irq_overflow_ડાબું = overflow_at - INT64_MAX;
        overflow_at = INT64_MAX;
    }

    timer_mod_anticipate_ns(s->hpm_timer, overflow_at);
}

/* Updates the internal cycle counter state when iocntinh:CY is changed. */
શૂન્ય riscv_iommu_process_iocntinh_cy(RISCVIOMMUState *s, bool prev_cy_inh)
{
    અચલ uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);

    /* We only need to process CY bit toggle. */
    bool cy_inh = !!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY);
    જો (cy_inh == prev_cy_inh) {
        પાછા;
    }

    trace_riscv_iommu_hpm_iocntinh_cy(prev_cy_inh);

    જો (!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Cycle counter is enabled. Just start the timer again and update
         * the clock snapshot value to point to the current time to make
         * sure iohpmcycles read is correct.
         */
        s->hpmcycle_prev = get_cycles();
        hpm_setup_timer(s, s->hpmcycle_val);
    } નહીંતર {
        /*
         * Cycle counter is disabled. Stop the timer and update the cycle
         * counter to record the current value which is last programmed
         * value + the cycles passed so far.
         */
        s->hpmcycle_val = s->hpmcycle_val + (get_cycles() - s->hpmcycle_prev);
        timer_del(s->hpm_timer);
    }
}

શૂન્ય riscv_iommu_process_hpmcycle_write(RISCVIOMMUState *s)
{
    અચલ uint64_t val = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_IOHPMCYCLES);
    અચલ uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);

    trace_riscv_iommu_hpm_cycle_write(ovf, val);

    /*
     * Clear OF bit in IOCNTOVF if it's being cleared in IOHPMCYCLES register.
     */
    જો (get_field(ovf, RISCV_IOMMU_IOCOUNTOVF_CY) &&
        !get_field(val, RISCV_IOMMU_IOHPMCYCLES_OVF)) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF, 0,
            RISCV_IOMMU_IOCOUNTOVF_CY);
    }

    s->hpmcycle_val = val & ~RISCV_IOMMU_IOHPMCYCLES_OVF;
    s->hpmcycle_prev = get_cycles();
    hpm_setup_timer(s, s->hpmcycle_val);
}

સ્થિર અંતઃરેખ bool check_valid_event_id(unsigned event_id)
{
    પાછા event_id > RISCV_IOMMU_HPMEVENT_INVALID &&
           event_id < RISCV_IOMMU_HPMEVENT_MAX;
}

સ્થિર gboolean hpm_event_equal(gpointer ચાવી, gpointer કિંમત, gpointer udata)
{
    uint32_t *pair = udata;

    જો (GPOINTER_TO_UINT(કિંમત) & (1 << pair[0])) {
        pair[1] = GPOINTER_TO_UINT(ચાવી);
        પાછા true;
    }

    પાછા false;
}

/* Caller must check ctr_idx against hpm_ctrs to see if its supported or not. */
સ્થિર શૂન્ય update_event_map(RISCVIOMMUState *s, uint64_t કિંમત,
                             uint32_t ctr_idx)
{
    unsigned event_id = get_field(કિંમત, RISCV_IOMMU_IOHPMEVT_EVENT_ID);
    uint32_t pair[2] = { ctr_idx, RISCV_IOMMU_HPMEVENT_INVALID };
    uint32_t new_કિંમત = 1 << ctr_idx;
    gpointer data;

    /*
     * If EventID field is RISCV_IOMMU_HPMEVENT_INVALID
     * remove the current mapping.
     */
    જો (event_id == RISCV_IOMMU_HPMEVENT_INVALID) {
        data = g_hash_table_find(s->hpm_event_ctr_map, hpm_event_equal, pair);

        new_કિંમત = GPOINTER_TO_UINT(data) & ~(new_કિંમત);
        જો (new_કિંમત != 0) {
            g_hash_table_replace(s->hpm_event_ctr_map,
                                 GUINT_TO_POINTER(pair[1]),
                                 GUINT_TO_POINTER(new_કિંમત));
        } નહીંતર {
            g_hash_table_remove(s->hpm_event_ctr_map,
                                GUINT_TO_POINTER(pair[1]));
        }

        પાછા;
    }

    /* Update the counter mask if the event is already enabled. */
    જો (g_hash_table_lookup_extended(s->hpm_event_ctr_map,
                                     GUINT_TO_POINTER(event_id),
                                     ખાલી,
                                     &data)) {
        new_કિંમત |= GPOINTER_TO_UINT(data);
    }

    g_hash_table_ઉમેરો(s->hpm_event_ctr_map,
                        GUINT_TO_POINTER(event_id),
                        GUINT_TO_POINTER(new_કિંમત));
}

શૂન્ય riscv_iommu_process_hpmevt_write(RISCVIOMMUState *s, uint32_t evt_reg)
{
    અચલ uint32_t ctr_idx = (evt_reg - RISCV_IOMMU_REG_IOHPMEVT_BASE) >> 3;
    અચલ uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    uint64_t val = riscv_iommu_reg_get64(s, evt_reg);

    જો (ctr_idx >= s->hpm_cntrs) {
        પાછા;
    }

    trace_riscv_iommu_hpm_evt_write(ctr_idx, ovf, val);

    /* Clear OF bit in IOCNTOVF if it's being cleared in IOHPMEVT register. */
    જો (get_field(ovf, BIT(ctr_idx + 1)) &&
        !get_field(val, RISCV_IOMMU_IOHPMEVT_OF)) {
        /* +1 to offset CYCLE register OF bit. */
        riscv_iommu_reg_mod32(
            s, RISCV_IOMMU_REG_IOCOUNTOVF, 0, BIT(ctr_idx + 1));
    }

    જો (!check_valid_event_id(get_field(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID))) {
        /* Reset EventID (WARL) field to invalid. */
        val = set_field(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID,
            RISCV_IOMMU_HPMEVENT_INVALID);
        riscv_iommu_reg_set64(s, evt_reg, val);
    }

    update_event_map(s, val, ctr_idx);
}
