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

#অন্তর্ভাৱ "qemu/osdep.h"
#অন্তর্ভাৱ "qapi/error.h"
#অন্তর্ভাৱ "qemu/module.h"
#অন্তর্ভাৱ "system/reset.h"
#অন্তর্ভাৱ "system/qtest.h"
#অন্তর্ভাৱ "qemu/cutils.h"
#অন্তর্ভাৱ "hw/core/sysbus.h"
#অন্তর্ভাৱ "target/riscv/cpu.h"
#অন্তর্ভাৱ "hw/core/qdev-properties.h"
#অন্তর্ভাৱ "hw/riscv/riscv_hart.h"
#অন্তর্ভাৱ "qemu/error-report.h"

স্থির ধ্রুৱ Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, অঙ্ক_harts, 1),
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
                      অঙ্ক_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      অঙ্ক_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, memory,
                     TYPE_MEMORY_REGION, MemoryRegion *),
};

স্থির শূন্য riscv_harts_cpu_reset(শূন্য *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#যদি defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
স্থির শূন্য csr_call(ৱর্ণ *cmd, uint64_t cpu_অঙ্ক, পূর্ণাঙ্ক csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_অঙ্ক));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    যদি (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } অন্যথা যদি (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

স্থির bool csr_qtest_callback(CharFrontend *chr, gchar **শব্দগুলি)
{
    যদি (strcmp(শব্দগুলি[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        পূর্ণাঙ্ক rc, csr;

        rc = qemu_strtou64(শব্দগুলি[2], রিক্ত, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(শব্দগুলি[3], রিক্ত, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(শব্দগুলি[4], রিক্ত, 0, &val);
        g_assert(rc == 0);
        csr_call(শব্দগুলি[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        ঘূৰাই true;
    }

    ঘূৰাই false;
}

স্থির শূন্য riscv_cpu_register_csr_qtest_callback(শূন্য)
{
    স্থির bool first = true;
    যদি (first) {
        first = false;
        qtest_set_command_cb(csr_qtest_callback);
    }
}
#endif

স্থির bool riscv_hart_realize(RISCVHartArrayState *s, পূর্ণাঙ্ক idx,
                               ৱর্ণ *cpu_type, Error **errp)
{
    object_initialize_child(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_set_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    যদি (s->memory) {
        object_property_set_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->memory), &error_abort);
    }

    যদি (s->harts[idx].cfg.ext_smrnmi) {
        যদি (idx < s->অঙ্ক_rnmi_irqvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        যদি (idx < s->অঙ্ক_rnmi_excpvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } অন্যথা {
        যদি (s->অঙ্ক_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        যদি (s->অঙ্ক_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_base + idx;
    qemu_register_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    ঘূৰাই qdev_realize(DEVICE(&s->harts[idx]), রিক্ত, errp);
}

স্থির শূন্য riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    পূর্ণাঙ্ক n;

    s->harts = g_new0(RISCVCPU, s->অঙ্ক_harts);

#যদি defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    যদি (qtest_enabled()) {
        riscv_cpu_register_csr_qtest_callback();
    }
#endif

    বাবে (n = 0; n < s->অঙ্ক_harts; n++) {
        যদি (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            ঘূৰাই;
        }
    }
}

স্থির শূন্য riscv_harts_class_init(ObjectClass *klass, ধ্রুৱ শূন্য *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

স্থির ধ্রুৱ TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = পরিমাণ(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

স্থির শূন্য riscv_harts_register_types(শূন্য)
{
    type_register_static(&riscv_harts_info);
}

type_init(riscv_harts_register_types)
