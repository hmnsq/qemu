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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/module.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/reset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/qtest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/cutils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv_hart.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"

ਸਥਿਰ ਅਚਲ Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, ਅੰਕ_harts, 1),
    DEFINE_PROP_UINT32("hartid-base", RISCVHartArrayState, hartid_ਅਧਾਰ, 0),
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
                      ਅੰਕ_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      ਅੰਕ_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, ਸਮ੍ਰਿਤੀ,
                     TYPE_ਸਮ੍ਰਿਤੀ_REGION, ਸਮ੍ਰਿਤੀ_Region *),
};

ਸਥਿਰ ਖਾਲੀ riscv_harts_cpu_reset(ਖਾਲੀ *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#ਜੇ defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
ਸਥਿਰ ਖਾਲੀ csr_call(ਅੱਖਰ *cmd, uint64_t cpu_ਅੰਕ, ਪੂਰਨ csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_ਅੰਕ));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    ਜੇ (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } ਨਹੀਂਤਾਂ ਜੇ (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

ਸਥਿਰ bool csr_qtest_callback(CharFrontend *chr, gchar **ਸ਼ਬਦ)
{
    ਜੇ (strcmp(ਸ਼ਬਦ[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        ਪੂਰਨ rc, csr;

        rc = qemu_strtou64(ਸ਼ਬਦ[2], ਨਿਰਾ, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(ਸ਼ਬਦ[3], ਨਿਰਾ, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(ਸ਼ਬਦ[4], ਨਿਰਾ, 0, &val);
        g_assert(rc == 0);
        csr_call(ਸ਼ਬਦ[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        ਵਾਪਸ true;
    }

    ਵਾਪਸ false;
}

ਸਥਿਰ ਖਾਲੀ riscv_cpu_ਰਜਿਸਟਰ_csr_qtest_callback(ਖਾਲੀ)
{
    ਸਥਿਰ bool ਪਹਿਲਾ = true;
    ਜੇ (ਪਹਿਲਾ) {
        ਪਹਿਲਾ = false;
        qtest_ਸੈੱਟ_command_cb(csr_qtest_callback);
    }
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ bool riscv_hart_realize(RISCVHartArrayState *s, ਪੂਰਨ idx,
                               ਅੱਖਰ *cpu_type, Error **errp)
{
    object_initialize_ਬੱਚਾ(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_ਸੈੱਟ_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    ਜੇ (s->ਸਮ੍ਰਿਤੀ) {
        object_property_ਸੈੱਟ_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->ਸਮ੍ਰਿਤੀ), &error_abort);
    }

    ਜੇ (s->harts[idx].cfg.ext_smrnmi) {
        ਜੇ (idx < s->ਅੰਕ_rnmi_irqvec) {
            qdev_prop_ਸੈੱਟ_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        ਜੇ (idx < s->ਅੰਕ_rnmi_excpvec) {
            qdev_prop_ਸੈੱਟ_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (s->ਅੰਕ_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        ਜੇ (s->ਅੰਕ_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_ਅਧਾਰ + idx;
    qemu_ਰਜਿਸਟਰ_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    ਵਾਪਸ qdev_realize(DEVICE(&s->harts[idx]), ਨਿਰਾ, errp);
}

ਸਥਿਰ ਖਾਲੀ riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    ਪੂਰਨ n;

    s->harts = g_new0(RISCVCPU, s->ਅੰਕ_harts);

#ਜੇ defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    ਜੇ (qtest_enabled()) {
        riscv_cpu_ਰਜਿਸਟਰ_csr_qtest_callback();
    }
#ਜੇ_ਅੰਤ

    ਲਈ (n = 0; n < s->ਅੰਕ_harts; n++) {
        ਜੇ (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            ਵਾਪਸ;
        }
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_harts_class_init(ObjectClass *klass, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_ਸੈੱਟ_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

ਸਥਿਰ ਅਚਲ TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .ਮਾਪਾ        = TYPE_SYS_BUS_DEVICE,
    .instance_ਮਾਪ = ਆਕਾਰ(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

ਸਥਿਰ ਖਾਲੀ riscv_harts_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&riscv_harts_info);
}

type_init(riscv_harts_ਰਜਿਸਟਰ_types)
