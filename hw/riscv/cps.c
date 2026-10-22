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
    حافظه_region_init(&s->container, obj, "mips-cps-container", UINT64_MAX);
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
    إذا (s->رقم_vp == 0) {
        error_setg(errp, "num-vp must be at least 1");
        أرجع;
    }
    إذا (s->رقم_vp > MAX_HARTS) {
        error_setg(errp, "num-vp cannot exceed %d", MAX_HARTS);
        أرجع;
    }

    /* Allocate CPU array */
    s->cpus = g_new0(CPUState *, s->رقم_vp);

    /* Set up cpu_index and mhartid for avaiable CPUs. */
    صحيح harts_درون_cluster = s->رقم_hart * s->رقم_core;
    صحيح رقم_از_clusters = s->رقم_vp / harts_درون_cluster;
    لكل (i = 0; i < s->رقم_vp; i++) {
        cpu = RISCV_CPU(object_new(s->cpu_type));

        /* All VPs are halted on reset. Leave powering up to CPC. */
        object_property_مجموعه_bool(OBJECT(cpu), "start-powered-off", true,
                                 &error_abort);

        إذا (!qdev_realize_and_unref(DEVICE(cpu), عدم, errp)) {
            أرجع;
        }

        /* Store CPU in array */
        s->cpus[i] = CPU(cpu);

        /* Set up mhartid */
        صحيح cluster_id = i / harts_درون_cluster;
        صحيح hart_id = (i % harts_درون_cluster) % s->رقم_hart;
        صحيح core_id = (i % harts_درون_cluster) / s->رقم_hart;
        صحيح mhartid = (cluster_id << MHARTID_CLUSTER_لغزش) +
                      (core_id << MHARTID_CORE_لغزش) +
                      (hart_id << MHARTID_HART_لغزش);
        cpu->env.mhartid = mhartid;
        qemu_ثبات_reset(main_cpu_reset, s->cpus[i]);
    }

    /* Cluster Power Controller */
    object_initialize_فرزند(OBJECT(dev), "cpc", &s->cpc, TYPE_RISCV_CPC);
    object_property_مجموعه_uint(OBJECT(&s->cpc), "cluster-id", 0,
                            &error_abort);
    object_property_مجموعه_uint(OBJECT(&s->cpc), "num-vp", s->رقم_vp,
                            &error_abort);
    object_property_مجموعه_uint(OBJECT(&s->cpc), "num-hart", s->رقم_hart,
                            &error_abort);
    object_property_مجموعه_uint(OBJECT(&s->cpc), "num-core", s->رقم_core,
                            &error_abort);

    /* Pass CPUs to CPC using link properties */
    لكل (i = 0; i < s->رقم_vp; i++) {
        حرف *propname = g_strdup_printf("cpu[%d]", i);
        object_property_مجموعه_link(OBJECT(&s->cpc), propname,
                                OBJECT(s->cpus[i]), &error_abort);
        g_free(propname);
    }

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->cpc), errp)) {
        أرجع;
    }

    حافظه_region_جمع_کن_subregion(&s->container, 0,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->cpc), 0));

    /* Global Configuration Registers */
    object_initialize_فرزند(OBJECT(dev), "gcr", &s->gcr, TYPE_RISCV_GCR);
    object_property_مجموعه_uint(OBJECT(&s->gcr), "cluster-id", 0,
                            &error_abort);
    object_property_مجموعه_uint(OBJECT(&s->gcr), "num-vp", s->رقم_vp,
                            &error_abort);
    object_property_مجموعه_int(OBJECT(&s->gcr), "gcr-rev", 0xa00,
                            &error_abort);
    object_property_مجموعه_int(OBJECT(&s->gcr), "gcr-base", s->gcr_پایه,
                            &error_abort);
    object_property_مجموعه_link(OBJECT(&s->gcr), "cpc", OBJECT(&s->cpc.mr),
                             &error_abort);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->gcr), errp)) {
        أرجع;
    }

    حافظه_region_جمع_کن_subregion(&s->container, s->gcr_پایه,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->gcr), 0));

    لكل (i = 0; i < رقم_از_clusters; i++) {
        uint64_t cm_پایه = GLOBAL_CM_پایه + (CM_اندازه * i);
        uint32_t hartid_پایه = i << MHARTID_CLUSTER_لغزش;
        s->aplic = riscv_aplic_create(&s->container,
                                      cm_پایه + AIA_PLIC_M_جابه_جایی,
                                      AIA_PLIC_M_اندازه,
                                      hartid_پایه, /* hartid_base */
                                      MAX_HARTS, /* num_harts */
                                      APLIC_رقم_SOURCES,
                                      APLIC_رقم_PRIO_بیتها,
                                      false, true, عدم);
        riscv_aplic_create(&s->container,
                           cm_پایه + AIA_PLIC_S_جابه_جایی,
                           AIA_PLIC_S_اندازه,
                           hartid_پایه, /* hartid_base */
                           MAX_HARTS, /* num_harts */
                           APLIC_رقم_SOURCES,
                           APLIC_رقم_PRIO_بیتها,
                           false, false, s->aplic);
        /* PLIC changes msi_nonbroken to ture. We revert the change. */
        msi_nonbroken = false;
        riscv_aclint_swi_create(&s->container,
                                cm_پایه + AIA_CLINT_جابه_جایی,
                                hartid_پایه, MAX_HARTS, false);
        riscv_aclint_mtimer_create(&s->container,
                                   cm_پایه + AIA_CLINT_جابه_جایی +
                                   RISCV_ACLINT_SWI_اندازه,
                                   RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                                   hartid_پایه,
                                   MAX_HARTS,
                                   RISCV_ACLINT_DEFAULT_MTIMECMP,
                                   RISCV_ACLINT_DEFAULT_MTIME,
                                   RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }
}

ساكن ثابت Property riscv_cps_properties[] = {
    DEFINE_PROP_UINT32("num-vp", RISCVCPSState, رقم_vp, 1),
    DEFINE_PROP_UINT32("num-hart", RISCVCPSState, رقم_hart, 1),
    DEFINE_PROP_UINT32("num-core", RISCVCPSState, رقم_core, 1),
    DEFINE_PROP_UINT64("gcr-base", RISCVCPSState, gcr_پایه, GCR_پایه_ADDR),
    DEFINE_PROP_STRING("cpu-type", RISCVCPSState, cpu_type),
};

ساكن فراغ riscv_cps_class_init(ObjectClass *klass, ثابت فراغ *داده)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = riscv_cps_realize;
    device_class_مجموعه_props(dc, riscv_cps_properties);
}

ساكن ثابت TypeInfo riscv_cps_info = {
    .name = TYPE_RISCV_CPS,
    .والد = TYPE_SYS_BUS_DEVICE,
    .instance_اندازه = حجم(RISCVCPSState),
    .instance_init = riscv_cps_init,
    .class_init = riscv_cps_class_init,
};

ساكن فراغ riscv_cps_ثبات_types(فراغ)
{
    type_ثبات_static(&riscv_cps_info);
}

type_init(riscv_cps_ثبات_types)
