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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "cpu-qom.h"
#अन्तर्भाव "qemu/cutils.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "system/device_tree.h"
#अन्तर्भाव "system/system.h"
#अन्तर्भाव "system/memory.h"
#अन्तर्भाव "target/riscv/cpu.h"
#अन्तर्भाव "hw/core/loader.h"
#अन्तर्भाव "hw/core/sysbus.h"
#अन्तर्भाव "hw/riscv/k230.h"
#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "hw/intc/sifive_plic.h"
#अन्तर्भाव "hw/char/serial-mm.h"
#अन्तर्भाव "hw/misc/unimp.h"

/* Align K230_SDK k230_canmv_defconfig */
#परिभाषय K230_प्रत्यक्षम्_OPENSBI_ADDR 0x8000000
#परिभाषय K230_प्रत्यक्षम्_KERNEL_ADDR  0x8200000
#परिभाषय K230_प्रत्यक्षम्_DTB_ADDR     0xa000000

स्थिर ध्रुव Mem_Map_प्रविष्टिः memmap[] = {
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

स्थिर शून्य k230_soc_init(Object *obj)
{
    K230SoCState *s = RISCV_K230_SOC(obj);
    RISCVHartArrayState *cpu0 = &s->c908_cpu;

    object_initialize_child(obj, "c908-cpu", cpu0, TYPE_RISCV_HART_ARRAY);
    object_initialize_child(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_K230_DDR_CFG);
    object_initialize_child(obj, "ddr-phy", &s->ddr_phy,
                            TYPE_K230_DDR_PHY);
    s->ddr_cfg.phy = &s->ddr_phy;
    object_initialize_child(obj, "k230-wdt0", &s->wdt[0], TYPE_K230_WDT);
    object_initialize_child(obj, "k230-wdt1", &s->wdt[1], TYPE_K230_WDT);
    object_initialize_child(obj, "k230-gsdma", &s->gsdma, TYPE_K230_GSDMA);
    object_initialize_child(obj, "k230-decomp-gzip", &s->decomp_gzip,
                            TYPE_K230_DECOMP_GZIP);

    qdev_prop_समुच्चयः_uint32(DEVICE(cpu0), "hartid-base", 0);
    qdev_prop_समुच्चयः_string(DEVICE(cpu0), "cpu-type", TYPE_RISCV_CPU_THEAD_C908);
    qdev_prop_समुच्चयः_uint64(DEVICE(cpu0), "resetvec",
                         memmap[K230_DEV_BOOTROM].आधारः);
}

स्थिर DeviceState *k230_create_plic(पूर्णाङ्क आधारः_hartid, पूर्णाङ्क hartid_गणनम्)
{
    g_autofree वर्ण *plic_hart_config = रिक्त;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hartid_गणनम्);

    /* Per-socket PLIC */
    प्रत्यागम sifive_plic_create(get_system_स्मृतिः(),
                              memmap[K230_DEV_PLIC].आधारः,
                              plic_hart_config, hartid_गणनम्, आधारः_hartid,
                              K230_PLIC_अङ्कः_SOURCES,
                              K230_PLIC_अङ्कः_PRIORITIES,
                              K230_PLIC_प्राथमिकता_आधारः, K230_PLIC_PENDING_आधारः,
                              K230_PLIC_ENABLE_आधारः, K230_PLIC_ENABLE_STRIDE,
                              K230_PLIC_CONTEXT_आधारः,
                              K230_PLIC_CONTEXT_STRIDE,
                              memmap[K230_DEV_PLIC].आकार);
}

स्थिर शून्य k230_create_uart(स्मृतिः_Region *sys_mem, DeviceState *plic,
                             पूर्णाङ्क सूचकांक)
{
    पूर्णाङ्क uart_dev = K230_DEV_UART0 + सूचकांक;
    g_autofree वर्ण *name = g_strdup_printf("uart%d", सूचकांक);

    /* Cover the non-16550 part of the SDK's 0x1000 UART window. */
    create_unimplemented_device(name, memmap[uart_dev].आधारः,
                                memmap[uart_dev].आकार);

    serial_mm_init(sys_mem, memmap[uart_dev].आधारः, 2,
                   qdev_get_gpio_अन्तर्(plic, K230_UART0_IRQ + सूचकांक),
                   399193, serial_hd(सूचकांक), DEVICE_LITTLE_ENDIAN);
}

