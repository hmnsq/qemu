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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qemu/timer.h"
#अन्तर्भाव "cpu_bits.h"
#अन्तर्भाव "riscv-iommu-hpm.h"
#अन्तर्भाव "riscv-iommu.h"
#अन्तर्भाव "riscv-iommu-bits.h"
#अन्तर्भाव "trace.h"

/* For now we assume IOMMU HPM frequency to be 1GHz so 1-cycle is of 1-ns. */
स्थिर अन्तःस्थ uint64_t get_आवृत्तयः(शून्य)
{
    प्रत्यागम qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

uint64_t riscv_iommu_hpmcycle_read(RISCVIOMMUState *s)
{
    ध्रुव uint64_t आवृत्तिः = riscv_iommu_reg_get64(
        s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ध्रुव uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    ध्रुव uint64_t ctr_पूर्व = s->hpmcycle_पूर्व;
    ध्रुव uint64_t ctr_val = s->hpmcycle_val;

    trace_riscv_iommu_hpm_read(आवृत्तिः, inhibit, ctr_पूर्व, ctr_val);

    यदि (get_क्षेत्रम्(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Counter should not increment if inhibit bit is set. We can't really
         * stop the QEMU_CLOCK_VIRTUAL, so we just return the last updated
         * counter value to indicate that counter was not incremented.
         */
        प्रत्यागम (ctr_val & RISCV_IOMMU_IOHPMCYCLES_गणक) |
               (आवृत्तिः & RISCV_IOMMU_IOHPMCYCLES_OVF);
    }

    प्रत्यागम (ctr_val + get_आवृत्तयः() - ctr_पूर्व) |
        (आवृत्तिः & RISCV_IOMMU_IOHPMCYCLES_OVF);
}

स्थिर शून्य hpm_incr_ctr(RISCVIOMMUState *s, uint32_t ctr_idx)
{
    ध्रुव uint32_t off = ctr_idx << 3;
    uint64_t cntr_val;

    cntr_val = ldq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_आधारः + off]);
    stq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_आधारः + off], cntr_val + 1);

    trace_riscv_iommu_hpm_incr_ctr(cntr_val);

    /* Handle the overflow scenario. */
    यदि (cntr_val == UINT64_MAX) {
        /*
         * Generate interrupt only if OF bit is clear. +1 to offset the cycle
         * register OF bit.
         */
        ध्रुव uint32_t ovf =
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF,
                                  BIT(ctr_idx + 1), 0);
        यदि (!get_क्षेत्रम्(ovf, BIT(ctr_idx + 1))) {
            riscv_iommu_reg_mod64(s,
                                  RISCV_IOMMU_REG_IOHPMEVT_आधारः + off,
                                  RISCV_IOMMU_IOHPMEVT_सम्बन्धि,
                                  0);
            riscv_iommu_notify(s, RISCV_IOMMU_INTR_PM);
        }
    }
}

