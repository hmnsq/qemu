/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#অন্তর্ভাব "qemu/osdep.h"

#অন্তর্ভাব "hw/riscv/device-common.h"

#অন্তর্ভাব "hw/block/flash.h"
#অন্তর্ভাব "hw/core/platform-bus.h"
#অন্তর্ভাব "hw/core/qdev.h"
#অন্তর্ভাব "hw/core/qdev-properties.h"
#অন্তর্ভাব "hw/core/sysbus.h"
#অন্তর্ভাব "hw/pci/pci.h"
#অন্তর্ভাব "hw/pci-host/gpex.h"
#অন্তর্ভাব "qapi/error.h"
#অন্তর্ভাব "qom/object.h"
#অন্তর্ভাব "system/address-spaces.h"
#অন্তর্ভাব "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       ধ্রুবক MemMapEntry *platform_bus_mem,
                                       পূর্ণসংখ্যা platform_bus_base_irq,
                                       পূর্ণসংখ্যা platform_bus_অঙ্ক_irqs)
{
    MemoryRegion *sysmem = get_system_memory();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_set_uint32(dev, "num_irqs", platform_bus_অঙ্ক_irqs);
    qdev_prop_set_uint32(dev, "mmio_size", platform_bus_mem->আকার_2);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    জন্য (পূর্ণসংখ্যা i = 0; i < platform_bus_অঙ্ক_irqs; i++) {
        পূর্ণসংখ্যা irq = platform_bus_base_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_in(irqchip, irq));
    }

    memory_region_add_subregion(sysmem,
                                platform_bus_mem->base,
                                sysbus_mmio_get_region(sysbus, 0));

    ফেরত dev;
}

PFlashCFI01 *riscv_flash_create(Object *parent, ধ্রুবক অক্ষর *name,
                                ধ্রুবক অক্ষর *alias_prop_name,
                                পূর্ণসংখ্যা flash_sector_আকার)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_set_uint64(dev, "sector-length", flash_sector_আকার);
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

    ফেরত PFLASH_CFI01(dev);
}

শূন্য riscv_init_flash_map(PFlashCFI01 *flash, hwaddr base, hwaddr আকার_2,
                          MemoryRegion *sysmem, পূর্ণসংখ্যা flash_sector_আকার)
{
    DeviceState *dev = DEVICE(flash);

    প্রতিজ্ঞা(QEMU_IS_ALIGNED(আকার_2, flash_sector_আকার));
    প্রতিজ্ঞা(আকার_2 / flash_sector_আকার <= UINT32_MAX);

    qdev_prop_set_uint32(dev, "num-blocks", আকার_2 / flash_sector_আকার);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    memory_region_add_subregion(sysmem, base,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(MemoryRegion *sys_mem,
                                  DeviceState *irqchip,
                                  ধ্রুবক MemMapEntry *pcie_ecam_mem,
                                  ধ্রুবক MemMapEntry *pcie_mmio_mem,
                                  ধ্রুবক MemMapEntry *উপরের_pcie_mmio_mem,
                                  ধ্রুবক MemMapEntry *pcie_pio_mem,
                                  পূর্ণসংখ্যা pcie_irq)
{
    DeviceState *dev;
    MemoryRegion *ecam_alias, *ecam_reg;
    MemoryRegion *mmio_alias, *উপরের_mmio_alias, *mmio_reg;
    hwaddr ecam_base = pcie_ecam_mem->base;
    hwaddr ecam_আকার = pcie_ecam_mem->আকার_2;
    hwaddr mmio_base = pcie_mmio_mem->base;
    hwaddr mmio_আকার = pcie_mmio_mem->আকার_2;
    hwaddr উপরের_mmio_base = উপরের_pcie_mmio_mem->base;
    hwaddr উপরের_mmio_আকার = উপরের_pcie_mmio_mem->আকার_2;
    hwaddr pio_base = pcie_pio_mem->base;
    hwaddr pio_আকার = pcie_pio_mem->আকার_2;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_set_uint(OBJECT(dev), PCI_HOST_ECAM_BASE,
                            ecam_base, ফাঁকা);
    object_property_set_int(OBJECT(dev), PCI_HOST_ECAM_আকার,
                            ecam_আকার, ফাঁকা);
    object_property_set_uint(OBJECT(dev), PCI_HOST_BELOW_4G_MMIO_BASE,
                             mmio_base, ফাঁকা);
    object_property_set_int(OBJECT(dev), PCI_HOST_BELOW_4_G_MMIO_আকার,
                            mmio_আকার, ফাঁকা);
    object_property_set_uint(OBJECT(dev), PCI_HOST_ABOVE_4G_MMIO_BASE,
                             উপরের_mmio_base, ফাঁকা);
    object_property_set_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_আকার,
                            উপরের_mmio_আকার, ফাঁকা);
    object_property_set_uint(OBJECT(dev), PCI_HOST_PIO_BASE,
                            pio_base, ফাঁকা);
    object_property_set_int(OBJECT(dev), PCI_HOST_PIO_আকার,
                            pio_আকার, ফাঁকা);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(MemoryRegion, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    memory_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_আকার);
    memory_region_add_subregion(get_system_memory(), ecam_base, ecam_alias);

    mmio_alias = g_new0(MemoryRegion, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    memory_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_base, mmio_আকার);
    memory_region_add_subregion(get_system_memory(), mmio_base, mmio_alias);

    /* Map high MMIO space */
    উপরের_mmio_alias = g_new0(MemoryRegion, 1);
    memory_region_init_alias(উপরের_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, উপরের_mmio_base, উপরের_mmio_আকার);
    memory_region_add_subregion(get_system_memory(), উপরের_mmio_base,
                                উপরের_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_base);

    জন্য (পূর্ণসংখ্যা i = 0; i < PCI_অঙ্ক_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_in(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_set_irq_অঙ্ক(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    ফেরত dev;
}
