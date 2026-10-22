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

#समावेश "qemu/osdep.h"
#समावेश "cpu-qom.h"
#समावेश "qemu/cutils.h"
#समावेश "qemu/error-report.h"
#समावेश "qapi/error.h"
#समावेश "system/device_tree.h"
#समावेश "system/system.h"
#समावेश "system/memory.h"
#समावेश "target/riscv/cpu.h"
#समावेश "hw/core/loader.h"
#समावेश "hw/core/sysbus.h"
#समावेश "hw/riscv/k230.h"
#समावेश "hw/riscv/boot.h"
#समावेश "hw/intc/riscv_aclint.h"
#समावेश "hw/intc/sifive_plic.h"
#समावेश "hw/char/serial-mm.h"
#समावेश "hw/misc/unimp.h"

/* Align K230_SDK k230_canmv_defconfig */
#घोषणा K230_DIRECT_OPENSBI_ADDR 0x8000000
#घोषणा K230_DIRECT_KERNEL_ADDR  0x8200000
#घोषणा K230_DIRECT_DTB_ADDR     0xa000000

अटल स्थिर MemMapEntry memmap[] = {
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

अटल व्योम k230_soc_init(Object *obj)
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

    qdev_prop_set_uint32(DEVICE(cpu0), "hartid-base", 0);
    qdev_prop_set_string(DEVICE(cpu0), "cpu-type", TYPE_RISCV_CPU_THEAD_C908);
    qdev_prop_set_uint64(DEVICE(cpu0), "resetvec",
                         memmap[K230_DEV_BOOTROM].base);
}

अटल DeviceState *k230_create_plic(पूर्णांक base_hartid, पूर्णांक hartid_गिनती)
{
    g_autofree अक्षर *plic_hart_config = शून्य;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hartid_गिनती);

    /* Per-socket PLIC */
    वापस sifive_plic_create(get_system_memory(),
                              memmap[K230_DEV_PLIC].base,
                              plic_hart_config, hartid_गिनती, base_hartid,
                              K230_PLIC_अंक_SOURCES,
                              K230_PLIC_अंक_PRIORITIES,
                              K230_PLIC_PRIORITY_BASE, K230_PLIC_PENDING_BASE,
                              K230_PLIC_ENABLE_BASE, K230_PLIC_ENABLE_STRIDE,
                              K230_PLIC_CONTEXT_BASE,
                              K230_PLIC_CONTEXT_STRIDE,
                              memmap[K230_DEV_PLIC].आकार);
}

अटल व्योम k230_create_uart(MemoryRegion *sys_mem, DeviceState *plic,
                             पूर्णांक सूचकांक)
{
    पूर्णांक uart_dev = K230_DEV_UART0 + सूचकांक;
    g_autofree अक्षर *name = g_strdup_printf("uart%d", सूचकांक);

    /* Cover the non-16550 part of the SDK's 0x1000 UART window. */
    create_unimplemented_device(name, memmap[uart_dev].base,
                                memmap[uart_dev].आकार);

    serial_mm_init(sys_mem, memmap[uart_dev].base, 2,
                   qdev_get_gpio_in(plic, K230_UART0_IRQ + सूचकांक),
                   399193, serial_hd(सूचकांक), DEVICE_LITTLE_ENDIAN);
}

