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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qemu/module.h"
#అన్తర్భావ "hw/riscv/cps.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "system/reset.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/riscv_aplic.h"
#అన్తర్భావ "hw/intc/riscv_imsic.h"
#అన్తర్భావ "hw/pci/msi.h"

స్థిర శూన్య riscv_cps_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RISCVCPSState *s = RISCV_CPS(obj);

    /*
     * Cover entire address space as there do not seem to be any
     * constraints for the base address of CPC .
     */
    జ్ఞాపకం_region_init(&s->container, obj, "mips-cps-container", UINT64_MAX);
    sysbus_init_mmio(sbd, &s->container);
}

స్థిర శూన్య main_cpu_reset(శూన్య *opaque)
{
    CPUState *cs = opaque;

    cpu_reset(cs);
}

స్థిర శూన్య riscv_cps_realize(DeviceState *dev, Error **errp)
{
    RISCVCPSState *s = RISCV_CPS(dev);
    RISCVCPU *cpu;
    పూర్ణాంకం i;

    /* Validate num_vp */
    ఒకవేళ (s->అంకె_vp == 0) {
        error_setg(errp, "num-vp must be at least 1");
        తిరిగి;
    }
    ఒకవేళ (s->అంకె_vp > MAX_HARTS) {
        error_setg(errp, "num-vp cannot exceed %d", MAX_HARTS);
        తిరిగి;
    }

    /* Allocate CPU array */
    s->cpus = g_new0(CPUState *, s->అంకె_vp);

    /* Set up cpu_index and mhartid for avaiable CPUs. */
    పూర్ణాంకం harts_లోపల_cluster = s->అంకె_hart * s->అంకె_core;
    పూర్ణాంకం అంకె_యొక్క_clusters = s->అంకె_vp / harts_లోపల_cluster;
    కొరకు (i = 0; i < s->అంకె_vp; i++) {
        cpu = RISCV_CPU(object_new(s->cpu_type));

        /* All VPs are halted on reset. Leave powering up to CPC. */
        object_property_సమితి_bool(OBJECT(cpu), "start-powered-off", true,
                                 &error_abort);

        ఒకవేళ (!qdev_realize_and_unref(DEVICE(cpu), ఖాళీ, errp)) {
            తిరిగి;
        }

        /* Store CPU in array */
        s->cpus[i] = CPU(cpu);

        /* Set up mhartid */
        పూర్ణాంకం cluster_id = i / harts_లోపల_cluster;
        పూర్ణాంకం hart_id = (i % harts_లోపల_cluster) % s->అంకె_hart;
        పూర్ణాంకం core_id = (i % harts_లోపల_cluster) / s->అంకె_hart;
        పూర్ణాంకం mhartid = (cluster_id << MHARTID_CLUSTER_జరుపు) +
                      (core_id << MHARTID_CORE_జరుపు) +
                      (hart_id << MHARTID_HART_జరుపు);
        cpu->env.mhartid = mhartid;
        qemu_రిజిస్టరు_reset(main_cpu_reset, s->cpus[i]);
    }

    /* Cluster Power Controller */
    object_initialize_బిడ్డ(OBJECT(dev), "cpc", &s->cpc, TYPE_RISCV_CPC);
    object_property_సమితి_uint(OBJECT(&s->cpc), "cluster-id", 0,
                            &error_abort);
    object_property_సమితి_uint(OBJECT(&s->cpc), "num-vp", s->అంకె_vp,
                            &error_abort);
    object_property_సమితి_uint(OBJECT(&s->cpc), "num-hart", s->అంకె_hart,
                            &error_abort);
    object_property_సమితి_uint(OBJECT(&s->cpc), "num-core", s->అంకె_core,
                            &error_abort);

    /* Pass CPUs to CPC using link properties */
    కొరకు (i = 0; i < s->అంకె_vp; i++) {
        అక్షరం *propname = g_strdup_printf("cpu[%d]", i);
        object_property_సమితి_link(OBJECT(&s->cpc), propname,
                                OBJECT(s->cpus[i]), &error_abort);
        g_free(propname);
    }

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpc), errp)) {
        తిరిగి;
    }

    జ్ఞాపకం_region_కూడు_subregion(&s->container, 0,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->cpc), 0));

    /* Global Configuration Registers */
    object_initialize_బిడ్డ(OBJECT(dev), "gcr", &s->gcr, TYPE_RISCV_GCR);
    object_property_సమితి_uint(OBJECT(&s->gcr), "cluster-id", 0,
                            &error_abort);
    object_property_సమితి_uint(OBJECT(&s->gcr), "num-vp", s->అంకె_vp,
                            &error_abort);
    object_property_సమితి_int(OBJECT(&s->gcr), "gcr-rev", 0xa00,
                            &error_abort);
    object_property_సమితి_int(OBJECT(&s->gcr), "gcr-base", s->gcr_పునాది,
                            &error_abort);
    object_property_సమితి_link(OBJECT(&s->gcr), "cpc", OBJECT(&s->cpc.mr),
                             &error_abort);
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->gcr), errp)) {
        తిరిగి;
    }

    జ్ఞాపకం_region_కూడు_subregion(&s->container, s->gcr_పునాది,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->gcr), 0));

    కొరకు (i = 0; i < అంకె_యొక్క_clusters; i++) {
        uint64_t cm_పునాది = GLOBAL_CM_పునాది + (CM_కొలత * i);
        uint32_t hartid_పునాది = i << MHARTID_CLUSTER_జరుపు;
        s->aplic = riscv_aplic_create(&s->container,
                                      cm_పునాది + AIA_PLIC_M_స్థానభ్రంశం,
                                      AIA_PLIC_M_కొలత,
                                      hartid_పునాది, /* hartid_base */
                                      MAX_HARTS, /* num_harts */
                                      APLIC_అంకె_SOURCES,
                                      APLIC_అంకె_PRIO_బిట్లు,
                                      false, true, ఖాళీ);
        riscv_aplic_create(&s->container,
                           cm_పునాది + AIA_PLIC_S_స్థానభ్రంశం,
                           AIA_PLIC_S_కొలత,
                           hartid_పునాది, /* hartid_base */
                           MAX_HARTS, /* num_harts */
                           APLIC_అంకె_SOURCES,
                           APLIC_అంకె_PRIO_బిట్లు,
                           false, false, s->aplic);
        /* PLIC changes msi_nonbroken to ture. We revert the change. */
        msi_nonbroken = false;
        riscv_aclint_swi_create(&s->container,
                                cm_పునాది + AIA_CLINT_స్థానభ్రంశం,
                                hartid_పునాది, MAX_HARTS, false);
        riscv_aclint_mtimer_create(&s->container,
                                   cm_పునాది + AIA_CLINT_స్థానభ్రంశం +
                                   RISCV_ACLINT_SWI_కొలత,
                                   RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                                   hartid_పునాది,
                                   MAX_HARTS,
                                   RISCV_ACLINT_DEFAULT_MTIMECMP,
                                   RISCV_ACLINT_DEFAULT_MTIME,
                                   RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }
}

స్థిర స్థిరరాశి Property riscv_cps_properties[] = {
    DEFINE_PROP_UINT32("num-vp", RISCVCPSState, అంకె_vp, 1),
    DEFINE_PROP_UINT32("num-hart", RISCVCPSState, అంకె_hart, 1),
    DEFINE_PROP_UINT32("num-core", RISCVCPSState, అంకె_core, 1),
    DEFINE_PROP_UINT64("gcr-base", RISCVCPSState, gcr_పునాది, GCR_పునాది_ADDR),
    DEFINE_PROP_STRING("cpu-type", RISCVCPSState, cpu_type),
};

స్థిర శూన్య riscv_cps_class_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = riscv_cps_realize;
    device_class_సమితి_props(dc, riscv_cps_properties);
}

స్థిర స్థిరరాశి TypeInfo riscv_cps_info = {
    .name = TYPE_RISCV_CPS,
    .మాతృ = TYPE_SYS_BUS_DEVICE,
    .instance_కొలత = పరిమాణం(RISCVCPSState),
    .instance_init = riscv_cps_init,
    .class_init = riscv_cps_class_init,
};

స్థిర శూన్య riscv_cps_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&riscv_cps_info);
}

type_init(riscv_cps_రిజిస్టరు_types)
