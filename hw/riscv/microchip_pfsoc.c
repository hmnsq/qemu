/*
 * QEMU RISC-V Board Compatible with Microchip PolarFire SoC Icicle Kit
 *
 * Copyright (c) 2020 Wind River Systems, Inc.
 *
 * Author:
 *   Bin Meng <bin.meng@windriver.com>
 *
 * Provides a board compatible with the Microchip PolarFire SoC Icicle Kit
 *
 * 0) CLINT (Core Level Interruptor)
 * 1) PLIC (Platform Level Interrupt Controller)
 * 2) eNVM (Embedded Non-Volatile Memory)
 * 3) MMUARTs (Multi-Mode UART)
 * 4) Cadence eMMC/SDHC controller and an SD card connected to it
 * 5) SiFive Platform DMA (Direct Memory Access Controller)
 * 6) GEM (Gigabit Ethernet MAC Controller)
 * 7) DMC (DDR Memory Controller)
 * 8) IOSCB modules
 *
 * This board currently generates devicetree dynamically that indicates at least
 * two harts and up to five harts.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "qemu/units.h"
#अन्तर्भाव "qemu/cutils.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qapi/visitor.h"
#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/core/loader.h"
#अन्तर्भाव "hw/core/sysbus.h"
#अन्तर्भाव "chardev/char.h"
#अन्तर्भाव "hw/cpu/cluster.h"
#अन्तर्भाव "target/riscv/cpu.h"
#अन्तर्भाव "hw/misc/unimp.h"
#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "hw/riscv/riscv_hart.h"
#अन्तर्भाव "hw/riscv/microchip_pfsoc.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "hw/intc/sifive_plic.h"
#अन्तर्भाव "system/device_tree.h"
#अन्तर्भाव "system/system.h"

/*
 * The BIOS image used by this machine is called Hart Software Services (HSS).
 * See https://github.com/polarfire-soc/hart-software-services
 */
#परिभाषय BIOS_FILENAME   "hss.bin"
#परिभाषय RESET_सदिशम्    0x20220000

/* GEM version */
#परिभाषय GEM_REVISION    0x0107010c

/*
 * The complete description of the whole PolarFire SoC memory map is scattered
 * in different documents. There are several places to look at for memory maps:
 *
 * 1 Chapter 11 "MSS Memory Map", in the doc "UG0880: PolarFire SoC FPGA
 *   Microprocessor Subsystem (MSS) User Guide", which can be downloaded from
 *   https://www.microsemi.com/document-portal/doc_download/
 *   1244570-ug0880-polarfire-soc-fpga-microprocessor-subsystem-mss-user-guide,
 *   describes the whole picture of the PolarFire SoC memory map.
 *
 * 2 A zip file for PolarFire soC memory map, which can be downloaded from
 *   https://www.microsemi.com/document-portal/doc_download/
 *   1244581-polarfire-soc-register-map, contains the following 2 major parts:
 *   - Register Map/PF_SoC_RegMap_V1_1/pfsoc_regmap.htm
 *     describes the complete integrated peripherals memory map
 *   - Register Map/PF_SoC_RegMap_V1_1/MPFS250T/mpfs250t_ioscb_memmap_dri.htm
 *     describes the complete IOSCB modules memory maps
 */
