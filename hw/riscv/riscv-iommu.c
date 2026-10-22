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

#شامل "qemu/osdep.h"
#شامل "qom/object.h"
#شامل "exec/target_page.h"
#شامل "hw/pci/pci_bus.h"
#شامل "hw/pci/pci_device.h"
#شامل "hw/core/qdev-properties.h"
#شامل "hw/riscv/riscv_hart.h"
#شامل "migration/vmstate.h"
#شامل "qapi/error.h"
#شامل "qemu/timer.h"
#شامل "qemu/target-info.h"
#شامل "qemu/bitops.h"

#شامل "cpu_bits.h"
#شامل "riscv-iommu.h"
#شامل "riscv-iommu-bits.h"
#شامل "riscv-iommu-hpm.h"
#شامل "trace.h"

#تعریف_کرو حد_بندی_CACHE_CTX               (1U << 7)
#تعریف_کرو حد_بندی_CACHE_IOT               (1U << 20)

/* Physical page number coversions */
#تعریف_کرو PPN_PHYS(ppn)                 ((ppn) << ہدف_صفحہ_بٹ)
#تعریف_کرو PPN_نیچے_کو(phy)                 ((phy) >> ہدف_صفحہ_بٹ)

قسمتعریف ساخت RISCVIOMMUEntry RISCVIOMMUEntry;

/* Device assigned I/O address space */
ساخت RISCVIOMMUSpace {
    IOMMUMemoryRegion iova_mr;  /* IOVA memory region for attached device */
    پتہ_Space iova_as;       /* IOVA address space for attached device */
    RISCVIOMMUState *iommu;     /* Managing IOMMU device state */
    PCIBus *bus;                /* PCI bus of the requester */
    uint8_t devfn;              /* Requester identifier, AKA device_id */
    bool notifier;              /* IOMMU unmap notifier enabled */
    QLIST_اندراج(RISCVIOMMUSpace) list;
};

قسمتعریف شمار RISCVIOMMUTrans_لیبل {
    RISCV_IOMMU_TRANS_لیبل_BY,  /* Bypass */
    RISCV_IOMMU_TRANS_لیبل_SS,  /* Single Stage */
    RISCV_IOMMU_TRANS_لیبل_VG,  /* G-stage only */
    RISCV_IOMMU_TRANS_لیبل_VN,  /* Nested translation */
} RISCVIOMMUTrans_لیبل;

/* Address translation cache entry */
ساخت RISCVIOMMUEntry {
    RISCVIOMMUTrans_لیبل لیبل;     /* Translation Tag */
    uint64_t iova:44;           /* IOVA Page Number */
    uint64_t pscid:20;          /* Process Soft-Context identifier */
    uint64_t phys:44;           /* Physical Page Number */
    uint64_t gscid:16;          /* Guest Soft-Context identifier */
    uint64_t perm:2;            /* IOMMU_RW flags */
};

/* IOMMU index for transactions without process_id specified. */
#تعریف_کرو RISCV_IOMMU_NOPROCID 0

ساکن uint32_t riscv_iommu_space_devid(RISCVIOMMUSpace *as)
{
    uint32_t devid = PCI_BUILD_BDF(pci_bus_عدد(as->bus), as->devfn);

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= as->iommu->bus << 8;
    واپس devid;
}

ساکن uint8_t riscv_iommu_get_icvec_سمتیہ(uint32_t icvec, uint32_t vec_type)
{
    انتخاب (vec_type) {
    حالت RISCV_IOMMU_INTR_CQ:
        واپس icvec & RISCV_IOMMU_ICVEC_CIV;
    حالت RISCV_IOMMU_INTR_FQ:
        واپس (icvec & RISCV_IOMMU_ICVEC_FIV) >> 4;
    حالت RISCV_IOMMU_INTR_PM:
        واپس (icvec & RISCV_IOMMU_ICVEC_PMIV) >> 8;
    حالت RISCV_IOMMU_INTR_PQ:
        واپس (icvec & RISCV_IOMMU_ICVEC_PIV) >> 12;
    طےشدہ:
        g_assert_not_reached();
    }
}

خالی riscv_iommu_notify(RISCVIOMMUState *s, صحیح vec_type)
{
    uint32_t ipsr, icvec, سمتیہ;

    اگر (!s->notify) {
        واپس;
    }

    icvec = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_ICVEC);
    ipsr = riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, (1 << vec_type), 0);

    اگر (!(ipsr & (1 << vec_type))) {
        سمتیہ = riscv_iommu_get_icvec_سمتیہ(icvec, vec_type);
        s->notify(s, سمتیہ);
        trace_riscv_iommu_notify_int_سمتیہ(vec_type, سمتیہ);
    }
}

خالی riscv_iommu_fault(RISCVIOMMUState *s, ساخت riscv_iommu_fq_ریکارڈ *ev)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t سر = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQH) & s->fq_نقاب;
    uint32_t دم = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQT) & s->fq_نقاب;
    uint32_t اگلا = (دم + 1) & s->fq_نقاب;
    uint32_t devid = get_میدان(ev->hdr, RISCV_IOMMU_FQ_HDR_DID);

    trace_riscv_iommu_flt(s->والد_obj.id, PCI_BUS_عدد(devid), PCI_خانہ(devid),
                          PCI_FUNC(devid), ev->hdr, ev->iotval);

    اگر (!(ctrl & RISCV_IOMMU_FQCSR_FQON) ||
        !!(ctrl & (RISCV_IOMMU_FQCSR_FQOF | RISCV_IOMMU_FQCSR_FQMF))) {
        واپس;
    }

    اگر (سر == اگلا) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                              RISCV_IOMMU_FQCSR_FQOF, 0);
    } ورنہ {
        dma_addr_t addr = s->fq_addr + دم * حجم(*ev);
        اگر (dma_حافظہ_write(s->ہدف_as, addr, ev, حجم(*ev),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                                  RISCV_IOMMU_FQCSR_FQMF, 0);
        } ورنہ {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_FQT, اگلا);
        }
    }

    اگر (ctrl & RISCV_IOMMU_FQCSR_FIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_FQ);
    }
}

