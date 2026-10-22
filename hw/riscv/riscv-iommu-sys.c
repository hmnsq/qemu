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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "hw/core/irq.h"
#अन्तर्भाव "hw/pci/pci_bus.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "hw/core/sysbus.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "qemu/host-utils.h"
#अन्तर्भाव "qemu/module.h"
#अन्तर्भाव "qom/object.h"
#अन्तर्भाव "target/riscv/cpu_bits.h"
#अन्तर्भाव "trace.h"

#अन्तर्भाव "riscv-iommu.h"

#परिभाषय RISCV_IOMMU_SYSDEV_ICVEC_VECTORS 0x3333

#परिभाषय RISCV_IOMMU_PCI_MSIX_VECTORS 5

/* RISC-V IOMMU System Platform Device Emulation */

संरचना RISCVIOMMUStateSys {
    SysBusDevice     parent;
    uint64_t         addr;
    uint32_t         आधारः_irq;
    DeviceState      *irqchip;
    RISCVIOMMUState  iommu;

    /* Wired int support */
    qemu_irq         irqs[RISCV_IOMMU_INTR_गणनम्];

    /* Memory Regions for MSIX table and pending bit entries. */
    स्मृतिः_Region msix_सारणी_mmio;
    स्मृतिः_Region msix_pba_mmio;
    uint8_t *msix_सारणी;
    uint8_t *msix_pba;
};

स्थिर uint64_t msix_सारणी_mmio_read(शून्य *opaque, hwaddr addr,
                                     अचिह्नित आकार)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + आकार <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_प्रविष्टिः_आकार);
    प्रत्यागम pci_get_long(s->msix_सारणी + addr);
}

स्थिर शून्य msix_सारणी_mmio_write(शून्य *opaque, hwaddr addr,
                                  uint64_t val, अचिह्नित आकार)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + आकार <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_प्रविष्टिः_आकार);
    pci_समुच्चयः_long(s->msix_सारणी + addr, val);
}

स्थिर ध्रुव स्मृतिः_Region_Ops msix_सारणी_mmio_ops = {
    .पठ = msix_सारणी_mmio_read,
    .लिख = msix_सारणी_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_आकार = 4,
        .max_access_आकार = 8,
    },
    .impl = {
        .max_access_आकार = 4,
    },
};

स्थिर uint64_t msix_pba_mmio_read(शून्य *opaque, hwaddr addr,
                                   अचिह्नित आकार)
{
    RISCVIOMMUStateSys *s = opaque;

    प्रत्यागम pci_get_long(s->msix_pba + addr);
}

स्थिर शून्य msix_pba_mmio_write(शून्य *opaque, hwaddr addr,
                                uint64_t val, अचिह्नित आकार)
{
}

स्थिर ध्रुव स्मृतिः_Region_Ops msix_pba_mmio_ops = {
    .पठ = msix_pba_mmio_read,
    .लिख = msix_pba_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_आकार = 4,
        .max_access_आकार = 8,
    },
    .impl = {
        .max_access_आकार = 4,
    },
};

स्थिर शून्य riscv_iommu_sysdev_init_msi(RISCVIOMMUStateSys *s,
                                        uint32_t n_vectors)
{
    RISCVIOMMUState *iommu = &s->iommu;
    uint32_t सारणी_आकार = n_vectors * PCI_MSIX_प्रविष्टिः_आकार;
    uint32_t सारणी_विस्थापनम् = RISCV_IOMMU_REG_MSI_CONFIG;
    uint32_t pba_आकार = QEMU_ALIGN_ऊर्ध्वम्(n_vectors, 64) / 8;
    uint32_t pba_विस्थापनम् = RISCV_IOMMU_REG_MSI_CONFIG + 256;

    s->msix_सारणी = g_malloc0(सारणी_आकार);
    s->msix_pba = g_malloc0(pba_आकार);

    स्मृतिः_region_init_io(&s->msix_सारणी_mmio, OBJECT(s), &msix_सारणी_mmio_ops,
                          s, "msix-table", सारणी_आकार);
    स्मृतिः_region_सङ्कलय_subregion(&iommu->regs_mr, सारणी_विस्थापनम्,
                                &s->msix_सारणी_mmio);

    स्मृतिः_region_init_io(&s->msix_pba_mmio, OBJECT(s), &msix_pba_mmio_ops, s,
                          "msix-pba", pba_आकार);
    स्मृतिः_region_सङ्कलय_subregion(&iommu->regs_mr, pba_विस्थापनम्,
                                &s->msix_pba_mmio);
}

स्थिर शून्य riscv_iommu_sysdev_send_MSI(RISCVIOMMUStateSys *s,
                                        uint32_t सदिशम्)
{
    uint8_t *सारणी_प्रविष्टिः = s->msix_सारणी + सदिशम् * PCI_MSIX_प्रविष्टिः_आकार;
    uint64_t msi_addr = pci_get_quad(सारणी_प्रविष्टिः + PCI_MSIX_प्रविष्टिः_अधस्तनम्_ADDR);
    uint32_t msi_दत्त = pci_get_long(सारणी_प्रविष्टिः + PCI_MSIX_प्रविष्टिः_दत्त);
    Mem_Tx_परिणाम परिणाम;

    स्थानाङ्कः_space_stl_le(&स्थानाङ्कः_space_स्मृतिः, msi_addr,
                         msi_दत्त, MEMTXATTRS_UNSPECIFIED, &परिणाम);

    यदि (परिणाम == MEMTX_OK) {
        trace_riscv_iommu_sys_msi_प्रेषितम्(सदिशम्, msi_addr, msi_दत्त, परिणाम);
    } अन्यथा {
        /* Record an access fault error in the fault queue */
        संरचना riscv_iommu_fq_लेखः ev = { 0 };
        RISCVIOMMUState *iommu = &s->iommu;

        ev.hdr = समुच्चयः_क्षेत्रम्(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE,
                           RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT);
        ev.hdr = समुच्चयः_क्षेत्रम्(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE,
                           RISCV_IOMMU_FQ_TTYPE_UADDR_WR);
        riscv_iommu_fault(iommu, &ev);
    }
}

