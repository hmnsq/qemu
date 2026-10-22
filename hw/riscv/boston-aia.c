/*
 * MIPS Boston-aia development board emulation.
 *
 * Copyright (c) 2016 Imagination Technologies
 *
 * Copyright (c) 2025 MIPS
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/units.h"

#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/char/serial-mm.h"
#అన్తర్భావ "hw/ide/pci.h"
#అన్తర్భావ "hw/ide/ahci-pci.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/riscv/cps.h"
#అన్తర్భావ "hw/pci-host/xilinx-pcie.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qemu/log.h"
#అన్తర్భావ "chardev/char.h"
#అన్తర్భావ "system/address-spaces.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "system/system.h"
#అన్తర్భావ "system/qtest.h"
#అన్తర్భావ "system/runstate.h"

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ "qom/object.h"

#నిర్వచించు TYPE_MIPS_BOSTON_AIA "mips-boston-aia"
రకనిర్వచనం నిర్మాణం BostonState BostonState;
DECLARE_INSTANCE_CHECKER(BostonState, BOSTON,
                         TYPE_MIPS_BOSTON_AIA)

గణన {
    BOSTON_PCIE2,
    BOSTON_PCIE2_MMIO,
    BOSTON_PLATREG,
    BOSTON_UART,
    BOSTON_LCD,
    BOSTON_FLASH,
    BOSTON_HIGHDDR,
};

స్థిర స్థిరరాశి Mem_Map_నమోదు boston_memmap[] = {
    [BOSTON_PCIE2] =      { 0x14000000,     0x2000000 },
    [BOSTON_PCIE2_MMIO] = { 0x16000000,      0x100000 },
    [BOSTON_PLATREG] =    { 0x17ffd000,        0x1000 },
    [BOSTON_UART] =       { 0x17ffe000,          0x20 },
    [BOSTON_LCD] =        { 0x17fff000,           0x8 },
    [BOSTON_FLASH] =      { 0x18000000,     0x8000000 },
    [BOSTON_HIGHDDR] =    { 0x80000000,           0x0 },
};

/* Interrupt numbers for APLIC. */
#నిర్వచించు UART_INT 4
#నిర్వచించు PCIE2_INT 7

నిర్మాణం BostonState {
    SysBusDevice మాతృ_obj;

    MachineState *mach;
    RISCVCPSState cps;
    SerialMM *uart;

    CharFrontend lcd_display;
    అక్షరం lcd_content[8];
    bool lcd_inited;
};

గణన boston_plat_reg {
    PLAT_FPGA_BUILD     = 0x00,
    PLAT_CORE_CL        = 0x04,
    PLAT_WRAPPER_CL     = 0x08,
    PLAT_SYSCLK_స్థితి  = 0x0c,
    PLAT_SOFTRST_CTL    = 0x10,
#నిర్వచించు PLAT_SOFTRST_CTL_SYSRESET       (1 << 4)
    PLAT_DDR3_స్థితి    = 0x14,
#నిర్వచించు PLAT_DDR3_స్థితి_LOCKED         (1 << 0)
#నిర్వచించు PLAT_DDR3_స్థితి_CALIBRATED     (1 << 2)
#నిర్వచించు PLAT_DDR3_INTERFACE_RESET       (1 << 3)
    PLAT_PCIE_స్థితి    = 0x18,
#నిర్వచించు PLAT_PCIE_స్థితి_PCIE0_LOCKED   (1 << 0)
#నిర్వచించు PLAT_PCIE_స్థితి_PCIE1_LOCKED   (1 << 8)
#నిర్వచించు PLAT_PCIE_స్థితి_PCIE2_LOCKED   (1 << 16)
    PLAT_FLASH_CTL      = 0x1c,
    PLAT_SPARE0         = 0x20,
    PLAT_SPARE1         = 0x24,
    PLAT_SPARE2         = 0x28,
    PLAT_SPARE3         = 0x2c,
    PLAT_MMCM_DIV       = 0x30,
#నిర్వచించు PLAT_MMCM_DIV_CLK0_DIV_జరుపు     0
#నిర్వచించు PLAT_MMCM_DIV_ఆదానం_జరుపు       8
#నిర్వచించు PLAT_MMCM_DIV_MUL_జరుపు         16
#నిర్వచించు PLAT_MMCM_DIV_CLK1_DIV_జరుపు     24
    PLAT_BUILD_CFG      = 0x34,
#నిర్వచించు PLAT_BUILD_CFG_IOCU_EN          (1 << 0)
#నిర్వచించు PLAT_BUILD_CFG_PCIE0_EN         (1 << 1)
#నిర్వచించు PLAT_BUILD_CFG_PCIE1_EN         (1 << 2)
#నిర్వచించు PLAT_BUILD_CFG_PCIE2_EN         (1 << 3)
    PLAT_DDR_CFG        = 0x38,
#నిర్వచించు PLAT_DDR_CFG_కొలత               (0xf << 0)
#నిర్వచించు PLAT_DDR_CFG_MHZ                (0xfff << 4)
    PLAT_NOC_PCIE0_ADDR = 0x3c,
    PLAT_NOC_PCIE1_ADDR = 0x40,
    PLAT_NOC_PCIE2_ADDR = 0x44,
    PLAT_SYS_CTL        = 0x48,
};

