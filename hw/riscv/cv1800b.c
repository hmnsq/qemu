/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sophgo CV1800B SoC
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/cv1800b.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu-qom.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "system/address-spaces.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/sifive_plic.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/sd/sdhci.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/unimp.h"

ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು cv1800b_memmap[] = {
    [CV1800_B_DEV_ತುದಿ_MISC]     = { 0x03000000,      0x1000 },
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

ಸ್ಥಿರ ಶೂನ್ಯ cv1800b_soc_instance_init(Object *obj)
{
    CV1800BSoCState *s = CV1800B_SOC(obj);

    object_initialize_ಮಗು(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_ಗಣ_string(DEVICE(&s->cpus), "cpu-type", TYPE_RISCV_CPU_THEAD_C906);
    qdev_prop_ಗಣ_uint64(DEVICE(&s->cpus), "resetvec",
                         cv1800b_memmap[CV1800B_DEV_ROM].ತಳ);

    object_initialize_ಮಗು(obj, "clk", &s->clk, TYPE_CV1800B_CLK);
    object_initialize_ಮಗು(obj, "uart", &s->uart, TYPE_DW8250);
    object_initialize_ಮಗು(obj, "sdhci", &s->sdhci, TYPE_SYSBUS_SDHCI);
}

ಸ್ಥಿರ ಶೂನ್ಯ cv1800b_soc_realize(DeviceState *dev, Error **errp)
{
    CV1800BSoCState *s = CV1800B_SOC(dev);
    MachineState *ms = MACHINE(qdev_get_machine());
    uint32_t ಅಂಕೆ_harts = ms->smp.cpus;
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    ಅಕ್ಷರ *plic_hart_config;

    qdev_prop_ಗಣ_uint32(DEVICE(&s->cpus), "num-harts", ಅಂಕೆ_harts);

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಮರಣೆ_region_init_rom(&s->rom, OBJECT(dev), "cv1800b.rom",
                           cv1800b_memmap[CV1800B_DEV_ROM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                cv1800b_memmap[CV1800B_DEV_ROM].ತಳ, &s->rom);

    riscv_aclint_swi_create(system_ಸ್ಮರಣೆ,
                            cv1800b_memmap[CV1800B_DEV_CLINT].ತಳ,
                            0, ಅಂಕೆ_harts, false);
    riscv_aclint_mtimer_create(system_ಸ್ಮರಣೆ,
                               cv1800b_memmap[CV1800B_DEV_CLINT].ತಳ +
                               RISCV_ACLINT_SWI_ಗಾತ್ರ,
                               RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ,
                               0, ಅಂಕೆ_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    plic_hart_config = riscv_plic_hart_config_string(ಅಂಕೆ_harts);
    s->plic = sifive_plic_create(
        system_ಸ್ಮರಣೆ,
        cv1800b_memmap[CV1800B_DEV_PLIC].ತಳ,
        plic_hart_config,
        ಅಂಕೆ_harts,
        0,
        CV1800_B_PLIC_ಅಂಕೆ_SOURCES,
        CV1800_B_PLIC_ಅಂಕೆ_PRIORITIES,
        0x0,
        0x1000,
        0x2000,
        0x80,
        0x200000,
        0x1000,
        cv1800b_memmap[CV1800B_DEV_PLIC].ಗಾತ್ರ_2);

    g_free(plic_hart_config);

    qdev_prop_ಗಣ_uint8(DEVICE(&s->uart), "regshift", 2);
    qdev_prop_ಗಣ_chr(DEVICE(&s->uart), "chardev", serial_hd(0));
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, cv1800b_memmap[CV1800B_DEV_UART0].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
                    qdev_get_gpio_ಒಳ(DEVICE(s->plic), CV1800B_UART0_IRQ));

    qdev_prop_ಗಣ_uint8(DEVICE(&s->sdhci), "sd-spec-version", 3);
    qdev_prop_ಗಣ_uint64(DEVICE(&s->sdhci), "capareg", 0x056900b9);
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0, cv1800b_memmap[CV1800B_DEV_SD0].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
                       qdev_get_gpio_ಒಳ(DEVICE(s->plic), CV1800B_SD0_IRQ));

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->clk), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->clk), 0,
                    cv1800b_memmap[CV1800B_DEV_CLK].ತಳ);

    create_unimplemented_device("cv1800b.top_misc",
                                cv1800b_memmap[CV1800_B_DEV_ತುದಿ_MISC].ತಳ,
                                cv1800b_memmap[CV1800_B_DEV_ತುದಿ_MISC].ಗಾತ್ರ_2);

    create_unimplemented_device("cv1800b.pinmux",
                                cv1800b_memmap[CV1800B_DEV_PINMUX].ತಳ,
                                cv1800b_memmap[CV1800B_DEV_PINMUX].ಗಾತ್ರ_2);

    create_unimplemented_device("cv1800b.rst",
                                cv1800b_memmap[CV1800B_DEV_RST].ತಳ,
                                cv1800b_memmap[CV1800B_DEV_RST].ಗಾತ್ರ_2);

    create_unimplemented_device("cv1800b.wdt",
                                cv1800b_memmap[CV1800B_DEV_WDT].ತಳ,
                                cv1800b_memmap[CV1800B_DEV_WDT].ಗಾತ್ರ_2);

    create_unimplemented_device("cv1800b.gpio0_3",
                                cv1800b_memmap[CV1800B_DEV_GPIO].ತಳ,
                                cv1800b_memmap[CV1800B_DEV_GPIO].ಗಾತ್ರ_2);

    create_unimplemented_device("cv1800b.rtc_gpio",
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].ತಳ,
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].ಗಾತ್ರ_2);

    create_unimplemented_device("cv1800b.rtc_io",
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].ತಳ,
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ cv1800b_soc_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = cv1800b_soc_realize;
    dc->user_creatable = false;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo cv1800b_soc_type_info = {
    .name = TYPE_CV1800B_SOC,
    .ಪೋಷಕ = TYPE_DEVICE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(CV1800BSoCState),
    .instance_init = cv1800b_soc_instance_init,
    .class_init = cv1800b_soc_class_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ cv1800b_soc_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&cv1800b_soc_type_info);
}

type_init(cv1800b_soc_ರಿಜಿಸ್ಟರ್_types)