स्थिर ध्रुव Mem_Map_प्रविष्टिः microchip_pfsoc_memmap[] = {
    [MICROCHIP_PFSOC_RSVD0] =           {        0x0,        0x100 },
    [MICROCHIP_PFSOC_DEBUG] =           {      0x100,        0xf00 },
    [MICROCHIP_PFSOC_E51_DTIM] =        {  0x1000000,       0x2000 },
    [MICROCHIP_PFSOC_BUSERR_UNIT0] =    {  0x1700000,       0x1000 },
    [MICROCHIP_PFSOC_BUSERR_UNIT1] =    {  0x1701000,       0x1000 },
    [MICROCHIP_PFSOC_BUSERR_UNIT2] =    {  0x1702000,       0x1000 },
    [MICROCHIP_PFSOC_BUSERR_UNIT3] =    {  0x1703000,       0x1000 },
    [MICROCHIP_PFSOC_BUSERR_UNIT4] =    {  0x1704000,       0x1000 },
    [MICROCHIP_PFSOC_CLINT] =           {  0x2000000,      0x10000 },
    [MICROCHIP_PFSOC_L2CC] =            {  0x2010000,       0x1000 },
    [MICROCHIP_PFSOC_DMA] =             {  0x3000000,     0x100000 },
    [MICROCHIP_PFSOC_L2LIM] =           {  0x8000000,    0x2000000 },
    [MICROCHIP_PFSOC_PLIC] =            {  0xc000000,    0x4000000 },
    [MICROCHIP_PFSOC_MMUART0] =         { 0x20000000,       0x1000 },
    [MICROCHIP_PFSOC_WDOG0] =           { 0x20001000,       0x1000 },
    [MICROCHIP_PFSOC_SYSREG] =          { 0x20002000,       0x2000 },
    [MICROCHIP_PFSOC_AXISW] =           { 0x20004000,       0x1000 },
    [MICROCHIP_PFSOC_MPUCFG] =          { 0x20005000,       0x1000 },
    [MICROCHIP_PFSOC_FMETER] =          { 0x20006000,       0x1000 },
    [MICROCHIP_PFSOC_DDR_SGMII_PHY] =   { 0x20007000,       0x1000 },
    [MICROCHIP_PFSOC_EMMC_SD] =         { 0x20008000,       0x1000 },
    [MICROCHIP_PFSOC_DDR_CFG] =         { 0x20080000,      0x40000 },
    [MICROCHIP_PFSOC_MMUART1] =         { 0x20100000,       0x1000 },
    [MICROCHIP_PFSOC_MMUART2] =         { 0x20102000,       0x1000 },
    [MICROCHIP_PFSOC_MMUART3] =         { 0x20104000,       0x1000 },
    [MICROCHIP_PFSOC_MMUART4] =         { 0x20106000,       0x1000 },
    [MICROCHIP_PFSOC_WDOG1] =           { 0x20101000,       0x1000 },
    [MICROCHIP_PFSOC_WDOG2] =           { 0x20103000,       0x1000 },
    [MICROCHIP_PFSOC_WDOG3] =           { 0x20105000,       0x1000 },
    [MICROCHIP_PFSOC_WDOG4] =           { 0x20106000,       0x1000 },
    [MICROCHIP_PFSOC_SPI0] =            { 0x20108000,       0x1000 },
    [MICROCHIP_PFSOC_SPI1] =            { 0x20109000,       0x1000 },
    [MICROCHIP_PFSOC_I2C0] =            { 0x2010a000,       0x1000 },
    [MICROCHIP_PFSOC_I2C1] =            { 0x2010b000,       0x1000 },
    [MICROCHIP_PFSOC_CAN0] =            { 0x2010c000,       0x1000 },
    [MICROCHIP_PFSOC_CAN1] =            { 0x2010d000,       0x1000 },
    [MICROCHIP_PFSOC_GEM0] =            { 0x20110000,       0x2000 },
    [MICROCHIP_PFSOC_GEM1] =            { 0x20112000,       0x2000 },
    [MICROCHIP_PFSOC_GPIO0] =           { 0x20120000,       0x1000 },
    [MICROCHIP_PFSOC_GPIO1] =           { 0x20121000,       0x1000 },
    [MICROCHIP_PFSOC_GPIO2] =           { 0x20122000,       0x1000 },
    [MICROCHIP_PFSOC_RTC] =             { 0x20124000,       0x1000 },
    [MICROCHIP_PFSOC_ENVM_CFG] =        { 0x20200000,       0x1000 },
    [MICROCHIP_PFSOC_ENVM_दत्त] =       { 0x20220000,      0x20000 },
    [MICROCHIP_PFSOC_USB] =             { 0x20201000,       0x1000 },
    [MICROCHIP_PFSOC_QSPI_XIP] =        { 0x21000000,    0x1000000 },
    [MICROCHIP_PFSOC_IOSCB] =           { 0x30000000,   0x10000000 },
    [MICROCHIP_PFSOC_FABRIC_FIC0] =   { 0x2000000000, 0x1000000000 },
    [MICROCHIP_PFSOC_FABRIC_FIC1] =   { 0x3000000000, 0x1000000000 },
    [MICROCHIP_PFSOC_FABRIC_FIC3] =     { 0x40000000,   0x20000000 },
    [MICROCHIP_PFSOC_DRAM_LO] =         { 0x80000000,   0x40000000 },
    [MICROCHIP_PFSOC_DRAM_LO_ALIAS] =   { 0xc0000000,   0x40000000 },
    [MICROCHIP_PFSOC_DRAM_HI] =       { 0x1000000000,          0x0 },
    [MICROCHIP_PFSOC_DRAM_HI_ALIAS] = { 0x1400000000,          0x0 },

};

