/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#अन्तर्भाव "qemu/osdep.h"

#अन्तर्भाव "hw/riscv/device-common.h"

#अन्तर्भाव "hw/block/flash.h"
#अन्तर्भाव "hw/core/platform-bus.h"
#अन्तर्भाव "hw/core/qdev.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "hw/core/sysbus.h"
#अन्तर्भाव "hw/pci/pci.h"
#अन्तर्भाव "hw/pci-host/gpex.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qom/object.h"
#अन्तर्भाव "system/address-spaces.h"
#अन्तर्भाव "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       ध्रुव MemMapEntry *platform_bus_mem,
                                       पूर्णाङ्क platform_bus_base_irq,
                                       पूर्णाङ्क platform_bus_अंक_irqs)
{
    MemoryRegion *sysmem = get_system_memory();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_set_uint32(dev, "num_irqs", platform_bus_अंक_irqs);
    qdev_prop_set_uint32(dev, "mmio_size", platform_bus_mem->size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    प्रति (पूर्णाङ्क i = 0; i < platform_bus_अंक_irqs; i++) {
        पूर्णाङ्क irq = platform_bus_base_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_in(irqchip, irq));
    }

    memory_region_add_subregion(sysmem,
                                platform_bus_mem->base,
                                sysbus_mmio_get_region(sysbus, 0));

    प्रत्यागम dev;
}

PFlashCFI01 *riscv_flash_create(Object *parent, ध्रुव वर्ण *name,
                                ध्रुव वर्ण *alias_prop_name,
                                पूर्णाङ्क flash_sector_size)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_set_uint64(dev, "sector-length", flash_sector_size);
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

    प्रत्यागम PFLASH_CFI01(dev);
}

शून्य riscv_init_flash_map(PFlashCFI01 *flash, hwaddr base, hwaddr size,
                          MemoryRegion *sysmem, पूर्णाङ्क flash_sector_size)
{
    DeviceState *dev = DEVICE(flash);

    प्रतिज्ञा(QEMU_IS_ALIGNED(size, flash_sector_size));
    प्रतिज्ञा(size / flash_sector_size <= UINT32_MAX);

    qdev_prop_set_uint32(dev, "num-blocks", size / flash_sector_size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    memory_region_add_subregion(sysmem, base,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(MemoryRegion *sys_mem,
                                  DeviceState *irqchip,
                                  ध्रुव MemMapEntry *pcie_ecam_mem,
                                  ध्रुव MemMapEntry *pcie_mmio_mem,
                                  ध्रुव MemMapEntry *ऊपरी_pcie_mmio_mem,
                                  ध्रुव MemMapEntry *pcie_pio_mem,
                                  पूर्णाङ्क pcie_irq)
{
    DeviceState *dev;
    MemoryRegion *ecam_alias, *ecam_reg;
    MemoryRegion *mmio_alias, *ऊपरी_mmio_alias, *mmio_reg;
    hwaddr ecam_base = pcie_ecam_mem->base;
    hwaddr ecam_size = pcie_ecam_mem->size;
    hwaddr mmio_base = pcie_mmio_mem->base;
    hwaddr mmio_size = pcie_mmio_mem->size;
    hwaddr ऊपरी_mmio_base = ऊपरी_pcie_mmio_mem->base;
    hwaddr ऊपरी_mmio_size = ऊपरी_pcie_mmio_mem->size;
    hwaddr pio_base = pcie_pio_mem->base;
    hwaddr pio_size = pcie_pio_mem->size;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_set_uint(OBJECT(dev), PCI_HOST_ECAM_BASE,
                            ecam_base, रिक्त);
    object_property_set_int(OBJECT(dev), PCI_HOST_ECAM_SIZE,
                            ecam_size, रिक्त);
    object_property_set_uint(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_BASE,
                             mmio_base, रिक्त);
    object_property_set_int(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_SIZE,
                            mmio_size, रिक्त);
    object_property_set_uint(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_BASE,
                             ऊपरी_mmio_base, रिक्त);
    object_property_set_int(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_SIZE,
                            ऊपरी_mmio_size, रिक्त);
    object_property_set_uint(OBJECT(dev), PCI_HOST_PIO_BASE,
                            pio_base, रिक्त);
    object_property_set_int(OBJECT(dev), PCI_HOST_PIO_SIZE,
                            pio_size, रिक्त);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(MemoryRegion, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    memory_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_size);
    memory_region_add_subregion(get_system_memory(), ecam_base, ecam_alias);

    mmio_alias = g_new0(MemoryRegion, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    memory_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_base, mmio_size);
    memory_region_add_subregion(get_system_memory(), mmio_base, mmio_alias);

    /* Map high MMIO space */
    ऊपरी_mmio_alias = g_new0(MemoryRegion, 1);
    memory_region_init_alias(ऊपरी_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, ऊपरी_mmio_base, ऊपरी_mmio_size);
    memory_region_add_subregion(get_system_memory(), ऊपरी_mmio_base,
                                ऊपरी_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_base);

    प्रति (पूर्णाङ्क i = 0; i < PCI_अंक_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_in(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_set_irq_अंक(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    प्रत्यागम dev;
}
