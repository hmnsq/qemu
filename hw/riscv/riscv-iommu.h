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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HW_RISCV_IOMMU_STATE_H
#ವ್ಯಾಖ್ಯಾನಿಸು HW_RISCV_IOMMU_STATE_H

#ಅನ್ತರ್ಭಾವ "qom/object.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "system/dma.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/iommu.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv-iommu-bits.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ riscv_iommu_igs_modes riscv_iommu_igs_ಬಹುಲಕ;

ರಚನೆ RISCVIOMMUState {
    /*< private >*/
    DeviceState ಪೋಷಕ_obj;

    /*< public >*/
    uint32_t ಆವೃತ್ತಿ;     /* Reported interface version number */
    uint32_t pid_ಬಿಟ್ಟುಗಳು;    /* process identifier width */
    uint32_t pas_ಬಿಟ್ಟುಗಳು;    /* physical address bits */
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

    uint32_t cq_ಮುಖವಾಡ;     /* Command queue index bit mask */
    uint32_t fq_ಮುಖವಾಡ;     /* Fault/event queue index bit mask */
    uint32_t pq_ಮುಖವಾಡ;     /* Page request queue index bit mask */

    /* interrupt notifier */
    ಶೂನ್ಯ (*notify)(RISCVIOMMUState *iommu, ಚಿಹ್ನೆರಹಿತ ಸದಿಶ);

    /* IOMMU target address space */
    ವಿಳಾಸ_Space *ಗುರಿ_as;
    ಸ್ಮರಣೆ_Region *ಗುರಿ_mr;

    /* MSI / MRIF access trap */
    ವಿಳಾಸ_Space trap_as;
    ಸ್ಮರಣೆ_Region trap_mr;

    GHash_ಕೋಷ್ಟಕ *ctx_cache;          /* Device translation Context Cache */

    GHash_ಕೋಷ್ಟಕ *iot_cache;          /* IO Translated Address Cache */
    ಚಿಹ್ನೆರಹಿತ iot_ಎಲ್ಲೆ;             /* IO Translation Cache size limit */

    /* MMIO Hardware Interface */
    ಸ್ಮರಣೆ_Region regs_mr;
    uint8_t *regs;  /* current register state */
    uint8_t *regs_wc;  /* write-1-to-clear mask */
    /*
     * read-only mask. NOTE: bits not present in this RO
     * mask are assumed to be read and write.
     */
    uint8_t *regs_ro;

    QLIST_ನಮೂದು(RISCVIOMMUState) iommus;
    QLIST_ತಲೆ(, RISCVIOMMUSpace) spaces;

    /* HPM cycle counter */
    QEMUTimer *hpm_timer;
    uint64_t hpmcycle_val;      /* Current value of cycle register */
    uint64_t hpmcycle_prev;     /* Saved value of QEMU_CLOCK_VIRTUAL clock */
    uint64_t irq_ಅತಿಪ್ರವಾಹ_ಎಡ; /* Value beyond INT64_MAX after overflow */

    /* HPM event counters */
    GHash_ಕೋಷ್ಟಕ *hpm_event_ctr_map; /* Mapping of events to counters */
    uint8_t hpm_cntrs;
};

ಶೂನ್ಯ riscv_iommu_pci_ಸಿದ್ಧತೆ_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
         Error **errp);
ಶೂನ್ಯ riscv_iommu_ಗಣ_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_ಬಹುಲಕ ಬಹುಲಕ);
ಶೂನ್ಯ riscv_iommu_reset(RISCVIOMMUState *s);
ಶೂನ್ಯ riscv_iommu_notify(RISCVIOMMUState *s, ಪೂರ್ಣಾಂಕ vec_type);
ಶೂನ್ಯ riscv_iommu_fault(RISCVIOMMUState *s, ರಚನೆ riscv_iommu_fq_ದಾಖಲೆ *ev);
DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, ಪೂರ್ಣಾಂಕ ತಳ_irq,
                                    bool is_32_ಬಿಟ್);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ RISCVIOMMUContext RISCVIOMMUContext;
/* Device translation context state. */
ರಚನೆ RISCVIOMMUContext {
    uint64_t devid:24;          /* Requester Id, AKA device_id */
    uint64_t process_id:20;     /* Process ID. PASID for PCIe */
    uint64_t tc;                /* Translation Control */
    uint64_t ta;                /* Translation Attributes */
    uint64_t satp;              /* S-Stage address translation and protection */
    uint64_t gatp;              /* G-Stage address translation and protection */
    uint64_t msi_addr_ಮುಖವಾಡ;     /* MSI filtering - address mask */
    uint64_t msi_addr_ನಮೂನೆ;  /* MSI filtering - address pattern */
    uint64_t msiptp;            /* MSI redirection page table pointer */
};

/* private helpers */

/* Register helper functions */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t riscv_iommu_reg_mod32(RISCVIOMMUState *s,
    ಚಿಹ್ನೆರಹಿತ idx, uint32_t ಗಣ, uint32_t clr)
{
    uint32_t val = ldl_le_p(s->regs + idx);
    stl_le_p(s->regs + idx, (val & ~clr) | ಗಣ);
    ಹಿಂತಿರುಗಿ val;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ riscv_iommu_reg_set32(RISCVIOMMUState *s, ಚಿಹ್ನೆರಹಿತ idx,
                                         uint32_t ಗಣ)
{
    stl_le_p(s->regs + idx, ಗಣ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t riscv_iommu_reg_get32(RISCVIOMMUState *s, ಚಿಹ್ನೆರಹಿತ idx)
{
    ಹಿಂತಿರುಗಿ ldl_le_p(s->regs + idx);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t riscv_iommu_reg_mod64(RISCVIOMMUState *s, ಚಿಹ್ನೆರಹಿತ idx,
                                             uint64_t ಗಣ, uint64_t clr)
{
    uint64_t val = ldq_le_p(s->regs + idx);
    stq_le_p(s->regs + idx, (val & ~clr) | ಗಣ);
    ಹಿಂತಿರುಗಿ val;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ riscv_iommu_reg_set64(RISCVIOMMUState *s, ಚಿಹ್ನೆರಹಿತ idx,
                                         uint64_t ಗಣ)
{
    stq_le_p(s->regs + idx, ಗಣ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t riscv_iommu_reg_get64(RISCVIOMMUState *s,
                                             ಚಿಹ್ನೆರಹಿತ idx)
{
    ಹಿಂತಿರುಗಿ ldq_le_p(s->regs + idx);
}
#ಆದರೆ_ಅಂತ್ಯ
