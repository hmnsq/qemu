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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qemu/units.h"

#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/char/serial-mm.h"
#अन्तर्भाव "hw/ide/pci.h"
#अन्तर्भाव "hw/ide/ahci-pci.h"
#अन्तर्भाव "hw/core/loader.h"
#अन्तर्भाव "hw/riscv/cps.h"
#अन्तर्भाव "hw/pci-host/xilinx-pcie.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "qemu/log.h"
#अन्तर्भाव "chardev/char.h"
#अन्तर्भाव "system/address-spaces.h"
#अन्तर्भाव "system/device_tree.h"
#अन्तर्भाव "system/system.h"
#अन्तर्भाव "system/qtest.h"
#अन्तर्भाव "system/runstate.h"

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव "qom/object.h"

#परिभाषय TYPE_MIPS_BOSTON_AIA "mips-boston-aia"
प्रकारनिर्देश संरचना BostonState BostonState;
DECLARE_INSTANCE_CHECKER(BostonState, BOSTON,
                         TYPE_MIPS_BOSTON_AIA)

गणना {
    BOSTON_PCIE2,
    BOSTON_PCIE2_MMIO,
    BOSTON_PLATREG,
    BOSTON_UART,
    BOSTON_LCD,
    BOSTON_FLASH,
    BOSTON_HIGHDDR,
};

स्थिर ध्रुव Mem_Map_प्रविष्टिः boston_memmap[] = {
    [BOSTON_PCIE2] =      { 0x14000000,     0x2000000 },
    [BOSTON_PCIE2_MMIO] = { 0x16000000,      0x100000 },
    [BOSTON_PLATREG] =    { 0x17ffd000,        0x1000 },
    [BOSTON_UART] =       { 0x17ffe000,          0x20 },
    [BOSTON_LCD] =        { 0x17fff000,           0x8 },
    [BOSTON_FLASH] =      { 0x18000000,     0x8000000 },
    [BOSTON_HIGHDDR] =    { 0x80000000,           0x0 },
};

/* Interrupt numbers for APLIC. */
#परिभाषय UART_INT 4
#परिभाषय PCIE2_INT 7

संरचना BostonState {
    SysBusDevice parent_obj;

    MachineState *mach;
    RISCVCPSState cps;
    SerialMM *uart;

    CharFrontend lcd_display;
    वर्ण lcd_content[8];
    bool lcd_inited;
};

गणना boston_plat_reg {
    PLAT_FPGA_BUILD     = 0x00,
    PLAT_CORE_CL        = 0x04,
    PLAT_WRAPPER_CL     = 0x08,
    PLAT_SYSCLK_स्थितिः  = 0x0c,
    PLAT_SOFTRST_CTL    = 0x10,
#परिभाषय PLAT_SOFTRST_CTL_SYSRESET       (1 << 4)
    PLAT_DDR3_स्थितिः    = 0x14,
#परिभाषय PLAT_DDR3_स्थितिः_LOCKED         (1 << 0)
#परिभाषय PLAT_DDR3_स्थितिः_CALIBRATED     (1 << 2)
#परिभाषय PLAT_DDR3_INTERFACE_RESET       (1 << 3)
    PLAT_PCIE_स्थितिः    = 0x18,
#परिभाषय PLAT_PCIE_स्थितिः_PCIE0_LOCKED   (1 << 0)
#परिभाषय PLAT_PCIE_स्थितिः_PCIE1_LOCKED   (1 << 8)
#परिभाषय PLAT_PCIE_स्थितिः_PCIE2_LOCKED   (1 << 16)
    PLAT_FLASH_CTL      = 0x1c,
    PLAT_SPARE0         = 0x20,
    PLAT_SPARE1         = 0x24,
    PLAT_SPARE2         = 0x28,
    PLAT_SPARE3         = 0x2c,
    PLAT_MMCM_DIV       = 0x30,
#परिभाषय PLAT_MMCM_DIV_CLK0_DIV_सरणम्     0
#परिभाषय PLAT_MMCM_DIV_निवेशः_सरणम्       8
#परिभाषय PLAT_MMCM_DIV_MUL_सरणम्         16
#परिभाषय PLAT_MMCM_DIV_CLK1_DIV_सरणम्     24
    PLAT_BUILD_CFG      = 0x34,
#परिभाषय PLAT_BUILD_CFG_IOCU_EN          (1 << 0)
#परिभाषय PLAT_BUILD_CFG_PCIE0_EN         (1 << 1)
#परिभाषय PLAT_BUILD_CFG_PCIE1_EN         (1 << 2)
#परिभाषय PLAT_BUILD_CFG_PCIE2_EN         (1 << 3)
    PLAT_DDR_CFG        = 0x38,
#परिभाषय PLAT_DDR_CFG_आकार               (0xf << 0)
#परिभाषय PLAT_DDR_CFG_MHZ                (0xfff << 4)
    PLAT_NOC_PCIE0_ADDR = 0x3c,
    PLAT_NOC_PCIE1_ADDR = 0x40,
    PLAT_NOC_PCIE2_ADDR = 0x44,
    PLAT_SYS_CTL        = 0x48,
};

