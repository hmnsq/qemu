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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HW_RISCV_IOMMU_STATE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HW_RISCV_IOMMU_STATE_H

#ਅਨ੍ਤਰ੍ਭਾਵ "qom/object.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/dma.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv-iommu-bits.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ riscv_iommu_igs_modes riscv_iommu_igs_ਬਹੁਲਕ;

ਬਣਤਰ RISCVIOMMUState {
    /*< private >*/
    DeviceState ਮਾਪਾ_obj;

    /*< public >*/
    uint32_t ਸੰਸਕਰਣ;     /* Reported interface version number */
    uint32_t pid_ਬਿੱਟ;    /* process identifier width */
    uint32_t pas_ਬਿੱਟ;    /* physical address bits */
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

    uint32_t cq_ਨਕਾਬ;     /* Command queue index bit mask */
    uint32_t fq_ਨਕਾਬ;     /* Fault/event queue index bit mask */
    uint32_t pq_ਨਕਾਬ;     /* Page request queue index bit mask */

    /* interrupt notifier */
    ਖਾਲੀ (*notify)(RISCVIOMMUState *iommu, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਦਿਸ਼);

    /* IOMMU target address space */
    ਪਤਾ_Space *ਨਿਸ਼ਾਨਾ_as;
    ਸਮ੍ਰਿਤੀ_Region *ਨਿਸ਼ਾਨਾ_mr;

    /* MSI / MRIF access trap */
    ਪਤਾ_Space trap_as;
    ਸਮ੍ਰਿਤੀ_Region trap_mr;

    GHash_ਸਾਰਣੀ *ctx_cache;          /* Device translation Context Cache */

    GHash_ਸਾਰਣੀ *iot_cache;          /* IO Translated Address Cache */
    ਬਿਨਾਨਿਸ਼ਾਨੀ iot_ਹੱਦਬੰਦੀ;             /* IO Translation Cache size limit */

    /* MMIO Hardware Interface */
    ਸਮ੍ਰਿਤੀ_Region regs_mr;
    uint8_t *regs;  /* current register state */
    uint8_t *regs_wc;  /* write-1-to-clear mask */
    /*
     * read-only mask. NOTE: bits not present in this RO
     * mask are assumed to be read and write.
     */
    uint8_t *regs_ro;

    QLIST_ਇੰਦਰਾਜ(RISCVIOMMUState) iommus;
    QLIST_ਸਿਰ(, RISCVIOMMUSpace) spaces;

    /* HPM cycle counter */
    QEMUTimer *hpm_timer;
    uint64_t hpmcycle_val;      /* Current value of cycle register */
    uint64_t hpmcycle_prev;     /* Saved value of QEMU_CLOCK_VIRTUAL clock */
    uint64_t irq_ਉਛਾਲ_ਖੱਬਾ; /* Value beyond INT64_MAX after overflow */

    /* HPM event counters */
    GHash_ਸਾਰਣੀ *hpm_event_ctr_map; /* Mapping of events to counters */
    uint8_t hpm_cntrs;
};

ਖਾਲੀ riscv_iommu_pci_ਤਿਆਰੀ_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
         Error **errp);
ਖਾਲੀ riscv_iommu_ਸੈੱਟ_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_ਬਹੁਲਕ ਬਹੁਲਕ);
ਖਾਲੀ riscv_iommu_reset(RISCVIOMMUState *s);
ਖਾਲੀ riscv_iommu_notify(RISCVIOMMUState *s, ਪੂਰਨ vec_type);
ਖਾਲੀ riscv_iommu_fault(RISCVIOMMUState *s, ਬਣਤਰ riscv_iommu_fq_ਰਿਕਾਰਡ *ev);
DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, ਪੂਰਨ ਅਧਾਰ_irq,
                                    bool is_32_ਦੋਆਧਾਰੀ_ਅੰਕ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ RISCVIOMMUContext RISCVIOMMUContext;
/* Device translation context state. */
ਬਣਤਰ RISCVIOMMUContext {
    uint64_t devid:24;          /* Requester Id, AKA device_id */
    uint64_t process_id:20;     /* Process ID. PASID for PCIe */
    uint64_t tc;                /* Translation Control */
    uint64_t ta;                /* Translation Attributes */
    uint64_t satp;              /* S-Stage address translation and protection */
    uint64_t gatp;              /* G-Stage address translation and protection */
    uint64_t msi_addr_ਨਕਾਬ;     /* MSI filtering - address mask */
    uint64_t msi_addr_ਨਮੂਨਾ;  /* MSI filtering - address pattern */
    uint64_t msiptp;            /* MSI redirection page table pointer */
};

/* private helpers */

/* Register helper functions */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t riscv_iommu_reg_mod32(RISCVIOMMUState *s,
    ਬਿਨਾਨਿਸ਼ਾਨੀ idx, uint32_t ਸੈੱਟ, uint32_t clr)
{
    uint32_t val = ldl_le_p(s->regs + idx);
    stl_le_p(s->regs + idx, (val & ~clr) | ਸੈੱਟ);
    ਵਾਪਸ val;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ riscv_iommu_reg_set32(RISCVIOMMUState *s, ਬਿਨਾਨਿਸ਼ਾਨੀ idx,
                                         uint32_t ਸੈੱਟ)
{
    stl_le_p(s->regs + idx, ਸੈੱਟ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t riscv_iommu_reg_get32(RISCVIOMMUState *s, ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
    ਵਾਪਸ ldl_le_p(s->regs + idx);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t riscv_iommu_reg_mod64(RISCVIOMMUState *s, ਬਿਨਾਨਿਸ਼ਾਨੀ idx,
                                             uint64_t ਸੈੱਟ, uint64_t clr)
{
    uint64_t val = ldq_le_p(s->regs + idx);
    stq_le_p(s->regs + idx, (val & ~clr) | ਸੈੱਟ);
    ਵਾਪਸ val;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ riscv_iommu_reg_set64(RISCVIOMMUState *s, ਬਿਨਾਨਿਸ਼ਾਨੀ idx,
                                         uint64_t ਸੈੱਟ)
{
    stq_le_p(s->regs + idx, ਸੈੱਟ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t riscv_iommu_reg_get64(RISCVIOMMUState *s,
                                             ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
    ਵਾਪਸ ldq_le_p(s->regs + idx);
}
#ਜੇ_ਅੰਤ
