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

#ضمن "qemu/osdep.h"
#ضمن "qemu/units.h"

#ضمن "hw/core/boards.h"
#ضمن "hw/char/serial-mm.h"
#ضمن "hw/ide/pci.h"
#ضمن "hw/ide/ahci-pci.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/riscv/cps.h"
#ضمن "hw/pci-host/xilinx-pcie.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "qapi/error.h"
#ضمن "qemu/error-report.h"
#ضمن "qemu/log.h"
#ضمن "chardev/char.h"
#ضمن "system/address-spaces.h"
#ضمن "system/device_tree.h"
#ضمن "system/system.h"
#ضمن "system/qtest.h"
#ضمن "system/runstate.h"

#ضمن <libfdt.h>
#ضمن "qom/object.h"

#تعریف_کړه TYPE_MIPS_BOSTON_AIA "mips-boston-aia"
تعريفنوع بنية BostonState BostonState;
DECLARE_INSTANCE_CHECKER(BostonState, BOSTON,
                         TYPE_MIPS_BOSTON_AIA)

تعداد {
    BOSTON_PCIE2,
    BOSTON_PCIE2_MMIO,
    BOSTON_PLATREG,
    BOSTON_UART,
    BOSTON_LCD,
    BOSTON_FLASH,
    BOSTON_HIGHDDR,
};

ساكن ثابت Mem_Map_ثبت boston_memmap[] = {
    [BOSTON_PCIE2] =      { 0x14000000,     0x2000000 },
    [BOSTON_PCIE2_MMIO] = { 0x16000000,      0x100000 },
    [BOSTON_PLATREG] =    { 0x17ffd000,        0x1000 },
    [BOSTON_UART] =       { 0x17ffe000,          0x20 },
    [BOSTON_LCD] =        { 0x17fff000,           0x8 },
    [BOSTON_FLASH] =      { 0x18000000,     0x8000000 },
    [BOSTON_HIGHDDR] =    { 0x80000000,           0x0 },
};

/* Interrupt numbers for APLIC. */
#تعریف_کړه UART_INT 4
#تعریف_کړه PCIE2_INT 7

بنية BostonState {
    SysBusDevice پلار_obj;

    MachineState *mach;
    RISCVCPSState cps;
    SerialMM *uart;

    CharFrontend lcd_display;
    حرف lcd_content[8];
    bool lcd_inited;
};

تعداد boston_plat_reg {
    PLAT_FPGA_BUILD     = 0x00,
    PLAT_CORE_CL        = 0x04,
    PLAT_WRAPPER_CL     = 0x08,
    PLAT_SYSCLK_حالت  = 0x0c,
    PLAT_SOFTRST_CTL    = 0x10,
#تعریف_کړه PLAT_SOFTRST_CTL_SYSRESET       (1 << 4)
    PLAT_DDR3_حالت    = 0x14,
#تعریف_کړه PLAT_DDR3_حالت_LOCKED         (1 << 0)
#تعریف_کړه PLAT_DDR3_حالت_CALIBRATED     (1 << 2)
#تعریف_کړه PLAT_DDR3_INTERFACE_RESET       (1 << 3)
    PLAT_PCIE_حالت    = 0x18,
#تعریف_کړه PLAT_PCIE_حالت_PCIE0_LOCKED   (1 << 0)
#تعریف_کړه PLAT_PCIE_حالت_PCIE1_LOCKED   (1 << 8)
#تعریف_کړه PLAT_PCIE_حالت_PCIE2_LOCKED   (1 << 16)
    PLAT_FLASH_CTL      = 0x1c,
    PLAT_SPARE0         = 0x20,
    PLAT_SPARE1         = 0x24,
    PLAT_SPARE2         = 0x28,
    PLAT_SPARE3         = 0x2c,
    PLAT_MMCM_DIV       = 0x30,
#تعریف_کړه PLAT_MMCM_DIV_CLK0_DIV_ښويدنه     0
#تعریف_کړه PLAT_MMCM_DIV_ننوت_ښويدنه       8
#تعریف_کړه PLAT_MMCM_DIV_MUL_ښويدنه         16
#تعریف_کړه PLAT_MMCM_DIV_CLK1_DIV_ښويدنه     24
    PLAT_BUILD_CFG      = 0x34,
#تعریف_کړه PLAT_BUILD_CFG_IOCU_EN          (1 << 0)
#تعریف_کړه PLAT_BUILD_CFG_PCIE0_EN         (1 << 1)
#تعریف_کړه PLAT_BUILD_CFG_PCIE1_EN         (1 << 2)
#تعریف_کړه PLAT_BUILD_CFG_PCIE2_EN         (1 << 3)
    PLAT_DDR_CFG        = 0x38,
#تعریف_کړه PLAT_DDR_CFG_اندازه               (0xf << 0)
#تعریف_کړه PLAT_DDR_CFG_MHZ                (0xfff << 4)
    PLAT_NOC_PCIE0_ADDR = 0x3c,
    PLAT_NOC_PCIE1_ADDR = 0x40,
    PLAT_NOC_PCIE2_ADDR = 0x44,
    PLAT_SYS_CTL        = 0x48,
};