स्थिर शून्य boston_lcd_event(शून्य *opaque, QEMUChrEvent event)
{
    BostonState *s = opaque;
    यदि (event == CHR_EVENT_OPENED && !s->lcd_inited) {
        qemu_chr_fe_printf(&s->lcd_display, "        ");
        s->lcd_inited = true;
    }
}

स्थिर uint64_t boston_lcd_read(शून्य *opaque, hwaddr addr,
                                अचिह्नित आकार)
{
    BostonState *s = opaque;
    uint64_t val = 0;

    विकल्प (आकार) {
    पक्ष 8:
        val |= (uint64_t)s->lcd_content[(addr + 7) & 0x7] << 56;
        val |= (uint64_t)s->lcd_content[(addr + 6) & 0x7] << 48;
        val |= (uint64_t)s->lcd_content[(addr + 5) & 0x7] << 40;
        val |= (uint64_t)s->lcd_content[(addr + 4) & 0x7] << 32;
        /* fall through */
    पक्ष 4:
        val |= (uint64_t)s->lcd_content[(addr + 3) & 0x7] << 24;
        val |= (uint64_t)s->lcd_content[(addr + 2) & 0x7] << 16;
        /* fall through */
    पक्ष 2:
        val |= (uint64_t)s->lcd_content[(addr + 1) & 0x7] << 8;
        /* fall through */
    पक्ष 1:
        val |= (uint64_t)s->lcd_content[(addr + 0) & 0x7];
        भङ्ग;
    }

    प्रत्यागम val;
}

स्थिर शून्य boston_lcd_write(शून्य *opaque, hwaddr addr,
                             uint64_t val, अचिह्नित आकार)
{
    BostonState *s = opaque;

    विकल्प (आकार) {
    पक्ष 8:
        s->lcd_content[(addr + 7) & 0x7] = val >> 56;
        s->lcd_content[(addr + 6) & 0x7] = val >> 48;
        s->lcd_content[(addr + 5) & 0x7] = val >> 40;
        s->lcd_content[(addr + 4) & 0x7] = val >> 32;
        /* fall through */
    पक्ष 4:
        s->lcd_content[(addr + 3) & 0x7] = val >> 24;
        s->lcd_content[(addr + 2) & 0x7] = val >> 16;
        /* fall through */
    पक्ष 2:
        s->lcd_content[(addr + 1) & 0x7] = val >> 8;
        /* fall through */
    पक्ष 1:
        s->lcd_content[(addr + 0) & 0x7] = val;
        भङ्ग;
    }

    qemu_chr_fe_printf(&s->lcd_display,
                       "\r%-8.8s", s->lcd_content);
}

