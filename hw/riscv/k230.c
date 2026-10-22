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

#شامل "qemu/osdep.h"
#شامل "cpu-qom.h"
#شامل "qemu/cutils.h"
#شامل "qemu/error-report.h"
#شامل "qapi/error.h"
#شامل "system/device_tree.h"
#شامل "system/system.h"
#شامل "system/memory.h"
#شامل "target/riscv/cpu.h"
#شامل "hw/core/loader.h"
#شامل "hw/core/sysbus.h"
#شامل "hw/riscv/k230.h"
#شامل "hw/riscv/boot.h"
#شامل "hw/intc/riscv_aclint.h"
#شامل "hw/intc/sifive_plic.h"
#شامل "hw/char/serial-mm.h"
#شامل "hw/misc/unimp.h"

/* Align K230_SDK k230_canmv_defconfig */
#تعریف_کرو K230_براہ_راست_OPENSBI_ADDR 0x8000000
#تعریف_کرو K230_براہ_راست_KERNEL_ADDR  0x8200000
#تعریف_کرو K230_براہ_راست_DTB_ADDR     0xa000000

ساکن ثابت Mem_Map_اندراج memmap[] = {
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

ساکن خالی k230_soc_init(Object *obj)
{
    K230SoCState *s = RISCV_K230_SOC(obj);
    RISCVHartArrayState *cpu0 = &s->c908_cpu;

    object_initialize_بچہ(obj, "c908-cpu", cpu0, TYPE_RISCV_HART_ARRAY);
    object_initialize_بچہ(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_K230_DDR_CFG);
    object_initialize_بچہ(obj, "ddr-phy", &s->ddr_phy,
                            TYPE_K230_DDR_PHY);
    s->ddr_cfg.phy = &s->ddr_phy;
    object_initialize_بچہ(obj, "k230-wdt0", &s->wdt[0], TYPE_K230_WDT);
    object_initialize_بچہ(obj, "k230-wdt1", &s->wdt[1], TYPE_K230_WDT);
    object_initialize_بچہ(obj, "k230-gsdma", &s->gsdma, TYPE_K230_GSDMA);
    object_initialize_بچہ(obj, "k230-decomp-gzip", &s->decomp_gzip,
                            TYPE_K230_DECOMP_GZIP);

    qdev_prop_سیٹ_uint32(DEVICE(cpu0), "hartid-base", 0);
    qdev_prop_سیٹ_string(DEVICE(cpu0), "cpu-type", TYPE_RISCV_CPU_THEAD_C908);
    qdev_prop_سیٹ_uint64(DEVICE(cpu0), "resetvec",
                         memmap[K230_DEV_BOOTROM].بنیاد);
}

ساکن DeviceState *k230_create_plic(صحیح بنیاد_hartid, صحیح hartid_گنتی)
{
    g_autofree حرف *plic_hart_config = تہی;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hartid_گنتی);

    /* Per-socket PLIC */
    واپس sifive_plic_create(get_system_حافظہ(),
                              memmap[K230_DEV_PLIC].بنیاد,
                              plic_hart_config, hartid_گنتی, بنیاد_hartid,
                              K230_PLIC_عدد_SOURCES,
                              K230_PLIC_عدد_PRIORITIES,
                              K230_PLIC_ترجیح_بنیاد, K230_PLIC_PENDING_بنیاد,
                              K230_PLIC_ENABLE_بنیاد, K230_PLIC_ENABLE_STRIDE,
                              K230_PLIC_CONTEXT_بنیاد,
                              K230_PLIC_CONTEXT_STRIDE,
                              memmap[K230_DEV_PLIC].سائز);
}

ساکن خالی k230_create_uart(حافظہ_Region *sys_mem, DeviceState *plic,
                             صحیح اشاریہ)
{
    صحیح uart_dev = K230_DEV_UART0 + اشاریہ;
    g_autofree حرف *name = g_strdup_printf("uart%d", اشاریہ);

    /* Cover the non-16550 part of the SDK's 0x1000 UART window. */
    create_unimplemented_device(name, memmap[uart_dev].بنیاد,
                                memmap[uart_dev].سائز);

    serial_mm_init(sys_mem, memmap[uart_dev].بنیاد, 2,
                   qdev_get_gpio_اندر(plic, K230_UART0_IRQ + اشاریہ),
                   399193, serial_hd(اشاریہ), DEVICE_LITTLE_ENDIAN);
}

