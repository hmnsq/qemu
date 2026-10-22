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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cpu-qom.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/cutils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/memory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/loader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/k230.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/sifive_plic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/char/serial-mm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/misc/unimp.h"

/* Align K230_SDK k230_canmv_defconfig */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K230_ਸਿੱਧਾ_OPENSBI_ADDR 0x8000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K230_ਸਿੱਧਾ_KERNEL_ADDR  0x8200000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K230_ਸਿੱਧਾ_DTB_ADDR     0xa000000

ਸਥਿਰ ਅਚਲ Mem_Map_ਇੰਦਰਾਜ memmap[] = {
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

ਸਥਿਰ ਖਾਲੀ k230_soc_init(Object *obj)
{
    K230SoCState *s = RISCV_K230_SOC(obj);
    RISCVHartArrayState *cpu0 = &s->c908_cpu;

    object_initialize_ਬੱਚਾ(obj, "c908-cpu", cpu0, TYPE_RISCV_HART_ARRAY);
    object_initialize_ਬੱਚਾ(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_K230_DDR_CFG);
    object_initialize_ਬੱਚਾ(obj, "ddr-phy", &s->ddr_phy,
                            TYPE_K230_DDR_PHY);
    s->ddr_cfg.phy = &s->ddr_phy;
    object_initialize_ਬੱਚਾ(obj, "k230-wdt0", &s->wdt[0], TYPE_K230_WDT);
    object_initialize_ਬੱਚਾ(obj, "k230-wdt1", &s->wdt[1], TYPE_K230_WDT);
    object_initialize_ਬੱਚਾ(obj, "k230-gsdma", &s->gsdma, TYPE_K230_GSDMA);
    object_initialize_ਬੱਚਾ(obj, "k230-decomp-gzip", &s->decomp_gzip,
                            TYPE_K230_DECOMP_GZIP);

    qdev_prop_ਸੈੱਟ_uint32(DEVICE(cpu0), "hartid-base", 0);
    qdev_prop_ਸੈੱਟ_string(DEVICE(cpu0), "cpu-type", TYPE_RISCV_CPU_THEAD_C908);
    qdev_prop_ਸੈੱਟ_uint64(DEVICE(cpu0), "resetvec",
                         memmap[K230_DEV_BOOTROM].ਅਧਾਰ);
}

ਸਥਿਰ DeviceState *k230_create_plic(ਪੂਰਨ ਅਧਾਰ_hartid, ਪੂਰਨ hartid_ਗਿਣਤੀ)
{
    g_autofree ਅੱਖਰ *plic_hart_config = ਨਿਰਾ;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hartid_ਗਿਣਤੀ);

    /* Per-socket PLIC */
    ਵਾਪਸ sifive_plic_create(get_system_ਸਮ੍ਰਿਤੀ(),
                              memmap[K230_DEV_PLIC].ਅਧਾਰ,
                              plic_hart_config, hartid_ਗਿਣਤੀ, ਅਧਾਰ_hartid,
                              K230_PLIC_ਅੰਕ_SOURCES,
                              K230_PLIC_ਅੰਕ_PRIORITIES,
                              K230_PLIC_ਤਰਜੀਹ_ਅਧਾਰ, K230_PLIC_PENDING_ਅਧਾਰ,
                              K230_PLIC_ENABLE_ਅਧਾਰ, K230_PLIC_ENABLE_STRIDE,
                              K230_PLIC_CONTEXT_ਅਧਾਰ,
                              K230_PLIC_CONTEXT_STRIDE,
                              memmap[K230_DEV_PLIC].ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ k230_create_uart(ਸਮ੍ਰਿਤੀ_Region *sys_mem, DeviceState *plic,
                             ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
    ਪੂਰਨ uart_dev = K230_DEV_UART0 + ਸੂਚਕਾਂਕ;
    g_autofree ਅੱਖਰ *name = g_strdup_printf("uart%d", ਸੂਚਕਾਂਕ);

    /* Cover the non-16550 part of the SDK's 0x1000 UART window. */
    create_unimplemented_device(name, memmap[uart_dev].ਅਧਾਰ,
                                memmap[uart_dev].ਮਾਪ);

    serial_mm_init(sys_mem, memmap[uart_dev].ਅਧਾਰ, 2,
                   qdev_get_gpio_ਵਿੱਚ(plic, K230_UART0_IRQ + ਸੂਚਕਾਂਕ),
                   399193, serial_hd(ਸੂਚਕਾਂਕ), DEVICE_LITTLE_ENDIAN);
}

ਸਥਿਰ ਖਾਲੀ k230_soc_realize(DeviceState *dev, Error **errp)
{
    K230SoCState *s = RISCV_K230_SOC(dev);
    ਸਮ੍ਰਿਤੀ_Region *sys_mem = get_system_ਸਮ੍ਰਿਤੀ();
    ਪੂਰਨ c908_cpus;

    sysbus_realize(SYS_BUS_DEVICE(&s->c908_cpu), &error_fatal);
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp)) {
        ਵਾਪਸ;
    }
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_phy), errp)) {
        ਵਾਪਸ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[K230_DEV_DDRC_CFG].ਅਧਾਰ);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_phy), 0,
                    memmap[K230_DEV_DDR_PHY].ਅਧਾਰ);

    c908_cpus = s->c908_cpu.ਅੰਕ_harts;

    /* SRAM */
    ਸਮ੍ਰਿਤੀ_region_init_ram(&s->sram, OBJECT(dev), "sram",
                           memmap[K230_DEV_SRAM].ਮਾਪ, &error_fatal);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(sys_mem, memmap[K230_DEV_SRAM].ਅਧਾਰ,
                                &s->sram);

    /* BootROM */
    ਸਮ੍ਰਿਤੀ_region_init_rom(&s->bootrom, OBJECT(dev), "bootrom",
                           memmap[K230_DEV_BOOTROM].ਮਾਪ, &error_fatal);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(sys_mem, memmap[K230_DEV_BOOTROM].ਅਧਾਰ,
                                &s->bootrom);

    /* PLIC */
    s->c908_plic = k230_create_plic(C908_CPU_HARTID, c908_cpus);

    /* CLINT */
    riscv_aclint_swi_create(sys_mem,
                            memmap[K230_DEV_CLINT].ਅਧਾਰ,
                            C908_CPU_HARTID, c908_cpus, false);
    riscv_aclint_mtimer_create(sys_mem,
                               memmap[K230_DEV_CLINT].ਅਧਾਰ + 0x4000,
                               RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ,
                               C908_CPU_HARTID, c908_cpus,
                               RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    /* UART */
    ਲਈ (ਪੂਰਨ i = 0; i < K230_UART_ਗਿਣਤੀ; i++) {
        k230_create_uart(sys_mem, DEVICE(s->c908_plic), i);
    }

    /* Watchdog */
    ਲਈ (ਪੂਰਨ i = 0; i < 2; i++) {
        ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->wdt[i]), errp)) {
            ਵਾਪਸ;
        }
    }

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[0]), 0, memmap[K230_DEV_WDT0].ਅਧਾਰ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[0]), 0,
                       qdev_get_gpio_ਵਿੱਚ(DEVICE(s->c908_plic), K230_WDT0_IRQ));

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[1]), 0, memmap[K230_DEV_WDT1].ਅਧਾਰ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[1]), 0,
                       qdev_get_gpio_ਵਿੱਚ(DEVICE(s->c908_plic), K230_WDT1_IRQ));

    /* Gsdma */
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->gsdma), errp)) {
        ਵਾਪਸ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gsdma), 0, memmap[K230_DEV_GSDMA].ਅਧਾਰ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gsdma), 0,
                       qdev_get_gpio_ਵਿੱਚ(DEVICE(s->c908_plic), K230_GSDMA_IRQ));

    /* Decomp gzip */
    qdev_prop_ਸੈੱਟ_uint64(DEVICE(&s->decomp_gzip), "sram-base",
                         memmap[K230_DEV_SRAM].ਅਧਾਰ);
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->decomp_gzip), errp)) {
        ਵਾਪਸ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->decomp_gzip), 0,
                    memmap[K230_DEV_DECOMP_GZIP].ਅਧਾਰ);
    ਲਈ (ਪੂਰਨ i = 0; i < K230_DECOMP_GZIP_ਅੰਕ_GPIOS_ਬਾਹਰਲਾ; i++) {
        qdev_connect_gpio_ਬਾਹਰਲਾ(DEVICE(&s->decomp_gzip), i,
                              qdev_get_gpio_ਵਿੱਚ(DEVICE(&s->gsdma), i));
    }
    ਲਈ (ਪੂਰਨ i = 0; i < K230_DECOMP_GZIP_ਅੰਕ_GPIOS_ਵਿੱਚ; i++) {
        qdev_connect_gpio_ਬਾਹਰਲਾ(DEVICE(&s->gsdma), i,
                              qdev_get_gpio_ਵਿੱਚ(DEVICE(&s->decomp_gzip), i));
    }

    /* Noc stub, the k230 decomp_gzip driver in uboot will access this region */
    create_unimplemented_device("noc-stub", 0x91302000, 0x1000);

    /* unimplemented devices */
    create_unimplemented_device("kpu.l2-cache",
                                memmap[K230_DEV_KPU_L2_CACHE].ਅਧਾਰ,
                                memmap[K230_DEV_KPU_L2_CACHE].ਮਾਪ);

    create_unimplemented_device("kpu_cfg", memmap[K230_DEV_KPU_CFG].ਅਧਾਰ,
                                memmap[K230_DEV_KPU_CFG].ਮਾਪ);

    create_unimplemented_device("fft", memmap[K230_DEV_FFT].ਅਧਾਰ,
                                memmap[K230_DEV_FFT].ਮਾਪ);

    create_unimplemented_device("2d-engine.ai",
                                memmap[K230_DEV_AI_2D_ENGINE].ਅਧਾਰ,
                                memmap[K230_DEV_AI_2D_ENGINE].ਮਾਪ);

    create_unimplemented_device("dma", memmap[K230_DEV_DMA].ਅਧਾਰ,
                                memmap[K230_DEV_DMA].ਮਾਪ);

    create_unimplemented_device("2d-engine.non-ai",
                                memmap[K230_DEV_NON_AI_2D].ਅਧਾਰ,
                                memmap[K230_DEV_NON_AI_2D].ਮਾਪ);

    create_unimplemented_device("isp", memmap[K230_DEV_ISP].ਅਧਾਰ,
                                memmap[K230_DEV_ISP].ਮਾਪ);

    create_unimplemented_device("dewarp", memmap[K230_DEV_DEWARP].ਅਧਾਰ,
                                memmap[K230_DEV_DEWARP].ਮਾਪ);

    create_unimplemented_device("rx-csi", memmap[K230_DEV_RX_CSI].ਅਧਾਰ,
                                memmap[K230_DEV_RX_CSI].ਮਾਪ);

    create_unimplemented_device("vpu", memmap[K230_DEV_H264].ਅਧਾਰ,
                                memmap[K230_DEV_H264].ਮਾਪ);

    create_unimplemented_device("gpu", memmap[K230_DEV_2P5D].ਅਧਾਰ,
                                memmap[K230_DEV_2P5D].ਮਾਪ);

    create_unimplemented_device("vo", memmap[K230_DEV_VO].ਅਧਾਰ,
                                memmap[K230_DEV_VO].ਮਾਪ);

    create_unimplemented_device("vo_cfg", memmap[K230_DEV_VO_CFG].ਅਧਾਰ,
                                memmap[K230_DEV_VO_CFG].ਮਾਪ);

    create_unimplemented_device("3d-engine", memmap[K230_DEV_3D_ENGINE].ਅਧਾਰ,
                                memmap[K230_DEV_3D_ENGINE].ਮਾਪ);

    create_unimplemented_device("pmu", memmap[K230_DEV_PMU].ਅਧਾਰ,
                                memmap[K230_DEV_PMU].ਮਾਪ);

    create_unimplemented_device("rtc", memmap[K230_DEV_RTC].ਅਧਾਰ,
                                memmap[K230_DEV_RTC].ਮਾਪ);

    create_unimplemented_device("cmu", memmap[K230_DEV_CMU].ਅਧਾਰ,
                                memmap[K230_DEV_CMU].ਮਾਪ);

    create_unimplemented_device("rmu", memmap[K230_DEV_RMU].ਅਧਾਰ,
                                memmap[K230_DEV_RMU].ਮਾਪ);

    create_unimplemented_device("boot", memmap[K230_DEV_BOOT].ਅਧਾਰ,
                                memmap[K230_DEV_BOOT].ਮਾਪ);

    create_unimplemented_device("pwr", memmap[K230_DEV_PWR].ਅਧਾਰ,
                                memmap[K230_DEV_PWR].ਮਾਪ);

    create_unimplemented_device("ipcm", memmap[K230_DEV_MAILBOX].ਅਧਾਰ,
                                memmap[K230_DEV_MAILBOX].ਮਾਪ);

    create_unimplemented_device("iomux", memmap[K230_DEV_IOMUX].ਅਧਾਰ,
                                memmap[K230_DEV_IOMUX].ਮਾਪ);

    create_unimplemented_device("timer", memmap[K230_DEV_TIMER].ਅਧਾਰ,
                                memmap[K230_DEV_TIMER].ਮਾਪ);

    create_unimplemented_device("wdt0", memmap[K230_DEV_WDT0].ਅਧਾਰ,
                                memmap[K230_DEV_WDT0].ਮਾਪ);

    create_unimplemented_device("wdt1", memmap[K230_DEV_WDT1].ਅਧਾਰ,
                                memmap[K230_DEV_WDT1].ਮਾਪ);

    create_unimplemented_device("ts", memmap[K230_DEV_TS].ਅਧਾਰ,
                                memmap[K230_DEV_TS].ਮਾਪ);

    create_unimplemented_device("hdi", memmap[K230_DEV_HDI].ਅਧਾਰ,
                                memmap[K230_DEV_HDI].ਮਾਪ);

    create_unimplemented_device("stc", memmap[K230_DEV_STC].ਅਧਾਰ,
                                memmap[K230_DEV_STC].ਮਾਪ);

    create_unimplemented_device("security", memmap[K230_DEV_SECURITY].ਅਧਾਰ,
                                memmap[K230_DEV_SECURITY].ਮਾਪ);

    create_unimplemented_device("i2c0", memmap[K230_DEV_I2C0].ਅਧਾਰ,
                                memmap[K230_DEV_I2C0].ਮਾਪ);

    create_unimplemented_device("i2c1", memmap[K230_DEV_I2C1].ਅਧਾਰ,
                                memmap[K230_DEV_I2C1].ਮਾਪ);

    create_unimplemented_device("i2c2", memmap[K230_DEV_I2C2].ਅਧਾਰ,
                                memmap[K230_DEV_I2C2].ਮਾਪ);

    create_unimplemented_device("i2c3", memmap[K230_DEV_I2C3].ਅਧਾਰ,
                                memmap[K230_DEV_I2C3].ਮਾਪ);

    create_unimplemented_device("i2c4", memmap[K230_DEV_I2C4].ਅਧਾਰ,
                                memmap[K230_DEV_I2C4].ਮਾਪ);

    create_unimplemented_device("pwm", memmap[K230_DEV_PWM].ਅਧਾਰ,
                                memmap[K230_DEV_PWM].ਮਾਪ);

    create_unimplemented_device("gpio0", memmap[K230_DEV_GPIO0].ਅਧਾਰ,
                                memmap[K230_DEV_GPIO0].ਮਾਪ);

    create_unimplemented_device("gpio1", memmap[K230_DEV_GPIO1].ਅਧਾਰ,
                                memmap[K230_DEV_GPIO1].ਮਾਪ);

    create_unimplemented_device("adc", memmap[K230_DEV_ADC].ਅਧਾਰ,
                                memmap[K230_DEV_ADC].ਮਾਪ);

    create_unimplemented_device("codec", memmap[K230_DEV_CODEC].ਅਧਾਰ,
                                memmap[K230_DEV_CODEC].ਮਾਪ);

    create_unimplemented_device("i2s", memmap[K230_DEV_I2S].ਅਧਾਰ,
                                memmap[K230_DEV_I2S].ਮਾਪ);

    create_unimplemented_device("usb0", memmap[K230_DEV_USB0].ਅਧਾਰ,
                                memmap[K230_DEV_USB0].ਮਾਪ);

    create_unimplemented_device("usb1", memmap[K230_DEV_USB1].ਅਧਾਰ,
                                memmap[K230_DEV_USB1].ਮਾਪ);

    create_unimplemented_device("sd0", memmap[K230_DEV_SD0].ਅਧਾਰ,
                                memmap[K230_DEV_SD0].ਮਾਪ);

    create_unimplemented_device("sd1", memmap[K230_DEV_SD1].ਅਧਾਰ,
                                memmap[K230_DEV_SD1].ਮਾਪ);

    create_unimplemented_device("qspi0", memmap[K230_DEV_QSPI0].ਅਧਾਰ,
                                memmap[K230_DEV_QSPI0].ਮਾਪ);

    create_unimplemented_device("qspi1", memmap[K230_DEV_QSPI1].ਅਧਾਰ,
                                memmap[K230_DEV_QSPI1].ਮਾਪ);

    create_unimplemented_device("spi", memmap[K230_DEV_SPI].ਅਧਾਰ,
                                memmap[K230_DEV_SPI].ਮਾਪ);

    create_unimplemented_device("hi_sys_cfg", memmap[K230_DEV_HI_SYS_CFG].ਅਧਾਰ,
                                memmap[K230_DEV_HI_SYS_CFG].ਮਾਪ);

    create_unimplemented_device("flash", memmap[K230_DEV_FLASH].ਅਧਾਰ,
                                memmap[K230_DEV_FLASH].ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ k230_soc_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = k230_soc_realize;
}

