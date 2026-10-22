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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "cpu-qom.h"
#ಅನ್ತರ್ಭಾವ "qemu/cutils.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "system/memory.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/k230.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/sifive_plic.h"
#ಅನ್ತರ್ಭಾವ "hw/char/serial-mm.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/unimp.h"

/* Align K230_SDK k230_canmv_defconfig */
#ವ್ಯಾಖ್ಯಾನಿಸು K230_ನೇರ_OPENSBI_ADDR 0x8000000
#ವ್ಯಾಖ್ಯಾನಿಸು K230_ನೇರ_KERNEL_ADDR  0x8200000
#ವ್ಯಾಖ್ಯಾನಿಸು K230_ನೇರ_DTB_ADDR     0xa000000

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು memmap[] = {
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

ಸ್ಥಿರ ಶೂನ್ಯ k230_soc_init(Object *obj)
{
    K230SoCState *s = RISCV_K230_SOC(obj);
    RISCVHartArrayState *cpu0 = &s->c908_cpu;

    object_initialize_ಮಗು(obj, "c908-cpu", cpu0, TYPE_RISCV_HART_ARRAY);
    object_initialize_ಮಗು(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_K230_DDR_CFG);
    object_initialize_ಮಗು(obj, "ddr-phy", &s->ddr_phy,
                            TYPE_K230_DDR_PHY);
    s->ddr_cfg.phy = &s->ddr_phy;
    object_initialize_ಮಗು(obj, "k230-wdt0", &s->wdt[0], TYPE_K230_WDT);
    object_initialize_ಮಗು(obj, "k230-wdt1", &s->wdt[1], TYPE_K230_WDT);
    object_initialize_ಮಗು(obj, "k230-gsdma", &s->gsdma, TYPE_K230_GSDMA);
    object_initialize_ಮಗು(obj, "k230-decomp-gzip", &s->decomp_gzip,
                            TYPE_K230_DECOMP_GZIP);

    qdev_prop_ಗಣ_uint32(DEVICE(cpu0), "hartid-base", 0);
    qdev_prop_ಗಣ_string(DEVICE(cpu0), "cpu-type", TYPE_RISCV_CPU_THEAD_C908);
    qdev_prop_ಗಣ_uint64(DEVICE(cpu0), "resetvec",
                         memmap[K230_DEV_BOOTROM].ತಳ);
}

ಸ್ಥಿರ DeviceState *k230_create_plic(ಪೂರ್ಣಾಂಕ ತಳ_hartid, ಪೂರ್ಣಾಂಕ hartid_ಎಣಿಕೆ)
{
    g_autofree ಅಕ್ಷರ *plic_hart_config = ಖಾಲಿ;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hartid_ಎಣಿಕೆ);

    /* Per-socket PLIC */
    ಹಿಂತಿರುಗಿ sifive_plic_create(get_system_ಸ್ಮರಣೆ(),
                              memmap[K230_DEV_PLIC].ತಳ,
                              plic_hart_config, hartid_ಎಣಿಕೆ, ತಳ_hartid,
                              K230_PLIC_ಅಂಕೆ_SOURCES,
                              K230_PLIC_ಅಂಕೆ_PRIORITIES,
                              K230_PLIC_ಆದ್ಯತೆ_ತಳ, K230_PLIC_PENDING_ತಳ,
                              K230_PLIC_ENABLE_ತಳ, K230_PLIC_ENABLE_STRIDE,
                              K230_PLIC_CONTEXT_ತಳ,
                              K230_PLIC_CONTEXT_STRIDE,
                              memmap[K230_DEV_PLIC].ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ k230_create_uart(ಸ್ಮರಣೆ_Region *sys_mem, DeviceState *plic,
                             ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
    ಪೂರ್ಣಾಂಕ uart_dev = K230_DEV_UART0 + ಸೂಚ್ಯಂಕ;
    g_autofree ಅಕ್ಷರ *name = g_strdup_printf("uart%d", ಸೂಚ್ಯಂಕ);

    /* Cover the non-16550 part of the SDK's 0x1000 UART window. */
    create_unimplemented_device(name, memmap[uart_dev].ತಳ,
                                memmap[uart_dev].ಗಾತ್ರ_2);

    serial_mm_init(sys_mem, memmap[uart_dev].ತಳ, 2,
                   qdev_get_gpio_ಒಳ(plic, K230_UART0_IRQ + ಸೂಚ್ಯಂಕ),
                   399193, serial_hd(ಸೂಚ್ಯಂಕ), DEVICE_LITTLE_ENDIAN);
}

ಸ್ಥಿರ ಶೂನ್ಯ k230_soc_realize(DeviceState *dev, Error **errp)
{
    K230SoCState *s = RISCV_K230_SOC(dev);
    ಸ್ಮರಣೆ_Region *sys_mem = get_system_ಸ್ಮರಣೆ();
    ಪೂರ್ಣಾಂಕ c908_cpus;

    sysbus_realize(SYS_BUS_DEVICE(&s->c908_cpu), &error_fatal);
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_phy), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[K230_DEV_DDRC_CFG].ತಳ);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_phy), 0,
                    memmap[K230_DEV_DDR_PHY].ತಳ);

    c908_cpus = s->c908_cpu.ಅಂಕೆ_harts;

    /* SRAM */
    ಸ್ಮರಣೆ_region_init_ram(&s->sram, OBJECT(dev), "sram",
                           memmap[K230_DEV_SRAM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem, memmap[K230_DEV_SRAM].ತಳ,
                                &s->sram);

    /* BootROM */
    ಸ್ಮರಣೆ_region_init_rom(&s->bootrom, OBJECT(dev), "bootrom",
                           memmap[K230_DEV_BOOTROM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem, memmap[K230_DEV_BOOTROM].ತಳ,
                                &s->bootrom);

    /* PLIC */
    s->c908_plic = k230_create_plic(C908_CPU_HARTID, c908_cpus);

    /* CLINT */
    riscv_aclint_swi_create(sys_mem,
                            memmap[K230_DEV_CLINT].ತಳ,
                            C908_CPU_HARTID, c908_cpus, false);
    riscv_aclint_mtimer_create(sys_mem,
                               memmap[K230_DEV_CLINT].ತಳ + 0x4000,
                               RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ,
                               C908_CPU_HARTID, c908_cpus,
                               RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    /* UART */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < K230_UART_ಎಣಿಕೆ; i++) {
        k230_create_uart(sys_mem, DEVICE(s->c908_plic), i);
    }

    /* Watchdog */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 2; i++) {
        ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->wdt[i]), errp)) {
            ಹಿಂತಿರುಗಿ;
        }
    }

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[0]), 0, memmap[K230_DEV_WDT0].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[0]), 0,
                       qdev_get_gpio_ಒಳ(DEVICE(s->c908_plic), K230_WDT0_IRQ));

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[1]), 0, memmap[K230_DEV_WDT1].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[1]), 0,
                       qdev_get_gpio_ಒಳ(DEVICE(s->c908_plic), K230_WDT1_IRQ));

    /* Gsdma */
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->gsdma), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gsdma), 0, memmap[K230_DEV_GSDMA].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gsdma), 0,
                       qdev_get_gpio_ಒಳ(DEVICE(s->c908_plic), K230_GSDMA_IRQ));

    /* Decomp gzip */
    qdev_prop_ಗಣ_uint64(DEVICE(&s->decomp_gzip), "sram-base",
                         memmap[K230_DEV_SRAM].ತಳ);
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->decomp_gzip), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->decomp_gzip), 0,
                    memmap[K230_DEV_DECOMP_GZIP].ತಳ);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < K230_DECOMP_GZIP_ಅಂಕೆ_GPIOS_ಹೊರಹೋಗು; i++) {
        qdev_connect_gpio_ಹೊರಹೋಗು(DEVICE(&s->decomp_gzip), i,
                              qdev_get_gpio_ಒಳ(DEVICE(&s->gsdma), i));
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < K230_DECOMP_GZIP_ಅಂಕೆ_GPIOS_ಒಳ; i++) {
        qdev_connect_gpio_ಹೊರಹೋಗು(DEVICE(&s->gsdma), i,
                              qdev_get_gpio_ಒಳ(DEVICE(&s->decomp_gzip), i));
    }

    /* Noc stub, the k230 decomp_gzip driver in uboot will access this region */
    create_unimplemented_device("noc-stub", 0x91302000, 0x1000);

    /* unimplemented devices */
    create_unimplemented_device("kpu.l2-cache",
                                memmap[K230_DEV_KPU_L2_CACHE].ತಳ,
                                memmap[K230_DEV_KPU_L2_CACHE].ಗಾತ್ರ_2);

    create_unimplemented_device("kpu_cfg", memmap[K230_DEV_KPU_CFG].ತಳ,
                                memmap[K230_DEV_KPU_CFG].ಗಾತ್ರ_2);

    create_unimplemented_device("fft", memmap[K230_DEV_FFT].ತಳ,
                                memmap[K230_DEV_FFT].ಗಾತ್ರ_2);

    create_unimplemented_device("2d-engine.ai",
                                memmap[K230_DEV_AI_2D_ENGINE].ತಳ,
                                memmap[K230_DEV_AI_2D_ENGINE].ಗಾತ್ರ_2);

    create_unimplemented_device("dma", memmap[K230_DEV_DMA].ತಳ,
                                memmap[K230_DEV_DMA].ಗಾತ್ರ_2);

    create_unimplemented_device("2d-engine.non-ai",
                                memmap[K230_DEV_NON_AI_2D].ತಳ,
                                memmap[K230_DEV_NON_AI_2D].ಗಾತ್ರ_2);

    create_unimplemented_device("isp", memmap[K230_DEV_ISP].ತಳ,
                                memmap[K230_DEV_ISP].ಗಾತ್ರ_2);

    create_unimplemented_device("dewarp", memmap[K230_DEV_DEWARP].ತಳ,
                                memmap[K230_DEV_DEWARP].ಗಾತ್ರ_2);

    create_unimplemented_device("rx-csi", memmap[K230_DEV_RX_CSI].ತಳ,
                                memmap[K230_DEV_RX_CSI].ಗಾತ್ರ_2);

    create_unimplemented_device("vpu", memmap[K230_DEV_H264].ತಳ,
                                memmap[K230_DEV_H264].ಗಾತ್ರ_2);

    create_unimplemented_device("gpu", memmap[K230_DEV_2P5D].ತಳ,
                                memmap[K230_DEV_2P5D].ಗಾತ್ರ_2);

    create_unimplemented_device("vo", memmap[K230_DEV_VO].ತಳ,
                                memmap[K230_DEV_VO].ಗಾತ್ರ_2);

    create_unimplemented_device("vo_cfg", memmap[K230_DEV_VO_CFG].ತಳ,
                                memmap[K230_DEV_VO_CFG].ಗಾತ್ರ_2);

    create_unimplemented_device("3d-engine", memmap[K230_DEV_3D_ENGINE].ತಳ,
                                memmap[K230_DEV_3D_ENGINE].ಗಾತ್ರ_2);

    create_unimplemented_device("pmu", memmap[K230_DEV_PMU].ತಳ,
                                memmap[K230_DEV_PMU].ಗಾತ್ರ_2);

    create_unimplemented_device("rtc", memmap[K230_DEV_RTC].ತಳ,
                                memmap[K230_DEV_RTC].ಗಾತ್ರ_2);

    create_unimplemented_device("cmu", memmap[K230_DEV_CMU].ತಳ,
                                memmap[K230_DEV_CMU].ಗಾತ್ರ_2);

    create_unimplemented_device("rmu", memmap[K230_DEV_RMU].ತಳ,
                                memmap[K230_DEV_RMU].ಗಾತ್ರ_2);

    create_unimplemented_device("boot", memmap[K230_DEV_BOOT].ತಳ,
                                memmap[K230_DEV_BOOT].ಗಾತ್ರ_2);

    create_unimplemented_device("pwr", memmap[K230_DEV_PWR].ತಳ,
                                memmap[K230_DEV_PWR].ಗಾತ್ರ_2);

    create_unimplemented_device("ipcm", memmap[K230_DEV_MAILBOX].ತಳ,
                                memmap[K230_DEV_MAILBOX].ಗಾತ್ರ_2);

    create_unimplemented_device("iomux", memmap[K230_DEV_IOMUX].ತಳ,
                                memmap[K230_DEV_IOMUX].ಗಾತ್ರ_2);

    create_unimplemented_device("timer", memmap[K230_DEV_TIMER].ತಳ,
                                memmap[K230_DEV_TIMER].ಗಾತ್ರ_2);

    create_unimplemented_device("wdt0", memmap[K230_DEV_WDT0].ತಳ,
                                memmap[K230_DEV_WDT0].ಗಾತ್ರ_2);

    create_unimplemented_device("wdt1", memmap[K230_DEV_WDT1].ತಳ,
                                memmap[K230_DEV_WDT1].ಗಾತ್ರ_2);

    create_unimplemented_device("ts", memmap[K230_DEV_TS].ತಳ,
                                memmap[K230_DEV_TS].ಗಾತ್ರ_2);

    create_unimplemented_device("hdi", memmap[K230_DEV_HDI].ತಳ,
                                memmap[K230_DEV_HDI].ಗಾತ್ರ_2);

    create_unimplemented_device("stc", memmap[K230_DEV_STC].ತಳ,
                                memmap[K230_DEV_STC].ಗಾತ್ರ_2);

    create_unimplemented_device("security", memmap[K230_DEV_SECURITY].ತಳ,
                                memmap[K230_DEV_SECURITY].ಗಾತ್ರ_2);

    create_unimplemented_device("i2c0", memmap[K230_DEV_I2C0].ತಳ,
                                memmap[K230_DEV_I2C0].ಗಾತ್ರ_2);

    create_unimplemented_device("i2c1", memmap[K230_DEV_I2C1].ತಳ,
                                memmap[K230_DEV_I2C1].ಗಾತ್ರ_2);

    create_unimplemented_device("i2c2", memmap[K230_DEV_I2C2].ತಳ,
                                memmap[K230_DEV_I2C2].ಗಾತ್ರ_2);

    create_unimplemented_device("i2c3", memmap[K230_DEV_I2C3].ತಳ,
                                memmap[K230_DEV_I2C3].ಗಾತ್ರ_2);

    create_unimplemented_device("i2c4", memmap[K230_DEV_I2C4].ತಳ,
                                memmap[K230_DEV_I2C4].ಗಾತ್ರ_2);

    create_unimplemented_device("pwm", memmap[K230_DEV_PWM].ತಳ,
                                memmap[K230_DEV_PWM].ಗಾತ್ರ_2);

    create_unimplemented_device("gpio0", memmap[K230_DEV_GPIO0].ತಳ,
                                memmap[K230_DEV_GPIO0].ಗಾತ್ರ_2);

    create_unimplemented_device("gpio1", memmap[K230_DEV_GPIO1].ತಳ,
                                memmap[K230_DEV_GPIO1].ಗಾತ್ರ_2);

    create_unimplemented_device("adc", memmap[K230_DEV_ADC].ತಳ,
                                memmap[K230_DEV_ADC].ಗಾತ್ರ_2);

    create_unimplemented_device("codec", memmap[K230_DEV_CODEC].ತಳ,
                                memmap[K230_DEV_CODEC].ಗಾತ್ರ_2);

    create_unimplemented_device("i2s", memmap[K230_DEV_I2S].ತಳ,
                                memmap[K230_DEV_I2S].ಗಾತ್ರ_2);

    create_unimplemented_device("usb0", memmap[K230_DEV_USB0].ತಳ,
                                memmap[K230_DEV_USB0].ಗಾತ್ರ_2);

    create_unimplemented_device("usb1", memmap[K230_DEV_USB1].ತಳ,
                                memmap[K230_DEV_USB1].ಗಾತ್ರ_2);

    create_unimplemented_device("sd0", memmap[K230_DEV_SD0].ತಳ,
                                memmap[K230_DEV_SD0].ಗಾತ್ರ_2);

    create_unimplemented_device("sd1", memmap[K230_DEV_SD1].ತಳ,
                                memmap[K230_DEV_SD1].ಗಾತ್ರ_2);

    create_unimplemented_device("qspi0", memmap[K230_DEV_QSPI0].ತಳ,
                                memmap[K230_DEV_QSPI0].ಗಾತ್ರ_2);

    create_unimplemented_device("qspi1", memmap[K230_DEV_QSPI1].ತಳ,
                                memmap[K230_DEV_QSPI1].ಗಾತ್ರ_2);

    create_unimplemented_device("spi", memmap[K230_DEV_SPI].ತಳ,
                                memmap[K230_DEV_SPI].ಗಾತ್ರ_2);

    create_unimplemented_device("hi_sys_cfg", memmap[K230_DEV_HI_SYS_CFG].ತಳ,
                                memmap[K230_DEV_HI_SYS_CFG].ಗಾತ್ರ_2);

    create_unimplemented_device("flash", memmap[K230_DEV_FLASH].ತಳ,
                                memmap[K230_DEV_FLASH].ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ k230_soc_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = k230_soc_realize;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo k230_soc_type_info = {
    .name = TYPE_RISCV_K230_SOC,
    .ಪೋಷಕ = TYPE_DEVICE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(K230SoCState),
    .instance_init = k230_soc_init,
    .class_init = k230_soc_class_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ k230_soc_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&k230_soc_type_info);
}

