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

#समावेश "qemu/osdep.h"
#समावेश "qom/object.h"
#समावेश "exec/target_page.h"
#समावेश "hw/pci/pci_bus.h"
#समावेश "hw/pci/pci_device.h"
#समावेश "hw/core/qdev-properties.h"
#समावेश "hw/riscv/riscv_hart.h"
#समावेश "migration/vmstate.h"
#समावेश "qapi/error.h"
#समावेश "qemu/timer.h"
#समावेश "qemu/target-info.h"
#समावेश "qemu/bitops.h"

#समावेश "cpu_bits.h"
#समावेश "riscv-iommu.h"
#समावेश "riscv-iommu-bits.h"
#समावेश "riscv-iommu-hpm.h"
#समावेश "trace.h"

#घोषणा LIMIT_CACHE_CTX               (1U << 7)
#घोषणा LIMIT_CACHE_IOT               (1U << 20)

/* Physical page number coversions */
#घोषणा PPN_PHYS(ppn)                 ((ppn) << TARGET_PAGE_BITS)
#घोषणा PPN_DOWN(phy)                 ((phy) >> TARGET_PAGE_BITS)

प्रकार काष्ठा RISCVIOMMUEntry RISCVIOMMUEntry;

/* Device assigned I/O address space */
काष्ठा RISCVIOMMUSpace {
    IOMMUMemoryRegion iova_mr;  /* IOVA memory region for attached device */
    AddressSpace iova_as;       /* IOVA address space for attached device */
    RISCVIOMMUState *iommu;     /* Managing IOMMU device state */
    PCIBus *bus;                /* PCI bus of the requester */
    uint8_t devfn;              /* Requester identifier, AKA device_id */
    bool notifier;              /* IOMMU unmap notifier enabled */
    QLIST_ENTRY(RISCVIOMMUSpace) list;
};

प्रकार क्रमागत RISCVIOMMUTransTag {
    RISCV_IOMMU_TRANS_TAG_BY,  /* Bypass */
    RISCV_IOMMU_TRANS_TAG_SS,  /* Single Stage */
    RISCV_IOMMU_TRANS_TAG_VG,  /* G-stage only */
    RISCV_IOMMU_TRANS_TAG_VN,  /* Nested translation */
} RISCVIOMMUTransTag;

/* Address translation cache entry */
काष्ठा RISCVIOMMUEntry {
    RISCVIOMMUTransTag tag;     /* Translation Tag */
    uint64_t iova:44;           /* IOVA Page Number */
    uint64_t pscid:20;          /* Process Soft-Context identifier */
    uint64_t phys:44;           /* Physical Page Number */
    uint64_t gscid:16;          /* Guest Soft-Context identifier */
    uint64_t perm:2;            /* IOMMU_RW flags */
};

/* IOMMU index for transactions without process_id specified. */
#घोषणा RISCV_IOMMU_NOPROCID 0

अटल uint32_t riscv_iommu_space_devid(RISCVIOMMUSpace *as)
{
    uint32_t devid = PCI_BUILD_BDF(pci_bus_अंक(as->bus), as->devfn);

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= as->iommu->bus << 8;
    वापस devid;
}

अटल uint8_t riscv_iommu_get_icvec_vector(uint32_t icvec, uint32_t vec_type)
{
    चयन (vec_type) {
    हाल RISCV_IOMMU_INTR_CQ:
        वापस icvec & RISCV_IOMMU_ICVEC_CIV;
    हाल RISCV_IOMMU_INTR_FQ:
        वापस (icvec & RISCV_IOMMU_ICVEC_FIV) >> 4;
    हाल RISCV_IOMMU_INTR_PM:
        वापस (icvec & RISCV_IOMMU_ICVEC_PMIV) >> 8;
    हाल RISCV_IOMMU_INTR_PQ:
        वापस (icvec & RISCV_IOMMU_ICVEC_PIV) >> 12;
    शेष:
        g_assert_not_reached();
    }
}

व्योम riscv_iommu_notify(RISCVIOMMUState *s, पूर्णांक vec_type)
{
    uint32_t ipsr, icvec, vector;

    अगर (!s->notify) {
        वापस;
    }

    icvec = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_ICVEC);
    ipsr = riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, (1 << vec_type), 0);

    अगर (!(ipsr & (1 << vec_type))) {
        vector = riscv_iommu_get_icvec_vector(icvec, vec_type);
        s->notify(s, vector);
        trace_riscv_iommu_notify_int_vector(vec_type, vector);
    }
}

व्योम riscv_iommu_fault(RISCVIOMMUState *s, काष्ठा riscv_iommu_fq_record *ev)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t शीर्ष = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQH) & s->fq_mask;
    uint32_t पुच्छ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQT) & s->fq_mask;
    uint32_t अग्र = (पुच्छ + 1) & s->fq_mask;
    uint32_t devid = get_field(ev->hdr, RISCV_IOMMU_FQ_HDR_DID);

    trace_riscv_iommu_flt(s->parent_obj.id, PCI_BUS_अंक(devid), PCI_SLOT(devid),
                          PCI_FUNC(devid), ev->hdr, ev->iotval);

    अगर (!(ctrl & RISCV_IOMMU_FQCSR_FQON) ||
        !!(ctrl & (RISCV_IOMMU_FQCSR_FQOF | RISCV_IOMMU_FQCSR_FQMF))) {
        वापस;
    }

    अगर (शीर्ष == अग्र) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                              RISCV_IOMMU_FQCSR_FQOF, 0);
    } अन्यथा {
        dma_addr_t addr = s->fq_addr + पुच्छ * माप(*ev);
        अगर (dma_memory_write(s->target_as, addr, ev, माप(*ev),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                                  RISCV_IOMMU_FQCSR_FQMF, 0);
        } अन्यथा {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_FQT, अग्र);
        }
    }

    अगर (ctrl & RISCV_IOMMU_FQCSR_FIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_FQ);
    }
}

