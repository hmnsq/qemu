/*
 * QEMU emulation of an RISC-V IOMMU
 *
 * Copyright (C) 2021-2023, Rivos Inc.
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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qom/object.h"
#अन्तर्भाव "exec/target_page.h"
#अन्तर्भाव "hw/pci/pci_bus.h"
#अन्तर्भाव "hw/pci/pci_device.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "hw/riscv/riscv_hart.h"
#अन्तर्भाव "migration/vmstate.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/timer.h"
#अन्तर्भाव "qemu/target-info.h"
#अन्तर्भाव "qemu/bitops.h"

#अन्तर्भाव "cpu_bits.h"
#अन्तर्भाव "riscv-iommu.h"
#अन्तर्भाव "riscv-iommu-bits.h"
#अन्तर्भाव "riscv-iommu-hpm.h"
#अन्तर्भाव "trace.h"

#परिभाषय सीमा_CACHE_CTX               (1U << 7)
#परिभाषय सीमा_CACHE_IOT               (1U << 20)

/* Physical page number coversions */
#परिभाषय PPN_PHYS(ppn)                 ((ppn) << TARGET_PAGE_द्वयङ्काः)
#परिभाषय PPN_अधस्तात्(phy)                 ((phy) >> TARGET_PAGE_द्वयङ्काः)

प्रकारनिर्देश संरचना RISCVIOMMUEntry RISCVIOMMUEntry;

/* Device assigned I/O address space */
संरचना RISCVIOMMUSpace {
    IOMMUMemoryRegion iova_mr;  /* IOVA memory region for attached device */
    स्थानाङ्कः_Space iova_as;       /* IOVA address space for attached device */
    RISCVIOMMUState *iommu;     /* Managing IOMMU device state */
    PCIBus *bus;                /* PCI bus of the requester */
    uint8_t devfn;              /* Requester identifier, AKA device_id */
    bool notifier;              /* IOMMU unmap notifier enabled */
    QLIST_प्रविष्टिः(RISCVIOMMUSpace) list;
};

प्रकारनिर्देश गणना RISCVIOMMUTrans_चिह्नकम् {
    RISCV_IOMMU_TRANS_चिह्नकम्_BY,  /* Bypass */
    RISCV_IOMMU_TRANS_चिह्नकम्_SS,  /* Single Stage */
    RISCV_IOMMU_TRANS_चिह्नकम्_VG,  /* G-stage only */
    RISCV_IOMMU_TRANS_चिह्नकम्_VN,  /* Nested translation */
} RISCVIOMMUTrans_चिह्नकम्;

/* Address translation cache entry */
संरचना RISCVIOMMUEntry {
    RISCVIOMMUTrans_चिह्नकम् चिह्नकम्;     /* Translation Tag */
    uint64_t iova:44;           /* IOVA Page Number */
    uint64_t pscid:20;          /* Process Soft-Context identifier */
    uint64_t phys:44;           /* Physical Page Number */
    uint64_t gscid:16;          /* Guest Soft-Context identifier */
    uint64_t perm:2;            /* IOMMU_RW flags */
};

/* IOMMU index for transactions without process_id specified. */
#परिभाषय RISCV_IOMMU_NOPROCID 0

स्थिर uint32_t riscv_iommu_space_devid(RISCVIOMMUSpace *as)
{
    uint32_t devid = PCI_BUILD_BDF(pci_bus_अङ्कः(as->bus), as->devfn);

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= as->iommu->bus << 8;
    प्रत्यागम devid;
}

स्थिर uint8_t riscv_iommu_get_icvec_सदिशम्(uint32_t icvec, uint32_t vec_type)
{
    विकल्प (vec_type) {
    पक्ष RISCV_IOMMU_INTR_CQ:
        प्रत्यागम icvec & RISCV_IOMMU_ICVEC_CIV;
    पक्ष RISCV_IOMMU_INTR_FQ:
        प्रत्यागम (icvec & RISCV_IOMMU_ICVEC_FIV) >> 4;
    पक्ष RISCV_IOMMU_INTR_PM:
        प्रत्यागम (icvec & RISCV_IOMMU_ICVEC_PMIV) >> 8;
    पक्ष RISCV_IOMMU_INTR_PQ:
        प्रत्यागम (icvec & RISCV_IOMMU_ICVEC_PIV) >> 12;
    स्वभाव:
        g_assert_not_reached();
    }
}

शून्य riscv_iommu_notify(RISCVIOMMUState *s, पूर्णाङ्क vec_type)
{
    uint32_t ipsr, icvec, सदिशम्;

    यदि (!s->notify) {
        प्रत्यागम;
    }

    icvec = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_ICVEC);
    ipsr = riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, (1 << vec_type), 0);

    यदि (!(ipsr & (1 << vec_type))) {
        सदिशम् = riscv_iommu_get_icvec_सदिशम्(icvec, vec_type);
        s->notify(s, सदिशम्);
        trace_riscv_iommu_notify_int_सदिशम्(vec_type, सदिशम्);
    }
}

शून्य riscv_iommu_fault(RISCVIOMMUState *s, संरचना riscv_iommu_fq_लेखः *ev)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t शीर्ष = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQH) & s->fq_आवरणम्;
    uint32_t पुच्छ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQT) & s->fq_आवरणम्;
    uint32_t अग्र = (पुच्छ + 1) & s->fq_आवरणम्;
    uint32_t devid = get_क्षेत्रम्(ev->hdr, RISCV_IOMMU_FQ_HDR_DID);

    trace_riscv_iommu_flt(s->parent_obj.id, PCI_BUS_अङ्कः(devid), PCI_SLOT(devid),
                          PCI_FUNC(devid), ev->hdr, ev->iotval);

    यदि (!(ctrl & RISCV_IOMMU_FQCSR_FQON) ||
        !!(ctrl & (RISCV_IOMMU_FQCSR_FQOF | RISCV_IOMMU_FQCSR_FQMF))) {
        प्रत्यागम;
    }

    यदि (शीर्ष == अग्र) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                              RISCV_IOMMU_FQCSR_FQOF, 0);
    } अन्यथा {
        dma_addr_t addr = s->fq_addr + पुच्छ * परिमाण(*ev);
        यदि (dma_स्मृतिः_write(s->target_as, addr, ev, परिमाण(*ev),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                                  RISCV_IOMMU_FQCSR_FQMF, 0);
        } अन्यथा {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_FQT, अग्र);
        }
    }

    यदि (ctrl & RISCV_IOMMU_FQCSR_FIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_FQ);
    }
}

स्थिर शून्य riscv_iommu_pri(RISCVIOMMUState *s,
    संरचना riscv_iommu_pq_लेखः *pr)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t शीर्ष = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQH) & s->pq_आवरणम्;
    uint32_t पुच्छ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQT) & s->pq_आवरणम्;
    uint32_t अग्र = (पुच्छ + 1) & s->pq_आवरणम्;
    uint32_t devid = get_क्षेत्रम्(pr->hdr, RISCV_IOMMU_PREQ_HDR_DID);

    trace_riscv_iommu_pri(s->parent_obj.id, PCI_BUS_अङ्कः(devid), PCI_SLOT(devid),
                          PCI_FUNC(devid), pr->payload);

    यदि (!(ctrl & RISCV_IOMMU_PQCSR_PQON) ||
        !!(ctrl & (RISCV_IOMMU_PQCSR_PQOF | RISCV_IOMMU_PQCSR_PQMF))) {
        प्रत्यागम;
    }

    यदि (शीर्ष == अग्र) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                              RISCV_IOMMU_PQCSR_PQOF, 0);
    } अन्यथा {
        dma_addr_t addr = s->pq_addr + पुच्छ * परिमाण(*pr);
        यदि (dma_स्मृतिः_write(s->target_as, addr, pr, परिमाण(*pr),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                                  RISCV_IOMMU_PQCSR_PQMF, 0);
        } अन्यथा {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_PQT, अग्र);
        }
    }

    यदि (ctrl & RISCV_IOMMU_PQCSR_PIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_PQ);
    }
}

/*
 * Discards all bits from 'val' whose matching bits in the same
 * positions in the mask 'ext' are zeros, and packs the remaining
 * bits from 'val' contiguously at the least-significant end of the
 * result, keeping the same bit order as 'val' and filling any
 * other bits at the most-significant end of the result with zeros.
 *
 * For example, for the following 'val' and 'ext', the return 'ret'
 * will be:
 *
 * val = a b c d e f g h
 * ext = 1 0 1 0 0 1 1 0
 * ret = 0 0 0 0 a c f g
 *
 * This function, taken from the riscv-iommu 1.0 spec, section 2.3.3
 * "Process to translate addresses of MSIs", is similar to bit manip
 * function PEXT (Parallel bits extract) from x86.
 */
स्थिर uint64_t riscv_iommu_pext_u64(uint64_t val, uint64_t ext)
{
    uint64_t ret = 0;
    uint64_t rot = 1;

    यावत् (ext) {
        यदि (ext & 1) {
            यदि (val & 1) {
                ret |= rot;
            }
            rot <<= 1;
        }
        val >>= 1;
        ext >>= 1;
    }

    प्रत्यागम ret;
}

/* Check if GPA matches MSI/MRIF pattern. */
स्थिर bool riscv_iommu_msi_check(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    dma_addr_t gpa)
{
    यदि (!s->enable_msi) {
        प्रत्यागम false;
    }

    यदि (get_क्षेत्रम्(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_बहुलकम्) !=
        RISCV_IOMMU_DC_MSIPTP_बहुलकम्_FLAT) {
        प्रत्यागम false; /* Invalid MSI/MRIF mode */
    }

    यदि ((PPN_अधस्तात्(gpa) ^ ctx->msi_addr_आदर्शः) & ~ctx->msi_addr_आवरणम्) {
        प्रत्यागम false; /* GPA not in MSI range defined by AIA IMSIC rules. */
    }

    प्रत्यागम true;
}

/* Returns the NAPOT page mask, or 0 for reserved encodings. */
स्थिर hwaddr riscv_iommu_napot_page_आवरणम्(hwaddr ppn, hwaddr addr, hwaddr *बहिर्गमनम्)
{
    पूर्णाङ्क napot_द्वयङ्काः = ctz64(ppn) + 1;
    hwaddr napot_आवरणम्, page_आवरणम्;

    /* The spec only defines 64KB (napot_bits == 4) */
    यदि (napot_द्वयङ्काः != 4) {
        प्रत्यागम 0;
    }

    napot_आवरणम् = (1ULL << napot_द्वयङ्काः) - 1;
    page_आवरणम् = PPN_PHYS(napot_आवरणम्) | (TARGET_PAGE_आकार - 1);

    *बहिर्गमनम् = PPN_PHYS(ppn & ~napot_आवरणम्) | (addr & page_आवरणम्);

    प्रत्यागम page_आवरणम्;
}

/*
 * RISCV IOMMU Address Translation Lookup - Page Table Walk
 *
 * Note: Code is based on get_physical_address() from target/riscv/cpu_helper.c
 * Both implementation can be merged into single helper function in future.
 * Keeping them separate for now, as error reporting and flow specifics are
 * sufficiently different for separate implementation.
 *
 * @s        : IOMMU Device State
 * @ctx      : Translation context for device id and process address space id.
 * @iotlb    : translation data: physical address and access mode.
 * @return   : success or fault cause code.
 */
