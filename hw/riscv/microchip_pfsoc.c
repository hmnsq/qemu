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

#ضمن "qemu/osdep.h"
#ضمن "qemu/error-report.h"
#ضمن "qemu/units.h"
#ضمن "qemu/cutils.h"
#ضمن "qapi/error.h"
#ضمن "qapi/visitor.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/core/sysbus.h"
#ضمن "chardev/char.h"
#ضمن "hw/cpu/cluster.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/misc/unimp.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "hw/riscv/microchip_pfsoc.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/sifive_plic.h"
#ضمن "system/device_tree.h"
#ضمن "system/system.h"

/*
 * The BIOS image used by this machine is called Hart Software Services (HSS).
 * See https://github.com/polarfire-soc/hart-software-services
 */
#وصف_ڪريو BIOS_FILENAME   "hss.bin"
#وصف_ڪريو RESET_ويڪٽر    0x20220000

/* GEM version */
#وصف_ڪريو GEM_REVISION    0x0107010c

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
ساكن ثابت Mem_Map_داخلا microchip_pfsoc_memmap[] = {
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
    [MICROCHIP_PFSOC_ENVM_معلومات] =       { 0x20220000,      0x20000 },
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

ساكن فراغ microchip_pfsoc_soc_instance_init(Object *obj)
{
    MicrochipPFSoCState *s = MICROCHIP_PFSOC(obj);

    object_initialize_ٻار(obj, "e-cluster", &s->e_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_سيٽ_uint32(DEVICE(&s->e_cluster), "cluster-id", 0);

    object_initialize_ٻار(OBJECT(&s->e_cluster), "e-cpus", &s->e_cpus,
                            TYPE_RISCV_HART_ARRAY);
    qdev_prop_سيٽ_uint32(DEVICE(&s->e_cpus), "num-harts", 1);
    qdev_prop_سيٽ_uint32(DEVICE(&s->e_cpus), "hartid-base", 0);
    qdev_prop_سيٽ_string(DEVICE(&s->e_cpus), "cpu-type",
                         TYPE_RISCV_CPU_SIFIVE_E51);
    qdev_prop_سيٽ_uint64(DEVICE(&s->e_cpus), "resetvec", RESET_ويڪٽر);

    object_initialize_ٻار(obj, "u-cluster", &s->u_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_سيٽ_uint32(DEVICE(&s->u_cluster), "cluster-id", 1);

    object_initialize_ٻار(OBJECT(&s->u_cluster), "u-cpus", &s->u_cpus,
                            TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    qdev_prop_سيٽ_uint32(DEVICE(&s->u_cpus), "hartid-base", 1);
    qdev_prop_سيٽ_string(DEVICE(&s->u_cpus), "cpu-type",
                         TYPE_RISCV_CPU_SIFIVE_U54);
    qdev_prop_سيٽ_uint64(DEVICE(&s->u_cpus), "resetvec", RESET_ويڪٽر);

    object_initialize_ٻار(obj, "dma-controller", &s->dma,
                            TYPE_SIFIVE_PDMA);

    object_initialize_ٻار(obj, "sysreg", &s->sysreg,
                            TYPE_MCHP_PFSOC_SYSREG);

    object_initialize_ٻار(obj, "ddr-sgmii-phy", &s->ddr_sgmii_phy,
                            TYPE_MCHP_PFSOC_DDR_SGMII_PHY);
    object_initialize_ٻار(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_MCHP_PFSOC_DDR_CFG);

    object_initialize_ٻار(obj, "gem0", &s->gem0, TYPE_CADENCE_GEM);
    object_initialize_ٻار(obj, "gem1", &s->gem1, TYPE_CADENCE_GEM);

    object_initialize_ٻار(obj, "sd-controller", &s->sdhci,
                            TYPE_CADENCE_SDHCI);

    object_initialize_ٻار(obj, "ioscb", &s->ioscb, TYPE_MCHP_PFSOC_IOSCB);
}

ساكن فراغ microchip_pfsoc_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    MicrochipIcicleKitState *iks = MICROCHIP_ICICLE_KIT_MACHINE(ms);
    MicrochipPFSoCState *s = MICROCHIP_PFSOC(dev);
    ثابت Mem_Map_داخلا *memmap = microchip_pfsoc_memmap;
    يادگيري_Region *system_يادگيري = get_system_يادگيري();
    يادگيري_Region *rsvd0_mem = g_new(يادگيري_Region, 1);
    يادگيري_Region *e51_dtim_mem = g_new(يادگيري_Region, 1);
    يادگيري_Region *l2lim_mem = g_new(يادگيري_Region, 1);
    يادگيري_Region *envm_معلومات = g_new(يادگيري_Region, 1);
    يادگيري_Region *qspi_xip_mem = g_new(يادگيري_Region, 1);
    حرف *plic_hart_config;
    صحيح i;

    sysbus_realize(SYS_BUS_DEVICE(&s->e_cpus), &error_abort);
    qdev_prop_سيٽ_uint32(DEVICE(&s->u_cpus), "num-harts", ms->smp.cpus - 1);
    sysbus_realize(SYS_BUS_DEVICE(&s->u_cpus), &error_abort);
    /*
     * The cluster must be realized after the RISC-V hart array container,
     * as the container's CPU object is only created on realize, and the
     * CPU must exist and have been parented into the cluster before the
     * cluster is realized.
     */
    qdev_realize(DEVICE(&s->e_cluster), عدم, &error_abort);
    qdev_realize(DEVICE(&s->u_cluster), عدم, &error_abort);

    /* Reserved Memory at address 0 */
    يادگيري_region_init_ram(rsvd0_mem, عدم, "microchip.pfsoc.rsvd0_mem",
                           memmap[MICROCHIP_PFSOC_RSVD0].سائيز, &error_fatal);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_RSVD0].بنياد,
                                rsvd0_mem);

    /* E51 DTIM */
    يادگيري_region_init_ram(e51_dtim_mem, عدم, "microchip.pfsoc.e51_dtim_mem",
                           memmap[MICROCHIP_PFSOC_E51_DTIM].سائيز, &error_fatal);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_E51_DTIM].بنياد,
                                e51_dtim_mem);

    /* Bus Error Units */
    create_unimplemented_device("microchip.pfsoc.buserr_unit0_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT0].بنياد,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT0].سائيز);
    create_unimplemented_device("microchip.pfsoc.buserr_unit1_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT1].بنياد,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT1].سائيز);
    create_unimplemented_device("microchip.pfsoc.buserr_unit2_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT2].بنياد,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT2].سائيز);
    create_unimplemented_device("microchip.pfsoc.buserr_unit3_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT3].بنياد,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT3].سائيز);
    create_unimplemented_device("microchip.pfsoc.buserr_unit4_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT4].بنياد,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT4].سائيز);

    /* CLINT */
    riscv_aclint_swi_create(system_يادگيري,
        memmap[MICROCHIP_PFSOC_CLINT].بنياد,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(system_يادگيري,
        memmap[MICROCHIP_PFSOC_CLINT].بنياد + RISCV_ACLINT_SWI_سائيز,
        RISCV_ACLINT_DEFAULT_MTIMER_سائيز, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        iks->clint_timebase_freq, false);

    /* L2 cache controller */
    create_unimplemented_device("microchip.pfsoc.l2cc",
        memmap[MICROCHIP_PFSOC_L2CC].بنياد, memmap[MICROCHIP_PFSOC_L2CC].سائيز);

    /*
     * Add L2-LIM at reset size.
     * This should be reduced in size as the L2 Cache Controller WayEnable
     * register is incremented. Unfortunately I don't see a nice (or any) way
     * to handle reducing or blocking out the L2 LIM while still allowing it
     * be re returned to all enabled after a reset. For the time being, just
     * leave it enabled all the time. This won't break anything, but will be
     * too generous to misbehaving guests.
     */
    يادگيري_region_init_ram(l2lim_mem, عدم, "microchip.pfsoc.l2lim",
                           memmap[MICROCHIP_PFSOC_L2LIM].سائيز, &error_fatal);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_L2LIM].بنياد,
                                l2lim_mem);

    /* create PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(ms->smp.cpus);

    /* PLIC */
    s->plic = sifive_plic_create(system_يادگيري,
        memmap[MICROCHIP_PFSOC_PLIC].بنياد,
        plic_hart_config, ms->smp.cpus, 0,
        MICROCHIP_PFSOC_PLIC_انگ_SOURCES,
        MICROCHIP_PFSOC_PLIC_انگ_PRIORITIES,
        MICROCHIP_PFSOC_PLIC_ترجيح_بنياد,
        MICROCHIP_PFSOC_PLIC_PENDING_بنياد,
        MICROCHIP_PFSOC_PLIC_ENABLE_بنياد,
        MICROCHIP_PFSOC_PLIC_ENABLE_STRIDE,
        MICROCHIP_PFSOC_PLIC_CONTEXT_بنياد,
        MICROCHIP_PFSOC_PLIC_CONTEXT_STRIDE,
        memmap[MICROCHIP_PFSOC_PLIC].سائيز);
    g_free(plic_hart_config);

    /* DMA */
    sysbus_realize(SYS_BUS_DEVICE(&s->dma), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->dma), 0,
                    memmap[MICROCHIP_PFSOC_DMA].بنياد);
    لكل (i = 0; i < SIFIVE_PDMA_IRQS; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->dma), i,
                           qdev_get_gpio_اندريون(DEVICE(s->plic),
                                            MICROCHIP_PFSOC_DMA_IRQ0 + i));
    }

    /* SYSREG */
    sysbus_realize(SYS_BUS_DEVICE(&s->sysreg), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sysreg), 0,
                    memmap[MICROCHIP_PFSOC_SYSREG].بنياد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sysreg), 0,
                       qdev_get_gpio_اندريون(DEVICE(s->plic),
                       MICROCHIP_PFSOC_MAILBOX_IRQ));

    /* AXISW */
    create_unimplemented_device("microchip.pfsoc.axisw",
        memmap[MICROCHIP_PFSOC_AXISW].بنياد,
        memmap[MICROCHIP_PFSOC_AXISW].سائيز);

    /* MPUCFG */
    create_unimplemented_device("microchip.pfsoc.mpucfg",
        memmap[MICROCHIP_PFSOC_MPUCFG].بنياد,
        memmap[MICROCHIP_PFSOC_MPUCFG].سائيز);

    /* FMETER */
    create_unimplemented_device("microchip.pfsoc.fmeter",
        memmap[MICROCHIP_PFSOC_FMETER].بنياد,
        memmap[MICROCHIP_PFSOC_FMETER].سائيز);

    /* DDR SGMII PHY */
    sysbus_realize(SYS_BUS_DEVICE(&s->ddr_sgmii_phy), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_sgmii_phy), 0,
                    memmap[MICROCHIP_PFSOC_DDR_SGMII_PHY].بنياد);

    /* DDR CFG */
    sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[MICROCHIP_PFSOC_DDR_CFG].بنياد);

    /* SDHCI */
    sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0,
                    memmap[MICROCHIP_PFSOC_EMMC_SD].بنياد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
        qdev_get_gpio_اندريون(DEVICE(s->plic), MICROCHIP_PFSOC_EMMC_SD_IRQ));

    /* MMUARTs */
    s->serial0 = mchp_pfsoc_mmuart_create(system_يادگيري,
        memmap[MICROCHIP_PFSOC_MMUART0].بنياد,
        qdev_get_gpio_اندريون(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART0_IRQ),
        serial_hd(0));
    s->serial1 = mchp_pfsoc_mmuart_create(system_يادگيري,
        memmap[MICROCHIP_PFSOC_MMUART1].بنياد,
        qdev_get_gpio_اندريون(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART1_IRQ),
        serial_hd(1));
    s->serial2 = mchp_pfsoc_mmuart_create(system_يادگيري,
        memmap[MICROCHIP_PFSOC_MMUART2].بنياد,
        qdev_get_gpio_اندريون(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART2_IRQ),
        serial_hd(2));
    s->serial3 = mchp_pfsoc_mmuart_create(system_يادگيري,
        memmap[MICROCHIP_PFSOC_MMUART3].بنياد,
        qdev_get_gpio_اندريون(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART3_IRQ),
        serial_hd(3));
    s->serial4 = mchp_pfsoc_mmuart_create(system_يادگيري,
        memmap[MICROCHIP_PFSOC_MMUART4].بنياد,
        qdev_get_gpio_اندريون(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART4_IRQ),
        serial_hd(4));

    /* Watchdogs */
    create_unimplemented_device("microchip.pfsoc.watchdog0",
        memmap[MICROCHIP_PFSOC_WDOG0].بنياد,
        memmap[MICROCHIP_PFSOC_WDOG0].سائيز);
    create_unimplemented_device("microchip.pfsoc.watchdog1",
        memmap[MICROCHIP_PFSOC_WDOG1].بنياد,
        memmap[MICROCHIP_PFSOC_WDOG1].سائيز);
    create_unimplemented_device("microchip.pfsoc.watchdog2",
        memmap[MICROCHIP_PFSOC_WDOG2].بنياد,
        memmap[MICROCHIP_PFSOC_WDOG2].سائيز);
    create_unimplemented_device("microchip.pfsoc.watchdog3",
        memmap[MICROCHIP_PFSOC_WDOG3].بنياد,
        memmap[MICROCHIP_PFSOC_WDOG3].سائيز);
    create_unimplemented_device("microchip.pfsoc.watchdog4",
        memmap[MICROCHIP_PFSOC_WDOG4].بنياد,
        memmap[MICROCHIP_PFSOC_WDOG4].سائيز);

    /* SPI */
    create_unimplemented_device("microchip.pfsoc.spi0",
        memmap[MICROCHIP_PFSOC_SPI0].بنياد,
        memmap[MICROCHIP_PFSOC_SPI0].سائيز);
    create_unimplemented_device("microchip.pfsoc.spi1",
        memmap[MICROCHIP_PFSOC_SPI1].بنياد,
        memmap[MICROCHIP_PFSOC_SPI1].سائيز);

    /* I2C */
    create_unimplemented_device("microchip.pfsoc.i2c0",
        memmap[MICROCHIP_PFSOC_I2C0].بنياد,
        memmap[MICROCHIP_PFSOC_I2C0].سائيز);
    create_unimplemented_device("microchip.pfsoc.i2c1",
        memmap[MICROCHIP_PFSOC_I2C1].بنياد,
        memmap[MICROCHIP_PFSOC_I2C1].سائيز);

    /* CAN */
    create_unimplemented_device("microchip.pfsoc.can0",
        memmap[MICROCHIP_PFSOC_CAN0].بنياد,
        memmap[MICROCHIP_PFSOC_CAN0].سائيز);
    create_unimplemented_device("microchip.pfsoc.can1",
        memmap[MICROCHIP_PFSOC_CAN1].بنياد,
        memmap[MICROCHIP_PFSOC_CAN1].سائيز);

    /* USB */
    create_unimplemented_device("microchip.pfsoc.usb",
        memmap[MICROCHIP_PFSOC_USB].بنياد,
        memmap[MICROCHIP_PFSOC_USB].سائيز);

    /* GEMs */
    qemu_configure_nic_device(DEVICE(&s->gem0), true, عدم);
    qemu_configure_nic_device(DEVICE(&s->gem1), true, عدم);

    object_property_سيٽ_int(OBJECT(&s->gem0), "revision", GEM_REVISION, errp);
    object_property_سيٽ_int(OBJECT(&s->gem0), "phy-addr", 8, errp);
    object_property_سيٽ_bool(OBJECT(&s->gem0), "phy-connected", false, errp);
    object_property_سيٽ_bool(OBJECT(&s->gem0), "pcs-enabled", true, errp);

    sysbus_realize(SYS_BUS_DEVICE(&s->gem0), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem0), 0,
                    memmap[MICROCHIP_PFSOC_GEM0].بنياد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem0), 0,
        qdev_get_gpio_اندريون(DEVICE(s->plic), MICROCHIP_PFSOC_GEM0_IRQ));

    object_property_سيٽ_int(OBJECT(&s->gem1), "revision", GEM_REVISION, errp);
    object_property_سيٽ_int(OBJECT(&s->gem1), "phy-addr", 9, errp);
    object_property_سيٽ_link(OBJECT(&s->gem1), "phy-consumer",
                             OBJECT(&s->gem0), errp);
    object_property_سيٽ_bool(OBJECT(&s->gem1), "pcs-enabled", true, errp);
    sysbus_realize(SYS_BUS_DEVICE(&s->gem1), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem1), 0,
                    memmap[MICROCHIP_PFSOC_GEM1].بنياد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem1), 0,
        qdev_get_gpio_اندريون(DEVICE(s->plic), MICROCHIP_PFSOC_GEM1_IRQ));

    /* GPIOs */
    create_unimplemented_device("microchip.pfsoc.gpio0",
        memmap[MICROCHIP_PFSOC_GPIO0].بنياد,
        memmap[MICROCHIP_PFSOC_GPIO0].سائيز);
    create_unimplemented_device("microchip.pfsoc.gpio1",
        memmap[MICROCHIP_PFSOC_GPIO1].بنياد,
        memmap[MICROCHIP_PFSOC_GPIO1].سائيز);
    create_unimplemented_device("microchip.pfsoc.gpio2",
        memmap[MICROCHIP_PFSOC_GPIO2].بنياد,
        memmap[MICROCHIP_PFSOC_GPIO2].سائيز);

    /* eNVM */
    يادگيري_region_init_rom(envm_معلومات, OBJECT(dev), "microchip.pfsoc.envm.data",
                           memmap[MICROCHIP_PFSOC_ENVM_معلومات].سائيز,
                           &error_fatal);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_ENVM_معلومات].بنياد,
                                envm_معلومات);

    /* IOSCB */
    sysbus_realize(SYS_BUS_DEVICE(&s->ioscb), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ioscb), 0,
                    memmap[MICROCHIP_PFSOC_IOSCB].بنياد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->ioscb), 0,
                       qdev_get_gpio_اندريون(DEVICE(s->plic),
                       MICROCHIP_PFSOC_MAILBOX_IRQ));

    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic3",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC3].بنياد,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC3].سائيز);
    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic0",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC0].بنياد,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC0].سائيز);
    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic1",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC1].بنياد,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC1].سائيز);

    /* QSPI Flash */
    يادگيري_region_init_rom(qspi_xip_mem, OBJECT(dev),
                           "microchip.pfsoc.qspi_xip",
                           memmap[MICROCHIP_PFSOC_QSPI_XIP].سائيز,
                           &error_fatal);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_QSPI_XIP].بنياد,
                                qspi_xip_mem);
}

