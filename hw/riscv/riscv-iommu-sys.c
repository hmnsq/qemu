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

#समावेश "qemu/osdep.h"
#समावेश "hw/core/irq.h"
#समावेश "hw/pci/pci_bus.h"
#समावेश "hw/core/qdev-properties.h"
#समावेश "hw/core/sysbus.h"
#समावेश "qapi/error.h"
#समावेश "qemu/error-report.h"
#समावेश "qemu/host-utils.h"
#समावेश "qemu/module.h"
#समावेश "qom/object.h"
#समावेश "target/riscv/cpu_bits.h"
#समावेश "trace.h"

#समावेश "riscv-iommu.h"

#घोषणा RISCV_IOMMU_SYSDEV_ICVEC_VECTORS 0x3333

#घोषणा RISCV_IOMMU_PCI_MSIX_VECTORS 5

/* RISC-V IOMMU System Platform Device Emulation */

काष्ठा RISCVIOMMUStateSys {
    SysBusDevice     parent;
    uint64_t         addr;
    uint32_t         base_irq;
    DeviceState      *irqchip;
    RISCVIOMMUState  iommu;

    /* Wired int support */
    qemu_irq         irqs[RISCV_IOMMU_INTR_गिनती];

    /* Memory Regions for MSIX table and pending bit entries. */
    MemoryRegion msix_table_mmio;
    MemoryRegion msix_pba_mmio;
    uint8_t *msix_table;
    uint8_t *msix_pba;
};

अटल uint64_t msix_table_mmio_read(व्योम *opaque, hwaddr addr,
                                     अचिन्हित आकार)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + आकार <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_ENTRY_आकार);
    वापस pci_get_long(s->msix_table + addr);
}

अटल व्योम msix_table_mmio_write(व्योम *opaque, hwaddr addr,
                                  uint64_t val, अचिन्हित आकार)
{
    RISCVIOMMUStateSys *s = opaque;

    g_assert(addr + आकार <= RISCV_IOMMU_PCI_MSIX_VECTORS * PCI_MSIX_ENTRY_आकार);
    pci_set_long(s->msix_table + addr, val);
}

अटल स्थिर MemoryRegionOps msix_table_mmio_ops = {
    .पढ़ो = msix_table_mmio_read,
    .ग_लिखो = msix_table_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_आकार = 4,
        .max_access_आकार = 8,
    },
    .impl = {
        .max_access_आकार = 4,
    },
};

अटल uint64_t msix_pba_mmio_read(व्योम *opaque, hwaddr addr,
                                   अचिन्हित आकार)
{
    RISCVIOMMUStateSys *s = opaque;

    वापस pci_get_long(s->msix_pba + addr);
}

अटल व्योम msix_pba_mmio_write(व्योम *opaque, hwaddr addr,
                                uint64_t val, अचिन्हित आकार)
{
}

अटल स्थिर MemoryRegionOps msix_pba_mmio_ops = {
    .पढ़ो = msix_pba_mmio_read,
    .ग_लिखो = msix_pba_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_आकार = 4,
        .max_access_आकार = 8,
    },
    .impl = {
        .max_access_आकार = 4,
    },
};

अटल व्योम riscv_iommu_sysdev_init_msi(RISCVIOMMUStateSys *s,
                                        uint32_t n_vectors)
{
    RISCVIOMMUState *iommu = &s->iommu;
    uint32_t table_आकार = n_vectors * PCI_MSIX_ENTRY_आकार;
    uint32_t table_offset = RISCV_IOMMU_REG_MSI_CONFIG;
    uint32_t pba_आकार = QEMU_ALIGN_UP(n_vectors, 64) / 8;
    uint32_t pba_offset = RISCV_IOMMU_REG_MSI_CONFIG + 256;

    s->msix_table = g_malloc0(table_आकार);
    s->msix_pba = g_malloc0(pba_आकार);

    memory_region_init_io(&s->msix_table_mmio, OBJECT(s), &msix_table_mmio_ops,
                          s, "msix-table", table_आकार);
    memory_region_add_subregion(&iommu->regs_mr, table_offset,
                                &s->msix_table_mmio);

    memory_region_init_io(&s->msix_pba_mmio, OBJECT(s), &msix_pba_mmio_ops, s,
                          "msix-pba", pba_आकार);
    memory_region_add_subregion(&iommu->regs_mr, pba_offset,
                                &s->msix_pba_mmio);
}

