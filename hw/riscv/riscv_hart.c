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

#અન્તર્ભાવ "qemu/osdep.h"
#અન્તર્ભાવ "qapi/error.h"
#અન્તર્ભાવ "qemu/module.h"
#અન્તર્ભાવ "system/reset.h"
#અન્તર્ભાવ "system/qtest.h"
#અન્તર્ભાવ "qemu/cutils.h"
#અન્તર્ભાવ "hw/core/sysbus.h"
#અન્તર્ભાવ "target/riscv/cpu.h"
#અન્તર્ભાવ "hw/core/qdev-properties.h"
#અન્તર્ભાવ "hw/riscv/riscv_hart.h"
#અન્તર્ભાવ "qemu/error-report.h"

સ્થિર અચલ Property riscv_harts_props[] = {
    DEFINE_PROP_UINT32("num-harts", RISCVHartArrayState, અંક_harts, 1),
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
                      અંક_rnmi_irqvec, rnmi_irqvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_ARRAY("rnmi-exception-vector", RISCVHartArrayState,
                      અંક_rnmi_excpvec, rnmi_excpvec, qdev_prop_uint64,
                      uint64_t),
    DEFINE_PROP_LINK("memory", RISCVHartArrayState, memory,
                     TYPE_MEMORY_REGION, MemoryRegion *),
};

સ્થિર શૂન્ય riscv_harts_cpu_reset(શૂન્ય *opaque)
{
    RISCVCPU *cpu = opaque;
    cpu_reset(CPU(cpu));
}

#જો defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
સ્થિર શૂન્ય csr_call(અક્ષર *cmd, uint64_t cpu_અંક, પૂર્ણાંક csrno, uint64_t *val)
{
    RISCVCPU *cpu = RISCV_CPU(cpu_by_arch_id(cpu_અંક));
    CPURISCVState *env = &cpu->env;

    RISCVException ret = RISCV_EXCP_NONE;
    જો (strcmp(cmd, "get_csr") == 0) {
        ret = riscv_csr_read_i64(env, csrno, val);
    } નહીંતર જો (strcmp(cmd, "set_csr") == 0) {
        ret = riscv_csr_write_i64(env, csrno, *val);
    }

    g_assert(ret == RISCV_EXCP_NONE);
}

સ્થિર bool csr_qtest_callback(CharFrontend *chr, gchar **શબ્દો)
{
    જો (strcmp(શબ્દો[0], "csr") == 0) {

        uint64_t cpu;
        uint64_t val;
        પૂર્ણાંક rc, csr;

        rc = qemu_strtou64(શબ્દો[2], ખાલી, 0, &cpu);
        g_assert(rc == 0);
        rc = qemu_strtoi(શબ્દો[3], ખાલી, 0, &csr);
        g_assert(rc == 0);
        rc = qemu_strtou64(શબ્દો[4], ખાલી, 0, &val);
        g_assert(rc == 0);
        csr_call(શબ્દો[1], cpu, csr, &val);

        qtest_sendf(chr, "OK 0 %"PRIx64"\n", val);

        પાછા true;
    }

    પાછા false;
}

સ્થિર શૂન્ય riscv_cpu_register_csr_qtest_callback(શૂન્ય)
{
    સ્થિર bool first = true;
    જો (first) {
        first = false;
        qtest_set_command_cb(csr_qtest_callback);
    }
}
#endif

સ્થિર bool riscv_hart_realize(RISCVHartArrayState *s, પૂર્ણાંક idx,
                               અક્ષર *cpu_type, Error **errp)
{
    object_initialize_child(OBJECT(s), "harts[*]", &s->harts[idx], cpu_type);
    qdev_prop_set_uint64(DEVICE(&s->harts[idx]), "resetvec", s->resetvec);

    /* Use private memory instead of system_memory if provided */
    જો (s->memory) {
        object_property_set_link(OBJECT(&s->harts[idx]), "memory",
                                 OBJECT(s->memory), &error_abort);
    }

    જો (s->harts[idx].cfg.ext_smrnmi) {
        જો (idx < s->અંક_rnmi_irqvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-interrupt-vector", s->rnmi_irqvec[idx]);
        }

        જો (idx < s->અંક_rnmi_excpvec) {
            qdev_prop_set_uint64(DEVICE(&s->harts[idx]),
                                 "rnmi-exception-vector", s->rnmi_excpvec[idx]);
        }
    } નહીંતર {
        જો (s->અંક_rnmi_irqvec > 0) {
            warn_report_once("rnmi-interrupt-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }

        જો (s->અંક_rnmi_excpvec > 0) {
            warn_report_once("rnmi-exception-vector property is ignored "
                             "because Smrnmi extension is not enabled.");
        }
    }

    s->harts[idx].env.mhartid = s->hartid_base + idx;
    qemu_register_reset(riscv_harts_cpu_reset, &s->harts[idx]);
    પાછા qdev_realize(DEVICE(&s->harts[idx]), ખાલી, errp);
}

સ્થિર શૂન્ય riscv_harts_realize(DeviceState *dev, Error **errp)
{
    RISCVHartArrayState *s = RISCV_HART_ARRAY(dev);
    પૂર્ણાંક n;

    s->harts = g_new0(RISCVCPU, s->અંક_harts);

#જો defined(CONFIG_TCG) && !defined(CONFIG_USER_ONLY)
    જો (qtest_enabled()) {
        riscv_cpu_register_csr_qtest_callback();
    }
#endif

    માટે (n = 0; n < s->અંક_harts; n++) {
        જો (!riscv_hart_realize(s, n, s->cpu_type, errp)) {
            પાછા;
        }
    }
}

સ્થિર શૂન્ય riscv_harts_class_init(ObjectClass *klass, અચલ શૂન્ય *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, riscv_harts_props);
    dc->realize = riscv_harts_realize;
}

સ્થિર અચલ TypeInfo riscv_harts_info = {
    .name          = TYPE_RISCV_HART_ARRAY,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = કદ(RISCVHartArrayState),
    .class_init    = riscv_harts_class_init,
};

સ્થિર શૂન્ય riscv_harts_register_types(શૂન્ય)
{
    type_register_static(&riscv_harts_info);
}

type_init(riscv_harts_register_types)
