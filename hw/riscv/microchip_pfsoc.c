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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"
#ಅನ್ತರ್ಭಾವ "qemu/cutils.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qapi/visitor.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "chardev/char.h"
#ಅನ್ತರ್ಭಾವ "hw/cpu/cluster.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/unimp.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/microchip_pfsoc.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/sifive_plic.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"

/*
 * The BIOS image used by this machine is called Hart Software Services (HSS).
 * See https://github.com/polarfire-soc/hart-software-services
 */
#ವ್ಯಾಖ್ಯಾನಿಸು BIOS_FILENAME   "hss.bin"
#ವ್ಯಾಖ್ಯಾನಿಸು RESET_ಸದಿಶ    0x20220000

/* GEM version */
#ವ್ಯಾಖ್ಯಾನಿಸು GEM_REVISION    0x0107010c

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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು microchip_pfsoc_memmap[] = {
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
    [MICROCHIP_PFSOC_ENVM_ದತ್ತಾಂಶ] =       { 0x20220000,      0x20000 },
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

ಸ್ಥಿರ ಶೂನ್ಯ microchip_pfsoc_soc_instance_init(Object *obj)
{
    MicrochipPFSoCState *s = MICROCHIP_PFSOC(obj);

    object_initialize_ಮಗು(obj, "e-cluster", &s->e_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->e_cluster), "cluster-id", 0);

    object_initialize_ಮಗು(OBJECT(&s->e_cluster), "e-cpus", &s->e_cpus,
                            TYPE_RISCV_HART_ARRAY);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->e_cpus), "num-harts", 1);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->e_cpus), "hartid-base", 0);
    qdev_prop_ಗಣ_string(DEVICE(&s->e_cpus), "cpu-type",
                         TYPE_RISCV_CPU_SIFIVE_E51);
    qdev_prop_ಗಣ_uint64(DEVICE(&s->e_cpus), "resetvec", RESET_ಸದಿಶ);

    object_initialize_ಮಗು(obj, "u-cluster", &s->u_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->u_cluster), "cluster-id", 1);

    object_initialize_ಮಗು(OBJECT(&s->u_cluster), "u-cpus", &s->u_cpus,
                            TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    qdev_prop_ಗಣ_uint32(DEVICE(&s->u_cpus), "hartid-base", 1);
    qdev_prop_ಗಣ_string(DEVICE(&s->u_cpus), "cpu-type",
                         TYPE_RISCV_CPU_SIFIVE_U54);
    qdev_prop_ಗಣ_uint64(DEVICE(&s->u_cpus), "resetvec", RESET_ಸದಿಶ);

    object_initialize_ಮಗು(obj, "dma-controller", &s->dma,
                            TYPE_SIFIVE_PDMA);

    object_initialize_ಮಗು(obj, "sysreg", &s->sysreg,
                            TYPE_MCHP_PFSOC_SYSREG);

    object_initialize_ಮಗು(obj, "ddr-sgmii-phy", &s->ddr_sgmii_phy,
                            TYPE_MCHP_PFSOC_DDR_SGMII_PHY);
    object_initialize_ಮಗು(obj, "ddr-cfg", &s->ddr_cfg,
                            TYPE_MCHP_PFSOC_DDR_CFG);

    object_initialize_ಮಗು(obj, "gem0", &s->gem0, TYPE_CADENCE_GEM);
    object_initialize_ಮಗು(obj, "gem1", &s->gem1, TYPE_CADENCE_GEM);

    object_initialize_ಮಗು(obj, "sd-controller", &s->sdhci,
                            TYPE_CADENCE_SDHCI);

    object_initialize_ಮಗು(obj, "ioscb", &s->ioscb, TYPE_MCHP_PFSOC_IOSCB);
}

