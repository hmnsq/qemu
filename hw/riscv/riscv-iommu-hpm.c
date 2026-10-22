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

#شامل "qemu/osdep.h"
#شامل "qemu/timer.h"
#شامل "cpu_bits.h"
#شامل "riscv-iommu-hpm.h"
#شامل "riscv-iommu.h"
#شامل "riscv-iommu-bits.h"
#شامل "trace.h"

/* For now we assume IOMMU HPM frequency to be 1GHz so 1-cycle is of 1-ns. */
ساکن اندرونی uint64_t get_ادوار(خالی)
{
    واپس qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

uint64_t riscv_iommu_hpmcycle_read(RISCVIOMMUState *s)
{
    ثابت uint64_t چکر = riscv_iommu_reg_get64(
        s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    ثابت uint64_t ctr_پچھلا = s->hpmcycle_پچھلا;
    ثابت uint64_t ctr_val = s->hpmcycle_val;

    trace_riscv_iommu_hpm_read(چکر, inhibit, ctr_پچھلا, ctr_val);

    اگر (get_میدان(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Counter should not increment if inhibit bit is set. We can't really
         * stop the QEMU_CLOCK_VIRTUAL, so we just return the last updated
         * counter value to indicate that counter was not incremented.
         */
        واپس (ctr_val & RISCV_IOMMU_IOHPMCYCLES_شمار) |
               (چکر & RISCV_IOMMU_IOHPMCYCLES_OVF);
    }

    واپس (ctr_val + get_ادوار() - ctr_پچھلا) |
        (چکر & RISCV_IOMMU_IOHPMCYCLES_OVF);
}

ساکن خالی hpm_incr_ctr(RISCVIOMMUState *s, uint32_t ctr_idx)
{
    ثابت uint32_t off = ctr_idx << 3;
    uint64_t cntr_val;

    cntr_val = ldq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_بنیاد + off]);
    stq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_بنیاد + off], cntr_val + 1);

    trace_riscv_iommu_hpm_incr_ctr(cntr_val);

    /* Handle the overflow scenario. */
    اگر (cntr_val == UINT64_MAX) {
        /*
         * Generate interrupt only if OF bit is clear. +1 to offset the cycle
         * register OF bit.
         */
        ثابت uint32_t ovf =
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF,
                                  ثنائی_ہندسہ(ctr_idx + 1), 0);
        اگر (!get_میدان(ovf, ثنائی_ہندسہ(ctr_idx + 1))) {
            riscv_iommu_reg_mod64(s,
                                  RISCV_IOMMU_REG_IOHPMEVT_بنیاد + off,
                                  RISCV_IOMMU_IOHPMEVT_کا,
                                  0);
            riscv_iommu_notify(s, RISCV_IOMMU_INTR_PM);
        }
    }
}