शून्य riscv_iommu_hpm_incr_ctr(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
                              अचिह्नित event_id)
{
    ध्रुव uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t did_gscid;
    uint32_t pid_pscid;
    uint32_t ctr_idx;
    gpointer मानम्;
    uint32_t ctrs;
    uint64_t evt;

    यदि (!(s->cap & RISCV_IOMMU_CAP_HPM)) {
        प्रत्यागम;
    }

    मानम् = g_hash_सारणी_lookup(s->hpm_event_ctr_map,
                                GUINT_गन्तव्यम्_POINTER(event_id));
    यदि (मानम् == रिक्त) {
        प्रत्यागम;
    }

    प्रति (ctrs = GPOINTER_गन्तव्यम्_UINT(मानम्); ctrs != 0; ctrs &= ctrs - 1) {
        ctr_idx = ctz32(ctrs);
        यदि (get_क्षेत्रम्(inhibit, BIT(ctr_idx + 1))) {
            अनुवर्त;
        }

        evt = riscv_iommu_reg_get64(s,
            RISCV_IOMMU_REG_IOHPMEVT_आधारः + (ctr_idx << 3));

        /*
         * It's quite possible that event ID has been changed in counter
         * but hashtable hasn't been updated yet. We don't want to increment
         * counter for the old event ID.
         */
        यदि (event_id != get_क्षेत्रम्(evt, RISCV_IOMMU_IOHPMEVT_EVENT_ID)) {
            अनुवर्त;
        }

        यदि (get_क्षेत्रम्(evt, RISCV_IOMMU_IOHPMEVT_IDT)) {
            did_gscid = get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
            pid_pscid = get_क्षेत्रम्(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        } अन्यथा {
            did_gscid = ctx->devid;
            pid_pscid = ctx->process_id;
        }

        यदि (get_क्षेत्रम्(evt, RISCV_IOMMU_IOHPMEVT_PV_PSCV)) {
            /*
             * If the transaction does not have a valid process_id, counter
             * increments if device_id matches DID_GSCID. If the transaction
             * has a valid process_id, counter increments if device_id
             * matches DID_GSCID and process_id matches PID_PSCID. See
             * IOMMU Specification, Chapter 5.23. Performance-monitoring
             * event selector.
             */
            यदि (ctx->process_id &&
                get_क्षेत्रम्(evt, RISCV_IOMMU_IOHPMEVT_PID_PSCID) != pid_pscid) {
                अनुवर्त;
            }
        }

        यदि (get_क्षेत्रम्(evt, RISCV_IOMMU_IOHPMEVT_DV_GSCV)) {
            uint32_t आवरणम् = ~0;

            यदि (get_क्षेत्रम्(evt, RISCV_IOMMU_IOHPMEVT_DMASK)) {
                /*
                 * 1001 1011   mask = GSCID
                 * 0000 0111   mask = mask ^ (mask + 1)
                 * 1111 1000   mask = ~mask;
                 */
                आवरणम् = get_क्षेत्रम्(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID);
                आवरणम् = आवरणम् ^ (आवरणम् + 1);
                आवरणम् = ~आवरणम्;
            }

            यदि ((get_क्षेत्रम्(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID) & आवरणम्) !=
                (did_gscid & आवरणम्)) {
                अनुवर्त;
            }
        }

        hpm_incr_ctr(s, ctr_idx);
    }
}

/* Timer callback for cycle counter overflow. */
शून्य riscv_iommu_hpm_timer_cb(शून्य *priv)
{
    RISCVIOMMUState *s = priv;
    ध्रुव uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t ovf;

    यदि (get_क्षेत्रम्(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        प्रत्यागम;
    }

    यदि (s->irq_अतिप्रवाहः_वामः > 0) {
        uint64_t irq_trigger_at =
            qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->irq_अतिप्रवाहः_वामः;
        timer_mod_anticipate_ns(s->hpm_timer, irq_trigger_at);
        s->irq_अतिप्रवाहः_वामः = 0;
        प्रत्यागम;
    }

    ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    यदि (!get_क्षेत्रम्(ovf, RISCV_IOMMU_IOCOUNTOVF_CY)) {
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

स्थिर शून्य hpm_सज्जता_timer(RISCVIOMMUState *s, uint64_t मानम्)
{
    ध्रुव uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint64_t अतिप्रवाहः_at, अतिप्रवाहः_ns;

    यदि (get_क्षेत्रम्(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        प्रत्यागम;
    }

    /*
     * We are using INT64_MAX here instead to UINT64_MAX because cycle counter
     * has 63-bit precision and INT64_MAX is the maximum it can store.
     */
    यदि (मानम्) {
        अतिप्रवाहः_ns = INT64_MAX - मानम् + 1;
    } अन्यथा {
        अतिप्रवाहः_ns = INT64_MAX;
    }

    अतिप्रवाहः_at = (uint64_t)qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + अतिप्रवाहः_ns;
    s->irq_अतिप्रवाहः_वामः = 0;

    यदि (अतिप्रवाहः_at > INT64_MAX) {
        s->irq_अतिप्रवाहः_वामः = अतिप्रवाहः_at - INT64_MAX;
        अतिप्रवाहः_at = INT64_MAX;
    }

    timer_mod_anticipate_ns(s->hpm_timer, अतिप्रवाहः_at);
}

/* Updates the internal cycle counter state when iocntinh:CY is changed. */
शून्य riscv_iommu_process_iocntinh_cy(RISCVIOMMUState *s, bool पूर्व_cy_inh)
{
    ध्रुव uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);

    /* We only need to process CY bit toggle. */
    bool cy_inh = !!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY);
    यदि (cy_inh == पूर्व_cy_inh) {
        प्रत्यागम;
    }

    trace_riscv_iommu_hpm_iocntinh_cy(पूर्व_cy_inh);

    यदि (!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Cycle counter is enabled. Just start the timer again and update
         * the clock snapshot value to point to the current time to make
         * sure iohpmcycles read is correct.
         */
        s->hpmcycle_पूर्व = get_आवृत्तयः();
        hpm_सज्जता_timer(s, s->hpmcycle_val);
    } अन्यथा {
        /*
         * Cycle counter is disabled. Stop the timer and update the cycle
         * counter to record the current value which is last programmed
         * value + the cycles passed so far.
         */
        s->hpmcycle_val = s->hpmcycle_val + (get_आवृत्तयः() - s->hpmcycle_पूर्व);
        timer_del(s->hpm_timer);
    }
}

शून्य riscv_iommu_process_hpmcycle_write(RISCVIOMMUState *s)
{
    ध्रुव uint64_t val = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_IOHPMCYCLES);
    ध्रुव uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);

    trace_riscv_iommu_hpm_आवृत्तिः_write(ovf, val);

    /*
     * Clear OF bit in IOCNTOVF if it's being cleared in IOHPMCYCLES register.
     */
    यदि (get_क्षेत्रम्(ovf, RISCV_IOMMU_IOCOUNTOVF_CY) &&
        !get_क्षेत्रम्(val, RISCV_IOMMU_IOHPMCYCLES_OVF)) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF, 0,
            RISCV_IOMMU_IOCOUNTOVF_CY);
    }

    s->hpmcycle_val = val & ~RISCV_IOMMU_IOHPMCYCLES_OVF;
    s->hpmcycle_पूर्व = get_आवृत्तयः();
    hpm_सज्जता_timer(s, s->hpmcycle_val);
}