अटल व्योम riscv_iommu_pri(RISCVIOMMUState *s,
    काष्ठा riscv_iommu_pq_record *pr)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t शीर्ष = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQH) & s->pq_mask;
    uint32_t पुच्छ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQT) & s->pq_mask;
    uint32_t अग्र = (पुच्छ + 1) & s->pq_mask;
    uint32_t devid = get_field(pr->hdr, RISCV_IOMMU_PREQ_HDR_DID);

    trace_riscv_iommu_pri(s->parent_obj.id, PCI_BUS_अंक(devid), PCI_SLOT(devid),
                          PCI_FUNC(devid), pr->payload);

    अगर (!(ctrl & RISCV_IOMMU_PQCSR_PQON) ||
        !!(ctrl & (RISCV_IOMMU_PQCSR_PQOF | RISCV_IOMMU_PQCSR_PQMF))) {
        वापस;
    }

    अगर (शीर्ष == अग्र) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                              RISCV_IOMMU_PQCSR_PQOF, 0);
    } अन्यथा {
        dma_addr_t addr = s->pq_addr + पुच्छ * माप(*pr);
        अगर (dma_memory_write(s->target_as, addr, pr, माप(*pr),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                                  RISCV_IOMMU_PQCSR_PQMF, 0);
        } अन्यथा {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_PQT, अग्र);
        }
    }

    अगर (ctrl & RISCV_IOMMU_PQCSR_PIE) {
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
अटल uint64_t riscv_iommu_pext_u64(uint64_t val, uint64_t ext)
{
    uint64_t ret = 0;
    uint64_t rot = 1;

    जबतक (ext) {
        अगर (ext & 1) {
            अगर (val & 1) {
                ret |= rot;
            }
            rot <<= 1;
        }
        val >>= 1;
        ext >>= 1;
    }

    वापस ret;
}

/* Check if GPA matches MSI/MRIF pattern. */
अटल bool riscv_iommu_msi_check(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    dma_addr_t gpa)
{
    अगर (!s->enable_msi) {
        वापस false;
    }

    अगर (get_field(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_MODE) !=
        RISCV_IOMMU_DC_MSIPTP_MODE_FLAT) {
        वापस false; /* Invalid MSI/MRIF mode */
    }

    अगर ((PPN_DOWN(gpa) ^ ctx->msi_addr_pattern) & ~ctx->msi_addr_mask) {
        वापस false; /* GPA not in MSI range defined by AIA IMSIC rules. */
    }

    वापस true;
}

/* Returns the NAPOT page mask, or 0 for reserved encodings. */
अटल hwaddr riscv_iommu_napot_page_mask(hwaddr ppn, hwaddr addr, hwaddr *out)
{
    पूर्णांक napot_bits = ctz64(ppn) + 1;
    hwaddr napot_mask, page_mask;

    /* The spec only defines 64KB (napot_bits == 4) */
    अगर (napot_bits != 4) {
        वापस 0;
    }

    napot_mask = (1ULL << napot_bits) - 1;
    page_mask = PPN_PHYS(napot_mask) | (TARGET_PAGE_आकार - 1);

    *out = PPN_PHYS(ppn & ~napot_mask) | (addr & page_mask);

    वापस page_mask;
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
अटल पूर्णांक riscv_iommu_spa_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb)
{
    IOMMUAccessFlags trans_perm = IOMMU_NONE;
    dma_addr_t addr, base;
    uint64_t satp, gatp, pte;
    bool en_s, en_g;
    काष्ठा {
        अचिन्हित अक्षर step;
        अचिन्हित अक्षर levels;
        अचिन्हित अक्षर ptidxbits;
        अचिन्हित अक्षर ptesize;
    } sc[2];
    /* Translation stage phase */
    क्रमागत {
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
    स्थिर IOMMUAccessFlags req_perm = iotlb->perm;

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
    अगर (!en_s && (req_perm & IOMMU_WO) &&
        riscv_iommu_msi_check(s, ctx, iotlb->iova)) {
        iotlb->target_as = &s->trap_as;
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_mask = ~TARGET_PAGE_MASK;
        वापस 0;
    }

    /* Exit early for pass-through mode. */
    अगर (!(en_s || en_g)) {
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_mask = ~TARGET_PAGE_MASK;
        /* Allow R/W in pass-through mode */
        iotlb->perm = IOMMU_RW;
        वापस 0;
    }

    /* S/G translation parameters. */
    क्रम (pass = 0; pass < 2; pass++) {
        uint32_t sv_mode;

        sc[pass].step = 0;
        अगर (pass ? (s->fctl & RISCV_IOMMU_FCTL_GXL) :
            (ctx->tc & RISCV_IOMMU_DC_TC_SXL)) {
            /* 32bit mode for GXL/SXL == 1 */
            चयन (pass ? gatp : satp) {
            हाल RISCV_IOMMU_DC_IOHGATP_MODE_BARE:
                sc[pass].levels    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                अवरोध;
            हाल RISCV_IOMMU_DC_IOHGATP_MODE_SV32X4:
                sv_mode = pass ? RISCV_IOMMU_CAP_SV32X4 : RISCV_IOMMU_CAP_SV32;
                अगर (!(s->cap & sv_mode)) {
                    वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].levels    = 2;
                sc[pass].ptidxbits = 10;
                sc[pass].ptesize   = 4;
                अवरोध;
            शेष:
                वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        } अन्यथा {
            /* 64bit mode for GXL/SXL == 0 */
            चयन (pass ? gatp : satp) {
            हाल RISCV_IOMMU_DC_IOHGATP_MODE_BARE:
                sc[pass].levels    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                अवरोध;
            हाल RISCV_IOMMU_DC_IOHGATP_MODE_SV39X4:
                sv_mode = pass ? RISCV_IOMMU_CAP_SV39X4 : RISCV_IOMMU_CAP_SV39;
                अगर (!(s->cap & sv_mode)) {
                    वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].levels    = 3;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                अवरोध;
            हाल RISCV_IOMMU_DC_IOHGATP_MODE_SV48X4:
                sv_mode = pass ? RISCV_IOMMU_CAP_SV48X4 : RISCV_IOMMU_CAP_SV48;
                अगर (!(s->cap & sv_mode)) {
                    वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].levels    = 4;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                अवरोध;
            हाल RISCV_IOMMU_DC_IOHGATP_MODE_SV57X4:
                sv_mode = pass ? RISCV_IOMMU_CAP_SV57X4 : RISCV_IOMMU_CAP_SV57;
                अगर (!(s->cap & sv_mode)) {
                    वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].levels    = 5;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                अवरोध;
            शेष:
                वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        }
    };

    /* S/G stages translation tables root pointers */
    gatp = PPN_PHYS(get_field(ctx->gatp, RISCV_IOMMU_ATP_PPN_FIELD));
    satp = PPN_PHYS(get_field(ctx->satp, RISCV_IOMMU_ATP_PPN_FIELD));
    addr = (en_s && en_g) ? satp : iotlb->iova;
    base = en_g ? gatp : satp;
    pass = en_g ? G_STAGE : S_STAGE;

    करो {
        स्थिर अचिन्हित widened = (pass && !sc[pass].step) ? 2 : 0;
        स्थिर अचिन्हित va_bits = widened + sc[pass].ptidxbits;
        स्थिर अचिन्हित va_skip = TARGET_PAGE_BITS + sc[pass].ptidxbits *
                                 (sc[pass].levels - 1 - sc[pass].step);
        स्थिर अचिन्हित idx = (addr >> va_skip) & ((1 << va_bits) - 1);
        स्थिर dma_addr_t pte_addr = base + idx * sc[pass].ptesize;
        स्थिर bool ade =
            ctx->tc & (pass ? RISCV_IOMMU_DC_TC_GADE : RISCV_IOMMU_DC_TC_SADE);

        अगर (ade && !(s->cap & RISCV_IOMMU_CAP_AMO_HWAD)) {
            /* GADE/SADE are reserved bits if AMO_HWAD is cleared.  */
            वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }

        /* Address range check before first level lookup */
        अगर (!sc[pass].step) {
            स्थिर uint64_t va_len = va_skip + va_bits;
            स्थिर uint64_t va_mask = (1ULL << va_len) - 1;

            अगर (pass == S_STAGE && va_len > 32) {
                uint64_t mask, masked_msbs;

                mask = MAKE_64BIT_MASK(0, target_long_bits() - va_len + 1);
                masked_msbs = (addr >> (va_len - 1)) & mask;

                अगर (masked_msbs != 0 && masked_msbs != mask) {
                    वापस (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S;
                }
            } अन्यथा {
                अगर ((addr & va_mask) != addr) {
                    वापस (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS;
                }
            }
        }


        अगर (pass == S_STAGE) {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_S_VS_WALKS);
        } अन्यथा {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_G_WALKS);
        }

        /* Read page table entry */
        अगर (sc[pass].ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->target_as, pte_addr, &pte32,
                             MEMTXATTRS_UNSPECIFIED);
            pte = pte32;
        } अन्यथा {
            ret = ldq_le_dma(s->target_as, pte_addr, &pte,
                             MEMTXATTRS_UNSPECIFIED);
        }
        अगर (ret != MEMTX_OK) {
            वापस (req_perm & IOMMU_WO) ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT
                                         : RISCV_IOMMU_FQ_CAUSE_RD_FAULT;
        }

        sc[pass].step++;
        hwaddr ppn = pte >> PTE_PPN_SHIFT;

        अगर (!(pte & PTE_V)) {
            अवरोध;                /* Invalid PTE */
        } अन्यथा अगर (pte & PTE_RESERVED(false)) {
            अवरोध;                /* Reserved PTE bits set */
        } अन्यथा अगर (!(pte & (PTE_R | PTE_W | PTE_X))) {
            base = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } अन्यथा अगर ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            अवरोध;                /* Reserved leaf PTE flags: PTE_W */
        } अन्यथा अगर ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            अवरोध;                /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } अन्यथा अगर (ppn & ((1ULL << (va_skip - TARGET_PAGE_BITS)) - 1)) {
            अवरोध;                /* Misaligned PPN */
        } अन्यथा अगर (!(pte & PTE_U) && !pv) {
            /*
             * All accesses are assumed to be User mode unless
             * process_id is valid (pv).  In case we have a
             * non-user mode leaf PTE and !pv we need to fault.
             */
            अवरोध;
        } अन्यथा अगर ((req_perm & IOMMU_RO) && !(pte & PTE_R)) {
            अवरोध;                /* Read access check failed */
        } अन्यथा अगर ((req_perm & IOMMU_WO) && !(pte & PTE_W)) {
            अवरोध;                /* Write access check failed */
        } अन्यथा अगर (!ade && !(pte & PTE_A)) {
            अवरोध;                /* Access bit not set */
        } अन्यथा अगर ((req_perm & IOMMU_WO) && !ade && !(pte & PTE_D)) {
            अवरोध;                /* Dirty bit not set */
        } अन्यथा अगर (pass == G_STAGE && !(pte & PTE_U)) {
            /*
             * riscv-iommu spec 1.0: "When checking the U bit in a
             * second-stage PTE, the transaction is treated as
             * not requesting supervisor privilege."
             *
             * I.e. we need to fault if this is a non-user PTE since
             * we are always in user mode at this point.
             */
            अवरोध;
        } अन्यथा {
            /* Leaf PTE, translation completed. */
            sc[pass].step = sc[pass].levels;

            अगर (pte & PTE_N) {
                hwaddr mask = riscv_iommu_napot_page_mask(ppn, addr, &base);

                अगर (!mask) {
                    अवरोध;
                }
                iotlb->addr_mask &= mask;
            } अन्यथा {
                base = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
                /* Update address mask based on smallest translation granularity */
                iotlb->addr_mask &= (1ULL << va_skip) - 1;
            }

            /* Continue with S-Stage translation? */
            अगर (pass && sc[0].step != sc[0].levels) {
                pass = S_STAGE;
                addr = iotlb->iova;
                जारी;
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
            अगर (pass == S_STAGE && (req_perm & IOMMU_WO) &&
                riscv_iommu_msi_check(s, ctx, base)) {
                /* Trap MSI writes and return GPA address. */
                iotlb->target_as = &s->trap_as;
                iotlb->addr_mask = ~TARGET_PAGE_MASK;
                वापस 0;
            }

            /* Continue with G-Stage translation? */
            अगर (!pass && en_g) {
                pass = G_STAGE;
                addr = base;
                base = gatp;
                sc[pass].step = 0;
                जारी;
            }

            iotlb->perm = trans_perm;
            वापस 0;
        }

        अगर (sc[pass].step == sc[pass].levels) {
            अवरोध; /* Can't find leaf PTE */
        }

        /* Continue with G-Stage translation? */
        अगर (!pass && en_g) {
            pass = G_STAGE;
            addr = base;
            base = gatp;
            sc[pass].step = 0;
        }
    } जबतक (1);

    /*
     * riscv_iommu_translate() will receive a fault and then call
     * riscv_iommu_report_fault() using iotlb->translated_addr
     * as iotval2.  Update translated_addr it with the latest
     * translated addr we have.
     */
    iotlb->translated_addr = addr;

    वापस (req_perm & IOMMU_WO) ?
                (pass ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S) :
                (pass ? RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S);
}

अटल व्योम riscv_iommu_report_fault(RISCVIOMMUState *s,
                                     RISCVIOMMUContext *ctx,
                                     uint32_t fault_type, uint32_t cause,
                                     bool pv,
                                     uint64_t iotval, uint64_t iotval2)
{
    काष्ठा riscv_iommu_fq_record ev = { 0 };

    अगर (ctx->tc & RISCV_IOMMU_DC_TC_DTF) {
        चयन (cause) {
        हाल RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED:
        हाल RISCV_IOMMU_FQ_CAUSE_DDT_LOAD_FAULT:
        हाल RISCV_IOMMU_FQ_CAUSE_DDT_INVALID:
        हाल RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED:
        हाल RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED:
        हाल RISCV_IOMMU_FQ_CAUSE_INTERNAL_DP_ERROR:
        हाल RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT:
            अवरोध;
        शेष:
            /* DTF prevents reporting a fault for this given cause */
            वापस;
        }
    }

    ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE, cause);
    ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE, fault_type);
    ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_DID, ctx->devid);
    ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_PV, pv);

    अगर (pv) {
        ev.hdr = set_field(ev.hdr, RISCV_IOMMU_FQ_HDR_PID, ctx->process_id);
    }

    ev.iotval = iotval;
    ev.iotval2 = iotval2;

    riscv_iommu_fault(s, &ev);
}

/* Redirect MSI write for given GPA. */
अटल Mem_Tx_परिणाम riscv_iommu_msi_write(RISCVIOMMUState *s,
    RISCVIOMMUContext *ctx, uint64_t gpa, uint64_t तथ्य,
    अचिन्हित आकार, MemTxAttrs attrs)
{
    Mem_Tx_परिणाम res;
    dma_addr_t addr;
    uint64_t intn;
    माप_प्रकार offset;
    uint32_t n190;
    uint64_t pte[2];
    पूर्णांक fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    पूर्णांक cause;

    /* Interrupt File Number */
    intn = riscv_iommu_pext_u64(PPN_DOWN(gpa), ctx->msi_addr_mask);
    offset = intn * माप(pte);

    /* fetch MSI PTE */
    addr = PPN_PHYS(get_field(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_PPN));
    अगर (addr & offset) {
        /* Interrupt file number out of range */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_LOAD_FAULT;
        जाओ err;
    }

    addr |= offset;
    res = dma_memory_read(s->target_as, addr, &pte, माप(pte),
            MEMTXATTRS_UNSPECIFIED);
    अगर (res != MEMTX_OK) {
        अगर (res == MEMTX_DECODE_ERROR) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED;
        } अन्यथा {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_LOAD_FAULT;
        }
        जाओ err;
    }

    le64_to_cpus(&pte[0]);
    le64_to_cpus(&pte[1]);

    अगर (!(pte[0] & RISCV_IOMMU_MSI_PTE_V) || (pte[0] & RISCV_IOMMU_MSI_PTE_C)) {
        /*
         * The spec mentions that: "If msipte.C == 1, then further
         * processing to interpret the PTE is implementation
         * defined.". We'll abort with cause = 262 for this
         * case too.
         */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_INVALID;
        जाओ err;
    }

    चयन (get_field(pte[0], RISCV_IOMMU_MSI_PTE_M)) {
    हाल RISCV_IOMMU_MSI_PTE_M_BASIC:
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
        अगर (pte[0] & (GENMASK_ULL(62, 54) | GENMASK_ULL(9, 3))) {
            res = MEMTX_DECODE_ERROR;
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
            जाओ err;
        }

        /* MSI Pass-through mode */
        addr = PPN_PHYS(get_field(pte[0], RISCV_IOMMU_MSI_PTE_PPN));

        trace_riscv_iommu_msi(s->parent_obj.id, PCI_BUS_अंक(ctx->devid),
                              PCI_SLOT(ctx->devid), PCI_FUNC(ctx->devid),
                              gpa, addr);

        res = dma_memory_write(s->target_as, addr, &तथ्य, आकार, attrs);
        अगर (res != MEMTX_OK) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
            जाओ err;
        }

        वापस MEMTX_OK;
    हाल RISCV_IOMMU_MSI_PTE_M_MRIF:
        /* MRIF mode, continue. */
        अवरोध;
    शेष:
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        जाओ err;
    }

    /*
     * Report an error for interrupt identities exceeding the maximum allowed
     * for an IMSIC interrupt file (2047) or destination address is not 32-bit
     * aligned. See IOMMU Specification, Chapter 2.3. MSI page tables.
     */
    अगर ((तथ्य > 2047) || (gpa & 3)) {
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        जाओ err;
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
    addr = addr | ((तथ्य & 0x7c0) >> 2);

    trace_riscv_iommu_msi(s->parent_obj.id, PCI_BUS_अंक(ctx->devid),
                          PCI_SLOT(ctx->devid), PCI_FUNC(ctx->devid),
                          gpa, addr);

    /* MRIF pending bit mask */
    तथ्य = 1ULL << (तथ्य & 0x03f);
    res = dma_memory_read(s->target_as, addr, &intn, माप(intn), attrs);
    अगर (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_LOAD_FAULT;
        जाओ err;
    }

    intn = intn | तथ्य;
    res = dma_memory_write(s->target_as, addr, &intn, माप(intn), attrs);
    अगर (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        जाओ err;
    }

    /* Get MRIF enable bits */
    addr = addr + माप(intn);
    res = dma_memory_read(s->target_as, addr, &intn, माप(intn), attrs);
    अगर (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_LOAD_FAULT;
        जाओ err;
    }

    अगर (!(intn & तथ्य)) {
        /* notification disabled, MRIF update completed. */
        वापस MEMTX_OK;
    }

    /* Send notification message */
    addr = PPN_PHYS(get_field(pte[1], RISCV_IOMMU_MSI_MRIF_NPPN));
    n190 = get_field(pte[1], RISCV_IOMMU_MSI_MRIF_NID) |
          (get_field(pte[1], RISCV_IOMMU_MSI_MRIF_NID_MSB) << 10);

    res = dma_memory_write(s->target_as, addr, &n190, माप(n190), attrs);
    अगर (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        जाओ err;
    }

    trace_riscv_iommu_mrif_notification(s->parent_obj.id, n190, addr);

    वापस MEMTX_OK;

err:
    riscv_iommu_report_fault(s, ctx, fault_type, cause,
                             !!ctx->process_id, 0, 0);
    वापस res;
}