ಸ್ಥಿರ ಶೂನ್ಯ microchip_pfsoc_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    MicrochipIcicleKitState *iks = MICROCHIP_ICICLE_KIT_MACHINE(ms);
    MicrochipPFSoCState *s = MICROCHIP_PFSOC(dev);
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = microchip_pfsoc_memmap;
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    ಸ್ಮರಣೆ_Region *rsvd0_mem = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_Region *e51_dtim_mem = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_Region *l2lim_mem = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_Region *envm_ದತ್ತಾಂಶ = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_Region *qspi_xip_mem = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಅಕ್ಷರ *plic_hart_config;
    ಪೂರ್ಣಾಂಕ i;

    sysbus_realize(SYS_BUS_DEVICE(&s->e_cpus), &error_abort);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->u_cpus), "num-harts", ms->smp.cpus - 1);
    sysbus_realize(SYS_BUS_DEVICE(&s->u_cpus), &error_abort);
    /*
     * The cluster must be realized after the RISC-V hart array container,
     * as the container's CPU object is only created on realize, and the
     * CPU must exist and have been parented into the cluster before the
     * cluster is realized.
     */
    qdev_realize(DEVICE(&s->e_cluster), ಖಾಲಿ, &error_abort);
    qdev_realize(DEVICE(&s->u_cluster), ಖಾಲಿ, &error_abort);

    /* Reserved Memory at address 0 */
    ಸ್ಮರಣೆ_region_init_ram(rsvd0_mem, ಖಾಲಿ, "microchip.pfsoc.rsvd0_mem",
                           memmap[MICROCHIP_PFSOC_RSVD0].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_RSVD0].ತಳ,
                                rsvd0_mem);

    /* E51 DTIM */
    ಸ್ಮರಣೆ_region_init_ram(e51_dtim_mem, ಖಾಲಿ, "microchip.pfsoc.e51_dtim_mem",
                           memmap[MICROCHIP_PFSOC_E51_DTIM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_E51_DTIM].ತಳ,
                                e51_dtim_mem);

    /* Bus Error Units */
    create_unimplemented_device("microchip.pfsoc.buserr_unit0_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT0].ತಳ,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT0].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.buserr_unit1_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT1].ತಳ,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT1].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.buserr_unit2_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT2].ತಳ,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT2].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.buserr_unit3_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT3].ತಳ,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT3].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.buserr_unit4_mem",
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT4].ತಳ,
        memmap[MICROCHIP_PFSOC_BUSERR_UNIT4].ಗಾತ್ರ_2);

    /* CLINT */
    riscv_aclint_swi_create(system_ಸ್ಮರಣೆ,
        memmap[MICROCHIP_PFSOC_CLINT].ತಳ,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(system_ಸ್ಮರಣೆ,
        memmap[MICROCHIP_PFSOC_CLINT].ತಳ + RISCV_ACLINT_SWI_ಗಾತ್ರ,
        RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        iks->clint_timebase_freq, false);

    /* L2 cache controller */
    create_unimplemented_device("microchip.pfsoc.l2cc",
        memmap[MICROCHIP_PFSOC_L2CC].ತಳ, memmap[MICROCHIP_PFSOC_L2CC].ಗಾತ್ರ_2);

    /*
     * Add L2-LIM at reset size.
     * This should be reduced in size as the L2 Cache Controller WayEnable
     * register is incremented. Unfortunately I don't see a nice (or any) way
     * to handle reducing or blocking out the L2 LIM while still allowing it
     * be re returned to all enabled after a reset. For the time being, just
     * leave it enabled all the time. This won't break anything, but will be
     * too generous to misbehaving guests.
     */
    ಸ್ಮರಣೆ_region_init_ram(l2lim_mem, ಖಾಲಿ, "microchip.pfsoc.l2lim",
                           memmap[MICROCHIP_PFSOC_L2LIM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_L2LIM].ತಳ,
                                l2lim_mem);

    /* create PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(ms->smp.cpus);

    /* PLIC */
    s->plic = sifive_plic_create(system_ಸ್ಮರಣೆ,
        memmap[MICROCHIP_PFSOC_PLIC].ತಳ,
        plic_hart_config, ms->smp.cpus, 0,
        MICROCHIP_PFSOC_PLIC_ಅಂಕೆ_SOURCES,
        MICROCHIP_PFSOC_PLIC_ಅಂಕೆ_PRIORITIES,
        MICROCHIP_PFSOC_PLIC_ಆದ್ಯತೆ_ತಳ,
        MICROCHIP_PFSOC_PLIC_PENDING_ತಳ,
        MICROCHIP_PFSOC_PLIC_ENABLE_ತಳ,
        MICROCHIP_PFSOC_PLIC_ENABLE_STRIDE,
        MICROCHIP_PFSOC_PLIC_CONTEXT_ತಳ,
        MICROCHIP_PFSOC_PLIC_CONTEXT_STRIDE,
        memmap[MICROCHIP_PFSOC_PLIC].ಗಾತ್ರ_2);
    g_free(plic_hart_config);

    /* DMA */
    sysbus_realize(SYS_BUS_DEVICE(&s->dma), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->dma), 0,
                    memmap[MICROCHIP_PFSOC_DMA].ತಳ);
    ಗಾಗಿ (i = 0; i < SIFIVE_PDMA_IRQS; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->dma), i,
                           qdev_get_gpio_ಒಳ(DEVICE(s->plic),
                                            MICROCHIP_PFSOC_DMA_IRQ0 + i));
    }

    /* SYSREG */
    sysbus_realize(SYS_BUS_DEVICE(&s->sysreg), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sysreg), 0,
                    memmap[MICROCHIP_PFSOC_SYSREG].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sysreg), 0,
                       qdev_get_gpio_ಒಳ(DEVICE(s->plic),
                       MICROCHIP_PFSOC_MAILBOX_IRQ));

    /* AXISW */
    create_unimplemented_device("microchip.pfsoc.axisw",
        memmap[MICROCHIP_PFSOC_AXISW].ತಳ,
        memmap[MICROCHIP_PFSOC_AXISW].ಗಾತ್ರ_2);

    /* MPUCFG */
    create_unimplemented_device("microchip.pfsoc.mpucfg",
        memmap[MICROCHIP_PFSOC_MPUCFG].ತಳ,
        memmap[MICROCHIP_PFSOC_MPUCFG].ಗಾತ್ರ_2);

    /* FMETER */
    create_unimplemented_device("microchip.pfsoc.fmeter",
        memmap[MICROCHIP_PFSOC_FMETER].ತಳ,
        memmap[MICROCHIP_PFSOC_FMETER].ಗಾತ್ರ_2);

    /* DDR SGMII PHY */
    sysbus_realize(SYS_BUS_DEVICE(&s->ddr_sgmii_phy), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_sgmii_phy), 0,
                    memmap[MICROCHIP_PFSOC_DDR_SGMII_PHY].ತಳ);

    /* DDR CFG */
    sysbus_realize(SYS_BUS_DEVICE(&s->ddr_cfg), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ddr_cfg), 0,
                    memmap[MICROCHIP_PFSOC_DDR_CFG].ತಳ);

    /* SDHCI */
    sysbus_realize(SYS_BUS_DEVICE(&s->sdhci), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->sdhci), 0,
                    memmap[MICROCHIP_PFSOC_EMMC_SD].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->sdhci), 0,
        qdev_get_gpio_ಒಳ(DEVICE(s->plic), MICROCHIP_PFSOC_EMMC_SD_IRQ));

    /* MMUARTs */
    s->serial0 = mchp_pfsoc_mmuart_create(system_ಸ್ಮರಣೆ,
        memmap[MICROCHIP_PFSOC_MMUART0].ತಳ,
        qdev_get_gpio_ಒಳ(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART0_IRQ),
        serial_hd(0));
    s->serial1 = mchp_pfsoc_mmuart_create(system_ಸ್ಮರಣೆ,
        memmap[MICROCHIP_PFSOC_MMUART1].ತಳ,
        qdev_get_gpio_ಒಳ(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART1_IRQ),
        serial_hd(1));
    s->serial2 = mchp_pfsoc_mmuart_create(system_ಸ್ಮರಣೆ,
        memmap[MICROCHIP_PFSOC_MMUART2].ತಳ,
        qdev_get_gpio_ಒಳ(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART2_IRQ),
        serial_hd(2));
    s->serial3 = mchp_pfsoc_mmuart_create(system_ಸ್ಮರಣೆ,
        memmap[MICROCHIP_PFSOC_MMUART3].ತಳ,
        qdev_get_gpio_ಒಳ(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART3_IRQ),
        serial_hd(3));
    s->serial4 = mchp_pfsoc_mmuart_create(system_ಸ್ಮರಣೆ,
        memmap[MICROCHIP_PFSOC_MMUART4].ತಳ,
        qdev_get_gpio_ಒಳ(DEVICE(s->plic), MICROCHIP_PFSOC_MMUART4_IRQ),
        serial_hd(4));

    /* Watchdogs */
    create_unimplemented_device("microchip.pfsoc.watchdog0",
        memmap[MICROCHIP_PFSOC_WDOG0].ತಳ,
        memmap[MICROCHIP_PFSOC_WDOG0].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.watchdog1",
        memmap[MICROCHIP_PFSOC_WDOG1].ತಳ,
        memmap[MICROCHIP_PFSOC_WDOG1].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.watchdog2",
        memmap[MICROCHIP_PFSOC_WDOG2].ತಳ,
        memmap[MICROCHIP_PFSOC_WDOG2].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.watchdog3",
        memmap[MICROCHIP_PFSOC_WDOG3].ತಳ,
        memmap[MICROCHIP_PFSOC_WDOG3].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.watchdog4",
        memmap[MICROCHIP_PFSOC_WDOG4].ತಳ,
        memmap[MICROCHIP_PFSOC_WDOG4].ಗಾತ್ರ_2);

    /* SPI */
    create_unimplemented_device("microchip.pfsoc.spi0",
        memmap[MICROCHIP_PFSOC_SPI0].ತಳ,
        memmap[MICROCHIP_PFSOC_SPI0].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.spi1",
        memmap[MICROCHIP_PFSOC_SPI1].ತಳ,
        memmap[MICROCHIP_PFSOC_SPI1].ಗಾತ್ರ_2);

    /* I2C */
    create_unimplemented_device("microchip.pfsoc.i2c0",
        memmap[MICROCHIP_PFSOC_I2C0].ತಳ,
        memmap[MICROCHIP_PFSOC_I2C0].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.i2c1",
        memmap[MICROCHIP_PFSOC_I2C1].ತಳ,
        memmap[MICROCHIP_PFSOC_I2C1].ಗಾತ್ರ_2);

    /* CAN */
    create_unimplemented_device("microchip.pfsoc.can0",
        memmap[MICROCHIP_PFSOC_CAN0].ತಳ,
        memmap[MICROCHIP_PFSOC_CAN0].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.can1",
        memmap[MICROCHIP_PFSOC_CAN1].ತಳ,
        memmap[MICROCHIP_PFSOC_CAN1].ಗಾತ್ರ_2);

    /* USB */
    create_unimplemented_device("microchip.pfsoc.usb",
        memmap[MICROCHIP_PFSOC_USB].ತಳ,
        memmap[MICROCHIP_PFSOC_USB].ಗಾತ್ರ_2);

    /* GEMs */
    qemu_configure_nic_device(DEVICE(&s->gem0), true, ಖಾಲಿ);
    qemu_configure_nic_device(DEVICE(&s->gem1), true, ಖಾಲಿ);

    object_property_ಗಣ_int(OBJECT(&s->gem0), "revision", GEM_REVISION, errp);
    object_property_ಗಣ_int(OBJECT(&s->gem0), "phy-addr", 8, errp);
    object_property_ಗಣ_bool(OBJECT(&s->gem0), "phy-connected", false, errp);
    object_property_ಗಣ_bool(OBJECT(&s->gem0), "pcs-enabled", true, errp);

    sysbus_realize(SYS_BUS_DEVICE(&s->gem0), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem0), 0,
                    memmap[MICROCHIP_PFSOC_GEM0].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem0), 0,
        qdev_get_gpio_ಒಳ(DEVICE(s->plic), MICROCHIP_PFSOC_GEM0_IRQ));

    object_property_ಗಣ_int(OBJECT(&s->gem1), "revision", GEM_REVISION, errp);
    object_property_ಗಣ_int(OBJECT(&s->gem1), "phy-addr", 9, errp);
    object_property_ಗಣ_link(OBJECT(&s->gem1), "phy-consumer",
                             OBJECT(&s->gem0), errp);
    object_property_ಗಣ_bool(OBJECT(&s->gem1), "pcs-enabled", true, errp);
    sysbus_realize(SYS_BUS_DEVICE(&s->gem1), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem1), 0,
                    memmap[MICROCHIP_PFSOC_GEM1].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem1), 0,
        qdev_get_gpio_ಒಳ(DEVICE(s->plic), MICROCHIP_PFSOC_GEM1_IRQ));

    /* GPIOs */
    create_unimplemented_device("microchip.pfsoc.gpio0",
        memmap[MICROCHIP_PFSOC_GPIO0].ತಳ,
        memmap[MICROCHIP_PFSOC_GPIO0].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.gpio1",
        memmap[MICROCHIP_PFSOC_GPIO1].ತಳ,
        memmap[MICROCHIP_PFSOC_GPIO1].ಗಾತ್ರ_2);
    create_unimplemented_device("microchip.pfsoc.gpio2",
        memmap[MICROCHIP_PFSOC_GPIO2].ತಳ,
        memmap[MICROCHIP_PFSOC_GPIO2].ಗಾತ್ರ_2);

    /* eNVM */
    ಸ್ಮರಣೆ_region_init_rom(envm_ದತ್ತಾಂಶ, OBJECT(dev), "microchip.pfsoc.envm.data",
                           memmap[MICROCHIP_PFSOC_ENVM_ದತ್ತಾಂಶ].ಗಾತ್ರ_2,
                           &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_ENVM_ದತ್ತಾಂಶ].ತಳ,
                                envm_ದತ್ತಾಂಶ);

    /* IOSCB */
    sysbus_realize(SYS_BUS_DEVICE(&s->ioscb), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->ioscb), 0,
                    memmap[MICROCHIP_PFSOC_IOSCB].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->ioscb), 0,
                       qdev_get_gpio_ಒಳ(DEVICE(s->plic),
                       MICROCHIP_PFSOC_MAILBOX_IRQ));

    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic3",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC3].ತಳ,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC3].ಗಾತ್ರ_2);
    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic0",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC0].ತಳ,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC0].ಗಾತ್ರ_2);
    /* FPGA Fabric */
    create_unimplemented_device("microchip.pfsoc.fabricfic1",
        memmap[MICROCHIP_PFSOC_FABRIC_FIC1].ತಳ,
        memmap[MICROCHIP_PFSOC_FABRIC_FIC1].ಗಾತ್ರ_2);

    /* QSPI Flash */
    ಸ್ಮರಣೆ_region_init_rom(qspi_xip_mem, OBJECT(dev),
                           "microchip.pfsoc.qspi_xip",
                           memmap[MICROCHIP_PFSOC_QSPI_XIP].ಗಾತ್ರ_2,
                           &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_QSPI_XIP].ತಳ,
                                qspi_xip_mem);
}