स्थिर पूर्णाङ्क riscv_iommu_spa_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb)
{
    IOMMUAccessFlags trans_perm = IOMMU_NONE;
    dma_addr_t addr, आधारः;
    uint64_t satp, gatp, pte;
    bool en_s, en_g;
    संरचना {
        अचिह्नित वर्ण चरणः;
        अचिह्नित वर्ण स्तराः;
        अचिह्नित वर्ण ptidxbits;
        अचिह्नित वर्ण ptesize;
    } sc[2];
    /* Translation stage phase */
    गणना {
        S_STAGE = 0,
        G_STAGE = 1,
    } pass;
    Mem_Tx_परिणाम ret;
    bool pv = !!ctx->process_id;
    /*
     * Keep the request permission separate from iotlb->perm.  G-stage
     * walks translate S-stage PTE addresses before the real leaf is
     * reached, but permission checks and fault types must still use the
     * original request.  A successful walk leaves iotlb->perm with the
     * effective leaf permission for the translation cache.
     */
    ध्रुव IOMMUAccessFlags req_perm = iotlb->perm;

    satp = get_क्षेत्रम्(ctx->satp, RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्);
    gatp = get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्);

    en_s = satp != RISCV_IOMMU_DC_FSC_बहुलकम्_BARE;
    en_g = gatp != RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE;

    /*
     * Early check for MSI address match when IOVA == GPA.
     * Note that the (!en_s) condition means that the MSI
     * page table may only be used when guest pages are
     * mapped using the g-stage page table, whether single-
     * or two-stage paging is enabled. It's unavoidable though,
     * because the spec mandates that we do a first-stage
     * translation before we check the MSI page table, which
     * means we can't do an early MSI check unless we have
     * strictly !en_s.
     */
    यदि (!en_s && (req_perm & IOMMU_WO) &&
        riscv_iommu_msi_check(s, ctx, iotlb->iova)) {
        iotlb->target_as = &s->trap_as;
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_आवरणम् = ~TARGET_PAGE_आवरणम्;
        प्रत्यागम 0;
    }

    /* Exit early for pass-through mode. */
    यदि (!(en_s || en_g)) {
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_आवरणम् = ~TARGET_PAGE_आवरणम्;
        /* Allow R/W in pass-through mode */
        iotlb->perm = IOMMU_RW;
        प्रत्यागम 0;
    }

    /* S/G translation parameters. */
    प्रति (pass = 0; pass < 2; pass++) {
        uint32_t sv_बहुलकम्;

        sc[pass].चरणः = 0;
        यदि (pass ? (s->fctl & RISCV_IOMMU_FCTL_GXL) :
            (ctx->tc & RISCV_IOMMU_DC_TC_SXL)) {
            /* 32bit mode for GXL/SXL == 1 */
            विकल्प (pass ? gatp : satp) {
            पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE:
                sc[pass].स्तराः    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV32_X4:
                sv_बहुलकम् = pass ? RISCV_IOMMU_CAP_SV32X4 : RISCV_IOMMU_CAP_SV32;
                यदि (!(s->cap & sv_बहुलकम्)) {
                    प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].स्तराः    = 2;
                sc[pass].ptidxbits = 10;
                sc[pass].ptesize   = 4;
                भङ्ग;
            स्वभाव:
                प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        } अन्यथा {
            /* 64bit mode for GXL/SXL == 0 */
            विकल्प (pass ? gatp : satp) {
            पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE:
                sc[pass].स्तराः    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV39_X4:
                sv_बहुलकम् = pass ? RISCV_IOMMU_CAP_SV39X4 : RISCV_IOMMU_CAP_SV39;
                यदि (!(s->cap & sv_बहुलकम्)) {
                    प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].स्तराः    = 3;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV48_X4:
                sv_बहुलकम् = pass ? RISCV_IOMMU_CAP_SV48X4 : RISCV_IOMMU_CAP_SV48;
                यदि (!(s->cap & sv_बहुलकम्)) {
                    प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].स्तराः    = 4;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV57_X4:
                sv_बहुलकम् = pass ? RISCV_IOMMU_CAP_SV57X4 : RISCV_IOMMU_CAP_SV57;
                यदि (!(s->cap & sv_बहुलकम्)) {
                    प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].स्तराः    = 5;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                भङ्ग;
            स्वभाव:
                प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        }
    };

    /* S/G stages translation tables root pointers */
    gatp = PPN_PHYS(get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_ATP_PPN_क्षेत्रम्));
    satp = PPN_PHYS(get_क्षेत्रम्(ctx->satp, RISCV_IOMMU_ATP_PPN_क्षेत्रम्));
    addr = (en_s && en_g) ? satp : iotlb->iova;
    आधारः = en_g ? gatp : satp;
    pass = en_g ? G_STAGE : S_STAGE;

    कुरु {
        ध्रुव अचिह्नित widened = (pass && !sc[pass].चरणः) ? 2 : 0;
        ध्रुव अचिह्नित va_द्वयङ्काः = widened + sc[pass].ptidxbits;
        ध्रुव अचिह्नित va_skip = TARGET_PAGE_द्वयङ्काः + sc[pass].ptidxbits *
                                 (sc[pass].स्तराः - 1 - sc[pass].चरणः);
        ध्रुव अचिह्नित idx = (addr >> va_skip) & ((1 << va_द्वयङ्काः) - 1);
        ध्रुव dma_addr_t pte_addr = आधारः + idx * sc[pass].ptesize;
        ध्रुव bool ade =
            ctx->tc & (pass ? RISCV_IOMMU_DC_TC_GADE : RISCV_IOMMU_DC_TC_SADE);

        यदि (ade && !(s->cap & RISCV_IOMMU_CAP_AMO_HWAD)) {
            /* GADE/SADE are reserved bits if AMO_HWAD is cleared.  */
            प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }

        /* Address range check before first level lookup */
        यदि (!sc[pass].चरणः) {
            ध्रुव uint64_t va_len = va_skip + va_द्वयङ्काः;
            ध्रुव uint64_t va_आवरणम् = (1ULL << va_len) - 1;

            यदि (pass == S_STAGE && va_len > 32) {
                uint64_t आवरणम्, masked_msbs;

                आवरणम् = MAKE_64_BIT_आवरणम्(0, target_long_द्वयङ्काः() - va_len + 1);
                masked_msbs = (addr >> (va_len - 1)) & आवरणम्;

                यदि (masked_msbs != 0 && masked_msbs != आवरणम्) {
                    प्रत्यागम (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S;
                }
            } अन्यथा {
                यदि ((addr & va_आवरणम्) != addr) {
                    प्रत्यागम (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS;
                }
            }
        }


        यदि (pass == S_STAGE) {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_S_VS_WALKS);
        } अन्यथा {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_G_WALKS);
        }

        /* Read page table entry */
        यदि (sc[pass].ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->target_as, pte_addr, &pte32,
                             MEMTXATTRS_UNSPECIFIED);
            pte = pte32;
        } अन्यथा {
            ret = ldq_le_dma(s->target_as, pte_addr, &pte,
                             MEMTXATTRS_UNSPECIFIED);
        }
        यदि (ret != MEMTX_OK) {
            प्रत्यागम (req_perm & IOMMU_WO) ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT
                                         : RISCV_IOMMU_FQ_CAUSE_RD_FAULT;
        }

        sc[pass].चरणः++;
        hwaddr ppn = pte >> PTE_PPN_सरणम्;

        यदि (!(pte & PTE_V)) {
            भङ्ग;                /* Invalid PTE */
        } अन्यथा यदि (pte & PTE_RESERVED(false)) {
            भङ्ग;                /* Reserved PTE bits set */
        } अन्यथा यदि (!(pte & (PTE_R | PTE_W | PTE_X))) {
            आधारः = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } अन्यथा यदि ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            भङ्ग;                /* Reserved leaf PTE flags: PTE_W */
        } अन्यथा यदि ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            भङ्ग;                /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } अन्यथा यदि (ppn & ((1ULL << (va_skip - TARGET_PAGE_द्वयङ्काः)) - 1)) {
            भङ्ग;                /* Misaligned PPN */
        } अन्यथा यदि (!(pte & PTE_U) && !pv) {
            /*
             * All accesses are assumed to be User mode unless
             * process_id is valid (pv).  In case we have a
             * non-user mode leaf PTE and !pv we need to fault.
             */
            भङ्ग;
        } अन्यथा यदि ((req_perm & IOMMU_RO) && !(pte & PTE_R)) {
            भङ्ग;                /* Read access check failed */
        } अन्यथा यदि ((req_perm & IOMMU_WO) && !(pte & PTE_W)) {
            भङ्ग;                /* Write access check failed */
        } अन्यथा यदि (!ade && !(pte & PTE_A)) {
            भङ्ग;                /* Access bit not set */
        } अन्यथा यदि ((req_perm & IOMMU_WO) && !ade && !(pte & PTE_D)) {
            भङ्ग;                /* Dirty bit not set */
        } अन्यथा यदि (pass == G_STAGE && !(pte & PTE_U)) {
            /*
             * riscv-iommu spec 1.0: "When checking the U bit in a
             * second-stage PTE, the transaction is treated as
             * not requesting supervisor privilege."
             *
             * I.e. we need to fault if this is a non-user PTE since
             * we are always in user mode at this point.
             */
            भङ्ग;
        } अन्यथा {
            /* Leaf PTE, translation completed. */
            sc[pass].चरणः = sc[pass].स्तराः;

            यदि (pte & PTE_N) {
                hwaddr आवरणम् = riscv_iommu_napot_page_आवरणम्(ppn, addr, &आधारः);

                यदि (!आवरणम्) {
                    भङ्ग;
                }
                iotlb->addr_आवरणम् &= आवरणम्;
            } अन्यथा {
                आधारः = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
                /* Update address mask based on smallest translation granularity */
                iotlb->addr_आवरणम् &= (1ULL << va_skip) - 1;
            }

            /* Continue with S-Stage translation? */
            यदि (pass && sc[0].चरणः != sc[0].स्तराः) {
                pass = S_STAGE;
                addr = iotlb->iova;
                अनुवर्त;
            }

            /* Cache the effective permission, not this request's subset. */
            IOMMUAccessFlags पर्णम्_perm = (pte & PTE_W) ?
                                         ((pte & PTE_R) ? IOMMU_RW : IOMMU_WO) :
                                         IOMMU_RO;

            trans_perm = trans_perm == IOMMU_NONE ?
                         पर्णम्_perm : trans_perm & पर्णम्_perm;

            /* Translation phase completed (GPA or SPA) */
            iotlb->translated_addr = आधारः;

            /* Check MSI GPA address match */
            यदि (pass == S_STAGE && (req_perm & IOMMU_WO) &&
                riscv_iommu_msi_check(s, ctx, आधारः)) {
                /* Trap MSI writes and return GPA address. */
                iotlb->target_as = &s->trap_as;
                iotlb->addr_आवरणम् = ~TARGET_PAGE_आवरणम्;
                प्रत्यागम 0;
            }

            /* Continue with G-Stage translation? */
            यदि (!pass && en_g) {
                pass = G_STAGE;
                addr = आधारः;
                आधारः = gatp;
                sc[pass].चरणः = 0;
                अनुवर्त;
            }

            iotlb->perm = trans_perm;
            प्रत्यागम 0;
        }

        यदि (sc[pass].चरणः == sc[pass].स्तराः) {
            भङ्ग; /* Can't find leaf PTE */
        }

        /* Continue with G-Stage translation? */
        यदि (!pass && en_g) {
            pass = G_STAGE;
            addr = आधारः;
            आधारः = gatp;
            sc[pass].चरणः = 0;
        }
    } यावत् (1);

    /*
     * riscv_iommu_translate() will receive a fault and then call
     * riscv_iommu_report_fault() using iotlb->translated_addr
     * as iotval2.  Update translated_addr it with the latest
     * translated addr we have.
     */
    iotlb->translated_addr = addr;

    प्रत्यागम (req_perm & IOMMU_WO) ?
                (pass ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S) :
                (pass ? RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S);
}

स्थिर शून्य riscv_iommu_report_fault(RISCVIOMMUState *s,
                                     RISCVIOMMUContext *ctx,
                                     uint32_t fault_type, uint32_t cause,
                                     bool pv,
                                     uint64_t iotval, uint64_t iotval2)
{
    संरचना riscv_iommu_fq_लेखः ev = { 0 };

    यदि (ctx->tc & RISCV_IOMMU_DC_TC_DTF) {
        विकल्प (cause) {
        पक्ष RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED:
        पक्ष RISCV_IOMMU_FQ_CAUSE_DDT_आरोपय_FAULT:
        पक्ष RISCV_IOMMU_FQ_CAUSE_DDT_INVALID:
        पक्ष RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED:
        पक्ष RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED:
        पक्ष RISCV_IOMMU_FQ_CAUSE_आन्तरिकम्_DP_ERROR:
        पक्ष RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT:
            भङ्ग;
        स्वभाव:
            /* DTF prevents reporting a fault for this given cause */
            प्रत्यागम;
        }
    }

    ev.hdr = समुच्चयः_क्षेत्रम्(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE, cause);
    ev.hdr = समुच्चयः_क्षेत्रम्(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE, fault_type);
    ev.hdr = समुच्चयः_क्षेत्रम्(ev.hdr, RISCV_IOMMU_FQ_HDR_DID, ctx->devid);
    ev.hdr = समुच्चयः_क्षेत्रम्(ev.hdr, RISCV_IOMMU_FQ_HDR_PV, pv);

    यदि (pv) {
        ev.hdr = समुच्चयः_क्षेत्रम्(ev.hdr, RISCV_IOMMU_FQ_HDR_PID, ctx->process_id);
    }

    ev.iotval = iotval;
    ev.iotval2 = iotval2;

    riscv_iommu_fault(s, &ev);
}

/* Redirect MSI write for given GPA. */
स्थिर Mem_Tx_परिणाम riscv_iommu_msi_write(RISCVIOMMUState *s,
    RISCVIOMMUContext *ctx, uint64_t gpa, uint64_t दत्त,
    अचिह्नित आकार, MemTxAttrs attrs)
{
    Mem_Tx_परिणाम res;
    dma_addr_t addr;
    uint64_t intn;
    आकार_t विस्थापनम्;
    uint32_t n190;
    uint64_t pte[2];
    पूर्णाङ्क fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    पूर्णाङ्क cause;

    /* Interrupt File Number */
    intn = riscv_iommu_pext_u64(PPN_अधस्तात्(gpa), ctx->msi_addr_आवरणम्);
    विस्थापनम् = intn * परिमाण(pte);

    /* fetch MSI PTE */
    addr = PPN_PHYS(get_क्षेत्रम्(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_PPN));
    यदि (addr & विस्थापनम्) {
        /* Interrupt file number out of range */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_आरोपय_FAULT;
        गच्छ err;
    }

    addr |= विस्थापनम्;
    res = dma_स्मृतिः_read(s->target_as, addr, &pte, परिमाण(pte),
            MEMTXATTRS_UNSPECIFIED);
    यदि (res != MEMTX_OK) {
        यदि (res == MEMTX_DECODE_ERROR) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED;
        } अन्यथा {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_आरोपय_FAULT;
        }
        गच्छ err;
    }

    le64_गन्तव्यम्_cpus(&pte[0]);
    le64_गन्तव्यम्_cpus(&pte[1]);

    यदि (!(pte[0] & RISCV_IOMMU_MSI_PTE_V) || (pte[0] & RISCV_IOMMU_MSI_PTE_C)) {
        /*
         * The spec mentions that: "If msipte.C == 1, then further
         * processing to interpret the PTE is implementation
         * defined.". We'll abort with cause = 262 for this
         * case too.
         */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_INVALID;
        गच्छ err;
    }

    विकल्प (get_क्षेत्रम्(pte[0], RISCV_IOMMU_MSI_PTE_M)) {
    पक्ष RISCV_IOMMU_MSI_PTE_M_BASIC:
        /*
         * riscv-iommu spec MSI PTE basic translate mode:
         * "When an MSI PTE has fields V = 1, C = 0, and M = 3
         *  (basic translate mode), the PTE's complete format is:
         *  First doubleword:  bit 63      C, = 0
         *                     bits 53:10  PPN
         *                     bits 2:1    M, = 3
         *                     bit 0       V, = 1
         *  All other bits of the first doubleword are reserved
         *  and must be set to zeros by software.  The second
         *  doubleword is ignored by an IOMMU so is free for
         *  software to use."
         *
         * In other words, bits 62:54 and 9:3 of pte[0] are reserved.
         */
        यदि (pte[0] & (GENMASK_ULL(62, 54) | GENMASK_ULL(9, 3))) {
            res = MEMTX_DECODE_ERROR;
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
            गच्छ err;
        }

        /* MSI Pass-through mode */
        addr = PPN_PHYS(get_क्षेत्रम्(pte[0], RISCV_IOMMU_MSI_PTE_PPN));

        trace_riscv_iommu_msi(s->parent_obj.id, PCI_BUS_अङ्कः(ctx->devid),
                              PCI_SLOT(ctx->devid), PCI_FUNC(ctx->devid),
                              gpa, addr);

        res = dma_स्मृतिः_write(s->target_as, addr, &दत्त, आकार, attrs);
        यदि (res != MEMTX_OK) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
            गच्छ err;
        }

        प्रत्यागम MEMTX_OK;
    पक्ष RISCV_IOMMU_MSI_PTE_M_MRIF:
        /* MRIF mode, continue. */
        भङ्ग;
    स्वभाव:
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        गच्छ err;
    }

    /*
     * Report an error for interrupt identities exceeding the maximum allowed
     * for an IMSIC interrupt file (2047) or destination address is not 32-bit
     * aligned. See IOMMU Specification, Chapter 2.3. MSI page tables.
     */
    यदि ((दत्त > 2047) || (gpa & 3)) {
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        गच्छ err;
    }

    /* MSI MRIF mode, non atomic pending bit update */

    /* MRIF pending bit address */
    addr = get_क्षेत्रम्(pte[0], RISCV_IOMMU_MSI_PTE_MRIF_ADDR) << 9;
    /*
     * AIA spec section "Format of a memory-resident interrupt file":
     * address offset 0x000 contains interrupt-pending bits for
     * identities 1-63, offfset 0x010 for identities 64-127, and
     * so it goes up to 0x1F0 for identities 1984-2047.
     *
     * Hence each batch of identities advances offset by 16 (0x010)
     * for every interrupt-pending bits.  This means that doing
     * (data & 0x7c0) will filter out the first 6 bits, then
     * a >> 2 will turn the result in the 0x10 steps we need.
     *
     * E.g:
     *
     * - (1-63 & 0x7c0) = 0, 0 >> 2 = 0, offset 0x000
     * - (64-127 & 0x7c0) = 64, 64 >> 2 = 16, offset 0x010
     * - (128-191 & 0x7c0) = 128, 128 >> 2 = 32, offset 0x020
     *
     * and so on.
     */
    addr = addr | ((दत्त & 0x7c0) >> 2);

    trace_riscv_iommu_msi(s->parent_obj.id, PCI_BUS_अङ्कः(ctx->devid),
                          PCI_SLOT(ctx->devid), PCI_FUNC(ctx->devid),
                          gpa, addr);

    /* MRIF pending bit mask */
    दत्त = 1ULL << (दत्त & 0x03f);
    res = dma_स्मृतिः_read(s->target_as, addr, &intn, परिमाण(intn), attrs);
    यदि (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_आरोपय_FAULT;
        गच्छ err;
    }

    intn = intn | दत्त;
    res = dma_स्मृतिः_write(s->target_as, addr, &intn, परिमाण(intn), attrs);
    यदि (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        गच्छ err;
    }

    /* Get MRIF enable bits */
    addr = addr + परिमाण(intn);
    res = dma_स्मृतिः_read(s->target_as, addr, &intn, परिमाण(intn), attrs);
    यदि (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_आरोपय_FAULT;
        गच्छ err;
    }

    यदि (!(intn & दत्त)) {
        /* notification disabled, MRIF update completed. */
        प्रत्यागम MEMTX_OK;
    }

    /* Send notification message */
    addr = PPN_PHYS(get_क्षेत्रम्(pte[1], RISCV_IOMMU_MSI_MRIF_NPPN));
    n190 = get_क्षेत्रम्(pte[1], RISCV_IOMMU_MSI_MRIF_NID) |
          (get_क्षेत्रम्(pte[1], RISCV_IOMMU_MSI_MRIF_NID_MSB) << 10);

    res = dma_स्मृतिः_write(s->target_as, addr, &n190, परिमाण(n190), attrs);
    यदि (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        गच्छ err;
    }

    trace_riscv_iommu_mrif_notification(s->parent_obj.id, n190, addr);

    प्रत्यागम MEMTX_OK;