स्थिर शून्य microchip_pfsoc_soc_instance_init(Object *obj)
{
    MicrochipPFSoCState *s = MICROCHIP_PFSOC(obj);

    object_initialize_child(obj, "e-cluster", &s->e_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_समुच्चयः_uint32(DEVICE(&s->e_cluster), "cluster-id", 0);

    object_initialize_child(OBJECT(&s->e_cluster), "e-cpus", &s->e_cpus,
                            TYPE_RISCV_HART_ARRAY);
    qdev_prop_समुच्चयः_uint32(DEVICE(&s->e_cpus), "num-harts", 1);
    qdev_prop_समुच्चयः_uint32(DEVICE(&s->e_cpus), "hartid-base", 0);
    qdev_prop_समुच्चयः_string(DEVICE(&s->e_cpus), "cpu-type",
                         TYPE_RISCV_CPU_SIFIVE_E51);
    qdev_prop_समुच्चयः_uint64(DEVICE(&s->e_cpus), "resetvec", RESET_सदिशम्);

    object_initialize_child(obj, "u-cluster", &s->u_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_समुच्चयः_uint32(DEVICE(&s->u_cluster), "cluster-id", 1);

    object_initialize_child(OBJECT(&s->u_cluster), "u-cpus", &s->u_cpus,
                            TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    qdev_prop_समुच्चयः_uint32(DEVICE(&s->u_cpus), "hartid-base", 1);
    qdev_prop_समुच्चयः_string(DEVICE(&s->u_cpus), "cpu-type",
                         TYPE_RISCV_CPU_SIFIVE_U54);
    qdev_prop_समुच्चयः_uint64(DEVICE(&s->u_cpus), "resetvec", RESET_सदिशम्);

    object_initialize_child(obj, "dma-controller", &s->dma,
                            TYPE_SIFIVE_PDMA);

    object_initialize_child(obj, "sysreg", &s->sysreg,
                            TYPE_MCHP_PFSOC_SYSREG);

    object_initialize_child(obj, "ddr-sgmii-phy", &s->ddr_sgmii_phy,
                            TYPE_MCHP_PFSOC_DDR_SGMII_PHY);
    object_initialize_child(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_MCHP_PFSOC_DDR_CFG);

    object_initialize_child(obj, "gem0", &s->gem0, TYPE_CADENCE_GEM);
    object_initialize_child(obj, "gem1", &s->gem1, TYPE_CADENCE_GEM);

    object_initialize_child(obj, "sd-controller", &s->sdhci,
                            TYPE_CADENCE_SDHCI);

    object_initialize_child(obj, "ioscb", &s->ioscb, TYPE_MCHP_PFSOC_IOSCB);
}

स्थिर शून्य microchip_pfsoc_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    MicrochipIcicleKitState *iks = MICROCHIP_ICICLE_KIT_MACHINE(ms);
    MicrochipPFSoCState *s = MICROCHIP_PFSOC(dev);
    ध्रुव Mem_Map_प्रविष्टिः *memmap = microchip_pfsoc_memmap;
    स्मृतिः_Region *system_स्मृतिः = get_system_स्मृतिः();
    स्मृतिः_Region *rsvd0_mem = g_new(स्मृतिः_Region, 1);
    स्मृतिः_Region *e51_dtim_mem = g_new(स्मृतिः_Region, 1);
    स्मृतिः_Region *l2lim_mem = g_new(स्मृतिः_Region, 1);
    स्मृतिः_Region *envm_दत्त = g_new(स्मृतिः_Region, 1);
    स्मृतिः_Region *qspi_xip_mem = g_new(स्मृतिः_Region, 1);
    वर्ण *plic_hart_config;
    पूर्णाङ्क i;

    sysbus_realize(SYS_BUS_DEVICE(&s->e_cpus), &error_abort);
    qdev_prop_समुच्चयः_uint32(DEVICE(&s->u_cpus), "num-harts", ms->smp.cpus - 1);
    sysbus_realize(SYS_BUS_DEVICE(&s->u_cpus), &error_abort);
    /*
     * The cluster must be realized after the RISC-V hart array container,
     * as the container's CPU object is only created on realize, and the
     * CPU must exist and have been parented into the cluster before the
     * cluster is realized.
     */
    qdev_realize(DEVICE(&s->e_cluster), रिक्त, &error_abort);
    qdev_realize(DEVICE(&s->u_cluster), रिक्त, &error_abort);

    /* Reserved Memory at address 0 */
    स्मृतिः_region_init_ram(rsvd0_mem, रिक्त, "microchip.pfsoc.rsvd0_mem",
                           memmap[MICROCHIP_PFSOC_RSVD0].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_RSVD0].आधारः,
                                rsvd0_mem);

    /* E51 DTIM */
    स्मृतिः_region_init_ram(e51_dtim_mem, रिक्त, "microchip.pfsoc.e51_dtim_mem",
                           memmap[MICROCHIP_PFSOC_E51_DTIM].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_E51_DTIM].आधारः,
                                e51_dtim_mem);

    /* Bus Error Units */
    create_unimplemented_device("microchip.pfsoc.buserr_unit0_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT0].आधारः,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT0].आकार);
    create_unimplemented_device("microchip.pfsoc.buserr_unit1_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT1].आधारः,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT1].आकार);
    create_unimplemented_device("microchip.pfsoc.buserr_unit2_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT2].आधारः,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT2].आकार);
    create_unimplemented_device("microchip.pfsoc.buserr_unit3_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT3].आधारः,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT3].आकार);
    create_unimplemented_device("microchip.pfsoc.buserr_unit4_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT4].आधारः,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT4].आकार);

    /* CLINT */
    riscv_aclint_swi_create(system_स्मृतिः,
        memmap[MICROCHIP_PFSOC_CLINT].आधारः,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(system_स्मृतिः,
        memmap[MICROCHIP_PFSOC_CLINT].आधारः + RISCV_ACLINT_SWI_आकार,
        RISCV_ACLINT_DEFAULT_MTIMER_आकार, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        iks->clint_timebase_freq, false);

    /* L2 cache controller */
    create_unimplemented_device("microchip.pfsoc.l2cc",
        memmap[MICROCHIP_PFSOC_L2CC].आधारः, memmap[MICROCHIP_PFSOC_L2CC].आकार);

    /*
     * Add L2-LIM at reset size.
     * This should be reduced in size as the L2 Cache Controller WayEnable
     * register is incremented. Unfortunately I don't see a nice (or any) way
     * to handle reducing or blocking out the L2 LIM while still allowing it
     * be re returned to all enabled after a reset. For the time being, just
     * leave it enabled all the time. This won't break anything, but will be
     * too generous to misbehaving guests.
     */
    स्मृतिः_region_init_ram(l2lim_mem, रिक्त, "microchip.pfsoc.l2lim",
                           memmap[MICROCHIP_PFSOC_L2LIM].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_L2LIM].आधारः,
                                l2lim_mem);

    /* create PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(ms->smp.cpus);

    /* PLIC */
    s->plic = sifive_plic_create(system_स्मृतिः,
        memmap[MICROCHIP_PFSOC_PLIC].आधारः,
        plic_hart_config, ms->smp.cpus, 0,
        MICROCHIP_PFSOC_PLIC_अङ्कः_SOURCES,
        MICROCHIP_PFSOC_PLIC_अङ्कः_PRIORITIES,
        MICROCHIP_PFSOC_PLIC_प्राथमिकता_आधारः,
        MICROCHIP_PFSOC_PLIC_PENDING_आधारः,
        MICROCHIP_PFSOC_PLIC_ENABLE_आधारः,
        MICROCHIP_PFSOC_PLIC_ENABLE_STRIDE,
        MICROCHIP_PFSOC_PLIC_CONTEXT_आधारः,
        MICROCHIP_PFSOC_PLIC_CONTEXT_STRIDE,
        memmap[MICROCHIP_PFSOC_PLIC].आकार);
    g_free(plic_hart_config);

    /* DMA */
    sysbus_realize(SYS_BUS_DEVICE(&s->dma), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->dma), 0,
                    memmap[MICROCHIP_PFSOC_DMA].आधारः);
    प्रति (i = 0; i < SIFIVE_PDMA_IRQS; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->dma), i,
                           qdev_get_gpio_अन्तर्(DEVICE(s->plic),
                                            MICROCHIP_PFSOC_DMA_IRQ0 + i));
    }

    /* SYSREG */
    sysbus_realize(SYS_BUS_DEVICE(&s->sysreg), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sysreg), 0,
                    memmap[MICROCHIP_PFSOC_SYSREG].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sysreg), 0,
                       qdev_get_gpio_अन्तर्(DEVICE(s->plic),
                       MICROCHIP_PFSOC_MAILBOX_IRQ));

    /* AXISW */
    create_unimplemented_device("microchip.pfsoc.axisw",
        memmap[MICROCHIP_PFSOC_AXISW].आधारः,
        memmap[MICROCHIP_PFSOC_AXISW].आकार);

    /* MPUCFG */
    create_unimplemented_device("microchip.pfsoc.mpucfg",
        memmap[MICROCHIP_PFSOC_MPUCFG].आधारः,
        memmap[MICROCHIP_PFSOC_MPUCFG].आकार);

    /* FMETER */
    create_unimplemented_device("microchip.pfsoc.fmeter",
        memmap[MICROCHIP_PFSOC_FMETER].आधारः,
        memmap[MICROCHIP_PFSOC_FMETER].आकार);

    /* DDR SGMII PHY */
    sysbus_realize(SYS_BUS_DEVICE(&s->ddr_sgmii_phy), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_sgmii_phy), 0,
                    memmap[MICROCHIP_PFSOC_DDR_SGMII_PHY].आधारः);

    /* DDR CFG */
    sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[MICROCHIP_PFSOC_DDR_CFG].आधारः);

    /* SDHCI */
    sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0,
                    memmap[MICROCHIP_PFSOC_EMMC_SD].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
        qdev_get_gpio_अन्तर्(DEVICE(s->plic), MICROCHIP_PFSOC_EMMC_SD_IRQ));

    /* MMUARTs */
    s->serial0 = mchp_pfsoc_mmuart_create(system_स्मृतिः,
        memmap[MICROCHIP_PFSOC_MMUART0].आधारः,
        qdev_get_gpio_अन्तर्(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART0_IRQ),
        serial_hd(0));
    s->serial1 = mchp_pfsoc_mmuart_create(system_स्मृतिः,
        memmap[MICROCHIP_PFSOC_MMUART1].आधारः,
        qdev_get_gpio_अन्तर्(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART1_IRQ),
        serial_hd(1));
    s->serial2 = mchp_pfsoc_mmuart_create(system_स्मृतिः,
        memmap[MICROCHIP_PFSOC_MMUART2].आधारः,
        qdev_get_gpio_अन्तर्(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART2_IRQ),
        serial_hd(2));
    s->serial3 = mchp_pfsoc_mmuart_create(system_स्मृतिः,
        memmap[MICROCHIP_PFSOC_MMUART3].आधारः,
        qdev_get_gpio_अन्तर्(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART3_IRQ),
        serial_hd(3));
    s->serial4 = mchp_pfsoc_mmuart_create(system_स्मृतिः,
        memmap[MICROCHIP_PFSOC_MMUART4].आधारः,
        qdev_get_gpio_अन्तर्(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART4_IRQ),
        serial_hd(4));

    /* Watchdogs */
    create_unimplemented_device("microchip.pfsoc.watchdog0",
        memmap[MICROCHIP_PFSOC_WDOG0].आधारः,
        memmap[MICROCHIP_PFSOC_WDOG0].आकार);
    create_unimplemented_device("microchip.pfsoc.watchdog1",
        memmap[MICROCHIP_PFSOC_WDOG1].आधारः,
        memmap[MICROCHIP_PFSOC_WDOG1].आकार);
    create_unimplemented_device("microchip.pfsoc.watchdog2",
        memmap[MICROCHIP_PFSOC_WDOG2].आधारः,
        memmap[MICROCHIP_PFSOC_WDOG2].आकार);
    create_unimplemented_device("microchip.pfsoc.watchdog3",
        memmap[MICROCHIP_PFSOC_WDOG3].आधारः,
        memmap[MICROCHIP_PFSOC_WDOG3].आकार);
    create_unimplemented_device("microchip.pfsoc.watchdog4",
        memmap[MICROCHIP_PFSOC_WDOG4].आधारः,
        memmap[MICROCHIP_PFSOC_WDOG4].आकार);

    /* SPI */
    create_unimplemented_device("microchip.pfsoc.spi0",
        memmap[MICROCHIP_PFSOC_SPI0].आधारः,
        memmap[MICROCHIP_PFSOC_SPI0].आकार);
    create_unimplemented_device("microchip.pfsoc.spi1",
        memmap[MICROCHIP_PFSOC_SPI1].आधारः,
        memmap[MICROCHIP_PFSOC_SPI1].आकार);

    /* I2C */
    create_unimplemented_device("microchip.pfsoc.i2c0",
        memmap[MICROCHIP_PFSOC_I2C0].आधारः,
        memmap[MICROCHIP_PFSOC_I2C0].आकार);
    create_unimplemented_device("microchip.pfsoc.i2c1",
        memmap[MICROCHIP_PFSOC_I2C1].आधारः,
        memmap[MICROCHIP_PFSOC_I2C1].आकार);

    /* CAN */
    create_unimplemented_device("microchip.pfsoc.can0",
        memmap[MICROCHIP_PFSOC_CAN0].आधारः,
        memmap[MICROCHIP_PFSOC_CAN0].आकार);
    create_unimplemented_device("microchip.pfsoc.can1",
        memmap[MICROCHIP_PFSOC_CAN1].आधारः,
        memmap[MICROCHIP_PFSOC_CAN1].आकार);

    /* USB */
    create_unimplemented_device("microchip.pfsoc.usb",
        memmap[MICROCHIP_PFSOC_USB].आधारः,
        memmap[MICROCHIP_PFSOC_USB].आकार);

    /* GEMs */
    qemu_configure_nic_device(DEVICE(&s->gem0), true, रिक्त);
    qemu_configure_nic_device(DEVICE(&s->gem1), true, रिक्त);

    object_property_समुच्चयः_int(OBJECT(&s->gem0), "revision", GEM_REVISION, errp);
    object_property_समुच्चयः_int(OBJECT(&s->gem0), "phy-addr", 8, errp);
    object_property_समुच्चयः_bool(OBJECT(&s->gem0), "phy-connected", false, errp);
    object_property_समुच्चयः_bool(OBJECT(&s->gem0), "pcs-enabled", true, errp);

    sysbus_realize(SYS_BUS_DEVICE(&s->gem0), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem0), 0,
                    memmap[MICROCHIP_PFSOC_GEM0].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem0), 0,
        qdev_get_gpio_अन्तर्(DEVICE(s->plic), MICROCHIP_PFSOC_GEM0_IRQ));

    object_property_समुच्चयः_int(OBJECT(&s->gem1), "revision", GEM_REVISION, errp);
    object_property_समुच्चयः_int(OBJECT(&s->gem1), "phy-addr", 9, errp);
    object_property_समुच्चयः_link(OBJECT(&s->gem1), "phy-consumer",
                             OBJECT(&s->gem0), errp);
    object_property_समुच्चयः_bool(OBJECT(&s->gem1), "pcs-enabled", true, errp);
    sysbus_realize(SYS_BUS_DEVICE(&s->gem1), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem1), 0,
                    memmap[MICROCHIP_PFSOC_GEM1].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem1), 0,
        qdev_get_gpio_अन्तर्(DEVICE(s->plic), MICROCHIP_PFSOC_GEM1_IRQ));

    /* GPIOs */
    create_unimplemented_device("microchip.pfsoc.gpio0",
        memmap[MICROCHIP_PFSOC_GPIO0].आधारः,
        memmap[MICROCHIP_PFSOC_GPIO0].आकार);
    create_unimplemented_device("microchip.pfsoc.gpio1",
        memmap[MICROCHIP_PFSOC_GPIO1].आधारः,
        memmap[MICROCHIP_PFSOC_GPIO1].आकार);
    create_unimplemented_device("microchip.pfsoc.gpio2",
        memmap[MICROCHIP_PFSOC_GPIO2].आधारः,
        memmap[MICROCHIP_PFSOC_GPIO2].आकार);

    /* eNVM */
    स्मृतिः_region_init_rom(envm_दत्त, OBJECT(dev), "microchip.pfsoc.envm.data",
                           memmap[MICROCHIP_PFSOC_ENVM_दत्त].आकार,
                           &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_ENVM_दत्त].आधारः,
                                envm_दत्त);

    /* IOSCB */
    sysbus_realize(SYS_BUS_DEVICE(&s->ioscb), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ioscb), 0,
                    memmap[MICROCHIP_PFSOC_IOSCB].आधारः);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->ioscb), 0,
                       qdev_get_gpio_अन्तर्(DEVICE(s->plic),
                       MICROCHIP_PFSOC_MAILBOX_IRQ));

    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic3",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC3].आधारः,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC3].आकार);
    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic0",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC0].आधारः,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC0].आकार);
    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic1",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC1].आधारः,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC1].आकार);

    /* QSPI Flash */
    स्मृतिः_region_init_rom(qspi_xip_mem, OBJECT(dev),
                           "microchip.pfsoc.qspi_xip",
                           memmap[MICROCHIP_PFSOC_QSPI_XIP].आकार,
                           &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_QSPI_XIP].आधारः,
                                qspi_xip_mem);
}