/*
 * Check device context configuration as described by the
 * riscv-iommu spec section "Device-context configuration
 * checks".
 */
अटल bool riscv_iommu_validate_device_ctx(RISCVIOMMUState *s,
                                            RISCVIOMMUContext *ctx)
{
    uint32_t fsc_mode, msi_mode;
    uint64_t gatp;

    अगर (ctx->tc & RISCV_IOMMU_DC_TC_RESERVED) {
        वापस false;
    }

    अगर (!(s->cap & RISCV_IOMMU_CAP_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI ||
         ctx->tc & RISCV_IOMMU_DC_TC_PRPR)) {
        वापस false;
    }

    अगर (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI)) {
        वापस false;
    }

    अगर (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI) &&
        ctx->tc & RISCV_IOMMU_DC_TC_PRPR) {
        वापस false;
    }

    अगर (!(s->cap & RISCV_IOMMU_CAP_T2GPA) &&
        ctx->tc & RISCV_IOMMU_DC_TC_T2GPA) {
        वापस false;
    }

    अगर (s->cap & RISCV_IOMMU_CAP_MSI_FLAT) {
        msi_mode = get_field(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_MODE);

        अगर (msi_mode != RISCV_IOMMU_DC_MSIPTP_MODE_OFF &&
            msi_mode != RISCV_IOMMU_DC_MSIPTP_MODE_FLAT) {
            वापस false;
        }
    }

    gatp = get_field(ctx->gatp, RISCV_IOMMU_ATP_MODE_FIELD);
    अगर (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA &&
        gatp == RISCV_IOMMU_DC_IOHGATP_MODE_BARE) {
        वापस false;
    }

    अगर (gatp != RISCV_IOMMU_DC_IOHGATP_MODE_BARE) {
        uint64_t iohgatp_ppn = get_field(ctx->gatp,
                                         RISCV_IOMMU_DC_IOHGATP_PPN);

        /*
         * One of the conditions for a misconfigured DDT entry
         * according to the riscv-spec: "DC.iohgatp.MODE is not
         * Bare and the root page table (address) determined by
         * DC.iohgatp.PPN is not aligned to a 16-KiB boundary."
         */
        अगर (PPN_PHYS(iohgatp_ppn) & ((1ULL << 14) - 1)) {
            वापस false;
        }
    }

    fsc_mode = get_field(ctx->satp, RISCV_IOMMU_DC_FSC_MODE);

    अगर (ctx->tc & RISCV_IOMMU_DC_TC_PDTV) {
        चयन (fsc_mode) {
        हाल RISCV_IOMMU_DC_FSC_PDTP_MODE_PD8:
            अगर (!(s->cap & RISCV_IOMMU_CAP_PD8)) {
                वापस false;
            }
            अवरोध;
        हाल RISCV_IOMMU_DC_FSC_PDTP_MODE_PD17:
            अगर (!(s->cap & RISCV_IOMMU_CAP_PD17)) {
                वापस false;
            }
            अवरोध;
        हाल RISCV_IOMMU_DC_FSC_PDTP_MODE_PD20:
            अगर (!(s->cap & RISCV_IOMMU_CAP_PD20)) {
                वापस false;
            }
            अवरोध;
        }
    } अन्यथा {
        /* DC.tc.PDTV is 0 */
        अगर (ctx->tc & RISCV_IOMMU_DC_TC_DPE) {
            वापस false;
        }

        अगर (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
            अगर (fsc_mode == RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV32 &&
                !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                वापस false;
            }
        } अन्यथा {
            चयन (fsc_mode) {
            हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV39:
                अगर (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                    वापस false;
                }
                अवरोध;
            हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV48:
                अगर (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                    वापस false;
                }
            अवरोध;
            हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV57:
                अगर (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                    वापस false;
                }
                अवरोध;
            }
        }
    }

    /*
     * CAP_END is always zero (only one endianess). FCTL_BE is
     * always zero (little-endian accesses). Thus TC_SBE must
     * always be LE, i.e. zero.
     */
    अगर (ctx->tc & RISCV_IOMMU_DC_TC_SBE) {
        वापस false;
    }

    वापस true;
}

/*
 * Validate process context (PC) according to section
 * "Process-context configuration checks".
 */
अटल bool riscv_iommu_validate_process_ctx(RISCVIOMMUState *s,
                                             RISCVIOMMUContext *ctx)
{
    uint32_t mode;

    अगर (get_field(ctx->ta, RISCV_IOMMU_PC_TA_RESERVED)) {
        वापस false;
    }

    अगर (get_field(ctx->satp, RISCV_IOMMU_PC_FSC_RESERVED)) {
        वापस false;
    }

    mode = get_field(ctx->satp, RISCV_IOMMU_DC_FSC_MODE);
    चयन (mode) {
    हाल RISCV_IOMMU_DC_FSC_MODE_BARE:
    /* sv39 and sv32 modes have the same value (8) */
    हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV39:
    हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV48:
    हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV57:
        अवरोध;
    शेष:
        वापस false;
    }

    अगर (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
        अगर (mode == RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV32 &&
            !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                वापस false;
        }
    } अन्यथा {
        चयन (mode) {
        हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV39:
            अगर (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                वापस false;
            }
            अवरोध;
        हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV48:
            अगर (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                वापस false;
            }
            अवरोध;
        हाल RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV57:
            अगर (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                वापस false;
            }
            अवरोध;
        }
    }

    वापस true;
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
अटल Mem_Tx_परिणाम pdt_memory_read(RISCVIOMMUState *s,
                                   RISCVIOMMUContext *ctx,
                                   dma_addr_t addr,
                                   व्योम *buf, dma_addr_t len,
                                   MemTxAttrs attrs)
{
    uint64_t gatp_mode, pte;
    काष्ठा {
        अचिन्हित अक्षर step;
        अचिन्हित अक्षर levels;
        अचिन्हित अक्षर ptidxbits;
        अचिन्हित अक्षर ptesize;
    } sc;
    Mem_Tx_परिणाम ret;
    dma_addr_t base = addr;

    /* G stages translation mode */
    gatp_mode = get_field(ctx->gatp, RISCV_IOMMU_ATP_MODE_FIELD);
    अगर (gatp_mode == RISCV_IOMMU_DC_IOHGATP_MODE_BARE) {
        जाओ out;
    }

    /* G stages translation tables root pointer */
    base = PPN_PHYS(get_field(ctx->gatp, RISCV_IOMMU_ATP_PPN_FIELD));

    /* Start at step 0 */
    sc.step = 0;

    अगर (s->fctl & RISCV_IOMMU_FCTL_GXL) {
        /* 32bit mode for GXL == 1 */
        चयन (gatp_mode) {
        हाल RISCV_IOMMU_DC_IOHGATP_MODE_SV32X4:
            अगर (!(s->cap & RISCV_IOMMU_CAP_SV32X4)) {
                वापस MEMTX_ACCESS_ERROR;
            }
            sc.levels    = 2;
            sc.ptidxbits = 10;
            sc.ptesize   = 4;
            अवरोध;
        शेष:
            वापस MEMTX_ACCESS_ERROR;
        }
    } अन्यथा {
        /* 64bit mode for GXL == 0 */
        चयन (gatp_mode) {
        हाल RISCV_IOMMU_DC_IOHGATP_MODE_SV39X4:
            अगर (!(s->cap & RISCV_IOMMU_CAP_SV39X4)) {
                वापस MEMTX_ACCESS_ERROR;
            }
            sc.levels    = 3;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            अवरोध;
        हाल RISCV_IOMMU_DC_IOHGATP_MODE_SV48X4:
            अगर (!(s->cap & RISCV_IOMMU_CAP_SV48X4)) {
                वापस MEMTX_ACCESS_ERROR;
            }
            sc.levels    = 4;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            अवरोध;
        हाल RISCV_IOMMU_DC_IOHGATP_MODE_SV57X4:
            अगर (!(s->cap & RISCV_IOMMU_CAP_SV57X4)) {
                वापस MEMTX_ACCESS_ERROR;
            }
            sc.levels    = 5;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            अवरोध;
        शेष:
            वापस MEMTX_ACCESS_ERROR;
        }
    }

    करो {
        स्थिर अचिन्हित va_bits = (sc.step ? 0 : 2) + sc.ptidxbits;
        स्थिर अचिन्हित va_skip = TARGET_PAGE_BITS + sc.ptidxbits *
                                 (sc.levels - 1 - sc.step);
        स्थिर अचिन्हित idx = (addr >> va_skip) & ((1 << va_bits) - 1);
        स्थिर dma_addr_t pte_addr = base + idx * sc.ptesize;

        /* Address range check before first level lookup */
        अगर (!sc.step) {
            स्थिर uint64_t va_mask = (1ULL << (va_skip + va_bits)) - 1;
            अगर ((addr & va_mask) != addr) {
                वापस MEMTX_ACCESS_ERROR;
            }
        }

        /* Read page table entry */
        अगर (sc.ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->target_as, pte_addr, &pte32, attrs);
            pte = pte32;
        } अन्यथा {
            ret = ldq_le_dma(s->target_as, pte_addr, &pte, attrs);
        }
        अगर (ret != MEMTX_OK) {
            वापस ret;
        }

        sc.step++;
        hwaddr ppn = pte >> PTE_PPN_SHIFT;

        अगर (!(pte & PTE_V)) {
            वापस MEMTX_ACCESS_ERROR; /* Invalid PTE */
        } अन्यथा अगर (!(pte & (PTE_R | PTE_W | PTE_X))) {
            base = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } अन्यथा अगर ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            वापस MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W */
        } अन्यथा अगर ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            वापस MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } अन्यथा अगर (ppn & ((1ULL << (va_skip - TARGET_PAGE_BITS)) - 1)) {
            वापस MEMTX_ACCESS_ERROR; /* Misaligned PPN */
        } अन्यथा {
            /* Leaf PTE, translation completed. */
            अगर (pte & PTE_N) {
                अगर (!riscv_iommu_napot_page_mask(ppn, addr, &base)) {
                    वापस MEMTX_ACCESS_ERROR;
                }
            } अन्यथा {
                base = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
            }
            अवरोध;
        }

        अगर (sc.step == sc.levels) {
            वापस MEMTX_ACCESS_ERROR; /* Can't find leaf PTE */
        }
    } जबतक (1);

out:
    वापस dma_memory_read(s->target_as, base, buf, len, attrs);
}