स्थिर शून्य k230_soc_realize(DeviceState *dev, Error **errp)
{
    K230SoCState *s = RISCV_K230_SOC(dev);
    स्मृतिः_Region *sys_mem = get_system_स्मृतिः();
    पूर्णाङ्क c908_cpus;

    sysbus_realize(SYS_BUS_DEVICE(&s->c908_cpu), &error_fatal);
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp)) {
        प्रत्यागम;
    }
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_phy), errp)) {
        प्रत्यागम;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[K230_DEV_DDRC_CFG].आधारः);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_phy), 0,
                    memmap[K230_DEV_DDR_PHY].आधारः);

    c908_cpus = s->c908_cpu.अङ्कः_harts;

    /* SRAM */
    स्मृतिः_region_init_ram(&s->sram, OBJECT(dev), "sram",
                           memmap[K230_DEV_SRAM].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(sys_mem, memmap[K230_DEV_SRAM].आधारः,
                                &s->sram);

    /* BootROM */
    स्मृतिः_region_init_rom(&s->bootrom, OBJECT(dev), "bootrom",
                           memmap[K230_DEV_BOOTROM].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(sys_mem, memmap[K230_DEV_BOOTROM].आधारः,
                                &s->bootrom);

    /* PLIC */
    s->c908_plic = k230_create_plic(C908_CPU_HARTID, c908_cpus);

    /* CLINT */
    riscv_aclint_swi_create(sys_mem,
                            memmap[K230_DEV_CLINT].आधारः,
                            C908_CPU_HARTID, c908_cpus, false);
    riscv_aclint_mtimer_create(sys_mem,
                               memmap[K230_DEV_CLINT].आधारः + 0x4000,
                               RISCV_ACLINT_DEFAULT_MTIMER_आकार,
                               C908_CPU_HARTID, c908_cpus,
                               RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    /* UART */
    प्रति (पूर्णाङ्क i = 0; i < K230_UART_गणनम्; i++) {
        k230_create_uart(sys_mem, DEVICE(s->c908_plic), i);
    }

    /* Watchdog */
    प्रति (पूर्णाङ्क i = 0; i < 2; i++) {
        यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->wdt[i]), errp)) {
            प्रत्यागम;
        }
    }

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[0]), 0, memmap[K230_DEV_WDT0].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[0]), 0,
                       qdev_get_gpio_अन्तर्(DEVICE(s->c908_plic), K230_WDT0_IRQ));

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[1]), 0, memmap[K230_DEV_WDT1].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[1]), 0,
                       qdev_get_gpio_अन्तर्(DEVICE(s->c908_plic), K230_WDT1_IRQ));

    /* Gsdma */
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->gsdma), errp)) {
        प्रत्यागम;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gsdma), 0, memmap[K230_DEV_GSDMA].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gsdma), 0,
                       qdev_get_gpio_अन्तर्(DEVICE(s->c908_plic), K230_GSDMA_IRQ));

    /* Decomp gzip */
    qdev_prop_समुच्चयः_uint64(DEVICE(&s->decomp_gzip), "sram-base",
                         memmap[K230_DEV_SRAM].आधारः);
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->decomp_gzip), errp)) {
        प्रत्यागम;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->decomp_gzip), 0,
                    memmap[K230_DEV_DECOMP_GZIP].आधारः);
    प्रति (पूर्णाङ्क i = 0; i < K230_DECOMP_GZIP_अङ्कः_GPIOS_बहिर्गमनम्; i++) {
        qdev_connect_gpio_बहिर्गमनम्(DEVICE(&s->decomp_gzip), i,
                              qdev_get_gpio_अन्तर्(DEVICE(&s->gsdma), i));
    }
    प्रति (पूर्णाङ्क i = 0; i < K230_DECOMP_GZIP_अङ्कः_GPIOS_अन्तर्; i++) {
        qdev_connect_gpio_बहिर्गमनम्(DEVICE(&s->gsdma), i,
                              qdev_get_gpio_अन्तर्(DEVICE(&s->decomp_gzip), i));
    }

    /* Noc stub, the k230 decomp_gzip driver in uboot will access this region */
    create_unimplemented_device("noc-stub", 0x91302000, 0x1000);

    /* unimplemented devices */
    create_unimplemented_device("kpu.l2-cache",
                                memmap[K230_DEV_KPU_L2_CACHE].आधारः,
                                memmap[K230_DEV_KPU_L2_CACHE].आकार);

    create_unimplemented_device("kpu_cfg", memmap[K230_DEV_KPU_CFG].आधारः,
                                memmap[K230_DEV_KPU_CFG].आकार);

    create_unimplemented_device("fft", memmap[K230_DEV_FFT].आधारः,
                                memmap[K230_DEV_FFT].आकार);

    create_unimplemented_device("2d-engine.ai",
                                memmap[K230_DEV_AI_2D_ENGINE].आधारः,
                                memmap[K230_DEV_AI_2D_ENGINE].आकार);

    create_unimplemented_device("dma", memmap[K230_DEV_DMA].आधारः,
                                memmap[K230_DEV_DMA].आकार);

    create_unimplemented_device("2d-engine.non-ai",
                                memmap[K230_DEV_NON_AI_2D].आधारः,
                                memmap[K230_DEV_NON_AI_2D].आकार);

    create_unimplemented_device("isp", memmap[K230_DEV_ISP].आधारः,
                                memmap[K230_DEV_ISP].आकार);

    create_unimplemented_device("dewarp", memmap[K230_DEV_DEWARP].आधारः,
                                memmap[K230_DEV_DEWARP].आकार);

    create_unimplemented_device("rx-csi", memmap[K230_DEV_RX_CSI].आधारः,
                                memmap[K230_DEV_RX_CSI].आकार);

    create_unimplemented_device("vpu", memmap[K230_DEV_H264].आधारः,
                                memmap[K230_DEV_H264].आकार);

    create_unimplemented_device("gpu", memmap[K230_DEV_2P5D].आधारः,
                                memmap[K230_DEV_2P5D].आकार);

    create_unimplemented_device("vo", memmap[K230_DEV_VO].आधारः,
                                memmap[K230_DEV_VO].आकार);

    create_unimplemented_device("vo_cfg", memmap[K230_DEV_VO_CFG].आधारः,
                                memmap[K230_DEV_VO_CFG].आकार);

    create_unimplemented_device("3d-engine", memmap[K230_DEV_3D_ENGINE].आधारः,
                                memmap[K230_DEV_3D_ENGINE].आकार);

    create_unimplemented_device("pmu", memmap[K230_DEV_PMU].आधारः,
                                memmap[K230_DEV_PMU].आकार);

    create_unimplemented_device("rtc", memmap[K230_DEV_RTC].आधारः,
                                memmap[K230_DEV_RTC].आकार);

    create_unimplemented_device("cmu", memmap[K230_DEV_CMU].आधारः,
                                memmap[K230_DEV_CMU].आकार);

    create_unimplemented_device("rmu", memmap[K230_DEV_RMU].आधारः,
                                memmap[K230_DEV_RMU].आकार);

    create_unimplemented_device("boot", memmap[K230_DEV_BOOT].आधारः,
                                memmap[K230_DEV_BOOT].आकार);

    create_unimplemented_device("pwr", memmap[K230_DEV_PWR].आधारः,
                                memmap[K230_DEV_PWR].आकार);

    create_unimplemented_device("ipcm", memmap[K230_DEV_MAILBOX].आधारः,
                                memmap[K230_DEV_MAILBOX].आकार);

    create_unimplemented_device("iomux", memmap[K230_DEV_IOMUX].आधारः,
                                memmap[K230_DEV_IOMUX].आकार);

    create_unimplemented_device("timer", memmap[K230_DEV_TIMER].आधारः,
                                memmap[K230_DEV_TIMER].आकार);

    create_unimplemented_device("wdt0", memmap[K230_DEV_WDT0].आधारः,
                                memmap[K230_DEV_WDT0].आकार);

    create_unimplemented_device("wdt1", memmap[K230_DEV_WDT1].आधारः,
                                memmap[K230_DEV_WDT1].आकार);

    create_unimplemented_device("ts", memmap[K230_DEV_TS].आधारः,
                                memmap[K230_DEV_TS].आकार);

    create_unimplemented_device("hdi", memmap[K230_DEV_HDI].आधारः,
                                memmap[K230_DEV_HDI].आकार);

    create_unimplemented_device("stc", memmap[K230_DEV_STC].आधारः,
                                memmap[K230_DEV_STC].आकार);

    create_unimplemented_device("security", memmap[K230_DEV_SECURITY].आधारः,
                                memmap[K230_DEV_SECURITY].आकार);

    create_unimplemented_device("i2c0", memmap[K230_DEV_I2C0].आधारः,
                                memmap[K230_DEV_I2C0].आकार);

    create_unimplemented_device("i2c1", memmap[K230_DEV_I2C1].आधारः,
                                memmap[K230_DEV_I2C1].आकार);

    create_unimplemented_device("i2c2", memmap[K230_DEV_I2C2].आधारः,
                                memmap[K230_DEV_I2C2].आकार);

    create_unimplemented_device("i2c3", memmap[K230_DEV_I2C3].आधारः,
                                memmap[K230_DEV_I2C3].आकार);

    create_unimplemented_device("i2c4", memmap[K230_DEV_I2C4].आधारः,
                                memmap[K230_DEV_I2C4].आकार);

    create_unimplemented_device("pwm", memmap[K230_DEV_PWM].आधारः,
                                memmap[K230_DEV_PWM].आकार);

    create_unimplemented_device("gpio0", memmap[K230_DEV_GPIO0].आधारः,
                                memmap[K230_DEV_GPIO0].आकार);

    create_unimplemented_device("gpio1", memmap[K230_DEV_GPIO1].आधारः,
                                memmap[K230_DEV_GPIO1].आकार);

    create_unimplemented_device("adc", memmap[K230_DEV_ADC].आधारः,
                                memmap[K230_DEV_ADC].आकार);

    create_unimplemented_device("codec", memmap[K230_DEV_CODEC].आधारः,
                                memmap[K230_DEV_CODEC].आकार);

    create_unimplemented_device("i2s", memmap[K230_DEV_I2S].आधारः,
                                memmap[K230_DEV_I2S].आकार);

    create_unimplemented_device("usb0", memmap[K230_DEV_USB0].आधारः,
                                memmap[K230_DEV_USB0].आकार);

    create_unimplemented_device("usb1", memmap[K230_DEV_USB1].आधारः,
                                memmap[K230_DEV_USB1].आकार);

    create_unimplemented_device("sd0", memmap[K230_DEV_SD0].आधारः,
                                memmap[K230_DEV_SD0].आकार);

    create_unimplemented_device("sd1", memmap[K230_DEV_SD1].आधारः,
                                memmap[K230_DEV_SD1].आकार);

    create_unimplemented_device("qspi0", memmap[K230_DEV_QSPI0].आधारः,
                                memmap[K230_DEV_QSPI0].आकार);

    create_unimplemented_device("qspi1", memmap[K230_DEV_QSPI1].आधारः,
                                memmap[K230_DEV_QSPI1].आकार);

    create_unimplemented_device("spi", memmap[K230_DEV_SPI].आधारः,
                                memmap[K230_DEV_SPI].आकार);

    create_unimplemented_device("hi_sys_cfg", memmap[K230_DEV_HI_SYS_CFG].आधारः,
                                memmap[K230_DEV_HI_SYS_CFG].आकार);

    create_unimplemented_device("flash", memmap[K230_DEV_FLASH].आधारः,
                                memmap[K230_DEV_FLASH].आकार);
}

