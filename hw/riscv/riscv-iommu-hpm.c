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

#ضمن "qemu/osdep.h"
#ضمن "qemu/timer.h"
#ضمن "cpu_bits.h"
#ضمن "riscv-iommu-hpm.h"
#ضمن "riscv-iommu.h"
#ضمن "riscv-iommu-bits.h"
#ضمن "trace.h"

/* For now we assume IOMMU HPM frequency to be 1GHz so 1-cycle is of 1-ns. */
ساكن مضمن uint64_t get_چرخهها(فراغ)
{
    أرجع qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

uint64_t riscv_iommu_hpmcycle_read(RISCVIOMMUState *s)
{
    ثابت uint64_t چرخه = riscv_iommu_reg_get64(
        s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    ثابت uint64_t ctr_prev = s->hpmcycle_prev;
    ثابت uint64_t ctr_val = s->hpmcycle_val;

    trace_riscv_iommu_hpm_read(چرخه, inhibit, ctr_prev, ctr_val);

    إذا (get_فیلد(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Counter should not increment if inhibit bit is set. We can't really
         * stop the QEMU_CLOCK_VIRTUAL, so we just return the last updated
         * counter value to indicate that counter was not incremented.
         */
        أرجع (ctr_val & RISCV_IOMMU_IOHPMCYCLES_COUNTER) |
               (چرخه & RISCV_IOMMU_IOHPMCYCLES_OVF);
    }

    أرجع (ctr_val + get_چرخهها() - ctr_prev) |
        (چرخه & RISCV_IOMMU_IOHPMCYCLES_OVF);
}

ساكن فراغ hpm_incr_ctr(RISCVIOMMUState *s, uint32_t ctr_idx)
{
    ثابت uint32_t off = ctr_idx << 3;
    uint64_t cntr_val;

    cntr_val = ldq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_پایه + off]);
    stq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_پایه + off], cntr_val + 1);

    trace_riscv_iommu_hpm_incr_ctr(cntr_val);

    /* Handle the overflow scenario. */
    إذا (cntr_val == UINT64_MAX) {
        /*
         * Generate interrupt only if OF bit is clear. +1 to offset the cycle
         * register OF bit.
         */
        ثابت uint32_t ovf =
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF,
                                  بیت(ctr_idx + 1), 0);
        إذا (!get_فیلد(ovf, بیت(ctr_idx + 1))) {
            riscv_iommu_reg_mod64(s,
                                  RISCV_IOMMU_REG_IOHPMEVT_پایه + off,
                                  RISCV_IOMMU_IOHPMEVT_از,
                                  0);
            riscv_iommu_notify(s, RISCV_IOMMU_INTR_PM);
        }
    }
}

