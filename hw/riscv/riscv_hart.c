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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/module.h"
#ಅನ್ತರ್ಭಾವ "system/reset.h"
#ಅನ್ತರ್ಭಾವ "system/qtest.h"
#ಅನ್ತರ್ಭಾವ "qemu/cutils.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"

ಸ್ಥಿರ ಧ್ರುವ Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, ಅಂಕೆ_harts, 1),
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
                      ಅಂಕೆ_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      ಅಂಕೆ_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, memory,
                     TYPE_MEMORY_REGION, MemoryRegion *),
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_harts_cpu_reset(ಶೂನ್ಯ *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#ಯದಿ defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
ಸ್ಥಿರ ಶೂನ್ಯ csr_call(ವರ್ಣ *cmd, uint64_t cpu_ಅಂಕೆ, ಪೂರ್ಣಾಙ್ಕ csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_ಅಂಕೆ));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    ಯದಿ (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } ಅನ್ಯಥಾ ಯದಿ (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

ಸ್ಥಿರ bool csr_qtest_callback(CharFrontend *chr, gchar **ಪದಗಳು)
{
    ಯದಿ (strcmp(ಪದಗಳು[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        ಪೂರ್ಣಾಙ್ಕ rc, csr;

        rc = qemu_strtou64(ಪದಗಳು[2], ರಿಕ್ತ, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(ಪದಗಳು[3], ರಿಕ್ತ, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(ಪದಗಳು[4], ರಿಕ್ತ, 0, &val);
        g_assert(rc == 0);
        csr_call(ಪದಗಳು[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        ಪ್ರತ್ಯಾಗಮ true;
    }

    ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_cpu_register_csr_qtest_callback(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ bool first = true;
    ಯದಿ (first) {
        first = false;
        qtest_set_command_cb(csr_qtest_callback);
    }
}
#endif

ಸ್ಥಿರ bool riscv_hart_realize(RISCVHartArrayState *s, ಪೂರ್ಣಾಙ್ಕ idx,
                               ವರ್ಣ *cpu_type, Error **errp)
{
    object_initialize_child(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_set_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    ಯದಿ (s->memory) {
        object_property_set_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->memory), &error_abort);
    }

    ಯದಿ (s->harts[idx].cfg.ext_smrnmi) {
        ಯದಿ (idx < s->ಅಂಕೆ_rnmi_irqvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        ಯದಿ (idx < s->ಅಂಕೆ_rnmi_excpvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } ಅನ್ಯಥಾ {
        ಯದಿ (s->ಅಂಕೆ_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        ಯದಿ (s->ಅಂಕೆ_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_base + idx;
    qemu_register_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    ಪ್ರತ್ಯಾಗಮ qdev_realize(DEVICE(&s->harts[idx]), ರಿಕ್ತ, errp);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    ಪೂರ್ಣಾಙ್ಕ n;

    s->harts = g_new0(RISCVCPU, s->ಅಂಕೆ_harts);

#ಯದಿ defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    ಯದಿ (qtest_enabled()) {
        riscv_cpu_register_csr_qtest_callback();
    }
#endif

    ಪ್ರತಿ (n = 0; n < s->ಅಂಕೆ_harts; n++) {
        ಯದಿ (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_harts_class_init(ObjectClass *klass, ಧ್ರುವ ಶೂನ್ಯ *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

ಸ್ಥಿರ ಧ್ರುವ TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = ಪರಿಮಾಣ(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_harts_register_types(ಶೂನ್ಯ)
{
    type_register_static(&riscv_harts_info);
}

type_init(riscv_harts_register_types)