ساکن خالی riscv_iommu_pri(RISCVIOMMUState *s,
    ساخت riscv_iommu_pq_ریکارڈ *pr)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t سر = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQH) & s->pq_نقاب;
    uint32_t دم = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQT) & s->pq_نقاب;
    uint32_t اگلا = (دم + 1) & s->pq_نقاب;
    uint32_t devid = get_میدان(pr->hdr, RISCV_IOMMU_PREQ_HDR_DID);

    trace_riscv_iommu_pri(s->والد_obj.id, PCI_BUS_عدد(devid), PCI_خانہ(devid),
                          PCI_FUNC(devid), pr->payload);

    اگر (!(ctrl & RISCV_IOMMU_PQCSR_PQON) ||
        !!(ctrl & (RISCV_IOMMU_PQCSR_PQOF | RISCV_IOMMU_PQCSR_PQMF))) {
        واپس;
    }

    اگر (سر == اگلا) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                              RISCV_IOMMU_PQCSR_PQOF, 0);
    } ورنہ {
        dma_addr_t addr = s->pq_addr + دم * حجم(*pr);
        اگر (dma_حافظہ_write(s->ہدف_as, addr, pr, حجم(*pr),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                                  RISCV_IOMMU_PQCSR_PQMF, 0);
        } ورنہ {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_PQT, اگلا);
        }
    }

    اگر (ctrl & RISCV_IOMMU_PQCSR_PIE) {
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
ساکن uint64_t riscv_iommu_pext_u64(uint64_t val, uint64_t ext)
{
    uint64_t ret = 0;
    uint64_t rot = 1;

    جب (ext) {
        اگر (ext & 1) {
            اگر (val & 1) {
                ret |= rot;
            }
            rot <<= 1;
        }
        val >>= 1;
        ext >>= 1;
    }

    واپس ret;
}

/* Check if GPA matches MSI/MRIF pattern. */
ساکن bool riscv_iommu_msi_check(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    dma_addr_t gpa)
{
    اگر (!s->enable_msi) {
        واپس false;
    }

    اگر (get_میدان(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_منوال) !=
        RISCV_IOMMU_DC_MSIPTP_منوال_FLAT) {
        واپس false; /* Invalid MSI/MRIF mode */
    }

    اگر ((PPN_نیچے_کو(gpa) ^ ctx->msi_addr_نمونہ) & ~ctx->msi_addr_نقاب) {
        واپس false; /* GPA not in MSI range defined by AIA IMSIC rules. */
    }

    واپس true;
}

/* Returns the NAPOT page mask, or 0 for reserved encodings. */
ساکن hwaddr riscv_iommu_napot_صفحہ_نقاب(hwaddr ppn, hwaddr addr, hwaddr *برآمد)
{
    صحیح napot_بٹ = ctz64(ppn) + 1;
    hwaddr napot_نقاب, صفحہ_نقاب;

    /* The spec only defines 64KB (napot_bits == 4) */
    اگر (napot_بٹ != 4) {
        واپس 0;
    }

    napot_نقاب = (1ULL << napot_بٹ) - 1;
    صفحہ_نقاب = PPN_PHYS(napot_نقاب) | (ہدف_صفحہ_سائز - 1);

    *برآمد = PPN_PHYS(ppn & ~napot_نقاب) | (addr & صفحہ_نقاب);

    واپس صفحہ_نقاب;
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
ساکن صحیح riscv_iommu_spa_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb)
{
    IOMMUAccessFlags trans_perm = IOMMU_NONE;
    dma_addr_t addr, بنیاد;
    uint64_t satp, gatp, pte;
    bool en_s, en_g;
    ساخت {
        غیرعلامتی حرف قدم;
        غیرعلامتی حرف سطحیں;
        غیرعلامتی حرف ptidxbits;
        غیرعلامتی حرف ptesize;
    } sc[2];
    /* Translation stage phase */
    شمار {
        S_STAGE = 0,
        G_STAGE = 1,
    } pass;
    Mem_Tx_نتیجہ ret;
    bool pv = !!ctx->process_id;
    /*
     * Keep the request permission separate from iotlb->perm.  G-stage
     * walks translate S-stage PTE addresses before the real leaf is
     * reached, but permission checks and fault types must still use the
     * original request.  A successful walk leaves iotlb->perm with the
     * effective leaf permission for the translation cache.
     */
    ثابت IOMMUAccessFlags req_perm = iotlb->perm;

    satp = get_میدان(ctx->satp, RISCV_IOMMU_ATP_منوال_میدان);
    gatp = get_میدان(ctx->gatp, RISCV_IOMMU_ATP_منوال_میدان);

    en_s = satp != RISCV_IOMMU_DC_FSC_منوال_BARE;
    en_g = gatp != RISCV_IOMMU_DC_IOHGATP_منوال_BARE;

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
    اگر (!en_s && (req_perm & IOMMU_WO) &&
        riscv_iommu_msi_check(s, ctx, iotlb->iova)) {
        iotlb->ہدف_as = &s->trap_as;
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_نقاب = ~ہدف_صفحہ_نقاب;
        واپس 0;
    }

    /* Exit early for pass-through mode. */
    اگر (!(en_s || en_g)) {
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_نقاب = ~ہدف_صفحہ_نقاب;
        /* Allow R/W in pass-through mode */
        iotlb->perm = IOMMU_RW;
        واپس 0;
    }

    /* S/G translation parameters. */
    برائے (pass = 0; pass < 2; pass++) {
        uint32_t sv_منوال;

        sc[pass].قدم = 0;
        اگر (pass ? (s->fctl & RISCV_IOMMU_FCTL_GXL) :
            (ctx->tc & RISCV_IOMMU_DC_TC_SXL)) {
            /* 32bit mode for GXL/SXL == 1 */
            انتخاب (pass ? gatp : satp) {
            حالت RISCV_IOMMU_DC_IOHGATP_منوال_BARE:
                sc[pass].سطحیں    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                توڑو;
            حالت RISCV_IOMMU_DC_IOHGATP_منوال_SV32_X4:
                sv_منوال = pass ? RISCV_IOMMU_CAP_SV32X4 : RISCV_IOMMU_CAP_SV32;
                اگر (!(s->cap & sv_منوال)) {
                    واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].سطحیں    = 2;
                sc[pass].ptidxbits = 10;
                sc[pass].ptesize   = 4;
                توڑو;
            طےشدہ:
                واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        } ورنہ {
            /* 64bit mode for GXL/SXL == 0 */
            انتخاب (pass ? gatp : satp) {
            حالت RISCV_IOMMU_DC_IOHGATP_منوال_BARE:
                sc[pass].سطحیں    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                توڑو;
            حالت RISCV_IOMMU_DC_IOHGATP_منوال_SV39_X4:
                sv_منوال = pass ? RISCV_IOMMU_CAP_SV39X4 : RISCV_IOMMU_CAP_SV39;
                اگر (!(s->cap & sv_منوال)) {
                    واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].سطحیں    = 3;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                توڑو;
            حالت RISCV_IOMMU_DC_IOHGATP_منوال_SV48_X4:
                sv_منوال = pass ? RISCV_IOMMU_CAP_SV48X4 : RISCV_IOMMU_CAP_SV48;
                اگر (!(s->cap & sv_منوال)) {
                    واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].سطحیں    = 4;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                توڑو;
            حالت RISCV_IOMMU_DC_IOHGATP_منوال_SV57_X4:
                sv_منوال = pass ? RISCV_IOMMU_CAP_SV57X4 : RISCV_IOMMU_CAP_SV57;
                اگر (!(s->cap & sv_منوال)) {
                    واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].سطحیں    = 5;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                توڑو;
            طےشدہ:
                واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        }
    };

    /* S/G stages translation tables root pointers */
    gatp = PPN_PHYS(get_میدان(ctx->gatp, RISCV_IOMMU_ATP_PPN_میدان));
    satp = PPN_PHYS(get_میدان(ctx->satp, RISCV_IOMMU_ATP_PPN_میدان));
    addr = (en_s && en_g) ? satp : iotlb->iova;
    بنیاد = en_g ? gatp : satp;
    pass = en_g ? G_STAGE : S_STAGE;

    کرو {
        ثابت غیرعلامتی widened = (pass && !sc[pass].قدم) ? 2 : 0;
        ثابت غیرعلامتی va_بٹ = widened + sc[pass].ptidxbits;
        ثابت غیرعلامتی va_skip = ہدف_صفحہ_بٹ + sc[pass].ptidxbits *
                                 (sc[pass].سطحیں - 1 - sc[pass].قدم);
        ثابت غیرعلامتی idx = (addr >> va_skip) & ((1 << va_بٹ) - 1);
        ثابت dma_addr_t pte_addr = بنیاد + idx * sc[pass].ptesize;
        ثابت bool ade =
            ctx->tc & (pass ? RISCV_IOMMU_DC_TC_GADE : RISCV_IOMMU_DC_TC_SADE);

        اگر (ade && !(s->cap & RISCV_IOMMU_CAP_AMO_HWAD)) {
            /* GADE/SADE are reserved bits if AMO_HWAD is cleared.  */
            واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }

        /* Address range check before first level lookup */
        اگر (!sc[pass].قدم) {
            ثابت uint64_t va_len = va_skip + va_بٹ;
            ثابت uint64_t va_نقاب = (1ULL << va_len) - 1;

            اگر (pass == S_STAGE && va_len > 32) {
                uint64_t نقاب, masked_msbs;

                نقاب = MAKE_64_ثنائی_ہندسہ_نقاب(0, ہدف_long_بٹ() - va_len + 1);
                masked_msbs = (addr >> (va_len - 1)) & نقاب;

                اگر (masked_msbs != 0 && masked_msbs != نقاب) {
                    واپس (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S;
                }
            } ورنہ {
                اگر ((addr & va_نقاب) != addr) {
                    واپس (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS;
                }
            }
        }


        اگر (pass == S_STAGE) {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_S_VS_WALKS);
        } ورنہ {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_G_WALKS);
        }

        /* Read page table entry */
        اگر (sc[pass].ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->ہدف_as, pte_addr, &pte32,
                             MEMTXATTRS_UNSPECIFIED);
            pte = pte32;
        } ورنہ {
            ret = ldq_le_dma(s->ہدف_as, pte_addr, &pte,
                             MEMTXATTRS_UNSPECIFIED);
        }
        اگر (ret != MEMTX_OK) {
            واپس (req_perm & IOMMU_WO) ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT
                                         : RISCV_IOMMU_FQ_CAUSE_RD_FAULT;
        }

        sc[pass].قدم++;
        hwaddr ppn = pte >> PTE_PPN_سرکاؤ;

        اگر (!(pte & PTE_V)) {
            توڑو;                /* Invalid PTE */
        } ورنہ اگر (pte & PTE_RESERVED(false)) {
            توڑو;                /* Reserved PTE bits set */
        } ورنہ اگر (!(pte & (PTE_R | PTE_W | PTE_X))) {
            بنیاد = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } ورنہ اگر ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            توڑو;                /* Reserved leaf PTE flags: PTE_W */
        } ورنہ اگر ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            توڑو;                /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } ورنہ اگر (ppn & ((1ULL << (va_skip - ہدف_صفحہ_بٹ)) - 1)) {
            توڑو;                /* Misaligned PPN */
        } ورنہ اگر (!(pte & PTE_U) && !pv) {
            /*
             * All accesses are assumed to be User mode unless
             * process_id is valid (pv).  In case we have a
             * non-user mode leaf PTE and !pv we need to fault.
             */
            توڑو;
        } ورنہ اگر ((req_perm & IOMMU_RO) && !(pte & PTE_R)) {
            توڑو;                /* Read access check failed */
        } ورنہ اگر ((req_perm & IOMMU_WO) && !(pte & PTE_W)) {
            توڑو;                /* Write access check failed */
        } ورنہ اگر (!ade && !(pte & PTE_A)) {
            توڑو;                /* Access bit not set */
        } ورنہ اگر ((req_perm & IOMMU_WO) && !ade && !(pte & PTE_D)) {
            توڑو;                /* Dirty bit not set */
        } ورنہ اگر (pass == G_STAGE && !(pte & PTE_U)) {
            /*
             * riscv-iommu spec 1.0: "When checking the U bit in a
             * second-stage PTE, the transaction is treated as
             * not requesting supervisor privilege."
             *
             * I.e. we need to fault if this is a non-user PTE since
             * we are always in user mode at this point.
             */
            توڑو;
        } ورنہ {
            /* Leaf PTE, translation completed. */
            sc[pass].قدم = sc[pass].سطحیں;

            اگر (pte & PTE_N) {
                hwaddr نقاب = riscv_iommu_napot_صفحہ_نقاب(ppn, addr, &بنیاد);

                اگر (!نقاب) {
                    توڑو;
                }
                iotlb->addr_نقاب &= نقاب;
            } ورنہ {
                بنیاد = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
                /* Update address mask based on smallest translation granularity */
                iotlb->addr_نقاب &= (1ULL << va_skip) - 1;
            }

            /* Continue with S-Stage translation? */
            اگر (pass && sc[0].قدم != sc[0].سطحیں) {
                pass = S_STAGE;
                addr = iotlb->iova;
                جاری;
            }

            /* Cache the effective permission, not this request's subset. */
            IOMMUAccessFlags برگ_perm = (pte & PTE_W) ?
                                         ((pte & PTE_R) ? IOMMU_RW : IOMMU_WO) :
                                         IOMMU_RO;

            trans_perm = trans_perm == IOMMU_NONE ?
                         برگ_perm : trans_perm & برگ_perm;

            /* Translation phase completed (GPA or SPA) */
            iotlb->translated_addr = بنیاد;

            /* Check MSI GPA address match */
            اگر (pass == S_STAGE && (req_perm & IOMMU_WO) &&
                riscv_iommu_msi_check(s, ctx, بنیاد)) {
                /* Trap MSI writes and return GPA address. */
                iotlb->ہدف_as = &s->trap_as;
                iotlb->addr_نقاب = ~ہدف_صفحہ_نقاب;
                واپس 0;
            }

            /* Continue with G-Stage translation? */
            اگر (!pass && en_g) {
                pass = G_STAGE;
                addr = بنیاد;
                بنیاد = gatp;
                sc[pass].قدم = 0;
                جاری;
            }

            iotlb->perm = trans_perm;
            واپس 0;
        }

        اگر (sc[pass].قدم == sc[pass].سطحیں) {
            توڑو; /* Can't find leaf PTE */
        }

        /* Continue with G-Stage translation? */
        اگر (!pass && en_g) {
            pass = G_STAGE;
            addr = بنیاد;
            بنیاد = gatp;
            sc[pass].قدم = 0;
        }
    } جب (1);

    /*
     * riscv_iommu_translate() will receive a fault and then call
     * riscv_iommu_report_fault() using iotlb->translated_addr
     * as iotval2.  Update translated_addr it with the latest
     * translated addr we have.
     */
    iotlb->translated_addr = addr;

    واپس (req_perm & IOMMU_WO) ?
                (pass ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S) :
                (pass ? RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S);
}

ساکن خالی riscv_iommu_report_fault(RISCVIOMMUState *s,
                                     RISCVIOMMUContext *ctx,
                                     uint32_t fault_type, uint32_t cause,
                                     bool pv,
                                     uint64_t iotval, uint64_t iotval2)
{
    ساخت riscv_iommu_fq_ریکارڈ ev = { 0 };

    اگر (ctx->tc & RISCV_IOMMU_DC_TC_DTF) {
        انتخاب (cause) {
        حالت RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED:
        حالت RISCV_IOMMU_FQ_CAUSE_DDT_بھرو_FAULT:
        حالت RISCV_IOMMU_FQ_CAUSE_DDT_INVALID:
        حالت RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED:
        حالت RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED:
        حالت RISCV_IOMMU_FQ_CAUSE_اندرونی_DP_ERROR:
        حالت RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT:
            توڑو;
        طےشدہ:
            /* DTF prevents reporting a fault for this given cause */
            واپس;
        }
    }

    ev.hdr = سیٹ_میدان(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE, cause);
    ev.hdr = سیٹ_میدان(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE, fault_type);
    ev.hdr = سیٹ_میدان(ev.hdr, RISCV_IOMMU_FQ_HDR_DID, ctx->devid);
    ev.hdr = سیٹ_میدان(ev.hdr, RISCV_IOMMU_FQ_HDR_PV, pv);

    اگر (pv) {
        ev.hdr = سیٹ_میدان(ev.hdr, RISCV_IOMMU_FQ_HDR_PID, ctx->process_id);
    }

    ev.iotval = iotval;
    ev.iotval2 = iotval2;

    riscv_iommu_fault(s, &ev);
}

/* Redirect MSI write for given GPA. */
ساکن Mem_Tx_نتیجہ riscv_iommu_msi_write(RISCVIOMMUState *s,
    RISCVIOMMUContext *ctx, uint64_t gpa, uint64_t ڈیٹا,
    غیرعلامتی سائز, MemTxAttrs attrs)
{
    Mem_Tx_نتیجہ res;
    dma_addr_t addr;
    uint64_t intn;
    سائز_t انحراف;
    uint32_t n190;
    uint64_t pte[2];
    صحیح fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    صحیح cause;

    /* Interrupt File Number */
    intn = riscv_iommu_pext_u64(PPN_نیچے_کو(gpa), ctx->msi_addr_نقاب);
    انحراف = intn * حجم(pte);

    /* fetch MSI PTE */
    addr = PPN_PHYS(get_میدان(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_PPN));
    اگر (addr & انحراف) {
        /* Interrupt file number out of range */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_بھرو_FAULT;
        جاؤ err;
    }

    addr |= انحراف;
    res = dma_حافظہ_read(s->ہدف_as, addr, &pte, حجم(pte),
            MEMTXATTRS_UNSPECIFIED);
    اگر (res != MEMTX_OK) {
        اگر (res == MEMTX_DECODE_ERROR) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED;
        } ورنہ {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_بھرو_FAULT;
        }
        جاؤ err;
    }

    le64_مقصد_cpus(&pte[0]);
    le64_مقصد_cpus(&pte[1]);

    اگر (!(pte[0] & RISCV_IOMMU_MSI_PTE_V) || (pte[0] & RISCV_IOMMU_MSI_PTE_C)) {
        /*
         * The spec mentions that: "If msipte.C == 1, then further
         * processing to interpret the PTE is implementation
         * defined.". We'll abort with cause = 262 for this
         * case too.
         */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_INVALID;
        جاؤ err;
    }

    انتخاب (get_میدان(pte[0], RISCV_IOMMU_MSI_PTE_M)) {
    حالت RISCV_IOMMU_MSI_PTE_M_BASIC:
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
        اگر (pte[0] & (GENMASK_ULL(62, 54) | GENMASK_ULL(9, 3))) {
            res = MEMTX_DECODE_ERROR;
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
            جاؤ err;
        }

        /* MSI Pass-through mode */
        addr = PPN_PHYS(get_میدان(pte[0], RISCV_IOMMU_MSI_PTE_PPN));

        trace_riscv_iommu_msi(s->والد_obj.id, PCI_BUS_عدد(ctx->devid),
                              PCI_خانہ(ctx->devid), PCI_FUNC(ctx->devid),
                              gpa, addr);

        res = dma_حافظہ_write(s->ہدف_as, addr, &ڈیٹا, سائز, attrs);
        اگر (res != MEMTX_OK) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
            جاؤ err;
        }

        واپس MEMTX_OK;
    حالت RISCV_IOMMU_MSI_PTE_M_MRIF:
        /* MRIF mode, continue. */
        توڑو;
    طےشدہ:
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        جاؤ err;
    }

    /*
     * Report an error for interrupt identities exceeding the maximum allowed
     * for an IMSIC interrupt file (2047) or destination address is not 32-bit
     * aligned. See IOMMU Specification, Chapter 2.3. MSI page tables.
     */
    اگر ((ڈیٹا > 2047) || (gpa & 3)) {
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        جاؤ err;
    }

    /* MSI MRIF mode, non atomic pending bit update */

    /* MRIF pending bit address */
    addr = get_میدان(pte[0], RISCV_IOMMU_MSI_PTE_MRIF_ADDR) << 9;
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
    addr = addr | ((ڈیٹا & 0x7c0) >> 2);

    trace_riscv_iommu_msi(s->والد_obj.id, PCI_BUS_عدد(ctx->devid),
                          PCI_خانہ(ctx->devid), PCI_FUNC(ctx->devid),
                          gpa, addr);

    /* MRIF pending bit mask */
    ڈیٹا = 1ULL << (ڈیٹا & 0x03f);
    res = dma_حافظہ_read(s->ہدف_as, addr, &intn, حجم(intn), attrs);
    اگر (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_بھرو_FAULT;
        جاؤ err;
    }

    intn = intn | ڈیٹا;
    res = dma_حافظہ_write(s->ہدف_as, addr, &intn, حجم(intn), attrs);
    اگر (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        جاؤ err;
    }

    /* Get MRIF enable bits */
    addr = addr + حجم(intn);
    res = dma_حافظہ_read(s->ہدف_as, addr, &intn, حجم(intn), attrs);
    اگر (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_بھرو_FAULT;
        جاؤ err;
    }

    اگر (!(intn & ڈیٹا)) {
        /* notification disabled, MRIF update completed. */
        واپس MEMTX_OK;
    }

    /* Send notification message */
    addr = PPN_PHYS(get_میدان(pte[1], RISCV_IOMMU_MSI_MRIF_NPPN));
    n190 = get_میدان(pte[1], RISCV_IOMMU_MSI_MRIF_NID) |
          (get_میدان(pte[1], RISCV_IOMMU_MSI_MRIF_NID_MSB) << 10);

    res = dma_حافظہ_write(s->ہدف_as, addr, &n190, حجم(n190), attrs);
    اگر (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        جاؤ err;
    }

    trace_riscv_iommu_mrif_notification(s->والد_obj.id, n190, addr);

    واپس MEMTX_OK;