स्थिर शून्य k230_soc_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = k230_soc_realize;
}

स्थिर ध्रुव TypeInfo k230_soc_type_info = {
    .name = TYPE_RISCV_K230_SOC,
    .parent = TYPE_DEVICE,
    .instance_आकार = परिमाण(K230SoCState),
    .instance_init = k230_soc_init,
    .class_init = k230_soc_class_init,
};

स्थिर शून्य k230_soc_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&k230_soc_type_info);
}

type_init(k230_soc_पञ्जिकाकोशः_types)

स्थिर शून्य k230_प्रत्यक्षम्_boot(K230MachineState *s, MachineState *machine)
{
    ध्रुव वर्ण *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    RISCVBootInfo boot_info = {0};
    hwaddr start_addr = K230_प्रत्यक्षम्_OPENSBI_ADDR;
    hwaddr firmware_अन्तः_addr = 0;
    hwaddr kernel_प्रविष्टिः = 0;
    पूर्णाङ्क fdt_आकार = 0;

    यदि (machine->firmware && !वाक्यं_तुलय(machine->firmware, "none")) {
        error_report("K230 direct boot requires OpenSBI firmware; omit "
                     "-bios none or pass OpenSBI with -bios");
        निर्गच्छ(EXIT_FAILURE);
    }

    यदि (!machine->dtb) {
        error_report("K230 direct boot requires -dtb");
        निर्गच्छ(EXIT_FAILURE);
    }

    machine->fdt = आरोपय_device_वृक्षः(machine->dtb, &fdt_आकार);
    यदि (!machine->fdt) {
        error_report("load_device_tree() failed");
        निर्गच्छ(EXIT_FAILURE);
    }

    qemu_fdt_सङ्कलय_मार्गः(machine->fdt, "/chosen");

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_आरोपय_kernel(machine, &boot_info, K230_प्रत्यक्षम्_KERNEL_ADDR, true, रिक्त);
    kernel_प्रविष्टिः = boot_info.image_नीचः_addr;

    riscv_आरोपय_fdt(K230_प्रत्यक्षम्_DTB_ADDR, machine->fdt);

    firmware_अन्तः_addr = riscv_find_and_आरोपय_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &start_addr, रिक्त);
    यदि (firmware_अन्तः_addr > K230_प्रत्यक्षम्_KERNEL_ADDR) {
        error_report("K230 firmware overlaps kernel address 0x%x",
                     K230_प्रत्यक्षम्_KERNEL_ADDR);
        निर्गच्छ(EXIT_FAILURE);
    }

    riscv_सज्जता_rom_reset_vec(machine, &s->soc.c908_cpu, start_addr,
                              memmap[K230_DEV_BOOTROM].आधारः,
                              memmap[K230_DEV_BOOTROM].आकार, kernel_प्रविष्टिः,
                              K230_प्रत्यक्षम्_DTB_ADDR);
}