స్థిర శూన్య boston_lcd_event(శూన్య *opaque, QEMUChrEvent event)
{
    BostonState *s = opaque;
    ఒకవేళ (event == CHR_EVENT_OPENED && !s->lcd_inited) {
        qemu_chr_fe_printf(&s->lcd_display, "        ");
        s->lcd_inited = true;
    }
}

స్థిర uint64_t boston_lcd_read(శూన్య *opaque, hwaddr addr,
                                గుర్తులేని కొలత)
{
    BostonState *s = opaque;
    uint64_t val = 0;

    ఎంపిక (కొలత) {
    స్థితి 8:
        val |= (uint64_t)s->lcd_content[(addr + 7) & 0x7] << 56;
        val |= (uint64_t)s->lcd_content[(addr + 6) & 0x7] << 48;
        val |= (uint64_t)s->lcd_content[(addr + 5) & 0x7] << 40;
        val |= (uint64_t)s->lcd_content[(addr + 4) & 0x7] << 32;
        /* fall through */
    స్థితి 4:
        val |= (uint64_t)s->lcd_content[(addr + 3) & 0x7] << 24;
        val |= (uint64_t)s->lcd_content[(addr + 2) & 0x7] << 16;
        /* fall through */
    స్థితి 2:
        val |= (uint64_t)s->lcd_content[(addr + 1) & 0x7] << 8;
        /* fall through */
    స్థితి 1:
        val |= (uint64_t)s->lcd_content[(addr + 0) & 0x7];
        ఆపు;
    }

    తిరిగి val;
}

స్థిర శూన్య boston_lcd_write(శూన్య *opaque, hwaddr addr,
                             uint64_t val, గుర్తులేని కొలత)
{
    BostonState *s = opaque;

    ఎంపిక (కొలత) {
    స్థితి 8:
        s->lcd_content[(addr + 7) & 0x7] = val >> 56;
        s->lcd_content[(addr + 6) & 0x7] = val >> 48;
        s->lcd_content[(addr + 5) & 0x7] = val >> 40;
        s->lcd_content[(addr + 4) & 0x7] = val >> 32;
        /* fall through */
    స్థితి 4:
        s->lcd_content[(addr + 3) & 0x7] = val >> 24;
        s->lcd_content[(addr + 2) & 0x7] = val >> 16;
        /* fall through */
    స్థితి 2:
        s->lcd_content[(addr + 1) & 0x7] = val >> 8;
        /* fall through */
    స్థితి 1:
        s->lcd_content[(addr + 0) & 0x7] = val;
        ఆపు;
    }

    qemu_chr_fe_printf(&s->lcd_display,
                       "\r%-8.8s", s->lcd_content);
}

