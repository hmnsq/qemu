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

#ضمن "qemu/osdep.h"
#ضمن "qapi/error.h"
#ضمن "qemu/module.h"
#ضمن "hw/riscv/cps.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "system/reset.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/riscv_aplic.h"
#ضمن "hw/intc/riscv_imsic.h"
#ضمن "hw/pci/msi.h"

ساكن فراغ riscv_cps_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RISCVCPSState *s = RISCV_CPS(obj);

    /*
     * Cover entire address space as there do not seem to be any
     * constraints for the base address of CPC .
     */
    حافظہ_region_init(&s->container, obj, "mips-cps-container", UINT64_MAX);
    sysbus_init_mmio(sbd, &s->container);
}

ساكن فراغ main_cpu_reset(فراغ *opaque)
{
    CPUState *cs = opaque;

    cpu_reset(cs);
}

ساكن فراغ riscv_cps_realize(DeviceState *dev, Error **errp)
{
    RISCVCPSState *s = RISCV_CPS(dev);
    RISCVCPU *cpu;
    صحيح i;

    /* Validate num_vp */
    إذا (s->عدد_vp == 0) {
        error_setg(errp, "num-vp must be at least 1");
        أرجع;
    }
    إذا (s->عدد_vp > MAX_HARTS) {
        error_setg(errp, "num-vp cannot exceed %d", MAX_HARTS);
        أرجع;
    }

    /* Allocate CPU array */
    s->cpus = g_new0(CPUState *, s->عدد_vp);

    /* Set up cpu_index and mhartid for avaiable CPUs. */
    صحيح harts_وچ_cluster = s->عدد_hart * s->عدد_core;
    صحيح عدد_دا_clusters = s->عدد_vp / harts_وچ_cluster;
    لكل (i = 0; i < s->عدد_vp; i++) {
        cpu = RISCV_CPU(object_new(s->cpu_type));

        /* All VPs are halted on reset. Leave powering up to CPC. */
        object_property_سیٹ_bool(OBJECT(cpu), "start-powered-off", true,
                                 &error_abort);

        إذا (!qdev_realize_and_unref(DEVICE(cpu), عدم, errp)) {
            أرجع;
        }

        /* Store CPU in array */
        s->cpus[i] = CPU(cpu);

        /* Set up mhartid */
        صحيح cluster_id = i / harts_وچ_cluster;
        صحيح hart_id = (i % harts_وچ_cluster) % s->عدد_hart;
        صحيح core_id = (i % harts_وچ_cluster) / s->عدد_hart;
        صحيح mhartid = (cluster_id << MHARTID_CLUSTER_کھسکاؤ) +
                      (core_id << MHARTID_CORE_کھسکاؤ) +
                      (hart_id << MHARTID_HART_کھسکاؤ);
        cpu->env.mhartid = mhartid;
        qemu_رجسٹر_reset(main_cpu_reset, s->cpus[i]);
    }

    /* Cluster Power Controller */
    object_initialize_بچہ(OBJECT(dev), "cpc", &s->cpc, TYPE_RISCV_CPC);
    object_property_سیٹ_uint(OBJECT(&s->cpc), "cluster-id", 0,
                            &error_abort);
    object_property_سیٹ_uint(OBJECT(&s->cpc), "num-vp", s->عدد_vp,
                            &error_abort);
    object_property_سیٹ_uint(OBJECT(&s->cpc), "num-hart", s->عدد_hart,
                            &error_abort);
    object_property_سیٹ_uint(OBJECT(&s->cpc), "num-core", s->عدد_core,
                            &error_abort);

    /* Pass CPUs to CPC using link properties */
    لكل (i = 0; i < s->عدد_vp; i++) {
        حرف *propname = g_strdup_printf("cpu[%d]", i);
        object_property_سیٹ_link(OBJECT(&s->cpc), propname,
                                OBJECT(s->cpus[i]), &error_abort);
        g_free(propname);
    }

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->cpc), errp)) {
        أرجع;
    }

    حافظہ_region_جوڑو_subregion(&s->container, 0,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->cpc), 0));

    /* Global Configuration Registers */
    object_initialize_بچہ(OBJECT(dev), "gcr", &s->gcr, TYPE_RISCV_GCR);
    object_property_سیٹ_uint(OBJECT(&s->gcr), "cluster-id", 0,
                            &error_abort);
    object_property_سیٹ_uint(OBJECT(&s->gcr), "num-vp", s->عدد_vp,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&s->gcr), "gcr-rev", 0xa00,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&s->gcr), "gcr-base", s->gcr_ادھار,
                            &error_abort);
    object_property_سیٹ_link(OBJECT(&s->gcr), "cpc", OBJECT(&s->cpc.mr),
                             &error_abort);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->gcr), errp)) {
        أرجع;
    }

    حافظہ_region_جوڑو_subregion(&s->container, s->gcr_ادھار,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->gcr), 0));

    لكل (i = 0; i < عدد_دا_clusters; i++) {
        uint64_t cm_ادھار = GLOBAL_CM_ادھار + (CM_ناپ * i);
        uint32_t hartid_ادھار = i << MHARTID_CLUSTER_کھسکاؤ;
        s->aplic = riscv_aplic_create(&s->container,
                                      cm_ادھار + AIA_PLIC_M_وتھ,
                                      AIA_PLIC_M_ناپ,
                                      hartid_ادھار, /* hartid_base */
                                      MAX_HARTS, /* num_harts */
                                      APLIC_عدد_SOURCES,
                                      APLIC_عدد_PRIO_بٹ,
                                      false, true, عدم);
        riscv_aplic_create(&s->container,
                           cm_ادھار + AIA_PLIC_S_وتھ,
                           AIA_PLIC_S_ناپ,
                           hartid_ادھار, /* hartid_base */
                           MAX_HARTS, /* num_harts */
                           APLIC_عدد_SOURCES,
                           APLIC_عدد_PRIO_بٹ,
                           false, false, s->aplic);
        /* PLIC changes msi_nonbroken to ture. We revert the change. */
        msi_nonbroken = false;
        riscv_aclint_swi_create(&s->container,
                                cm_ادھار + AIA_CLINT_وتھ,
                                hartid_ادھار, MAX_HARTS, false);
        riscv_aclint_mtimer_create(&s->container,
                                   cm_ادھار + AIA_CLINT_وتھ +
                                   RISCV_ACLINT_SWI_ناپ,
                                   RISCV_ACLINT_DEFAULT_MTIMER_ناپ,
                                   hartid_ادھار,
                                   MAX_HARTS,
                                   RISCV_ACLINT_DEFAULT_MTIMECMP,
                                   RISCV_ACLINT_DEFAULT_MTIME,
                                   RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }
}

