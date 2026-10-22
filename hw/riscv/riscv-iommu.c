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

#define LIMIT_CACHE_CTX               (1U << 7)
#define LIMIT_CACHE_IOT               (1U << 20)

/* Physical page number coversions */
#define PPN_PHYS(ppn)                 ((ppn) << TARGET_PAGE_BITS)
#define PPN_DOWN(phy)                 ((phy) >> TARGET_PAGE_BITS)

प्रकारनिर्देश संरचना RISCVIOMMUEntry RISCVIOMMUEntry;

/* Device assigned I/O address space */
संरचना RISCVIOMMUSpace {
    IOMMUMemoryRegion iova_mr;  /* IOVA memory region for attached device */
    AddressSpace iova_as;       /* IOVA address space for attached device */
    RISCVIOMMUState *iommu;     /* Managing IOMMU device state */
    PCIBus *bus;                /* PCI bus of the requester */
    uint8_t devfn;              /* Requester identifier, AKA device_id */
    bool notifier;              /* IOMMU unmap notifier enabled */
    QLIST_ENTRY(RISCVIOMMUSpace) list;
};

प्रकारनिर्देश गणना RISCVIOMMUTransTag {
    RISCV_IOMMU_TRANS_TAG_BY,  /* Bypass */
    RISCV_IOMMU_TRANS_TAG_SS,  /* Single Stage */
    RISCV_IOMMU_TRANS_TAG_VG,  /* G-stage only */
    RISCV_IOMMU_TRANS_TAG_VN,  /* Nested translation */
} RISCVIOMMUTransTag;

/* Address translation cache entry */
संरचना RISCVIOMMUEntry {
    RISCVIOMMUTransTag tag;     /* Translation Tag */
    uint64_t iova:44;           /* IOVA Page Number */
    uint64_t pscid:20;          /* Process Soft-Context identifier */
    uint64_t phys:44;           /* Physical Page Number */
    uint64_t gscid:16;          /* Guest Soft-Context identifier */
    uint64_t perm:2;            /* IOMMU_RW flags */
};

/* IOMMU index for transactions without process_id specified. */
#define RISCV_IOMMU_NOPROCID 0

स्थिर uint32_t riscv_iommu_space_devid(RISCVIOMMUSpace *as)
{
    uint32_t devid = PCI_BUILD_BDF(pci_bus_अङ्कः(as->bus), as->devfn);

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= as->iommu->bus << 8;
    पटिनिवत्त devid;
}

स्थिर uint8_t riscv_iommu_get_icvec_vector(uint32_t icvec, uint32_t vec_type)
{
    विकल्प (vec_type) {
    पक्ष RISCV_IOMMU_INTR_CQ:
        पटिनिवत्त icvec & RISCV_IOMMU_ICVEC_CIV;
    पक्ष RISCV_IOMMU_INTR_FQ:
        पटिनिवत्त (icvec & RISCV_IOMMU_ICVEC_FIV) >> 4;
    पक्ष RISCV_IOMMU_INTR_PM:
        पटिनिवत्त (icvec & RISCV_IOMMU_ICVEC_PMIV) >> 8;
    पक्ष RISCV_IOMMU_INTR_PQ:
        पटिनिवत्त (icvec & RISCV_IOMMU_ICVEC_PIV) >> 12;
    स्वभाव:
        g_assert_not_reached();
    }
}

शून्य riscv_iommu_notify(RISCVIOMMUState *s, पूर्णाङ्क vec_type)
{
    uint32_t ipsr, icvec, vector;

    सचे (!s->notify) {
        पटिनिवत्त;
    }

    icvec = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_ICVEC);
    ipsr = riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, (1 << vec_type), 0);

    सचे (!(ipsr & (1 << vec_type))) {
        vector = riscv_iommu_get_icvec_vector(icvec, vec_type);
        s->notify(s, vector);
        trace_riscv_iommu_notify_int_vector(vec_type, vector);
    }
}

शून्य riscv_iommu_fault(RISCVIOMMUState *s, संरचना riscv_iommu_fq_record *ev)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t head = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQH) & s->fq_mask;
    uint32_t tail = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQT) & s->fq_mask;
    uint32_t next = (tail + 1) & s->fq_mask;
    uint32_t devid = get_field(ev->hdr, RISCV_IOMMU_FQ_HDR_DID);

    trace_riscv_iommu_flt(s->parent_obj.id, PCI_BUS_अङ्कः(devid), PCI_SLOT(devid),
                          PCI_FUNC(devid), ev->hdr, ev->iotval);

    सचे (!(ctrl & RISCV_IOMMU_FQCSR_FQON) ||
        !!(ctrl & (RISCV_IOMMU_FQCSR_FQOF | RISCV_IOMMU_FQCSR_FQMF))) {
        पटिनिवत्त;
    }

    सचे (head == next) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                              RISCV_IOMMU_FQCSR_FQOF, 0);
    } अञ्ञथा {
        dma_addr_t addr = s->fq_addr + tail * परिमाण(*ev);
        सचे (dma_memory_write(s->target_as, addr, ev, परिमाण(*ev),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                                  RISCV_IOMMU_FQCSR_FQMF, 0);
        } अञ्ञथा {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_FQT, next);
        }
    }

    सचे (ctrl & RISCV_IOMMU_FQCSR_FIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_FQ);
    }
}

स्थिर शून्य riscv_iommu_pri(RISCVIOMMUState *s,
    संरचना riscv_iommu_pq_record *pr)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t head = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQH) & s->pq_mask;
    uint32_t tail = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQT) & s->pq_mask;
    uint32_t next = (tail + 1) & s->pq_mask;
    uint32_t devid = get_field(pr->hdr, RISCV_IOMMU_PREQ_HDR_DID);

    trace_riscv_iommu_pri(s->parent_obj.id, PCI_BUS_अङ्कः(devid), PCI_SLOT(devid),
                          PCI_FUNC(devid), pr->payload);

    सचे (!(ctrl & RISCV_IOMMU_PQCSR_PQON) ||
        !!(ctrl & (RISCV_IOMMU_PQCSR_PQOF | RISCV_IOMMU_PQCSR_PQMF))) {
        पटिनिवत्त;
    }

    सचे (head == next) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                              RISCV_IOMMU_PQCSR_PQOF, 0);
    } अञ्ञथा {
        dma_addr_t addr = s->pq_addr + tail * परिमाण(*pr);
        सचे (dma_memory_write(s->target_as, addr, pr, परिमाण(*pr),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                                  RISCV_IOMMU_PQCSR_PQMF, 0);
        } अञ्ञथा {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_PQT, next);
        }
    }

    सचे (ctrl & RISCV_IOMMU_PQCSR_PIE) {
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

    याव (ext) {
        सचे (ext & 1) {
            सचे (val & 1) {
                ret |= rot;
            }
            rot <<= 1;
        }
        val >>= 1;
        ext >>= 1;
    }

    पटिनिवत्त ret;
}

/* Check if GPA matches MSI/MRIF pattern. */
स्थिर bool riscv_iommu_msi_check(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    dma_addr_t gpa)
{
    सचे (!s->enable_msi) {
        पटिनिवत्त false;
    }

    सचे (get_field(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_MODE) !=
        RISCV_IOMMU_DC_MSIPTP_MODE_FLAT) {
        पटिनिवत्त false; /* Invalid MSI/MRIF mode */
    }

    सचे ((PPN_DOWN(gpa) ^ ctx->msi_addr_pattern) & ~ctx->msi_addr_mask) {
        पटिनिवत्त false; /* GPA not in MSI range defined by AIA IMSIC rules. */
    }

    पटिनिवत्त true;
}

