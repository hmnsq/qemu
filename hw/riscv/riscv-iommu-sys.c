/*
 * QEMU emulation of an RISC-V IOMMU Platform Device
 *
 * Copyright (C) 2022-2023 Rivos Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "hw/core/irq.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pci_bus.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qemu/host-utils.h"
#ಅನ್ತರ್ಭಾವ "qemu/module.h"
#ಅನ್ತರ್ಭಾವ "qom/object.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu_bits.h"
#ಅನ್ತರ್ಭಾವ "trace.h"

#ಅನ್ತರ್ಭಾವ "riscv-iommu.h"

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_SYSDEV_ICVEC_VECTORS 0x3333

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PCI_MSIX_VECTORS 5

/* RISC-V IOMMU System Platform Device Emulation */

ರಚನೆ RISCVIOMMUStateSys {
    SysBusDevice     ಪೋಷಕ;
    uint64_t         addr;
    uint32_t         ತಳ_irq;
    DeviceState      *irqchip;
    RISCVIOMMUState  iommu;

    /* Wired int support */
    qemu_irq         irqs[RISCV_IOMMU_INTR_ಎಣಿಕೆ];

    /* Memory Regions for MSIX table and pending bit entries. */
    ಸ್ಮರಣೆ_Region msix_ಕೋಷ್ಟಕ_mmio;
    ಸ್ಮರಣೆ_Region msix_pba_mmio;
    uint8_t *msix_ಕೋಷ್ಟಕ;
    uint8_t *msix_pba;
};

ಸ್ಥಿರ uint64_t msix_ಕೋಷ್ಟಕ_mmio_read(ಶೂನ್ಯ *opaque, hwaddr addr,
                                     ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + ಗಾತ್ರ_2 <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_ನಮೂದು_ಗಾತ್ರ);
    ಹಿಂತಿರುಗಿ pci_get_long(s->msix_ಕೋಷ್ಟಕ + addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ msix_ಕೋಷ್ಟಕ_mmio_write(ಶೂನ್ಯ *opaque, hwaddr addr,
                                  uint64_t val, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + ಗಾತ್ರ_2 <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_ನಮೂದು_ಗಾತ್ರ);
    pci_ಗಣ_long(s->msix_ಕೋಷ್ಟಕ + addr, val);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಸ್ಮರಣೆ_Region_Ops msix_ಕೋಷ್ಟಕ_mmio_ops = {
    .ಪಠ = msix_ಕೋಷ್ಟಕ_mmio_read,
    .ಲಿಖ = msix_ಕೋಷ್ಟಕ_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_ಗಾತ್ರ = 4,
        .max_access_ಗಾತ್ರ = 8,
    },
    .impl = {
        .max_access_ಗಾತ್ರ = 4,
    },
};