/*
 * RISC-V IOMMU Device Context Loopkup - Device Directory Tree Walk
 *
 * @s         : IOMMU Device State
 * @ctx       : Device Translation Context with devid and process_id set.
 * @return    : success or fault code.
 */
अटल पूर्णांक riscv_iommu_ctx_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx)
{
    स्थिर uint64_t ddtp = s->ddtp;
    अचिन्हित mode = get_field(ddtp, RISCV_IOMMU_DDTP_MODE);
    dma_addr_t addr = PPN_PHYS(get_field(ddtp, RISCV_IOMMU_DDTP_PPN));
    काष्ठा riscv_iommu_dc dc;
    /* Device Context format: 0: extended (64 bytes) | 1: base (32 bytes) */
    स्थिर पूर्णांक dc_fmt = !s->enable_msi;
    स्थिर माप_प्रकार dc_len = माप(dc) >> dc_fmt;
    पूर्णांक depth;
    uint64_t de;

    चयन (mode) {
    हाल RISCV_IOMMU_DDTP_MODE_OFF:
        वापस RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED;

    हाल RISCV_IOMMU_DDTP_MODE_BARE:
        /* mock up pass-through translation context */
        ctx->gatp = set_field(0, RISCV_IOMMU_ATP_MODE_FIELD,
            RISCV_IOMMU_DC_IOHGATP_MODE_BARE);
        ctx->satp = set_field(0, RISCV_IOMMU_ATP_MODE_FIELD,
            RISCV_IOMMU_DC_FSC_MODE_BARE);

        ctx->tc = RISCV_IOMMU_DC_TC_V;
        अगर (s->enable_ats) {
            ctx->tc |= RISCV_IOMMU_DC_TC_EN_ATS;
        }

        ctx->ta = 0;
        ctx->msiptp = 0;
        वापस 0;

    हाल RISCV_IOMMU_DDTP_MODE_1LVL:
        depth = 0;
        अवरोध;

    हाल RISCV_IOMMU_DDTP_MODE_2LVL:
        depth = 1;
        अवरोध;

    हाल RISCV_IOMMU_DDTP_MODE_3LVL:
        depth = 2;
        अवरोध;

    शेष:
        वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /*
     * Check supported device id width (in bits).
     * See IOMMU Specification, Chapter 6. Software guidelines.
     * - if extended device-context format is used:
     *   1LVL: 6, 2LVL: 15, 3LVL: 24
     * - if base device-context format is used:
     *   1LVL: 7, 2LVL: 16, 3LVL: 24
     */
    अगर (ctx->devid >= (1 << (depth * 9 + 6 + (dc_fmt && depth != 2)))) {
        वापस RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
    }

    /* Device directory tree walk */
    क्रम (; depth-- > 0; ) {
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
        स्थिर पूर्णांक split = depth * 9 + 6 + dc_fmt;
        addr |= ((ctx->devid >> split) << 3) & ~TARGET_PAGE_MASK;
        अगर (dma_memory_read(s->target_as, addr, &de, माप(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            वापस RISCV_IOMMU_FQ_CAUSE_DDT_LOAD_FAULT;
        }
        le64_to_cpus(&de);
        अगर (!(de & RISCV_IOMMU_DDTE_VALID)) {
            /* invalid directory entry */
            वापस RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        अगर (de & ~(RISCV_IOMMU_DDTE_PPN | RISCV_IOMMU_DDTE_VALID)) {
            /* reserved bits set */
            वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }
        addr = PPN_PHYS(get_field(de, RISCV_IOMMU_DDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);

    /* index into device context entry page */
    addr |= (ctx->devid * dc_len) & ~TARGET_PAGE_MASK;

    स_रखो(&dc, 0, माप(dc));
    अगर (dma_memory_read(s->target_as, addr, &dc, dc_len,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        वापस RISCV_IOMMU_FQ_CAUSE_DDT_LOAD_FAULT;
    }

    /* Set translation context. */
    ctx->tc = le64_to_cpu(dc.tc);
    ctx->gatp = le64_to_cpu(dc.iohgatp);
    ctx->satp = le64_to_cpu(dc.fsc);
    ctx->ta = le64_to_cpu(dc.ta);
    ctx->msiptp = le64_to_cpu(dc.msiptp);
    ctx->msi_addr_mask = le64_to_cpu(dc.msi_addr_mask);
    ctx->msi_addr_pattern = le64_to_cpu(dc.msi_addr_pattern);

    अगर (!(ctx->tc & RISCV_IOMMU_DC_TC_V)) {
        वापस RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
    }

    अगर (!riscv_iommu_validate_device_ctx(s, ctx)) {
        वापस RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /* FSC field checks */
    mode = get_field(ctx->satp, RISCV_IOMMU_DC_FSC_MODE);
    addr = PPN_PHYS(get_field(ctx->satp, RISCV_IOMMU_DC_FSC_PPN));

    अगर (!(ctx->tc & RISCV_IOMMU_DC_TC_PDTV)) {
        अगर (ctx->process_id != RISCV_IOMMU_NOPROCID) {
            /* PID is disabled */
            वापस RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
        }
        अगर (mode > RISCV_IOMMU_DC_FSC_IOSATP_MODE_SV57) {
            /* Invalid translation mode */
            वापस RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        वापस 0;
    }

    अगर (ctx->process_id == RISCV_IOMMU_NOPROCID) {
        अगर (!(ctx->tc & RISCV_IOMMU_DC_TC_DPE)) {
            /* No default process_id enabled, set BARE mode */
            ctx->satp = 0ULL;
            वापस 0;
        } अन्यथा {
            /* Use default process_id #0 */
            ctx->process_id = 0;
        }
    }

    अगर (mode == RISCV_IOMMU_DC_FSC_MODE_BARE) {
        /* No S-Stage translation, done. */
        वापस 0;
    }

    /* FSC.TC.PDTV enabled */
    अगर (mode > RISCV_IOMMU_DC_FSC_PDTP_MODE_PD20) {
        /* Invalid PDTP.MODE */
        वापस RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    क्रम (depth = mode - RISCV_IOMMU_DC_FSC_PDTP_MODE_PD8; depth-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

        /*
         * Select process id index bits based on process directory tree
         * level. See IOMMU Specification, 2.2. Process-Directory-Table.
         */
        स्थिर पूर्णांक split = depth * 9 + 8;
        addr |= ((ctx->process_id >> split) << 3) & ~TARGET_PAGE_MASK;
        अगर (pdt_memory_read(s, ctx, addr, &de, माप(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            वापस RISCV_IOMMU_FQ_CAUSE_PDT_LOAD_FAULT;
        }
        le64_to_cpus(&de);
        अगर (!(de & RISCV_IOMMU_PDTE_VALID)) {
            वापस RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
        }
        addr = PPN_PHYS(get_field(de, RISCV_IOMMU_PDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

    /* Leaf entry in PDT */
    addr |= (ctx->process_id << 4) & ~TARGET_PAGE_MASK;
    अगर (pdt_memory_read(s, ctx, addr, &dc.ta, माप(uint64_t) * 2,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        वापस RISCV_IOMMU_FQ_CAUSE_PDT_LOAD_FAULT;
    }

    /* Use FSC and TA from process directory entry. */
    ctx->ta = le64_to_cpu(dc.ta);
    ctx->satp = le64_to_cpu(dc.fsc);

    अगर (!(ctx->ta & RISCV_IOMMU_PC_TA_V)) {
        वापस RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
    }

    अगर (!riscv_iommu_validate_process_ctx(s, ctx)) {
        वापस RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    वापस 0;
}

/* Translation Context cache support */
अटल gboolean riscv_iommu_ctx_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUContext *c1 = (RISCVIOMMUContext *) v1;
    RISCVIOMMUContext *c2 = (RISCVIOMMUContext *) v2;
    वापस c1->devid == c2->devid &&
           c1->process_id == c2->process_id;
}

अटल guint riscv_iommu_ctx_hash(gconstpointer v)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) v;
    /*
     * Generate simple hash of (process_id, devid)
     * assuming 24-bit wide devid.
     */
    वापस (guint)(ctx->devid) + ((guint)(ctx->process_id) << 24);
}

अटल व्योम riscv_iommu_ctx_inval_devid_procid(gpointer कुंजी, gpointer मूल्य,
                                               gpointer तथ्य)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मूल्य;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) तथ्य;
    अगर (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid &&
        ctx->process_id == arg->process_id) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

अटल व्योम riscv_iommu_ctx_inval_devid(gpointer कुंजी, gpointer मूल्य,
                                        gpointer तथ्य)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मूल्य;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) तथ्य;
    अगर (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

अटल व्योम riscv_iommu_ctx_inval_all(gpointer कुंजी, gpointer मूल्य,
                                      gpointer तथ्य)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) मूल्य;
    अगर (ctx->tc & RISCV_IOMMU_DC_TC_V) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

अटल व्योम riscv_iommu_ctx_inval(RISCVIOMMUState *s, GHFunc func,
                                  uint32_t devid, uint32_t process_id)
{
    GHashTable *ctx_cache;
    RISCVIOMMUContext कुंजी = {
        .devid = devid,
        .process_id = process_id,
    };
    ctx_cache = g_hash_table_ref(s->ctx_cache);
    g_hash_table_foreach(ctx_cache, func, &कुंजी);
    g_hash_table_unref(ctx_cache);
}

/* Find or allocate translation context for a given {device_id, process_id} */
अटल RISCVIOMMUContext *riscv_iommu_ctx(RISCVIOMMUState *s,
                                          अचिन्हित devid, अचिन्हित process_id,
                                          IOMMUAccessFlags perm, uint64_t iova,
                                          व्योम **ref)
{
    GHashTable *ctx_cache;
    RISCVIOMMUContext *ctx;
    RISCVIOMMUContext कुंजी = {
        .devid = devid,
        .process_id = process_id,
    };
    अचिन्हित mode = get_field(s->ddtp, RISCV_IOMMU_DDTP_MODE);
    uint32_t fault_type;

    ctx_cache = g_hash_table_ref(s->ctx_cache);

    अगर (mode != RISCV_IOMMU_DDTP_MODE_OFF &&
        mode != RISCV_IOMMU_DDTP_MODE_BARE) {
        ctx = g_hash_table_lookup(ctx_cache, &कुंजी);

        अगर (ctx && (ctx->tc & RISCV_IOMMU_DC_TC_V)) {
            *ref = ctx_cache;
            वापस ctx;
        }
    }

    ctx = g_new0(RISCVIOMMUContext, 1);
    ctx->devid = devid;
    ctx->process_id = process_id;

    पूर्णांक fault = riscv_iommu_ctx_fetch(s, ctx);
    अगर (!fault) {
        अगर (mode != RISCV_IOMMU_DDTP_MODE_BARE) {
            अगर (g_hash_table_आकार(ctx_cache) >= LIMIT_CACHE_CTX) {
                g_hash_table_unref(ctx_cache);
                ctx_cache = g_hash_table_new_full(riscv_iommu_ctx_hash,
                                                  riscv_iommu_ctx_equal,
                                                  g_free, शून्य);
                g_hash_table_ref(ctx_cache);
                g_hash_table_unref(qatomic_xchg(&s->ctx_cache, ctx_cache));
            }

            g_hash_table_add(ctx_cache, ctx);
            *ref = ctx_cache;
        } अन्यथा {
            g_hash_table_unref(ctx_cache);
            /* Remember ctx so it can be freed */
            *ref = ctx;
        }
        वापस ctx;
    }

    g_hash_table_unref(ctx_cache);
    *ref = शून्य;

    /*
     * TODO: (1) do we need to distinguish other fault types
     * for ctx fetching and (2) evaluate putting the 'fault_type'
     * logic inside riscv_iommu_report_fault() - there's at
     * least one other place (end of riscv_iommu_translate())
     * that does something similar.
     */
    अगर (perm & IOMMU_RO) {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
    } अन्यथा {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    }

    riscv_iommu_report_fault(s, ctx, fault_type, fault,
                             !!process_id, iova, 0);

    g_free(ctx);
    वापस शून्य;
}

अटल व्योम riscv_iommu_ctx_put(RISCVIOMMUState *s, व्योम *ref)
{
    अचिन्हित mode = get_field(s->ddtp, RISCV_IOMMU_DDTP_MODE);

    अगर (!ref) {
        वापस;
    }

    /* ref is pointing to ctx in Bare mode. Bare mode ctx is not cached */
    अगर (mode == RISCV_IOMMU_DDTP_MODE_BARE) {
        g_free(ref);
    } अन्यथा {
        g_hash_table_unref((GHashTable *)ref);
    }
}

/* Find or allocate address space for a given device */
अटल AddressSpace *riscv_iommu_space(RISCVIOMMUState *s, PCIBus *bus,
                                            पूर्णांक devfn)
{
    RISCVIOMMUSpace *as;

    QLIST_FOREACH(as, &s->spaces, list) {
        अगर (as->bus == bus && as->devfn == devfn) {
            अवरोध;
        }
    }

    अगर (as == शून्य) {
        अक्षर name[64];
        as = g_new0(RISCVIOMMUSpace, 1);

        as->iommu = s;
        as->bus = bus;
        as->devfn = devfn;

        snprintf(name, माप(name), "riscv-iommu-%04x:%02x.%d-iova",
            pci_bus_अंक(bus), PCI_SLOT(devfn), PCI_FUNC(devfn));

        /* IOVA address space, untranslated addresses */
        memory_region_init_iommu(&as->iova_mr, माप(as->iova_mr),
            TYPE_RISCV_IOMMU_MEMORY_REGION,
            OBJECT(s), "riscv_iommu", UINT64_MAX);
        address_space_init(&as->iova_as, MEMORY_REGION(&as->iova_mr), name);

        QLIST_जोड़ो_शीर्ष(&s->spaces, as, list);

        trace_riscv_iommu_new(s->parent_obj.id, pci_bus_अंक(bus),
                PCI_SLOT(devfn), PCI_FUNC(devfn));
    }
    वापस &as->iova_as;
}

/* Translation Object cache support */
अटल gboolean riscv_iommu_iot_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUEntry *t1 = (RISCVIOMMUEntry *) v1;
    RISCVIOMMUEntry *t2 = (RISCVIOMMUEntry *) v2;
    वापस t1->gscid == t2->gscid && t1->pscid == t2->pscid &&
           t1->iova == t2->iova && t1->tag == t2->tag;
}

अटल guint riscv_iommu_iot_hash(gconstpointer v)
{
    RISCVIOMMUEntry *t = (RISCVIOMMUEntry *) v;
    वापस (guint)t->iova;
}

/* GV: 0 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 0 AV: 0 GVMA: 1 */
अटल
व्योम riscv_iommu_iot_inval_all(gpointer कुंजी, gpointer मूल्य, gpointer तथ्य)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मूल्य;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) तथ्य;
    अगर (iot->tag == arg->tag) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 0 PSCV: 1 GVMA: 0 */
अटल
व्योम riscv_iommu_iot_inval_pscid(gpointer कुंजी, gpointer मूल्य, gpointer तथ्य)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मूल्य;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) तथ्य;
    अगर (iot->tag == arg->tag &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 0 GVMA: 0 */
अटल
व्योम riscv_iommu_iot_inval_iova(gpointer कुंजी, gpointer मूल्य, gpointer तथ्य)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मूल्य;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) तथ्य;
    अगर (iot->tag == arg->tag &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 1 GVMA: 0 */
अटल व्योम riscv_iommu_iot_inval_pscid_iova(gpointer कुंजी, gpointer मूल्य,
                                             gpointer तथ्य)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मूल्य;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) तथ्य;
    अगर (iot->tag == arg->tag &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 0 GVMA: 1 */
अटल
व्योम riscv_iommu_iot_inval_gscid(gpointer कुंजी, gpointer मूल्य, gpointer तथ्य)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मूल्य;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) तथ्य;
    अगर (iot->tag == arg->tag &&
        iot->gscid == arg->gscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 1 GVMA: 0 */
अटल व्योम riscv_iommu_iot_inval_gscid_pscid(gpointer कुंजी, gpointer मूल्य,
                                              gpointer तथ्य)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मूल्य;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) तथ्य;
    अगर (iot->tag == arg->tag &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 1 GVMA: 1 */
अटल व्योम riscv_iommu_iot_inval_gscid_iova(gpointer कुंजी, gpointer मूल्य,
                                             gpointer तथ्य)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मूल्य;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) तथ्य;
    अगर (iot->tag == arg->tag &&
        iot->gscid == arg->gscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 1 GVMA: 0 */
अटल व्योम riscv_iommu_iot_inval_gscid_pscid_iova(gpointer कुंजी, gpointer मूल्य,
                                                   gpointer तथ्य)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) मूल्य;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) तथ्य;
    अगर (iot->tag == arg->tag &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* caller should keep ref-count for iot_cache object */
अटल RISCVIOMMUEntry *riscv_iommu_iot_lookup(RISCVIOMMUContext *ctx,
    GHashTable *iot_cache, hwaddr iova, RISCVIOMMUTransTag transtag)
{
    RISCVIOMMUEntry कुंजी = {
        .tag   = transtag,
        .gscid = get_field(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID),
        .pscid = get_field(ctx->ta, RISCV_IOMMU_DC_TA_PSCID),
        .iova  = PPN_DOWN(iova),
    };
    वापस g_hash_table_lookup(iot_cache, &कुंजी);
}

/* caller should keep ref-count for iot_cache object */
अटल व्योम riscv_iommu_iot_update(RISCVIOMMUState *s,
    GHashTable *iot_cache, RISCVIOMMUEntry *iot)
{
    अगर (!s->iot_limit) {
        वापस;
    }

    अगर (g_hash_table_आकार(s->iot_cache) >= s->iot_limit) {
        iot_cache = g_hash_table_new_full(riscv_iommu_iot_hash,
                                          riscv_iommu_iot_equal,
                                          g_free, शून्य);
        g_hash_table_unref(qatomic_xchg(&s->iot_cache, iot_cache));
    }
    g_hash_table_add(iot_cache, iot);
}

अटल व्योम riscv_iommu_iot_inval(RISCVIOMMUState *s, GHFunc func,
    uint32_t gscid, uint32_t pscid, hwaddr iova, RISCVIOMMUTransTag transtag)
{
    GHashTable *iot_cache;
    RISCVIOMMUEntry कुंजी = {
        .tag = transtag,
        .gscid = gscid,
        .pscid = pscid,
        .iova  = PPN_DOWN(iova),
    };

    iot_cache = g_hash_table_ref(s->iot_cache);
    g_hash_table_foreach(iot_cache, func, &कुंजी);
    g_hash_table_unref(iot_cache);
}

अटल RISCVIOMMUTransTag riscv_iommu_get_transtag(RISCVIOMMUContext *ctx)
{
    uint64_t satp = get_field(ctx->satp, RISCV_IOMMU_ATP_MODE_FIELD);
    uint64_t gatp = get_field(ctx->gatp, RISCV_IOMMU_ATP_MODE_FIELD);

    अगर (satp == RISCV_IOMMU_DC_FSC_MODE_BARE) {
        वापस (gatp == RISCV_IOMMU_DC_IOHGATP_MODE_BARE) ?
            RISCV_IOMMU_TRANS_TAG_BY : RISCV_IOMMU_TRANS_TAG_VG;
    } अन्यथा {
        वापस (gatp == RISCV_IOMMU_DC_IOHGATP_MODE_BARE) ?
            RISCV_IOMMU_TRANS_TAG_SS : RISCV_IOMMU_TRANS_TAG_VN;
    }
}

अटल पूर्णांक riscv_iommu_translate(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb, bool enable_cache)
{
    RISCVIOMMUTransTag transtag = riscv_iommu_get_transtag(ctx);
    RISCVIOMMUEntry *iot;
    IOMMUAccessFlags perm;
    bool enable_pid;
    bool enable_pri;
    GHashTable *iot_cache;
    पूर्णांक fault;

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_URQ);

    iot_cache = g_hash_table_ref(s->iot_cache);
    enable_pri = (iotlb->perm == IOMMU_NONE) &&
                 (ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI);
    enable_pid = (ctx->tc & RISCV_IOMMU_DC_TC_PDTV);

    /* Check for ATS request. */
    अगर (iotlb->perm == IOMMU_NONE) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_ATS_RQ);
        /* Check if ATS is disabled. */
        अगर (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS)) {
            enable_pri = false;
            fault = RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
            जाओ done;
        }
    }

    iot = riscv_iommu_iot_lookup(ctx, iot_cache, iotlb->iova, transtag);
    perm = iot ? iot->perm : IOMMU_NONE;
    अगर (perm != IOMMU_NONE) {
        iotlb->translated_addr = PPN_PHYS(iot->phys);
        iotlb->addr_mask = ~TARGET_PAGE_MASK;
        iotlb->perm = perm;
        fault = 0;
        जाओ done;
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_TLB_MISS);

    /* Translate using device directory / page table information. */
    fault = riscv_iommu_spa_fetch(s, ctx, iotlb);

    अगर (!fault && iotlb->target_as == &s->trap_as) {
        /* Do not cache trapped MSI translations */
        जाओ done;
    }

    /*
     * We made an implementation choice to not cache identity-mapped
     * translations, as allowed by the specification, to avoid
     * translation cache evictions for other devices sharing the
     * IOMMU hardware model.
     */
    अगर (!fault && iotlb->translated_addr != iotlb->iova && enable_cache) {
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

    अगर (enable_pri && fault) {
        काष्ठा riscv_iommu_pq_record pr = {0};
        अगर (enable_pid) {
            pr.hdr = set_field(RISCV_IOMMU_PREQ_HDR_PV,
                               RISCV_IOMMU_PREQ_HDR_PID, ctx->process_id);
        }
        pr.hdr = set_field(pr.hdr, RISCV_IOMMU_PREQ_HDR_DID, ctx->devid);
        pr.payload = (iotlb->iova & TARGET_PAGE_MASK) |
                     RISCV_IOMMU_PREQ_PAYLOAD_M;
        riscv_iommu_pri(s, &pr);
        वापस fault;
    }

    अगर (fault) {
        अचिन्हित ttype = RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ;

        अगर ((iotlb->perm & IOMMU_RW) == IOMMU_RW
            || iotlb->perm & IOMMU_WO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
        } अन्यथा अगर (iotlb->perm & IOMMU_RO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
        }

        riscv_iommu_report_fault(s, ctx, ttype, fault, enable_pid,
                                 iotlb->iova, iotlb->translated_addr);
        वापस fault;
    }

    वापस 0;
}

/* IOMMU Command Interface */
अटल Mem_Tx_परिणाम riscv_iommu_iofence(RISCVIOMMUState *s, bool notify,
    uint64_t addr, uint32_t तथ्य)
{
    /*
     * ATS processing in this implementation of the IOMMU is synchronous,
     * no need to wait for completions here.
     */
    अगर (!notify) {
        वापस MEMTX_OK;
    }

    वापस dma_memory_write(s->target_as, addr, &तथ्य, माप(तथ्य),
        MEMTXATTRS_UNSPECIFIED);
}

अटल व्योम riscv_iommu_ats(RISCVIOMMUState *s,
    काष्ठा riscv_iommu_command *cmd, IOMMUNotifierFlag flag,
    IOMMUAccessFlags perm,
    व्योम (*trace_fn)(स्थिर अक्षर *id))
{
    RISCVIOMMUSpace *as;
    IOMMUNotifier *n;
    IOMMUTLBEvent event;
    uint32_t pid;
    uint32_t devid;
    स्थिर bool pv = cmd->dword0 & RISCV_IOMMU_CMD_ATS_PV;

    अगर (cmd->dword0 & RISCV_IOMMU_CMD_ATS_DSV) {
        /* Use device segment and requester id */
        devid = get_field(cmd->dword0,
            RISCV_IOMMU_CMD_ATS_DSEG | RISCV_IOMMU_CMD_ATS_RID);
    } अन्यथा {
        devid = get_field(cmd->dword0, RISCV_IOMMU_CMD_ATS_RID);
    }

    pid = get_field(cmd->dword0, RISCV_IOMMU_CMD_ATS_PID);

    QLIST_FOREACH(as, &s->spaces, list) {
        अगर (riscv_iommu_space_devid(as) == devid) {
            अवरोध;
        }
    }

    अगर (!as || !as->notifier) {
        वापस;
    }

    event.type = flag;
    event.entry.perm = perm;
    event.entry.target_as = s->target_as;

    IOMMU_NOTIFIER_FOREACH(n, &as->iova_mr) {
        अगर (!pv || n->iommu_idx == pid) {
            event.entry.iova = n->start;
            event.entry.addr_mask = n->end - n->start;
            trace_fn(as->iova_mr.parent_obj.name);
            memory_region_notify_iommu_one(n, &event);
        }
    }
}

अटल व्योम riscv_iommu_ats_inval(RISCVIOMMUState *s,
    काष्ठा riscv_iommu_command *cmd)
{
    वापस riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_DEVIOTLB_UNMAP, IOMMU_NONE,
                           trace_riscv_iommu_ats_inval);
}

अटल व्योम riscv_iommu_ats_prgr(RISCVIOMMUState *s,
    काष्ठा riscv_iommu_command *cmd)
{
    अचिन्हित resp_code = get_field(cmd->dword1,
                                   RISCV_IOMMU_CMD_ATS_PRGR_RESP_CODE);

    /* Using the access flag to carry response code information */
    IOMMUAccessFlags perm = resp_code ? IOMMU_NONE : IOMMU_RW;
    वापस riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_MAP, perm,
                           trace_riscv_iommu_ats_prgr);
}

अटल व्योम riscv_iommu_process_ddtp(RISCVIOMMUState *s)
{
    uint64_t old_ddtp = s->ddtp;
    uint64_t new_ddtp = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_DDTP);
    अचिन्हित new_mode = get_field(new_ddtp, RISCV_IOMMU_DDTP_MODE);
    अचिन्हित old_mode = get_field(old_ddtp, RISCV_IOMMU_DDTP_MODE);
    bool ok = false;

    /*
     * Check for allowed DDTP.MODE transitions:
     * {OFF, BARE}        -> {OFF, BARE, 1LVL, 2LVL, 3LVL}
     * {1LVL, 2LVL, 3LVL} -> {OFF, BARE}
     */
    अगर (new_mode == old_mode ||
        new_mode == RISCV_IOMMU_DDTP_MODE_OFF ||
        new_mode == RISCV_IOMMU_DDTP_MODE_BARE) {
        ok = true;
    } अन्यथा अगर (new_mode == RISCV_IOMMU_DDTP_MODE_1LVL ||
               new_mode == RISCV_IOMMU_DDTP_MODE_2LVL ||
               new_mode == RISCV_IOMMU_DDTP_MODE_3LVL) {
        ok = old_mode == RISCV_IOMMU_DDTP_MODE_OFF ||
             old_mode == RISCV_IOMMU_DDTP_MODE_BARE;
    }

    अगर (ok) {
        /* clear reserved and busy bits, report back sanitized version */
        new_ddtp = set_field(new_ddtp & RISCV_IOMMU_DDTP_PPN,
                             RISCV_IOMMU_DDTP_MODE, new_mode);
    } अन्यथा {
        new_ddtp = old_ddtp;
    }
    s->ddtp = new_ddtp;

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, new_ddtp);
}

/* Command function and opcode field. */
#घोषणा RISCV_IOMMU_CMD(func, op) (((func) << 7) | (op))

अटल व्योम riscv_iommu_process_cq_पुच्छ(RISCVIOMMUState *s)
{
    काष्ठा riscv_iommu_command cmd;
    Mem_Tx_परिणाम res;
    dma_addr_t addr;
    uint32_t पुच्छ, शीर्ष, ctrl;
    uint64_t cmd_opcode;
    GHFunc func;

    ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    पुच्छ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQT) & s->cq_mask;
    शीर्ष = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQH) & s->cq_mask;

    /* Check for pending error or queue processing disabled */
    अगर (!(ctrl & RISCV_IOMMU_CQCSR_CQON) ||
        !!(ctrl & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        वापस;
    }

    जबतक (पुच्छ != शीर्ष) {
        addr = s->cq_addr  + शीर्ष * माप(cmd);
        res = dma_memory_read(s->target_as, addr, &cmd, माप(cmd),
                              MEMTXATTRS_UNSPECIFIED);

        अगर (res != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                  RISCV_IOMMU_CQCSR_CQMF, 0);
            जाओ fault;
        }

        trace_riscv_iommu_cmd(s->parent_obj.id, cmd.dword0, cmd.dword1);

        cmd_opcode = get_field(cmd.dword0,
                               RISCV_IOMMU_CMD_OPCODE | RISCV_IOMMU_CMD_FUNC);

        चयन (cmd_opcode) {
        हाल RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOFENCE_FUNC_C,
                             RISCV_IOMMU_CMD_IOFENCE_OPCODE):
            अगर (cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_RESERVED) {
                जाओ cmd_ill;
            }

            res = riscv_iommu_iofence(s,
                cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_AV, cmd.dword1 << 2,
                get_field(cmd.dword0, RISCV_IOMMU_CMD_IOFENCE_तथ्य));

            अगर (res != MEMTX_OK) {
                riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                      RISCV_IOMMU_CQCSR_CQMF, 0);
                जाओ fault;
            }
            अवरोध;

        हाल RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA,
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

            अगर (pscv) {
                /* illegal command arguments IOTINVAL.GVMA & PSCV == 1 */
                जाओ cmd_ill;
            }

            func = riscv_iommu_iot_inval_all;

            अगर (gv) {
                func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                              riscv_iommu_iot_inval_gscid;
            }

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_TAG_VG);

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_TAG_VN);
            अवरोध;
        }

        हाल RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA,
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

            अगर (gv) {
                transtag = RISCV_IOMMU_TRANS_TAG_VN;
                अगर (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_gscid_pscid_iova :
                                  riscv_iommu_iot_inval_gscid_pscid;
                } अन्यथा {
                    func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                                  riscv_iommu_iot_inval_gscid;
                }
            } अन्यथा {
                transtag = RISCV_IOMMU_TRANS_TAG_SS;
                अगर (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_pscid_iova :
                                  riscv_iommu_iot_inval_pscid;
                } अन्यथा {
                    func = (av) ? riscv_iommu_iot_inval_iova :
                                  riscv_iommu_iot_inval_all;
                }
            }

            riscv_iommu_iot_inval(s, func, gscid, pscid, iova, transtag);
            अवरोध;
        }

        हाल RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            अगर (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* invalidate all device context cache mappings */
                func = riscv_iommu_ctx_inval_all;
            } अन्यथा {
                /* invalidate all device context matching DID */
                func = riscv_iommu_ctx_inval_devid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_field(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID), 0);
            अवरोध;

        हाल RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            अगर (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* illegal command arguments IODIR_PDT & DV == 0 */
                जाओ cmd_ill;
            } अन्यथा {
                func = riscv_iommu_ctx_inval_devid_procid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_field(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID),
                get_field(cmd.dword0, RISCV_IOMMU_CMD_IODIR_PID));
            अवरोध;

        /* ATS commands */
        हाल RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_INVAL,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            अगर (!s->enable_ats) {
                जाओ cmd_ill;
            }

            riscv_iommu_ats_inval(s, &cmd);
            अवरोध;

        हाल RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_PRGR,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            अगर (!s->enable_ats) {
                जाओ cmd_ill;
            }

            riscv_iommu_ats_prgr(s, &cmd);
            अवरोध;

        शेष:
        cmd_ill:
            /* Invalid instruction, do not advance instruction index. */
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                RISCV_IOMMU_CQCSR_CMD_ILL, 0);
            जाओ fault;
        }

        /* Advance and update head pointer after command completes. */
        शीर्ष = (शीर्ष + 1) & s->cq_mask;
        riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_CQH, शीर्ष);
    }
    वापस;

