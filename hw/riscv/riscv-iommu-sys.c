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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "hw/core/irq.h"
#అన్తర్భావ "hw/pci/pci_bus.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qemu/host-utils.h"
#అన్తర్భావ "qemu/module.h"
#అన్తర్భావ "qom/object.h"
#అన్తర్భావ "target/riscv/cpu_bits.h"
#అన్తర్భావ "trace.h"

#అన్తర్భావ "riscv-iommu.h"

#నిర్వచించు RISCV_IOMMU_SYSDEV_ICVEC_VECTORS 0x3333

#నిర్వచించు RISCV_IOMMU_PCI_MSIX_VECTORS 5

/* RISC-V IOMMU System Platform Device Emulation */

నిర్మాణం RISCVIOMMUStateSys {
    SysBusDevice     మాతృ;
    uint64_t         addr;
    uint32_t         పునాది_irq;
    DeviceState      *irqchip;
    RISCVIOMMUState  iommu;

    /* Wired int support */
    qemu_irq         irqs[RISCV_IOMMU_INTR_లెక్క];

    /* Memory Regions for MSIX table and pending bit entries. */
    జ్ఞాపకం_Region msix_పట్టిక_mmio;
    జ్ఞాపకం_Region msix_pba_mmio;
    uint8_t *msix_పట్టిక;
    uint8_t *msix_pba;
};

స్థిర uint64_t msix_పట్టిక_mmio_read(శూన్య *opaque, hwaddr addr,
                                     గుర్తులేని కొలత)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + కొలత <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_నమోదు_కొలత);
    తిరిగి pci_get_long(s->msix_పట్టిక + addr);
}

స్థిర శూన్య msix_పట్టిక_mmio_write(శూన్య *opaque, hwaddr addr,
                                  uint64_t val, గుర్తులేని కొలత)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + కొలత <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_నమోదు_కొలత);
    pci_సమితి_long(s->msix_పట్టిక + addr, val);
}

స్థిర స్థిరరాశి జ్ఞాపకం_Region_Ops msix_పట్టిక_mmio_ops = {
    .పఠ = msix_పట్టిక_mmio_read,
    .లిఖ = msix_పట్టిక_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_కొలత = 4,
        .max_access_కొలత = 8,
    },
    .impl = {
        .max_access_కొలత = 4,
    },
};

స్థిర uint64_t msix_pba_mmio_read(శూన్య *opaque, hwaddr addr,
                                   గుర్తులేని కొలత)
{
    RISCVIOMMUStateSys *s = opaque;

    తిరిగి pci_get_long(s->msix_pba + addr);
}

స్థిర శూన్య msix_pba_mmio_write(శూన్య *opaque, hwaddr addr,
                                uint64_t val, గుర్తులేని కొలత)
{
}

స్థిర స్థిరరాశి జ్ఞాపకం_Region_Ops msix_pba_mmio_ops = {
    .పఠ = msix_pba_mmio_read,
    .లిఖ = msix_pba_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_కొలత = 4,
        .max_access_కొలత = 8,
    },
    .impl = {
        .max_access_కొలత = 4,
    },
};