स्थिर शून्य microchip_pfsoc_soc_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = microchip_pfsoc_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

स्थिर ध्रुव TypeInfo microchip_pfsoc_soc_type_info = {
    .name = TYPE_MICROCHIP_PFSOC,
    .parent = TYPE_DEVICE,
    .instance_आकार = परिमाण(MicrochipPFSoCState),
    .instance_init = microchip_pfsoc_soc_instance_init,
    .class_init = microchip_pfsoc_soc_class_init,
};

स्थिर शून्य microchip_pfsoc_soc_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&microchip_pfsoc_soc_type_info);
}

type_init(microchip_pfsoc_soc_पञ्जिकाकोशः_types)

स्थिर शून्य microchip_icicle_kit_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    ध्रुव Mem_Map_प्रविष्टिः *memmap = microchip_pfsoc_memmap;
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(machine);
    स्मृतिः_Region *system_स्मृतिः = get_system_स्मृतिः();
    स्मृतिः_Region *mem_नीचः = g_new(स्मृतिः_Region, 1);
    स्मृतिः_Region *mem_नीचः_alias = g_new(स्मृतिः_Region, 1);
    स्मृतिः_Region *mem_उच्चः = g_new(स्मृतिः_Region, 1);
    स्मृतिः_Region *mem_उच्चः_alias = g_new(स्मृतिः_Region, 1);
    uint64_t mem_नीचः_आकार, mem_उच्चः_आकार;
    hwaddr firmware_आरोपय_addr;
    ध्रुव वर्ण *firmware_name;
    hwaddr firmware_अन्तः_addr;
    vaddr kernel_start_addr;
    uint64_t kernel_प्रविष्टिः;
    uint64_t fdt_आरोपय_addr;
    DriveInfo *dinfo = drive_get(IF_SD, 0, 0);
    RISCVBootInfo boot_info;

    /* Sanity check on RAM size */
    यदि (machine->ram_आकार < mc->default_ram_आकार) {
        वर्ण *sz = आकार_गन्तव्यम्_str(mc->default_ram_आकार);
        error_report("Invalid RAM size, should be bigger than %s", sz);
        g_free(sz);
        निर्गच्छ(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            TYPE_MICROCHIP_PFSOC);
    qdev_realize(DEVICE(&s->soc), रिक्त, &error_fatal);

    /* Split RAM into low and high regions using aliases to machine->ram */
    mem_नीचः_आकार = memmap[MICROCHIP_PFSOC_DRAM_LO].आकार;
    mem_उच्चः_आकार = machine->ram_आकार - mem_नीचः_आकार;
    स्मृतिः_region_init_alias(mem_नीचः, रिक्त,
                             "microchip.icicle.kit.ram_low", machine->ram,
                             0, mem_नीचः_आकार);
    स्मृतिः_region_init_alias(mem_उच्चः, रिक्त,
                             "microchip.icicle.kit.ram_high", machine->ram,
                             mem_नीचः_आकार, mem_उच्चः_आकार);

    /* Register RAM */
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_DRAM_LO].आधारः,
                                mem_नीचः);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_DRAM_HI].आधारः,
                                mem_उच्चः);

    /* Create aliases for the low and high RAM regions */
    स्मृतिः_region_init_alias(mem_नीचः_alias, रिक्त,
                             "microchip.icicle.kit.ram_low.alias",
                             mem_नीचः, 0, mem_नीचः_आकार);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_DRAM_LO_ALIAS].आधारः,
                                mem_नीचः_alias);
    स्मृतिः_region_init_alias(mem_उच्चः_alias, रिक्त,
                             "microchip.icicle.kit.ram_high.alias",
                             mem_उच्चः, 0, mem_उच्चः_आकार);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                memmap[MICROCHIP_PFSOC_DRAM_HI_ALIAS].आधारः,
                                mem_उच्चः_alias);

    /* Attach an SD card */
    यदि (dinfo) {
        CadenceSDHCIState *sdhci = &(s->soc.sdhci);
        DeviceState *card = qdev_new(TYPE_SD_CARD);

        qdev_prop_समुच्चयः_drive_err(card, "drive", blk_by_legacy_dinfo(dinfo),
                                &error_fatal);
        qdev_realize_and_unref(card, sdhci->bus, &error_fatal);
    }

    /*
     * We follow the following table to select which firmware we use.
     *
     * -bios         | -kernel    | firmware
     * --------------+------------+--------
     * none          |          N | error
     * none          |          Y | kernel
     * NULL, default |          N | BIOS_FILENAME
     * NULL, default |          Y | RISCV64_BIOS_BIN
     * other         | don't care | other
     */
    यदि (machine->firmware && !वाक्यं_तुलय(machine->firmware, "none")) {
        यदि (!machine->kernel_filename) {
            error_report("for -bios none, a kernel is required");
            निर्गच्छ(1);
        }

        firmware_name = रिक्त;
        firmware_आरोपय_addr = RESET_सदिशम्;
    } अन्यथा यदि (!machine->firmware || !वाक्यं_तुलय(machine->firmware, "default")) {
        यदि (machine->kernel_filename) {
            firmware_name = RISCV64_BIOS_BIN;
            firmware_आरोपय_addr = memmap[MICROCHIP_PFSOC_DRAM_LO].आधारः;
        } अन्यथा {
            firmware_name = BIOS_FILENAME;
            firmware_आरोपय_addr = RESET_सदिशम्;
        }
    } अन्यथा {
        firmware_name = machine->firmware;
        firmware_आरोपय_addr = RESET_सदिशम्;
    }

    riscv_boot_info_init_discontig_mem(&boot_info, &s->soc.u_cpus,
                                       memmap[MICROCHIP_PFSOC_DRAM_LO].आधारः,
                                       mem_नीचः_आकार);

    /* Load the firmware if necessary */
    firmware_अन्तः_addr = firmware_आरोपय_addr;
    यदि (firmware_name) {
        वर्ण *filename = riscv_find_firmware(firmware_name, रिक्त);
        यदि (filename) {
            firmware_अन्तः_addr = riscv_आरोपय_firmware(machine, &boot_info,
                                                    filename,
                                                    &firmware_आरोपय_addr, रिक्त);
            g_free(filename);
        }
    }

    यदि (machine->kernel_filename) {
        kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                         firmware_अन्तः_addr);

        riscv_आरोपय_kernel(machine, &boot_info, kernel_start_addr,
                          true, रिक्त);
        kernel_प्रविष्टिः = boot_info.image_नीचः_addr;

        यदि (machine->dtb) {
            पूर्णाङ्क fdt_आकार;
            machine->fdt = आरोपय_device_वृक्षः(machine->dtb, &fdt_आकार);
            यदि (!machine->fdt) {
                error_report("load_device_tree() failed");
                निर्गच्छ(1);
            }

            /* Compute the FDT load address in DRAM */
            hwaddr kernel_ram_आधारः = memmap[MICROCHIP_PFSOC_DRAM_LO].आधारः;
            hwaddr kernel_ram_आकार = memmap[MICROCHIP_PFSOC_DRAM_LO].आकार;

            यदि (kernel_प्रविष्टिः - kernel_ram_आधारः >= kernel_ram_आकार) {
                kernel_ram_आधारः = memmap[MICROCHIP_PFSOC_DRAM_HI].आधारः;
                kernel_ram_आकार = mem_उच्चः_आकार;
            }

            fdt_आरोपय_addr = riscv_compute_fdt_addr(kernel_ram_आधारः, kernel_ram_आकार,
                                                   machine, &boot_info);
            riscv_आरोपय_fdt(fdt_आरोपय_addr, machine->fdt);
        } अन्यथा {
            warn_report_once("The QEMU microchip-icicle-kit machine does not "
                             "generate a device tree, so no device tree is "
                             "being provided to the guest.");
            fdt_आरोपय_addr = 0;
        }

        hwaddr start_addr;
        यदि (firmware_name) {
            start_addr = firmware_आरोपय_addr;
        } अन्यथा {
            start_addr = kernel_प्रविष्टिः;
        }

        /* Load the reset vector */
        riscv_सज्जता_rom_reset_vec(machine, &s->soc.u_cpus, start_addr,
                                  memmap[MICROCHIP_PFSOC_ENVM_दत्त].आधारः,
                                  memmap[MICROCHIP_PFSOC_ENVM_दत्त].आकार,
                                  kernel_प्रविष्टिः, fdt_आरोपय_addr);
    }
}