ساكن فراغ microchip_pfsoc_soc_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = microchip_pfsoc_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

ساكن ثابت TypeInfo microchip_pfsoc_soc_type_info = {
    .name = TYPE_MICROCHIP_PFSOC,
    .والد = TYPE_DEVICE,
    .instance_سائيز = حجم(MicrochipPFSoCState),
    .instance_init = microchip_pfsoc_soc_instance_init,
    .class_init = microchip_pfsoc_soc_class_init,
};

ساكن فراغ microchip_pfsoc_soc_رجسٽر_types(فراغ)
{
    type_رجسٽر_static(&microchip_pfsoc_soc_type_info);
}

type_init(microchip_pfsoc_soc_رجسٽر_types)

ساكن فراغ microchip_icicle_kit_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    ثابت Mem_Map_داخلا *memmap = microchip_pfsoc_memmap;
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(machine);
    يادگيري_Region *system_يادگيري = get_system_يادگيري();
    يادگيري_Region *mem_هيٺيون = g_new(يادگيري_Region, 1);
    يادگيري_Region *mem_هيٺيون_alias = g_new(يادگيري_Region, 1);
    يادگيري_Region *mem_مٿاهون = g_new(يادگيري_Region, 1);
    يادگيري_Region *mem_مٿاهون_alias = g_new(يادگيري_Region, 1);
    uint64_t mem_هيٺيون_سائيز, mem_مٿاهون_سائيز;
    hwaddr firmware_ڀريو_addr;
    ثابت حرف *firmware_name;
    hwaddr firmware_پڇاڙي_addr;
    vaddr kernel_شروعات_addr;
    uint64_t kernel_داخلا;
    uint64_t fdt_ڀريو_addr;
    DriveInfo *dinfo = drive_get(IF_SD, 0, 0);
    RISCVBootInfo boot_info;

    /* Sanity check on RAM size */
    إذا (machine->ram_سائيز < mc->default_ram_سائيز) {
        حرف *sz = سائيز_منزل_str(mc->default_ram_سائيز);
        error_report("Invalid RAM size, should be bigger than %s", sz);
        g_free(sz);
        ٻاهر_نڪرو(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_ٻار(OBJECT(machine), "soc", &s->soc,
                            TYPE_MICROCHIP_PFSOC);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    /* Split RAM into low and high regions using aliases to machine->ram */
    mem_هيٺيون_سائيز = memmap[MICROCHIP_PFSOC_DRAM_LO].سائيز;
    mem_مٿاهون_سائيز = machine->ram_سائيز - mem_هيٺيون_سائيز;
    يادگيري_region_init_alias(mem_هيٺيون, عدم,
                             "microchip.icicle.kit.ram_low", machine->ram,
                             0, mem_هيٺيون_سائيز);
    يادگيري_region_init_alias(mem_مٿاهون, عدم,
                             "microchip.icicle.kit.ram_high", machine->ram,
                             mem_هيٺيون_سائيز, mem_مٿاهون_سائيز);

    /* Register RAM */
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_DRAM_LO].بنياد,
                                mem_هيٺيون);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_DRAM_HI].بنياد,
                                mem_مٿاهون);

    /* Create aliases for the low and high RAM regions */
    يادگيري_region_init_alias(mem_هيٺيون_alias, عدم,
                             "microchip.icicle.kit.ram_low.alias",
                             mem_هيٺيون, 0, mem_هيٺيون_سائيز);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_DRAM_LO_ALIAS].بنياد,
                                mem_هيٺيون_alias);
    يادگيري_region_init_alias(mem_مٿاهون_alias, عدم,
                             "microchip.icicle.kit.ram_high.alias",
                             mem_مٿاهون, 0, mem_مٿاهون_سائيز);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[MICROCHIP_PFSOC_DRAM_HI_ALIAS].بنياد,
                                mem_مٿاهون_alias);

    /* Attach an SD card */
    إذا (dinfo) {
        CadenceSDHCIState *sdhci = &(s->soc.sdhci);
        DeviceState *card = qdev_new(TYPE_SD_CARD);

        qdev_prop_سيٽ_drive_err(card, "drive", blk_by_legacy_dinfo(dinfo),
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
    إذا (machine->firmware && !strcmp(machine->firmware, "none")) {
        إذا (!machine->kernel_filename) {
            error_report("for -bios none, a kernel is required");
            ٻاهر_نڪرو(1);
        }

        firmware_name = عدم;
        firmware_ڀريو_addr = RESET_ويڪٽر;
    } وإلا إذا (!machine->firmware || !strcmp(machine->firmware, "default")) {
        إذا (machine->kernel_filename) {
            firmware_name = RISCV64_BIOS_BIN;
            firmware_ڀريو_addr = memmap[MICROCHIP_PFSOC_DRAM_LO].بنياد;
        } وإلا {
            firmware_name = BIOS_FILENAME;
            firmware_ڀريو_addr = RESET_ويڪٽر;
        }
    } وإلا {
        firmware_name = machine->firmware;
        firmware_ڀريو_addr = RESET_ويڪٽر;
    }

    riscv_boot_info_init_discontig_mem(&boot_info, &s->soc.u_cpus,
                                       memmap[MICROCHIP_PFSOC_DRAM_LO].بنياد,
                                       mem_هيٺيون_سائيز);

    /* Load the firmware if necessary */
    firmware_پڇاڙي_addr = firmware_ڀريو_addr;
    إذا (firmware_name) {
        حرف *filename = riscv_ڳوليو_firmware(firmware_name, عدم);
        إذا (filename) {
            firmware_پڇاڙي_addr = riscv_ڀريو_firmware(machine, &boot_info,
                                                    filename,
                                                    &firmware_ڀريو_addr, عدم);
            g_free(filename);
        }
    }

    إذا (machine->kernel_filename) {
        kernel_شروعات_addr = riscv_calc_kernel_شروعات_addr(&boot_info,
                                                         firmware_پڇاڙي_addr);

        riscv_ڀريو_kernel(machine, &boot_info, kernel_شروعات_addr,
                          true, عدم);
        kernel_داخلا = boot_info.image_هيٺيون_addr;

        إذا (machine->dtb) {
            صحيح fdt_سائيز;
            machine->fdt = ڀريو_device_وڻ(machine->dtb, &fdt_سائيز);
            إذا (!machine->fdt) {
                error_report("load_device_tree() failed");
                ٻاهر_نڪرو(1);
            }

            /* Compute the FDT load address in DRAM */
            hwaddr kernel_ram_بنياد = memmap[MICROCHIP_PFSOC_DRAM_LO].بنياد;
            hwaddr kernel_ram_سائيز = memmap[MICROCHIP_PFSOC_DRAM_LO].سائيز;

            إذا (kernel_داخلا - kernel_ram_بنياد >= kernel_ram_سائيز) {
                kernel_ram_بنياد = memmap[MICROCHIP_PFSOC_DRAM_HI].بنياد;
                kernel_ram_سائيز = mem_مٿاهون_سائيز;
            }

            fdt_ڀريو_addr = riscv_compute_fdt_addr(kernel_ram_بنياد, kernel_ram_سائيز,
                                                   machine, &boot_info);
            riscv_ڀريو_fdt(fdt_ڀريو_addr, machine->fdt);
        } وإلا {
            warn_report_once("The QEMU microchip-icicle-kit machine does not "
                             "generate a device tree, so no device tree is "
                             "being provided to the guest.");
            fdt_ڀريو_addr = 0;
        }

        hwaddr شروعات_addr;
        إذا (firmware_name) {
            شروعات_addr = firmware_ڀريو_addr;
        } وإلا {
            شروعات_addr = kernel_داخلا;
        }

        /* Load the reset vector */
        riscv_تياري_rom_reset_vec(machine, &s->soc.u_cpus, شروعات_addr,
                                  memmap[MICROCHIP_PFSOC_ENVM_معلومات].بنياد,
                                  memmap[MICROCHIP_PFSOC_ENVM_معلومات].سائيز,
                                  kernel_داخلا, fdt_ڀريو_addr);
    }
}

ساكن فراغ microchip_icicle_kit_سيٽ_clint_timebase_freq(Object *obj,
                                                         Visitor *v,
                                                         ثابت حرف *name,
                                                         فراغ *opaque,
                                                         Error **errp)
{
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    uint32_t قدر;

    إذا (!گهمو_type_uint32(v, name, &قدر, errp)) {
        أرجع;
    }

    s->clint_timebase_freq = قدر;
}

ساكن فراغ microchip_icicle_kit_get_clint_timebase_freq(Object *obj,
                                                         Visitor *v,
                                                         ثابت حرف *name,
                                                         فراغ *opaque,
                                                         Error **errp)
{
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    uint32_t قدر = s->clint_timebase_freq;

    گهمو_type_uint32(v, name, &قدر, errp);
}

ساكن فراغ microchip_icicle_kit_machine_instance_init(Object *obj)
{
    MicrochipIcicleKitState *m = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    m->clint_timebase_freq = 1000000;
}

ساكن فراغ microchip_icicle_kit_machine_class_init(ObjectClass *oc,
                                                    ثابت فراغ *معلومات)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Microchip PolarFire SoC Icicle Kit";
    mc->init = microchip_icicle_kit_machine_init;
    mc->max_cpus = MICROCHIP_PFSOC_MANAGEMENT_CPU_ڳڻپ +
                   MICROCHIP_PFSOC_COMPUTE_CPU_ڳڻپ;
    mc->min_cpus = MICROCHIP_PFSOC_MANAGEMENT_CPU_ڳڻپ + 1;
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
    mc->default_ram_سائيز = 1537 * MiB;

    object_class_property_جوڙيو(oc, "clint-timebase-frequency", "uint32_t",
                              microchip_icicle_kit_get_clint_timebase_freq,
                              microchip_icicle_kit_سيٽ_clint_timebase_freq,
                              عدم, عدم);
    object_class_property_سيٽ_description(oc, "clint-timebase-frequency",
                                  "Set CLINT timebase frequency in Hz.");
}

ساكن ثابت TypeInfo microchip_icicle_kit_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("microchip-icicle-kit"),
    .والد     = TYPE_MACHINE,
    .class_init = microchip_icicle_kit_machine_class_init,
    .instance_init = microchip_icicle_kit_machine_instance_init,
    .instance_سائيز = حجم(MicrochipIcicleKitState),
};

ساكن فراغ microchip_icicle_kit_machine_init_رجسٽر_types(فراغ)
{
    type_رجسٽر_static(&microchip_icicle_kit_machine_typeinfo);
}

type_init(microchip_icicle_kit_machine_init_رجسٽر_types)