స్థిర శూన్య riscv_iommu_sysdev_init_msi(RISCVIOMMUStateSys *s,
                                        uint32_t n_vectors)
{
    RISCVIOMMUState *iommu = &s->iommu;
    uint32_t పట్టిక_కొలత = n_vectors * PCI_MSIX_నమోదు_కొలత;
    uint32_t పట్టిక_స్థానభ్రంశం = RISCV_IOMMU_REG_MSI_CONFIG;
    uint32_t pba_కొలత = QEMU_ALIGN_పైకి(n_vectors, 64) / 8;
    uint32_t pba_స్థానభ్రంశం = RISCV_IOMMU_REG_MSI_CONFIG + 256;

    s->msix_పట్టిక = g_malloc0(పట్టిక_కొలత);
    s->msix_pba = g_malloc0(pba_కొలత);

    జ్ఞాపకం_region_init_io(&s->msix_పట్టిక_mmio, OBJECT(s), &msix_పట్టిక_mmio_ops,
                          s, "msix-table", పట్టిక_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(&iommu->regs_mr, పట్టిక_స్థానభ్రంశం,
                                &s->msix_పట్టిక_mmio);

    జ్ఞాపకం_region_init_io(&s->msix_pba_mmio, OBJECT(s), &msix_pba_mmio_ops, s,
                          "msix-pba", pba_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(&iommu->regs_mr, pba_స్థానభ్రంశం,
                                &s->msix_pba_mmio);
}

స్థిర శూన్య riscv_iommu_sysdev_send_MSI(RISCVIOMMUStateSys *s,
                                        uint32_t సదిశ)
{
    uint8_t *పట్టిక_నమోదు = s->msix_పట్టిక + సదిశ * PCI_MSIX_నమోదు_కొలత;
    uint64_t msi_addr = pci_get_quad(పట్టిక_నమోదు + PCI_MSIX_నమోదు_క్రింది_ADDR);
    uint32_t msi_దత్తాంశం = pci_get_long(పట్టిక_నమోదు + PCI_MSIX_నమోదు_దత్తాంశం);
    Mem_Tx_ఫలితం ఫలితం;

    చిరునామా_space_stl_le(&చిరునామా_space_జ్ఞాపకం, msi_addr,
                         msi_దత్తాంశం, MEMTXATTRS_UNSPECIFIED, &ఫలితం);

    ఒకవేళ (ఫలితం == MEMTX_OK) {
        trace_riscv_iommu_sys_msi_పంపిన(సదిశ, msi_addr, msi_దత్తాంశం, ఫలితం);
    } లేకపోతే {
        /* Record an access fault error in the fault queue */
        నిర్మాణం riscv_iommu_fq_రికార్డు ev = { 0 };
        RISCVIOMMUState *iommu = &s->iommu;

        ev.hdr = సమితి_క్షేత్రం(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE,
                           RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT);
        ev.hdr = సమితి_క్షేత్రం(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE,
                           RISCV_IOMMU_FQ_TTYPE_UADDR_WR);
        riscv_iommu_fault(iommu, &ev);
    }
}

స్థిర శూన్య riscv_iommu_sysdev_notify(RISCVIOMMUState *iommu,
                                      గుర్తులేని సదిశ)
{
    RISCVIOMMUStateSys *s = container_యొక్క(iommu, RISCVIOMMUStateSys, iommu);
    uint32_t fctl =  riscv_iommu_reg_get32(iommu, RISCV_IOMMU_REG_FCTL);

    ఒకవేళ (fctl & RISCV_IOMMU_FCTL_WSI) {
        qemu_irq_pulse(s->irqs[సదిశ]);
        trace_riscv_iommu_sys_irq_పంపిన(సదిశ);
        తిరిగి;
    }

    riscv_iommu_sysdev_send_MSI(s, సదిశ);
}

స్థిర శూన్య riscv_iommu_sys_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(dev);
    SysBusDevice *sysdev = SYS_BUS_DEVICE(s);
    PCIBus *pci_bus;
    qemu_irq irq;

    qdev_realize(DEVICE(&s->iommu), ఖాళీ, errp);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iommu.regs_mr);
    ఒకవేళ (s->addr) {
        sysbus_mmio_map(SYS_BUS_DEVICE(s), 0, s->addr);
    }

    pci_bus = (PCIBus *) object_resolve_మార్గం_type("", TYPE_PCI_BUS, ఖాళీ);
    ఒకవేళ (pci_bus) {
        riscv_iommu_pci_అమరిక_iommu(&s->iommu, pci_bus, errp);
    }

    s->iommu.notify = riscv_iommu_sysdev_notify;

    /* 4 IRQs are defined starting from s->base_irq */
    కొరకు (పూర్ణాంకం i = 0; i < RISCV_IOMMU_INTR_లెక్క; i++) {
        sysbus_init_irq(sysdev, &s->irqs[i]);
        irq = qdev_get_gpio_లోపల(s->irqchip, s->పునాది_irq + i);
        sysbus_connect_irq(sysdev, i, irq);
    }

    riscv_iommu_sysdev_init_msi(s, RISCV_IOMMU_PCI_MSIX_VECTORS);
}

స్థిర శూన్య riscv_iommu_sys_init(Object *obj)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_బిడ్డ(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_SYSDEV_ICVEC_VECTORS;
    riscv_iommu_సమితి_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_BOTH);
}

DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, పూర్ణాంకం పునాది_irq,
                                    bool is_32_బిట్టు)
{
    DeviceState *iommu_sys = qdev_new(TYPE_RISCV_IOMMU_SYS);

    object_property_సమితి_uint(OBJECT(iommu_sys), "addr", addr, &error_fatal);
    object_property_సమితి_uint(OBJECT(iommu_sys), "base-irq",
                             పునాది_irq, &error_fatal);
    object_property_సమితి_link(OBJECT(iommu_sys), "irqchip",
                             OBJECT(mmio_irqchip),
                             &error_fatal);
    /*
     * For riscv64 use a physical address size of 56 bits (44 bit PPN),
     * and for riscv32 use 34 bits (22 bit PPN).
     */
    object_property_సమితి_uint(OBJECT(iommu_sys), "pas-bits",
                             is_32_బిట్టు ? 34 : 56,
                             &error_fatal);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(iommu_sys), &error_fatal);

    తిరిగి iommu_sys;
}

స్థిర స్థిరరాశి Property riscv_iommu_sys_properties[] = {
    DEFINE_PROP_UINT64("addr", RISCVIOMMUStateSys, addr, 0),
    DEFINE_PROP_UINT32("base-irq", RISCVIOMMUStateSys, పునాది_irq, 0),
    DEFINE_PROP_LINK("irqchip", RISCVIOMMUStateSys, irqchip,
                     TYPE_DEVICE, DeviceState *),
};

స్థిర శూన్య riscv_iommu_sys_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStateSys *sys = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &sys->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_sys_reset_hold(type);
}

స్థిర శూన్య riscv_iommu_sys_class_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_sys_reset_hold;

    dc->realize = riscv_iommu_sys_realize;
    సమితి_బిట్టు(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_సమితి_props(dc, riscv_iommu_sys_properties);
}

స్థిర స్థిరరాశి TypeInfo riscv_iommu_sys = {
    .name          = TYPE_RISCV_IOMMU_SYS,
    .మాతృ        = TYPE_SYS_BUS_DEVICE,
    .class_init    = riscv_iommu_sys_class_init,
    .instance_init = riscv_iommu_sys_init,
    .instance_కొలత = పరిమాణం(RISCVIOMMUStateSys),
};

స్థిర శూన్య riscv_iommu_రిజిస్టరు_sys(శూన్య)
{
    type_రిజిస్టరు_static(&riscv_iommu_sys);
}

type_init(riscv_iommu_రిజిస్టరు_sys)
