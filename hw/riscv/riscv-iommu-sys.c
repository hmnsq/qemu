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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/irq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pci_bus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/host-utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/module.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qom/object.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu_bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "trace.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_SYSDEV_ICVEC_VECTORS 0x3333

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PCI_MSIX_VECTORS 5

/* RISC-V IOMMU System Platform Device Emulation */

ਬਣਤਰ RISCVIOMMUStateSys {
    SysBusDevice     ਮਾਪਾ;
    uint64_t         addr;
    uint32_t         ਅਧਾਰ_irq;
    DeviceState      *irqchip;
    RISCVIOMMUState  iommu;

    /* Wired int support */
    qemu_irq         irqs[RISCV_IOMMU_INTR_ਗਿਣਤੀ];

    /* Memory Regions for MSIX table and pending bit entries. */
    ਸਮ੍ਰਿਤੀ_Region msix_ਸਾਰਣੀ_mmio;
    ਸਮ੍ਰਿਤੀ_Region msix_pba_mmio;
    uint8_t *msix_ਸਾਰਣੀ;
    uint8_t *msix_pba;
};

ਸਥਿਰ uint64_t msix_ਸਾਰਣੀ_mmio_read(ਖਾਲੀ *opaque, hwaddr addr,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + ਮਾਪ <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_ਇੰਦਰਾਜ_ਮਾਪ);
    ਵਾਪਸ pci_get_long(s->msix_ਸਾਰਣੀ + addr);
}

ਸਥਿਰ ਖਾਲੀ msix_ਸਾਰਣੀ_mmio_write(ਖਾਲੀ *opaque, hwaddr addr,
                                  uint64_t val, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + ਮਾਪ <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_ਇੰਦਰਾਜ_ਮਾਪ);
    pci_ਸੈੱਟ_long(s->msix_ਸਾਰਣੀ + addr, val);
}

ਸਥਿਰ ਅਚਲ ਸਮ੍ਰਿਤੀ_Region_Ops msix_ਸਾਰਣੀ_mmio_ops = {
    .ਪਠ = msix_ਸਾਰਣੀ_mmio_read,
    .ਲਿਖ = msix_ਸਾਰਣੀ_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_ਮਾਪ = 4,
        .max_access_ਮਾਪ = 8,
    },
    .impl = {
        .max_access_ਮਾਪ = 4,
    },
};

ਸਥਿਰ uint64_t msix_pba_mmio_read(ਖਾਲੀ *opaque, hwaddr addr,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
    RISCVIOMMUStateSys *s = opaque;

    ਵਾਪਸ pci_get_long(s->msix_pba + addr);
}

ਸਥਿਰ ਖਾਲੀ msix_pba_mmio_write(ਖਾਲੀ *opaque, hwaddr addr,
                                uint64_t val, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
}

ਸਥਿਰ ਅਚਲ ਸਮ੍ਰਿਤੀ_Region_Ops msix_pba_mmio_ops = {
    .ਪਠ = msix_pba_mmio_read,
    .ਲਿਖ = msix_pba_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_ਮਾਪ = 4,
        .max_access_ਮਾਪ = 8,
    },
    .impl = {
        .max_access_ਮਾਪ = 4,
    },
};

