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

#ضمن "qemu/osdep.h"
#ضمن "cpu-qom.h"
#ضمن "qemu/cutils.h"
#ضمن "qemu/error-report.h"
#ضمن "qapi/error.h"
#ضمن "system/device_tree.h"
#ضمن "system/system.h"
#ضمن "system/memory.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/core/sysbus.h"
#ضمن "hw/riscv/k230.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/sifive_plic.h"
#ضمن "hw/char/serial-mm.h"
#ضمن "hw/misc/unimp.h"

/* Align K230_SDK k230_canmv_defconfig */
#تعریف_کړه K230_مستقیم_OPENSBI_ADDR 0x8000000
#تعریف_کړه K230_مستقیم_KERNEL_ADDR  0x8200000
#تعریف_کړه K230_مستقیم_DTB_ADDR     0xa000000

ساكن ثابت Mem_Map_ثبت memmap[] = {
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

ساكن فراغ k230_soc_init(Object *obj)
{
    K230SoCState *s = RISCV_K230_SOC(obj);
    RISCVHartArrayState *cpu0 = &s->c908_cpu;

    object_initialize_ماشوم(obj, "c908-cpu", cpu0, TYPE_RISCV_HART_ARRAY);
    object_initialize_ماشوم(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_K230_DDR_CFG);
    object_initialize_ماشوم(obj, "ddr-phy", &s->ddr_phy,
                            TYPE_K230_DDR_PHY);
    s->ddr_cfg.phy = &s->ddr_phy;
    object_initialize_ماشوم(obj, "k230-wdt0", &s->wdt[0], TYPE_K230_WDT);
    object_initialize_ماشوم(obj, "k230-wdt1", &s->wdt[1], TYPE_K230_WDT);
    object_initialize_ماشوم(obj, "k230-gsdma", &s->gsdma, TYPE_K230_GSDMA);
    object_initialize_ماشوم(obj, "k230-decomp-gzip", &s->decomp_gzip,
                            TYPE_K230_DECOMP_GZIP);

    qdev_prop_سېټ_uint32(DEVICE(cpu0), "hartid-base", 0);
    qdev_prop_سېټ_string(DEVICE(cpu0), "cpu-type", TYPE_RISCV_CPU_THEAD_C908);
    qdev_prop_سېټ_uint64(DEVICE(cpu0), "resetvec",
                         memmap[K230_DEV_BOOTROM].بنسټ);
}

ساكن DeviceState *k230_create_plic(صحيح بنسټ_hartid, صحيح hartid_شمېر)
{
    g_autofree حرف *plic_hart_config = عدم;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hartid_شمېر);

    /* Per-socket PLIC */
    أرجع sifive_plic_create(get_system_حافظه(),
                              memmap[K230_DEV_PLIC].بنسټ,
                              plic_hart_config, hartid_شمېر, بنسټ_hartid,
                              K230_PLIC_عدد_SOURCES,
                              K230_PLIC_عدد_PRIORITIES,
                              K230_PLIC_لومړیتوب_بنسټ, K230_PLIC_PENDING_بنسټ,
                              K230_PLIC_ENABLE_بنسټ, K230_PLIC_ENABLE_STRIDE,
                              K230_PLIC_CONTEXT_بنسټ,
                              K230_PLIC_CONTEXT_STRIDE,
                              memmap[K230_DEV_PLIC].اندازه);
}

ساكن فراغ k230_create_uart(حافظه_Region *sys_mem, DeviceState *plic,
                             صحيح شاخص)
{
    صحيح uart_dev = K230_DEV_UART0 + شاخص;
    g_autofree حرف *name = g_strdup_printf("uart%d", شاخص);

    /* Cover the non-16550 part of the SDK's 0x1000 UART window. */
    create_unimplemented_device(name, memmap[uart_dev].بنسټ,
                                memmap[uart_dev].اندازه);

    serial_mm_init(sys_mem, memmap[uart_dev].بنسټ, 2,
                   qdev_get_gpio_دننه(plic, K230_UART0_IRQ + شاخص),
                   399193, serial_hd(شاخص), DEVICE_LITTLE_ENDIAN);
}

