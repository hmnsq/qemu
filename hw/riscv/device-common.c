/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#অন্তর্ভাৱ "qemu/osdep.h"

#অন্তর্ভাৱ "hw/riscv/device-common.h"

#অন্তর্ভাৱ "hw/block/flash.h"
#অন্তর্ভাৱ "hw/core/platform-bus.h"
#অন্তর্ভাৱ "hw/core/qdev.h"
#অন্তর্ভাৱ "hw/core/qdev-properties.h"
#অন্তর্ভাৱ "hw/core/sysbus.h"
#অন্তর্ভাৱ "hw/pci/pci.h"
#অন্তর্ভাৱ "hw/pci-host/gpex.h"
#অন্তর্ভাৱ "qapi/error.h"
#অন্তর্ভাৱ "qom/object.h"
#অন্তর্ভাৱ "system/address-spaces.h"
#অন্তর্ভাৱ "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       ধ্রুৱ MemMapEntry *platform_bus_mem,
                                       পূর্ণাঙ্ক platform_bus_base_irq,
                                       পূর্ণাঙ্ক platform_bus_অঙ্ক_irqs)
{
    MemoryRegion *sysmem = get_system_memory();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_set_uint32(dev, "num_irqs", platform_bus_অঙ্ক_irqs);
    qdev_prop_set_uint32(dev, "mmio_size", platform_bus_mem->size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    বাবে (পূর্ণাঙ্ক i = 0; i < platform_bus_অঙ্ক_irqs; i++) {
        পূর্ণাঙ্ক irq = platform_bus_base_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_in(irqchip, irq));
    }

    memory_region_add_subregion(sysmem,
                                platform_bus_mem->base,
                                sysbus_mmio_get_region(sysbus, 0));

    ঘূৰাই dev;
}

PFlashCFI01 *riscv_flash_create(Object *parent, ধ্রুৱ ৱর্ণ *name,
                                ধ্রুৱ ৱর্ণ *alias_prop_name,
                                পূর্ণাঙ্ক flash_sector_size)
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

    ঘূৰাই PFLASH_CFI01(dev);
}

শূন্য riscv_init_flash_map(PFlashCFI01 *flash, hwaddr base, hwaddr size,
                          MemoryRegion *sysmem, পূর্ণাঙ্ক flash_sector_size)
{
    DeviceState *dev = DEVICE(flash);

    প্রতিজ্ঞা(QEMU_IS_ALIGNED(size, flash_sector_size));
    প্রতিজ্ঞা(size / flash_sector_size <= UINT32_MAX);

    qdev_prop_set_uint32(dev, "num-blocks", size / flash_sector_size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    memory_region_add_subregion(sysmem, base,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(MemoryRegion *sys_mem,
                                  DeviceState *irqchip,
                                  ধ্রুৱ MemMapEntry *pcie_ecam_mem,
                                  ধ্রুৱ MemMapEntry *pcie_mmio_mem,
                                  ধ্রুৱ MemMapEntry *উপৰেৰ_pcie_mmio_mem,
                                  ধ্রুৱ MemMapEntry *pcie_pio_mem,
                                  পূর্ণাঙ্ক pcie_irq)
{
    DeviceState *dev;
    MemoryRegion *ecam_alias, *ecam_reg;
    MemoryRegion *mmio_alias, *উপৰেৰ_mmio_alias, *mmio_reg;
    hwaddr ecam_base = pcie_ecam_mem->base;
    hwaddr ecam_size = pcie_ecam_mem->size;
    hwaddr mmio_base = pcie_mmio_mem->base;
    hwaddr mmio_size = pcie_mmio_mem->size;
    hwaddr উপৰেৰ_mmio_base = উপৰেৰ_pcie_mmio_mem->base;
    hwaddr উপৰেৰ_mmio_size = উপৰেৰ_pcie_mmio_mem->size;
    hwaddr pio_base = pcie_pio_mem->base;
    hwaddr pio_size = pcie_pio_mem->size;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_set_uint(OBJECT(dev), PCI_HOST_ECAM_BASE,
                            ecam_base, রিক্ত);
    object_property_set_int(OBJECT(dev), PCI_HOST_ECAM_SIZE,
                            ecam_size, রিক্ত);
    object_property_set_uint(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_BASE,
                             mmio_base, রিক্ত);
    object_property_set_int(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_SIZE,
                            mmio_size, রিক্ত);
    object_property_set_uint(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_BASE,
                             উপৰেৰ_mmio_base, রিক্ত);
    object_property_set_int(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_SIZE,
                            উপৰেৰ_mmio_size, রিক্ত);
    object_property_set_uint(OBJECT(dev), PCI_HOST_PIO_BASE,
                            pio_base, রিক্ত);
    object_property_set_int(OBJECT(dev), PCI_HOST_PIO_SIZE,
                            pio_size, রিক্ত);

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
    উপৰেৰ_mmio_alias = g_new0(MemoryRegion, 1);
    memory_region_init_alias(উপৰেৰ_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, উপৰেৰ_mmio_base, উপৰেৰ_mmio_size);
    memory_region_add_subregion(get_system_memory(), উপৰেৰ_mmio_base,
                                উপৰেৰ_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_base);

    বাবে (পূর্ণাঙ্ক i = 0; i < PCI_অঙ্ক_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_in(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_set_irq_অঙ্ক(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    ঘূৰাই dev;
}