err:
    riscv_iommu_report_fault(s, ctx, fault_type, cause,
                             !!ctx->process_id, 0, 0);
    प्रत्यागम res;
}

/*
 * Check device context configuration as described by the
 * riscv-iommu spec section "Device-context configuration
 * checks".
 */
स्थिर bool riscv_iommu_validate_device_ctx(RISCVIOMMUState *s,
                                            RISCVIOMMUContext *ctx)
{
    uint32_t fsc_बहुलकम्, msi_बहुलकम्;
    uint64_t gatp;

    यदि (ctx->tc & RISCV_IOMMU_DC_TC_RESERVED) {
        प्रत्यागम false;
    }

    यदि (!(s->cap & RISCV_IOMMU_CAP_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI ||
         ctx->tc & RISCV_IOMMU_DC_TC_PRPR)) {
        प्रत्यागम false;
    }

    यदि (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI)) {
        प्रत्यागम false;
    }

    यदि (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI) &&
        ctx->tc & RISCV_IOMMU_DC_TC_PRPR) {
        प्रत्यागम false;
    }

    यदि (!(s->cap & RISCV_IOMMU_CAP_T2GPA) &&
        ctx->tc & RISCV_IOMMU_DC_TC_T2GPA) {
        प्रत्यागम false;
    }

    यदि (s->cap & RISCV_IOMMU_CAP_MSI_FLAT) {
        msi_बहुलकम् = get_क्षेत्रम्(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_बहुलकम्);

        यदि (msi_बहुलकम् != RISCV_IOMMU_DC_MSIPTP_बहुलकम्_OFF &&
            msi_बहुलकम् != RISCV_IOMMU_DC_MSIPTP_बहुलकम्_FLAT) {
            प्रत्यागम false;
        }
    }

    gatp = get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्);
    यदि (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA &&
        gatp == RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE) {
        प्रत्यागम false;
    }

    यदि (gatp != RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE) {
        uint64_t iohgatp_ppn = get_क्षेत्रम्(ctx->gatp,
                                         RISCV_IOMMU_DC_IOHGATP_PPN);

        /*
         * One of the conditions for a misconfigured DDT entry
         * according to the riscv-spec: "DC.iohgatp.MODE is not
         * Bare and the root page table (address) determined by
         * DC.iohgatp.PPN is not aligned to a 16-KiB boundary."
         */
        यदि (PPN_PHYS(iohgatp_ppn) & ((1ULL << 14) - 1)) {
            प्रत्यागम false;
        }
    }

    fsc_बहुलकम् = get_क्षेत्रम्(ctx->satp, RISCV_IOMMU_DC_FSC_बहुलकम्);

    यदि (ctx->tc & RISCV_IOMMU_DC_TC_PDTV) {
        विकल्प (fsc_बहुलकम्) {
        पक्ष RISCV_IOMMU_DC_FSC_PDTP_बहुलकम्_PD8:
            यदि (!(s->cap & RISCV_IOMMU_CAP_PD8)) {
                प्रत्यागम false;
            }
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_FSC_PDTP_बहुलकम्_PD17:
            यदि (!(s->cap & RISCV_IOMMU_CAP_PD17)) {
                प्रत्यागम false;
            }
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_FSC_PDTP_बहुलकम्_PD20:
            यदि (!(s->cap & RISCV_IOMMU_CAP_PD20)) {
                प्रत्यागम false;
            }
            भङ्ग;
        }
    } अन्यथा {
        /* DC.tc.PDTV is 0 */
        यदि (ctx->tc & RISCV_IOMMU_DC_TC_DPE) {
            प्रत्यागम false;
        }

        यदि (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
            यदि (fsc_बहुलकम् == RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV32 &&
                !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                प्रत्यागम false;
            }
        } अन्यथा {
            विकल्प (fsc_बहुलकम्) {
            पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV39:
                यदि (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                    प्रत्यागम false;
                }
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV48:
                यदि (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                    प्रत्यागम false;
                }
            भङ्ग;
            पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV57:
                यदि (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                    प्रत्यागम false;
                }
                भङ्ग;
            }
        }
    }

    /*
     * CAP_END is always zero (only one endianess). FCTL_BE is
     * always zero (little-endian accesses). Thus TC_SBE must
     * always be LE, i.e. zero.
     */
    यदि (ctx->tc & RISCV_IOMMU_DC_TC_SBE) {
        प्रत्यागम false;
    }

    प्रत्यागम true;
}

/*
 * Validate process context (PC) according to section
 * "Process-context configuration checks".
 */
स्थिर bool riscv_iommu_validate_process_ctx(RISCVIOMMUState *s,
                                             RISCVIOMMUContext *ctx)
{
    uint32_t बहुलकम्;

    यदि (get_क्षेत्रम्(ctx->ta, RISCV_IOMMU_PC_TA_RESERVED)) {
        प्रत्यागम false;
    }

    यदि (get_क्षेत्रम्(ctx->satp, RISCV_IOMMU_PC_FSC_RESERVED)) {
        प्रत्यागम false;
    }

    बहुलकम् = get_क्षेत्रम्(ctx->satp, RISCV_IOMMU_DC_FSC_बहुलकम्);
    विकल्प (बहुलकम्) {
    पक्ष RISCV_IOMMU_DC_FSC_बहुलकम्_BARE:
    /* sv39 and sv32 modes have the same value (8) */
    पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV39:
    पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV48:
    पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV57:
        भङ्ग;
    स्वभाव:
        प्रत्यागम false;
    }

    यदि (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
        यदि (बहुलकम् == RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV32 &&
            !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                प्रत्यागम false;
        }
    } अन्यथा {
        विकल्प (बहुलकम्) {
        पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV39:
            यदि (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                प्रत्यागम false;
            }
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV48:
            यदि (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                प्रत्यागम false;
            }
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV57:
            यदि (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                प्रत्यागम false;
            }
            भङ्ग;
        }
    }

    प्रत्यागम true;
}

/**
 * pdt_memory_read: PDT wrapper of dma_memory_read.
 *
 * @s: IOMMU Device State
 * @ctx: Device Translation Context with devid and pasid set
 * @addr: address within that address space
 * @buf: buffer with the data transferred
 * @len: length of the data transferred
 * @attrs: memory transaction attributes
 */
स्थिर Mem_Tx_परिणाम pdt_स्मृतिः_read(RISCVIOMMUState *s,
                                   RISCVIOMMUContext *ctx,
                                   dma_addr_t addr,
                                   शून्य *buf, dma_addr_t len,
                                   MemTxAttrs attrs)
{
    uint64_t gatp_बहुलकम्, pte;
    संरचना {
        अचिह्नित वर्ण चरणः;
        अचिह्नित वर्ण स्तराः;
        अचिह्नित वर्ण ptidxbits;
        अचिह्नित वर्ण ptesize;
    } sc;
    Mem_Tx_परिणाम ret;
    dma_addr_t आधारः = addr;

    /* G stages translation mode */
    gatp_बहुलकम् = get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्);
    यदि (gatp_बहुलकम् == RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE) {
        गच्छ बहिर्गमनम्;
    }

    /* G stages translation tables root pointer */
    आधारः = PPN_PHYS(get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_ATP_PPN_क्षेत्रम्));

    /* Start at step 0 */
    sc.चरणः = 0;

    यदि (s->fctl & RISCV_IOMMU_FCTL_GXL) {
        /* 32bit mode for GXL == 1 */
        विकल्प (gatp_बहुलकम्) {
        पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV32_X4:
            यदि (!(s->cap & RISCV_IOMMU_CAP_SV32X4)) {
                प्रत्यागम MEMTX_ACCESS_ERROR;
            }
            sc.स्तराः    = 2;
            sc.ptidxbits = 10;
            sc.ptesize   = 4;
            भङ्ग;
        स्वभाव:
            प्रत्यागम MEMTX_ACCESS_ERROR;
        }
    } अन्यथा {
        /* 64bit mode for GXL == 0 */
        विकल्प (gatp_बहुलकम्) {
        पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV39_X4:
            यदि (!(s->cap & RISCV_IOMMU_CAP_SV39X4)) {
                प्रत्यागम MEMTX_ACCESS_ERROR;
            }
            sc.स्तराः    = 3;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV48_X4:
            यदि (!(s->cap & RISCV_IOMMU_CAP_SV48X4)) {
                प्रत्यागम MEMTX_ACCESS_ERROR;
            }
            sc.स्तराः    = 4;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV57_X4:
            यदि (!(s->cap & RISCV_IOMMU_CAP_SV57X4)) {
                प्रत्यागम MEMTX_ACCESS_ERROR;
            }
            sc.स्तराः    = 5;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            भङ्ग;
        स्वभाव:
            प्रत्यागम MEMTX_ACCESS_ERROR;
        }
    }

    कुरु {
        ध्रुव अचिह्नित va_द्वयङ्काः = (sc.चरणः ? 0 : 2) + sc.ptidxbits;
        ध्रुव अचिह्नित va_skip = TARGET_PAGE_द्वयङ्काः + sc.ptidxbits *
                                 (sc.स्तराः - 1 - sc.चरणः);
        ध्रुव अचिह्नित idx = (addr >> va_skip) & ((1 << va_द्वयङ्काः) - 1);
        ध्रुव dma_addr_t pte_addr = आधारः + idx * sc.ptesize;

        /* Address range check before first level lookup */
        यदि (!sc.चरणः) {
            ध्रुव uint64_t va_आवरणम् = (1ULL << (va_skip + va_द्वयङ्काः)) - 1;
            यदि ((addr & va_आवरणम्) != addr) {
                प्रत्यागम MEMTX_ACCESS_ERROR;
            }
        }

        /* Read page table entry */
        यदि (sc.ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->target_as, pte_addr, &pte32, attrs);
            pte = pte32;
        } अन्यथा {
            ret = ldq_le_dma(s->target_as, pte_addr, &pte, attrs);
        }
        यदि (ret != MEMTX_OK) {
            प्रत्यागम ret;
        }

        sc.चरणः++;
        hwaddr ppn = pte >> PTE_PPN_सरणम्;

        यदि (!(pte & PTE_V)) {
            प्रत्यागम MEMTX_ACCESS_ERROR; /* Invalid PTE */
        } अन्यथा यदि (!(pte & (PTE_R | PTE_W | PTE_X))) {
            आधारः = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } अन्यथा यदि ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            प्रत्यागम MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W */
        } अन्यथा यदि ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            प्रत्यागम MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } अन्यथा यदि (ppn & ((1ULL << (va_skip - TARGET_PAGE_द्वयङ्काः)) - 1)) {
            प्रत्यागम MEMTX_ACCESS_ERROR; /* Misaligned PPN */
        } अन्यथा {
            /* Leaf PTE, translation completed. */
            यदि (pte & PTE_N) {
                यदि (!riscv_iommu_napot_page_आवरणम्(ppn, addr, &आधारः)) {
                    प्रत्यागम MEMTX_ACCESS_ERROR;
                }
            } अन्यथा {
                आधारः = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
            }
            भङ्ग;
        }

        यदि (sc.चरणः == sc.स्तराः) {
            प्रत्यागम MEMTX_ACCESS_ERROR; /* Can't find leaf PTE */
        }
    } यावत् (1);

बहिर्गमनम्:
    प्रत्यागम dma_स्मृतिः_read(s->target_as, आधारः, buf, len, attrs);
}

/*
 * RISC-V IOMMU Device Context Loopkup - Device Directory Tree Walk
 *
 * @s         : IOMMU Device State
 * @ctx       : Device Translation Context with devid and process_id set.
 * @return    : success or fault code.
 */