ساكن فراغ k230_soc_realize(DeviceState *dev, Error **errp)
{
    K230SoCState *s = RISCV_K230_SOC(dev);
    حافظه_Region *sys_mem = get_system_حافظه();
    صحيح c908_cpus;

    sysbus_realize(SYS_BUS_DEVICE(&s->c908_cpu), &error_fatal);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp)) {
        أرجع;
    }
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_phy), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[K230_DEV_DDRC_CFG].بنسټ);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_phy), 0,
                    memmap[K230_DEV_DDR_PHY].بنسټ);

    c908_cpus = s->c908_cpu.عدد_harts;

    /* SRAM */
    حافظه_region_init_ram(&s->sram, OBJECT(dev), "sram",
                           memmap[K230_DEV_SRAM].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(sys_mem, memmap[K230_DEV_SRAM].بنسټ,
                                &s->sram);

    /* BootROM */
    حافظه_region_init_rom(&s->bootrom, OBJECT(dev), "bootrom",
                           memmap[K230_DEV_BOOTROM].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(sys_mem, memmap[K230_DEV_BOOTROM].بنسټ,
                                &s->bootrom);

    /* PLIC */
    s->c908_plic = k230_create_plic(C908_CPU_HARTID, c908_cpus);

    /* CLINT */
    riscv_aclint_swi_create(sys_mem,
                            memmap[K230_DEV_CLINT].بنسټ,
                            C908_CPU_HARTID, c908_cpus, false);
    riscv_aclint_mtimer_create(sys_mem,
                               memmap[K230_DEV_CLINT].بنسټ + 0x4000,
                               RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                               C908_CPU_HARTID, c908_cpus,
                               RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    /* UART */
    لكل (صحيح i = 0; i < K230_UART_شمېر; i++) {
        k230_create_uart(sys_mem, DEVICE(s->c908_plic), i);
    }

    /* Watchdog */
    لكل (صحيح i = 0; i < 2; i++) {
        إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->wdt[i]), errp)) {
            أرجع;
        }
    }

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[0]), 0, memmap[K230_DEV_WDT0].بنسټ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[0]), 0,
                       qdev_get_gpio_دننه(DEVICE(s->c908_plic), K230_WDT0_IRQ));

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[1]), 0, memmap[K230_DEV_WDT1].بنسټ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[1]), 0,
                       qdev_get_gpio_دننه(DEVICE(s->c908_plic), K230_WDT1_IRQ));

    /* Gsdma */
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->gsdma), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gsdma), 0, memmap[K230_DEV_GSDMA].بنسټ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gsdma), 0,
                       qdev_get_gpio_دننه(DEVICE(s->c908_plic), K230_GSDMA_IRQ));

    /* Decomp gzip */
    qdev_prop_سېټ_uint64(DEVICE(&s->decomp_gzip), "sram-base",
                         memmap[K230_DEV_SRAM].بنسټ);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->decomp_gzip), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->decomp_gzip), 0,
                    memmap[K230_DEV_DECOMP_GZIP].بنسټ);
    لكل (صحيح i = 0; i < K230_DECOMP_GZIP_عدد_GPIOS_بهر; i++) {
        qdev_connect_gpio_بهر(DEVICE(&s->decomp_gzip), i,
                              qdev_get_gpio_دننه(DEVICE(&s->gsdma), i));
    }
    لكل (صحيح i = 0; i < K230_DECOMP_GZIP_عدد_GPIOS_دننه; i++) {
        qdev_connect_gpio_بهر(DEVICE(&s->gsdma), i,
                              qdev_get_gpio_دننه(DEVICE(&s->decomp_gzip), i));
    }

    /* Noc stub, the k230 decomp_gzip driver in uboot will access this region */
    create_unimplemented_device("noc-stub", 0x91302000, 0x1000);

    /* unimplemented devices */
    create_unimplemented_device("kpu.l2-cache",
                                memmap[K230_DEV_KPU_L2_CACHE].بنسټ,
                                memmap[K230_DEV_KPU_L2_CACHE].اندازه);

    create_unimplemented_device("kpu_cfg", memmap[K230_DEV_KPU_CFG].بنسټ,
                                memmap[K230_DEV_KPU_CFG].اندازه);

    create_unimplemented_device("fft", memmap[K230_DEV_FFT].بنسټ,
                                memmap[K230_DEV_FFT].اندازه);

    create_unimplemented_device("2d-engine.ai",
                                memmap[K230_DEV_AI_2D_ENGINE].بنسټ,
                                memmap[K230_DEV_AI_2D_ENGINE].اندازه);

    create_unimplemented_device("dma", memmap[K230_DEV_DMA].بنسټ,
                                memmap[K230_DEV_DMA].اندازه);

    create_unimplemented_device("2d-engine.non-ai",
                                memmap[K230_DEV_NON_AI_2D].بنسټ,
                                memmap[K230_DEV_NON_AI_2D].اندازه);

    create_unimplemented_device("isp", memmap[K230_DEV_ISP].بنسټ,
                                memmap[K230_DEV_ISP].اندازه);

    create_unimplemented_device("dewarp", memmap[K230_DEV_DEWARP].بنسټ,
                                memmap[K230_DEV_DEWARP].اندازه);

    create_unimplemented_device("rx-csi", memmap[K230_DEV_RX_CSI].بنسټ,
                                memmap[K230_DEV_RX_CSI].اندازه);

    create_unimplemented_device("vpu", memmap[K230_DEV_H264].بنسټ,
                                memmap[K230_DEV_H264].اندازه);

    create_unimplemented_device("gpu", memmap[K230_DEV_2P5D].بنسټ,
                                memmap[K230_DEV_2P5D].اندازه);

    create_unimplemented_device("vo", memmap[K230_DEV_VO].بنسټ,
                                memmap[K230_DEV_VO].اندازه);

    create_unimplemented_device("vo_cfg", memmap[K230_DEV_VO_CFG].بنسټ,
                                memmap[K230_DEV_VO_CFG].اندازه);

    create_unimplemented_device("3d-engine", memmap[K230_DEV_3D_ENGINE].بنسټ,
                                memmap[K230_DEV_3D_ENGINE].اندازه);

    create_unimplemented_device("pmu", memmap[K230_DEV_PMU].بنسټ,
                                memmap[K230_DEV_PMU].اندازه);

    create_unimplemented_device("rtc", memmap[K230_DEV_RTC].بنسټ,
                                memmap[K230_DEV_RTC].اندازه);

    create_unimplemented_device("cmu", memmap[K230_DEV_CMU].بنسټ,
                                memmap[K230_DEV_CMU].اندازه);

    create_unimplemented_device("rmu", memmap[K230_DEV_RMU].بنسټ,
                                memmap[K230_DEV_RMU].اندازه);

    create_unimplemented_device("boot", memmap[K230_DEV_BOOT].بنسټ,
                                memmap[K230_DEV_BOOT].اندازه);

    create_unimplemented_device("pwr", memmap[K230_DEV_PWR].بنسټ,
                                memmap[K230_DEV_PWR].اندازه);

    create_unimplemented_device("ipcm", memmap[K230_DEV_MAILBOX].بنسټ,
                                memmap[K230_DEV_MAILBOX].اندازه);

    create_unimplemented_device("iomux", memmap[K230_DEV_IOMUX].بنسټ,
                                memmap[K230_DEV_IOMUX].اندازه);

    create_unimplemented_device("timer", memmap[K230_DEV_TIMER].بنسټ,
                                memmap[K230_DEV_TIMER].اندازه);

    create_unimplemented_device("wdt0", memmap[K230_DEV_WDT0].بنسټ,
                                memmap[K230_DEV_WDT0].اندازه);

    create_unimplemented_device("wdt1", memmap[K230_DEV_WDT1].بنسټ,
                                memmap[K230_DEV_WDT1].اندازه);

    create_unimplemented_device("ts", memmap[K230_DEV_TS].بنسټ,
                                memmap[K230_DEV_TS].اندازه);

    create_unimplemented_device("hdi", memmap[K230_DEV_HDI].بنسټ,
                                memmap[K230_DEV_HDI].اندازه);

    create_unimplemented_device("stc", memmap[K230_DEV_STC].بنسټ,
                                memmap[K230_DEV_STC].اندازه);

    create_unimplemented_device("security", memmap[K230_DEV_SECURITY].بنسټ,
                                memmap[K230_DEV_SECURITY].اندازه);

    create_unimplemented_device("i2c0", memmap[K230_DEV_I2C0].بنسټ,
                                memmap[K230_DEV_I2C0].اندازه);

    create_unimplemented_device("i2c1", memmap[K230_DEV_I2C1].بنسټ,
                                memmap[K230_DEV_I2C1].اندازه);

    create_unimplemented_device("i2c2", memmap[K230_DEV_I2C2].بنسټ,
                                memmap[K230_DEV_I2C2].اندازه);

    create_unimplemented_device("i2c3", memmap[K230_DEV_I2C3].بنسټ,
                                memmap[K230_DEV_I2C3].اندازه);

    create_unimplemented_device("i2c4", memmap[K230_DEV_I2C4].بنسټ,
                                memmap[K230_DEV_I2C4].اندازه);

    create_unimplemented_device("pwm", memmap[K230_DEV_PWM].بنسټ,
                                memmap[K230_DEV_PWM].اندازه);

    create_unimplemented_device("gpio0", memmap[K230_DEV_GPIO0].بنسټ,
                                memmap[K230_DEV_GPIO0].اندازه);

    create_unimplemented_device("gpio1", memmap[K230_DEV_GPIO1].بنسټ,
                                memmap[K230_DEV_GPIO1].اندازه);

    create_unimplemented_device("adc", memmap[K230_DEV_ADC].بنسټ,
                                memmap[K230_DEV_ADC].اندازه);

    create_unimplemented_device("codec", memmap[K230_DEV_CODEC].بنسټ,
                                memmap[K230_DEV_CODEC].اندازه);

    create_unimplemented_device("i2s", memmap[K230_DEV_I2S].بنسټ,
                                memmap[K230_DEV_I2S].اندازه);

    create_unimplemented_device("usb0", memmap[K230_DEV_USB0].بنسټ,
                                memmap[K230_DEV_USB0].اندازه);

    create_unimplemented_device("usb1", memmap[K230_DEV_USB1].بنسټ,
                                memmap[K230_DEV_USB1].اندازه);

    create_unimplemented_device("sd0", memmap[K230_DEV_SD0].بنسټ,
                                memmap[K230_DEV_SD0].اندازه);

    create_unimplemented_device("sd1", memmap[K230_DEV_SD1].بنسټ,
                                memmap[K230_DEV_SD1].اندازه);

    create_unimplemented_device("qspi0", memmap[K230_DEV_QSPI0].بنسټ,
                                memmap[K230_DEV_QSPI0].اندازه);

    create_unimplemented_device("qspi1", memmap[K230_DEV_QSPI1].بنسټ,
                                memmap[K230_DEV_QSPI1].اندازه);

    create_unimplemented_device("spi", memmap[K230_DEV_SPI].بنسټ,
                                memmap[K230_DEV_SPI].اندازه);

    create_unimplemented_device("hi_sys_cfg", memmap[K230_DEV_HI_SYS_CFG].بنسټ,
                                memmap[K230_DEV_HI_SYS_CFG].اندازه);

    create_unimplemented_device("flash", memmap[K230_DEV_FLASH].بنسټ,
                                memmap[K230_DEV_FLASH].اندازه);
}

