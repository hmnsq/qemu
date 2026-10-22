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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qom/object.h"
#అన్తర్భావ "exec/target_page.h"
#అన్తర్భావ "hw/pci/pci_bus.h"
#అన్తర్భావ "hw/pci/pci_device.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "migration/vmstate.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qemu/timer.h"
#అన్తర్భావ "qemu/target-info.h"
#అన్తర్భావ "qemu/bitops.h"

#అన్తర్భావ "cpu_bits.h"
#అన్తర్భావ "riscv-iommu.h"
#అన్తర్భావ "riscv-iommu-bits.h"
#అన్తర్భావ "riscv-iommu-hpm.h"
#అన్తర్భావ "trace.h"

#నిర్వచించు హద్దు_CACHE_CTX               (1U << 7)
#నిర్వచించు హద్దు_CACHE_IOT               (1U << 20)

/* Physical page number coversions */
#నిర్వచించు PPN_PHYS(ppn)                 ((ppn) << లక్ష్యం_పుట_బిట్లు)
#నిర్వచించు PPN_కిందికి(phy)                 ((phy) >> లక్ష్యం_పుట_బిట్లు)

రకనిర్వచనం నిర్మాణం RISCVIOMMUEntry RISCVIOMMUEntry;

/* Device assigned I/O address space */
నిర్మాణం RISCVIOMMUSpace {
    IOMMUMemoryRegion iova_mr;  /* IOVA memory region for attached device */
    చిరునామా_Space iova_as;       /* IOVA address space for attached device */
    RISCVIOMMUState *iommu;     /* Managing IOMMU device state */
    PCIBus *bus;                /* PCI bus of the requester */
    uint8_t devfn;              /* Requester identifier, AKA device_id */
    bool notifier;              /* IOMMU unmap notifier enabled */
    QLIST_నమోదు(RISCVIOMMUSpace) list;
};

రకనిర్వచనం గణన RISCVIOMMUTrans_ట్యాగ్ {
    RISCV_IOMMU_TRANS_ట్యాగ్_BY,  /* Bypass */
    RISCV_IOMMU_TRANS_ట్యాగ్_SS,  /* Single Stage */
    RISCV_IOMMU_TRANS_ట్యాగ్_VG,  /* G-stage only */
    RISCV_IOMMU_TRANS_ట్యాగ్_VN,  /* Nested translation */
} RISCVIOMMUTrans_ట్యాగ్;

/* Address translation cache entry */
నిర్మాణం RISCVIOMMUEntry {
    RISCVIOMMUTrans_ట్యాగ్ ట్యాగ్;     /* Translation Tag */
    uint64_t iova:44;           /* IOVA Page Number */
    uint64_t pscid:20;          /* Process Soft-Context identifier */
    uint64_t phys:44;           /* Physical Page Number */
    uint64_t gscid:16;          /* Guest Soft-Context identifier */
    uint64_t perm:2;            /* IOMMU_RW flags */
};

/* IOMMU index for transactions without process_id specified. */
#నిర్వచించు RISCV_IOMMU_NOPROCID 0

స్థిర uint32_t riscv_iommu_space_devid(RISCVIOMMUSpace *as)
{
    uint32_t devid = PCI_BUILD_BDF(pci_bus_అంకె(as->bus), as->devfn);

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= as->iommu->bus << 8;
    తిరిగి devid;
}

స్థిర uint8_t riscv_iommu_get_icvec_సదిశ(uint32_t icvec, uint32_t vec_type)
{
    ఎంపిక (vec_type) {
    స్థితి RISCV_IOMMU_INTR_CQ:
        తిరిగి icvec & RISCV_IOMMU_ICVEC_CIV;
    స్థితి RISCV_IOMMU_INTR_FQ:
        తిరిగి (icvec & RISCV_IOMMU_ICVEC_FIV) >> 4;
    స్థితి RISCV_IOMMU_INTR_PM:
        తిరిగి (icvec & RISCV_IOMMU_ICVEC_PMIV) >> 8;
    స్థితి RISCV_IOMMU_INTR_PQ:
        తిరిగి (icvec & RISCV_IOMMU_ICVEC_PIV) >> 12;
    అప్రమేయ:
        g_assert_not_reached();
    }
}

శూన్య riscv_iommu_notify(RISCVIOMMUState *s, పూర్ణాంకం vec_type)
{
    uint32_t ipsr, icvec, సదిశ;

    ఒకవేళ (!s->notify) {
        తిరిగి;
    }

    icvec = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_ICVEC);
    ipsr = riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, (1 << vec_type), 0);

    ఒకవేళ (!(ipsr & (1 << vec_type))) {
        సదిశ = riscv_iommu_get_icvec_సదిశ(icvec, vec_type);
        s->notify(s, సదిశ);
        trace_riscv_iommu_notify_int_సదిశ(vec_type, సదిశ);
    }
}

శూన్య riscv_iommu_fault(RISCVIOMMUState *s, నిర్మాణం riscv_iommu_fq_రికార్డు *ev)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t తల = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQH) & s->fq_ముసుగు;
    uint32_t తోక = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQT) & s->fq_ముసుగు;
    uint32_t తదుపరి = (తోక + 1) & s->fq_ముసుగు;
    uint32_t devid = get_క్షేత్రం(ev->hdr, RISCV_IOMMU_FQ_HDR_DID);

    trace_riscv_iommu_flt(s->మాతృ_obj.id, PCI_BUS_అంకె(devid), PCI_స్థానం(devid),
                          PCI_FUNC(devid), ev->hdr, ev->iotval);

    ఒకవేళ (!(ctrl & RISCV_IOMMU_FQCSR_FQON) ||
        !!(ctrl & (RISCV_IOMMU_FQCSR_FQOF | RISCV_IOMMU_FQCSR_FQMF))) {
        తిరిగి;
    }

    ఒకవేళ (తల == తదుపరి) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                              RISCV_IOMMU_FQCSR_FQOF, 0);
    } లేకపోతే {
        dma_addr_t addr = s->fq_addr + తోక * పరిమాణం(*ev);
        ఒకవేళ (dma_జ్ఞాపకం_write(s->లక్ష్యం_as, addr, ev, పరిమాణం(*ev),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                                  RISCV_IOMMU_FQCSR_FQMF, 0);
        } లేకపోతే {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_FQT, తదుపరి);
        }
    }

    ఒకవేళ (ctrl & RISCV_IOMMU_FQCSR_FIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_FQ);
    }
}

