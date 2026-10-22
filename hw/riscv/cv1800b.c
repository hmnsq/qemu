/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sophgo CV1800B SoC
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#ضمن "qemu/osdep.h"
#ضمن "qapi/error.h"
#ضمن "hw/riscv/cv1800b.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "target/riscv/cpu-qom.h"
#ضمن "system/system.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "system/address-spaces.h"
#ضمن "hw/intc/sifive_plic.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/sd/sdhci.h"
#ضمن "hw/misc/unimp.h"

ثابت Mem_Map_مدخل cv1800b_memmap[] = {
    [CV1800_B_DEV_بالاترین_MISC]     = { 0x03000000,      0x1000 },
    [CV1800B_DEV_PINMUX]       = { 0x03001000,      0x1000 },
    [CV1800B_DEV_CLK]          = { 0x03002000,      0x1000 },
    [CV1800B_DEV_RST]          = { 0x03003000,      0x1000 },
    [CV1800B_DEV_WDT]          = { 0x03010000,      0x1000 },
    [CV1800B_DEV_GPIO]         = { 0x03020000,      0x4000 },
    [CV1800B_DEV_UART0]        = { 0x04140000,     0x10000 },
    [CV1800B_DEV_SD0]          = { 0x04310000,     0x10000 },
    [CV1800B_DEV_ROM]          = { 0x04400000,     0x10000 },
    [CV1800B_DEV_RTC_GPIO]     = { 0x05021000,      0x1000 },
    [CV1800B_DEV_RTC_IO]       = { 0x05027000,      0x1000 },
    [CV1800B_DEV_PLIC]         = { 0x70000000,   0x4000000 },
    [CV1800B_DEV_CLINT]        = { 0x74000000,     0x10000 },
    [CV1800B_DEV_DRAM]         = { 0x80000000,         0x0 },
};

ساكن فراغ cv1800b_soc_instance_init(Object *obj)
{
    CV1800BSoCState *s = CV1800B_SOC(obj);

    object_initialize_فرزند(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    qdev_prop_مجموعه_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_مجموعه_string(DEVICE(&s->cpus), "cpu-type", TYPE_RISCV_CPU_THEAD_C906);
    qdev_prop_مجموعه_uint64(DEVICE(&s->cpus), "resetvec",
                         cv1800b_memmap[CV1800B_DEV_ROM].پایه);

    object_initialize_فرزند(obj, "clk", &s->clk, TYPE_CV1800B_CLK);
    object_initialize_فرزند(obj, "uart", &s->uart, TYPE_DW8250);
    object_initialize_فرزند(obj, "sdhci", &s->sdhci, TYPE_SYSBUS_SDHCI);
}

ساكن فراغ cv1800b_soc_realize(DeviceState *dev, Error **errp)
{
    CV1800BSoCState *s = CV1800B_SOC(dev);
    MachineState *ms = MACHINE(qdev_get_machine());
    uint32_t رقم_harts = ms->smp.cpus;
    حافظه_Region *system_حافظه = get_system_حافظه();
    حرف *plic_hart_config;

    qdev_prop_مجموعه_uint32(DEVICE(&s->cpus), "num-harts", رقم_harts);

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        أرجع;
    }

    حافظه_region_init_rom(&s->rom, OBJECT(dev), "cv1800b.rom",
                           cv1800b_memmap[CV1800B_DEV_ROM].اندازه, &error_fatal);
    حافظه_region_جمع_کن_subregion(system_حافظه,
                                cv1800b_memmap[CV1800B_DEV_ROM].پایه, &s->rom);

    riscv_aclint_swi_create(system_حافظه,
                            cv1800b_memmap[CV1800B_DEV_CLINT].پایه,
                            0, رقم_harts, false);
    riscv_aclint_mtimer_create(system_حافظه,
                               cv1800b_memmap[CV1800B_DEV_CLINT].پایه +
                               RISCV_ACLINT_SWI_اندازه,
                               RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                               0, رقم_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    plic_hart_config = riscv_plic_hart_config_string(رقم_harts);
    s->plic = sifive_plic_create(
        system_حافظه,
        cv1800b_memmap[CV1800B_DEV_PLIC].پایه,
        plic_hart_config,
        رقم_harts,
        0,
        CV1800_B_PLIC_رقم_SOURCES,
        CV1800_B_PLIC_رقم_PRIORITIES,
        0x0,
        0x1000,
        0x2000,
        0x80,
        0x200000,
        0x1000,
        cv1800b_memmap[CV1800B_DEV_PLIC].اندازه);

    g_free(plic_hart_config);

    qdev_prop_مجموعه_uint8(DEVICE(&s->uart), "regshift", 2);
    qdev_prop_مجموعه_chr(DEVICE(&s->uart), "chardev", serial_hd(0));
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, cv1800b_memmap[CV1800B_DEV_UART0].پایه);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
                    qdev_get_gpio_درون(DEVICE(s->plic), CV1800B_UART0_IRQ));

    qdev_prop_مجموعه_uint8(DEVICE(&s->sdhci), "sd-spec-version", 3);
    qdev_prop_مجموعه_uint64(DEVICE(&s->sdhci), "capareg", 0x056900b9);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0, cv1800b_memmap[CV1800B_DEV_SD0].پایه);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
                       qdev_get_gpio_درون(DEVICE(s->plic), CV1800B_SD0_IRQ));

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->clk), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->clk), 0,
                    cv1800b_memmap[CV1800B_DEV_CLK].پایه);

    create_unimplemented_device("cv1800b.top_misc",
                                cv1800b_memmap[CV1800_B_DEV_بالاترین_MISC].پایه,
                                cv1800b_memmap[CV1800_B_DEV_بالاترین_MISC].اندازه);

    create_unimplemented_device("cv1800b.pinmux",
                                cv1800b_memmap[CV1800B_DEV_PINMUX].پایه,
                                cv1800b_memmap[CV1800B_DEV_PINMUX].اندازه);

    create_unimplemented_device("cv1800b.rst",
                                cv1800b_memmap[CV1800B_DEV_RST].پایه,
                                cv1800b_memmap[CV1800B_DEV_RST].اندازه);

    create_unimplemented_device("cv1800b.wdt",
                                cv1800b_memmap[CV1800B_DEV_WDT].پایه,
                                cv1800b_memmap[CV1800B_DEV_WDT].اندازه);

    create_unimplemented_device("cv1800b.gpio0_3",
                                cv1800b_memmap[CV1800B_DEV_GPIO].پایه,
                                cv1800b_memmap[CV1800B_DEV_GPIO].اندازه);

    create_unimplemented_device("cv1800b.rtc_gpio",
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].پایه,
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].اندازه);

    create_unimplemented_device("cv1800b.rtc_io",
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].پایه,
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].اندازه);
}

ساكن فراغ cv1800b_soc_class_init(ObjectClass *oc, ثابت فراغ *داده)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = cv1800b_soc_realize;
    dc->user_creatable = false;
}

ساكن ثابت TypeInfo cv1800b_soc_type_info = {
    .name = TYPE_CV1800B_SOC,
    .والد = TYPE_DEVICE,
    .instance_اندازه = حجم(CV1800BSoCState),
    .instance_init = cv1800b_soc_instance_init,
    .class_init = cv1800b_soc_class_init,
};

ساكن فراغ cv1800b_soc_ثبات_types(فراغ)
{
    type_ثبات_static(&cv1800b_soc_type_info);
}

type_init(cv1800b_soc_ثبات_types)