ساكن فراغ k230_soc_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = k230_soc_realize;
}

ساكن ثابت TypeInfo k230_soc_type_info = {
    .name = TYPE_RISCV_K230_SOC,
    .پلار = TYPE_DEVICE,
    .instance_اندازه = حجم(K230SoCState),
    .instance_init = k230_soc_init,
    .class_init = k230_soc_class_init,
};

ساكن فراغ k230_soc_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&k230_soc_type_info);
}

type_init(k230_soc_ثبتوونکی_types)

ساكن فراغ k230_مستقیم_boot(K230MachineState *s, MachineState *machine)
{
    ثابت حرف *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    RISCVBootInfo boot_info = {0};
    hwaddr پیل_addr = K230_مستقیم_OPENSBI_ADDR;
    hwaddr firmware_پای_ټکی_addr = 0;
    hwaddr kernel_ثبت = 0;
    صحيح fdt_اندازه = 0;

    إذا (machine->firmware && !strcmp(machine->firmware, "none")) {
        error_report("K230 direct boot requires OpenSBI firmware; omit "
                     "-bios none or pass OpenSBI with -bios");
        وتل(EXIT_FAILURE);
    }

    إذا (!machine->dtb) {
        error_report("K230 direct boot requires -dtb");
        وتل(EXIT_FAILURE);
    }

    machine->fdt = بارول_device_ونه(machine->dtb, &fdt_اندازه);
    إذا (!machine->fdt) {
        error_report("load_device_tree() failed");
        وتل(EXIT_FAILURE);
    }

    qemu_fdt_جمع_کړه_لار(machine->fdt, "/chosen");

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_بارول_kernel(machine, &boot_info, K230_مستقیم_KERNEL_ADDR, true, عدم);
    kernel_ثبت = boot_info.image_ټیټ_addr;

    riscv_بارول_fdt(K230_مستقیم_DTB_ADDR, machine->fdt);

    firmware_پای_ټکی_addr = riscv_پیدا_کړه_and_بارول_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &پیل_addr, عدم);
    إذا (firmware_پای_ټکی_addr > K230_مستقیم_KERNEL_ADDR) {
        error_report("K230 firmware overlaps kernel address 0x%x",
                     K230_مستقیم_KERNEL_ADDR);
        وتل(EXIT_FAILURE);
    }

    riscv_تیاری_rom_reset_vec(machine, &s->soc.c908_cpu, پیل_addr,
                              memmap[K230_DEV_BOOTROM].بنسټ,
                              memmap[K230_DEV_BOOTROM].اندازه, kernel_ثبت,
                              K230_مستقیم_DTB_ADDR);
}