err:
    riscv_iommu_report_fault(s, ctx, fault_type, cause,
                             !!ctx->process_id, 0, 0);
    واپس res;
}

/*
 * Check device context configuration as described by the
 * riscv-iommu spec section "Device-context configuration
 * checks".
 */
ساکن bool riscv_iommu_validate_device_ctx(RISCVIOMMUState *s,
                                            RISCVIOMMUContext *ctx)
{
    uint32_t fsc_منوال, msi_منوال;
    uint64_t gatp;

    اگر (ctx->tc & RISCV_IOMMU_DC_TC_RESERVED) {
        واپس false;
    }

    اگر (!(s->cap & RISCV_IOMMU_CAP_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI ||
         ctx->tc & RISCV_IOMMU_DC_TC_PRPR)) {
        واپس false;
    }

    اگر (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI)) {
        واپس false;
    }

    اگر (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI) &&
        ctx->tc & RISCV_IOMMU_DC_TC_PRPR) {
        واپس false;
    }

    اگر (!(s->cap & RISCV_IOMMU_CAP_T2GPA) &&
        ctx->tc & RISCV_IOMMU_DC_TC_T2GPA) {
        واپس false;
    }

    اگر (s->cap & RISCV_IOMMU_CAP_MSI_FLAT) {
        msi_منوال = get_میدان(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_منوال);

        اگر (msi_منوال != RISCV_IOMMU_DC_MSIPTP_منوال_OFF &&
            msi_منوال != RISCV_IOMMU_DC_MSIPTP_منوال_FLAT) {
            واپس false;
        }
    }

    gatp = get_میدان(ctx->gatp, RISCV_IOMMU_ATP_منوال_میدان);
    اگر (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA &&
        gatp == RISCV_IOMMU_DC_IOHGATP_منوال_BARE) {
        واپس false;
    }

    اگر (gatp != RISCV_IOMMU_DC_IOHGATP_منوال_BARE) {
        uint64_t iohgatp_ppn = get_میدان(ctx->gatp,
                                         RISCV_IOMMU_DC_IOHGATP_PPN);

        /*
         * One of the conditions for a misconfigured DDT entry
         * according to the riscv-spec: "DC.iohgatp.MODE is not
         * Bare and the root page table (address) determined by
         * DC.iohgatp.PPN is not aligned to a 16-KiB boundary."
         */
        اگر (PPN_PHYS(iohgatp_ppn) & ((1ULL << 14) - 1)) {
            واپس false;
        }
    }

    fsc_منوال = get_میدان(ctx->satp, RISCV_IOMMU_DC_FSC_منوال);

    اگر (ctx->tc & RISCV_IOMMU_DC_TC_PDTV) {
        انتخاب (fsc_منوال) {
        حالت RISCV_IOMMU_DC_FSC_PDTP_منوال_PD8:
            اگر (!(s->cap & RISCV_IOMMU_CAP_PD8)) {
                واپس false;
            }
            توڑو;
        حالت RISCV_IOMMU_DC_FSC_PDTP_منوال_PD17:
            اگر (!(s->cap & RISCV_IOMMU_CAP_PD17)) {
                واپس false;
            }
            توڑو;
        حالت RISCV_IOMMU_DC_FSC_PDTP_منوال_PD20:
            اگر (!(s->cap & RISCV_IOMMU_CAP_PD20)) {
                واپس false;
            }
            توڑو;
        }
    } ورنہ {
        /* DC.tc.PDTV is 0 */
        اگر (ctx->tc & RISCV_IOMMU_DC_TC_DPE) {
            واپس false;
        }

        اگر (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
            اگر (fsc_منوال == RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV32 &&
                !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                واپس false;
            }
        } ورنہ {
            انتخاب (fsc_منوال) {
            حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV39:
                اگر (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                    واپس false;
                }
                توڑو;
            حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV48:
                اگر (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                    واپس false;
                }
            توڑو;
            حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV57:
                اگر (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                    واپس false;
                }
                توڑو;
            }
        }
    }

    /*
     * CAP_END is always zero (only one endianess). FCTL_BE is
     * always zero (little-endian accesses). Thus TC_SBE must
     * always be LE, i.e. zero.
     */
    اگر (ctx->tc & RISCV_IOMMU_DC_TC_SBE) {
        واپس false;
    }

    واپس true;
}

/*
 * Validate process context (PC) according to section
 * "Process-context configuration checks".
 */
ساکن bool riscv_iommu_validate_process_ctx(RISCVIOMMUState *s,
                                             RISCVIOMMUContext *ctx)
{
    uint32_t منوال;

    اگر (get_میدان(ctx->ta, RISCV_IOMMU_PC_TA_RESERVED)) {
        واپس false;
    }

    اگر (get_میدان(ctx->satp, RISCV_IOMMU_PC_FSC_RESERVED)) {
        واپس false;
    }

    منوال = get_میدان(ctx->satp, RISCV_IOMMU_DC_FSC_منوال);
    انتخاب (منوال) {
    حالت RISCV_IOMMU_DC_FSC_منوال_BARE:
    /* sv39 and sv32 modes have the same value (8) */
    حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV39:
    حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV48:
    حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV57:
        توڑو;
    طےشدہ:
        واپس false;
    }

    اگر (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
        اگر (منوال == RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV32 &&
            !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                واپس false;
        }
    } ورنہ {
        انتخاب (منوال) {
        حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV39:
            اگر (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                واپس false;
            }
            توڑو;
        حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV48:
            اگر (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                واپس false;
            }
            توڑو;
        حالت RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV57:
            اگر (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                واپس false;
            }
            توڑو;
        }
    }

    واپس true;
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
ساکن Mem_Tx_نتیجہ pdt_حافظہ_read(RISCVIOMMUState *s,
                                   RISCVIOMMUContext *ctx,
                                   dma_addr_t addr,
                                   خالی *buf, dma_addr_t len,
                                   MemTxAttrs attrs)
{
    uint64_t gatp_منوال, pte;
    ساخت {
        غیرعلامتی حرف قدم;
        غیرعلامتی حرف سطحیں;
        غیرعلامتی حرف ptidxbits;
        غیرعلامتی حرف ptesize;
    } sc;
    Mem_Tx_نتیجہ ret;
    dma_addr_t بنیاد = addr;

    /* G stages translation mode */
    gatp_منوال = get_میدان(ctx->gatp, RISCV_IOMMU_ATP_منوال_میدان);
    اگر (gatp_منوال == RISCV_IOMMU_DC_IOHGATP_منوال_BARE) {
        جاؤ برآمد;
    }

    /* G stages translation tables root pointer */
    بنیاد = PPN_PHYS(get_میدان(ctx->gatp, RISCV_IOMMU_ATP_PPN_میدان));

    /* Start at step 0 */
    sc.قدم = 0;

    اگر (s->fctl & RISCV_IOMMU_FCTL_GXL) {
        /* 32bit mode for GXL == 1 */
        انتخاب (gatp_منوال) {
        حالت RISCV_IOMMU_DC_IOHGATP_منوال_SV32_X4:
            اگر (!(s->cap & RISCV_IOMMU_CAP_SV32X4)) {
                واپس MEMTX_ACCESS_ERROR;
            }
            sc.سطحیں    = 2;
            sc.ptidxbits = 10;
            sc.ptesize   = 4;
            توڑو;
        طےشدہ:
            واپس MEMTX_ACCESS_ERROR;
        }
    } ورنہ {
        /* 64bit mode for GXL == 0 */
        انتخاب (gatp_منوال) {
        حالت RISCV_IOMMU_DC_IOHGATP_منوال_SV39_X4:
            اگر (!(s->cap & RISCV_IOMMU_CAP_SV39X4)) {
                واپس MEMTX_ACCESS_ERROR;
            }
            sc.سطحیں    = 3;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            توڑو;
        حالت RISCV_IOMMU_DC_IOHGATP_منوال_SV48_X4:
            اگر (!(s->cap & RISCV_IOMMU_CAP_SV48X4)) {
                واپس MEMTX_ACCESS_ERROR;
            }
            sc.سطحیں    = 4;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            توڑو;
        حالت RISCV_IOMMU_DC_IOHGATP_منوال_SV57_X4:
            اگر (!(s->cap & RISCV_IOMMU_CAP_SV57X4)) {
                واپس MEMTX_ACCESS_ERROR;
            }
            sc.سطحیں    = 5;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            توڑو;
        طےشدہ:
            واپس MEMTX_ACCESS_ERROR;
        }
    }

    کرو {
        ثابت غیرعلامتی va_بٹ = (sc.قدم ? 0 : 2) + sc.ptidxbits;
        ثابت غیرعلامتی va_skip = ہدف_صفحہ_بٹ + sc.ptidxbits *
                                 (sc.سطحیں - 1 - sc.قدم);
        ثابت غیرعلامتی idx = (addr >> va_skip) & ((1 << va_بٹ) - 1);
        ثابت dma_addr_t pte_addr = بنیاد + idx * sc.ptesize;

        /* Address range check before first level lookup */
        اگر (!sc.قدم) {
            ثابت uint64_t va_نقاب = (1ULL << (va_skip + va_بٹ)) - 1;
            اگر ((addr & va_نقاب) != addr) {
                واپس MEMTX_ACCESS_ERROR;
            }
        }

        /* Read page table entry */
        اگر (sc.ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->ہدف_as, pte_addr, &pte32, attrs);
            pte = pte32;
        } ورنہ {
            ret = ldq_le_dma(s->ہدف_as, pte_addr, &pte, attrs);
        }
        اگر (ret != MEMTX_OK) {
            واپس ret;
        }

        sc.قدم++;
        hwaddr ppn = pte >> PTE_PPN_سرکاؤ;

        اگر (!(pte & PTE_V)) {
            واپس MEMTX_ACCESS_ERROR; /* Invalid PTE */
        } ورنہ اگر (!(pte & (PTE_R | PTE_W | PTE_X))) {
            بنیاد = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } ورنہ اگر ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            واپس MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W */
        } ورنہ اگر ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            واپس MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } ورنہ اگر (ppn & ((1ULL << (va_skip - ہدف_صفحہ_بٹ)) - 1)) {
            واپس MEMTX_ACCESS_ERROR; /* Misaligned PPN */
        } ورنہ {
            /* Leaf PTE, translation completed. */
            اگر (pte & PTE_N) {
                اگر (!riscv_iommu_napot_صفحہ_نقاب(ppn, addr, &بنیاد)) {
                    واپس MEMTX_ACCESS_ERROR;
                }
            } ورنہ {
                بنیاد = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
            }
            توڑو;
        }

        اگر (sc.قدم == sc.سطحیں) {
            واپس MEMTX_ACCESS_ERROR; /* Can't find leaf PTE */
        }
    } جب (1);

برآمد:
    واپس dma_حافظہ_read(s->ہدف_as, بنیاد, buf, len, attrs);
}

/*
 * RISC-V IOMMU Device Context Loopkup - Device Directory Tree Walk
 *
 * @s         : IOMMU Device State
 * @ctx       : Device Translation Context with devid and process_id set.
 * @return    : success or fault code.
 */