ಸ್ಥಿರ ಶೂನ್ಯ microchip_pfsoc_soc_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = microchip_pfsoc_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo microchip_pfsoc_soc_type_info = {
    .name = TYPE_MICROCHIP_PFSOC,
    .ಪೋಷಕ = TYPE_DEVICE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(MicrochipPFSoCState),
    .instance_init = microchip_pfsoc_soc_instance_init,
    .class_init = microchip_pfsoc_soc_class_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ microchip_pfsoc_soc_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&microchip_pfsoc_soc_type_info);
}

type_init(microchip_pfsoc_soc_ರಿಜಿಸ್ಟರ್_types)

ಸ್ಥಿರ ಶೂನ್ಯ microchip_icicle_kit_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = microchip_pfsoc_memmap;
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(machine);
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    ಸ್ಮರಣೆ_Region *mem_ಕೆಳಗಿನ = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_Region *mem_ಕೆಳಗಿನ_alias = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_Region *mem_ಮೇಲಿನ = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_Region *mem_ಮೇಲಿನ_alias = g_new(ಸ್ಮರಣೆ_Region, 1);
    uint64_t mem_ಕೆಳಗಿನ_ಗಾತ್ರ, mem_ಮೇಲಿನ_ಗಾತ್ರ;
    hwaddr firmware_ತುಂಬು_addr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *firmware_name;
    hwaddr firmware_ಕೊನೆ_addr;
    vaddr kernel_ಆರಂಭ_addr;
    uint64_t kernel_ನಮೂದು;
    uint64_t fdt_ತುಂಬು_addr;
    DriveInfo *dinfo = drive_get(IF_SD, 0, 0);
    RISCVBootInfo boot_info;

    /* Sanity check on RAM size */
    ಆದರೆ (machine->ram_ಗಾತ್ರ < mc->default_ram_ಗಾತ್ರ) {
        ಅಕ್ಷರ *sz = ಗಾತ್ರ_ಗಮ್ಯ_str(mc->default_ram_ಗಾತ್ರ);
        error_report("Invalid RAM size, should be bigger than %s", sz);
        g_free(sz);
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_ಮಗು(OBJECT(machine), "soc", &s->soc,
                            TYPE_MICROCHIP_PFSOC);
    qdev_realize(DEVICE(&s->soc), ಖಾಲಿ, &error_fatal);

    /* Split RAM into low and high regions using aliases to machine->ram */
    mem_ಕೆಳಗಿನ_ಗಾತ್ರ = memmap[MICROCHIP_PFSOC_DRAM_LO].ಗಾತ್ರ_2;
    mem_ಮೇಲಿನ_ಗಾತ್ರ = machine->ram_ಗಾತ್ರ - mem_ಕೆಳಗಿನ_ಗಾತ್ರ;
    ಸ್ಮರಣೆ_region_init_alias(mem_ಕೆಳಗಿನ, ಖಾಲಿ,
                             "microchip.icicle.kit.ram_low", machine->ram,
                             0, mem_ಕೆಳಗಿನ_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_init_alias(mem_ಮೇಲಿನ, ಖಾಲಿ,
                             "microchip.icicle.kit.ram_high", machine->ram,
                             mem_ಕೆಳಗಿನ_ಗಾತ್ರ, mem_ಮೇಲಿನ_ಗಾತ್ರ);

    /* Register RAM */
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_DRAM_LO].ತಳ,
                                mem_ಕೆಳಗಿನ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_DRAM_HI].ತಳ,
                                mem_ಮೇಲಿನ);

    /* Create aliases for the low and high RAM regions */
    ಸ್ಮರಣೆ_region_init_alias(mem_ಕೆಳಗಿನ_alias, ಖಾಲಿ,
                             "microchip.icicle.kit.ram_low.alias",
                             mem_ಕೆಳಗಿನ, 0, mem_ಕೆಳಗಿನ_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_DRAM_LO_ALIAS].ತಳ,
                                mem_ಕೆಳಗಿನ_alias);
    ಸ್ಮರಣೆ_region_init_alias(mem_ಮೇಲಿನ_alias, ಖಾಲಿ,
                             "microchip.icicle.kit.ram_high.alias",
                             mem_ಮೇಲಿನ, 0, mem_ಮೇಲಿನ_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[MICROCHIP_PFSOC_DRAM_HI_ALIAS].ತಳ,
                                mem_ಮೇಲಿನ_alias);

    /* Attach an SD card */
    ಆದರೆ (dinfo) {
        CadenceSDHCIState *sdhci = &(s->soc.sdhci);
        DeviceState *card = qdev_new(TYPE_SD_CARD);

        qdev_prop_ಗಣ_drive_err(card, "drive", blk_by_legacy_dinfo(dinfo),
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
    ಆದರೆ (machine->firmware && !strcmp(machine->firmware, "none")) {
        ಆದರೆ (!machine->kernel_filename) {
            error_report("for -bios none, a kernel is required");
            ನಿರ್ಗಮಿಸು(1);
        }

        firmware_name = ಖಾಲಿ;
        firmware_ತುಂಬು_addr = RESET_ಸದಿಶ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!machine->firmware || !strcmp(machine->firmware, "default")) {
        ಆದರೆ (machine->kernel_filename) {
            firmware_name = RISCV64_BIOS_BIN;
            firmware_ತುಂಬು_addr = memmap[MICROCHIP_PFSOC_DRAM_LO].ತಳ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            firmware_name = BIOS_FILENAME;
            firmware_ತುಂಬು_addr = RESET_ಸದಿಶ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        firmware_name = machine->firmware;
        firmware_ತುಂಬು_addr = RESET_ಸದಿಶ;
    }

    riscv_boot_info_init_discontig_mem(&boot_info, &s->soc.u_cpus,
                                       memmap[MICROCHIP_PFSOC_DRAM_LO].ತಳ,
                                       mem_ಕೆಳಗಿನ_ಗಾತ್ರ);

    /* Load the firmware if necessary */
    firmware_ಕೊನೆ_addr = firmware_ತುಂಬು_addr;
    ಆದರೆ (firmware_name) {
        ಅಕ್ಷರ *filename = riscv_ಕಂಡುಹಿಡಿ_firmware(firmware_name, ಖಾಲಿ);
        ಆದರೆ (filename) {
            firmware_ಕೊನೆ_addr = riscv_ತುಂಬು_firmware(machine, &boot_info,
                                                    filename,
                                                    &firmware_ತುಂಬು_addr, ಖಾಲಿ);
            g_free(filename);
        }
    }

    ಆದರೆ (machine->kernel_filename) {
        kernel_ಆರಂಭ_addr = riscv_calc_kernel_ಆರಂಭ_addr(&boot_info,
                                                         firmware_ಕೊನೆ_addr);

        riscv_ತುಂಬು_kernel(machine, &boot_info, kernel_ಆರಂಭ_addr,
                          true, ಖಾಲಿ);
        kernel_ನಮೂದು = boot_info.image_ಕೆಳಗಿನ_addr;

        ಆದರೆ (machine->dtb) {
            ಪೂರ್ಣಾಂಕ fdt_ಗಾತ್ರ;
            machine->fdt = ತುಂಬು_device_ವೃಕ್ಷ(machine->dtb, &fdt_ಗಾತ್ರ);
            ಆದರೆ (!machine->fdt) {
                error_report("load_device_tree() failed");
                ನಿರ್ಗಮಿಸು(1);
            }

            /* Compute the FDT load address in DRAM */
            hwaddr kernel_ram_ತಳ = memmap[MICROCHIP_PFSOC_DRAM_LO].ತಳ;
            hwaddr kernel_ram_ಗಾತ್ರ = memmap[MICROCHIP_PFSOC_DRAM_LO].ಗಾತ್ರ_2;

            ಆದರೆ (kernel_ನಮೂದು - kernel_ram_ತಳ >= kernel_ram_ಗಾತ್ರ) {
                kernel_ram_ತಳ = memmap[MICROCHIP_PFSOC_DRAM_HI].ತಳ;
                kernel_ram_ಗಾತ್ರ = mem_ಮೇಲಿನ_ಗಾತ್ರ;
            }

            fdt_ತುಂಬು_addr = riscv_compute_fdt_addr(kernel_ram_ತಳ, kernel_ram_ಗಾತ್ರ,
                                                   machine, &boot_info);
            riscv_ತುಂಬು_fdt(fdt_ತುಂಬು_addr, machine->fdt);
        } ಇಲ್ಲದಿದ್ದರೆ {
            warn_report_once("The QEMU microchip-icicle-kit machine does not "
                             "generate a device tree, so no device tree is "
                             "being provided to the guest.");
            fdt_ತುಂಬು_addr = 0;
        }

        hwaddr ಆರಂಭ_addr;
        ಆದರೆ (firmware_name) {
            ಆರಂಭ_addr = firmware_ತುಂಬು_addr;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ_addr = kernel_ನಮೂದು;
        }

        /* Load the reset vector */
        riscv_ಸಿದ್ಧತೆ_rom_reset_vec(machine, &s->soc.u_cpus, ಆರಂಭ_addr,
                                  memmap[MICROCHIP_PFSOC_ENVM_ದತ್ತಾಂಶ].ತಳ,
                                  memmap[MICROCHIP_PFSOC_ENVM_ದತ್ತಾಂಶ].ಗಾತ್ರ_2,
                                  kernel_ನಮೂದು, fdt_ತುಂಬು_addr);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ microchip_icicle_kit_ಗಣ_clint_timebase_freq(Object *obj,
                                                         Visitor *v,
                                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                                         ಶೂನ್ಯ *opaque,
                                                         Error **errp)
{
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    uint32_t ಮೌಲ್ಯ;

    ಆದರೆ (!ಭೇಟಿ_type_uint32(v, name, &ಮೌಲ್ಯ, errp)) {
        ಹಿಂತಿರುಗಿ;
    }

    s->clint_timebase_freq = ಮೌಲ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ microchip_icicle_kit_get_clint_timebase_freq(Object *obj,
                                                         Visitor *v,
                                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                                         ಶೂನ್ಯ *opaque,
                                                         Error **errp)
{
    MicrochipIcicleKitState *s = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    uint32_t ಮೌಲ್ಯ = s->clint_timebase_freq;

    ಭೇಟಿ_type_uint32(v, name, &ಮೌಲ್ಯ, errp);
}

ಸ್ಥಿರ ಶೂನ್ಯ microchip_icicle_kit_machine_instance_init(Object *obj)
{
    MicrochipIcicleKitState *m = MICROCHIP_ICICLE_KIT_MACHINE(obj);
    m->clint_timebase_freq = 1000000;
}

ಸ್ಥಿರ ಶೂನ್ಯ microchip_icicle_kit_machine_class_init(ObjectClass *oc,
                                                    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Microchip PolarFire SoC Icicle Kit";
    mc->init = microchip_icicle_kit_machine_init;
    mc->max_cpus = MICROCHIP_PFSOC_MANAGEMENT_CPU_ಎಣಿಕೆ +
                   MICROCHIP_PFSOC_COMPUTE_CPU_ಎಣಿಕೆ;
    mc->min_cpus = MICROCHIP_PFSOC_MANAGEMENT_CPU_ಎಣಿಕೆ + 1;
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
    mc->default_ram_ಗಾತ್ರ = 1537 * MiB;

    object_class_property_ಕೂಡಿಸು(oc, "clint-timebase-frequency", "uint32_t",
                              microchip_icicle_kit_get_clint_timebase_freq,
                              microchip_icicle_kit_ಗಣ_clint_timebase_freq,
                              ಖಾಲಿ, ಖಾಲಿ);
    object_class_property_ಗಣ_description(oc, "clint-timebase-frequency",
                                  "Set CLINT timebase frequency in Hz.");
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo microchip_icicle_kit_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("microchip-icicle-kit"),
    .ಪೋಷಕ     = TYPE_MACHINE,
    .class_init = microchip_icicle_kit_machine_class_init,
    .instance_init = microchip_icicle_kit_machine_instance_init,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(MicrochipIcicleKitState),
};

ಸ್ಥಿರ ಶೂನ್ಯ microchip_icicle_kit_machine_init_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&microchip_icicle_kit_machine_typeinfo);
}

type_init(microchip_icicle_kit_machine_init_ರಿಜಿಸ್ಟರ್_types)