స్థిర శూన్య riscv_iommu_pri(RISCVIOMMUState *s,
    నిర్మాణం riscv_iommu_pq_రికార్డు *pr)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t తల = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQH) & s->pq_ముసుగు;
    uint32_t తోక = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQT) & s->pq_ముసుగు;
    uint32_t తదుపరి = (తోక + 1) & s->pq_ముసుగు;
    uint32_t devid = get_క్షేత్రం(pr->hdr, RISCV_IOMMU_PREQ_HDR_DID);

    trace_riscv_iommu_pri(s->మాతృ_obj.id, PCI_BUS_అంకె(devid), PCI_స్థానం(devid),
                          PCI_FUNC(devid), pr->payload);

    ఒకవేళ (!(ctrl & RISCV_IOMMU_PQCSR_PQON) ||
        !!(ctrl & (RISCV_IOMMU_PQCSR_PQOF | RISCV_IOMMU_PQCSR_PQMF))) {
        తిరిగి;
    }

    ఒకవేళ (తల == తదుపరి) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                              RISCV_IOMMU_PQCSR_PQOF, 0);
    } లేకపోతే {
        dma_addr_t addr = s->pq_addr + తోక * పరిమాణం(*pr);
        ఒకవేళ (dma_జ్ఞాపకం_write(s->లక్ష్యం_as, addr, pr, పరిమాణం(*pr),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                                  RISCV_IOMMU_PQCSR_PQMF, 0);
        } లేకపోతే {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_PQT, తదుపరి);
        }
    }

    ఒకవేళ (ctrl & RISCV_IOMMU_PQCSR_PIE) {
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
స్థిర uint64_t riscv_iommu_pext_u64(uint64_t val, uint64_t ext)
{
    uint64_t ret = 0;
    uint64_t rot = 1;

    అయితే (ext) {
        ఒకవేళ (ext & 1) {
            ఒకవేళ (val & 1) {
                ret |= rot;
            }
            rot <<= 1;
        }
        val >>= 1;
        ext >>= 1;
    }

    తిరిగి ret;
}

/* Check if GPA matches MSI/MRIF pattern. */
స్థిర bool riscv_iommu_msi_check(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    dma_addr_t gpa)
{
    ఒకవేళ (!s->enable_msi) {
        తిరిగి false;
    }

    ఒకవేళ (get_క్షేత్రం(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_బహుళకం) !=
        RISCV_IOMMU_DC_MSIPTP_బహుళకం_FLAT) {
        తిరిగి false; /* Invalid MSI/MRIF mode */
    }

    ఒకవేళ ((PPN_కిందికి(gpa) ^ ctx->msi_addr_నమూనా) & ~ctx->msi_addr_ముసుగు) {
        తిరిగి false; /* GPA not in MSI range defined by AIA IMSIC rules. */
    }

    తిరిగి true;
}

/* Returns the NAPOT page mask, or 0 for reserved encodings. */
స్థిర hwaddr riscv_iommu_napot_పుట_ముసుగు(hwaddr ppn, hwaddr addr, hwaddr *బహిర్గతం)
{
    పూర్ణాంకం napot_బిట్లు = ctz64(ppn) + 1;
    hwaddr napot_ముసుగు, పుట_ముసుగు;

    /* The spec only defines 64KB (napot_bits == 4) */
    ఒకవేళ (napot_బిట్లు != 4) {
        తిరిగి 0;
    }

    napot_ముసుగు = (1ULL << napot_బిట్లు) - 1;
    పుట_ముసుగు = PPN_PHYS(napot_ముసుగు) | (లక్ష్యం_పుట_కొలత - 1);

    *బహిర్గతం = PPN_PHYS(ppn & ~napot_ముసుగు) | (addr & పుట_ముసుగు);

    తిరిగి పుట_ముసుగు;
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
స్థిర పూర్ణాంకం riscv_iommu_spa_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb)
{
    IOMMUAccessFlags trans_perm = IOMMU_NONE;
    dma_addr_t addr, పునాది;
    uint64_t satp, gatp, pte;
    bool en_s, en_g;
    నిర్మాణం {
        గుర్తులేని అక్షరం అడుగు;
        గుర్తులేని అక్షరం స్థాయిలు;
        గుర్తులేని అక్షరం ptidxbits;
        గుర్తులేని అక్షరం ptesize;
    } sc[2];
    /* Translation stage phase */
    గణన {
        S_STAGE = 0,
        G_STAGE = 1,
    } pass;
    Mem_Tx_ఫలితం ret;
    bool pv = !!ctx->process_id;
    /*
     * Keep the request permission separate from iotlb->perm.  G-stage
     * walks translate S-stage PTE addresses before the real leaf is
     * reached, but permission checks and fault types must still use the
     * original request.  A successful walk leaves iotlb->perm with the
     * effective leaf permission for the translation cache.
     */
    స్థిరరాశి IOMMUAccessFlags req_perm = iotlb->perm;

    satp = get_క్షేత్రం(ctx->satp, RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం);
    gatp = get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం);

    en_s = satp != RISCV_IOMMU_DC_FSC_బహుళకం_BARE;
    en_g = gatp != RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE;

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
    ఒకవేళ (!en_s && (req_perm & IOMMU_WO) &&
        riscv_iommu_msi_check(s, ctx, iotlb->iova)) {
        iotlb->లక్ష్యం_as = &s->trap_as;
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_ముసుగు = ~లక్ష్యం_పుట_ముసుగు;
        తిరిగి 0;
    }

    /* Exit early for pass-through mode. */
    ఒకవేళ (!(en_s || en_g)) {
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_ముసుగు = ~లక్ష్యం_పుట_ముసుగు;
        /* Allow R/W in pass-through mode */
        iotlb->perm = IOMMU_RW;
        తిరిగి 0;
    }

    /* S/G translation parameters. */
    కొరకు (pass = 0; pass < 2; pass++) {
        uint32_t sv_బహుళకం;

        sc[pass].అడుగు = 0;
        ఒకవేళ (pass ? (s->fctl & RISCV_IOMMU_FCTL_GXL) :
            (ctx->tc & RISCV_IOMMU_DC_TC_SXL)) {
            /* 32bit mode for GXL/SXL == 1 */
            ఎంపిక (pass ? gatp : satp) {
            స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE:
                sc[pass].స్థాయిలు    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                ఆపు;
            స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV32_X4:
                sv_బహుళకం = pass ? RISCV_IOMMU_CAP_SV32X4 : RISCV_IOMMU_CAP_SV32;
                ఒకవేళ (!(s->cap & sv_బహుళకం)) {
                    తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].స్థాయిలు    = 2;
                sc[pass].ptidxbits = 10;
                sc[pass].ptesize   = 4;
                ఆపు;
            అప్రమేయ:
                తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        } లేకపోతే {
            /* 64bit mode for GXL/SXL == 0 */
            ఎంపిక (pass ? gatp : satp) {
            స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE:
                sc[pass].స్థాయిలు    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                ఆపు;
            స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV39_X4:
                sv_బహుళకం = pass ? RISCV_IOMMU_CAP_SV39X4 : RISCV_IOMMU_CAP_SV39;
                ఒకవేళ (!(s->cap & sv_బహుళకం)) {
                    తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].స్థాయిలు    = 3;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ఆపు;
            స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV48_X4:
                sv_బహుళకం = pass ? RISCV_IOMMU_CAP_SV48X4 : RISCV_IOMMU_CAP_SV48;
                ఒకవేళ (!(s->cap & sv_బహుళకం)) {
                    తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].స్థాయిలు    = 4;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ఆపు;
            స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV57_X4:
                sv_బహుళకం = pass ? RISCV_IOMMU_CAP_SV57X4 : RISCV_IOMMU_CAP_SV57;
                ఒకవేళ (!(s->cap & sv_బహుళకం)) {
                    తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].స్థాయిలు    = 5;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ఆపు;
            అప్రమేయ:
                తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        }
    };

    /* S/G stages translation tables root pointers */
    gatp = PPN_PHYS(get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_ATP_PPN_క్షేత్రం));
    satp = PPN_PHYS(get_క్షేత్రం(ctx->satp, RISCV_IOMMU_ATP_PPN_క్షేత్రం));
    addr = (en_s && en_g) ? satp : iotlb->iova;
    పునాది = en_g ? gatp : satp;
    pass = en_g ? G_STAGE : S_STAGE;

    చేయి {
        స్థిరరాశి గుర్తులేని widened = (pass && !sc[pass].అడుగు) ? 2 : 0;
        స్థిరరాశి గుర్తులేని va_బిట్లు = widened + sc[pass].ptidxbits;
        స్థిరరాశి గుర్తులేని va_skip = లక్ష్యం_పుట_బిట్లు + sc[pass].ptidxbits *
                                 (sc[pass].స్థాయిలు - 1 - sc[pass].అడుగు);
        స్థిరరాశి గుర్తులేని idx = (addr >> va_skip) & ((1 << va_బిట్లు) - 1);
        స్థిరరాశి dma_addr_t pte_addr = పునాది + idx * sc[pass].ptesize;
        స్థిరరాశి bool ade =
            ctx->tc & (pass ? RISCV_IOMMU_DC_TC_GADE : RISCV_IOMMU_DC_TC_SADE);

        ఒకవేళ (ade && !(s->cap & RISCV_IOMMU_CAP_AMO_HWAD)) {
            /* GADE/SADE are reserved bits if AMO_HWAD is cleared.  */
            తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }

        /* Address range check before first level lookup */
        ఒకవేళ (!sc[pass].అడుగు) {
            స్థిరరాశి uint64_t va_len = va_skip + va_బిట్లు;
            స్థిరరాశి uint64_t va_ముసుగు = (1ULL << va_len) - 1;

            ఒకవేళ (pass == S_STAGE && va_len > 32) {
                uint64_t ముసుగు, masked_msbs;

                ముసుగు = MAKE_64_బిట్టు_ముసుగు(0, లక్ష్యం_long_బిట్లు() - va_len + 1);
                masked_msbs = (addr >> (va_len - 1)) & ముసుగు;

                ఒకవేళ (masked_msbs != 0 && masked_msbs != ముసుగు) {
                    తిరిగి (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S;
                }
            } లేకపోతే {
                ఒకవేళ ((addr & va_ముసుగు) != addr) {
                    తిరిగి (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS;
                }
            }
        }


        ఒకవేళ (pass == S_STAGE) {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_S_VS_WALKS);
        } లేకపోతే {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_G_WALKS);
        }

        /* Read page table entry */
        ఒకవేళ (sc[pass].ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->లక్ష్యం_as, pte_addr, &pte32,
                             MEMTXATTRS_UNSPECIFIED);
            pte = pte32;
        } లేకపోతే {
            ret = ldq_le_dma(s->లక్ష్యం_as, pte_addr, &pte,
                             MEMTXATTRS_UNSPECIFIED);
        }
        ఒకవేళ (ret != MEMTX_OK) {
            తిరిగి (req_perm & IOMMU_WO) ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT
                                         : RISCV_IOMMU_FQ_CAUSE_RD_FAULT;
        }

        sc[pass].అడుగు++;
        hwaddr ppn = pte >> PTE_PPN_జరుపు;

        ఒకవేళ (!(pte & PTE_V)) {
            ఆపు;                /* Invalid PTE */
        } లేకపోతే ఒకవేళ (pte & PTE_RESERVED(false)) {
            ఆపు;                /* Reserved PTE bits set */
        } లేకపోతే ఒకవేళ (!(pte & (PTE_R | PTE_W | PTE_X))) {
            పునాది = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } లేకపోతే ఒకవేళ ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            ఆపు;                /* Reserved leaf PTE flags: PTE_W */
        } లేకపోతే ఒకవేళ ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            ఆపు;                /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } లేకపోతే ఒకవేళ (ppn & ((1ULL << (va_skip - లక్ష్యం_పుట_బిట్లు)) - 1)) {
            ఆపు;                /* Misaligned PPN */
        } లేకపోతే ఒకవేళ (!(pte & PTE_U) && !pv) {
            /*
             * All accesses are assumed to be User mode unless
             * process_id is valid (pv).  In case we have a
             * non-user mode leaf PTE and !pv we need to fault.
             */
            ఆపు;
        } లేకపోతే ఒకవేళ ((req_perm & IOMMU_RO) && !(pte & PTE_R)) {
            ఆపు;                /* Read access check failed */
        } లేకపోతే ఒకవేళ ((req_perm & IOMMU_WO) && !(pte & PTE_W)) {
            ఆపు;                /* Write access check failed */
        } లేకపోతే ఒకవేళ (!ade && !(pte & PTE_A)) {
            ఆపు;                /* Access bit not set */
        } లేకపోతే ఒకవేళ ((req_perm & IOMMU_WO) && !ade && !(pte & PTE_D)) {
            ఆపు;                /* Dirty bit not set */
        } లేకపోతే ఒకవేళ (pass == G_STAGE && !(pte & PTE_U)) {
            /*
             * riscv-iommu spec 1.0: "When checking the U bit in a
             * second-stage PTE, the transaction is treated as
             * not requesting supervisor privilege."
             *
             * I.e. we need to fault if this is a non-user PTE since
             * we are always in user mode at this point.
             */
            ఆపు;
        } లేకపోతే {
            /* Leaf PTE, translation completed. */
            sc[pass].అడుగు = sc[pass].స్థాయిలు;

            ఒకవేళ (pte & PTE_N) {
                hwaddr ముసుగు = riscv_iommu_napot_పుట_ముసుగు(ppn, addr, &పునాది);

                ఒకవేళ (!ముసుగు) {
                    ఆపు;
                }
                iotlb->addr_ముసుగు &= ముసుగు;
            } లేకపోతే {
                పునాది = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
                /* Update address mask based on smallest translation granularity */
                iotlb->addr_ముసుగు &= (1ULL << va_skip) - 1;
            }

            /* Continue with S-Stage translation? */
            ఒకవేళ (pass && sc[0].అడుగు != sc[0].స్థాయిలు) {
                pass = S_STAGE;
                addr = iotlb->iova;
                కొనసాగు;
            }

            /* Cache the effective permission, not this request's subset. */
            IOMMUAccessFlags ఆకు_perm = (pte & PTE_W) ?
                                         ((pte & PTE_R) ? IOMMU_RW : IOMMU_WO) :
                                         IOMMU_RO;

            trans_perm = trans_perm == IOMMU_NONE ?
                         ఆకు_perm : trans_perm & ఆకు_perm;

            /* Translation phase completed (GPA or SPA) */
            iotlb->translated_addr = పునాది;

            /* Check MSI GPA address match */
            ఒకవేళ (pass == S_STAGE && (req_perm & IOMMU_WO) &&
                riscv_iommu_msi_check(s, ctx, పునాది)) {
                /* Trap MSI writes and return GPA address. */
                iotlb->లక్ష్యం_as = &s->trap_as;
                iotlb->addr_ముసుగు = ~లక్ష్యం_పుట_ముసుగు;
                తిరిగి 0;
            }

            /* Continue with G-Stage translation? */
            ఒకవేళ (!pass && en_g) {
                pass = G_STAGE;
                addr = పునాది;
                పునాది = gatp;
                sc[pass].అడుగు = 0;
                కొనసాగు;
            }

            iotlb->perm = trans_perm;
            తిరిగి 0;
        }

        ఒకవేళ (sc[pass].అడుగు == sc[pass].స్థాయిలు) {
            ఆపు; /* Can't find leaf PTE */
        }

        /* Continue with G-Stage translation? */
        ఒకవేళ (!pass && en_g) {
            pass = G_STAGE;
            addr = పునాది;
            పునాది = gatp;
            sc[pass].అడుగు = 0;
        }
    } అయితే (1);

    /*
     * riscv_iommu_translate() will receive a fault and then call
     * riscv_iommu_report_fault() using iotlb->translated_addr
     * as iotval2.  Update translated_addr it with the latest
     * translated addr we have.
     */
    iotlb->translated_addr = addr;

    తిరిగి (req_perm & IOMMU_WO) ?
                (pass ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S) :
                (pass ? RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S);
}

స్థిర శూన్య riscv_iommu_report_fault(RISCVIOMMUState *s,
                                     RISCVIOMMUContext *ctx,
                                     uint32_t fault_type, uint32_t cause,
                                     bool pv,
                                     uint64_t iotval, uint64_t iotval2)
{
    నిర్మాణం riscv_iommu_fq_రికార్డు ev = { 0 };

    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_DTF) {
        ఎంపిక (cause) {
        స్థితి RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED:
        స్థితి RISCV_IOMMU_FQ_CAUSE_DDT_ఎక్కించు_FAULT:
        స్థితి RISCV_IOMMU_FQ_CAUSE_DDT_INVALID:
        స్థితి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED:
        స్థితి RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED:
        స్థితి RISCV_IOMMU_FQ_CAUSE_అంతర్గత_DP_ERROR:
        స్థితి RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT:
            ఆపు;
        అప్రమేయ:
            /* DTF prevents reporting a fault for this given cause */
            తిరిగి;
        }
    }

    ev.hdr = సమితి_క్షేత్రం(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE, cause);
    ev.hdr = సమితి_క్షేత్రం(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE, fault_type);
    ev.hdr = సమితి_క్షేత్రం(ev.hdr, RISCV_IOMMU_FQ_HDR_DID, ctx->devid);
    ev.hdr = సమితి_క్షేత్రం(ev.hdr, RISCV_IOMMU_FQ_HDR_PV, pv);

    ఒకవేళ (pv) {
        ev.hdr = సమితి_క్షేత్రం(ev.hdr, RISCV_IOMMU_FQ_HDR_PID, ctx->process_id);
    }

    ev.iotval = iotval;
    ev.iotval2 = iotval2;

    riscv_iommu_fault(s, &ev);
}

/* Redirect MSI write for given GPA. */
స్థిర Mem_Tx_ఫలితం riscv_iommu_msi_write(RISCVIOMMUState *s,
    RISCVIOMMUContext *ctx, uint64_t gpa, uint64_t దత్తాంశం,
    గుర్తులేని కొలత, MemTxAttrs attrs)
{
    Mem_Tx_ఫలితం res;
    dma_addr_t addr;
    uint64_t intn;
    కొలత_t స్థానభ్రంశం;
    uint32_t n190;
    uint64_t pte[2];
    పూర్ణాంకం fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    పూర్ణాంకం cause;

    /* Interrupt File Number */
    intn = riscv_iommu_pext_u64(PPN_కిందికి(gpa), ctx->msi_addr_ముసుగు);
    స్థానభ్రంశం = intn * పరిమాణం(pte);

    /* fetch MSI PTE */
    addr = PPN_PHYS(get_క్షేత్రం(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_PPN));
    ఒకవేళ (addr & స్థానభ్రంశం) {
        /* Interrupt file number out of range */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ఎక్కించు_FAULT;
        వెళ్ళు err;
    }

    addr |= స్థానభ్రంశం;
    res = dma_జ్ఞాపకం_read(s->లక్ష్యం_as, addr, &pte, పరిమాణం(pte),
            MEMTXATTRS_UNSPECIFIED);
    ఒకవేళ (res != MEMTX_OK) {
        ఒకవేళ (res == MEMTX_DECODE_ERROR) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED;
        } లేకపోతే {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_ఎక్కించు_FAULT;
        }
        వెళ్ళు err;
    }

    le64_గమ్యం_cpus(&pte[0]);
    le64_గమ్యం_cpus(&pte[1]);

    ఒకవేళ (!(pte[0] & RISCV_IOMMU_MSI_PTE_V) || (pte[0] & RISCV_IOMMU_MSI_PTE_C)) {
        /*
         * The spec mentions that: "If msipte.C == 1, then further
         * processing to interpret the PTE is implementation
         * defined.". We'll abort with cause = 262 for this
         * case too.
         */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_INVALID;
        వెళ్ళు err;
    }

    ఎంపిక (get_క్షేత్రం(pte[0], RISCV_IOMMU_MSI_PTE_M)) {
    స్థితి RISCV_IOMMU_MSI_PTE_M_BASIC:
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
        ఒకవేళ (pte[0] & (GENMASK_ULL(62, 54) | GENMASK_ULL(9, 3))) {
            res = MEMTX_DECODE_ERROR;
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
            వెళ్ళు err;
        }

        /* MSI Pass-through mode */
        addr = PPN_PHYS(get_క్షేత్రం(pte[0], RISCV_IOMMU_MSI_PTE_PPN));

        trace_riscv_iommu_msi(s->మాతృ_obj.id, PCI_BUS_అంకె(ctx->devid),
                              PCI_స్థానం(ctx->devid), PCI_FUNC(ctx->devid),
                              gpa, addr);

        res = dma_జ్ఞాపకం_write(s->లక్ష్యం_as, addr, &దత్తాంశం, కొలత, attrs);
        ఒకవేళ (res != MEMTX_OK) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
            వెళ్ళు err;
        }

        తిరిగి MEMTX_OK;
    స్థితి RISCV_IOMMU_MSI_PTE_M_MRIF:
        /* MRIF mode, continue. */
        ఆపు;
    అప్రమేయ:
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        వెళ్ళు err;
    }

    /*
     * Report an error for interrupt identities exceeding the maximum allowed
     * for an IMSIC interrupt file (2047) or destination address is not 32-bit
     * aligned. See IOMMU Specification, Chapter 2.3. MSI page tables.
     */
    ఒకవేళ ((దత్తాంశం > 2047) || (gpa & 3)) {
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        వెళ్ళు err;
    }

    /* MSI MRIF mode, non atomic pending bit update */

    /* MRIF pending bit address */
    addr = get_క్షేత్రం(pte[0], RISCV_IOMMU_MSI_PTE_MRIF_ADDR) << 9;
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
    addr = addr | ((దత్తాంశం & 0x7c0) >> 2);

    trace_riscv_iommu_msi(s->మాతృ_obj.id, PCI_BUS_అంకె(ctx->devid),
                          PCI_స్థానం(ctx->devid), PCI_FUNC(ctx->devid),
                          gpa, addr);

    /* MRIF pending bit mask */
    దత్తాంశం = 1ULL << (దత్తాంశం & 0x03f);
    res = dma_జ్ఞాపకం_read(s->లక్ష్యం_as, addr, &intn, పరిమాణం(intn), attrs);
    ఒకవేళ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ఎక్కించు_FAULT;
        వెళ్ళు err;
    }

    intn = intn | దత్తాంశం;
    res = dma_జ్ఞాపకం_write(s->లక్ష్యం_as, addr, &intn, పరిమాణం(intn), attrs);
    ఒకవేళ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        వెళ్ళు err;
    }

    /* Get MRIF enable bits */
    addr = addr + పరిమాణం(intn);
    res = dma_జ్ఞాపకం_read(s->లక్ష్యం_as, addr, &intn, పరిమాణం(intn), attrs);
    ఒకవేళ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ఎక్కించు_FAULT;
        వెళ్ళు err;
    }

    ఒకవేళ (!(intn & దత్తాంశం)) {
        /* notification disabled, MRIF update completed. */
        తిరిగి MEMTX_OK;
    }

    /* Send notification message */
    addr = PPN_PHYS(get_క్షేత్రం(pte[1], RISCV_IOMMU_MSI_MRIF_NPPN));
    n190 = get_క్షేత్రం(pte[1], RISCV_IOMMU_MSI_MRIF_NID) |
          (get_క్షేత్రం(pte[1], RISCV_IOMMU_MSI_MRIF_NID_MSB) << 10);

    res = dma_జ్ఞాపకం_write(s->లక్ష్యం_as, addr, &n190, పరిమాణం(n190), attrs);
    ఒకవేళ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        వెళ్ళు err;
    }

    trace_riscv_iommu_mrif_notification(s->మాతృ_obj.id, n190, addr);

    తిరిగి MEMTX_OK;

