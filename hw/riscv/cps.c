/*
 * Coherent Processing System emulation.
 *
 * Copyright (c) 2016 Imagination Technologies
 *
 * Copyright (c) 2025 MIPS
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/module.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/cps.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/reset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aplic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_imsic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/msi.h"

ਸਥਿਰ ਖਾਲੀ riscv_cps_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RISCVCPSState *s = RISCV_CPS(obj);

    /*
     * Cover entire address space as there do not seem to be any
     * constraints for the base address of CPC .
     */
    ਸਮ੍ਰਿਤੀ_region_init(&s->container, obj, "mips-cps-container", UINT64_MAX);
    sysbus_init_mmio(sbd, &s->container);
}

ਸਥਿਰ ਖਾਲੀ main_cpu_reset(ਖਾਲੀ *opaque)
{
    CPUState *cs = opaque;

    cpu_reset(cs);
}

ਸਥਿਰ ਖਾਲੀ riscv_cps_realize(DeviceState *dev, Error **errp)
{
    RISCVCPSState *s = RISCV_CPS(dev);
    RISCVCPU *cpu;
    ਪੂਰਨ i;

    /* Validate num_vp */
    ਜੇ (s->ਅੰਕ_vp == 0) {
        error_setg(errp, "num-vp must be at least 1");
        ਵਾਪਸ;
    }
    ਜੇ (s->ਅੰਕ_vp > MAX_HARTS) {
        error_setg(errp, "num-vp cannot exceed %d", MAX_HARTS);
        ਵਾਪਸ;
    }

    /* Allocate CPU array */
    s->cpus = g_new0(CPUState *, s->ਅੰਕ_vp);

    /* Set up cpu_index and mhartid for avaiable CPUs. */
    ਪੂਰਨ harts_ਵਿੱਚ_cluster = s->ਅੰਕ_hart * s->ਅੰਕ_core;
    ਪੂਰਨ ਅੰਕ_ਦਾ_clusters = s->ਅੰਕ_vp / harts_ਵਿੱਚ_cluster;
    ਲਈ (i = 0; i < s->ਅੰਕ_vp; i++) {
        cpu = RISCV_CPU(object_new(s->cpu_type));

        /* All VPs are halted on reset. Leave powering up to CPC. */
        object_property_ਸੈੱਟ_bool(OBJECT(cpu), "start-powered-off", true,
                                 &error_abort);

        ਜੇ (!qdev_realize_and_unref(DEVICE(cpu), ਨਿਰਾ, errp)) {
            ਵਾਪਸ;
        }

        /* Store CPU in array */
        s->cpus[i] = CPU(cpu);

        /* Set up mhartid */
        ਪੂਰਨ cluster_id = i / harts_ਵਿੱਚ_cluster;
        ਪੂਰਨ hart_id = (i % harts_ਵਿੱਚ_cluster) % s->ਅੰਕ_hart;
        ਪੂਰਨ core_id = (i % harts_ਵਿੱਚ_cluster) / s->ਅੰਕ_hart;
        ਪੂਰਨ mhartid = (cluster_id << MHARTID_CLUSTER_ਖਿਸਕਾਓ) +
                      (core_id << MHARTID_CORE_ਖਿਸਕਾਓ) +
                      (hart_id << MHARTID_HART_ਖਿਸਕਾਓ);
        cpu->env.mhartid = mhartid;
        qemu_ਰਜਿਸਟਰ_reset(main_cpu_reset, s->cpus[i]);
    }

    /* Cluster Power Controller */
    object_initialize_ਬੱਚਾ(OBJECT(dev), "cpc", &s->cpc, TYPE_RISCV_CPC);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->cpc), "cluster-id", 0,
                            &error_abort);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->cpc), "num-vp", s->ਅੰਕ_vp,
                            &error_abort);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->cpc), "num-hart", s->ਅੰਕ_hart,
                            &error_abort);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->cpc), "num-core", s->ਅੰਕ_core,
                            &error_abort);

    /* Pass CPUs to CPC using link properties */
    ਲਈ (i = 0; i < s->ਅੰਕ_vp; i++) {
        ਅੱਖਰ *propname = g_strdup_printf("cpu[%d]", i);
        object_property_ਸੈੱਟ_link(OBJECT(&s->cpc), propname,
                                OBJECT(s->cpus[i]), &error_abort);
        g_free(propname);
    }

    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpc), errp)) {
        ਵਾਪਸ;
    }

    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(&s->container, 0,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->cpc), 0));

    /* Global Configuration Registers */
    object_initialize_ਬੱਚਾ(OBJECT(dev), "gcr", &s->gcr, TYPE_RISCV_GCR);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->gcr), "cluster-id", 0,
                            &error_abort);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->gcr), "num-vp", s->ਅੰਕ_vp,
                            &error_abort);
    object_property_ਸੈੱਟ_int(OBJECT(&s->gcr), "gcr-rev", 0xa00,
                            &error_abort);
    object_property_ਸੈੱਟ_int(OBJECT(&s->gcr), "gcr-base", s->gcr_ਅਧਾਰ,
                            &error_abort);
    object_property_ਸੈੱਟ_link(OBJECT(&s->gcr), "cpc", OBJECT(&s->cpc.mr),
                             &error_abort);
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->gcr), errp)) {
        ਵਾਪਸ;
    }

    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(&s->container, s->gcr_ਅਧਾਰ,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->gcr), 0));

    ਲਈ (i = 0; i < ਅੰਕ_ਦਾ_clusters; i++) {
        uint64_t cm_ਅਧਾਰ = GLOBAL_CM_ਅਧਾਰ + (CM_ਮਾਪ * i);
        uint32_t hartid_ਅਧਾਰ = i << MHARTID_CLUSTER_ਖਿਸਕਾਓ;
        s->aplic = riscv_aplic_create(&s->container,
                                      cm_ਅਧਾਰ + AIA_PLIC_M_ਵਿੱਥ,
                                      AIA_PLIC_M_ਮਾਪ,
                                      hartid_ਅਧਾਰ, /* hartid_base */
                                      MAX_HARTS, /* num_harts */
                                      APLIC_ਅੰਕ_SOURCES,
                                      APLIC_ਅੰਕ_PRIO_ਬਿੱਟ,
                                      false, true, ਨਿਰਾ);
        riscv_aplic_create(&s->container,
                           cm_ਅਧਾਰ + AIA_PLIC_S_ਵਿੱਥ,
                           AIA_PLIC_S_ਮਾਪ,
                           hartid_ਅਧਾਰ, /* hartid_base */
                           MAX_HARTS, /* num_harts */
                           APLIC_ਅੰਕ_SOURCES,
                           APLIC_ਅੰਕ_PRIO_ਬਿੱਟ,
                           false, false, s->aplic);
        /* PLIC changes msi_nonbroken to ture. We revert the change. */
        msi_nonbroken = false;
        riscv_aclint_swi_create(&s->container,
                                cm_ਅਧਾਰ + AIA_CLINT_ਵਿੱਥ,
                                hartid_ਅਧਾਰ, MAX_HARTS, false);
        riscv_aclint_mtimer_create(&s->container,
                                   cm_ਅਧਾਰ + AIA_CLINT_ਵਿੱਥ +
                                   RISCV_ACLINT_SWI_ਮਾਪ,
                                   RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ,
                                   hartid_ਅਧਾਰ,
                                   MAX_HARTS,
                                   RISCV_ACLINT_DEFAULT_MTIMECMP,
                                   RISCV_ACLINT_DEFAULT_MTIME,
                                   RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }
}

