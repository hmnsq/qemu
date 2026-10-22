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

#അന്തര്ഭാവ "qemu/osdep.h"
#അന്തര്ഭാവ "qapi/error.h"
#അന്തര്ഭാവ "qemu/module.h"
#അന്തര്ഭാവ "system/reset.h"
#അന്തര്ഭാവ "system/qtest.h"
#അന്തര്ഭാവ "qemu/cutils.h"
#അന്തര്ഭാവ "hw/core/sysbus.h"
#അന്തര്ഭാവ "target/riscv/cpu.h"
#അന്തര്ഭാവ "hw/core/qdev-properties.h"
#അന്തര്ഭാവ "hw/riscv/riscv_hart.h"
#അന്തര്ഭാവ "qemu/error-report.h"

സ്ഥിതം സ്ഥിരം Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, അക്കം_harts, 1),
    DEFINE_PROP_UINT32("hartid-base", RISCVHartArrayState, hartid_base, 0),
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
                      അക്കം_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      അക്കം_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, memory,
                     TYPE_MEMORY_REGION, MemoryRegion *),
};

സ്ഥിതം ശൂന്യം riscv_harts_cpu_reset(ശൂന്യം *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#എങ്കിൽ defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
സ്ഥിതം ശൂന്യം csr_call(അക്ഷരം *cmd, uint64_t cpu_അക്കം, പൂർണ്ണസംഖ്യ csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_അക്കം));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    എങ്കിൽ (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

സ്ഥിതം bool csr_qtest_callback(CharFrontend *chr, gchar **വാക്കുകൾ)
{
    എങ്കിൽ (strcmp(വാക്കുകൾ[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        പൂർണ്ണസംഖ്യ rc, csr;

        rc = qemu_strtou64(വാക്കുകൾ[2], ശൂന്യ, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(വാക്കുകൾ[3], ശൂന്യ, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(വാക്കുകൾ[4], ശൂന്യ, 0, &val);
        g_assert(rc == 0);
        csr_call(വാക്കുകൾ[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        തിരികെ true;
    }

    തിരികെ false;
}

സ്ഥിതം ശൂന്യം riscv_cpu_register_csr_qtest_callback(ശൂന്യം)
{
    സ്ഥിതം bool first = true;
    എങ്കിൽ (first) {
        first = false;
        qtest_set_command_cb(csr_qtest_callback);
    }
}
#endif

സ്ഥിതം bool riscv_hart_realize(RISCVHartArrayState *s, പൂർണ്ണസംഖ്യ idx,
                               അക്ഷരം *cpu_type, Error **errp)
{
    object_initialize_child(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_set_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    എങ്കിൽ (s->memory) {
        object_property_set_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->memory), &error_abort);
    }

    എങ്കിൽ (s->harts[idx].cfg.ext_smrnmi) {
        എങ്കിൽ (idx < s->അക്കം_rnmi_irqvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        എങ്കിൽ (idx < s->അക്കം_rnmi_excpvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (s->അക്കം_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        എങ്കിൽ (s->അക്കം_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_base + idx;
    qemu_register_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    തിരികെ qdev_realize(DEVICE(&s->harts[idx]), ശൂന്യ, errp);
}

സ്ഥിതം ശൂന്യം riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    പൂർണ്ണസംഖ്യ n;

    s->harts = g_new0(RISCVCPU, s->അക്കം_harts);

#എങ്കിൽ defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    എങ്കിൽ (qtest_enabled()) {
        riscv_cpu_register_csr_qtest_callback();
    }
#endif

    വേണ്ടി (n = 0; n < s->അക്കം_harts; n++) {
        എങ്കിൽ (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            തിരികെ;
        }
    }
}

സ്ഥിതം ശൂന്യം riscv_harts_class_init(ObjectClass *klass, സ്ഥിരം ശൂന്യം *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

സ്ഥിതം സ്ഥിരം TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = വലുപ്പം(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

സ്ഥിതം ശൂന്യം riscv_harts_register_types(ശൂന്യം)
{
    type_register_static(&riscv_harts_info);
}

type_init(riscv_harts_register_types)
