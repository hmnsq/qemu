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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/units.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/char/serial-mm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/ide/pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/ide/ahci-pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/loader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/cps.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci-host/xilinx-pcie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chardev/char.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/address-spaces.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/qtest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/runstate.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "qom/object.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TYPE_MIPS_BOSTON_AIA "mips-boston-aia"
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ BostonState BostonState;
DECLARE_INSTANCE_CHECKER(BostonState, BOSTON,
                         TYPE_MIPS_BOSTON_AIA)

ਗਿਣਤੀ {
    BOSTON_PCIE2,
    BOSTON_PCIE2_MMIO,
    BOSTON_PLATREG,
    BOSTON_UART,
    BOSTON_LCD,
    BOSTON_FLASH,
    BOSTON_HIGHDDR,
};

ਸਥਿਰ ਅਚਲ Mem_Map_ਇੰਦਰਾਜ boston_memmap[] = {
    [BOSTON_PCIE2] =      { 0x14000000,     0x2000000 },
    [BOSTON_PCIE2_MMIO] = { 0x16000000,      0x100000 },
    [BOSTON_PLATREG] =    { 0x17ffd000,        0x1000 },
    [BOSTON_UART] =       { 0x17ffe000,          0x20 },
    [BOSTON_LCD] =        { 0x17fff000,           0x8 },
    [BOSTON_FLASH] =      { 0x18000000,     0x8000000 },
    [BOSTON_HIGHDDR] =    { 0x80000000,           0x0 },
};

/* Interrupt numbers for APLIC. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_INT 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PCIE2_INT 7

ਬਣਤਰ BostonState {
    SysBusDevice ਮਾਪਾ_obj;

    MachineState *mach;
    RISCVCPSState cps;
    SerialMM *uart;

    CharFrontend lcd_display;
    ਅੱਖਰ lcd_content[8];
    bool lcd_inited;
};

ਗਿਣਤੀ boston_plat_reg {
    PLAT_FPGA_BUILD     = 0x00,
    PLAT_CORE_CL        = 0x04,
    PLAT_WRAPPER_CL     = 0x08,
    PLAT_SYSCLK_ਸਥਿਤੀ  = 0x0c,
    PLAT_SOFTRST_CTL    = 0x10,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_SOFTRST_CTL_SYSRESET       (1 << 4)
    PLAT_DDR3_ਸਥਿਤੀ    = 0x14,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_DDR3_ਸਥਿਤੀ_LOCKED         (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_DDR3_ਸਥਿਤੀ_CALIBRATED     (1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_DDR3_INTERFACE_RESET       (1 << 3)
    PLAT_PCIE_ਸਥਿਤੀ    = 0x18,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_PCIE_ਸਥਿਤੀ_PCIE0_LOCKED   (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_PCIE_ਸਥਿਤੀ_PCIE1_LOCKED   (1 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_PCIE_ਸਥਿਤੀ_PCIE2_LOCKED   (1 << 16)
    PLAT_FLASH_CTL      = 0x1c,
    PLAT_SPARE0         = 0x20,
    PLAT_SPARE1         = 0x24,
    PLAT_SPARE2         = 0x28,
    PLAT_SPARE3         = 0x2c,
    PLAT_MMCM_DIV       = 0x30,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_MMCM_DIV_CLK0_DIV_ਖਿਸਕਾਓ     0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_MMCM_DIV_ਨਿਵੇਸ਼_ਖਿਸਕਾਓ       8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_MMCM_DIV_MUL_ਖਿਸਕਾਓ         16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_MMCM_DIV_CLK1_DIV_ਖਿਸਕਾਓ     24
    PLAT_BUILD_CFG      = 0x34,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_BUILD_CFG_IOCU_EN          (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_BUILD_CFG_PCIE0_EN         (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_BUILD_CFG_PCIE1_EN         (1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_BUILD_CFG_PCIE2_EN         (1 << 3)
    PLAT_DDR_CFG        = 0x38,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_DDR_CFG_ਮਾਪ               (0xf << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLAT_DDR_CFG_MHZ                (0xfff << 4)
    PLAT_NOC_PCIE0_ADDR = 0x3c,
    PLAT_NOC_PCIE1_ADDR = 0x40,
    PLAT_NOC_PCIE2_ADDR = 0x44,
    PLAT_SYS_CTL        = 0x48,
};

ਸਥਿਰ ਖਾਲੀ boston_lcd_event(ਖਾਲੀ *opaque, QEMUChrEvent event)
{
    BostonState *s = opaque;
    ਜੇ (event == CHR_EVENT_OPENED && !s->lcd_inited) {
        qemu_chr_fe_printf(&s->lcd_display, "        ");
        s->lcd_inited = true;
    }
}

ਸਥਿਰ uint64_t boston_lcd_read(ਖਾਲੀ *opaque, hwaddr addr,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
    BostonState *s = opaque;
    uint64_t val = 0;

    ਚੋਣ (ਮਾਪ) {
    ਹਾਲਤ 8:
        val |= (uint64_t)s->lcd_content[(addr + 7) & 0x7] << 56;
        val |= (uint64_t)s->lcd_content[(addr + 6) & 0x7] << 48;
        val |= (uint64_t)s->lcd_content[(addr + 5) & 0x7] << 40;
        val |= (uint64_t)s->lcd_content[(addr + 4) & 0x7] << 32;
        /* fall through */
    ਹਾਲਤ 4:
        val |= (uint64_t)s->lcd_content[(addr + 3) & 0x7] << 24;
        val |= (uint64_t)s->lcd_content[(addr + 2) & 0x7] << 16;
        /* fall through */
    ਹਾਲਤ 2:
        val |= (uint64_t)s->lcd_content[(addr + 1) & 0x7] << 8;
        /* fall through */
    ਹਾਲਤ 1:
        val |= (uint64_t)s->lcd_content[(addr + 0) & 0x7];
        ਰੋਕੋ;
    }

    ਵਾਪਸ val;
}