ਸਥਿਰ ਅਚਲ Property riscv_cps_properties[] = {
    DEFINE_PROP_UINT32("num-vp", RISCVCPSState, ਅੰਕ_vp, 1),
    DEFINE_PROP_UINT32("num-hart", RISCVCPSState, ਅੰਕ_hart, 1),
    DEFINE_PROP_UINT32("num-core", RISCVCPSState, ਅੰਕ_core, 1),
    DEFINE_PROP_UINT64("gcr-base", RISCVCPSState, gcr_ਅਧਾਰ, GCR_ਅਧਾਰ_ADDR),
    DEFINE_PROP_STRING("cpu-type", RISCVCPSState, cpu_type),
};

ਸਥਿਰ ਖਾਲੀ riscv_cps_class_init(ObjectClass *klass, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = riscv_cps_realize;
    device_class_ਸੈੱਟ_props(dc, riscv_cps_properties);
}

ਸਥਿਰ ਅਚਲ TypeInfo riscv_cps_info = {
    .name = TYPE_RISCV_CPS,
    .ਮਾਪਾ = TYPE_SYS_BUS_DEVICE,
    .instance_ਮਾਪ = ਆਕਾਰ(RISCVCPSState),
    .instance_init = riscv_cps_init,
    .class_init = riscv_cps_class_init,
};

ਸਥਿਰ ਖਾਲੀ riscv_cps_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&riscv_cps_info);
}

type_init(riscv_cps_ਰਜਿਸਟਰ_types)