fault:
    अगर (ctrl & RISCV_IOMMU_CQCSR_CIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_CQ);
    }
}

अटल व्योम riscv_iommu_process_cq_control(RISCVIOMMUState *s)
{
    uint64_t base;
    uint32_t ctrl_set = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_set & RISCV_IOMMU_CQCSR_CQEN);
    bool active = !!(ctrl_set & RISCV_IOMMU_CQCSR_CQON);

    अगर (enable && !active) {
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
    } अन्यथा अगर (!enable && active) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~0);
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQON;
    } अन्यथा {
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, ctrl_set, ctrl_clr);

    /*
     * After clearing error bits (CMD_ILL, CQMF), if queue is still active,
     * re-process pending command.
     */
    ctrl_set = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    अगर ((ctrl_set & RISCV_IOMMU_CQCSR_CQON) &&
        !(ctrl_set & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        riscv_iommu_process_cq_पुच्छ(s);
    }
}

अटल व्योम riscv_iommu_process_fq_control(RISCVIOMMUState *s)
{
    uint64_t base;
    uint32_t ctrl_set = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_set & RISCV_IOMMU_FQCSR_FQEN);
    bool active = !!(ctrl_set & RISCV_IOMMU_FQCSR_FQON);

    अगर (enable && !active) {
        base = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_FQB);
        s->fq_mask = (2ULL << get_field(base, RISCV_IOMMU_FQB_LOG2SZ)) - 1;
        s->fq_addr = PPN_PHYS(get_field(base, RISCV_IOMMU_FQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~s->fq_mask);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQT], 0);
        ctrl_set = RISCV_IOMMU_FQCSR_FQON;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQMF |
            RISCV_IOMMU_FQCSR_FQOF;
    } अन्यथा अगर (!enable && active) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~0);
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQON;
    } अन्यथा {
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, ctrl_set, ctrl_clr);
}

