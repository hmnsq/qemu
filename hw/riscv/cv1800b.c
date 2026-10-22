/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sophgo CV1800B SoC
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#অন্তর্ভাব "qemu/osdep.h"
#অন্তর্ভাব "qapi/error.h"
#অন্তর্ভাব "hw/riscv/cv1800b.h"
#অন্তর্ভাব "hw/core/qdev-properties.h"
#অন্তর্ভাব "target/riscv/cpu-qom.h"
#অন্তর্ভাব "system/system.h"
#অন্তর্ভাব "hw/intc/riscv_aclint.h"
#অন্তর্ভাব "system/address-spaces.h"
#অন্তর্ভাব "hw/intc/sifive_plic.h"
#অন্তর্ভাব "target/riscv/cpu.h"
#অন্তর্ভাব "hw/riscv/boot.h"
#অন্তর্ভাব "hw/sd/sdhci.h"
#অন্তর্ভাব "hw/misc/unimp.h"

ধ্রুব MemMapEntry cv1800b_memmap[] = {
    [CV1800_B_DEV_শীর্ষ_MISC]     = { 0x03000000,      0x1000 },
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

স্থির শূন্য cv1800b_soc_instance_init(Object *obj)
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

স্থির শূন্য cv1800b_soc_realize(DeviceState *dev, Error **errp)
{
    CV1800BSoCState *s = CV1800B_SOC(dev);
    MachineState *ms = MACHINE(qdev_get_machine());
    uint32_t অঙ্ক_harts = ms->smp.cpus;
    MemoryRegion *system_memory = get_system_memory();
    বর্ণ *plic_hart_config;

    qdev_prop_set_uint32(DEVICE(&s->cpus), "num-harts", অঙ্ক_harts);

    যদি (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        প্রত্যাগম;
    }

    memory_region_init_rom(&s->rom, OBJECT(dev), "cv1800b.rom",
                           cv1800b_memmap[CV1800B_DEV_ROM].size, &error_fatal);
    memory_region_add_subregion(system_memory,
                                cv1800b_memmap[CV1800B_DEV_ROM].base, &s->rom);

    riscv_aclint_swi_create(system_memory,
                            cv1800b_memmap[CV1800B_DEV_CLINT].base,
                            0, অঙ্ক_harts, false);
    riscv_aclint_mtimer_create(system_memory,
                               cv1800b_memmap[CV1800B_DEV_CLINT].base +
                               RISCV_ACLINT_SWI_SIZE,
                               RISCV_ACLINT_DEFAULT_MTIMER_SIZE,
                               0, অঙ্ক_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    plic_hart_config = riscv_plic_hart_config_string(অঙ্ক_harts);
    s->plic = sifive_plic_create(
        system_memory,
        cv1800b_memmap[CV1800B_DEV_PLIC].base,
        plic_hart_config,
        অঙ্ক_harts,
        0,
        CV1800_B_PLIC_অঙ্ক_SOURCES,
        CV1800_B_PLIC_অঙ্ক_PRIORITIES,
        0x0,
        0x1000,
        0x2000,
        0x80,
        0x200000,
        0x1000,
        cv1800b_memmap[CV1800B_DEV_PLIC].size);

    g_free(plic_hart_config);

    qdev_prop_set_uint8(DEVICE(&s->uart), "regshift", 2);
    qdev_prop_set_chr(DEVICE(&s->uart), "chardev", serial_hd(0));
    যদি (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        প্রত্যাগম;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, cv1800b_memmap[CV1800B_DEV_UART0].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
                    qdev_get_gpio_in(DEVICE(s->plic), CV1800B_UART0_IRQ));

    qdev_prop_set_uint8(DEVICE(&s->sdhci), "sd-spec-version", 3);
    qdev_prop_set_uint64(DEVICE(&s->sdhci), "capareg", 0x056900b9);
    যদি (!sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp)) {
        প্রত্যাগম;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0, cv1800b_memmap[CV1800B_DEV_SD0].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
                       qdev_get_gpio_in(DEVICE(s->plic), CV1800B_SD0_IRQ));

    যদি (!sysbus_realize(SYS_BUS_DEVICE(&s->clk), errp)) {
        প্রত্যাগম;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->clk), 0,
                    cv1800b_memmap[CV1800B_DEV_CLK].base);

    create_unimplemented_device("cv1800b.top_misc",
                                cv1800b_memmap[CV1800_B_DEV_শীর্ষ_MISC].base,
                                cv1800b_memmap[CV1800_B_DEV_শীর্ষ_MISC].size);

    create_unimplemented_device("cv1800b.pinmux",
                                cv1800b_memmap[CV1800B_DEV_PINMUX].base,
                                cv1800b_memmap[CV1800B_DEV_PINMUX].size);

    create_unimplemented_device("cv1800b.rst",
                                cv1800b_memmap[CV1800B_DEV_RST].base,
                                cv1800b_memmap[CV1800B_DEV_RST].size);

    create_unimplemented_device("cv1800b.wdt",
                                cv1800b_memmap[CV1800B_DEV_WDT].base,
                                cv1800b_memmap[CV1800B_DEV_WDT].size);

    create_unimplemented_device("cv1800b.gpio0_3",
                                cv1800b_memmap[CV1800B_DEV_GPIO].base,
                                cv1800b_memmap[CV1800B_DEV_GPIO].size);

    create_unimplemented_device("cv1800b.rtc_gpio",
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].base,
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].size);

    create_unimplemented_device("cv1800b.rtc_io",
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].base,
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].size);
}

স্থির শূন্য cv1800b_soc_class_init(ObjectClass *oc, ধ্রুব শূন্য *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = cv1800b_soc_realize;
    dc->user_creatable = false;
}

স্থির ধ্রুব TypeInfo cv1800b_soc_type_info = {
    .name = TYPE_CV1800B_SOC,
    .parent = TYPE_DEVICE,
    .instance_size = পরিমাণ(CV1800BSoCState),
    .instance_init = cv1800b_soc_instance_init,
    .class_init = cv1800b_soc_class_init,
};

স্থির শূন্য cv1800b_soc_register_types(শূন্য)
{
    type_register_static(&cv1800b_soc_type_info);
}

type_init(cv1800b_soc_register_types)
