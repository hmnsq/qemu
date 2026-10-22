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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "exec/target_page.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/msi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/msix.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pci_bus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv_hart.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "migration/vmstate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/host-utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qom/object.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cpu_bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu-bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "trace.h"

/* RISC-V IOMMU PCI Device Emulation */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_PCI_CLASS_SYSTEM_IOMMU     0x0806

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PCI_MSIX_VECTORS 5

/*
 * 4 vectors that can be used by civ, fiv, pmiv and piv. Number of
 * vectors is represented by 2^N, where N = number of writable bits
 * in each cause. For 4 vectors we'll write 0b11 (3) in each reg.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PCI_ICVEC_VECTORS 0x3333

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ RISCVIOMMUStatePci {
    PCIDevice        pci;     /* Parent PCIe device state */
    uint16_t         vendor_id;
    uint16_t         device_id;
    uint8_t          revision;
    ਸਮ੍ਰਿਤੀ_Region     bar0;    /* PCI BAR (including MSI-x config) */
    RISCVIOMMUState  iommu;   /* common IOMMU state */
} RISCVIOMMUStatePci;

/* interrupt delivery callback */
ਸਥਿਰ ਖਾਲੀ riscv_iommu_pci_notify(RISCVIOMMUState *iommu, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਦਿਸ਼)
{
    RISCVIOMMUStatePci *s = container_ਦਾ(iommu, RISCVIOMMUStatePci, iommu);

    ਜੇ (msix_enabled(&(s->pci))) {
        msix_notify(&(s->pci), ਸਦਿਸ਼);
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_pci_realize(PCIDevice *dev, Error **errp)
{
    RISCVIOMMUStatePci *s = DO_UPCAST(RISCVIOMMUStatePci, pci, dev);
    RISCVIOMMUState *iommu = &s->iommu;
    uint8_t *pci_conf = dev->config;
    Error *err = ਨਿਰਾ;

    pci_ਸੈੱਟ_ਸ਼ਬਦ(pci_conf + PCI_VENDOR_ID, s->vendor_id);
    pci_ਸੈੱਟ_ਸ਼ਬਦ(pci_conf + PCI_SUBSYSTEM_VENDOR_ID, s->vendor_id);
    pci_ਸੈੱਟ_ਸ਼ਬਦ(pci_conf + PCI_DEVICE_ID, s->device_id);
    pci_ਸੈੱਟ_ਸ਼ਬਦ(pci_conf + PCI_SUBSYSTEM_ID, s->device_id);
    pci_ਸੈੱਟ_byte(pci_conf + PCI_REVISION_ID, s->revision);

    /* Set device id for trace / debug */
    DEVICE(iommu)->id = g_strdup_printf("%02x:%02x.%01x",
        pci_dev_bus_ਅੰਕ(dev), PCI_ਥਾਂ(dev->devfn), PCI_FUNC(dev->devfn));
    qdev_realize(DEVICE(iommu), ਨਿਰਾ, errp);

    ਸਮ੍ਰਿਤੀ_region_init(&s->bar0, OBJECT(s), "riscv-iommu-bar0",
        QEMU_ALIGN_ਉੱਪਰ(ਸਮ੍ਰਿਤੀ_region_ਮਾਪ(&iommu->regs_mr), ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਮਾਪ));
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(&s->bar0, 0, &iommu->regs_mr);

    pcie_endpoint_cap_init(dev, 0);

    pci_ਰਜਿਸਟਰ_bar(dev, 0, PCI_ਅਧਾਰ_ਪਤਾ_SPACE_ਸਮ੍ਰਿਤੀ |
                     PCI_ਅਧਾਰ_ਪਤਾ_MEM_TYPE_64, &s->bar0);

    ਪੂਰਨ ret = msix_init(dev, RISCV_IOMMU_PCI_MSIX_VECTORS,
                        &s->bar0, 0, RISCV_IOMMU_REG_MSI_CONFIG,
                        &s->bar0, 0, RISCV_IOMMU_REG_MSI_CONFIG + 256, 0, &err);

    ਜੇ (ret == -ENOTSUP) {
        /*
         * MSI-x is not supported by the platform.
         * Driver should use timer/polling based notification handlers.
         */
        warn_report_err(err);
    } ਨਹੀਂਤਾਂ ਜੇ (ret < 0) {
        error_propagate(errp, err);
        ਵਾਪਸ;
    } ਨਹੀਂਤਾਂ {
        /* Mark all ICVEC MSIx vectors as used */
        ਲਈ (ਪੂਰਨ i = 0; i < RISCV_IOMMU_PCI_MSIX_VECTORS; i++) {
            msix_ਸਦਿਸ਼_use(dev, i);
        }

        iommu->notify = riscv_iommu_pci_notify;
    }

    PCIBus *bus = pci_device_ਜੜ੍ਹ_bus(dev);
    ਜੇ (!bus) {
        error_setg(errp, "can't find PCIe root port for %02x:%02x.%x",
            pci_bus_ਅੰਕ(pci_get_bus(dev)), PCI_ਥਾਂ(dev->devfn),
            PCI_FUNC(dev->devfn));
        ਵਾਪਸ;
    }

    riscv_iommu_pci_ਤਿਆਰੀ_iommu(iommu, bus, errp);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_pci_exit(PCIDevice *pci_dev)
{
    pci_ਤਿਆਰੀ_iommu(pci_device_ਜੜ੍ਹ_bus(pci_dev), ਨਿਰਾ, ਨਿਰਾ);
}

ਸਥਿਰ ਅਚਲ VMStateDescription riscv_iommu_vmstate = {
    .name = "riscv-iommu",
    .unmigratable = 1
};

ਸਥਿਰ ਖਾਲੀ riscv_iommu_pci_init(Object *obj)
{
    RISCVIOMMUStatePci *s = RISCV_IOMMU_PCI(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_ਬੱਚਾ(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_PCI_ICVEC_VECTORS;
    riscv_iommu_ਸੈੱਟ_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_MSI);

    /* Report maximum physical address size of riscv64 */
    iommu->pas_ਬਿੱਟ = 56;
}

ਸਥਿਰ ਅਚਲ Property riscv_iommu_pci_properties[] = {
    DEFINE_PROP_UINT16("vendor-id", RISCVIOMMUStatePci, vendor_id,
                       PCI_VENDOR_ID_REDHAT),
    DEFINE_PROP_UINT16("device-id", RISCVIOMMUStatePci, device_id,
                       PCI_DEVICE_ID_REDHAT_RISCV_IOMMU),
    DEFINE_PROP_UINT8("revision", RISCVIOMMUStatePci, revision, 0x01),
};

ਸਥਿਰ ਖਾਲੀ riscv_iommu_pci_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStatePci *pci = RISCV_IOMMU_PCI(obj);
    RISCVIOMMUState *iommu = &pci->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_pci_reset_hold(type);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_pci_class_init(ObjectClass *klass, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_pci_reset_hold;

    k->realize = riscv_iommu_pci_realize;
    k->ਬਾਹਰ_ਨਿਕਲੋ = riscv_iommu_pci_exit;
    k->class_id = RISCV_PCI_CLASS_SYSTEM_IOMMU;
    dc->desc = "RISCV-IOMMU DMA Remapping device";
    dc->vmsd = &riscv_iommu_vmstate;
    dc->hotpluggable = false;
    dc->user_creatable = true;
    ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_ਸੈੱਟ_props(dc, riscv_iommu_pci_properties);
}

ਸਥਿਰ ਅਚਲ TypeInfo riscv_iommu_pci = {
    .name = TYPE_RISCV_IOMMU_PCI,
    .ਮਾਪਾ = TYPE_PCI_DEVICE,
    .class_init = riscv_iommu_pci_class_init,
    .instance_init = riscv_iommu_pci_init,
    .instance_ਮਾਪ = ਆਕਾਰ(RISCVIOMMUStatePci),
    .interfaces = (ਅਚਲ InterfaceInfo[]) {
        { INTERFACE_PCIE_DEVICE },
        { },
    },
};

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ਰਜਿਸਟਰ_pci_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&riscv_iommu_pci);
}

type_init(riscv_iommu_ਰਜਿਸਟਰ_pci_types);
