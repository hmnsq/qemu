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

#ضمن "qemu/osdep.h"
#ضمن "hw/core/irq.h"
#ضمن "hw/pci/pci_bus.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/core/sysbus.h"
#ضمن "qapi/error.h"
#ضمن "qemu/error-report.h"
#ضمن "qemu/host-utils.h"
#ضمن "qemu/module.h"
#ضمن "qom/object.h"
#ضمن "target/riscv/cpu_bits.h"
#ضمن "trace.h"

#ضمن "riscv-iommu.h"

#define RISCV_IOMMU_SYSDEV_ICVEC_VECTORS 0x3333

#define RISCV_IOMMU_PCI_MSIX_VECTORS 5

/* RISC-V IOMMU System Platform Device Emulation */

بنية RISCVIOMMUStateSys {
    SysBusDevice     parent;
    uint64_t         addr;
    uint32_t         base_irq;
    DeviceState      *irqchip;
    RISCVIOMMUState  iommu;

    /* Wired int support */
    qemu_irq         irqs[RISCV_IOMMU_INTR_ژماردن];

    /* Memory Regions for MSIX table and pending bit entries. */
    MemoryRegion msix_table_mmio;
    MemoryRegion msix_pba_mmio;
    uint8_t *msix_table;
    uint8_t *msix_pba;
};

ساكن uint64_t msix_table_mmio_read(فراغ *opaque, hwaddr addr,
                                     unsigned size)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + size <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_ENTRY_SIZE);
    أرجع pci_get_long(s->msix_table + addr);
}

ساكن فراغ msix_table_mmio_write(فراغ *opaque, hwaddr addr,
                                  uint64_t val, unsigned size)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + size <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_ENTRY_SIZE);
    pci_set_long(s->msix_table + addr, val);
}

ساكن ثابت MemoryRegionOps msix_table_mmio_ops = {
    .اقرأ = msix_table_mmio_read,
    .اكتب = msix_table_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 8,
    },
    .impl = {
        .max_access_size = 4,
    },
};

ساكن uint64_t msix_pba_mmio_read(فراغ *opaque, hwaddr addr,
                                   unsigned size)
{
    RISCVIOMMUStateSys *s = opaque;

    أرجع pci_get_long(s->msix_pba + addr);
}

ساكن فراغ msix_pba_mmio_write(فراغ *opaque, hwaddr addr,
                                uint64_t val, unsigned size)
{
}

ساكن ثابت MemoryRegionOps msix_pba_mmio_ops = {
    .اقرأ = msix_pba_mmio_read,
    .اكتب = msix_pba_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 8,
    },
    .impl = {
        .max_access_size = 4,
    },
};

ساكن فراغ riscv_iommu_sysdev_init_msi(RISCVIOMMUStateSys *s,
                                        uint32_t n_vectors)
{
    RISCVIOMMUState *iommu = &s->iommu;
    uint32_t table_size = n_vectors * PCI_MSIX_ENTRY_SIZE;
    uint32_t table_offset = RISCV_IOMMU_REG_MSI_CONFIG;
    uint32_t pba_size = QEMU_ALIGN_UP(n_vectors, 64) / 8;
    uint32_t pba_offset = RISCV_IOMMU_REG_MSI_CONFIG + 256;

    s->msix_table = g_malloc0(table_size);
    s->msix_pba = g_malloc0(pba_size);

    memory_region_init_io(&s->msix_table_mmio, OBJECT(s), &msix_table_mmio_ops,
                          s, "msix-table", table_size);
    memory_region_add_subregion(&iommu->regs_mr, table_offset,
                                &s->msix_table_mmio);

    memory_region_init_io(&s->msix_pba_mmio, OBJECT(s), &msix_pba_mmio_ops, s,
                          "msix-pba", pba_size);
    memory_region_add_subregion(&iommu->regs_mr, pba_offset,
                                &s->msix_pba_mmio);
}

ساكن فراغ riscv_iommu_sysdev_send_MSI(RISCVIOMMUStateSys *s,
                                        uint32_t vector)
{
    uint8_t *table_entry = s->msix_table + vector * PCI_MSIX_ENTRY_SIZE;
    uint64_t msi_addr = pci_get_quad(table_entry + PCI_MSIX_ENTRY_LOWER_ADDR);
    uint32_t msi_data = pci_get_long(table_entry + PCI_MSIX_ENTRY_DATA);
    MemTxResult result;

    address_space_stl_le(&address_space_memory, msi_addr,
                         msi_data, MEMTXATTRS_UNSPECIFIED, &result);

    إذا (result == MEMTX_OK) {
        trace_riscv_iommu_sys_msi_sent(vector, msi_addr, msi_data, result);
    } وإلا {
        /* Record an access fault error in the fault queue */
        بنية riscv_iommu_fq_record ev = { 0 };
        RISCVIOMMUState *iommu = &s->iommu;

        ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE,
                           RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT);
        ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE,
                           RISCV_IOMMU_FQ_TTYPE_UADDR_WR);
        riscv_iommu_fault(iommu, &ev);
    }
}