/* Returns the NAPOT page mask, or 0 for reserved encodings. */
स्थिर hwaddr riscv_iommu_napot_page_mask(hwaddr ppn, hwaddr addr, hwaddr *out)
{
    पूर्णाङ्क napot_bits = ctz64(ppn) + 1;
    hwaddr napot_mask, page_mask;

    /* The spec only defines 64KB (napot_bits == 4) */
    सचे (napot_bits != 4) {
        पटिनिवत्त 0;
    }

    napot_mask = (1ULL << napot_bits) - 1;
    page_mask = PPN_PHYS(napot_mask) | (TARGET_PAGE_SIZE - 1);

    *out = PPN_PHYS(ppn & ~napot_mask) | (addr & page_mask);

    पटिनिवत्त page_mask;
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
    dma_addr_t addr, base;
    uint64_t satp, gatp, pte;
    bool en_s, en_g;
    संरचना {
        unsigned वर्ण step;
        unsigned वर्ण levels;
        unsigned वर्ण ptidxbits;
        unsigned वर्ण ptesize;
    } sc[2];
    /* Translation stage phase */
    गणना {
        S_STAGE = 0,
        G_STAGE = 1,
    } pass;
    MemTxResult ret;
    bool pv = !!ctx->process_id;
    /*
     * Keep the request permission separate from iotlb->perm.  G-stage
     * walks translate S-stage PTE addresses before the real leaf is
     * reached, but permission checks and fault types must still use the
     * original request.  A successful walk leaves iotlb->perm with the
     * effective leaf permission for the translation cache.
     */
    ध्रुव IOMMUAccessFlags req_perm = iotlb->perm;

    satp = get_field(ctx->satp, RISCV_IOMMU_ATP_MODE_FIELD);
    gatp = get_field(ctx->gatp, RISCV_IOMMU_ATP_MODE_FIELD);

    en_s = satp != RISCV_IOMMU_DC_FSC_MODE_BARE;
    en_g = gatp != RISCV_IOMMU_DC_IOHGATP_MODE_BARE;

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
    सचे (!en_s && (req_perm & IOMMU_WO) &&
        riscv_iommu_msi_check(s, ctx, iotlb->iova)) {
        iotlb->target_as = &s->trap_as;
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_mask = ~TARGET_PAGE_MASK;
        पटिनिवत्त 0;
    }

    /* Exit early for pass-through mode. */
    सचे (!(en_s || en_g)) {
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_mask = ~TARGET_PAGE_MASK;
        /* Allow R/W in pass-through mode */
        iotlb->perm = IOMMU_RW;
        पटिनिवत्त 0;
    }

    /* S/G translation parameters. */
    अत्थाय (pass = 0; pass < 2; pass++) {
        uint32_t sv_mode;

        sc[pass].step = 0;
        सचे (pass ? (s->fctl & RISCV_IOMMU_FCTL_GXL) :
            (ctx->tc & RISCV_IOMMU_DC_TC_SXL)) {
            /* 32bit mode for GXL/SXL == 1 */
            विकल्प (pass ? gatp : satp) {
            पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_BARE:
                sc[pass].levels    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_SV32X4:
                sv_mode = pass ? RISCV_IOMMU_CAP_SV32X4 : RISCV_IOMMU_CAP_SV32;
                सचे (!(s->cap & sv_mode)) {
                    पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].levels    = 2;
                sc[pass].ptidxbits = 10;
                sc[pass].ptesize   = 4;
                भङ्ग;
            स्वभाव:
                पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        } अञ्ञथा {
            /* 64bit mode for GXL/SXL == 0 */
            विकल्प (pass ? gatp : satp) {
            पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_BARE:
                sc[pass].levels    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_SV39X4:
                sv_mode = pass ? RISCV_IOMMU_CAP_SV39X4 : RISCV_IOMMU_CAP_SV39;
                सचे (!(s->cap & sv_mode)) {
                    पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].levels    = 3;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_SV48X4:
                sv_mode = pass ? RISCV_IOMMU_CAP_SV48X4 : RISCV_IOMMU_CAP_SV48;
                सचे (!(s->cap & sv_mode)) {
                    पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].levels    = 4;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_SV57X4:
                sv_mode = pass ? RISCV_IOMMU_CAP_SV57X4 : RISCV_IOMMU_CAP_SV57;
                सचे (!(s->cap & sv_mode)) {
                    पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].levels    = 5;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                भङ्ग;
            स्वभाव:
                पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        }
    };

    /* S/G stages translation tables root pointers */
    gatp = PPN_PHYS(get_field(ctx->gatp, RISCV_IOMMU_ATP_PPN_FIELD));
    satp = PPN_PHYS(get_field(ctx->satp, RISCV_IOMMU_ATP_PPN_FIELD));
    addr = (en_s && en_g) ? satp : iotlb->iova;
    base = en_g ? gatp : satp;
    pass = en_g ? G_STAGE : S_STAGE;

    करोहि {
        ध्रुव unsigned widened = (pass && !sc[pass].step) ? 2 : 0;
        ध्रुव unsigned va_bits = widened + sc[pass].ptidxbits;
        ध्रुव unsigned va_skip = TARGET_PAGE_BITS + sc[pass].ptidxbits *
                                 (sc[pass].levels - 1 - sc[pass].step);
        ध्रुव unsigned idx = (addr >> va_skip) & ((1 << va_bits) - 1);
        ध्रुव dma_addr_t pte_addr = base + idx * sc[pass].ptesize;
        ध्रुव bool ade =
            ctx->tc & (pass ? RISCV_IOMMU_DC_TC_GADE : RISCV_IOMMU_DC_TC_SADE);

        सचे (ade && !(s->cap & RISCV_IOMMU_CAP_AMO_HWAD)) {
            /* GADE/SADE are reserved bits if AMO_HWAD is cleared.  */
            पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }

        /* Address range check before first level lookup */
        सचे (!sc[pass].step) {
            ध्रुव uint64_t va_len = va_skip + va_bits;
            ध्रुव uint64_t va_mask = (1ULL << va_len) - 1;

            सचे (pass == S_STAGE && va_len > 32) {
                uint64_t mask, masked_msbs;

                mask = MAKE_64BIT_MASK(0, target_long_bits() - va_len + 1);
                masked_msbs = (addr >> (va_len - 1)) & mask;

                सचे (masked_msbs != 0 && masked_msbs != mask) {
                    पटिनिवत्त (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S;
                }
            } अञ्ञथा {
                सचे ((addr & va_mask) != addr) {
                    पटिनिवत्त (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS;
                }
            }
        }


        सचे (pass == S_STAGE) {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_S_VS_WALKS);
        } अञ्ञथा {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_G_WALKS);
        }

        /* Read page table entry */
        सचे (sc[pass].ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->target_as, pte_addr, &pte32,
                             MEMTXATTRS_UNSPECIFIED);
            pte = pte32;
        } अञ्ञथा {
            ret = ldq_le_dma(s->target_as, pte_addr, &pte,
                             MEMTXATTRS_UNSPECIFIED);
        }
        सचे (ret != MEMTX_OK) {
            पटिनिवत्त (req_perm & IOMMU_WO) ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT
                                         : RISCV_IOMMU_FQ_CAUSE_RD_FAULT;
        }

        sc[pass].step++;
        hwaddr ppn = pte >> PTE_PPN_SHIFT;

        सचे (!(pte & PTE_V)) {
            भङ्ग;                /* Invalid PTE */
        } अञ्ञथा सचे (pte & PTE_RESERVED(false)) {
            भङ्ग;                /* Reserved PTE bits set */
        } अञ्ञथा सचे (!(pte & (PTE_R | PTE_W | PTE_X))) {
            base = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } अञ्ञथा सचे ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            भङ्ग;                /* Reserved leaf PTE flags: PTE_W */
        } अञ्ञथा सचे ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            भङ्ग;                /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } अञ्ञथा सचे (ppn & ((1ULL << (va_skip - TARGET_PAGE_BITS)) - 1)) {
            भङ्ग;                /* Misaligned PPN */
        } अञ्ञथा सचे (!(pte & PTE_U) && !pv) {
            /*
             * All accesses are assumed to be User mode unless
             * process_id is valid (pv).  In case we have a
             * non-user mode leaf PTE and !pv we need to fault.
             */
            भङ्ग;
        } अञ्ञथा सचे ((req_perm & IOMMU_RO) && !(pte & PTE_R)) {
            भङ्ग;                /* Read access check failed */
        } अञ्ञथा सचे ((req_perm & IOMMU_WO) && !(pte & PTE_W)) {
            भङ्ग;                /* Write access check failed */
        } अञ्ञथा सचे (!ade && !(pte & PTE_A)) {
            भङ्ग;                /* Access bit not set */
        } अञ्ञथा सचे ((req_perm & IOMMU_WO) && !ade && !(pte & PTE_D)) {
            भङ्ग;                /* Dirty bit not set */
        } अञ्ञथा सचे (pass == G_STAGE && !(pte & PTE_U)) {
            /*
             * riscv-iommu spec 1.0: "When checking the U bit in a
             * second-stage PTE, the transaction is treated as
             * not requesting supervisor privilege."
             *
             * I.e. we need to fault if this is a non-user PTE since
             * we are always in user mode at this point.
             */
            भङ्ग;
        } अञ्ञथा {
            /* Leaf PTE, translation completed. */
            sc[pass].step = sc[pass].levels;

            सचे (pte & PTE_N) {
                hwaddr mask = riscv_iommu_napot_page_mask(ppn, addr, &base);

                सचे (!mask) {
                    भङ्ग;
                }
                iotlb->addr_mask &= mask;
            } अञ्ञथा {
                base = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
                /* Update address mask based on smallest translation granularity */
                iotlb->addr_mask &= (1ULL << va_skip) - 1;
            }

            /* Continue with S-Stage translation? */
            सचे (pass && sc[0].step != sc[0].levels) {
                pass = S_STAGE;
                addr = iotlb->iova;
                अनुवर्त;
            }

            /* Cache the effective permission, not this request's subset. */
            IOMMUAccessFlags leaf_perm = (pte & PTE_W) ?
                                         ((pte & PTE_R) ? IOMMU_RW : IOMMU_WO) :
                                         IOMMU_RO;

            trans_perm = trans_perm == IOMMU_NONE ?
                         leaf_perm : trans_perm & leaf_perm;

            /* Translation phase completed (GPA or SPA) */
            iotlb->translated_addr = base;

            /* Check MSI GPA address match */
            सचे (pass == S_STAGE && (req_perm & IOMMU_WO) &&
                riscv_iommu_msi_check(s, ctx, base)) {
                /* Trap MSI writes and return GPA address. */
                iotlb->target_as = &s->trap_as;
                iotlb->addr_mask = ~TARGET_PAGE_MASK;
                पटिनिवत्त 0;
            }

            /* Continue with G-Stage translation? */
            सचे (!pass && en_g) {
                pass = G_STAGE;
                addr = base;
                base = gatp;
                sc[pass].step = 0;
                अनुवर्त;
            }

            iotlb->perm = trans_perm;
            पटिनिवत्त 0;
        }

        सचे (sc[pass].step == sc[pass].levels) {
            भङ्ग; /* Can't find leaf PTE */
        }

        /* Continue with G-Stage translation? */
        सचे (!pass && en_g) {
            pass = G_STAGE;
            addr = base;
            base = gatp;
            sc[pass].step = 0;
        }
    } याव (1);

    /*
     * riscv_iommu_translate() will receive a fault and then call
     * riscv_iommu_report_fault() using iotlb->translated_addr
     * as iotval2.  Update translated_addr it with the latest
     * translated addr we have.
     */
    iotlb->translated_addr = addr;

    पटिनिवत्त (req_perm & IOMMU_WO) ?
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
    संरचना riscv_iommu_fq_record ev = { 0 };

    सचे (ctx->tc & RISCV_IOMMU_DC_TC_DTF) {
        विकल्प (cause) {
        पक्ष RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED:
        पक्ष RISCV_IOMMU_FQ_CAUSE_DDT_LOAD_FAULT:
        पक्ष RISCV_IOMMU_FQ_CAUSE_DDT_INVALID:
        पक्ष RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED:
        पक्ष RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED:
        पक्ष RISCV_IOMMU_FQ_CAUSE_INTERNAL_DP_ERROR:
        पक्ष RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT:
            भङ्ग;
        स्वभाव:
            /* DTF prevents reporting a fault for this given cause */
            पटिनिवत्त;
        }
    }

    ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE, cause);
    ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE, fault_type);
    ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_DID, ctx->devid);
    ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_PV, pv);

    सचे (pv) {
        ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_PID, ctx->process_id);
    }

    ev.iotval = iotval;
    ev.iotval2 = iotval2;

    riscv_iommu_fault(s, &ev);
}

/* Redirect MSI write for given GPA. */
स्थिर MemTxResult riscv_iommu_msi_write(RISCVIOMMUState *s,
    RISCVIOMMUContext *ctx, uint64_t gpa, uint64_t data,
    unsigned size, MemTxAttrs attrs)
{
    MemTxResult res;
    dma_addr_t addr;
    uint64_t intn;
    size_t offset;
    uint32_t n190;
    uint64_t pte[2];
    पूर्णाङ्क fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    पूर्णाङ्क cause;

    /* Interrupt File Number */
    intn = riscv_iommu_pext_u64(PPN_DOWN(gpa), ctx->msi_addr_mask);
    offset = intn * परिमाण(pte);

    /* fetch MSI PTE */
    addr = PPN_PHYS(get_field(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_PPN));
    सचे (addr & offset) {
        /* Interrupt file number out of range */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_LOAD_FAULT;
        गच्छ err;
    }

    addr |= offset;
    res = dma_memory_read(s->target_as, addr, &pte, परिमाण(pte),
            MEMTXATTRS_UNSPECIFIED);
    सचे (res != MEMTX_OK) {
        सचे (res == MEMTX_DECODE_ERROR) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED;
        } अञ्ञथा {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_LOAD_FAULT;
        }
        गच्छ err;
    }

    le64_to_cpus(&pte[0]);
    le64_to_cpus(&pte[1]);

    सचे (!(pte[0] & RISCV_IOMMU_MSI_PTE_V) || (pte[0] & RISCV_IOMMU_MSI_PTE_C)) {
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

    विकल्प (get_field(pte[0], RISCV_IOMMU_MSI_PTE_M)) {
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
        सचे (pte[0] & (GENMASK_ULL(62, 54) | GENMASK_ULL(9, 3))) {
            res = MEMTX_DECODE_ERROR;
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
            गच्छ err;
        }

        /* MSI Pass-through mode */
        addr = PPN_PHYS(get_field(pte[0], RISCV_IOMMU_MSI_PTE_PPN));

        trace_riscv_iommu_msi(s->parent_obj.id, PCI_BUS_अङ्कः(ctx->devid),
                              PCI_SLOT(ctx->devid), PCI_FUNC(ctx->devid),
                              gpa, addr);

        res = dma_memory_write(s->target_as, addr, &data, size, attrs);
        सचे (res != MEMTX_OK) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
            गच्छ err;
        }

        पटिनिवत्त MEMTX_OK;
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
    सचे ((data > 2047) || (gpa & 3)) {
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        गच्छ err;
    }

    /* MSI MRIF mode, non atomic pending bit update */

    /* MRIF pending bit address */
    addr = get_field(pte[0], RISCV_IOMMU_MSI_PTE_MRIF_ADDR) << 9;
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
    addr = addr | ((data & 0x7c0) >> 2);

    trace_riscv_iommu_msi(s->parent_obj.id, PCI_BUS_अङ्कः(ctx->devid),
                          PCI_SLOT(ctx->devid), PCI_FUNC(ctx->devid),
                          gpa, addr);

    /* MRIF pending bit mask */
    data = 1ULL << (data & 0x03f);
    res = dma_memory_read(s->target_as, addr, &intn, परिमाण(intn), attrs);
    सचे (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_LOAD_FAULT;
        गच्छ err;
    }

    intn = intn | data;
    res = dma_memory_write(s->target_as, addr, &intn, परिमाण(intn), attrs);
    सचे (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        गच्छ err;
    }

    /* Get MRIF enable bits */
    addr = addr + परिमाण(intn);
    res = dma_memory_read(s->target_as, addr, &intn, परिमाण(intn), attrs);
    सचे (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_LOAD_FAULT;
        गच्छ err;
    }

    सचे (!(intn & data)) {
        /* notification disabled, MRIF update completed. */
        पटिनिवत्त MEMTX_OK;
    }

    /* Send notification message */
    addr = PPN_PHYS(get_field(pte[1], RISCV_IOMMU_MSI_MRIF_NPPN));
    n190 = get_field(pte[1], RISCV_IOMMU_MSI_MRIF_NID) |
          (get_field(pte[1], RISCV_IOMMU_MSI_MRIF_NID_MSB) << 10);

    res = dma_memory_write(s->target_as, addr, &n190, परिमाण(n190), attrs);
    सचे (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        गच्छ err;
    }

    trace_riscv_iommu_mrif_notification(s->parent_obj.id, n190, addr);

    पटिनिवत्त MEMTX_OK;

err:
    riscv_iommu_report_fault(s, ctx, fault_type, cause,
                             !!ctx->process_id, 0, 0);
    पटिनिवत्त res;
}

