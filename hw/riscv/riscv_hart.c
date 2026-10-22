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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/module.h"
#अन्तर्भाव "system/reset.h"
#अन्तर्भाव "system/qtest.h"
#अन्तर्भाव "qemu/cutils.h"
#अन्तर्भाव "hw/core/sysbus.h"
#अन्तर्भाव "target/riscv/cpu.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "hw/riscv/riscv_hart.h"
#अन्तर्भाव "qemu/error-report.h"

स्थिर ध्रुव Property riscv_harts_props[] = {
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

स्थिर शून्य riscv_harts_cpu_reset(शून्य *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#यदि defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
स्थिर शून्य csr_call(वर्ण *cmd, uint64_t cpu_अंक, पूर्णाङ्क csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_अंक));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    यदि (म_भेद(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } अन्यथा यदि (म_भेद(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

स्थिर bool csr_qtest_callback(CharFrontend *chr, gchar **शब्दगण)
{
    यदि (म_भेद(शब्दगण[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        पूर्णाङ्क rc, csr;

        rc = qemu_strtou64(शब्दगण[2], रिक्त, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(शब्दगण[3], रिक्त, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(शब्दगण[4], रिक्त, 0, &val);
        g_assert(rc == 0);
        csr_call(शब्दगण[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        प्रत्यागम true;
    }

    प्रत्यागम false;
}

स्थिर शून्य riscv_cpu_register_csr_qtest_callback(शून्य)
{
    स्थिर bool first = true;
    यदि (first) {
        first = false;
        qtest_set_command_cb(csr_qtest_callback);
    }
}
#endif

स्थिर bool riscv_hart_realize(RISCVHartArrayState *s, पूर्णाङ्क idx,
                               वर्ण *cpu_type, Error **errp)
{
    object_initialize_child(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_set_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    यदि (s->memory) {
        object_property_set_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->memory), &error_abort);
    }

    यदि (s->harts[idx].cfg.ext_smrnmi) {
        यदि (idx < s->अंक_rnmi_irqvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        यदि (idx < s->अंक_rnmi_excpvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } अन्यथा {
        यदि (s->अंक_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        यदि (s->अंक_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_base + idx;
    qemu_register_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    प्रत्यागम qdev_realize(DEVICE(&s->harts[idx]), रिक्त, errp);
}

स्थिर शून्य riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    पूर्णाङ्क n;

    s->harts = g_new0(RISCVCPU, s->अंक_harts);

#यदि defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    यदि (qtest_enabled()) {
        riscv_cpu_register_csr_qtest_callback();
    }
#endif

    प्रति (n = 0; n < s->अंक_harts; n++) {
        यदि (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            प्रत्यागम;
        }
    }
}

स्थिर शून्य riscv_harts_class_init(ObjectClass *klass, ध्रुव शून्य *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

स्थिर ध्रुव TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = परिमाण(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

स्थिर शून्य riscv_harts_register_types(शून्य)
{
    type_register_static(&riscv_harts_info);
}

type_init(riscv_harts_register_types)