ساکن صحیح riscv_iommu_ctx_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx)
{
    ثابت uint64_t ddtp = s->ddtp;
    غیرعلامتی منوال = get_میدان(ddtp, RISCV_IOMMU_DDTP_منوال);
    dma_addr_t addr = PPN_PHYS(get_میدان(ddtp, RISCV_IOMMU_DDTP_PPN));
    ساخت riscv_iommu_dc dc;
    /* Device Context format: 0: extended (64 bytes) | 1: base (32 bytes) */
    ثابت صحیح dc_fmt = !s->enable_msi;
    ثابت سائز_t dc_len = حجم(dc) >> dc_fmt;
    صحیح گہرائی;
    uint64_t de;

    انتخاب (منوال) {
    حالت RISCV_IOMMU_DDTP_منوال_OFF:
        واپس RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED;

    حالت RISCV_IOMMU_DDTP_منوال_BARE:
        /* mock up pass-through translation context */
        ctx->gatp = سیٹ_میدان(0, RISCV_IOMMU_ATP_منوال_میدان,
            RISCV_IOMMU_DC_IOHGATP_منوال_BARE);
        ctx->satp = سیٹ_میدان(0, RISCV_IOMMU_ATP_منوال_میدان,
            RISCV_IOMMU_DC_FSC_منوال_BARE);

        ctx->tc = RISCV_IOMMU_DC_TC_V;
        اگر (s->enable_ats) {
            ctx->tc |= RISCV_IOMMU_DC_TC_EN_ATS;
        }

        ctx->ta = 0;
        ctx->msiptp = 0;
        واپس 0;

    حالت RISCV_IOMMU_DDTP_منوال_1_LVL:
        گہرائی = 0;
        توڑو;

    حالت RISCV_IOMMU_DDTP_منوال_2_LVL:
        گہرائی = 1;
        توڑو;

    حالت RISCV_IOMMU_DDTP_منوال_3_LVL:
        گہرائی = 2;
        توڑو;

    طےشدہ:
        واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /*
     * Check supported device id width (in bits).
     * See IOMMU Specification, Chapter 6. Software guidelines.
     * - if extended device-context format is used:
     *   1LVL: 6, 2LVL: 15, 3LVL: 24
     * - if base device-context format is used:
     *   1LVL: 7, 2LVL: 16, 3LVL: 24
     */
    اگر (ctx->devid >= (1 << (گہرائی * 9 + 6 + (dc_fmt && گہرائی != 2)))) {
        واپس RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
    }

    /* Device directory tree walk */
    برائے (; گہرائی-- > 0; ) {
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
        ثابت صحیح بانٹو = گہرائی * 9 + 6 + dc_fmt;
        addr |= ((ctx->devid >> بانٹو) << 3) & ~ہدف_صفحہ_نقاب;
        اگر (dma_حافظہ_read(s->ہدف_as, addr, &de, حجم(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            واپس RISCV_IOMMU_FQ_CAUSE_DDT_بھرو_FAULT;
        }
        le64_مقصد_cpus(&de);
        اگر (!(de & RISCV_IOMMU_DDTE_VALID)) {
            /* invalid directory entry */
            واپس RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        اگر (de & ~(RISCV_IOMMU_DDTE_PPN | RISCV_IOMMU_DDTE_VALID)) {
            /* reserved bits set */
            واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }
        addr = PPN_PHYS(get_میدان(de, RISCV_IOMMU_DDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);

    /* index into device context entry page */
    addr |= (ctx->devid * dc_len) & ~ہدف_صفحہ_نقاب;

    memset(&dc, 0, حجم(dc));
    اگر (dma_حافظہ_read(s->ہدف_as, addr, &dc, dc_len,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        واپس RISCV_IOMMU_FQ_CAUSE_DDT_بھرو_FAULT;
    }

    /* Set translation context. */
    ctx->tc = le64_مقصد_cpu(dc.tc);
    ctx->gatp = le64_مقصد_cpu(dc.iohgatp);
    ctx->satp = le64_مقصد_cpu(dc.fsc);
    ctx->ta = le64_مقصد_cpu(dc.ta);
    ctx->msiptp = le64_مقصد_cpu(dc.msiptp);
    ctx->msi_addr_نقاب = le64_مقصد_cpu(dc.msi_addr_نقاب);
    ctx->msi_addr_نمونہ = le64_مقصد_cpu(dc.msi_addr_نمونہ);

    اگر (!(ctx->tc & RISCV_IOMMU_DC_TC_V)) {
        واپس RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
    }

    اگر (!riscv_iommu_validate_device_ctx(s, ctx)) {
        واپس RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /* FSC field checks */
    منوال = get_میدان(ctx->satp, RISCV_IOMMU_DC_FSC_منوال);
    addr = PPN_PHYS(get_میدان(ctx->satp, RISCV_IOMMU_DC_FSC_PPN));

    اگر (!(ctx->tc & RISCV_IOMMU_DC_TC_PDTV)) {
        اگر (ctx->process_id != RISCV_IOMMU_NOPROCID) {
            /* PID is disabled */
            واپس RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
        }
        اگر (منوال > RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV57) {
            /* Invalid translation mode */
            واپس RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        واپس 0;
    }

    اگر (ctx->process_id == RISCV_IOMMU_NOPROCID) {
        اگر (!(ctx->tc & RISCV_IOMMU_DC_TC_DPE)) {
            /* No default process_id enabled, set BARE mode */
            ctx->satp = 0ULL;
            واپس 0;
        } ورنہ {
            /* Use default process_id #0 */
            ctx->process_id = 0;
        }
    }

    اگر (منوال == RISCV_IOMMU_DC_FSC_منوال_BARE) {
        /* No S-Stage translation, done. */
        واپس 0;
    }

    /* FSC.TC.PDTV enabled */
    اگر (منوال > RISCV_IOMMU_DC_FSC_PDTP_منوال_PD20) {
        /* Invalid PDTP.MODE */
        واپس RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    برائے (گہرائی = منوال - RISCV_IOMMU_DC_FSC_PDTP_منوال_PD8; گہرائی-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

        /*
         * Select process id index bits based on process directory tree
         * level. See IOMMU Specification, 2.2. Process-Directory-Table.
         */
        ثابت صحیح بانٹو = گہرائی * 9 + 8;
        addr |= ((ctx->process_id >> بانٹو) << 3) & ~ہدف_صفحہ_نقاب;
        اگر (pdt_حافظہ_read(s, ctx, addr, &de, حجم(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            واپس RISCV_IOMMU_FQ_CAUSE_PDT_بھرو_FAULT;
        }
        le64_مقصد_cpus(&de);
        اگر (!(de & RISCV_IOMMU_PDTE_VALID)) {
            واپس RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
        }
        addr = PPN_PHYS(get_میدان(de, RISCV_IOMMU_PDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

    /* Leaf entry in PDT */
    addr |= (ctx->process_id << 4) & ~ہدف_صفحہ_نقاب;
    اگر (pdt_حافظہ_read(s, ctx, addr, &dc.ta, حجم(uint64_t) * 2,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        واپس RISCV_IOMMU_FQ_CAUSE_PDT_بھرو_FAULT;
    }

    /* Use FSC and TA from process directory entry. */
    ctx->ta = le64_مقصد_cpu(dc.ta);
    ctx->satp = le64_مقصد_cpu(dc.fsc);

    اگر (!(ctx->ta & RISCV_IOMMU_PC_TA_V)) {
        واپس RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
    }

    اگر (!riscv_iommu_validate_process_ctx(s, ctx)) {
        واپس RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    واپس 0;
}

/* Translation Context cache support */
ساکن gboolean riscv_iommu_ctx_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUContext *c1 = (RISCVIOMMUContext *) v1;
    RISCVIOMMUContext *c2 = (RISCVIOMMUContext *) v2;
    واپس c1->devid == c2->devid &&
           c1->process_id == c2->process_id;
}

ساکن guint riscv_iommu_ctx_hash(gconstpointer v)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) v;
    /*
     * Generate simple hash of (process_id, devid)
     * assuming 24-bit wide devid.
     */
    واپس (guint)(ctx->devid) + ((guint)(ctx->process_id) << 24);
}

ساکن خالی riscv_iommu_ctx_inval_devid_procid(gpointer کلید, gpointer قدر,
                                               gpointer ڈیٹا)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) قدر;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) ڈیٹا;
    اگر (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid &&
        ctx->process_id == arg->process_id) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ساکن خالی riscv_iommu_ctx_inval_devid(gpointer کلید, gpointer قدر,
                                        gpointer ڈیٹا)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) قدر;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) ڈیٹا;
    اگر (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ساکن خالی riscv_iommu_ctx_inval_all(gpointer کلید, gpointer قدر,
                                      gpointer ڈیٹا)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) قدر;
    اگر (ctx->tc & RISCV_IOMMU_DC_TC_V) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ساکن خالی riscv_iommu_ctx_inval(RISCVIOMMUState *s, GHFunc func,
                                  uint32_t devid, uint32_t process_id)
{
    GHash_جدول *ctx_cache;
    RISCVIOMMUContext کلید = {
        .devid = devid,
        .process_id = process_id,
    };
    ctx_cache = g_hash_جدول_ref(s->ctx_cache);
    g_hash_جدول_foreach(ctx_cache, func, &کلید);
    g_hash_جدول_unref(ctx_cache);
}

/* Find or allocate translation context for a given {device_id, process_id} */
ساکن RISCVIOMMUContext *riscv_iommu_ctx(RISCVIOMMUState *s,
                                          غیرعلامتی devid, غیرعلامتی process_id,
                                          IOMMUAccessFlags perm, uint64_t iova,
                                          خالی **ref)
{
    GHash_جدول *ctx_cache;
    RISCVIOMMUContext *ctx;
    RISCVIOMMUContext کلید = {
        .devid = devid,
        .process_id = process_id,
    };
    غیرعلامتی منوال = get_میدان(s->ddtp, RISCV_IOMMU_DDTP_منوال);
    uint32_t fault_type;

    ctx_cache = g_hash_جدول_ref(s->ctx_cache);

    اگر (منوال != RISCV_IOMMU_DDTP_منوال_OFF &&
        منوال != RISCV_IOMMU_DDTP_منوال_BARE) {
        ctx = g_hash_جدول_lookup(ctx_cache, &کلید);

        اگر (ctx && (ctx->tc & RISCV_IOMMU_DC_TC_V)) {
            *ref = ctx_cache;
            واپس ctx;
        }
    }

    ctx = g_new0(RISCVIOMMUContext, 1);
    ctx->devid = devid;
    ctx->process_id = process_id;

    صحیح fault = riscv_iommu_ctx_fetch(s, ctx);
    اگر (!fault) {
        اگر (منوال != RISCV_IOMMU_DDTP_منوال_BARE) {
            اگر (g_hash_جدول_سائز(ctx_cache) >= حد_بندی_CACHE_CTX) {
                g_hash_جدول_unref(ctx_cache);
                ctx_cache = g_hash_جدول_new_بھرا(riscv_iommu_ctx_hash,
                                                  riscv_iommu_ctx_equal,
                                                  g_free, تہی);
                g_hash_جدول_ref(ctx_cache);
                g_hash_جدول_unref(qatomic_xchg(&s->ctx_cache, ctx_cache));
            }

            g_hash_جدول_جوڑو(ctx_cache, ctx);
            *ref = ctx_cache;
        } ورنہ {
            g_hash_جدول_unref(ctx_cache);
            /* Remember ctx so it can be freed */
            *ref = ctx;
        }
        واپس ctx;
    }

    g_hash_جدول_unref(ctx_cache);
    *ref = تہی;

    /*
     * TODO: (1) do we need to distinguish other fault types
     * for ctx fetching and (2) evaluate putting the 'fault_type'
     * logic inside riscv_iommu_report_fault() - there's at
     * least one other place (end of riscv_iommu_translate())
     * that does something similar.
     */
    اگر (perm & IOMMU_RO) {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
    } ورنہ {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    }

    riscv_iommu_report_fault(s, ctx, fault_type, fault,
                             !!process_id, iova, 0);

    g_free(ctx);
    واپس تہی;
}

ساکن خالی riscv_iommu_ctx_put(RISCVIOMMUState *s, خالی *ref)
{
    غیرعلامتی منوال = get_میدان(s->ddtp, RISCV_IOMMU_DDTP_منوال);

    اگر (!ref) {
        واپس;
    }

    /* ref is pointing to ctx in Bare mode. Bare mode ctx is not cached */
    اگر (منوال == RISCV_IOMMU_DDTP_منوال_BARE) {
        g_free(ref);
    } ورنہ {
        g_hash_جدول_unref((GHash_جدول *)ref);
    }
}

/* Find or allocate address space for a given device */
ساکن پتہ_Space *riscv_iommu_space(RISCVIOMMUState *s, PCIBus *bus,
                                            صحیح devfn)
{
    RISCVIOMMUSpace *as;

    QLIST_FOREACH(as, &s->spaces, list) {
        اگر (as->bus == bus && as->devfn == devfn) {
            توڑو;
        }
    }

    اگر (as == تہی) {
        حرف name[64];
        as = g_new0(RISCVIOMMUSpace, 1);

        as->iommu = s;
        as->bus = bus;
        as->devfn = devfn;

        snprintf(name, حجم(name), "riscv-iommu-%04x:%02x.%d-iova",
            pci_bus_عدد(bus), PCI_خانہ(devfn), PCI_FUNC(devfn));

        /* IOVA address space, untranslated addresses */
        حافظہ_region_init_iommu(&as->iova_mr, حجم(as->iova_mr),
            TYPE_RISCV_IOMMU_حافظہ_REGION,
            OBJECT(s), "riscv_iommu", UINT64_MAX);
        پتہ_space_init(&as->iova_as, حافظہ_REGION(&as->iova_mr), name);

        QLIST_داخل_کرو_سر(&s->spaces, as, list);

        trace_riscv_iommu_new(s->والد_obj.id, pci_bus_عدد(bus),
                PCI_خانہ(devfn), PCI_FUNC(devfn));
    }
    واپس &as->iova_as;
}

/* Translation Object cache support */
ساکن gboolean riscv_iommu_iot_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUEntry *t1 = (RISCVIOMMUEntry *) v1;
    RISCVIOMMUEntry *t2 = (RISCVIOMMUEntry *) v2;
    واپس t1->gscid == t2->gscid && t1->pscid == t2->pscid &&
           t1->iova == t2->iova && t1->لیبل == t2->لیبل;
}

ساکن guint riscv_iommu_iot_hash(gconstpointer v)
{
    RISCVIOMMUEntry *t = (RISCVIOMMUEntry *) v;
    واپس (guint)t->iova;
}

/* GV: 0 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 0 AV: 0 GVMA: 1 */
ساکن
خالی riscv_iommu_iot_inval_all(gpointer کلید, gpointer قدر, gpointer ڈیٹا)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) قدر;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ڈیٹا;
    اگر (iot->لیبل == arg->لیبل) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 0 PSCV: 1 GVMA: 0 */
ساکن
خالی riscv_iommu_iot_inval_pscid(gpointer کلید, gpointer قدر, gpointer ڈیٹا)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) قدر;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ڈیٹا;
    اگر (iot->لیبل == arg->لیبل &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 0 GVMA: 0 */
ساکن
خالی riscv_iommu_iot_inval_iova(gpointer کلید, gpointer قدر, gpointer ڈیٹا)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) قدر;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ڈیٹا;
    اگر (iot->لیبل == arg->لیبل &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 1 GVMA: 0 */
ساکن خالی riscv_iommu_iot_inval_pscid_iova(gpointer کلید, gpointer قدر,
                                             gpointer ڈیٹا)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) قدر;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ڈیٹا;
    اگر (iot->لیبل == arg->لیبل &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 0 GVMA: 1 */
ساکن
خالی riscv_iommu_iot_inval_gscid(gpointer کلید, gpointer قدر, gpointer ڈیٹا)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) قدر;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ڈیٹا;
    اگر (iot->لیبل == arg->لیبل &&
        iot->gscid == arg->gscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 1 GVMA: 0 */
ساکن خالی riscv_iommu_iot_inval_gscid_pscid(gpointer کلید, gpointer قدر,
                                              gpointer ڈیٹا)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) قدر;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ڈیٹا;
    اگر (iot->لیبل == arg->لیبل &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 1 GVMA: 1 */
ساکن خالی riscv_iommu_iot_inval_gscid_iova(gpointer کلید, gpointer قدر,
                                             gpointer ڈیٹا)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) قدر;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ڈیٹا;
    اگر (iot->لیبل == arg->لیبل &&
        iot->gscid == arg->gscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 1 GVMA: 0 */
ساکن خالی riscv_iommu_iot_inval_gscid_pscid_iova(gpointer کلید, gpointer قدر,
                                                   gpointer ڈیٹا)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) قدر;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ڈیٹا;
    اگر (iot->لیبل == arg->لیبل &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* caller should keep ref-count for iot_cache object */
ساکن RISCVIOMMUEntry *riscv_iommu_iot_lookup(RISCVIOMMUContext *ctx,
    GHash_جدول *iot_cache, hwaddr iova, RISCVIOMMUTrans_لیبل transtag)
{
    RISCVIOMMUEntry کلید = {
        .لیبل   = transtag,
        .gscid = get_میدان(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID),
        .pscid = get_میدان(ctx->ta, RISCV_IOMMU_DC_TA_PSCID),
        .iova  = PPN_نیچے_کو(iova),
    };
    واپس g_hash_جدول_lookup(iot_cache, &کلید);
}

/* caller should keep ref-count for iot_cache object */
ساکن خالی riscv_iommu_iot_update(RISCVIOMMUState *s,
    GHash_جدول *iot_cache, RISCVIOMMUEntry *iot)
{
    اگر (!s->iot_حد_بندی) {
        واپس;
    }

    اگر (g_hash_جدول_سائز(s->iot_cache) >= s->iot_حد_بندی) {
        iot_cache = g_hash_جدول_new_بھرا(riscv_iommu_iot_hash,
                                          riscv_iommu_iot_equal,
                                          g_free, تہی);
        g_hash_جدول_unref(qatomic_xchg(&s->iot_cache, iot_cache));
    }
    g_hash_جدول_جوڑو(iot_cache, iot);
}

ساکن خالی riscv_iommu_iot_inval(RISCVIOMMUState *s, GHFunc func,
    uint32_t gscid, uint32_t pscid, hwaddr iova, RISCVIOMMUTrans_لیبل transtag)
{
    GHash_جدول *iot_cache;
    RISCVIOMMUEntry کلید = {
        .لیبل = transtag,
        .gscid = gscid,
        .pscid = pscid,
        .iova  = PPN_نیچے_کو(iova),
    };

    iot_cache = g_hash_جدول_ref(s->iot_cache);
    g_hash_جدول_foreach(iot_cache, func, &کلید);
    g_hash_جدول_unref(iot_cache);
}

ساکن RISCVIOMMUTrans_لیبل riscv_iommu_get_transtag(RISCVIOMMUContext *ctx)
{
    uint64_t satp = get_میدان(ctx->satp, RISCV_IOMMU_ATP_منوال_میدان);
    uint64_t gatp = get_میدان(ctx->gatp, RISCV_IOMMU_ATP_منوال_میدان);

    اگر (satp == RISCV_IOMMU_DC_FSC_منوال_BARE) {
        واپس (gatp == RISCV_IOMMU_DC_IOHGATP_منوال_BARE) ?
            RISCV_IOMMU_TRANS_لیبل_BY : RISCV_IOMMU_TRANS_لیبل_VG;
    } ورنہ {
        واپس (gatp == RISCV_IOMMU_DC_IOHGATP_منوال_BARE) ?
            RISCV_IOMMU_TRANS_لیبل_SS : RISCV_IOMMU_TRANS_لیبل_VN;
    }
}

ساکن صحیح riscv_iommu_translate(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb, bool enable_cache)
{
    RISCVIOMMUTrans_لیبل transtag = riscv_iommu_get_transtag(ctx);
    RISCVIOMMUEntry *iot;
    IOMMUAccessFlags perm;
    bool enable_pid;
    bool enable_pri;
    GHash_جدول *iot_cache;
    صحیح fault;

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_URQ);

    iot_cache = g_hash_جدول_ref(s->iot_cache);
    enable_pri = (iotlb->perm == IOMMU_NONE) &&
                 (ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI);
    enable_pid = (ctx->tc & RISCV_IOMMU_DC_TC_PDTV);

    /* Check for ATS request. */
    اگر (iotlb->perm == IOMMU_NONE) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_ATS_RQ);
        /* Check if ATS is disabled. */
        اگر (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS)) {
            enable_pri = false;
            fault = RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
            جاؤ مکمل;
        }
    }

    iot = riscv_iommu_iot_lookup(ctx, iot_cache, iotlb->iova, transtag);
    perm = iot ? iot->perm : IOMMU_NONE;
    اگر (perm != IOMMU_NONE) {
        iotlb->translated_addr = PPN_PHYS(iot->phys);
        iotlb->addr_نقاب = ~ہدف_صفحہ_نقاب;
        iotlb->perm = perm;
        fault = 0;
        جاؤ مکمل;
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_TLB_ناکامی);

    /* Translate using device directory / page table information. */
    fault = riscv_iommu_spa_fetch(s, ctx, iotlb);

    اگر (!fault && iotlb->ہدف_as == &s->trap_as) {
        /* Do not cache trapped MSI translations */
        جاؤ مکمل;
    }

    /*
     * We made an implementation choice to not cache identity-mapped
     * translations, as allowed by the specification, to avoid
     * translation cache evictions for other devices sharing the
     * IOMMU hardware model.
     */
    اگر (!fault && iotlb->translated_addr != iotlb->iova && enable_cache) {
        iot = g_new0(RISCVIOMMUEntry, 1);
        iot->iova = PPN_نیچے_کو(iotlb->iova);
        iot->phys = PPN_نیچے_کو(iotlb->translated_addr);
        iot->gscid = get_میدان(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
        iot->pscid = get_میدان(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        iot->perm = iotlb->perm;
        iot->لیبل = transtag;
        riscv_iommu_iot_update(s, iot_cache, iot);
    }

مکمل:
    g_hash_جدول_unref(iot_cache);

    اگر (enable_pri && fault) {
        ساخت riscv_iommu_pq_ریکارڈ pr = {0};
        اگر (enable_pid) {
            pr.hdr = سیٹ_میدان(RISCV_IOMMU_PREQ_HDR_PV,
                               RISCV_IOMMU_PREQ_HDR_PID, ctx->process_id);
        }
        pr.hdr = سیٹ_میدان(pr.hdr, RISCV_IOMMU_PREQ_HDR_DID, ctx->devid);
        pr.payload = (iotlb->iova & ہدف_صفحہ_نقاب) |
                     RISCV_IOMMU_PREQ_PAYLOAD_M;
        riscv_iommu_pri(s, &pr);
        واپس fault;
    }

    اگر (fault) {
        غیرعلامتی ttype = RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ;

        اگر ((iotlb->perm & IOMMU_RW) == IOMMU_RW
            || iotlb->perm & IOMMU_WO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
        } ورنہ اگر (iotlb->perm & IOMMU_RO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
        }

        riscv_iommu_report_fault(s, ctx, ttype, fault, enable_pid,
                                 iotlb->iova, iotlb->translated_addr);
        واپس fault;
    }

    واپس 0;
}

/* IOMMU Command Interface */
ساکن Mem_Tx_نتیجہ riscv_iommu_iofence(RISCVIOMMUState *s, bool notify,
    uint64_t addr, uint32_t ڈیٹا)
{
    /*
     * ATS processing in this implementation of the IOMMU is synchronous,
     * no need to wait for completions here.
     */
    اگر (!notify) {
        واپس MEMTX_OK;
    }

    واپس dma_حافظہ_write(s->ہدف_as, addr, &ڈیٹا, حجم(ڈیٹا),
        MEMTXATTRS_UNSPECIFIED);
}

ساکن خالی riscv_iommu_ats(RISCVIOMMUState *s,
    ساخت riscv_iommu_command *cmd, IOMMUNotifierFlag flag,
    IOMMUAccessFlags perm,
    خالی (*trace_fn)(ثابت حرف *id))
{
    RISCVIOMMUSpace *as;
    IOMMUNotifier *n;
    IOMMUTLBEvent event;
    uint32_t pid;
    uint32_t devid;
    ثابت bool pv = cmd->dword0 & RISCV_IOMMU_CMD_ATS_PV;

    اگر (cmd->dword0 & RISCV_IOMMU_CMD_ATS_DSV) {
        /* Use device segment and requester id */
        devid = get_میدان(cmd->dword0,
            RISCV_IOMMU_CMD_ATS_DSEG | RISCV_IOMMU_CMD_ATS_RID);
    } ورنہ {
        devid = get_میدان(cmd->dword0, RISCV_IOMMU_CMD_ATS_RID);
    }

    pid = get_میدان(cmd->dword0, RISCV_IOMMU_CMD_ATS_PID);

    QLIST_FOREACH(as, &s->spaces, list) {
        اگر (riscv_iommu_space_devid(as) == devid) {
            توڑو;
        }
    }

    اگر (!as || !as->notifier) {
        واپس;
    }

    event.type = flag;
    event.اندراج_2.perm = perm;
    event.اندراج_2.ہدف_as = s->ہدف_as;

    IOMMU_NOTIFIER_FOREACH(n, &as->iova_mr) {
        اگر (!pv || n->iommu_idx == pid) {
            event.اندراج_2.iova = n->آغاز;
            event.اندراج_2.addr_نقاب = n->انتہا - n->آغاز;
            trace_fn(as->iova_mr.والد_obj.name);
            حافظہ_region_notify_iommu_one(n, &event);
        }
    }
}

ساکن خالی riscv_iommu_ats_inval(RISCVIOMMUState *s,
    ساخت riscv_iommu_command *cmd)
{
    واپس riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_DEVIOTLB_UNMAP, IOMMU_NONE,
                           trace_riscv_iommu_ats_inval);
}

ساکن خالی riscv_iommu_ats_prgr(RISCVIOMMUState *s,
    ساخت riscv_iommu_command *cmd)
{
    غیرعلامتی resp_رمز = get_میدان(cmd->dword1,
                                   RISCV_IOMMU_CMD_ATS_PRGR_RESP_رمز);

    /* Using the access flag to carry response code information */
    IOMMUAccessFlags perm = resp_رمز ? IOMMU_NONE : IOMMU_RW;
    واپس riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_MAP, perm,
                           trace_riscv_iommu_ats_prgr);
}

ساکن خالی riscv_iommu_process_ddtp(RISCVIOMMUState *s)
{
    uint64_t old_ddtp = s->ddtp;
    uint64_t new_ddtp = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_DDTP);
    غیرعلامتی new_منوال = get_میدان(new_ddtp, RISCV_IOMMU_DDTP_منوال);
    غیرعلامتی old_منوال = get_میدان(old_ddtp, RISCV_IOMMU_DDTP_منوال);
    bool ok = false;

    /*
     * Check for allowed DDTP.MODE transitions:
     * {OFF, BARE}        -> {OFF, BARE, 1LVL, 2LVL, 3LVL}
     * {1LVL, 2LVL, 3LVL} -> {OFF, BARE}
     */
    اگر (new_منوال == old_منوال ||
        new_منوال == RISCV_IOMMU_DDTP_منوال_OFF ||
        new_منوال == RISCV_IOMMU_DDTP_منوال_BARE) {
        ok = true;
    } ورنہ اگر (new_منوال == RISCV_IOMMU_DDTP_منوال_1_LVL ||
               new_منوال == RISCV_IOMMU_DDTP_منوال_2_LVL ||
               new_منوال == RISCV_IOMMU_DDTP_منوال_3_LVL) {
        ok = old_منوال == RISCV_IOMMU_DDTP_منوال_OFF ||
             old_منوال == RISCV_IOMMU_DDTP_منوال_BARE;
    }

    اگر (ok) {
        /* clear reserved and busy bits, report back sanitized version */
        new_ddtp = سیٹ_میدان(new_ddtp & RISCV_IOMMU_DDTP_PPN,
                             RISCV_IOMMU_DDTP_منوال, new_منوال);
    } ورنہ {
        new_ddtp = old_ddtp;
    }
    s->ddtp = new_ddtp;

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, new_ddtp);
}

/* Command function and opcode field. */
#تعریف_کرو RISCV_IOMMU_CMD(func, op) (((func) << 7) | (op))

ساکن خالی riscv_iommu_process_cq_دم(RISCVIOMMUState *s)
{
    ساخت riscv_iommu_command cmd;
    Mem_Tx_نتیجہ res;
    dma_addr_t addr;
    uint32_t دم, سر, ctrl;
    uint64_t cmd_opcode;
    GHFunc func;

    ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    دم = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQT) & s->cq_نقاب;
    سر = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQH) & s->cq_نقاب;

    /* Check for pending error or queue processing disabled */
    اگر (!(ctrl & RISCV_IOMMU_CQCSR_CQON) ||
        !!(ctrl & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        واپس;
    }

    جب (دم != سر) {
        addr = s->cq_addr  + سر * حجم(cmd);
        res = dma_حافظہ_read(s->ہدف_as, addr, &cmd, حجم(cmd),
                              MEMTXATTRS_UNSPECIFIED);

        اگر (res != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                  RISCV_IOMMU_CQCSR_CQMF, 0);
            جاؤ fault;
        }

        trace_riscv_iommu_cmd(s->والد_obj.id, cmd.dword0, cmd.dword1);

        cmd_opcode = get_میدان(cmd.dword0,
                               RISCV_IOMMU_CMD_OPCODE | RISCV_IOMMU_CMD_FUNC);

        انتخاب (cmd_opcode) {
        حالت RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOFENCE_FUNC_C,
                             RISCV_IOMMU_CMD_IOFENCE_OPCODE):
            اگر (cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_RESERVED) {
                جاؤ cmd_ill;
            }

            res = riscv_iommu_iofence(s,
                cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_AV, cmd.dword1 << 2,
                get_میدان(cmd.dword0, RISCV_IOMMU_CMD_IOFENCE_ڈیٹا));

            اگر (res != MEMTX_OK) {
                riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                      RISCV_IOMMU_CQCSR_CQMF, 0);
                جاؤ fault;
            }
            توڑو;

        حالت RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_میدان(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_میدان(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & ہدف_صفحہ_نقاب;

            اگر (pscv) {
                /* illegal command arguments IOTINVAL.GVMA & PSCV == 1 */
                جاؤ cmd_ill;
            }

            func = riscv_iommu_iot_inval_all;

            اگر (gv) {
                func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                              riscv_iommu_iot_inval_gscid;
            }

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_لیبل_VG);

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_لیبل_VN);
            توڑو;
        }

        حالت RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_میدان(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_میدان(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & ہدف_صفحہ_نقاب;
            RISCVIOMMUTrans_لیبل transtag;

            اگر (gv) {
                transtag = RISCV_IOMMU_TRANS_لیبل_VN;
                اگر (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_gscid_pscid_iova :
                                  riscv_iommu_iot_inval_gscid_pscid;
                } ورنہ {
                    func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                                  riscv_iommu_iot_inval_gscid;
                }
            } ورنہ {
                transtag = RISCV_IOMMU_TRANS_لیبل_SS;
                اگر (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_pscid_iova :
                                  riscv_iommu_iot_inval_pscid;
                } ورنہ {
                    func = (av) ? riscv_iommu_iot_inval_iova :
                                  riscv_iommu_iot_inval_all;
                }
            }

            riscv_iommu_iot_inval(s, func, gscid, pscid, iova, transtag);
            توڑو;
        }

        حالت RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            اگر (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* invalidate all device context cache mappings */
                func = riscv_iommu_ctx_inval_all;
            } ورنہ {
                /* invalidate all device context matching DID */
                func = riscv_iommu_ctx_inval_devid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_میدان(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID), 0);
            توڑو;

        حالت RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            اگر (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* illegal command arguments IODIR_PDT & DV == 0 */
                جاؤ cmd_ill;
            } ورنہ {
                func = riscv_iommu_ctx_inval_devid_procid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_میدان(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID),
                get_میدان(cmd.dword0, RISCV_IOMMU_CMD_IODIR_PID));
            توڑو;

        /* ATS commands */
        حالت RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_INVAL,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            اگر (!s->enable_ats) {
                جاؤ cmd_ill;
            }

            riscv_iommu_ats_inval(s, &cmd);
            توڑو;

        حالت RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_PRGR,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            اگر (!s->enable_ats) {
                جاؤ cmd_ill;
            }

            riscv_iommu_ats_prgr(s, &cmd);
            توڑو;

        طےشدہ:
        cmd_ill:
            /* Invalid instruction, do not advance instruction index. */
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                RISCV_IOMMU_CQCSR_CMD_ILL, 0);
            جاؤ fault;
        }

        /* Advance and update head pointer after command completes. */
        سر = (سر + 1) & s->cq_نقاب;
        riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_CQH, سر);
    }
    واپس;

