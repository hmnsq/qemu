/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sophgo CV1800B SoC
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#समावेश "qemu/osdep.h"
#समावेश "qapi/error.h"
#समावेश "hw/riscv/cv1800b.h"
#समावेश "hw/core/qdev-properties.h"
#समावेश "target/riscv/cpu-qom.h"
#समावेश "system/system.h"
#समावेश "hw/intc/riscv_aclint.h"
#समावेश "system/address-spaces.h"
#समावेश "hw/intc/sifive_plic.h"
#समावेश "target/riscv/cpu.h"
#समावेश "hw/riscv/boot.h"
#समावेश "hw/sd/sdhci.h"
#समावेश "hw/misc/unimp.h"

स्थिर MemMapEntry cv1800b_memmap[] = {
    [CV1800_B_DEV_शीर्ष_MISC]     = { 0x03000000,      0x1000 },
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

अटल व्योम cv1800b_soc_instance_init(Object *obj)
{
    CV1800BSoCState *s = CV1800B_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    qdev_prop_set_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_set_string(DEVICE(&s->cpus), "cpu-type", TYPE_RISCV_CPU_THEAD_C906);
    qdev_prop_set_uint64(DEVICE(&s->cpus), "resetvec",
                         cv1800b_memmap[CV1800B_DEV_ROM].base);

    object_initialize_child(obj, "clk", &s->clk, TYPE_CV1800B_CLK);
    object_initialize_child(obj, "uart", &s->uart, TYPE_DW8250);
    object_initialize_child(obj, "sdhci", &s->sdhci, TYPE_SYSBUS_SDHCI);
}

अटल व्योम cv1800b_soc_realize(DeviceState *dev, Error **errp)
{
    CV1800BSoCState *s = CV1800B_SOC(dev);
    MachineState *ms = MACHINE(qdev_get_machine());
    uint32_t अंक_harts = ms->smp.cpus;
    MemoryRegion *system_memory = get_system_memory();
    अक्षर *plic_hart_config;

    qdev_prop_set_uint32(DEVICE(&s->cpus), "num-harts", अंक_harts);

    अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        वापस;
    }

    memory_region_init_rom(&s->rom, OBJECT(dev), "cv1800b.rom",
                           cv1800b_memmap[CV1800B_DEV_ROM].आकार, &error_fatal);
    memory_region_add_subregion(system_memory,
                                cv1800b_memmap[CV1800B_DEV_ROM].base, &s->rom);

    riscv_aclint_swi_create(system_memory,
                            cv1800b_memmap[CV1800B_DEV_CLINT].base,
                            0, अंक_harts, false);
    riscv_aclint_mtimer_create(system_memory,
                               cv1800b_memmap[CV1800B_DEV_CLINT].base +
                               RISCV_ACLINT_SWI_आकार,
                               RISCV_ACLINT_DEFAULT_MTIMER_आकार,
                               0, अंक_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    plic_hart_config = riscv_plic_hart_config_string(अंक_harts);
    s->plic = sifive_plic_create(
        system_memory,
        cv1800b_memmap[CV1800B_DEV_PLIC].base,
        plic_hart_config,
        अंक_harts,
        0,
        CV1800_B_PLIC_अंक_SOURCES,
        CV1800_B_PLIC_अंक_PRIORITIES,
        0x0,
        0x1000,
        0x2000,
        0x80,
        0x200000,
        0x1000,
        cv1800b_memmap[CV1800B_DEV_PLIC].आकार);

    g_free(plic_hart_config);

    qdev_prop_set_uint8(DEVICE(&s->uart), "regshift", 2);
    qdev_prop_set_chr(DEVICE(&s->uart), "chardev", serial_hd(0));
    अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        वापस;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, cv1800b_memmap[CV1800B_DEV_UART0].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
                    qdev_get_gpio_in(DEVICE(s->plic), CV1800B_UART0_IRQ));

    qdev_prop_set_uint8(DEVICE(&s->sdhci), "sd-spec-version", 3);
    qdev_prop_set_uint64(DEVICE(&s->sdhci), "capareg", 0x056900b9);
    अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp)) {
        वापस;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0, cv1800b_memmap[CV1800B_DEV_SD0].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
                       qdev_get_gpio_in(DEVICE(s->plic), CV1800B_SD0_IRQ));

    अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->clk), errp)) {
        वापस;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->clk), 0,
                    cv1800b_memmap[CV1800B_DEV_CLK].base);

    create_unimplemented_device("cv1800b.top_misc",
                                cv1800b_memmap[CV1800_B_DEV_शीर्ष_MISC].base,
                                cv1800b_memmap[CV1800_B_DEV_शीर्ष_MISC].आकार);

    create_unimplemented_device("cv1800b.pinmux",
                                cv1800b_memmap[CV1800B_DEV_PINMUX].base,
                                cv1800b_memmap[CV1800B_DEV_PINMUX].आकार);

    create_unimplemented_device("cv1800b.rst",
                                cv1800b_memmap[CV1800B_DEV_RST].base,
                                cv1800b_memmap[CV1800B_DEV_RST].आकार);

    create_unimplemented_device("cv1800b.wdt",
                                cv1800b_memmap[CV1800B_DEV_WDT].base,
                                cv1800b_memmap[CV1800B_DEV_WDT].आकार);

    create_unimplemented_device("cv1800b.gpio0_3",
                                cv1800b_memmap[CV1800B_DEV_GPIO].base,
                                cv1800b_memmap[CV1800B_DEV_GPIO].आकार);

    create_unimplemented_device("cv1800b.rtc_gpio",
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].base,
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].आकार);

    create_unimplemented_device("cv1800b.rtc_io",
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].base,
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].आकार);
}

अटल व्योम cv1800b_soc_class_init(ObjectClass *oc, स्थिर व्योम *तथ्य)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = cv1800b_soc_realize;
    dc->user_creatable = false;
}

अटल स्थिर TypeInfo cv1800b_soc_type_info = {
    .name = TYPE_CV1800B_SOC,
    .parent = TYPE_DEVICE,
    .instance_आकार = माप(CV1800BSoCState),
    .instance_init = cv1800b_soc_instance_init,
    .class_init = cv1800b_soc_class_init,
};

अटल व्योम cv1800b_soc_register_types(व्योम)
{
    type_register_static(&cv1800b_soc_type_info);
}

type_init(cv1800b_soc_register_types)
