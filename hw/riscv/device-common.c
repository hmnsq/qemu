/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#شامل "qemu/osdep.h"

#شامل "hw/riscv/device-common.h"

#شامل "hw/block/flash.h"
#شامل "hw/core/platform-bus.h"
#شامل "hw/core/qdev.h"
#شامل "hw/core/qdev-properties.h"
#شامل "hw/core/sysbus.h"
#شامل "hw/pci/pci.h"
#شامل "hw/pci-host/gpex.h"
#شامل "qapi/error.h"
#شامل "qom/object.h"
#شامل "system/address-spaces.h"
#شامل "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       ثابت Mem_Map_اندراج *platform_bus_mem,
                                       صحیح platform_bus_بنیاد_irq,
                                       صحیح platform_bus_عدد_irqs)
{
    حافظہ_Region *sysmem = get_system_حافظہ();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_سیٹ_uint32(dev, "num_irqs", platform_bus_عدد_irqs);
    qdev_prop_سیٹ_uint32(dev, "mmio_size", platform_bus_mem->سائز);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    برائے (صحیح i = 0; i < platform_bus_عدد_irqs; i++) {
        صحیح irq = platform_bus_بنیاد_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_اندر(irqchip, irq));
    }

    حافظہ_region_جوڑو_subregion(sysmem,
                                platform_bus_mem->بنیاد,
                                sysbus_mmio_get_region(sysbus, 0));

    واپس dev;
}

PFlashCFI01 *riscv_flash_create(Object *والد, ثابت حرف *name,
                                ثابت حرف *alias_prop_name,
                                صحیح flash_sector_سائز)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_سیٹ_uint64(dev, "sector-length", flash_sector_سائز);
    qdev_prop_سیٹ_uint8(dev, "width", 4);
    qdev_prop_سیٹ_uint8(dev, "device-width", 2);
    qdev_prop_سیٹ_ثنائی_ہندسہ(dev, "big-endian", false);
    qdev_prop_سیٹ_uint16(dev, "id0", 0x89);
    qdev_prop_سیٹ_uint16(dev, "id1", 0x18);
    qdev_prop_سیٹ_uint16(dev, "id2", 0x00);
    qdev_prop_سیٹ_uint16(dev, "id3", 0x00);
    qdev_prop_سیٹ_string(dev, "name", name);

    object_property_جوڑو_بچہ(والد, name, OBJECT(dev));
    object_property_جوڑو_alias(والد, alias_prop_name,
                              OBJECT(dev), "drive");

    واپس PFLASH_CFI01(dev);
}

خالی riscv_init_flash_map(PFlashCFI01 *flash, hwaddr بنیاد, hwaddr سائز,
                          حافظہ_Region *sysmem, صحیح flash_sector_سائز)
{
    DeviceState *dev = DEVICE(flash);

    أكد(QEMU_IS_ALIGNED(سائز, flash_sector_سائز));
    أكد(سائز / flash_sector_سائز <= UINT32_MAX);

    qdev_prop_سیٹ_uint32(dev, "num-blocks", سائز / flash_sector_سائز);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    حافظہ_region_جوڑو_subregion(sysmem, بنیاد,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(حافظہ_Region *sys_mem,
                                  DeviceState *irqchip,
                                  ثابت Mem_Map_اندراج *pcie_ecam_mem,
                                  ثابت Mem_Map_اندراج *pcie_mmio_mem,
                                  ثابت Mem_Map_اندراج *بلند_pcie_mmio_mem,
                                  ثابت Mem_Map_اندراج *pcie_pio_mem,
                                  صحیح pcie_irq)
{
    DeviceState *dev;
    حافظہ_Region *ecam_alias, *ecam_reg;
    حافظہ_Region *mmio_alias, *بلند_mmio_alias, *mmio_reg;
    hwaddr ecam_بنیاد = pcie_ecam_mem->بنیاد;
    hwaddr ecam_سائز = pcie_ecam_mem->سائز;
    hwaddr mmio_بنیاد = pcie_mmio_mem->بنیاد;
    hwaddr mmio_سائز = pcie_mmio_mem->سائز;
    hwaddr بلند_mmio_بنیاد = بلند_pcie_mmio_mem->بنیاد;
    hwaddr بلند_mmio_سائز = بلند_pcie_mmio_mem->سائز;
    hwaddr pio_بنیاد = pcie_pio_mem->بنیاد;
    hwaddr pio_سائز = pcie_pio_mem->سائز;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_سیٹ_uint(OBJECT(dev), PCI_HOST_ECAM_بنیاد,
                            ecam_بنیاد, تہی);
    object_property_سیٹ_int(OBJECT(dev), PCI_HOST_ECAM_سائز,
                            ecam_سائز, تہی);
    object_property_سیٹ_uint(OBJECT(dev), PCI_HOST_نیچے_4_G_MMIO_بنیاد,
                             mmio_بنیاد, تہی);
    object_property_سیٹ_int(OBJECT(dev), PCI_HOST_نیچے_4_G_MMIO_سائز,
                            mmio_سائز, تہی);
    object_property_سیٹ_uint(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_بنیاد,
                             بلند_mmio_بنیاد, تہی);
    object_property_سیٹ_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_سائز,
                            بلند_mmio_سائز, تہی);
    object_property_سیٹ_uint(OBJECT(dev), PCI_HOST_PIO_بنیاد,
                            pio_بنیاد, تہی);
    object_property_سیٹ_int(OBJECT(dev), PCI_HOST_PIO_سائز,
                            pio_سائز, تہی);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(حافظہ_Region, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    حافظہ_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_سائز);
    حافظہ_region_جوڑو_subregion(get_system_حافظہ(), ecam_بنیاد, ecam_alias);

    mmio_alias = g_new0(حافظہ_Region, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    حافظہ_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_بنیاد, mmio_سائز);
    حافظہ_region_جوڑو_subregion(get_system_حافظہ(), mmio_بنیاد, mmio_alias);

    /* Map high MMIO space */
    بلند_mmio_alias = g_new0(حافظہ_Region, 1);
    حافظہ_region_init_alias(بلند_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, بلند_mmio_بنیاد, بلند_mmio_سائز);
    حافظہ_region_جوڑو_subregion(get_system_حافظہ(), بلند_mmio_بنیاد,
                                بلند_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_بنیاد);

    برائے (صحیح i = 0; i < PCI_عدد_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_اندر(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_سیٹ_irq_عدد(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    واپس dev;
}
