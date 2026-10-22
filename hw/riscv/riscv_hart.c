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

#شامل "qemu/osdep.h"
#شامل "qapi/error.h"
#شامل "qemu/module.h"
#شامل "system/reset.h"
#شامل "system/qtest.h"
#شامل "qemu/cutils.h"
#شامل "hw/core/sysbus.h"
#شامل "target/riscv/cpu.h"
#شامل "hw/core/qdev-properties.h"
#شامل "hw/riscv/riscv_hart.h"
#شامل "qemu/error-report.h"

ساکن ثابت Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, عدد_harts, 1),
    DEFINE_PROP_UINT32("hartid-base", RISCVHartArrayState, hartid_بنیاد, 0),
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
                      عدد_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      عدد_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, حافظہ,
                     TYPE_حافظہ_REGION, حافظہ_Region *),
};

ساکن خالی riscv_harts_cpu_reset(خالی *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#اگر defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
ساکن خالی csr_call(حرف *cmd, uint64_t cpu_عدد, صحیح csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_عدد));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    اگر (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } ورنہ اگر (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

ساکن bool csr_qtest_callback(CharFrontend *chr, gchar **الفاظ)
{
    اگر (strcmp(الفاظ[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        صحیح rc, csr;

        rc = qemu_strtou64(الفاظ[2], تہی, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(الفاظ[3], تہی, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(الفاظ[4], تہی, 0, &val);
        g_assert(rc == 0);
        csr_call(الفاظ[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        واپس true;
    }

    واپس false;
}

ساکن خالی riscv_cpu_رجسٹر_csr_qtest_callback(خالی)
{
    ساکن bool پہلا = true;
    اگر (پہلا) {
        پہلا = false;
        qtest_سیٹ_command_cb(csr_qtest_callback);
    }
}
#اگر_ختم

ساکن bool riscv_hart_realize(RISCVHartArrayState *s, صحیح idx,
                               حرف *cpu_type, Error **errp)
{
    object_initialize_بچہ(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_سیٹ_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    اگر (s->حافظہ) {
        object_property_سیٹ_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->حافظہ), &error_abort);
    }

    اگر (s->harts[idx].cfg.ext_smrnmi) {
        اگر (idx < s->عدد_rnmi_irqvec) {
            qdev_prop_سیٹ_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        اگر (idx < s->عدد_rnmi_excpvec) {
            qdev_prop_سیٹ_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } ورنہ {
        اگر (s->عدد_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        اگر (s->عدد_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_بنیاد + idx;
    qemu_رجسٹر_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    واپس qdev_realize(DEVICE(&s->harts[idx]), تہی, errp);
}

ساکن خالی riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    صحیح n;

    s->harts = g_new0(RISCVCPU, s->عدد_harts);

#اگر defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    اگر (qtest_enabled()) {
        riscv_cpu_رجسٹر_csr_qtest_callback();
    }
#اگر_ختم

    برائے (n = 0; n < s->عدد_harts; n++) {
        اگر (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            واپس;
        }
    }
}

ساکن خالی riscv_harts_class_init(ObjectClass *klass, ثابت خالی *ڈیٹا)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_سیٹ_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

ساکن ثابت TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .والد        = TYPE_SYS_BUS_DEVICE,
    .instance_سائز = حجم(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

ساکن خالی riscv_harts_رجسٹر_types(خالی)
{
    type_رجسٹر_static(&riscv_harts_info);
}

type_init(riscv_harts_رجسٹر_types)
