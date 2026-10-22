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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/timer.h"
#అన్తర్భావ "cpu_bits.h"
#అన్తర్భావ "riscv-iommu-hpm.h"
#అన్తర్భావ "riscv-iommu.h"
#అన్తర్భావ "riscv-iommu-bits.h"
#అన్తర్భావ "trace.h"

/* For now we assume IOMMU HPM frequency to be 1GHz so 1-cycle is of 1-ns. */
స్థిర అంతఃవరుస uint64_t get_చక్రాలు(శూన్య)
{
    తిరిగి qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
}

uint64_t riscv_iommu_hpmcycle_read(RISCVIOMMUState *s)
{
    స్థిరరాశి uint64_t చక్రం = riscv_iommu_reg_get64(
        s, RISCV_IOMMU_REG_IOHPMCYCLES);
    స్థిరరాశి uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    స్థిరరాశి uint64_t ctr_prev = s->hpmcycle_prev;
    స్థిరరాశి uint64_t ctr_val = s->hpmcycle_val;

    trace_riscv_iommu_hpm_read(చక్రం, inhibit, ctr_prev, ctr_val);

    ఒకవేళ (get_క్షేత్రం(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Counter should not increment if inhibit bit is set. We can't really
         * stop the QEMU_CLOCK_VIRTUAL, so we just return the last updated
         * counter value to indicate that counter was not incremented.
         */
        తిరిగి (ctr_val & RISCV_IOMMU_IOHPMCYCLES_COUNTER) |
               (చక్రం & RISCV_IOMMU_IOHPMCYCLES_OVF);
    }

    తిరిగి (ctr_val + get_చక్రాలు() - ctr_prev) |
        (చక్రం & RISCV_IOMMU_IOHPMCYCLES_OVF);
}

స్థిర శూన్య hpm_incr_ctr(RISCVIOMMUState *s, uint32_t ctr_idx)
{
    స్థిరరాశి uint32_t off = ctr_idx << 3;
    uint64_t cntr_val;

    cntr_val = ldq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_పునాది + off]);
    stq_le_p(&s->regs[RISCV_IOMMU_REG_IOHPMCTR_పునాది + off], cntr_val + 1);

    trace_riscv_iommu_hpm_incr_ctr(cntr_val);

    /* Handle the overflow scenario. */
    ఒకవేళ (cntr_val == UINT64_MAX) {
        /*
         * Generate interrupt only if OF bit is clear. +1 to offset the cycle
         * register OF bit.
         */
        స్థిరరాశి uint32_t ovf =
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF,
                                  బిట్టు(ctr_idx + 1), 0);
        ఒకవేళ (!get_క్షేత్రం(ovf, బిట్టు(ctr_idx + 1))) {
            riscv_iommu_reg_mod64(s,
                                  RISCV_IOMMU_REG_IOHPMEVT_పునాది + off,
                                  RISCV_IOMMU_IOHPMEVT_యొక్క,
                                  0);
            riscv_iommu_notify(s, RISCV_IOMMU_INTR_PM);
        }
    }
}