अटल व्योम riscv_iommu_process_pq_control(RISCVIOMMUState *s)
{
    uint64_t base;
    uint32_t ctrl_set = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_set & RISCV_IOMMU_PQCSR_PQEN);
    bool active = !!(ctrl_set & RISCV_IOMMU_PQCSR_PQON);

    अगर (enable && !active) {
        base = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_PQB);
        s->pq_mask = (2ULL << get_field(base, RISCV_IOMMU_PQB_LOG2SZ)) - 1;
        s->pq_addr = PPN_PHYS(get_field(base, RISCV_IOMMU_PQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~s->pq_mask);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQT], 0);
        ctrl_set = RISCV_IOMMU_PQCSR_PQON;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQMF |
            RISCV_IOMMU_PQCSR_PQOF;
    } अन्यथा अगर (!enable && active) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~0);
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQON;
    } अन्यथा {
        ctrl_set = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, ctrl_set, ctrl_clr);
}

अटल व्योम riscv_iommu_process_dbg(RISCVIOMMUState *s)
{
    uint64_t iova = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_IOVA);
    uint64_t ctrl = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_CTL);
    अचिन्हित devid = get_field(ctrl, RISCV_IOMMU_TR_REQ_CTL_DID);
    अचिन्हित pid = get_field(ctrl, RISCV_IOMMU_TR_REQ_CTL_PID);
    IOMMUAccessFlags perm = ctrl & RISCV_IOMMU_TR_REQ_CTL_NW
                            ? IOMMU_RO : IOMMU_RW;
    RISCVIOMMUContext *ctx;
    व्योम *ref;

    अगर (!(ctrl & RISCV_IOMMU_TR_REQ_CTL_GO_BUSY)) {
        वापस;
    }

    ctx = riscv_iommu_ctx(s, devid, pid, perm, iova, &ref);
    अगर (ctx == शून्य) {
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE,
                                 RISCV_IOMMU_TR_RESPONSE_FAULT |
                                 (RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED << 10));
    } अन्यथा {
        IOMMUTLBEntry iotlb = {
            .iova = iova,
            .perm = perm,
            .addr_mask = ~0,
            .target_as = शून्य,
        };
        पूर्णांक fault = riscv_iommu_translate(s, ctx, &iotlb, false);
        अगर (fault) {
            iova = RISCV_IOMMU_TR_RESPONSE_FAULT | (((uint64_t) fault) << 10);
        } अन्यथा {
            iova = iotlb.translated_addr & ~iotlb.addr_mask;
            iova = set_field(0, RISCV_IOMMU_TR_RESPONSE_PPN, PPN_DOWN(iova));
        }
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE, iova);
    }

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
        RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    riscv_iommu_ctx_put(s, ref);
}

