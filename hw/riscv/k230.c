/*
 * QEMU RISC-V Virt Board Compatible with Kendryte K230 SDK
 *
 * Copyright (c) 2025 Chao Liu <chao.liu.zevorn@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Provides a board compatible with the Kendryte K230 SDK
 *
 * K230 Technical Reference Manual V0.3.1 (2024-11-18):
 * https://github.com/revyos/external-docs/blob/master/K230/en-us/K230_Technical_Reference_Manual_V0.3.1_20241118.pdf
 *
 * For more information, see <https://www.kendryte.com/en/proDetail/230>
 */

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "cpu-qom.h"
#అన్తర్భావ "qemu/cutils.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "system/system.h"
#అన్తర్భావ "system/memory.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "hw/riscv/k230.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/sifive_plic.h"
#అన్తర్భావ "hw/char/serial-mm.h"
#అన్తర్భావ "hw/misc/unimp.h"

/* Align K230_SDK k230_canmv_defconfig */
#నిర్వచించు K230_ప్రత్యక్ష_OPENSBI_ADDR 0x8000000
#నిర్వచించు K230_ప్రత్యక్ష_KERNEL_ADDR  0x8200000
#నిర్వచించు K230_ప్రత్యక్ష_DTB_ADDR     0xa000000

స్థిర స్థిరరాశి Mem_Map_నమోదు memmap[] = {
    [K230_DEV_DDRC] =         { 0x00000000,  0x80000000 },
    [K230_DEV_KPU_L2_CACHE] = { 0x80000000,  0x00200000 },
    [K230_DEV_SRAM] =         { 0x80200000,  0x00200000 },
    [K230_DEV_KPU_CFG] =      { 0x80400000,  0x00000800 },
    [K230_DEV_FFT] =          { 0x80400800,  0x00000400 },
    [K230_DEV_AI_2D_ENGINE] = { 0x80400C00,  0x00000800 },
    [K230_DEV_GSDMA] =        { 0x80800000,  0x00004000 },
    [K230_DEV_DMA] =          { 0x80804000,  0x00004000 },
    [K230_DEV_DECOMP_GZIP] =  { 0x80808000,  0x00004000 },
    [K230_DEV_NON_AI_2D] =    { 0x8080C000,  0x00004000 },
    [K230_DEV_ISP] =          { 0x90000000,  0x00008000 },
    [K230_DEV_DEWARP] =       { 0x90008000,  0x00001000 },
    [K230_DEV_RX_CSI] =       { 0x90009000,  0x00002000 },
    [K230_DEV_H264] =         { 0x90400000,  0x00010000 },
    [K230_DEV_2P5D] =         { 0x90800000,  0x00040000 },
    [K230_DEV_VO] =           { 0x90840000,  0x00010000 },
    [K230_DEV_VO_CFG] =       { 0x90850000,  0x00001000 },
    [K230_DEV_3D_ENGINE] =    { 0x90A00000,  0x00000800 },
    [K230_DEV_PMU] =          { 0x91000000,  0x00000C00 },
    [K230_DEV_RTC] =          { 0x91000C00,  0x00000400 },
    [K230_DEV_CMU] =          { 0x91100000,  0x00001000 },
    [K230_DEV_RMU] =          { 0x91101000,  0x00001000 },
    [K230_DEV_BOOT] =         { 0x91102000,  0x00001000 },
    [K230_DEV_PWR] =          { 0x91103000,  0x00001000 },
    [K230_DEV_MAILBOX] =      { 0x91104000,  0x00001000 },
    [K230_DEV_IOMUX] =        { 0x91105000,  0x00000800 },
    [K230_DEV_TIMER] =        { 0x91105800,  0x00000800 },
    [K230_DEV_WDT0] =         { 0x91106000,  0x00000800 },
    [K230_DEV_WDT1] =         { 0x91106800,  0x00000800 },
    [K230_DEV_TS] =           { 0x91107000,  0x00000800 },
    [K230_DEV_HDI] =          { 0x91107800,  0x00000800 },
    [K230_DEV_STC] =          { 0x91108000,  0x00000800 },
    [K230_DEV_BOOTROM] =      { 0x91200000,  0x00010000 },
    [K230_DEV_SECURITY] =     { 0x91210000,  0x00008000 },
    [K230_DEV_UART0] =        { 0x91400000,  0x00001000 },
    [K230_DEV_UART1] =        { 0x91401000,  0x00001000 },
    [K230_DEV_UART2] =        { 0x91402000,  0x00001000 },
    [K230_DEV_UART3] =        { 0x91403000,  0x00001000 },
    [K230_DEV_UART4] =        { 0x91404000,  0x00001000 },
    [K230_DEV_I2C0] =         { 0x91405000,  0x00001000 },
    [K230_DEV_I2C1] =         { 0x91406000,  0x00001000 },
    [K230_DEV_I2C2] =         { 0x91407000,  0x00001000 },
    [K230_DEV_I2C3] =         { 0x91408000,  0x00001000 },
    [K230_DEV_I2C4] =         { 0x91409000,  0x00001000 },
    [K230_DEV_PWM] =          { 0x9140A000,  0x00001000 },
    [K230_DEV_GPIO0] =        { 0x9140B000,  0x00001000 },
    [K230_DEV_GPIO1] =        { 0x9140C000,  0x00001000 },
    [K230_DEV_ADC] =          { 0x9140D000,  0x00001000 },
    [K230_DEV_CODEC] =        { 0x9140E000,  0x00001000 },
    [K230_DEV_I2S] =          { 0x9140F000,  0x00001000 },
    [K230_DEV_USB0] =         { 0x91500000,  0x00010000 },
    [K230_DEV_USB1] =         { 0x91540000,  0x00010000 },
    [K230_DEV_SD0] =          { 0x91580000,  0x00001000 },
    [K230_DEV_SD1] =          { 0x91581000,  0x00001000 },
    [K230_DEV_QSPI0] =        { 0x91582000,  0x00001000 },
    [K230_DEV_QSPI1] =        { 0x91583000,  0x00001000 },
    [K230_DEV_SPI] =          { 0x91584000,  0x00001000 },
    [K230_DEV_HI_SYS_CFG] =   { 0x91585000,  0x00000400 },
    [K230_DEV_DDRC_CFG] =     { 0x98000000,  0x02000000 },
    [K230_DEV_DDR_PHY] =      { 0x9A000000,  0x00400000 },
    [K230_DEV_FLASH] =        { 0xC0000000,  0x08000000 },
    [K230_DEV_PLIC] =         { 0xF00000000, 0x00400000 },
    [K230_DEV_CLINT] =        { 0xF04000000, 0x00400000 },
};