ساکن خالی k230_soc_realize(DeviceState *dev, Error **errp)
{
    K230SoCState *s = RISCV_K230_SOC(dev);
    حافظہ_Region *sys_mem = get_system_حافظہ();
    صحیح c908_cpus;

    sysbus_realize(SYS_BUS_DEVICE(&s->c908_cpu), &error_fatal);
    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp)) {
        واپس;
    }
    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_phy), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[K230_DEV_DDRC_CFG].بنیاد);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_phy), 0,
                    memmap[K230_DEV_DDR_PHY].بنیاد);

    c908_cpus = s->c908_cpu.عدد_harts;

    /* SRAM */
    حافظہ_region_init_ram(&s->sram, OBJECT(dev), "sram",
                           memmap[K230_DEV_SRAM].سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion(sys_mem, memmap[K230_DEV_SRAM].بنیاد,
                                &s->sram);

    /* BootROM */
    حافظہ_region_init_rom(&s->bootrom, OBJECT(dev), "bootrom",
                           memmap[K230_DEV_BOOTROM].سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion(sys_mem, memmap[K230_DEV_BOOTROM].بنیاد,
                                &s->bootrom);

    /* PLIC */
    s->c908_plic = k230_create_plic(C908_CPU_HARTID, c908_cpus);

    /* CLINT */
    riscv_aclint_swi_create(sys_mem,
                            memmap[K230_DEV_CLINT].بنیاد,
                            C908_CPU_HARTID, c908_cpus, false);
    riscv_aclint_mtimer_create(sys_mem,
                               memmap[K230_DEV_CLINT].بنیاد + 0x4000,
                               RISCV_ACLINT_DEFAULT_MTIMER_سائز,
                               C908_CPU_HARTID, c908_cpus,
                               RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    /* UART */
    برائے (صحیح i = 0; i < K230_UART_گنتی; i++) {
        k230_create_uart(sys_mem, DEVICE(s->c908_plic), i);
    }

    /* Watchdog */
    برائے (صحیح i = 0; i < 2; i++) {
        اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->wdt[i]), errp)) {
            واپس;
        }
    }

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[0]), 0, memmap[K230_DEV_WDT0].بنیاد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[0]), 0,
                       qdev_get_gpio_اندر(DEVICE(s->c908_plic), K230_WDT0_IRQ));

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[1]), 0, memmap[K230_DEV_WDT1].بنیاد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[1]), 0,
                       qdev_get_gpio_اندر(DEVICE(s->c908_plic), K230_WDT1_IRQ));

    /* Gsdma */
    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->gsdma), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gsdma), 0, memmap[K230_DEV_GSDMA].بنیاد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gsdma), 0,
                       qdev_get_gpio_اندر(DEVICE(s->c908_plic), K230_GSDMA_IRQ));

    /* Decomp gzip */
    qdev_prop_سیٹ_uint64(DEVICE(&s->decomp_gzip), "sram-base",
                         memmap[K230_DEV_SRAM].بنیاد);
    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->decomp_gzip), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->decomp_gzip), 0,
                    memmap[K230_DEV_DECOMP_GZIP].بنیاد);
    برائے (صحیح i = 0; i < K230_DECOMP_GZIP_عدد_GPIOS_برآمد; i++) {
        qdev_connect_gpio_برآمد(DEVICE(&s->decomp_gzip), i,
                              qdev_get_gpio_اندر(DEVICE(&s->gsdma), i));
    }
    برائے (صحیح i = 0; i < K230_DECOMP_GZIP_عدد_GPIOS_اندر; i++) {
        qdev_connect_gpio_برآمد(DEVICE(&s->gsdma), i,
                              qdev_get_gpio_اندر(DEVICE(&s->decomp_gzip), i));
    }

    /* Noc stub, the k230 decomp_gzip driver in uboot will access this region */
    create_unimplemented_device("noc-stub", 0x91302000, 0x1000);

    /* unimplemented devices */
    create_unimplemented_device("kpu.l2-cache",
                                memmap[K230_DEV_KPU_L2_CACHE].بنیاد,
                                memmap[K230_DEV_KPU_L2_CACHE].سائز);

    create_unimplemented_device("kpu_cfg", memmap[K230_DEV_KPU_CFG].بنیاد,
                                memmap[K230_DEV_KPU_CFG].سائز);

    create_unimplemented_device("fft", memmap[K230_DEV_FFT].بنیاد,
                                memmap[K230_DEV_FFT].سائز);

    create_unimplemented_device("2d-engine.ai",
                                memmap[K230_DEV_AI_2D_ENGINE].بنیاد,
                                memmap[K230_DEV_AI_2D_ENGINE].سائز);

    create_unimplemented_device("dma", memmap[K230_DEV_DMA].بنیاد,
                                memmap[K230_DEV_DMA].سائز);

    create_unimplemented_device("2d-engine.non-ai",
                                memmap[K230_DEV_NON_AI_2D].بنیاد,
                                memmap[K230_DEV_NON_AI_2D].سائز);

    create_unimplemented_device("isp", memmap[K230_DEV_ISP].بنیاد,
                                memmap[K230_DEV_ISP].سائز);

    create_unimplemented_device("dewarp", memmap[K230_DEV_DEWARP].بنیاد,
                                memmap[K230_DEV_DEWARP].سائز);

    create_unimplemented_device("rx-csi", memmap[K230_DEV_RX_CSI].بنیاد,
                                memmap[K230_DEV_RX_CSI].سائز);

    create_unimplemented_device("vpu", memmap[K230_DEV_H264].بنیاد,
                                memmap[K230_DEV_H264].سائز);

    create_unimplemented_device("gpu", memmap[K230_DEV_2P5D].بنیاد,
                                memmap[K230_DEV_2P5D].سائز);

    create_unimplemented_device("vo", memmap[K230_DEV_VO].بنیاد,
                                memmap[K230_DEV_VO].سائز);

    create_unimplemented_device("vo_cfg", memmap[K230_DEV_VO_CFG].بنیاد,
                                memmap[K230_DEV_VO_CFG].سائز);

    create_unimplemented_device("3d-engine", memmap[K230_DEV_3D_ENGINE].بنیاد,
                                memmap[K230_DEV_3D_ENGINE].سائز);

    create_unimplemented_device("pmu", memmap[K230_DEV_PMU].بنیاد,
                                memmap[K230_DEV_PMU].سائز);

    create_unimplemented_device("rtc", memmap[K230_DEV_RTC].بنیاد,
                                memmap[K230_DEV_RTC].سائز);

    create_unimplemented_device("cmu", memmap[K230_DEV_CMU].بنیاد,
                                memmap[K230_DEV_CMU].سائز);

    create_unimplemented_device("rmu", memmap[K230_DEV_RMU].بنیاد,
                                memmap[K230_DEV_RMU].سائز);

    create_unimplemented_device("boot", memmap[K230_DEV_BOOT].بنیاد,
                                memmap[K230_DEV_BOOT].سائز);

    create_unimplemented_device("pwr", memmap[K230_DEV_PWR].بنیاد,
                                memmap[K230_DEV_PWR].سائز);

    create_unimplemented_device("ipcm", memmap[K230_DEV_MAILBOX].بنیاد,
                                memmap[K230_DEV_MAILBOX].سائز);

    create_unimplemented_device("iomux", memmap[K230_DEV_IOMUX].بنیاد,
                                memmap[K230_DEV_IOMUX].سائز);

    create_unimplemented_device("timer", memmap[K230_DEV_TIMER].بنیاد,
                                memmap[K230_DEV_TIMER].سائز);

    create_unimplemented_device("wdt0", memmap[K230_DEV_WDT0].بنیاد,
                                memmap[K230_DEV_WDT0].سائز);

    create_unimplemented_device("wdt1", memmap[K230_DEV_WDT1].بنیاد,
                                memmap[K230_DEV_WDT1].سائز);

    create_unimplemented_device("ts", memmap[K230_DEV_TS].بنیاد,
                                memmap[K230_DEV_TS].سائز);

    create_unimplemented_device("hdi", memmap[K230_DEV_HDI].بنیاد,
                                memmap[K230_DEV_HDI].سائز);

    create_unimplemented_device("stc", memmap[K230_DEV_STC].بنیاد,
                                memmap[K230_DEV_STC].سائز);

    create_unimplemented_device("security", memmap[K230_DEV_SECURITY].بنیاد,
                                memmap[K230_DEV_SECURITY].سائز);

    create_unimplemented_device("i2c0", memmap[K230_DEV_I2C0].بنیاد,
                                memmap[K230_DEV_I2C0].سائز);

    create_unimplemented_device("i2c1", memmap[K230_DEV_I2C1].بنیاد,
                                memmap[K230_DEV_I2C1].سائز);

    create_unimplemented_device("i2c2", memmap[K230_DEV_I2C2].بنیاد,
                                memmap[K230_DEV_I2C2].سائز);

    create_unimplemented_device("i2c3", memmap[K230_DEV_I2C3].بنیاد,
                                memmap[K230_DEV_I2C3].سائز);

    create_unimplemented_device("i2c4", memmap[K230_DEV_I2C4].بنیاد,
                                memmap[K230_DEV_I2C4].سائز);

    create_unimplemented_device("pwm", memmap[K230_DEV_PWM].بنیاد,
                                memmap[K230_DEV_PWM].سائز);

    create_unimplemented_device("gpio0", memmap[K230_DEV_GPIO0].بنیاد,
                                memmap[K230_DEV_GPIO0].سائز);

    create_unimplemented_device("gpio1", memmap[K230_DEV_GPIO1].بنیاد,
                                memmap[K230_DEV_GPIO1].سائز);

    create_unimplemented_device("adc", memmap[K230_DEV_ADC].بنیاد,
                                memmap[K230_DEV_ADC].سائز);

    create_unimplemented_device("codec", memmap[K230_DEV_CODEC].بنیاد,
                                memmap[K230_DEV_CODEC].سائز);

    create_unimplemented_device("i2s", memmap[K230_DEV_I2S].بنیاد,
                                memmap[K230_DEV_I2S].سائز);

    create_unimplemented_device("usb0", memmap[K230_DEV_USB0].بنیاد,
                                memmap[K230_DEV_USB0].سائز);

    create_unimplemented_device("usb1", memmap[K230_DEV_USB1].بنیاد,
                                memmap[K230_DEV_USB1].سائز);

    create_unimplemented_device("sd0", memmap[K230_DEV_SD0].بنیاد,
                                memmap[K230_DEV_SD0].سائز);

    create_unimplemented_device("sd1", memmap[K230_DEV_SD1].بنیاد,
                                memmap[K230_DEV_SD1].سائز);

    create_unimplemented_device("qspi0", memmap[K230_DEV_QSPI0].بنیاد,
                                memmap[K230_DEV_QSPI0].سائز);

    create_unimplemented_device("qspi1", memmap[K230_DEV_QSPI1].بنیاد,
                                memmap[K230_DEV_QSPI1].سائز);

    create_unimplemented_device("spi", memmap[K230_DEV_SPI].بنیاد,
                                memmap[K230_DEV_SPI].سائز);

    create_unimplemented_device("hi_sys_cfg", memmap[K230_DEV_HI_SYS_CFG].بنیاد,
                                memmap[K230_DEV_HI_SYS_CFG].سائز);

    create_unimplemented_device("flash", memmap[K230_DEV_FLASH].بنیاد,
                                memmap[K230_DEV_FLASH].سائز);
}