fault:
    اگر (ctrl & RISCV_IOMMU_CQCSR_CIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_CQ);
    }
}

ساکن خالی riscv_iommu_process_cq_control(RISCVIOMMUState *s)
{
    uint64_t بنیاد;
    uint32_t ctrl_سیٹ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_سیٹ & RISCV_IOMMU_CQCSR_CQEN);
    bool فعال = !!(ctrl_سیٹ & RISCV_IOMMU_CQCSR_CQON);

    اگر (enable && !فعال) {
        بنیاد = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_CQB);
        s->cq_نقاب = (2ULL << get_میدان(بنیاد, RISCV_IOMMU_CQB_LOG2SZ)) - 1;
        s->cq_addr = PPN_PHYS(get_میدان(بنیاد, RISCV_IOMMU_CQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~s->cq_نقاب);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQT], 0);
        ctrl_سیٹ = RISCV_IOMMU_CQCSR_CQON;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQMF |
                   RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CMD_مقصد |
                   RISCV_IOMMU_CQCSR_FENCE_W_IP;
    } ورنہ اگر (!enable && فعال) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~0);
        ctrl_سیٹ = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQON;
    } ورنہ {
        ctrl_سیٹ = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, ctrl_سیٹ, ctrl_clr);

    /*
     * After clearing error bits (CMD_ILL, CQMF), if queue is still active,
     * re-process pending command.
     */
    ctrl_سیٹ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    اگر ((ctrl_سیٹ & RISCV_IOMMU_CQCSR_CQON) &&
        !(ctrl_سیٹ & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        riscv_iommu_process_cq_دم(s);
    }
}