स्थिर शून्य k230_firmware_boot(K230MachineState *s, MachineState *machine)
{
    ध्रुव वर्ण *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    hwaddr start_addr = memmap[K230_DEV_DDRC].आधारः;
    RISCVBootInfo boot_info = {0};

    यदि (machine->dtb || (machine->kernel_cmdline && *machine->kernel_cmdline)) {
        error_report("K230 firmware boot does not support -dtb or -append");
        निर्गच्छ(EXIT_FAILURE);
    }

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_find_and_आरोपय_firmware(machine, &boot_info, firmware_name,
                                 &start_addr, रिक्त);

    riscv_सज्जता_rom_reset_vec(machine, &s->soc.c908_cpu, start_addr,
                              memmap[K230_DEV_BOOTROM].आधारः,
                              memmap[K230_DEV_BOOTROM].आकार, 0, 0);
}

स्थिर शून्य k230_machine_समाप्तम्(Notifier *notifier, शून्य *दत्त)
{
    K230MachineState *s = container_सम्बन्धि(notifier, K230MachineState,
                                       machine_समाप्तम्);
    MachineState *machine = MACHINE(s);

    यदि (machine->kernel_filename) {
        k230_प्रत्यक्षम्_boot(s, machine);
    } अन्यथा {
        k230_firmware_boot(s, machine);
    }
}