/*
 * Check device context configuration as described by the
 * riscv-iommu spec section "Device-context configuration
 * checks".
 */
स्थिर bool riscv_iommu_validate_device_ctx(RISCVIOMMUState *s,
                                            RISCVIOMMUContext *ctx)
{
    uint32_t fsc_mode, msi_mode;
    uint64_t gatp;

    सचे (ctx->tc & RISCV_IOMMU_DC_TC_RESERVED) {
        पटिनिवत्त false;
    }

    सचे (!(s->cap & RISCV_IOMMU_CAP_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI ||
         ctx->tc & RISCV_IOMMU_DC_TC_PRPR)) {
        पटिनिवत्त false;
    }

    सचे (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI)) {
        पटिनिवत्त false;
    }

    सचे (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI) &&
        ctx->tc & RISCV_IOMMU_DC_TC_PRPR) {
        पटिनिवत्त false;
    }

    सचे (!(s->cap & RISCV_IOMMU_CAP_T2GPA) &&
        ctx->tc & RISCV_IOMMU_DC_TC_T2GPA) {
        पटिनिवत्त false;
    }

    सचे (s->cap & RISCV_IOMMU_CAP_MSI_FLAT) {
        msi_mode = get_field(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_MODE);

        सचे (msi_mode != RISCV_IOMMU_DC_MSIPTP_MODE_OFF &&
            msi_mode != RISCV_IOMMU_DC_MSIPTP_MODE_FLAT) {
            पटिनिवत्त false;
        }
    }

    gatp = get_field(ctx->gatp, RISCV_IOMMU_ATP_MODE_FIELD);
    सचे (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA &&
        gatp == RISCV_IOMMU_DC_IOHGATP_MODE_BARE) {
        पटिनिवत्त false;
    }

    सचे (gatp != RISCV_IOMMU_DC_IOHGATP_MODE_BARE) {
        uint64_t iohgatp_ppn = get_field(ctx->gatp,
                                         RISCV_IOMMU_DC_IOHGATP_PPN);

        /*
         * One of the conditions for a misconfigured DDT entry
         * according to the riscv-spec: "DC.iohgatp.MODE is not
         * Bare and the root page table (address) determined by
         * DC.iohgatp.PPN is not aligned to a 16-KiB boundary."
         */
        सचे (PPN_PHYS(iohgatp_ppn) & ((1ULL << 14) - 1)) {
            पटिनिवत्त false;
        }
    }

    fsc_mode = get_field(ctx->satp, RISCV_IOMMU_DC_FSC_MODE);

    सचे (ctx->tc & RISCV_IOMMU_DC_TC_PDTV) {
        विकल्प (fsc_mode) {
        पक्ष RISCV_IOMMU_DC_FSC_PDTP_MODE_PD8:
            सचे (!(s->cap & RISCV_IOMMU_CAP_PD8)) {
                पटिनिवत्त false;
            }
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_FSC_PDTP_MODE_PD17:
            सचे (!(s->cap & RISCV_IOMMU_CAP_PD17)) {
                पटिनिवत्त false;
            }
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_FSC_PDTP_MODE_PD20:
            सचे (!(s->cap & RISCV_IOMMU_CAP_PD20)) {
                पटिनिवत्त false;
            }
            भङ्ग;
        }
    } अञ्ञथा {
        /* DC.tc.PDTV is 0 */
        सचे (ctx->tc & RISCV_IOMMU_DC_TC_DPE) {
            पटिनिवत्त false;
        }

        सचे (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
            सचे (fsc_mode == RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV32 &&
                !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                पटिनिवत्त false;
            }
        } अञ्ञथा {
            विकल्प (fsc_mode) {
            पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV39:
                सचे (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                    पटिनिवत्त false;
                }
                भङ्ग;
            पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV48:
                सचे (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                    पटिनिवत्त false;
                }
            भङ्ग;
            पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV57:
                सचे (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                    पटिनिवत्त false;
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
    सचे (ctx->tc & RISCV_IOMMU_DC_TC_SBE) {
        पटिनिवत्त false;
    }

    पटिनिवत्त true;
}

/*
 * Validate process context (PC) according to section
 * "Process-context configuration checks".
 */
स्थिर bool riscv_iommu_validate_process_ctx(RISCVIOMMUState *s,
                                             RISCVIOMMUContext *ctx)
{
    uint32_t mode;

    सचे (get_field(ctx->ta, RISCV_IOMMU_PC_TA_RESERVED)) {
        पटिनिवत्त false;
    }

    सचे (get_field(ctx->satp, RISCV_IOMMU_PC_FSC_RESERVED)) {
        पटिनिवत्त false;
    }

    mode = get_field(ctx->satp, RISCV_IOMMU_DC_FSC_MODE);
    विकल्प (mode) {
    पक्ष RISCV_IOMMU_DC_FSC_MODE_BARE:
    /* sv39 and sv32 modes have the same value (8) */
    पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV39:
    पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV48:
    पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV57:
        भङ्ग;
    स्वभाव:
        पटिनिवत्त false;
    }

    सचे (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
        सचे (mode == RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV32 &&
            !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                पटिनिवत्त false;
        }
    } अञ्ञथा {
        विकल्प (mode) {
        पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV39:
            सचे (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                पटिनिवत्त false;
            }
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV48:
            सचे (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                पटिनिवत्त false;
            }
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV57:
            सचे (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                पटिनिवत्त false;
            }
            भङ्ग;
        }
    }

    पटिनिवत्त true;
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
स्थिर MemTxResult pdt_memory_read(RISCVIOMMUState *s,
                                   RISCVIOMMUContext *ctx,
                                   dma_addr_t addr,
                                   शून्य *buf, dma_addr_t len,
                                   MemTxAttrs attrs)
{
    uint64_t gatp_mode, pte;
    संरचना {
        unsigned वर्ण step;
        unsigned वर्ण levels;
        unsigned वर्ण ptidxbits;
        unsigned वर्ण ptesize;
    } sc;
    MemTxResult ret;
    dma_addr_t base = addr;

    /* G stages translation mode */
    gatp_mode = get_field(ctx->gatp, RISCV_IOMMU_ATP_MODE_FIELD);
    सचे (gatp_mode == RISCV_IOMMU_DC_IOHGATP_MODE_BARE) {
        गच्छ out;
    }

    /* G stages translation tables root pointer */
    base = PPN_PHYS(get_field(ctx->gatp, RISCV_IOMMU_ATP_PPN_FIELD));

    /* Start at step 0 */
    sc.step = 0;

    सचे (s->fctl & RISCV_IOMMU_FCTL_GXL) {
        /* 32bit mode for GXL == 1 */
        विकल्प (gatp_mode) {
        पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_SV32X4:
            सचे (!(s->cap & RISCV_IOMMU_CAP_SV32X4)) {
                पटिनिवत्त MEMTX_ACCESS_ERROR;
            }
            sc.levels    = 2;
            sc.ptidxbits = 10;
            sc.ptesize   = 4;
            भङ्ग;
        स्वभाव:
            पटिनिवत्त MEMTX_ACCESS_ERROR;
        }
    } अञ्ञथा {
        /* 64bit mode for GXL == 0 */
        विकल्प (gatp_mode) {
        पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_SV39X4:
            सचे (!(s->cap & RISCV_IOMMU_CAP_SV39X4)) {
                पटिनिवत्त MEMTX_ACCESS_ERROR;
            }
            sc.levels    = 3;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_SV48X4:
            सचे (!(s->cap & RISCV_IOMMU_CAP_SV48X4)) {
                पटिनिवत्त MEMTX_ACCESS_ERROR;
            }
            sc.levels    = 4;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            भङ्ग;
        पक्ष RISCV_IOMMU_DC_IOHGATP_MODE_SV57X4:
            सचे (!(s->cap & RISCV_IOMMU_CAP_SV57X4)) {
                पटिनिवत्त MEMTX_ACCESS_ERROR;
            }
            sc.levels    = 5;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            भङ्ग;
        स्वभाव:
            पटिनिवत्त MEMTX_ACCESS_ERROR;
        }
    }

    करोहि {
        ध्रुव unsigned va_bits = (sc.step ? 0 : 2) + sc.ptidxbits;
        ध्रुव unsigned va_skip = TARGET_PAGE_BITS + sc.ptidxbits *
                                 (sc.levels - 1 - sc.step);
        ध्रुव unsigned idx = (addr >> va_skip) & ((1 << va_bits) - 1);
        ध्रुव dma_addr_t pte_addr = base + idx * sc.ptesize;

        /* Address range check before first level lookup */
        सचे (!sc.step) {
            ध्रुव uint64_t va_mask = (1ULL << (va_skip + va_bits)) - 1;
            सचे ((addr & va_mask) != addr) {
                पटिनिवत्त MEMTX_ACCESS_ERROR;
            }
        }

        /* Read page table entry */
        सचे (sc.ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->target_as, pte_addr, &pte32, attrs);
            pte = pte32;
        } अञ्ञथा {
            ret = ldq_le_dma(s->target_as, pte_addr, &pte, attrs);
        }
        सचे (ret != MEMTX_OK) {
            पटिनिवत्त ret;
        }

        sc.step++;
        hwaddr ppn = pte >> PTE_PPN_SHIFT;

        सचे (!(pte & PTE_V)) {
            पटिनिवत्त MEMTX_ACCESS_ERROR; /* Invalid PTE */
        } अञ्ञथा सचे (!(pte & (PTE_R | PTE_W | PTE_X))) {
            base = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } अञ्ञथा सचे ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            पटिनिवत्त MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W */
        } अञ्ञथा सचे ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            पटिनिवत्त MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } अञ्ञथा सचे (ppn & ((1ULL << (va_skip - TARGET_PAGE_BITS)) - 1)) {
            पटिनिवत्त MEMTX_ACCESS_ERROR; /* Misaligned PPN */
        } अञ्ञथा {
            /* Leaf PTE, translation completed. */
            सचे (pte & PTE_N) {
                सचे (!riscv_iommu_napot_page_mask(ppn, addr, &base)) {
                    पटिनिवत्त MEMTX_ACCESS_ERROR;
                }
            } अञ्ञथा {
                base = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
            }
            भङ्ग;
        }

        सचे (sc.step == sc.levels) {
            पटिनिवत्त MEMTX_ACCESS_ERROR; /* Can't find leaf PTE */
        }
    } याव (1);

out:
    पटिनिवत्त dma_memory_read(s->target_as, base, buf, len, attrs);
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
    unsigned mode = get_field(ddtp, RISCV_IOMMU_DDTP_MODE);
    dma_addr_t addr = PPN_PHYS(get_field(ddtp, RISCV_IOMMU_DDTP_PPN));
    संरचना riscv_iommu_dc dc;
    /* Device Context format: 0: extended (64 bytes) | 1: base (32 bytes) */
    ध्रुव पूर्णाङ्क dc_fmt = !s->enable_msi;
    ध्रुव size_t dc_len = परिमाण(dc) >> dc_fmt;
    पूर्णाङ्क depth;
    uint64_t de;

    विकल्प (mode) {
    पक्ष RISCV_IOMMU_DDTP_MODE_OFF:
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED;

    पक्ष RISCV_IOMMU_DDTP_MODE_BARE:
        /* mock up pass-through translation context */
        ctx->gatp = set_field(0, RISCV_IOMMU_ATP_MODE_FIELD,
            RISCV_IOMMU_DC_IOHGATP_MODE_BARE);
        ctx->satp = set_field(0, RISCV_IOMMU_ATP_MODE_FIELD,
            RISCV_IOMMU_DC_FSC_MODE_BARE);

        ctx->tc = RISCV_IOMMU_DC_TC_V;
        सचे (s->enable_ats) {
            ctx->tc |= RISCV_IOMMU_DC_TC_EN_ATS;
        }

        ctx->ta = 0;
        ctx->msiptp = 0;
        पटिनिवत्त 0;

    पक्ष RISCV_IOMMU_DDTP_MODE_1LVL:
        depth = 0;
        भङ्ग;

    पक्ष RISCV_IOMMU_DDTP_MODE_2LVL:
        depth = 1;
        भङ्ग;

    पक्ष RISCV_IOMMU_DDTP_MODE_3LVL:
        depth = 2;
        भङ्ग;

    स्वभाव:
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /*
     * Check supported device id width (in bits).
     * See IOMMU Specification, Chapter 6. Software guidelines.
     * - if extended device-context format is used:
     *   1LVL: 6, 2LVL: 15, 3LVL: 24
     * - if base device-context format is used:
     *   1LVL: 7, 2LVL: 16, 3LVL: 24
     */
    सचे (ctx->devid >= (1 << (depth * 9 + 6 + (dc_fmt && depth != 2)))) {
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
    }

    /* Device directory tree walk */
    अत्थाय (; depth-- > 0; ) {
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
        ध्रुव पूर्णाङ्क split = depth * 9 + 6 + dc_fmt;
        addr |= ((ctx->devid >> split) << 3) & ~TARGET_PAGE_MASK;
        सचे (dma_memory_read(s->target_as, addr, &de, परिमाण(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_LOAD_FAULT;
        }
        le64_to_cpus(&de);
        सचे (!(de & RISCV_IOMMU_DDTE_VALID)) {
            /* invalid directory entry */
            पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        सचे (de & ~(RISCV_IOMMU_DDTE_PPN | RISCV_IOMMU_DDTE_VALID)) {
            /* reserved bits set */
            पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }
        addr = PPN_PHYS(get_field(de, RISCV_IOMMU_DDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);

    /* index into device context entry page */
    addr |= (ctx->devid * dc_len) & ~TARGET_PAGE_MASK;

    memset(&dc, 0, परिमाण(dc));
    सचे (dma_memory_read(s->target_as, addr, &dc, dc_len,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_LOAD_FAULT;
    }

    /* Set translation context. */
    ctx->tc = le64_to_cpu(dc.tc);
    ctx->gatp = le64_to_cpu(dc.iohgatp);
    ctx->satp = le64_to_cpu(dc.fsc);
    ctx->ta = le64_to_cpu(dc.ta);
    ctx->msiptp = le64_to_cpu(dc.msiptp);
    ctx->msi_addr_mask = le64_to_cpu(dc.msi_addr_mask);
    ctx->msi_addr_pattern = le64_to_cpu(dc.msi_addr_pattern);

    सचे (!(ctx->tc & RISCV_IOMMU_DC_TC_V)) {
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
    }

    सचे (!riscv_iommu_validate_device_ctx(s, ctx)) {
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /* FSC field checks */
    mode = get_field(ctx->satp, RISCV_IOMMU_DC_FSC_MODE);
    addr = PPN_PHYS(get_field(ctx->satp, RISCV_IOMMU_DC_FSC_PPN));

    सचे (!(ctx->tc & RISCV_IOMMU_DC_TC_PDTV)) {
        सचे (ctx->process_id != RISCV_IOMMU_NOPROCID) {
            /* PID is disabled */
            पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
        }
        सचे (mode > RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV57) {
            /* Invalid translation mode */
            पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        पटिनिवत्त 0;
    }

    सचे (ctx->process_id == RISCV_IOMMU_NOPROCID) {
        सचे (!(ctx->tc & RISCV_IOMMU_DC_TC_DPE)) {
            /* No default process_id enabled, set BARE mode */
            ctx->satp = 0ULL;
            पटिनिवत्त 0;
        } अञ्ञथा {
            /* Use default process_id #0 */
            ctx->process_id = 0;
        }
    }

    सचे (mode == RISCV_IOMMU_DC_FSC_MODE_BARE) {
        /* No S-Stage translation, done. */
        पटिनिवत्त 0;
    }

    /* FSC.TC.PDTV enabled */
    सचे (mode > RISCV_IOMMU_DC_FSC_PDTP_MODE_PD20) {
        /* Invalid PDTP.MODE */
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    अत्थाय (depth = mode - RISCV_IOMMU_DC_FSC_PDTP_MODE_PD8; depth-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

        /*
         * Select process id index bits based on process directory tree
         * level. See IOMMU Specification, 2.2. Process-Directory-Table.
         */
        ध्रुव पूर्णाङ्क split = depth * 9 + 8;
        addr |= ((ctx->process_id >> split) << 3) & ~TARGET_PAGE_MASK;
        सचे (pdt_memory_read(s, ctx, addr, &de, परिमाण(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_PDT_LOAD_FAULT;
        }
        le64_to_cpus(&de);
        सचे (!(de & RISCV_IOMMU_PDTE_VALID)) {
            पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
        }
        addr = PPN_PHYS(get_field(de, RISCV_IOMMU_PDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

    /* Leaf entry in PDT */
    addr |= (ctx->process_id << 4) & ~TARGET_PAGE_MASK;
    सचे (pdt_memory_read(s, ctx, addr, &dc.ta, परिमाण(uint64_t) * 2,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_PDT_LOAD_FAULT;
    }

    /* Use FSC and TA from process directory entry. */
    ctx->ta = le64_to_cpu(dc.ta);
    ctx->satp = le64_to_cpu(dc.fsc);

    सचे (!(ctx->ta & RISCV_IOMMU_PC_TA_V)) {
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
    }

    सचे (!riscv_iommu_validate_process_ctx(s, ctx)) {
        पटिनिवत्त RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    पटिनिवत्त 0;
}

/* Translation Context cache support */
स्थिर gboolean riscv_iommu_ctx_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUContext *c1 = (RISCVIOMMUContext *) v1;
    RISCVIOMMUContext *c2 = (RISCVIOMMUContext *) v2;
    पटिनिवत्त c1->devid == c2->devid &&
           c1->process_id == c2->process_id;
}

स्थिर guint riscv_iommu_ctx_hash(gconstpointer v)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) v;
    /*
     * Generate simple hash of (process_id, devid)
     * assuming 24-bit wide devid.
     */
    पटिनिवत्त (guint)(ctx->devid) + ((guint)(ctx->process_id) << 24);
}

स्थिर शून्य riscv_iommu_ctx_inval_devid_procid(gpointer कुञ्चिका, gpointer मानम्,
                                               gpointer data)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मानम्;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) data;
    सचे (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid &&
        ctx->process_id == arg->process_id) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

स्थिर शून्य riscv_iommu_ctx_inval_devid(gpointer कुञ्चिका, gpointer मानम्,
                                        gpointer data)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मानम्;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) data;
    सचे (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

स्थिर शून्य riscv_iommu_ctx_inval_all(gpointer कुञ्चिका, gpointer मानम्,
                                      gpointer data)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मानम्;
    सचे (ctx->tc & RISCV_IOMMU_DC_TC_V) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

स्थिर शून्य riscv_iommu_ctx_inval(RISCVIOMMUState *s, GHFunc func,
                                  uint32_t devid, uint32_t process_id)
{
    GHashTable *ctx_cache;
    RISCVIOMMUContext कुञ्चिका = {
        .devid = devid,
        .process_id = process_id,
    };
    ctx_cache = g_hash_table_ref(s->ctx_cache);
    g_hash_table_foreach(ctx_cache, func, &कुञ्चिका);
    g_hash_table_unref(ctx_cache);
}

/* Find or allocate translation context for a given {device_id, process_id} */
स्थिर RISCVIOMMUContext *riscv_iommu_ctx(RISCVIOMMUState *s,
                                          unsigned devid, unsigned process_id,
                                          IOMMUAccessFlags perm, uint64_t iova,
                                          शून्य **ref)
{
    GHashTable *ctx_cache;
    RISCVIOMMUContext *ctx;
    RISCVIOMMUContext कुञ्चिका = {
        .devid = devid,
        .process_id = process_id,
    };
    unsigned mode = get_field(s->ddtp, RISCV_IOMMU_DDTP_MODE);
    uint32_t fault_type;

    ctx_cache = g_hash_table_ref(s->ctx_cache);

    सचे (mode != RISCV_IOMMU_DDTP_MODE_OFF &&
        mode != RISCV_IOMMU_DDTP_MODE_BARE) {
        ctx = g_hash_table_lookup(ctx_cache, &कुञ्चिका);

        सचे (ctx && (ctx->tc & RISCV_IOMMU_DC_TC_V)) {
            *ref = ctx_cache;
            पटिनिवत्त ctx;
        }
    }

    ctx = g_new0(RISCVIOMMUContext, 1);
    ctx->devid = devid;
    ctx->process_id = process_id;

    पूर्णाङ्क fault = riscv_iommu_ctx_fetch(s, ctx);
    सचे (!fault) {
        सचे (mode != RISCV_IOMMU_DDTP_MODE_BARE) {
            सचे (g_hash_table_size(ctx_cache) >= LIMIT_CACHE_CTX) {
                g_hash_table_unref(ctx_cache);
                ctx_cache = g_hash_table_new_full(riscv_iommu_ctx_hash,
                                                  riscv_iommu_ctx_equal,
                                                  g_free, रिक्त);
                g_hash_table_ref(ctx_cache);
                g_hash_table_unref(qatomic_xchg(&s->ctx_cache, ctx_cache));
            }

            g_hash_table_add(ctx_cache, ctx);
            *ref = ctx_cache;
        } अञ्ञथा {
            g_hash_table_unref(ctx_cache);
            /* Remember ctx so it can be freed */
            *ref = ctx;
        }
        पटिनिवत्त ctx;
    }

    g_hash_table_unref(ctx_cache);
    *ref = रिक्त;

    /*
     * TODO: (1) do we need to distinguish other fault types
     * for ctx fetching and (2) evaluate putting the 'fault_type'
     * logic inside riscv_iommu_report_fault() - there's at
     * least one other place (end of riscv_iommu_translate())
     * that does something similar.
     */
    सचे (perm & IOMMU_RO) {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
    } अञ्ञथा {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    }

    riscv_iommu_report_fault(s, ctx, fault_type, fault,
                             !!process_id, iova, 0);

    g_free(ctx);
    पटिनिवत्त रिक्त;
}

स्थिर शून्य riscv_iommu_ctx_put(RISCVIOMMUState *s, शून्य *ref)
{
    unsigned mode = get_field(s->ddtp, RISCV_IOMMU_DDTP_MODE);

    सचे (!ref) {
        पटिनिवत्त;
    }

    /* ref is pointing to ctx in Bare mode. Bare mode ctx is not cached */
    सचे (mode == RISCV_IOMMU_DDTP_MODE_BARE) {
        g_free(ref);
    } अञ्ञथा {
        g_hash_table_unref((GHashTable *)ref);
    }
}

/* Find or allocate address space for a given device */
स्थिर AddressSpace *riscv_iommu_space(RISCVIOMMUState *s, PCIBus *bus,
                                            पूर्णाङ्क devfn)
{
    RISCVIOMMUSpace *as;

    QLIST_FOREACH(as, &s->spaces, list) {
        सचे (as->bus == bus && as->devfn == devfn) {
            भङ्ग;
        }
    }

    सचे (as == रिक्त) {
        वर्ण name[64];
        as = g_new0(RISCVIOMMUSpace, 1);

        as->iommu = s;
        as->bus = bus;
        as->devfn = devfn;

        snprintf(name, परिमाण(name), "riscv-iommu-%04x:%02x.%d-iova",
            pci_bus_अङ्कः(bus), PCI_SLOT(devfn), PCI_FUNC(devfn));

        /* IOVA address space, untranslated addresses */
        memory_region_init_iommu(&as->iova_mr, परिमाण(as->iova_mr),
            TYPE_RISCV_IOMMU_MEMORY_REGION,
            OBJECT(s), "riscv_iommu", UINT64_MAX);
        address_space_init(&as->iova_as, MEMORY_REGION(&as->iova_mr), name);

        QLIST_निवेशय_HEAD(&s->spaces, as, list);

        trace_riscv_iommu_new(s->parent_obj.id, pci_bus_अङ्कः(bus),
                PCI_SLOT(devfn), PCI_FUNC(devfn));
    }
    पटिनिवत्त &as->iova_as;
}

/* Translation Object cache support */
स्थिर gboolean riscv_iommu_iot_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUEntry *t1 = (RISCVIOMMUEntry *) v1;
    RISCVIOMMUEntry *t2 = (RISCVIOMMUEntry *) v2;
    पटिनिवत्त t1->gscid == t2->gscid && t1->pscid == t2->pscid &&
           t1->iova == t2->iova && t1->tag == t2->tag;
}

स्थिर guint riscv_iommu_iot_hash(gconstpointer v)
{
    RISCVIOMMUEntry *t = (RISCVIOMMUEntry *) v;
    पटिनिवत्त (guint)t->iova;
}

/* GV: 0 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 0 AV: 0 GVMA: 1 */
स्थिर
शून्य riscv_iommu_iot_inval_all(gpointer कुञ्चिका, gpointer मानम्, gpointer data)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) data;
    सचे (iot->tag == arg->tag) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 0 PSCV: 1 GVMA: 0 */
स्थिर
शून्य riscv_iommu_iot_inval_pscid(gpointer कुञ्चिका, gpointer मानम्, gpointer data)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) data;
    सचे (iot->tag == arg->tag &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 0 GVMA: 0 */
स्थिर
शून्य riscv_iommu_iot_inval_iova(gpointer कुञ्चिका, gpointer मानम्, gpointer data)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) data;
    सचे (iot->tag == arg->tag &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 1 GVMA: 0 */
स्थिर शून्य riscv_iommu_iot_inval_pscid_iova(gpointer कुञ्चिका, gpointer मानम्,
                                             gpointer data)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) data;
    सचे (iot->tag == arg->tag &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 0 GVMA: 1 */
स्थिर
शून्य riscv_iommu_iot_inval_gscid(gpointer कुञ्चिका, gpointer मानम्, gpointer data)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) data;
    सचे (iot->tag == arg->tag &&
        iot->gscid == arg->gscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 1 GVMA: 0 */
स्थिर शून्य riscv_iommu_iot_inval_gscid_pscid(gpointer कुञ्चिका, gpointer मानम्,
                                              gpointer data)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) data;
    सचे (iot->tag == arg->tag &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 1 GVMA: 1 */
स्थिर शून्य riscv_iommu_iot_inval_gscid_iova(gpointer कुञ्चिका, gpointer मानम्,
                                             gpointer data)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) data;
    सचे (iot->tag == arg->tag &&
        iot->gscid == arg->gscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 1 GVMA: 0 */
स्थिर शून्य riscv_iommu_iot_inval_gscid_pscid_iova(gpointer कुञ्चिका, gpointer मानम्,
                                                   gpointer data)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मानम्;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) data;
    सचे (iot->tag == arg->tag &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* caller should keep ref-count for iot_cache object */
स्थिर RISCVIOMMUEntry *riscv_iommu_iot_lookup(RISCVIOMMUContext *ctx,
    GHashTable *iot_cache, hwaddr iova, RISCVIOMMUTransTag transtag)
{
    RISCVIOMMUEntry कुञ्चिका = {
        .tag   = transtag,
        .gscid = get_field(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID),
        .pscid = get_field(ctx->ta, RISCV_IOMMU_DC_TA_PSCID),
        .iova  = PPN_DOWN(iova),
    };
    पटिनिवत्त g_hash_table_lookup(iot_cache, &कुञ्चिका);
}

/* caller should keep ref-count for iot_cache object */
स्थिर शून्य riscv_iommu_iot_update(RISCVIOMMUState *s,
    GHashTable *iot_cache, RISCVIOMMUEntry *iot)
{
    सचे (!s->iot_limit) {
        पटिनिवत्त;
    }

    सचे (g_hash_table_size(s->iot_cache) >= s->iot_limit) {
        iot_cache = g_hash_table_new_full(riscv_iommu_iot_hash,
                                          riscv_iommu_iot_equal,
                                          g_free, रिक्त);
        g_hash_table_unref(qatomic_xchg(&s->iot_cache, iot_cache));
    }
    g_hash_table_add(iot_cache, iot);
}

स्थिर शून्य riscv_iommu_iot_inval(RISCVIOMMUState *s, GHFunc func,
    uint32_t gscid, uint32_t pscid, hwaddr iova, RISCVIOMMUTransTag transtag)
{
    GHashTable *iot_cache;
    RISCVIOMMUEntry कुञ्चिका = {
        .tag = transtag,
        .gscid = gscid,
        .pscid = pscid,
        .iova  = PPN_DOWN(iova),
    };

    iot_cache = g_hash_table_ref(s->iot_cache);
    g_hash_table_foreach(iot_cache, func, &कुञ्चिका);
    g_hash_table_unref(iot_cache);
}

स्थिर RISCVIOMMUTransTag riscv_iommu_get_transtag(RISCVIOMMUContext *ctx)
{
    uint64_t satp = get_field(ctx->satp, RISCV_IOMMU_ATP_MODE_FIELD);
    uint64_t gatp = get_field(ctx->gatp, RISCV_IOMMU_ATP_MODE_FIELD);

    सचे (satp == RISCV_IOMMU_DC_FSC_MODE_BARE) {
        पटिनिवत्त (gatp == RISCV_IOMMU_DC_IOHGATP_MODE_BARE) ?
            RISCV_IOMMU_TRANS_TAG_BY : RISCV_IOMMU_TRANS_TAG_VG;
    } अञ्ञथा {
        पटिनिवत्त (gatp == RISCV_IOMMU_DC_IOHGATP_MODE_BARE) ?
            RISCV_IOMMU_TRANS_TAG_SS : RISCV_IOMMU_TRANS_TAG_VN;
    }
}

स्थिर पूर्णाङ्क riscv_iommu_translate(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb, bool enable_cache)
{
    RISCVIOMMUTransTag transtag = riscv_iommu_get_transtag(ctx);
    RISCVIOMMUEntry *iot;
    IOMMUAccessFlags perm;
    bool enable_pid;
    bool enable_pri;
    GHashTable *iot_cache;
    पूर्णाङ्क fault;

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_URQ);

    iot_cache = g_hash_table_ref(s->iot_cache);
    enable_pri = (iotlb->perm == IOMMU_NONE) &&
                 (ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI);
    enable_pid = (ctx->tc & RISCV_IOMMU_DC_TC_PDTV);

    /* Check for ATS request. */
    सचे (iotlb->perm == IOMMU_NONE) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_ATS_RQ);
        /* Check if ATS is disabled. */
        सचे (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS)) {
            enable_pri = false;
            fault = RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
            गच्छ done;
        }
    }

    iot = riscv_iommu_iot_lookup(ctx, iot_cache, iotlb->iova, transtag);
    perm = iot ? iot->perm : IOMMU_NONE;
    सचे (perm != IOMMU_NONE) {
        iotlb->translated_addr = PPN_PHYS(iot->phys);
        iotlb->addr_mask = ~TARGET_PAGE_MASK;
        iotlb->perm = perm;
        fault = 0;
        गच्छ done;
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_TLB_MISS);

    /* Translate using device directory / page table information. */
    fault = riscv_iommu_spa_fetch(s, ctx, iotlb);

    सचे (!fault && iotlb->target_as == &s->trap_as) {
        /* Do not cache trapped MSI translations */
        गच्छ done;
    }

    /*
     * We made an implementation choice to not cache identity-mapped
     * translations, as allowed by the specification, to avoid
     * translation cache evictions for other devices sharing the
     * IOMMU hardware model.
     */
    सचे (!fault && iotlb->translated_addr != iotlb->iova && enable_cache) {
        iot = g_new0(RISCVIOMMUEntry, 1);
        iot->iova = PPN_DOWN(iotlb->iova);
        iot->phys = PPN_DOWN(iotlb->translated_addr);
        iot->gscid = get_field(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
        iot->pscid = get_field(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        iot->perm = iotlb->perm;
        iot->tag = transtag;
        riscv_iommu_iot_update(s, iot_cache, iot);
    }

done:
    g_hash_table_unref(iot_cache);

    सचे (enable_pri && fault) {
        संरचना riscv_iommu_pq_record pr = {0};
        सचे (enable_pid) {
            pr.hdr = set_field(RISCV_IOMMU_PREQ_HDR_PV,
                               RISCV_IOMMU_PREQ_HDR_PID, ctx->process_id);
        }
        pr.hdr = set_field(pr.hdr, RISCV_IOMMU_PREQ_HDR_DID, ctx->devid);
        pr.payload = (iotlb->iova & TARGET_PAGE_MASK) |
                     RISCV_IOMMU_PREQ_PAYLOAD_M;
        riscv_iommu_pri(s, &pr);
        पटिनिवत्त fault;
    }

    सचे (fault) {
        unsigned ttype = RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ;

        सचे ((iotlb->perm & IOMMU_RW) == IOMMU_RW
            || iotlb->perm & IOMMU_WO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
        } अञ्ञथा सचे (iotlb->perm & IOMMU_RO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
        }

        riscv_iommu_report_fault(s, ctx, ttype, fault, enable_pid,
                                 iotlb->iova, iotlb->translated_addr);
        पटिनिवत्त fault;
    }

    पटिनिवत्त 0;
}

/* IOMMU Command Interface */
स्थिर MemTxResult riscv_iommu_iofence(RISCVIOMMUState *s, bool notify,
    uint64_t addr, uint32_t data)
{
    /*
     * ATS processing in this implementation of the IOMMU is synchronous,
     * no need to wait for completions here.
     */
    सचे (!notify) {
        पटिनिवत्त MEMTX_OK;
    }

    पटिनिवत्त dma_memory_write(s->target_as, addr, &data, परिमाण(data),
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

    सचे (cmd->dword0 & RISCV_IOMMU_CMD_ATS_DSV) {
        /* Use device segment and requester id */
        devid = get_field(cmd->dword0,
            RISCV_IOMMU_CMD_ATS_DSEG | RISCV_IOMMU_CMD_ATS_RID);
    } अञ्ञथा {
        devid = get_field(cmd->dword0, RISCV_IOMMU_CMD_ATS_RID);
    }

    pid = get_field(cmd->dword0, RISCV_IOMMU_CMD_ATS_PID);

    QLIST_FOREACH(as, &s->spaces, list) {
        सचे (riscv_iommu_space_devid(as) == devid) {
            भङ्ग;
        }
    }

    सचे (!as || !as->notifier) {
        पटिनिवत्त;
    }

    event.type = flag;
    event.entry.perm = perm;
    event.entry.target_as = s->target_as;

    IOMMU_NOTIFIER_FOREACH(n, &as->iova_mr) {
        सचे (!pv || n->iommu_idx == pid) {
            event.entry.iova = n->start;
            event.entry.addr_mask = n->end - n->start;
            trace_fn(as->iova_mr.parent_obj.name);
            memory_region_notify_iommu_one(n, &event);
        }
    }
}

स्थिर शून्य riscv_iommu_ats_inval(RISCVIOMMUState *s,
    संरचना riscv_iommu_command *cmd)
{
    पटिनिवत्त riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_DEVIOTLB_UNMAP, IOMMU_NONE,
                           trace_riscv_iommu_ats_inval);
}

स्थिर शून्य riscv_iommu_ats_prgr(RISCVIOMMUState *s,
    संरचना riscv_iommu_command *cmd)
{
    unsigned resp_code = get_field(cmd->dword1,
                                   RISCV_IOMMU_CMD_ATS_PRGR_RESP_CODE);

    /* Using the access flag to carry response code information */
    IOMMUAccessFlags perm = resp_code ? IOMMU_NONE : IOMMU_RW;
    पटिनिवत्त riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_MAP, perm,
                           trace_riscv_iommu_ats_prgr);
}

स्थिर शून्य riscv_iommu_process_ddtp(RISCVIOMMUState *s)
{
    uint64_t old_ddtp = s->ddtp;
    uint64_t new_ddtp = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_DDTP);
    unsigned new_mode = get_field(new_ddtp, RISCV_IOMMU_DDTP_MODE);
    unsigned old_mode = get_field(old_ddtp, RISCV_IOMMU_DDTP_MODE);
    bool ok = false;

    /*
     * Check for allowed DDTP.MODE transitions:
     * {OFF, BARE}        -> {OFF, BARE, 1LVL, 2LVL, 3LVL}
     * {1LVL, 2LVL, 3LVL} -> {OFF, BARE}
     */
    सचे (new_mode == old_mode ||
        new_mode == RISCV_IOMMU_DDTP_MODE_OFF ||
        new_mode == RISCV_IOMMU_DDTP_MODE_BARE) {
        ok = true;
    } अञ्ञथा सचे (new_mode == RISCV_IOMMU_DDTP_MODE_1LVL ||
               new_mode == RISCV_IOMMU_DDTP_MODE_2LVL ||
               new_mode == RISCV_IOMMU_DDTP_MODE_3LVL) {
        ok = old_mode == RISCV_IOMMU_DDTP_MODE_OFF ||
             old_mode == RISCV_IOMMU_DDTP_MODE_BARE;
    }

    सचे (ok) {
        /* clear reserved and busy bits, report back sanitized version */
        new_ddtp = set_field(new_ddtp & RISCV_IOMMU_DDTP_PPN,
                             RISCV_IOMMU_DDTP_MODE, new_mode);
    } अञ्ञथा {
        new_ddtp = old_ddtp;
    }
    s->ddtp = new_ddtp;

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, new_ddtp);
}

/* Command function and opcode field. */
#define RISCV_IOMMU_CMD(func, op) (((func) << 7) | (op))

स्थिर शून्य riscv_iommu_process_cq_tail(RISCVIOMMUState *s)
{
    संरचना riscv_iommu_command cmd;
    MemTxResult res;
    dma_addr_t addr;
    uint32_t tail, head, ctrl;
    uint64_t cmd_opcode;
    GHFunc func;

    ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    tail = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQT) & s->cq_mask;
    head = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQH) & s->cq_mask;

    /* Check for pending error or queue processing disabled */
    सचे (!(ctrl & RISCV_IOMMU_CQCSR_CQON) ||
        !!(ctrl & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        पटिनिवत्त;
    }

    याव (tail != head) {
        addr = s->cq_addr  + head * परिमाण(cmd);
        res = dma_memory_read(s->target_as, addr, &cmd, परिमाण(cmd),
                              MEMTXATTRS_UNSPECIFIED);

        सचे (res != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                  RISCV_IOMMU_CQCSR_CQMF, 0);
            गच्छ fault;
        }

        trace_riscv_iommu_cmd(s->parent_obj.id, cmd.dword0, cmd.dword1);

        cmd_opcode = get_field(cmd.dword0,
                               RISCV_IOMMU_CMD_OPCODE | RISCV_IOMMU_CMD_FUNC);

        विकल्प (cmd_opcode) {
        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOFENCE_FUNC_C,
                             RISCV_IOMMU_CMD_IOFENCE_OPCODE):
            सचे (cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_RESERVED) {
                गच्छ cmd_ill;
            }

            res = riscv_iommu_iofence(s,
                cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_AV, cmd.dword1 << 2,
                get_field(cmd.dword0, RISCV_IOMMU_CMD_IOFENCE_DATA));

            सचे (res != MEMTX_OK) {
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
            uint32_t gscid = get_field(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_field(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & TARGET_PAGE_MASK;

            सचे (pscv) {
                /* illegal command arguments IOTINVAL.GVMA & PSCV == 1 */
                गच्छ cmd_ill;
            }

            func = riscv_iommu_iot_inval_all;

            सचे (gv) {
                func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                              riscv_iommu_iot_inval_gscid;
            }

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_TAG_VG);

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_TAG_VN);
            भङ्ग;
        }

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_field(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_field(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & TARGET_PAGE_MASK;
            RISCVIOMMUTransTag transtag;

            सचे (gv) {
                transtag = RISCV_IOMMU_TRANS_TAG_VN;
                सचे (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_gscid_pscid_iova :
                                  riscv_iommu_iot_inval_gscid_pscid;
                } अञ्ञथा {
                    func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                                  riscv_iommu_iot_inval_gscid;
                }
            } अञ्ञथा {
                transtag = RISCV_IOMMU_TRANS_TAG_SS;
                सचे (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_pscid_iova :
                                  riscv_iommu_iot_inval_pscid;
                } अञ्ञथा {
                    func = (av) ? riscv_iommu_iot_inval_iova :
                                  riscv_iommu_iot_inval_all;
                }
            }

            riscv_iommu_iot_inval(s, func, gscid, pscid, iova, transtag);
            भङ्ग;
        }

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            सचे (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* invalidate all device context cache mappings */
                func = riscv_iommu_ctx_inval_all;
            } अञ्ञथा {
                /* invalidate all device context matching DID */
                func = riscv_iommu_ctx_inval_devid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_field(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID), 0);
            भङ्ग;

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            सचे (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* illegal command arguments IODIR_PDT & DV == 0 */
                गच्छ cmd_ill;
            } अञ्ञथा {
                func = riscv_iommu_ctx_inval_devid_procid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_field(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID),
                get_field(cmd.dword0, RISCV_IOMMU_CMD_IODIR_PID));
            भङ्ग;

        /* ATS commands */
        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_INVAL,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            सचे (!s->enable_ats) {
                गच्छ cmd_ill;
            }

            riscv_iommu_ats_inval(s, &cmd);
            भङ्ग;

        पक्ष RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_PRGR,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            सचे (!s->enable_ats) {
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
        head = (head + 1) & s->cq_mask;
        riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_CQH, head);
    }
    पटिनिवत्त;

fault:
    सचे (ctrl & RISCV_IOMMU_CQCSR_CIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_CQ);
    }
}

स्थिर शून्य riscv_iommu_process_cq_control(RISCVIOMMUState *s)
{
    uint64_t base;
    uint32_t ctrl_set = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_set & RISCV_IOMMU_CQCSR_CQEN);
    bool active = !!(ctrl_set & RISCV_IOMMU_CQCSR_CQON);

    सचे (enable && !active) {
        base = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_CQB);
        s->cq_mask = (2ULL << get_field(base, RISCV_IOMMU_CQB_LOG2SZ)) - 1;
        s->cq_addr = PPN_PHYS(get_field(base, RISCV_IOMMU_CQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~s->cq_mask);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQT], 0);
        ctrl_set = RISCV_IOMMU_CQCSR_CQON;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQMF |
                   RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CMD_TO |
                   RISCV_IOMMU_CQCSR_FENCE_W_IP;
    } अञ्ञथा सचे (!enable && active) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~0);
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQON;
    } अञ्ञथा {
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, ctrl_set, ctrl_clr);

    /*
     * After clearing error bits (CMD_ILL, CQMF), if queue is still active,
     * re-process pending command.
     */
    ctrl_set = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    सचे ((ctrl_set & RISCV_IOMMU_CQCSR_CQON) &&
        !(ctrl_set & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        riscv_iommu_process_cq_tail(s);
    }
}

स्थिर शून्य riscv_iommu_process_fq_control(RISCVIOMMUState *s)
{
    uint64_t base;
    uint32_t ctrl_set = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_set & RISCV_IOMMU_FQCSR_FQEN);
    bool active = !!(ctrl_set & RISCV_IOMMU_FQCSR_FQON);

    सचे (enable && !active) {
        base = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_FQB);
        s->fq_mask = (2ULL << get_field(base, RISCV_IOMMU_FQB_LOG2SZ)) - 1;
        s->fq_addr = PPN_PHYS(get_field(base, RISCV_IOMMU_FQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~s->fq_mask);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQT], 0);
        ctrl_set = RISCV_IOMMU_FQCSR_FQON;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQMF |
            RISCV_IOMMU_FQCSR_FQOF;
    } अञ्ञथा सचे (!enable && active) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~0);
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQON;
    } अञ्ञथा {
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, ctrl_set, ctrl_clr);
}