స్థిర స్థిరరాశి జ్ఞాపకం_Region_Ops boston_lcd_ops = {
    .పఠ = boston_lcd_read,
    .లిఖ = boston_lcd_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

స్థిర uint64_t boston_platreg_read(శూన్య *opaque, hwaddr addr,
                                    గుర్తులేని కొలత)
{
    BostonState *s = opaque;
    uint32_t gic_freq, val;

    ఎంపిక (addr & 0xffff) {
    స్థితి PLAT_FPGA_BUILD:
    స్థితి PLAT_CORE_CL:
    స్థితి PLAT_WRAPPER_CL:
        తిరిగి 0;
    స్థితి PLAT_DDR3_స్థితి:
        తిరిగి PLAT_DDR3_స్థితి_LOCKED | PLAT_DDR3_స్థితి_CALIBRATED
               | PLAT_DDR3_INTERFACE_RESET;
    స్థితి PLAT_MMCM_DIV:
        gic_freq = 25000000 / 1000000;
        val = gic_freq << PLAT_MMCM_DIV_ఆదానం_జరుపు;
        val |= 1 << PLAT_MMCM_DIV_MUL_జరుపు;
        val |= 1 << PLAT_MMCM_DIV_CLK0_DIV_జరుపు;
        val |= 1 << PLAT_MMCM_DIV_CLK1_DIV_జరుపు;
        తిరిగి val;
    స్థితి PLAT_BUILD_CFG:
        val = PLAT_BUILD_CFG_PCIE0_EN;
        val |= PLAT_BUILD_CFG_PCIE1_EN;
        val |= PLAT_BUILD_CFG_PCIE2_EN;
        తిరిగి val;
    స్థితి PLAT_DDR_CFG:
        val = s->mach->ram_కొలత / GiB;
        ప్రతిజ్ఞా(!(val & ~PLAT_DDR_CFG_కొలత));
        val |= PLAT_DDR_CFG_MHZ;
        తిరిగి val;
    అప్రమేయ:
        qemu_log_ముసుగు(LOG_UNIMP, "Read platform register 0x%" HWADDR_PRIx "\n",
                      addr & 0xffff);
        తిరిగి 0;
    }
}

స్థిర శూన్య boston_platreg_write(శూన్య *opaque, hwaddr addr,
                                 uint64_t val, గుర్తులేని కొలత)
{
    ఒకవేళ (కొలత != 4) {
        qemu_log_ముసుగు(LOG_UNIMP, "%uB platform register write\n", కొలత);
        తిరిగి;
    }

    ఎంపిక (addr & 0xffff) {
    స్థితి PLAT_FPGA_BUILD:
    స్థితి PLAT_CORE_CL:
    స్థితి PLAT_WRAPPER_CL:
    స్థితి PLAT_DDR3_స్థితి:
    స్థితి PLAT_PCIE_స్థితి:
    స్థితి PLAT_MMCM_DIV:
    స్థితి PLAT_BUILD_CFG:
    స్థితి PLAT_DDR_CFG:
        /* read only */
        ఆపు;
    స్థితి PLAT_SOFTRST_CTL:
        ఒకవేళ (val & PLAT_SOFTRST_CTL_SYSRESET) {
            qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        }
        ఆపు;
    అప్రమేయ:
        qemu_log_ముసుగు(LOG_UNIMP, "Write platform register 0x%" HWADDR_PRIx
                      " = 0x%" PRIx64 "\n", addr & 0xffff, val);
        ఆపు;
    }
}

స్థిర స్థిరరాశి జ్ఞాపకం_Region_Ops boston_platreg_ops = {
    .పఠ = boston_platreg_read,
    .లిఖ = boston_platreg_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_కొలత = 4,
        .max_access_కొలత = 4,
    },
};

స్థిర స్థిరరాశి TypeInfo boston_device = {
    .name          = TYPE_MIPS_BOSTON_AIA,
    .మాతృ        = TYPE_SYS_BUS_DEVICE,
    .instance_కొలత = పరిమాణం(BostonState),
};

స్థిర శూన్య boston_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&boston_device);
}
type_init(boston_రిజిస్టరు_types)

#నిర్వచించు అంకె_INSNS 6
స్థిర శూన్య gen_firmware(uint32_t *p)
{
    పూర్ణాంకం i;
    uint32_t reset_vec[అంకె_INSNS] = {
           /* CM relocate */
           0x1fb802b7,     /* li   t0,0x1fb80000   */
           0x16100337,     /* li   t1,0x16100000   */
           0x0062b423,     /* sd   t1,8(t0)        */
           /* Jump to 0x80000000 */
           0x00100293,     /* li   t0,1            */
           0x01f29293,     /* slli t0,t0,1f        */
           0x00028067      /* jr   t0              */
    };

    కొరకు (i = 0; i < అంకె_INSNS; i++) {
        *p++ = reset_vec[i];
    }
}

