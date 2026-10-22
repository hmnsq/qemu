/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#అన్తర్భావ "qemu/osdep.h"

#అన్తర్భావ "hw/riscv/device-common.h"

#అన్తర్భావ "hw/block/flash.h"
#అన్తర్భావ "hw/core/platform-bus.h"
#అన్తర్భావ "hw/core/qdev.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "hw/pci/pci.h"
#అన్తర్భావ "hw/pci-host/gpex.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qom/object.h"
#అన్తర్భావ "system/address-spaces.h"
#అన్తర్భావ "system/memory.h"


DeviceState *riscv_create_platform_bus(DeviceState *irqchip,
                                       స్థిరరాశి Mem_Map_నమోదు *platform_bus_mem,
                                       పూర్ణాంకం platform_bus_పునాది_irq,
                                       పూర్ణాంకం platform_bus_అంకె_irqs)
{
    జ్ఞాపకం_Region *sysmem = get_system_జ్ఞాపకం();
    DeviceState *dev;
    SysBusDevice *sysbus;

    dev = qdev_new(TYPE_PLATFORM_BUS_DEVICE);
    dev->id = g_strdup(TYPE_PLATFORM_BUS_DEVICE);
    qdev_prop_సమితి_uint32(dev, "num_irqs", platform_bus_అంకె_irqs);
    qdev_prop_సమితి_uint32(dev, "mmio_size", platform_bus_mem->కొలత);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus = SYS_BUS_DEVICE(dev);
    కొరకు (పూర్ణాంకం i = 0; i < platform_bus_అంకె_irqs; i++) {
        పూర్ణాంకం irq = platform_bus_పునాది_irq + i;
        sysbus_connect_irq(sysbus, i, qdev_get_gpio_లోపల(irqchip, irq));
    }

    జ్ఞాపకం_region_కూడు_subregion(sysmem,
                                platform_bus_mem->పునాది,
                                sysbus_mmio_get_region(sysbus, 0));

    తిరిగి dev;
}

PFlashCFI01 *riscv_flash_create(Object *మాతృ, స్థిరరాశి అక్షరం *name,
                                స్థిరరాశి అక్షరం *alias_prop_name,
                                పూర్ణాంకం flash_sector_కొలత)
{
    /*
     * Create a single flash device.  We use the same parameters as
     * the flash devices on the ARM virt board.
     */
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    qdev_prop_సమితి_uint64(dev, "sector-length", flash_sector_కొలత);
    qdev_prop_సమితి_uint8(dev, "width", 4);
    qdev_prop_సమితి_uint8(dev, "device-width", 2);
    qdev_prop_సమితి_బిట్టు(dev, "big-endian", false);
    qdev_prop_సమితి_uint16(dev, "id0", 0x89);
    qdev_prop_సమితి_uint16(dev, "id1", 0x18);
    qdev_prop_సమితి_uint16(dev, "id2", 0x00);
    qdev_prop_సమితి_uint16(dev, "id3", 0x00);
    qdev_prop_సమితి_string(dev, "name", name);

    object_property_కూడు_బిడ్డ(మాతృ, name, OBJECT(dev));
    object_property_కూడు_alias(మాతృ, alias_prop_name,
                              OBJECT(dev), "drive");

    తిరిగి PFLASH_CFI01(dev);
}

శూన్య riscv_init_flash_map(PFlashCFI01 *flash, hwaddr పునాది, hwaddr కొలత,
                          జ్ఞాపకం_Region *sysmem, పూర్ణాంకం flash_sector_కొలత)
{
    DeviceState *dev = DEVICE(flash);

    ప్రతిజ్ఞా(QEMU_IS_ALIGNED(కొలత, flash_sector_కొలత));
    ప్రతిజ్ఞా(కొలత / flash_sector_కొలత <= UINT32_MAX);

    qdev_prop_సమితి_uint32(dev, "num-blocks", కొలత / flash_sector_కొలత);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    జ్ఞాపకం_region_కూడు_subregion(sysmem, పునాది,
                            sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0));
}