स्थिर पूर्णाङ्क riscv_iommu_ctx_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx)
{
    ध्रुव uint64_t ddtp = s->ddtp;
    अचिह्नित बहुलकम् = get_क्षेत्रम्(ddtp, RISCV_IOMMU_DDTP_बहुलकम्);
    dma_addr_t addr = PPN_PHYS(get_क्षेत्रम्(ddtp, RISCV_IOMMU_DDTP_PPN));
    संरचना riscv_iommu_dc dc;
    /* Device Context format: 0: extended (64 bytes) | 1: base (32 bytes) */
    ध्रुव पूर्णाङ्क dc_fmt = !s->enable_msi;
    ध्रुव आकार_t dc_len = परिमाण(dc) >> dc_fmt;
    पूर्णाङ्क गभीरता;
    uint64_t de;

    विकल्प (बहुलकम्) {
    पक्ष RISCV_IOMMU_DDTP_बहुलकम्_OFF:
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED;

    पक्ष RISCV_IOMMU_DDTP_बहुलकम्_BARE:
        /* mock up pass-through translation context */
        ctx->gatp = समुच्चयः_क्षेत्रम्(0, RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्,
            RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE);
        ctx->satp = समुच्चयः_क्षेत्रम्(0, RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्,
            RISCV_IOMMU_DC_FSC_बहुलकम्_BARE);

        ctx->tc = RISCV_IOMMU_DC_TC_V;
        यदि (s->enable_ats) {
            ctx->tc |= RISCV_IOMMU_DC_TC_EN_ATS;
        }

        ctx->ta = 0;
        ctx->msiptp = 0;
        प्रत्यागम 0;

    पक्ष RISCV_IOMMU_DDTP_बहुलकम्_1_LVL:
        गभीरता = 0;
        भङ्ग;

    पक्ष RISCV_IOMMU_DDTP_बहुलकम्_2_LVL:
        गभीरता = 1;
        भङ्ग;

    पक्ष RISCV_IOMMU_DDTP_बहुलकम्_3_LVL:
        गभीरता = 2;
        भङ्ग;

    स्वभाव:
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /*
     * Check supported device id width (in bits).
     * See IOMMU Specification, Chapter 6. Software guidelines.
     * - if extended device-context format is used:
     *   1LVL: 6, 2LVL: 15, 3LVL: 24
     * - if base device-context format is used:
     *   1LVL: 7, 2LVL: 16, 3LVL: 24
     */
    यदि (ctx->devid >= (1 << (गभीरता * 9 + 6 + (dc_fmt && गभीरता != 2)))) {
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
    }

    /* Device directory tree walk */
    प्रति (; गभीरता-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);
        /*
         * Select device id index bits based on device directory tree level
         * and device context format.
         * See IOMMU Specification, Chapter 2. Data Structures.
         * - if extended device-context format is used:
         *   device index: [23:15][14:6][5:0]
         * - if base device-context format is used:
         *   device index: [23:16][15:7][6:0]
         */
        ध्रुव पूर्णाङ्क विभज = गभीरता * 9 + 6 + dc_fmt;
        addr |= ((ctx->devid >> विभज) << 3) & ~TARGET_PAGE_आवरणम्;
        यदि (dma_स्मृतिः_read(s->target_as, addr, &de, परिमाण(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_आरोपय_FAULT;
        }
        le64_गन्तव्यम्_cpus(&de);
        यदि (!(de & RISCV_IOMMU_DDTE_VALID)) {
            /* invalid directory entry */
            प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        यदि (de & ~(RISCV_IOMMU_DDTE_PPN | RISCV_IOMMU_DDTE_VALID)) {
            /* reserved bits set */
            प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }
        addr = PPN_PHYS(get_क्षेत्रम्(de, RISCV_IOMMU_DDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);

    /* index into device context entry page */
    addr |= (ctx->devid * dc_len) & ~TARGET_PAGE_आवरणम्;

    स्मृतिं_पूरय(&dc, 0, परिमाण(dc));
    यदि (dma_स्मृतिः_read(s->target_as, addr, &dc, dc_len,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_आरोपय_FAULT;
    }

    /* Set translation context. */
    ctx->tc = le64_गन्तव्यम्_cpu(dc.tc);
    ctx->gatp = le64_गन्तव्यम्_cpu(dc.iohgatp);
    ctx->satp = le64_गन्तव्यम्_cpu(dc.fsc);
    ctx->ta = le64_गन्तव्यम्_cpu(dc.ta);
    ctx->msiptp = le64_गन्तव्यम्_cpu(dc.msiptp);
    ctx->msi_addr_आवरणम् = le64_गन्तव्यम्_cpu(dc.msi_addr_आवरणम्);
    ctx->msi_addr_आदर्शः = le64_गन्तव्यम्_cpu(dc.msi_addr_आदर्शः);

    यदि (!(ctx->tc & RISCV_IOMMU_DC_TC_V)) {
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
    }

    यदि (!riscv_iommu_validate_device_ctx(s, ctx)) {
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /* FSC field checks */
    बहुलकम् = get_क्षेत्रम्(ctx->satp, RISCV_IOMMU_DC_FSC_बहुलकम्);
    addr = PPN_PHYS(get_क्षेत्रम्(ctx->satp, RISCV_IOMMU_DC_FSC_PPN));

    यदि (!(ctx->tc & RISCV_IOMMU_DC_TC_PDTV)) {
        यदि (ctx->process_id != RISCV_IOMMU_NOPROCID) {
            /* PID is disabled */
            प्रत्यागम RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
        }
        यदि (बहुलकम् > RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV57) {
            /* Invalid translation mode */
            प्रत्यागम RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        प्रत्यागम 0;
    }

    यदि (ctx->process_id == RISCV_IOMMU_NOPROCID) {
        यदि (!(ctx->tc & RISCV_IOMMU_DC_TC_DPE)) {
            /* No default process_id enabled, set BARE mode */
            ctx->satp = 0ULL;
            प्रत्यागम 0;
        } अन्यथा {
            /* Use default process_id #0 */
            ctx->process_id = 0;
        }
    }

    यदि (बहुलकम् == RISCV_IOMMU_DC_FSC_बहुलकम्_BARE) {
        /* No S-Stage translation, done. */
        प्रत्यागम 0;
    }

    /* FSC.TC.PDTV enabled */
    यदि (बहुलकम् > RISCV_IOMMU_DC_FSC_PDTP_बहुलकम्_PD20) {
        /* Invalid PDTP.MODE */
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    प्रति (गभीरता = बहुलकम् - RISCV_IOMMU_DC_FSC_PDTP_बहुलकम्_PD8; गभीरता-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

        /*
         * Select process id index bits based on process directory tree
         * level. See IOMMU Specification, 2.2. Process-Directory-Table.
         */
        ध्रुव पूर्णाङ्क विभज = गभीरता * 9 + 8;
        addr |= ((ctx->process_id >> विभज) << 3) & ~TARGET_PAGE_आवरणम्;
        यदि (pdt_स्मृतिः_read(s, ctx, addr, &de, परिमाण(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            प्रत्यागम RISCV_IOMMU_FQ_CAUSE_PDT_आरोपय_FAULT;
        }
        le64_गन्तव्यम्_cpus(&de);
        यदि (!(de & RISCV_IOMMU_PDTE_VALID)) {
            प्रत्यागम RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
        }
        addr = PPN_PHYS(get_क्षेत्रम्(de, RISCV_IOMMU_PDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

    /* Leaf entry in PDT */
    addr |= (ctx->process_id << 4) & ~TARGET_PAGE_आवरणम्;
    यदि (pdt_स्मृतिः_read(s, ctx, addr, &dc.ta, परिमाण(uint64_t) * 2,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_PDT_आरोपय_FAULT;
    }

    /* Use FSC and TA from process directory entry. */
    ctx->ta = le64_गन्तव्यम्_cpu(dc.ta);
    ctx->satp = le64_गन्तव्यम्_cpu(dc.fsc);

    यदि (!(ctx->ta & RISCV_IOMMU_PC_TA_V)) {
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
    }

    यदि (!riscv_iommu_validate_process_ctx(s, ctx)) {
        प्रत्यागम RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    प्रत्यागम 0;
}

/* Translation Context cache support */
स्थिर gboolean riscv_iommu_ctx_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUContext *c1 = (RISCVIOMMUContext *) v1;
    RISCVIOMMUContext *c2 = (RISCVIOMMUContext *) v2;
    प्रत्यागम c1->devid == c2->devid &&
           c1->process_id == c2->process_id;
}

स्थिर guint riscv_iommu_ctx_hash(gconstpointer v)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) v;
    /*
     * Generate simple hash of (process_id, devid)
     * assuming 24-bit wide devid.
     */
    प्रत्यागम (guint)(ctx->devid) + ((guint)(ctx->process_id) << 24);
}

स्थिर शून्य riscv_iommu_ctx_inval_devid_procid(gpointer कुञ्चिका, gpointer मानम्,
                                               gpointer दत्त)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मानम्;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) दत्त;
    यदि (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid &&
        ctx->process_id == arg->process_id) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

स्थिर शून्य riscv_iommu_ctx_inval_devid(gpointer कुञ्चिका, gpointer मानम्,
                                        gpointer दत्त)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मानम्;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) दत्त;
    यदि (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

स्थिर शून्य riscv_iommu_ctx_inval_all(gpointer कुञ्चिका, gpointer मानम्,
                                      gpointer दत्त)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मानम्;
    यदि (ctx->tc & RISCV_IOMMU_DC_TC_V) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

स्थिर शून्य riscv_iommu_ctx_inval(RISCVIOMMUState *s, GHFunc func,
                                  uint32_t devid, uint32_t process_id)
{
    GHash_सारणी *ctx_cache;
    RISCVIOMMUContext कुञ्चिका = {
        .devid = devid,
        .process_id = process_id,
    };
    ctx_cache = g_hash_सारणी_ref(s->ctx_cache);
    g_hash_सारणी_foreach(ctx_cache, func, &कुञ्चिका);
    g_hash_सारणी_unref(ctx_cache);
}

/* Find or allocate translation context for a given {device_id, process_id} */
स्थिर RISCVIOMMUContext *riscv_iommu_ctx(RISCVIOMMUState *s,
                                          अचिह्नित devid, अचिह्नित process_id,
                                          IOMMUAccessFlags perm, uint64_t iova,
                                          शून्य **ref)
{
    GHash_सारणी *ctx_cache;
    RISCVIOMMUContext *ctx;
    RISCVIOMMUContext कुञ्चिका = {
        .devid = devid,
        .process_id = process_id,
    };
    अचिह्नित बहुलकम् = get_क्षेत्रम्(s->ddtp, RISCV_IOMMU_DDTP_बहुलकम्);
    uint32_t fault_type;

    ctx_cache = g_hash_सारणी_ref(s->ctx_cache);

    यदि (बहुलकम् != RISCV_IOMMU_DDTP_बहुलकम्_OFF &&
        बहुलकम् != RISCV_IOMMU_DDTP_बहुलकम्_BARE) {
        ctx = g_hash_सारणी_lookup(ctx_cache, &कुञ्चिका);

        यदि (ctx && (ctx->tc & RISCV_IOMMU_DC_TC_V)) {
            *ref = ctx_cache;
            प्रत्यागम ctx;
        }
    }

    ctx = g_new0(RISCVIOMMUContext, 1);
    ctx->devid = devid;
    ctx->process_id = process_id;

    पूर्णाङ्क fault = riscv_iommu_ctx_fetch(s, ctx);
    यदि (!fault) {
        यदि (बहुलकम् != RISCV_IOMMU_DDTP_बहुलकम्_BARE) {
            यदि (g_hash_सारणी_आकार(ctx_cache) >= सीमा_CACHE_CTX) {
                g_hash_सारणी_unref(ctx_cache);
                ctx_cache = g_hash_सारणी_new_पूर्णम्(riscv_iommu_ctx_hash,
                                                  riscv_iommu_ctx_equal,
                                                  g_free, रिक्त);
                g_hash_सारणी_ref(ctx_cache);
                g_hash_सारणी_unref(qatomic_xchg(&s->ctx_cache, ctx_cache));
            }

            g_hash_सारणी_सङ्कलय(ctx_cache, ctx);
            *ref = ctx_cache;
        } अन्यथा {
            g_hash_सारणी_unref(ctx_cache);
            /* Remember ctx so it can be freed */
            *ref = ctx;
        }
        प्रत्यागम ctx;
    }

    g_hash_सारणी_unref(ctx_cache);
    *ref = रिक्त;

    /*
     * TODO: (1) do we need to distinguish other fault types
     * for ctx fetching and (2) evaluate putting the 'fault_type'
     * logic inside riscv_iommu_report_fault() - there's at
     * least one other place (end of riscv_iommu_translate())
     * that does something similar.
     */
    यदि (perm & IOMMU_RO) {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
    } अन्यथा {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    }

    riscv_iommu_report_fault(s, ctx, fault_type, fault,
                             !!process_id, iova, 0);

    g_free(ctx);
    प्रत्यागम रिक्त;
}

स्थिर शून्य riscv_iommu_ctx_put(RISCVIOMMUState *s, शून्य *ref)
{
    अचिह्नित बहुलकम् = get_क्षेत्रम्(s->ddtp, RISCV_IOMMU_DDTP_बहुलकम्);

    यदि (!ref) {
        प्रत्यागम;
    }

    /* ref is pointing to ctx in Bare mode. Bare mode ctx is not cached */
    यदि (बहुलकम् == RISCV_IOMMU_DDTP_बहुलकम्_BARE) {
        g_free(ref);
    } अन्यथा {
        g_hash_सारणी_unref((GHash_सारणी *)ref);
    }
}

/* Find or allocate address space for a given device */
स्थिर स्थानाङ्कः_Space *riscv_iommu_space(RISCVIOMMUState *s, PCIBus *bus,
                                            पूर्णाङ्क devfn)
{
    RISCVIOMMUSpace *as;

    QLIST_FOREACH(as, &s->spaces, list) {
        यदि (as->bus == bus && as->devfn == devfn) {
            भङ्ग;
        }
    }

    यदि (as == रिक्त) {
        वर्ण name[64];
        as = g_new0(RISCVIOMMUSpace, 1);

        as->iommu = s;
        as->bus = bus;
        as->devfn = devfn;

        snprintf(name, परिमाण(name), "riscv-iommu-%04x:%02x.%d-iova",
            pci_bus_अङ्कः(bus), PCI_SLOT(devfn), PCI_FUNC(devfn));

        /* IOVA address space, untranslated addresses */
        स्मृतिः_region_init_iommu(&as->iova_mr, परिमाण(as->iova_mr),
            TYPE_RISCV_IOMMU_स्मृतिः_REGION,
            OBJECT(s), "riscv_iommu", UINT64_MAX);
        स्थानाङ्कः_space_init(&as->iova_as, स्मृतिः_REGION(&as->iova_mr), name);

        QLIST_निवेशय_शीर्ष(&s->spaces, as, list);

        trace_riscv_iommu_new(s->parent_obj.id, pci_bus_अङ्कः(bus),
                PCI_SLOT(devfn), PCI_FUNC(devfn));
    }
    प्रत्यागम &as->iova_as;
}

/* Translation Object cache support */
स्थिर gboolean riscv_iommu_iot_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUEntry *t1 = (RISCVIOMMUEntry *) v1;
    RISCVIOMMUEntry *t2 = (RISCVIOMMUEntry *) v2;
    प्रत्यागम t1->gscid == t2->gscid && t1->pscid == t2->pscid &&
           t1->iova == t2->iova && t1->चिह्नकम् == t2->चिह्नकम्;
}

स्थिर guint riscv_iommu_iot_hash(gconstpointer v)
{
    RISCVIOMMUEntry *t = (RISCVIOMMUEntry *) v;
    प्रत्यागम (guint)t->iova;
}

/* GV: 0 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 0 AV: 0 GVMA: 1 */
स्थिर
शून्य riscv_iommu_iot_inval_all(gpointer कुञ्चिका, gpointer मानम्, gpointer दत्त)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) दत्त;
    यदि (iot->चिह्नकम् == arg->चिह्नकम्) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 0 PSCV: 1 GVMA: 0 */
स्थिर
शून्य riscv_iommu_iot_inval_pscid(gpointer कुञ्चिका, gpointer मानम्, gpointer दत्त)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) दत्त;
    यदि (iot->चिह्नकम् == arg->चिह्नकम् &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 0 GVMA: 0 */
स्थिर
शून्य riscv_iommu_iot_inval_iova(gpointer कुञ्चिका, gpointer मानम्, gpointer दत्त)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) दत्त;
    यदि (iot->चिह्नकम् == arg->चिह्नकम् &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 1 GVMA: 0 */
स्थिर शून्य riscv_iommu_iot_inval_pscid_iova(gpointer कुञ्चिका, gpointer मानम्,
                                             gpointer दत्त)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) दत्त;
    यदि (iot->चिह्नकम् == arg->चिह्नकम् &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 0 GVMA: 1 */
स्थिर
शून्य riscv_iommu_iot_inval_gscid(gpointer कुञ्चिका, gpointer मानम्, gpointer दत्त)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) दत्त;
    यदि (iot->चिह्नकम् == arg->चिह्नकम् &&
        iot->gscid == arg->gscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 1 GVMA: 0 */
स्थिर शून्य riscv_iommu_iot_inval_gscid_pscid(gpointer कुञ्चिका, gpointer मानम्,
                                              gpointer दत्त)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) दत्त;
    यदि (iot->चिह्नकम् == arg->चिह्नकम् &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 1 GVMA: 1 */
स्थिर शून्य riscv_iommu_iot_inval_gscid_iova(gpointer कुञ्चिका, gpointer मानम्,
                                             gpointer दत्त)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) दत्त;
    यदि (iot->चिह्नकम् == arg->चिह्नकम् &&
        iot->gscid == arg->gscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 1 GVMA: 0 */
स्थिर शून्य riscv_iommu_iot_inval_gscid_pscid_iova(gpointer कुञ्चिका, gpointer मानम्,
                                                   gpointer दत्त)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) दत्त;
    यदि (iot->चिह्नकम् == arg->चिह्नकम् &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* caller should keep ref-count for iot_cache object */
स्थिर RISCVIOMMUEntry *riscv_iommu_iot_lookup(RISCVIOMMUContext *ctx,
    GHash_सारणी *iot_cache, hwaddr iova, RISCVIOMMUTrans_चिह्नकम् transtag)
{
    RISCVIOMMUEntry कुञ्चिका = {
        .चिह्नकम्   = transtag,
        .gscid = get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID),
        .pscid = get_क्षेत्रम्(ctx->ta, RISCV_IOMMU_DC_TA_PSCID),
        .iova  = PPN_अधस्तात्(iova),
    };
    प्रत्यागम g_hash_सारणी_lookup(iot_cache, &कुञ्चिका);
}

/* caller should keep ref-count for iot_cache object */
स्थिर शून्य riscv_iommu_iot_update(RISCVIOMMUState *s,
    GHash_सारणी *iot_cache, RISCVIOMMUEntry *iot)
{
    यदि (!s->iot_सीमा) {
        प्रत्यागम;
    }

    यदि (g_hash_सारणी_आकार(s->iot_cache) >= s->iot_सीमा) {
        iot_cache = g_hash_सारणी_new_पूर्णम्(riscv_iommu_iot_hash,
                                          riscv_iommu_iot_equal,
                                          g_free, रिक्त);
        g_hash_सारणी_unref(qatomic_xchg(&s->iot_cache, iot_cache));
    }
    g_hash_सारणी_सङ्कलय(iot_cache, iot);
}

स्थिर शून्य riscv_iommu_iot_inval(RISCVIOMMUState *s, GHFunc func,
    uint32_t gscid, uint32_t pscid, hwaddr iova, RISCVIOMMUTrans_चिह्नकम् transtag)
{
    GHash_सारणी *iot_cache;
    RISCVIOMMUEntry कुञ्चिका = {
        .चिह्नकम् = transtag,
        .gscid = gscid,
        .pscid = pscid,
        .iova  = PPN_अधस्तात्(iova),
    };

    iot_cache = g_hash_सारणी_ref(s->iot_cache);
    g_hash_सारणी_foreach(iot_cache, func, &कुञ्चिका);
    g_hash_सारणी_unref(iot_cache);
}

स्थिर RISCVIOMMUTrans_चिह्नकम् riscv_iommu_get_transtag(RISCVIOMMUContext *ctx)
{
    uint64_t satp = get_क्षेत्रम्(ctx->satp, RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्);
    uint64_t gatp = get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्);

    यदि (satp == RISCV_IOMMU_DC_FSC_बहुलकम्_BARE) {
        प्रत्यागम (gatp == RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE) ?
            RISCV_IOMMU_TRANS_चिह्नकम्_BY : RISCV_IOMMU_TRANS_चिह्नकम्_VG;
    } अन्यथा {
        प्रत्यागम (gatp == RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE) ?
            RISCV_IOMMU_TRANS_चिह्नकम्_SS : RISCV_IOMMU_TRANS_चिह्नकम्_VN;
    }
}

स्थिर पूर्णाङ्क riscv_iommu_translate(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb, bool enable_cache)
{
    RISCVIOMMUTrans_चिह्नकम् transtag = riscv_iommu_get_transtag(ctx);
    RISCVIOMMUEntry *iot;
    IOMMUAccessFlags perm;
    bool enable_pid;
    bool enable_pri;
    GHash_सारणी *iot_cache;
    पूर्णाङ्क fault;

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_URQ);

    iot_cache = g_hash_सारणी_ref(s->iot_cache);
    enable_pri = (iotlb->perm == IOMMU_NONE) &&
                 (ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI);
    enable_pid = (ctx->tc & RISCV_IOMMU_DC_TC_PDTV);

    /* Check for ATS request. */
    यदि (iotlb->perm == IOMMU_NONE) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_ATS_RQ);
        /* Check if ATS is disabled. */
        यदि (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS)) {
            enable_pri = false;
            fault = RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
            गच्छ समाप्तम्;
        }
    }

    iot = riscv_iommu_iot_lookup(ctx, iot_cache, iotlb->iova, transtag);
    perm = iot ? iot->perm : IOMMU_NONE;
    यदि (perm != IOMMU_NONE) {
        iotlb->translated_addr = PPN_PHYS(iot->phys);
        iotlb->addr_आवरणम् = ~TARGET_PAGE_आवरणम्;
        iotlb->perm = perm;
        fault = 0;
        गच्छ समाप्तम्;
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_TLB_अलब्धिः);

    /* Translate using device directory / page table information. */
    fault = riscv_iommu_spa_fetch(s, ctx, iotlb);

    यदि (!fault && iotlb->target_as == &s->trap_as) {
        /* Do not cache trapped MSI translations */
        गच्छ समाप्तम्;
    }

    /*
     * We made an implementation choice to not cache identity-mapped
     * translations, as allowed by the specification, to avoid
     * translation cache evictions for other devices sharing the
     * IOMMU hardware model.
     */
    यदि (!fault && iotlb->translated_addr != iotlb->iova && enable_cache) {
        iot = g_new0(RISCVIOMMUEntry, 1);
        iot->iova = PPN_अधस्तात्(iotlb->iova);
        iot->phys = PPN_अधस्तात्(iotlb->translated_addr);
        iot->gscid = get_क्षेत्रम्(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
        iot->pscid = get_क्षेत्रम्(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        iot->perm = iotlb->perm;
        iot->चिह्नकम् = transtag;
        riscv_iommu_iot_update(s, iot_cache, iot);
    }

समाप्तम्:
    g_hash_सारणी_unref(iot_cache);

    यदि (enable_pri && fault) {
        संरचना riscv_iommu_pq_लेखः pr = {0};
        यदि (enable_pid) {
            pr.hdr = समुच्चयः_क्षेत्रम्(RISCV_IOMMU_PREQ_HDR_PV,
                               RISCV_IOMMU_PREQ_HDR_PID, ctx->process_id);
        }
        pr.hdr = समुच्चयः_क्षेत्रम्(pr.hdr, RISCV_IOMMU_PREQ_HDR_DID, ctx->devid);
        pr.payload = (iotlb->iova & TARGET_PAGE_आवरणम्) |
                     RISCV_IOMMU_PREQ_PAYLOAD_M;
        riscv_iommu_pri(s, &pr);
        प्रत्यागम fault;
    }

    यदि (fault) {
        अचिह्नित ttype = RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ;

        यदि ((iotlb->perm & IOMMU_RW) == IOMMU_RW
            || iotlb->perm & IOMMU_WO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
        } अन्यथा यदि (iotlb->perm & IOMMU_RO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
        }

        riscv_iommu_report_fault(s, ctx, ttype, fault, enable_pid,
                                 iotlb->iova, iotlb->translated_addr);
        प्रत्यागम fault;
    }

    प्रत्यागम 0;
}

/* IOMMU Command Interface */
स्थिर Mem_Tx_परिणाम riscv_iommu_iofence(RISCVIOMMUState *s, bool notify,
    uint64_t addr, uint32_t दत्त)
{
    /*
     * ATS processing in this implementation of the IOMMU is synchronous,
     * no need to wait for completions here.
     */
    यदि (!notify) {
        प्रत्यागम MEMTX_OK;
    }

    प्रत्यागम dma_स्मृतिः_write(s->target_as, addr, &दत्त, परिमाण(दत्त),
        MEMTXATTRS_UNSPECIFIED);
}

स्थिर शून्य riscv_iommu_ats(RISCVIOMMUState *s,
    संरचना riscv_iommu_command *cmd, IOMMUNotifierFlag flag,
    IOMMUAccessFlags perm,
    शून्य (*trace_fn)(ध्रुव वर्ण *id))
{
    RISCVIOMMUSpace *as;
    IOMMUNotifier *n;
    IOMMUTLBEvent event;
    uint32_t pid;
    uint32_t devid;
    ध्रुव bool pv = cmd->dword0 & RISCV_IOMMU_CMD_ATS_PV;

    यदि (cmd->dword0 & RISCV_IOMMU_CMD_ATS_DSV) {
        /* Use device segment and requester id */
        devid = get_क्षेत्रम्(cmd->dword0,
            RISCV_IOMMU_CMD_ATS_DSEG | RISCV_IOMMU_CMD_ATS_RID);
    } अन्यथा {
        devid = get_क्षेत्रम्(cmd->dword0, RISCV_IOMMU_CMD_ATS_RID);
    }

    pid = get_क्षेत्रम्(cmd->dword0, RISCV_IOMMU_CMD_ATS_PID);

    QLIST_FOREACH(as, &s->spaces, list) {
        यदि (riscv_iommu_space_devid(as) == devid) {
            भङ्ग;
        }
    }

    यदि (!as || !as->notifier) {
        प्रत्यागम;
    }

    event.type = flag;
    event.प्रविष्टिः.perm = perm;
    event.प्रविष्टिः.target_as = s->target_as;

    IOMMU_NOTIFIER_FOREACH(n, &as->iova_mr) {
        यदि (!pv || n->iommu_idx == pid) {
            event.प्रविष्टिः.iova = n->start;
            event.प्रविष्टिः.addr_आवरणम् = n->अन्तः - n->start;
            trace_fn(as->iova_mr.parent_obj.name);
            स्मृतिः_region_notify_iommu_one(n, &event);
        }
    }
}

स्थिर शून्य riscv_iommu_ats_inval(RISCVIOMMUState *s,
    संरचना riscv_iommu_command *cmd)
{
    प्रत्यागम riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_DEVIOTLB_UNMAP, IOMMU_NONE,
                           trace_riscv_iommu_ats_inval);
}

स्थिर शून्य riscv_iommu_ats_prgr(RISCVIOMMUState *s,
    संरचना riscv_iommu_command *cmd)
{
    अचिह्नित resp_संहिता = get_क्षेत्रम्(cmd->dword1,
                                   RISCV_IOMMU_CMD_ATS_PRGR_RESP_संहिता);

    /* Using the access flag to carry response code information */
    IOMMUAccessFlags perm = resp_संहिता ? IOMMU_NONE : IOMMU_RW;
    प्रत्यागम riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_MAP, perm,
                           trace_riscv_iommu_ats_prgr);
}

स्थिर शून्य riscv_iommu_process_ddtp(RISCVIOMMUState *s)
{
    uint64_t old_ddtp = s->ddtp;
    uint64_t new_ddtp = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_DDTP);
    अचिह्नित new_बहुलकम् = get_क्षेत्रम्(new_ddtp, RISCV_IOMMU_DDTP_बहुलकम्);
    अचिह्नित old_बहुलकम् = get_क्षेत्रम्(old_ddtp, RISCV_IOMMU_DDTP_बहुलकम्);
    bool ok = false;

    /*
     * Check for allowed DDTP.MODE transitions:
     * {OFF, BARE}        -> {OFF, BARE, 1LVL, 2LVL, 3LVL}
     * {1LVL, 2LVL, 3LVL} -> {OFF, BARE}
     */
    यदि (new_बहुलकम् == old_बहुलकम् ||
        new_बहुलकम् == RISCV_IOMMU_DDTP_बहुलकम्_OFF ||
        new_बहुलकम् == RISCV_IOMMU_DDTP_बहुलकम्_BARE) {
        ok = true;
    } अन्यथा यदि (new_बहुलकम् == RISCV_IOMMU_DDTP_बहुलकम्_1_LVL ||
               new_बहुलकम् == RISCV_IOMMU_DDTP_बहुलकम्_2_LVL ||
               new_बहुलकम् == RISCV_IOMMU_DDTP_बहुलकम्_3_LVL) {
        ok = old_बहुलकम् == RISCV_IOMMU_DDTP_बहुलकम्_OFF ||
             old_बहुलकम् == RISCV_IOMMU_DDTP_बहुलकम्_BARE;
    }

    यदि (ok) {
        /* clear reserved and busy bits, report back sanitized version */
        new_ddtp = समुच्चयः_क्षेत्रम्(new_ddtp & RISCV_IOMMU_DDTP_PPN,
                             RISCV_IOMMU_DDTP_बहुलकम्, new_बहुलकम्);
    } अन्यथा {
        new_ddtp = old_ddtp;
    }
    s->ddtp = new_ddtp;

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, new_ddtp);
}

/* Command function and opcode field. */
#परिभाषय RISCV_IOMMU_CMD(func, op) (((func) << 7) | (op))

स्थिर शून्य riscv_iommu_process_cq_पुच्छ(RISCVIOMMUState *s)
{
    संरचना riscv_iommu_command cmd;
    Mem_Tx_परिणाम res;
    dma_addr_t addr;
    uint32_t पुच्छ, शीर्ष, ctrl;
    uint64_t cmd_opcode;
    GHFunc func;

    ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    पुच्छ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQT) & s->cq_आवरणम्;
    शीर्ष = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQH) & s->cq_आवरणम्;

    /* Check for pending error or queue processing disabled */
    यदि (!(ctrl & RISCV_IOMMU_CQCSR_CQON) ||
        !!(ctrl & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        प्रत्यागम;
    }

    यावत् (पुच्छ != शीर्ष) {
        addr = s->cq_addr  + शीर्ष * परिमाण(cmd);
        res = dma_स्मृतिः_read(s->target_as, addr, &cmd, परिमाण(cmd),
                              MEMTXATTRS_UNSPECIFIED);

        यदि (res != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                  RISCV_IOMMU_CQCSR_CQMF, 0);
            गच्छ fault;
        }

        trace_riscv_iommu_cmd(s->parent_obj.id, cmd.dword0, cmd.dword1);

        cmd_opcode = get_क्षेत्रम्(cmd.dword0,
                               RISCV_IOMMU_CMD_OPCODE | RISCV_IOMMU_CMD_FUNC);

        विकल्प (cmd_opcode) {
        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOFENCE_FUNC_C,
                             RISCV_IOMMU_CMD_IOFENCE_OPCODE):
            यदि (cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_RESERVED) {
                गच्छ cmd_ill;
            }

            res = riscv_iommu_iofence(s,
                cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_AV, cmd.dword1 << 2,
                get_क्षेत्रम्(cmd.dword0, RISCV_IOMMU_CMD_IOFENCE_दत्त));

            यदि (res != MEMTX_OK) {
                riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                      RISCV_IOMMU_CQCSR_CQMF, 0);
                गच्छ fault;
            }
            भङ्ग;

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_क्षेत्रम्(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_क्षेत्रम्(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & TARGET_PAGE_आवरणम्;

            यदि (pscv) {
                /* illegal command arguments IOTINVAL.GVMA & PSCV == 1 */
                गच्छ cmd_ill;
            }

            func = riscv_iommu_iot_inval_all;

            यदि (gv) {
                func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                              riscv_iommu_iot_inval_gscid;
            }

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_चिह्नकम्_VG);

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_चिह्नकम्_VN);
            भङ्ग;
        }

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_क्षेत्रम्(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_क्षेत्रम्(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & TARGET_PAGE_आवरणम्;
            RISCVIOMMUTrans_चिह्नकम् transtag;

            यदि (gv) {
                transtag = RISCV_IOMMU_TRANS_चिह्नकम्_VN;
                यदि (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_gscid_pscid_iova :
                                  riscv_iommu_iot_inval_gscid_pscid;
                } अन्यथा {
                    func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                                  riscv_iommu_iot_inval_gscid;
                }
            } अन्यथा {
                transtag = RISCV_IOMMU_TRANS_चिह्नकम्_SS;
                यदि (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_pscid_iova :
                                  riscv_iommu_iot_inval_pscid;
                } अन्यथा {
                    func = (av) ? riscv_iommu_iot_inval_iova :
                                  riscv_iommu_iot_inval_all;
                }
            }

            riscv_iommu_iot_inval(s, func, gscid, pscid, iova, transtag);
            भङ्ग;
        }

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            यदि (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* invalidate all device context cache mappings */
                func = riscv_iommu_ctx_inval_all;
            } अन्यथा {
                /* invalidate all device context matching DID */
                func = riscv_iommu_ctx_inval_devid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_क्षेत्रम्(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID), 0);
            भङ्ग;

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            यदि (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* illegal command arguments IODIR_PDT & DV == 0 */
                गच्छ cmd_ill;
            } अन्यथा {
                func = riscv_iommu_ctx_inval_devid_procid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_क्षेत्रम्(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID),
                get_क्षेत्रम्(cmd.dword0, RISCV_IOMMU_CMD_IODIR_PID));
            भङ्ग;

        /* ATS commands */
        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_INVAL,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            यदि (!s->enable_ats) {
                गच्छ cmd_ill;
            }

            riscv_iommu_ats_inval(s, &cmd);
            भङ्ग;

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_PRGR,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            यदि (!s->enable_ats) {
                गच्छ cmd_ill;
            }

            riscv_iommu_ats_prgr(s, &cmd);
            भङ्ग;

        स्वभाव:
        cmd_ill:
            /* Invalid instruction, do not advance instruction index. */
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                RISCV_IOMMU_CQCSR_CMD_ILL, 0);
            गच्छ fault;
        }

        /* Advance and update head pointer after command completes. */
        शीर्ष = (शीर्ष + 1) & s->cq_आवरणम्;
        riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_CQH, शीर्ष);
    }
    प्रत्यागम;

