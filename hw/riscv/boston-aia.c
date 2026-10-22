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

#شامل "qemu/osdep.h"
#شامل "qemu/units.h"

#شامل "hw/core/boards.h"
#شامل "hw/char/serial-mm.h"
#شامل "hw/ide/pci.h"
#شامل "hw/ide/ahci-pci.h"
#شامل "hw/core/loader.h"
#شامل "hw/riscv/cps.h"
#شامل "hw/pci-host/xilinx-pcie.h"
#شامل "hw/core/qdev-properties.h"
#شامل "qapi/error.h"
#شامل "qemu/error-report.h"
#شامل "qemu/log.h"
#شامل "chardev/char.h"
#شامل "system/address-spaces.h"
#شامل "system/device_tree.h"
#شامل "system/system.h"
#شامل "system/qtest.h"
#شامل "system/runstate.h"

#شامل <libfdt.h>
#شامل "qom/object.h"

#تعریف_کرو TYPE_MIPS_BOSTON_AIA "mips-boston-aia"
قسمتعریف ساخت BostonState BostonState;
DECLARE_INSTANCE_CHECKER(BostonState, BOSTON,
                         TYPE_MIPS_BOSTON_AIA)

شمار {
    BOSTON_PCIE2,
    BOSTON_PCIE2_MMIO,
    BOSTON_PLATREG,
    BOSTON_UART,
    BOSTON_LCD,
    BOSTON_FLASH,
    BOSTON_HIGHDDR,
};

ساکن ثابت Mem_Map_اندراج boston_memmap[] = {
    [BOSTON_PCIE2] =      { 0x14000000,     0x2000000 },
    [BOSTON_PCIE2_MMIO] = { 0x16000000,      0x100000 },
    [BOSTON_PLATREG] =    { 0x17ffd000,        0x1000 },
    [BOSTON_UART] =       { 0x17ffe000,          0x20 },
    [BOSTON_LCD] =        { 0x17fff000,           0x8 },
    [BOSTON_FLASH] =      { 0x18000000,     0x8000000 },
    [BOSTON_HIGHDDR] =    { 0x80000000,           0x0 },
};

/* Interrupt numbers for APLIC. */
#تعریف_کرو UART_INT 4
#تعریف_کرو PCIE2_INT 7

ساخت BostonState {
    SysBusDevice والد_obj;

    MachineState *mach;
    RISCVCPSState cps;
    SerialMM *uart;

    CharFrontend lcd_display;
    حرف lcd_content[8];
    bool lcd_inited;
};

شمار boston_plat_reg {
    PLAT_FPGA_BUILD     = 0x00,
    PLAT_CORE_CL        = 0x04,
    PLAT_WRAPPER_CL     = 0x08,
    PLAT_SYSCLK_حالت  = 0x0c,
    PLAT_SOFTRST_CTL    = 0x10,
#تعریف_کرو PLAT_SOFTRST_CTL_SYSRESET       (1 << 4)
    PLAT_DDR3_حالت    = 0x14,
#تعریف_کرو PLAT_DDR3_حالت_LOCKED         (1 << 0)
#تعریف_کرو PLAT_DDR3_حالت_CALIBRATED     (1 << 2)
#تعریف_کرو PLAT_DDR3_INTERFACE_RESET       (1 << 3)
    PLAT_PCIE_حالت    = 0x18,
#تعریف_کرو PLAT_PCIE_حالت_PCIE0_LOCKED   (1 << 0)
#تعریف_کرو PLAT_PCIE_حالت_PCIE1_LOCKED   (1 << 8)
#تعریف_کرو PLAT_PCIE_حالت_PCIE2_LOCKED   (1 << 16)
    PLAT_FLASH_CTL      = 0x1c,
    PLAT_SPARE0         = 0x20,
    PLAT_SPARE1         = 0x24,
    PLAT_SPARE2         = 0x28,
    PLAT_SPARE3         = 0x2c,
    PLAT_MMCM_DIV       = 0x30,
#تعریف_کرو PLAT_MMCM_DIV_CLK0_DIV_سرکاؤ     0
#تعریف_کرو PLAT_MMCM_DIV_داخلہ_سرکاؤ       8
#تعریف_کرو PLAT_MMCM_DIV_MUL_سرکاؤ         16
#تعریف_کرو PLAT_MMCM_DIV_CLK1_DIV_سرکاؤ     24
    PLAT_BUILD_CFG      = 0x34,
#تعریف_کرو PLAT_BUILD_CFG_IOCU_EN          (1 << 0)
#تعریف_کرو PLAT_BUILD_CFG_PCIE0_EN         (1 << 1)
#تعریف_کرو PLAT_BUILD_CFG_PCIE1_EN         (1 << 2)
#تعریف_کرو PLAT_BUILD_CFG_PCIE2_EN         (1 << 3)
    PLAT_DDR_CFG        = 0x38,
#تعریف_کرو PLAT_DDR_CFG_سائز               (0xf << 0)
#تعریف_کرو PLAT_DDR_CFG_MHZ                (0xfff << 4)
    PLAT_NOC_PCIE0_ADDR = 0x3c,
    PLAT_NOC_PCIE1_ADDR = 0x40,
    PLAT_NOC_PCIE2_ADDR = 0x44,
    PLAT_SYS_CTL        = 0x48,
};