ساكن ثابت Property riscv_cps_properties[] = {
    DEFINE_PROP_UINT32("num-vp", RISCVCPSState, عدد_vp, 1),
    DEFINE_PROP_UINT32("num-hart", RISCVCPSState, عدد_hart, 1),
    DEFINE_PROP_UINT32("num-core", RISCVCPSState, عدد_core, 1),
    DEFINE_PROP_UINT64("gcr-base", RISCVCPSState, gcr_ادھار, GCR_ادھار_ADDR),
    DEFINE_PROP_STRING("cpu-type", RISCVCPSState, cpu_type),
};

ساكن فراغ riscv_cps_class_init(ObjectClass *klass, ثابت فراغ *انکڑے)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = riscv_cps_realize;
    device_class_سیٹ_props(dc, riscv_cps_properties);
}

ساكن ثابت TypeInfo riscv_cps_info = {
    .name = TYPE_RISCV_CPS,
    .ماپا = TYPE_SYS_BUS_DEVICE,
    .instance_ناپ = حجم(RISCVCPSState),
    .instance_init = riscv_cps_init,
    .class_init = riscv_cps_class_init,
};

ساكن فراغ riscv_cps_رجسٹر_types(فراغ)
{
    type_رجسٹر_static(&riscv_cps_info);
}

type_init(riscv_cps_رجسٹر_types)
