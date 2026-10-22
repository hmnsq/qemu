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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/module.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/cps.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "system/reset.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aplic.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_imsic.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/msi.h"

ಸ್ಥಿರ ಶೂನ್ಯ riscv_cps_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RISCVCPSState *s = RISCV_CPS(obj);

    /*
     * Cover entire address space as there do not seem to be any
     * constraints for the base address of CPC .
     */
    ಸ್ಮರಣೆ_region_init(&s->container, obj, "mips-cps-container", UINT64_MAX);
    sysbus_init_mmio(sbd, &s->container);
}

ಸ್ಥಿರ ಶೂನ್ಯ main_cpu_reset(ಶೂನ್ಯ *opaque)
{
    CPUState *cs = opaque;

    cpu_reset(cs);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_cps_realize(DeviceState *dev, Error **errp)
{
    RISCVCPSState *s = RISCV_CPS(dev);
    RISCVCPU *cpu;
    ಪೂರ್ಣಾಂಕ i;

    /* Validate num_vp */
    ಆದರೆ (s->ಅಂಕೆ_vp == 0) {
        error_setg(errp, "num-vp must be at least 1");
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (s->ಅಂಕೆ_vp > MAX_HARTS) {
        error_setg(errp, "num-vp cannot exceed %d", MAX_HARTS);
        ಹಿಂತಿರುಗಿ;
    }

    /* Allocate CPU array */
    s->cpus = g_new0(CPUState *, s->ಅಂಕೆ_vp);

    /* Set up cpu_index and mhartid for avaiable CPUs. */
    ಪೂರ್ಣಾಂಕ harts_ಒಳ_cluster = s->ಅಂಕೆ_hart * s->ಅಂಕೆ_core;
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ದ_clusters = s->ಅಂಕೆ_vp / harts_ಒಳ_cluster;
    ಗಾಗಿ (i = 0; i < s->ಅಂಕೆ_vp; i++) {
        cpu = RISCV_CPU(object_new(s->cpu_type));

        /* All VPs are halted on reset. Leave powering up to CPC. */
        object_property_ಗಣ_bool(OBJECT(cpu), "start-powered-off", true,
                                 &error_abort);

        ಆದರೆ (!qdev_realize_and_unref(DEVICE(cpu), ಖಾಲಿ, errp)) {
            ಹಿಂತಿರುಗಿ;
        }

        /* Store CPU in array */
        s->cpus[i] = CPU(cpu);

        /* Set up mhartid */
        ಪೂರ್ಣಾಂಕ cluster_id = i / harts_ಒಳ_cluster;
        ಪೂರ್ಣಾಂಕ hart_id = (i % harts_ಒಳ_cluster) % s->ಅಂಕೆ_hart;
        ಪೂರ್ಣಾಂಕ core_id = (i % harts_ಒಳ_cluster) / s->ಅಂಕೆ_hart;
        ಪೂರ್ಣಾಂಕ mhartid = (cluster_id << MHARTID_CLUSTER_ಸರಿಸು) +
                      (core_id << MHARTID_CORE_ಸರಿಸು) +
                      (hart_id << MHARTID_HART_ಸರಿಸು);
        cpu->env.mhartid = mhartid;
        qemu_ರಿಜಿಸ್ಟರ್_reset(main_cpu_reset, s->cpus[i]);
    }

    /* Cluster Power Controller */
    object_initialize_ಮಗು(OBJECT(dev), "cpc", &s->cpc, TYPE_RISCV_CPC);
    object_property_ಗಣ_uint(OBJECT(&s->cpc), "cluster-id", 0,
                            &error_abort);
    object_property_ಗಣ_uint(OBJECT(&s->cpc), "num-vp", s->ಅಂಕೆ_vp,
                            &error_abort);
    object_property_ಗಣ_uint(OBJECT(&s->cpc), "num-hart", s->ಅಂಕೆ_hart,
                            &error_abort);
    object_property_ಗಣ_uint(OBJECT(&s->cpc), "num-core", s->ಅಂಕೆ_core,
                            &error_abort);

    /* Pass CPUs to CPC using link properties */
    ಗಾಗಿ (i = 0; i < s->ಅಂಕೆ_vp; i++) {
        ಅಕ್ಷರ *propname = g_strdup_printf("cpu[%d]", i);
        object_property_ಗಣ_link(OBJECT(&s->cpc), propname,
                                OBJECT(s->cpus[i]), &error_abort);
        g_free(propname);
    }

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpc), errp)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(&s->container, 0,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->cpc), 0));

    /* Global Configuration Registers */
    object_initialize_ಮಗು(OBJECT(dev), "gcr", &s->gcr, TYPE_RISCV_GCR);
    object_property_ಗಣ_uint(OBJECT(&s->gcr), "cluster-id", 0,
                            &error_abort);
    object_property_ಗಣ_uint(OBJECT(&s->gcr), "num-vp", s->ಅಂಕೆ_vp,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&s->gcr), "gcr-rev", 0xa00,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&s->gcr), "gcr-base", s->gcr_ತಳ,
                            &error_abort);
    object_property_ಗಣ_link(OBJECT(&s->gcr), "cpc", OBJECT(&s->cpc.mr),
                             &error_abort);
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->gcr), errp)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(&s->container, s->gcr_ತಳ,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->gcr), 0));

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ದ_clusters; i++) {
        uint64_t cm_ತಳ = GLOBAL_CM_ತಳ + (CM_ಗಾತ್ರ * i);
        uint32_t hartid_ತಳ = i << MHARTID_CLUSTER_ಸರಿಸು;
        s->aplic = riscv_aplic_create(&s->container,
                                      cm_ತಳ + AIA_PLIC_M_ಸ್ಥಾನಾಂತರ,
                                      AIA_PLIC_M_ಗಾತ್ರ,
                                      hartid_ತಳ, /* hartid_base */
                                      MAX_HARTS, /* num_harts */
                                      APLIC_ಅಂಕೆ_SOURCES,
                                      APLIC_ಅಂಕೆ_PRIO_ಬಿಟ್ಟುಗಳು,
                                      false, true, ಖಾಲಿ);
        riscv_aplic_create(&s->container,
                           cm_ತಳ + AIA_PLIC_S_ಸ್ಥಾನಾಂತರ,
                           AIA_PLIC_S_ಗಾತ್ರ,
                           hartid_ತಳ, /* hartid_base */
                           MAX_HARTS, /* num_harts */
                           APLIC_ಅಂಕೆ_SOURCES,
                           APLIC_ಅಂಕೆ_PRIO_ಬಿಟ್ಟುಗಳು,
                           false, false, s->aplic);
        /* PLIC changes msi_nonbroken to ture. We revert the change. */
        msi_nonbroken = false;
        riscv_aclint_swi_create(&s->container,
                                cm_ತಳ + AIA_CLINT_ಸ್ಥಾನಾಂತರ,
                                hartid_ತಳ, MAX_HARTS, false);
        riscv_aclint_mtimer_create(&s->container,
                                   cm_ತಳ + AIA_CLINT_ಸ್ಥಾನಾಂತರ +
                                   RISCV_ACLINT_SWI_ಗಾತ್ರ,
                                   RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ,
                                   hartid_ತಳ,
                                   MAX_HARTS,
                                   RISCV_ACLINT_DEFAULT_MTIMECMP,
                                   RISCV_ACLINT_DEFAULT_MTIME,
                                   RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Property riscv_cps_properties[] = {
    DEFINE_PROP_UINT32("num-vp", RISCVCPSState, ಅಂಕೆ_vp, 1),
    DEFINE_PROP_UINT32("num-hart", RISCVCPSState, ಅಂಕೆ_hart, 1),
    DEFINE_PROP_UINT32("num-core", RISCVCPSState, ಅಂಕೆ_core, 1),
    DEFINE_PROP_UINT64("gcr-base", RISCVCPSState, gcr_ತಳ, GCR_ತಳ_ADDR),
    DEFINE_PROP_STRING("cpu-type", RISCVCPSState, cpu_type),
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_cps_class_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = riscv_cps_realize;
    device_class_ಗಣ_props(dc, riscv_cps_properties);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo riscv_cps_info = {
    .name = TYPE_RISCV_CPS,
    .ಪೋಷಕ = TYPE_SYS_BUS_DEVICE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(RISCVCPSState),
    .instance_init = riscv_cps_init,
    .class_init = riscv_cps_class_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_cps_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&riscv_cps_info);
}

type_init(riscv_cps_ರಿಜಿಸ್ಟರ್_types)