స్థిర అంతఃవరుస XilinxPCIEHost *
xilinx_pcie_init(జ్ఞాపకం_Region *sys_mem, uint32_t bus_nr,
                 hwaddr cfg_పునాది, uint64_t cfg_కొలత,
                 hwaddr mmio_పునాది, uint64_t mmio_కొలత,
                 qemu_irq irq)
{
    DeviceState *dev;
    జ్ఞాపకం_Region *cfg, *mmio;

    dev = qdev_new(TYPE_XILINX_PCIE_HOST);

    qdev_prop_సమితి_uint32(dev, "bus_nr", bus_nr);
    qdev_prop_సమితి_uint64(dev, "cfg_base", cfg_పునాది);
    qdev_prop_సమితి_uint64(dev, "cfg_size", cfg_కొలత);
    qdev_prop_సమితి_uint64(dev, "mmio_base", mmio_పునాది);
    qdev_prop_సమితి_uint64(dev, "mmio_size", mmio_కొలత);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    cfg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    జ్ఞాపకం_region_కూడు_subregion_overlap(sys_mem, cfg_పునాది, cfg, 0);

    mmio = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    జ్ఞాపకం_region_కూడు_subregion_overlap(sys_mem, 0, mmio, 0);

    qdev_connect_gpio_బహిర్గతం_named(dev, "interrupt_out", 0, irq);

    తిరిగి XILINX_PCIE_HOST(dev);
}