स्थिर शून्य microchip_icicle_kit_समुच्चयः_clint_timebase_freq(Object *obj,
                                                         Visitor *v,
                                                         ध्रुव वर्ण *name,
                                                         शून्य *opaque,
                                                         Error **errp)
{
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    uint32_t मानम्;

    यदि (!दर्शय_type_uint32(v, name, &मानम्, errp)) {
        प्रत्यागम;
    }

    s->clint_timebase_freq = मानम्;
}

स्थिर शून्य microchip_icicle_kit_get_clint_timebase_freq(Object *obj,
                                                         Visitor *v,
                                                         ध्रुव वर्ण *name,
                                                         शून्य *opaque,
                                                         Error **errp)
{
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    uint32_t मानम् = s->clint_timebase_freq;

    दर्शय_type_uint32(v, name, &मानम्, errp);
}

स्थिर शून्य microchip_icicle_kit_machine_instance_init(Object *obj)
{
    MicrochipIcicleKitState *m = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    m->clint_timebase_freq = 1000000;
}

स्थिर शून्य microchip_icicle_kit_machine_class_init(ObjectClass *oc,
                                                    ध्रुव शून्य *दत्त)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Microchip PolarFire SoC Icicle Kit";
    mc->init = microchip_icicle_kit_machine_init;
    mc->max_cpus = MICROCHIP_PFSOC_MANAGEMENT_CPU_गणनम् +
                   MICROCHIP_PFSOC_COMPUTE_CPU_गणनम्;
    mc->min_cpus = MICROCHIP_PFSOC_MANAGEMENT_CPU_गणनम् + 1;
    mc->default_cpus = mc->min_cpus;
    mc->default_ram_id = "microchip.icicle.kit.ram";
    mc->auto_create_sdcard = true;

    /*
     * Map 513 MiB high memory, the minimum required high memory size, because
     * HSS will do memory test against the high memory address range regardless
     * of physical memory installed.
     *
     * See memory_tests() in mss_ddr.c in the HSS source code.
     */
    mc->default_ram_आकार = 1537 * MiB;

    object_class_property_सङ्कलय(oc, "clint-timebase-frequency", "uint32_t",
                              microchip_icicle_kit_get_clint_timebase_freq,
                              microchip_icicle_kit_समुच्चयः_clint_timebase_freq,
                              रिक्त, रिक्त);
    object_class_property_समुच्चयः_description(oc, "clint-timebase-frequency",
                                  "Set CLINT timebase frequency in Hz.");
}

स्थिर ध्रुव TypeInfo microchip_icicle_kit_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("microchip-icicle-kit"),
    .parent     = TYPE_MACHINE,
    .class_init = microchip_icicle_kit_machine_class_init,
    .instance_init = microchip_icicle_kit_machine_instance_init,
    .instance_आकार = परिमाण(MicrochipIcicleKitState),
};

स्थिर शून्य microchip_icicle_kit_machine_init_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&microchip_icicle_kit_machine_typeinfo);
}

type_init(microchip_icicle_kit_machine_init_पञ्जिकाकोशः_types)