ساکن خالی boston_lcd_event(خالی *opaque, QEMUChrEvent event)
{
    BostonState *s = opaque;
    اگر (event == CHR_EVENT_OPENED && !s->lcd_inited) {
        qemu_chr_fe_printf(&s->lcd_display, "        ");
        s->lcd_inited = true;
    }
}

ساکن uint64_t boston_lcd_read(خالی *opaque, hwaddr addr,
                                غیرعلامتی سائز)
{
    BostonState *s = opaque;
    uint64_t val = 0;

    انتخاب (سائز) {
    حالت 8:
        val |= (uint64_t)s->lcd_content[(addr + 7) & 0x7] << 56;
        val |= (uint64_t)s->lcd_content[(addr + 6) & 0x7] << 48;
        val |= (uint64_t)s->lcd_content[(addr + 5) & 0x7] << 40;
        val |= (uint64_t)s->lcd_content[(addr + 4) & 0x7] << 32;
        /* fall through */
    حالت 4:
        val |= (uint64_t)s->lcd_content[(addr + 3) & 0x7] << 24;
        val |= (uint64_t)s->lcd_content[(addr + 2) & 0x7] << 16;
        /* fall through */
    حالت 2:
        val |= (uint64_t)s->lcd_content[(addr + 1) & 0x7] << 8;
        /* fall through */
    حالت 1:
        val |= (uint64_t)s->lcd_content[(addr + 0) & 0x7];
        توڑو;
    }

    واپس val;
}

ساکن خالی boston_lcd_write(خالی *opaque, hwaddr addr,
                             uint64_t val, غیرعلامتی سائز)
{
    BostonState *s = opaque;

    انتخاب (سائز) {
    حالت 8:
        s->lcd_content[(addr + 7) & 0x7] = val >> 56;
        s->lcd_content[(addr + 6) & 0x7] = val >> 48;
        s->lcd_content[(addr + 5) & 0x7] = val >> 40;
        s->lcd_content[(addr + 4) & 0x7] = val >> 32;
        /* fall through */
    حالت 4:
        s->lcd_content[(addr + 3) & 0x7] = val >> 24;
        s->lcd_content[(addr + 2) & 0x7] = val >> 16;
        /* fall through */
    حالت 2:
        s->lcd_content[(addr + 1) & 0x7] = val >> 8;
        /* fall through */
    حالت 1:
        s->lcd_content[(addr + 0) & 0x7] = val;
        توڑو;
    }

    qemu_chr_fe_printf(&s->lcd_display,
                       "\r%-8.8s", s->lcd_content);
}