अटल व्योम k230_soc_realize(DeviceState *dev, Error **errp)
{
    K230SoCState *s = RISCV_K230_SOC(dev);
    MemoryRegion *sys_mem = get_system_memory();
    पूर्णांक c908_cpus;

    sysbus_realize(SYS_BUS_DEVICE(&s->c908_cpu), &error_fatal);
    अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp)) {
        वापस;
    }
    अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->ddr_phy), errp)) {
        वापस;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[K230_DEV_DDRC_CFG].base);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_phy), 0,
                    memmap[K230_DEV_DDR_PHY].base);

    c908_cpus = s->c908_cpu.अंक_harts;

    /* SRAM */
    memory_region_init_ram(&s->sram, OBJECT(dev), "sram",
                           memmap[K230_DEV_SRAM].आकार, &error_fatal);
    memory_region_add_subregion(sys_mem, memmap[K230_DEV_SRAM].base,
                                &s->sram);

    /* BootROM */
    memory_region_init_rom(&s->bootrom, OBJECT(dev), "bootrom",
                           memmap[K230_DEV_BOOTROM].आकार, &error_fatal);
    memory_region_add_subregion(sys_mem, memmap[K230_DEV_BOOTROM].base,
                                &s->bootrom);

    /* PLIC */
    s->c908_plic = k230_create_plic(C908_CPU_HARTID, c908_cpus);

    /* CLINT */
    riscv_aclint_swi_create(sys_mem,
                            memmap[K230_DEV_CLINT].base,
                            C908_CPU_HARTID, c908_cpus, false);
    riscv_aclint_mtimer_create(sys_mem,
                               memmap[K230_DEV_CLINT].base + 0x4000,
                               RISCV_ACLINT_DEFAULT_MTIMER_आकार,
                               C908_CPU_HARTID, c908_cpus,
                               RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

    /* UART */
    क्रम (पूर्णांक i = 0; i < K230_UART_गिनती; i++) {
        k230_create_uart(sys_mem, DEVICE(s->c908_plic), i);
    }

    /* Watchdog */
    क्रम (पूर्णांक i = 0; i < 2; i++) {
        अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->wdt[i]), errp)) {
            वापस;
        }
    }

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[0]), 0, memmap[K230_DEV_WDT0].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[0]), 0,
                       qdev_get_gpio_in(DEVICE(s->c908_plic), K230_WDT0_IRQ));

    sysbus_mmio_map(SYS_BUS_DEVICE(&s->wdt[1]), 0, memmap[K230_DEV_WDT1].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->wdt[1]), 0,
                       qdev_get_gpio_in(DEVICE(s->c908_plic), K230_WDT1_IRQ));

    /* Gsdma */
    अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->gsdma), errp)) {
        वापस;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gsdma), 0, memmap[K230_DEV_GSDMA].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gsdma), 0,
                       qdev_get_gpio_in(DEVICE(s->c908_plic), K230_GSDMA_IRQ));

    /* Decomp gzip */
    qdev_prop_set_uint64(DEVICE(&s->decomp_gzip), "sram-base",
                         memmap[K230_DEV_SRAM].base);
    अगर (!sysbus_realize(SYS_BUS_DEVICE(&s->decomp_gzip), errp)) {
        वापस;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->decomp_gzip), 0,
                    memmap[K230_DEV_DECOMP_GZIP].base);
    क्रम (पूर्णांक i = 0; i < K230_DECOMP_GZIP_अंक_GPIOS_OUT; i++) {
        qdev_connect_gpio_out(DEVICE(&s->decomp_gzip), i,
                              qdev_get_gpio_in(DEVICE(&s->gsdma), i));
    }
    क्रम (पूर्णांक i = 0; i < K230_DECOMP_GZIP_अंक_GPIOS_IN; i++) {
        qdev_connect_gpio_out(DEVICE(&s->gsdma), i,
                              qdev_get_gpio_in(DEVICE(&s->decomp_gzip), i));
    }

    /* Noc stub, the k230 decomp_gzip driver in uboot will access this region */
    create_unimplemented_device("noc-stub", 0x91302000, 0x1000);

    /* unimplemented devices */
    create_unimplemented_device("kpu.l2-cache",
                                memmap[K230_DEV_KPU_L2_CACHE].base,
                                memmap[K230_DEV_KPU_L2_CACHE].आकार);

    create_unimplemented_device("kpu_cfg", memmap[K230_DEV_KPU_CFG].base,
                                memmap[K230_DEV_KPU_CFG].आकार);

    create_unimplemented_device("fft", memmap[K230_DEV_FFT].base,
                                memmap[K230_DEV_FFT].आकार);

    create_unimplemented_device("2d-engine.ai",
                                memmap[K230_DEV_AI_2D_ENGINE].base,
                                memmap[K230_DEV_AI_2D_ENGINE].आकार);

    create_unimplemented_device("dma", memmap[K230_DEV_DMA].base,
                                memmap[K230_DEV_DMA].आकार);

    create_unimplemented_device("2d-engine.non-ai",
                                memmap[K230_DEV_NON_AI_2D].base,
                                memmap[K230_DEV_NON_AI_2D].आकार);

    create_unimplemented_device("isp", memmap[K230_DEV_ISP].base,
                                memmap[K230_DEV_ISP].आकार);

    create_unimplemented_device("dewarp", memmap[K230_DEV_DEWARP].base,
                                memmap[K230_DEV_DEWARP].आकार);

    create_unimplemented_device("rx-csi", memmap[K230_DEV_RX_CSI].base,
                                memmap[K230_DEV_RX_CSI].आकार);

    create_unimplemented_device("vpu", memmap[K230_DEV_H264].base,
                                memmap[K230_DEV_H264].आकार);

    create_unimplemented_device("gpu", memmap[K230_DEV_2P5D].base,
                                memmap[K230_DEV_2P5D].आकार);

    create_unimplemented_device("vo", memmap[K230_DEV_VO].base,
                                memmap[K230_DEV_VO].आकार);

    create_unimplemented_device("vo_cfg", memmap[K230_DEV_VO_CFG].base,
                                memmap[K230_DEV_VO_CFG].आकार);

    create_unimplemented_device("3d-engine", memmap[K230_DEV_3D_ENGINE].base,
                                memmap[K230_DEV_3D_ENGINE].आकार);

    create_unimplemented_device("pmu", memmap[K230_DEV_PMU].base,
                                memmap[K230_DEV_PMU].आकार);

    create_unimplemented_device("rtc", memmap[K230_DEV_RTC].base,
                                memmap[K230_DEV_RTC].आकार);

    create_unimplemented_device("cmu", memmap[K230_DEV_CMU].base,
                                memmap[K230_DEV_CMU].आकार);

    create_unimplemented_device("rmu", memmap[K230_DEV_RMU].base,
                                memmap[K230_DEV_RMU].आकार);

    create_unimplemented_device("boot", memmap[K230_DEV_BOOT].base,
                                memmap[K230_DEV_BOOT].आकार);

    create_unimplemented_device("pwr", memmap[K230_DEV_PWR].base,
                                memmap[K230_DEV_PWR].आकार);

    create_unimplemented_device("ipcm", memmap[K230_DEV_MAILBOX].base,
                                memmap[K230_DEV_MAILBOX].आकार);

    create_unimplemented_device("iomux", memmap[K230_DEV_IOMUX].base,
                                memmap[K230_DEV_IOMUX].आकार);

    create_unimplemented_device("timer", memmap[K230_DEV_TIMER].base,
                                memmap[K230_DEV_TIMER].आकार);

    create_unimplemented_device("wdt0", memmap[K230_DEV_WDT0].base,
                                memmap[K230_DEV_WDT0].आकार);

    create_unimplemented_device("wdt1", memmap[K230_DEV_WDT1].base,
                                memmap[K230_DEV_WDT1].आकार);

    create_unimplemented_device("ts", memmap[K230_DEV_TS].base,
                                memmap[K230_DEV_TS].आकार);

    create_unimplemented_device("hdi", memmap[K230_DEV_HDI].base,
                                memmap[K230_DEV_HDI].आकार);

    create_unimplemented_device("stc", memmap[K230_DEV_STC].base,
                                memmap[K230_DEV_STC].आकार);

    create_unimplemented_device("security", memmap[K230_DEV_SECURITY].base,
                                memmap[K230_DEV_SECURITY].आकार);

    create_unimplemented_device("i2c0", memmap[K230_DEV_I2C0].base,
                                memmap[K230_DEV_I2C0].आकार);

    create_unimplemented_device("i2c1", memmap[K230_DEV_I2C1].base,
                                memmap[K230_DEV_I2C1].आकार);

    create_unimplemented_device("i2c2", memmap[K230_DEV_I2C2].base,
                                memmap[K230_DEV_I2C2].आकार);

    create_unimplemented_device("i2c3", memmap[K230_DEV_I2C3].base,
                                memmap[K230_DEV_I2C3].आकार);

    create_unimplemented_device("i2c4", memmap[K230_DEV_I2C4].base,
                                memmap[K230_DEV_I2C4].आकार);

    create_unimplemented_device("pwm", memmap[K230_DEV_PWM].base,
                                memmap[K230_DEV_PWM].आकार);

    create_unimplemented_device("gpio0", memmap[K230_DEV_GPIO0].base,
                                memmap[K230_DEV_GPIO0].आकार);

    create_unimplemented_device("gpio1", memmap[K230_DEV_GPIO1].base,
                                memmap[K230_DEV_GPIO1].आकार);

    create_unimplemented_device("adc", memmap[K230_DEV_ADC].base,
                                memmap[K230_DEV_ADC].आकार);

    create_unimplemented_device("codec", memmap[K230_DEV_CODEC].base,
                                memmap[K230_DEV_CODEC].आकार);

    create_unimplemented_device("i2s", memmap[K230_DEV_I2S].base,
                                memmap[K230_DEV_I2S].आकार);

    create_unimplemented_device("usb0", memmap[K230_DEV_USB0].base,
                                memmap[K230_DEV_USB0].आकार);

    create_unimplemented_device("usb1", memmap[K230_DEV_USB1].base,
                                memmap[K230_DEV_USB1].आकार);

    create_unimplemented_device("sd0", memmap[K230_DEV_SD0].base,
                                memmap[K230_DEV_SD0].आकार);

    create_unimplemented_device("sd1", memmap[K230_DEV_SD1].base,
                                memmap[K230_DEV_SD1].आकार);

    create_unimplemented_device("qspi0", memmap[K230_DEV_QSPI0].base,
                                memmap[K230_DEV_QSPI0].आकार);

    create_unimplemented_device("qspi1", memmap[K230_DEV_QSPI1].base,
                                memmap[K230_DEV_QSPI1].आकार);

    create_unimplemented_device("spi", memmap[K230_DEV_SPI].base,
                                memmap[K230_DEV_SPI].आकार);

    create_unimplemented_device("hi_sys_cfg", memmap[K230_DEV_HI_SYS_CFG].base,
                                memmap[K230_DEV_HI_SYS_CFG].आकार);

    create_unimplemented_device("flash", memmap[K230_DEV_FLASH].base,
                                memmap[K230_DEV_FLASH].आकार);
}