ਸਥਿਰ ਅਚਲ TypeInfo k230_soc_type_info = {
    .name = TYPE_RISCV_K230_SOC,
    .ਮਾਪਾ = TYPE_DEVICE,
    .instance_ਮਾਪ = ਆਕਾਰ(K230SoCState),
    .instance_init = k230_soc_init,
    .class_init = k230_soc_class_init,
};

ਸਥਿਰ ਖਾਲੀ k230_soc_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&k230_soc_type_info);
}

type_init(k230_soc_ਰਜਿਸਟਰ_types)

ਸਥਿਰ ਖਾਲੀ k230_ਸਿੱਧਾ_boot(K230MachineState *s, MachineState *machine)
{
    ਅਚਲ ਅੱਖਰ *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    RISCVBootInfo boot_info = {0};
    hwaddr ਸ਼ੁਰੂ_addr = K230_ਸਿੱਧਾ_OPENSBI_ADDR;
    hwaddr firmware_ਸਿਰਾ_addr = 0;
    hwaddr kernel_ਇੰਦਰਾਜ = 0;
    ਪੂਰਨ fdt_ਮਾਪ = 0;

    ਜੇ (machine->firmware && !strcmp(machine->firmware, "none")) {
        error_report("K230 direct boot requires OpenSBI firmware; omit "
                     "-bios none or pass OpenSBI with -bios");
        ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
    }

    ਜੇ (!machine->dtb) {
        error_report("K230 direct boot requires -dtb");
        ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
    }

    machine->fdt = ਲੱਦੋ_device_ਰੁੱਖ(machine->dtb, &fdt_ਮਾਪ);
    ਜੇ (!machine->fdt) {
        error_report("load_device_tree() failed");
        ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
    }

    qemu_fdt_ਜੋੜੋ_ਰਸਤਾ(machine->fdt, "/chosen");

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_ਲੱਦੋ_kernel(machine, &boot_info, K230_ਸਿੱਧਾ_KERNEL_ADDR, true, ਨਿਰਾ);
    kernel_ਇੰਦਰਾਜ = boot_info.image_ਨੀਵਾਂ_addr;

    riscv_ਲੱਦੋ_fdt(K230_ਸਿੱਧਾ_DTB_ADDR, machine->fdt);

    firmware_ਸਿਰਾ_addr = riscv_ਲੱਭੋ_and_ਲੱਦੋ_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ਸ਼ੁਰੂ_addr, ਨਿਰਾ);
    ਜੇ (firmware_ਸਿਰਾ_addr > K230_ਸਿੱਧਾ_KERNEL_ADDR) {
        error_report("K230 firmware overlaps kernel address 0x%x",
                     K230_ਸਿੱਧਾ_KERNEL_ADDR);
        ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
    }

    riscv_ਤਿਆਰੀ_rom_reset_vec(machine, &s->soc.c908_cpu, ਸ਼ੁਰੂ_addr,
                              memmap[K230_DEV_BOOTROM].ਅਧਾਰ,
                              memmap[K230_DEV_BOOTROM].ਮਾਪ, kernel_ਇੰਦਰਾਜ,
                              K230_ਸਿੱਧਾ_DTB_ADDR);
}

