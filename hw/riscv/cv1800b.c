/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sophgo CV1800B SoC
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#شامل "qemu/osdep.h"
#شامل "qapi/error.h"
#شامل "hw/riscv/cv1800b.h"
#شامل "hw/core/qdev-properties.h"
#شامل "target/riscv/cpu-qom.h"
#شامل "system/system.h"
#شامل "hw/intc/riscv_aclint.h"
#شامل "system/address-spaces.h"
#شامل "hw/intc/sifive_plic.h"
#شامل "target/riscv/cpu.h"
#شامل "hw/riscv/boot.h"
#شامل "hw/sd/sdhci.h"
#شامل "hw/misc/unimp.h"

ثابت Mem_Map_اندراج cv1800b_memmap[] = {
    [CV1800_B_DEV_چوٹی_MISC]     = { 0x03000000,      0x1000 },
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

ساکن خالی cv1800b_soc_instance_init(Object *obj)
{
    CV1800BSoCState *s = CV1800B_SOC(obj);

    object_initialize_بچہ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    qdev_prop_سیٹ_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_سیٹ_string(DEVICE(&s->cpus), "cpu-type", TYPE_RISCV_CPU_THEAD_C906);
    qdev_prop_سیٹ_uint64(DEVICE(&s->cpus), "resetvec",
                         cv1800b_memmap[CV1800B_DEV_ROM].بنیاد);

    object_initialize_بچہ(obj, "clk", &s->clk, TYPE_CV1800B_CLK);
    object_initialize_بچہ(obj, "uart", &s->uart, TYPE_DW8250);
    object_initialize_بچہ(obj, "sdhci", &s->sdhci, TYPE_SYSBUS_SDHCI);
}

ساکن خالی cv1800b_soc_realize(DeviceState *dev, Error **errp)
{
    CV1800BSoCState *s = CV1800B_SOC(dev);
    MachineState *ms = MACHINE(qdev_get_machine());
    uint32_t عدد_harts = ms->smp.cpus;
    حافظہ_Region *system_حافظہ = get_system_حافظہ();
    حرف *plic_hart_config;

    qdev_prop_سیٹ_uint32(DEVICE(&s->cpus), "num-harts", عدد_harts);

    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        واپس;
    }

    حافظہ_region_init_rom(&s->rom, OBJECT(dev), "cv1800b.rom",
                           cv1800b_memmap[CV1800B_DEV_ROM].سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion(system_حافظہ,
                                cv1800b_memmap[CV1800B_DEV_ROM].بنیاد, &s->rom);

    riscv_aclint_swi_create(system_حافظہ,
                            cv1800b_memmap[CV1800B_DEV_CLINT].بنیاد,
                            0, عدد_harts, false);
    riscv_aclint_mtimer_create(system_حافظہ,
                               cv1800b_memmap[CV1800B_DEV_CLINT].بنیاد +
                               RISCV_ACLINT_SWI_سائز,
                               RISCV_ACLINT_DEFAULT_MTIMER_سائز,
                               0, عدد_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    plic_hart_config = riscv_plic_hart_config_string(عدد_harts);
    s->plic = sifive_plic_create(
        system_حافظہ,
        cv1800b_memmap[CV1800B_DEV_PLIC].بنیاد,
        plic_hart_config,
        عدد_harts,
        0,
        CV1800_B_PLIC_عدد_SOURCES,
        CV1800_B_PLIC_عدد_PRIORITIES,
        0x0,
        0x1000,
        0x2000,
        0x80,
        0x200000,
        0x1000,
        cv1800b_memmap[CV1800B_DEV_PLIC].سائز);

    g_free(plic_hart_config);

    qdev_prop_سیٹ_uint8(DEVICE(&s->uart), "regshift", 2);
    qdev_prop_سیٹ_chr(DEVICE(&s->uart), "chardev", serial_hd(0));
    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, cv1800b_memmap[CV1800B_DEV_UART0].بنیاد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
                    qdev_get_gpio_اندر(DEVICE(s->plic), CV1800B_UART0_IRQ));

    qdev_prop_سیٹ_uint8(DEVICE(&s->sdhci), "sd-spec-version", 3);
    qdev_prop_سیٹ_uint64(DEVICE(&s->sdhci), "capareg", 0x056900b9);
    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0, cv1800b_memmap[CV1800B_DEV_SD0].بنیاد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
                       qdev_get_gpio_اندر(DEVICE(s->plic), CV1800B_SD0_IRQ));

    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->clk), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->clk), 0,
                    cv1800b_memmap[CV1800B_DEV_CLK].بنیاد);

    create_unimplemented_device("cv1800b.top_misc",
                                cv1800b_memmap[CV1800_B_DEV_چوٹی_MISC].بنیاد,
                                cv1800b_memmap[CV1800_B_DEV_چوٹی_MISC].سائز);

    create_unimplemented_device("cv1800b.pinmux",
                                cv1800b_memmap[CV1800B_DEV_PINMUX].بنیاد,
                                cv1800b_memmap[CV1800B_DEV_PINMUX].سائز);

    create_unimplemented_device("cv1800b.rst",
                                cv1800b_memmap[CV1800B_DEV_RST].بنیاد,
                                cv1800b_memmap[CV1800B_DEV_RST].سائز);

    create_unimplemented_device("cv1800b.wdt",
                                cv1800b_memmap[CV1800B_DEV_WDT].بنیاد,
                                cv1800b_memmap[CV1800B_DEV_WDT].سائز);

    create_unimplemented_device("cv1800b.gpio0_3",
                                cv1800b_memmap[CV1800B_DEV_GPIO].بنیاد,
                                cv1800b_memmap[CV1800B_DEV_GPIO].سائز);

    create_unimplemented_device("cv1800b.rtc_gpio",
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].بنیاد,
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].سائز);

    create_unimplemented_device("cv1800b.rtc_io",
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].بنیاد,
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].سائز);
}

ساکن خالی cv1800b_soc_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = cv1800b_soc_realize;
    dc->user_creatable = false;
}

ساکن ثابت TypeInfo cv1800b_soc_type_info = {
    .name = TYPE_CV1800B_SOC,
    .والد = TYPE_DEVICE,
    .instance_سائز = حجم(CV1800BSoCState),
    .instance_init = cv1800b_soc_instance_init,
    .class_init = cv1800b_soc_class_init,
};

ساکن خالی cv1800b_soc_رجسٹر_types(خالی)
{
    type_رجسٹر_static(&cv1800b_soc_type_info);
}

type_init(cv1800b_soc_رجسٹر_types)