స్థిర శూన్య k230_soc_init(Object *obj)
{
    K230SoCState *s = RISCV_K230_SOC(obj);
    RISCVHartArrayState *cpu0 = &s->c908_cpu;

    object_initialize_బిడ్డ(obj, "c908-cpu", cpu0, TYPE_RISCV_HART_ARRAY);
    object_initialize_బిడ్డ(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_K230_DDR_CFG);
    object_initialize_బిడ్డ(obj, "ddr-phy", &s->ddr_phy,
                            TYPE_K230_DDR_PHY);
    s->ddr_cfg.phy = &s->ddr_phy;
    object_initialize_బిడ్డ(obj, "k230-wdt0", &s->wdt[0], TYPE_K230_WDT);
    object_initialize_బిడ్డ(obj, "k230-wdt1", &s->wdt[1], TYPE_K230_WDT);
    object_initialize_బిడ్డ(obj, "k230-gsdma", &s->gsdma, TYPE_K230_GSDMA);
    object_initialize_బిడ్డ(obj, "k230-decomp-gzip", &s->decomp_gzip,
                            TYPE_K230_DECOMP_GZIP);

    qdev_prop_సమితి_uint32(DEVICE(cpu0), "hartid-base", 0);
    qdev_prop_సమితి_string(DEVICE(cpu0), "cpu-type", TYPE_RISCV_CPU_THEAD_C908);
    qdev_prop_సమితి_uint64(DEVICE(cpu0), "resetvec",
                         memmap[K230_DEV_BOOTROM].పునాది);
}

