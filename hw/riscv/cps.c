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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/module.h"
#अन्तर्भाव "hw/riscv/cps.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "system/reset.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "hw/intc/riscv_aplic.h"
#अन्तर्भाव "hw/intc/riscv_imsic.h"
#अन्तर्भाव "hw/pci/msi.h"

स्थिर शून्य riscv_cps_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RISCVCPSState *s = RISCV_CPS(obj);

    /*
     * Cover entire address space as there do not seem to be any
     * constraints for the base address of CPC .
     */
    स्मृतिः_region_init(&s->container, obj, "mips-cps-container", UINT64_MAX);
    sysbus_init_mmio(sbd, &s->container);
}

स्थिर शून्य main_cpu_reset(शून्य *opaque)
{
    CPUState *cs = opaque;

    cpu_reset(cs);
}

स्थिर शून्य riscv_cps_realize(DeviceState *dev, Error **errp)
{
    RISCVCPSState *s = RISCV_CPS(dev);
    RISCVCPU *cpu;
    पूर्णाङ्क i;

    /* Validate num_vp */
    यदि (s->अङ्कः_vp == 0) {
        error_setg(errp, "num-vp must be at least 1");
        प्रत्यागम;
    }
    यदि (s->अङ्कः_vp > MAX_HARTS) {
        error_setg(errp, "num-vp cannot exceed %d", MAX_HARTS);
        प्रत्यागम;
    }

    /* Allocate CPU array */
    s->cpus = g_new0(CPUState *, s->अङ्कः_vp);

    /* Set up cpu_index and mhartid for avaiable CPUs. */
    पूर्णाङ्क harts_अन्तर्_cluster = s->अङ्कः_hart * s->अङ्कः_core;
    पूर्णाङ्क अङ्कः_सम्बन्धि_clusters = s->अङ्कः_vp / harts_अन्तर्_cluster;
    प्रति (i = 0; i < s->अङ्कः_vp; i++) {
        cpu = RISCV_CPU(object_new(s->cpu_type));

        /* All VPs are halted on reset. Leave powering up to CPC. */
        object_property_समुच्चयः_bool(OBJECT(cpu), "start-powered-off", true,
                                 &error_abort);

        यदि (!qdev_realize_and_unref(DEVICE(cpu), रिक्त, errp)) {
            प्रत्यागम;
        }

        /* Store CPU in array */
        s->cpus[i] = CPU(cpu);

        /* Set up mhartid */
        पूर्णाङ्क cluster_id = i / harts_अन्तर्_cluster;
        पूर्णाङ्क hart_id = (i % harts_अन्तर्_cluster) % s->अङ्कः_hart;
        पूर्णाङ्क core_id = (i % harts_अन्तर्_cluster) / s->अङ्कः_hart;
        पूर्णाङ्क mhartid = (cluster_id << MHARTID_CLUSTER_सरणम्) +
                      (core_id << MHARTID_CORE_सरणम्) +
                      (hart_id << MHARTID_HART_सरणम्);
        cpu->env.mhartid = mhartid;
        qemu_पञ्जिकाकोशः_reset(main_cpu_reset, s->cpus[i]);
    }

    /* Cluster Power Controller */
    object_initialize_child(OBJECT(dev), "cpc", &s->cpc, TYPE_RISCV_CPC);
    object_property_समुच्चयः_uint(OBJECT(&s->cpc), "cluster-id", 0,
                            &error_abort);
    object_property_समुच्चयः_uint(OBJECT(&s->cpc), "num-vp", s->अङ्कः_vp,
                            &error_abort);
    object_property_समुच्चयः_uint(OBJECT(&s->cpc), "num-hart", s->अङ्कः_hart,
                            &error_abort);
    object_property_समुच्चयः_uint(OBJECT(&s->cpc), "num-core", s->अङ्कः_core,
                            &error_abort);

    /* Pass CPUs to CPC using link properties */
    प्रति (i = 0; i < s->अङ्कः_vp; i++) {
        वर्ण *propname = g_strdup_printf("cpu[%d]", i);
        object_property_समुच्चयः_link(OBJECT(&s->cpc), propname,
                                OBJECT(s->cpus[i]), &error_abort);
        g_free(propname);
    }

    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->cpc), errp)) {
        प्रत्यागम;
    }

    स्मृतिः_region_सङ्कलय_subregion(&s->container, 0,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->cpc), 0));

    /* Global Configuration Registers */
    object_initialize_child(OBJECT(dev), "gcr", &s->gcr, TYPE_RISCV_GCR);
    object_property_समुच्चयः_uint(OBJECT(&s->gcr), "cluster-id", 0,
                            &error_abort);
    object_property_समुच्चयः_uint(OBJECT(&s->gcr), "num-vp", s->अङ्कः_vp,
                            &error_abort);
    object_property_समुच्चयः_int(OBJECT(&s->gcr), "gcr-rev", 0xa00,
                            &error_abort);
    object_property_समुच्चयः_int(OBJECT(&s->gcr), "gcr-base", s->gcr_आधारः,
                            &error_abort);
    object_property_समुच्चयः_link(OBJECT(&s->gcr), "cpc", OBJECT(&s->cpc.mr),
                             &error_abort);
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->gcr), errp)) {
        प्रत्यागम;
    }

    स्मृतिः_region_सङ्कलय_subregion(&s->container, s->gcr_आधारः,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->gcr), 0));

    प्रति (i = 0; i < अङ्कः_सम्बन्धि_clusters; i++) {
        uint64_t cm_आधारः = GLOBAL_CM_आधारः + (CM_आकार * i);
        uint32_t hartid_आधारः = i << MHARTID_CLUSTER_सरणम्;
        s->aplic = riscv_aplic_create(&s->container,
                                      cm_आधारः + AIA_PLIC_M_विस्थापनम्,
                                      AIA_PLIC_M_आकार,
                                      hartid_आधारः, /* hartid_base */
                                      MAX_HARTS, /* num_harts */
                                      APLIC_अङ्कः_SOURCES,
                                      APLIC_अङ्कः_PRIO_द्वयङ्काः,
                                      false, true, रिक्त);
        riscv_aplic_create(&s->container,
                           cm_आधारः + AIA_PLIC_S_विस्थापनम्,
                           AIA_PLIC_S_आकार,
                           hartid_आधारः, /* hartid_base */
                           MAX_HARTS, /* num_harts */
                           APLIC_अङ्कः_SOURCES,
                           APLIC_अङ्कः_PRIO_द्वयङ्काः,
                           false, false, s->aplic);
        /* PLIC changes msi_nonbroken to ture. We revert the change. */
        msi_nonbroken = false;
        riscv_aclint_swi_create(&s->container,
                                cm_आधारः + AIA_CLINT_विस्थापनम्,
                                hartid_आधारः, MAX_HARTS, false);
        riscv_aclint_mtimer_create(&s->container,
                                   cm_आधारः + AIA_CLINT_विस्थापनम् +
                                   RISCV_ACLINT_SWI_आकार,
                                   RISCV_ACLINT_DEFAULT_MTIMER_आकार,
                                   hartid_आधारः,
                                   MAX_HARTS,
                                   RISCV_ACLINT_DEFAULT_MTIMECMP,
                                   RISCV_ACLINT_DEFAULT_MTIME,
                                   RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }
}

स्थिर ध्रुव Property riscv_cps_properties[] = {
    DEFINE_PROP_UINT32("num-vp", RISCVCPSState, अङ्कः_vp, 1),
    DEFINE_PROP_UINT32("num-hart", RISCVCPSState, अङ्कः_hart, 1),
    DEFINE_PROP_UINT32("num-core", RISCVCPSState, अङ्कः_core, 1),
    DEFINE_PROP_UINT64("gcr-base", RISCVCPSState, gcr_आधारः, GCR_आधारः_ADDR),
    DEFINE_PROP_STRING("cpu-type", RISCVCPSState, cpu_type),
};

स्थिर शून्य riscv_cps_class_init(ObjectClass *klass, ध्रुव शून्य *दत्त)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = riscv_cps_realize;
    device_class_समुच्चयः_props(dc, riscv_cps_properties);
}

स्थिर ध्रुव TypeInfo riscv_cps_info = {
    .name = TYPE_RISCV_CPS,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_आकार = परिमाण(RISCVCPSState),
    .instance_init = riscv_cps_init,
    .class_init = riscv_cps_class_init,
};

स्थिर शून्य riscv_cps_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&riscv_cps_info);
}

type_init(riscv_cps_पञ्जिकाकोशः_types)
