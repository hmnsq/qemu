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

#ଅନ୍ତର୍ଭାଵ "qemu/osdep.h"
#ଅନ୍ତର୍ଭାଵ "qapi/error.h"
#ଅନ୍ତର୍ଭାଵ "qemu/module.h"
#ଅନ୍ତର୍ଭାଵ "system/reset.h"
#ଅନ୍ତର୍ଭାଵ "system/qtest.h"
#ଅନ୍ତର୍ଭାଵ "qemu/cutils.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/sysbus.h"
#ଅନ୍ତର୍ଭାଵ "target/riscv/cpu.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/qdev-properties.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/riscv_hart.h"
#ଅନ୍ତର୍ଭାଵ "qemu/error-report.h"

ସ୍ଥିର ଅଚଳ Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, ଅଙ୍କ_harts, 1),
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
                      ଅଙ୍କ_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      ଅଙ୍କ_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, memory,
                     TYPE_MEMORY_REGION, MemoryRegion *),
};

ସ୍ଥିର ଶୂନ୍ୟ riscv_harts_cpu_reset(ଶୂନ୍ୟ *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#ଯଦି defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
ସ୍ଥିର ଶୂନ୍ୟ csr_call(ଅକ୍ଷର *cmd, uint64_t cpu_ଅଙ୍କ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_ଅଙ୍କ));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    ଯଦି (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } ଅନ୍ୟଥା ଯଦି (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

ସ୍ଥିର bool csr_qtest_callback(CharFrontend *chr, gchar **ଶବ୍ଦଗୁଡ଼ିକ)
{
    ଯଦି (strcmp(ଶବ୍ଦଗୁଡ଼ିକ[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, csr;

        rc = qemu_strtou64(ଶବ୍ଦଗୁଡ଼ିକ[2], ଖାଲି, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(ଶବ୍ଦଗୁଡ଼ିକ[3], ଖାଲି, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(ଶବ୍ଦଗୁଡ଼ିକ[4], ଖାଲି, 0, &val);
        g_assert(rc == 0);
        csr_call(ଶବ୍ଦଗୁଡ଼ିକ[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        ଫେରନ୍ତୁ true;
    }

    ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ଶୂନ୍ୟ riscv_cpu_register_csr_qtest_callback(ଶୂନ୍ୟ)
{
    ସ୍ଥିର bool first = true;
    ଯଦି (first) {
        first = false;
        qtest_set_command_cb(csr_qtest_callback);
    }
}
#endif

ସ୍ଥିର bool riscv_hart_realize(RISCVHartArrayState *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx,
                               ଅକ୍ଷର *cpu_type, Error **errp)
{
    object_initialize_child(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_set_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    ଯଦି (s->memory) {
        object_property_set_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->memory), &error_abort);
    }

    ଯଦି (s->harts[idx].cfg.ext_smrnmi) {
        ଯଦି (idx < s->ଅଙ୍କ_rnmi_irqvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        ଯଦି (idx < s->ଅଙ୍କ_rnmi_excpvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } ଅନ୍ୟଥା {
        ଯଦି (s->ଅଙ୍କ_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        ଯଦି (s->ଅଙ୍କ_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_base + idx;
    qemu_register_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    ଫେରନ୍ତୁ qdev_realize(DEVICE(&s->harts[idx]), ଖାଲି, errp);
}

ସ୍ଥିର ଶୂନ୍ୟ riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;

    s->harts = g_new0(RISCVCPU, s->ଅଙ୍କ_harts);

#ଯଦି defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    ଯଦି (qtest_enabled()) {
        riscv_cpu_register_csr_qtest_callback();
    }
#endif

    ପାଇଁ (n = 0; n < s->ଅଙ୍କ_harts; n++) {
        ଯଦି (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            ଫେରନ୍ତୁ;
        }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ riscv_harts_class_init(ObjectClass *klass, ଅଚଳ ଶୂନ୍ୟ *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

ସ୍ଥିର ଅଚଳ TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = ଆକାର(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

ସ୍ଥିର ଶୂନ୍ୟ riscv_harts_register_types(ଶୂନ୍ୟ)
{
    type_register_static(&riscv_harts_info);
}

type_init(riscv_harts_register_types)
