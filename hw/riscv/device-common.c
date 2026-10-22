/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن "qemu/osdep.h"

#ضمن "hw/riscv/device-common.h"

#ضمن "hw/block/flash.h"
#ضمن "hw/core/platform-bus.h"
#ضمن "hw/core/qdev.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/core/sysbus.h"
#ضمن "hw/pci/pci.h"
#ضمن "hw/pci-host/gpex.h"
#ضمن "qapi/error.h"
#ضمن "qom/object.h"
#ضمن "system/address-spaces.h"
#ضمن "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       ثابت Mem_Map_اندراج *platform_bus_mem,
                                       صحيح platform_bus_ادھار_irq,
                                       صحيح platform_bus_عدد_irqs)
{
    حافظہ_Region *sysmem = get_system_حافظہ();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_سیٹ_uint32(dev, "num_irqs", platform_bus_عدد_irqs);
    qdev_prop_سیٹ_uint32(dev, "mmio_size", platform_bus_mem->ناپ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    لكل (صحيح i = 0; i < platform_bus_عدد_irqs; i++) {
        صحيح irq = platform_bus_ادھار_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_وچ(irqchip, irq));
    }

    حافظہ_region_جوڑو_subregion(sysmem,
                                platform_bus_mem->ادھار,
                                sysbus_mmio_get_region(sysbus, 0));

    أرجع dev;
}

PFlashCFI01 *riscv_flash_create(Object *ماپا, ثابت حرف *name,
                                ثابت حرف *alias_prop_name,
                                صحيح flash_sector_ناپ)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_سیٹ_uint64(dev, "sector-length", flash_sector_ناپ);
    qdev_prop_سیٹ_uint8(dev, "width", 4);
    qdev_prop_سیٹ_uint8(dev, "device-width", 2);
    qdev_prop_سیٹ_ثنائی_ہندسہ(dev, "big-endian", false);
    qdev_prop_سیٹ_uint16(dev, "id0", 0x89);
    qdev_prop_سیٹ_uint16(dev, "id1", 0x18);
    qdev_prop_سیٹ_uint16(dev, "id2", 0x00);
    qdev_prop_سیٹ_uint16(dev, "id3", 0x00);
    qdev_prop_سیٹ_string(dev, "name", name);

    object_property_جوڑو_بچہ(ماپا, name, OBJECT(dev));
    object_property_جوڑو_alias(ماپا, alias_prop_name,
                              OBJECT(dev), "drive");

    أرجع PFLASH_CFI01(dev);
}

فراغ riscv_init_flash_map(PFlashCFI01 *flash, hwaddr ادھار, hwaddr ناپ,
                          حافظہ_Region *sysmem, صحيح flash_sector_ناپ)
{
    DeviceState *dev = DEVICE(flash);

    أكد(QEMU_IS_ALIGNED(ناپ, flash_sector_ناپ));
    أكد(ناپ / flash_sector_ناپ <= UINT32_MAX);

    qdev_prop_سیٹ_uint32(dev, "num-blocks", ناپ / flash_sector_ناپ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    حافظہ_region_جوڑو_subregion(sysmem, ادھار,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(حافظہ_Region *sys_mem,
                                  DeviceState *irqchip,
                                  ثابت Mem_Map_اندراج *pcie_ecam_mem,
                                  ثابت Mem_Map_اندراج *pcie_mmio_mem,
                                  ثابت Mem_Map_اندراج *بلند_pcie_mmio_mem,
                                  ثابت Mem_Map_اندراج *pcie_pio_mem,
                                  صحيح pcie_irq)
{
    DeviceState *dev;
    حافظہ_Region *ecam_alias, *ecam_reg;
    حافظہ_Region *mmio_alias, *بلند_mmio_alias, *mmio_reg;
    hwaddr ecam_ادھار = pcie_ecam_mem->ادھار;
    hwaddr ecam_ناپ = pcie_ecam_mem->ناپ;
    hwaddr mmio_ادھار = pcie_mmio_mem->ادھار;
    hwaddr mmio_ناپ = pcie_mmio_mem->ناپ;
    hwaddr بلند_mmio_ادھار = بلند_pcie_mmio_mem->ادھار;
    hwaddr بلند_mmio_ناپ = بلند_pcie_mmio_mem->ناپ;
    hwaddr pio_ادھار = pcie_pio_mem->ادھار;
    hwaddr pio_ناپ = pcie_pio_mem->ناپ;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_سیٹ_uint(OBJECT(dev), PCI_HOST_ECAM_ادھار,
                            ecam_ادھار, عدم);
    object_property_سیٹ_int(OBJECT(dev), PCI_HOST_ECAM_ناپ,
                            ecam_ناپ, عدم);
    object_property_سیٹ_uint(OBJECT(dev), PCI_HOST_ہیٹھاں_4_G_MMIO_ادھار,
                             mmio_ادھار, عدم);
    object_property_سیٹ_int(OBJECT(dev), PCI_HOST_ہیٹھاں_4_G_MMIO_ناپ,
                            mmio_ناپ, عدم);
    object_property_سیٹ_uint(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_ادھار,
                             بلند_mmio_ادھار, عدم);
    object_property_سیٹ_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_ناپ,
                            بلند_mmio_ناپ, عدم);
    object_property_سیٹ_uint(OBJECT(dev), PCI_HOST_PIO_ادھار,
                            pio_ادھار, عدم);
    object_property_سیٹ_int(OBJECT(dev), PCI_HOST_PIO_ناپ,
                            pio_ناپ, عدم);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(حافظہ_Region, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    حافظہ_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_ناپ);
    حافظہ_region_جوڑو_subregion(get_system_حافظہ(), ecam_ادھار, ecam_alias);

    mmio_alias = g_new0(حافظہ_Region, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    حافظہ_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_ادھار, mmio_ناپ);
    حافظہ_region_جوڑو_subregion(get_system_حافظہ(), mmio_ادھار, mmio_alias);

    /* Map high MMIO space */
    بلند_mmio_alias = g_new0(حافظہ_Region, 1);
    حافظہ_region_init_alias(بلند_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, بلند_mmio_ادھار, بلند_mmio_ناپ);
    حافظہ_region_جوڑو_subregion(get_system_حافظہ(), بلند_mmio_ادھار,
                                بلند_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_ادھار);

    لكل (صحيح i = 0; i < PCI_عدد_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_وچ(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_سیٹ_irq_عدد(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    أرجع dev;
}