శూన్య riscv_iommu_hpm_incr_ctr(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
                              గుర్తులేని event_id)
{
    స్థిరరాశి uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t did_gscid;
    uint32_t pid_pscid;
    uint32_t ctr_idx;
    gpointer విలువ;
    uint32_t ctrs;
    uint64_t evt;

    ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_HPM)) {
        తిరిగి;
    }

    విలువ = g_hash_పట్టిక_lookup(s->hpm_event_ctr_map,
                                GUINT_గమ్యం_సూచిక(event_id));
    ఒకవేళ (విలువ == ఖాళీ) {
        తిరిగి;
    }

    కొరకు (ctrs = GPOINTER_గమ్యం_UINT(విలువ); ctrs != 0; ctrs &= ctrs - 1) {
        ctr_idx = ctz32(ctrs);
        ఒకవేళ (get_క్షేత్రం(inhibit, బిట్టు(ctr_idx + 1))) {
            కొనసాగు;
        }

        evt = riscv_iommu_reg_get64(s,
            RISCV_IOMMU_REG_IOHPMEVT_పునాది + (ctr_idx << 3));

        /*
         * It's quite possible that event ID has been changed in counter
         * but hashtable hasn't been updated yet. We don't want to increment
         * counter for the old event ID.
         */
        ఒకవేళ (event_id != get_క్షేత్రం(evt, RISCV_IOMMU_IOHPMEVT_EVENT_ID)) {
            కొనసాగు;
        }

        ఒకవేళ (get_క్షేత్రం(evt, RISCV_IOMMU_IOHPMEVT_IDT)) {
            did_gscid = get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
            pid_pscid = get_క్షేత్రం(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        } లేకపోతే {
            did_gscid = ctx->devid;
            pid_pscid = ctx->process_id;
        }

        ఒకవేళ (get_క్షేత్రం(evt, RISCV_IOMMU_IOHPMEVT_PV_PSCV)) {
            /*
             * If the transaction does not have a valid process_id, counter
             * increments if device_id matches DID_GSCID. If the transaction
             * has a valid process_id, counter increments if device_id
             * matches DID_GSCID and process_id matches PID_PSCID. See
             * IOMMU Specification, Chapter 5.23. Performance-monitoring
             * event selector.
             */
            ఒకవేళ (ctx->process_id &&
                get_క్షేత్రం(evt, RISCV_IOMMU_IOHPMEVT_PID_PSCID) != pid_pscid) {
                కొనసాగు;
            }
        }

        ఒకవేళ (get_క్షేత్రం(evt, RISCV_IOMMU_IOHPMEVT_DV_GSCV)) {
            uint32_t ముసుగు = ~0;

            ఒకవేళ (get_క్షేత్రం(evt, RISCV_IOMMU_IOHPMEVT_DMASK)) {
                /*
                 * 1001 1011   mask = GSCID
                 * 0000 0111   mask = mask ^ (mask + 1)
                 * 1111 1000   mask = ~mask;
                 */
                ముసుగు = get_క్షేత్రం(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID);
                ముసుగు = ముసుగు ^ (ముసుగు + 1);
                ముసుగు = ~ముసుగు;
            }

            ఒకవేళ ((get_క్షేత్రం(evt, RISCV_IOMMU_IOHPMEVT_DID_GSCID) & ముసుగు) !=
                (did_gscid & ముసుగు)) {
                కొనసాగు;
            }
        }

        hpm_incr_ctr(s, ctr_idx);
    }
}