స్థిర DeviceState *k230_create_plic(పూర్ణాంకం పునాది_hartid, పూర్ణాంకం hartid_లెక్క)
{
    g_autofree అక్షరం *plic_hart_config = ఖాళీ;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hartid_లెక్క);

    /* Per-socket PLIC */
    తిరిగి sifive_plic_create(get_system_జ్ఞాపకం(),
                              memmap[K230_DEV_PLIC].పునాది,
                              plic_hart_config, hartid_లెక్క, పునాది_hartid,
                              K230_PLIC_అంకె_SOURCES,
                              K230_PLIC_అంకె_PRIORITIES,
                              K230_PLIC_ప్రాధాన్యత_పునాది, K230_PLIC_PENDING_పునాది,
                              K230_PLIC_ENABLE_పునాది, K230_PLIC_ENABLE_STRIDE,
                              K230_PLIC_CONTEXT_పునాది,
                              K230_PLIC_CONTEXT_STRIDE,
                              memmap[K230_DEV_PLIC].కొలత);
}

స్థిర శూన్య k230_create_uart(జ్ఞాపకం_Region *sys_mem, DeviceState *plic,
                             పూర్ణాంకం సూచీ)
{
    పూర్ణాంకం uart_dev = K230_DEV_UART0 + సూచీ;
    g_autofree అక్షరం *name = g_strdup_printf("uart%d", సూచీ);

    /* Cover the non-16550 part of the SDK's 0x1000 UART window. */
    create_unimplemented_device(name, memmap[uart_dev].పునాది,
                                memmap[uart_dev].కొలత);

    serial_mm_init(sys_mem, memmap[uart_dev].పునాది, 2,
                   qdev_get_gpio_లోపల(plic, K230_UART0_IRQ + సూచీ),
                   399193, serial_hd(సూచీ), DEVICE_LITTLE_ENDIAN);
}