type_init(k230_soc_ರಿಜಿಸ್ಟರ್_types)

ಸ್ಥಿರ ಶೂನ್ಯ k230_ನೇರ_boot(K230MachineState *s, MachineState *machine)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    RISCVBootInfo boot_info = {0};
    hwaddr ಆರಂಭ_addr = K230_ನೇರ_OPENSBI_ADDR;
    hwaddr firmware_ಕೊನೆ_addr = 0;
    hwaddr kernel_ನಮೂದು = 0;
    ಪೂರ್ಣಾಂಕ fdt_ಗಾತ್ರ = 0;

    ಆದರೆ (machine->firmware && !strcmp(machine->firmware, "none")) {
        error_report("K230 direct boot requires OpenSBI firmware; omit "
                     "-bios none or pass OpenSBI with -bios");
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    ಆದರೆ (!machine->dtb) {
        error_report("K230 direct boot requires -dtb");
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    machine->fdt = ತುಂಬು_device_ವೃಕ್ಷ(machine->dtb, &fdt_ಗಾತ್ರ);
    ಆದರೆ (!machine->fdt) {
        error_report("load_device_tree() failed");
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    qemu_fdt_ಕೂಡಿಸು_ಮಾರ್ಗ(machine->fdt, "/chosen");

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_ತುಂಬು_kernel(machine, &boot_info, K230_ನೇರ_KERNEL_ADDR, true, ಖಾಲಿ);
    kernel_ನಮೂದು = boot_info.image_ಕೆಳಗಿನ_addr;

    riscv_ತುಂಬು_fdt(K230_ನೇರ_DTB_ADDR, machine->fdt);

    firmware_ಕೊನೆ_addr = riscv_ಕಂಡುಹಿಡಿ_and_ತುಂಬು_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ಆರಂಭ_addr, ಖಾಲಿ);
    ಆದರೆ (firmware_ಕೊನೆ_addr > K230_ನೇರ_KERNEL_ADDR) {
        error_report("K230 firmware overlaps kernel address 0x%x",
                     K230_ನೇರ_KERNEL_ADDR);
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    riscv_ಸಿದ್ಧತೆ_rom_reset_vec(machine, &s->soc.c908_cpu, ಆರಂಭ_addr,
                              memmap[K230_DEV_BOOTROM].ತಳ,
                              memmap[K230_DEV_BOOTROM].ಗಾತ್ರ_2, kernel_ನಮೂದು,
                              K230_ನೇರ_DTB_ADDR);
}

ಸ್ಥಿರ ಶೂನ್ಯ k230_firmware_boot(K230MachineState *s, MachineState *machine)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    hwaddr ಆರಂಭ_addr = memmap[K230_DEV_DDRC].ತಳ;
    RISCVBootInfo boot_info = {0};

    ಆದರೆ (machine->dtb || (machine->kernel_cmdline && *machine->kernel_cmdline)) {
        error_report("K230 firmware boot does not support -dtb or -append");
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_ಕಂಡುಹಿಡಿ_and_ತುಂಬು_firmware(machine, &boot_info, firmware_name,
                                 &ಆರಂಭ_addr, ಖಾಲಿ);

    riscv_ಸಿದ್ಧತೆ_rom_reset_vec(machine, &s->soc.c908_cpu, ಆರಂಭ_addr,
                              memmap[K230_DEV_BOOTROM].ತಳ,
                              memmap[K230_DEV_BOOTROM].ಗಾತ್ರ_2, 0, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ k230_machine_ಮುಗಿದ(Notifier *notifier, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    K230MachineState *s = container_ದ(notifier, K230MachineState,
                                       machine_ಮುಗಿದ);
    MachineState *machine = MACHINE(s);

    ಆದರೆ (machine->kernel_filename) {
        k230_ನೇರ_boot(s, machine);
    } ಇಲ್ಲದಿದ್ದರೆ {
        k230_firmware_boot(s, machine);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ k230_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    K230MachineState *s = RISCV_K230_MACHINE(machine);
    ಸ್ಮರಣೆ_Region *sys_mem = get_system_ಸ್ಮರಣೆ();

    ಆದರೆ (machine->ram_ಗಾತ್ರ < mc->default_ram_ಗಾತ್ರ) {
        ಅಕ್ಷರ *sz = ಗಾತ್ರ_ಗಮ್ಯ_str(mc->default_ram_ಗಾತ್ರ);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_ಮಗು(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_K230_SOC);
    qdev_realize(DEVICE(&s->soc), ಖಾಲಿ, &error_fatal);

    /* Data Memory */
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem, memmap[K230_DEV_DDRC].ತಳ,
                                machine->ram);

    s->machine_ಮುಗಿದ.notify = k230_machine_ಮುಗಿದ;
    qemu_ಕೂಡಿಸು_machine_init_ಮುಗಿದ_notifier(&s->machine_ಮುಗಿದ);
}

ಸ್ಥಿರ ಶೂನ್ಯ k230_machine_instance_init(Object *obj)
{
}

ಸ್ಥಿರ ಶೂನ್ಯ k230_machine_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with Kendryte K230 SDK";
    mc->init = k230_machine_init;
    mc->default_cpus = 1;
    mc->default_ram_id = "riscv.K230.ram"; /* DDR */
    mc->default_ram_ಗಾತ್ರ = memmap[K230_DEV_DDRC].ಗಾತ್ರ_2;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo k230_machine_typeinfo = {
    .name       = TYPE_RISCV_K230_MACHINE,
    .ಪೋಷಕ     = TYPE_MACHINE,
    .class_init = k230_machine_class_init,
    .instance_init = k230_machine_instance_init,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(K230MachineState),
};

ಸ್ಥಿರ ಶೂನ್ಯ k230_machine_init_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&k230_machine_typeinfo);
}

type_init(k230_machine_init_ರಿಜಿಸ್ಟರ್_types)
