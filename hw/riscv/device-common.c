/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#समावेश "qemu/osdep.h"

#समावेश "hw/riscv/device-common.h"

#समावेश "hw/block/flash.h"
#समावेश "hw/core/platform-bus.h"
#समावेश "hw/core/qdev.h"
#समावेश "hw/core/qdev-properties.h"
#समावेश "hw/core/sysbus.h"
#समावेश "hw/pci/pci.h"
#समावेश "hw/pci-host/gpex.h"
#समावेश "qapi/error.h"
#समावेश "qom/object.h"
#समावेश "system/address-spaces.h"
#समावेश "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       स्थिर MemMapEntry *platform_bus_mem,
                                       पूर्णांक platform_bus_base_irq,
                                       पूर्णांक platform_bus_अंक_irqs)
{
    MemoryRegion *sysmem = get_system_memory();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_set_uint32(dev, "num_irqs", platform_bus_अंक_irqs);
    qdev_prop_set_uint32(dev, "mmio_size", platform_bus_mem->आकार);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    क्रम (पूर्णांक i = 0; i < platform_bus_अंक_irqs; i++) {
        पूर्णांक irq = platform_bus_base_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_in(irqchip, irq));
    }

    memory_region_add_subregion(sysmem,
                                platform_bus_mem->base,
                                sysbus_mmio_get_region(sysbus, 0));

    वापस dev;
}

PFlashCFI01 *riscv_flash_create(Object *parent, स्थिर अक्षर *name,
                                स्थिर अक्षर *alias_prop_name,
                                पूर्णांक flash_sector_आकार)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_set_uint64(dev, "sector-length", flash_sector_आकार);
    qdev_prop_set_uint8(dev, "width", 4);
    qdev_prop_set_uint8(dev, "device-width", 2);
    qdev_prop_set_bit(dev, "big-endian", false);
    qdev_prop_set_uint16(dev, "id0", 0x89);
    qdev_prop_set_uint16(dev, "id1", 0x18);
    qdev_prop_set_uint16(dev, "id2", 0x00);
    qdev_prop_set_uint16(dev, "id3", 0x00);
    qdev_prop_set_string(dev, "name", name);

    object_property_add_child(parent, name, OBJECT(dev));
    object_property_add_alias(parent, alias_prop_name,
                              OBJECT(dev), "drive");

    वापस PFLASH_CFI01(dev);
}

व्योम riscv_init_flash_map(PFlashCFI01 *flash, hwaddr base, hwaddr आकार,
                          MemoryRegion *sysmem, पूर्णांक flash_sector_आकार)
{
    DeviceState *dev = DEVICE(flash);

    निश्चित(QEMU_IS_ALIGNED(आकार, flash_sector_आकार));
    निश्चित(आकार / flash_sector_आकार <= UINT32_MAX);

    qdev_prop_set_uint32(dev, "num-blocks", आकार / flash_sector_आकार);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    memory_region_add_subregion(sysmem, base,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(MemoryRegion *sys_mem,
                                  DeviceState *irqchip,
                                  स्थिर MemMapEntry *pcie_ecam_mem,
                                  स्थिर MemMapEntry *pcie_mmio_mem,
                                  स्थिर MemMapEntry *ऊपरी_pcie_mmio_mem,
                                  स्थिर MemMapEntry *pcie_pio_mem,
                                  पूर्णांक pcie_irq)
{
    DeviceState *dev;
    MemoryRegion *ecam_alias, *ecam_reg;
    MemoryRegion *mmio_alias, *ऊपरी_mmio_alias, *mmio_reg;
    hwaddr ecam_base = pcie_ecam_mem->base;
    hwaddr ecam_आकार = pcie_ecam_mem->आकार;
    hwaddr mmio_base = pcie_mmio_mem->base;
    hwaddr mmio_आकार = pcie_mmio_mem->आकार;
    hwaddr ऊपरी_mmio_base = ऊपरी_pcie_mmio_mem->base;
    hwaddr ऊपरी_mmio_आकार = ऊपरी_pcie_mmio_mem->आकार;
    hwaddr pio_base = pcie_pio_mem->base;
    hwaddr pio_आकार = pcie_pio_mem->आकार;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_set_uint(OBJECT(dev), PCI_HOST_ECAM_BASE,
                            ecam_base, शून्य);
    object_property_set_int(OBJECT(dev), PCI_HOST_ECAM_आकार,
                            ecam_आकार, शून्य);
    object_property_set_uint(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_BASE,
                             mmio_base, शून्य);
    object_property_set_int(OBJECT(dev), PCI_HOST_BELOW_4_G_MMIO_आकार,
                            mmio_आकार, शून्य);
    object_property_set_uint(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_BASE,
                             ऊपरी_mmio_base, शून्य);
    object_property_set_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_आकार,
                            ऊपरी_mmio_आकार, शून्य);
    object_property_set_uint(OBJECT(dev), PCI_HOST_PIO_BASE,
                            pio_base, शून्य);
    object_property_set_int(OBJECT(dev), PCI_HOST_PIO_आकार,
                            pio_आकार, शून्य);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(MemoryRegion, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    memory_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_आकार);
    memory_region_add_subregion(get_system_memory(), ecam_base, ecam_alias);

    mmio_alias = g_new0(MemoryRegion, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    memory_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_base, mmio_आकार);
    memory_region_add_subregion(get_system_memory(), mmio_base, mmio_alias);

    /* Map high MMIO space */
    ऊपरी_mmio_alias = g_new0(MemoryRegion, 1);
    memory_region_init_alias(ऊपरी_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, ऊपरी_mmio_base, ऊपरी_mmio_आकार);
    memory_region_add_subregion(get_system_memory(), ऊपरी_mmio_base,
                                ऊपरी_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_base);

    क्रम (पूर्णांक i = 0; i < PCI_अंक_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_in(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_set_irq_अंक(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    वापस dev;
}
