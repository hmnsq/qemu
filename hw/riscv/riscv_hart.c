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

#समावेश "qemu/osdep.h"
#समावेश "qapi/error.h"
#समावेश "qemu/module.h"
#समावेश "system/reset.h"
#समावेश "system/qtest.h"
#समावेश "qemu/cutils.h"
#समावेश "hw/core/sysbus.h"
#समावेश "target/riscv/cpu.h"
#समावेश "hw/core/qdev-properties.h"
#समावेश "hw/riscv/riscv_hart.h"
#समावेश "qemu/error-report.h"

अटल स्थिर Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, अंक_harts, 1),
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
                      अंक_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      अंक_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, memory,
                     TYPE_MEMORY_REGION, MemoryRegion *),
};

अटल व्योम riscv_harts_cpu_reset(व्योम *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#अगर defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
अटल व्योम csr_call(अक्षर *cmd, uint64_t cpu_अंक, पूर्णांक csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_अंक));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    अगर (म_भेद(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } अन्यथा अगर (म_भेद(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

अटल bool csr_qtest_callback(CharFrontend *chr, gchar **शब्दगण)
{
    अगर (म_भेद(शब्दगण[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        पूर्णांक rc, csr;

        rc = qemu_strtou64(शब्दगण[2], शून्य, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(शब्दगण[3], शून्य, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(शब्दगण[4], शून्य, 0, &val);
        g_assert(rc == 0);
        csr_call(शब्दगण[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        वापस true;
    }

    वापस false;
}

अटल व्योम riscv_cpu_register_csr_qtest_callback(व्योम)
{
    अटल bool first = true;
    अगर (first) {
        first = false;
        qtest_set_command_cb(csr_qtest_callback);
    }
}
#पूर्ण_अगर

अटल bool riscv_hart_realize(RISCVHartArrayState *s, पूर्णांक idx,
                               अक्षर *cpu_type, Error **errp)
{
    object_initialize_child(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_set_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    अगर (s->memory) {
        object_property_set_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->memory), &error_abort);
    }

    अगर (s->harts[idx].cfg.ext_smrnmi) {
        अगर (idx < s->अंक_rnmi_irqvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        अगर (idx < s->अंक_rnmi_excpvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } अन्यथा {
        अगर (s->अंक_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        अगर (s->अंक_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_base + idx;
    qemu_register_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    वापस qdev_realize(DEVICE(&s->harts[idx]), शून्य, errp);
}

अटल व्योम riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    पूर्णांक n;

    s->harts = g_new0(RISCVCPU, s->अंक_harts);

#अगर defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    अगर (qtest_enabled()) {
        riscv_cpu_register_csr_qtest_callback();
    }
#पूर्ण_अगर

    क्रम (n = 0; n < s->अंक_harts; n++) {
        अगर (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            वापस;
        }
    }
}

अटल व्योम riscv_harts_class_init(ObjectClass *klass, स्थिर व्योम *तथ्य)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

अटल स्थिर TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_आकार = माप(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

अटल व्योम riscv_harts_register_types(व्योम)
{
    type_register_static(&riscv_harts_info);
}

type_init(riscv_harts_register_types)