స్థిర శూన్య k230_soc_realize(DeviceState *dev, Error **errp)
{
    K230SoCState *s = RISCV_K230_SOC(dev);
    జ్ఞాపకం_Region *sys_mem = get_system_జ్ఞాపకం();
    పూర్ణాంకం c908_cpus;

    sysbus_realize(SYS_BUS_DEVICE(&s->c908_cpu), &error_fatal);
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp)) {
        తిరిగి;
    }
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_phy), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[K230_DEV_DDRC_CFG].పునాది);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_phy), 0,
                    memmap[K230_DEV_DDR_PHY].పునాది);

    c908_cpus = s->c908_cpu.అంకె_harts;

    /* SRAM */
    జ్ఞాపకం_region_init_ram(&s->sram, OBJECT(dev), "sram",
                           memmap[K230_DEV_SRAM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(sys_mem, memmap[K230_DEV_SRAM].పునాది,
                                &s->sram);

    /* BootROM */
    జ్ఞాపకం_region_init_rom(&s->bootrom, OBJECT(dev), "bootrom",
                           memmap[K230_DEV_BOOTROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(sys_mem, memmap[K230_DEV_BOOTROM].పునాది,
                                &s->bootrom);

    /* PLIC */
    s->c908_plic = k230_create_plic(C908_CPU_HARTID, c908_cpus);

    /* CLINT */
    riscv_aclint_swi_create(sys_mem,
                            memmap[K230_DEV_CLINT].పునాది,
                            C908_CPU_HARTID, c908_cpus, false);
    riscv_aclint_mtimer_create(sys_mem,
                               memmap[K230_DEV_CLINT].పునాది + 0x4000,
                               RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                               C908_CPU_HARTID, c908_cpus,
                               RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    /* UART */
    కొరకు (పూర్ణాంకం i = 0; i < K230_UART_లెక్క; i++) {
        k230_create_uart(sys_mem, DEVICE(s->c908_plic), i);
    }

    /* Watchdog */
    కొరకు (పూర్ణాంకం i = 0; i < 2; i++) {
        ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->wdt[i]), errp)) {
            తిరిగి;
        }
    }

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[0]), 0, memmap[K230_DEV_WDT0].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[0]), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->c908_plic), K230_WDT0_IRQ));

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[1]), 0, memmap[K230_DEV_WDT1].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[1]), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->c908_plic), K230_WDT1_IRQ));

    /* Gsdma */
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->gsdma), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gsdma), 0, memmap[K230_DEV_GSDMA].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gsdma), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->c908_plic), K230_GSDMA_IRQ));

    /* Decomp gzip */
    qdev_prop_సమితి_uint64(DEVICE(&s->decomp_gzip), "sram-base",
                         memmap[K230_DEV_SRAM].పునాది);
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->decomp_gzip), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->decomp_gzip), 0,
                    memmap[K230_DEV_DECOMP_GZIP].పునాది);
    కొరకు (పూర్ణాంకం i = 0; i < K230_DECOMP_GZIP_అంకె_GPIOS_బహిర్గతం; i++) {
        qdev_connect_gpio_బహిర్గతం(DEVICE(&s->decomp_gzip), i,
                              qdev_get_gpio_లోపల(DEVICE(&s->gsdma), i));
    }
    కొరకు (పూర్ణాంకం i = 0; i < K230_DECOMP_GZIP_అంకె_GPIOS_లోపల; i++) {
        qdev_connect_gpio_బహిర్గతం(DEVICE(&s->gsdma), i,
                              qdev_get_gpio_లోపల(DEVICE(&s->decomp_gzip), i));
    }

    /* Noc stub, the k230 decomp_gzip driver in uboot will access this region */
    create_unimplemented_device("noc-stub", 0x91302000, 0x1000);

    /* unimplemented devices */
    create_unimplemented_device("kpu.l2-cache",
                                memmap[K230_DEV_KPU_L2_CACHE].పునాది,
                                memmap[K230_DEV_KPU_L2_CACHE].కొలత);

    create_unimplemented_device("kpu_cfg", memmap[K230_DEV_KPU_CFG].పునాది,
                                memmap[K230_DEV_KPU_CFG].కొలత);

    create_unimplemented_device("fft", memmap[K230_DEV_FFT].పునాది,
                                memmap[K230_DEV_FFT].కొలత);

    create_unimplemented_device("2d-engine.ai",
                                memmap[K230_DEV_AI_2D_ENGINE].పునాది,
                                memmap[K230_DEV_AI_2D_ENGINE].కొలత);

    create_unimplemented_device("dma", memmap[K230_DEV_DMA].పునాది,
                                memmap[K230_DEV_DMA].కొలత);

    create_unimplemented_device("2d-engine.non-ai",
                                memmap[K230_DEV_NON_AI_2D].పునాది,
                                memmap[K230_DEV_NON_AI_2D].కొలత);

    create_unimplemented_device("isp", memmap[K230_DEV_ISP].పునాది,
                                memmap[K230_DEV_ISP].కొలత);

    create_unimplemented_device("dewarp", memmap[K230_DEV_DEWARP].పునాది,
                                memmap[K230_DEV_DEWARP].కొలత);

    create_unimplemented_device("rx-csi", memmap[K230_DEV_RX_CSI].పునాది,
                                memmap[K230_DEV_RX_CSI].కొలత);

    create_unimplemented_device("vpu", memmap[K230_DEV_H264].పునాది,
                                memmap[K230_DEV_H264].కొలత);

    create_unimplemented_device("gpu", memmap[K230_DEV_2P5D].పునాది,
                                memmap[K230_DEV_2P5D].కొలత);

    create_unimplemented_device("vo", memmap[K230_DEV_VO].పునాది,
                                memmap[K230_DEV_VO].కొలత);

    create_unimplemented_device("vo_cfg", memmap[K230_DEV_VO_CFG].పునాది,
                                memmap[K230_DEV_VO_CFG].కొలత);

    create_unimplemented_device("3d-engine", memmap[K230_DEV_3D_ENGINE].పునాది,
                                memmap[K230_DEV_3D_ENGINE].కొలత);

    create_unimplemented_device("pmu", memmap[K230_DEV_PMU].పునాది,
                                memmap[K230_DEV_PMU].కొలత);

    create_unimplemented_device("rtc", memmap[K230_DEV_RTC].పునాది,
                                memmap[K230_DEV_RTC].కొలత);

    create_unimplemented_device("cmu", memmap[K230_DEV_CMU].పునాది,
                                memmap[K230_DEV_CMU].కొలత);

    create_unimplemented_device("rmu", memmap[K230_DEV_RMU].పునాది,
                                memmap[K230_DEV_RMU].కొలత);

    create_unimplemented_device("boot", memmap[K230_DEV_BOOT].పునాది,
                                memmap[K230_DEV_BOOT].కొలత);

    create_unimplemented_device("pwr", memmap[K230_DEV_PWR].పునాది,
                                memmap[K230_DEV_PWR].కొలత);

    create_unimplemented_device("ipcm", memmap[K230_DEV_MAILBOX].పునాది,
                                memmap[K230_DEV_MAILBOX].కొలత);

    create_unimplemented_device("iomux", memmap[K230_DEV_IOMUX].పునాది,
                                memmap[K230_DEV_IOMUX].కొలత);

    create_unimplemented_device("timer", memmap[K230_DEV_TIMER].పునాది,
                                memmap[K230_DEV_TIMER].కొలత);

    create_unimplemented_device("wdt0", memmap[K230_DEV_WDT0].పునాది,
                                memmap[K230_DEV_WDT0].కొలత);

    create_unimplemented_device("wdt1", memmap[K230_DEV_WDT1].పునాది,
                                memmap[K230_DEV_WDT1].కొలత);

    create_unimplemented_device("ts", memmap[K230_DEV_TS].పునాది,
                                memmap[K230_DEV_TS].కొలత);

    create_unimplemented_device("hdi", memmap[K230_DEV_HDI].పునాది,
                                memmap[K230_DEV_HDI].కొలత);

    create_unimplemented_device("stc", memmap[K230_DEV_STC].పునాది,
                                memmap[K230_DEV_STC].కొలత);

    create_unimplemented_device("security", memmap[K230_DEV_SECURITY].పునాది,
                                memmap[K230_DEV_SECURITY].కొలత);

    create_unimplemented_device("i2c0", memmap[K230_DEV_I2C0].పునాది,
                                memmap[K230_DEV_I2C0].కొలత);

    create_unimplemented_device("i2c1", memmap[K230_DEV_I2C1].పునాది,
                                memmap[K230_DEV_I2C1].కొలత);

    create_unimplemented_device("i2c2", memmap[K230_DEV_I2C2].పునాది,
                                memmap[K230_DEV_I2C2].కొలత);

    create_unimplemented_device("i2c3", memmap[K230_DEV_I2C3].పునాది,
                                memmap[K230_DEV_I2C3].కొలత);

    create_unimplemented_device("i2c4", memmap[K230_DEV_I2C4].పునాది,
                                memmap[K230_DEV_I2C4].కొలత);

    create_unimplemented_device("pwm", memmap[K230_DEV_PWM].పునాది,
                                memmap[K230_DEV_PWM].కొలత);

    create_unimplemented_device("gpio0", memmap[K230_DEV_GPIO0].పునాది,
                                memmap[K230_DEV_GPIO0].కొలత);

    create_unimplemented_device("gpio1", memmap[K230_DEV_GPIO1].పునాది,
                                memmap[K230_DEV_GPIO1].కొలత);

    create_unimplemented_device("adc", memmap[K230_DEV_ADC].పునాది,
                                memmap[K230_DEV_ADC].కొలత);

    create_unimplemented_device("codec", memmap[K230_DEV_CODEC].పునాది,
                                memmap[K230_DEV_CODEC].కొలత);

    create_unimplemented_device("i2s", memmap[K230_DEV_I2S].పునాది,
                                memmap[K230_DEV_I2S].కొలత);

    create_unimplemented_device("usb0", memmap[K230_DEV_USB0].పునాది,
                                memmap[K230_DEV_USB0].కొలత);

    create_unimplemented_device("usb1", memmap[K230_DEV_USB1].పునాది,
                                memmap[K230_DEV_USB1].కొలత);

    create_unimplemented_device("sd0", memmap[K230_DEV_SD0].పునాది,
                                memmap[K230_DEV_SD0].కొలత);

    create_unimplemented_device("sd1", memmap[K230_DEV_SD1].పునాది,
                                memmap[K230_DEV_SD1].కొలత);

    create_unimplemented_device("qspi0", memmap[K230_DEV_QSPI0].పునాది,
                                memmap[K230_DEV_QSPI0].కొలత);

    create_unimplemented_device("qspi1", memmap[K230_DEV_QSPI1].పునాది,
                                memmap[K230_DEV_QSPI1].కొలత);

    create_unimplemented_device("spi", memmap[K230_DEV_SPI].పునాది,
                                memmap[K230_DEV_SPI].కొలత);

    create_unimplemented_device("hi_sys_cfg", memmap[K230_DEV_HI_SYS_CFG].పునాది,
                                memmap[K230_DEV_HI_SYS_CFG].కొలత);

    create_unimplemented_device("flash", memmap[K230_DEV_FLASH].పునాది,
                                memmap[K230_DEV_FLASH].కొలత);
}

