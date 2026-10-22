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

#شامل "qemu/osdep.h"
#شامل "hw/core/irq.h"
#شامل "hw/pci/pci_bus.h"
#شامل "hw/core/qdev-properties.h"
#شامل "hw/core/sysbus.h"
#شامل "qapi/error.h"
#شامل "qemu/error-report.h"
#شامل "qemu/host-utils.h"
#شامل "qemu/module.h"
#شامل "qom/object.h"
#شامل "target/riscv/cpu_bits.h"
#شامل "trace.h"

#شامل "riscv-iommu.h"

#تعریف_کرو RISCV_IOMMU_SYSDEV_ICVEC_VECTORS 0x3333

#تعریف_کرو RISCV_IOMMU_PCI_MSIX_VECTORS 5

/* RISC-V IOMMU System Platform Device Emulation */

ساخت RISCVIOMMUStateSys {
    SysBusDevice     والد;
    uint64_t         addr;
    uint32_t         بنیاد_irq;
    DeviceState      *irqchip;
    RISCVIOMMUState  iommu;

    /* Wired int support */
    qemu_irq         irqs[RISCV_IOMMU_INTR_گنتی];

    /* Memory Regions for MSIX table and pending bit entries. */
    حافظہ_Region msix_جدول_mmio;
    حافظہ_Region msix_pba_mmio;
    uint8_t *msix_جدول;
    uint8_t *msix_pba;
};

ساکن uint64_t msix_جدول_mmio_read(خالی *opaque, hwaddr addr,
                                     غیرعلامتی سائز)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + سائز <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_اندراج_سائز);
    واپس pci_get_long(s->msix_جدول + addr);
}

ساکن خالی msix_جدول_mmio_write(خالی *opaque, hwaddr addr,
                                  uint64_t val, غیرعلامتی سائز)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + سائز <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_اندراج_سائز);
    pci_سیٹ_long(s->msix_جدول + addr, val);
}

ساکن ثابت حافظہ_Region_Ops msix_جدول_mmio_ops = {
    .اقرأ = msix_جدول_mmio_read,
    .اكتب = msix_جدول_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_سائز = 4,
        .max_access_سائز = 8,
    },
    .impl = {
        .max_access_سائز = 4,
    },
};

ساکن uint64_t msix_pba_mmio_read(خالی *opaque, hwaddr addr,
                                   غیرعلامتی سائز)
{
    RISCVIOMMUStateSys *s = opaque;

    واپس pci_get_long(s->msix_pba + addr);
}

ساکن خالی msix_pba_mmio_write(خالی *opaque, hwaddr addr,
                                uint64_t val, غیرعلامتی سائز)
{
}

ساکن ثابت حافظہ_Region_Ops msix_pba_mmio_ops = {
    .اقرأ = msix_pba_mmio_read,
    .اكتب = msix_pba_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_سائز = 4,
        .max_access_سائز = 8,
    },
    .impl = {
        .max_access_سائز = 4,
    },
};

ساکن خالی riscv_iommu_sysdev_init_msi(RISCVIOMMUStateSys *s,
                                        uint32_t n_vectors)
{
    RISCVIOMMUState *iommu = &s->iommu;
    uint32_t جدول_سائز = n_vectors * PCI_MSIX_اندراج_سائز;
    uint32_t جدول_انحراف = RISCV_IOMMU_REG_MSI_CONFIG;
    uint32_t pba_سائز = QEMU_ALIGN_اوپر(n_vectors, 64) / 8;
    uint32_t pba_انحراف = RISCV_IOMMU_REG_MSI_CONFIG + 256;

    s->msix_جدول = g_malloc0(جدول_سائز);
    s->msix_pba = g_malloc0(pba_سائز);

    حافظہ_region_init_io(&s->msix_جدول_mmio, OBJECT(s), &msix_جدول_mmio_ops,
                          s, "msix-table", جدول_سائز);
    حافظہ_region_جوڑو_subregion(&iommu->regs_mr, جدول_انحراف,
                                &s->msix_جدول_mmio);

    حافظہ_region_init_io(&s->msix_pba_mmio, OBJECT(s), &msix_pba_mmio_ops, s,
                          "msix-pba", pba_سائز);
    حافظہ_region_جوڑو_subregion(&iommu->regs_mr, pba_انحراف,
                                &s->msix_pba_mmio);
}

ساکن خالی riscv_iommu_sysdev_send_MSI(RISCVIOMMUStateSys *s,
                                        uint32_t سمتیہ)
{
    uint8_t *جدول_اندراج = s->msix_جدول + سمتیہ * PCI_MSIX_اندراج_سائز;
    uint64_t msi_addr = pci_get_quad(جدول_اندراج + PCI_MSIX_اندراج_زیریں_ADDR);
    uint32_t msi_ڈیٹا = pci_get_long(جدول_اندراج + PCI_MSIX_اندراج_ڈیٹا);
    Mem_Tx_نتیجہ نتیجہ;

    پتہ_space_stl_le(&پتہ_space_حافظہ, msi_addr,
                         msi_ڈیٹا, MEMTXATTRS_UNSPECIFIED, &نتیجہ);

    اگر (نتیجہ == MEMTX_OK) {
        trace_riscv_iommu_sys_msi_بھیجا(سمتیہ, msi_addr, msi_ڈیٹا, نتیجہ);
    } ورنہ {
        /* Record an access fault error in the fault queue */
        ساخت riscv_iommu_fq_ریکارڈ ev = { 0 };
        RISCVIOMMUState *iommu = &s->iommu;

        ev.hdr = سیٹ_میدان(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE,
                           RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT);
        ev.hdr = سیٹ_میدان(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE,
                           RISCV_IOMMU_FQ_TTYPE_UADDR_WR);
        riscv_iommu_fault(iommu, &ev);
    }
}