err:
    riscv_iommu_report_fault(s, ctx, fault_type, cause,
                             !!ctx->process_id, 0, 0);
    తిరిగి res;
}

/*
 * Check device context configuration as described by the
 * riscv-iommu spec section "Device-context configuration
 * checks".
 */
స్థిర bool riscv_iommu_validate_device_ctx(RISCVIOMMUState *s,
                                            RISCVIOMMUContext *ctx)
{
    uint32_t fsc_బహుళకం, msi_బహుళకం;
    uint64_t gatp;

    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_RESERVED) {
        తిరిగి false;
    }

    ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI ||
         ctx->tc & RISCV_IOMMU_DC_TC_PRPR)) {
        తిరిగి false;
    }

    ఒకవేళ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI)) {
        తిరిగి false;
    }

    ఒకవేళ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI) &&
        ctx->tc & RISCV_IOMMU_DC_TC_PRPR) {
        తిరిగి false;
    }

    ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_T2GPA) &&
        ctx->tc & RISCV_IOMMU_DC_TC_T2GPA) {
        తిరిగి false;
    }

    ఒకవేళ (s->cap & RISCV_IOMMU_CAP_MSI_FLAT) {
        msi_బహుళకం = get_క్షేత్రం(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_బహుళకం);

        ఒకవేళ (msi_బహుళకం != RISCV_IOMMU_DC_MSIPTP_బహుళకం_OFF &&
            msi_బహుళకం != RISCV_IOMMU_DC_MSIPTP_బహుళకం_FLAT) {
            తిరిగి false;
        }
    }

    gatp = get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం);
    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA &&
        gatp == RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE) {
        తిరిగి false;
    }

    ఒకవేళ (gatp != RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE) {
        uint64_t iohgatp_ppn = get_క్షేత్రం(ctx->gatp,
                                         RISCV_IOMMU_DC_IOHGATP_PPN);

        /*
         * One of the conditions for a misconfigured DDT entry
         * according to the riscv-spec: "DC.iohgatp.MODE is not
         * Bare and the root page table (address) determined by
         * DC.iohgatp.PPN is not aligned to a 16-KiB boundary."
         */
        ఒకవేళ (PPN_PHYS(iohgatp_ppn) & ((1ULL << 14) - 1)) {
            తిరిగి false;
        }
    }

    fsc_బహుళకం = get_క్షేత్రం(ctx->satp, RISCV_IOMMU_DC_FSC_బహుళకం);

    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_PDTV) {
        ఎంపిక (fsc_బహుళకం) {
        స్థితి RISCV_IOMMU_DC_FSC_PDTP_బహుళకం_PD8:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_PD8)) {
                తిరిగి false;
            }
            ఆపు;
        స్థితి RISCV_IOMMU_DC_FSC_PDTP_బహుళకం_PD17:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_PD17)) {
                తిరిగి false;
            }
            ఆపు;
        స్థితి RISCV_IOMMU_DC_FSC_PDTP_బహుళకం_PD20:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_PD20)) {
                తిరిగి false;
            }
            ఆపు;
        }
    } లేకపోతే {
        /* DC.tc.PDTV is 0 */
        ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_DPE) {
            తిరిగి false;
        }

        ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
            ఒకవేళ (fsc_బహుళకం == RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV32 &&
                !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                తిరిగి false;
            }
        } లేకపోతే {
            ఎంపిక (fsc_బహుళకం) {
            స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV39:
                ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                    తిరిగి false;
                }
                ఆపు;
            స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV48:
                ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                    తిరిగి false;
                }
            ఆపు;
            స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV57:
                ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                    తిరిగి false;
                }
                ఆపు;
            }
        }
    }

    /*
     * CAP_END is always zero (only one endianess). FCTL_BE is
     * always zero (little-endian accesses). Thus TC_SBE must
     * always be LE, i.e. zero.
     */
    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_SBE) {
        తిరిగి false;
    }

    తిరిగి true;
}

/*
 * Validate process context (PC) according to section
 * "Process-context configuration checks".
 */
స్థిర bool riscv_iommu_validate_process_ctx(RISCVIOMMUState *s,
                                             RISCVIOMMUContext *ctx)
{
    uint32_t బహుళకం;

    ఒకవేళ (get_క్షేత్రం(ctx->ta, RISCV_IOMMU_PC_TA_RESERVED)) {
        తిరిగి false;
    }

    ఒకవేళ (get_క్షేత్రం(ctx->satp, RISCV_IOMMU_PC_FSC_RESERVED)) {
        తిరిగి false;
    }

    బహుళకం = get_క్షేత్రం(ctx->satp, RISCV_IOMMU_DC_FSC_బహుళకం);
    ఎంపిక (బహుళకం) {
    స్థితి RISCV_IOMMU_DC_FSC_బహుళకం_BARE:
    /* sv39 and sv32 modes have the same value (8) */
    స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV39:
    స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV48:
    స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV57:
        ఆపు;
    అప్రమేయ:
        తిరిగి false;
    }

    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
        ఒకవేళ (బహుళకం == RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV32 &&
            !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                తిరిగి false;
        }
    } లేకపోతే {
        ఎంపిక (బహుళకం) {
        స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV39:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                తిరిగి false;
            }
            ఆపు;
        స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV48:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                తిరిగి false;
            }
            ఆపు;
        స్థితి RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV57:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                తిరిగి false;
            }
            ఆపు;
        }
    }

    తిరిగి true;
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
స్థిర Mem_Tx_ఫలితం pdt_జ్ఞాపకం_read(RISCVIOMMUState *s,
                                   RISCVIOMMUContext *ctx,
                                   dma_addr_t addr,
                                   శూన్య *buf, dma_addr_t len,
                                   MemTxAttrs attrs)
{
    uint64_t gatp_బహుళకం, pte;
    నిర్మాణం {
        గుర్తులేని అక్షరం అడుగు;
        గుర్తులేని అక్షరం స్థాయిలు;
        గుర్తులేని అక్షరం ptidxbits;
        గుర్తులేని అక్షరం ptesize;
    } sc;
    Mem_Tx_ఫలితం ret;
    dma_addr_t పునాది = addr;

    /* G stages translation mode */
    gatp_బహుళకం = get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం);
    ఒకవేళ (gatp_బహుళకం == RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE) {
        వెళ్ళు బహిర్గతం;
    }

    /* G stages translation tables root pointer */
    పునాది = PPN_PHYS(get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_ATP_PPN_క్షేత్రం));

    /* Start at step 0 */
    sc.అడుగు = 0;

    ఒకవేళ (s->fctl & RISCV_IOMMU_FCTL_GXL) {
        /* 32bit mode for GXL == 1 */
        ఎంపిక (gatp_బహుళకం) {
        స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV32_X4:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV32X4)) {
                తిరిగి MEMTX_ACCESS_ERROR;
            }
            sc.స్థాయిలు    = 2;
            sc.ptidxbits = 10;
            sc.ptesize   = 4;
            ఆపు;
        అప్రమేయ:
            తిరిగి MEMTX_ACCESS_ERROR;
        }
    } లేకపోతే {
        /* 64bit mode for GXL == 0 */
        ఎంపిక (gatp_బహుళకం) {
        స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV39_X4:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV39X4)) {
                తిరిగి MEMTX_ACCESS_ERROR;
            }
            sc.స్థాయిలు    = 3;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ఆపు;
        స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV48_X4:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV48X4)) {
                తిరిగి MEMTX_ACCESS_ERROR;
            }
            sc.స్థాయిలు    = 4;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ఆపు;
        స్థితి RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV57_X4:
            ఒకవేళ (!(s->cap & RISCV_IOMMU_CAP_SV57X4)) {
                తిరిగి MEMTX_ACCESS_ERROR;
            }
            sc.స్థాయిలు    = 5;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ఆపు;
        అప్రమేయ:
            తిరిగి MEMTX_ACCESS_ERROR;
        }
    }

    చేయి {
        స్థిరరాశి గుర్తులేని va_బిట్లు = (sc.అడుగు ? 0 : 2) + sc.ptidxbits;
        స్థిరరాశి గుర్తులేని va_skip = లక్ష్యం_పుట_బిట్లు + sc.ptidxbits *
                                 (sc.స్థాయిలు - 1 - sc.అడుగు);
        స్థిరరాశి గుర్తులేని idx = (addr >> va_skip) & ((1 << va_బిట్లు) - 1);
        స్థిరరాశి dma_addr_t pte_addr = పునాది + idx * sc.ptesize;

        /* Address range check before first level lookup */
        ఒకవేళ (!sc.అడుగు) {
            స్థిరరాశి uint64_t va_ముసుగు = (1ULL << (va_skip + va_బిట్లు)) - 1;
            ఒకవేళ ((addr & va_ముసుగు) != addr) {
                తిరిగి MEMTX_ACCESS_ERROR;
            }
        }

        /* Read page table entry */
        ఒకవేళ (sc.ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->లక్ష్యం_as, pte_addr, &pte32, attrs);
            pte = pte32;
        } లేకపోతే {
            ret = ldq_le_dma(s->లక్ష్యం_as, pte_addr, &pte, attrs);
        }
        ఒకవేళ (ret != MEMTX_OK) {
            తిరిగి ret;
        }

        sc.అడుగు++;
        hwaddr ppn = pte >> PTE_PPN_జరుపు;

        ఒకవేళ (!(pte & PTE_V)) {
            తిరిగి MEMTX_ACCESS_ERROR; /* Invalid PTE */
        } లేకపోతే ఒకవేళ (!(pte & (PTE_R | PTE_W | PTE_X))) {
            పునాది = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } లేకపోతే ఒకవేళ ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            తిరిగి MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W */
        } లేకపోతే ఒకవేళ ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            తిరిగి MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } లేకపోతే ఒకవేళ (ppn & ((1ULL << (va_skip - లక్ష్యం_పుట_బిట్లు)) - 1)) {
            తిరిగి MEMTX_ACCESS_ERROR; /* Misaligned PPN */
        } లేకపోతే {
            /* Leaf PTE, translation completed. */
            ఒకవేళ (pte & PTE_N) {
                ఒకవేళ (!riscv_iommu_napot_పుట_ముసుగు(ppn, addr, &పునాది)) {
                    తిరిగి MEMTX_ACCESS_ERROR;
                }
            } లేకపోతే {
                పునాది = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
            }
            ఆపు;
        }

        ఒకవేళ (sc.అడుగు == sc.స్థాయిలు) {
            తిరిగి MEMTX_ACCESS_ERROR; /* Can't find leaf PTE */
        }
    } అయితే (1);

బహిర్గతం:
    తిరిగి dma_జ్ఞాపకం_read(s->లక్ష్యం_as, పునాది, buf, len, attrs);
}

/*
 * RISC-V IOMMU Device Context Loopkup - Device Directory Tree Walk
 *
 * @s         : IOMMU Device State
 * @ctx       : Device Translation Context with devid and process_id set.
 * @return    : success or fault code.
 */