ساكن فراغ boston_lcd_event(فراغ *opaque, QEMUChrEvent event)
{
    BostonState *s = opaque;
    إذا (event == CHR_EVENT_OPENED && !s->lcd_inited) {
        qemu_chr_fe_printf(&s->lcd_display, "        ");
        s->lcd_inited = true;
    }
}

ساكن uint64_t boston_lcd_read(فراغ *opaque, hwaddr addr,
                                بینښه اندازه)
{
    BostonState *s = opaque;
    uint64_t val = 0;

    بدل (اندازه) {
    حالة 8:
        val |= (uint64_t)s->lcd_content[(addr + 7) & 0x7] << 56;
        val |= (uint64_t)s->lcd_content[(addr + 6) & 0x7] << 48;
        val |= (uint64_t)s->lcd_content[(addr + 5) & 0x7] << 40;
        val |= (uint64_t)s->lcd_content[(addr + 4) & 0x7] << 32;
        /* fall through */
    حالة 4:
        val |= (uint64_t)s->lcd_content[(addr + 3) & 0x7] << 24;
        val |= (uint64_t)s->lcd_content[(addr + 2) & 0x7] << 16;
        /* fall through */
    حالة 2:
        val |= (uint64_t)s->lcd_content[(addr + 1) & 0x7] << 8;
        /* fall through */
    حالة 1:
        val |= (uint64_t)s->lcd_content[(addr + 0) & 0x7];
        اكسر;
    }

    أرجع val;
}

ساكن فراغ boston_lcd_write(فراغ *opaque, hwaddr addr,
                             uint64_t val, بینښه اندازه)
{
    BostonState *s = opaque;

    بدل (اندازه) {
    حالة 8:
        s->lcd_content[(addr + 7) & 0x7] = val >> 56;
        s->lcd_content[(addr + 6) & 0x7] = val >> 48;
        s->lcd_content[(addr + 5) & 0x7] = val >> 40;
        s->lcd_content[(addr + 4) & 0x7] = val >> 32;
        /* fall through */
    حالة 4:
        s->lcd_content[(addr + 3) & 0x7] = val >> 24;
        s->lcd_content[(addr + 2) & 0x7] = val >> 16;
        /* fall through */
    حالة 2:
        s->lcd_content[(addr + 1) & 0x7] = val >> 8;
        /* fall through */
    حالة 1:
        s->lcd_content[(addr + 0) & 0x7] = val;
        اكسر;
    }

    qemu_chr_fe_printf(&s->lcd_display,
                       "\r%-8.8s", s->lcd_content);
}