ساكن فراغ riscv_iommu_sysdev_notify(RISCVIOMMUState *iommu,
                                      unsigned vector)
{
    RISCVIOMMUStateSys *s = container_of(iommu, RISCVIOMMUStateSys, iommu);
    uint32_t fctl =  riscv_iommu_reg_get32(iommu, RISCV_IOMMU_REG_FCTL);

    إذا (fctl & RISCV_IOMMU_FCTL_WSI) {
        qemu_irq_pulse(s->irqs[vector]);
        trace_riscv_iommu_sys_irq_sent(vector);
        أرجع;
    }

    riscv_iommu_sysdev_send_MSI(s, vector);
}

ساكن فراغ riscv_iommu_sys_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(dev);
    SysBusDevice *sysdev = SYS_BUS_DEVICE(s);
    PCIBus *pci_bus;
    qemu_irq irq;

    qdev_realize(DEVICE(&s->iommu), عدم, errp);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iommu.regs_mr);
    إذا (s->addr) {
        sysbus_mmio_map(SYS_BUS_DEVICE(s), 0, s->addr);
    }

    pci_bus = (PCIBus *) object_resolve_path_type("", TYPE_PCI_BUS, عدم);
    إذا (pci_bus) {
        riscv_iommu_pci_setup_iommu(&s->iommu, pci_bus, errp);
    }

    s->iommu.notify = riscv_iommu_sysdev_notify;

    /* 4 IRQs are defined starting from s->base_irq */
    لكل (صحيح i = 0; i < RISCV_IOMMU_INTR_ژماردن; i++) {
        sysbus_init_irq(sysdev, &s->irqs[i]);
        irq = qdev_get_gpio_in(s->irqchip, s->base_irq + i);
        sysbus_connect_irq(sysdev, i, irq);
    }

    riscv_iommu_sysdev_init_msi(s, RISCV_IOMMU_PCI_MSIX_VECTORS);
}

ساكن فراغ riscv_iommu_sys_init(Object *obj)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_child(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_SYSDEV_ICVEC_VECTORS;
    riscv_iommu_set_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_BOTH);
}

DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, صحيح base_irq,
                                    bool is_32_bit)
{
    DeviceState *iommu_sys = qdev_new(TYPE_RISCV_IOMMU_SYS);

    object_property_set_uint(OBJECT(iommu_sys), "addr", addr, &error_fatal);
    object_property_set_uint(OBJECT(iommu_sys), "base-irq",
                             base_irq, &error_fatal);
    object_property_set_link(OBJECT(iommu_sys), "irqchip",
                             OBJECT(mmio_irqchip),
                             &error_fatal);
    /*
     * For riscv64 use a physical address size of 56 bits (44 bit PPN),
     * and for riscv32 use 34 bits (22 bit PPN).
     */
    object_property_set_uint(OBJECT(iommu_sys), "pas-bits",
                             is_32_bit ? 34 : 56,
                             &error_fatal);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(iommu_sys), &error_fatal);

    أرجع iommu_sys;
}

ساكن ثابت Property riscv_iommu_sys_properties[] = {
    DEFINE_PROP_UINT64("addr", RISCVIOMMUStateSys, addr, 0),
    DEFINE_PROP_UINT32("base-irq", RISCVIOMMUStateSys, base_irq, 0),
    DEFINE_PROP_LINK("irqchip", RISCVIOMMUStateSys, irqchip,
                     TYPE_DEVICE, DeviceState *),
};

ساكن فراغ riscv_iommu_sys_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStateSys *sys = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &sys->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_sys_reset_hold(type);
}

ساكن فراغ riscv_iommu_sys_class_init(ObjectClass *klass, ثابت فراغ *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_sys_reset_hold;

    dc->realize = riscv_iommu_sys_realize;
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_set_props(dc, riscv_iommu_sys_properties);
}

ساكن ثابت TypeInfo riscv_iommu_sys = {
    .name          = TYPE_RISCV_IOMMU_SYS,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .class_init    = riscv_iommu_sys_class_init,
    .instance_init = riscv_iommu_sys_init,
    .instance_size = حجم(RISCVIOMMUStateSys),
};

ساكن فراغ riscv_iommu_register_sys(فراغ)
{
    type_register_static(&riscv_iommu_sys);
}

type_init(riscv_iommu_register_sys)