స్థిర పూర్ణాంకం riscv_iommu_ctx_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx)
{
    స్థిరరాశి uint64_t ddtp = s->ddtp;
    గుర్తులేని బహుళకం = get_క్షేత్రం(ddtp, RISCV_IOMMU_DDTP_బహుళకం);
    dma_addr_t addr = PPN_PHYS(get_క్షేత్రం(ddtp, RISCV_IOMMU_DDTP_PPN));
    నిర్మాణం riscv_iommu_dc dc;
    /* Device Context format: 0: extended (64 bytes) | 1: base (32 bytes) */
    స్థిరరాశి పూర్ణాంకం dc_fmt = !s->enable_msi;
    స్థిరరాశి కొలత_t dc_len = పరిమాణం(dc) >> dc_fmt;
    పూర్ణాంకం లోతు;
    uint64_t de;

    ఎంపిక (బహుళకం) {
    స్థితి RISCV_IOMMU_DDTP_బహుళకం_OFF:
        తిరిగి RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED;

    స్థితి RISCV_IOMMU_DDTP_బహుళకం_BARE:
        /* mock up pass-through translation context */
        ctx->gatp = సమితి_క్షేత్రం(0, RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం,
            RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE);
        ctx->satp = సమితి_క్షేత్రం(0, RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం,
            RISCV_IOMMU_DC_FSC_బహుళకం_BARE);

        ctx->tc = RISCV_IOMMU_DC_TC_V;
        ఒకవేళ (s->enable_ats) {
            ctx->tc |= RISCV_IOMMU_DC_TC_EN_ATS;
        }

        ctx->ta = 0;
        ctx->msiptp = 0;
        తిరిగి 0;

    స్థితి RISCV_IOMMU_DDTP_బహుళకం_1_LVL:
        లోతు = 0;
        ఆపు;

    స్థితి RISCV_IOMMU_DDTP_బహుళకం_2_LVL:
        లోతు = 1;
        ఆపు;

    స్థితి RISCV_IOMMU_DDTP_బహుళకం_3_LVL:
        లోతు = 2;
        ఆపు;

    అప్రమేయ:
        తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /*
     * Check supported device id width (in bits).
     * See IOMMU Specification, Chapter 6. Software guidelines.
     * - if extended device-context format is used:
     *   1LVL: 6, 2LVL: 15, 3LVL: 24
     * - if base device-context format is used:
     *   1LVL: 7, 2LVL: 16, 3LVL: 24
     */
    ఒకవేళ (ctx->devid >= (1 << (లోతు * 9 + 6 + (dc_fmt && లోతు != 2)))) {
        తిరిగి RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
    }

    /* Device directory tree walk */
    కొరకు (; లోతు-- > 0; ) {
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
        స్థిరరాశి పూర్ణాంకం విభజించు = లోతు * 9 + 6 + dc_fmt;
        addr |= ((ctx->devid >> విభజించు) << 3) & ~లక్ష్యం_పుట_ముసుగు;
        ఒకవేళ (dma_జ్ఞాపకం_read(s->లక్ష్యం_as, addr, &de, పరిమాణం(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_ఎక్కించు_FAULT;
        }
        le64_గమ్యం_cpus(&de);
        ఒకవేళ (!(de & RISCV_IOMMU_DDTE_VALID)) {
            /* invalid directory entry */
            తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        ఒకవేళ (de & ~(RISCV_IOMMU_DDTE_PPN | RISCV_IOMMU_DDTE_VALID)) {
            /* reserved bits set */
            తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }
        addr = PPN_PHYS(get_క్షేత్రం(de, RISCV_IOMMU_DDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);

    /* index into device context entry page */
    addr |= (ctx->devid * dc_len) & ~లక్ష్యం_పుట_ముసుగు;

    memset(&dc, 0, పరిమాణం(dc));
    ఒకవేళ (dma_జ్ఞాపకం_read(s->లక్ష్యం_as, addr, &dc, dc_len,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_ఎక్కించు_FAULT;
    }

    /* Set translation context. */
    ctx->tc = le64_గమ్యం_cpu(dc.tc);
    ctx->gatp = le64_గమ్యం_cpu(dc.iohgatp);
    ctx->satp = le64_గమ్యం_cpu(dc.fsc);
    ctx->ta = le64_గమ్యం_cpu(dc.ta);
    ctx->msiptp = le64_గమ్యం_cpu(dc.msiptp);
    ctx->msi_addr_ముసుగు = le64_గమ్యం_cpu(dc.msi_addr_ముసుగు);
    ctx->msi_addr_నమూనా = le64_గమ్యం_cpu(dc.msi_addr_నమూనా);

    ఒకవేళ (!(ctx->tc & RISCV_IOMMU_DC_TC_V)) {
        తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
    }

    ఒకవేళ (!riscv_iommu_validate_device_ctx(s, ctx)) {
        తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /* FSC field checks */
    బహుళకం = get_క్షేత్రం(ctx->satp, RISCV_IOMMU_DC_FSC_బహుళకం);
    addr = PPN_PHYS(get_క్షేత్రం(ctx->satp, RISCV_IOMMU_DC_FSC_PPN));

    ఒకవేళ (!(ctx->tc & RISCV_IOMMU_DC_TC_PDTV)) {
        ఒకవేళ (ctx->process_id != RISCV_IOMMU_NOPROCID) {
            /* PID is disabled */
            తిరిగి RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
        }
        ఒకవేళ (బహుళకం > RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV57) {
            /* Invalid translation mode */
            తిరిగి RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        తిరిగి 0;
    }

    ఒకవేళ (ctx->process_id == RISCV_IOMMU_NOPROCID) {
        ఒకవేళ (!(ctx->tc & RISCV_IOMMU_DC_TC_DPE)) {
            /* No default process_id enabled, set BARE mode */
            ctx->satp = 0ULL;
            తిరిగి 0;
        } లేకపోతే {
            /* Use default process_id #0 */
            ctx->process_id = 0;
        }
    }

    ఒకవేళ (బహుళకం == RISCV_IOMMU_DC_FSC_బహుళకం_BARE) {
        /* No S-Stage translation, done. */
        తిరిగి 0;
    }

    /* FSC.TC.PDTV enabled */
    ఒకవేళ (బహుళకం > RISCV_IOMMU_DC_FSC_PDTP_బహుళకం_PD20) {
        /* Invalid PDTP.MODE */
        తిరిగి RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    కొరకు (లోతు = బహుళకం - RISCV_IOMMU_DC_FSC_PDTP_బహుళకం_PD8; లోతు-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

        /*
         * Select process id index bits based on process directory tree
         * level. See IOMMU Specification, 2.2. Process-Directory-Table.
         */
        స్థిరరాశి పూర్ణాంకం విభజించు = లోతు * 9 + 8;
        addr |= ((ctx->process_id >> విభజించు) << 3) & ~లక్ష్యం_పుట_ముసుగు;
        ఒకవేళ (pdt_జ్ఞాపకం_read(s, ctx, addr, &de, పరిమాణం(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            తిరిగి RISCV_IOMMU_FQ_CAUSE_PDT_ఎక్కించు_FAULT;
        }
        le64_గమ్యం_cpus(&de);
        ఒకవేళ (!(de & RISCV_IOMMU_PDTE_VALID)) {
            తిరిగి RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
        }
        addr = PPN_PHYS(get_క్షేత్రం(de, RISCV_IOMMU_PDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

    /* Leaf entry in PDT */
    addr |= (ctx->process_id << 4) & ~లక్ష్యం_పుట_ముసుగు;
    ఒకవేళ (pdt_జ్ఞాపకం_read(s, ctx, addr, &dc.ta, పరిమాణం(uint64_t) * 2,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        తిరిగి RISCV_IOMMU_FQ_CAUSE_PDT_ఎక్కించు_FAULT;
    }

    /* Use FSC and TA from process directory entry. */
    ctx->ta = le64_గమ్యం_cpu(dc.ta);
    ctx->satp = le64_గమ్యం_cpu(dc.fsc);

    ఒకవేళ (!(ctx->ta & RISCV_IOMMU_PC_TA_V)) {
        తిరిగి RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
    }

    ఒకవేళ (!riscv_iommu_validate_process_ctx(s, ctx)) {
        తిరిగి RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    తిరిగి 0;
}

/* Translation Context cache support */
స్థిర gboolean riscv_iommu_ctx_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUContext *c1 = (RISCVIOMMUContext *) v1;
    RISCVIOMMUContext *c2 = (RISCVIOMMUContext *) v2;
    తిరిగి c1->devid == c2->devid &&
           c1->process_id == c2->process_id;
}

స్థిర guint riscv_iommu_ctx_hash(gconstpointer v)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) v;
    /*
     * Generate simple hash of (process_id, devid)
     * assuming 24-bit wide devid.
     */
    తిరిగి (guint)(ctx->devid) + ((guint)(ctx->process_id) << 24);
}

స్థిర శూన్య riscv_iommu_ctx_inval_devid_procid(gpointer కీలకం, gpointer విలువ,
                                               gpointer దత్తాంశం)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) విలువ;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) దత్తాంశం;
    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid &&
        ctx->process_id == arg->process_id) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

స్థిర శూన్య riscv_iommu_ctx_inval_devid(gpointer కీలకం, gpointer విలువ,
                                        gpointer దత్తాంశం)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) విలువ;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) దత్తాంశం;
    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

స్థిర శూన్య riscv_iommu_ctx_inval_all(gpointer కీలకం, gpointer విలువ,
                                      gpointer దత్తాంశం)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) విలువ;
    ఒకవేళ (ctx->tc & RISCV_IOMMU_DC_TC_V) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

స్థిర శూన్య riscv_iommu_ctx_inval(RISCVIOMMUState *s, GHFunc func,
                                  uint32_t devid, uint32_t process_id)
{
    GHash_పట్టిక *ctx_cache;
    RISCVIOMMUContext కీలకం = {
        .devid = devid,
        .process_id = process_id,
    };
    ctx_cache = g_hash_పట్టిక_ref(s->ctx_cache);
    g_hash_పట్టిక_foreach(ctx_cache, func, &కీలకం);
    g_hash_పట్టిక_unref(ctx_cache);
}

/* Find or allocate translation context for a given {device_id, process_id} */
స్థిర RISCVIOMMUContext *riscv_iommu_ctx(RISCVIOMMUState *s,
                                          గుర్తులేని devid, గుర్తులేని process_id,
                                          IOMMUAccessFlags perm, uint64_t iova,
                                          శూన్య **ref)
{
    GHash_పట్టిక *ctx_cache;
    RISCVIOMMUContext *ctx;
    RISCVIOMMUContext కీలకం = {
        .devid = devid,
        .process_id = process_id,
    };
    గుర్తులేని బహుళకం = get_క్షేత్రం(s->ddtp, RISCV_IOMMU_DDTP_బహుళకం);
    uint32_t fault_type;

    ctx_cache = g_hash_పట్టిక_ref(s->ctx_cache);

    ఒకవేళ (బహుళకం != RISCV_IOMMU_DDTP_బహుళకం_OFF &&
        బహుళకం != RISCV_IOMMU_DDTP_బహుళకం_BARE) {
        ctx = g_hash_పట్టిక_lookup(ctx_cache, &కీలకం);

        ఒకవేళ (ctx && (ctx->tc & RISCV_IOMMU_DC_TC_V)) {
            *ref = ctx_cache;
            తిరిగి ctx;
        }
    }

    ctx = g_new0(RISCVIOMMUContext, 1);
    ctx->devid = devid;
    ctx->process_id = process_id;

    పూర్ణాంకం fault = riscv_iommu_ctx_fetch(s, ctx);
    ఒకవేళ (!fault) {
        ఒకవేళ (బహుళకం != RISCV_IOMMU_DDTP_బహుళకం_BARE) {
            ఒకవేళ (g_hash_పట్టిక_కొలత(ctx_cache) >= హద్దు_CACHE_CTX) {
                g_hash_పట్టిక_unref(ctx_cache);
                ctx_cache = g_hash_పట్టిక_new_నిండిన(riscv_iommu_ctx_hash,
                                                  riscv_iommu_ctx_equal,
                                                  g_free, ఖాళీ);
                g_hash_పట్టిక_ref(ctx_cache);
                g_hash_పట్టిక_unref(qatomic_xchg(&s->ctx_cache, ctx_cache));
            }

            g_hash_పట్టిక_కూడు(ctx_cache, ctx);
            *ref = ctx_cache;
        } లేకపోతే {
            g_hash_పట్టిక_unref(ctx_cache);
            /* Remember ctx so it can be freed */
            *ref = ctx;
        }
        తిరిగి ctx;
    }

    g_hash_పట్టిక_unref(ctx_cache);
    *ref = ఖాళీ;

    /*
     * TODO: (1) do we need to distinguish other fault types
     * for ctx fetching and (2) evaluate putting the 'fault_type'
     * logic inside riscv_iommu_report_fault() - there's at
     * least one other place (end of riscv_iommu_translate())
     * that does something similar.
     */
    ఒకవేళ (perm & IOMMU_RO) {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
    } లేకపోతే {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    }

    riscv_iommu_report_fault(s, ctx, fault_type, fault,
                             !!process_id, iova, 0);

    g_free(ctx);
    తిరిగి ఖాళీ;
}

స్థిర శూన్య riscv_iommu_ctx_put(RISCVIOMMUState *s, శూన్య *ref)
{
    గుర్తులేని బహుళకం = get_క్షేత్రం(s->ddtp, RISCV_IOMMU_DDTP_బహుళకం);

    ఒకవేళ (!ref) {
        తిరిగి;
    }

    /* ref is pointing to ctx in Bare mode. Bare mode ctx is not cached */
    ఒకవేళ (బహుళకం == RISCV_IOMMU_DDTP_బహుళకం_BARE) {
        g_free(ref);
    } లేకపోతే {
        g_hash_పట్టిక_unref((GHash_పట్టిక *)ref);
    }
}

/* Find or allocate address space for a given device */
స్థిర చిరునామా_Space *riscv_iommu_space(RISCVIOMMUState *s, PCIBus *bus,
                                            పూర్ణాంకం devfn)
{
    RISCVIOMMUSpace *as;

    QLIST_FOREACH(as, &s->spaces, list) {
        ఒకవేళ (as->bus == bus && as->devfn == devfn) {
            ఆపు;
        }
    }

    ఒకవేళ (as == ఖాళీ) {
        అక్షరం name[64];
        as = g_new0(RISCVIOMMUSpace, 1);

        as->iommu = s;
        as->bus = bus;
        as->devfn = devfn;

        snprintf(name, పరిమాణం(name), "riscv-iommu-%04x:%02x.%d-iova",
            pci_bus_అంకె(bus), PCI_స్థానం(devfn), PCI_FUNC(devfn));

        /* IOVA address space, untranslated addresses */
        జ్ఞాపకం_region_init_iommu(&as->iova_mr, పరిమాణం(as->iova_mr),
            TYPE_RISCV_IOMMU_జ్ఞాపకం_REGION,
            OBJECT(s), "riscv_iommu", UINT64_MAX);
        చిరునామా_space_init(&as->iova_as, జ్ఞాపకం_REGION(&as->iova_mr), name);

        QLIST_చొప్పించు_తల(&s->spaces, as, list);

        trace_riscv_iommu_new(s->మాతృ_obj.id, pci_bus_అంకె(bus),
                PCI_స్థానం(devfn), PCI_FUNC(devfn));
    }
    తిరిగి &as->iova_as;
}

/* Translation Object cache support */
స్థిర gboolean riscv_iommu_iot_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUEntry *t1 = (RISCVIOMMUEntry *) v1;
    RISCVIOMMUEntry *t2 = (RISCVIOMMUEntry *) v2;
    తిరిగి t1->gscid == t2->gscid && t1->pscid == t2->pscid &&
           t1->iova == t2->iova && t1->ట్యాగ్ == t2->ట్యాగ్;
}

స్థిర guint riscv_iommu_iot_hash(gconstpointer v)
{
    RISCVIOMMUEntry *t = (RISCVIOMMUEntry *) v;
    తిరిగి (guint)t->iova;
}

/* GV: 0 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 0 AV: 0 GVMA: 1 */
స్థిర
శూన్య riscv_iommu_iot_inval_all(gpointer కీలకం, gpointer విలువ, gpointer దత్తాంశం)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) విలువ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) దత్తాంశం;
    ఒకవేళ (iot->ట్యాగ్ == arg->ట్యాగ్) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 0 PSCV: 1 GVMA: 0 */
స్థిర
శూన్య riscv_iommu_iot_inval_pscid(gpointer కీలకం, gpointer విలువ, gpointer దత్తాంశం)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) విలువ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) దత్తాంశం;
    ఒకవేళ (iot->ట్యాగ్ == arg->ట్యాగ్ &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 0 GVMA: 0 */
స్థిర
శూన్య riscv_iommu_iot_inval_iova(gpointer కీలకం, gpointer విలువ, gpointer దత్తాంశం)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) విలువ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) దత్తాంశం;
    ఒకవేళ (iot->ట్యాగ్ == arg->ట్యాగ్ &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 1 GVMA: 0 */
స్థిర శూన్య riscv_iommu_iot_inval_pscid_iova(gpointer కీలకం, gpointer విలువ,
                                             gpointer దత్తాంశం)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) విలువ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) దత్తాంశం;
    ఒకవేళ (iot->ట్యాగ్ == arg->ట్యాగ్ &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 0 GVMA: 1 */
స్థిర
శూన్య riscv_iommu_iot_inval_gscid(gpointer కీలకం, gpointer విలువ, gpointer దత్తాంశం)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) విలువ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) దత్తాంశం;
    ఒకవేళ (iot->ట్యాగ్ == arg->ట్యాగ్ &&
        iot->gscid == arg->gscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 1 GVMA: 0 */
స్థిర శూన్య riscv_iommu_iot_inval_gscid_pscid(gpointer కీలకం, gpointer విలువ,
                                              gpointer దత్తాంశం)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) విలువ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) దత్తాంశం;
    ఒకవేళ (iot->ట్యాగ్ == arg->ట్యాగ్ &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 1 GVMA: 1 */
స్థిర శూన్య riscv_iommu_iot_inval_gscid_iova(gpointer కీలకం, gpointer విలువ,
                                             gpointer దత్తాంశం)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) విలువ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) దత్తాంశం;
    ఒకవేళ (iot->ట్యాగ్ == arg->ట్యాగ్ &&
        iot->gscid == arg->gscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 1 GVMA: 0 */
స్థిర శూన్య riscv_iommu_iot_inval_gscid_pscid_iova(gpointer కీలకం, gpointer విలువ,
                                                   gpointer దత్తాంశం)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) విలువ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) దత్తాంశం;
    ఒకవేళ (iot->ట్యాగ్ == arg->ట్యాగ్ &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* caller should keep ref-count for iot_cache object */
స్థిర RISCVIOMMUEntry *riscv_iommu_iot_lookup(RISCVIOMMUContext *ctx,
    GHash_పట్టిక *iot_cache, hwaddr iova, RISCVIOMMUTrans_ట్యాగ్ transtag)
{
    RISCVIOMMUEntry కీలకం = {
        .ట్యాగ్   = transtag,
        .gscid = get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID),
        .pscid = get_క్షేత్రం(ctx->ta, RISCV_IOMMU_DC_TA_PSCID),
        .iova  = PPN_కిందికి(iova),
    };
    తిరిగి g_hash_పట్టిక_lookup(iot_cache, &కీలకం);
}

