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

#ضمن "qemu/osdep.h"
#ضمن "qapi/error.h"
#ضمن "qemu/module.h"
#ضمن "system/reset.h"
#ضمن "system/qtest.h"
#ضمن "qemu/cutils.h"
#ضمن "hw/core/sysbus.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "qemu/error-report.h"

ساكن ثابت Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, انگ_harts, 1),
    DEFINE_PROP_UINT32("hartid-base", RISCVHartArrayState, hartid_بنياد, 0),
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
                      انگ_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      انگ_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, يادگيري,
                     TYPE_يادگيري_REGION, يادگيري_Region *),
};

ساكن فراغ riscv_harts_cpu_reset(فراغ *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#إذا defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
ساكن فراغ csr_call(حرف *cmd, uint64_t cpu_انگ, صحيح csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_انگ));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    إذا (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } وإلا إذا (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

ساكن bool csr_qtest_callback(CharFrontend *chr, gchar **لفظ)
{
    إذا (strcmp(لفظ[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        صحيح rc, csr;

        rc = qemu_strtou64(لفظ[2], عدم, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(لفظ[3], عدم, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(لفظ[4], عدم, 0, &val);
        g_assert(rc == 0);
        csr_call(لفظ[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        أرجع true;
    }

    أرجع false;
}

ساكن فراغ riscv_cpu_رجسٽر_csr_qtest_callback(فراغ)
{
    ساكن bool پهريون = true;
    إذا (پهريون) {
        پهريون = false;
        qtest_سيٽ_command_cb(csr_qtest_callback);
    }
}
#جيڪڏهن_پورو

ساكن bool riscv_hart_realize(RISCVHartArrayState *s, صحيح idx,
                               حرف *cpu_type, Error **errp)
{
    object_initialize_ٻار(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_سيٽ_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    إذا (s->يادگيري) {
        object_property_سيٽ_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->يادگيري), &error_abort);
    }

    إذا (s->harts[idx].cfg.ext_smrnmi) {
        إذا (idx < s->انگ_rnmi_irqvec) {
            qdev_prop_سيٽ_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        إذا (idx < s->انگ_rnmi_excpvec) {
            qdev_prop_سيٽ_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } وإلا {
        إذا (s->انگ_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        إذا (s->انگ_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_بنياد + idx;
    qemu_رجسٽر_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    أرجع qdev_realize(DEVICE(&s->harts[idx]), عدم, errp);
}

ساكن فراغ riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    صحيح n;

    s->harts = g_new0(RISCVCPU, s->انگ_harts);

#إذا defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    إذا (qtest_enabled()) {
        riscv_cpu_رجسٽر_csr_qtest_callback();
    }
#جيڪڏهن_پورو

    لكل (n = 0; n < s->انگ_harts; n++) {
        إذا (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            أرجع;
        }
    }
}

ساكن فراغ riscv_harts_class_init(ObjectClass *klass, ثابت فراغ *معلومات)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_سيٽ_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

ساكن ثابت TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .والد        = TYPE_SYS_BUS_DEVICE,
    .instance_سائيز = حجم(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

ساكن فراغ riscv_harts_رجسٽر_types(فراغ)
{
    type_رجسٽر_static(&riscv_harts_info);
}

type_init(riscv_harts_رجسٽر_types)