अटल व्योम k230_soc_class_init(ObjectClass *oc, स्थिर व्योम *तथ्य)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = k230_soc_realize;
}

अटल स्थिर TypeInfo k230_soc_type_info = {
    .name = TYPE_RISCV_K230_SOC,
    .parent = TYPE_DEVICE,
    .instance_आकार = माप(K230SoCState),
    .instance_init = k230_soc_init,
    .class_init = k230_soc_class_init,
};

अटल व्योम k230_soc_register_types(व्योम)
{
    type_register_static(&k230_soc_type_info);
}

type_init(k230_soc_register_types)

अटल व्योम k230_direct_boot(K230MachineState *s, MachineState *machine)
{
    स्थिर अक्षर *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    RISCVBootInfo boot_info = {0};
    hwaddr start_addr = K230_DIRECT_OPENSBI_ADDR;
    hwaddr firmware_end_addr = 0;
    hwaddr kernel_entry = 0;
    पूर्णांक fdt_आकार = 0;

    अगर (machine->firmware && !म_भेद(machine->firmware, "none")) {
        error_report("K230 direct boot requires OpenSBI firmware; omit "
                     "-bios none or pass OpenSBI with -bios");
        निकास(निकास_त्रुटि);
    }

    अगर (!machine->dtb) {
        error_report("K230 direct boot requires -dtb");
        निकास(निकास_त्रुटि);
    }

    machine->fdt = load_device_वृक्ष(machine->dtb, &fdt_आकार);
    अगर (!machine->fdt) {
        error_report("load_device_tree() failed");
        निकास(निकास_त्रुटि);
    }

    qemu_fdt_add_path(machine->fdt, "/chosen");

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_load_kernel(machine, &boot_info, K230_DIRECT_KERNEL_ADDR, true, शून्य);
    kernel_entry = boot_info.image_निचला_addr;

    riscv_load_fdt(K230_DIRECT_DTB_ADDR, machine->fdt);

    firmware_end_addr = riscv_find_and_load_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &start_addr, शून्य);
    अगर (firmware_end_addr > K230_DIRECT_KERNEL_ADDR) {
        error_report("K230 firmware overlaps kernel address 0x%x",
                     K230_DIRECT_KERNEL_ADDR);
        निकास(निकास_त्रुटि);
    }

    riscv_setup_rom_reset_vec(machine, &s->soc.c908_cpu, start_addr,
                              memmap[K230_DEV_BOOTROM].base,
                              memmap[K230_DEV_BOOTROM].आकार, kernel_entry,
                              K230_DIRECT_DTB_ADDR);
}