स्थिर अन्तःस्थ bool check_valid_event_id(अचिह्नित event_id)
{
    प्रत्यागम event_id > RISCV_IOMMU_HPMEVENT_INVALID &&
           event_id < RISCV_IOMMU_HPMEVENT_MAX;
}

स्थिर gboolean hpm_event_equal(gpointer कुञ्चिका, gpointer मानम्, gpointer udata)
{
    uint32_t *pair = udata;

    यदि (GPOINTER_गन्तव्यम्_UINT(मानम्) & (1 << pair[0])) {
        pair[1] = GPOINTER_गन्तव्यम्_UINT(कुञ्चिका);
        प्रत्यागम true;
    }

    प्रत्यागम false;
}

/* Caller must check ctr_idx against hpm_ctrs to see if its supported or not. */
स्थिर शून्य update_event_map(RISCVIOMMUState *s, uint64_t मानम्,
                             uint32_t ctr_idx)
{
    अचिह्नित event_id = get_क्षेत्रम्(मानम्, RISCV_IOMMU_IOHPMEVT_EVENT_ID);
    uint32_t pair[2] = { ctr_idx, RISCV_IOMMU_HPMEVENT_INVALID };
    uint32_t new_मानम् = 1 << ctr_idx;
    gpointer दत्त;

    /*
     * If EventID field is RISCV_IOMMU_HPMEVENT_INVALID
     * remove the current mapping.
     */
    यदि (event_id == RISCV_IOMMU_HPMEVENT_INVALID) {
        दत्त = g_hash_सारणी_find(s->hpm_event_ctr_map, hpm_event_equal, pair);

        new_मानम् = GPOINTER_गन्तव्यम्_UINT(दत्त) & ~(new_मानम्);
        यदि (new_मानम् != 0) {
            g_hash_सारणी_replace(s->hpm_event_ctr_map,
                                 GUINT_गन्तव्यम्_POINTER(pair[1]),
                                 GUINT_गन्तव्यम्_POINTER(new_मानम्));
        } अन्यथा {
            g_hash_सारणी_remove(s->hpm_event_ctr_map,
                                GUINT_गन्तव्यम्_POINTER(pair[1]));
        }

        प्रत्यागम;
    }

    /* Update the counter mask if the event is already enabled. */
    यदि (g_hash_सारणी_lookup_extended(s->hpm_event_ctr_map,
                                     GUINT_गन्तव्यम्_POINTER(event_id),
                                     रिक्त,
                                     &दत्त)) {
        new_मानम् |= GPOINTER_गन्तव्यम्_UINT(दत्त);
    }

    g_hash_सारणी_निवेशय(s->hpm_event_ctr_map,
                        GUINT_गन्तव्यम्_POINTER(event_id),
                        GUINT_गन्तव्यम्_POINTER(new_मानम्));
}

शून्य riscv_iommu_process_hpmevt_write(RISCVIOMMUState *s, uint32_t evt_reg)
{
    ध्रुव uint32_t ctr_idx = (evt_reg - RISCV_IOMMU_REG_IOHPMEVT_आधारः) >> 3;
    ध्रुव uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    uint64_t val = riscv_iommu_reg_get64(s, evt_reg);

    यदि (ctr_idx >= s->hpm_cntrs) {
        प्रत्यागम;
    }

    trace_riscv_iommu_hpm_evt_write(ctr_idx, ovf, val);

    /* Clear OF bit in IOCNTOVF if it's being cleared in IOHPMEVT register. */
    यदि (get_क्षेत्रम्(ovf, BIT(ctr_idx + 1)) &&
        !get_क्षेत्रम्(val, RISCV_IOMMU_IOHPMEVT_सम्बन्धि)) {
        /* +1 to offset CYCLE register OF bit. */
        riscv_iommu_reg_mod32(
            s, RISCV_IOMMU_REG_IOCOUNTOVF, 0, BIT(ctr_idx + 1));
    }

    यदि (!check_valid_event_id(get_क्षेत्रम्(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID))) {
        /* Reset EventID (WARL) field to invalid. */
        val = समुच्चयः_क्षेत्रम्(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID,
            RISCV_IOMMU_HPMEVENT_INVALID);
        riscv_iommu_reg_set64(s, evt_reg, val);
    }

    update_event_map(s, val, ctr_idx);
}
