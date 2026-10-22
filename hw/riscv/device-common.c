/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/device-common.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hw/block/flash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/platform-bus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci-host/gpex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qom/object.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/address-spaces.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *platform_bus_mem,
                                       ਪੂਰਨ platform_bus_ਅਧਾਰ_irq,
                                       ਪੂਰਨ platform_bus_ਅੰਕ_irqs)
{
    ਸਮ੍ਰਿਤੀ_Region *sysmem = get_system_ਸਮ੍ਰਿਤੀ();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_ਸੈੱਟ_uint32(dev, "num_irqs", platform_bus_ਅੰਕ_irqs);
    qdev_prop_ਸੈੱਟ_uint32(dev, "mmio_size", platform_bus_mem->ਮਾਪ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    ਲਈ (ਪੂਰਨ i = 0; i < platform_bus_ਅੰਕ_irqs; i++) {
        ਪੂਰਨ irq = platform_bus_ਅਧਾਰ_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_ਵਿੱਚ(irqchip, irq));
    }

    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(sysmem,
                                platform_bus_mem->ਅਧਾਰ,
                                sysbus_mmio_get_region(sysbus, 0));

    ਵਾਪਸ dev;
}

PFlashCFI01 *riscv_flash_create(Object *ਮਾਪਾ, ਅਚਲ ਅੱਖਰ *name,
                                ਅਚਲ ਅੱਖਰ *alias_prop_name,
                                ਪੂਰਨ flash_sector_ਮਾਪ)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_ਸੈੱਟ_uint64(dev, "sector-length", flash_sector_ਮਾਪ);
    qdev_prop_ਸੈੱਟ_uint8(dev, "width", 4);
    qdev_prop_ਸੈੱਟ_uint8(dev, "device-width", 2);
    qdev_prop_ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(dev, "big-endian", false);
    qdev_prop_ਸੈੱਟ_uint16(dev, "id0", 0x89);
    qdev_prop_ਸੈੱਟ_uint16(dev, "id1", 0x18);
    qdev_prop_ਸੈੱਟ_uint16(dev, "id2", 0x00);
    qdev_prop_ਸੈੱਟ_uint16(dev, "id3", 0x00);
    qdev_prop_ਸੈੱਟ_string(dev, "name", name);

    object_property_ਜੋੜੋ_ਬੱਚਾ(ਮਾਪਾ, name, OBJECT(dev));
    object_property_ਜੋੜੋ_alias(ਮਾਪਾ, alias_prop_name,
                              OBJECT(dev), "drive");

    ਵਾਪਸ PFLASH_CFI01(dev);
}