ਸਥਿਰ ਖਾਲੀ boston_lcd_write(ਖਾਲੀ *opaque, hwaddr addr,
                             uint64_t val, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
    BostonState *s = opaque;

    ਚੋਣ (ਮਾਪ) {
    ਹਾਲਤ 8:
        s->lcd_content[(addr + 7) & 0x7] = val >> 56;
        s->lcd_content[(addr + 6) & 0x7] = val >> 48;
        s->lcd_content[(addr + 5) & 0x7] = val >> 40;
        s->lcd_content[(addr + 4) & 0x7] = val >> 32;
        /* fall through */
    ਹਾਲਤ 4:
        s->lcd_content[(addr + 3) & 0x7] = val >> 24;
        s->lcd_content[(addr + 2) & 0x7] = val >> 16;
        /* fall through */
    ਹਾਲਤ 2:
        s->lcd_content[(addr + 1) & 0x7] = val >> 8;
        /* fall through */
    ਹਾਲਤ 1:
        s->lcd_content[(addr + 0) & 0x7] = val;
        ਰੋਕੋ;
    }

    qemu_chr_fe_printf(&s->lcd_display,
                       "\r%-8.8s", s->lcd_content);
}

ਸਥਿਰ ਅਚਲ ਸਮ੍ਰਿਤੀ_Region_Ops boston_lcd_ops = {
    .ਪਠ = boston_lcd_read,
    .ਲਿਖ = boston_lcd_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

ਸਥਿਰ uint64_t boston_platreg_read(ਖਾਲੀ *opaque, hwaddr addr,
                                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
    BostonState *s = opaque;
    uint32_t gic_freq, val;

    ਚੋਣ (addr & 0xffff) {
    ਹਾਲਤ PLAT_FPGA_BUILD:
    ਹਾਲਤ PLAT_CORE_CL:
    ਹਾਲਤ PLAT_WRAPPER_CL:
        ਵਾਪਸ 0;
    ਹਾਲਤ PLAT_DDR3_ਸਥਿਤੀ:
        ਵਾਪਸ PLAT_DDR3_ਸਥਿਤੀ_LOCKED | PLAT_DDR3_ਸਥਿਤੀ_CALIBRATED
               | PLAT_DDR3_INTERFACE_RESET;
    ਹਾਲਤ PLAT_MMCM_DIV:
        gic_freq = 25000000 / 1000000;
        val = gic_freq << PLAT_MMCM_DIV_ਨਿਵੇਸ਼_ਖਿਸਕਾਓ;
        val |= 1 << PLAT_MMCM_DIV_MUL_ਖਿਸਕਾਓ;
        val |= 1 << PLAT_MMCM_DIV_CLK0_DIV_ਖਿਸਕਾਓ;
        val |= 1 << PLAT_MMCM_DIV_CLK1_DIV_ਖਿਸਕਾਓ;
        ਵਾਪਸ val;
    ਹਾਲਤ PLAT_BUILD_CFG:
        val = PLAT_BUILD_CFG_PCIE0_EN;
        val |= PLAT_BUILD_CFG_PCIE1_EN;
        val |= PLAT_BUILD_CFG_PCIE2_EN;
        ਵਾਪਸ val;
    ਹਾਲਤ PLAT_DDR_CFG:
        val = s->mach->ram_ਮਾਪ / GiB;
        ਪ੍ਰਤਿਜ੍ਞਾ(!(val & ~PLAT_DDR_CFG_ਮਾਪ));
        val |= PLAT_DDR_CFG_MHZ;
        ਵਾਪਸ val;
    ਮੂਲ:
        qemu_log_ਨਕਾਬ(LOG_UNIMP, "Read platform register 0x%" HWADDR_PRIx "\n",
                      addr & 0xffff);
        ਵਾਪਸ 0;
    }
}

ਸਥਿਰ ਖਾਲੀ boston_platreg_write(ਖਾਲੀ *opaque, hwaddr addr,
                                 uint64_t val, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
    ਜੇ (ਮਾਪ != 4) {
        qemu_log_ਨਕਾਬ(LOG_UNIMP, "%uB platform register write\n", ਮਾਪ);
        ਵਾਪਸ;
    }

    ਚੋਣ (addr & 0xffff) {
    ਹਾਲਤ PLAT_FPGA_BUILD:
    ਹਾਲਤ PLAT_CORE_CL:
    ਹਾਲਤ PLAT_WRAPPER_CL:
    ਹਾਲਤ PLAT_DDR3_ਸਥਿਤੀ:
    ਹਾਲਤ PLAT_PCIE_ਸਥਿਤੀ:
    ਹਾਲਤ PLAT_MMCM_DIV:
    ਹਾਲਤ PLAT_BUILD_CFG:
    ਹਾਲਤ PLAT_DDR_CFG:
        /* read only */
        ਰੋਕੋ;
    ਹਾਲਤ PLAT_SOFTRST_CTL:
        ਜੇ (val & PLAT_SOFTRST_CTL_SYSRESET) {
            qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        }
        ਰੋਕੋ;
    ਮੂਲ:
        qemu_log_ਨਕਾਬ(LOG_UNIMP, "Write platform register 0x%" HWADDR_PRIx
                      " = 0x%" PRIx64 "\n", addr & 0xffff, val);
        ਰੋਕੋ;
    }
}

ਸਥਿਰ ਅਚਲ ਸਮ੍ਰਿਤੀ_Region_Ops boston_platreg_ops = {
    .ਪਠ = boston_platreg_read,
    .ਲਿਖ = boston_platreg_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_ਮਾਪ = 4,
        .max_access_ਮਾਪ = 4,
    },
};