fault:
    यदि (ctrl & RISCV_IOMMU_CQCSR_CIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_CQ);
    }
}

स्थिर शून्य riscv_iommu_process_cq_control(RISCVIOMMUState *s)
{
    uint64_t आधारः;
    uint32_t ctrl_समुच्चयः = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_समुच्चयः & RISCV_IOMMU_CQCSR_CQEN);
    bool सक्रिय = !!(ctrl_समुच्चयः & RISCV_IOMMU_CQCSR_CQON);

    यदि (enable && !सक्रिय) {
        आधारः = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_CQB);
        s->cq_आवरणम् = (2ULL << get_क्षेत्रम्(आधारः, RISCV_IOMMU_CQB_LOG2SZ)) - 1;
        s->cq_addr = PPN_PHYS(get_क्षेत्रम्(आधारः, RISCV_IOMMU_CQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~s->cq_आवरणम्);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQT], 0);
        ctrl_समुच्चयः = RISCV_IOMMU_CQCSR_CQON;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQMF |
                   RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CMD_गन्तव्यम् |
                   RISCV_IOMMU_CQCSR_FENCE_W_IP;
    } अन्यथा यदि (!enable && सक्रिय) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~0);
        ctrl_समुच्चयः = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQON;
    } अन्यथा {
        ctrl_समुच्चयः = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, ctrl_समुच्चयः, ctrl_clr);

    /*
     * After clearing error bits (CMD_ILL, CQMF), if queue is still active,
     * re-process pending command.
     */
    ctrl_समुच्चयः = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    यदि ((ctrl_समुच्चयः & RISCV_IOMMU_CQCSR_CQON) &&
        !(ctrl_समुच्चयः & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        riscv_iommu_process_cq_पुच्छ(s);
    }
}

