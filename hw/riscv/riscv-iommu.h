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

#جيڪڏهن_وصف_نه_ٿيل HW_RISCV_IOMMU_STATE_H
#وصف_ڪريو HW_RISCV_IOMMU_STATE_H

#ضمن "qom/object.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "system/dma.h"
#ضمن "hw/riscv/iommu.h"
#ضمن "hw/riscv/riscv-iommu-bits.h"

تعريفنوع تعداد riscv_iommu_igs_modes riscv_iommu_igs_گهڻائي;

بنية RISCVIOMMUState {
    /*< private >*/
    DeviceState والد_obj;

    /*< public >*/
    uint32_t نسخو;     /* Reported interface version number */
    uint32_t pid_بٽ;    /* process identifier width */
    uint32_t pas_بٽ;    /* physical address bits */
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
    فراغ (*notify)(RISCVIOMMUState *iommu, بنانشان ويڪٽر);

    /* IOMMU target address space */
    پتو_Space *هدف_as;
    يادگيري_Region *هدف_mr;

    /* MSI / MRIF access trap */
    پتو_Space trap_as;
    يادگيري_Region trap_mr;

    GHash_جدول *ctx_cache;          /* Device translation Context Cache */

    GHash_جدول *iot_cache;          /* IO Translated Address Cache */
    بنانشان iot_حد_بندي;             /* IO Translation Cache size limit */

    /* MMIO Hardware Interface */
    يادگيري_Region regs_mr;
    uint8_t *regs;  /* current register state */
    uint8_t *regs_wc;  /* write-1-to-clear mask */
    /*
     * read-only mask. NOTE: bits not present in this RO
     * mask are assumed to be read and write.
     */
    uint8_t *regs_ro;

    QLIST_داخلا(RISCVIOMMUState) iommus;
    QLIST_سر(, RISCVIOMMUSpace) spaces;

    /* HPM cycle counter */
    QEMUTimer *hpm_timer;
    uint64_t hpmcycle_val;      /* Current value of cycle register */
    uint64_t hpmcycle_prev;     /* Saved value of QEMU_CLOCK_VIRTUAL clock */
    uint64_t irq_حد_کان_مٿي_کاٻو; /* Value beyond INT64_MAX after overflow */

    /* HPM event counters */
    GHash_جدول *hpm_event_ctr_map; /* Mapping of events to counters */
    uint8_t hpm_cntrs;
};

فراغ riscv_iommu_pci_تياري_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
         Error **errp);
فراغ riscv_iommu_سيٽ_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_گهڻائي گهڻائي);
فراغ riscv_iommu_reset(RISCVIOMMUState *s);
فراغ riscv_iommu_notify(RISCVIOMMUState *s, صحيح vec_type);
فراغ riscv_iommu_fault(RISCVIOMMUState *s, بنية riscv_iommu_fq_رڪارڊ *ev);
DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, صحيح بنياد_irq,
                                    bool is_32_ٻٽو_انگ);

تعريفنوع بنية RISCVIOMMUContext RISCVIOMMUContext;
/* Device translation context state. */
بنية RISCVIOMMUContext {
    uint64_t devid:24;          /* Requester Id, AKA device_id */
    uint64_t process_id:20;     /* Process ID. PASID for PCIe */
    uint64_t tc;                /* Translation Control */
    uint64_t ta;                /* Translation Attributes */
    uint64_t satp;              /* S-Stage address translation and protection */
    uint64_t gatp;              /* G-Stage address translation and protection */
    uint64_t msi_addr_نقاب;     /* MSI filtering - address mask */
    uint64_t msi_addr_نمونو;  /* MSI filtering - address pattern */
    uint64_t msiptp;            /* MSI redirection page table pointer */
};

/* private helpers */

/* Register helper functions */
ساكن مضمن uint32_t riscv_iommu_reg_mod32(RISCVIOMMUState *s,
    بنانشان idx, uint32_t سيٽ, uint32_t clr)
{
    uint32_t val = ldl_le_p(s->regs + idx);
    stl_le_p(s->regs + idx, (val & ~clr) | سيٽ);
    أرجع val;
}

ساكن مضمن فراغ riscv_iommu_reg_set32(RISCVIOMMUState *s, بنانشان idx,
                                         uint32_t سيٽ)
{
    stl_le_p(s->regs + idx, سيٽ);
}

ساكن مضمن uint32_t riscv_iommu_reg_get32(RISCVIOMMUState *s, بنانشان idx)
{
    أرجع ldl_le_p(s->regs + idx);
}

ساكن مضمن uint64_t riscv_iommu_reg_mod64(RISCVIOMMUState *s, بنانشان idx,
                                             uint64_t سيٽ, uint64_t clr)
{
    uint64_t val = ldq_le_p(s->regs + idx);
    stq_le_p(s->regs + idx, (val & ~clr) | سيٽ);
    أرجع val;
}

ساكن مضمن فراغ riscv_iommu_reg_set64(RISCVIOMMUState *s, بنانشان idx,
                                         uint64_t سيٽ)
{
    stq_le_p(s->regs + idx, سيٽ);
}

ساكن مضمن uint64_t riscv_iommu_reg_get64(RISCVIOMMUState *s,
                                             بنانشان idx)
{
    أرجع ldq_le_p(s->regs + idx);
}
#جيڪڏهن_پورو