स्थिर शून्य riscv_iommu_process_pq_control(RISCVIOMMUState *s)
{
    uint64_t base;
    uint32_t ctrl_set = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_set & RISCV_IOMMU_PQCSR_PQEN);
    bool active = !!(ctrl_set & RISCV_IOMMU_PQCSR_PQON);

    सचे (enable && !active) {
        base = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_PQB);
        s->pq_mask = (2ULL << get_field(base, RISCV_IOMMU_PQB_LOG2SZ)) - 1;
        s->pq_addr = PPN_PHYS(get_field(base, RISCV_IOMMU_PQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~s->pq_mask);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQT], 0);
        ctrl_set = RISCV_IOMMU_PQCSR_PQON;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQMF |
            RISCV_IOMMU_PQCSR_PQOF;
    } अञ्ञथा सचे (!enable && active) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~0);
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQON;
    } अञ्ञथा {
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, ctrl_set, ctrl_clr);
}

स्थिर शून्य riscv_iommu_process_dbg(RISCVIOMMUState *s)
{
    uint64_t iova = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_IOVA);
    uint64_t ctrl = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_CTL);
    unsigned devid = get_field(ctrl, RISCV_IOMMU_TR_REQ_CTL_DID);
    unsigned pid = get_field(ctrl, RISCV_IOMMU_TR_REQ_CTL_PID);
    IOMMUAccessFlags perm = ctrl & RISCV_IOMMU_TR_REQ_CTL_NW
                            ? IOMMU_RO : IOMMU_RW;
    RISCVIOMMUContext *ctx;
    शून्य *ref;

    सचे (!(ctrl & RISCV_IOMMU_TR_REQ_CTL_GO_BUSY)) {
        पटिनिवत्त;
    }

    ctx = riscv_iommu_ctx(s, devid, pid, perm, iova, &ref);
    सचे (ctx == रिक्त) {
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE,
                                 RISCV_IOMMU_TR_RESPONSE_FAULT |
                                 (RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED << 10));
    } अञ्ञथा {
        IOMMUTLBEntry iotlb = {
            .iova = iova,
            .perm = perm,
            .addr_mask = ~0,
            .target_as = रिक्त,
        };
        पूर्णाङ्क fault = riscv_iommu_translate(s, ctx, &iotlb, false);
        सचे (fault) {
            iova = RISCV_IOMMU_TR_RESPONSE_FAULT | (((uint64_t) fault) << 10);
        } अञ्ञथा {
            iova = iotlb.translated_addr & ~iotlb.addr_mask;
            iova = set_field(0, RISCV_IOMMU_TR_RESPONSE_PPN, PPN_DOWN(iova));
        }
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE, iova);
    }

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
        RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    riscv_iommu_ctx_put(s, ref);
}