स्थिर शून्य riscv_iommu_process_fq_control(RISCVIOMMUState *s)
{
    uint64_t आधारः;
    uint32_t ctrl_समुच्चयः = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_समुच्चयः & RISCV_IOMMU_FQCSR_FQEN);
    bool सक्रिय = !!(ctrl_समुच्चयः & RISCV_IOMMU_FQCSR_FQON);

    यदि (enable && !सक्रिय) {
        आधारः = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_FQB);
        s->fq_आवरणम् = (2ULL << get_क्षेत्रम्(आधारः, RISCV_IOMMU_FQB_LOG2SZ)) - 1;
        s->fq_addr = PPN_PHYS(get_क्षेत्रम्(आधारः, RISCV_IOMMU_FQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~s->fq_आवरणम्);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQT], 0);
        ctrl_समुच्चयः = RISCV_IOMMU_FQCSR_FQON;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQMF |
            RISCV_IOMMU_FQCSR_FQOF;
    } अन्यथा यदि (!enable && सक्रिय) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~0);
        ctrl_समुच्चयः = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQON;
    } अन्यथा {
        ctrl_समुच्चयः = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, ctrl_समुच्चयः, ctrl_clr);
}

स्थिर शून्य riscv_iommu_process_pq_control(RISCVIOMMUState *s)
{
    uint64_t आधारः;
    uint32_t ctrl_समुच्चयः = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_समुच्चयः & RISCV_IOMMU_PQCSR_PQEN);
    bool सक्रिय = !!(ctrl_समुच्चयः & RISCV_IOMMU_PQCSR_PQON);

    यदि (enable && !सक्रिय) {
        आधारः = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_PQB);
        s->pq_आवरणम् = (2ULL << get_क्षेत्रम्(आधारः, RISCV_IOMMU_PQB_LOG2SZ)) - 1;
        s->pq_addr = PPN_PHYS(get_क्षेत्रम्(आधारः, RISCV_IOMMU_PQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~s->pq_आवरणम्);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQT], 0);
        ctrl_समुच्चयः = RISCV_IOMMU_PQCSR_PQON;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQMF |
            RISCV_IOMMU_PQCSR_PQOF;
    } अन्यथा यदि (!enable && सक्रिय) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~0);
        ctrl_समुच्चयः = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQON;
    } अन्यथा {
        ctrl_समुच्चयः = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, ctrl_समुच्चयः, ctrl_clr);
}

स्थिर शून्य riscv_iommu_process_dbg(RISCVIOMMUState *s)
{
    uint64_t iova = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_IOVA);
    uint64_t ctrl = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_CTL);
    अचिह्नित devid = get_क्षेत्रम्(ctrl, RISCV_IOMMU_TR_REQ_CTL_DID);
    अचिह्नित pid = get_क्षेत्रम्(ctrl, RISCV_IOMMU_TR_REQ_CTL_PID);
    IOMMUAccessFlags perm = ctrl & RISCV_IOMMU_TR_REQ_CTL_NW
                            ? IOMMU_RO : IOMMU_RW;
    RISCVIOMMUContext *ctx;
    शून्य *ref;

    यदि (!(ctrl & RISCV_IOMMU_TR_REQ_CTL_GO_BUSY)) {
        प्रत्यागम;
    }

    ctx = riscv_iommu_ctx(s, devid, pid, perm, iova, &ref);
    यदि (ctx == रिक्त) {
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE,
                                 RISCV_IOMMU_TR_RESPONSE_FAULT |
                                 (RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED << 10));
    } अन्यथा {
        IOMMUTLBEntry iotlb = {
            .iova = iova,
            .perm = perm,
            .addr_आवरणम् = ~0,
            .target_as = रिक्त,
        };
        पूर्णाङ्क fault = riscv_iommu_translate(s, ctx, &iotlb, false);
        यदि (fault) {
            iova = RISCV_IOMMU_TR_RESPONSE_FAULT | (((uint64_t) fault) << 10);
        } अन्यथा {
            iova = iotlb.translated_addr & ~iotlb.addr_आवरणम्;
            iova = समुच्चयः_क्षेत्रम्(0, RISCV_IOMMU_TR_RESPONSE_PPN, PPN_अधस्तात्(iova));
        }
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE, iova);
    }

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
        RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    riscv_iommu_ctx_put(s, ref);
}

प्रकारनिर्देश शून्य riscv_iommu_process_fn(RISCVIOMMUState *s);

स्थिर शून्य riscv_iommu_update_icvec(RISCVIOMMUState *s, uint64_t दत्त)
{
    uint64_t icvec = 0;

    icvec |= MIN(दत्त & RISCV_IOMMU_ICVEC_CIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_CIV);

    icvec |= MIN(दत्त & RISCV_IOMMU_ICVEC_FIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_FIV);

    icvec |= MIN(दत्त & RISCV_IOMMU_ICVEC_PMIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PMIV);

    icvec |= MIN(दत्त & RISCV_IOMMU_ICVEC_PIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PIV);

    trace_riscv_iommu_icvec_write(दत्त, icvec);

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_ICVEC, icvec);
}

स्थिर शून्य riscv_iommu_update_ipsr(RISCVIOMMUState *s, uint64_t दत्त)
{
    uint32_t cqcsr, fqcsr, pqcsr;
    uint32_t ipsr_समुच्चयः = 0;
    uint32_t ipsr_clr = 0;

    यदि (दत्त & RISCV_IOMMU_IPSR_CIP) {
        cqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);

        यदि (cqcsr & RISCV_IOMMU_CQCSR_CIE &&
            (cqcsr & RISCV_IOMMU_CQCSR_FENCE_W_IP ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ILL ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_गन्तव्यम् ||
             cqcsr & RISCV_IOMMU_CQCSR_CQMF)) {
            ipsr_समुच्चयः |= RISCV_IOMMU_IPSR_CIP;
        } अन्यथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
        }
    } अन्यथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
    }

    यदि (दत्त & RISCV_IOMMU_IPSR_FIP) {
        fqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);

        यदि (fqcsr & RISCV_IOMMU_FQCSR_FIE &&
            (fqcsr & RISCV_IOMMU_FQCSR_FQOF ||
             fqcsr & RISCV_IOMMU_FQCSR_FQMF)) {
            ipsr_समुच्चयः |= RISCV_IOMMU_IPSR_FIP;
        } अन्यथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
        }
    } अन्यथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
    }

    यदि (!(दत्त & RISCV_IOMMU_IPSR_PMIP)) {
        ipsr_clr |= RISCV_IOMMU_IPSR_PMIP;
    }

    यदि (दत्त & RISCV_IOMMU_IPSR_PIP) {
        pqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);

        यदि (pqcsr & RISCV_IOMMU_PQCSR_PIE &&
            (pqcsr & RISCV_IOMMU_PQCSR_PQOF ||
             pqcsr & RISCV_IOMMU_PQCSR_PQMF)) {
            ipsr_समुच्चयः |= RISCV_IOMMU_IPSR_PIP;
        } अन्यथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
        }
    } अन्यथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, ipsr_समुच्चयः, ipsr_clr);
}