अटल व्योम k230_firmware_boot(K230MachineState *s, MachineState *machine)
{
    स्थिर अक्षर *firmware_name = riscv_default_firmware_name(&s->soc.c908_cpu);
    hwaddr start_addr = memmap[K230_DEV_DDRC].base;
    RISCVBootInfo boot_info = {0};

    अगर (machine->dtb || (machine->kernel_cmdline && *machine->kernel_cmdline)) {
        error_report("K230 firmware boot does not support -dtb or -append");
        निकास(निकास_त्रुटि);
    }

    riscv_boot_info_init(&boot_info, &s->soc.c908_cpu);
    riscv_find_and_load_firmware(machine, &boot_info, firmware_name,
                                 &start_addr, शून्य);

    riscv_setup_rom_reset_vec(machine, &s->soc.c908_cpu, start_addr,
                              memmap[K230_DEV_BOOTROM].base,
                              memmap[K230_DEV_BOOTROM].आकार, 0, 0);
}

अटल व्योम k230_machine_done(Notifier *notifier, व्योम *तथ्य)
{
    K230MachineState *s = container_of(notifier, K230MachineState,
                                       machine_done);
    MachineState *machine = MACHINE(s);

    अगर (machine->kernel_filename) {
        k230_direct_boot(s, machine);
    } अन्यथा {
        k230_firmware_boot(s, machine);
    }
}