ਸਥਿਰ ਖਾਲੀ riscv_iommu_sysdev_init_msi(RISCVIOMMUStateSys *s,
                                        uint32_t n_vectors)
{
    RISCVIOMMUState *iommu = &s->iommu;
    uint32_t ਸਾਰਣੀ_ਮਾਪ = n_vectors * PCI_MSIX_ਇੰਦਰਾਜ_ਮਾਪ;
    uint32_t ਸਾਰਣੀ_ਵਿੱਥ = RISCV_IOMMU_REG_MSI_CONFIG;
    uint32_t pba_ਮਾਪ = QEMU_ALIGN_ਉੱਪਰ(n_vectors, 64) / 8;
    uint32_t pba_ਵਿੱਥ = RISCV_IOMMU_REG_MSI_CONFIG + 256;

    s->msix_ਸਾਰਣੀ = g_malloc0(ਸਾਰਣੀ_ਮਾਪ);
    s->msix_pba = g_malloc0(pba_ਮਾਪ);

    ਸਮ੍ਰਿਤੀ_region_init_io(&s->msix_ਸਾਰਣੀ_mmio, OBJECT(s), &msix_ਸਾਰਣੀ_mmio_ops,
                          s, "msix-table", ਸਾਰਣੀ_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(&iommu->regs_mr, ਸਾਰਣੀ_ਵਿੱਥ,
                                &s->msix_ਸਾਰਣੀ_mmio);

    ਸਮ੍ਰਿਤੀ_region_init_io(&s->msix_pba_mmio, OBJECT(s), &msix_pba_mmio_ops, s,
                          "msix-pba", pba_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(&iommu->regs_mr, pba_ਵਿੱਥ,
                                &s->msix_pba_mmio);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_sysdev_send_MSI(RISCVIOMMUStateSys *s,
                                        uint32_t ਸਦਿਸ਼)
{
    uint8_t *ਸਾਰਣੀ_ਇੰਦਰਾਜ = s->msix_ਸਾਰਣੀ + ਸਦਿਸ਼ * PCI_MSIX_ਇੰਦਰਾਜ_ਮਾਪ;
    uint64_t msi_addr = pci_get_quad(ਸਾਰਣੀ_ਇੰਦਰਾਜ + PCI_MSIX_ਇੰਦਰਾਜ_ਹੇਠਲਾ_ADDR);
    uint32_t msi_ਅੰਕੜੇ = pci_get_long(ਸਾਰਣੀ_ਇੰਦਰਾਜ + PCI_MSIX_ਇੰਦਰਾਜ_ਅੰਕੜੇ);
    Mem_Tx_ਨਤੀਜਾ ਨਤੀਜਾ;

    ਪਤਾ_space_stl_le(&ਪਤਾ_space_ਸਮ੍ਰਿਤੀ, msi_addr,
                         msi_ਅੰਕੜੇ, MEMTXATTRS_UNSPECIFIED, &ਨਤੀਜਾ);

    ਜੇ (ਨਤੀਜਾ == MEMTX_OK) {
        trace_riscv_iommu_sys_msi_ਭੇਜਿਆ(ਸਦਿਸ਼, msi_addr, msi_ਅੰਕੜੇ, ਨਤੀਜਾ);
    } ਨਹੀਂਤਾਂ {
        /* Record an access fault error in the fault queue */
        ਬਣਤਰ riscv_iommu_fq_ਰਿਕਾਰਡ ev = { 0 };
        RISCVIOMMUState *iommu = &s->iommu;

        ev.hdr = ਸੈੱਟ_ਖੇਤਰ(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE,
                           RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT);
        ev.hdr = ਸੈੱਟ_ਖੇਤਰ(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE,
                           RISCV_IOMMU_FQ_TTYPE_UADDR_WR);
        riscv_iommu_fault(iommu, &ev);
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_sysdev_notify(RISCVIOMMUState *iommu,
                                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਦਿਸ਼)
{
    RISCVIOMMUStateSys *s = container_ਦਾ(iommu, RISCVIOMMUStateSys, iommu);
    uint32_t fctl =  riscv_iommu_reg_get32(iommu, RISCV_IOMMU_REG_FCTL);

    ਜੇ (fctl & RISCV_IOMMU_FCTL_WSI) {
        qemu_irq_pulse(s->irqs[ਸਦਿਸ਼]);
        trace_riscv_iommu_sys_irq_ਭੇਜਿਆ(ਸਦਿਸ਼);
        ਵਾਪਸ;
    }

    riscv_iommu_sysdev_send_MSI(s, ਸਦਿਸ਼);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_sys_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(dev);
    SysBusDevice *sysdev = SYS_BUS_DEVICE(s);
    PCIBus *pci_bus;
    qemu_irq irq;

    qdev_realize(DEVICE(&s->iommu), ਨਿਰਾ, errp);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iommu.regs_mr);
    ਜੇ (s->addr) {
        sysbus_mmio_map(SYS_BUS_DEVICE(s), 0, s->addr);
    }

    pci_bus = (PCIBus *) object_resolve_ਰਸਤਾ_type("", TYPE_PCI_BUS, ਨਿਰਾ);
    ਜੇ (pci_bus) {
        riscv_iommu_pci_ਤਿਆਰੀ_iommu(&s->iommu, pci_bus, errp);
    }

    s->iommu.notify = riscv_iommu_sysdev_notify;

    /* 4 IRQs are defined starting from s->base_irq */
    ਲਈ (ਪੂਰਨ i = 0; i < RISCV_IOMMU_INTR_ਗਿਣਤੀ; i++) {
        sysbus_init_irq(sysdev, &s->irqs[i]);
        irq = qdev_get_gpio_ਵਿੱਚ(s->irqchip, s->ਅਧਾਰ_irq + i);
        sysbus_connect_irq(sysdev, i, irq);
    }

    riscv_iommu_sysdev_init_msi(s, RISCV_IOMMU_PCI_MSIX_VECTORS);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_sys_init(Object *obj)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_ਬੱਚਾ(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_SYSDEV_ICVEC_VECTORS;
    riscv_iommu_ਸੈੱਟ_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_BOTH);
}

DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, ਪੂਰਨ ਅਧਾਰ_irq,
                                    bool is_32_ਦੋਆਧਾਰੀ_ਅੰਕ)
{
    DeviceState *iommu_sys = qdev_new(TYPE_RISCV_IOMMU_SYS);

    object_property_ਸੈੱਟ_uint(OBJECT(iommu_sys), "addr", addr, &error_fatal);
    object_property_ਸੈੱਟ_uint(OBJECT(iommu_sys), "base-irq",
                             ਅਧਾਰ_irq, &error_fatal);
    object_property_ਸੈੱਟ_link(OBJECT(iommu_sys), "irqchip",
                             OBJECT(mmio_irqchip),
                             &error_fatal);
    /*
     * For riscv64 use a physical address size of 56 bits (44 bit PPN),
     * and for riscv32 use 34 bits (22 bit PPN).
     */
    object_property_ਸੈੱਟ_uint(OBJECT(iommu_sys), "pas-bits",
                             is_32_ਦੋਆਧਾਰੀ_ਅੰਕ ? 34 : 56,
                             &error_fatal);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(iommu_sys), &error_fatal);

    ਵਾਪਸ iommu_sys;
}

ਸਥਿਰ ਅਚਲ Property riscv_iommu_sys_properties[] = {
    DEFINE_PROP_UINT64("addr", RISCVIOMMUStateSys, addr, 0),
    DEFINE_PROP_UINT32("base-irq", RISCVIOMMUStateSys, ਅਧਾਰ_irq, 0),
    DEFINE_PROP_LINK("irqchip", RISCVIOMMUStateSys, irqchip,
                     TYPE_DEVICE, DeviceState *),
};

ਸਥਿਰ ਖਾਲੀ riscv_iommu_sys_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStateSys *sys = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &sys->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_sys_reset_hold(type);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_sys_class_init(ObjectClass *klass, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_sys_reset_hold;

    dc->realize = riscv_iommu_sys_realize;
    ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_ਸੈੱਟ_props(dc, riscv_iommu_sys_properties);
}

ਸਥਿਰ ਅਚਲ TypeInfo riscv_iommu_sys = {
    .name          = TYPE_RISCV_IOMMU_SYS,
    .ਮਾਪਾ        = TYPE_SYS_BUS_DEVICE,
    .class_init    = riscv_iommu_sys_class_init,
    .instance_init = riscv_iommu_sys_init,
    .instance_ਮਾਪ = ਆਕਾਰ(RISCVIOMMUStateSys),
};

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ਰਜਿਸਟਰ_sys(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&riscv_iommu_sys);
}

type_init(riscv_iommu_ਰਜਿਸਟਰ_sys)