ساکن ثابت حافظہ_Region_Ops boston_lcd_ops = {
    .اقرأ = boston_lcd_read,
    .اكتب = boston_lcd_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

ساکن uint64_t boston_platreg_read(خالی *opaque, hwaddr addr,
                                    غیرعلامتی سائز)
{
    BostonState *s = opaque;
    uint32_t gic_freq, val;

    انتخاب (addr & 0xffff) {
    حالت PLAT_FPGA_BUILD:
    حالت PLAT_CORE_CL:
    حالت PLAT_WRAPPER_CL:
        واپس 0;
    حالت PLAT_DDR3_حالت:
        واپس PLAT_DDR3_حالت_LOCKED | PLAT_DDR3_حالت_CALIBRATED
               | PLAT_DDR3_INTERFACE_RESET;
    حالت PLAT_MMCM_DIV:
        gic_freq = 25000000 / 1000000;
        val = gic_freq << PLAT_MMCM_DIV_داخلہ_سرکاؤ;
        val |= 1 << PLAT_MMCM_DIV_MUL_سرکاؤ;
        val |= 1 << PLAT_MMCM_DIV_CLK0_DIV_سرکاؤ;
        val |= 1 << PLAT_MMCM_DIV_CLK1_DIV_سرکاؤ;
        واپس val;
    حالت PLAT_BUILD_CFG:
        val = PLAT_BUILD_CFG_PCIE0_EN;
        val |= PLAT_BUILD_CFG_PCIE1_EN;
        val |= PLAT_BUILD_CFG_PCIE2_EN;
        واپس val;
    حالت PLAT_DDR_CFG:
        val = s->mach->ram_سائز / GiB;
        أكد(!(val & ~PLAT_DDR_CFG_سائز));
        val |= PLAT_DDR_CFG_MHZ;
        واپس val;
    طےشدہ:
        qemu_log_نقاب(LOG_UNIMP, "Read platform register 0x%" HWADDR_PRIx "\n",
                      addr & 0xffff);
        واپس 0;
    }
}

ساکن خالی boston_platreg_write(خالی *opaque, hwaddr addr,
                                 uint64_t val, غیرعلامتی سائز)
{
    اگر (سائز != 4) {
        qemu_log_نقاب(LOG_UNIMP, "%uB platform register write\n", سائز);
        واپس;
    }

    انتخاب (addr & 0xffff) {
    حالت PLAT_FPGA_BUILD:
    حالت PLAT_CORE_CL:
    حالت PLAT_WRAPPER_CL:
    حالت PLAT_DDR3_حالت:
    حالت PLAT_PCIE_حالت:
    حالت PLAT_MMCM_DIV:
    حالت PLAT_BUILD_CFG:
    حالت PLAT_DDR_CFG:
        /* read only */
        توڑو;
    حالت PLAT_SOFTRST_CTL:
        اگر (val & PLAT_SOFTRST_CTL_SYSRESET) {
            qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        }
        توڑو;
    طےشدہ:
        qemu_log_نقاب(LOG_UNIMP, "Write platform register 0x%" HWADDR_PRIx
                      " = 0x%" PRIx64 "\n", addr & 0xffff, val);
        توڑو;
    }
}

ساکن ثابت حافظہ_Region_Ops boston_platreg_ops = {
    .اقرأ = boston_platreg_read,
    .اكتب = boston_platreg_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_سائز = 4,
        .max_access_سائز = 4,
    },
};

ساکن ثابت TypeInfo boston_device = {
    .name          = TYPE_MIPS_BOSTON_AIA,
    .والد        = TYPE_SYS_BUS_DEVICE,
    .instance_سائز = حجم(BostonState),
};

ساکن خالی boston_رجسٹر_types(خالی)
{
    type_رجسٹر_static(&boston_device);
}
type_init(boston_رجسٹر_types)

#تعریف_کرو عدد_INSNS 6
ساکن خالی gen_firmware(uint32_t *p)
{
    صحیح i;
    uint32_t reset_vec[عدد_INSNS] = {
           /* CM relocate */
           0x1fb802b7,     /* li   t0,0x1fb80000   */
           0x16100337,     /* li   t1,0x16100000   */
           0x0062b423,     /* sd   t1,8(t0)        */
           /* Jump to 0x80000000 */
           0x00100293,     /* li   t0,1            */
           0x01f29293,     /* slli t0,t0,1f        */
           0x00028067      /* jr   t0              */
    };

    برائے (i = 0; i < عدد_INSNS; i++) {
        *p++ = reset_vec[i];
    }
}