ساكن ثابت حافظه_Region_Ops boston_lcd_ops = {
    .اقرأ = boston_lcd_read,
    .اكتب = boston_lcd_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

ساكن uint64_t boston_platreg_read(فراغ *opaque, hwaddr addr,
                                    بینښه اندازه)
{
    BostonState *s = opaque;
    uint32_t gic_freq, val;

    بدل (addr & 0xffff) {
    حالة PLAT_FPGA_BUILD:
    حالة PLAT_CORE_CL:
    حالة PLAT_WRAPPER_CL:
        أرجع 0;
    حالة PLAT_DDR3_حالت:
        أرجع PLAT_DDR3_حالت_LOCKED | PLAT_DDR3_حالت_CALIBRATED
               | PLAT_DDR3_INTERFACE_RESET;
    حالة PLAT_MMCM_DIV:
        gic_freq = 25000000 / 1000000;
        val = gic_freq << PLAT_MMCM_DIV_ننوت_ښويدنه;
        val |= 1 << PLAT_MMCM_DIV_MUL_ښويدنه;
        val |= 1 << PLAT_MMCM_DIV_CLK0_DIV_ښويدنه;
        val |= 1 << PLAT_MMCM_DIV_CLK1_DIV_ښويدنه;
        أرجع val;
    حالة PLAT_BUILD_CFG:
        val = PLAT_BUILD_CFG_PCIE0_EN;
        val |= PLAT_BUILD_CFG_PCIE1_EN;
        val |= PLAT_BUILD_CFG_PCIE2_EN;
        أرجع val;
    حالة PLAT_DDR_CFG:
        val = s->mach->ram_اندازه / GiB;
        أكد(!(val & ~PLAT_DDR_CFG_اندازه));
        val |= PLAT_DDR_CFG_MHZ;
        أرجع val;
    افتراضي:
        qemu_log_ماسک(LOG_UNIMP, "Read platform register 0x%" HWADDR_PRIx "\n",
                      addr & 0xffff);
        أرجع 0;
    }
}

ساكن فراغ boston_platreg_write(فراغ *opaque, hwaddr addr,
                                 uint64_t val, بینښه اندازه)
{
    إذا (اندازه != 4) {
        qemu_log_ماسک(LOG_UNIMP, "%uB platform register write\n", اندازه);
        أرجع;
    }

    بدل (addr & 0xffff) {
    حالة PLAT_FPGA_BUILD:
    حالة PLAT_CORE_CL:
    حالة PLAT_WRAPPER_CL:
    حالة PLAT_DDR3_حالت:
    حالة PLAT_PCIE_حالت:
    حالة PLAT_MMCM_DIV:
    حالة PLAT_BUILD_CFG:
    حالة PLAT_DDR_CFG:
        /* read only */
        اكسر;
    حالة PLAT_SOFTRST_CTL:
        إذا (val & PLAT_SOFTRST_CTL_SYSRESET) {
            qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        }
        اكسر;
    افتراضي:
        qemu_log_ماسک(LOG_UNIMP, "Write platform register 0x%" HWADDR_PRIx
                      " = 0x%" PRIx64 "\n", addr & 0xffff, val);
        اكسر;
    }
}

ساكن ثابت حافظه_Region_Ops boston_platreg_ops = {
    .اقرأ = boston_platreg_read,
    .اكتب = boston_platreg_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_اندازه = 4,
        .max_access_اندازه = 4,
    },
};

ساكن ثابت TypeInfo boston_device = {
    .name          = TYPE_MIPS_BOSTON_AIA,
    .پلار        = TYPE_SYS_BUS_DEVICE,
    .instance_اندازه = حجم(BostonState),
};

ساكن فراغ boston_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&boston_device);
}
type_init(boston_ثبتوونکی_types)

#تعریف_کړه عدد_INSNS 6
ساكن فراغ gen_firmware(uint32_t *p)
{
    صحيح i;
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

    لكل (i = 0; i < عدد_INSNS; i++) {
        *p++ = reset_vec[i];
    }
}

