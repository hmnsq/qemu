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
                                       ध्रुव Mem_Map_प्रविष्टिः *platform_bus_mem,
                                       पूर्णाङ्क platform_bus_आधारः_irq,
                                       पूर्णाङ्क platform_bus_अङ्कः_irqs)
{
    स्मृतिः_Region *sysmem = get_system_स्मृतिः();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_समुच्चयः_uint32(dev, "num_irqs", platform_bus_अङ्कः_irqs);
    qdev_prop_समुच्चयः_uint32(dev, "mmio_size", platform_bus_mem->आकार);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    प्रति (पूर्णाङ्क i = 0; i < platform_bus_अङ्कः_irqs; i++) {
        पूर्णाङ्क irq = platform_bus_आधारः_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_अन्तर्(irqchip, irq));
    }

    स्मृतिः_region_सङ्कलय_subregion(sysmem,
                                platform_bus_mem->आधारः,
                                sysbus_mmio_get_region(sysbus, 0));

    प्रत्यागम dev;
}

PFlashCFI01 *riscv_flash_create(Object *parent, ध्रुव वर्ण *name,
                                ध्रुव वर्ण *alias_prop_name,
                                पूर्णाङ्क flash_sector_आकार)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_समुच्चयः_uint64(dev, "sector-length", flash_sector_आकार);
    qdev_prop_समुच्चयः_uint8(dev, "width", 4);
    qdev_prop_समुच्चयः_uint8(dev, "device-width", 2);
    qdev_prop_समुच्चयः_bit(dev, "big-endian", false);
    qdev_prop_समुच्चयः_uint16(dev, "id0", 0x89);
    qdev_prop_समुच्चयः_uint16(dev, "id1", 0x18);
    qdev_prop_समुच्चयः_uint16(dev, "id2", 0x00);
    qdev_prop_समुच्चयः_uint16(dev, "id3", 0x00);
    qdev_prop_समुच्चयः_string(dev, "name", name);

    object_property_सङ्कलय_child(parent, name, OBJECT(dev));
    object_property_सङ्कलय_alias(parent, alias_prop_name,
                              OBJECT(dev), "drive");

    प्रत्यागम PFLASH_CFI01(dev);
}

शून्य riscv_init_flash_map(PFlashCFI01 *flash, hwaddr आधारः, hwaddr आकार,
                          स्मृतिः_Region *sysmem, पूर्णाङ्क flash_sector_आकार)
{
    DeviceState *dev = DEVICE(flash);

    प्रतिज्ञा(QEMU_IS_ALIGNED(आकार, flash_sector_आकार));
    प्रतिज्ञा(आकार / flash_sector_आकार <= UINT32_MAX);

    qdev_prop_समुच्चयः_uint32(dev, "num-blocks", आकार / flash_sector_आकार);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    स्मृतिः_region_सङ्कलय_subregion(sysmem, आधारः,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(स्मृतिः_Region *sys_mem,
                                  DeviceState *irqchip,
                                  ध्रुव Mem_Map_प्रविष्टिः *pcie_ecam_mem,
                                  ध्रुव Mem_Map_प्रविष्टिः *pcie_mmio_mem,
                                  ध्रुव Mem_Map_प्रविष्टिः *उच्चः_pcie_mmio_mem,
                                  ध्रुव Mem_Map_प्रविष्टिः *pcie_pio_mem,
                                  पूर्णाङ्क pcie_irq)
{
    DeviceState *dev;
    स्मृतिः_Region *ecam_alias, *ecam_reg;
    स्मृतिः_Region *mmio_alias, *उच्चः_mmio_alias, *mmio_reg;
    hwaddr ecam_आधारः = pcie_ecam_mem->आधारः;
    hwaddr ecam_आकार = pcie_ecam_mem->आकार;
    hwaddr mmio_आधारः = pcie_mmio_mem->आधारः;
    hwaddr mmio_आकार = pcie_mmio_mem->आकार;
    hwaddr उच्चः_mmio_आधारः = उच्चः_pcie_mmio_mem->आधारः;
    hwaddr उच्चः_mmio_आकार = उच्चः_pcie_mmio_mem->आकार;
    hwaddr pio_आधारः = pcie_pio_mem->आधारः;
    hwaddr pio_आकार = pcie_pio_mem->आकार;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_समुच्चयः_uint(OBJECT(dev), PCI_HOST_ECAM_आधारः,
                            ecam_आधारः, रिक्त);
    object_property_समुच्चयः_int(OBJECT(dev), PCI_HOST_ECAM_आकार,
                            ecam_आकार, रिक्त);
    object_property_समुच्चयः_uint(OBJECT(dev), PCI_HOST_अधः_4_G_MMIO_आधारः,
                             mmio_आधारः, रिक्त);
    object_property_समुच्चयः_int(OBJECT(dev), PCI_HOST_अधः_4_G_MMIO_आकार,
                            mmio_आकार, रिक्त);
    object_property_समुच्चयः_uint(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_आधारः,
                             उच्चः_mmio_आधारः, रिक्त);
    object_property_समुच्चयः_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_आकार,
                            उच्चः_mmio_आकार, रिक्त);
    object_property_समुच्चयः_uint(OBJECT(dev), PCI_HOST_PIO_आधारः,
                            pio_आधारः, रिक्त);
    object_property_समुच्चयः_int(OBJECT(dev), PCI_HOST_PIO_आकार,
                            pio_आकार, रिक्त);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(स्मृतिः_Region, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    स्मृतिः_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_आकार);
    स्मृतिः_region_सङ्कलय_subregion(get_system_स्मृतिः(), ecam_आधारः, ecam_alias);

    mmio_alias = g_new0(स्मृतिः_Region, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    स्मृतिः_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_आधारः, mmio_आकार);
    स्मृतिः_region_सङ्कलय_subregion(get_system_स्मृतिः(), mmio_आधारः, mmio_alias);

    /* Map high MMIO space */
    उच्चः_mmio_alias = g_new0(स्मृतिः_Region, 1);
    स्मृतिः_region_init_alias(उच्चः_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, उच्चः_mmio_आधारः, उच्चः_mmio_आकार);
    स्मृतिः_region_सङ्कलय_subregion(get_system_स्मृतिः(), उच्चः_mmio_आधारः,
                                उच्चः_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_आधारः);

    प्रति (पूर्णाङ्क i = 0; i < PCI_अङ्कः_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_अन्तर्(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_समुच्चयः_irq_अङ्कः(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    प्रत्यागम dev;
}