ساكن فراغ k230_firmware_boot(K230MachineState *s, MachineState *machine)
{
    ثابت حرف *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    hwaddr پیل_addr = memmap[K230_DEV_DDRC].بنسټ;
    RISCVBootInfo boot_info = {0};

    إذا (machine->dtb || (machine->kernel_cmdline && *machine->kernel_cmdline)) {
        error_report("K230 firmware boot does not support -dtb or -append");
        وتل(EXIT_FAILURE);
    }

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_پیدا_کړه_and_بارول_firmware(machine, &boot_info, firmware_name,
                                 &پیل_addr, عدم);

    riscv_تیاری_rom_reset_vec(machine, &s->soc.c908_cpu, پیل_addr,
                              memmap[K230_DEV_BOOTROM].بنسټ,
                              memmap[K230_DEV_BOOTROM].اندازه, 0, 0);
}

ساكن فراغ k230_machine_بشپړ(Notifier *notifier, فراغ *معلومات)
{
    K230MachineState *s = container_د(notifier, K230MachineState,
                                       machine_بشپړ);
    MachineState *machine = MACHINE(s);

    إذا (machine->kernel_filename) {
        k230_مستقیم_boot(s, machine);
    } وإلا {
        k230_firmware_boot(s, machine);
    }
}

ساكن فراغ k230_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    K230MachineState *s = RISCV_K230_MACHINE(machine);
    حافظه_Region *sys_mem = get_system_حافظه();

    إذا (machine->ram_اندازه < mc->default_ram_اندازه) {
        حرف *sz = اندازه_منزل_str(mc->default_ram_اندازه);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        وتل(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_ماشوم(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_K230_SOC);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    /* Data Memory */
    حافظه_region_جمع_کړه_subregion(sys_mem, memmap[K230_DEV_DDRC].بنسټ,
                                machine->ram);

    s->machine_بشپړ.notify = k230_machine_بشپړ;
    qemu_جمع_کړه_machine_init_بشپړ_notifier(&s->machine_بشپړ);
}

ساكن فراغ k230_machine_instance_init(Object *obj)
{
}

ساكن فراغ k230_machine_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with Kendryte K230 SDK";
    mc->init = k230_machine_init;
    mc->default_cpus = 1;
    mc->default_ram_id = "riscv.K230.ram"; /* DDR */
    mc->default_ram_اندازه = memmap[K230_DEV_DDRC].اندازه;
}

ساكن ثابت TypeInfo k230_machine_typeinfo = {
    .name       = TYPE_RISCV_K230_MACHINE,
    .پلار     = TYPE_MACHINE,
    .class_init = k230_machine_class_init,
    .instance_init = k230_machine_instance_init,
    .instance_اندازه = حجم(K230MachineState),
};

ساكن فراغ k230_machine_init_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&k230_machine_typeinfo);
}

type_init(k230_machine_init_ثبتوونکی_types)