ਸਥਿਰ ਅਚਲ TypeInfo boston_device = {
    .name          = TYPE_MIPS_BOSTON_AIA,
    .ਮਾਪਾ        = TYPE_SYS_BUS_DEVICE,
    .instance_ਮਾਪ = ਆਕਾਰ(BostonState),
};

ਸਥਿਰ ਖਾਲੀ boston_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&boston_device);
}
type_init(boston_ਰਜਿਸਟਰ_types)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕ_INSNS 6
ਸਥਿਰ ਖਾਲੀ gen_firmware(uint32_t *p)
{
    ਪੂਰਨ i;
    uint32_t reset_vec[ਅੰਕ_INSNS] = {
           /* CM relocate */
           0x1fb802b7,     /* li   t0,0x1fb80000   */
           0x16100337,     /* li   t1,0x16100000   */
           0x0062b423,     /* sd   t1,8(t0)        */
           /* Jump to 0x80000000 */
           0x00100293,     /* li   t0,1            */
           0x01f29293,     /* slli t0,t0,1f        */
           0x00028067      /* jr   t0              */
    };

    ਲਈ (i = 0; i < ਅੰਕ_INSNS; i++) {
        *p++ = reset_vec[i];
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ XilinxPCIEHost *
xilinx_pcie_init(ਸਮ੍ਰਿਤੀ_Region *sys_mem, uint32_t bus_nr,
                 hwaddr cfg_ਅਧਾਰ, uint64_t cfg_ਮਾਪ,
                 hwaddr mmio_ਅਧਾਰ, uint64_t mmio_ਮਾਪ,
                 qemu_irq irq)
{
    DeviceState *dev;
    ਸਮ੍ਰਿਤੀ_Region *cfg, *mmio;

    dev = qdev_new(TYPE_XILINX_PCIE_HOST);

    qdev_prop_ਸੈੱਟ_uint32(dev, "bus_nr", bus_nr);
    qdev_prop_ਸੈੱਟ_uint64(dev, "cfg_base", cfg_ਅਧਾਰ);
    qdev_prop_ਸੈੱਟ_uint64(dev, "cfg_size", cfg_ਮਾਪ);
    qdev_prop_ਸੈੱਟ_uint64(dev, "mmio_base", mmio_ਅਧਾਰ);
    qdev_prop_ਸੈੱਟ_uint64(dev, "mmio_size", mmio_ਮਾਪ);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    cfg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion_overlap(sys_mem, cfg_ਅਧਾਰ, cfg, 0);

    mmio = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion_overlap(sys_mem, 0, mmio, 0);

    qdev_connect_gpio_ਬਾਹਰਲਾ_named(dev, "interrupt_out", 0, irq);

    ਵਾਪਸ XILINX_PCIE_HOST(dev);
}

ਸਥਿਰ ਖਾਲੀ boston_mach_init(MachineState *machine)
{
    DeviceState *dev;
    BostonState *s;
    ਸਮ੍ਰਿਤੀ_Region *flash, *ddr_ਨੀਵਾਂ_alias, *lcd, *platreg;
    ਸਮ੍ਰਿਤੀ_Region *sys_mem = get_system_ਸਮ੍ਰਿਤੀ();
    XilinxPCIEHost *pcie2;
    PCIDevice *pdev;
    AHCIPCIState *ich9;
    DriveInfo *hd[6];
    Chardev *chr;
    ਪੂਰਨ fw_ਮਾਪ;

    ਜੇ ((machine->ram_ਮਾਪ % GiB) ||
        (machine->ram_ਮਾਪ > (4 * GiB))) {
        error_report("Memory size must be 1GB, 2GB, 3GB, or 4GB");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਜੇ (machine->smp.cpus / machine->smp.cores / machine->smp.ਧਾਗੇ > 1) {
        error_report(
            "Invalid -smp x,cores=y,threads=z. The max number of clusters "
            "supported is 1");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    dev = qdev_new(TYPE_MIPS_BOSTON_AIA);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    s = BOSTON(dev);
    s->mach = machine;

    object_initialize_ਬੱਚਾ(OBJECT(machine), "cps", &s->cps, TYPE_RISCV_CPS);
    object_property_ਸੈੱਟ_str(OBJECT(&s->cps), "cpu-type", machine->cpu_type,
                            &error_fatal);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->cps), "num-vp", machine->smp.cpus,
                             &error_fatal);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->cps), "num-hart", machine->smp.ਧਾਗੇ,
                             &error_fatal);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->cps), "num-core", machine->smp.cores,
                             &error_fatal);
    object_property_ਸੈੱਟ_uint(OBJECT(&s->cps), "gcr-base", GCR_ਅਧਾਰ_ADDR,
                             &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->cps), &error_fatal);

    sysbus_mmio_map_overlap(SYS_BUS_DEVICE(&s->cps), 0, 0, 1);

    flash =  g_new(ਸਮ੍ਰਿਤੀ_Region, 1);
    ਸਮ੍ਰਿਤੀ_region_init_rom(flash, ਨਿਰਾ, "boston.flash",
                           boston_memmap[BOSTON_FLASH].ਮਾਪ, &error_fatal);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_FLASH].ਅਧਾਰ,
                                        flash, 0);

    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_HIGHDDR].ਅਧਾਰ,
                                        machine->ram, 0);

    ddr_ਨੀਵਾਂ_alias = g_new(ਸਮ੍ਰਿਤੀ_Region, 1);
    ਸਮ੍ਰਿਤੀ_region_init_alias(ddr_ਨੀਵਾਂ_alias, ਨਿਰਾ, "boston_low.ddr",
                             machine->ram, 0,
                             MIN(machine->ram_ਮਾਪ, (256 * MiB)));
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion_overlap(sys_mem, 0, ddr_ਨੀਵਾਂ_alias, 0);

    pcie2 = xilinx_pcie_init(sys_mem, 2,
                             boston_memmap[BOSTON_PCIE2].ਅਧਾਰ,
                             boston_memmap[BOSTON_PCIE2].ਮਾਪ,
                             boston_memmap[BOSTON_PCIE2_MMIO].ਅਧਾਰ,
                             boston_memmap[BOSTON_PCIE2_MMIO].ਮਾਪ,
                             qdev_get_gpio_ਵਿੱਚ(s->cps.aplic, PCIE2_INT));

    platreg = g_new(ਸਮ੍ਰਿਤੀ_Region, 1);
    ਸਮ੍ਰਿਤੀ_region_init_io(platreg, ਨਿਰਾ, &boston_platreg_ops, s,
                          "boston-platregs",
                          boston_memmap[BOSTON_PLATREG].ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion_overlap(sys_mem,
                          boston_memmap[BOSTON_PLATREG].ਅਧਾਰ, platreg, 0);

    s->uart = serial_mm_init(sys_mem, boston_memmap[BOSTON_UART].ਅਧਾਰ, 2,
                             qdev_get_gpio_ਵਿੱਚ(s->cps.aplic, UART_INT), 10000000,
                             serial_hd(0), DEVICE_LITTLE_ENDIAN);

    lcd = g_new(ਸਮ੍ਰਿਤੀ_Region, 1);
    ਸਮ੍ਰਿਤੀ_region_init_io(lcd, ਨਿਰਾ, &boston_lcd_ops, s, "boston-lcd", 0x8);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion_overlap(sys_mem,
                                        boston_memmap[BOSTON_LCD].ਅਧਾਰ, lcd, 0);

    chr = qemu_chr_new("lcd", "vc:320x240", ਨਿਰਾ);
    qemu_chr_fe_init(&s->lcd_display, chr, ਨਿਰਾ);
    qemu_chr_fe_ਸੈੱਟ_handlers(&s->lcd_display, ਨਿਰਾ, ਨਿਰਾ,
                             boston_lcd_event, ਨਿਰਾ, s, ਨਿਰਾ, true);

    pdev = pci_create_simple_multifunction(&PCI_BRIDGE(&pcie2->ਜੜ੍ਹ)->sec_bus,
                                           PCI_DEVFN(0, 0), TYPE_ICH9_AHCI);
    ich9 = ICH9_AHCI(pdev);
    g_assert(ARRAY_ਮਾਪ(hd) == ich9->ahci.ports);
    ide_drive_get(hd, ich9->ahci.ports);
    ahci_ide_create_devs(&ich9->ahci, hd);

    /* Create e1000e using slot 0 func 1 */
    pci_init_nic_ਵਿੱਚ_ਥਾਂ(&PCI_BRIDGE(&pcie2->ਜੜ੍ਹ)->sec_bus, "e1000e", ਨਿਰਾ,
                         "00.1");
    pci_init_nic_devices(&PCI_BRIDGE(&pcie2->ਜੜ੍ਹ)->sec_bus, "e1000e");

    ਜੇ (machine->firmware) {
        fw_ਮਾਪ = ਲੱਦੋ_image_targphys(machine->firmware,
                                      0x1fc00000, 4 * MiB, ਨਿਰਾ);
        ਜੇ (fw_ਮਾਪ == -1) {
            error_report("unable to load firmware image '%s'",
                          machine->firmware);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
        ਜੇ (machine->kernel_filename) {
                fw_ਮਾਪ = ਲੱਦੋ_image_targphys(machine->kernel_filename,
                                              0x80000000, 64 * MiB, ਨਿਰਾ);
                ਜੇ (fw_ਮਾਪ == -1) {
                    error_report("unable to load kernel image '%s'",
                                  machine->kernel_filename);
                    ਬਾਹਰ_ਨਿਕਲੋ(1);
                }
        }
    } ਨਹੀਂਤਾਂ ਜੇ (machine->kernel_filename) {
        fw_ਮਾਪ = ਲੱਦੋ_image_targphys(machine->kernel_filename,
                                      0x80000000, 64 * MiB, ਨਿਰਾ);
        ਜੇ (fw_ਮਾਪ == -1) {
            error_report("unable to load kernel image '%s'",
                          machine->kernel_filename);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }

        gen_firmware(ਸਮ੍ਰਿਤੀ_region_get_ram_ptr(flash) + 0x7c00000);
    } ਨਹੀਂਤਾਂ ਜੇ (!qtest_enabled()) {
        error_report("Please provide either a -kernel or -bios argument");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
}

ਸਥਿਰ ਖਾਲੀ boston_mach_class_init(MachineClass *mc)
{
    mc->desc = "MIPS Boston-aia";
    mc->init = boston_mach_init;
    mc->ਖੰਡ_default_type = IF_IDE;
    mc->default_ram_ਮਾਪ = 2 * GiB;
    mc->default_ram_id = "boston.ddr";
    mc->max_cpus = MAX_HARTS;
    mc->default_cpu_type = TYPE_RISCV_CPU_MIPS_P8700;
}

DEFINE_MACHINE("boston-aia", boston_mach_class_init)
