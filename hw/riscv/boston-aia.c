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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"

#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/char/serial-mm.h"
#ಅನ್ತರ್ಭಾವ "hw/ide/pci.h"
#ಅನ್ತರ್ಭಾವ "hw/ide/ahci-pci.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/cps.h"
#ಅನ್ತರ್ಭಾವ "hw/pci-host/xilinx-pcie.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qemu/log.h"
#ಅನ್ತರ್ಭಾವ "chardev/char.h"
#ಅನ್ತರ್ಭಾವ "system/address-spaces.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "system/qtest.h"
#ಅನ್ತರ್ಭಾವ "system/runstate.h"

#ಅನ್ತರ್ಭಾವ <libfdt.h>
#ಅನ್ತರ್ಭಾವ "qom/object.h"

#ವ್ಯಾಖ್ಯಾನಿಸು TYPE_MIPS_BOSTON_AIA "mips-boston-aia"
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ BostonState BostonState;
DECLARE_INSTANCE_CHECKER(BostonState, BOSTON,
                         TYPE_MIPS_BOSTON_AIA)

ಎಣಿಕೆ {
    BOSTON_PCIE2,
    BOSTON_PCIE2_MMIO,
    BOSTON_PLATREG,
    BOSTON_UART,
    BOSTON_LCD,
    BOSTON_FLASH,
    BOSTON_HIGHDDR,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು boston_memmap[] = {
    [BOSTON_PCIE2] =      { 0x14000000,     0x2000000 },
    [BOSTON_PCIE2_MMIO] = { 0x16000000,      0x100000 },
    [BOSTON_PLATREG] =    { 0x17ffd000,        0x1000 },
    [BOSTON_UART] =       { 0x17ffe000,          0x20 },
    [BOSTON_LCD] =        { 0x17fff000,           0x8 },
    [BOSTON_FLASH] =      { 0x18000000,     0x8000000 },
    [BOSTON_HIGHDDR] =    { 0x80000000,           0x0 },
};

/* Interrupt numbers for APLIC. */
#ವ್ಯಾಖ್ಯಾನಿಸು UART_INT 4
#ವ್ಯಾಖ್ಯಾನಿಸು PCIE2_INT 7

ರಚನೆ BostonState {
    SysBusDevice ಪೋಷಕ_obj;

    MachineState *mach;
    RISCVCPSState cps;
    SerialMM *uart;

    CharFrontend lcd_display;
    ಅಕ್ಷರ lcd_content[8];
    bool lcd_inited;
};

ಎಣಿಕೆ boston_plat_reg {
    PLAT_FPGA_BUILD     = 0x00,
    PLAT_CORE_CL        = 0x04,
    PLAT_WRAPPER_CL     = 0x08,
    PLAT_SYSCLK_ಸ್ಥಿತಿ  = 0x0c,
    PLAT_SOFTRST_CTL    = 0x10,
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_SOFTRST_CTL_SYSRESET       (1 << 4)
    PLAT_DDR3_ಸ್ಥಿತಿ    = 0x14,
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_DDR3_ಸ್ಥಿತಿ_LOCKED         (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_DDR3_ಸ್ಥಿತಿ_CALIBRATED     (1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_DDR3_INTERFACE_RESET       (1 << 3)
    PLAT_PCIE_ಸ್ಥಿತಿ    = 0x18,
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_PCIE_ಸ್ಥಿತಿ_PCIE0_LOCKED   (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_PCIE_ಸ್ಥಿತಿ_PCIE1_LOCKED   (1 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_PCIE_ಸ್ಥಿತಿ_PCIE2_LOCKED   (1 << 16)
    PLAT_FLASH_CTL      = 0x1c,
    PLAT_SPARE0         = 0x20,
    PLAT_SPARE1         = 0x24,
    PLAT_SPARE2         = 0x28,
    PLAT_SPARE3         = 0x2c,
    PLAT_MMCM_DIV       = 0x30,
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_MMCM_DIV_CLK0_DIV_ಸರಿಸು     0
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_MMCM_DIV_ಒಳಹರಿವು_ಸರಿಸು       8
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_MMCM_DIV_MUL_ಸರಿಸು         16
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_MMCM_DIV_CLK1_DIV_ಸರಿಸು     24
    PLAT_BUILD_CFG      = 0x34,
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_BUILD_CFG_IOCU_EN          (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_BUILD_CFG_PCIE0_EN         (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_BUILD_CFG_PCIE1_EN         (1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_BUILD_CFG_PCIE2_EN         (1 << 3)
    PLAT_DDR_CFG        = 0x38,
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_DDR_CFG_ಗಾತ್ರ               (0xf << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು PLAT_DDR_CFG_MHZ                (0xfff << 4)
    PLAT_NOC_PCIE0_ADDR = 0x3c,
    PLAT_NOC_PCIE1_ADDR = 0x40,
    PLAT_NOC_PCIE2_ADDR = 0x44,
    PLAT_SYS_CTL        = 0x48,
};

ಸ್ಥಿರ ಶೂನ್ಯ boston_lcd_event(ಶೂನ್ಯ *opaque, QEMUChrEvent event)
{
    BostonState *s = opaque;
    ಆದರೆ (event == CHR_EVENT_OPENED && !s->lcd_inited) {
        qemu_chr_fe_printf(&s->lcd_display, "        ");
        s->lcd_inited = true;
    }
}

ಸ್ಥಿರ uint64_t boston_lcd_read(ಶೂನ್ಯ *opaque, hwaddr addr,
                                ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2)
{
    BostonState *s = opaque;
    uint64_t val = 0;

    ಆಯ್ಕೆ (ಗಾತ್ರ_2) {
    ಸ್ಥಿತಿ 8:
        val |= (uint64_t)s->lcd_content[(addr + 7) & 0x7] << 56;
        val |= (uint64_t)s->lcd_content[(addr + 6) & 0x7] << 48;
        val |= (uint64_t)s->lcd_content[(addr + 5) & 0x7] << 40;
        val |= (uint64_t)s->lcd_content[(addr + 4) & 0x7] << 32;
        /* fall through */
    ಸ್ಥಿತಿ 4:
        val |= (uint64_t)s->lcd_content[(addr + 3) & 0x7] << 24;
        val |= (uint64_t)s->lcd_content[(addr + 2) & 0x7] << 16;
        /* fall through */
    ಸ್ಥಿತಿ 2:
        val |= (uint64_t)s->lcd_content[(addr + 1) & 0x7] << 8;
        /* fall through */
    ಸ್ಥಿತಿ 1:
        val |= (uint64_t)s->lcd_content[(addr + 0) & 0x7];
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ val;
}

ಸ್ಥಿರ ಶೂನ್ಯ boston_lcd_write(ಶೂನ್ಯ *opaque, hwaddr addr,
                             uint64_t val, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2)
{
    BostonState *s = opaque;

    ಆಯ್ಕೆ (ಗಾತ್ರ_2) {
    ಸ್ಥಿತಿ 8:
        s->lcd_content[(addr + 7) & 0x7] = val >> 56;
        s->lcd_content[(addr + 6) & 0x7] = val >> 48;
        s->lcd_content[(addr + 5) & 0x7] = val >> 40;
        s->lcd_content[(addr + 4) & 0x7] = val >> 32;
        /* fall through */
    ಸ್ಥಿತಿ 4:
        s->lcd_content[(addr + 3) & 0x7] = val >> 24;
        s->lcd_content[(addr + 2) & 0x7] = val >> 16;
        /* fall through */
    ಸ್ಥಿತಿ 2:
        s->lcd_content[(addr + 1) & 0x7] = val >> 8;
        /* fall through */
    ಸ್ಥಿತಿ 1:
        s->lcd_content[(addr + 0) & 0x7] = val;
        ನಿಲ್ಲಿಸು;
    }

    qemu_chr_fe_printf(&s->lcd_display,
                       "\r%-8.8s", s->lcd_content);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಸ್ಮರಣೆ_Region_Ops boston_lcd_ops = {
    .ಪಠ = boston_lcd_read,
    .ಲಿಖ = boston_lcd_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

ಸ್ಥಿರ uint64_t boston_platreg_read(ಶೂನ್ಯ *opaque, hwaddr addr,
                                    ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2)
{
    BostonState *s = opaque;
    uint32_t gic_freq, val;

    ಆಯ್ಕೆ (addr & 0xffff) {
    ಸ್ಥಿತಿ PLAT_FPGA_BUILD:
    ಸ್ಥಿತಿ PLAT_CORE_CL:
    ಸ್ಥಿತಿ PLAT_WRAPPER_CL:
        ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ PLAT_DDR3_ಸ್ಥಿತಿ:
        ಹಿಂತಿರುಗಿ PLAT_DDR3_ಸ್ಥಿತಿ_LOCKED | PLAT_DDR3_ಸ್ಥಿತಿ_CALIBRATED
               | PLAT_DDR3_INTERFACE_RESET;
    ಸ್ಥಿತಿ PLAT_MMCM_DIV:
        gic_freq = 25000000 / 1000000;
        val = gic_freq << PLAT_MMCM_DIV_ಒಳಹರಿವು_ಸರಿಸು;
        val |= 1 << PLAT_MMCM_DIV_MUL_ಸರಿಸು;
        val |= 1 << PLAT_MMCM_DIV_CLK0_DIV_ಸರಿಸು;
        val |= 1 << PLAT_MMCM_DIV_CLK1_DIV_ಸರಿಸು;
        ಹಿಂತಿರುಗಿ val;
    ಸ್ಥಿತಿ PLAT_BUILD_CFG:
        val = PLAT_BUILD_CFG_PCIE0_EN;
        val |= PLAT_BUILD_CFG_PCIE1_EN;
        val |= PLAT_BUILD_CFG_PCIE2_EN;
        ಹಿಂತಿರುಗಿ val;
    ಸ್ಥಿತಿ PLAT_DDR_CFG:
        val = s->mach->ram_ಗಾತ್ರ / GiB;
        ಪ್ರತಿಜ್ಞಾ(!(val & ~PLAT_DDR_CFG_ಗಾತ್ರ));
        val |= PLAT_DDR_CFG_MHZ;
        ಹಿಂತಿರುಗಿ val;
    ಪೂರ್ವನಿಯೋಜಿತ:
        qemu_log_ಮುಖವಾಡ(LOG_UNIMP, "Read platform register 0x%" HWADDR_PRIx "\n",
                      addr & 0xffff);
        ಹಿಂತಿರುಗಿ 0;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ boston_platreg_write(ಶೂನ್ಯ *opaque, hwaddr addr,
                                 uint64_t val, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2)
{
    ಆದರೆ (ಗಾತ್ರ_2 != 4) {
        qemu_log_ಮುಖವಾಡ(LOG_UNIMP, "%uB platform register write\n", ಗಾತ್ರ_2);
        ಹಿಂತಿರುಗಿ;
    }

    ಆಯ್ಕೆ (addr & 0xffff) {
    ಸ್ಥಿತಿ PLAT_FPGA_BUILD:
    ಸ್ಥಿತಿ PLAT_CORE_CL:
    ಸ್ಥಿತಿ PLAT_WRAPPER_CL:
    ಸ್ಥಿತಿ PLAT_DDR3_ಸ್ಥಿತಿ:
    ಸ್ಥಿತಿ PLAT_PCIE_ಸ್ಥಿತಿ:
    ಸ್ಥಿತಿ PLAT_MMCM_DIV:
    ಸ್ಥಿತಿ PLAT_BUILD_CFG:
    ಸ್ಥಿತಿ PLAT_DDR_CFG:
        /* read only */
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ PLAT_SOFTRST_CTL:
        ಆದರೆ (val & PLAT_SOFTRST_CTL_SYSRESET) {
            qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        qemu_log_ಮುಖವಾಡ(LOG_UNIMP, "Write platform register 0x%" HWADDR_PRIx
                      " = 0x%" PRIx64 "\n", addr & 0xffff, val);
        ನಿಲ್ಲಿಸು;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಸ್ಮರಣೆ_Region_Ops boston_platreg_ops = {
    .ಪಠ = boston_platreg_read,
    .ಲಿಖ = boston_platreg_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_ಗಾತ್ರ = 4,
        .max_access_ಗಾತ್ರ = 4,
    },
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo boston_device = {
    .name          = TYPE_MIPS_BOSTON_AIA,
    .ಪೋಷಕ        = TYPE_SYS_BUS_DEVICE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(BostonState),
};

ಸ್ಥಿರ ಶೂನ್ಯ boston_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&boston_device);
}
type_init(boston_ರಿಜಿಸ್ಟರ್_types)

#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಕೆ_INSNS 6
ಸ್ಥಿರ ಶೂನ್ಯ gen_firmware(uint32_t *p)
{
    ಪೂರ್ಣಾಂಕ i;
    uint32_t reset_vec[ಅಂಕೆ_INSNS] = {
           /* CM relocate */
           0x1fb802b7,     /* li   t0,0x1fb80000   */
           0x16100337,     /* li   t1,0x16100000   */
           0x0062b423,     /* sd   t1,8(t0)        */
           /* Jump to 0x80000000 */
           0x00100293,     /* li   t0,1            */
           0x01f29293,     /* slli t0,t0,1f        */
           0x00028067      /* jr   t0              */
    };

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_INSNS; i++) {
        *p++ = reset_vec[i];
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ XilinxPCIEHost *
xilinx_pcie_init(ಸ್ಮರಣೆ_Region *sys_mem, uint32_t bus_nr,
                 hwaddr cfg_ತಳ, uint64_t cfg_ಗಾತ್ರ,
                 hwaddr mmio_ತಳ, uint64_t mmio_ಗಾತ್ರ,
                 qemu_irq irq)
{
    DeviceState *dev;
    ಸ್ಮರಣೆ_Region *cfg, *mmio;

    dev = qdev_new(TYPE_XILINX_PCIE_HOST);

    qdev_prop_ಗಣ_uint32(dev, "bus_nr", bus_nr);
    qdev_prop_ಗಣ_uint64(dev, "cfg_base", cfg_ತಳ);
    qdev_prop_ಗಣ_uint64(dev, "cfg_size", cfg_ಗಾತ್ರ);
    qdev_prop_ಗಣ_uint64(dev, "mmio_base", mmio_ತಳ);
    qdev_prop_ಗಣ_uint64(dev, "mmio_size", mmio_ಗಾತ್ರ);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    cfg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion_overlap(sys_mem, cfg_ತಳ, cfg, 0);

    mmio = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion_overlap(sys_mem, 0, mmio, 0);

    qdev_connect_gpio_ಹೊರಹೋಗು_named(dev, "interrupt_out", 0, irq);

    ಹಿಂತಿರುಗಿ XILINX_PCIE_HOST(dev);
}

ಸ್ಥಿರ ಶೂನ್ಯ boston_mach_init(MachineState *machine)
{
    DeviceState *dev;
    BostonState *s;
    ಸ್ಮರಣೆ_Region *flash, *ddr_ಕೆಳಗಿನ_alias, *lcd, *platreg;
    ಸ್ಮರಣೆ_Region *sys_mem = get_system_ಸ್ಮರಣೆ();
    XilinxPCIEHost *pcie2;
    PCIDevice *pdev;
    AHCIPCIState *ich9;
    DriveInfo *hd[6];
    Chardev *chr;
    ಪೂರ್ಣಾಂಕ fw_ಗಾತ್ರ;

    ಆದರೆ ((machine->ram_ಗಾತ್ರ % GiB) ||
        (machine->ram_ಗಾತ್ರ > (4 * GiB))) {
        error_report("Memory size must be 1GB, 2GB, 3GB, or 4GB");
        ನಿರ್ಗಮಿಸು(1);
    }

    ಆದರೆ (machine->smp.cpus / machine->smp.cores / machine->smp.ಎಳೆಗಳು > 1) {
        error_report(
            "Invalid -smp x,cores=y,threads=z. The max number of clusters "
            "supported is 1");
        ನಿರ್ಗಮಿಸು(1);
    }

    dev = qdev_new(TYPE_MIPS_BOSTON_AIA);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    s = BOSTON(dev);
    s->mach = machine;

    object_initialize_ಮಗು(OBJECT(machine), "cps", &s->cps, TYPE_RISCV_CPS);
    object_property_ಗಣ_str(OBJECT(&s->cps), "cpu-type", machine->cpu_type,
                            &error_fatal);
    object_property_ಗಣ_uint(OBJECT(&s->cps), "num-vp", machine->smp.cpus,
                             &error_fatal);
    object_property_ಗಣ_uint(OBJECT(&s->cps), "num-hart", machine->smp.ಎಳೆಗಳು,
                             &error_fatal);
    object_property_ಗಣ_uint(OBJECT(&s->cps), "num-core", machine->smp.cores,
                             &error_fatal);
    object_property_ಗಣ_uint(OBJECT(&s->cps), "gcr-base", GCR_ತಳ_ADDR,
                             &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->cps), &error_fatal);

    sysbus_mmio_map_overlap(SYS_BUS_DEVICE(&s->cps), 0, 0, 1);

    flash =  g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_region_init_rom(flash, ಖಾಲಿ, "boston.flash",
                           boston_memmap[BOSTON_FLASH].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_FLASH].ತಳ,
                                        flash, 0);

    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_HIGHDDR].ತಳ,
                                        machine->ram, 0);

    ddr_ಕೆಳಗಿನ_alias = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_region_init_alias(ddr_ಕೆಳಗಿನ_alias, ಖಾಲಿ, "boston_low.ddr",
                             machine->ram, 0,
                             MIN(machine->ram_ಗಾತ್ರ, (256 * MiB)));
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion_overlap(sys_mem, 0, ddr_ಕೆಳಗಿನ_alias, 0);

    pcie2 = xilinx_pcie_init(sys_mem, 2,
                             boston_memmap[BOSTON_PCIE2].ತಳ,
                             boston_memmap[BOSTON_PCIE2].ಗಾತ್ರ_2,
                             boston_memmap[BOSTON_PCIE2_MMIO].ತಳ,
                             boston_memmap[BOSTON_PCIE2_MMIO].ಗಾತ್ರ_2,
                             qdev_get_gpio_ಒಳ(s->cps.aplic, PCIE2_INT));

    platreg = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_region_init_io(platreg, ಖಾಲಿ, &boston_platreg_ops, s,
                          "boston-platregs",
                          boston_memmap[BOSTON_PLATREG].ಗಾತ್ರ_2);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion_overlap(sys_mem,
                          boston_memmap[BOSTON_PLATREG].ತಳ, platreg, 0);

    s->uart = serial_mm_init(sys_mem, boston_memmap[BOSTON_UART].ತಳ, 2,
                             qdev_get_gpio_ಒಳ(s->cps.aplic, UART_INT), 10000000,
                             serial_hd(0), DEVICE_LITTLE_ENDIAN);

    lcd = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_region_init_io(lcd, ಖಾಲಿ, &boston_lcd_ops, s, "boston-lcd", 0x8);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_LCD].ತಳ, lcd, 0);

    chr = qemu_chr_new("lcd", "vc:320x240", ಖಾಲಿ);
    qemu_chr_fe_init(&s->lcd_display, chr, ಖಾಲಿ);
    qemu_chr_fe_ಗಣ_handlers(&s->lcd_display, ಖಾಲಿ, ಖಾಲಿ,
                             boston_lcd_event, ಖಾಲಿ, s, ಖಾಲಿ, true);

    pdev = pci_create_simple_multifunction(&PCI_BRIDGE(&pcie2->ಬೇರು)->sec_bus,
                                           PCI_DEVFN(0, 0), TYPE_ICH9_AHCI);
    ich9 = ICH9_AHCI(pdev);
    g_assert(ARRAY_ಗಾತ್ರ(hd) == ich9->ahci.ports);
    ide_drive_get(hd, ich9->ahci.ports);
    ahci_ide_create_devs(&ich9->ahci, hd);

    /* Create e1000e using slot 0 func 1 */
    pci_init_nic_ಒಳ_ಸ್ಥಾನ(&PCI_BRIDGE(&pcie2->ಬೇರು)->sec_bus, "e1000e", ಖಾಲಿ,
                         "00.1");
    pci_init_nic_devices(&PCI_BRIDGE(&pcie2->ಬೇರು)->sec_bus, "e1000e");

    ಆದರೆ (machine->firmware) {
        fw_ಗಾತ್ರ = ತುಂಬು_image_targphys(machine->firmware,
                                      0x1fc00000, 4 * MiB, ಖಾಲಿ);
        ಆದರೆ (fw_ಗಾತ್ರ == -1) {
            error_report("unable to load firmware image '%s'",
                          machine->firmware);
            ನಿರ್ಗಮಿಸು(1);
        }
        ಆದರೆ (machine->kernel_filename) {
                fw_ಗಾತ್ರ = ತುಂಬು_image_targphys(machine->kernel_filename,
                                              0x80000000, 64 * MiB, ಖಾಲಿ);
                ಆದರೆ (fw_ಗಾತ್ರ == -1) {
                    error_report("unable to load kernel image '%s'",
                                  machine->kernel_filename);
                    ನಿರ್ಗಮಿಸು(1);
                }
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (machine->kernel_filename) {
        fw_ಗಾತ್ರ = ತುಂಬು_image_targphys(machine->kernel_filename,
                                      0x80000000, 64 * MiB, ಖಾಲಿ);
        ಆದರೆ (fw_ಗಾತ್ರ == -1) {
            error_report("unable to load kernel image '%s'",
                          machine->kernel_filename);
            ನಿರ್ಗಮಿಸು(1);
        }

        gen_firmware(ಸ್ಮರಣೆ_region_get_ram_ptr(flash) + 0x7c00000);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!qtest_enabled()) {
        error_report("Please provide either a -kernel or -bios argument");
        ನಿರ್ಗಮಿಸು(1);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ boston_mach_class_init(MachineClass *mc)
{
    mc->desc = "MIPS Boston-aia";
    mc->init = boston_mach_init;
    mc->ಖಂಡ_default_type = IF_IDE;
    mc->default_ram_ಗಾತ್ರ = 2 * GiB;
    mc->default_ram_id = "boston.ddr";
    mc->max_cpus = MAX_HARTS;
    mc->default_cpu_type = TYPE_RISCV_CPU_MIPS_P8700;
}

DEFINE_MACHINE("boston-aia", boston_mach_class_init)
