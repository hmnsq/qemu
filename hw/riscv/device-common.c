/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"

#ಅನ್ತರ್ಭಾವ "hw/riscv/device-common.h"

#ಅನ್ತರ್ಭಾವ "hw/block/flash.h"
#ಅನ್ತರ್ಭಾವ "hw/core/platform-bus.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pci.h"
#ಅನ್ತರ್ಭಾವ "hw/pci-host/gpex.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qom/object.h"
#ಅನ್ತರ್ಭಾವ "system/address-spaces.h"
#ಅನ್ತರ್ಭಾವ "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       ಧ್ರುವ MemMapEntry *platform_bus_mem,
                                       ಪೂರ್ಣಾಙ್ಕ platform_bus_base_irq,
                                       ಪೂರ್ಣಾಙ್ಕ platform_bus_ಅಂಕೆ_irqs)
{
    MemoryRegion *sysmem = get_system_memory();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_set_uint32(dev, "num_irqs", platform_bus_ಅಂಕೆ_irqs);
    qdev_prop_set_uint32(dev, "mmio_size", platform_bus_mem->size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < platform_bus_ಅಂಕೆ_irqs; i++) {
        ಪೂರ್ಣಾಙ್ಕ irq = platform_bus_base_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_in(irqchip, irq));
    }

    memory_region_add_subregion(sysmem,
                                platform_bus_mem->base,
                                sysbus_mmio_get_region(sysbus, 0));

    ಪ್ರತ್ಯಾಗಮ dev;
}

PFlashCFI01 *riscv_flash_create(Object *parent, ಧ್ರುವ ವರ್ಣ *name,
                                ಧ್ರುವ ವರ್ಣ *alias_prop_name,
                                ಪೂರ್ಣಾಙ್ಕ flash_sector_size)
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

    ಪ್ರತ್ಯಾಗಮ PFLASH_CFI01(dev);
}

ಶೂನ್ಯ riscv_init_flash_map(PFlashCFI01 *flash, hwaddr base, hwaddr size,
                          MemoryRegion *sysmem, ಪೂರ್ಣಾಙ್ಕ flash_sector_size)
{
    DeviceState *dev = DEVICE(flash);

    ಪ್ರತಿಜ್ಞಾ(QEMU_IS_ALIGNED(size, flash_sector_size));
    ಪ್ರತಿಜ್ಞಾ(size / flash_sector_size <= UINT32_MAX);

    qdev_prop_set_uint32(dev, "num-blocks", size / flash_sector_size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    memory_region_add_subregion(sysmem, base,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(MemoryRegion *sys_mem,
                                  DeviceState *irqchip,
                                  ಧ್ರುವ MemMapEntry *pcie_ecam_mem,
                                  ಧ್ರುವ MemMapEntry *pcie_mmio_mem,
                                  ಧ್ರುವ MemMapEntry *ಮೇಲಿನ_pcie_mmio_mem,
                                  ಧ್ರುವ MemMapEntry *pcie_pio_mem,
                                  ಪೂರ್ಣಾಙ್ಕ pcie_irq)
{
    DeviceState *dev;
    MemoryRegion *ecam_alias, *ecam_reg;
    MemoryRegion *mmio_alias, *ಮೇಲಿನ_mmio_alias, *mmio_reg;
    hwaddr ecam_base = pcie_ecam_mem->base;
    hwaddr ecam_size = pcie_ecam_mem->size;
    hwaddr mmio_base = pcie_mmio_mem->base;
    hwaddr mmio_size = pcie_mmio_mem->size;
    hwaddr ಮೇಲಿನ_mmio_base = ಮೇಲಿನ_pcie_mmio_mem->base;
    hwaddr ಮೇಲಿನ_mmio_size = ಮೇಲಿನ_pcie_mmio_mem->size;
    hwaddr pio_base = pcie_pio_mem->base;
    hwaddr pio_size = pcie_pio_mem->size;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_set_uint(OBJECT(dev), PCI_HOST_ECAM_BASE,
                            ecam_base, ರಿಕ್ತ);
    object_property_set_int(OBJECT(dev), PCI_HOST_ECAM_SIZE,
                            ecam_size, ರಿಕ್ತ);
    object_property_set_uint(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_BASE,
                             mmio_base, ರಿಕ್ತ);
    object_property_set_int(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_SIZE,
                            mmio_size, ರಿಕ್ತ);
    object_property_set_uint(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_BASE,
                             ಮೇಲಿನ_mmio_base, ರಿಕ್ತ);
    object_property_set_int(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_SIZE,
                            ಮೇಲಿನ_mmio_size, ರಿಕ್ತ);
    object_property_set_uint(OBJECT(dev), PCI_HOST_PIO_BASE,
                            pio_base, ರಿಕ್ತ);
    object_property_set_int(OBJECT(dev), PCI_HOST_PIO_SIZE,
                            pio_size, ರಿಕ್ತ);

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
    ಮೇಲಿನ_mmio_alias = g_new0(MemoryRegion, 1);
    memory_region_init_alias(ಮೇಲಿನ_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, ಮೇಲಿನ_mmio_base, ಮೇಲಿನ_mmio_size);
    memory_region_add_subregion(get_system_memory(), ಮೇಲಿನ_mmio_base,
                                ಮೇಲಿನ_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_base);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < PCI_ಅಂಕೆ_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_in(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_set_irq_ಅಂಕೆ(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    ಪ್ರತ್ಯಾಗಮ dev;
}
