/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ଅନ୍ତର୍ଭାଵ "qemu/osdep.h"

#ଅନ୍ତର୍ଭାଵ "hw/riscv/device-common.h"

#ଅନ୍ତର୍ଭାଵ "hw/block/flash.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/platform-bus.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/qdev.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/qdev-properties.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/sysbus.h"
#ଅନ୍ତର୍ଭାଵ "hw/pci/pci.h"
#ଅନ୍ତର୍ଭାଵ "hw/pci-host/gpex.h"
#ଅନ୍ତର୍ଭାଵ "qapi/error.h"
#ଅନ୍ତର୍ଭାଵ "qom/object.h"
#ଅନ୍ତର୍ଭାଵ "system/address-spaces.h"
#ଅନ୍ତର୍ଭାଵ "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       ଅଚଳ MemMapEntry *platform_bus_mem,
                                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା platform_bus_base_irq,
                                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା platform_bus_ଅଙ୍କ_irqs)
{
    MemoryRegion *sysmem = get_system_memory();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_set_uint32(dev, "num_irqs", platform_bus_ଅଙ୍କ_irqs);
    qdev_prop_set_uint32(dev, "mmio_size", platform_bus_mem->size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < platform_bus_ଅଙ୍କ_irqs; i++) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq = platform_bus_base_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_in(irqchip, irq));
    }

    memory_region_add_subregion(sysmem,
                                platform_bus_mem->base,
                                sysbus_mmio_get_region(sysbus, 0));

    ଫେରନ୍ତୁ dev;
}

PFlashCFI01 *riscv_flash_create(Object *parent, ଅଚଳ ଅକ୍ଷର *name,
                                ଅଚଳ ଅକ୍ଷର *alias_prop_name,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flash_sector_size)
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

    ଫେରନ୍ତୁ PFLASH_CFI01(dev);
}

ଶୂନ୍ୟ riscv_init_flash_map(PFlashCFI01 *flash, hwaddr base, hwaddr size,
                          MemoryRegion *sysmem, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flash_sector_size)
{
    DeviceState *dev = DEVICE(flash);

    ପ୍ରତିଜ୍ଞା(QEMU_IS_ALIGNED(size, flash_sector_size));
    ପ୍ରତିଜ୍ଞା(size / flash_sector_size <= UINT32_MAX);

    qdev_prop_set_uint32(dev, "num-blocks", size / flash_sector_size);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    memory_region_add_subregion(sysmem, base,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(MemoryRegion *sys_mem,
                                  DeviceState *irqchip,
                                  ଅଚଳ MemMapEntry *pcie_ecam_mem,
                                  ଅଚଳ MemMapEntry *pcie_mmio_mem,
                                  ଅଚଳ MemMapEntry *ଉପର_pcie_mmio_mem,
                                  ଅଚଳ MemMapEntry *pcie_pio_mem,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pcie_irq)
{
    DeviceState *dev;
    MemoryRegion *ecam_alias, *ecam_reg;
    MemoryRegion *mmio_alias, *ଉପର_mmio_alias, *mmio_reg;
    hwaddr ecam_base = pcie_ecam_mem->base;
    hwaddr ecam_size = pcie_ecam_mem->size;
    hwaddr mmio_base = pcie_mmio_mem->base;
    hwaddr mmio_size = pcie_mmio_mem->size;
    hwaddr ଉପର_mmio_base = ଉପର_pcie_mmio_mem->base;
    hwaddr ଉପର_mmio_size = ଉପର_pcie_mmio_mem->size;
    hwaddr pio_base = pcie_pio_mem->base;
    hwaddr pio_size = pcie_pio_mem->size;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_set_uint(OBJECT(dev), PCI_HOST_ECAM_BASE,
                            ecam_base, ଖାଲି);
    object_property_set_int(OBJECT(dev), PCI_HOST_ECAM_SIZE,
                            ecam_size, ଖାଲି);
    object_property_set_uint(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_BASE,
                             mmio_base, ଖାଲି);
    object_property_set_int(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_SIZE,
                            mmio_size, ଖାଲି);
    object_property_set_uint(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_BASE,
                             ଉପର_mmio_base, ଖାଲି);
    object_property_set_int(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_SIZE,
                            ଉପର_mmio_size, ଖାଲି);
    object_property_set_uint(OBJECT(dev), PCI_HOST_PIO_BASE,
                            pio_base, ଖାଲି);
    object_property_set_int(OBJECT(dev), PCI_HOST_PIO_SIZE,
                            pio_size, ଖାଲି);

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
    ଉପର_mmio_alias = g_new0(MemoryRegion, 1);
    memory_region_init_alias(ଉପର_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, ଉପର_mmio_base, ଉପର_mmio_size);
    memory_region_add_subregion(get_system_memory(), ଉପର_mmio_base,
                                ଉପର_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_base);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < PCI_ଅଙ୍କ_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_in(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_set_irq_ଅଙ୍କ(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    ଫେରନ୍ତୁ dev;
}