/* Timer callback for cycle counter overflow. */
శూన్య riscv_iommu_hpm_timer_cb(శూన్య *priv)
{
    RISCVIOMMUState *s = priv;
    స్థిరరాశి uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint32_t ovf;

    ఒకవేళ (get_క్షేత్రం(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        తిరిగి;
    }

    ఒకవేళ (s->irq_అధికప్రవాహం_ఎడమ > 0) {
        uint64_t irq_trigger_at =
            qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + s->irq_అధికప్రవాహం_ఎడమ;
        timer_mod_anticipate_ns(s->hpm_timer, irq_trigger_at);
        s->irq_అధికప్రవాహం_ఎడమ = 0;
        తిరిగి;
    }

    ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    ఒకవేళ (!get_క్షేత్రం(ovf, RISCV_IOMMU_IOCOUNTOVF_CY)) {
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

స్థిర శూన్య hpm_అమరిక_timer(RISCVIOMMUState *s, uint64_t విలువ)
{
    స్థిరరాశి uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);
    uint64_t అధికప్రవాహం_at, అధికప్రవాహం_ns;

    ఒకవేళ (get_క్షేత్రం(inhibit, RISCV_IOMMU_IOCOUNTINH_CY)) {
        తిరిగి;
    }

    /*
     * We are using INT64_MAX here instead to UINT64_MAX because cycle counter
     * has 63-bit precision and INT64_MAX is the maximum it can store.
     */
    ఒకవేళ (విలువ) {
        అధికప్రవాహం_ns = INT64_MAX - విలువ + 1;
    } లేకపోతే {
        అధికప్రవాహం_ns = INT64_MAX;
    }

    అధికప్రవాహం_at = (uint64_t)qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + అధికప్రవాహం_ns;
    s->irq_అధికప్రవాహం_ఎడమ = 0;

    ఒకవేళ (అధికప్రవాహం_at > INT64_MAX) {
        s->irq_అధికప్రవాహం_ఎడమ = అధికప్రవాహం_at - INT64_MAX;
        అధికప్రవాహం_at = INT64_MAX;
    }

    timer_mod_anticipate_ns(s->hpm_timer, అధికప్రవాహం_at);
}

/* Updates the internal cycle counter state when iocntinh:CY is changed. */
శూన్య riscv_iommu_process_iocntinh_cy(RISCVIOMMUState *s, bool prev_cy_inh)
{
    స్థిరరాశి uint32_t inhibit = riscv_iommu_reg_get32(
        s, RISCV_IOMMU_REG_IOCOUNTINH);

    /* We only need to process CY bit toggle. */
    bool cy_inh = !!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY);
    ఒకవేళ (cy_inh == prev_cy_inh) {
        తిరిగి;
    }

    trace_riscv_iommu_hpm_iocntinh_cy(prev_cy_inh);

    ఒకవేళ (!(inhibit & RISCV_IOMMU_IOCOUNTINH_CY)) {
        /*
         * Cycle counter is enabled. Just start the timer again and update
         * the clock snapshot value to point to the current time to make
         * sure iohpmcycles read is correct.
         */
        s->hpmcycle_prev = get_చక్రాలు();
        hpm_అమరిక_timer(s, s->hpmcycle_val);
    } లేకపోతే {
        /*
         * Cycle counter is disabled. Stop the timer and update the cycle
         * counter to record the current value which is last programmed
         * value + the cycles passed so far.
         */
        s->hpmcycle_val = s->hpmcycle_val + (get_చక్రాలు() - s->hpmcycle_prev);
        timer_del(s->hpm_timer);
    }
}

శూన్య riscv_iommu_process_hpmcycle_write(RISCVIOMMUState *s)
{
    స్థిరరాశి uint64_t val = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_IOHPMCYCLES);
    స్థిరరాశి uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);

    trace_riscv_iommu_hpm_చక్రం_write(ovf, val);

    /*
     * Clear OF bit in IOCNTOVF if it's being cleared in IOHPMCYCLES register.
     */
    ఒకవేళ (get_క్షేత్రం(ovf, RISCV_IOMMU_IOCOUNTOVF_CY) &&
        !get_క్షేత్రం(val, RISCV_IOMMU_IOHPMCYCLES_OVF)) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IOCOUNTOVF, 0,
            RISCV_IOMMU_IOCOUNTOVF_CY);
    }

    s->hpmcycle_val = val & ~RISCV_IOMMU_IOHPMCYCLES_OVF;
    s->hpmcycle_prev = get_చక్రాలు();
    hpm_అమరిక_timer(s, s->hpmcycle_val);
}

స్థిర అంతఃవరుస bool check_valid_event_id(గుర్తులేని event_id)
{
    తిరిగి event_id > RISCV_IOMMU_HPMEVENT_INVALID &&
           event_id < RISCV_IOMMU_HPMEVENT_MAX;
}

స్థిర gboolean hpm_event_equal(gpointer కీలకం, gpointer విలువ, gpointer udata)
{
    uint32_t *pair = udata;

    ఒకవేళ (GPOINTER_గమ్యం_UINT(విలువ) & (1 << pair[0])) {
        pair[1] = GPOINTER_గమ్యం_UINT(కీలకం);
        తిరిగి true;
    }

    తిరిగి false;
}