प्रकार व्योम riscv_iommu_process_fn(RISCVIOMMUState *s);

अटल व्योम riscv_iommu_update_icvec(RISCVIOMMUState *s, uint64_t तथ्य)
{
    uint64_t icvec = 0;

    icvec |= MIN(तथ्य & RISCV_IOMMU_ICVEC_CIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_CIV);

    icvec |= MIN(तथ्य & RISCV_IOMMU_ICVEC_FIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_FIV);

    icvec |= MIN(तथ्य & RISCV_IOMMU_ICVEC_PMIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PMIV);

    icvec |= MIN(तथ्य & RISCV_IOMMU_ICVEC_PIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PIV);

    trace_riscv_iommu_icvec_write(तथ्य, icvec);

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_ICVEC, icvec);
}

अटल व्योम riscv_iommu_update_ipsr(RISCVIOMMUState *s, uint64_t तथ्य)
{
    uint32_t cqcsr, fqcsr, pqcsr;
    uint32_t ipsr_set = 0;
    uint32_t ipsr_clr = 0;

    अगर (तथ्य & RISCV_IOMMU_IPSR_CIP) {
        cqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);

        अगर (cqcsr & RISCV_IOMMU_CQCSR_CIE &&
            (cqcsr & RISCV_IOMMU_CQCSR_FENCE_W_IP ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ILL ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_TO ||
             cqcsr & RISCV_IOMMU_CQCSR_CQMF)) {
            ipsr_set |= RISCV_IOMMU_IPSR_CIP;
        } अन्यथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
        }
    } अन्यथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
    }

    अगर (तथ्य & RISCV_IOMMU_IPSR_FIP) {
        fqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);

        अगर (fqcsr & RISCV_IOMMU_FQCSR_FIE &&
            (fqcsr & RISCV_IOMMU_FQCSR_FQOF ||
             fqcsr & RISCV_IOMMU_FQCSR_FQMF)) {
            ipsr_set |= RISCV_IOMMU_IPSR_FIP;
        } अन्यथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
        }
    } अन्यथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
    }

    अगर (!(तथ्य & RISCV_IOMMU_IPSR_PMIP)) {
        ipsr_clr |= RISCV_IOMMU_IPSR_PMIP;
    }

    अगर (तथ्य & RISCV_IOMMU_IPSR_PIP) {
        pqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);

        अगर (pqcsr & RISCV_IOMMU_PQCSR_PIE &&
            (pqcsr & RISCV_IOMMU_PQCSR_PQOF ||
             pqcsr & RISCV_IOMMU_PQCSR_PQMF)) {
            ipsr_set |= RISCV_IOMMU_IPSR_PIP;
        } अन्यथा {
            ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
        }
    } अन्यथा {
        ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, ipsr_set, ipsr_clr);
}

अटल व्योम riscv_iommu_process_hpm_writes(RISCVIOMMUState *s,
                                           uint32_t regb,
                                           bool पूर्व_cy_inh)
{
    चयन (regb) {
    हाल RISCV_IOMMU_REG_IOCOUNTINH:
        riscv_iommu_process_iocntinh_cy(s, पूर्व_cy_inh);
        अवरोध;

    हाल RISCV_IOMMU_REG_IOHPMCYCLES:
    हाल RISCV_IOMMU_REG_IOHPMCYCLES + 4:
        riscv_iommu_process_hpmcycle_write(s);
        अवरोध;

    हाल RISCV_IOMMU_REG_IOHPMEVT_BASE ...
        RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_अंक) + 4:
        riscv_iommu_process_hpmevt_write(s, regb & ~7);
        अवरोध;
    }
}

/*
 * Write the resulting value of 'data' for the reg specified
 * by 'reg_addr', after considering read-only/read-write/write-clear
 * bits, in the pointer 'dest'.
 *
 * The result is written in little-endian.
 */
अटल व्योम riscv_iommu_write_reg_val(RISCVIOMMUState *s,
                                      व्योम *dest, hwaddr reg_addr,
                                      पूर्णांक आकार, uint64_t तथ्य)
{
    uint64_t ro = ldn_le_p(&s->regs_ro[reg_addr], आकार);
    uint64_t wc = ldn_le_p(&s->regs_wc[reg_addr], आकार);
    uint64_t curr_val = ldn_le_p(&s->regs[reg_addr], आकार);

    stn_le_p(dest, आकार, ((curr_val & ro) | (तथ्य & ~ro)) & ~(तथ्य & wc));
}

अटल Mem_Tx_परिणाम riscv_iommu_mmio_write(व्योम *opaque, hwaddr addr,
                                          uint64_t तथ्य, अचिन्हित आकार,
                                          MemTxAttrs attrs)
{
    riscv_iommu_process_fn *process_fn = शून्य;
    RISCVIOMMUState *s = opaque;
    uint32_t regb = addr & ~3;
    uint32_t busy = 0;
    uint64_t val = 0;
    bool cy_inh = false;

    अगर ((addr & (आकार - 1)) != 0) {
        /* Unsupported MMIO alignment or access size */
        वापस MEMTX_ERROR;
    }

    अगर (addr + आकार > RISCV_IOMMU_REG_MSI_CONFIG) {
        /* Unsupported MMIO access location. */
        वापस MEMTX_ACCESS_ERROR;
    }

    /* Track actionable MMIO write. */
    चयन (regb) {
    हाल RISCV_IOMMU_REG_DDTP:
    हाल RISCV_IOMMU_REG_DDTP + 4:
        process_fn = riscv_iommu_process_ddtp;
        regb = RISCV_IOMMU_REG_DDTP;
        busy = RISCV_IOMMU_DDTP_BUSY;
        अवरोध;

    हाल RISCV_IOMMU_REG_CQT:
        process_fn = riscv_iommu_process_cq_पुच्छ;
        अवरोध;

    हाल RISCV_IOMMU_REG_CQCSR:
        process_fn = riscv_iommu_process_cq_control;
        busy = RISCV_IOMMU_CQCSR_BUSY;
        अवरोध;

    हाल RISCV_IOMMU_REG_FQCSR:
        process_fn = riscv_iommu_process_fq_control;
        busy = RISCV_IOMMU_FQCSR_BUSY;
        अवरोध;

    हाल RISCV_IOMMU_REG_PQCSR:
        process_fn = riscv_iommu_process_pq_control;
        busy = RISCV_IOMMU_PQCSR_BUSY;
        अवरोध;

    हाल RISCV_IOMMU_REG_ICVEC:
    हाल RISCV_IOMMU_REG_IPSR:
        /*
         * ICVEC and IPSR have special read/write procedures. We'll
         * call their respective helpers and exit.
         */
        riscv_iommu_write_reg_val(s, &val, addr, आकार, तथ्य);

        /*
         * 'val' is stored as LE. Switch to host endianess
         * before using it.
         */
        val = le64_to_cpu(val);

        अगर (regb == RISCV_IOMMU_REG_ICVEC) {
            riscv_iommu_update_icvec(s, val);
        } अन्यथा {
            riscv_iommu_update_ipsr(s, val);
        }

        वापस MEMTX_OK;

    हाल RISCV_IOMMU_REG_TR_REQ_CTL:
        process_fn = riscv_iommu_process_dbg;
        regb = RISCV_IOMMU_REG_TR_REQ_CTL;
        busy = RISCV_IOMMU_TR_REQ_CTL_GO_BUSY;
        अवरोध;

    हाल RISCV_IOMMU_REG_IOCOUNTINH:
        अगर (addr != RISCV_IOMMU_REG_IOCOUNTINH) {
            अवरोध;
        }
        /* Store previous value of CY bit. */
        cy_inh = !!(riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTINH) &
            RISCV_IOMMU_IOCOUNTINH_CY);
        अवरोध;


    शेष:
        अवरोध;
    }

    /*
     * Registers update might be not synchronized with core logic.
     * If system software updates register when relevant BUSY bit
     * is set IOMMU behavior of additional writes to the register
     * is UNSPECIFIED.
     */
    riscv_iommu_write_reg_val(s, &s->regs[addr], addr, आकार, तथ्य);

    /* Busy flag update, MSB 4-byte register. */
    अगर (busy) {
        uint32_t rw = ldl_le_p(&s->regs[regb]);
        stl_le_p(&s->regs[regb], rw | busy);
    }

    /* Process HPM writes and update any internal state if needed. */
    अगर (regb >= RISCV_IOMMU_REG_IOCOUNTOVF &&
        regb <= (RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_अंक) + 4)) {
        riscv_iommu_process_hpm_writes(s, regb, cy_inh);
    }

    अगर (process_fn) {
        process_fn(s);
    }

    वापस MEMTX_OK;
}

