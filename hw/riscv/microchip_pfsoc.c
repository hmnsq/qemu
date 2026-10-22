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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qemu/units.h"
#అన్తర్భావ "qemu/cutils.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qapi/visitor.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "chardev/char.h"
#అన్తర్భావ "hw/cpu/cluster.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/misc/unimp.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "hw/riscv/microchip_pfsoc.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/sifive_plic.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "system/system.h"

/*
 * The BIOS image used by this machine is called Hart Software Services (HSS).
 * See https://github.com/polarfire-soc/hart-software-services
 */
#నిర్వచించు BIOS_FILENAME   "hss.bin"
#నిర్వచించు RESET_సదిశ    0x20220000

/* GEM version */
#నిర్వచించు GEM_REVISION    0x0107010c

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
స్థిర స్థిరరాశి Mem_Map_నమోదు microchip_pfsoc_memmap[] = {
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
    [MICROCHIP_PFSOC_ENVM_దత్తాంశం] =       { 0x20220000,      0x20000 },
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

స్థిర శూన్య microchip_pfsoc_soc_instance_init(Object *obj)
{
    MicrochipPFSoCState *s = MICROCHIP_PFSOC(obj);

    object_initialize_బిడ్డ(obj, "e-cluster", &s->e_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_సమితి_uint32(DEVICE(&s->e_cluster), "cluster-id", 0);

    object_initialize_బిడ్డ(OBJECT(&s->e_cluster), "e-cpus", &s->e_cpus,
                            TYPE_RISCV_HART_ARRAY);
    qdev_prop_సమితి_uint32(DEVICE(&s->e_cpus), "num-harts", 1);
    qdev_prop_సమితి_uint32(DEVICE(&s->e_cpus), "hartid-base", 0);
    qdev_prop_సమితి_string(DEVICE(&s->e_cpus), "cpu-type",
                         TYPE_RISCV_CPU_SIFIVE_E51);
    qdev_prop_సమితి_uint64(DEVICE(&s->e_cpus), "resetvec", RESET_సదిశ);

    object_initialize_బిడ్డ(obj, "u-cluster", &s->u_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_సమితి_uint32(DEVICE(&s->u_cluster), "cluster-id", 1);

    object_initialize_బిడ్డ(OBJECT(&s->u_cluster), "u-cpus", &s->u_cpus,
                            TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    qdev_prop_సమితి_uint32(DEVICE(&s->u_cpus), "hartid-base", 1);
    qdev_prop_సమితి_string(DEVICE(&s->u_cpus), "cpu-type",
                         TYPE_RISCV_CPU_SIFIVE_U54);
    qdev_prop_సమితి_uint64(DEVICE(&s->u_cpus), "resetvec", RESET_సదిశ);

    object_initialize_బిడ్డ(obj, "dma-controller", &s->dma,
                            TYPE_SIFIVE_PDMA);

    object_initialize_బిడ్డ(obj, "sysreg", &s->sysreg,
                            TYPE_MCHP_PFSOC_SYSREG);

    object_initialize_బిడ్డ(obj, "ddr-sgmii-phy", &s->ddr_sgmii_phy,
                            TYPE_MCHP_PFSOC_DDR_SGMII_PHY);
    object_initialize_బిడ్డ(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_MCHP_PFSOC_DDR_CFG);

    object_initialize_బిడ్డ(obj, "gem0", &s->gem0, TYPE_CADENCE_GEM);
    object_initialize_బిడ్డ(obj, "gem1", &s->gem1, TYPE_CADENCE_GEM);

    object_initialize_బిడ్డ(obj, "sd-controller", &s->sdhci,
                            TYPE_CADENCE_SDHCI);

    object_initialize_బిడ్డ(obj, "ioscb", &s->ioscb, TYPE_MCHP_PFSOC_IOSCB);
}

స్థిర శూన్య microchip_pfsoc_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    MicrochipIcicleKitState *iks = MICROCHIP_ICICLE_KIT_MACHINE(ms);
    MicrochipPFSoCState *s = MICROCHIP_PFSOC(dev);
    స్థిరరాశి Mem_Map_నమోదు *memmap = microchip_pfsoc_memmap;
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    జ్ఞాపకం_Region *rsvd0_mem = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *e51_dtim_mem = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *l2lim_mem = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *envm_దత్తాంశం = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *qspi_xip_mem = g_new(జ్ఞాపకం_Region, 1);
    అక్షరం *plic_hart_config;
    పూర్ణాంకం i;

    sysbus_realize(SYS_BUS_DEVICE(&s->e_cpus), &error_abort);
    qdev_prop_సమితి_uint32(DEVICE(&s->u_cpus), "num-harts", ms->smp.cpus - 1);
    sysbus_realize(SYS_BUS_DEVICE(&s->u_cpus), &error_abort);
    /*
     * The cluster must be realized after the RISC-V hart array container,
     * as the container's CPU object is only created on realize, and the
     * CPU must exist and have been parented into the cluster before the
     * cluster is realized.
     */
    qdev_realize(DEVICE(&s->e_cluster), ఖాళీ, &error_abort);
    qdev_realize(DEVICE(&s->u_cluster), ఖాళీ, &error_abort);

    /* Reserved Memory at address 0 */
    జ్ఞాపకం_region_init_ram(rsvd0_mem, ఖాళీ, "microchip.pfsoc.rsvd0_mem",
                           memmap[MICROCHIP_PFSOC_RSVD0].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_RSVD0].పునాది,
                                rsvd0_mem);

    /* E51 DTIM */
    జ్ఞాపకం_region_init_ram(e51_dtim_mem, ఖాళీ, "microchip.pfsoc.e51_dtim_mem",
                           memmap[MICROCHIP_PFSOC_E51_DTIM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_E51_DTIM].పునాది,
                                e51_dtim_mem);

    /* Bus Error Units */
    create_unimplemented_device("microchip.pfsoc.buserr_unit0_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT0].పునాది,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT0].కొలత);
    create_unimplemented_device("microchip.pfsoc.buserr_unit1_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT1].పునాది,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT1].కొలత);
    create_unimplemented_device("microchip.pfsoc.buserr_unit2_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT2].పునాది,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT2].కొలత);
    create_unimplemented_device("microchip.pfsoc.buserr_unit3_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT3].పునాది,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT3].కొలత);
    create_unimplemented_device("microchip.pfsoc.buserr_unit4_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT4].పునాది,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT4].కొలత);

    /* CLINT */
    riscv_aclint_swi_create(system_జ్ఞాపకం,
        memmap[MICROCHIP_PFSOC_CLINT].పునాది,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(system_జ్ఞాపకం,
        memmap[MICROCHIP_PFSOC_CLINT].పునాది + RISCV_ACLINT_SWI_కొలత,
        RISCV_ACLINT_DEFAULT_MTIMER_కొలత, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        iks->clint_timebase_freq, false);

    /* L2 cache controller */
    create_unimplemented_device("microchip.pfsoc.l2cc",
        memmap[MICROCHIP_PFSOC_L2CC].పునాది, memmap[MICROCHIP_PFSOC_L2CC].కొలత);

    /*
     * Add L2-LIM at reset size.
     * This should be reduced in size as the L2 Cache Controller WayEnable
     * register is incremented. Unfortunately I don't see a nice (or any) way
     * to handle reducing or blocking out the L2 LIM while still allowing it
     * be re returned to all enabled after a reset. For the time being, just
     * leave it enabled all the time. This won't break anything, but will be
     * too generous to misbehaving guests.
     */
    జ్ఞాపకం_region_init_ram(l2lim_mem, ఖాళీ, "microchip.pfsoc.l2lim",
                           memmap[MICROCHIP_PFSOC_L2LIM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_L2LIM].పునాది,
                                l2lim_mem);

    /* create PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(ms->smp.cpus);

    /* PLIC */
    s->plic = sifive_plic_create(system_జ్ఞాపకం,
        memmap[MICROCHIP_PFSOC_PLIC].పునాది,
        plic_hart_config, ms->smp.cpus, 0,
        MICROCHIP_PFSOC_PLIC_అంకె_SOURCES,
        MICROCHIP_PFSOC_PLIC_అంకె_PRIORITIES,
        MICROCHIP_PFSOC_PLIC_ప్రాధాన్యత_పునాది,
        MICROCHIP_PFSOC_PLIC_PENDING_పునాది,
        MICROCHIP_PFSOC_PLIC_ENABLE_పునాది,
        MICROCHIP_PFSOC_PLIC_ENABLE_STRIDE,
        MICROCHIP_PFSOC_PLIC_CONTEXT_పునాది,
        MICROCHIP_PFSOC_PLIC_CONTEXT_STRIDE,
        memmap[MICROCHIP_PFSOC_PLIC].కొలత);
    g_free(plic_hart_config);

    /* DMA */
    sysbus_realize(SYS_BUS_DEVICE(&s->dma), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->dma), 0,
                    memmap[MICROCHIP_PFSOC_DMA].పునాది);
    కొరకు (i = 0; i < SIFIVE_PDMA_IRQS; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->dma), i,
                           qdev_get_gpio_లోపల(DEVICE(s->plic),
                                            MICROCHIP_PFSOC_DMA_IRQ0 + i));
    }

    /* SYSREG */
    sysbus_realize(SYS_BUS_DEVICE(&s->sysreg), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sysreg), 0,
                    memmap[MICROCHIP_PFSOC_SYSREG].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sysreg), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->plic),
                       MICROCHIP_PFSOC_MAILBOX_IRQ));

    /* AXISW */
    create_unimplemented_device("microchip.pfsoc.axisw",
        memmap[MICROCHIP_PFSOC_AXISW].పునాది,
        memmap[MICROCHIP_PFSOC_AXISW].కొలత);

    /* MPUCFG */
    create_unimplemented_device("microchip.pfsoc.mpucfg",
        memmap[MICROCHIP_PFSOC_MPUCFG].పునాది,
        memmap[MICROCHIP_PFSOC_MPUCFG].కొలత);

    /* FMETER */
    create_unimplemented_device("microchip.pfsoc.fmeter",
        memmap[MICROCHIP_PFSOC_FMETER].పునాది,
        memmap[MICROCHIP_PFSOC_FMETER].కొలత);

    /* DDR SGMII PHY */
    sysbus_realize(SYS_BUS_DEVICE(&s->ddr_sgmii_phy), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_sgmii_phy), 0,
                    memmap[MICROCHIP_PFSOC_DDR_SGMII_PHY].పునాది);

    /* DDR CFG */
    sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[MICROCHIP_PFSOC_DDR_CFG].పునాది);

    /* SDHCI */
    sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0,
                    memmap[MICROCHIP_PFSOC_EMMC_SD].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
        qdev_get_gpio_లోపల(DEVICE(s->plic), MICROCHIP_PFSOC_EMMC_SD_IRQ));

    /* MMUARTs */
    s->serial0 = mchp_pfsoc_mmuart_create(system_జ్ఞాపకం,
        memmap[MICROCHIP_PFSOC_MMUART0].పునాది,
        qdev_get_gpio_లోపల(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART0_IRQ),
        serial_hd(0));
    s->serial1 = mchp_pfsoc_mmuart_create(system_జ్ఞాపకం,
        memmap[MICROCHIP_PFSOC_MMUART1].పునాది,
        qdev_get_gpio_లోపల(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART1_IRQ),
        serial_hd(1));
    s->serial2 = mchp_pfsoc_mmuart_create(system_జ్ఞాపకం,
        memmap[MICROCHIP_PFSOC_MMUART2].పునాది,
        qdev_get_gpio_లోపల(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART2_IRQ),
        serial_hd(2));
    s->serial3 = mchp_pfsoc_mmuart_create(system_జ్ఞాపకం,
        memmap[MICROCHIP_PFSOC_MMUART3].పునాది,
        qdev_get_gpio_లోపల(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART3_IRQ),
        serial_hd(3));
    s->serial4 = mchp_pfsoc_mmuart_create(system_జ్ఞాపకం,
        memmap[MICROCHIP_PFSOC_MMUART4].పునాది,
        qdev_get_gpio_లోపల(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART4_IRQ),
        serial_hd(4));

    /* Watchdogs */
    create_unimplemented_device("microchip.pfsoc.watchdog0",
        memmap[MICROCHIP_PFSOC_WDOG0].పునాది,
        memmap[MICROCHIP_PFSOC_WDOG0].కొలత);
    create_unimplemented_device("microchip.pfsoc.watchdog1",
        memmap[MICROCHIP_PFSOC_WDOG1].పునాది,
        memmap[MICROCHIP_PFSOC_WDOG1].కొలత);
    create_unimplemented_device("microchip.pfsoc.watchdog2",
        memmap[MICROCHIP_PFSOC_WDOG2].పునాది,
        memmap[MICROCHIP_PFSOC_WDOG2].కొలత);
    create_unimplemented_device("microchip.pfsoc.watchdog3",
        memmap[MICROCHIP_PFSOC_WDOG3].పునాది,
        memmap[MICROCHIP_PFSOC_WDOG3].కొలత);
    create_unimplemented_device("microchip.pfsoc.watchdog4",
        memmap[MICROCHIP_PFSOC_WDOG4].పునాది,
        memmap[MICROCHIP_PFSOC_WDOG4].కొలత);

    /* SPI */
    create_unimplemented_device("microchip.pfsoc.spi0",
        memmap[MICROCHIP_PFSOC_SPI0].పునాది,
        memmap[MICROCHIP_PFSOC_SPI0].కొలత);
    create_unimplemented_device("microchip.pfsoc.spi1",
        memmap[MICROCHIP_PFSOC_SPI1].పునాది,
        memmap[MICROCHIP_PFSOC_SPI1].కొలత);

    /* I2C */
    create_unimplemented_device("microchip.pfsoc.i2c0",
        memmap[MICROCHIP_PFSOC_I2C0].పునాది,
        memmap[MICROCHIP_PFSOC_I2C0].కొలత);
    create_unimplemented_device("microchip.pfsoc.i2c1",
        memmap[MICROCHIP_PFSOC_I2C1].పునాది,
        memmap[MICROCHIP_PFSOC_I2C1].కొలత);

    /* CAN */
    create_unimplemented_device("microchip.pfsoc.can0",
        memmap[MICROCHIP_PFSOC_CAN0].పునాది,
        memmap[MICROCHIP_PFSOC_CAN0].కొలత);
    create_unimplemented_device("microchip.pfsoc.can1",
        memmap[MICROCHIP_PFSOC_CAN1].పునాది,
        memmap[MICROCHIP_PFSOC_CAN1].కొలత);

    /* USB */
    create_unimplemented_device("microchip.pfsoc.usb",
        memmap[MICROCHIP_PFSOC_USB].పునాది,
        memmap[MICROCHIP_PFSOC_USB].కొలత);

    /* GEMs */
    qemu_configure_nic_device(DEVICE(&s->gem0), true, ఖాళీ);
    qemu_configure_nic_device(DEVICE(&s->gem1), true, ఖాళీ);

    object_property_సమితి_int(OBJECT(&s->gem0), "revision", GEM_REVISION, errp);
    object_property_సమితి_int(OBJECT(&s->gem0), "phy-addr", 8, errp);
    object_property_సమితి_bool(OBJECT(&s->gem0), "phy-connected", false, errp);
    object_property_సమితి_bool(OBJECT(&s->gem0), "pcs-enabled", true, errp);

    sysbus_realize(SYS_BUS_DEVICE(&s->gem0), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem0), 0,
                    memmap[MICROCHIP_PFSOC_GEM0].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem0), 0,
        qdev_get_gpio_లోపల(DEVICE(s->plic), MICROCHIP_PFSOC_GEM0_IRQ));

    object_property_సమితి_int(OBJECT(&s->gem1), "revision", GEM_REVISION, errp);
    object_property_సమితి_int(OBJECT(&s->gem1), "phy-addr", 9, errp);
    object_property_సమితి_link(OBJECT(&s->gem1), "phy-consumer",
                             OBJECT(&s->gem0), errp);
    object_property_సమితి_bool(OBJECT(&s->gem1), "pcs-enabled", true, errp);
    sysbus_realize(SYS_BUS_DEVICE(&s->gem1), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem1), 0,
                    memmap[MICROCHIP_PFSOC_GEM1].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem1), 0,
        qdev_get_gpio_లోపల(DEVICE(s->plic), MICROCHIP_PFSOC_GEM1_IRQ));

    /* GPIOs */
    create_unimplemented_device("microchip.pfsoc.gpio0",
        memmap[MICROCHIP_PFSOC_GPIO0].పునాది,
        memmap[MICROCHIP_PFSOC_GPIO0].కొలత);
    create_unimplemented_device("microchip.pfsoc.gpio1",
        memmap[MICROCHIP_PFSOC_GPIO1].పునాది,
        memmap[MICROCHIP_PFSOC_GPIO1].కొలత);
    create_unimplemented_device("microchip.pfsoc.gpio2",
        memmap[MICROCHIP_PFSOC_GPIO2].పునాది,
        memmap[MICROCHIP_PFSOC_GPIO2].కొలత);

    /* eNVM */
    జ్ఞాపకం_region_init_rom(envm_దత్తాంశం, OBJECT(dev), "microchip.pfsoc.envm.data",
                           memmap[MICROCHIP_PFSOC_ENVM_దత్తాంశం].కొలత,
                           &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_ENVM_దత్తాంశం].పునాది,
                                envm_దత్తాంశం);

    /* IOSCB */
    sysbus_realize(SYS_BUS_DEVICE(&s->ioscb), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ioscb), 0,
                    memmap[MICROCHIP_PFSOC_IOSCB].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->ioscb), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->plic),
                       MICROCHIP_PFSOC_MAILBOX_IRQ));

    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic3",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC3].పునాది,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC3].కొలత);
    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic0",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC0].పునాది,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC0].కొలత);
    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic1",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC1].పునాది,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC1].కొలత);

    /* QSPI Flash */
    జ్ఞాపకం_region_init_rom(qspi_xip_mem, OBJECT(dev),
                           "microchip.pfsoc.qspi_xip",
                           memmap[MICROCHIP_PFSOC_QSPI_XIP].కొలత,
                           &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_QSPI_XIP].పునాది,
                                qspi_xip_mem);
}