ساکن خالی riscv_iommu_process_fq_control(RISCVIOMMUState *s)
{
    uint64_t بنیاد;
    uint32_t ctrl_سیٹ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_سیٹ & RISCV_IOMMU_FQCSR_FQEN);
    bool فعال = !!(ctrl_سیٹ & RISCV_IOMMU_FQCSR_FQON);

    اگر (enable && !فعال) {
        بنیاد = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_FQB);
        s->fq_نقاب = (2ULL << get_میدان(بنیاد, RISCV_IOMMU_FQB_LOG2SZ)) - 1;
        s->fq_addr = PPN_PHYS(get_میدان(بنیاد, RISCV_IOMMU_FQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~s->fq_نقاب);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQT], 0);
        ctrl_سیٹ = RISCV_IOMMU_FQCSR_FQON;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQMF |
            RISCV_IOMMU_FQCSR_FQOF;
    } ورنہ اگر (!enable && فعال) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~0);
        ctrl_سیٹ = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQON;
    } ورنہ {
        ctrl_سیٹ = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, ctrl_سیٹ, ctrl_clr);
}

ساکن خالی riscv_iommu_process_pq_control(RISCVIOMMUState *s)
{
    uint64_t بنیاد;
    uint32_t ctrl_سیٹ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_سیٹ & RISCV_IOMMU_PQCSR_PQEN);
    bool فعال = !!(ctrl_سیٹ & RISCV_IOMMU_PQCSR_PQON);

    اگر (enable && !فعال) {
        بنیاد = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_PQB);
        s->pq_نقاب = (2ULL << get_میدان(بنیاد, RISCV_IOMMU_PQB_LOG2SZ)) - 1;
        s->pq_addr = PPN_PHYS(get_میدان(بنیاد, RISCV_IOMMU_PQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~s->pq_نقاب);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQT], 0);
        ctrl_سیٹ = RISCV_IOMMU_PQCSR_PQON;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQMF |
            RISCV_IOMMU_PQCSR_PQOF;
    } ورنہ اگر (!enable && فعال) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~0);
        ctrl_سیٹ = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQON;
    } ورنہ {
        ctrl_سیٹ = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, ctrl_سیٹ, ctrl_clr);
}

ساکن خالی riscv_iommu_process_dbg(RISCVIOMMUState *s)
{
    uint64_t iova = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_IOVA);
    uint64_t ctrl = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_CTL);
    غیرعلامتی devid = get_میدان(ctrl, RISCV_IOMMU_TR_REQ_CTL_DID);
    غیرعلامتی pid = get_میدان(ctrl, RISCV_IOMMU_TR_REQ_CTL_PID);
    IOMMUAccessFlags perm = ctrl & RISCV_IOMMU_TR_REQ_CTL_NW
                            ? IOMMU_RO : IOMMU_RW;
    RISCVIOMMUContext *ctx;
    خالی *ref;

    اگر (!(ctrl & RISCV_IOMMU_TR_REQ_CTL_GO_BUSY)) {
        واپس;
    }

    ctx = riscv_iommu_ctx(s, devid, pid, perm, iova, &ref);
    اگر (ctx == تہی) {
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE,
                                 RISCV_IOMMU_TR_RESPONSE_FAULT |
                                 (RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED << 10));
    } ورنہ {
        IOMMUTLBEntry iotlb = {
            .iova = iova,
            .perm = perm,
            .addr_نقاب = ~0,
            .ہدف_as = تہی,
        };
        صحیح fault = riscv_iommu_translate(s, ctx, &iotlb, false);
        اگر (fault) {
            iova = RISCV_IOMMU_TR_RESPONSE_FAULT | (((uint64_t) fault) << 10);
        } ورنہ {
            iova = iotlb.translated_addr & ~iotlb.addr_نقاب;
            iova = سیٹ_میدان(0, RISCV_IOMMU_TR_RESPONSE_PPN, PPN_نیچے_کو(iova));
        }
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE, iova);
    }

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
        RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    riscv_iommu_ctx_put(s, ref);
}

قسمتعریف خالی riscv_iommu_process_fn(RISCVIOMMUState *s);

ساکن خالی riscv_iommu_update_icvec(RISCVIOMMUState *s, uint64_t ڈیٹا)
{
    uint64_t icvec = 0;

    icvec |= MIN(ڈیٹا & RISCV_IOMMU_ICVEC_CIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_CIV);

    icvec |= MIN(ڈیٹا & RISCV_IOMMU_ICVEC_FIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_FIV);

    icvec |= MIN(ڈیٹا & RISCV_IOMMU_ICVEC_PMIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PMIV);

    icvec |= MIN(ڈیٹا & RISCV_IOMMU_ICVEC_PIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PIV);

    trace_riscv_iommu_icvec_write(ڈیٹا, icvec);

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_ICVEC, icvec);
}

ساکن خالی riscv_iommu_update_ipsr(RISCVIOMMUState *s, uint64_t ڈیٹا)
{
    uint32_t cqcsr, fqcsr, pqcsr;
    uint32_t ipsr_سیٹ = 0;
    uint32_t ipsr_clr = 0;

    اگر (ڈیٹا & RISCV_IOMMU_IPSR_CIP) {
        cqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);

        اگر (cqcsr & RISCV_IOMMU_CQCSR_CIE &&
            (cqcsr & RISCV_IOMMU_CQCSR_FENCE_W_IP ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ILL ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_مقصد ||
             cqcsr & RISCV_IOMMU_CQCSR_CQMF)) {
            ipsr_سیٹ |= RISCV_IOMMU_IPSR_CIP;
        } ورنہ {
            ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
        }
    } ورنہ {
        ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
    }

    اگر (ڈیٹا & RISCV_IOMMU_IPSR_FIP) {
        fqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);

        اگر (fqcsr & RISCV_IOMMU_FQCSR_FIE &&
            (fqcsr & RISCV_IOMMU_FQCSR_FQOF ||
             fqcsr & RISCV_IOMMU_FQCSR_FQMF)) {
            ipsr_سیٹ |= RISCV_IOMMU_IPSR_FIP;
        } ورنہ {
            ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
        }
    } ورنہ {
        ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
    }

    اگر (!(ڈیٹا & RISCV_IOMMU_IPSR_PMIP)) {
        ipsr_clr |= RISCV_IOMMU_IPSR_PMIP;
    }

    اگر (ڈیٹا & RISCV_IOMMU_IPSR_PIP) {
        pqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);

        اگر (pqcsr & RISCV_IOMMU_PQCSR_PIE &&
            (pqcsr & RISCV_IOMMU_PQCSR_PQOF ||
             pqcsr & RISCV_IOMMU_PQCSR_PQMF)) {
            ipsr_سیٹ |= RISCV_IOMMU_IPSR_PIP;
        } ورنہ {
            ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
        }
    } ورنہ {
        ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, ipsr_سیٹ, ipsr_clr);
}

