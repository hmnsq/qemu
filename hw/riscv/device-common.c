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
                                       ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *platform_bus_mem,
                                       ಪೂರ್ಣಾಂಕ platform_bus_ತಳ_irq,
                                       ಪೂರ್ಣಾಂಕ platform_bus_ಅಂಕೆ_irqs)
{
    ಸ್ಮರಣೆ_Region *sysmem = get_system_ಸ್ಮರಣೆ();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_ಗಣ_uint32(dev, "num_irqs", platform_bus_ಅಂಕೆ_irqs);
    qdev_prop_ಗಣ_uint32(dev, "mmio_size", platform_bus_mem->ಗಾತ್ರ_2);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < platform_bus_ಅಂಕೆ_irqs; i++) {
        ಪೂರ್ಣಾಂಕ irq = platform_bus_ತಳ_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_ಒಳ(irqchip, irq));
    }

    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sysmem,
                                platform_bus_mem->ತಳ,
                                sysbus_mmio_get_region(sysbus, 0));

    ಹಿಂತಿರುಗಿ dev;
}

PFlashCFI01 *riscv_flash_create(Object *ಪೋಷಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias_prop_name,
                                ಪೂರ್ಣಾಂಕ flash_sector_ಗಾತ್ರ)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_ಗಣ_uint64(dev, "sector-length", flash_sector_ಗಾತ್ರ);
    qdev_prop_ಗಣ_uint8(dev, "width", 4);
    qdev_prop_ಗಣ_uint8(dev, "device-width", 2);
    qdev_prop_ಗಣ_ಬಿಟ್(dev, "big-endian", false);
    qdev_prop_ಗಣ_uint16(dev, "id0", 0x89);
    qdev_prop_ಗಣ_uint16(dev, "id1", 0x18);
    qdev_prop_ಗಣ_uint16(dev, "id2", 0x00);
    qdev_prop_ಗಣ_uint16(dev, "id3", 0x00);
    qdev_prop_ಗಣ_string(dev, "name", name);

    object_property_ಕೂಡಿಸು_ಮಗು(ಪೋಷಕ, name, OBJECT(dev));
    object_property_ಕೂಡಿಸು_alias(ಪೋಷಕ, alias_prop_name,
                              OBJECT(dev), "drive");

    ಹಿಂತಿರುಗಿ PFLASH_CFI01(dev);
}

ಶೂನ್ಯ riscv_init_flash_map(PFlashCFI01 *flash, hwaddr ತಳ, hwaddr ಗಾತ್ರ_2,
                          ಸ್ಮರಣೆ_Region *sysmem, ಪೂರ್ಣಾಂಕ flash_sector_ಗಾತ್ರ)
{
    DeviceState *dev = DEVICE(flash);

    ಪ್ರತಿಜ್ಞಾ(QEMU_IS_ALIGNED(ಗಾತ್ರ_2, flash_sector_ಗಾತ್ರ));
    ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_2 / flash_sector_ಗಾತ್ರ <= UINT32_MAX);

    qdev_prop_ಗಣ_uint32(dev, "num-blocks", ಗಾತ್ರ_2 / flash_sector_ಗಾತ್ರ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sysmem, ತಳ,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(ಸ್ಮರಣೆ_Region *sys_mem,
                                  DeviceState *irqchip,
                                  ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *pcie_ecam_mem,
                                  ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *pcie_mmio_mem,
                                  ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *ಮೇಲಿನ_pcie_mmio_mem,
                                  ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *pcie_pio_mem,
                                  ಪೂರ್ಣಾಂಕ pcie_irq)
{
    DeviceState *dev;
    ಸ್ಮರಣೆ_Region *ecam_alias, *ecam_reg;
    ಸ್ಮರಣೆ_Region *mmio_alias, *ಮೇಲಿನ_mmio_alias, *mmio_reg;
    hwaddr ecam_ತಳ = pcie_ecam_mem->ತಳ;
    hwaddr ecam_ಗಾತ್ರ = pcie_ecam_mem->ಗಾತ್ರ_2;
    hwaddr mmio_ತಳ = pcie_mmio_mem->ತಳ;
    hwaddr mmio_ಗಾತ್ರ = pcie_mmio_mem->ಗಾತ್ರ_2;
    hwaddr ಮೇಲಿನ_mmio_ತಳ = ಮೇಲಿನ_pcie_mmio_mem->ತಳ;
    hwaddr ಮೇಲಿನ_mmio_ಗಾತ್ರ = ಮೇಲಿನ_pcie_mmio_mem->ಗಾತ್ರ_2;
    hwaddr pio_ತಳ = pcie_pio_mem->ತಳ;
    hwaddr pio_ಗಾತ್ರ = pcie_pio_mem->ಗಾತ್ರ_2;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_ಗಣ_uint(OBJECT(dev), PCI_HOST_ECAM_ತಳ,
                            ecam_ತಳ, ಖಾಲಿ);
    object_property_ಗಣ_int(OBJECT(dev), PCI_HOST_ECAM_ಗಾತ್ರ,
                            ecam_ಗಾತ್ರ, ಖಾಲಿ);
    object_property_ಗಣ_uint(OBJECT(dev), PCI_HOST_ಕೆಳಗೆ_4_G_MMIO_ತಳ,
                             mmio_ತಳ, ಖಾಲಿ);
    object_property_ಗಣ_int(OBJECT(dev), PCI_HOST_ಕೆಳಗೆ_4_G_MMIO_ಗಾತ್ರ,
                            mmio_ಗಾತ್ರ, ಖಾಲಿ);
    object_property_ಗಣ_uint(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_ತಳ,
                             ಮೇಲಿನ_mmio_ತಳ, ಖಾಲಿ);
    object_property_ಗಣ_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_ಗಾತ್ರ,
                            ಮೇಲಿನ_mmio_ಗಾತ್ರ, ಖಾಲಿ);
    object_property_ಗಣ_uint(OBJECT(dev), PCI_HOST_PIO_ತಳ,
                            pio_ತಳ, ಖಾಲಿ);
    object_property_ಗಣ_int(OBJECT(dev), PCI_HOST_PIO_ಗಾತ್ರ,
                            pio_ಗಾತ್ರ, ಖಾಲಿ);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(ಸ್ಮರಣೆ_Region, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    ಸ್ಮರಣೆ_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(get_system_ಸ್ಮರಣೆ(), ecam_ತಳ, ecam_alias);

    mmio_alias = g_new0(ಸ್ಮರಣೆ_Region, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    ಸ್ಮರಣೆ_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_ತಳ, mmio_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(get_system_ಸ್ಮರಣೆ(), mmio_ತಳ, mmio_alias);

    /* Map high MMIO space */
    ಮೇಲಿನ_mmio_alias = g_new0(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_region_init_alias(ಮೇಲಿನ_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, ಮೇಲಿನ_mmio_ತಳ, ಮೇಲಿನ_mmio_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(get_system_ಸ್ಮರಣೆ(), ಮೇಲಿನ_mmio_ತಳ,
                                ಮೇಲಿನ_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_ತಳ);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < PCI_ಅಂಕೆ_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_ಒಳ(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_ಗಣ_irq_ಅಂಕೆ(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    ಹಿಂತಿರುಗಿ dev;
}