స్థిర శూన్య microchip_pfsoc_soc_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = microchip_pfsoc_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

స్థిర స్థిరరాశి TypeInfo microchip_pfsoc_soc_type_info = {
    .name = TYPE_MICROCHIP_PFSOC,
    .మాతృ = TYPE_DEVICE,
    .instance_కొలత = పరిమాణం(MicrochipPFSoCState),
    .instance_init = microchip_pfsoc_soc_instance_init,
    .class_init = microchip_pfsoc_soc_class_init,
};

స్థిర శూన్య microchip_pfsoc_soc_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&microchip_pfsoc_soc_type_info);
}

type_init(microchip_pfsoc_soc_రిజిస్టరు_types)

స్థిర శూన్య microchip_icicle_kit_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    స్థిరరాశి Mem_Map_నమోదు *memmap = microchip_pfsoc_memmap;
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(machine);
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    జ్ఞాపకం_Region *mem_దిగువ = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *mem_దిగువ_alias = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *mem_ఎగువ = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *mem_ఎగువ_alias = g_new(జ్ఞాపకం_Region, 1);
    uint64_t mem_దిగువ_కొలత, mem_ఎగువ_కొలత;
    hwaddr firmware_ఎక్కించు_addr;
    స్థిరరాశి అక్షరం *firmware_name;
    hwaddr firmware_చివర_addr;
    vaddr kernel_ప్రారంభం_addr;
    uint64_t kernel_నమోదు;
    uint64_t fdt_ఎక్కించు_addr;
    DriveInfo *dinfo = drive_get(IF_SD, 0, 0);
    RISCVBootInfo boot_info;

    /* Sanity check on RAM size */
    ఒకవేళ (machine->ram_కొలత < mc->default_ram_కొలత) {
        అక్షరం *sz = కొలత_గమ్యం_str(mc->default_ram_కొలత);
        error_report("Invalid RAM size, should be bigger than %s", sz);
        g_free(sz);
        నిష్క్రమించు(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_బిడ్డ(OBJECT(machine), "soc", &s->soc,
                            TYPE_MICROCHIP_PFSOC);
    qdev_realize(DEVICE(&s->soc), ఖాళీ, &error_fatal);

    /* Split RAM into low and high regions using aliases to machine->ram */
    mem_దిగువ_కొలత = memmap[MICROCHIP_PFSOC_DRAM_LO].కొలత;
    mem_ఎగువ_కొలత = machine->ram_కొలత - mem_దిగువ_కొలత;
    జ్ఞాపకం_region_init_alias(mem_దిగువ, ఖాళీ,
                             "microchip.icicle.kit.ram_low", machine->ram,
                             0, mem_దిగువ_కొలత);
    జ్ఞాపకం_region_init_alias(mem_ఎగువ, ఖాళీ,
                             "microchip.icicle.kit.ram_high", machine->ram,
                             mem_దిగువ_కొలత, mem_ఎగువ_కొలత);

    /* Register RAM */
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_DRAM_LO].పునాది,
                                mem_దిగువ);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_DRAM_HI].పునాది,
                                mem_ఎగువ);

    /* Create aliases for the low and high RAM regions */
    జ్ఞాపకం_region_init_alias(mem_దిగువ_alias, ఖాళీ,
                             "microchip.icicle.kit.ram_low.alias",
                             mem_దిగువ, 0, mem_దిగువ_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_DRAM_LO_ALIAS].పునాది,
                                mem_దిగువ_alias);
    జ్ఞాపకం_region_init_alias(mem_ఎగువ_alias, ఖాళీ,
                             "microchip.icicle.kit.ram_high.alias",
                             mem_ఎగువ, 0, mem_ఎగువ_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[MICROCHIP_PFSOC_DRAM_HI_ALIAS].పునాది,
                                mem_ఎగువ_alias);

    /* Attach an SD card */
    ఒకవేళ (dinfo) {
        CadenceSDHCIState *sdhci = &(s->soc.sdhci);
        DeviceState *card = qdev_new(TYPE_SD_CARD);

        qdev_prop_సమితి_drive_err(card, "drive", blk_by_legacy_dinfo(dinfo),
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
    ఒకవేళ (machine->firmware && !strcmp(machine->firmware, "none")) {
        ఒకవేళ (!machine->kernel_filename) {
            error_report("for -bios none, a kernel is required");
            నిష్క్రమించు(1);
        }

        firmware_name = ఖాళీ;
        firmware_ఎక్కించు_addr = RESET_సదిశ;
    } లేకపోతే ఒకవేళ (!machine->firmware || !strcmp(machine->firmware, "default")) {
        ఒకవేళ (machine->kernel_filename) {
            firmware_name = RISCV64_BIOS_BIN;
            firmware_ఎక్కించు_addr = memmap[MICROCHIP_PFSOC_DRAM_LO].పునాది;
        } లేకపోతే {
            firmware_name = BIOS_FILENAME;
            firmware_ఎక్కించు_addr = RESET_సదిశ;
        }
    } లేకపోతే {
        firmware_name = machine->firmware;
        firmware_ఎక్కించు_addr = RESET_సదిశ;
    }

    riscv_boot_info_init_discontig_mem(&boot_info, &s->soc.u_cpus,
                                       memmap[MICROCHIP_PFSOC_DRAM_LO].పునాది,
                                       mem_దిగువ_కొలత);

    /* Load the firmware if necessary */
    firmware_చివర_addr = firmware_ఎక్కించు_addr;
    ఒకవేళ (firmware_name) {
        అక్షరం *filename = riscv_కనుగొను_firmware(firmware_name, ఖాళీ);
        ఒకవేళ (filename) {
            firmware_చివర_addr = riscv_ఎక్కించు_firmware(machine, &boot_info,
                                                    filename,
                                                    &firmware_ఎక్కించు_addr, ఖాళీ);
            g_free(filename);
        }
    }

    ఒకవేళ (machine->kernel_filename) {
        kernel_ప్రారంభం_addr = riscv_calc_kernel_ప్రారంభం_addr(&boot_info,
                                                         firmware_చివర_addr);

        riscv_ఎక్కించు_kernel(machine, &boot_info, kernel_ప్రారంభం_addr,
                          true, ఖాళీ);
        kernel_నమోదు = boot_info.image_దిగువ_addr;

        ఒకవేళ (machine->dtb) {
            పూర్ణాంకం fdt_కొలత;
            machine->fdt = ఎక్కించు_device_వృక్షం(machine->dtb, &fdt_కొలత);
            ఒకవేళ (!machine->fdt) {
                error_report("load_device_tree() failed");
                నిష్క్రమించు(1);
            }

            /* Compute the FDT load address in DRAM */
            hwaddr kernel_ram_పునాది = memmap[MICROCHIP_PFSOC_DRAM_LO].పునాది;
            hwaddr kernel_ram_కొలత = memmap[MICROCHIP_PFSOC_DRAM_LO].కొలత;

            ఒకవేళ (kernel_నమోదు - kernel_ram_పునాది >= kernel_ram_కొలత) {
                kernel_ram_పునాది = memmap[MICROCHIP_PFSOC_DRAM_HI].పునాది;
                kernel_ram_కొలత = mem_ఎగువ_కొలత;
            }

            fdt_ఎక్కించు_addr = riscv_compute_fdt_addr(kernel_ram_పునాది, kernel_ram_కొలత,
                                                   machine, &boot_info);
            riscv_ఎక్కించు_fdt(fdt_ఎక్కించు_addr, machine->fdt);
        } లేకపోతే {
            warn_report_once("The QEMU microchip-icicle-kit machine does not "
                             "generate a device tree, so no device tree is "
                             "being provided to the guest.");
            fdt_ఎక్కించు_addr = 0;
        }

        hwaddr ప్రారంభం_addr;
        ఒకవేళ (firmware_name) {
            ప్రారంభం_addr = firmware_ఎక్కించు_addr;
        } లేకపోతే {
            ప్రారంభం_addr = kernel_నమోదు;
        }

        /* Load the reset vector */
        riscv_అమరిక_rom_reset_vec(machine, &s->soc.u_cpus, ప్రారంభం_addr,
                                  memmap[MICROCHIP_PFSOC_ENVM_దత్తాంశం].పునాది,
                                  memmap[MICROCHIP_PFSOC_ENVM_దత్తాంశం].కొలత,
                                  kernel_నమోదు, fdt_ఎక్కించు_addr);
    }
}

స్థిర శూన్య microchip_icicle_kit_సమితి_clint_timebase_freq(Object *obj,
                                                         Visitor *v,
                                                         స్థిరరాశి అక్షరం *name,
                                                         శూన్య *opaque,
                                                         Error **errp)
{
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    uint32_t విలువ;

    ఒకవేళ (!సందర్శించు_type_uint32(v, name, &విలువ, errp)) {
        తిరిగి;
    }

    s->clint_timebase_freq = విలువ;
}

స్థిర శూన్య microchip_icicle_kit_get_clint_timebase_freq(Object *obj,
                                                         Visitor *v,
                                                         స్థిరరాశి అక్షరం *name,
                                                         శూన్య *opaque,
                                                         Error **errp)
{
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    uint32_t విలువ = s->clint_timebase_freq;

    సందర్శించు_type_uint32(v, name, &విలువ, errp);
}

స్థిర శూన్య microchip_icicle_kit_machine_instance_init(Object *obj)
{
    MicrochipIcicleKitState *m = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    m->clint_timebase_freq = 1000000;
}

స్థిర శూన్య microchip_icicle_kit_machine_class_init(ObjectClass *oc,
                                                    స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Microchip PolarFire SoC Icicle Kit";
    mc->init = microchip_icicle_kit_machine_init;
    mc->max_cpus = MICROCHIP_PFSOC_MANAGEMENT_CPU_లెక్క +
                   MICROCHIP_PFSOC_COMPUTE_CPU_లెక్క;
    mc->min_cpus = MICROCHIP_PFSOC_MANAGEMENT_CPU_లెక్క + 1;
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
    mc->default_ram_కొలత = 1537 * MiB;

    object_class_property_కూడు(oc, "clint-timebase-frequency", "uint32_t",
                              microchip_icicle_kit_get_clint_timebase_freq,
                              microchip_icicle_kit_సమితి_clint_timebase_freq,
                              ఖాళీ, ఖాళీ);
    object_class_property_సమితి_description(oc, "clint-timebase-frequency",
                                  "Set CLINT timebase frequency in Hz.");
}

స్థిర స్థిరరాశి TypeInfo microchip_icicle_kit_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("microchip-icicle-kit"),
    .మాతృ     = TYPE_MACHINE,
    .class_init = microchip_icicle_kit_machine_class_init,
    .instance_init = microchip_icicle_kit_machine_instance_init,
    .instance_కొలత = పరిమాణం(MicrochipIcicleKitState),
};

స్థిర శూన్య microchip_icicle_kit_machine_init_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&microchip_icicle_kit_machine_typeinfo);
}

type_init(microchip_icicle_kit_machine_init_రిజిస్టరు_types)
