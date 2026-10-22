/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sophgo CV1800B SoC
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "hw/riscv/cv1800b.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "target/riscv/cpu-qom.h"
#अन्तर्भाव "system/system.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "system/address-spaces.h"
#अन्तर्भाव "hw/intc/sifive_plic.h"
#अन्तर्भाव "target/riscv/cpu.h"
#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "hw/sd/sdhci.h"
#अन्तर्भाव "hw/misc/unimp.h"

ध्रुव Mem_Map_प्रविष्टिः cv1800b_memmap[] = {
    [CV1800_B_DEV_शिखरम्_MISC]     = { 0x03000000,      0x1000 },
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

स्थिर शून्य cv1800b_soc_instance_init(Object *obj)
{
    CV1800BSoCState *s = CV1800B_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    qdev_prop_समुच्चयः_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_समुच्चयः_string(DEVICE(&s->cpus), "cpu-type", TYPE_RISCV_CPU_THEAD_C906);
    qdev_prop_समुच्चयः_uint64(DEVICE(&s->cpus), "resetvec",
                         cv1800b_memmap[CV1800B_DEV_ROM].आधारः);

    object_initialize_child(obj, "clk", &s->clk, TYPE_CV1800B_CLK);
    object_initialize_child(obj, "uart", &s->uart, TYPE_DW8250);
    object_initialize_child(obj, "sdhci", &s->sdhci, TYPE_SYSBUS_SDHCI);
}

स्थिर शून्य cv1800b_soc_realize(DeviceState *dev, Error **errp)
{
    CV1800BSoCState *s = CV1800B_SOC(dev);
    MachineState *ms = MACHINE(qdev_get_machine());
    uint32_t अङ्कः_harts = ms->smp.cpus;
    स्मृतिः_Region *system_स्मृतिः = get_system_स्मृतिः();
    वर्ण *plic_hart_config;

    qdev_prop_समुच्चयः_uint32(DEVICE(&s->cpus), "num-harts", अङ्कः_harts);

    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        प्रत्यागम;
    }

    स्मृतिः_region_init_rom(&s->rom, OBJECT(dev), "cv1800b.rom",
                           cv1800b_memmap[CV1800B_DEV_ROM].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                cv1800b_memmap[CV1800B_DEV_ROM].आधारः, &s->rom);

    riscv_aclint_swi_create(system_स्मृतिः,
                            cv1800b_memmap[CV1800B_DEV_CLINT].आधारः,
                            0, अङ्कः_harts, false);
    riscv_aclint_mtimer_create(system_स्मृतिः,
                               cv1800b_memmap[CV1800B_DEV_CLINT].आधारः +
                               RISCV_ACLINT_SWI_आकार,
                               RISCV_ACLINT_DEFAULT_MTIMER_आकार,
                               0, अङ्कः_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    plic_hart_config = riscv_plic_hart_config_string(अङ्कः_harts);
    s->plic = sifive_plic_create(
        system_स्मृतिः,
        cv1800b_memmap[CV1800B_DEV_PLIC].आधारः,
        plic_hart_config,
        अङ्कः_harts,
        0,
        CV1800_B_PLIC_अङ्कः_SOURCES,
        CV1800_B_PLIC_अङ्कः_PRIORITIES,
        0x0,
        0x1000,
        0x2000,
        0x80,
        0x200000,
        0x1000,
        cv1800b_memmap[CV1800B_DEV_PLIC].आकार);

    g_free(plic_hart_config);

    qdev_prop_समुच्चयः_uint8(DEVICE(&s->uart), "regshift", 2);
    qdev_prop_समुच्चयः_chr(DEVICE(&s->uart), "chardev", serial_hd(0));
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        प्रत्यागम;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, cv1800b_memmap[CV1800B_DEV_UART0].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
                    qdev_get_gpio_अन्तर्(DEVICE(s->plic), CV1800B_UART0_IRQ));

    qdev_prop_समुच्चयः_uint8(DEVICE(&s->sdhci), "sd-spec-version", 3);
    qdev_prop_समुच्चयः_uint64(DEVICE(&s->sdhci), "capareg", 0x056900b9);
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp)) {
        प्रत्यागम;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0, cv1800b_memmap[CV1800B_DEV_SD0].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
                       qdev_get_gpio_अन्तर्(DEVICE(s->plic), CV1800B_SD0_IRQ));

    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->clk), errp)) {
        प्रत्यागम;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->clk), 0,
                    cv1800b_memmap[CV1800B_DEV_CLK].आधारः);

    create_unimplemented_device("cv1800b.top_misc",
                                cv1800b_memmap[CV1800_B_DEV_शिखरम्_MISC].आधारः,
                                cv1800b_memmap[CV1800_B_DEV_शिखरम्_MISC].आकार);

    create_unimplemented_device("cv1800b.pinmux",
                                cv1800b_memmap[CV1800B_DEV_PINMUX].आधारः,
                                cv1800b_memmap[CV1800B_DEV_PINMUX].आकार);

    create_unimplemented_device("cv1800b.rst",
                                cv1800b_memmap[CV1800B_DEV_RST].आधारः,
                                cv1800b_memmap[CV1800B_DEV_RST].आकार);

    create_unimplemented_device("cv1800b.wdt",
                                cv1800b_memmap[CV1800B_DEV_WDT].आधारः,
                                cv1800b_memmap[CV1800B_DEV_WDT].आकार);

    create_unimplemented_device("cv1800b.gpio0_3",
                                cv1800b_memmap[CV1800B_DEV_GPIO].आधारः,
                                cv1800b_memmap[CV1800B_DEV_GPIO].आकार);

    create_unimplemented_device("cv1800b.rtc_gpio",
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].आधारः,
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].आकार);

    create_unimplemented_device("cv1800b.rtc_io",
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].आधारः,
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].आकार);
}

स्थिर शून्य cv1800b_soc_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = cv1800b_soc_realize;
    dc->user_creatable = false;
}

स्थिर ध्रुव TypeInfo cv1800b_soc_type_info = {
    .name = TYPE_CV1800B_SOC,
    .parent = TYPE_DEVICE,
    .instance_आकार = परिमाण(CV1800BSoCState),
    .instance_init = cv1800b_soc_instance_init,
    .class_init = cv1800b_soc_class_init,
};

स्थिर शून्य cv1800b_soc_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&cv1800b_soc_type_info);
}

type_init(cv1800b_soc_पञ्जिकाकोशः_types)