ಸ್ಥಿರ uint64_t msix_pba_mmio_read(ಶೂನ್ಯ *opaque, hwaddr addr,
                                   ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2)
{
    RISCVIOMMUStateSys *s = opaque;

    ಹಿಂತಿರುಗಿ pci_get_long(s->msix_pba + addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ msix_pba_mmio_write(ಶೂನ್ಯ *opaque, hwaddr addr,
                                uint64_t val, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2)
{
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಸ್ಮರಣೆ_Region_Ops msix_pba_mmio_ops = {
    .ಪಠ = msix_pba_mmio_read,
    .ಲಿಖ = msix_pba_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_ಗಾತ್ರ = 4,
        .max_access_ಗಾತ್ರ = 8,
    },
    .impl = {
        .max_access_ಗಾತ್ರ = 4,
    },
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_sysdev_init_msi(RISCVIOMMUStateSys *s,
                                        uint32_t n_vectors)
{
    RISCVIOMMUState *iommu = &s->iommu;
    uint32_t ಕೋಷ್ಟಕ_ಗಾತ್ರ = n_vectors * PCI_MSIX_ನಮೂದು_ಗಾತ್ರ;
    uint32_t ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ = RISCV_IOMMU_REG_MSI_CONFIG;
    uint32_t pba_ಗಾತ್ರ = QEMU_ALIGN_ಮೇಲೆ(n_vectors, 64) / 8;
    uint32_t pba_ಸ್ಥಾನಾಂತರ = RISCV_IOMMU_REG_MSI_CONFIG + 256;

    s->msix_ಕೋಷ್ಟಕ = g_malloc0(ಕೋಷ್ಟಕ_ಗಾತ್ರ);
    s->msix_pba = g_malloc0(pba_ಗಾತ್ರ);

    ಸ್ಮರಣೆ_region_init_io(&s->msix_ಕೋಷ್ಟಕ_mmio, OBJECT(s), &msix_ಕೋಷ್ಟಕ_mmio_ops,
                          s, "msix-table", ಕೋಷ್ಟಕ_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(&iommu->regs_mr, ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ,
                                &s->msix_ಕೋಷ್ಟಕ_mmio);

    ಸ್ಮರಣೆ_region_init_io(&s->msix_pba_mmio, OBJECT(s), &msix_pba_mmio_ops, s,
                          "msix-pba", pba_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(&iommu->regs_mr, pba_ಸ್ಥಾನಾಂತರ,
                                &s->msix_pba_mmio);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_sysdev_send_MSI(RISCVIOMMUStateSys *s,
                                        uint32_t ಸದಿಶ)
{
    uint8_t *ಕೋಷ್ಟಕ_ನಮೂದು = s->msix_ಕೋಷ್ಟಕ + ಸದಿಶ * PCI_MSIX_ನಮೂದು_ಗಾತ್ರ;
    uint64_t msi_addr = pci_get_quad(ಕೋಷ್ಟಕ_ನಮೂದು + PCI_MSIX_ನಮೂದು_ಅಧೋ_ADDR);
    uint32_t msi_ದತ್ತಾಂಶ = pci_get_long(ಕೋಷ್ಟಕ_ನಮೂದು + PCI_MSIX_ನಮೂದು_ದತ್ತಾಂಶ);
    Mem_Tx_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ;

    ವಿಳಾಸ_space_stl_le(&ವಿಳಾಸ_space_ಸ್ಮರಣೆ, msi_addr,
                         msi_ದತ್ತಾಂಶ, MEMTXATTRS_UNSPECIFIED, &ಫಲಿತಾಂಶ);

    ಆದರೆ (ಫಲಿತಾಂಶ == MEMTX_OK) {
        trace_riscv_iommu_sys_msi_ಕಳುಹಿಸಿದ(ಸದಿಶ, msi_addr, msi_ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Record an access fault error in the fault queue */
        ರಚನೆ riscv_iommu_fq_ದಾಖಲೆ ev = { 0 };
        RISCVIOMMUState *iommu = &s->iommu;

        ev.hdr = ಗಣ_ಕ್ಷೇತ್ರ(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE,
                           RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT);
        ev.hdr = ಗಣ_ಕ್ಷೇತ್ರ(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE,
                           RISCV_IOMMU_FQ_TTYPE_UADDR_WR);
        riscv_iommu_fault(iommu, &ev);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_sysdev_notify(RISCVIOMMUState *iommu,
                                      ಚಿಹ್ನೆರಹಿತ ಸದಿಶ)
{
    RISCVIOMMUStateSys *s = container_ದ(iommu, RISCVIOMMUStateSys, iommu);
    uint32_t fctl =  riscv_iommu_reg_get32(iommu, RISCV_IOMMU_REG_FCTL);

    ಆದರೆ (fctl & RISCV_IOMMU_FCTL_WSI) {
        qemu_irq_pulse(s->irqs[ಸದಿಶ]);
        trace_riscv_iommu_sys_irq_ಕಳುಹಿಸಿದ(ಸದಿಶ);
        ಹಿಂತಿರುಗಿ;
    }

    riscv_iommu_sysdev_send_MSI(s, ಸದಿಶ);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_sys_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(dev);
    SysBusDevice *sysdev = SYS_BUS_DEVICE(s);
    PCIBus *pci_bus;
    qemu_irq irq;

    qdev_realize(DEVICE(&s->iommu), ಖಾಲಿ, errp);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iommu.regs_mr);
    ಆದರೆ (s->addr) {
        sysbus_mmio_map(SYS_BUS_DEVICE(s), 0, s->addr);
    }

    pci_bus = (PCIBus *) object_resolve_ಮಾರ್ಗ_type("", TYPE_PCI_BUS, ಖಾಲಿ);
    ಆದರೆ (pci_bus) {
        riscv_iommu_pci_ಸಿದ್ಧತೆ_iommu(&s->iommu, pci_bus, errp);
    }

    s->iommu.notify = riscv_iommu_sysdev_notify;

    /* 4 IRQs are defined starting from s->base_irq */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < RISCV_IOMMU_INTR_ಎಣಿಕೆ; i++) {
        sysbus_init_irq(sysdev, &s->irqs[i]);
        irq = qdev_get_gpio_ಒಳ(s->irqchip, s->ತಳ_irq + i);
        sysbus_connect_irq(sysdev, i, irq);
    }

    riscv_iommu_sysdev_init_msi(s, RISCV_IOMMU_PCI_MSIX_VECTORS);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_sys_init(Object *obj)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_ಮಗು(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_SYSDEV_ICVEC_VECTORS;
    riscv_iommu_ಗಣ_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_BOTH);
}

DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, ಪೂರ್ಣಾಂಕ ತಳ_irq,
                                    bool is_32_ಬಿಟ್)
{
    DeviceState *iommu_sys = qdev_new(TYPE_RISCV_IOMMU_SYS);

    object_property_ಗಣ_uint(OBJECT(iommu_sys), "addr", addr, &error_fatal);
    object_property_ಗಣ_uint(OBJECT(iommu_sys), "base-irq",
                             ತಳ_irq, &error_fatal);
    object_property_ಗಣ_link(OBJECT(iommu_sys), "irqchip",
                             OBJECT(mmio_irqchip),
                             &error_fatal);
    /*
     * For riscv64 use a physical address size of 56 bits (44 bit PPN),
     * and for riscv32 use 34 bits (22 bit PPN).
     */
    object_property_ಗಣ_uint(OBJECT(iommu_sys), "pas-bits",
                             is_32_ಬಿಟ್ ? 34 : 56,
                             &error_fatal);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(iommu_sys), &error_fatal);

    ಹಿಂತಿರುಗಿ iommu_sys;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Property riscv_iommu_sys_properties[] = {
    DEFINE_PROP_UINT64("addr", RISCVIOMMUStateSys, addr, 0),
    DEFINE_PROP_UINT32("base-irq", RISCVIOMMUStateSys, ತಳ_irq, 0),
    DEFINE_PROP_LINK("irqchip", RISCVIOMMUStateSys, irqchip,
                     TYPE_DEVICE, DeviceState *),
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_sys_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStateSys *sys = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &sys->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_sys_reset_hold(type);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_sys_class_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_sys_reset_hold;

    dc->realize = riscv_iommu_sys_realize;
    ಗಣ_ಬಿಟ್(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_ಗಣ_props(dc, riscv_iommu_sys_properties);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo riscv_iommu_sys = {
    .name          = TYPE_RISCV_IOMMU_SYS,
    .ಪೋಷಕ        = TYPE_SYS_BUS_DEVICE,
    .class_init    = riscv_iommu_sys_class_init,
    .instance_init = riscv_iommu_sys_init,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(RISCVIOMMUStateSys),
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ರಿಜಿಸ್ಟರ್_sys(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&riscv_iommu_sys);
}

type_init(riscv_iommu_ರಿಜಿಸ್ಟರ್_sys)
