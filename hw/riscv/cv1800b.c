/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Sophgo CV1800B SoC
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "hw/riscv/cv1800b.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "target/riscv/cpu-qom.h"
#అన్తర్భావ "system/system.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "system/address-spaces.h"
#అన్తర్భావ "hw/intc/sifive_plic.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/sd/sdhci.h"
#అన్తర్భావ "hw/misc/unimp.h"

స్థిరరాశి Mem_Map_నమోదు cv1800b_memmap[] = {
    [CV1800_B_DEV_శిఖరం_MISC]     = { 0x03000000,      0x1000 },
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

స్థిర శూన్య cv1800b_soc_instance_init(Object *obj)
{
    CV1800BSoCState *s = CV1800B_SOC(obj);

    object_initialize_బిడ్డ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    qdev_prop_సమితి_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_సమితి_string(DEVICE(&s->cpus), "cpu-type", TYPE_RISCV_CPU_THEAD_C906);
    qdev_prop_సమితి_uint64(DEVICE(&s->cpus), "resetvec",
                         cv1800b_memmap[CV1800B_DEV_ROM].పునాది);

    object_initialize_బిడ్డ(obj, "clk", &s->clk, TYPE_CV1800B_CLK);
    object_initialize_బిడ్డ(obj, "uart", &s->uart, TYPE_DW8250);
    object_initialize_బిడ్డ(obj, "sdhci", &s->sdhci, TYPE_SYSBUS_SDHCI);
}

స్థిర శూన్య cv1800b_soc_realize(DeviceState *dev, Error **errp)
{
    CV1800BSoCState *s = CV1800B_SOC(dev);
    MachineState *ms = MACHINE(qdev_get_machine());
    uint32_t అంకె_harts = ms->smp.cpus;
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    అక్షరం *plic_hart_config;

    qdev_prop_సమితి_uint32(DEVICE(&s->cpus), "num-harts", అంకె_harts);

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        తిరిగి;
    }

    జ్ఞాపకం_region_init_rom(&s->rom, OBJECT(dev), "cv1800b.rom",
                           cv1800b_memmap[CV1800B_DEV_ROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                cv1800b_memmap[CV1800B_DEV_ROM].పునాది, &s->rom);

    riscv_aclint_swi_create(system_జ్ఞాపకం,
                            cv1800b_memmap[CV1800B_DEV_CLINT].పునాది,
                            0, అంకె_harts, false);
    riscv_aclint_mtimer_create(system_జ్ఞాపకం,
                               cv1800b_memmap[CV1800B_DEV_CLINT].పునాది +
                               RISCV_ACLINT_SWI_కొలత,
                               RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                               0, అంకె_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    plic_hart_config = riscv_plic_hart_config_string(అంకె_harts);
    s->plic = sifive_plic_create(
        system_జ్ఞాపకం,
        cv1800b_memmap[CV1800B_DEV_PLIC].పునాది,
        plic_hart_config,
        అంకె_harts,
        0,
        CV1800_B_PLIC_అంకె_SOURCES,
        CV1800_B_PLIC_అంకె_PRIORITIES,
        0x0,
        0x1000,
        0x2000,
        0x80,
        0x200000,
        0x1000,
        cv1800b_memmap[CV1800B_DEV_PLIC].కొలత);

    g_free(plic_hart_config);

    qdev_prop_సమితి_uint8(DEVICE(&s->uart), "regshift", 2);
    qdev_prop_సమితి_chr(DEVICE(&s->uart), "chardev", serial_hd(0));
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, cv1800b_memmap[CV1800B_DEV_UART0].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
                    qdev_get_gpio_లోపల(DEVICE(s->plic), CV1800B_UART0_IRQ));

    qdev_prop_సమితి_uint8(DEVICE(&s->sdhci), "sd-spec-version", 3);
    qdev_prop_సమితి_uint64(DEVICE(&s->sdhci), "capareg", 0x056900b9);
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0, cv1800b_memmap[CV1800B_DEV_SD0].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->plic), CV1800B_SD0_IRQ));

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->clk), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->clk), 0,
                    cv1800b_memmap[CV1800B_DEV_CLK].పునాది);

    create_unimplemented_device("cv1800b.top_misc",
                                cv1800b_memmap[CV1800_B_DEV_శిఖరం_MISC].పునాది,
                                cv1800b_memmap[CV1800_B_DEV_శిఖరం_MISC].కొలత);

    create_unimplemented_device("cv1800b.pinmux",
                                cv1800b_memmap[CV1800B_DEV_PINMUX].పునాది,
                                cv1800b_memmap[CV1800B_DEV_PINMUX].కొలత);

    create_unimplemented_device("cv1800b.rst",
                                cv1800b_memmap[CV1800B_DEV_RST].పునాది,
                                cv1800b_memmap[CV1800B_DEV_RST].కొలత);

    create_unimplemented_device("cv1800b.wdt",
                                cv1800b_memmap[CV1800B_DEV_WDT].పునాది,
                                cv1800b_memmap[CV1800B_DEV_WDT].కొలత);

    create_unimplemented_device("cv1800b.gpio0_3",
                                cv1800b_memmap[CV1800B_DEV_GPIO].పునాది,
                                cv1800b_memmap[CV1800B_DEV_GPIO].కొలత);

    create_unimplemented_device("cv1800b.rtc_gpio",
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].పునాది,
                                cv1800b_memmap[CV1800B_DEV_RTC_GPIO].కొలత);

    create_unimplemented_device("cv1800b.rtc_io",
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].పునాది,
                                cv1800b_memmap[CV1800B_DEV_RTC_IO].కొలత);
}

స్థిర శూన్య cv1800b_soc_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = cv1800b_soc_realize;
    dc->user_creatable = false;
}

స్థిర స్థిరరాశి TypeInfo cv1800b_soc_type_info = {
    .name = TYPE_CV1800B_SOC,
    .మాతృ = TYPE_DEVICE,
    .instance_కొలత = పరిమాణం(CV1800BSoCState),
    .instance_init = cv1800b_soc_instance_init,
    .class_init = cv1800b_soc_class_init,
};

స్థిర శూన్య cv1800b_soc_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&cv1800b_soc_type_info);
}

type_init(cv1800b_soc_రిజిస్టరు_types)
