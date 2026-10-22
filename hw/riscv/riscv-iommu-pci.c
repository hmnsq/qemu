/*
 * QEMU emulation of an RISC-V IOMMU
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
#ಅನ್ತರ್ಭಾವ "exec/target_page.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/msi.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/msix.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pci_bus.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "migration/vmstate.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qemu/host-utils.h"
#ಅನ್ತರ್ಭಾವ "qom/object.h"

#ಅನ್ತರ್ಭಾವ "cpu_bits.h"
#ಅನ್ತರ್ಭಾವ "riscv-iommu.h"
#ಅನ್ತರ್ಭಾವ "riscv-iommu-bits.h"
#ಅನ್ತರ್ಭಾವ "trace.h"

/* RISC-V IOMMU PCI Device Emulation */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_PCI_CLASS_SYSTEM_IOMMU     0x0806

/*
 * 4 MSIx vectors for ICVEC, one for MRIF. The spec mentions in
 * the "Placement and data flow" section that:
 *
 * "The interfaces related to recording an incoming MSI in a memory-resident
 * interrupt file (MRIF) are implementation-specific. The partitioning of
 * responsibility between the IOMMU and the IO bridge for recording the
 * incoming MSI in an MRIF and generating the associated notice MSI are
 * implementation-specific."
 *
 * We're making a design decision to create the MSIx for MRIF in the
 * IOMMU MSIx emulation.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PCI_MSIX_VECTORS 5

/*
 * 4 vectors that can be used by civ, fiv, pmiv and piv. Number of
 * vectors is represented by 2^N, where N = number of writable bits
 * in each cause. For 4 vectors we'll write 0b11 (3) in each reg.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PCI_ICVEC_VECTORS 0x3333

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ RISCVIOMMUStatePci {
    PCIDevice        pci;     /* Parent PCIe device state */
    uint16_t         vendor_id;
    uint16_t         device_id;
    uint8_t          revision;
    ಸ್ಮರಣೆ_Region     bar0;    /* PCI BAR (including MSI-x config) */
    RISCVIOMMUState  iommu;   /* common IOMMU state */
} RISCVIOMMUStatePci;