ساکن اندرونی XilinxPCIEHost *
xilinx_pcie_init(حافظہ_Region *sys_mem, uint32_t bus_nr,
                 hwaddr cfg_بنیاد, uint64_t cfg_سائز,
                 hwaddr mmio_بنیاد, uint64_t mmio_سائز,
                 qemu_irq irq)
{
    DeviceState *dev;
    حافظہ_Region *cfg, *mmio;

    dev = qdev_new(TYPE_XILINX_PCIE_HOST);

    qdev_prop_سیٹ_uint32(dev, "bus_nr", bus_nr);
    qdev_prop_سیٹ_uint64(dev, "cfg_base", cfg_بنیاد);
    qdev_prop_سیٹ_uint64(dev, "cfg_size", cfg_سائز);
    qdev_prop_سیٹ_uint64(dev, "mmio_base", mmio_بنیاد);
    qdev_prop_سیٹ_uint64(dev, "mmio_size", mmio_سائز);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    cfg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    حافظہ_region_جوڑو_subregion_overlap(sys_mem, cfg_بنیاد, cfg, 0);

    mmio = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    حافظہ_region_جوڑو_subregion_overlap(sys_mem, 0, mmio, 0);

    qdev_connect_gpio_برآمد_named(dev, "interrupt_out", 0, irq);

    واپس XILINX_PCIE_HOST(dev);
}