ساكن مضمن XilinxPCIEHost *
xilinx_pcie_init(حافظه_Region *sys_mem, uint32_t bus_nr,
                 hwaddr cfg_بنسټ, uint64_t cfg_اندازه,
                 hwaddr mmio_بنسټ, uint64_t mmio_اندازه,
                 qemu_irq irq)
{
    DeviceState *dev;
    حافظه_Region *cfg, *mmio;

    dev = qdev_new(TYPE_XILINX_PCIE_HOST);

    qdev_prop_سېټ_uint32(dev, "bus_nr", bus_nr);
    qdev_prop_سېټ_uint64(dev, "cfg_base", cfg_بنسټ);
    qdev_prop_سېټ_uint64(dev, "cfg_size", cfg_اندازه);
    qdev_prop_سېټ_uint64(dev, "mmio_base", mmio_بنسټ);
    qdev_prop_سېټ_uint64(dev, "mmio_size", mmio_اندازه);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    cfg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    حافظه_region_جمع_کړه_subregion_overlap(sys_mem, cfg_بنسټ, cfg, 0);

    mmio = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    حافظه_region_جمع_کړه_subregion_overlap(sys_mem, 0, mmio, 0);

    qdev_connect_gpio_بهر_named(dev, "interrupt_out", 0, irq);

    أرجع XILINX_PCIE_HOST(dev);
}