DeviceState *riscv_gpex_pcie_init(జ్ఞాపకం_Region *sys_mem,
                                  DeviceState *irqchip,
                                  స్థిరరాశి Mem_Map_నమోదు *pcie_ecam_mem,
                                  స్థిరరాశి Mem_Map_నమోదు *pcie_mmio_mem,
                                  స్థిరరాశి Mem_Map_నమోదు *ఎగువ_pcie_mmio_mem,
                                  స్థిరరాశి Mem_Map_నమోదు *pcie_pio_mem,
                                  పూర్ణాంకం pcie_irq)
{
    DeviceState *dev;
    జ్ఞాపకం_Region *ecam_alias, *ecam_reg;
    జ్ఞాపకం_Region *mmio_alias, *ఎగువ_mmio_alias, *mmio_reg;
    hwaddr ecam_పునాది = pcie_ecam_mem->పునాది;
    hwaddr ecam_కొలత = pcie_ecam_mem->కొలత;
    hwaddr mmio_పునాది = pcie_mmio_mem->పునాది;
    hwaddr mmio_కొలత = pcie_mmio_mem->కొలత;
    hwaddr ఎగువ_mmio_పునాది = ఎగువ_pcie_mmio_mem->పునాది;
    hwaddr ఎగువ_mmio_కొలత = ఎగువ_pcie_mmio_mem->కొలత;
    hwaddr pio_పునాది = pcie_pio_mem->పునాది;
    hwaddr pio_కొలత = pcie_pio_mem->కొలత;

    dev = qdev_new(TYPE_GPEX_HOST);

    /* Set GPEX object properties for the virt machine */
    object_property_సమితి_uint(OBJECT(dev), PCI_HOST_ECAM_పునాది,
                            ecam_పునాది, ఖాళీ);
    object_property_సమితి_int(OBJECT(dev), PCI_HOST_ECAM_కొలత,
                            ecam_కొలత, ఖాళీ);
    object_property_సమితి_uint(OBJECT(dev), PCI_HOST_క్రింద_4_G_MMIO_పునాది,
                             mmio_పునాది, ఖాళీ);
    object_property_సమితి_int(OBJECT(dev), PCI_HOST_క్రింద_4_G_MMIO_కొలత,
                            mmio_కొలత, ఖాళీ);
    object_property_సమితి_uint(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_పునాది,
                             ఎగువ_mmio_పునాది, ఖాళీ);
    object_property_సమితి_int(OBJECT(dev), PCI_HOST_ABOVE_4_G_MMIO_కొలత,
                            ఎగువ_mmio_కొలత, ఖాళీ);
    object_property_సమితి_uint(OBJECT(dev), PCI_HOST_PIO_పునాది,
                            pio_పునాది, ఖాళీ);
    object_property_సమితి_int(OBJECT(dev), PCI_HOST_PIO_కొలత,
                            pio_కొలత, ఖాళీ);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    ecam_alias = g_new0(జ్ఞాపకం_Region, 1);
    ecam_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 0);
    జ్ఞాపకం_region_init_alias(ecam_alias, OBJECT(dev), "pcie-ecam",
                             ecam_reg, 0, ecam_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(get_system_జ్ఞాపకం(), ecam_పునాది, ecam_alias);

    mmio_alias = g_new0(జ్ఞాపకం_Region, 1);
    mmio_reg = sysbus_mmio_get_region(SYS_BUS_DEVICE(dev), 1);
    జ్ఞాపకం_region_init_alias(mmio_alias, OBJECT(dev), "pcie-mmio",
                             mmio_reg, mmio_పునాది, mmio_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(get_system_జ్ఞాపకం(), mmio_పునాది, mmio_alias);

    /* Map high MMIO space */
    ఎగువ_mmio_alias = g_new0(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_region_init_alias(ఎగువ_mmio_alias, OBJECT(dev), "pcie-mmio-high",
                             mmio_reg, ఎగువ_mmio_పునాది, ఎగువ_mmio_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(get_system_జ్ఞాపకం(), ఎగువ_mmio_పునాది,
                                ఎగువ_mmio_alias);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 2, pio_పునాది);

    కొరకు (పూర్ణాంకం i = 0; i < PCI_అంకె_PINS; i++) {
        qemu_irq irq = qdev_get_gpio_లోపల(irqchip, pcie_irq + i);

        sysbus_connect_irq(SYS_BUS_DEVICE(dev), i, irq);
        gpex_సమితి_irq_అంకె(GPEX_HOST(dev), i, pcie_irq + i);
    }

    GPEX_HOST(dev)->gpex_cfg.bus = PCI_HOST_BRIDGE(dev)->bus;
    తిరిగి dev;
}