/* caller should keep ref-count for iot_cache object */
స్థిర శూన్య riscv_iommu_iot_update(RISCVIOMMUState *s,
    GHash_పట్టిక *iot_cache, RISCVIOMMUEntry *iot)
{
    ఒకవేళ (!s->iot_హద్దు) {
        తిరిగి;
    }

    ఒకవేళ (g_hash_పట్టిక_కొలత(s->iot_cache) >= s->iot_హద్దు) {
        iot_cache = g_hash_పట్టిక_new_నిండిన(riscv_iommu_iot_hash,
                                          riscv_iommu_iot_equal,
                                          g_free, ఖాళీ);
        g_hash_పట్టిక_unref(qatomic_xchg(&s->iot_cache, iot_cache));
    }
    g_hash_పట్టిక_కూడు(iot_cache, iot);
}

స్థిర శూన్య riscv_iommu_iot_inval(RISCVIOMMUState *s, GHFunc func,
    uint32_t gscid, uint32_t pscid, hwaddr iova, RISCVIOMMUTrans_ట్యాగ్ transtag)
{
    GHash_పట్టిక *iot_cache;
    RISCVIOMMUEntry కీలకం = {
        .ట్యాగ్ = transtag,
        .gscid = gscid,
        .pscid = pscid,
        .iova  = PPN_కిందికి(iova),
    };

    iot_cache = g_hash_పట్టిక_ref(s->iot_cache);
    g_hash_పట్టిక_foreach(iot_cache, func, &కీలకం);
    g_hash_పట్టిక_unref(iot_cache);
}

స్థిర RISCVIOMMUTrans_ట్యాగ్ riscv_iommu_get_transtag(RISCVIOMMUContext *ctx)
{
    uint64_t satp = get_క్షేత్రం(ctx->satp, RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం);
    uint64_t gatp = get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం);

    ఒకవేళ (satp == RISCV_IOMMU_DC_FSC_బహుళకం_BARE) {
        తిరిగి (gatp == RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE) ?
            RISCV_IOMMU_TRANS_ట్యాగ్_BY : RISCV_IOMMU_TRANS_ట్యాగ్_VG;
    } లేకపోతే {
        తిరిగి (gatp == RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE) ?
            RISCV_IOMMU_TRANS_ట్యాగ్_SS : RISCV_IOMMU_TRANS_ట్యాగ్_VN;
    }
}

స్థిర పూర్ణాంకం riscv_iommu_translate(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb, bool enable_cache)
{
    RISCVIOMMUTrans_ట్యాగ్ transtag = riscv_iommu_get_transtag(ctx);
    RISCVIOMMUEntry *iot;
    IOMMUAccessFlags perm;
    bool enable_pid;
    bool enable_pri;
    GHash_పట్టిక *iot_cache;
    పూర్ణాంకం fault;

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_URQ);

    iot_cache = g_hash_పట్టిక_ref(s->iot_cache);
    enable_pri = (iotlb->perm == IOMMU_NONE) &&
                 (ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI);
    enable_pid = (ctx->tc & RISCV_IOMMU_DC_TC_PDTV);

    /* Check for ATS request. */
    ఒకవేళ (iotlb->perm == IOMMU_NONE) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_ATS_RQ);
        /* Check if ATS is disabled. */
        ఒకవేళ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS)) {
            enable_pri = false;
            fault = RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
            వెళ్ళు పూర్తి;
        }
    }

    iot = riscv_iommu_iot_lookup(ctx, iot_cache, iotlb->iova, transtag);
    perm = iot ? iot->perm : IOMMU_NONE;
    ఒకవేళ (perm != IOMMU_NONE) {
        iotlb->translated_addr = PPN_PHYS(iot->phys);
        iotlb->addr_ముసుగు = ~లక్ష్యం_పుట_ముసుగు;
        iotlb->perm = perm;
        fault = 0;
        వెళ్ళు పూర్తి;
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_TLB_తప్పిపోవు);

    /* Translate using device directory / page table information. */
    fault = riscv_iommu_spa_fetch(s, ctx, iotlb);

    ఒకవేళ (!fault && iotlb->లక్ష్యం_as == &s->trap_as) {
        /* Do not cache trapped MSI translations */
        వెళ్ళు పూర్తి;
    }

    /*
     * We made an implementation choice to not cache identity-mapped
     * translations, as allowed by the specification, to avoid
     * translation cache evictions for other devices sharing the
     * IOMMU hardware model.
     */
    ఒకవేళ (!fault && iotlb->translated_addr != iotlb->iova && enable_cache) {
        iot = g_new0(RISCVIOMMUEntry, 1);
        iot->iova = PPN_కిందికి(iotlb->iova);
        iot->phys = PPN_కిందికి(iotlb->translated_addr);
        iot->gscid = get_క్షేత్రం(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
        iot->pscid = get_క్షేత్రం(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        iot->perm = iotlb->perm;
        iot->ట్యాగ్ = transtag;
        riscv_iommu_iot_update(s, iot_cache, iot);
    }

పూర్తి:
    g_hash_పట్టిక_unref(iot_cache);

    ఒకవేళ (enable_pri && fault) {
        నిర్మాణం riscv_iommu_pq_రికార్డు pr = {0};
        ఒకవేళ (enable_pid) {
            pr.hdr = సమితి_క్షేత్రం(RISCV_IOMMU_PREQ_HDR_PV,
                               RISCV_IOMMU_PREQ_HDR_PID, ctx->process_id);
        }
        pr.hdr = సమితి_క్షేత్రం(pr.hdr, RISCV_IOMMU_PREQ_HDR_DID, ctx->devid);
        pr.payload = (iotlb->iova & లక్ష్యం_పుట_ముసుగు) |
                     RISCV_IOMMU_PREQ_PAYLOAD_M;
        riscv_iommu_pri(s, &pr);
        తిరిగి fault;
    }

    ఒకవేళ (fault) {
        గుర్తులేని ttype = RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ;

        ఒకవేళ ((iotlb->perm & IOMMU_RW) == IOMMU_RW
            || iotlb->perm & IOMMU_WO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
        } లేకపోతే ఒకవేళ (iotlb->perm & IOMMU_RO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
        }

        riscv_iommu_report_fault(s, ctx, ttype, fault, enable_pid,
                                 iotlb->iova, iotlb->translated_addr);
        తిరిగి fault;
    }

    తిరిగి 0;
}

/* IOMMU Command Interface */
స్థిర Mem_Tx_ఫలితం riscv_iommu_iofence(RISCVIOMMUState *s, bool notify,
    uint64_t addr, uint32_t దత్తాంశం)
{
    /*
     * ATS processing in this implementation of the IOMMU is synchronous,
     * no need to wait for completions here.
     */
    ఒకవేళ (!notify) {
        తిరిగి MEMTX_OK;
    }

    తిరిగి dma_జ్ఞాపకం_write(s->లక్ష్యం_as, addr, &దత్తాంశం, పరిమాణం(దత్తాంశం),
        MEMTXATTRS_UNSPECIFIED);
}

స్థిర శూన్య riscv_iommu_ats(RISCVIOMMUState *s,
    నిర్మాణం riscv_iommu_command *cmd, IOMMUNotifierFlag flag,
    IOMMUAccessFlags perm,
    శూన్య (*trace_fn)(స్థిరరాశి అక్షరం *id))
{
    RISCVIOMMUSpace *as;
    IOMMUNotifier *n;
    IOMMUTLBEvent event;
    uint32_t pid;
    uint32_t devid;
    స్థిరరాశి bool pv = cmd->dword0 & RISCV_IOMMU_CMD_ATS_PV;

    ఒకవేళ (cmd->dword0 & RISCV_IOMMU_CMD_ATS_DSV) {
        /* Use device segment and requester id */
        devid = get_క్షేత్రం(cmd->dword0,
            RISCV_IOMMU_CMD_ATS_DSEG | RISCV_IOMMU_CMD_ATS_RID);
    } లేకపోతే {
        devid = get_క్షేత్రం(cmd->dword0, RISCV_IOMMU_CMD_ATS_RID);
    }

    pid = get_క్షేత్రం(cmd->dword0, RISCV_IOMMU_CMD_ATS_PID);

    QLIST_FOREACH(as, &s->spaces, list) {
        ఒకవేళ (riscv_iommu_space_devid(as) == devid) {
            ఆపు;
        }
    }

    ఒకవేళ (!as || !as->notifier) {
        తిరిగి;
    }

    event.type = flag;
    event.నమోదు_2.perm = perm;
    event.నమోదు_2.లక్ష్యం_as = s->లక్ష్యం_as;

    IOMMU_NOTIFIER_FOREACH(n, &as->iova_mr) {
        ఒకవేళ (!pv || n->iommu_idx == pid) {
            event.నమోదు_2.iova = n->ప్రారంభం;
            event.నమోదు_2.addr_ముసుగు = n->చివర - n->ప్రారంభం;
            trace_fn(as->iova_mr.మాతృ_obj.name);
            జ్ఞాపకం_region_notify_iommu_one(n, &event);
        }
    }
}

స్థిర శూన్య riscv_iommu_ats_inval(RISCVIOMMUState *s,
    నిర్మాణం riscv_iommu_command *cmd)
{
    తిరిగి riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_DEVIOTLB_UNMAP, IOMMU_NONE,
                           trace_riscv_iommu_ats_inval);
}

స్థిర శూన్య riscv_iommu_ats_prgr(RISCVIOMMUState *s,
    నిర్మాణం riscv_iommu_command *cmd)
{
    గుర్తులేని resp_సంకేతం = get_క్షేత్రం(cmd->dword1,
                                   RISCV_IOMMU_CMD_ATS_PRGR_RESP_సంకేతం);

    /* Using the access flag to carry response code information */
    IOMMUAccessFlags perm = resp_సంకేతం ? IOMMU_NONE : IOMMU_RW;
    తిరిగి riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_MAP, perm,
                           trace_riscv_iommu_ats_prgr);
}