प्रकारनिर्देश शून्य riscv_iommu_process_fn(RISCVIOMMUState *s);

स्थिर शून्य riscv_iommu_update_icvec(RISCVIOMMUState *s, uint64_t data)
{
    uint64_t icvec = 0;

    icvec |= MIN(data & RISCV_IOMMU_ICVEC_CIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_CIV);

    icvec |= MIN(data & RISCV_IOMMU_ICVEC_FIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_FIV);

    icvec |= MIN(data & RISCV_IOMMU_ICVEC_PMIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PMIV);

    icvec |= MIN(data & RISCV_IOMMU_ICVEC_PIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PIV);

    trace_riscv_iommu_icvec_write(data, icvec);

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_ICVEC, icvec);
}

स्थिर शून्य riscv_iommu_update_ipsr(RISCVIOMMUState *s, uint64_t data)
{
    uint32_t cqcsr, fqcsr, pqcsr;
    uint32_t ipsr_set = 0;
    uint32_t ipsr_clr = 0;

    सचे (data & RISCV_IOMMU_IPSR_CIP) {
        cqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);

        सचे (cqcsr & RISCV_IOMMU_CQCSR_CIE &&
            (cqcsr & RISCV_IOMMU_CQCSR_FENCE_W_IP ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ILL ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_TO ||
             cqcsr & RISCV_IOMMU_CQCSR_CQMF)) {
            ipsr_set |= RISCV_IOMMU_IPSR_CIP;
        } अञ्ञथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
        }
    } अञ्ञथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
    }

    सचे (data & RISCV_IOMMU_IPSR_FIP) {
        fqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);

        सचे (fqcsr & RISCV_IOMMU_FQCSR_FIE &&
            (fqcsr & RISCV_IOMMU_FQCSR_FQOF ||
             fqcsr & RISCV_IOMMU_FQCSR_FQMF)) {
            ipsr_set |= RISCV_IOMMU_IPSR_FIP;
        } अञ्ञथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
        }
    } अञ्ञथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
    }

    सचे (!(data & RISCV_IOMMU_IPSR_PMIP)) {
        ipsr_clr |= RISCV_IOMMU_IPSR_PMIP;
    }

    सचे (data & RISCV_IOMMU_IPSR_PIP) {
        pqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);

        सचे (pqcsr & RISCV_IOMMU_PQCSR_PIE &&
            (pqcsr & RISCV_IOMMU_PQCSR_PQOF ||
             pqcsr & RISCV_IOMMU_PQCSR_PQMF)) {
            ipsr_set |= RISCV_IOMMU_IPSR_PIP;
        } अञ्ञथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
        }
    } अञ्ञथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, ipsr_set, ipsr_clr);
}