ساکن خالی riscv_iommu_sysdev_notify(RISCVIOMMUState *iommu,
                                      غیرعلامتی سمتیہ)
{
    RISCVIOMMUStateSys *s = container_کا(iommu, RISCVIOMMUStateSys, iommu);
    uint32_t fctl =  riscv_iommu_reg_get32(iommu, RISCV_IOMMU_REG_FCTL);

    اگر (fctl & RISCV_IOMMU_FCTL_WSI) {
        qemu_irq_pulse(s->irqs[سمتیہ]);
        trace_riscv_iommu_sys_irq_بھیجا(سمتیہ);
        واپس;
    }

    riscv_iommu_sysdev_send_MSI(s, سمتیہ);
}

ساکن خالی riscv_iommu_sys_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(dev);
    SysBusDevice *sysdev = SYS_BUS_DEVICE(s);
    PCIBus *pci_bus;
    qemu_irq irq;

    qdev_realize(DEVICE(&s->iommu), تہی, errp);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iommu.regs_mr);
    اگر (s->addr) {
        sysbus_mmio_map(SYS_BUS_DEVICE(s), 0, s->addr);
    }

    pci_bus = (PCIBus *) object_resolve_راستہ_type("", TYPE_PCI_BUS, تہی);
    اگر (pci_bus) {
        riscv_iommu_pci_تیاری_iommu(&s->iommu, pci_bus, errp);
    }

    s->iommu.notify = riscv_iommu_sysdev_notify;

    /* 4 IRQs are defined starting from s->base_irq */
    برائے (صحیح i = 0; i < RISCV_IOMMU_INTR_گنتی; i++) {
        sysbus_init_irq(sysdev, &s->irqs[i]);
        irq = qdev_get_gpio_اندر(s->irqchip, s->بنیاد_irq + i);
        sysbus_connect_irq(sysdev, i, irq);
    }

    riscv_iommu_sysdev_init_msi(s, RISCV_IOMMU_PCI_MSIX_VECTORS);
}

ساکن خالی riscv_iommu_sys_init(Object *obj)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_بچہ(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_SYSDEV_ICVEC_VECTORS;
    riscv_iommu_سیٹ_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_BOTH);
}

DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, صحیح بنیاد_irq,
                                    bool is_32_ثنائی_ہندسہ)
{
    DeviceState *iommu_sys = qdev_new(TYPE_RISCV_IOMMU_SYS);

    object_property_سیٹ_uint(OBJECT(iommu_sys), "addr", addr, &error_fatal);
    object_property_سیٹ_uint(OBJECT(iommu_sys), "base-irq",
                             بنیاد_irq, &error_fatal);
    object_property_سیٹ_link(OBJECT(iommu_sys), "irqchip",
                             OBJECT(mmio_irqchip),
                             &error_fatal);
    /*
     * For riscv64 use a physical address size of 56 bits (44 bit PPN),
     * and for riscv32 use 34 bits (22 bit PPN).
     */
    object_property_سیٹ_uint(OBJECT(iommu_sys), "pas-bits",
                             is_32_ثنائی_ہندسہ ? 34 : 56,
                             &error_fatal);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(iommu_sys), &error_fatal);

    واپس iommu_sys;
}

ساکن ثابت Property riscv_iommu_sys_properties[] = {
    DEFINE_PROP_UINT64("addr", RISCVIOMMUStateSys, addr, 0),
    DEFINE_PROP_UINT32("base-irq", RISCVIOMMUStateSys, بنیاد_irq, 0),
    DEFINE_PROP_LINK("irqchip", RISCVIOMMUStateSys, irqchip,
                     TYPE_DEVICE, DeviceState *),
};

ساکن خالی riscv_iommu_sys_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStateSys *sys = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &sys->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_sys_reset_hold(type);
}

ساکن خالی riscv_iommu_sys_class_init(ObjectClass *klass, ثابت خالی *ڈیٹا)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_sys_reset_hold;

    dc->realize = riscv_iommu_sys_realize;
    سیٹ_ثنائی_ہندسہ(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_سیٹ_props(dc, riscv_iommu_sys_properties);
}

ساکن ثابت TypeInfo riscv_iommu_sys = {
    .name          = TYPE_RISCV_IOMMU_SYS,
    .والد        = TYPE_SYS_BUS_DEVICE,
    .class_init    = riscv_iommu_sys_class_init,
    .instance_init = riscv_iommu_sys_init,
    .instance_سائز = حجم(RISCVIOMMUStateSys),
};

ساکن خالی riscv_iommu_رجسٹر_sys(خالی)
{
    type_رجسٹر_static(&riscv_iommu_sys);
}

type_init(riscv_iommu_رجسٹر_sys)