ਖਾਲੀ riscv_init_flash_map(PFlashCFI01 *flash, hwaddr ਅਧਾਰ, hwaddr ਮਾਪ,
                          ਸਮ੍ਰਿਤੀ_Region *sysmem, ਪੂਰਨ flash_sector_ਮਾਪ)
{
    DeviceState *dev = DEVICE(flash);

    ਪ੍ਰਤਿਜ੍ਞਾ(QEMU_IS_ALIGNED(ਮਾਪ, flash_sector_ਮਾਪ));
    ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ / flash_sector_ਮਾਪ <= UINT32_MAX);

    qdev_prop_ਸੈੱਟ_uint32(dev, "num-blocks", ਮਾਪ / flash_sector_ਮਾਪ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(sysmem, ਅਧਾਰ,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(ਸਮ੍ਰਿਤੀ_Region *sys_mem,
                                  DeviceState *irqchip,
                                  ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *pcie_ecam_mem,
                                  ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *pcie_mmio_mem,
                                  ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *ਉੱਚਾ_pcie_mmio_mem,
                                  ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *pcie_pio_mem,
                                  ਪੂਰਨ pcie_irq)
{
    DeviceState *dev;
    ਸਮ੍ਰਿਤੀ_Region *ecam_alias, *ecam_reg;
    ਸਮ੍ਰਿਤੀ_Region *mmio_alias, *ਉੱਚਾ_mmio_alias, *mmio_reg;
    hwaddr ecam_ਅਧਾਰ = pcie_ecam_mem->ਅਧਾਰ;
    hwaddr ecam_ਮਾਪ = pcie_ecam_mem->ਮਾਪ;
    hwaddr mmio_ਅਧਾਰ = pcie_mmio_mem->ਅਧਾਰ;
    hwaddr mmio_ਮਾਪ = pcie_mmio_mem->ਮਾਪ;
    hwaddr ਉੱਚਾ_mmio_ਅਧਾਰ = ਉੱਚਾ_pcie_mmio_mem->ਅਧਾਰ;
    hwaddr ਉੱਚਾ_mmio_ਮਾਪ = ਉੱਚਾ_pcie_mmio_mem->ਮਾਪ;
    hwaddr pio_ਅਧਾਰ = pcie_pio_mem->ਅਧਾਰ;
    hwaddr pio_ਮਾਪ = pcie_pio_mem->ਮਾਪ;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_ਸੈੱਟ_uint(OBJECT(dev), PCI_HOST_ECAM_ਅਧਾਰ,
                            ecam_ਅਧਾਰ, ਨਿਰਾ);
    object_property_ਸੈੱਟ_int(OBJECT(dev), PCI_HOST_ECAM_ਮਾਪ,
                            ecam_ਮਾਪ, ਨਿਰਾ);
    object_property_ਸੈੱਟ_uint(OBJECT(dev), PCI_HOST_ਹੇਠਾਂ_4_G_MMIO_ਅਧਾਰ,
                             mmio_ਅਧਾਰ, ਨਿਰਾ);
    object_property_ਸੈੱਟ_int(OBJECT(dev), PCI_HOST_ਹੇਠਾਂ_4_G_MMIO_ਮਾਪ,
                            mmio_ਮਾਪ, ਨਿਰਾ);
    object_property_ਸੈੱਟ_uint(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_ਅਧਾਰ,
                             ਉੱਚਾ_mmio_ਅਧਾਰ, ਨਿਰਾ);
    object_property_ਸੈੱਟ_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_ਮਾਪ,
                            ਉੱਚਾ_mmio_ਮਾਪ, ਨਿਰਾ);
    object_property_ਸੈੱਟ_uint(OBJECT(dev), PCI_HOST_PIO_ਅਧਾਰ,
                            pio_ਅਧਾਰ, ਨਿਰਾ);
    object_property_ਸੈੱਟ_int(OBJECT(dev), PCI_HOST_PIO_ਮਾਪ,
                            pio_ਮਾਪ, ਨਿਰਾ);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(ਸਮ੍ਰਿਤੀ_Region, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    ਸਮ੍ਰਿਤੀ_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(get_system_ਸਮ੍ਰਿਤੀ(), ecam_ਅਧਾਰ, ecam_alias);

    mmio_alias = g_new0(ਸਮ੍ਰਿਤੀ_Region, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    ਸਮ੍ਰਿਤੀ_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_ਅਧਾਰ, mmio_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(get_system_ਸਮ੍ਰਿਤੀ(), mmio_ਅਧਾਰ, mmio_alias);

    /* Map high MMIO space */
    ਉੱਚਾ_mmio_alias = g_new0(ਸਮ੍ਰਿਤੀ_Region, 1);
    ਸਮ੍ਰਿਤੀ_region_init_alias(ਉੱਚਾ_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, ਉੱਚਾ_mmio_ਅਧਾਰ, ਉੱਚਾ_mmio_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(get_system_ਸਮ੍ਰਿਤੀ(), ਉੱਚਾ_mmio_ਅਧਾਰ,
                                ਉੱਚਾ_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_ਅਧਾਰ);

    ਲਈ (ਪੂਰਨ i = 0; i < PCI_ਅੰਕ_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_ਵਿੱਚ(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_ਸੈੱਟ_irq_ਅੰਕ(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    ਵਾਪਸ dev;
}
