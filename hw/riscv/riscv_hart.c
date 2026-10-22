/*
 * QEMU RISCV Hart Array
 *
 * Copyright (c) 2017 SiFive, Inc.
 *
 * Holds the state of a homogeneous array of RISC-V harts
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qemu/module.h"
#అన్తర్భావ "system/reset.h"
#అన్తర్భావ "system/qtest.h"
#అన్తర్భావ "qemu/cutils.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "qemu/error-report.h"

స్థిర స్థిరరాశి Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, అంకె_harts, 1),
    DEFINE_PROP_UINT32("hartid-base", RISCVHartArrayState, hartid_పునాది, 0),
    DEFINE_PROP_STRING("cpu-type", RISCVHartArrayState, cpu_type),
    DEFINE_PROP_UINT64("resetvec", RISCVHartArrayState, resetvec,
                       DEFAULT_RSTVEC),

    /*
     * Smrnmi implementation-defined interrupt and exception trap handlers.
     *
     * When an RNMI interrupt is detected, the hart then enters M-mode and
     * jumps to the address defined by "rnmi-interrupt-vector".
     *
     * When the hart encounters an exception while executing in M-mode with
     * the mnstatus.NMIE bit clear, the hart then jumps to the address
     * defined by "rnmi-exception-vector".
     */
    DEFINE_PROP_ARRAY("rnmi-interrupt-vector", RISCVHartArrayState,
                      అంకె_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      అంకె_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, జ్ఞాపకం,
                     TYPE_జ్ఞాపకం_REGION, జ్ఞాపకం_Region *),
};

స్థిర శూన్య riscv_harts_cpu_reset(శూన్య *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#ఒకవేళ defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
స్థిర శూన్య csr_call(అక్షరం *cmd, uint64_t cpu_అంకె, పూర్ణాంకం csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_అంకె));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    ఒకవేళ (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } లేకపోతే ఒకవేళ (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

స్థిర bool csr_qtest_callback(CharFrontend *chr, gchar **పదాలు)
{
    ఒకవేళ (strcmp(పదాలు[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        పూర్ణాంకం rc, csr;

        rc = qemu_strtou64(పదాలు[2], ఖాళీ, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(పదాలు[3], ఖాళీ, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(పదాలు[4], ఖాళీ, 0, &val);
        g_assert(rc == 0);
        csr_call(పదాలు[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        తిరిగి true;
    }

    తిరిగి false;
}

స్థిర శూన్య riscv_cpu_రిజిస్టరు_csr_qtest_callback(శూన్య)
{
    స్థిర bool మొదటి = true;
    ఒకవేళ (మొదటి) {
        మొదటి = false;
        qtest_సమితి_command_cb(csr_qtest_callback);
    }
}
#అయితే_ముగింపు

స్థిర bool riscv_hart_realize(RISCVHartArrayState *s, పూర్ణాంకం idx,
                               అక్షరం *cpu_type, Error **errp)
{
    object_initialize_బిడ్డ(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_సమితి_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    ఒకవేళ (s->జ్ఞాపకం) {
        object_property_సమితి_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->జ్ఞాపకం), &error_abort);
    }

    ఒకవేళ (s->harts[idx].cfg.ext_smrnmi) {
        ఒకవేళ (idx < s->అంకె_rnmi_irqvec) {
            qdev_prop_సమితి_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        ఒకవేళ (idx < s->అంకె_rnmi_excpvec) {
            qdev_prop_సమితి_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } లేకపోతే {
        ఒకవేళ (s->అంకె_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        ఒకవేళ (s->అంకె_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_పునాది + idx;
    qemu_రిజిస్టరు_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    తిరిగి qdev_realize(DEVICE(&s->harts[idx]), ఖాళీ, errp);
}

స్థిర శూన్య riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    పూర్ణాంకం n;

    s->harts = g_new0(RISCVCPU, s->అంకె_harts);

#ఒకవేళ defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    ఒకవేళ (qtest_enabled()) {
        riscv_cpu_రిజిస్టరు_csr_qtest_callback();
    }
#అయితే_ముగింపు

    కొరకు (n = 0; n < s->అంకె_harts; n++) {
        ఒకవేళ (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            తిరిగి;
        }
    }
}

స్థిర శూన్య riscv_harts_class_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_సమితి_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

స్థిర స్థిరరాశి TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .మాతృ        = TYPE_SYS_BUS_DEVICE,
    .instance_కొలత = పరిమాణం(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

స్థిర శూన్య riscv_harts_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&riscv_harts_info);
}

type_init(riscv_harts_రిజిస్టరు_types)