ساکن خالی boston_mach_init(MachineState *machine)
{
    DeviceState *dev;
    BostonState *s;
    حافظہ_Region *flash, *ddr_کم_alias, *lcd, *platreg;
    حافظہ_Region *sys_mem = get_system_حافظہ();
    XilinxPCIEHost *pcie2;
    PCIDevice *pdev;
    AHCIPCIState *ich9;
    DriveInfo *hd[6];
    Chardev *chr;
    صحیح fw_سائز;

    اگر ((machine->ram_سائز % GiB) ||
        (machine->ram_سائز > (4 * GiB))) {
        error_report("Memory size must be 1GB, 2GB, 3GB, or 4GB");
        باہر_نکلو(1);
    }

    اگر (machine->smp.cpus / machine->smp.cores / machine->smp.دھاگے > 1) {
        error_report(
            "Invalid -smp x,cores=y,threads=z. The max number of clusters "
            "supported is 1");
        باہر_نکلو(1);
    }

    dev = qdev_new(TYPE_MIPS_BOSTON_AIA);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    s = BOSTON(dev);
    s->mach = machine;

    object_initialize_بچہ(OBJECT(machine), "cps", &s->cps, TYPE_RISCV_CPS);
    object_property_سیٹ_str(OBJECT(&s->cps), "cpu-type", machine->cpu_type,
                            &error_fatal);
    object_property_سیٹ_uint(OBJECT(&s->cps), "num-vp", machine->smp.cpus,
                             &error_fatal);
    object_property_سیٹ_uint(OBJECT(&s->cps), "num-hart", machine->smp.دھاگے,
                             &error_fatal);
    object_property_سیٹ_uint(OBJECT(&s->cps), "num-core", machine->smp.cores,
                             &error_fatal);
    object_property_سیٹ_uint(OBJECT(&s->cps), "gcr-base", GCR_بنیاد_ADDR,
                             &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->cps), &error_fatal);

    sysbus_mmio_map_overlap(SYS_BUS_DEVICE(&s->cps), 0, 0, 1);

    flash =  g_new(حافظہ_Region, 1);
    حافظہ_region_init_rom(flash, تہی, "boston.flash",
                           boston_memmap[BOSTON_FLASH].سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_FLASH].بنیاد,
                                        flash, 0);

    حافظہ_region_جوڑو_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_HIGHDDR].بنیاد,
                                        machine->ram, 0);

    ddr_کم_alias = g_new(حافظہ_Region, 1);
    حافظہ_region_init_alias(ddr_کم_alias, تہی, "boston_low.ddr",
                             machine->ram, 0,
                             MIN(machine->ram_سائز, (256 * MiB)));
    حافظہ_region_جوڑو_subregion_overlap(sys_mem, 0, ddr_کم_alias, 0);

    pcie2 = xilinx_pcie_init(sys_mem, 2,
                             boston_memmap[BOSTON_PCIE2].بنیاد,
                             boston_memmap[BOSTON_PCIE2].سائز,
                             boston_memmap[BOSTON_PCIE2_MMIO].بنیاد,
                             boston_memmap[BOSTON_PCIE2_MMIO].سائز,
                             qdev_get_gpio_اندر(s->cps.aplic, PCIE2_INT));

    platreg = g_new(حافظہ_Region, 1);
    حافظہ_region_init_io(platreg, تہی, &boston_platreg_ops, s,
                          "boston-platregs",
                          boston_memmap[BOSTON_PLATREG].سائز);
    حافظہ_region_جوڑو_subregion_overlap(sys_mem,
                          boston_memmap[BOSTON_PLATREG].بنیاد, platreg, 0);

    s->uart = serial_mm_init(sys_mem, boston_memmap[BOSTON_UART].بنیاد, 2,
                             qdev_get_gpio_اندر(s->cps.aplic, UART_INT), 10000000,
                             serial_hd(0), DEVICE_LITTLE_ENDIAN);

    lcd = g_new(حافظہ_Region, 1);
    حافظہ_region_init_io(lcd, تہی, &boston_lcd_ops, s, "boston-lcd", 0x8);
    حافظہ_region_جوڑو_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_LCD].بنیاد, lcd, 0);

    chr = qemu_chr_new("lcd", "vc:320x240", تہی);
    qemu_chr_fe_init(&s->lcd_display, chr, تہی);
    qemu_chr_fe_سیٹ_handlers(&s->lcd_display, تہی, تہی,
                             boston_lcd_event, تہی, s, تہی, true);

    pdev = pci_create_simple_multifunction(&PCI_BRIDGE(&pcie2->جڑ)->sec_bus,
                                           PCI_DEVFN(0, 0), TYPE_ICH9_AHCI);
    ich9 = ICH9_AHCI(pdev);
    g_assert(ARRAY_سائز(hd) == ich9->ahci.ports);
    ide_drive_get(hd, ich9->ahci.ports);
    ahci_ide_create_devs(&ich9->ahci, hd);

    /* Create e1000e using slot 0 func 1 */
    pci_init_nic_اندر_خانہ(&PCI_BRIDGE(&pcie2->جڑ)->sec_bus, "e1000e", تہی,
                         "00.1");
    pci_init_nic_devices(&PCI_BRIDGE(&pcie2->جڑ)->sec_bus, "e1000e");

    اگر (machine->firmware) {
        fw_سائز = بھرو_image_targphys(machine->firmware,
                                      0x1fc00000, 4 * MiB, تہی);
        اگر (fw_سائز == -1) {
            error_report("unable to load firmware image '%s'",
                          machine->firmware);
            باہر_نکلو(1);
        }
        اگر (machine->kernel_filename) {
                fw_سائز = بھرو_image_targphys(machine->kernel_filename,
                                              0x80000000, 64 * MiB, تہی);
                اگر (fw_سائز == -1) {
                    error_report("unable to load kernel image '%s'",
                                  machine->kernel_filename);
                    باہر_نکلو(1);
                }
        }
    } ورنہ اگر (machine->kernel_filename) {
        fw_سائز = بھرو_image_targphys(machine->kernel_filename,
                                      0x80000000, 64 * MiB, تہی);
        اگر (fw_سائز == -1) {
            error_report("unable to load kernel image '%s'",
                          machine->kernel_filename);
            باہر_نکلو(1);
        }

        gen_firmware(حافظہ_region_get_ram_ptr(flash) + 0x7c00000);
    } ورنہ اگر (!qtest_enabled()) {
        error_report("Please provide either a -kernel or -bios argument");
        باہر_نکلو(1);
    }
}

ساکن خالی boston_mach_class_init(MachineClass *mc)
{
    mc->desc = "MIPS Boston-aia";
    mc->init = boston_mach_init;
    mc->بلاک_default_type = IF_IDE;
    mc->default_ram_سائز = 2 * GiB;
    mc->default_ram_id = "boston.ddr";
    mc->max_cpus = MAX_HARTS;
    mc->default_cpu_type = TYPE_RISCV_CPU_MIPS_P8700;
}

DEFINE_MACHINE("boston-aia", boston_mach_class_init)