ساکن خالی k230_soc_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = k230_soc_realize;
}

ساکن ثابت TypeInfo k230_soc_type_info = {
    .name = TYPE_RISCV_K230_SOC,
    .والد = TYPE_DEVICE,
    .instance_سائز = حجم(K230SoCState),
    .instance_init = k230_soc_init,
    .class_init = k230_soc_class_init,
};

ساکن خالی k230_soc_رجسٹر_types(خالی)
{
    type_رجسٹر_static(&k230_soc_type_info);
}

type_init(k230_soc_رجسٹر_types)

ساکن خالی k230_براہ_راست_boot(K230MachineState *s, MachineState *machine)
{
    ثابت حرف *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    RISCVBootInfo boot_info = {0};
    hwaddr آغاز_addr = K230_براہ_راست_OPENSBI_ADDR;
    hwaddr firmware_انتہا_addr = 0;
    hwaddr kernel_اندراج = 0;
    صحیح fdt_سائز = 0;

    اگر (machine->firmware && !strcmp(machine->firmware, "none")) {
        error_report("K230 direct boot requires OpenSBI firmware; omit "
                     "-bios none or pass OpenSBI with -bios");
        باہر_نکلو(EXIT_FAILURE);
    }

    اگر (!machine->dtb) {
        error_report("K230 direct boot requires -dtb");
        باہر_نکلو(EXIT_FAILURE);
    }

    machine->fdt = بھرو_device_درخت(machine->dtb, &fdt_سائز);
    اگر (!machine->fdt) {
        error_report("load_device_tree() failed");
        باہر_نکلو(EXIT_FAILURE);
    }

    qemu_fdt_جوڑو_راستہ(machine->fdt, "/chosen");

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_بھرو_kernel(machine, &boot_info, K230_براہ_راست_KERNEL_ADDR, true, تہی);
    kernel_اندراج = boot_info.image_کم_addr;

    riscv_بھرو_fdt(K230_براہ_راست_DTB_ADDR, machine->fdt);

    firmware_انتہا_addr = riscv_ڈھونڈو_and_بھرو_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &آغاز_addr, تہی);
    اگر (firmware_انتہا_addr > K230_براہ_راست_KERNEL_ADDR) {
        error_report("K230 firmware overlaps kernel address 0x%x",
                     K230_براہ_راست_KERNEL_ADDR);
        باہر_نکلو(EXIT_FAILURE);
    }

    riscv_تیاری_rom_reset_vec(machine, &s->soc.c908_cpu, آغاز_addr,
                              memmap[K230_DEV_BOOTROM].بنیاد,
                              memmap[K230_DEV_BOOTROM].سائز, kernel_اندراج,
                              K230_براہ_راست_DTB_ADDR);
}