ساكن فراغ boston_mach_init(MachineState *machine)
{
    DeviceState *dev;
    BostonState *s;
    حافظه_Region *flash, *ddr_ټیټ_alias, *lcd, *platreg;
    حافظه_Region *sys_mem = get_system_حافظه();
    XilinxPCIEHost *pcie2;
    PCIDevice *pdev;
    AHCIPCIState *ich9;
    DriveInfo *hd[6];
    Chardev *chr;
    صحيح fw_اندازه;

    إذا ((machine->ram_اندازه % GiB) ||
        (machine->ram_اندازه > (4 * GiB))) {
        error_report("Memory size must be 1GB, 2GB, 3GB, or 4GB");
        وتل(1);
    }

    إذا (machine->smp.cpus / machine->smp.cores / machine->smp.تارونه > 1) {
        error_report(
            "Invalid -smp x,cores=y,threads=z. The max number of clusters "
            "supported is 1");
        وتل(1);
    }

    dev = qdev_new(TYPE_MIPS_BOSTON_AIA);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    s = BOSTON(dev);
    s->mach = machine;

    object_initialize_ماشوم(OBJECT(machine), "cps", &s->cps, TYPE_RISCV_CPS);
    object_property_سېټ_str(OBJECT(&s->cps), "cpu-type", machine->cpu_type,
                            &error_fatal);
    object_property_سېټ_uint(OBJECT(&s->cps), "num-vp", machine->smp.cpus,
                             &error_fatal);
    object_property_سېټ_uint(OBJECT(&s->cps), "num-hart", machine->smp.تارونه,
                             &error_fatal);
    object_property_سېټ_uint(OBJECT(&s->cps), "num-core", machine->smp.cores,
                             &error_fatal);
    object_property_سېټ_uint(OBJECT(&s->cps), "gcr-base", GCR_بنسټ_ADDR,
                             &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->cps), &error_fatal);

    sysbus_mmio_map_overlap(SYS_BUS_DEVICE(&s->cps), 0, 0, 1);

    flash =  g_new(حافظه_Region, 1);
    حافظه_region_init_rom(flash, عدم, "boston.flash",
                           boston_memmap[BOSTON_FLASH].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_FLASH].بنسټ,
                                        flash, 0);

    حافظه_region_جمع_کړه_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_HIGHDDR].بنسټ,
                                        machine->ram, 0);

    ddr_ټیټ_alias = g_new(حافظه_Region, 1);
    حافظه_region_init_alias(ddr_ټیټ_alias, عدم, "boston_low.ddr",
                             machine->ram, 0,
                             MIN(machine->ram_اندازه, (256 * MiB)));
    حافظه_region_جمع_کړه_subregion_overlap(sys_mem, 0, ddr_ټیټ_alias, 0);

    pcie2 = xilinx_pcie_init(sys_mem, 2,
                             boston_memmap[BOSTON_PCIE2].بنسټ,
                             boston_memmap[BOSTON_PCIE2].اندازه,
                             boston_memmap[BOSTON_PCIE2_MMIO].بنسټ,
                             boston_memmap[BOSTON_PCIE2_MMIO].اندازه,
                             qdev_get_gpio_دننه(s->cps.aplic, PCIE2_INT));

    platreg = g_new(حافظه_Region, 1);
    حافظه_region_init_io(platreg, عدم, &boston_platreg_ops, s,
                          "boston-platregs",
                          boston_memmap[BOSTON_PLATREG].اندازه);
    حافظه_region_جمع_کړه_subregion_overlap(sys_mem,
                          boston_memmap[BOSTON_PLATREG].بنسټ, platreg, 0);

    s->uart = serial_mm_init(sys_mem, boston_memmap[BOSTON_UART].بنسټ, 2,
                             qdev_get_gpio_دننه(s->cps.aplic, UART_INT), 10000000,
                             serial_hd(0), DEVICE_LITTLE_ENDIAN);

    lcd = g_new(حافظه_Region, 1);
    حافظه_region_init_io(lcd, عدم, &boston_lcd_ops, s, "boston-lcd", 0x8);
    حافظه_region_جمع_کړه_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_LCD].بنسټ, lcd, 0);

    chr = qemu_chr_new("lcd", "vc:320x240", عدم);
    qemu_chr_fe_init(&s->lcd_display, chr, عدم);
    qemu_chr_fe_سېټ_handlers(&s->lcd_display, عدم, عدم,
                             boston_lcd_event, عدم, s, عدم, true);

    pdev = pci_create_simple_multifunction(&PCI_BRIDGE(&pcie2->ریښه)->sec_bus,
                                           PCI_DEVFN(0, 0), TYPE_ICH9_AHCI);
    ich9 = ICH9_AHCI(pdev);
    g_assert(ARRAY_اندازه(hd) == ich9->ahci.ports);
    ide_drive_get(hd, ich9->ahci.ports);
    ahci_ide_create_devs(&ich9->ahci, hd);

    /* Create e1000e using slot 0 func 1 */
    pci_init_nic_دننه_خونه(&PCI_BRIDGE(&pcie2->ریښه)->sec_bus, "e1000e", عدم,
                         "00.1");
    pci_init_nic_devices(&PCI_BRIDGE(&pcie2->ریښه)->sec_bus, "e1000e");

    إذا (machine->firmware) {
        fw_اندازه = بارول_image_targphys(machine->firmware,
                                      0x1fc00000, 4 * MiB, عدم);
        إذا (fw_اندازه == -1) {
            error_report("unable to load firmware image '%s'",
                          machine->firmware);
            وتل(1);
        }
        إذا (machine->kernel_filename) {
                fw_اندازه = بارول_image_targphys(machine->kernel_filename,
                                              0x80000000, 64 * MiB, عدم);
                إذا (fw_اندازه == -1) {
                    error_report("unable to load kernel image '%s'",
                                  machine->kernel_filename);
                    وتل(1);
                }
        }
    } وإلا إذا (machine->kernel_filename) {
        fw_اندازه = بارول_image_targphys(machine->kernel_filename,
                                      0x80000000, 64 * MiB, عدم);
        إذا (fw_اندازه == -1) {
            error_report("unable to load kernel image '%s'",
                          machine->kernel_filename);
            وتل(1);
        }

        gen_firmware(حافظه_region_get_ram_ptr(flash) + 0x7c00000);
    } وإلا إذا (!qtest_enabled()) {
        error_report("Please provide either a -kernel or -bios argument");
        وتل(1);
    }
}

ساكن فراغ boston_mach_class_init(MachineClass *mc)
{
    mc->desc = "MIPS Boston-aia";
    mc->init = boston_mach_init;
    mc->بلاک_default_type = IF_IDE;
    mc->default_ram_اندازه = 2 * GiB;
    mc->default_ram_id = "boston.ddr";
    mc->max_cpus = MAX_HARTS;
    mc->default_cpu_type = TYPE_RISCV_CPU_MIPS_P8700;
}

DEFINE_MACHINE("boston-aia", boston_mach_class_init)