స్థిర శూన్య k230_soc_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = k230_soc_realize;
}

స్థిర స్థిరరాశి TypeInfo k230_soc_type_info = {
    .name = TYPE_RISCV_K230_SOC,
    .మాతృ = TYPE_DEVICE,
    .instance_కొలత = పరిమాణం(K230SoCState),
    .instance_init = k230_soc_init,
    .class_init = k230_soc_class_init,
};

స్థిర శూన్య k230_soc_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&k230_soc_type_info);
}

type_init(k230_soc_రిజిస్టరు_types)

స్థిర శూన్య k230_ప్రత్యక్ష_boot(K230MachineState *s, MachineState *machine)
{
    స్థిరరాశి అక్షరం *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    RISCVBootInfo boot_info = {0};
    hwaddr ప్రారంభం_addr = K230_ప్రత్యక్ష_OPENSBI_ADDR;
    hwaddr firmware_చివర_addr = 0;
    hwaddr kernel_నమోదు = 0;
    పూర్ణాంకం fdt_కొలత = 0;

    ఒకవేళ (machine->firmware && !strcmp(machine->firmware, "none")) {
        error_report("K230 direct boot requires OpenSBI firmware; omit "
                     "-bios none or pass OpenSBI with -bios");
        నిష్క్రమించు(EXIT_FAILURE);
    }

    ఒకవేళ (!machine->dtb) {
        error_report("K230 direct boot requires -dtb");
        నిష్క్రమించు(EXIT_FAILURE);
    }

    machine->fdt = ఎక్కించు_device_వృక్షం(machine->dtb, &fdt_కొలత);
    ఒకవేళ (!machine->fdt) {
        error_report("load_device_tree() failed");
        నిష్క్రమించు(EXIT_FAILURE);
    }

    qemu_fdt_కూడు_మార్గం(machine->fdt, "/chosen");

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_ఎక్కించు_kernel(machine, &boot_info, K230_ప్రత్యక్ష_KERNEL_ADDR, true, ఖాళీ);
    kernel_నమోదు = boot_info.image_దిగువ_addr;

    riscv_ఎక్కించు_fdt(K230_ప్రత్యక్ష_DTB_ADDR, machine->fdt);

    firmware_చివర_addr = riscv_కనుగొను_and_ఎక్కించు_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ప్రారంభం_addr, ఖాళీ);
    ఒకవేళ (firmware_చివర_addr > K230_ప్రత్యక్ష_KERNEL_ADDR) {
        error_report("K230 firmware overlaps kernel address 0x%x",
                     K230_ప్రత్యక్ష_KERNEL_ADDR);
        నిష్క్రమించు(EXIT_FAILURE);
    }

    riscv_అమరిక_rom_reset_vec(machine, &s->soc.c908_cpu, ప్రారంభం_addr,
                              memmap[K230_DEV_BOOTROM].పునాది,
                              memmap[K230_DEV_BOOTROM].కొలత, kernel_నమోదు,
                              K230_ప్రత్యక్ష_DTB_ADDR);
}