ਸਥਿਰ ਖਾਲੀ k230_firmware_boot(K230MachineState *s, MachineState *machine)
{
    ਅਚਲ ਅੱਖਰ *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    hwaddr ਸ਼ੁਰੂ_addr = memmap[K230_DEV_DDRC].ਅਧਾਰ;
    RISCVBootInfo boot_info = {0};

    ਜੇ (machine->dtb || (machine->kernel_cmdline && *machine->kernel_cmdline)) {
        error_report("K230 firmware boot does not support -dtb or -append");
        ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
    }

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_ਲੱਭੋ_and_ਲੱਦੋ_firmware(machine, &boot_info, firmware_name,
                                 &ਸ਼ੁਰੂ_addr, ਨਿਰਾ);

    riscv_ਤਿਆਰੀ_rom_reset_vec(machine, &s->soc.c908_cpu, ਸ਼ੁਰੂ_addr,
                              memmap[K230_DEV_BOOTROM].ਅਧਾਰ,
                              memmap[K230_DEV_BOOTROM].ਮਾਪ, 0, 0);
}

ਸਥਿਰ ਖਾਲੀ k230_machine_ਪੂਰਾ(Notifier *notifier, ਖਾਲੀ *ਅੰਕੜੇ)
{
    K230MachineState *s = container_ਦਾ(notifier, K230MachineState,
                                       machine_ਪੂਰਾ);
    MachineState *machine = MACHINE(s);

    ਜੇ (machine->kernel_filename) {
        k230_ਸਿੱਧਾ_boot(s, machine);
    } ਨਹੀਂਤਾਂ {
        k230_firmware_boot(s, machine);
    }
}