स्थिर शून्य riscv_iommu_sysdev_notify(RISCVIOMMUState *iommu,
                                      अचिह्नित सदिशम्)
{
    RISCVIOMMUStateSys *s = container_सम्बन्धि(iommu, RISCVIOMMUStateSys, iommu);
    uint32_t fctl =  riscv_iommu_reg_get32(iommu, RISCV_IOMMU_REG_FCTL);

    यदि (fctl & RISCV_IOMMU_FCTL_WSI) {
        qemu_irq_pulse(s->irqs[सदिशम्]);
        trace_riscv_iommu_sys_irq_प्रेषितम्(सदिशम्);
        प्रत्यागम;
    }

    riscv_iommu_sysdev_send_MSI(s, सदिशम्);
}

स्थिर शून्य riscv_iommu_sys_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(dev);
    SysBusDevice *sysdev = SYS_BUS_DEVICE(s);
    PCIBus *pci_bus;
    qemu_irq irq;

    qdev_realize(DEVICE(&s->iommu), रिक्त, errp);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iommu.regs_mr);
    यदि (s->addr) {
        sysbus_mmio_map(SYS_BUS_DEVICE(s), 0, s->addr);
    }

    pci_bus = (PCIBus *) object_resolve_मार्गः_type("", TYPE_PCI_BUS, रिक्त);
    यदि (pci_bus) {
        riscv_iommu_pci_सज्जता_iommu(&s->iommu, pci_bus, errp);
    }

    s->iommu.notify = riscv_iommu_sysdev_notify;

    /* 4 IRQs are defined starting from s->base_irq */
    प्रति (पूर्णाङ्क i = 0; i < RISCV_IOMMU_INTR_गणनम्; i++) {
        sysbus_init_irq(sysdev, &s->irqs[i]);
        irq = qdev_get_gpio_अन्तर्(s->irqchip, s->आधारः_irq + i);
        sysbus_connect_irq(sysdev, i, irq);
    }

    riscv_iommu_sysdev_init_msi(s, RISCV_IOMMU_PCI_MSIX_VECTORS);
}

स्थिर शून्य riscv_iommu_sys_init(Object *obj)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_child(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_SYSDEV_ICVEC_VECTORS;
    riscv_iommu_समुच्चयः_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_BOTH);
}

DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, पूर्णाङ्क आधारः_irq,
                                    bool is_32_bit)
{
    DeviceState *iommu_sys = qdev_new(TYPE_RISCV_IOMMU_SYS);

    object_property_समुच्चयः_uint(OBJECT(iommu_sys), "addr", addr, &error_fatal);
    object_property_समुच्चयः_uint(OBJECT(iommu_sys), "base-irq",
                             आधारः_irq, &error_fatal);
    object_property_समुच्चयः_link(OBJECT(iommu_sys), "irqchip",
                             OBJECT(mmio_irqchip),
                             &error_fatal);
    /*
     * For riscv64 use a physical address size of 56 bits (44 bit PPN),
     * and for riscv32 use 34 bits (22 bit PPN).
     */
    object_property_समुच्चयः_uint(OBJECT(iommu_sys), "pas-bits",
                             is_32_bit ? 34 : 56,
                             &error_fatal);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(iommu_sys), &error_fatal);

    प्रत्यागम iommu_sys;
}

स्थिर ध्रुव Property riscv_iommu_sys_properties[] = {
    DEFINE_PROP_UINT64("addr", RISCVIOMMUStateSys, addr, 0),
    DEFINE_PROP_UINT32("base-irq", RISCVIOMMUStateSys, आधारः_irq, 0),
    DEFINE_PROP_LINK("irqchip", RISCVIOMMUStateSys, irqchip,
                     TYPE_DEVICE, DeviceState *),
};

स्थिर शून्य riscv_iommu_sys_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStateSys *sys = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &sys->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_sys_reset_hold(type);
}

स्थिर शून्य riscv_iommu_sys_class_init(ObjectClass *klass, ध्रुव शून्य *दत्त)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_sys_reset_hold;

    dc->realize = riscv_iommu_sys_realize;
    समुच्चयः_bit(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_समुच्चयः_props(dc, riscv_iommu_sys_properties);
}

स्थिर ध्रुव TypeInfo riscv_iommu_sys = {
    .name          = TYPE_RISCV_IOMMU_SYS,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .class_init    = riscv_iommu_sys_class_init,
    .instance_init = riscv_iommu_sys_init,
    .instance_आकार = परिमाण(RISCVIOMMUStateSys),
};

स्थिर शून्य riscv_iommu_पञ्जिकाकोशः_sys(शून्य)
{
    type_पञ्जिकाकोशः_static(&riscv_iommu_sys);
}

type_init(riscv_iommu_पञ्जिकाकोशः_sys)