स्थिर शून्य riscv_iommu_process_hpm_writes(RISCVIOMMUState *s,
                                           uint32_t regb,
                                           bool prev_cy_inh)
{
    विकल्प (regb) {
    पक्ष RISCV_IOMMU_REG_IOCOUNTINH:
        riscv_iommu_process_iocntinh_cy(s, prev_cy_inh);
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_IOHPMCYCLES:
    पक्ष RISCV_IOMMU_REG_IOHPMCYCLES + 4:
        riscv_iommu_process_hpmcycle_write(s);
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_IOHPMEVT_BASE ...
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
                                      पूर्णाङ्क size, uint64_t data)
{
    uint64_t ro = ldn_le_p(&s->regs_ro[reg_addr], size);
    uint64_t wc = ldn_le_p(&s->regs_wc[reg_addr], size);
    uint64_t curr_val = ldn_le_p(&s->regs[reg_addr], size);

    stn_le_p(dest, size, ((curr_val & ro) | (data & ~ro)) & ~(data & wc));
}

स्थिर MemTxResult riscv_iommu_mmio_write(शून्य *opaque, hwaddr addr,
                                          uint64_t data, unsigned size,
                                          MemTxAttrs attrs)
{
    riscv_iommu_process_fn *process_fn = रिक्त;
    RISCVIOMMUState *s = opaque;
    uint32_t regb = addr & ~3;
    uint32_t busy = 0;
    uint64_t val = 0;
    bool cy_inh = false;

    सचे ((addr & (size - 1)) != 0) {
        /* Unsupported MMIO alignment or access size */
        पटिनिवत्त MEMTX_ERROR;
    }

    सचे (addr + size > RISCV_IOMMU_REG_MSI_CONFIG) {
        /* Unsupported MMIO access location. */
        पटिनिवत्त MEMTX_ACCESS_ERROR;
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
        process_fn = riscv_iommu_process_cq_tail;
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
        riscv_iommu_write_reg_val(s, &val, addr, size, data);

        /*
         * 'val' is stored as LE. Switch to host endianess
         * before using it.
         */
        val = le64_to_cpu(val);

        सचे (regb == RISCV_IOMMU_REG_ICVEC) {
            riscv_iommu_update_icvec(s, val);
        } अञ्ञथा {
            riscv_iommu_update_ipsr(s, val);
        }

        पटिनिवत्त MEMTX_OK;

    पक्ष RISCV_IOMMU_REG_TR_REQ_CTL:
        process_fn = riscv_iommu_process_dbg;
        regb = RISCV_IOMMU_REG_TR_REQ_CTL;
        busy = RISCV_IOMMU_TR_REQ_CTL_GO_BUSY;
        भङ्ग;

    पक्ष RISCV_IOMMU_REG_IOCOUNTINH:
        सचे (addr != RISCV_IOMMU_REG_IOCOUNTINH) {
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
    riscv_iommu_write_reg_val(s, &s->regs[addr], addr, size, data);

    /* Busy flag update, MSB 4-byte register. */
    सचे (busy) {
        uint32_t rw = ldl_le_p(&s->regs[regb]);
        stl_le_p(&s->regs[regb], rw | busy);
    }

    /* Process HPM writes and update any internal state if needed. */
    सचे (regb >= RISCV_IOMMU_REG_IOCOUNTOVF &&
        regb <= (RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_अङ्कः) + 4)) {
        riscv_iommu_process_hpm_writes(s, regb, cy_inh);
    }

    सचे (process_fn) {
        process_fn(s);
    }

    पटिनिवत्त MEMTX_OK;
}

स्थिर MemTxResult riscv_iommu_mmio_read(शून्य *opaque, hwaddr addr,
    uint64_t *data, unsigned size, MemTxAttrs attrs)
{
    RISCVIOMMUState *s = opaque;
    uint64_t val = -1;
    uint8_t *ptr;

    सचे ((addr & (size - 1)) != 0) {
        /* Unsupported MMIO alignment. */
        पटिनिवत्त MEMTX_ERROR;
    }

    सचे (addr + size > RISCV_IOMMU_REG_MSI_CONFIG) {
        पटिनिवत्त MEMTX_ACCESS_ERROR;
    }

    /* Compute cycle register value. */
    सचे ((addr & ~7) == RISCV_IOMMU_REG_IOHPMCYCLES) {
        val = riscv_iommu_hpmcycle_read(s);
        ptr = (uint8_t *)&val + (addr & 7);
    } अञ्ञथा सचे ((addr & ~3) == RISCV_IOMMU_REG_IOCOUNTOVF) {
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
    } अञ्ञथा {
        ptr = &s->regs[addr];
    }

    val = ldn_le_p(ptr, size);

    *data = val;

    पटिनिवत्त MEMTX_OK;
}

स्थिर ध्रुव MemoryRegionOps riscv_iommu_mmio_ops = {
    .read_with_attrs = riscv_iommu_mmio_read,
    .write_with_attrs = riscv_iommu_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 4,
        .max_access_size = 8,
        .unaligned = false,
    },
    .valid = {
        .min_access_size = 4,
        .max_access_size = 8,
    }
};

/*
 * Translations matching MSI pattern check are redirected to "riscv-iommu-trap"
 * memory region as untranslated address, for additional MSI/MRIF interception
 * by IOMMU interrupt remapping implementation.
 * Note: Device emulation code generating an MSI is expected to provide a valid
 * memory transaction attributes with requested_id set.
 */
स्थिर MemTxResult riscv_iommu_trap_write(शून्य *opaque, hwaddr addr,
    uint64_t data, unsigned size, MemTxAttrs attrs)
{
    RISCVIOMMUState* s = (RISCVIOMMUState *)opaque;
    RISCVIOMMUContext *ctx;
    MemTxResult res;
    शून्य *ref;
    uint32_t devid = attrs.requester_id;

    सचे (attrs.unspecified) {
        पटिनिवत्त MEMTX_ACCESS_ERROR;
    }

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= s->bus << 8;

    ctx = riscv_iommu_ctx(s, devid, 0, IOMMU_RW, addr, &ref);
    सचे (ctx == रिक्त) {
        res = MEMTX_ACCESS_ERROR;
    } अञ्ञथा {
        res = riscv_iommu_msi_write(s, ctx, addr, data, size, attrs);
    }
    riscv_iommu_ctx_put(s, ref);
    पटिनिवत्त res;
}

स्थिर MemTxResult riscv_iommu_trap_read(शून्य *opaque, hwaddr addr,
    uint64_t *data, unsigned size, MemTxAttrs attrs)
{
    पटिनिवत्त MEMTX_ACCESS_ERROR;
}

स्थिर ध्रुव MemoryRegionOps riscv_iommu_trap_ops = {
    .read_with_attrs = riscv_iommu_trap_read,
    .write_with_attrs = riscv_iommu_trap_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 4,
        .max_access_size = 8,
    },
    .valid = {
        .min_access_size = 4,
        .max_access_size = 8,
    }
};

