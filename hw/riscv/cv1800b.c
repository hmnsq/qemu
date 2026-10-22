/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sophgo CV1800B SoC
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/cv1800b.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu-qom.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/address-spaces.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/sifive_plic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/sd/sdhci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/misc/unimp.h"

ਅਚਲ Mem_Map_ਇੰਦਰਾਜ cv1800b_memmap[] = {
    [CV1800_B_DEV_ਸਿਖਰ_MISC]     = { 0x03000000,      0x1000 },
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

ਸਥਿਰ ਖਾਲੀ cv1800b_soc_instance_init(Object *obj)
{
    CV1800BSoCState *s = CV1800B_SOC(obj);

    object_initialize_ਬੱਚਾ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    qdev_prop_ਸੈੱਟ_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_ਸੈੱਟ_string(DEVICE(&s->cpus), "cpu-type", TYPE_RISCV_CPU_THEAD_C906);
    qdev_prop_ਸੈੱਟ_uint64(DEVICE(&s->cpus), "resetvec",
                         cv1800b_memmap[CV1800B_DEV_ROM].ਅਧਾਰ);

    object_initialize_ਬੱਚਾ(obj, "clk", &s->clk, TYPE_CV1800B_CLK);
    object_initialize_ਬੱਚਾ(obj, "uart", &s->uart, TYPE_DW8250);
    object_initialize_ਬੱਚਾ(obj, "sdhci", &s->sdhci, TYPE_SYSBUS_SDHCI);
}

ਸਥਿਰ ਖਾਲੀ cv1800b_soc_realize(DeviceState *dev, Error **errp)
{
    CV1800BSoCState *s = CV1800B_SOC(dev);
    MachineState *ms = MACHINE(qdev_get_machine());
    uint32_t ਅੰਕ_harts = ms->smp.cpus;
    ਸਮ੍ਰਿਤੀ_Region *system_ਸਮ੍ਰਿਤੀ = get_system_ਸਮ੍ਰਿਤੀ();
    ਅੱਖਰ *plic_hart_config;

    qdev_prop_ਸੈੱਟ_uint32(DEVICE(&s->cpus), "num-harts", ਅੰਕ_harts);

    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        ਵਾਪਸ;
    }

    ਸਮ੍ਰਿਤੀ_region_init_rom(&s->rom, OBJECT(dev), "cv1800b.rom",
                           cv1800b_memmap[CV1800B_DEV_ROM].ਮਾਪ, &error_fatal);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ,
                                cv1800b_memmap[CV1800B_DEV_ROM].ਅਧਾਰ, &s->rom);

    riscv_aclint_swi_create(system_ਸਮ੍ਰਿਤੀ,
                            cv1800b_memmap[CV1800B_DEV_CLINT].ਅਧਾਰ,
                            0, ਅੰਕ_harts, false);
    riscv_aclint_mtimer_create(system_ਸਮ੍ਰਿਤੀ,
                               cv1800b_memmap[CV1800B_DEV_CLINT].ਅਧਾਰ +
                               RISCV_ACLINT_SWI_ਮਾਪ,
                               RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ,
                               0, ਅੰਕ_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    plic_hart_config = riscv_plic_hart_config_string(ਅੰਕ_harts);
    s->plic = sifive_plic_create(
        system_ਸਮ੍ਰਿਤੀ,
        cv1800b_memmap[CV1800B_DEV_PLIC].ਅਧਾਰ,
        plic_hart_config,
        ਅੰਕ_harts,
        0,
        CV1800_B_PLIC_ਅੰਕ_SOURCES,
        CV1800_B_PLIC_ਅੰਕ_PRIORITIES,
        0x0,
        0x1000,
        0x2000,
        0x80,
        0x200000,
        0x1000,
        cv1800b_memmap[CV1800B_DEV_PLIC].ਮਾਪ);

    g_free(plic_hart_config);

    qdev_prop_ਸੈੱਟ_uint8(DEVICE(&s->uart), "regshift", 2);
    qdev_prop_ਸੈੱਟ_chr(DEVICE(&s->uart), "chardev", serial_hd(0));
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        ਵਾਪਸ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, cv1800b_memmap[CV1800B_DEV_UART0].ਅਧਾਰ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
                    qdev_get_gpio_ਵਿੱਚ(DEVICE(s->plic), CV1800B_UART0_IRQ));

    qdev_prop_ਸੈੱਟ_uint8(DEVICE(&s->sdhci), "sd-spec-version", 3);
    qdev_prop_ਸੈੱਟ_uint64(DEVICE(&s->sdhci), "capareg", 0x056900b9);
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp)) {
        ਵਾਪਸ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0, cv1800b_memmap[CV1800B_DEV_SD0].ਅਧਾਰ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
                       qdev_get_gpio_ਵਿੱਚ(DEVICE(s->plic), CV1800B_SD0_IRQ));

    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->clk), errp)) {
        ਵਾਪਸ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->clk), 0,
                    cv1800b_memmap[CV1800B_DEV_CLK].ਅਧਾਰ);

    create_unimplemented_device("cv1800b.top_misc",
                                cv1800b_memmap[CV1800_B_DEV_ਸਿਖਰ_MISC].ਅਧਾਰ,
                                cv1800b_memmap[CV1800_B_DEV_ਸਿਖਰ_MISC].ਮਾਪ);

    create_unimplemented_device("cv1800b.pinmux",
                                cv1800b_memmap[CV1800B_DEV_PINMUX].ਅਧਾਰ,
                                cv1800b_memmap[CV1800B_DEV_PINMUX].ਮਾਪ);

    create_unimplemented_device("cv1800b.rst",
                                cv1800b_memmap[CV1800B_DEV_RST].ਅਧਾਰ,
                                cv1800b_memmap[CV1800B_DEV_RST].ਮਾਪ);

    create_unimplemented_device("cv1800b.wdt",
                                cv1800b_memmap[CV1800B_DEV_WDT].ਅਧਾਰ,
                                cv1800b_memmap[CV1800B_DEV_WDT].ਮਾਪ);

    create_unimplemented_device("cv1800b.gpio0_3",
                                cv1800b_memmap[CV1800B_DEV_GPIO].ਅਧਾਰ,
                                cv1800b_memmap[CV1800B_DEV_GPIO].ਮਾਪ);

    create_unimplemented_device("cv1800b.rtc_gpio",
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].ਅਧਾਰ,
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].ਮਾਪ);

    create_unimplemented_device("cv1800b.rtc_io",
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].ਅਧਾਰ,
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ cv1800b_soc_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = cv1800b_soc_realize;
    dc->user_creatable = false;
}

ਸਥਿਰ ਅਚਲ TypeInfo cv1800b_soc_type_info = {
    .name = TYPE_CV1800B_SOC,
    .ਮਾਪਾ = TYPE_DEVICE,
    .instance_ਮਾਪ = ਆਕਾਰ(CV1800BSoCState),
    .instance_init = cv1800b_soc_instance_init,
    .class_init = cv1800b_soc_class_init,
};

ਸਥਿਰ ਖਾਲੀ cv1800b_soc_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&cv1800b_soc_type_info);
}

type_init(cv1800b_soc_ਰਜਿਸਟਰ_types)