स्थिर ध्रुव स्मृतिः_Region_Ops boston_lcd_ops = {
    .पठ = boston_lcd_read,
    .लिख = boston_lcd_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

स्थिर uint64_t boston_platreg_read(शून्य *opaque, hwaddr addr,
                                    अचिह्नित आकार)
{
    BostonState *s = opaque;
    uint32_t gic_freq, val;

    विकल्प (addr & 0xffff) {
    पक्ष PLAT_FPGA_BUILD:
    पक्ष PLAT_CORE_CL:
    पक्ष PLAT_WRAPPER_CL:
        प्रत्यागम 0;
    पक्ष PLAT_DDR3_स्थितिः:
        प्रत्यागम PLAT_DDR3_स्थितिः_LOCKED | PLAT_DDR3_स्थितिः_CALIBRATED
               | PLAT_DDR3_INTERFACE_RESET;
    पक्ष PLAT_MMCM_DIV:
        gic_freq = 25000000 / 1000000;
        val = gic_freq << PLAT_MMCM_DIV_निवेशः_सरणम्;
        val |= 1 << PLAT_MMCM_DIV_MUL_सरणम्;
        val |= 1 << PLAT_MMCM_DIV_CLK0_DIV_सरणम्;
        val |= 1 << PLAT_MMCM_DIV_CLK1_DIV_सरणम्;
        प्रत्यागम val;
    पक्ष PLAT_BUILD_CFG:
        val = PLAT_BUILD_CFG_PCIE0_EN;
        val |= PLAT_BUILD_CFG_PCIE1_EN;
        val |= PLAT_BUILD_CFG_PCIE2_EN;
        प्रत्यागम val;
    पक्ष PLAT_DDR_CFG:
        val = s->mach->ram_आकार / GiB;
        प्रतिज्ञा(!(val & ~PLAT_DDR_CFG_आकार));
        val |= PLAT_DDR_CFG_MHZ;
        प्रत्यागम val;
    स्वभाव:
        qemu_log_आवरणम्(LOG_UNIMP, "Read platform register 0x%" HWADDR_PRIx "\n",
                      addr & 0xffff);
        प्रत्यागम 0;
    }
}

स्थिर शून्य boston_platreg_write(शून्य *opaque, hwaddr addr,
                                 uint64_t val, अचिह्नित आकार)
{
    यदि (आकार != 4) {
        qemu_log_आवरणम्(LOG_UNIMP, "%uB platform register write\n", आकार);
        प्रत्यागम;
    }

    विकल्प (addr & 0xffff) {
    पक्ष PLAT_FPGA_BUILD:
    पक्ष PLAT_CORE_CL:
    पक्ष PLAT_WRAPPER_CL:
    पक्ष PLAT_DDR3_स्थितिः:
    पक्ष PLAT_PCIE_स्थितिः:
    पक्ष PLAT_MMCM_DIV:
    पक्ष PLAT_BUILD_CFG:
    पक्ष PLAT_DDR_CFG:
        /* read only */
        भङ्ग;
    पक्ष PLAT_SOFTRST_CTL:
        यदि (val & PLAT_SOFTRST_CTL_SYSRESET) {
            qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        }
        भङ्ग;
    स्वभाव:
        qemu_log_आवरणम्(LOG_UNIMP, "Write platform register 0x%" HWADDR_PRIx
                      " = 0x%" PRIx64 "\n", addr & 0xffff, val);
        भङ्ग;
    }
}

स्थिर ध्रुव स्मृतिः_Region_Ops boston_platreg_ops = {
    .पठ = boston_platreg_read,
    .लिख = boston_platreg_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_आकार = 4,
        .max_access_आकार = 4,
    },
};

स्थिर ध्रुव TypeInfo boston_device = {
    .name          = TYPE_MIPS_BOSTON_AIA,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_आकार = परिमाण(BostonState),
};

स्थिर शून्य boston_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&boston_device);
}
type_init(boston_पञ्जिकाकोशः_types)

#परिभाषय अङ्कः_INSNS 6
स्थिर शून्य gen_firmware(uint32_t *p)
{
    पूर्णाङ्क i;
    uint32_t reset_vec[अङ्कः_INSNS] = {
           /* CM relocate */
           0x1fb802b7,     /* li   t0,0x1fb80000   */
           0x16100337,     /* li   t1,0x16100000   */
           0x0062b423,     /* sd   t1,8(t0)        */
           /* Jump to 0x80000000 */
           0x00100293,     /* li   t0,1            */
           0x01f29293,     /* slli t0,t0,1f        */
           0x00028067      /* jr   t0              */
    };

    प्रति (i = 0; i < अङ्कः_INSNS; i++) {
        *p++ = reset_vec[i];
    }
}