فراغ riscv_iommu_hpm_incr_ctr(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
                              بیعلامت event_id)
{
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t did_gscid;
    uint32_t pid_pscid;
    uint32_t ctr_idx;
    gpointer مقدار;
    uint32_t ctrs;
    uint64_t evt;

    إذا (!(s->cap & RISCV_IOMMU_CAP_HPM)) {
        أرجع;
    }

    مقدار = g_hash_جدول_lookup(s->hpm_event_ctr_map,
                                GUINT_مقصد_اشاره_گر(event_id));
    إذا (مقدار == عدم) {
        أرجع;
    }

    لكل (ctrs = GPOINTER_مقصد_UINT(مقدار); ctrs != 0; ctrs &= ctrs - 1) {
        ctr_idx = ctz32(ctrs);
        إذا (get_فیلد(inhibit, بیت(ctr_idx + 1))) {
            استمر;
        }

        evt = riscv_iommu_reg_get64(s,
            RISCV_IOMMU_REG_IOHPMEVT_پایه + (ctr_idx << 3));

        /*
         * It's quite possible that event ID has been changed in counter
         * but hashtable hasn't been updated yet. We don't want to increment
         * counter for the old event ID.
         */
        إذا (event_id != get_فیلد(evt, RISCV_IOMMU_IOHPMEVT_EVENT_ID)) {
            استمر;
        }

        إذا (get_فیلد(evt, RISCV_IOMMU_IOHPMEVT_IDT)) {
            did_gscid = get_فیلد(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
            pid_pscid = get_فیلد(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        } وإلا {
            did_gscid = ctx->devid;
            pid_pscid = ctx->process_id;
        }

        إذا (get_فیلد(evt, RISCV_IOMMU_IOHPMEVT_PV_PSCV)) {
            /*
             * If the transaction does not have a valid process_id, counter
             * increments if device_id matches DID_GSCID. If the transaction
             * has a valid process_id, counter increments if device_id
             * matches DID_GSCID and process_id matches PID_PSCID. See
             * IOMMU Specification, Chapter 5.23. Performance-monitoring
             * event selector.
             */
            إذا (ctx->process_id &&
                get_فیلد(evt, RISCV_IOMMU_IOHPMEVT_PID_PSCID) != pid_pscid) {
                استمر;
            }
        }

        إذا (get_فیلد(evt, RISCV_IOMMU_IOHPMEVT_DV_GSCV)) {
            uint32_t ماسک = ~0;

            إذا (get_فیلد(evt, RISCV_IOMMU_IOHPMEVT_DMASK)) {
                /*
                 * 1001 1011   mask = GSCID
                 * 0000 0111   mask = mask ^ (mask + 1)
                 * 1111 1000   mask = ~mask;
                 */
                ماسک = get_فیلد(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID);
                ماسک = ماسک ^ (ماسک + 1);
                ماسک = ~ماسک;
            }

            إذا ((get_فیلد(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID) & ماسک) !=
                (did_gscid & ماسک)) {
                استمر;
            }
        }

        hpm_incr_ctr(s, ctr_idx);
    }
}

/* Timer callback for cycle counter overflow. */
فراغ riscv_iommu_hpm_timer_cb(فراغ *priv)
{
    RISCVIOMMUState *s = priv;
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t ovf;

    إذا (get_فیلد(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        أرجع;
    }

    إذا (s->irq_سرریز_چپ > 0) {
        uint64_t irq_trigger_at =
            qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->irq_سرریز_چپ;
        timer_mod_anticipate_ns(s->hpm_timer, irq_trigger_at);
        s->irq_سرریز_چپ = 0;
        أرجع;
    }

    ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    إذا (!get_فیلد(ovf, RISCV_IOMMU_IOCOUNTOVF_CY)) {
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

ساكن فراغ hpm_راهاندازی_timer(RISCVIOMMUState *s, uint64_t مقدار)
{
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint64_t سرریز_at, سرریز_ns;

    إذا (get_فیلد(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        أرجع;
    }

    /*
     * We are using INT64_MAX here instead to UINT64_MAX because cycle counter
     * has 63-bit precision and INT64_MAX is the maximum it can store.
     */
    إذا (مقدار) {
        سرریز_ns = INT64_MAX - مقدار + 1;
    } وإلا {
        سرریز_ns = INT64_MAX;
    }

    سرریز_at = (uint64_t)qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + سرریز_ns;
    s->irq_سرریز_چپ = 0;

    إذا (سرریز_at > INT64_MAX) {
        s->irq_سرریز_چپ = سرریز_at - INT64_MAX;
        سرریز_at = INT64_MAX;
    }

    timer_mod_anticipate_ns(s->hpm_timer, سرریز_at);
}

/* Updates the internal cycle counter state when iocntinh:CY is changed. */
فراغ riscv_iommu_process_iocntinh_cy(RISCVIOMMUState *s, bool prev_cy_inh)
{
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);

    /* We only need to process CY bit toggle. */
    bool cy_inh = !!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY);
    إذا (cy_inh == prev_cy_inh) {
        أرجع;
    }

    trace_riscv_iommu_hpm_iocntinh_cy(prev_cy_inh);

    إذا (!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Cycle counter is enabled. Just start the timer again and update
         * the clock snapshot value to point to the current time to make
         * sure iohpmcycles read is correct.
         */
        s->hpmcycle_prev = get_چرخهها();
        hpm_راهاندازی_timer(s, s->hpmcycle_val);
    } وإلا {
        /*
         * Cycle counter is disabled. Stop the timer and update the cycle
         * counter to record the current value which is last programmed
         * value + the cycles passed so far.
         */
        s->hpmcycle_val = s->hpmcycle_val + (get_چرخهها() - s->hpmcycle_prev);
        timer_del(s->hpm_timer);
    }
}

فراغ riscv_iommu_process_hpmcycle_write(RISCVIOMMUState *s)
{
    ثابت uint64_t val = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ثابت uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);

    trace_riscv_iommu_hpm_چرخه_write(ovf, val);

    /*
     * Clear OF bit in IOCNTOVF if it's being cleared in IOHPMCYCLES register.
     */
    إذا (get_فیلد(ovf, RISCV_IOMMU_IOCOUNTOVF_CY) &&
        !get_فیلد(val, RISCV_IOMMU_IOHPMCYCLES_OVF)) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF, 0,
            RISCV_IOMMU_IOCOUNTOVF_CY);
    }

    s->hpmcycle_val = val & ~RISCV_IOMMU_IOHPMCYCLES_OVF;
    s->hpmcycle_prev = get_چرخهها();
    hpm_راهاندازی_timer(s, s->hpmcycle_val);
}

ساكن مضمن bool check_valid_event_id(بیعلامت event_id)
{
    أرجع event_id > RISCV_IOMMU_HPMEVENT_INVALID &&
           event_id < RISCV_IOMMU_HPMEVENT_MAX;
}

ساكن gboolean hpm_event_equal(gpointer کلید, gpointer مقدار, gpointer udata)
{
    uint32_t *pair = udata;

    إذا (GPOINTER_مقصد_UINT(مقدار) & (1 << pair[0])) {
        pair[1] = GPOINTER_مقصد_UINT(کلید);
        أرجع true;
    }

    أرجع false;
}

/* Caller must check ctr_idx against hpm_ctrs to see if its supported or not. */
ساكن فراغ update_event_map(RISCVIOMMUState *s, uint64_t مقدار,
                             uint32_t ctr_idx)
{
    بیعلامت event_id = get_فیلد(مقدار, RISCV_IOMMU_IOHPMEVT_EVENT_ID);
    uint32_t pair[2] = { ctr_idx, RISCV_IOMMU_HPMEVENT_INVALID };
    uint32_t new_مقدار = 1 << ctr_idx;
    gpointer داده;

    /*
     * If EventID field is RISCV_IOMMU_HPMEVENT_INVALID
     * remove the current mapping.
     */
    إذا (event_id == RISCV_IOMMU_HPMEVENT_INVALID) {
        داده = g_hash_جدول_پیدا_کن(s->hpm_event_ctr_map, hpm_event_equal, pair);

        new_مقدار = GPOINTER_مقصد_UINT(داده) & ~(new_مقدار);
        إذا (new_مقدار != 0) {
            g_hash_جدول_replace(s->hpm_event_ctr_map,
                                 GUINT_مقصد_اشاره_گر(pair[1]),
                                 GUINT_مقصد_اشاره_گر(new_مقدار));
        } وإلا {
            g_hash_جدول_remove(s->hpm_event_ctr_map,
                                GUINT_مقصد_اشاره_گر(pair[1]));
        }

        أرجع;
    }

    /* Update the counter mask if the event is already enabled. */
    إذا (g_hash_جدول_lookup_extended(s->hpm_event_ctr_map,
                                     GUINT_مقصد_اشاره_گر(event_id),
                                     عدم,
                                     &داده)) {
        new_مقدار |= GPOINTER_مقصد_UINT(داده);
    }

    g_hash_جدول_درج(s->hpm_event_ctr_map,
                        GUINT_مقصد_اشاره_گر(event_id),
                        GUINT_مقصد_اشاره_گر(new_مقدار));
}

فراغ riscv_iommu_process_hpmevt_write(RISCVIOMMUState *s, uint32_t evt_reg)
{
    ثابت uint32_t ctr_idx = (evt_reg - RISCV_IOMMU_REG_IOHPMEVT_پایه) >> 3;
    ثابت uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    uint64_t val = riscv_iommu_reg_get64(s, evt_reg);

    إذا (ctr_idx >= s->hpm_cntrs) {
        أرجع;
    }

    trace_riscv_iommu_hpm_evt_write(ctr_idx, ovf, val);

    /* Clear OF bit in IOCNTOVF if it's being cleared in IOHPMEVT register. */
    إذا (get_فیلد(ovf, بیت(ctr_idx + 1)) &&
        !get_فیلد(val, RISCV_IOMMU_IOHPMEVT_از)) {
        /* +1 to offset CYCLE register OF bit. */
        riscv_iommu_reg_mod32(
            s, RISCV_IOMMU_REG_IOCOUNTOVF, 0, بیت(ctr_idx + 1));
    }

    إذا (!check_valid_event_id(get_فیلد(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID))) {
        /* Reset EventID (WARL) field to invalid. */
        val = مجموعه_فیلد(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID,
            RISCV_IOMMU_HPMEVENT_INVALID);
        riscv_iommu_reg_set64(s, evt_reg, val);
    }

    update_event_map(s, val, ctr_idx);
}