స్థిర శూన్య riscv_iommu_process_ddtp(RISCVIOMMUState *s)
{
    uint64_t old_ddtp = s->ddtp;
    uint64_t new_ddtp = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_DDTP);
    గుర్తులేని new_బహుళకం = get_క్షేత్రం(new_ddtp, RISCV_IOMMU_DDTP_బహుళకం);
    గుర్తులేని old_బహుళకం = get_క్షేత్రం(old_ddtp, RISCV_IOMMU_DDTP_బహుళకం);
    bool ok = false;

    /*
     * Check for allowed DDTP.MODE transitions:
     * {OFF, BARE}        -> {OFF, BARE, 1LVL, 2LVL, 3LVL}
     * {1LVL, 2LVL, 3LVL} -> {OFF, BARE}
     */
    ఒకవేళ (new_బహుళకం == old_బహుళకం ||
        new_బహుళకం == RISCV_IOMMU_DDTP_బహుళకం_OFF ||
        new_బహుళకం == RISCV_IOMMU_DDTP_బహుళకం_BARE) {
        ok = true;
    } లేకపోతే ఒకవేళ (new_బహుళకం == RISCV_IOMMU_DDTP_బహుళకం_1_LVL ||
               new_బహుళకం == RISCV_IOMMU_DDTP_బహుళకం_2_LVL ||
               new_బహుళకం == RISCV_IOMMU_DDTP_బహుళకం_3_LVL) {
        ok = old_బహుళకం == RISCV_IOMMU_DDTP_బహుళకం_OFF ||
             old_బహుళకం == RISCV_IOMMU_DDTP_బహుళకం_BARE;
    }

    ఒకవేళ (ok) {
        /* clear reserved and busy bits, report back sanitized version */
        new_ddtp = సమితి_క్షేత్రం(new_ddtp & RISCV_IOMMU_DDTP_PPN,
                             RISCV_IOMMU_DDTP_బహుళకం, new_బహుళకం);
    } లేకపోతే {
        new_ddtp = old_ddtp;
    }
    s->ddtp = new_ddtp;

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, new_ddtp);
}

/* Command function and opcode field. */
#నిర్వచించు RISCV_IOMMU_CMD(func, op) (((func) << 7) | (op))

స్థిర శూన్య riscv_iommu_process_cq_తోక(RISCVIOMMUState *s)
{
    నిర్మాణం riscv_iommu_command cmd;
    Mem_Tx_ఫలితం res;
    dma_addr_t addr;
    uint32_t తోక, తల, ctrl;
    uint64_t cmd_opcode;
    GHFunc func;

    ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    తోక = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQT) & s->cq_ముసుగు;
    తల = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQH) & s->cq_ముసుగు;

    /* Check for pending error or queue processing disabled */
    ఒకవేళ (!(ctrl & RISCV_IOMMU_CQCSR_CQON) ||
        !!(ctrl & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        తిరిగి;
    }

    అయితే (తోక != తల) {
        addr = s->cq_addr  + తల * పరిమాణం(cmd);
        res = dma_జ్ఞాపకం_read(s->లక్ష్యం_as, addr, &cmd, పరిమాణం(cmd),
                              MEMTXATTRS_UNSPECIFIED);

        ఒకవేళ (res != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                  RISCV_IOMMU_CQCSR_CQMF, 0);
            వెళ్ళు fault;
        }

        trace_riscv_iommu_cmd(s->మాతృ_obj.id, cmd.dword0, cmd.dword1);

        cmd_opcode = get_క్షేత్రం(cmd.dword0,
                               RISCV_IOMMU_CMD_OPCODE | RISCV_IOMMU_CMD_FUNC);

        ఎంపిక (cmd_opcode) {
        స్థితి RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOFENCE_FUNC_C,
                             RISCV_IOMMU_CMD_IOFENCE_OPCODE):
            ఒకవేళ (cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_RESERVED) {
                వెళ్ళు cmd_ill;
            }

            res = riscv_iommu_iofence(s,
                cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_AV, cmd.dword1 << 2,
                get_క్షేత్రం(cmd.dword0, RISCV_IOMMU_CMD_IOFENCE_దత్తాంశం));

            ఒకవేళ (res != MEMTX_OK) {
                riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                      RISCV_IOMMU_CQCSR_CQMF, 0);
                వెళ్ళు fault;
            }
            ఆపు;

        స్థితి RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_క్షేత్రం(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_క్షేత్రం(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & లక్ష్యం_పుట_ముసుగు;

            ఒకవేళ (pscv) {
                /* illegal command arguments IOTINVAL.GVMA & PSCV == 1 */
                వెళ్ళు cmd_ill;
            }

            func = riscv_iommu_iot_inval_all;

            ఒకవేళ (gv) {
                func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                              riscv_iommu_iot_inval_gscid;
            }

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_ట్యాగ్_VG);

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_ట్యాగ్_VN);
            ఆపు;
        }

        స్థితి RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_క్షేత్రం(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_క్షేత్రం(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & లక్ష్యం_పుట_ముసుగు;
            RISCVIOMMUTrans_ట్యాగ్ transtag;

            ఒకవేళ (gv) {
                transtag = RISCV_IOMMU_TRANS_ట్యాగ్_VN;
                ఒకవేళ (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_gscid_pscid_iova :
                                  riscv_iommu_iot_inval_gscid_pscid;
                } లేకపోతే {
                    func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                                  riscv_iommu_iot_inval_gscid;
                }
            } లేకపోతే {
                transtag = RISCV_IOMMU_TRANS_ట్యాగ్_SS;
                ఒకవేళ (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_pscid_iova :
                                  riscv_iommu_iot_inval_pscid;
                } లేకపోతే {
                    func = (av) ? riscv_iommu_iot_inval_iova :
                                  riscv_iommu_iot_inval_all;
                }
            }

            riscv_iommu_iot_inval(s, func, gscid, pscid, iova, transtag);
            ఆపు;
        }

        స్థితి RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            ఒకవేళ (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* invalidate all device context cache mappings */
                func = riscv_iommu_ctx_inval_all;
            } లేకపోతే {
                /* invalidate all device context matching DID */
                func = riscv_iommu_ctx_inval_devid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_క్షేత్రం(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID), 0);
            ఆపు;

        స్థితి RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            ఒకవేళ (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* illegal command arguments IODIR_PDT & DV == 0 */
                వెళ్ళు cmd_ill;
            } లేకపోతే {
                func = riscv_iommu_ctx_inval_devid_procid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_క్షేత్రం(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID),
                get_క్షేత్రం(cmd.dword0, RISCV_IOMMU_CMD_IODIR_PID));
            ఆపు;

        /* ATS commands */
        స్థితి RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_INVAL,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            ఒకవేళ (!s->enable_ats) {
                వెళ్ళు cmd_ill;
            }

            riscv_iommu_ats_inval(s, &cmd);
            ఆపు;

        స్థితి RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_PRGR,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            ఒకవేళ (!s->enable_ats) {
                వెళ్ళు cmd_ill;
            }

            riscv_iommu_ats_prgr(s, &cmd);
            ఆపు;

        అప్రమేయ:
        cmd_ill:
            /* Invalid instruction, do not advance instruction index. */
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                RISCV_IOMMU_CQCSR_CMD_ILL, 0);
            వెళ్ళు fault;
        }

        /* Advance and update head pointer after command completes. */
        తల = (తల + 1) & s->cq_ముసుగు;
        riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_CQH, తల);
    }
    తిరిగి;

fault:
    ఒకవేళ (ctrl & RISCV_IOMMU_CQCSR_CIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_CQ);
    }
}

స్థిర శూన్య riscv_iommu_process_cq_control(RISCVIOMMUState *s)
{
    uint64_t పునాది;
    uint32_t ctrl_సమితి = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_సమితి & RISCV_IOMMU_CQCSR_CQEN);
    bool చురుకైన = !!(ctrl_సమితి & RISCV_IOMMU_CQCSR_CQON);

    ఒకవేళ (enable && !చురుకైన) {
        పునాది = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_CQB);
        s->cq_ముసుగు = (2ULL << get_క్షేత్రం(పునాది, RISCV_IOMMU_CQB_LOG2SZ)) - 1;
        s->cq_addr = PPN_PHYS(get_క్షేత్రం(పునాది, RISCV_IOMMU_CQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~s->cq_ముసుగు);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQT], 0);
        ctrl_సమితి = RISCV_IOMMU_CQCSR_CQON;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQMF |
                   RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CMD_గమ్యం |
                   RISCV_IOMMU_CQCSR_FENCE_W_IP;
    } లేకపోతే ఒకవేళ (!enable && చురుకైన) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~0);
        ctrl_సమితి = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQON;
    } లేకపోతే {
        ctrl_సమితి = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, ctrl_సమితి, ctrl_clr);

    /*
     * After clearing error bits (CMD_ILL, CQMF), if queue is still active,
     * re-process pending command.
     */
    ctrl_సమితి = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    ఒకవేళ ((ctrl_సమితి & RISCV_IOMMU_CQCSR_CQON) &&
        !(ctrl_సమితి & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        riscv_iommu_process_cq_తోక(s);
    }
}

స్థిర శూన్య riscv_iommu_process_fq_control(RISCVIOMMUState *s)
{
    uint64_t పునాది;
    uint32_t ctrl_సమితి = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_సమితి & RISCV_IOMMU_FQCSR_FQEN);
    bool చురుకైన = !!(ctrl_సమితి & RISCV_IOMMU_FQCSR_FQON);

    ఒకవేళ (enable && !చురుకైన) {
        పునాది = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_FQB);
        s->fq_ముసుగు = (2ULL << get_క్షేత్రం(పునాది, RISCV_IOMMU_FQB_LOG2SZ)) - 1;
        s->fq_addr = PPN_PHYS(get_క్షేత్రం(పునాది, RISCV_IOMMU_FQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~s->fq_ముసుగు);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQT], 0);
        ctrl_సమితి = RISCV_IOMMU_FQCSR_FQON;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQMF |
            RISCV_IOMMU_FQCSR_FQOF;
    } లేకపోతే ఒకవేళ (!enable && చురుకైన) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~0);
        ctrl_సమితి = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQON;
    } లేకపోతే {
        ctrl_సమితి = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, ctrl_సమితి, ctrl_clr);
}

స్థిర శూన్య riscv_iommu_process_pq_control(RISCVIOMMUState *s)
{
    uint64_t పునాది;
    uint32_t ctrl_సమితి = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_సమితి & RISCV_IOMMU_PQCSR_PQEN);
    bool చురుకైన = !!(ctrl_సమితి & RISCV_IOMMU_PQCSR_PQON);

    ఒకవేళ (enable && !చురుకైన) {
        పునాది = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_PQB);
        s->pq_ముసుగు = (2ULL << get_క్షేత్రం(పునాది, RISCV_IOMMU_PQB_LOG2SZ)) - 1;
        s->pq_addr = PPN_PHYS(get_క్షేత్రం(పునాది, RISCV_IOMMU_PQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~s->pq_ముసుగు);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQT], 0);
        ctrl_సమితి = RISCV_IOMMU_PQCSR_PQON;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQMF |
            RISCV_IOMMU_PQCSR_PQOF;
    } లేకపోతే ఒకవేళ (!enable && చురుకైన) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~0);
        ctrl_సమితి = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQON;
    } లేకపోతే {
        ctrl_సమితి = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, ctrl_సమితి, ctrl_clr);
}

స్థిర శూన్య riscv_iommu_process_dbg(RISCVIOMMUState *s)
{
    uint64_t iova = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_IOVA);
    uint64_t ctrl = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_CTL);
    గుర్తులేని devid = get_క్షేత్రం(ctrl, RISCV_IOMMU_TR_REQ_CTL_DID);
    గుర్తులేని pid = get_క్షేత్రం(ctrl, RISCV_IOMMU_TR_REQ_CTL_PID);
    IOMMUAccessFlags perm = ctrl & RISCV_IOMMU_TR_REQ_CTL_NW
                            ? IOMMU_RO : IOMMU_RW;
    RISCVIOMMUContext *ctx;
    శూన్య *ref;

    ఒకవేళ (!(ctrl & RISCV_IOMMU_TR_REQ_CTL_GO_BUSY)) {
        తిరిగి;
    }

    ctx = riscv_iommu_ctx(s, devid, pid, perm, iova, &ref);
    ఒకవేళ (ctx == ఖాళీ) {
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE,
                                 RISCV_IOMMU_TR_RESPONSE_FAULT |
                                 (RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED << 10));
    } లేకపోతే {
        IOMMUTLBEntry iotlb = {
            .iova = iova,
            .perm = perm,
            .addr_ముసుగు = ~0,
            .లక్ష్యం_as = ఖాళీ,
        };
        పూర్ణాంకం fault = riscv_iommu_translate(s, ctx, &iotlb, false);
        ఒకవేళ (fault) {
            iova = RISCV_IOMMU_TR_RESPONSE_FAULT | (((uint64_t) fault) << 10);
        } లేకపోతే {
            iova = iotlb.translated_addr & ~iotlb.addr_ముసుగు;
            iova = సమితి_క్షేత్రం(0, RISCV_IOMMU_TR_RESPONSE_PPN, PPN_కిందికి(iova));
        }
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE, iova);
    }

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
        RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    riscv_iommu_ctx_put(s, ref);
}

రకనిర్వచనం శూన్య riscv_iommu_process_fn(RISCVIOMMUState *s);

స్థిర శూన్య riscv_iommu_update_icvec(RISCVIOMMUState *s, uint64_t దత్తాంశం)
{
    uint64_t icvec = 0;

    icvec |= MIN(దత్తాంశం & RISCV_IOMMU_ICVEC_CIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_CIV);

    icvec |= MIN(దత్తాంశం & RISCV_IOMMU_ICVEC_FIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_FIV);

    icvec |= MIN(దత్తాంశం & RISCV_IOMMU_ICVEC_PMIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PMIV);

    icvec |= MIN(దత్తాంశం & RISCV_IOMMU_ICVEC_PIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PIV);

    trace_riscv_iommu_icvec_write(దత్తాంశం, icvec);

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_ICVEC, icvec);
}