स्थिर शून्य riscv_iommu_process_hpm_writes(RISCVIOMMUState *s,
                                           uint32_t regb,
                                           bool पूर्व_cy_inh)
{
    विकल्प (regb) {
    पक्ष RISCV_IOMMU_REG_IOCOUNTINH:
        riscv_iommu_process_iocntinh_cy(s, पूर्व_cy_inh);
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_IOHPMCYCLES:
    पक्ष RISCV_IOMMU_REG_IOHPMCYCLES + 4:
        riscv_iommu_process_hpmcycle_write(s);
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_IOHPMEVT_आधारः ...
        RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_अङ्कः) + 4:
        riscv_iommu_process_hpmevt_write(s, regb & ~7);
        भङ्ग;
    }
}

/*
 * Write the resulting value of 'data' for the reg specified
 * by 'reg_addr', after considering read-only/read-write/write-clear
 * bits, in the pointer 'dest'.
 *
 * The result is written in little-endian.
 */
स्थिर शून्य riscv_iommu_write_reg_val(RISCVIOMMUState *s,
                                      शून्य *dest, hwaddr reg_addr,
                                      पूर्णाङ्क आकार, uint64_t दत्त)
{
    uint64_t ro = ldn_le_p(&s->regs_ro[reg_addr], आकार);
    uint64_t wc = ldn_le_p(&s->regs_wc[reg_addr], आकार);
    uint64_t curr_val = ldn_le_p(&s->regs[reg_addr], आकार);

    stn_le_p(dest, आकार, ((curr_val & ro) | (दत्त & ~ro)) & ~(दत्त & wc));
}

स्थिर Mem_Tx_परिणाम riscv_iommu_mmio_write(शून्य *opaque, hwaddr addr,
                                          uint64_t दत्त, अचिह्नित आकार,
                                          MemTxAttrs attrs)
{
    riscv_iommu_process_fn *process_fn = रिक्त;
    RISCVIOMMUState *s = opaque;
    uint32_t regb = addr & ~3;
    uint32_t busy = 0;
    uint64_t val = 0;
    bool cy_inh = false;

    यदि ((addr & (आकार - 1)) != 0) {
        /* Unsupported MMIO alignment or access size */
        प्रत्यागम MEMTX_ERROR;
    }

    यदि (addr + आकार > RISCV_IOMMU_REG_MSI_CONFIG) {
        /* Unsupported MMIO access location. */
        प्रत्यागम MEMTX_ACCESS_ERROR;
    }

    /* Track actionable MMIO write. */
    विकल्प (regb) {
    पक्ष RISCV_IOMMU_REG_DDTP:
    पक्ष RISCV_IOMMU_REG_DDTP + 4:
        process_fn = riscv_iommu_process_ddtp;
        regb = RISCV_IOMMU_REG_DDTP;
        busy = RISCV_IOMMU_DDTP_BUSY;
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_CQT:
        process_fn = riscv_iommu_process_cq_पुच्छ;
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_CQCSR:
        process_fn = riscv_iommu_process_cq_control;
        busy = RISCV_IOMMU_CQCSR_BUSY;
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_FQCSR:
        process_fn = riscv_iommu_process_fq_control;
        busy = RISCV_IOMMU_FQCSR_BUSY;
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_PQCSR:
        process_fn = riscv_iommu_process_pq_control;
        busy = RISCV_IOMMU_PQCSR_BUSY;
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_ICVEC:
    पक्ष RISCV_IOMMU_REG_IPSR:
        /*
         * ICVEC and IPSR have special read/write procedures. We'll
         * call their respective helpers and exit.
         */
        riscv_iommu_write_reg_val(s, &val, addr, आकार, दत्त);

        /*
         * 'val' is stored as LE. Switch to host endianess
         * before using it.
         */
        val = le64_गन्तव्यम्_cpu(val);

        यदि (regb == RISCV_IOMMU_REG_ICVEC) {
            riscv_iommu_update_icvec(s, val);
        } अन्यथा {
            riscv_iommu_update_ipsr(s, val);
        }

        प्रत्यागम MEMTX_OK;

    पक्ष RISCV_IOMMU_REG_TR_REQ_CTL:
        process_fn = riscv_iommu_process_dbg;
        regb = RISCV_IOMMU_REG_TR_REQ_CTL;
        busy = RISCV_IOMMU_TR_REQ_CTL_GO_BUSY;
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_IOCOUNTINH:
        यदि (addr != RISCV_IOMMU_REG_IOCOUNTINH) {
            भङ्ग;
        }
        /* Store previous value of CY bit. */
        cy_inh = !!(riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTINH) &
            RISCV_IOMMU_IOCOUNTINH_CY);
        भङ्ग;


    स्वभाव:
        भङ्ग;
    }

    /*
     * Registers update might be not synchronized with core logic.
     * If system software updates register when relevant BUSY bit
     * is set IOMMU behavior of additional writes to the register
     * is UNSPECIFIED.
     */
    riscv_iommu_write_reg_val(s, &s->regs[addr], addr, आकार, दत्त);

    /* Busy flag update, MSB 4-byte register. */
    यदि (busy) {
        uint32_t rw = ldl_le_p(&s->regs[regb]);
        stl_le_p(&s->regs[regb], rw | busy);
    }

    /* Process HPM writes and update any internal state if needed. */
    यदि (regb >= RISCV_IOMMU_REG_IOCOUNTOVF &&
        regb <= (RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_अङ्कः) + 4)) {
        riscv_iommu_process_hpm_writes(s, regb, cy_inh);
    }

    यदि (process_fn) {
        process_fn(s);
    }

    प्रत्यागम MEMTX_OK;
}

स्थिर Mem_Tx_परिणाम riscv_iommu_mmio_read(शून्य *opaque, hwaddr addr,
    uint64_t *दत्त, अचिह्नित आकार, MemTxAttrs attrs)
{
    RISCVIOMMUState *s = opaque;
    uint64_t val = -1;
    uint8_t *ptr;

    यदि ((addr & (आकार - 1)) != 0) {
        /* Unsupported MMIO alignment. */
        प्रत्यागम MEMTX_ERROR;
    }

    यदि (addr + आकार > RISCV_IOMMU_REG_MSI_CONFIG) {
        प्रत्यागम MEMTX_ACCESS_ERROR;
    }

    /* Compute cycle register value. */
    यदि ((addr & ~7) == RISCV_IOMMU_REG_IOHPMCYCLES) {
        val = riscv_iommu_hpmcycle_read(s);
        ptr = (uint8_t *)&val + (addr & 7);
    } अन्यथा यदि ((addr & ~3) == RISCV_IOMMU_REG_IOCOUNTOVF) {
        /*
         * Software can read RISCV_IOMMU_REG_IOCOUNTOVF before timer
         * callback completes. In which case CY_OF bit in
         * RISCV_IOMMU_IOHPMCYCLES_OVF would be 0. Here we take the
         * CY_OF bit state from RISCV_IOMMU_REG_IOHPMCYCLES register as
         * it's not dependent over the timer callback and is computed
         * from cycle overflow.
         */
        val = ldq_le_p(&s->regs[addr]);
        val |= (riscv_iommu_hpmcycle_read(s) & RISCV_IOMMU_IOHPMCYCLES_OVF)
                   ? RISCV_IOMMU_IOCOUNTOVF_CY
                   : 0;
        ptr = (uint8_t *)&val + (addr & 3);
    } अन्यथा {
        ptr = &s->regs[addr];
    }

    val = ldn_le_p(ptr, आकार);

    *दत्त = val;

    प्रत्यागम MEMTX_OK;
}

स्थिर ध्रुव स्मृतिः_Region_Ops riscv_iommu_mmio_ops = {
    .read_with_attrs = riscv_iommu_mmio_read,
    .write_with_attrs = riscv_iommu_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_आकार = 4,
        .max_access_आकार = 8,
        .unaligned = false,
    },
    .valid = {
        .min_access_आकार = 4,
        .max_access_आकार = 8,
    }
};

/*
 * Translations matching MSI pattern check are redirected to "riscv-iommu-trap"
 * memory region as untranslated address, for additional MSI/MRIF interception
 * by IOMMU interrupt remapping implementation.
 * Note: Device emulation code generating an MSI is expected to provide a valid
 * memory transaction attributes with requested_id set.
 */
स्थिर Mem_Tx_परिणाम riscv_iommu_trap_write(शून्य *opaque, hwaddr addr,
    uint64_t दत्त, अचिह्नित आकार, MemTxAttrs attrs)
{
    RISCVIOMMUState* s = (RISCVIOMMUState *)opaque;
    RISCVIOMMUContext *ctx;
    Mem_Tx_परिणाम res;
    शून्य *ref;
    uint32_t devid = attrs.requester_id;

    यदि (attrs.unspecified) {
        प्रत्यागम MEMTX_ACCESS_ERROR;
    }

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= s->bus << 8;

    ctx = riscv_iommu_ctx(s, devid, 0, IOMMU_RW, addr, &ref);
    यदि (ctx == रिक्त) {
        res = MEMTX_ACCESS_ERROR;
    } अन्यथा {
        res = riscv_iommu_msi_write(s, ctx, addr, दत्त, आकार, attrs);
    }
    riscv_iommu_ctx_put(s, ref);
    प्रत्यागम res;
}

स्थिर Mem_Tx_परिणाम riscv_iommu_trap_read(शून्य *opaque, hwaddr addr,
    uint64_t *दत्त, अचिह्नित आकार, MemTxAttrs attrs)
{
    प्रत्यागम MEMTX_ACCESS_ERROR;
}

स्थिर ध्रुव स्मृतिः_Region_Ops riscv_iommu_trap_ops = {
    .read_with_attrs = riscv_iommu_trap_read,
    .write_with_attrs = riscv_iommu_trap_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_आकार = 4,
        .max_access_आकार = 8,
    },
    .valid = {
        .min_access_आकार = 4,
        .max_access_आकार = 8,
    }
};

शून्य riscv_iommu_समुच्चयः_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_बहुलकम् बहुलकम्)
{
    s->cap = समुच्चयः_क्षेत्रम्(s->cap, RISCV_IOMMU_CAP_IGS, बहुलकम्);
}

स्थिर शून्य riscv_iommu_instance_init(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    /* Enable translation debug interface */
    s->cap = RISCV_IOMMU_CAP_DBG;

    /* TODO: method to report supported PID bits */
    s->pid_द्वयङ्काः = 8; /* restricted to size of MemTxAttrs.pid */
    s->cap |= RISCV_IOMMU_CAP_PD8;

    /* register storage */
    s->regs = g_new0(uint8_t, RISCV_IOMMU_REG_आकार);
    s->regs_ro = g_new0(uint8_t, RISCV_IOMMU_REG_आकार);
    s->regs_wc = g_new0(uint8_t, RISCV_IOMMU_REG_आकार);

     /* Mark all registers read-only */
    स्मृतिं_पूरय(s->regs_ro, 0xff, RISCV_IOMMU_REG_आकार);

    /* Device translation context cache */
    s->ctx_cache = g_hash_सारणी_new_पूर्णम्(riscv_iommu_ctx_hash,
                                         riscv_iommu_ctx_equal,
                                         g_free, रिक्त);

    s->iot_cache = g_hash_सारणी_new_पूर्णम्(riscv_iommu_iot_hash,
                                         riscv_iommu_iot_equal,
                                         g_free, रिक्त);

    s->iommus.le_अग्र = रिक्त;
    s->iommus.le_पूर्व = रिक्त;
    QLIST_INIT(&s->spaces);
}

स्थिर शून्य riscv_iommu_instance_finalize(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    g_free(s->regs);
    g_free(s->regs_ro);
    g_free(s->regs_wc);

    g_hash_सारणी_unref(s->ctx_cache);
    g_hash_सारणी_unref(s->iot_cache);
}