शून्य riscv_iommu_set_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_mode mode)
{
    s->cap = set_field(s->cap, RISCV_IOMMU_CAP_IGS, mode);
}

स्थिर शून्य riscv_iommu_instance_init(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    /* Enable translation debug interface */
    s->cap = RISCV_IOMMU_CAP_DBG;

    /* TODO: method to report supported PID bits */
    s->pid_bits = 8; /* restricted to size of MemTxAttrs.pid */
    s->cap |= RISCV_IOMMU_CAP_PD8;

    /* register storage */
    s->regs = g_new0(uint8_t, RISCV_IOMMU_REG_SIZE);
    s->regs_ro = g_new0(uint8_t, RISCV_IOMMU_REG_SIZE);
    s->regs_wc = g_new0(uint8_t, RISCV_IOMMU_REG_SIZE);

     /* Mark all registers read-only */
    memset(s->regs_ro, 0xff, RISCV_IOMMU_REG_SIZE);

    /* Device translation context cache */
    s->ctx_cache = g_hash_table_new_full(riscv_iommu_ctx_hash,
                                         riscv_iommu_ctx_equal,
                                         g_free, रिक्त);

    s->iot_cache = g_hash_table_new_full(riscv_iommu_iot_hash,
                                         riscv_iommu_iot_equal,
                                         g_free, रिक्त);

    s->iommus.le_next = रिक्त;
    s->iommus.le_prev = रिक्त;
    QLIST_INIT(&s->spaces);
}

स्थिर शून्य riscv_iommu_instance_finalize(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    g_free(s->regs);
    g_free(s->regs_ro);
    g_free(s->regs_wc);

    g_hash_table_unref(s->ctx_cache);
    g_hash_table_unref(s->iot_cache);
}