अटल Mem_Tx_परिणाम riscv_iommu_mmio_read(व्योम *opaque, hwaddr addr,
    uint64_t *तथ्य, अचिन्हित आकार, MemTxAttrs attrs)
{
    RISCVIOMMUState *s = opaque;
    uint64_t val = -1;
    uint8_t *ptr;

    अगर ((addr & (आकार - 1)) != 0) {
        /* Unsupported MMIO alignment. */
        वापस MEMTX_ERROR;
    }

    अगर (addr + आकार > RISCV_IOMMU_REG_MSI_CONFIG) {
        वापस MEMTX_ACCESS_ERROR;
    }

    /* Compute cycle register value. */
    अगर ((addr & ~7) == RISCV_IOMMU_REG_IOHPMCYCLES) {
        val = riscv_iommu_hpmcycle_read(s);
        ptr = (uint8_t *)&val + (addr & 7);
    } अन्यथा अगर ((addr & ~3) == RISCV_IOMMU_REG_IOCOUNTOVF) {
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

    *तथ्य = val;

    वापस MEMTX_OK;
}

अटल स्थिर MemoryRegionOps riscv_iommu_mmio_ops = {
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
अटल Mem_Tx_परिणाम riscv_iommu_trap_write(व्योम *opaque, hwaddr addr,
    uint64_t तथ्य, अचिन्हित आकार, MemTxAttrs attrs)
{
    RISCVIOMMUState* s = (RISCVIOMMUState *)opaque;
    RISCVIOMMUContext *ctx;
    Mem_Tx_परिणाम res;
    व्योम *ref;
    uint32_t devid = attrs.requester_id;

    अगर (attrs.unspecified) {
        वापस MEMTX_ACCESS_ERROR;
    }

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= s->bus << 8;

    ctx = riscv_iommu_ctx(s, devid, 0, IOMMU_RW, addr, &ref);
    अगर (ctx == शून्य) {
        res = MEMTX_ACCESS_ERROR;
    } अन्यथा {
        res = riscv_iommu_msi_write(s, ctx, addr, तथ्य, आकार, attrs);
    }
    riscv_iommu_ctx_put(s, ref);
    वापस res;
}

अटल Mem_Tx_परिणाम riscv_iommu_trap_read(व्योम *opaque, hwaddr addr,
    uint64_t *तथ्य, अचिन्हित आकार, MemTxAttrs attrs)
{
    वापस MEMTX_ACCESS_ERROR;
}

अटल स्थिर MemoryRegionOps riscv_iommu_trap_ops = {
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

व्योम riscv_iommu_set_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_mode mode)
{
    s->cap = set_field(s->cap, RISCV_IOMMU_CAP_IGS, mode);
}

अटल व्योम riscv_iommu_instance_init(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    /* Enable translation debug interface */
    s->cap = RISCV_IOMMU_CAP_DBG;

    /* TODO: method to report supported PID bits */
    s->pid_bits = 8; /* restricted to size of MemTxAttrs.pid */
    s->cap |= RISCV_IOMMU_CAP_PD8;

    /* register storage */
    s->regs = g_new0(uint8_t, RISCV_IOMMU_REG_आकार);
    s->regs_ro = g_new0(uint8_t, RISCV_IOMMU_REG_आकार);
    s->regs_wc = g_new0(uint8_t, RISCV_IOMMU_REG_आकार);

     /* Mark all registers read-only */
    स_रखो(s->regs_ro, 0xff, RISCV_IOMMU_REG_आकार);

    /* Device translation context cache */
    s->ctx_cache = g_hash_table_new_full(riscv_iommu_ctx_hash,
                                         riscv_iommu_ctx_equal,
                                         g_free, शून्य);

    s->iot_cache = g_hash_table_new_full(riscv_iommu_iot_hash,
                                         riscv_iommu_iot_equal,
                                         g_free, शून्य);

    s->iommus.le_अग्र = शून्य;
    s->iommus.le_पूर्व = शून्य;
    QLIST_INIT(&s->spaces);
}

अटल व्योम riscv_iommu_instance_finalize(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    g_free(s->regs);
    g_free(s->regs_ro);
    g_free(s->regs_wc);

    g_hash_table_unref(s->ctx_cache);
    g_hash_table_unref(s->iot_cache);
}

अटल व्योम riscv_iommu_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    /* Report QEMU target physical address space limits. */
    s->cap = set_field(s->cap, RISCV_IOMMU_CAP_PAS, s->pas_bits);

    s->cap |= s->version & RISCV_IOMMU_CAP_VERSION;
    अगर (s->enable_msi) {
        s->cap |= RISCV_IOMMU_CAP_MSI_FLAT | RISCV_IOMMU_CAP_MSI_MRIF;
    }
    अगर (s->enable_ats) {
        s->cap |= RISCV_IOMMU_CAP_ATS;
    }
    अगर (s->enable_s_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32 | RISCV_IOMMU_CAP_SV39 |
                  RISCV_IOMMU_CAP_SV48 | RISCV_IOMMU_CAP_SV57;
    }
    अगर (s->enable_g_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32X4 | RISCV_IOMMU_CAP_SV39X4 |
                  RISCV_IOMMU_CAP_SV48X4 | RISCV_IOMMU_CAP_SV57X4 |
                  RISCV_IOMMU_CAP_SVRSW60T59B;
    }

    अगर (s->hpm_cntrs > 0) {
        /* Clip number of HPM counters to maximum supported (31). */
        अगर (s->hpm_cntrs > RISCV_IOMMU_IOCOUNT_अंक) {
            s->hpm_cntrs = RISCV_IOMMU_IOCOUNT_अंक;
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
        "riscv-iommu-regs", RISCV_IOMMU_REG_आकार);

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
    अगर (s->cap & RISCV_IOMMU_CAP_DBG) {
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_IOVA], 0);
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_CTL],
            RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    }

    /* If HPM registers are enabled. */
    अगर (s->cap & RISCV_IOMMU_CAP_HPM) {
        /* +1 for cycle counter bit. */
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOCOUNTINH],
                 ~((2 << s->hpm_cntrs) - 1));
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCYCLES], 0);
        स_रखो(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCTR_BASE],
               0x00, s->hpm_cntrs * 8);
        स_रखो(&s->regs_ro[RISCV_IOMMU_REG_IOHPMEVT_BASE],
               0x00, s->hpm_cntrs * 8);
    }

    /* Memory region for downstream access, if specified. */
    अगर (s->target_mr) {
        s->target_as = g_new0(AddressSpace, 1);
        address_space_init(s->target_as, s->target_mr,
            "riscv-iommu-downstream");
    } अन्यथा {
        /* Fallback to global system memory. */
        s->target_as = &address_space_memory;
    }

    /* Memory region for untranslated MRIF/MSI writes */
    memory_region_init_io(&s->trap_mr, OBJECT(dev), &riscv_iommu_trap_ops, s,
            "riscv-iommu-trap", ~0ULL);
    address_space_init(&s->trap_as, &s->trap_mr, "riscv-iommu-trap-as");

    अगर (s->cap & RISCV_IOMMU_CAP_HPM) {
        s->hpm_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL, riscv_iommu_hpm_timer_cb, s);
        s->hpm_event_ctr_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
}

अटल व्योम riscv_iommu_unrealize(DeviceState *dev)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    अगर (s->cap & RISCV_IOMMU_CAP_HPM) {
        g_hash_table_unref(s->hpm_event_ctr_map);
        timer_free(s->hpm_timer);
    }
}

व्योम riscv_iommu_reset(RISCVIOMMUState *s)
{
    uint32_t reg_clr;
    पूर्णांक ddtp_mode;

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

अटल स्थिर Property riscv_iommu_properties[] = {
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
                      RISCV_IOMMU_IOCOUNT_अंक),
};

अटल व्योम riscv_iommu_class_init(ObjectClass *klass, स्थिर व्योम *तथ्य)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    /* internal device for riscv-iommu-{pci/sys}, not user-creatable */
    dc->user_creatable = false;
    dc->realize = riscv_iommu_realize;
    dc->unrealize = riscv_iommu_unrealize;
    device_class_set_props(dc, riscv_iommu_properties);
}

अटल स्थिर TypeInfo riscv_iommu_info = {
    .name = TYPE_RISCV_IOMMU,
    .parent = TYPE_DEVICE,
    .instance_आकार = माप(RISCVIOMMUState),
    .instance_init = riscv_iommu_instance_init,
    .instance_finalize = riscv_iommu_instance_finalize,
    .class_init = riscv_iommu_class_init,
};

अटल स्थिर अक्षर *IOMMU_FLAG_STR[] = {
    "NA",
    "RO",
    "WR",
    "RW",
};

/* RISC-V IOMMU Memory Region - Address Translation Space */
अटल IOMMUTLBEntry riscv_iommu_memory_region_translate(
    IOMMUMemoryRegion *iommu_mr, hwaddr addr,
    IOMMUAccessFlags flag, पूर्णांक iommu_idx)
{
    RISCVIOMMUSpace *as = container_of(iommu_mr, RISCVIOMMUSpace, iova_mr);
    RISCVIOMMUContext *ctx;
    व्योम *ref;
    IOMMUTLBEntry iotlb = {
        .iova = addr,
        .target_as = as->iommu->target_as,
        .addr_mask = ~0ULL,
        .perm = flag,
    };
    uint32_t devid = riscv_iommu_space_devid(as);

    ctx = riscv_iommu_ctx(as->iommu, devid, iommu_idx, flag, addr, &ref);
    अगर (ctx == शून्य) {
        /* Translation disabled or invalid. */
        iotlb.addr_mask = 0;
        iotlb.perm = IOMMU_NONE;
    } अन्यथा अगर (riscv_iommu_translate(as->iommu, ctx, &iotlb, true)) {
        /* Translation disabled or fault reported. */
        iotlb.addr_mask = 0;
        iotlb.perm = IOMMU_NONE;
    }

    /* Trace all dma translations with original access flags. */
    trace_riscv_iommu_dma(as->iommu->parent_obj.id, PCI_BUS_अंक(devid),
                          PCI_SLOT(devid), PCI_FUNC(devid), iommu_idx,
                          IOMMU_FLAG_STR[flag & IOMMU_RW], iotlb.iova,
                          iotlb.translated_addr);

    riscv_iommu_ctx_put(as->iommu, ref);

    वापस iotlb;
}

अटल पूर्णांक riscv_iommu_memory_region_notify(
    IOMMUMemoryRegion *iommu_mr, IOMMUNotifierFlag old,
    IOMMUNotifierFlag new, Error **errp)
{
    RISCVIOMMUSpace *as = container_of(iommu_mr, RISCVIOMMUSpace, iova_mr);

    अगर (old == IOMMU_NOTIFIER_NONE) {
        as->notifier = true;
        trace_riscv_iommu_notifier_add(iommu_mr->parent_obj.name);
    } अन्यथा अगर (new == IOMMU_NOTIFIER_NONE) {
        as->notifier = false;
        trace_riscv_iommu_notifier_del(iommu_mr->parent_obj.name);
    }

    वापस 0;
}

अटल अंतरभूत bool pci_is_iommu(PCIDevice *pdev)
{
    वापस pci_get_word(pdev->config + PCI_CLASS_DEVICE) == 0x0806;
}

अटल AddressSpace *riscv_iommu_find_as(PCIBus *bus, व्योम *opaque, पूर्णांक devfn)
{
    RISCVIOMMUState *s = (RISCVIOMMUState *) opaque;
    PCIDevice *pdev = pci_find_device(bus, pci_bus_अंक(bus), devfn);
    AddressSpace *as = शून्य;

    अगर (pdev && pci_is_iommu(pdev)) {
        वापस s->target_as;
    }

    /* Find first registered IOMMU device */
    जबतक (s->iommus.le_पूर्व) {
        s = *(s->iommus.le_पूर्व);
    }

    /* Find first matching IOMMU */
    जबतक (s != शून्य && as == शून्य) {
        as = riscv_iommu_space(s, bus, devfn);
        s = s->iommus.le_अग्र;
    }

    वापस as ? as : &address_space_memory;
}

अटल स्थिर PCIIOMMUOps riscv_iommu_ops = {
    .get_address_space = riscv_iommu_find_as,
};

व्योम riscv_iommu_pci_setup_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
        Error **errp)
{
    अगर (bus->iommu_ops &&
        bus->iommu_ops->get_address_space == riscv_iommu_find_as) {
        /* Allow multiple IOMMUs on the same PCIe bus, link known devices */
        RISCVIOMMUState *last = (RISCVIOMMUState *)bus->iommu_opaque;
        QLIST_जोड़ो_AFTER(last, iommu, iommus);
    } अन्यथा अगर (!bus->iommu_ops && !bus->iommu_opaque) {
        pci_setup_iommu(bus, &riscv_iommu_ops, iommu);
    } अन्यथा {
        error_setg(errp, "can't register secondary IOMMU for PCI bus #%d",
            pci_bus_अंक(bus));
    }
}

अटल पूर्णांक riscv_iommu_memory_region_सूचकांक(IOMMUMemoryRegion *iommu_mr,
    MemTxAttrs attrs)
{
    वापस attrs.unspecified ? RISCV_IOMMU_NOPROCID : (पूर्णांक)attrs.pid;
}

अटल पूर्णांक riscv_iommu_memory_region_सूचकांक_len(IOMMUMemoryRegion *iommu_mr)
{
    RISCVIOMMUSpace *as = container_of(iommu_mr, RISCVIOMMUSpace, iova_mr);
    वापस 1 << as->iommu->pid_bits;
}

अटल व्योम riscv_iommu_memory_region_init(ObjectClass *klass, स्थिर व्योम *तथ्य)
{
    IOMMUMemoryRegionClass *imrc = IOMMU_MEMORY_REGION_CLASS(klass);

    imrc->translate = riscv_iommu_memory_region_translate;
    imrc->notify_flag_changed = riscv_iommu_memory_region_notify;
    imrc->attrs_to_सूचकांक = riscv_iommu_memory_region_सूचकांक;
    imrc->अंक_indexes = riscv_iommu_memory_region_सूचकांक_len;
}

अटल स्थिर TypeInfo riscv_iommu_memory_region_info = {
    .parent = TYPE_IOMMU_MEMORY_REGION,
    .name = TYPE_RISCV_IOMMU_MEMORY_REGION,
    .class_init = riscv_iommu_memory_region_init,
};

अटल व्योम riscv_iommu_register_mr_types(व्योम)
{
    type_register_static(&riscv_iommu_memory_region_info);
    type_register_static(&riscv_iommu_info);
}

type_init(riscv_iommu_register_mr_types);