స్థిర శూన్య riscv_iommu_update_ipsr(RISCVIOMMUState *s, uint64_t దత్తాంశం)
{
    uint32_t cqcsr, fqcsr, pqcsr;
    uint32_t ipsr_సమితి = 0;
    uint32_t ipsr_clr = 0;

    ఒకవేళ (దత్తాంశం & RISCV_IOMMU_IPSR_CIP) {
        cqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);

        ఒకవేళ (cqcsr & RISCV_IOMMU_CQCSR_CIE &&
            (cqcsr & RISCV_IOMMU_CQCSR_FENCE_W_IP ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ILL ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_గమ్యం ||
             cqcsr & RISCV_IOMMU_CQCSR_CQMF)) {
            ipsr_సమితి |= RISCV_IOMMU_IPSR_CIP;
        } లేకపోతే {
            ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
        }
    } లేకపోతే {
        ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
    }

    ఒకవేళ (దత్తాంశం & RISCV_IOMMU_IPSR_FIP) {
        fqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);

        ఒకవేళ (fqcsr & RISCV_IOMMU_FQCSR_FIE &&
            (fqcsr & RISCV_IOMMU_FQCSR_FQOF ||
             fqcsr & RISCV_IOMMU_FQCSR_FQMF)) {
            ipsr_సమితి |= RISCV_IOMMU_IPSR_FIP;
        } లేకపోతే {
            ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
        }
    } లేకపోతే {
        ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
    }

    ఒకవేళ (!(దత్తాంశం & RISCV_IOMMU_IPSR_PMIP)) {
        ipsr_clr |= RISCV_IOMMU_IPSR_PMIP;
    }

    ఒకవేళ (దత్తాంశం & RISCV_IOMMU_IPSR_PIP) {
        pqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);

        ఒకవేళ (pqcsr & RISCV_IOMMU_PQCSR_PIE &&
            (pqcsr & RISCV_IOMMU_PQCSR_PQOF ||
             pqcsr & RISCV_IOMMU_PQCSR_PQMF)) {
            ipsr_సమితి |= RISCV_IOMMU_IPSR_PIP;
        } లేకపోతే {
            ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
        }
    } లేకపోతే {
        ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, ipsr_సమితి, ipsr_clr);
}

స్థిర శూన్య riscv_iommu_process_hpm_writes(RISCVIOMMUState *s,
                                           uint32_t regb,
                                           bool prev_cy_inh)
{
    ఎంపిక (regb) {
    స్థితి RISCV_IOMMU_REG_IOCOUNTINH:
        riscv_iommu_process_iocntinh_cy(s, prev_cy_inh);
        ఆపు;

    స్థితి RISCV_IOMMU_REG_IOHPMCYCLES:
    స్థితి RISCV_IOMMU_REG_IOHPMCYCLES + 4:
        riscv_iommu_process_hpmcycle_write(s);
        ఆపు;

    స్థితి RISCV_IOMMU_REG_IOHPMEVT_పునాది ...
        RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_అంకె) + 4:
        riscv_iommu_process_hpmevt_write(s, regb & ~7);
        ఆపు;
    }
}

/*
 * Write the resulting value of 'data' for the reg specified
 * by 'reg_addr', after considering read-only/read-write/write-clear
 * bits, in the pointer 'dest'.
 *
 * The result is written in little-endian.
 */
స్థిర శూన్య riscv_iommu_write_reg_val(RISCVIOMMUState *s,
                                      శూన్య *dest, hwaddr reg_addr,
                                      పూర్ణాంకం కొలత, uint64_t దత్తాంశం)
{
    uint64_t ro = ldn_le_p(&s->regs_ro[reg_addr], కొలత);
    uint64_t wc = ldn_le_p(&s->regs_wc[reg_addr], కొలత);
    uint64_t curr_val = ldn_le_p(&s->regs[reg_addr], కొలత);

    stn_le_p(dest, కొలత, ((curr_val & ro) | (దత్తాంశం & ~ro)) & ~(దత్తాంశం & wc));
}

స్థిర Mem_Tx_ఫలితం riscv_iommu_mmio_write(శూన్య *opaque, hwaddr addr,
                                          uint64_t దత్తాంశం, గుర్తులేని కొలత,
                                          MemTxAttrs attrs)
{
    riscv_iommu_process_fn *process_fn = ఖాళీ;
    RISCVIOMMUState *s = opaque;
    uint32_t regb = addr & ~3;
    uint32_t busy = 0;
    uint64_t val = 0;
    bool cy_inh = false;

    ఒకవేళ ((addr & (కొలత - 1)) != 0) {
        /* Unsupported MMIO alignment or access size */
        తిరిగి MEMTX_ERROR;
    }

    ఒకవేళ (addr + కొలత > RISCV_IOMMU_REG_MSI_CONFIG) {
        /* Unsupported MMIO access location. */
        తిరిగి MEMTX_ACCESS_ERROR;
    }

    /* Track actionable MMIO write. */
    ఎంపిక (regb) {
    స్థితి RISCV_IOMMU_REG_DDTP:
    స్థితి RISCV_IOMMU_REG_DDTP + 4:
        process_fn = riscv_iommu_process_ddtp;
        regb = RISCV_IOMMU_REG_DDTP;
        busy = RISCV_IOMMU_DDTP_BUSY;
        ఆపు;

    స్థితి RISCV_IOMMU_REG_CQT:
        process_fn = riscv_iommu_process_cq_తోక;
        ఆపు;

    స్థితి RISCV_IOMMU_REG_CQCSR:
        process_fn = riscv_iommu_process_cq_control;
        busy = RISCV_IOMMU_CQCSR_BUSY;
        ఆపు;

    స్థితి RISCV_IOMMU_REG_FQCSR:
        process_fn = riscv_iommu_process_fq_control;
        busy = RISCV_IOMMU_FQCSR_BUSY;
        ఆపు;

    స్థితి RISCV_IOMMU_REG_PQCSR:
        process_fn = riscv_iommu_process_pq_control;
        busy = RISCV_IOMMU_PQCSR_BUSY;
        ఆపు;

    స్థితి RISCV_IOMMU_REG_ICVEC:
    స్థితి RISCV_IOMMU_REG_IPSR:
        /*
         * ICVEC and IPSR have special read/write procedures. We'll
         * call their respective helpers and exit.
         */
        riscv_iommu_write_reg_val(s, &val, addr, కొలత, దత్తాంశం);

        /*
         * 'val' is stored as LE. Switch to host endianess
         * before using it.
         */
        val = le64_గమ్యం_cpu(val);

        ఒకవేళ (regb == RISCV_IOMMU_REG_ICVEC) {
            riscv_iommu_update_icvec(s, val);
        } లేకపోతే {
            riscv_iommu_update_ipsr(s, val);
        }

        తిరిగి MEMTX_OK;

    స్థితి RISCV_IOMMU_REG_TR_REQ_CTL:
        process_fn = riscv_iommu_process_dbg;
        regb = RISCV_IOMMU_REG_TR_REQ_CTL;
        busy = RISCV_IOMMU_TR_REQ_CTL_GO_BUSY;
        ఆపు;

    స్థితి RISCV_IOMMU_REG_IOCOUNTINH:
        ఒకవేళ (addr != RISCV_IOMMU_REG_IOCOUNTINH) {
            ఆపు;
        }
        /* Store previous value of CY bit. */
        cy_inh = !!(riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTINH) &
            RISCV_IOMMU_IOCOUNTINH_CY);
        ఆపు;


    అప్రమేయ:
        ఆపు;
    }

    /*
     * Registers update might be not synchronized with core logic.
     * If system software updates register when relevant BUSY bit
     * is set IOMMU behavior of additional writes to the register
     * is UNSPECIFIED.
     */
    riscv_iommu_write_reg_val(s, &s->regs[addr], addr, కొలత, దత్తాంశం);

    /* Busy flag update, MSB 4-byte register. */
    ఒకవేళ (busy) {
        uint32_t rw = ldl_le_p(&s->regs[regb]);
        stl_le_p(&s->regs[regb], rw | busy);
    }

    /* Process HPM writes and update any internal state if needed. */
    ఒకవేళ (regb >= RISCV_IOMMU_REG_IOCOUNTOVF &&
        regb <= (RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_అంకె) + 4)) {
        riscv_iommu_process_hpm_writes(s, regb, cy_inh);
    }

    ఒకవేళ (process_fn) {
        process_fn(s);
    }

    తిరిగి MEMTX_OK;
}

స్థిర Mem_Tx_ఫలితం riscv_iommu_mmio_read(శూన్య *opaque, hwaddr addr,
    uint64_t *దత్తాంశం, గుర్తులేని కొలత, MemTxAttrs attrs)
{
    RISCVIOMMUState *s = opaque;
    uint64_t val = -1;
    uint8_t *ptr;

    ఒకవేళ ((addr & (కొలత - 1)) != 0) {
        /* Unsupported MMIO alignment. */
        తిరిగి MEMTX_ERROR;
    }

    ఒకవేళ (addr + కొలత > RISCV_IOMMU_REG_MSI_CONFIG) {
        తిరిగి MEMTX_ACCESS_ERROR;
    }

    /* Compute cycle register value. */
    ఒకవేళ ((addr & ~7) == RISCV_IOMMU_REG_IOHPMCYCLES) {
        val = riscv_iommu_hpmcycle_read(s);
        ptr = (uint8_t *)&val + (addr & 7);
    } లేకపోతే ఒకవేళ ((addr & ~3) == RISCV_IOMMU_REG_IOCOUNTOVF) {
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
    } లేకపోతే {
        ptr = &s->regs[addr];
    }

    val = ldn_le_p(ptr, కొలత);

    *దత్తాంశం = val;

    తిరిగి MEMTX_OK;
}

స్థిర స్థిరరాశి జ్ఞాపకం_Region_Ops riscv_iommu_mmio_ops = {
    .read_with_attrs = riscv_iommu_mmio_read,
    .write_with_attrs = riscv_iommu_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_కొలత = 4,
        .max_access_కొలత = 8,
        .unaligned = false,
    },
    .valid = {
        .min_access_కొలత = 4,
        .max_access_కొలత = 8,
    }
};

/*
 * Translations matching MSI pattern check are redirected to "riscv-iommu-trap"
 * memory region as untranslated address, for additional MSI/MRIF interception
 * by IOMMU interrupt remapping implementation.
 * Note: Device emulation code generating an MSI is expected to provide a valid
 * memory transaction attributes with requested_id set.
 */
స్థిర Mem_Tx_ఫలితం riscv_iommu_trap_write(శూన్య *opaque, hwaddr addr,
    uint64_t దత్తాంశం, గుర్తులేని కొలత, MemTxAttrs attrs)
{
    RISCVIOMMUState* s = (RISCVIOMMUState *)opaque;
    RISCVIOMMUContext *ctx;
    Mem_Tx_ఫలితం res;
    శూన్య *ref;
    uint32_t devid = attrs.requester_id;

    ఒకవేళ (attrs.unspecified) {
        తిరిగి MEMTX_ACCESS_ERROR;
    }

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= s->bus << 8;

    ctx = riscv_iommu_ctx(s, devid, 0, IOMMU_RW, addr, &ref);
    ఒకవేళ (ctx == ఖాళీ) {
        res = MEMTX_ACCESS_ERROR;
    } లేకపోతే {
        res = riscv_iommu_msi_write(s, ctx, addr, దత్తాంశం, కొలత, attrs);
    }
    riscv_iommu_ctx_put(s, ref);
    తిరిగి res;
}

స్థిర Mem_Tx_ఫలితం riscv_iommu_trap_read(శూన్య *opaque, hwaddr addr,
    uint64_t *దత్తాంశం, గుర్తులేని కొలత, MemTxAttrs attrs)
{
    తిరిగి MEMTX_ACCESS_ERROR;
}

స్థిర స్థిరరాశి జ్ఞాపకం_Region_Ops riscv_iommu_trap_ops = {
    .read_with_attrs = riscv_iommu_trap_read,
    .write_with_attrs = riscv_iommu_trap_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_కొలత = 4,
        .max_access_కొలత = 8,
    },
    .valid = {
        .min_access_కొలత = 4,
        .max_access_కొలత = 8,
    }
};

శూన్య riscv_iommu_సమితి_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_బహుళకం బహుళకం)
{
    s->cap = సమితి_క్షేత్రం(s->cap, RISCV_IOMMU_CAP_IGS, బహుళకం);
}

స్థిర శూన్య riscv_iommu_instance_init(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    /* Enable translation debug interface */
    s->cap = RISCV_IOMMU_CAP_DBG;

    /* TODO: method to report supported PID bits */
    s->pid_బిట్లు = 8; /* restricted to size of MemTxAttrs.pid */
    s->cap |= RISCV_IOMMU_CAP_PD8;

    /* register storage */
    s->regs = g_new0(uint8_t, RISCV_IOMMU_REG_కొలత);
    s->regs_ro = g_new0(uint8_t, RISCV_IOMMU_REG_కొలత);
    s->regs_wc = g_new0(uint8_t, RISCV_IOMMU_REG_కొలత);

     /* Mark all registers read-only */
    memset(s->regs_ro, 0xff, RISCV_IOMMU_REG_కొలత);

    /* Device translation context cache */
    s->ctx_cache = g_hash_పట్టిక_new_నిండిన(riscv_iommu_ctx_hash,
                                         riscv_iommu_ctx_equal,
                                         g_free, ఖాళీ);

    s->iot_cache = g_hash_పట్టిక_new_నిండిన(riscv_iommu_iot_hash,
                                         riscv_iommu_iot_equal,
                                         g_free, ఖాళీ);

    s->iommus.le_తదుపరి = ఖాళీ;
    s->iommus.le_prev = ఖాళీ;
    QLIST_INIT(&s->spaces);
}

స్థిర శూన్య riscv_iommu_instance_finalize(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    g_free(s->regs);
    g_free(s->regs_ro);
    g_free(s->regs_wc);

    g_hash_పట్టిక_unref(s->ctx_cache);
    g_hash_పట్టిక_unref(s->iot_cache);
}