अटल व्योम k230_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    K230MachineState *s = RISCV_K230_MACHINE(machine);
    MemoryRegion *sys_mem = get_system_memory();

    अगर (machine->ram_आकार < mc->default_ram_आकार) {
        अक्षर *sz = आकार_to_str(mc->default_ram_आकार);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        निकास(निकास_त्रुटि);
    }

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_K230_SOC);
    qdev_realize(DEVICE(&s->soc), शून्य, &error_fatal);

    /* Data Memory */
    memory_region_add_subregion(sys_mem, memmap[K230_DEV_DDRC].base,
                                machine->ram);

    s->machine_done.notify = k230_machine_done;
    qemu_add_machine_init_done_notifier(&s->machine_done);
}

अटल व्योम k230_machine_instance_init(Object *obj)
{
}

अटल व्योम k230_machine_class_init(ObjectClass *oc, स्थिर व्योम *तथ्य)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with Kendryte K230 SDK";
    mc->init = k230_machine_init;
    mc->default_cpus = 1;
    mc->default_ram_id = "riscv.K230.ram"; /* DDR */
    mc->default_ram_आकार = memmap[K230_DEV_DDRC].आकार;
}

अटल स्थिर TypeInfo k230_machine_typeinfo = {
    .name       = TYPE_RISCV_K230_MACHINE,
    .parent     = TYPE_MACHINE,
    .class_init = k230_machine_class_init,
    .instance_init = k230_machine_instance_init,
    .instance_आकार = माप(K230MachineState),
};

अटल व्योम k230_machine_init_register_types(व्योम)
{
    type_register_static(&k230_machine_typeinfo);
}

type_init(k230_machine_init_register_types)