خالی riscv_iommu_hpm_incr_ctr(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
                              غیرعلامتی event_id)
{
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t did_gscid;
    uint32_t pid_pscid;
    uint32_t ctr_idx;
    gpointer قدر;
    uint32_t ctrs;
    uint64_t evt;

    اگر (!(s->cap & RISCV_IOMMU_CAP_HPM)) {
        واپس;
    }

    قدر = g_hash_جدول_lookup(s->hpm_event_ctr_map,
                                GUINT_مقصد_اشارہ_گر(event_id));
    اگر (قدر == تہی) {
        واپس;
    }

    برائے (ctrs = GPOINTER_مقصد_UINT(قدر); ctrs != 0; ctrs &= ctrs - 1) {
        ctr_idx = ctz32(ctrs);
        اگر (get_میدان(inhibit, ثنائی_ہندسہ(ctr_idx + 1))) {
            جاری;
        }

        evt = riscv_iommu_reg_get64(s,
            RISCV_IOMMU_REG_IOHPMEVT_بنیاد + (ctr_idx << 3));

        /*
         * It's quite possible that event ID has been changed in counter
         * but hashtable hasn't been updated yet. We don't want to increment
         * counter for the old event ID.
         */
        اگر (event_id != get_میدان(evt, RISCV_IOMMU_IOHPMEVT_EVENT_ID)) {
            جاری;
        }

        اگر (get_میدان(evt, RISCV_IOMMU_IOHPMEVT_IDT)) {
            did_gscid = get_میدان(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
            pid_pscid = get_میدان(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        } ورنہ {
            did_gscid = ctx->devid;
            pid_pscid = ctx->process_id;
        }

        اگر (get_میدان(evt, RISCV_IOMMU_IOHPMEVT_PV_PSCV)) {
            /*
             * If the transaction does not have a valid process_id, counter
             * increments if device_id matches DID_GSCID. If the transaction
             * has a valid process_id, counter increments if device_id
             * matches DID_GSCID and process_id matches PID_PSCID. See
             * IOMMU Specification, Chapter 5.23. Performance-monitoring
             * event selector.
             */
            اگر (ctx->process_id &&
                get_میدان(evt, RISCV_IOMMU_IOHPMEVT_PID_PSCID) != pid_pscid) {
                جاری;
            }
        }

        اگر (get_میدان(evt, RISCV_IOMMU_IOHPMEVT_DV_GSCV)) {
            uint32_t نقاب = ~0;

            اگر (get_میدان(evt, RISCV_IOMMU_IOHPMEVT_DMASK)) {
                /*
                 * 1001 1011   mask = GSCID
                 * 0000 0111   mask = mask ^ (mask + 1)
                 * 1111 1000   mask = ~mask;
                 */
                نقاب = get_میدان(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID);
                نقاب = نقاب ^ (نقاب + 1);
                نقاب = ~نقاب;
            }

            اگر ((get_میدان(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID) & نقاب) !=
                (did_gscid & نقاب)) {
                جاری;
            }
        }

        hpm_incr_ctr(s, ctr_idx);
    }
}

/* Timer callback for cycle counter overflow. */
خالی riscv_iommu_hpm_timer_cb(خالی *priv)
{
    RISCVIOMMUState *s = priv;
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t ovf;

    اگر (get_میدان(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        واپس;
    }

    اگر (s->irq_حد_سے_تجاوز_بایاں > 0) {
        uint64_t irq_trigger_at =
            qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->irq_حد_سے_تجاوز_بایاں;
        timer_mod_anticipate_ns(s->hpm_timer, irq_trigger_at);
        s->irq_حد_سے_تجاوز_بایاں = 0;
        واپس;
    }

    ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    اگر (!get_میدان(ovf, RISCV_IOMMU_IOCOUNTOVF_CY)) {
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

ساکن خالی hpm_تیاری_timer(RISCVIOMMUState *s, uint64_t قدر)
{
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint64_t حد_سے_تجاوز_at, حد_سے_تجاوز_ns;

    اگر (get_میدان(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        واپس;
    }

    /*
     * We are using INT64_MAX here instead to UINT64_MAX because cycle counter
     * has 63-bit precision and INT64_MAX is the maximum it can store.
     */
    اگر (قدر) {
        حد_سے_تجاوز_ns = INT64_MAX - قدر + 1;
    } ورنہ {
        حد_سے_تجاوز_ns = INT64_MAX;
    }

    حد_سے_تجاوز_at = (uint64_t)qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + حد_سے_تجاوز_ns;
    s->irq_حد_سے_تجاوز_بایاں = 0;

    اگر (حد_سے_تجاوز_at > INT64_MAX) {
        s->irq_حد_سے_تجاوز_بایاں = حد_سے_تجاوز_at - INT64_MAX;
        حد_سے_تجاوز_at = INT64_MAX;
    }

    timer_mod_anticipate_ns(s->hpm_timer, حد_سے_تجاوز_at);
}

/* Updates the internal cycle counter state when iocntinh:CY is changed. */
خالی riscv_iommu_process_iocntinh_cy(RISCVIOMMUState *s, bool پچھلا_cy_inh)
{
    ثابت uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);

    /* We only need to process CY bit toggle. */
    bool cy_inh = !!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY);
    اگر (cy_inh == پچھلا_cy_inh) {
        واپس;
    }

    trace_riscv_iommu_hpm_iocntinh_cy(پچھلا_cy_inh);

    اگر (!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Cycle counter is enabled. Just start the timer again and update
         * the clock snapshot value to point to the current time to make
         * sure iohpmcycles read is correct.
         */
        s->hpmcycle_پچھلا = get_ادوار();
        hpm_تیاری_timer(s, s->hpmcycle_val);
    } ورنہ {
        /*
         * Cycle counter is disabled. Stop the timer and update the cycle
         * counter to record the current value which is last programmed
         * value + the cycles passed so far.
         */
        s->hpmcycle_val = s->hpmcycle_val + (get_ادوار() - s->hpmcycle_پچھلا);
        timer_del(s->hpm_timer);
    }
}

خالی riscv_iommu_process_hpmcycle_write(RISCVIOMMUState *s)
{
    ثابت uint64_t val = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ثابت uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);

    trace_riscv_iommu_hpm_چکر_write(ovf, val);

    /*
     * Clear OF bit in IOCNTOVF if it's being cleared in IOHPMCYCLES register.
     */
    اگر (get_میدان(ovf, RISCV_IOMMU_IOCOUNTOVF_CY) &&
        !get_میدان(val, RISCV_IOMMU_IOHPMCYCLES_OVF)) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF, 0,
            RISCV_IOMMU_IOCOUNTOVF_CY);
    }

    s->hpmcycle_val = val & ~RISCV_IOMMU_IOHPMCYCLES_OVF;
    s->hpmcycle_پچھلا = get_ادوار();
    hpm_تیاری_timer(s, s->hpmcycle_val);
}

ساکن اندرونی bool check_valid_event_id(غیرعلامتی event_id)
{
    واپس event_id > RISCV_IOMMU_HPMEVENT_INVALID &&
           event_id < RISCV_IOMMU_HPMEVENT_MAX;
}

ساکن gboolean hpm_event_equal(gpointer کلید, gpointer قدر, gpointer udata)
{
    uint32_t *pair = udata;

    اگر (GPOINTER_مقصد_UINT(قدر) & (1 << pair[0])) {
        pair[1] = GPOINTER_مقصد_UINT(کلید);
        واپس true;
    }

    واپس false;
}

/* Caller must check ctr_idx against hpm_ctrs to see if its supported or not. */
ساکن خالی update_event_map(RISCVIOMMUState *s, uint64_t قدر,
                             uint32_t ctr_idx)
{
    غیرعلامتی event_id = get_میدان(قدر, RISCV_IOMMU_IOHPMEVT_EVENT_ID);
    uint32_t pair[2] = { ctr_idx, RISCV_IOMMU_HPMEVENT_INVALID };
    uint32_t new_قدر = 1 << ctr_idx;
    gpointer ڈیٹا;

    /*
     * If EventID field is RISCV_IOMMU_HPMEVENT_INVALID
     * remove the current mapping.
     */
    اگر (event_id == RISCV_IOMMU_HPMEVENT_INVALID) {
        ڈیٹا = g_hash_جدول_ڈھونڈو(s->hpm_event_ctr_map, hpm_event_equal, pair);

        new_قدر = GPOINTER_مقصد_UINT(ڈیٹا) & ~(new_قدر);
        اگر (new_قدر != 0) {
            g_hash_جدول_replace(s->hpm_event_ctr_map,
                                 GUINT_مقصد_اشارہ_گر(pair[1]),
                                 GUINT_مقصد_اشارہ_گر(new_قدر));
        } ورنہ {
            g_hash_جدول_remove(s->hpm_event_ctr_map,
                                GUINT_مقصد_اشارہ_گر(pair[1]));
        }

        واپس;
    }

    /* Update the counter mask if the event is already enabled. */
    اگر (g_hash_جدول_lookup_extended(s->hpm_event_ctr_map,
                                     GUINT_مقصد_اشارہ_گر(event_id),
                                     تہی,
                                     &ڈیٹا)) {
        new_قدر |= GPOINTER_مقصد_UINT(ڈیٹا);
    }

    g_hash_جدول_داخل_کرو(s->hpm_event_ctr_map,
                        GUINT_مقصد_اشارہ_گر(event_id),
                        GUINT_مقصد_اشارہ_گر(new_قدر));
}

خالی riscv_iommu_process_hpmevt_write(RISCVIOMMUState *s, uint32_t evt_reg)
{
    ثابت uint32_t ctr_idx = (evt_reg - RISCV_IOMMU_REG_IOHPMEVT_بنیاد) >> 3;
    ثابت uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    uint64_t val = riscv_iommu_reg_get64(s, evt_reg);

    اگر (ctr_idx >= s->hpm_cntrs) {
        واپس;
    }

    trace_riscv_iommu_hpm_evt_write(ctr_idx, ovf, val);

    /* Clear OF bit in IOCNTOVF if it's being cleared in IOHPMEVT register. */
    اگر (get_میدان(ovf, ثنائی_ہندسہ(ctr_idx + 1)) &&
        !get_میدان(val, RISCV_IOMMU_IOHPMEVT_کا)) {
        /* +1 to offset CYCLE register OF bit. */
        riscv_iommu_reg_mod32(
            s, RISCV_IOMMU_REG_IOCOUNTOVF, 0, ثنائی_ہندسہ(ctr_idx + 1));
    }

    اگر (!check_valid_event_id(get_میدان(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID))) {
        /* Reset EventID (WARL) field to invalid. */
        val = سیٹ_میدان(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID,
            RISCV_IOMMU_HPMEVENT_INVALID);
        riscv_iommu_reg_set64(s, evt_reg, val);
    }

    update_event_map(s, val, ctr_idx);
}