अटल व्योम riscv_iommu_sysdev_send_MSI(RISCVIOMMUStateSys *s,
                                        uint32_t vector)
{
    uint8_t *table_entry = s->msix_table + vector * PCI_MSIX_ENTRY_आकार;
    uint64_t msi_addr = pci_get_quad(table_entry + PCI_MSIX_ENTRY_LOWER_ADDR);
    uint32_t msi_तथ्य = pci_get_long(table_entry + PCI_MSIX_ENTRY_तथ्य);
    Mem_Tx_परिणाम परिणाम;

    address_space_stl_le(&address_space_memory, msi_addr,
                         msi_तथ्य, MEMTXATTRS_UNSPECIFIED, &परिणाम);

    अगर (परिणाम == MEMTX_OK) {
        trace_riscv_iommu_sys_msi_sent(vector, msi_addr, msi_तथ्य, परिणाम);
    } अन्यथा {
        /* Record an access fault error in the fault queue */
        काष्ठा riscv_iommu_fq_record ev = { 0 };
        RISCVIOMMUState *iommu = &s->iommu;

        ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE,
                           RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT);
        ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE,
                           RISCV_IOMMU_FQ_TTYPE_UADDR_WR);
        riscv_iommu_fault(iommu, &ev);
    }
}

अटल व्योम riscv_iommu_sysdev_notify(RISCVIOMMUState *iommu,
                                      अचिन्हित vector)
{
    RISCVIOMMUStateSys *s = container_of(iommu, RISCVIOMMUStateSys, iommu);
    uint32_t fctl =  riscv_iommu_reg_get32(iommu, RISCV_IOMMU_REG_FCTL);

    अगर (fctl & RISCV_IOMMU_FCTL_WSI) {
        qemu_irq_pulse(s->irqs[vector]);
        trace_riscv_iommu_sys_irq_sent(vector);
        वापस;
    }

    riscv_iommu_sysdev_send_MSI(s, vector);
}

अटल व्योम riscv_iommu_sys_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(dev);
    SysBusDevice *sysdev = SYS_BUS_DEVICE(s);
    PCIBus *pci_bus;
    qemu_irq irq;

    qdev_realize(DEVICE(&s->iommu), शून्य, errp);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->iommu.regs_mr);
    अगर (s->addr) {
        sysbus_mmio_map(SYS_BUS_DEVICE(s), 0, s->addr);
    }

    pci_bus = (PCIBus *) object_resolve_path_type("", TYPE_PCI_BUS, शून्य);
    अगर (pci_bus) {
        riscv_iommu_pci_setup_iommu(&s->iommu, pci_bus, errp);
    }

    s->iommu.notify = riscv_iommu_sysdev_notify;

    /* 4 IRQs are defined starting from s->base_irq */
    क्रम (पूर्णांक i = 0; i < RISCV_IOMMU_INTR_गिनती; i++) {
        sysbus_init_irq(sysdev, &s->irqs[i]);
        irq = qdev_get_gpio_in(s->irqchip, s->base_irq + i);
        sysbus_connect_irq(sysdev, i, irq);
    }

    riscv_iommu_sysdev_init_msi(s, RISCV_IOMMU_PCI_MSIX_VECTORS);
}

अटल व्योम riscv_iommu_sys_init(Object *obj)
{
    RISCVIOMMUStateSys *s = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &s->iommu;

    object_initialize_child(obj, "iommu", iommu, TYPE_RISCV_IOMMU);
    qdev_alias_all_properties(DEVICE(iommu), obj);

    iommu->icvec_avail_vectors = RISCV_IOMMU_SYSDEV_ICVEC_VECTORS;
    riscv_iommu_set_cap_igs(iommu, RISCV_IOMMU_CAP_IGS_BOTH);
}

DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, पूर्णांक base_irq,
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

    वापस iommu_sys;
}

अटल स्थिर Property riscv_iommu_sys_properties[] = {
    DEFINE_PROP_UINT64("addr", RISCVIOMMUStateSys, addr, 0),
    DEFINE_PROP_UINT32("base-irq", RISCVIOMMUStateSys, base_irq, 0),
    DEFINE_PROP_LINK("irqchip", RISCVIOMMUStateSys, irqchip,
                     TYPE_DEVICE, DeviceState *),
};

अटल व्योम riscv_iommu_sys_reset_hold(Object *obj, ResetType type)
{
    RISCVIOMMUStateSys *sys = RISCV_IOMMU_SYS(obj);
    RISCVIOMMUState *iommu = &sys->iommu;

    riscv_iommu_reset(iommu);

    trace_riscv_iommu_sys_reset_hold(type);
}

अटल व्योम riscv_iommu_sys_class_init(ObjectClass *klass, स्थिर व्योम *तथ्य)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ResettableClass *rc = RESETTABLE_CLASS(klass);

    rc->phases.hold = riscv_iommu_sys_reset_hold;

    dc->realize = riscv_iommu_sys_realize;
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
    device_class_set_props(dc, riscv_iommu_sys_properties);
}

अटल स्थिर TypeInfo riscv_iommu_sys = {
    .name          = TYPE_RISCV_IOMMU_SYS,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .class_init    = riscv_iommu_sys_class_init,
    .instance_init = riscv_iommu_sys_init,
    .instance_आकार = माप(RISCVIOMMUStateSys),
};

अटल व्योम riscv_iommu_register_sys(व्योम)
{
    type_register_static(&riscv_iommu_sys);
}

type_init(riscv_iommu_register_sys)