/* Caller must check ctr_idx against hpm_ctrs to see if its supported or not. */
స్థిర శూన్య update_event_map(RISCVIOMMUState *s, uint64_t విలువ,
                             uint32_t ctr_idx)
{
    గుర్తులేని event_id = get_క్షేత్రం(విలువ, RISCV_IOMMU_IOHPMEVT_EVENT_ID);
    uint32_t pair[2] = { ctr_idx, RISCV_IOMMU_HPMEVENT_INVALID };
    uint32_t new_విలువ = 1 << ctr_idx;
    gpointer దత్తాంశం;

    /*
     * If EventID field is RISCV_IOMMU_HPMEVENT_INVALID
     * remove the current mapping.
     */
    ఒకవేళ (event_id == RISCV_IOMMU_HPMEVENT_INVALID) {
        దత్తాంశం = g_hash_పట్టిక_కనుగొను(s->hpm_event_ctr_map, hpm_event_equal, pair);

        new_విలువ = GPOINTER_గమ్యం_UINT(దత్తాంశం) & ~(new_విలువ);
        ఒకవేళ (new_విలువ != 0) {
            g_hash_పట్టిక_replace(s->hpm_event_ctr_map,
                                 GUINT_గమ్యం_సూచిక(pair[1]),
                                 GUINT_గమ్యం_సూచిక(new_విలువ));
        } లేకపోతే {
            g_hash_పట్టిక_remove(s->hpm_event_ctr_map,
                                GUINT_గమ్యం_సూచిక(pair[1]));
        }

        తిరిగి;
    }

    /* Update the counter mask if the event is already enabled. */
    ఒకవేళ (g_hash_పట్టిక_lookup_extended(s->hpm_event_ctr_map,
                                     GUINT_గమ్యం_సూచిక(event_id),
                                     ఖాళీ,
                                     &దత్తాంశం)) {
        new_విలువ |= GPOINTER_గమ్యం_UINT(దత్తాంశం);
    }

    g_hash_పట్టిక_చొప్పించు(s->hpm_event_ctr_map,
                        GUINT_గమ్యం_సూచిక(event_id),
                        GUINT_గమ్యం_సూచిక(new_విలువ));
}

శూన్య riscv_iommu_process_hpmevt_write(RISCVIOMMUState *s, uint32_t evt_reg)
{
    స్థిరరాశి uint32_t ctr_idx = (evt_reg - RISCV_IOMMU_REG_IOHPMEVT_పునాది) >> 3;
    స్థిరరాశి uint32_t ovf = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTOVF);
    uint64_t val = riscv_iommu_reg_get64(s, evt_reg);

    ఒకవేళ (ctr_idx >= s->hpm_cntrs) {
        తిరిగి;
    }

    trace_riscv_iommu_hpm_evt_write(ctr_idx, ovf, val);

    /* Clear OF bit in IOCNTOVF if it's being cleared in IOHPMEVT register. */
    ఒకవేళ (get_క్షేత్రం(ovf, బిట్టు(ctr_idx + 1)) &&
        !get_క్షేత్రం(val, RISCV_IOMMU_IOHPMEVT_యొక్క)) {
        /* +1 to offset CYCLE register OF bit. */
        riscv_iommu_reg_mod32(
            s, RISCV_IOMMU_REG_IOCOUNTOVF, 0, బిట్టు(ctr_idx + 1));
    }

    ఒకవేళ (!check_valid_event_id(get_క్షేత్రం(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID))) {
        /* Reset EventID (WARL) field to invalid. */
        val = సమితి_క్షేత్రం(val, RISCV_IOMMU_IOHPMEVT_EVENT_ID,
            RISCV_IOMMU_HPMEVENT_INVALID);
        riscv_iommu_reg_set64(s, evt_reg, val);
    }

    update_event_map(s, val, ctr_idx);
}