स्थिर अन्तःस्थ XilinxPCIEHost *
xilinx_pcie_init(स्मृतिः_Region *sys_mem, uint32_t bus_nr,
                 hwaddr cfg_आधारः, uint64_t cfg_आकार,
                 hwaddr mmio_आधारः, uint64_t mmio_आकार,
                 qemu_irq irq)
{
    DeviceState *dev;
    स्मृतिः_Region *cfg, *mmio;

    dev = qdev_new(TYPE_XILINX_PCIE_HOST);

    qdev_prop_समुच्चयः_uint32(dev, "bus_nr", bus_nr);
    qdev_prop_समुच्चयः_uint64(dev, "cfg_base", cfg_आधारः);
    qdev_prop_समुच्चयः_uint64(dev, "cfg_size", cfg_आकार);
    qdev_prop_समुच्चयः_uint64(dev, "mmio_base", mmio_आधारः);
    qdev_prop_समुच्चयः_uint64(dev, "mmio_size", mmio_आकार);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    cfg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    स्मृतिः_region_सङ्कलय_subregion_overlap(sys_mem, cfg_आधारः, cfg, 0);

    mmio = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    स्मृतिः_region_सङ्कलय_subregion_overlap(sys_mem, 0, mmio, 0);

    qdev_connect_gpio_बहिर्गमनम्_named(dev, "interrupt_out", 0, irq);

    प्रत्यागम XILINX_PCIE_HOST(dev);
}