ساکن خالی k230_firmware_boot(K230MachineState *s, MachineState *machine)
{
    ثابت حرف *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    hwaddr آغاز_addr = memmap[K230_DEV_DDRC].بنیاد;
    RISCVBootInfo boot_info = {0};

    اگر (machine->dtb || (machine->kernel_cmdline && *machine->kernel_cmdline)) {
        error_report("K230 firmware boot does not support -dtb or -append");
        باہر_نکلو(EXIT_FAILURE);
    }

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_ڈھونڈو_and_بھرو_firmware(machine, &boot_info, firmware_name,
                                 &آغاز_addr, تہی);

    riscv_تیاری_rom_reset_vec(machine, &s->soc.c908_cpu, آغاز_addr,
                              memmap[K230_DEV_BOOTROM].بنیاد,
                              memmap[K230_DEV_BOOTROM].سائز, 0, 0);
}

ساکن خالی k230_machine_مکمل(Notifier *notifier, خالی *ڈیٹا)
{
    K230MachineState *s = container_کا(notifier, K230MachineState,
                                       machine_مکمل);
    MachineState *machine = MACHINE(s);

    اگر (machine->kernel_filename) {
        k230_براہ_راست_boot(s, machine);
    } ورنہ {
        k230_firmware_boot(s, machine);
    }
}