स्थिर शून्य k230_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    K230MachineState *s = RISCV_K230_MACHINE(machine);
    स्मृतिः_Region *sys_mem = get_system_स्मृतिः();

    यदि (machine->ram_आकार < mc->default_ram_आकार) {
        वर्ण *sz = आकार_गन्तव्यम्_str(mc->default_ram_आकार);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        निर्गच्छ(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_K230_SOC);
    qdev_realize(DEVICE(&s->soc), रिक्त, &error_fatal);

    /* Data Memory */
    स्मृतिः_region_सङ्कलय_subregion(sys_mem, memmap[K230_DEV_DDRC].आधारः,
                                machine->ram);

    s->machine_समाप्तम्.notify = k230_machine_समाप्तम्;
    qemu_सङ्कलय_machine_init_समाप्तम्_notifier(&s->machine_समाप्तम्);
}

स्थिर शून्य k230_machine_instance_init(Object *obj)
{
}

स्थिर शून्य k230_machine_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with Kendryte K230 SDK";
    mc->init = k230_machine_init;
    mc->default_cpus = 1;
    mc->default_ram_id = "riscv.K230.ram"; /* DDR */
    mc->default_ram_आकार = memmap[K230_DEV_DDRC].आकार;
}

स्थिर ध्रुव TypeInfo k230_machine_typeinfo = {
    .name       = TYPE_RISCV_K230_MACHINE,
    .parent     = TYPE_MACHINE,
    .class_init = k230_machine_class_init,
    .instance_init = k230_machine_instance_init,
    .instance_आकार = परिमाण(K230MachineState),
};

स्थिर शून्य k230_machine_init_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&k230_machine_typeinfo);
}

type_init(k230_machine_init_पञ्जिकाकोशः_types)
