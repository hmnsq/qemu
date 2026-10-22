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

#यदि_अपरिभाषितम् HW_RISCV_IOMMU_STATE_H
#परिभाषय HW_RISCV_IOMMU_STATE_H

#अन्तर्भाव "qom/object.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "system/dma.h"
#अन्तर्भाव "hw/riscv/iommu.h"
#अन्तर्भाव "hw/riscv/riscv-iommu-bits.h"

प्रकारनिर्देश गणना riscv_iommu_igs_modes riscv_iommu_igs_बहुलकम्;

संरचना RISCVIOMMUState {
    /*< private >*/
    DeviceState parent_obj;

    /*< public >*/
    uint32_t संस्करणम्;     /* Reported interface version number */
    uint32_t pid_द्वयङ्काः;    /* process identifier width */
    uint32_t pas_द्वयङ्काः;    /* physical address bits */
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

    uint32_t cq_आवरणम्;     /* Command queue index bit mask */
    uint32_t fq_आवरणम्;     /* Fault/event queue index bit mask */
    uint32_t pq_आवरणम्;     /* Page request queue index bit mask */

    /* interrupt notifier */
    शून्य (*notify)(RISCVIOMMUState *iommu, अचिह्नित सदिशम्);

    /* IOMMU target address space */
    स्थानाङ्कः_Space *target_as;
    स्मृतिः_Region *target_mr;

    /* MSI / MRIF access trap */
    स्थानाङ्कः_Space trap_as;
    स्मृतिः_Region trap_mr;

    GHash_सारणी *ctx_cache;          /* Device translation Context Cache */

    GHash_सारणी *iot_cache;          /* IO Translated Address Cache */
    अचिह्नित iot_सीमा;             /* IO Translation Cache size limit */

    /* MMIO Hardware Interface */
    स्मृतिः_Region regs_mr;
    uint8_t *regs;  /* current register state */
    uint8_t *regs_wc;  /* write-1-to-clear mask */
    /*
     * read-only mask. NOTE: bits not present in this RO
     * mask are assumed to be read and write.
     */
    uint8_t *regs_ro;

    QLIST_प्रविष्टिः(RISCVIOMMUState) iommus;
    QLIST_शीर्ष(, RISCVIOMMUSpace) spaces;

    /* HPM cycle counter */
    QEMUTimer *hpm_timer;
    uint64_t hpmcycle_val;      /* Current value of cycle register */
    uint64_t hpmcycle_पूर्व;     /* Saved value of QEMU_CLOCK_VIRTUAL clock */
    uint64_t irq_अतिप्रवाहः_वामः; /* Value beyond INT64_MAX after overflow */

    /* HPM event counters */
    GHash_सारणी *hpm_event_ctr_map; /* Mapping of events to counters */
    uint8_t hpm_cntrs;
};

शून्य riscv_iommu_pci_सज्जता_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
         Error **errp);
शून्य riscv_iommu_समुच्चयः_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_बहुलकम् बहुलकम्);
शून्य riscv_iommu_reset(RISCVIOMMUState *s);
शून्य riscv_iommu_notify(RISCVIOMMUState *s, पूर्णाङ्क vec_type);
शून्य riscv_iommu_fault(RISCVIOMMUState *s, संरचना riscv_iommu_fq_लेखः *ev);
DeviceState *riscv_create_iommu_sys(DeviceState *mmio_irqchip,
                                    hwaddr addr, पूर्णाङ्क आधारः_irq,
                                    bool is_32_bit);

प्रकारनिर्देश संरचना RISCVIOMMUContext RISCVIOMMUContext;
/* Device translation context state. */
संरचना RISCVIOMMUContext {
    uint64_t devid:24;          /* Requester Id, AKA device_id */
    uint64_t process_id:20;     /* Process ID. PASID for PCIe */
    uint64_t tc;                /* Translation Control */
    uint64_t ta;                /* Translation Attributes */
    uint64_t satp;              /* S-Stage address translation and protection */
    uint64_t gatp;              /* G-Stage address translation and protection */
    uint64_t msi_addr_आवरणम्;     /* MSI filtering - address mask */
    uint64_t msi_addr_आदर्शः;  /* MSI filtering - address pattern */
    uint64_t msiptp;            /* MSI redirection page table pointer */
};

/* private helpers */

/* Register helper functions */
स्थिर अन्तःस्थ uint32_t riscv_iommu_reg_mod32(RISCVIOMMUState *s,
    अचिह्नित idx, uint32_t समुच्चयः, uint32_t clr)
{
    uint32_t val = ldl_le_p(s->regs + idx);
    stl_le_p(s->regs + idx, (val & ~clr) | समुच्चयः);
    प्रत्यागम val;
}

स्थिर अन्तःस्थ शून्य riscv_iommu_reg_set32(RISCVIOMMUState *s, अचिह्नित idx,
                                         uint32_t समुच्चयः)
{
    stl_le_p(s->regs + idx, समुच्चयः);
}

स्थिर अन्तःस्थ uint32_t riscv_iommu_reg_get32(RISCVIOMMUState *s, अचिह्नित idx)
{
    प्रत्यागम ldl_le_p(s->regs + idx);
}

स्थिर अन्तःस्थ uint64_t riscv_iommu_reg_mod64(RISCVIOMMUState *s, अचिह्नित idx,
                                             uint64_t समुच्चयः, uint64_t clr)
{
    uint64_t val = ldq_le_p(s->regs + idx);
    stq_le_p(s->regs + idx, (val & ~clr) | समुच्चयः);
    प्रत्यागम val;
}

स्थिर अन्तःस्थ शून्य riscv_iommu_reg_set64(RISCVIOMMUState *s, अचिह्नित idx,
                                         uint64_t समुच्चयः)
{
    stq_le_p(s->regs + idx, समुच्चयः);
}

स्थिर अन्तःस्थ uint64_t riscv_iommu_reg_get64(RISCVIOMMUState *s,
                                             अचिह्नित idx)
{
    प्रत्यागम ldq_le_p(s->regs + idx);
}
#यदि_अन्तः
