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

#اگر_غیر_تعریف_شدہ HW_RISCV_IOMMU_STATE_H
#تعریف_کرو HW_RISCV_IOMMU_STATE_H

#شامل "qom/object.h"
#شامل "hw/core/qdev-properties.h"
#شامل "system/dma.h"
#شامل "hw/riscv/iommu.h"
#شامل "hw/riscv/riscv-iommu-bits.h"

قسمتعریف شمار riscv_iommu_igs_modes riscv_iommu_igs_منوال;

ساخت RISCVIOMMUState {
    /*< private >*/
    DeviceState والد_obj;

    /*< public >*/
    uint32_t نسخہ;     /* Reported interface version number */
    uint32_t pid_بٹ;    /* process identifier width */
    uint32_t pas_بٹ;    /* physical address bits */
    uint32_t bus;         /* PCI bus mapping for non-root endpoints */

    uint64_t cap;         /* IOMMU supported capabilities */
    uint64_t fctl;        /* IOMMU enabled features */
    uint64_t icvec_avail_vectors;  /* Available interrupt vectors in ICVEC */

    bool enable_off;      /* Enable out-of-reset OFF mode (DMA disabled) */
    bool enable_msi;      /* Enable MSI remapping */
    bool enable_ats;      /* Enable ATS support */
    bool enable_s_stage;  /* Enable S/VS-Stage translation */
    bool enable_g_stage;  /* Enable G-Stage translation */

    /* IOMMU Internal State */
    uint64_t ddtp;        /* Validated Device Directory Tree Root Pointer */

    dma_addr_t cq_addr;   /* Command queue base physical address */
    dma_addr_t fq_addr;   /* Fault/event queue base physical address */
    dma_addr_t pq_addr;   /* Page request queue base physical address */

    uint32_t cq_نقاب;     /* Command queue index bit mask */
    uint32_t fq_نقاب;     /* Fault/event queue index bit mask */
    uint32_t pq_نقاب;     /* Page request queue index bit mask */

    /* interrupt notifier */
    خالی (*notify)(RISCVIOMMUState *iommu, غیرعلامتی سمتیہ);

    /* IOMMU target address space */
    پتہ_Space *ہدف_as;
    حافظہ_Region *ہدف_mr;

    /* MSI / MRIF access trap */
    پتہ_Space trap_as;
    حافظہ_Region trap_mr;

    GHash_جدول *ctx_cache;          /* Device translation Context Cache */

    GHash_جدول *iot_cache;          /* IO Translated Address Cache */
    غیرعلامتی iot_حد_بندی;             /* IO Translation Cache size limit */

    /* MMIO Hardware Interface */
    حافظہ_Region regs_mr;
    uint8_t *regs;  /* current register state */
    uint8_t *regs_wc;  /* write-1-to-clear mask */
    /*
     * read-only mask. NOTE: bits not present in this RO
     * mask are assumed to be read and write.
     */
    uint8_t *regs_ro;

    QLIST_اندراج(RISCVIOMMUState) iommus;
    QLIST_سر(, RISCVIOMMUSpace) spaces;

    /* HPM cycle counter */
    QEMUTimer *hpm_timer;
    uint64_t hpmcycle_val;      /* Current value of cycle register */
    uint64_t hpmcycle_پچھلا;     /* Saved value of QEMU_CLOCK_VIRTUAL clock */
    uint64_t irq_حد_سے_تجاوز_بایاں; /* Value beyond INT64_MAX after overflow */

    /* HPM event counters */
    GHash_جدول *hpm_event_ctr_map; /* Mapping of events to counters */
    uint8_t hpm_cntrs;
};

خالی riscv_iommu_pci_تیاری_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
         Error **errp);
خالی riscv_iommu_سیٹ_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_منوال منوال);
خالی riscv_iommu_reset(RISCVIOMMUState *s);
خالی riscv_iommu_notify(RISCVIOMMUState *s, صحیح vec_type);
خالی riscv_iommu_fault(RISCVIOMMUState *s, ساخت riscv_iommu_fq_ریکارڈ *ev);
DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, صحیح بنیاد_irq,
                                    bool is_32_ثنائی_ہندسہ);

قسمتعریف ساخت RISCVIOMMUContext RISCVIOMMUContext;
/* Device translation context state. */
ساخت RISCVIOMMUContext {
    uint64_t devid:24;          /* Requester Id, AKA device_id */
    uint64_t process_id:20;     /* Process ID. PASID for PCIe */
    uint64_t tc;                /* Translation Control */
    uint64_t ta;                /* Translation Attributes */
    uint64_t satp;              /* S-Stage address translation and protection */
    uint64_t gatp;              /* G-Stage address translation and protection */
    uint64_t msi_addr_نقاب;     /* MSI filtering - address mask */
    uint64_t msi_addr_نمونہ;  /* MSI filtering - address pattern */
    uint64_t msiptp;            /* MSI redirection page table pointer */
};

/* private helpers */

/* Register helper functions */
ساکن اندرونی uint32_t riscv_iommu_reg_mod32(RISCVIOMMUState *s,
    غیرعلامتی idx, uint32_t سیٹ, uint32_t clr)
{
    uint32_t val = ldl_le_p(s->regs + idx);
    stl_le_p(s->regs + idx, (val & ~clr) | سیٹ);
    واپس val;
}

ساکن اندرونی خالی riscv_iommu_reg_set32(RISCVIOMMUState *s, غیرعلامتی idx,
                                         uint32_t سیٹ)
{
    stl_le_p(s->regs + idx, سیٹ);
}

ساکن اندرونی uint32_t riscv_iommu_reg_get32(RISCVIOMMUState *s, غیرعلامتی idx)
{
    واپس ldl_le_p(s->regs + idx);
}

ساکن اندرونی uint64_t riscv_iommu_reg_mod64(RISCVIOMMUState *s, غیرعلامتی idx,
                                             uint64_t سیٹ, uint64_t clr)
{
    uint64_t val = ldq_le_p(s->regs + idx);
    stq_le_p(s->regs + idx, (val & ~clr) | سیٹ);
    واپس val;
}

ساکن اندرونی خالی riscv_iommu_reg_set64(RISCVIOMMUState *s, غیرعلامتی idx,
                                         uint64_t سیٹ)
{
    stq_le_p(s->regs + idx, سیٹ);
}

ساکن اندرونی uint64_t riscv_iommu_reg_get64(RISCVIOMMUState *s,
                                             غیرعلامتی idx)
{
    واپس ldq_le_p(s->regs + idx);
}
#اگر_ختم
