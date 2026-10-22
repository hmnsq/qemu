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

#ضمن "qemu/osdep.h"
#ضمن "exec/target_page.h"
#ضمن "hw/pci/msi.h"
#ضمن "hw/pci/msix.h"
#ضمن "hw/pci/pci_bus.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "migration/vmstate.h"
#ضمن "qapi/error.h"
#ضمن "qemu/error-report.h"
#ضمن "qemu/host-utils.h"
#ضمن "qom/object.h"

#ضمن "cpu_bits.h"
#ضمن "riscv-iommu.h"
#ضمن "riscv-iommu-bits.h"
#ضمن "trace.h"

/* RISC-V IOMMU PCI Device Emulation */
#define RISCV_PCI_CLASS_SYSTEM_IOMMU     0x0806

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
#define RISCV_IOMMU_PCI_MSIX_VECTORS 5

/*
 * 4 vectors that can be used by civ, fiv, pmiv and piv. Number of
 * vectors is represented by 2^N, where N = number of writable bits
 * in each cause. For 4 vectors we'll write 0b11 (3) in each reg.
 */
#define RISCV_IOMMU_PCI_ICVEC_VECTORS 0x3333

تعريفنوع بنية RISCVIOMMUStatePci {
    PCIDevice        pci;     /* Parent PCIe device state */
    uint16_t         vendor_id;
    uint16_t         device_id;
    uint8_t          revision;
    MemoryRegion     bar0;    /* PCI BAR (including MSI-x config) */
    RISCVIOMMUState  iommu;   /* common IOMMU state */
} RISCVIOMMUStatePci;

/* interrupt delivery callback */
ساكن فراغ riscv_iommu_pci_notify(RISCVIOMMUState *iommu, unsigned vector)
{
    RISCVIOMMUStatePci *s = container_of(iommu, RISCVIOMMUStatePci, iommu);

    إذا (msix_enabled(&(s->pci))) {
        msix_notify(&(s->pci), vector);
    }
}

ساكن فراغ riscv_iommu_pci_realize(PCIDevice *dev, Error **errp)
{
    RISCVIOMMUStatePci *s = DO_UPCAST(RISCVIOMMUStatePci, pci, dev);
    RISCVIOMMUState *iommu = &s->iommu;
    uint8_t *pci_conf = dev->config;
    Error *err = عدم;

    pci_set_word(pci_conf + PCI_VENDOR_ID, s->vendor_id);
    pci_set_word(pci_conf + PCI_SUBSYSTEM_VENDOR_ID, s->vendor_id);
    pci_set_word(pci_conf + PCI_DEVICE_ID, s->device_id);
    pci_set_word(pci_conf + PCI_SUBSYSTEM_ID, s->device_id);
    pci_set_byte(pci_conf + PCI_REVISION_ID, s->revision);

    /* Set device id for trace / debug */
    DEVICE(iommu)->id = g_strdup_printf("%02x:%02x.%01x",
        pci_dev_bus_رقم(dev), PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn));
    qdev_realize(DEVICE(iommu), عدم, errp);

    memory_region_init(&s->bar0, OBJECT(s), "riscv-iommu-bar0",
        QEMU_ALIGN_UP(memory_region_size(&iommu->regs_mr), TARGET_PAGE_SIZE));
    memory_region_add_subregion(&s->bar0, 0, &iommu->regs_mr);

    pcie_endpoint_cap_init(dev, 0);

    pci_register_bar(dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY |
                     PCI_BASE_ADDRESS_MEM_TYPE_64, &s->bar0);

    صحيح ret = msix_init(dev, RISCV_IOMMU_PCI_MSIX_VECTORS,
                        &s->bar0, 0, RISCV_IOMMU_REG_MSI_CONFIG,
                        &s->bar0, 0, RISCV_IOMMU_REG_MSI_CONFIG + 256, 0, &err);

    إذا (ret == -ENOTSUP) {
        /*
         * MSI-x is not supported by the platform.
         * Driver should use timer/polling based notification handlers.
         */
        warn_report_err(err);
    } وإلا إذا (ret < 0) {
        error_propagate(errp, err);
        أرجع;
    } وإلا {
        /* Mark all ICVEC MSIx vectors as used */
        لكل (صحيح i = 0; i < RISCV_IOMMU_PCI_MSIX_VECTORS; i++) {
            msix_vector_use(dev, i);
        }

        iommu->notify = riscv_iommu_pci_notify;
    }

    PCIBus *bus = pci_device_جذر_bus(dev);
    إذا (!bus) {
        error_setg(errp, "can't find PCIe root port for %02x:%02x.%x",
            pci_bus_رقم(pci_get_bus(dev)), PCI_SLOT(dev->devfn),
            PCI_FUNC(dev->devfn));
        أرجع;
    }

    riscv_iommu_pci_setup_iommu(iommu, bus, errp);
}

ساكن فراغ riscv_iommu_pci_exit(PCIDevice *pci_dev)
{
    pci_setup_iommu(pci_device_جذر_bus(pci_dev), عدم, عدم);
}

ساكن ثابت VMStateDescription riscv_iommu_vmstate = {
    .name = "riscv-iommu",
    .unmigratable = 1
};

ساكن فراغ riscv_iommu_pci_init(Object *obj)
{
    RISCVIOMMUStatePci *s = RISCV_IOMMU_PCI(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_child(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_PCI_ICVEC_VECTORS;
    riscv_iommu_set_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_MSI);

    /* Report maximum physical address size of riscv64 */
    iommu->pas_bits = 56;
}

ساكن ثابت Property riscv_iommu_pci_properties[] = {
    DEFINE_PROP_UINT16("vendor-id", RISCVIOMMUStatePci, vendor_id,
                       PCI_VENDOR_ID_REDHAT),
    DEFINE_PROP_UINT16("device-id", RISCVIOMMUStatePci, device_id,
                       PCI_DEVICE_ID_REDHAT_RISCV_IOMMU),
    DEFINE_PROP_UINT8("revision", RISCVIOMMUStatePci, revision, 0x01),
};

ساكن فراغ riscv_iommu_pci_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStatePci *pci = RISCV_IOMMU_PCI(obj);
    RISCVIOMMUState *iommu = &pci->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_pci_reset_hold(type);
}

ساكن فراغ riscv_iommu_pci_class_init(ObjectClass *klass, ثابت فراغ *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_pci_reset_hold;

    k->realize = riscv_iommu_pci_realize;
    k->exit = riscv_iommu_pci_exit;
    k->class_id = RISCV_PCI_CLASS_SYSTEM_IOMMU;
    dc->desc = "RISCV-IOMMU DMA Remapping device";
    dc->vmsd = &riscv_iommu_vmstate;
    dc->hotpluggable = false;
    dc->user_creatable = true;
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_set_props(dc, riscv_iommu_pci_properties);
}

ساكن ثابت TypeInfo riscv_iommu_pci = {
    .name = TYPE_RISCV_IOMMU_PCI,
    .parent = TYPE_PCI_DEVICE,
    .class_init = riscv_iommu_pci_class_init,
    .instance_init = riscv_iommu_pci_init,
    .instance_size = حجم(RISCVIOMMUStatePci),
    .interfaces = (ثابت InterfaceInfo[]) {
        { INTERFACE_PCIE_DEVICE },
        { },
    },
};

ساكن فراغ riscv_iommu_register_pci_types(فراغ)
{
    type_register_static(&riscv_iommu_pci);
}

type_init(riscv_iommu_register_pci_types);