ساکن خالی riscv_iommu_process_hpm_writes(RISCVIOMMUState *s,
                                           uint32_t regb,
                                           bool پچھلا_cy_inh)
{
    انتخاب (regb) {
    حالت RISCV_IOMMU_REG_IOCOUNTINH:
        riscv_iommu_process_iocntinh_cy(s, پچھلا_cy_inh);
        توڑو;

    حالت RISCV_IOMMU_REG_IOHPMCYCLES:
    حالت RISCV_IOMMU_REG_IOHPMCYCLES + 4:
        riscv_iommu_process_hpmcycle_write(s);
        توڑو;

    حالت RISCV_IOMMU_REG_IOHPMEVT_بنیاد ...
        RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_عدد) + 4:
        riscv_iommu_process_hpmevt_write(s, regb & ~7);
        توڑو;
    }
}

/*
 * Write the resulting value of 'data' for the reg specified
 * by 'reg_addr', after considering read-only/read-write/write-clear
 * bits, in the pointer 'dest'.
 *
 * The result is written in little-endian.
 */
ساکن خالی riscv_iommu_write_reg_val(RISCVIOMMUState *s,
                                      خالی *dest, hwaddr reg_addr,
                                      صحیح سائز, uint64_t ڈیٹا)
{
    uint64_t ro = ldn_le_p(&s->regs_ro[reg_addr], سائز);
    uint64_t wc = ldn_le_p(&s->regs_wc[reg_addr], سائز);
    uint64_t curr_val = ldn_le_p(&s->regs[reg_addr], سائز);

    stn_le_p(dest, سائز, ((curr_val & ro) | (ڈیٹا & ~ro)) & ~(ڈیٹا & wc));
}

ساکن Mem_Tx_نتیجہ riscv_iommu_mmio_write(خالی *opaque, hwaddr addr,
                                          uint64_t ڈیٹا, غیرعلامتی سائز,
                                          MemTxAttrs attrs)
{
    riscv_iommu_process_fn *process_fn = تہی;
    RISCVIOMMUState *s = opaque;
    uint32_t regb = addr & ~3;
    uint32_t busy = 0;
    uint64_t val = 0;
    bool cy_inh = false;

    اگر ((addr & (سائز - 1)) != 0) {
        /* Unsupported MMIO alignment or access size */
        واپس MEMTX_ERROR;
    }

    اگر (addr + سائز > RISCV_IOMMU_REG_MSI_CONFIG) {
        /* Unsupported MMIO access location. */
        واپس MEMTX_ACCESS_ERROR;
    }

    /* Track actionable MMIO write. */
    انتخاب (regb) {
    حالت RISCV_IOMMU_REG_DDTP:
    حالت RISCV_IOMMU_REG_DDTP + 4:
        process_fn = riscv_iommu_process_ddtp;
        regb = RISCV_IOMMU_REG_DDTP;
        busy = RISCV_IOMMU_DDTP_BUSY;
        توڑو;

    حالت RISCV_IOMMU_REG_CQT:
        process_fn = riscv_iommu_process_cq_دم;
        توڑو;

    حالت RISCV_IOMMU_REG_CQCSR:
        process_fn = riscv_iommu_process_cq_control;
        busy = RISCV_IOMMU_CQCSR_BUSY;
        توڑو;

    حالت RISCV_IOMMU_REG_FQCSR:
        process_fn = riscv_iommu_process_fq_control;
        busy = RISCV_IOMMU_FQCSR_BUSY;
        توڑو;

    حالت RISCV_IOMMU_REG_PQCSR:
        process_fn = riscv_iommu_process_pq_control;
        busy = RISCV_IOMMU_PQCSR_BUSY;
        توڑو;

    حالت RISCV_IOMMU_REG_ICVEC:
    حالت RISCV_IOMMU_REG_IPSR:
        /*
         * ICVEC and IPSR have special read/write procedures. We'll
         * call their respective helpers and exit.
         */
        riscv_iommu_write_reg_val(s, &val, addr, سائز, ڈیٹا);

        /*
         * 'val' is stored as LE. Switch to host endianess
         * before using it.
         */
        val = le64_مقصد_cpu(val);

        اگر (regb == RISCV_IOMMU_REG_ICVEC) {
            riscv_iommu_update_icvec(s, val);
        } ورنہ {
            riscv_iommu_update_ipsr(s, val);
        }

        واپس MEMTX_OK;

    حالت RISCV_IOMMU_REG_TR_REQ_CTL:
        process_fn = riscv_iommu_process_dbg;
        regb = RISCV_IOMMU_REG_TR_REQ_CTL;
        busy = RISCV_IOMMU_TR_REQ_CTL_GO_BUSY;
        توڑو;

    حالت RISCV_IOMMU_REG_IOCOUNTINH:
        اگر (addr != RISCV_IOMMU_REG_IOCOUNTINH) {
            توڑو;
        }
        /* Store previous value of CY bit. */
        cy_inh = !!(riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTINH) &
            RISCV_IOMMU_IOCOUNTINH_CY);
        توڑو;


    طےشدہ:
        توڑو;
    }

    /*
     * Registers update might be not synchronized with core logic.
     * If system software updates register when relevant BUSY bit
     * is set IOMMU behavior of additional writes to the register
     * is UNSPECIFIED.
     */
    riscv_iommu_write_reg_val(s, &s->regs[addr], addr, سائز, ڈیٹا);

    /* Busy flag update, MSB 4-byte register. */
    اگر (busy) {
        uint32_t rw = ldl_le_p(&s->regs[regb]);
        stl_le_p(&s->regs[regb], rw | busy);
    }

    /* Process HPM writes and update any internal state if needed. */
    اگر (regb >= RISCV_IOMMU_REG_IOCOUNTOVF &&
        regb <= (RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_عدد) + 4)) {
        riscv_iommu_process_hpm_writes(s, regb, cy_inh);
    }

    اگر (process_fn) {
        process_fn(s);
    }

    واپس MEMTX_OK;
}

ساکن Mem_Tx_نتیجہ riscv_iommu_mmio_read(خالی *opaque, hwaddr addr,
    uint64_t *ڈیٹا, غیرعلامتی سائز, MemTxAttrs attrs)
{
    RISCVIOMMUState *s = opaque;
    uint64_t val = -1;
    uint8_t *ptr;

    اگر ((addr & (سائز - 1)) != 0) {
        /* Unsupported MMIO alignment. */
        واپس MEMTX_ERROR;
    }

    اگر (addr + سائز > RISCV_IOMMU_REG_MSI_CONFIG) {
        واپس MEMTX_ACCESS_ERROR;
    }

    /* Compute cycle register value. */
    اگر ((addr & ~7) == RISCV_IOMMU_REG_IOHPMCYCLES) {
        val = riscv_iommu_hpmcycle_read(s);
        ptr = (uint8_t *)&val + (addr & 7);
    } ورنہ اگر ((addr & ~3) == RISCV_IOMMU_REG_IOCOUNTOVF) {
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
    } ورنہ {
        ptr = &s->regs[addr];
    }

    val = ldn_le_p(ptr, سائز);

    *ڈیٹا = val;

    واپس MEMTX_OK;
}

ساکن ثابت حافظہ_Region_Ops riscv_iommu_mmio_ops = {
    .read_with_attrs = riscv_iommu_mmio_read,
    .write_with_attrs = riscv_iommu_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_سائز = 4,
        .max_access_سائز = 8,
        .unaligned = false,
    },
    .valid = {
        .min_access_سائز = 4,
        .max_access_سائز = 8,
    }
};

/*
 * Translations matching MSI pattern check are redirected to "riscv-iommu-trap"
 * memory region as untranslated address, for additional MSI/MRIF interception
 * by IOMMU interrupt remapping implementation.
 * Note: Device emulation code generating an MSI is expected to provide a valid
 * memory transaction attributes with requested_id set.
 */
ساکن Mem_Tx_نتیجہ riscv_iommu_trap_write(خالی *opaque, hwaddr addr,
    uint64_t ڈیٹا, غیرعلامتی سائز, MemTxAttrs attrs)
{
    RISCVIOMMUState* s = (RISCVIOMMUState *)opaque;
    RISCVIOMMUContext *ctx;
    Mem_Tx_نتیجہ res;
    خالی *ref;
    uint32_t devid = attrs.requester_id;

    اگر (attrs.unspecified) {
        واپس MEMTX_ACCESS_ERROR;
    }

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= s->bus << 8;

    ctx = riscv_iommu_ctx(s, devid, 0, IOMMU_RW, addr, &ref);
    اگر (ctx == تہی) {
        res = MEMTX_ACCESS_ERROR;
    } ورنہ {
        res = riscv_iommu_msi_write(s, ctx, addr, ڈیٹا, سائز, attrs);
    }
    riscv_iommu_ctx_put(s, ref);
    واپس res;
}

ساکن Mem_Tx_نتیجہ riscv_iommu_trap_read(خالی *opaque, hwaddr addr,
    uint64_t *ڈیٹا, غیرعلامتی سائز, MemTxAttrs attrs)
{
    واپس MEMTX_ACCESS_ERROR;
}

ساکن ثابت حافظہ_Region_Ops riscv_iommu_trap_ops = {
    .read_with_attrs = riscv_iommu_trap_read,
    .write_with_attrs = riscv_iommu_trap_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_سائز = 4,
        .max_access_سائز = 8,
    },
    .valid = {
        .min_access_سائز = 4,
        .max_access_سائز = 8,
    }
};

خالی riscv_iommu_سیٹ_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_منوال منوال)
{
    s->cap = سیٹ_میدان(s->cap, RISCV_IOMMU_CAP_IGS, منوال);
}

ساکن خالی riscv_iommu_instance_init(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    /* Enable translation debug interface */
    s->cap = RISCV_IOMMU_CAP_DBG;

    /* TODO: method to report supported PID bits */
    s->pid_بٹ = 8; /* restricted to size of MemTxAttrs.pid */
    s->cap |= RISCV_IOMMU_CAP_PD8;

    /* register storage */
    s->regs = g_new0(uint8_t, RISCV_IOMMU_REG_سائز);
    s->regs_ro = g_new0(uint8_t, RISCV_IOMMU_REG_سائز);
    s->regs_wc = g_new0(uint8_t, RISCV_IOMMU_REG_سائز);

     /* Mark all registers read-only */
    memset(s->regs_ro, 0xff, RISCV_IOMMU_REG_سائز);

    /* Device translation context cache */
    s->ctx_cache = g_hash_جدول_new_بھرا(riscv_iommu_ctx_hash,
                                         riscv_iommu_ctx_equal,
                                         g_free, تہی);

    s->iot_cache = g_hash_جدول_new_بھرا(riscv_iommu_iot_hash,
                                         riscv_iommu_iot_equal,
                                         g_free, تہی);

    s->iommus.le_اگلا = تہی;
    s->iommus.le_پچھلا = تہی;
    QLIST_INIT(&s->spaces);
}

ساکن خالی riscv_iommu_instance_finalize(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    g_free(s->regs);
    g_free(s->regs_ro);
    g_free(s->regs_wc);

    g_hash_جدول_unref(s->ctx_cache);
    g_hash_جدول_unref(s->iot_cache);
}

