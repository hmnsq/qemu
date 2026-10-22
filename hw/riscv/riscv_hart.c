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

#அந்தர்பாவ "qemu/osdep.h"
#அந்தர்பாவ "qapi/error.h"
#அந்தர்பாவ "qemu/module.h"
#அந்தர்பாவ "system/reset.h"
#அந்தர்பாவ "system/qtest.h"
#அந்தர்பாவ "qemu/cutils.h"
#அந்தர்பாவ "hw/core/sysbus.h"
#அந்தர்பாவ "target/riscv/cpu.h"
#அந்தர்பாவ "hw/core/qdev-properties.h"
#அந்தர்பாவ "hw/riscv/riscv_hart.h"
#அந்தர்பாவ "qemu/error-report.h"

நிலையான மாறிலி Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, இலக்கம்_harts, 1),
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
                      இலக்கம்_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      இலக்கம்_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, memory,
                     TYPE_MEMORY_REGION, MemoryRegion *),
};

நிலையான வெறுமை riscv_harts_cpu_reset(வெறுமை *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#எனில் defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
நிலையான வெறுமை csr_call(எழுத்து *cmd, uint64_t cpu_இலக்கம், முழு csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_இலக்கம்));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    எனில் (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } இல்லையெனில் எனில் (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

நிலையான bool csr_qtest_callback(CharFrontend *chr, gchar **சொற்கள்)
{
    எனில் (strcmp(சொற்கள்[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        முழு rc, csr;

        rc = qemu_strtou64(சொற்கள்[2], வெற்று, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(சொற்கள்[3], வெற்று, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(சொற்கள்[4], வெற்று, 0, &val);
        g_assert(rc == 0);
        csr_call(சொற்கள்[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        திரும்பு true;
    }

    திரும்பு false;
}

நிலையான வெறுமை riscv_cpu_register_csr_qtest_callback(வெறுமை)
{
    நிலையான bool first = true;
    எனில் (first) {
        first = false;
        qtest_set_command_cb(csr_qtest_callback);
    }
}
#endif

நிலையான bool riscv_hart_realize(RISCVHartArrayState *s, முழு idx,
                               எழுத்து *cpu_type, Error **errp)
{
    object_initialize_child(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_set_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    எனில் (s->memory) {
        object_property_set_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->memory), &error_abort);
    }

    எனில் (s->harts[idx].cfg.ext_smrnmi) {
        எனில் (idx < s->இலக்கம்_rnmi_irqvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        எனில் (idx < s->இலக்கம்_rnmi_excpvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } இல்லையெனில் {
        எனில் (s->இலக்கம்_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        எனில் (s->இலக்கம்_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_base + idx;
    qemu_register_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    திரும்பு qdev_realize(DEVICE(&s->harts[idx]), வெற்று, errp);
}

நிலையான வெறுமை riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    முழு n;

    s->harts = g_new0(RISCVCPU, s->இலக்கம்_harts);

#எனில் defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    எனில் (qtest_enabled()) {
        riscv_cpu_register_csr_qtest_callback();
    }
#endif

    க்கு (n = 0; n < s->இலக்கம்_harts; n++) {
        எனில் (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            திரும்பு;
        }
    }
}

நிலையான வெறுமை riscv_harts_class_init(ObjectClass *klass, மாறிலி வெறுமை *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

நிலையான மாறிலி TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = அளவு(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

நிலையான வெறுமை riscv_harts_register_types(வெறுமை)
{
    type_register_static(&riscv_harts_info);
}

type_init(riscv_harts_register_types)