स्थिर शून्य riscv_iommu_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    /* Report QEMU target physical address space limits. */
    s->cap = समुच्चयः_क्षेत्रम्(s->cap, RISCV_IOMMU_CAP_PAS, s->pas_द्वयङ्काः);

    s->cap |= s->संस्करणम् & RISCV_IOMMU_CAP_संस्करणम्;
    यदि (s->enable_msi) {
        s->cap |= RISCV_IOMMU_CAP_MSI_FLAT | RISCV_IOMMU_CAP_MSI_MRIF;
    }
    यदि (s->enable_ats) {
        s->cap |= RISCV_IOMMU_CAP_ATS;
    }
    यदि (s->enable_s_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32 | RISCV_IOMMU_CAP_SV39 |
                  RISCV_IOMMU_CAP_SV48 | RISCV_IOMMU_CAP_SV57;
    }
    यदि (s->enable_g_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32X4 | RISCV_IOMMU_CAP_SV39X4 |
                  RISCV_IOMMU_CAP_SV48X4 | RISCV_IOMMU_CAP_SV57X4 |
                  RISCV_IOMMU_CAP_SVRSW60T59B;
    }

    यदि (s->hpm_cntrs > 0) {
        /* Clip number of HPM counters to maximum supported (31). */
        यदि (s->hpm_cntrs > RISCV_IOMMU_IOCOUNT_अङ्कः) {
            s->hpm_cntrs = RISCV_IOMMU_IOCOUNT_अङ्कः;
        }
        /* Enable hardware performance monitor interface */
        s->cap |= RISCV_IOMMU_CAP_HPM;
    }

    /* Out-of-reset translation mode: OFF (DMA disabled) BARE (passthrough) */
    s->ddtp = समुच्चयः_क्षेत्रम्(0, RISCV_IOMMU_DDTP_बहुलकम्, s->enable_off ?
                        RISCV_IOMMU_DDTP_बहुलकम्_OFF : RISCV_IOMMU_DDTP_बहुलकम्_BARE);

    /*
     * Register complete MMIO space, including MSI/PBA registers.
     * Note, PCIDevice implementation will add overlapping MR for MSI/PBA,
     * managed directly by the PCIDevice implementation.
     */
    स्मृतिः_region_init_io(&s->regs_mr, OBJECT(dev), &riscv_iommu_mmio_ops, s,
        "riscv-iommu-regs", RISCV_IOMMU_REG_आकार);

    /* Set power-on register state */
    stq_le_p(&s->regs[RISCV_IOMMU_REG_CAP], s->cap);

    stq_le_p(&s->regs[RISCV_IOMMU_REG_FCTL], 0);
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FCTL],
             ~(RISCV_IOMMU_FCTL_GXL | RISCV_IOMMU_FCTL_WSI));

    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_DDTP],
        ~(RISCV_IOMMU_DDTP_PPN | RISCV_IOMMU_DDTP_बहुलकम्));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQB],
        ~(RISCV_IOMMU_CQB_LOG2SZ | RISCV_IOMMU_CQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQB],
        ~(RISCV_IOMMU_FQB_LOG2SZ | RISCV_IOMMU_FQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQB],
        ~(RISCV_IOMMU_PQB_LOG2SZ | RISCV_IOMMU_PQB_PPN));
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_CQCSR], RISCV_IOMMU_CQCSR_CQMF |
        RISCV_IOMMU_CQCSR_CMD_गन्तव्यम् | RISCV_IOMMU_CQCSR_CMD_ILL);
    stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQCSR], RISCV_IOMMU_CQCSR_CQON |
        RISCV_IOMMU_CQCSR_BUSY);
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_FQCSR], RISCV_IOMMU_FQCSR_FQMF |
        RISCV_IOMMU_FQCSR_FQOF);
    stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQCSR], RISCV_IOMMU_FQCSR_FQON |
        RISCV_IOMMU_FQCSR_BUSY);
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_PQCSR], RISCV_IOMMU_PQCSR_PQMF |
        RISCV_IOMMU_PQCSR_PQOF);
    stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQCSR], RISCV_IOMMU_PQCSR_PQON |
        RISCV_IOMMU_PQCSR_BUSY);
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_IPSR], ~0);
    stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_ICVEC], 0);
    stq_le_p(&s->regs[RISCV_IOMMU_REG_DDTP], s->ddtp);
    /* If debug registers enabled. */
    यदि (s->cap & RISCV_IOMMU_CAP_DBG) {
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_IOVA], 0);
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_CTL],
            RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    }

    /* If HPM registers are enabled. */
    यदि (s->cap & RISCV_IOMMU_CAP_HPM) {
        /* +1 for cycle counter bit. */
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOCOUNTINH],
                 ~((2 << s->hpm_cntrs) - 1));
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCYCLES], 0);
        स्मृतिं_पूरय(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCTR_आधारः],
               0x00, s->hpm_cntrs * 8);
        स्मृतिं_पूरय(&s->regs_ro[RISCV_IOMMU_REG_IOHPMEVT_आधारः],
               0x00, s->hpm_cntrs * 8);
    }

    /* Memory region for downstream access, if specified. */
    यदि (s->target_mr) {
        s->target_as = g_new0(स्थानाङ्कः_Space, 1);
        स्थानाङ्कः_space_init(s->target_as, s->target_mr,
            "riscv-iommu-downstream");
    } अन्यथा {
        /* Fallback to global system memory. */
        s->target_as = &स्थानाङ्कः_space_स्मृतिः;
    }

    /* Memory region for untranslated MRIF/MSI writes */
    स्मृतिः_region_init_io(&s->trap_mr, OBJECT(dev), &riscv_iommu_trap_ops, s,
            "riscv-iommu-trap", ~0ULL);
    स्थानाङ्कः_space_init(&s->trap_as, &s->trap_mr, "riscv-iommu-trap-as");

    यदि (s->cap & RISCV_IOMMU_CAP_HPM) {
        s->hpm_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL, riscv_iommu_hpm_timer_cb, s);
        s->hpm_event_ctr_map = g_hash_सारणी_new(g_प्रत्यक्षम्_hash, g_प्रत्यक्षम्_equal);
    }
}

स्थिर शून्य riscv_iommu_unrealize(DeviceState *dev)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    यदि (s->cap & RISCV_IOMMU_CAP_HPM) {
        g_hash_सारणी_unref(s->hpm_event_ctr_map);
        timer_free(s->hpm_timer);
    }
}

शून्य riscv_iommu_reset(RISCVIOMMUState *s)
{
    uint32_t reg_clr;
    पूर्णाङ्क ddtp_बहुलकम्;

    /*
     * Clear DDTP while setting DDTP_mode back to user
     * initial setting.
     */
    ddtp_बहुलकम् = s->enable_off ?
                RISCV_IOMMU_DDTP_बहुलकम्_OFF : RISCV_IOMMU_DDTP_बहुलकम्_BARE;
    s->ddtp = समुच्चयः_क्षेत्रम्(0, RISCV_IOMMU_DDTP_बहुलकम्, ddtp_बहुलकम्);
    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, s->ddtp);

    reg_clr = RISCV_IOMMU_CQCSR_CQEN | RISCV_IOMMU_CQCSR_CIE |
              RISCV_IOMMU_CQCSR_CQON | RISCV_IOMMU_CQCSR_BUSY;
    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, 0, reg_clr);

    reg_clr = RISCV_IOMMU_FQCSR_FQEN | RISCV_IOMMU_FQCSR_FIE |
              RISCV_IOMMU_FQCSR_FQON | RISCV_IOMMU_FQCSR_BUSY;
    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, 0, reg_clr);

    reg_clr = RISCV_IOMMU_PQCSR_PQEN | RISCV_IOMMU_PQCSR_PIE |
              RISCV_IOMMU_PQCSR_PQON | RISCV_IOMMU_PQCSR_BUSY;
    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, 0, reg_clr);

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
                          RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);

    riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_IPSR, 0);

    g_hash_सारणी_remove_all(s->ctx_cache);
    g_hash_सारणी_remove_all(s->iot_cache);
}

स्थिर ध्रुव Property riscv_iommu_properties[] = {
    DEFINE_PROP_UINT32("version", RISCVIOMMUState, संस्करणम्,
        RISCV_IOMMU_SPEC_DOT_VER),
    DEFINE_PROP_UINT32("pas-bits", RISCVIOMMUState, pas_द्वयङ्काः, 0),
    DEFINE_PROP_UINT32("bus", RISCVIOMMUState, bus, 0x0),
    DEFINE_PROP_UINT32("ioatc-limit", RISCVIOMMUState, iot_सीमा,
        सीमा_CACHE_IOT),
    DEFINE_PROP_BOOL("intremap", RISCVIOMMUState, enable_msi, TRUE),
    DEFINE_PROP_BOOL("ats", RISCVIOMMUState, enable_ats, TRUE),
    DEFINE_PROP_BOOL("off", RISCVIOMMUState, enable_off, TRUE),
    DEFINE_PROP_BOOL("s-stage", RISCVIOMMUState, enable_s_stage, TRUE),
    DEFINE_PROP_BOOL("g-stage", RISCVIOMMUState, enable_g_stage, TRUE),
    DEFINE_PROP_LINK("downstream-mr", RISCVIOMMUState, target_mr,
        TYPE_स्मृतिः_REGION, स्मृतिः_Region *),
    DEFINE_PROP_UINT8("hpm-counters", RISCVIOMMUState, hpm_cntrs,
                      RISCV_IOMMU_IOCOUNT_अङ्कः),
};

स्थिर शून्य riscv_iommu_class_init(ObjectClass *klass, ध्रुव शून्य *दत्त)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    /* internal device for riscv-iommu-{pci/sys}, not user-creatable */
    dc->user_creatable = false;
    dc->realize = riscv_iommu_realize;
    dc->unrealize = riscv_iommu_unrealize;
    device_class_समुच्चयः_props(dc, riscv_iommu_properties);
}

स्थिर ध्रुव TypeInfo riscv_iommu_info = {
    .name = TYPE_RISCV_IOMMU,
    .parent = TYPE_DEVICE,
    .instance_आकार = परिमाण(RISCVIOMMUState),
    .instance_init = riscv_iommu_instance_init,
    .instance_finalize = riscv_iommu_instance_finalize,
    .class_init = riscv_iommu_class_init,
};

स्थिर ध्रुव वर्ण *IOMMU_FLAG_STR[] = {
    "NA",
    "RO",
    "WR",
    "RW",
};

/* RISC-V IOMMU Memory Region - Address Translation Space */
स्थिर IOMMUTLBEntry riscv_iommu_स्मृतिः_region_translate(
    IOMMUMemoryRegion *iommu_mr, hwaddr addr,
    IOMMUAccessFlags flag, पूर्णाङ्क iommu_idx)
{
    RISCVIOMMUSpace *as = container_सम्बन्धि(iommu_mr, RISCVIOMMUSpace, iova_mr);
    RISCVIOMMUContext *ctx;
    शून्य *ref;
    IOMMUTLBEntry iotlb = {
        .iova = addr,
        .target_as = as->iommu->target_as,
        .addr_आवरणम् = ~0ULL,
        .perm = flag,
    };
    uint32_t devid = riscv_iommu_space_devid(as);

    ctx = riscv_iommu_ctx(as->iommu, devid, iommu_idx, flag, addr, &ref);
    यदि (ctx == रिक्त) {
        /* Translation disabled or invalid. */
        iotlb.addr_आवरणम् = 0;
        iotlb.perm = IOMMU_NONE;
    } अन्यथा यदि (riscv_iommu_translate(as->iommu, ctx, &iotlb, true)) {
        /* Translation disabled or fault reported. */
        iotlb.addr_आवरणम् = 0;
        iotlb.perm = IOMMU_NONE;
    }

    /* Trace all dma translations with original access flags. */
    trace_riscv_iommu_dma(as->iommu->parent_obj.id, PCI_BUS_अङ्कः(devid),
                          PCI_SLOT(devid), PCI_FUNC(devid), iommu_idx,
                          IOMMU_FLAG_STR[flag & IOMMU_RW], iotlb.iova,
                          iotlb.translated_addr);

    riscv_iommu_ctx_put(as->iommu, ref);

    प्रत्यागम iotlb;
}

स्थिर पूर्णाङ्क riscv_iommu_स्मृतिः_region_notify(
    IOMMUMemoryRegion *iommu_mr, IOMMUNotifierFlag old,
    IOMMUNotifierFlag new, Error **errp)
{
    RISCVIOMMUSpace *as = container_सम्बन्धि(iommu_mr, RISCVIOMMUSpace, iova_mr);

    यदि (old == IOMMU_NOTIFIER_NONE) {
        as->notifier = true;
        trace_riscv_iommu_notifier_सङ्कलय(iommu_mr->parent_obj.name);
    } अन्यथा यदि (new == IOMMU_NOTIFIER_NONE) {
        as->notifier = false;
        trace_riscv_iommu_notifier_del(iommu_mr->parent_obj.name);
    }

    प्रत्यागम 0;
}

स्थिर अन्तःस्थ bool pci_is_iommu(PCIDevice *pdev)
{
    प्रत्यागम pci_get_word(pdev->config + PCI_CLASS_DEVICE) == 0x0806;
}

स्थिर स्थानाङ्कः_Space *riscv_iommu_find_as(PCIBus *bus, शून्य *opaque, पूर्णाङ्क devfn)
{
    RISCVIOMMUState *s = (RISCVIOMMUState *) opaque;
    PCIDevice *pdev = pci_find_device(bus, pci_bus_अङ्कः(bus), devfn);
    स्थानाङ्कः_Space *as = रिक्त;

    यदि (pdev && pci_is_iommu(pdev)) {
        प्रत्यागम s->target_as;
    }

    /* Find first registered IOMMU device */
    यावत् (s->iommus.le_पूर्व) {
        s = *(s->iommus.le_पूर्व);
    }

    /* Find first matching IOMMU */
    यावत् (s != रिक्त && as == रिक्त) {
        as = riscv_iommu_space(s, bus, devfn);
        s = s->iommus.le_अग्र;
    }

    प्रत्यागम as ? as : &स्थानाङ्कः_space_स्मृतिः;
}

स्थिर ध्रुव PCIIOMMUOps riscv_iommu_ops = {
    .get_स्थानाङ्कः_space = riscv_iommu_find_as,
};

शून्य riscv_iommu_pci_सज्जता_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
        Error **errp)
{
    यदि (bus->iommu_ops &&
        bus->iommu_ops->get_स्थानाङ्कः_space == riscv_iommu_find_as) {
        /* Allow multiple IOMMUs on the same PCIe bus, link known devices */
        RISCVIOMMUState *चरमः = (RISCVIOMMUState *)bus->iommu_opaque;
        QLIST_निवेशय_AFTER(चरमः, iommu, iommus);
    } अन्यथा यदि (!bus->iommu_ops && !bus->iommu_opaque) {
        pci_सज्जता_iommu(bus, &riscv_iommu_ops, iommu);
    } अन्यथा {
        error_setg(errp, "can't register secondary IOMMU for PCI bus #%d",
            pci_bus_अङ्कः(bus));
    }
}

स्थिर पूर्णाङ्क riscv_iommu_स्मृतिः_region_सूचकांक(IOMMUMemoryRegion *iommu_mr,
    MemTxAttrs attrs)
{
    प्रत्यागम attrs.unspecified ? RISCV_IOMMU_NOPROCID : (पूर्णाङ्क)attrs.pid;
}

स्थिर पूर्णाङ्क riscv_iommu_स्मृतिः_region_सूचकांक_len(IOMMUMemoryRegion *iommu_mr)
{
    RISCVIOMMUSpace *as = container_सम्बन्धि(iommu_mr, RISCVIOMMUSpace, iova_mr);
    प्रत्यागम 1 << as->iommu->pid_द्वयङ्काः;
}

स्थिर शून्य riscv_iommu_स्मृतिः_region_init(ObjectClass *klass, ध्रुव शून्य *दत्त)
{
    IOMMUMemoryRegionClass *imrc = IOMMU_स्मृतिः_REGION_CLASS(klass);

    imrc->translate = riscv_iommu_स्मृतिः_region_translate;
    imrc->notify_flag_परिवर्तितम् = riscv_iommu_स्मृतिः_region_notify;
    imrc->attrs_गन्तव्यम्_सूचकांक = riscv_iommu_स्मृतिः_region_सूचकांक;
    imrc->अङ्कः_indexes = riscv_iommu_स्मृतिः_region_सूचकांक_len;
}

स्थिर ध्रुव TypeInfo riscv_iommu_स्मृतिः_region_info = {
    .parent = TYPE_IOMMU_स्मृतिः_REGION,
    .name = TYPE_RISCV_IOMMU_स्मृतिः_REGION,
    .class_init = riscv_iommu_स्मृतिः_region_init,
};

स्थिर शून्य riscv_iommu_पञ्जिकाकोशः_mr_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&riscv_iommu_स्मृतिः_region_info);
    type_पञ्जिकाकोशः_static(&riscv_iommu_info);
}

type_init(riscv_iommu_पञ्जिकाकोशः_mr_types);