ਸਥਿਰ ਖਾਲੀ k230_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    K230MachineState *s = RISCV_K230_MACHINE(machine);
    ਸਮ੍ਰਿਤੀ_Region *sys_mem = get_system_ਸਮ੍ਰਿਤੀ();

    ਜੇ (machine->ram_ਮਾਪ < mc->default_ram_ਮਾਪ) {
        ਅੱਖਰ *sz = ਮਾਪ_ਮੰਜ਼ਿਲ_str(mc->default_ram_ਮਾਪ);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_ਬੱਚਾ(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_K230_SOC);
    qdev_realize(DEVICE(&s->soc), ਨਿਰਾ, &error_fatal);

    /* Data Memory */
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(sys_mem, memmap[K230_DEV_DDRC].ਅਧਾਰ,
                                machine->ram);

    s->machine_ਪੂਰਾ.notify = k230_machine_ਪੂਰਾ;
    qemu_ਜੋੜੋ_machine_init_ਪੂਰਾ_notifier(&s->machine_ਪੂਰਾ);
}

ਸਥਿਰ ਖਾਲੀ k230_machine_instance_init(Object *obj)
{
}

ਸਥਿਰ ਖਾਲੀ k230_machine_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with Kendryte K230 SDK";
    mc->init = k230_machine_init;
    mc->default_cpus = 1;
    mc->default_ram_id = "riscv.K230.ram"; /* DDR */
    mc->default_ram_ਮਾਪ = memmap[K230_DEV_DDRC].ਮਾਪ;
}

ਸਥਿਰ ਅਚਲ TypeInfo k230_machine_typeinfo = {
    .name       = TYPE_RISCV_K230_MACHINE,
    .ਮਾਪਾ     = TYPE_MACHINE,
    .class_init = k230_machine_class_init,
    .instance_init = k230_machine_instance_init,
    .instance_ਮਾਪ = ਆਕਾਰ(K230MachineState),
};

ਸਥਿਰ ਖਾਲੀ k230_machine_init_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&k230_machine_typeinfo);
}

type_init(k230_machine_init_ਰਜਿਸਟਰ_types)