స్థిర శూన్య boston_mach_init(MachineState *machine)
{
    DeviceState *dev;
    BostonState *s;
    జ్ఞాపకం_Region *flash, *ddr_దిగువ_alias, *lcd, *platreg;
    జ్ఞాపకం_Region *sys_mem = get_system_జ్ఞాపకం();
    XilinxPCIEHost *pcie2;
    PCIDevice *pdev;
    AHCIPCIState *ich9;
    DriveInfo *hd[6];
    Chardev *chr;
    పూర్ణాంకం fw_కొలత;

    ఒకవేళ ((machine->ram_కొలత % GiB) ||
        (machine->ram_కొలత > (4 * GiB))) {
        error_report("Memory size must be 1GB, 2GB, 3GB, or 4GB");
        నిష్క్రమించు(1);
    }

    ఒకవేళ (machine->smp.cpus / machine->smp.cores / machine->smp.దారాలు > 1) {
        error_report(
            "Invalid -smp x,cores=y,threads=z. The max number of clusters "
            "supported is 1");
        నిష్క్రమించు(1);
    }

    dev = qdev_new(TYPE_MIPS_BOSTON_AIA);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    s = BOSTON(dev);
    s->mach = machine;

    object_initialize_బిడ్డ(OBJECT(machine), "cps", &s->cps, TYPE_RISCV_CPS);
    object_property_సమితి_str(OBJECT(&s->cps), "cpu-type", machine->cpu_type,
                            &error_fatal);
    object_property_సమితి_uint(OBJECT(&s->cps), "num-vp", machine->smp.cpus,
                             &error_fatal);
    object_property_సమితి_uint(OBJECT(&s->cps), "num-hart", machine->smp.దారాలు,
                             &error_fatal);
    object_property_సమితి_uint(OBJECT(&s->cps), "num-core", machine->smp.cores,
                             &error_fatal);
    object_property_సమితి_uint(OBJECT(&s->cps), "gcr-base", GCR_పునాది_ADDR,
                             &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->cps), &error_fatal);

    sysbus_mmio_map_overlap(SYS_BUS_DEVICE(&s->cps), 0, 0, 1);

    flash =  g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_region_init_rom(flash, ఖాళీ, "boston.flash",
                           boston_memmap[BOSTON_FLASH].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_FLASH].పునాది,
                                        flash, 0);

    జ్ఞాపకం_region_కూడు_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_HIGHDDR].పునాది,
                                        machine->ram, 0);

    ddr_దిగువ_alias = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_region_init_alias(ddr_దిగువ_alias, ఖాళీ, "boston_low.ddr",
                             machine->ram, 0,
                             MIN(machine->ram_కొలత, (256 * MiB)));
    జ్ఞాపకం_region_కూడు_subregion_overlap(sys_mem, 0, ddr_దిగువ_alias, 0);

    pcie2 = xilinx_pcie_init(sys_mem, 2,
                             boston_memmap[BOSTON_PCIE2].పునాది,
                             boston_memmap[BOSTON_PCIE2].కొలత,
                             boston_memmap[BOSTON_PCIE2_MMIO].పునాది,
                             boston_memmap[BOSTON_PCIE2_MMIO].కొలత,
                             qdev_get_gpio_లోపల(s->cps.aplic, PCIE2_INT));

    platreg = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_region_init_io(platreg, ఖాళీ, &boston_platreg_ops, s,
                          "boston-platregs",
                          boston_memmap[BOSTON_PLATREG].కొలత);
    జ్ఞాపకం_region_కూడు_subregion_overlap(sys_mem,
                          boston_memmap[BOSTON_PLATREG].పునాది, platreg, 0);

    s->uart = serial_mm_init(sys_mem, boston_memmap[BOSTON_UART].పునాది, 2,
                             qdev_get_gpio_లోపల(s->cps.aplic, UART_INT), 10000000,
                             serial_hd(0), DEVICE_LITTLE_ENDIAN);

    lcd = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_region_init_io(lcd, ఖాళీ, &boston_lcd_ops, s, "boston-lcd", 0x8);
    జ్ఞాపకం_region_కూడు_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_LCD].పునాది, lcd, 0);

    chr = qemu_chr_new("lcd", "vc:320x240", ఖాళీ);
    qemu_chr_fe_init(&s->lcd_display, chr, ఖాళీ);
    qemu_chr_fe_సమితి_handlers(&s->lcd_display, ఖాళీ, ఖాళీ,
                             boston_lcd_event, ఖాళీ, s, ఖాళీ, true);

    pdev = pci_create_simple_multifunction(&PCI_BRIDGE(&pcie2->మూలం)->sec_bus,
                                           PCI_DEVFN(0, 0), TYPE_ICH9_AHCI);
    ich9 = ICH9_AHCI(pdev);
    g_assert(ARRAY_కొలత(hd) == ich9->ahci.ports);
    ide_drive_get(hd, ich9->ahci.ports);
    ahci_ide_create_devs(&ich9->ahci, hd);

    /* Create e1000e using slot 0 func 1 */
    pci_init_nic_లోపల_స్థానం(&PCI_BRIDGE(&pcie2->మూలం)->sec_bus, "e1000e", ఖాళీ,
                         "00.1");
    pci_init_nic_devices(&PCI_BRIDGE(&pcie2->మూలం)->sec_bus, "e1000e");

    ఒకవేళ (machine->firmware) {
        fw_కొలత = ఎక్కించు_image_targphys(machine->firmware,
                                      0x1fc00000, 4 * MiB, ఖాళీ);
        ఒకవేళ (fw_కొలత == -1) {
            error_report("unable to load firmware image '%s'",
                          machine->firmware);
            నిష్క్రమించు(1);
        }
        ఒకవేళ (machine->kernel_filename) {
                fw_కొలత = ఎక్కించు_image_targphys(machine->kernel_filename,
                                              0x80000000, 64 * MiB, ఖాళీ);
                ఒకవేళ (fw_కొలత == -1) {
                    error_report("unable to load kernel image '%s'",
                                  machine->kernel_filename);
                    నిష్క్రమించు(1);
                }
        }
    } లేకపోతే ఒకవేళ (machine->kernel_filename) {
        fw_కొలత = ఎక్కించు_image_targphys(machine->kernel_filename,
                                      0x80000000, 64 * MiB, ఖాళీ);
        ఒకవేళ (fw_కొలత == -1) {
            error_report("unable to load kernel image '%s'",
                          machine->kernel_filename);
            నిష్క్రమించు(1);
        }

        gen_firmware(జ్ఞాపకం_region_get_ram_ptr(flash) + 0x7c00000);
    } లేకపోతే ఒకవేళ (!qtest_enabled()) {
        error_report("Please provide either a -kernel or -bios argument");
        నిష్క్రమించు(1);
    }
}

స్థిర శూన్య boston_mach_class_init(MachineClass *mc)
{
    mc->desc = "MIPS Boston-aia";
    mc->init = boston_mach_init;
    mc->ఖండం_default_type = IF_IDE;
    mc->default_ram_కొలత = 2 * GiB;
    mc->default_ram_id = "boston.ddr";
    mc->max_cpus = MAX_HARTS;
    mc->default_cpu_type = TYPE_RISCV_CPU_MIPS_P8700;
}

DEFINE_MACHINE("boston-aia", boston_mach_class_init)