स्थिर शून्य riscv_iommu_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    /* Report QEMU target physical address space limits. */
    s->cap = set_field(s->cap, RISCV_IOMMU_CAP_PAS, s->pas_bits);

    s->cap |= s->version & RISCV_IOMMU_CAP_VERSION;
    सचे (s->enable_msi) {
        s->cap |= RISCV_IOMMU_CAP_MSI_FLAT | RISCV_IOMMU_CAP_MSI_MRIF;
    }
    सचे (s->enable_ats) {
        s->cap |= RISCV_IOMMU_CAP_ATS;
    }
    सचे (s->enable_s_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32 | RISCV_IOMMU_CAP_SV39 |
                  RISCV_IOMMU_CAP_SV48 | RISCV_IOMMU_CAP_SV57;
    }
    सचे (s->enable_g_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32X4 | RISCV_IOMMU_CAP_SV39X4 |
                  RISCV_IOMMU_CAP_SV48X4 | RISCV_IOMMU_CAP_SV57X4 |
                  RISCV_IOMMU_CAP_SVRSW60T59B;
    }

    सचे (s->hpm_cntrs > 0) {
        /* Clip number of HPM counters to maximum supported (31). */
        सचे (s->hpm_cntrs > RISCV_IOMMU_IOCOUNT_अङ्कः) {
            s->hpm_cntrs = RISCV_IOMMU_IOCOUNT_अङ्कः;
        }
        /* Enable hardware performance monitor interface */
        s->cap |= RISCV_IOMMU_CAP_HPM;
    }

    /* Out-of-reset translation mode: OFF (DMA disabled) BARE (passthrough) */
    s->ddtp = set_field(0, RISCV_IOMMU_DDTP_MODE, s->enable_off ?
                        RISCV_IOMMU_DDTP_MODE_OFF : RISCV_IOMMU_DDTP_MODE_BARE);

    /*
     * Register complete MMIO space, including MSI/PBA registers.
     * Note, PCIDevice implementation will add overlapping MR for MSI/PBA,
     * managed directly by the PCIDevice implementation.
     */
    memory_region_init_io(&s->regs_mr, OBJECT(dev), &riscv_iommu_mmio_ops, s,
        "riscv-iommu-regs", RISCV_IOMMU_REG_SIZE);

    /* Set power-on register state */
    stq_le_p(&s->regs[RISCV_IOMMU_REG_CAP], s->cap);

    stq_le_p(&s->regs[RISCV_IOMMU_REG_FCTL], 0);
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FCTL],
             ~(RISCV_IOMMU_FCTL_GXL | RISCV_IOMMU_FCTL_WSI));

    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_DDTP],
        ~(RISCV_IOMMU_DDTP_PPN | RISCV_IOMMU_DDTP_MODE));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQB],
        ~(RISCV_IOMMU_CQB_LOG2SZ | RISCV_IOMMU_CQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQB],
        ~(RISCV_IOMMU_FQB_LOG2SZ | RISCV_IOMMU_FQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQB],
        ~(RISCV_IOMMU_PQB_LOG2SZ | RISCV_IOMMU_PQB_PPN));
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_CQCSR], RISCV_IOMMU_CQCSR_CQMF |
        RISCV_IOMMU_CQCSR_CMD_TO | RISCV_IOMMU_CQCSR_CMD_ILL);
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
    सचे (s->cap & RISCV_IOMMU_CAP_DBG) {
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_IOVA], 0);
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_CTL],
            RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    }

    /* If HPM registers are enabled. */
    सचे (s->cap & RISCV_IOMMU_CAP_HPM) {
        /* +1 for cycle counter bit. */
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOCOUNTINH],
                 ~((2 << s->hpm_cntrs) - 1));
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCYCLES], 0);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCTR_BASE],
               0x00, s->hpm_cntrs * 8);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMEVT_BASE],
               0x00, s->hpm_cntrs * 8);
    }

    /* Memory region for downstream access, if specified. */
    सचे (s->target_mr) {
        s->target_as = g_new0(AddressSpace, 1);
        address_space_init(s->target_as, s->target_mr,
            "riscv-iommu-downstream");
    } अञ्ञथा {
        /* Fallback to global system memory. */
        s->target_as = &address_space_memory;
    }

    /* Memory region for untranslated MRIF/MSI writes */
    memory_region_init_io(&s->trap_mr, OBJECT(dev), &riscv_iommu_trap_ops, s,
            "riscv-iommu-trap", ~0ULL);
    address_space_init(&s->trap_as, &s->trap_mr, "riscv-iommu-trap-as");

    सचे (s->cap & RISCV_IOMMU_CAP_HPM) {
        s->hpm_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL, riscv_iommu_hpm_timer_cb, s);
        s->hpm_event_ctr_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
}

स्थिर शून्य riscv_iommu_unrealize(DeviceState *dev)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    सचे (s->cap & RISCV_IOMMU_CAP_HPM) {
        g_hash_table_unref(s->hpm_event_ctr_map);
        timer_free(s->hpm_timer);
    }
}

शून्य riscv_iommu_reset(RISCVIOMMUState *s)
{
    uint32_t reg_clr;
    पूर्णाङ्क ddtp_mode;

    /*
     * Clear DDTP while setting DDTP_mode back to user
     * initial setting.
     */
    ddtp_mode = s->enable_off ?
                RISCV_IOMMU_DDTP_MODE_OFF : RISCV_IOMMU_DDTP_MODE_BARE;
    s->ddtp = set_field(0, RISCV_IOMMU_DDTP_MODE, ddtp_mode);
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

    g_hash_table_remove_all(s->ctx_cache);
    g_hash_table_remove_all(s->iot_cache);
}

स्थिर ध्रुव Property riscv_iommu_properties[] = {
    DEFINE_PROP_UINT32("version", RISCVIOMMUState, version,
        RISCV_IOMMU_SPEC_DOT_VER),
    DEFINE_PROP_UINT32("pas-bits", RISCVIOMMUState, pas_bits, 0),
    DEFINE_PROP_UINT32("bus", RISCVIOMMUState, bus, 0x0),
    DEFINE_PROP_UINT32("ioatc-limit", RISCVIOMMUState, iot_limit,
        LIMIT_CACHE_IOT),
    DEFINE_PROP_BOOL("intremap", RISCVIOMMUState, enable_msi, TRUE),
    DEFINE_PROP_BOOL("ats", RISCVIOMMUState, enable_ats, TRUE),
    DEFINE_PROP_BOOL("off", RISCVIOMMUState, enable_off, TRUE),
    DEFINE_PROP_BOOL("s-stage", RISCVIOMMUState, enable_s_stage, TRUE),
    DEFINE_PROP_BOOL("g-stage", RISCVIOMMUState, enable_g_stage, TRUE),
    DEFINE_PROP_LINK("downstream-mr", RISCVIOMMUState, target_mr,
        TYPE_MEMORY_REGION, MemoryRegion *),
    DEFINE_PROP_UINT8("hpm-counters", RISCVIOMMUState, hpm_cntrs,
                      RISCV_IOMMU_IOCOUNT_अङ्कः),
};

स्थिर शून्य riscv_iommu_class_init(ObjectClass *klass, ध्रुव शून्य *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    /* internal device for riscv-iommu-{pci/sys}, not user-creatable */
    dc->user_creatable = false;
    dc->realize = riscv_iommu_realize;
    dc->unrealize = riscv_iommu_unrealize;
    device_class_set_props(dc, riscv_iommu_properties);
}

स्थिर ध्रुव TypeInfo riscv_iommu_info = {
    .name = TYPE_RISCV_IOMMU,
    .parent = TYPE_DEVICE,
    .instance_size = परिमाण(RISCVIOMMUState),
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
स्थिर IOMMUTLBEntry riscv_iommu_memory_region_translate(
    IOMMUMemoryRegion *iommu_mr, hwaddr addr,
    IOMMUAccessFlags flag, पूर्णाङ्क iommu_idx)
{
    RISCVIOMMUSpace *as = container_of(iommu_mr, RISCVIOMMUSpace, iova_mr);
    RISCVIOMMUContext *ctx;
    शून्य *ref;
    IOMMUTLBEntry iotlb = {
        .iova = addr,
        .target_as = as->iommu->target_as,
        .addr_mask = ~0ULL,
        .perm = flag,
    };
    uint32_t devid = riscv_iommu_space_devid(as);

    ctx = riscv_iommu_ctx(as->iommu, devid, iommu_idx, flag, addr, &ref);
    सचे (ctx == रिक्त) {
        /* Translation disabled or invalid. */
        iotlb.addr_mask = 0;
        iotlb.perm = IOMMU_NONE;
    } अञ्ञथा सचे (riscv_iommu_translate(as->iommu, ctx, &iotlb, true)) {
        /* Translation disabled or fault reported. */
        iotlb.addr_mask = 0;
        iotlb.perm = IOMMU_NONE;
    }

    /* Trace all dma translations with original access flags. */
    trace_riscv_iommu_dma(as->iommu->parent_obj.id, PCI_BUS_अङ्कः(devid),
                          PCI_SLOT(devid), PCI_FUNC(devid), iommu_idx,
                          IOMMU_FLAG_STR[flag & IOMMU_RW], iotlb.iova,
                          iotlb.translated_addr);

    riscv_iommu_ctx_put(as->iommu, ref);

    पटिनिवत्त iotlb;
}

स्थिर पूर्णाङ्क riscv_iommu_memory_region_notify(
    IOMMUMemoryRegion *iommu_mr, IOMMUNotifierFlag old,
    IOMMUNotifierFlag new, Error **errp)
{
    RISCVIOMMUSpace *as = container_of(iommu_mr, RISCVIOMMUSpace, iova_mr);

    सचे (old == IOMMU_NOTIFIER_NONE) {
        as->notifier = true;
        trace_riscv_iommu_notifier_add(iommu_mr->parent_obj.name);
    } अञ्ञथा सचे (new == IOMMU_NOTIFIER_NONE) {
        as->notifier = false;
        trace_riscv_iommu_notifier_del(iommu_mr->parent_obj.name);
    }

    पटिनिवत्त 0;
}

स्थिर अन्तःस्थ bool pci_is_iommu(PCIDevice *pdev)
{
    पटिनिवत्त pci_get_word(pdev->config + PCI_CLASS_DEVICE) == 0x0806;
}

स्थिर AddressSpace *riscv_iommu_find_as(PCIBus *bus, शून्य *opaque, पूर्णाङ्क devfn)
{
    RISCVIOMMUState *s = (RISCVIOMMUState *) opaque;
    PCIDevice *pdev = pci_find_device(bus, pci_bus_अङ्कः(bus), devfn);
    AddressSpace *as = रिक्त;

    सचे (pdev && pci_is_iommu(pdev)) {
        पटिनिवत्त s->target_as;
    }

    /* Find first registered IOMMU device */
    याव (s->iommus.le_prev) {
        s = *(s->iommus.le_prev);
    }

    /* Find first matching IOMMU */
    याव (s != रिक्त && as == रिक्त) {
        as = riscv_iommu_space(s, bus, devfn);
        s = s->iommus.le_next;
    }

    पटिनिवत्त as ? as : &address_space_memory;
}

स्थिर ध्रुव PCIIOMMUOps riscv_iommu_ops = {
    .get_address_space = riscv_iommu_find_as,
};

शून्य riscv_iommu_pci_setup_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
        Error **errp)
{
    सचे (bus->iommu_ops &&
        bus->iommu_ops->get_address_space == riscv_iommu_find_as) {
        /* Allow multiple IOMMUs on the same PCIe bus, link known devices */
        RISCVIOMMUState *last = (RISCVIOMMUState *)bus->iommu_opaque;
        QLIST_निवेशय_AFTER(last, iommu, iommus);
    } अञ्ञथा सचे (!bus->iommu_ops && !bus->iommu_opaque) {
        pci_setup_iommu(bus, &riscv_iommu_ops, iommu);
    } अञ्ञथा {
        error_setg(errp, "can't register secondary IOMMU for PCI bus #%d",
            pci_bus_अङ्कः(bus));
    }
}

स्थिर पूर्णाङ्क riscv_iommu_memory_region_index(IOMMUMemoryRegion *iommu_mr,
    MemTxAttrs attrs)
{
    पटिनिवत्त attrs.unspecified ? RISCV_IOMMU_NOPROCID : (पूर्णाङ्क)attrs.pid;
}

स्थिर पूर्णाङ्क riscv_iommu_memory_region_index_len(IOMMUMemoryRegion *iommu_mr)
{
    RISCVIOMMUSpace *as = container_of(iommu_mr, RISCVIOMMUSpace, iova_mr);
    पटिनिवत्त 1 << as->iommu->pid_bits;
}

स्थिर शून्य riscv_iommu_memory_region_init(ObjectClass *klass, ध्रुव शून्य *data)
{
    IOMMUMemoryRegionClass *imrc = IOMMU_MEMORY_REGION_CLASS(klass);

    imrc->translate = riscv_iommu_memory_region_translate;
    imrc->notify_flag_changed = riscv_iommu_memory_region_notify;
    imrc->attrs_to_index = riscv_iommu_memory_region_index;
    imrc->अङ्कः_indexes = riscv_iommu_memory_region_index_len;
}

स्थिर ध्रुव TypeInfo riscv_iommu_memory_region_info = {
    .parent = TYPE_IOMMU_MEMORY_REGION,
    .name = TYPE_RISCV_IOMMU_MEMORY_REGION,
    .class_init = riscv_iommu_memory_region_init,
};

स्थिर शून्य riscv_iommu_register_mr_types(शून्य)
{
    type_register_static(&riscv_iommu_memory_region_info);
    type_register_static(&riscv_iommu_info);
}

type_init(riscv_iommu_register_mr_types);
