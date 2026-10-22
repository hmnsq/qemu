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

#અન્તર્ભાવ "qemu/osdep.h"
#અન્તર્ભાવ "qapi/error.h"
#અન્તર્ભાવ "qemu/module.h"
#અન્તર્ભાવ "hw/riscv/cps.h"
#અન્તર્ભાવ "hw/core/qdev-properties.h"
#અન્તર્ભાવ "system/reset.h"
#અન્તર્ભાવ "hw/intc/riscv_aclint.h"
#અન્તર્ભાવ "hw/intc/riscv_aplic.h"
#અન્તર્ભાવ "hw/intc/riscv_imsic.h"
#અન્તર્ભાવ "hw/pci/msi.h"

સ્થિર શૂન્ય riscv_cps_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RISCVCPSState *s = RISCV_CPS(obj);

    /*
     * Cover entire address space as there do not seem to be any
     * constraints for the base address of CPC .
     */
    memory_region_init(&s->container, obj, "mips-cps-container", UINT64_MAX);
    sysbus_init_mmio(sbd, &s->container);
}

સ્થિર શૂન્ય main_cpu_reset(શૂન્ય *opaque)
{
    CPUState *cs = opaque;

    cpu_reset(cs);
}

સ્થિર શૂન્ય riscv_cps_realize(DeviceState *dev, Error **errp)
{
    RISCVCPSState *s = RISCV_CPS(dev);
    RISCVCPU *cpu;
    પૂર્ણાંક i;

    /* Validate num_vp */
    જો (s->અંક_vp == 0) {
        error_setg(errp, "num-vp must be at least 1");
        પાછા;
    }
    જો (s->અંક_vp > MAX_HARTS) {
        error_setg(errp, "num-vp cannot exceed %d", MAX_HARTS);
        પાછા;
    }

    /* Allocate CPU array */
    s->cpus = g_new0(CPUState *, s->અંક_vp);

    /* Set up cpu_index and mhartid for avaiable CPUs. */
    પૂર્ણાંક harts_in_cluster = s->અંક_hart * s->અંક_core;
    પૂર્ણાંક અંક_of_clusters = s->અંક_vp / harts_in_cluster;
    માટે (i = 0; i < s->અંક_vp; i++) {
        cpu = RISCV_CPU(object_new(s->cpu_type));

        /* All VPs are halted on reset. Leave powering up to CPC. */
        object_property_set_bool(OBJECT(cpu), "start-powered-off", true,
                                 &error_abort);

        જો (!qdev_realize_and_unref(DEVICE(cpu), ખાલી, errp)) {
            પાછા;
        }

        /* Store CPU in array */
        s->cpus[i] = CPU(cpu);

        /* Set up mhartid */
        પૂર્ણાંક cluster_id = i / harts_in_cluster;
        પૂર્ણાંક hart_id = (i % harts_in_cluster) % s->અંક_hart;
        પૂર્ણાંક core_id = (i % harts_in_cluster) / s->અંક_hart;
        પૂર્ણાંક mhartid = (cluster_id << MHARTID_CLUSTER_SHIFT) +
                      (core_id << MHARTID_CORE_SHIFT) +
                      (hart_id << MHARTID_HART_SHIFT);
        cpu->env.mhartid = mhartid;
        qemu_register_reset(main_cpu_reset, s->cpus[i]);
    }

    /* Cluster Power Controller */
    object_initialize_child(OBJECT(dev), "cpc", &s->cpc, TYPE_RISCV_CPC);
    object_property_set_uint(OBJECT(&s->cpc), "cluster-id", 0,
                            &error_abort);
    object_property_set_uint(OBJECT(&s->cpc), "num-vp", s->અંક_vp,
                            &error_abort);
    object_property_set_uint(OBJECT(&s->cpc), "num-hart", s->અંક_hart,
                            &error_abort);
    object_property_set_uint(OBJECT(&s->cpc), "num-core", s->અંક_core,
                            &error_abort);

    /* Pass CPUs to CPC using link properties */
    માટે (i = 0; i < s->અંક_vp; i++) {
        અક્ષર *propname = g_strdup_printf("cpu[%d]", i);
        object_property_set_link(OBJECT(&s->cpc), propname,
                                OBJECT(s->cpus[i]), &error_abort);
        g_free(propname);
    }

    જો (!sysbus_realize(SYS_BUS_DEVICE(&s->cpc), errp)) {
        પાછા;
    }

    memory_region_add_subregion(&s->container, 0,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->cpc), 0));

    /* Global Configuration Registers */
    object_initialize_child(OBJECT(dev), "gcr", &s->gcr, TYPE_RISCV_GCR);
    object_property_set_uint(OBJECT(&s->gcr), "cluster-id", 0,
                            &error_abort);
    object_property_set_uint(OBJECT(&s->gcr), "num-vp", s->અંક_vp,
                            &error_abort);
    object_property_set_int(OBJECT(&s->gcr), "gcr-rev", 0xa00,
                            &error_abort);
    object_property_set_int(OBJECT(&s->gcr), "gcr-base", s->gcr_base,
                            &error_abort);
    object_property_set_link(OBJECT(&s->gcr), "cpc", OBJECT(&s->cpc.mr),
                             &error_abort);
    જો (!sysbus_realize(SYS_BUS_DEVICE(&s->gcr), errp)) {
        પાછા;
    }

    memory_region_add_subregion(&s->container, s->gcr_base,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->gcr), 0));

    માટે (i = 0; i < અંક_of_clusters; i++) {
        uint64_t cm_base = GLOBAL_CM_BASE + (CM_SIZE * i);
        uint32_t hartid_base = i << MHARTID_CLUSTER_SHIFT;
        s->aplic = riscv_aplic_create(&s->container,
                                      cm_base + AIA_PLIC_M_OFFSET,
                                      AIA_PLIC_M_SIZE,
                                      hartid_base, /* hartid_base */
                                      MAX_HARTS, /* num_harts */
                                      APLIC_અંક_SOURCES,
                                      APLIC_અંક_PRIO_BITS,
                                      false, true, ખાલી);
        riscv_aplic_create(&s->container,
                           cm_base + AIA_PLIC_S_OFFSET,
                           AIA_PLIC_S_SIZE,
                           hartid_base, /* hartid_base */
                           MAX_HARTS, /* num_harts */
                           APLIC_અંક_SOURCES,
                           APLIC_અંક_PRIO_BITS,
                           false, false, s->aplic);
        /* PLIC changes msi_nonbroken to ture. We revert the change. */
        msi_nonbroken = false;
        riscv_aclint_swi_create(&s->container,
                                cm_base + AIA_CLINT_OFFSET,
                                hartid_base, MAX_HARTS, false);
        riscv_aclint_mtimer_create(&s->container,
                                   cm_base + AIA_CLINT_OFFSET +
                                   RISCV_ACLINT_SWI_SIZE,
                                   RISCV_ACLINT_DEFAULT_MTIMER_SIZE,
                                   hartid_base,
                                   MAX_HARTS,
                                   RISCV_ACLINT_DEFAULT_MTIMECMP,
                                   RISCV_ACLINT_DEFAULT_MTIME,
                                   RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }
}

સ્થિર અચલ Property riscv_cps_properties[] = {
    DEFINE_PROP_UINT32("num-vp", RISCVCPSState, અંક_vp, 1),
    DEFINE_PROP_UINT32("num-hart", RISCVCPSState, અંક_hart, 1),
    DEFINE_PROP_UINT32("num-core", RISCVCPSState, અંક_core, 1),
    DEFINE_PROP_UINT64("gcr-base", RISCVCPSState, gcr_base, GCR_BASE_ADDR),
    DEFINE_PROP_STRING("cpu-type", RISCVCPSState, cpu_type),
};

સ્થિર શૂન્ય riscv_cps_class_init(ObjectClass *klass, અચલ શૂન્ય *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = riscv_cps_realize;
    device_class_set_props(dc, riscv_cps_properties);
}

સ્થિર અચલ TypeInfo riscv_cps_info = {
    .name = TYPE_RISCV_CPS,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = કદ(RISCVCPSState),
    .instance_init = riscv_cps_init,
    .class_init = riscv_cps_class_init,
};

સ્થિર શૂન્ય riscv_cps_register_types(શૂન્ય)
{
    type_register_static(&riscv_cps_info);
}

type_init(riscv_cps_register_types)