स्थिर शून्य boston_mach_init(MachineState *machine)
{
    DeviceState *dev;
    BostonState *s;
    स्मृतिः_Region *flash, *ddr_नीचः_alias, *lcd, *platreg;
    स्मृतिः_Region *sys_mem = get_system_स्मृतिः();
    XilinxPCIEHost *pcie2;
    PCIDevice *pdev;
    AHCIPCIState *ich9;
    DriveInfo *hd[6];
    Chardev *chr;
    पूर्णाङ्क fw_आकार;

    यदि ((machine->ram_आकार % GiB) ||
        (machine->ram_आकार > (4 * GiB))) {
        error_report("Memory size must be 1GB, 2GB, 3GB, or 4GB");
        निर्गच्छ(1);
    }

    यदि (machine->smp.cpus / machine->smp.cores / machine->smp.तन्तवः > 1) {
        error_report(
            "Invalid -smp x,cores=y,threads=z. The max number of clusters "
            "supported is 1");
        निर्गच्छ(1);
    }

    dev = qdev_new(TYPE_MIPS_BOSTON_AIA);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    s = BOSTON(dev);
    s->mach = machine;

    object_initialize_child(OBJECT(machine), "cps", &s->cps, TYPE_RISCV_CPS);
    object_property_समुच्चयः_str(OBJECT(&s->cps), "cpu-type", machine->cpu_type,
                            &error_fatal);
    object_property_समुच्चयः_uint(OBJECT(&s->cps), "num-vp", machine->smp.cpus,
                             &error_fatal);
    object_property_समुच्चयः_uint(OBJECT(&s->cps), "num-hart", machine->smp.तन्तवः,
                             &error_fatal);
    object_property_समुच्चयः_uint(OBJECT(&s->cps), "num-core", machine->smp.cores,
                             &error_fatal);
    object_property_समुच्चयः_uint(OBJECT(&s->cps), "gcr-base", GCR_आधारः_ADDR,
                             &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->cps), &error_fatal);

    sysbus_mmio_map_overlap(SYS_BUS_DEVICE(&s->cps), 0, 0, 1);

    flash =  g_new(स्मृतिः_Region, 1);
    स्मृतिः_region_init_rom(flash, रिक्त, "boston.flash",
                           boston_memmap[BOSTON_FLASH].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_FLASH].आधारः,
                                        flash, 0);

    स्मृतिः_region_सङ्कलय_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_HIGHDDR].आधारः,
                                        machine->ram, 0);

    ddr_नीचः_alias = g_new(स्मृतिः_Region, 1);
    स्मृतिः_region_init_alias(ddr_नीचः_alias, रिक्त, "boston_low.ddr",
                             machine->ram, 0,
                             MIN(machine->ram_आकार, (256 * MiB)));
    स्मृतिः_region_सङ्कलय_subregion_overlap(sys_mem, 0, ddr_नीचः_alias, 0);

    pcie2 = xilinx_pcie_init(sys_mem, 2,
                             boston_memmap[BOSTON_PCIE2].आधारः,
                             boston_memmap[BOSTON_PCIE2].आकार,
                             boston_memmap[BOSTON_PCIE2_MMIO].आधारः,
                             boston_memmap[BOSTON_PCIE2_MMIO].आकार,
                             qdev_get_gpio_अन्तर्(s->cps.aplic, PCIE2_INT));

    platreg = g_new(स्मृतिः_Region, 1);
    स्मृतिः_region_init_io(platreg, रिक्त, &boston_platreg_ops, s,
                          "boston-platregs",
                          boston_memmap[BOSTON_PLATREG].आकार);
    स्मृतिः_region_सङ्कलय_subregion_overlap(sys_mem,
                          boston_memmap[BOSTON_PLATREG].आधारः, platreg, 0);

    s->uart = serial_mm_init(sys_mem, boston_memmap[BOSTON_UART].आधारः, 2,
                             qdev_get_gpio_अन्तर्(s->cps.aplic, UART_INT), 10000000,
                             serial_hd(0), DEVICE_LITTLE_ENDIAN);

    lcd = g_new(स्मृतिः_Region, 1);
    स्मृतिः_region_init_io(lcd, रिक्त, &boston_lcd_ops, s, "boston-lcd", 0x8);
    स्मृतिः_region_सङ्कलय_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_LCD].आधारः, lcd, 0);

    chr = qemu_chr_new("lcd", "vc:320x240", रिक्त);
    qemu_chr_fe_init(&s->lcd_display, chr, रिक्त);
    qemu_chr_fe_समुच्चयः_handlers(&s->lcd_display, रिक्त, रिक्त,
                             boston_lcd_event, रिक्त, s, रिक्त, true);

    pdev = pci_create_simple_multifunction(&PCI_BRIDGE(&pcie2->मूलम्)->sec_bus,
                                           PCI_DEVFN(0, 0), TYPE_ICH9_AHCI);
    ich9 = ICH9_AHCI(pdev);
    g_assert(ARRAY_आकार(hd) == ich9->ahci.ports);
    ide_drive_get(hd, ich9->ahci.ports);
    ahci_ide_create_devs(&ich9->ahci, hd);

    /* Create e1000e using slot 0 func 1 */
    pci_init_nic_अन्तर्_slot(&PCI_BRIDGE(&pcie2->मूलम्)->sec_bus, "e1000e", रिक्त,
                         "00.1");
    pci_init_nic_devices(&PCI_BRIDGE(&pcie2->मूलम्)->sec_bus, "e1000e");

    यदि (machine->firmware) {
        fw_आकार = आरोपय_image_targphys(machine->firmware,
                                      0x1fc00000, 4 * MiB, रिक्त);
        यदि (fw_आकार == -1) {
            error_report("unable to load firmware image '%s'",
                          machine->firmware);
            निर्गच्छ(1);
        }
        यदि (machine->kernel_filename) {
                fw_आकार = आरोपय_image_targphys(machine->kernel_filename,
                                              0x80000000, 64 * MiB, रिक्त);
                यदि (fw_आकार == -1) {
                    error_report("unable to load kernel image '%s'",
                                  machine->kernel_filename);
                    निर्गच्छ(1);
                }
        }
    } अन्यथा यदि (machine->kernel_filename) {
        fw_आकार = आरोपय_image_targphys(machine->kernel_filename,
                                      0x80000000, 64 * MiB, रिक्त);
        यदि (fw_आकार == -1) {
            error_report("unable to load kernel image '%s'",
                          machine->kernel_filename);
            निर्गच्छ(1);
        }

        gen_firmware(स्मृतिः_region_get_ram_ptr(flash) + 0x7c00000);
    } अन्यथा यदि (!qtest_enabled()) {
        error_report("Please provide either a -kernel or -bios argument");
        निर्गच्छ(1);
    }
}

स्थिर शून्य boston_mach_class_init(MachineClass *mc)
{
    mc->desc = "MIPS Boston-aia";
    mc->init = boston_mach_init;
    mc->खण्डम्_default_type = IF_IDE;
    mc->default_ram_आकार = 2 * GiB;
    mc->default_ram_id = "boston.ddr";
    mc->max_cpus = MAX_HARTS;
    mc->default_cpu_type = TYPE_RISCV_CPU_MIPS_P8700;
}

DEFINE_MACHINE("boston-aia", boston_mach_class_init)