ساکن خالی k230_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    K230MachineState *s = RISCV_K230_MACHINE(machine);
    حافظہ_Region *sys_mem = get_system_حافظہ();

    اگر (machine->ram_سائز < mc->default_ram_سائز) {
        حرف *sz = سائز_مقصد_str(mc->default_ram_سائز);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        باہر_نکلو(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_بچہ(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_K230_SOC);
    qdev_realize(DEVICE(&s->soc), تہی, &error_fatal);

    /* Data Memory */
    حافظہ_region_جوڑو_subregion(sys_mem, memmap[K230_DEV_DDRC].بنیاد,
                                machine->ram);

    s->machine_مکمل.notify = k230_machine_مکمل;
    qemu_جوڑو_machine_init_مکمل_notifier(&s->machine_مکمل);
}

ساکن خالی k230_machine_instance_init(Object *obj)
{
}

ساکن خالی k230_machine_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with Kendryte K230 SDK";
    mc->init = k230_machine_init;
    mc->default_cpus = 1;
    mc->default_ram_id = "riscv.K230.ram"; /* DDR */
    mc->default_ram_سائز = memmap[K230_DEV_DDRC].سائز;
}

ساکن ثابت TypeInfo k230_machine_typeinfo = {
    .name       = TYPE_RISCV_K230_MACHINE,
    .والد     = TYPE_MACHINE,
    .class_init = k230_machine_class_init,
    .instance_init = k230_machine_instance_init,
    .instance_سائز = حجم(K230MachineState),
};

ساکن خالی k230_machine_init_رجسٹر_types(خالی)
{
    type_رجسٹر_static(&k230_machine_typeinfo);
}

type_init(k230_machine_init_رجسٹر_types)
