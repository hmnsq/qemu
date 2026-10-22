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

#ifndef HW_RISCV_IOMMU_STATE_H
#define HW_RISCV_IOMMU_STATE_H

#অন্তর্ভাৱ "qom/object.h"
#অন্তর্ভাৱ "hw/core/qdev-properties.h"
#অন্তর্ভাৱ "system/dma.h"
#অন্তর্ভাৱ "hw/riscv/iommu.h"
#অন্তর্ভাৱ "hw/riscv/riscv-iommu-bits.h"

প্রকারনির্দেশ গণনা riscv_iommu_igs_modes riscv_iommu_igs_mode;

সংরচনা RISCVIOMMUState {
    /*< private >*/
    DeviceState parent_obj;

    /*< public >*/
    uint32_t version;     /* Reported interface version number */
    uint32_t pid_bits;    /* process identifier width */
    uint32_t pas_bits;    /* physical address bits */
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

    uint32_t cq_mask;     /* Command queue index bit mask */
    uint32_t fq_mask;     /* Fault/event queue index bit mask */
    uint32_t pq_mask;     /* Page request queue index bit mask */

    /* interrupt notifier */
    শূন্য (*notify)(RISCVIOMMUState *iommu, unsigned vector);

    /* IOMMU target address space */
    AddressSpace *target_as;
    MemoryRegion *target_mr;

    /* MSI / MRIF access trap */
    AddressSpace trap_as;
    MemoryRegion trap_mr;

    GHashTable *ctx_cache;          /* Device translation Context Cache */

    GHashTable *iot_cache;          /* IO Translated Address Cache */
    unsigned iot_limit;             /* IO Translation Cache size limit */

    /* MMIO Hardware Interface */
    MemoryRegion regs_mr;
    uint8_t *regs;  /* current register state */
    uint8_t *regs_wc;  /* write-1-to-clear mask */
    /*
     * read-only mask. NOTE: bits not present in this RO
     * mask are assumed to be read and write.
     */
    uint8_t *regs_ro;

    QLIST_ENTRY(RISCVIOMMUState) iommus;
    QLIST_HEAD(, RISCVIOMMUSpace) spaces;

    /* HPM cycle counter */
    QEMUTimer *hpm_timer;
    uint64_t hpmcycle_val;      /* Current value of cycle register */
    uint64_t hpmcycle_prev;     /* Saved value of QEMU_CLOCK_VIRTUAL clock */
    uint64_t irq_overflow_বাম; /* Value beyond INT64_MAX after overflow */

    /* HPM event counters */
    GHashTable *hpm_event_ctr_map; /* Mapping of events to counters */
    uint8_t hpm_cntrs;
};

শূন্য riscv_iommu_pci_setup_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
         Error **errp);
শূন্য riscv_iommu_set_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_mode mode);
শূন্য riscv_iommu_reset(RISCVIOMMUState *s);
শূন্য riscv_iommu_notify(RISCVIOMMUState *s, পূর্ণাঙ্ক vec_type);
শূন্য riscv_iommu_fault(RISCVIOMMUState *s, সংরচনা riscv_iommu_fq_record *ev);
DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, পূর্ণাঙ্ক base_irq,
                                    bool is_32_bit);

প্রকারনির্দেশ সংরচনা RISCVIOMMUContext RISCVIOMMUContext;
/* Device translation context state. */
সংরচনা RISCVIOMMUContext {
    uint64_t devid:24;          /* Requester Id, AKA device_id */
    uint64_t process_id:20;     /* Process ID. PASID for PCIe */
    uint64_t tc;                /* Translation Control */
    uint64_t ta;                /* Translation Attributes */
    uint64_t satp;              /* S-Stage address translation and protection */
    uint64_t gatp;              /* G-Stage address translation and protection */
    uint64_t msi_addr_mask;     /* MSI filtering - address mask */
    uint64_t msi_addr_pattern;  /* MSI filtering - address pattern */
    uint64_t msiptp;            /* MSI redirection page table pointer */
};

/* private helpers */

/* Register helper functions */
স্থির অন্তঃস্থ uint32_t riscv_iommu_reg_mod32(RISCVIOMMUState *s,
    unsigned idx, uint32_t set, uint32_t clr)
{
    uint32_t val = ldl_le_p(s->regs + idx);
    stl_le_p(s->regs + idx, (val & ~clr) | set);
    ঘূৰাই val;
}

স্থির অন্তঃস্থ শূন্য riscv_iommu_reg_set32(RISCVIOMMUState *s, unsigned idx,
                                         uint32_t set)
{
    stl_le_p(s->regs + idx, set);
}

স্থির অন্তঃস্থ uint32_t riscv_iommu_reg_get32(RISCVIOMMUState *s, unsigned idx)
{
    ঘূৰাই ldl_le_p(s->regs + idx);
}

স্থির অন্তঃস্থ uint64_t riscv_iommu_reg_mod64(RISCVIOMMUState *s, unsigned idx,
                                             uint64_t set, uint64_t clr)
{
    uint64_t val = ldq_le_p(s->regs + idx);
    stq_le_p(s->regs + idx, (val & ~clr) | set);
    ঘূৰাই val;
}

স্থির অন্তঃস্থ শূন্য riscv_iommu_reg_set64(RISCVIOMMUState *s, unsigned idx,
                                         uint64_t set)
{
    stq_le_p(s->regs + idx, set);
}

স্থির অন্তঃস্থ uint64_t riscv_iommu_reg_get64(RISCVIOMMUState *s,
                                             unsigned idx)
{
    ঘূৰাই ldq_le_p(s->regs + idx);
}
#endif