ساکن خالی riscv_iommu_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    /* Report QEMU target physical address space limits. */
    s->cap = سیٹ_میدان(s->cap, RISCV_IOMMU_CAP_PAS, s->pas_بٹ);

    s->cap |= s->نسخہ & RISCV_IOMMU_CAP_نسخہ;
    اگر (s->enable_msi) {
        s->cap |= RISCV_IOMMU_CAP_MSI_FLAT | RISCV_IOMMU_CAP_MSI_MRIF;
    }
    اگر (s->enable_ats) {
        s->cap |= RISCV_IOMMU_CAP_ATS;
    }
    اگر (s->enable_s_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32 | RISCV_IOMMU_CAP_SV39 |
                  RISCV_IOMMU_CAP_SV48 | RISCV_IOMMU_CAP_SV57;
    }
    اگر (s->enable_g_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32X4 | RISCV_IOMMU_CAP_SV39X4 |
                  RISCV_IOMMU_CAP_SV48X4 | RISCV_IOMMU_CAP_SV57X4 |
                  RISCV_IOMMU_CAP_SVRSW60T59B;
    }

    اگر (s->hpm_cntrs > 0) {
        /* Clip number of HPM counters to maximum supported (31). */
        اگر (s->hpm_cntrs > RISCV_IOMMU_IOCOUNT_عدد) {
            s->hpm_cntrs = RISCV_IOMMU_IOCOUNT_عدد;
        }
        /* Enable hardware performance monitor interface */
        s->cap |= RISCV_IOMMU_CAP_HPM;
    }

    /* Out-of-reset translation mode: OFF (DMA disabled) BARE (passthrough) */
    s->ddtp = سیٹ_میدان(0, RISCV_IOMMU_DDTP_منوال, s->enable_off ?
                        RISCV_IOMMU_DDTP_منوال_OFF : RISCV_IOMMU_DDTP_منوال_BARE);

    /*
     * Register complete MMIO space, including MSI/PBA registers.
     * Note, PCIDevice implementation will add overlapping MR for MSI/PBA,
     * managed directly by the PCIDevice implementation.
     */
    حافظہ_region_init_io(&s->regs_mr, OBJECT(dev), &riscv_iommu_mmio_ops, s,
        "riscv-iommu-regs", RISCV_IOMMU_REG_سائز);

    /* Set power-on register state */
    stq_le_p(&s->regs[RISCV_IOMMU_REG_CAP], s->cap);

    stq_le_p(&s->regs[RISCV_IOMMU_REG_FCTL], 0);
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FCTL],
             ~(RISCV_IOMMU_FCTL_GXL | RISCV_IOMMU_FCTL_WSI));

    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_DDTP],
        ~(RISCV_IOMMU_DDTP_PPN | RISCV_IOMMU_DDTP_منوال));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQB],
        ~(RISCV_IOMMU_CQB_LOG2SZ | RISCV_IOMMU_CQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQB],
        ~(RISCV_IOMMU_FQB_LOG2SZ | RISCV_IOMMU_FQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQB],
        ~(RISCV_IOMMU_PQB_LOG2SZ | RISCV_IOMMU_PQB_PPN));
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_CQCSR], RISCV_IOMMU_CQCSR_CQMF |
        RISCV_IOMMU_CQCSR_CMD_مقصد | RISCV_IOMMU_CQCSR_CMD_ILL);
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
    اگر (s->cap & RISCV_IOMMU_CAP_DBG) {
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_IOVA], 0);
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_CTL],
            RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    }

    /* If HPM registers are enabled. */
    اگر (s->cap & RISCV_IOMMU_CAP_HPM) {
        /* +1 for cycle counter bit. */
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOCOUNTINH],
                 ~((2 << s->hpm_cntrs) - 1));
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCYCLES], 0);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCTR_بنیاد],
               0x00, s->hpm_cntrs * 8);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMEVT_بنیاد],
               0x00, s->hpm_cntrs * 8);
    }

    /* Memory region for downstream access, if specified. */
    اگر (s->ہدف_mr) {
        s->ہدف_as = g_new0(پتہ_Space, 1);
        پتہ_space_init(s->ہدف_as, s->ہدف_mr,
            "riscv-iommu-downstream");
    } ورنہ {
        /* Fallback to global system memory. */
        s->ہدف_as = &پتہ_space_حافظہ;
    }

    /* Memory region for untranslated MRIF/MSI writes */
    حافظہ_region_init_io(&s->trap_mr, OBJECT(dev), &riscv_iommu_trap_ops, s,
            "riscv-iommu-trap", ~0ULL);
    پتہ_space_init(&s->trap_as, &s->trap_mr, "riscv-iommu-trap-as");

    اگر (s->cap & RISCV_IOMMU_CAP_HPM) {
        s->hpm_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL, riscv_iommu_hpm_timer_cb, s);
        s->hpm_event_ctr_map = g_hash_جدول_new(g_براہ_راست_hash, g_براہ_راست_equal);
    }
}

ساکن خالی riscv_iommu_unrealize(DeviceState *dev)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    اگر (s->cap & RISCV_IOMMU_CAP_HPM) {
        g_hash_جدول_unref(s->hpm_event_ctr_map);
        timer_free(s->hpm_timer);
    }
}

خالی riscv_iommu_reset(RISCVIOMMUState *s)
{
    uint32_t reg_clr;
    صحیح ddtp_منوال;

    /*
     * Clear DDTP while setting DDTP_mode back to user
     * initial setting.
     */
    ddtp_منوال = s->enable_off ?
                RISCV_IOMMU_DDTP_منوال_OFF : RISCV_IOMMU_DDTP_منوال_BARE;
    s->ddtp = سیٹ_میدان(0, RISCV_IOMMU_DDTP_منوال, ddtp_منوال);
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

    g_hash_جدول_remove_all(s->ctx_cache);
    g_hash_جدول_remove_all(s->iot_cache);
}

ساکن ثابت Property riscv_iommu_properties[] = {
    DEFINE_PROP_UINT32("version", RISCVIOMMUState, نسخہ,
        RISCV_IOMMU_SPEC_DOT_VER),
    DEFINE_PROP_UINT32("pas-bits", RISCVIOMMUState, pas_بٹ, 0),
    DEFINE_PROP_UINT32("bus", RISCVIOMMUState, bus, 0x0),
    DEFINE_PROP_UINT32("ioatc-limit", RISCVIOMMUState, iot_حد_بندی,
        حد_بندی_CACHE_IOT),
    DEFINE_PROP_BOOL("intremap", RISCVIOMMUState, enable_msi, TRUE),
    DEFINE_PROP_BOOL("ats", RISCVIOMMUState, enable_ats, TRUE),
    DEFINE_PROP_BOOL("off", RISCVIOMMUState, enable_off, TRUE),
    DEFINE_PROP_BOOL("s-stage", RISCVIOMMUState, enable_s_stage, TRUE),
    DEFINE_PROP_BOOL("g-stage", RISCVIOMMUState, enable_g_stage, TRUE),
    DEFINE_PROP_LINK("downstream-mr", RISCVIOMMUState, ہدف_mr,
        TYPE_حافظہ_REGION, حافظہ_Region *),
    DEFINE_PROP_UINT8("hpm-counters", RISCVIOMMUState, hpm_cntrs,
                      RISCV_IOMMU_IOCOUNT_عدد),
};

ساکن خالی riscv_iommu_class_init(ObjectClass *klass, ثابت خالی *ڈیٹا)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    /* internal device for riscv-iommu-{pci/sys}, not user-creatable */
    dc->user_creatable = false;
    dc->realize = riscv_iommu_realize;
    dc->unrealize = riscv_iommu_unrealize;
    device_class_سیٹ_props(dc, riscv_iommu_properties);
}

ساکن ثابت TypeInfo riscv_iommu_info = {
    .name = TYPE_RISCV_IOMMU,
    .والد = TYPE_DEVICE,
    .instance_سائز = حجم(RISCVIOMMUState),
    .instance_init = riscv_iommu_instance_init,
    .instance_finalize = riscv_iommu_instance_finalize,
    .class_init = riscv_iommu_class_init,
};

ساکن ثابت حرف *IOMMU_FLAG_STR[] = {
    "NA",
    "RO",
    "WR",
    "RW",
};

/* RISC-V IOMMU Memory Region - Address Translation Space */
ساکن IOMMUTLBEntry riscv_iommu_حافظہ_region_translate(
    IOMMUMemoryRegion *iommu_mr, hwaddr addr,
    IOMMUAccessFlags flag, صحیح iommu_idx)
{
    RISCVIOMMUSpace *as = container_کا(iommu_mr, RISCVIOMMUSpace, iova_mr);
    RISCVIOMMUContext *ctx;
    خالی *ref;
    IOMMUTLBEntry iotlb = {
        .iova = addr,
        .ہدف_as = as->iommu->ہدف_as,
        .addr_نقاب = ~0ULL,
        .perm = flag,
    };
    uint32_t devid = riscv_iommu_space_devid(as);

    ctx = riscv_iommu_ctx(as->iommu, devid, iommu_idx, flag, addr, &ref);
    اگر (ctx == تہی) {
        /* Translation disabled or invalid. */
        iotlb.addr_نقاب = 0;
        iotlb.perm = IOMMU_NONE;
    } ورنہ اگر (riscv_iommu_translate(as->iommu, ctx, &iotlb, true)) {
        /* Translation disabled or fault reported. */
        iotlb.addr_نقاب = 0;
        iotlb.perm = IOMMU_NONE;
    }

    /* Trace all dma translations with original access flags. */
    trace_riscv_iommu_dma(as->iommu->والد_obj.id, PCI_BUS_عدد(devid),
                          PCI_خانہ(devid), PCI_FUNC(devid), iommu_idx,
                          IOMMU_FLAG_STR[flag & IOMMU_RW], iotlb.iova,
                          iotlb.translated_addr);

    riscv_iommu_ctx_put(as->iommu, ref);

    واپس iotlb;
}

ساکن صحیح riscv_iommu_حافظہ_region_notify(
    IOMMUMemoryRegion *iommu_mr, IOMMUNotifierFlag old,
    IOMMUNotifierFlag new, Error **errp)
{
    RISCVIOMMUSpace *as = container_کا(iommu_mr, RISCVIOMMUSpace, iova_mr);

    اگر (old == IOMMU_NOTIFIER_NONE) {
        as->notifier = true;
        trace_riscv_iommu_notifier_جوڑو(iommu_mr->والد_obj.name);
    } ورنہ اگر (new == IOMMU_NOTIFIER_NONE) {
        as->notifier = false;
        trace_riscv_iommu_notifier_del(iommu_mr->والد_obj.name);
    }

    واپس 0;
}

ساکن اندرونی bool pci_is_iommu(PCIDevice *pdev)
{
    واپس pci_get_لفظ(pdev->config + PCI_CLASS_DEVICE) == 0x0806;
}

ساکن پتہ_Space *riscv_iommu_ڈھونڈو_as(PCIBus *bus, خالی *opaque, صحیح devfn)
{
    RISCVIOMMUState *s = (RISCVIOMMUState *) opaque;
    PCIDevice *pdev = pci_ڈھونڈو_device(bus, pci_bus_عدد(bus), devfn);
    پتہ_Space *as = تہی;

    اگر (pdev && pci_is_iommu(pdev)) {
        واپس s->ہدف_as;
    }

    /* Find first registered IOMMU device */
    جب (s->iommus.le_پچھلا) {
        s = *(s->iommus.le_پچھلا);
    }

    /* Find first matching IOMMU */
    جب (s != تہی && as == تہی) {
        as = riscv_iommu_space(s, bus, devfn);
        s = s->iommus.le_اگلا;
    }

    واپس as ? as : &پتہ_space_حافظہ;
}

ساکن ثابت PCIIOMMUOps riscv_iommu_ops = {
    .get_پتہ_space = riscv_iommu_ڈھونڈو_as,
};

خالی riscv_iommu_pci_تیاری_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
        Error **errp)
{
    اگر (bus->iommu_ops &&
        bus->iommu_ops->get_پتہ_space == riscv_iommu_ڈھونڈو_as) {
        /* Allow multiple IOMMUs on the same PCIe bus, link known devices */
        RISCVIOMMUState *آخری = (RISCVIOMMUState *)bus->iommu_opaque;
        QLIST_داخل_کرو_AFTER(آخری, iommu, iommus);
    } ورنہ اگر (!bus->iommu_ops && !bus->iommu_opaque) {
        pci_تیاری_iommu(bus, &riscv_iommu_ops, iommu);
    } ورنہ {
        error_setg(errp, "can't register secondary IOMMU for PCI bus #%d",
            pci_bus_عدد(bus));
    }
}

ساکن صحیح riscv_iommu_حافظہ_region_اشاریہ(IOMMUMemoryRegion *iommu_mr,
    MemTxAttrs attrs)
{
    واپس attrs.unspecified ? RISCV_IOMMU_NOPROCID : (صحیح)attrs.pid;
}

ساکن صحیح riscv_iommu_حافظہ_region_اشاریہ_len(IOMMUMemoryRegion *iommu_mr)
{
    RISCVIOMMUSpace *as = container_کا(iommu_mr, RISCVIOMMUSpace, iova_mr);
    واپس 1 << as->iommu->pid_بٹ;
}

ساکن خالی riscv_iommu_حافظہ_region_init(ObjectClass *klass, ثابت خالی *ڈیٹا)
{
    IOMMUMemoryRegionClass *imrc = IOMMU_حافظہ_REGION_CLASS(klass);

    imrc->translate = riscv_iommu_حافظہ_region_translate;
    imrc->notify_flag_بدلا = riscv_iommu_حافظہ_region_notify;
    imrc->attrs_مقصد_اشاریہ = riscv_iommu_حافظہ_region_اشاریہ;
    imrc->عدد_indexes = riscv_iommu_حافظہ_region_اشاریہ_len;
}

ساکن ثابت TypeInfo riscv_iommu_حافظہ_region_info = {
    .والد = TYPE_IOMMU_حافظہ_REGION,
    .name = TYPE_RISCV_IOMMU_حافظہ_REGION,
    .class_init = riscv_iommu_حافظہ_region_init,
};

ساکن خالی riscv_iommu_رجسٹر_mr_types(خالی)
{
    type_رجسٹر_static(&riscv_iommu_حافظہ_region_info);
    type_رجسٹر_static(&riscv_iommu_info);
}

type_init(riscv_iommu_رجسٹر_mr_types);