స్థిర శూన్య riscv_iommu_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    /* Report QEMU target physical address space limits. */
    s->cap = సమితి_క్షేత్రం(s->cap, RISCV_IOMMU_CAP_PAS, s->pas_బిట్లు);

    s->cap |= s->సంస్కరణ & RISCV_IOMMU_CAP_సంస్కరణ;
    ఒకవేళ (s->enable_msi) {
        s->cap |= RISCV_IOMMU_CAP_MSI_FLAT | RISCV_IOMMU_CAP_MSI_MRIF;
    }
    ఒకవేళ (s->enable_ats) {
        s->cap |= RISCV_IOMMU_CAP_ATS;
    }
    ఒకవేళ (s->enable_s_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32 | RISCV_IOMMU_CAP_SV39 |
                  RISCV_IOMMU_CAP_SV48 | RISCV_IOMMU_CAP_SV57;
    }
    ఒకవేళ (s->enable_g_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32X4 | RISCV_IOMMU_CAP_SV39X4 |
                  RISCV_IOMMU_CAP_SV48X4 | RISCV_IOMMU_CAP_SV57X4 |
                  RISCV_IOMMU_CAP_SVRSW60T59B;
    }

    ఒకవేళ (s->hpm_cntrs > 0) {
        /* Clip number of HPM counters to maximum supported (31). */
        ఒకవేళ (s->hpm_cntrs > RISCV_IOMMU_IOCOUNT_అంకె) {
            s->hpm_cntrs = RISCV_IOMMU_IOCOUNT_అంకె;
        }
        /* Enable hardware performance monitor interface */
        s->cap |= RISCV_IOMMU_CAP_HPM;
    }

    /* Out-of-reset translation mode: OFF (DMA disabled) BARE (passthrough) */
    s->ddtp = సమితి_క్షేత్రం(0, RISCV_IOMMU_DDTP_బహుళకం, s->enable_off ?
                        RISCV_IOMMU_DDTP_బహుళకం_OFF : RISCV_IOMMU_DDTP_బహుళకం_BARE);

    /*
     * Register complete MMIO space, including MSI/PBA registers.
     * Note, PCIDevice implementation will add overlapping MR for MSI/PBA,
     * managed directly by the PCIDevice implementation.
     */
    జ్ఞాపకం_region_init_io(&s->regs_mr, OBJECT(dev), &riscv_iommu_mmio_ops, s,
        "riscv-iommu-regs", RISCV_IOMMU_REG_కొలత);

    /* Set power-on register state */
    stq_le_p(&s->regs[RISCV_IOMMU_REG_CAP], s->cap);

    stq_le_p(&s->regs[RISCV_IOMMU_REG_FCTL], 0);
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FCTL],
             ~(RISCV_IOMMU_FCTL_GXL | RISCV_IOMMU_FCTL_WSI));

    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_DDTP],
        ~(RISCV_IOMMU_DDTP_PPN | RISCV_IOMMU_DDTP_బహుళకం));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQB],
        ~(RISCV_IOMMU_CQB_LOG2SZ | RISCV_IOMMU_CQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQB],
        ~(RISCV_IOMMU_FQB_LOG2SZ | RISCV_IOMMU_FQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQB],
        ~(RISCV_IOMMU_PQB_LOG2SZ | RISCV_IOMMU_PQB_PPN));
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_CQCSR], RISCV_IOMMU_CQCSR_CQMF |
        RISCV_IOMMU_CQCSR_CMD_గమ్యం | RISCV_IOMMU_CQCSR_CMD_ILL);
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
    ఒకవేళ (s->cap & RISCV_IOMMU_CAP_DBG) {
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_IOVA], 0);
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_CTL],
            RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    }

    /* If HPM registers are enabled. */
    ఒకవేళ (s->cap & RISCV_IOMMU_CAP_HPM) {
        /* +1 for cycle counter bit. */
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOCOUNTINH],
                 ~((2 << s->hpm_cntrs) - 1));
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCYCLES], 0);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCTR_పునాది],
               0x00, s->hpm_cntrs * 8);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMEVT_పునాది],
               0x00, s->hpm_cntrs * 8);
    }

    /* Memory region for downstream access, if specified. */
    ఒకవేళ (s->లక్ష్యం_mr) {
        s->లక్ష్యం_as = g_new0(చిరునామా_Space, 1);
        చిరునామా_space_init(s->లక్ష్యం_as, s->లక్ష్యం_mr,
            "riscv-iommu-downstream");
    } లేకపోతే {
        /* Fallback to global system memory. */
        s->లక్ష్యం_as = &చిరునామా_space_జ్ఞాపకం;
    }

    /* Memory region for untranslated MRIF/MSI writes */
    జ్ఞాపకం_region_init_io(&s->trap_mr, OBJECT(dev), &riscv_iommu_trap_ops, s,
            "riscv-iommu-trap", ~0ULL);
    చిరునామా_space_init(&s->trap_as, &s->trap_mr, "riscv-iommu-trap-as");

    ఒకవేళ (s->cap & RISCV_IOMMU_CAP_HPM) {
        s->hpm_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL, riscv_iommu_hpm_timer_cb, s);
        s->hpm_event_ctr_map = g_hash_పట్టిక_new(g_ప్రత్యక్ష_hash, g_ప్రత్యక్ష_equal);
    }
}

స్థిర శూన్య riscv_iommu_unrealize(DeviceState *dev)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    ఒకవేళ (s->cap & RISCV_IOMMU_CAP_HPM) {
        g_hash_పట్టిక_unref(s->hpm_event_ctr_map);
        timer_free(s->hpm_timer);
    }
}

శూన్య riscv_iommu_reset(RISCVIOMMUState *s)
{
    uint32_t reg_clr;
    పూర్ణాంకం ddtp_బహుళకం;

    /*
     * Clear DDTP while setting DDTP_mode back to user
     * initial setting.
     */
    ddtp_బహుళకం = s->enable_off ?
                RISCV_IOMMU_DDTP_బహుళకం_OFF : RISCV_IOMMU_DDTP_బహుళకం_BARE;
    s->ddtp = సమితి_క్షేత్రం(0, RISCV_IOMMU_DDTP_బహుళకం, ddtp_బహుళకం);
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

    g_hash_పట్టిక_remove_all(s->ctx_cache);
    g_hash_పట్టిక_remove_all(s->iot_cache);
}

స్థిర స్థిరరాశి Property riscv_iommu_properties[] = {
    DEFINE_PROP_UINT32("version", RISCVIOMMUState, సంస్కరణ,
        RISCV_IOMMU_SPEC_DOT_VER),
    DEFINE_PROP_UINT32("pas-bits", RISCVIOMMUState, pas_బిట్లు, 0),
    DEFINE_PROP_UINT32("bus", RISCVIOMMUState, bus, 0x0),
    DEFINE_PROP_UINT32("ioatc-limit", RISCVIOMMUState, iot_హద్దు,
        హద్దు_CACHE_IOT),
    DEFINE_PROP_BOOL("intremap", RISCVIOMMUState, enable_msi, TRUE),
    DEFINE_PROP_BOOL("ats", RISCVIOMMUState, enable_ats, TRUE),
    DEFINE_PROP_BOOL("off", RISCVIOMMUState, enable_off, TRUE),
    DEFINE_PROP_BOOL("s-stage", RISCVIOMMUState, enable_s_stage, TRUE),
    DEFINE_PROP_BOOL("g-stage", RISCVIOMMUState, enable_g_stage, TRUE),
    DEFINE_PROP_LINK("downstream-mr", RISCVIOMMUState, లక్ష్యం_mr,
        TYPE_జ్ఞాపకం_REGION, జ్ఞాపకం_Region *),
    DEFINE_PROP_UINT8("hpm-counters", RISCVIOMMUState, hpm_cntrs,
                      RISCV_IOMMU_IOCOUNT_అంకె),
};

స్థిర శూన్య riscv_iommu_class_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    /* internal device for riscv-iommu-{pci/sys}, not user-creatable */
    dc->user_creatable = false;
    dc->realize = riscv_iommu_realize;
    dc->unrealize = riscv_iommu_unrealize;
    device_class_సమితి_props(dc, riscv_iommu_properties);
}

స్థిర స్థిరరాశి TypeInfo riscv_iommu_info = {
    .name = TYPE_RISCV_IOMMU,
    .మాతృ = TYPE_DEVICE,
    .instance_కొలత = పరిమాణం(RISCVIOMMUState),
    .instance_init = riscv_iommu_instance_init,
    .instance_finalize = riscv_iommu_instance_finalize,
    .class_init = riscv_iommu_class_init,
};

స్థిర స్థిరరాశి అక్షరం *IOMMU_FLAG_STR[] = {
    "NA",
    "RO",
    "WR",
    "RW",
};

/* RISC-V IOMMU Memory Region - Address Translation Space */
స్థిర IOMMUTLBEntry riscv_iommu_జ్ఞాపకం_region_translate(
    IOMMUMemoryRegion *iommu_mr, hwaddr addr,
    IOMMUAccessFlags flag, పూర్ణాంకం iommu_idx)
{
    RISCVIOMMUSpace *as = container_యొక్క(iommu_mr, RISCVIOMMUSpace, iova_mr);
    RISCVIOMMUContext *ctx;
    శూన్య *ref;
    IOMMUTLBEntry iotlb = {
        .iova = addr,
        .లక్ష్యం_as = as->iommu->లక్ష్యం_as,
        .addr_ముసుగు = ~0ULL,
        .perm = flag,
    };
    uint32_t devid = riscv_iommu_space_devid(as);

    ctx = riscv_iommu_ctx(as->iommu, devid, iommu_idx, flag, addr, &ref);
    ఒకవేళ (ctx == ఖాళీ) {
        /* Translation disabled or invalid. */
        iotlb.addr_ముసుగు = 0;
        iotlb.perm = IOMMU_NONE;
    } లేకపోతే ఒకవేళ (riscv_iommu_translate(as->iommu, ctx, &iotlb, true)) {
        /* Translation disabled or fault reported. */
        iotlb.addr_ముసుగు = 0;
        iotlb.perm = IOMMU_NONE;
    }

    /* Trace all dma translations with original access flags. */
    trace_riscv_iommu_dma(as->iommu->మాతృ_obj.id, PCI_BUS_అంకె(devid),
                          PCI_స్థానం(devid), PCI_FUNC(devid), iommu_idx,
                          IOMMU_FLAG_STR[flag & IOMMU_RW], iotlb.iova,
                          iotlb.translated_addr);

    riscv_iommu_ctx_put(as->iommu, ref);

    తిరిగి iotlb;
}

స్థిర పూర్ణాంకం riscv_iommu_జ్ఞాపకం_region_notify(
    IOMMUMemoryRegion *iommu_mr, IOMMUNotifierFlag old,
    IOMMUNotifierFlag new, Error **errp)
{
    RISCVIOMMUSpace *as = container_యొక్క(iommu_mr, RISCVIOMMUSpace, iova_mr);

    ఒకవేళ (old == IOMMU_NOTIFIER_NONE) {
        as->notifier = true;
        trace_riscv_iommu_notifier_కూడు(iommu_mr->మాతృ_obj.name);
    } లేకపోతే ఒకవేళ (new == IOMMU_NOTIFIER_NONE) {
        as->notifier = false;
        trace_riscv_iommu_notifier_del(iommu_mr->మాతృ_obj.name);
    }

    తిరిగి 0;
}

స్థిర అంతఃవరుస bool pci_is_iommu(PCIDevice *pdev)
{
    తిరిగి pci_get_మాట(pdev->config + PCI_CLASS_DEVICE) == 0x0806;
}

స్థిర చిరునామా_Space *riscv_iommu_కనుగొను_as(PCIBus *bus, శూన్య *opaque, పూర్ణాంకం devfn)
{
    RISCVIOMMUState *s = (RISCVIOMMUState *) opaque;
    PCIDevice *pdev = pci_కనుగొను_device(bus, pci_bus_అంకె(bus), devfn);
    చిరునామా_Space *as = ఖాళీ;

    ఒకవేళ (pdev && pci_is_iommu(pdev)) {
        తిరిగి s->లక్ష్యం_as;
    }

    /* Find first registered IOMMU device */
    అయితే (s->iommus.le_prev) {
        s = *(s->iommus.le_prev);
    }

    /* Find first matching IOMMU */
    అయితే (s != ఖాళీ && as == ఖాళీ) {
        as = riscv_iommu_space(s, bus, devfn);
        s = s->iommus.le_తదుపరి;
    }

    తిరిగి as ? as : &చిరునామా_space_జ్ఞాపకం;
}

స్థిర స్థిరరాశి PCIIOMMUOps riscv_iommu_ops = {
    .get_చిరునామా_space = riscv_iommu_కనుగొను_as,
};

శూన్య riscv_iommu_pci_అమరిక_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
        Error **errp)
{
    ఒకవేళ (bus->iommu_ops &&
        bus->iommu_ops->get_చిరునామా_space == riscv_iommu_కనుగొను_as) {
        /* Allow multiple IOMMUs on the same PCIe bus, link known devices */
        RISCVIOMMUState *చివరి = (RISCVIOMMUState *)bus->iommu_opaque;
        QLIST_చొప్పించు_AFTER(చివరి, iommu, iommus);
    } లేకపోతే ఒకవేళ (!bus->iommu_ops && !bus->iommu_opaque) {
        pci_అమరిక_iommu(bus, &riscv_iommu_ops, iommu);
    } లేకపోతే {
        error_setg(errp, "can't register secondary IOMMU for PCI bus #%d",
            pci_bus_అంకె(bus));
    }
}

స్థిర పూర్ణాంకం riscv_iommu_జ్ఞాపకం_region_సూచీ(IOMMUMemoryRegion *iommu_mr,
    MemTxAttrs attrs)
{
    తిరిగి attrs.unspecified ? RISCV_IOMMU_NOPROCID : (పూర్ణాంకం)attrs.pid;
}

స్థిర పూర్ణాంకం riscv_iommu_జ్ఞాపకం_region_సూచీ_len(IOMMUMemoryRegion *iommu_mr)
{
    RISCVIOMMUSpace *as = container_యొక్క(iommu_mr, RISCVIOMMUSpace, iova_mr);
    తిరిగి 1 << as->iommu->pid_బిట్లు;
}

స్థిర శూన్య riscv_iommu_జ్ఞాపకం_region_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
{
    IOMMUMemoryRegionClass *imrc = IOMMU_జ్ఞాపకం_REGION_CLASS(klass);

    imrc->translate = riscv_iommu_జ్ఞాపకం_region_translate;
    imrc->notify_flag_మారింది = riscv_iommu_జ్ఞాపకం_region_notify;
    imrc->attrs_గమ్యం_సూచీ = riscv_iommu_జ్ఞాపకం_region_సూచీ;
    imrc->అంకె_indexes = riscv_iommu_జ్ఞాపకం_region_సూచీ_len;
}

స్థిర స్థిరరాశి TypeInfo riscv_iommu_జ్ఞాపకం_region_info = {
    .మాతృ = TYPE_IOMMU_జ్ఞాపకం_REGION,
    .name = TYPE_RISCV_IOMMU_జ్ఞాపకం_REGION,
    .class_init = riscv_iommu_జ్ఞాపకం_region_init,
};

స్థిర శూన్య riscv_iommu_రిజిస్టరు_mr_types(శూన్య)
{
    type_రిజిస్టరు_static(&riscv_iommu_జ్ఞాపకం_region_info);
    type_రిజిస్టరు_static(&riscv_iommu_info);
}

type_init(riscv_iommu_రిజిస్టరు_mr_types);