/* interrupt delivery callback */
ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_pci_notify(RISCVIOMMUState *iommu, ಚಿಹ್ನೆರಹಿತ ಸದಿಶ)
{
    RISCVIOMMUStatePci *s = container_ದ(iommu, RISCVIOMMUStatePci, iommu);

    ಆದರೆ (msix_enabled(&(s->pci))) {
        msix_notify(&(s->pci), ಸದಿಶ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_pci_realize(PCIDevice *dev, Error **errp)
{
    RISCVIOMMUStatePci *s = DO_UPCAST(RISCVIOMMUStatePci, pci, dev);
    RISCVIOMMUState *iommu = &s->iommu;
    uint8_t *pci_conf = dev->config;
    Error *err = ಖಾಲಿ;

    pci_ಗಣ_ಶಬ್ದ(pci_conf + PCI_VENDOR_ID, s->vendor_id);
    pci_ಗಣ_ಶಬ್ದ(pci_conf + PCI_SUBSYSTEM_VENDOR_ID, s->vendor_id);
    pci_ಗಣ_ಶಬ್ದ(pci_conf + PCI_DEVICE_ID, s->device_id);
    pci_ಗಣ_ಶಬ್ದ(pci_conf + PCI_SUBSYSTEM_ID, s->device_id);
    pci_ಗಣ_byte(pci_conf + PCI_REVISION_ID, s->revision);

    /* Set device id for trace / debug */
    DEVICE(iommu)->id = g_strdup_printf("%02x:%02x.%01x",
        pci_dev_bus_ಅಂಕೆ(dev), PCI_ಸ್ಥಾನ(dev->devfn), PCI_FUNC(dev->devfn));
    qdev_realize(DEVICE(iommu), ಖಾಲಿ, errp);

    ಸ್ಮರಣೆ_region_init(&s->bar0, OBJECT(s), "riscv-iommu-bar0",
        QEMU_ALIGN_ಮೇಲೆ(ಸ್ಮರಣೆ_region_ಗಾತ್ರ(&iommu->regs_mr), ಗುರಿ_ಪುಟ_ಗಾತ್ರ));
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(&s->bar0, 0, &iommu->regs_mr);

    pcie_endpoint_cap_init(dev, 0);

    pci_ರಿಜಿಸ್ಟರ್_bar(dev, 0, PCI_ತಳ_ವಿಳಾಸ_SPACE_ಸ್ಮರಣೆ |
                     PCI_ತಳ_ವಿಳಾಸ_MEM_TYPE_64, &s->bar0);

    ಪೂರ್ಣಾಂಕ ret = msix_init(dev, RISCV_IOMMU_PCI_MSIX_VECTORS,
                        &s->bar0, 0, RISCV_IOMMU_REG_MSI_CONFIG,
                        &s->bar0, 0, RISCV_IOMMU_REG_MSI_CONFIG + 256, 0, &err);

    ಆದರೆ (ret == -ENOTSUP) {
        /*
         * MSI-x is not supported by the platform.
         * Driver should use timer/polling based notification handlers.
         */
        warn_report_err(err);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret < 0) {
        error_propagate(errp, err);
        ಹಿಂತಿರುಗಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Mark all ICVEC MSIx vectors as used */
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < RISCV_IOMMU_PCI_MSIX_VECTORS; i++) {
            msix_ಸದಿಶ_use(dev, i);
        }

        iommu->notify = riscv_iommu_pci_notify;
    }

    PCIBus *bus = pci_device_ಬೇರು_bus(dev);
    ಆದರೆ (!bus) {
        error_setg(errp, "can't find PCIe root port for %02x:%02x.%x",
            pci_bus_ಅಂಕೆ(pci_get_bus(dev)), PCI_ಸ್ಥಾನ(dev->devfn),
            PCI_FUNC(dev->devfn));
        ಹಿಂತಿರುಗಿ;
    }

    riscv_iommu_pci_ಸಿದ್ಧತೆ_iommu(iommu, bus, errp);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_pci_exit(PCIDevice *pci_dev)
{
    pci_ಸಿದ್ಧತೆ_iommu(pci_device_ಬೇರು_bus(pci_dev), ಖಾಲಿ, ಖಾಲಿ);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ VMStateDescription riscv_iommu_vmstate = {
    .name = "riscv-iommu",
    .unmigratable = 1
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_pci_init(Object *obj)
{
    RISCVIOMMUStatePci *s = RISCV_IOMMU_PCI(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_ಮಗು(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_PCI_ICVEC_VECTORS;
    riscv_iommu_ಗಣ_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_MSI);

    /* Report maximum physical address size of riscv64 */
    iommu->pas_ಬಿಟ್ಟುಗಳು = 56;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Property riscv_iommu_pci_properties[] = {
    DEFINE_PROP_UINT16("vendor-id", RISCVIOMMUStatePci, vendor_id,
                       PCI_VENDOR_ID_REDHAT),
    DEFINE_PROP_UINT16("device-id", RISCVIOMMUStatePci, device_id,
                       PCI_DEVICE_ID_REDHAT_RISCV_IOMMU),
    DEFINE_PROP_UINT8("revision", RISCVIOMMUStatePci, revision, 0x01),
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_pci_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStatePci *pci = RISCV_IOMMU_PCI(obj);
    RISCVIOMMUState *iommu = &pci->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_pci_reset_hold(type);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_pci_class_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_pci_reset_hold;

    k->realize = riscv_iommu_pci_realize;
    k->ನಿರ್ಗಮಿಸು = riscv_iommu_pci_exit;
    k->class_id = RISCV_PCI_CLASS_SYSTEM_IOMMU;
    dc->desc = "RISCV-IOMMU DMA Remapping device";
    dc->vmsd = &riscv_iommu_vmstate;
    dc->hotpluggable = false;
    dc->user_creatable = true;
    ಗಣ_ಬಿಟ್(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_ಗಣ_props(dc, riscv_iommu_pci_properties);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo riscv_iommu_pci = {
    .name = TYPE_RISCV_IOMMU_PCI,
    .ಪೋಷಕ = TYPE_PCI_DEVICE,
    .class_init = riscv_iommu_pci_class_init,
    .instance_init = riscv_iommu_pci_init,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(RISCVIOMMUStatePci),
    .interfaces = (ಸ್ಥಿರಾಂಕ InterfaceInfo[]) {
        { INTERFACE_PCIE_DEVICE },
        { },
    },
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ರಿಜಿಸ್ಟರ್_pci_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&riscv_iommu_pci);
}

type_init(riscv_iommu_ರಿಜಿಸ್ಟರ್_pci_types);