స్థిర శూన్య k230_firmware_boot(K230MachineState *s, MachineState *machine)
{
    స్థిరరాశి అక్షరం *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    hwaddr ప్రారంభం_addr = memmap[K230_DEV_DDRC].పునాది;
    RISCVBootInfo boot_info = {0};

    ఒకవేళ (machine->dtb || (machine->kernel_cmdline && *machine->kernel_cmdline)) {
        error_report("K230 firmware boot does not support -dtb or -append");
        నిష్క్రమించు(EXIT_FAILURE);
    }

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_కనుగొను_and_ఎక్కించు_firmware(machine, &boot_info, firmware_name,
                                 &ప్రారంభం_addr, ఖాళీ);

    riscv_అమరిక_rom_reset_vec(machine, &s->soc.c908_cpu, ప్రారంభం_addr,
                              memmap[K230_DEV_BOOTROM].పునాది,
                              memmap[K230_DEV_BOOTROM].కొలత, 0, 0);
}

స్థిర శూన్య k230_machine_పూర్తి(Notifier *notifier, శూన్య *దత్తాంశం)
{
    K230MachineState *s = container_యొక్క(notifier, K230MachineState,
                                       machine_పూర్తి);
    MachineState *machine = MACHINE(s);

    ఒకవేళ (machine->kernel_filename) {
        k230_ప్రత్యక్ష_boot(s, machine);
    } లేకపోతే {
        k230_firmware_boot(s, machine);
    }
}

