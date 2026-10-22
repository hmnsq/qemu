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
                                       ثابت Mem_Map_ثبت *platform_bus_mem,
                                       صحيح platform_bus_بنسټ_irq,
                                       صحيح platform_bus_عدد_irqs)
{
    حافظه_Region *sysmem = get_system_حافظه();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_سېټ_uint32(dev, "num_irqs", platform_bus_عدد_irqs);
    qdev_prop_سېټ_uint32(dev, "mmio_size", platform_bus_mem->اندازه);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    لكل (صحيح i = 0; i < platform_bus_عدد_irqs; i++) {
        صحيح irq = platform_bus_بنسټ_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_دننه(irqchip, irq));
    }

    حافظه_region_جمع_کړه_subregion(sysmem,
                                platform_bus_mem->بنسټ,
                                sysbus_mmio_get_region(sysbus, 0));

    أرجع dev;
}

PFlashCFI01 *riscv_flash_create(Object *پلار, ثابت حرف *name,
                                ثابت حرف *alias_prop_name,
                                صحيح flash_sector_اندازه)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_سېټ_uint64(dev, "sector-length", flash_sector_اندازه);
    qdev_prop_سېټ_uint8(dev, "width", 4);
    qdev_prop_سېټ_uint8(dev, "device-width", 2);
    qdev_prop_سېټ_بټ(dev, "big-endian", false);
    qdev_prop_سېټ_uint16(dev, "id0", 0x89);
    qdev_prop_سېټ_uint16(dev, "id1", 0x18);
    qdev_prop_سېټ_uint16(dev, "id2", 0x00);
    qdev_prop_سېټ_uint16(dev, "id3", 0x00);
    qdev_prop_سېټ_string(dev, "name", name);

    object_property_جمع_کړه_ماشوم(پلار, name, OBJECT(dev));
    object_property_جمع_کړه_alias(پلار, alias_prop_name,
                              OBJECT(dev), "drive");

    أرجع PFLASH_CFI01(dev);
}

فراغ riscv_init_flash_map(PFlashCFI01 *flash, hwaddr بنسټ, hwaddr اندازه,
                          حافظه_Region *sysmem, صحيح flash_sector_اندازه)
{
    DeviceState *dev = DEVICE(flash);

    أكد(QEMU_IS_ALIGNED(اندازه, flash_sector_اندازه));
    أكد(اندازه / flash_sector_اندازه <= UINT32_MAX);

    qdev_prop_سېټ_uint32(dev, "num-blocks", اندازه / flash_sector_اندازه);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    حافظه_region_جمع_کړه_subregion(sysmem, بنسټ,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(حافظه_Region *sys_mem,
                                  DeviceState *irqchip,
                                  ثابت Mem_Map_ثبت *pcie_ecam_mem,
                                  ثابت Mem_Map_ثبت *pcie_mmio_mem,
                                  ثابت Mem_Map_ثبت *لوړ_pcie_mmio_mem,
                                  ثابت Mem_Map_ثبت *pcie_pio_mem,
                                  صحيح pcie_irq)
{
    DeviceState *dev;
    حافظه_Region *ecam_alias, *ecam_reg;
    حافظه_Region *mmio_alias, *لوړ_mmio_alias, *mmio_reg;
    hwaddr ecam_بنسټ = pcie_ecam_mem->بنسټ;
    hwaddr ecam_اندازه = pcie_ecam_mem->اندازه;
    hwaddr mmio_بنسټ = pcie_mmio_mem->بنسټ;
    hwaddr mmio_اندازه = pcie_mmio_mem->اندازه;
    hwaddr لوړ_mmio_بنسټ = لوړ_pcie_mmio_mem->بنسټ;
    hwaddr لوړ_mmio_اندازه = لوړ_pcie_mmio_mem->اندازه;
    hwaddr pio_بنسټ = pcie_pio_mem->بنسټ;
    hwaddr pio_اندازه = pcie_pio_mem->اندازه;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_سېټ_uint(OBJECT(dev), PCI_HOST_ECAM_بنسټ,
                            ecam_بنسټ, عدم);
    object_property_سېټ_int(OBJECT(dev), PCI_HOST_ECAM_اندازه,
                            ecam_اندازه, عدم);
    object_property_سېټ_uint(OBJECT(dev), PCI_HOST_لاندې_4_G_MMIO_بنسټ,
                             mmio_بنسټ, عدم);
    object_property_سېټ_int(OBJECT(dev), PCI_HOST_لاندې_4_G_MMIO_اندازه,
                            mmio_اندازه, عدم);
    object_property_سېټ_uint(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_بنسټ,
                             لوړ_mmio_بنسټ, عدم);
    object_property_سېټ_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_اندازه,
                            لوړ_mmio_اندازه, عدم);
    object_property_سېټ_uint(OBJECT(dev), PCI_HOST_PIO_بنسټ,
                            pio_بنسټ, عدم);
    object_property_سېټ_int(OBJECT(dev), PCI_HOST_PIO_اندازه,
                            pio_اندازه, عدم);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(حافظه_Region, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    حافظه_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_اندازه);
    حافظه_region_جمع_کړه_subregion(get_system_حافظه(), ecam_بنسټ, ecam_alias);

    mmio_alias = g_new0(حافظه_Region, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    حافظه_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_بنسټ, mmio_اندازه);
    حافظه_region_جمع_کړه_subregion(get_system_حافظه(), mmio_بنسټ, mmio_alias);

    /* Map high MMIO space */
    لوړ_mmio_alias = g_new0(حافظه_Region, 1);
    حافظه_region_init_alias(لوړ_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, لوړ_mmio_بنسټ, لوړ_mmio_اندازه);
    حافظه_region_جمع_کړه_subregion(get_system_حافظه(), لوړ_mmio_بنسټ,
                                لوړ_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_بنسټ);

    لكل (صحيح i = 0; i < PCI_عدد_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_دننه(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_سېټ_irq_عدد(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    أرجع dev;
}
