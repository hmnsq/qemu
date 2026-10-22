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
                                       ثابت Mem_Map_داخلا *platform_bus_mem,
                                       صحيح platform_bus_بنياد_irq,
                                       صحيح platform_bus_انگ_irqs)
{
    يادگيري_Region *sysmem = get_system_يادگيري();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_سيٽ_uint32(dev, "num_irqs", platform_bus_انگ_irqs);
    qdev_prop_سيٽ_uint32(dev, "mmio_size", platform_bus_mem->سائيز);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    لكل (صحيح i = 0; i < platform_bus_انگ_irqs; i++) {
        صحيح irq = platform_bus_بنياد_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_اندريون(irqchip, irq));
    }

    يادگيري_region_جوڙيو_subregion(sysmem,
                                platform_bus_mem->بنياد,
                                sysbus_mmio_get_region(sysbus, 0));

    أرجع dev;
}

PFlashCFI01 *riscv_flash_create(Object *والد, ثابت حرف *name,
                                ثابت حرف *alias_prop_name,
                                صحيح flash_sector_سائيز)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_سيٽ_uint64(dev, "sector-length", flash_sector_سائيز);
    qdev_prop_سيٽ_uint8(dev, "width", 4);
    qdev_prop_سيٽ_uint8(dev, "device-width", 2);
    qdev_prop_سيٽ_ٻٽو_انگ(dev, "big-endian", false);
    qdev_prop_سيٽ_uint16(dev, "id0", 0x89);
    qdev_prop_سيٽ_uint16(dev, "id1", 0x18);
    qdev_prop_سيٽ_uint16(dev, "id2", 0x00);
    qdev_prop_سيٽ_uint16(dev, "id3", 0x00);
    qdev_prop_سيٽ_string(dev, "name", name);

    object_property_جوڙيو_ٻار(والد, name, OBJECT(dev));
    object_property_جوڙيو_alias(والد, alias_prop_name,
                              OBJECT(dev), "drive");

    أرجع PFLASH_CFI01(dev);
}

فراغ riscv_init_flash_map(PFlashCFI01 *flash, hwaddr بنياد, hwaddr سائيز,
                          يادگيري_Region *sysmem, صحيح flash_sector_سائيز)
{
    DeviceState *dev = DEVICE(flash);

    أكد(QEMU_IS_ALIGNED(سائيز, flash_sector_سائيز));
    أكد(سائيز / flash_sector_سائيز <= UINT32_MAX);

    qdev_prop_سيٽ_uint32(dev, "num-blocks", سائيز / flash_sector_سائيز);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    يادگيري_region_جوڙيو_subregion(sysmem, بنياد,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(يادگيري_Region *sys_mem,
                                  DeviceState *irqchip,
                                  ثابت Mem_Map_داخلا *pcie_ecam_mem,
                                  ثابت Mem_Map_داخلا *pcie_mmio_mem,
                                  ثابت Mem_Map_داخلا *مٿاهون_pcie_mmio_mem,
                                  ثابت Mem_Map_داخلا *pcie_pio_mem,
                                  صحيح pcie_irq)
{
    DeviceState *dev;
    يادگيري_Region *ecam_alias, *ecam_reg;
    يادگيري_Region *mmio_alias, *مٿاهون_mmio_alias, *mmio_reg;
    hwaddr ecam_بنياد = pcie_ecam_mem->بنياد;
    hwaddr ecam_سائيز = pcie_ecam_mem->سائيز;
    hwaddr mmio_بنياد = pcie_mmio_mem->بنياد;
    hwaddr mmio_سائيز = pcie_mmio_mem->سائيز;
    hwaddr مٿاهون_mmio_بنياد = مٿاهون_pcie_mmio_mem->بنياد;
    hwaddr مٿاهون_mmio_سائيز = مٿاهون_pcie_mmio_mem->سائيز;
    hwaddr pio_بنياد = pcie_pio_mem->بنياد;
    hwaddr pio_سائيز = pcie_pio_mem->سائيز;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_سيٽ_uint(OBJECT(dev), PCI_HOST_ECAM_بنياد,
                            ecam_بنياد, عدم);
    object_property_سيٽ_int(OBJECT(dev), PCI_HOST_ECAM_سائيز,
                            ecam_سائيز, عدم);
    object_property_سيٽ_uint(OBJECT(dev), PCI_HOST_هيٺ_4_G_MMIO_بنياد,
                             mmio_بنياد, عدم);
    object_property_سيٽ_int(OBJECT(dev), PCI_HOST_هيٺ_4_G_MMIO_سائيز,
                            mmio_سائيز, عدم);
    object_property_سيٽ_uint(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_بنياد,
                             مٿاهون_mmio_بنياد, عدم);
    object_property_سيٽ_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_سائيز,
                            مٿاهون_mmio_سائيز, عدم);
    object_property_سيٽ_uint(OBJECT(dev), PCI_HOST_PIO_بنياد,
                            pio_بنياد, عدم);
    object_property_سيٽ_int(OBJECT(dev), PCI_HOST_PIO_سائيز,
                            pio_سائيز, عدم);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(يادگيري_Region, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    يادگيري_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_سائيز);
    يادگيري_region_جوڙيو_subregion(get_system_يادگيري(), ecam_بنياد, ecam_alias);

    mmio_alias = g_new0(يادگيري_Region, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    يادگيري_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_بنياد, mmio_سائيز);
    يادگيري_region_جوڙيو_subregion(get_system_يادگيري(), mmio_بنياد, mmio_alias);

    /* Map high MMIO space */
    مٿاهون_mmio_alias = g_new0(يادگيري_Region, 1);
    يادگيري_region_init_alias(مٿاهون_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, مٿاهون_mmio_بنياد, مٿاهون_mmio_سائيز);
    يادگيري_region_جوڙيو_subregion(get_system_يادگيري(), مٿاهون_mmio_بنياد,
                                مٿاهون_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_بنياد);

    لكل (صحيح i = 0; i < PCI_انگ_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_اندريون(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_سيٽ_irq_انگ(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    أرجع dev;
}