స్థిర శూన్య k230_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    K230MachineState *s = RISCV_K230_MACHINE(machine);
    జ్ఞాపకం_Region *sys_mem = get_system_జ్ఞాపకం();

    ఒకవేళ (machine->ram_కొలత < mc->default_ram_కొలత) {
        అక్షరం *sz = కొలత_గమ్యం_str(mc->default_ram_కొలత);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        నిష్క్రమించు(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_బిడ్డ(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_K230_SOC);
    qdev_realize(DEVICE(&s->soc), ఖాళీ, &error_fatal);

    /* Data Memory */
    జ్ఞాపకం_region_కూడు_subregion(sys_mem, memmap[K230_DEV_DDRC].పునాది,
                                machine->ram);

    s->machine_పూర్తి.notify = k230_machine_పూర్తి;
    qemu_కూడు_machine_init_పూర్తి_notifier(&s->machine_పూర్తి);
}

స్థిర శూన్య k230_machine_instance_init(Object *obj)
{
}

స్థిర శూన్య k230_machine_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with Kendryte K230 SDK";
    mc->init = k230_machine_init;
    mc->default_cpus = 1;
    mc->default_ram_id = "riscv.K230.ram"; /* DDR */
    mc->default_ram_కొలత = memmap[K230_DEV_DDRC].కొలత;
}

స్థిర స్థిరరాశి TypeInfo k230_machine_typeinfo = {
    .name       = TYPE_RISCV_K230_MACHINE,
    .మాతృ     = TYPE_MACHINE,
    .class_init = k230_machine_class_init,
    .instance_init = k230_machine_instance_init,
    .instance_కొలత = పరిమాణం(K230MachineState),
};

స్థిర శూన్య k230_machine_init_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&k230_machine_typeinfo);
}

type_init(k230_machine_init_రిజిస్టరు_types)
