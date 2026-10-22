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
                                       ثابت MemMapEntry *platform_bus_mem,
                                       صحيح platform_bus_base_irq,
                                       صحيح platform_bus_num_irqs)
{
    MemoryRegion *sysmem = get_system_memory();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_set_uint32(dev, "num_irqs", platform_bus_num_irqs);
    qdev_prop_set_uint32(dev, "mmio_size", platform_bus_mem->size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    עבור (صحيح i = 0; i < platform_bus_num_irqs; i++) {
        صحيح irq = platform_bus_base_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_in(irqchip, irq));
    }

    memory_region_add_subregion(sysmem,
                                platform_bus_mem->base,
                                sysbus_mmio_get_region(sysbus, 0));

    החזר dev;
}

PFlashCFI01 *riscv_flash_create(Object *parent, ثابت حرف *name,
                                ثابت حرف *alias_prop_name,
                                صحيح flash_sector_size)
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

    החזר PFLASH_CFI01(dev);
}

فراغ riscv_init_flash_map(PFlashCFI01 *flash, hwaddr base, hwaddr size,
                          MemoryRegion *sysmem, صحيح flash_sector_size)
{
    DeviceState *dev = DEVICE(flash);

    أكد(QEMU_IS_ALIGNED(size, flash_sector_size));
    أكد(size / flash_sector_size <= UINT32_MAX);

    qdev_prop_set_uint32(dev, "num-blocks", size / flash_sector_size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    memory_region_add_subregion(sysmem, base,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(MemoryRegion *sys_mem,
                                  DeviceState *irqchip,
                                  ثابت MemMapEntry *pcie_ecam_mem,
                                  ثابت MemMapEntry *pcie_mmio_mem,
                                  ثابت MemMapEntry *high_pcie_mmio_mem,
                                  ثابت MemMapEntry *pcie_pio_mem,
                                  صحيح pcie_irq)
{
    DeviceState *dev;
    MemoryRegion *ecam_alias, *ecam_reg;
    MemoryRegion *mmio_alias, *high_mmio_alias, *mmio_reg;
    hwaddr ecam_base = pcie_ecam_mem->base;
    hwaddr ecam_size = pcie_ecam_mem->size;
    hwaddr mmio_base = pcie_mmio_mem->base;
    hwaddr mmio_size = pcie_mmio_mem->size;
    hwaddr high_mmio_base = high_pcie_mmio_mem->base;
    hwaddr high_mmio_size = high_pcie_mmio_mem->size;
    hwaddr pio_base = pcie_pio_mem->base;
    hwaddr pio_size = pcie_pio_mem->size;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_set_uint(OBJECT(dev), PCI_HOST_ECAM_BASE,
                            ecam_base, عدم);
    object_property_set_int(OBJECT(dev), PCI_HOST_ECAM_SIZE,
                            ecam_size, عدم);
    object_property_set_uint(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_BASE,
                             mmio_base, عدم);
    object_property_set_int(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_SIZE,
                            mmio_size, عدم);
    object_property_set_uint(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_BASE,
                             high_mmio_base, عدم);
    object_property_set_int(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_SIZE,
                            high_mmio_size, عدم);
    object_property_set_uint(OBJECT(dev), PCI_HOST_PIO_BASE,
                            pio_base, عدم);
    object_property_set_int(OBJECT(dev), PCI_HOST_PIO_SIZE,
                            pio_size, عدم);

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
    high_mmio_alias = g_new0(MemoryRegion, 1);
    memory_region_init_alias(high_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, high_mmio_base, high_mmio_size);
    memory_region_add_subregion(get_system_memory(), high_mmio_base,
                                high_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_base);

    עבור (صحيح i = 0; i < PCI_NUM_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_in(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_set_irq_num(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    החזר dev;
}
