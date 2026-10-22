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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qom/object.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "exec/target_page.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pci_bus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pci_device.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv_hart.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "migration/vmstate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/timer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/target-info.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/bitops.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cpu_bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu-bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv-iommu-hpm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "trace.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਹੱਦਬੰਦੀ_CACHE_CTX               (1U << 7)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਹੱਦਬੰਦੀ_CACHE_IOT               (1U << 20)

/* Physical page number coversions */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PPN_PHYS(ppn)                 ((ppn) << ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਬਿੱਟ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PPN_ਥੱਲੇ(phy)                 ((phy) >> ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਬਿੱਟ)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ RISCVIOMMUEntry RISCVIOMMUEntry;

/* Device assigned I/O address space */
ਬਣਤਰ RISCVIOMMUSpace {
    IOMMUMemoryRegion iova_mr;  /* IOVA memory region for attached device */
    ਪਤਾ_Space iova_as;       /* IOVA address space for attached device */
    RISCVIOMMUState *iommu;     /* Managing IOMMU device state */
    PCIBus *bus;                /* PCI bus of the requester */
    uint8_t devfn;              /* Requester identifier, AKA device_id */
    bool notifier;              /* IOMMU unmap notifier enabled */
    QLIST_ਇੰਦਰਾਜ(RISCVIOMMUSpace) list;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ RISCVIOMMUTrans_ਟੈਗ {
    RISCV_IOMMU_TRANS_ਟੈਗ_BY,  /* Bypass */
    RISCV_IOMMU_TRANS_ਟੈਗ_SS,  /* Single Stage */
    RISCV_IOMMU_TRANS_ਟੈਗ_VG,  /* G-stage only */
    RISCV_IOMMU_TRANS_ਟੈਗ_VN,  /* Nested translation */
} RISCVIOMMUTrans_ਟੈਗ;

/* Address translation cache entry */
ਬਣਤਰ RISCVIOMMUEntry {
    RISCVIOMMUTrans_ਟੈਗ ਟੈਗ;     /* Translation Tag */
    uint64_t iova:44;           /* IOVA Page Number */
    uint64_t pscid:20;          /* Process Soft-Context identifier */
    uint64_t phys:44;           /* Physical Page Number */
    uint64_t gscid:16;          /* Guest Soft-Context identifier */
    uint64_t perm:2;            /* IOMMU_RW flags */
};

/* IOMMU index for transactions without process_id specified. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_NOPROCID 0

ਸਥਿਰ uint32_t riscv_iommu_space_devid(RISCVIOMMUSpace *as)
{
    uint32_t devid = PCI_BUILD_BDF(pci_bus_ਅੰਕ(as->bus), as->devfn);

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= as->iommu->bus << 8;
    ਵਾਪਸ devid;
}

ਸਥਿਰ uint8_t riscv_iommu_get_icvec_ਸਦਿਸ਼(uint32_t icvec, uint32_t vec_type)
{
    ਚੋਣ (vec_type) {
    ਹਾਲਤ RISCV_IOMMU_INTR_CQ:
        ਵਾਪਸ icvec & RISCV_IOMMU_ICVEC_CIV;
    ਹਾਲਤ RISCV_IOMMU_INTR_FQ:
        ਵਾਪਸ (icvec & RISCV_IOMMU_ICVEC_FIV) >> 4;
    ਹਾਲਤ RISCV_IOMMU_INTR_PM:
        ਵਾਪਸ (icvec & RISCV_IOMMU_ICVEC_PMIV) >> 8;
    ਹਾਲਤ RISCV_IOMMU_INTR_PQ:
        ਵਾਪਸ (icvec & RISCV_IOMMU_ICVEC_PIV) >> 12;
    ਮੂਲ:
        g_assert_not_reached();
    }
}

ਖਾਲੀ riscv_iommu_notify(RISCVIOMMUState *s, ਪੂਰਨ vec_type)
{
    uint32_t ipsr, icvec, ਸਦਿਸ਼;

    ਜੇ (!s->notify) {
        ਵਾਪਸ;
    }

    icvec = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_ICVEC);
    ipsr = riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, (1 << vec_type), 0);

    ਜੇ (!(ipsr & (1 << vec_type))) {
        ਸਦਿਸ਼ = riscv_iommu_get_icvec_ਸਦਿਸ਼(icvec, vec_type);
        s->notify(s, ਸਦਿਸ਼);
        trace_riscv_iommu_notify_int_ਸਦਿਸ਼(vec_type, ਸਦਿਸ਼);
    }
}

ਖਾਲੀ riscv_iommu_fault(RISCVIOMMUState *s, ਬਣਤਰ riscv_iommu_fq_ਰਿਕਾਰਡ *ev)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ਸਿਰ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQH) & s->fq_ਨਕਾਬ;
    uint32_t ਪੂਛ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQT) & s->fq_ਨਕਾਬ;
    uint32_t ਅਗਲਾ = (ਪੂਛ + 1) & s->fq_ਨਕਾਬ;
    uint32_t devid = get_ਖੇਤਰ(ev->hdr, RISCV_IOMMU_FQ_HDR_DID);

    trace_riscv_iommu_flt(s->ਮਾਪਾ_obj.id, PCI_BUS_ਅੰਕ(devid), PCI_ਥਾਂ(devid),
                          PCI_FUNC(devid), ev->hdr, ev->iotval);

    ਜੇ (!(ctrl & RISCV_IOMMU_FQCSR_FQON) ||
        !!(ctrl & (RISCV_IOMMU_FQCSR_FQOF | RISCV_IOMMU_FQCSR_FQMF))) {
        ਵਾਪਸ;
    }

    ਜੇ (ਸਿਰ == ਅਗਲਾ) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                              RISCV_IOMMU_FQCSR_FQOF, 0);
    } ਨਹੀਂਤਾਂ {
        dma_addr_t addr = s->fq_addr + ਪੂਛ * ਆਕਾਰ(*ev);
        ਜੇ (dma_ਸਮ੍ਰਿਤੀ_write(s->ਨਿਸ਼ਾਨਾ_as, addr, ev, ਆਕਾਰ(*ev),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                                  RISCV_IOMMU_FQCSR_FQMF, 0);
        } ਨਹੀਂਤਾਂ {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_FQT, ਅਗਲਾ);
        }
    }

    ਜੇ (ctrl & RISCV_IOMMU_FQCSR_FIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_FQ);
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_pri(RISCVIOMMUState *s,
    ਬਣਤਰ riscv_iommu_pq_ਰਿਕਾਰਡ *pr)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ਸਿਰ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQH) & s->pq_ਨਕਾਬ;
    uint32_t ਪੂਛ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQT) & s->pq_ਨਕਾਬ;
    uint32_t ਅਗਲਾ = (ਪੂਛ + 1) & s->pq_ਨਕਾਬ;
    uint32_t devid = get_ਖੇਤਰ(pr->hdr, RISCV_IOMMU_PREQ_HDR_DID);

    trace_riscv_iommu_pri(s->ਮਾਪਾ_obj.id, PCI_BUS_ਅੰਕ(devid), PCI_ਥਾਂ(devid),
                          PCI_FUNC(devid), pr->payload);

    ਜੇ (!(ctrl & RISCV_IOMMU_PQCSR_PQON) ||
        !!(ctrl & (RISCV_IOMMU_PQCSR_PQOF | RISCV_IOMMU_PQCSR_PQMF))) {
        ਵਾਪਸ;
    }

    ਜੇ (ਸਿਰ == ਅਗਲਾ) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                              RISCV_IOMMU_PQCSR_PQOF, 0);
    } ਨਹੀਂਤਾਂ {
        dma_addr_t addr = s->pq_addr + ਪੂਛ * ਆਕਾਰ(*pr);
        ਜੇ (dma_ਸਮ੍ਰਿਤੀ_write(s->ਨਿਸ਼ਾਨਾ_as, addr, pr, ਆਕਾਰ(*pr),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                                  RISCV_IOMMU_PQCSR_PQMF, 0);
        } ਨਹੀਂਤਾਂ {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_PQT, ਅਗਲਾ);
        }
    }

    ਜੇ (ctrl & RISCV_IOMMU_PQCSR_PIE) {
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
ਸਥਿਰ uint64_t riscv_iommu_pext_u64(uint64_t val, uint64_t ext)
{
    uint64_t ret = 0;
    uint64_t rot = 1;

    ਜਦੋਂ (ext) {
        ਜੇ (ext & 1) {
            ਜੇ (val & 1) {
                ret |= rot;
            }
            rot <<= 1;
        }
        val >>= 1;
        ext >>= 1;
    }

    ਵਾਪਸ ret;
}

/* Check if GPA matches MSI/MRIF pattern. */
ਸਥਿਰ bool riscv_iommu_msi_check(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    dma_addr_t gpa)
{
    ਜੇ (!s->enable_msi) {
        ਵਾਪਸ false;
    }

    ਜੇ (get_ਖੇਤਰ(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_ਬਹੁਲਕ) !=
        RISCV_IOMMU_DC_MSIPTP_ਬਹੁਲਕ_FLAT) {
        ਵਾਪਸ false; /* Invalid MSI/MRIF mode */
    }

    ਜੇ ((PPN_ਥੱਲੇ(gpa) ^ ctx->msi_addr_ਨਮੂਨਾ) & ~ctx->msi_addr_ਨਕਾਬ) {
        ਵਾਪਸ false; /* GPA not in MSI range defined by AIA IMSIC rules. */
    }

    ਵਾਪਸ true;
}

/* Returns the NAPOT page mask, or 0 for reserved encodings. */
ਸਥਿਰ hwaddr riscv_iommu_napot_ਪੰਨਾ_ਨਕਾਬ(hwaddr ppn, hwaddr addr, hwaddr *ਬਾਹਰਲਾ)
{
    ਪੂਰਨ napot_ਬਿੱਟ = ctz64(ppn) + 1;
    hwaddr napot_ਨਕਾਬ, ਪੰਨਾ_ਨਕਾਬ;

    /* The spec only defines 64KB (napot_bits == 4) */
    ਜੇ (napot_ਬਿੱਟ != 4) {
        ਵਾਪਸ 0;
    }

    napot_ਨਕਾਬ = (1ULL << napot_ਬਿੱਟ) - 1;
    ਪੰਨਾ_ਨਕਾਬ = PPN_PHYS(napot_ਨਕਾਬ) | (ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਮਾਪ - 1);

    *ਬਾਹਰਲਾ = PPN_PHYS(ppn & ~napot_ਨਕਾਬ) | (addr & ਪੰਨਾ_ਨਕਾਬ);

    ਵਾਪਸ ਪੰਨਾ_ਨਕਾਬ;
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
ਸਥਿਰ ਪੂਰਨ riscv_iommu_spa_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb)
{
    IOMMUAccessFlags trans_perm = IOMMU_NONE;
    dma_addr_t addr, ਅਧਾਰ;
    uint64_t satp, gatp, pte;
    bool en_s, en_g;
    ਬਣਤਰ {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਕਦਮ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪੱਧਰ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ptidxbits;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ptesize;
    } sc[2];
    /* Translation stage phase */
    ਗਿਣਤੀ {
        S_STAGE = 0,
        G_STAGE = 1,
    } pass;
    Mem_Tx_ਨਤੀਜਾ ret;
    bool pv = !!ctx->process_id;
    /*
     * Keep the request permission separate from iotlb->perm.  G-stage
     * walks translate S-stage PTE addresses before the real leaf is
     * reached, but permission checks and fault types must still use the
     * original request.  A successful walk leaves iotlb->perm with the
     * effective leaf permission for the translation cache.
     */
    ਅਚਲ IOMMUAccessFlags req_perm = iotlb->perm;

    satp = get_ਖੇਤਰ(ctx->satp, RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ);
    gatp = get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ);

    en_s = satp != RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ_BARE;
    en_g = gatp != RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE;

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
    ਜੇ (!en_s && (req_perm & IOMMU_WO) &&
        riscv_iommu_msi_check(s, ctx, iotlb->iova)) {
        iotlb->ਨਿਸ਼ਾਨਾ_as = &s->trap_as;
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_ਨਕਾਬ = ~ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;
        ਵਾਪਸ 0;
    }

    /* Exit early for pass-through mode. */
    ਜੇ (!(en_s || en_g)) {
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_ਨਕਾਬ = ~ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;
        /* Allow R/W in pass-through mode */
        iotlb->perm = IOMMU_RW;
        ਵਾਪਸ 0;
    }

    /* S/G translation parameters. */
    ਲਈ (pass = 0; pass < 2; pass++) {
        uint32_t sv_ਬਹੁਲਕ;

        sc[pass].ਕਦਮ = 0;
        ਜੇ (pass ? (s->fctl & RISCV_IOMMU_FCTL_GXL) :
            (ctx->tc & RISCV_IOMMU_DC_TC_SXL)) {
            /* 32bit mode for GXL/SXL == 1 */
            ਚੋਣ (pass ? gatp : satp) {
            ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE:
                sc[pass].ਪੱਧਰ    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                ਰੋਕੋ;
            ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV32_X4:
                sv_ਬਹੁਲਕ = pass ? RISCV_IOMMU_CAP_SV32X4 : RISCV_IOMMU_CAP_SV32;
                ਜੇ (!(s->cap & sv_ਬਹੁਲਕ)) {
                    ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].ਪੱਧਰ    = 2;
                sc[pass].ptidxbits = 10;
                sc[pass].ptesize   = 4;
                ਰੋਕੋ;
            ਮੂਲ:
                ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        } ਨਹੀਂਤਾਂ {
            /* 64bit mode for GXL/SXL == 0 */
            ਚੋਣ (pass ? gatp : satp) {
            ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE:
                sc[pass].ਪੱਧਰ    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                ਰੋਕੋ;
            ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV39_X4:
                sv_ਬਹੁਲਕ = pass ? RISCV_IOMMU_CAP_SV39X4 : RISCV_IOMMU_CAP_SV39;
                ਜੇ (!(s->cap & sv_ਬਹੁਲਕ)) {
                    ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].ਪੱਧਰ    = 3;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ਰੋਕੋ;
            ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV48_X4:
                sv_ਬਹੁਲਕ = pass ? RISCV_IOMMU_CAP_SV48X4 : RISCV_IOMMU_CAP_SV48;
                ਜੇ (!(s->cap & sv_ਬਹੁਲਕ)) {
                    ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].ਪੱਧਰ    = 4;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ਰੋਕੋ;
            ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV57_X4:
                sv_ਬਹੁਲਕ = pass ? RISCV_IOMMU_CAP_SV57X4 : RISCV_IOMMU_CAP_SV57;
                ਜੇ (!(s->cap & sv_ਬਹੁਲਕ)) {
                    ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].ਪੱਧਰ    = 5;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ਰੋਕੋ;
            ਮੂਲ:
                ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        }
    };

    /* S/G stages translation tables root pointers */
    gatp = PPN_PHYS(get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_ATP_PPN_ਖੇਤਰ));
    satp = PPN_PHYS(get_ਖੇਤਰ(ctx->satp, RISCV_IOMMU_ATP_PPN_ਖੇਤਰ));
    addr = (en_s && en_g) ? satp : iotlb->iova;
    ਅਧਾਰ = en_g ? gatp : satp;
    pass = en_g ? G_STAGE : S_STAGE;

    ਕਰੋ {
        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ widened = (pass && !sc[pass].ਕਦਮ) ? 2 : 0;
        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ va_ਬਿੱਟ = widened + sc[pass].ptidxbits;
        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ va_skip = ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਬਿੱਟ + sc[pass].ptidxbits *
                                 (sc[pass].ਪੱਧਰ - 1 - sc[pass].ਕਦਮ);
        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ idx = (addr >> va_skip) & ((1 << va_ਬਿੱਟ) - 1);
        ਅਚਲ dma_addr_t pte_addr = ਅਧਾਰ + idx * sc[pass].ptesize;
        ਅਚਲ bool ade =
            ctx->tc & (pass ? RISCV_IOMMU_DC_TC_GADE : RISCV_IOMMU_DC_TC_SADE);

        ਜੇ (ade && !(s->cap & RISCV_IOMMU_CAP_AMO_HWAD)) {
            /* GADE/SADE are reserved bits if AMO_HWAD is cleared.  */
            ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }

        /* Address range check before first level lookup */
        ਜੇ (!sc[pass].ਕਦਮ) {
            ਅਚਲ uint64_t va_len = va_skip + va_ਬਿੱਟ;
            ਅਚਲ uint64_t va_ਨਕਾਬ = (1ULL << va_len) - 1;

            ਜੇ (pass == S_STAGE && va_len > 32) {
                uint64_t ਨਕਾਬ, masked_msbs;

                ਨਕਾਬ = MAKE_64_ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ(0, ਨਿਸ਼ਾਨਾ_long_ਬਿੱਟ() - va_len + 1);
                masked_msbs = (addr >> (va_len - 1)) & ਨਕਾਬ;

                ਜੇ (masked_msbs != 0 && masked_msbs != ਨਕਾਬ) {
                    ਵਾਪਸ (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S;
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ ((addr & va_ਨਕਾਬ) != addr) {
                    ਵਾਪਸ (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS;
                }
            }
        }


        ਜੇ (pass == S_STAGE) {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_S_VS_WALKS);
        } ਨਹੀਂਤਾਂ {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_G_WALKS);
        }

        /* Read page table entry */
        ਜੇ (sc[pass].ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->ਨਿਸ਼ਾਨਾ_as, pte_addr, &pte32,
                             MEMTXATTRS_UNSPECIFIED);
            pte = pte32;
        } ਨਹੀਂਤਾਂ {
            ret = ldq_le_dma(s->ਨਿਸ਼ਾਨਾ_as, pte_addr, &pte,
                             MEMTXATTRS_UNSPECIFIED);
        }
        ਜੇ (ret != MEMTX_OK) {
            ਵਾਪਸ (req_perm & IOMMU_WO) ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT
                                         : RISCV_IOMMU_FQ_CAUSE_RD_FAULT;
        }

        sc[pass].ਕਦਮ++;
        hwaddr ppn = pte >> PTE_PPN_ਖਿਸਕਾਓ;

        ਜੇ (!(pte & PTE_V)) {
            ਰੋਕੋ;                /* Invalid PTE */
        } ਨਹੀਂਤਾਂ ਜੇ (pte & PTE_RESERVED(false)) {
            ਰੋਕੋ;                /* Reserved PTE bits set */
        } ਨਹੀਂਤਾਂ ਜੇ (!(pte & (PTE_R | PTE_W | PTE_X))) {
            ਅਧਾਰ = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } ਨਹੀਂਤਾਂ ਜੇ ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            ਰੋਕੋ;                /* Reserved leaf PTE flags: PTE_W */
        } ਨਹੀਂਤਾਂ ਜੇ ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            ਰੋਕੋ;                /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } ਨਹੀਂਤਾਂ ਜੇ (ppn & ((1ULL << (va_skip - ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਬਿੱਟ)) - 1)) {
            ਰੋਕੋ;                /* Misaligned PPN */
        } ਨਹੀਂਤਾਂ ਜੇ (!(pte & PTE_U) && !pv) {
            /*
             * All accesses are assumed to be User mode unless
             * process_id is valid (pv).  In case we have a
             * non-user mode leaf PTE and !pv we need to fault.
             */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ ਜੇ ((req_perm & IOMMU_RO) && !(pte & PTE_R)) {
            ਰੋਕੋ;                /* Read access check failed */
        } ਨਹੀਂਤਾਂ ਜੇ ((req_perm & IOMMU_WO) && !(pte & PTE_W)) {
            ਰੋਕੋ;                /* Write access check failed */
        } ਨਹੀਂਤਾਂ ਜੇ (!ade && !(pte & PTE_A)) {
            ਰੋਕੋ;                /* Access bit not set */
        } ਨਹੀਂਤਾਂ ਜੇ ((req_perm & IOMMU_WO) && !ade && !(pte & PTE_D)) {
            ਰੋਕੋ;                /* Dirty bit not set */
        } ਨਹੀਂਤਾਂ ਜੇ (pass == G_STAGE && !(pte & PTE_U)) {
            /*
             * riscv-iommu spec 1.0: "When checking the U bit in a
             * second-stage PTE, the transaction is treated as
             * not requesting supervisor privilege."
             *
             * I.e. we need to fault if this is a non-user PTE since
             * we are always in user mode at this point.
             */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ {
            /* Leaf PTE, translation completed. */
            sc[pass].ਕਦਮ = sc[pass].ਪੱਧਰ;

            ਜੇ (pte & PTE_N) {
                hwaddr ਨਕਾਬ = riscv_iommu_napot_ਪੰਨਾ_ਨਕਾਬ(ppn, addr, &ਅਧਾਰ);

                ਜੇ (!ਨਕਾਬ) {
                    ਰੋਕੋ;
                }
                iotlb->addr_ਨਕਾਬ &= ਨਕਾਬ;
            } ਨਹੀਂਤਾਂ {
                ਅਧਾਰ = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
                /* Update address mask based on smallest translation granularity */
                iotlb->addr_ਨਕਾਬ &= (1ULL << va_skip) - 1;
            }

            /* Continue with S-Stage translation? */
            ਜੇ (pass && sc[0].ਕਦਮ != sc[0].ਪੱਧਰ) {
                pass = S_STAGE;
                addr = iotlb->iova;
                ਜਾਰੀ;
            }

            /* Cache the effective permission, not this request's subset. */
            IOMMUAccessFlags ਪੱਤੀ_perm = (pte & PTE_W) ?
                                         ((pte & PTE_R) ? IOMMU_RW : IOMMU_WO) :
                                         IOMMU_RO;

            trans_perm = trans_perm == IOMMU_NONE ?
                         ਪੱਤੀ_perm : trans_perm & ਪੱਤੀ_perm;

            /* Translation phase completed (GPA or SPA) */
            iotlb->translated_addr = ਅਧਾਰ;

            /* Check MSI GPA address match */
            ਜੇ (pass == S_STAGE && (req_perm & IOMMU_WO) &&
                riscv_iommu_msi_check(s, ctx, ਅਧਾਰ)) {
                /* Trap MSI writes and return GPA address. */
                iotlb->ਨਿਸ਼ਾਨਾ_as = &s->trap_as;
                iotlb->addr_ਨਕਾਬ = ~ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;
                ਵਾਪਸ 0;
            }

            /* Continue with G-Stage translation? */
            ਜੇ (!pass && en_g) {
                pass = G_STAGE;
                addr = ਅਧਾਰ;
                ਅਧਾਰ = gatp;
                sc[pass].ਕਦਮ = 0;
                ਜਾਰੀ;
            }

            iotlb->perm = trans_perm;
            ਵਾਪਸ 0;
        }

        ਜੇ (sc[pass].ਕਦਮ == sc[pass].ਪੱਧਰ) {
            ਰੋਕੋ; /* Can't find leaf PTE */
        }

        /* Continue with G-Stage translation? */
        ਜੇ (!pass && en_g) {
            pass = G_STAGE;
            addr = ਅਧਾਰ;
            ਅਧਾਰ = gatp;
            sc[pass].ਕਦਮ = 0;
        }
    } ਜਦੋਂ (1);

    /*
     * riscv_iommu_translate() will receive a fault and then call
     * riscv_iommu_report_fault() using iotlb->translated_addr
     * as iotval2.  Update translated_addr it with the latest
     * translated addr we have.
     */
    iotlb->translated_addr = addr;

    ਵਾਪਸ (req_perm & IOMMU_WO) ?
                (pass ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S) :
                (pass ? RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_report_fault(RISCVIOMMUState *s,
                                     RISCVIOMMUContext *ctx,
                                     uint32_t fault_type, uint32_t cause,
                                     bool pv,
                                     uint64_t iotval, uint64_t iotval2)
{
    ਬਣਤਰ riscv_iommu_fq_ਰਿਕਾਰਡ ev = { 0 };

    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_DTF) {
        ਚੋਣ (cause) {
        ਹਾਲਤ RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED:
        ਹਾਲਤ RISCV_IOMMU_FQ_CAUSE_DDT_ਲੱਦੋ_FAULT:
        ਹਾਲਤ RISCV_IOMMU_FQ_CAUSE_DDT_INVALID:
        ਹਾਲਤ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED:
        ਹਾਲਤ RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED:
        ਹਾਲਤ RISCV_IOMMU_FQ_CAUSE_ਅੰਦਰੂਨੀ_DP_ERROR:
        ਹਾਲਤ RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT:
            ਰੋਕੋ;
        ਮੂਲ:
            /* DTF prevents reporting a fault for this given cause */
            ਵਾਪਸ;
        }
    }

    ev.hdr = ਸੈੱਟ_ਖੇਤਰ(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE, cause);
    ev.hdr = ਸੈੱਟ_ਖੇਤਰ(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE, fault_type);
    ev.hdr = ਸੈੱਟ_ਖੇਤਰ(ev.hdr, RISCV_IOMMU_FQ_HDR_DID, ctx->devid);
    ev.hdr = ਸੈੱਟ_ਖੇਤਰ(ev.hdr, RISCV_IOMMU_FQ_HDR_PV, pv);

    ਜੇ (pv) {
        ev.hdr = ਸੈੱਟ_ਖੇਤਰ(ev.hdr, RISCV_IOMMU_FQ_HDR_PID, ctx->process_id);
    }

    ev.iotval = iotval;
    ev.iotval2 = iotval2;

    riscv_iommu_fault(s, &ev);
}

/* Redirect MSI write for given GPA. */
ਸਥਿਰ Mem_Tx_ਨਤੀਜਾ riscv_iommu_msi_write(RISCVIOMMUState *s,
    RISCVIOMMUContext *ctx, uint64_t gpa, uint64_t ਅੰਕੜੇ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ, MemTxAttrs attrs)
{
    Mem_Tx_ਨਤੀਜਾ res;
    dma_addr_t addr;
    uint64_t intn;
    ਮਾਪ_t ਵਿੱਥ;
    uint32_t n190;
    uint64_t pte[2];
    ਪੂਰਨ fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    ਪੂਰਨ cause;

    /* Interrupt File Number */
    intn = riscv_iommu_pext_u64(PPN_ਥੱਲੇ(gpa), ctx->msi_addr_ਨਕਾਬ);
    ਵਿੱਥ = intn * ਆਕਾਰ(pte);

    /* fetch MSI PTE */
    addr = PPN_PHYS(get_ਖੇਤਰ(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_PPN));
    ਜੇ (addr & ਵਿੱਥ) {
        /* Interrupt file number out of range */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ਲੱਦੋ_FAULT;
        ਜਾਓ err;
    }

    addr |= ਵਿੱਥ;
    res = dma_ਸਮ੍ਰਿਤੀ_read(s->ਨਿਸ਼ਾਨਾ_as, addr, &pte, ਆਕਾਰ(pte),
            MEMTXATTRS_UNSPECIFIED);
    ਜੇ (res != MEMTX_OK) {
        ਜੇ (res == MEMTX_DECODE_ERROR) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED;
        } ਨਹੀਂਤਾਂ {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_ਲੱਦੋ_FAULT;
        }
        ਜਾਓ err;
    }

    le64_ਮੰਜ਼ਿਲ_cpus(&pte[0]);
    le64_ਮੰਜ਼ਿਲ_cpus(&pte[1]);

    ਜੇ (!(pte[0] & RISCV_IOMMU_MSI_PTE_V) || (pte[0] & RISCV_IOMMU_MSI_PTE_C)) {
        /*
         * The spec mentions that: "If msipte.C == 1, then further
         * processing to interpret the PTE is implementation
         * defined.". We'll abort with cause = 262 for this
         * case too.
         */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_INVALID;
        ਜਾਓ err;
    }

    ਚੋਣ (get_ਖੇਤਰ(pte[0], RISCV_IOMMU_MSI_PTE_M)) {
    ਹਾਲਤ RISCV_IOMMU_MSI_PTE_M_BASIC:
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
        ਜੇ (pte[0] & (GENMASK_ULL(62, 54) | GENMASK_ULL(9, 3))) {
            res = MEMTX_DECODE_ERROR;
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
            ਜਾਓ err;
        }

        /* MSI Pass-through mode */
        addr = PPN_PHYS(get_ਖੇਤਰ(pte[0], RISCV_IOMMU_MSI_PTE_PPN));

        trace_riscv_iommu_msi(s->ਮਾਪਾ_obj.id, PCI_BUS_ਅੰਕ(ctx->devid),
                              PCI_ਥਾਂ(ctx->devid), PCI_FUNC(ctx->devid),
                              gpa, addr);

        res = dma_ਸਮ੍ਰਿਤੀ_write(s->ਨਿਸ਼ਾਨਾ_as, addr, &ਅੰਕੜੇ, ਮਾਪ, attrs);
        ਜੇ (res != MEMTX_OK) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
            ਜਾਓ err;
        }

        ਵਾਪਸ MEMTX_OK;
    ਹਾਲਤ RISCV_IOMMU_MSI_PTE_M_MRIF:
        /* MRIF mode, continue. */
        ਰੋਕੋ;
    ਮੂਲ:
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        ਜਾਓ err;
    }

    /*
     * Report an error for interrupt identities exceeding the maximum allowed
     * for an IMSIC interrupt file (2047) or destination address is not 32-bit
     * aligned. See IOMMU Specification, Chapter 2.3. MSI page tables.
     */
    ਜੇ ((ਅੰਕੜੇ > 2047) || (gpa & 3)) {
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        ਜਾਓ err;
    }

    /* MSI MRIF mode, non atomic pending bit update */

    /* MRIF pending bit address */
    addr = get_ਖੇਤਰ(pte[0], RISCV_IOMMU_MSI_PTE_MRIF_ADDR) << 9;
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
    addr = addr | ((ਅੰਕੜੇ & 0x7c0) >> 2);

    trace_riscv_iommu_msi(s->ਮਾਪਾ_obj.id, PCI_BUS_ਅੰਕ(ctx->devid),
                          PCI_ਥਾਂ(ctx->devid), PCI_FUNC(ctx->devid),
                          gpa, addr);

    /* MRIF pending bit mask */
    ਅੰਕੜੇ = 1ULL << (ਅੰਕੜੇ & 0x03f);
    res = dma_ਸਮ੍ਰਿਤੀ_read(s->ਨਿਸ਼ਾਨਾ_as, addr, &intn, ਆਕਾਰ(intn), attrs);
    ਜੇ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ਲੱਦੋ_FAULT;
        ਜਾਓ err;
    }

    intn = intn | ਅੰਕੜੇ;
    res = dma_ਸਮ੍ਰਿਤੀ_write(s->ਨਿਸ਼ਾਨਾ_as, addr, &intn, ਆਕਾਰ(intn), attrs);
    ਜੇ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        ਜਾਓ err;
    }

    /* Get MRIF enable bits */
    addr = addr + ਆਕਾਰ(intn);
    res = dma_ਸਮ੍ਰਿਤੀ_read(s->ਨਿਸ਼ਾਨਾ_as, addr, &intn, ਆਕਾਰ(intn), attrs);
    ਜੇ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ਲੱਦੋ_FAULT;
        ਜਾਓ err;
    }

    ਜੇ (!(intn & ਅੰਕੜੇ)) {
        /* notification disabled, MRIF update completed. */
        ਵਾਪਸ MEMTX_OK;
    }

    /* Send notification message */
    addr = PPN_PHYS(get_ਖੇਤਰ(pte[1], RISCV_IOMMU_MSI_MRIF_NPPN));
    n190 = get_ਖੇਤਰ(pte[1], RISCV_IOMMU_MSI_MRIF_NID) |
          (get_ਖੇਤਰ(pte[1], RISCV_IOMMU_MSI_MRIF_NID_MSB) << 10);

    res = dma_ਸਮ੍ਰਿਤੀ_write(s->ਨਿਸ਼ਾਨਾ_as, addr, &n190, ਆਕਾਰ(n190), attrs);
    ਜੇ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        ਜਾਓ err;
    }

    trace_riscv_iommu_mrif_notification(s->ਮਾਪਾ_obj.id, n190, addr);

    ਵਾਪਸ MEMTX_OK;

err:
    riscv_iommu_report_fault(s, ctx, fault_type, cause,
                             !!ctx->process_id, 0, 0);
    ਵਾਪਸ res;
}

/*
 * Check device context configuration as described by the
 * riscv-iommu spec section "Device-context configuration
 * checks".
 */
ਸਥਿਰ bool riscv_iommu_validate_device_ctx(RISCVIOMMUState *s,
                                            RISCVIOMMUContext *ctx)
{
    uint32_t fsc_ਬਹੁਲਕ, msi_ਬਹੁਲਕ;
    uint64_t gatp;

    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_RESERVED) {
        ਵਾਪਸ false;
    }

    ਜੇ (!(s->cap & RISCV_IOMMU_CAP_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI ||
         ctx->tc & RISCV_IOMMU_DC_TC_PRPR)) {
        ਵਾਪਸ false;
    }

    ਜੇ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI)) {
        ਵਾਪਸ false;
    }

    ਜੇ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI) &&
        ctx->tc & RISCV_IOMMU_DC_TC_PRPR) {
        ਵਾਪਸ false;
    }

    ਜੇ (!(s->cap & RISCV_IOMMU_CAP_T2GPA) &&
        ctx->tc & RISCV_IOMMU_DC_TC_T2GPA) {
        ਵਾਪਸ false;
    }

    ਜੇ (s->cap & RISCV_IOMMU_CAP_MSI_FLAT) {
        msi_ਬਹੁਲਕ = get_ਖੇਤਰ(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_ਬਹੁਲਕ);

        ਜੇ (msi_ਬਹੁਲਕ != RISCV_IOMMU_DC_MSIPTP_ਬਹੁਲਕ_OFF &&
            msi_ਬਹੁਲਕ != RISCV_IOMMU_DC_MSIPTP_ਬਹੁਲਕ_FLAT) {
            ਵਾਪਸ false;
        }
    }

    gatp = get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ);
    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA &&
        gatp == RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE) {
        ਵਾਪਸ false;
    }

    ਜੇ (gatp != RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE) {
        uint64_t iohgatp_ppn = get_ਖੇਤਰ(ctx->gatp,
                                         RISCV_IOMMU_DC_IOHGATP_PPN);

        /*
         * One of the conditions for a misconfigured DDT entry
         * according to the riscv-spec: "DC.iohgatp.MODE is not
         * Bare and the root page table (address) determined by
         * DC.iohgatp.PPN is not aligned to a 16-KiB boundary."
         */
        ਜੇ (PPN_PHYS(iohgatp_ppn) & ((1ULL << 14) - 1)) {
            ਵਾਪਸ false;
        }
    }

    fsc_ਬਹੁਲਕ = get_ਖੇਤਰ(ctx->satp, RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ);

    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_PDTV) {
        ਚੋਣ (fsc_ਬਹੁਲਕ) {
        ਹਾਲਤ RISCV_IOMMU_DC_FSC_PDTP_ਬਹੁਲਕ_PD8:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_PD8)) {
                ਵਾਪਸ false;
            }
            ਰੋਕੋ;
        ਹਾਲਤ RISCV_IOMMU_DC_FSC_PDTP_ਬਹੁਲਕ_PD17:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_PD17)) {
                ਵਾਪਸ false;
            }
            ਰੋਕੋ;
        ਹਾਲਤ RISCV_IOMMU_DC_FSC_PDTP_ਬਹੁਲਕ_PD20:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_PD20)) {
                ਵਾਪਸ false;
            }
            ਰੋਕੋ;
        }
    } ਨਹੀਂਤਾਂ {
        /* DC.tc.PDTV is 0 */
        ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_DPE) {
            ਵਾਪਸ false;
        }

        ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
            ਜੇ (fsc_ਬਹੁਲਕ == RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV32 &&
                !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                ਵਾਪਸ false;
            }
        } ਨਹੀਂਤਾਂ {
            ਚੋਣ (fsc_ਬਹੁਲਕ) {
            ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV39:
                ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                    ਵਾਪਸ false;
                }
                ਰੋਕੋ;
            ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV48:
                ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                    ਵਾਪਸ false;
                }
            ਰੋਕੋ;
            ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV57:
                ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                    ਵਾਪਸ false;
                }
                ਰੋਕੋ;
            }
        }
    }

    /*
     * CAP_END is always zero (only one endianess). FCTL_BE is
     * always zero (little-endian accesses). Thus TC_SBE must
     * always be LE, i.e. zero.
     */
    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_SBE) {
        ਵਾਪਸ false;
    }

    ਵਾਪਸ true;
}

/*
 * Validate process context (PC) according to section
 * "Process-context configuration checks".
 */
ਸਥਿਰ bool riscv_iommu_validate_process_ctx(RISCVIOMMUState *s,
                                             RISCVIOMMUContext *ctx)
{
    uint32_t ਬਹੁਲਕ;

    ਜੇ (get_ਖੇਤਰ(ctx->ta, RISCV_IOMMU_PC_TA_RESERVED)) {
        ਵਾਪਸ false;
    }

    ਜੇ (get_ਖੇਤਰ(ctx->satp, RISCV_IOMMU_PC_FSC_RESERVED)) {
        ਵਾਪਸ false;
    }

    ਬਹੁਲਕ = get_ਖੇਤਰ(ctx->satp, RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ);
    ਚੋਣ (ਬਹੁਲਕ) {
    ਹਾਲਤ RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ_BARE:
    /* sv39 and sv32 modes have the same value (8) */
    ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV39:
    ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV48:
    ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV57:
        ਰੋਕੋ;
    ਮੂਲ:
        ਵਾਪਸ false;
    }

    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
        ਜੇ (ਬਹੁਲਕ == RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV32 &&
            !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                ਵਾਪਸ false;
        }
    } ਨਹੀਂਤਾਂ {
        ਚੋਣ (ਬਹੁਲਕ) {
        ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV39:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                ਵਾਪਸ false;
            }
            ਰੋਕੋ;
        ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV48:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                ਵਾਪਸ false;
            }
            ਰੋਕੋ;
        ਹਾਲਤ RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV57:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                ਵਾਪਸ false;
            }
            ਰੋਕੋ;
        }
    }

    ਵਾਪਸ true;
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
ਸਥਿਰ Mem_Tx_ਨਤੀਜਾ pdt_ਸਮ੍ਰਿਤੀ_read(RISCVIOMMUState *s,
                                   RISCVIOMMUContext *ctx,
                                   dma_addr_t addr,
                                   ਖਾਲੀ *buf, dma_addr_t len,
                                   MemTxAttrs attrs)
{
    uint64_t gatp_ਬਹੁਲਕ, pte;
    ਬਣਤਰ {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਕਦਮ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪੱਧਰ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ptidxbits;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ptesize;
    } sc;
    Mem_Tx_ਨਤੀਜਾ ret;
    dma_addr_t ਅਧਾਰ = addr;

    /* G stages translation mode */
    gatp_ਬਹੁਲਕ = get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ);
    ਜੇ (gatp_ਬਹੁਲਕ == RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE) {
        ਜਾਓ ਬਾਹਰਲਾ;
    }

    /* G stages translation tables root pointer */
    ਅਧਾਰ = PPN_PHYS(get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_ATP_PPN_ਖੇਤਰ));

    /* Start at step 0 */
    sc.ਕਦਮ = 0;

    ਜੇ (s->fctl & RISCV_IOMMU_FCTL_GXL) {
        /* 32bit mode for GXL == 1 */
        ਚੋਣ (gatp_ਬਹੁਲਕ) {
        ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV32_X4:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV32X4)) {
                ਵਾਪਸ MEMTX_ACCESS_ERROR;
            }
            sc.ਪੱਧਰ    = 2;
            sc.ptidxbits = 10;
            sc.ptesize   = 4;
            ਰੋਕੋ;
        ਮੂਲ:
            ਵਾਪਸ MEMTX_ACCESS_ERROR;
        }
    } ਨਹੀਂਤਾਂ {
        /* 64bit mode for GXL == 0 */
        ਚੋਣ (gatp_ਬਹੁਲਕ) {
        ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV39_X4:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV39X4)) {
                ਵਾਪਸ MEMTX_ACCESS_ERROR;
            }
            sc.ਪੱਧਰ    = 3;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ਰੋਕੋ;
        ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV48_X4:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV48X4)) {
                ਵਾਪਸ MEMTX_ACCESS_ERROR;
            }
            sc.ਪੱਧਰ    = 4;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ਰੋਕੋ;
        ਹਾਲਤ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV57_X4:
            ਜੇ (!(s->cap & RISCV_IOMMU_CAP_SV57X4)) {
                ਵਾਪਸ MEMTX_ACCESS_ERROR;
            }
            sc.ਪੱਧਰ    = 5;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ਰੋਕੋ;
        ਮੂਲ:
            ਵਾਪਸ MEMTX_ACCESS_ERROR;
        }
    }

    ਕਰੋ {
        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ va_ਬਿੱਟ = (sc.ਕਦਮ ? 0 : 2) + sc.ptidxbits;
        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ va_skip = ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਬਿੱਟ + sc.ptidxbits *
                                 (sc.ਪੱਧਰ - 1 - sc.ਕਦਮ);
        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ idx = (addr >> va_skip) & ((1 << va_ਬਿੱਟ) - 1);
        ਅਚਲ dma_addr_t pte_addr = ਅਧਾਰ + idx * sc.ptesize;

        /* Address range check before first level lookup */
        ਜੇ (!sc.ਕਦਮ) {
            ਅਚਲ uint64_t va_ਨਕਾਬ = (1ULL << (va_skip + va_ਬਿੱਟ)) - 1;
            ਜੇ ((addr & va_ਨਕਾਬ) != addr) {
                ਵਾਪਸ MEMTX_ACCESS_ERROR;
            }
        }

        /* Read page table entry */
        ਜੇ (sc.ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->ਨਿਸ਼ਾਨਾ_as, pte_addr, &pte32, attrs);
            pte = pte32;
        } ਨਹੀਂਤਾਂ {
            ret = ldq_le_dma(s->ਨਿਸ਼ਾਨਾ_as, pte_addr, &pte, attrs);
        }
        ਜੇ (ret != MEMTX_OK) {
            ਵਾਪਸ ret;
        }

        sc.ਕਦਮ++;
        hwaddr ppn = pte >> PTE_PPN_ਖਿਸਕਾਓ;

        ਜੇ (!(pte & PTE_V)) {
            ਵਾਪਸ MEMTX_ACCESS_ERROR; /* Invalid PTE */
        } ਨਹੀਂਤਾਂ ਜੇ (!(pte & (PTE_R | PTE_W | PTE_X))) {
            ਅਧਾਰ = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } ਨਹੀਂਤਾਂ ਜੇ ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            ਵਾਪਸ MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W */
        } ਨਹੀਂਤਾਂ ਜੇ ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            ਵਾਪਸ MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } ਨਹੀਂਤਾਂ ਜੇ (ppn & ((1ULL << (va_skip - ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਬਿੱਟ)) - 1)) {
            ਵਾਪਸ MEMTX_ACCESS_ERROR; /* Misaligned PPN */
        } ਨਹੀਂਤਾਂ {
            /* Leaf PTE, translation completed. */
            ਜੇ (pte & PTE_N) {
                ਜੇ (!riscv_iommu_napot_ਪੰਨਾ_ਨਕਾਬ(ppn, addr, &ਅਧਾਰ)) {
                    ਵਾਪਸ MEMTX_ACCESS_ERROR;
                }
            } ਨਹੀਂਤਾਂ {
                ਅਧਾਰ = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
            }
            ਰੋਕੋ;
        }

        ਜੇ (sc.ਕਦਮ == sc.ਪੱਧਰ) {
            ਵਾਪਸ MEMTX_ACCESS_ERROR; /* Can't find leaf PTE */
        }
    } ਜਦੋਂ (1);

ਬਾਹਰਲਾ:
    ਵਾਪਸ dma_ਸਮ੍ਰਿਤੀ_read(s->ਨਿਸ਼ਾਨਾ_as, ਅਧਾਰ, buf, len, attrs);
}

/*
 * RISC-V IOMMU Device Context Loopkup - Device Directory Tree Walk
 *
 * @s         : IOMMU Device State
 * @ctx       : Device Translation Context with devid and process_id set.
 * @return    : success or fault code.
 */
ਸਥਿਰ ਪੂਰਨ riscv_iommu_ctx_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx)
{
    ਅਚਲ uint64_t ddtp = s->ddtp;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਹੁਲਕ = get_ਖੇਤਰ(ddtp, RISCV_IOMMU_DDTP_ਬਹੁਲਕ);
    dma_addr_t addr = PPN_PHYS(get_ਖੇਤਰ(ddtp, RISCV_IOMMU_DDTP_PPN));
    ਬਣਤਰ riscv_iommu_dc dc;
    /* Device Context format: 0: extended (64 bytes) | 1: base (32 bytes) */
    ਅਚਲ ਪੂਰਨ dc_fmt = !s->enable_msi;
    ਅਚਲ ਮਾਪ_t dc_len = ਆਕਾਰ(dc) >> dc_fmt;
    ਪੂਰਨ ਡੂੰਘਾਈ;
    uint64_t de;

    ਚੋਣ (ਬਹੁਲਕ) {
    ਹਾਲਤ RISCV_IOMMU_DDTP_ਬਹੁਲਕ_OFF:
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED;

    ਹਾਲਤ RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE:
        /* mock up pass-through translation context */
        ctx->gatp = ਸੈੱਟ_ਖੇਤਰ(0, RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ,
            RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE);
        ctx->satp = ਸੈੱਟ_ਖੇਤਰ(0, RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ,
            RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ_BARE);

        ctx->tc = RISCV_IOMMU_DC_TC_V;
        ਜੇ (s->enable_ats) {
            ctx->tc |= RISCV_IOMMU_DC_TC_EN_ATS;
        }

        ctx->ta = 0;
        ctx->msiptp = 0;
        ਵਾਪਸ 0;

    ਹਾਲਤ RISCV_IOMMU_DDTP_ਬਹੁਲਕ_1_LVL:
        ਡੂੰਘਾਈ = 0;
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_DDTP_ਬਹੁਲਕ_2_LVL:
        ਡੂੰਘਾਈ = 1;
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_DDTP_ਬਹੁਲਕ_3_LVL:
        ਡੂੰਘਾਈ = 2;
        ਰੋਕੋ;

    ਮੂਲ:
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /*
     * Check supported device id width (in bits).
     * See IOMMU Specification, Chapter 6. Software guidelines.
     * - if extended device-context format is used:
     *   1LVL: 6, 2LVL: 15, 3LVL: 24
     * - if base device-context format is used:
     *   1LVL: 7, 2LVL: 16, 3LVL: 24
     */
    ਜੇ (ctx->devid >= (1 << (ਡੂੰਘਾਈ * 9 + 6 + (dc_fmt && ਡੂੰਘਾਈ != 2)))) {
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
    }

    /* Device directory tree walk */
    ਲਈ (; ਡੂੰਘਾਈ-- > 0; ) {
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
        ਅਚਲ ਪੂਰਨ ਵੰਡੋ = ਡੂੰਘਾਈ * 9 + 6 + dc_fmt;
        addr |= ((ctx->devid >> ਵੰਡੋ) << 3) & ~ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;
        ਜੇ (dma_ਸਮ੍ਰਿਤੀ_read(s->ਨਿਸ਼ਾਨਾ_as, addr, &de, ਆਕਾਰ(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_ਲੱਦੋ_FAULT;
        }
        le64_ਮੰਜ਼ਿਲ_cpus(&de);
        ਜੇ (!(de & RISCV_IOMMU_DDTE_VALID)) {
            /* invalid directory entry */
            ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        ਜੇ (de & ~(RISCV_IOMMU_DDTE_PPN | RISCV_IOMMU_DDTE_VALID)) {
            /* reserved bits set */
            ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }
        addr = PPN_PHYS(get_ਖੇਤਰ(de, RISCV_IOMMU_DDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);

    /* index into device context entry page */
    addr |= (ctx->devid * dc_len) & ~ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;

    memset(&dc, 0, ਆਕਾਰ(dc));
    ਜੇ (dma_ਸਮ੍ਰਿਤੀ_read(s->ਨਿਸ਼ਾਨਾ_as, addr, &dc, dc_len,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_ਲੱਦੋ_FAULT;
    }

    /* Set translation context. */
    ctx->tc = le64_ਮੰਜ਼ਿਲ_cpu(dc.tc);
    ctx->gatp = le64_ਮੰਜ਼ਿਲ_cpu(dc.iohgatp);
    ctx->satp = le64_ਮੰਜ਼ਿਲ_cpu(dc.fsc);
    ctx->ta = le64_ਮੰਜ਼ਿਲ_cpu(dc.ta);
    ctx->msiptp = le64_ਮੰਜ਼ਿਲ_cpu(dc.msiptp);
    ctx->msi_addr_ਨਕਾਬ = le64_ਮੰਜ਼ਿਲ_cpu(dc.msi_addr_ਨਕਾਬ);
    ctx->msi_addr_ਨਮੂਨਾ = le64_ਮੰਜ਼ਿਲ_cpu(dc.msi_addr_ਨਮੂਨਾ);

    ਜੇ (!(ctx->tc & RISCV_IOMMU_DC_TC_V)) {
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
    }

    ਜੇ (!riscv_iommu_validate_device_ctx(s, ctx)) {
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /* FSC field checks */
    ਬਹੁਲਕ = get_ਖੇਤਰ(ctx->satp, RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ);
    addr = PPN_PHYS(get_ਖੇਤਰ(ctx->satp, RISCV_IOMMU_DC_FSC_PPN));

    ਜੇ (!(ctx->tc & RISCV_IOMMU_DC_TC_PDTV)) {
        ਜੇ (ctx->process_id != RISCV_IOMMU_NOPROCID) {
            /* PID is disabled */
            ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
        }
        ਜੇ (ਬਹੁਲਕ > RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV57) {
            /* Invalid translation mode */
            ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        ਵਾਪਸ 0;
    }

    ਜੇ (ctx->process_id == RISCV_IOMMU_NOPROCID) {
        ਜੇ (!(ctx->tc & RISCV_IOMMU_DC_TC_DPE)) {
            /* No default process_id enabled, set BARE mode */
            ctx->satp = 0ULL;
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ {
            /* Use default process_id #0 */
            ctx->process_id = 0;
        }
    }

    ਜੇ (ਬਹੁਲਕ == RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ_BARE) {
        /* No S-Stage translation, done. */
        ਵਾਪਸ 0;
    }

    /* FSC.TC.PDTV enabled */
    ਜੇ (ਬਹੁਲਕ > RISCV_IOMMU_DC_FSC_PDTP_ਬਹੁਲਕ_PD20) {
        /* Invalid PDTP.MODE */
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    ਲਈ (ਡੂੰਘਾਈ = ਬਹੁਲਕ - RISCV_IOMMU_DC_FSC_PDTP_ਬਹੁਲਕ_PD8; ਡੂੰਘਾਈ-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

        /*
         * Select process id index bits based on process directory tree
         * level. See IOMMU Specification, 2.2. Process-Directory-Table.
         */
        ਅਚਲ ਪੂਰਨ ਵੰਡੋ = ਡੂੰਘਾਈ * 9 + 8;
        addr |= ((ctx->process_id >> ਵੰਡੋ) << 3) & ~ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;
        ਜੇ (pdt_ਸਮ੍ਰਿਤੀ_read(s, ctx, addr, &de, ਆਕਾਰ(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_PDT_ਲੱਦੋ_FAULT;
        }
        le64_ਮੰਜ਼ਿਲ_cpus(&de);
        ਜੇ (!(de & RISCV_IOMMU_PDTE_VALID)) {
            ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
        }
        addr = PPN_PHYS(get_ਖੇਤਰ(de, RISCV_IOMMU_PDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

    /* Leaf entry in PDT */
    addr |= (ctx->process_id << 4) & ~ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;
    ਜੇ (pdt_ਸਮ੍ਰਿਤੀ_read(s, ctx, addr, &dc.ta, ਆਕਾਰ(uint64_t) * 2,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_PDT_ਲੱਦੋ_FAULT;
    }

    /* Use FSC and TA from process directory entry. */
    ctx->ta = le64_ਮੰਜ਼ਿਲ_cpu(dc.ta);
    ctx->satp = le64_ਮੰਜ਼ਿਲ_cpu(dc.fsc);

    ਜੇ (!(ctx->ta & RISCV_IOMMU_PC_TA_V)) {
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
    }

    ਜੇ (!riscv_iommu_validate_process_ctx(s, ctx)) {
        ਵਾਪਸ RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    ਵਾਪਸ 0;
}

/* Translation Context cache support */
ਸਥਿਰ gboolean riscv_iommu_ctx_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUContext *c1 = (RISCVIOMMUContext *) v1;
    RISCVIOMMUContext *c2 = (RISCVIOMMUContext *) v2;
    ਵਾਪਸ c1->devid == c2->devid &&
           c1->process_id == c2->process_id;
}

ਸਥਿਰ guint riscv_iommu_ctx_hash(gconstpointer v)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) v;
    /*
     * Generate simple hash of (process_id, devid)
     * assuming 24-bit wide devid.
     */
    ਵਾਪਸ (guint)(ctx->devid) + ((guint)(ctx->process_id) << 24);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ctx_inval_devid_procid(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ,
                                               gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) ਮੁੱਲ;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) ਅੰਕੜੇ;
    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid &&
        ctx->process_id == arg->process_id) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ctx_inval_devid(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ,
                                        gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) ਮੁੱਲ;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) ਅੰਕੜੇ;
    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ctx_inval_all(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ,
                                      gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) ਮੁੱਲ;
    ਜੇ (ctx->tc & RISCV_IOMMU_DC_TC_V) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ctx_inval(RISCVIOMMUState *s, GHFunc func,
                                  uint32_t devid, uint32_t process_id)
{
    GHash_ਸਾਰਣੀ *ctx_cache;
    RISCVIOMMUContext ਕੁੰਜੀ = {
        .devid = devid,
        .process_id = process_id,
    };
    ctx_cache = g_hash_ਸਾਰਣੀ_ref(s->ctx_cache);
    g_hash_ਸਾਰਣੀ_foreach(ctx_cache, func, &ਕੁੰਜੀ);
    g_hash_ਸਾਰਣੀ_unref(ctx_cache);
}

/* Find or allocate translation context for a given {device_id, process_id} */
ਸਥਿਰ RISCVIOMMUContext *riscv_iommu_ctx(RISCVIOMMUState *s,
                                          ਬਿਨਾਨਿਸ਼ਾਨੀ devid, ਬਿਨਾਨਿਸ਼ਾਨੀ process_id,
                                          IOMMUAccessFlags perm, uint64_t iova,
                                          ਖਾਲੀ **ref)
{
    GHash_ਸਾਰਣੀ *ctx_cache;
    RISCVIOMMUContext *ctx;
    RISCVIOMMUContext ਕੁੰਜੀ = {
        .devid = devid,
        .process_id = process_id,
    };
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਹੁਲਕ = get_ਖੇਤਰ(s->ddtp, RISCV_IOMMU_DDTP_ਬਹੁਲਕ);
    uint32_t fault_type;

    ctx_cache = g_hash_ਸਾਰਣੀ_ref(s->ctx_cache);

    ਜੇ (ਬਹੁਲਕ != RISCV_IOMMU_DDTP_ਬਹੁਲਕ_OFF &&
        ਬਹੁਲਕ != RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE) {
        ctx = g_hash_ਸਾਰਣੀ_lookup(ctx_cache, &ਕੁੰਜੀ);

        ਜੇ (ctx && (ctx->tc & RISCV_IOMMU_DC_TC_V)) {
            *ref = ctx_cache;
            ਵਾਪਸ ctx;
        }
    }

    ctx = g_new0(RISCVIOMMUContext, 1);
    ctx->devid = devid;
    ctx->process_id = process_id;

    ਪੂਰਨ fault = riscv_iommu_ctx_fetch(s, ctx);
    ਜੇ (!fault) {
        ਜੇ (ਬਹੁਲਕ != RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE) {
            ਜੇ (g_hash_ਸਾਰਣੀ_ਮਾਪ(ctx_cache) >= ਹੱਦਬੰਦੀ_CACHE_CTX) {
                g_hash_ਸਾਰਣੀ_unref(ctx_cache);
                ctx_cache = g_hash_ਸਾਰਣੀ_new_ਭਰਿਆ(riscv_iommu_ctx_hash,
                                                  riscv_iommu_ctx_equal,
                                                  g_free, ਨਿਰਾ);
                g_hash_ਸਾਰਣੀ_ref(ctx_cache);
                g_hash_ਸਾਰਣੀ_unref(qatomic_xchg(&s->ctx_cache, ctx_cache));
            }

            g_hash_ਸਾਰਣੀ_ਜੋੜੋ(ctx_cache, ctx);
            *ref = ctx_cache;
        } ਨਹੀਂਤਾਂ {
            g_hash_ਸਾਰਣੀ_unref(ctx_cache);
            /* Remember ctx so it can be freed */
            *ref = ctx;
        }
        ਵਾਪਸ ctx;
    }

    g_hash_ਸਾਰਣੀ_unref(ctx_cache);
    *ref = ਨਿਰਾ;

    /*
     * TODO: (1) do we need to distinguish other fault types
     * for ctx fetching and (2) evaluate putting the 'fault_type'
     * logic inside riscv_iommu_report_fault() - there's at
     * least one other place (end of riscv_iommu_translate())
     * that does something similar.
     */
    ਜੇ (perm & IOMMU_RO) {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
    } ਨਹੀਂਤਾਂ {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    }

    riscv_iommu_report_fault(s, ctx, fault_type, fault,
                             !!process_id, iova, 0);

    g_free(ctx);
    ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ctx_put(RISCVIOMMUState *s, ਖਾਲੀ *ref)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਹੁਲਕ = get_ਖੇਤਰ(s->ddtp, RISCV_IOMMU_DDTP_ਬਹੁਲਕ);

    ਜੇ (!ref) {
        ਵਾਪਸ;
    }

    /* ref is pointing to ctx in Bare mode. Bare mode ctx is not cached */
    ਜੇ (ਬਹੁਲਕ == RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE) {
        g_free(ref);
    } ਨਹੀਂਤਾਂ {
        g_hash_ਸਾਰਣੀ_unref((GHash_ਸਾਰਣੀ *)ref);
    }
}

/* Find or allocate address space for a given device */
ਸਥਿਰ ਪਤਾ_Space *riscv_iommu_space(RISCVIOMMUState *s, PCIBus *bus,
                                            ਪੂਰਨ devfn)
{
    RISCVIOMMUSpace *as;

    QLIST_FOREACH(as, &s->spaces, list) {
        ਜੇ (as->bus == bus && as->devfn == devfn) {
            ਰੋਕੋ;
        }
    }

    ਜੇ (as == ਨਿਰਾ) {
        ਅੱਖਰ name[64];
        as = g_new0(RISCVIOMMUSpace, 1);

        as->iommu = s;
        as->bus = bus;
        as->devfn = devfn;

        snprintf(name, ਆਕਾਰ(name), "riscv-iommu-%04x:%02x.%d-iova",
            pci_bus_ਅੰਕ(bus), PCI_ਥਾਂ(devfn), PCI_FUNC(devfn));

        /* IOVA address space, untranslated addresses */
        ਸਮ੍ਰਿਤੀ_region_init_iommu(&as->iova_mr, ਆਕਾਰ(as->iova_mr),
            TYPE_RISCV_IOMMU_ਸਮ੍ਰਿਤੀ_REGION,
            OBJECT(s), "riscv_iommu", UINT64_MAX);
        ਪਤਾ_space_init(&as->iova_as, ਸਮ੍ਰਿਤੀ_REGION(&as->iova_mr), name);

        QLIST_ਪਾਓ_ਸਿਰ(&s->spaces, as, list);

        trace_riscv_iommu_new(s->ਮਾਪਾ_obj.id, pci_bus_ਅੰਕ(bus),
                PCI_ਥਾਂ(devfn), PCI_FUNC(devfn));
    }
    ਵਾਪਸ &as->iova_as;
}

/* Translation Object cache support */
ਸਥਿਰ gboolean riscv_iommu_iot_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUEntry *t1 = (RISCVIOMMUEntry *) v1;
    RISCVIOMMUEntry *t2 = (RISCVIOMMUEntry *) v2;
    ਵਾਪਸ t1->gscid == t2->gscid && t1->pscid == t2->pscid &&
           t1->iova == t2->iova && t1->ਟੈਗ == t2->ਟੈਗ;
}

ਸਥਿਰ guint riscv_iommu_iot_hash(gconstpointer v)
{
    RISCVIOMMUEntry *t = (RISCVIOMMUEntry *) v;
    ਵਾਪਸ (guint)t->iova;
}

/* GV: 0 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 0 AV: 0 GVMA: 1 */
ਸਥਿਰ
ਖਾਲੀ riscv_iommu_iot_inval_all(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ, gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ਮੁੱਲ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ਅੰਕੜੇ;
    ਜੇ (iot->ਟੈਗ == arg->ਟੈਗ) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 0 PSCV: 1 GVMA: 0 */
ਸਥਿਰ
ਖਾਲੀ riscv_iommu_iot_inval_pscid(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ, gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ਮੁੱਲ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ਅੰਕੜੇ;
    ਜੇ (iot->ਟੈਗ == arg->ਟੈਗ &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 0 GVMA: 0 */
ਸਥਿਰ
ਖਾਲੀ riscv_iommu_iot_inval_iova(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ, gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ਮੁੱਲ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ਅੰਕੜੇ;
    ਜੇ (iot->ਟੈਗ == arg->ਟੈਗ &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 1 GVMA: 0 */
ਸਥਿਰ ਖਾਲੀ riscv_iommu_iot_inval_pscid_iova(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ,
                                             gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ਮੁੱਲ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ਅੰਕੜੇ;
    ਜੇ (iot->ਟੈਗ == arg->ਟੈਗ &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 0 GVMA: 1 */
ਸਥਿਰ
ਖਾਲੀ riscv_iommu_iot_inval_gscid(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ, gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ਮੁੱਲ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ਅੰਕੜੇ;
    ਜੇ (iot->ਟੈਗ == arg->ਟੈਗ &&
        iot->gscid == arg->gscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 1 GVMA: 0 */
ਸਥਿਰ ਖਾਲੀ riscv_iommu_iot_inval_gscid_pscid(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ,
                                              gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ਮੁੱਲ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ਅੰਕੜੇ;
    ਜੇ (iot->ਟੈਗ == arg->ਟੈਗ &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 1 GVMA: 1 */
ਸਥਿਰ ਖਾਲੀ riscv_iommu_iot_inval_gscid_iova(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ,
                                             gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ਮੁੱਲ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ਅੰਕੜੇ;
    ਜੇ (iot->ਟੈਗ == arg->ਟੈਗ &&
        iot->gscid == arg->gscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 1 GVMA: 0 */
ਸਥਿਰ ਖਾਲੀ riscv_iommu_iot_inval_gscid_pscid_iova(gpointer ਕੁੰਜੀ, gpointer ਮੁੱਲ,
                                                   gpointer ਅੰਕੜੇ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ਮੁੱਲ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ਅੰਕੜੇ;
    ਜੇ (iot->ਟੈਗ == arg->ਟੈਗ &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* caller should keep ref-count for iot_cache object */
ਸਥਿਰ RISCVIOMMUEntry *riscv_iommu_iot_lookup(RISCVIOMMUContext *ctx,
    GHash_ਸਾਰਣੀ *iot_cache, hwaddr iova, RISCVIOMMUTrans_ਟੈਗ transtag)
{
    RISCVIOMMUEntry ਕੁੰਜੀ = {
        .ਟੈਗ   = transtag,
        .gscid = get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID),
        .pscid = get_ਖੇਤਰ(ctx->ta, RISCV_IOMMU_DC_TA_PSCID),
        .iova  = PPN_ਥੱਲੇ(iova),
    };
    ਵਾਪਸ g_hash_ਸਾਰਣੀ_lookup(iot_cache, &ਕੁੰਜੀ);
}

/* caller should keep ref-count for iot_cache object */
ਸਥਿਰ ਖਾਲੀ riscv_iommu_iot_update(RISCVIOMMUState *s,
    GHash_ਸਾਰਣੀ *iot_cache, RISCVIOMMUEntry *iot)
{
    ਜੇ (!s->iot_ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ;
    }

    ਜੇ (g_hash_ਸਾਰਣੀ_ਮਾਪ(s->iot_cache) >= s->iot_ਹੱਦਬੰਦੀ) {
        iot_cache = g_hash_ਸਾਰਣੀ_new_ਭਰਿਆ(riscv_iommu_iot_hash,
                                          riscv_iommu_iot_equal,
                                          g_free, ਨਿਰਾ);
        g_hash_ਸਾਰਣੀ_unref(qatomic_xchg(&s->iot_cache, iot_cache));
    }
    g_hash_ਸਾਰਣੀ_ਜੋੜੋ(iot_cache, iot);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_iot_inval(RISCVIOMMUState *s, GHFunc func,
    uint32_t gscid, uint32_t pscid, hwaddr iova, RISCVIOMMUTrans_ਟੈਗ transtag)
{
    GHash_ਸਾਰਣੀ *iot_cache;
    RISCVIOMMUEntry ਕੁੰਜੀ = {
        .ਟੈਗ = transtag,
        .gscid = gscid,
        .pscid = pscid,
        .iova  = PPN_ਥੱਲੇ(iova),
    };

    iot_cache = g_hash_ਸਾਰਣੀ_ref(s->iot_cache);
    g_hash_ਸਾਰਣੀ_foreach(iot_cache, func, &ਕੁੰਜੀ);
    g_hash_ਸਾਰਣੀ_unref(iot_cache);
}

ਸਥਿਰ RISCVIOMMUTrans_ਟੈਗ riscv_iommu_get_transtag(RISCVIOMMUContext *ctx)
{
    uint64_t satp = get_ਖੇਤਰ(ctx->satp, RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ);
    uint64_t gatp = get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ);

    ਜੇ (satp == RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ_BARE) {
        ਵਾਪਸ (gatp == RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE) ?
            RISCV_IOMMU_TRANS_ਟੈਗ_BY : RISCV_IOMMU_TRANS_ਟੈਗ_VG;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ (gatp == RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE) ?
            RISCV_IOMMU_TRANS_ਟੈਗ_SS : RISCV_IOMMU_TRANS_ਟੈਗ_VN;
    }
}

ਸਥਿਰ ਪੂਰਨ riscv_iommu_translate(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb, bool enable_cache)
{
    RISCVIOMMUTrans_ਟੈਗ transtag = riscv_iommu_get_transtag(ctx);
    RISCVIOMMUEntry *iot;
    IOMMUAccessFlags perm;
    bool enable_pid;
    bool enable_pri;
    GHash_ਸਾਰਣੀ *iot_cache;
    ਪੂਰਨ fault;

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_URQ);

    iot_cache = g_hash_ਸਾਰਣੀ_ref(s->iot_cache);
    enable_pri = (iotlb->perm == IOMMU_NONE) &&
                 (ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI);
    enable_pid = (ctx->tc & RISCV_IOMMU_DC_TC_PDTV);

    /* Check for ATS request. */
    ਜੇ (iotlb->perm == IOMMU_NONE) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_ATS_RQ);
        /* Check if ATS is disabled. */
        ਜੇ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS)) {
            enable_pri = false;
            fault = RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
            ਜਾਓ ਪੂਰਾ;
        }
    }

    iot = riscv_iommu_iot_lookup(ctx, iot_cache, iotlb->iova, transtag);
    perm = iot ? iot->perm : IOMMU_NONE;
    ਜੇ (perm != IOMMU_NONE) {
        iotlb->translated_addr = PPN_PHYS(iot->phys);
        iotlb->addr_ਨਕਾਬ = ~ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;
        iotlb->perm = perm;
        fault = 0;
        ਜਾਓ ਪੂਰਾ;
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_TLB_ਖੁੰਝ);

    /* Translate using device directory / page table information. */
    fault = riscv_iommu_spa_fetch(s, ctx, iotlb);

    ਜੇ (!fault && iotlb->ਨਿਸ਼ਾਨਾ_as == &s->trap_as) {
        /* Do not cache trapped MSI translations */
        ਜਾਓ ਪੂਰਾ;
    }

    /*
     * We made an implementation choice to not cache identity-mapped
     * translations, as allowed by the specification, to avoid
     * translation cache evictions for other devices sharing the
     * IOMMU hardware model.
     */
    ਜੇ (!fault && iotlb->translated_addr != iotlb->iova && enable_cache) {
        iot = g_new0(RISCVIOMMUEntry, 1);
        iot->iova = PPN_ਥੱਲੇ(iotlb->iova);
        iot->phys = PPN_ਥੱਲੇ(iotlb->translated_addr);
        iot->gscid = get_ਖੇਤਰ(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
        iot->pscid = get_ਖੇਤਰ(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        iot->perm = iotlb->perm;
        iot->ਟੈਗ = transtag;
        riscv_iommu_iot_update(s, iot_cache, iot);
    }

ਪੂਰਾ:
    g_hash_ਸਾਰਣੀ_unref(iot_cache);

    ਜੇ (enable_pri && fault) {
        ਬਣਤਰ riscv_iommu_pq_ਰਿਕਾਰਡ pr = {0};
        ਜੇ (enable_pid) {
            pr.hdr = ਸੈੱਟ_ਖੇਤਰ(RISCV_IOMMU_PREQ_HDR_PV,
                               RISCV_IOMMU_PREQ_HDR_PID, ctx->process_id);
        }
        pr.hdr = ਸੈੱਟ_ਖੇਤਰ(pr.hdr, RISCV_IOMMU_PREQ_HDR_DID, ctx->devid);
        pr.payload = (iotlb->iova & ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ) |
                     RISCV_IOMMU_PREQ_PAYLOAD_M;
        riscv_iommu_pri(s, &pr);
        ਵਾਪਸ fault;
    }

    ਜੇ (fault) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ttype = RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ;

        ਜੇ ((iotlb->perm & IOMMU_RW) == IOMMU_RW
            || iotlb->perm & IOMMU_WO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
        } ਨਹੀਂਤਾਂ ਜੇ (iotlb->perm & IOMMU_RO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
        }

        riscv_iommu_report_fault(s, ctx, ttype, fault, enable_pid,
                                 iotlb->iova, iotlb->translated_addr);
        ਵਾਪਸ fault;
    }

    ਵਾਪਸ 0;
}

/* IOMMU Command Interface */
ਸਥਿਰ Mem_Tx_ਨਤੀਜਾ riscv_iommu_iofence(RISCVIOMMUState *s, bool notify,
    uint64_t addr, uint32_t ਅੰਕੜੇ)
{
    /*
     * ATS processing in this implementation of the IOMMU is synchronous,
     * no need to wait for completions here.
     */
    ਜੇ (!notify) {
        ਵਾਪਸ MEMTX_OK;
    }

    ਵਾਪਸ dma_ਸਮ੍ਰਿਤੀ_write(s->ਨਿਸ਼ਾਨਾ_as, addr, &ਅੰਕੜੇ, ਆਕਾਰ(ਅੰਕੜੇ),
        MEMTXATTRS_UNSPECIFIED);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ats(RISCVIOMMUState *s,
    ਬਣਤਰ riscv_iommu_command *cmd, IOMMUNotifierFlag flag,
    IOMMUAccessFlags perm,
    ਖਾਲੀ (*trace_fn)(ਅਚਲ ਅੱਖਰ *id))
{
    RISCVIOMMUSpace *as;
    IOMMUNotifier *n;
    IOMMUTLBEvent event;
    uint32_t pid;
    uint32_t devid;
    ਅਚਲ bool pv = cmd->dword0 & RISCV_IOMMU_CMD_ATS_PV;

    ਜੇ (cmd->dword0 & RISCV_IOMMU_CMD_ATS_DSV) {
        /* Use device segment and requester id */
        devid = get_ਖੇਤਰ(cmd->dword0,
            RISCV_IOMMU_CMD_ATS_DSEG | RISCV_IOMMU_CMD_ATS_RID);
    } ਨਹੀਂਤਾਂ {
        devid = get_ਖੇਤਰ(cmd->dword0, RISCV_IOMMU_CMD_ATS_RID);
    }

    pid = get_ਖੇਤਰ(cmd->dword0, RISCV_IOMMU_CMD_ATS_PID);

    QLIST_FOREACH(as, &s->spaces, list) {
        ਜੇ (riscv_iommu_space_devid(as) == devid) {
            ਰੋਕੋ;
        }
    }

    ਜੇ (!as || !as->notifier) {
        ਵਾਪਸ;
    }

    event.type = flag;
    event.ਇੰਦਰਾਜ.perm = perm;
    event.ਇੰਦਰਾਜ.ਨਿਸ਼ਾਨਾ_as = s->ਨਿਸ਼ਾਨਾ_as;

    IOMMU_NOTIFIER_FOREACH(n, &as->iova_mr) {
        ਜੇ (!pv || n->iommu_idx == pid) {
            event.ਇੰਦਰਾਜ.iova = n->ਸ਼ੁਰੂ;
            event.ਇੰਦਰਾਜ.addr_ਨਕਾਬ = n->ਸਿਰਾ - n->ਸ਼ੁਰੂ;
            trace_fn(as->iova_mr.ਮਾਪਾ_obj.name);
            ਸਮ੍ਰਿਤੀ_region_notify_iommu_one(n, &event);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ats_inval(RISCVIOMMUState *s,
    ਬਣਤਰ riscv_iommu_command *cmd)
{
    ਵਾਪਸ riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_DEVIOTLB_UNMAP, IOMMU_NONE,
                           trace_riscv_iommu_ats_inval);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ats_prgr(RISCVIOMMUState *s,
    ਬਣਤਰ riscv_iommu_command *cmd)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ resp_ਕੋਡ = get_ਖੇਤਰ(cmd->dword1,
                                   RISCV_IOMMU_CMD_ATS_PRGR_RESP_ਕੋਡ);

    /* Using the access flag to carry response code information */
    IOMMUAccessFlags perm = resp_ਕੋਡ ? IOMMU_NONE : IOMMU_RW;
    ਵਾਪਸ riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_MAP, perm,
                           trace_riscv_iommu_ats_prgr);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_process_ddtp(RISCVIOMMUState *s)
{
    uint64_t old_ddtp = s->ddtp;
    uint64_t new_ddtp = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_DDTP);
    ਬਿਨਾਨਿਸ਼ਾਨੀ new_ਬਹੁਲਕ = get_ਖੇਤਰ(new_ddtp, RISCV_IOMMU_DDTP_ਬਹੁਲਕ);
    ਬਿਨਾਨਿਸ਼ਾਨੀ old_ਬਹੁਲਕ = get_ਖੇਤਰ(old_ddtp, RISCV_IOMMU_DDTP_ਬਹੁਲਕ);
    bool ok = false;

    /*
     * Check for allowed DDTP.MODE transitions:
     * {OFF, BARE}        -> {OFF, BARE, 1LVL, 2LVL, 3LVL}
     * {1LVL, 2LVL, 3LVL} -> {OFF, BARE}
     */
    ਜੇ (new_ਬਹੁਲਕ == old_ਬਹੁਲਕ ||
        new_ਬਹੁਲਕ == RISCV_IOMMU_DDTP_ਬਹੁਲਕ_OFF ||
        new_ਬਹੁਲਕ == RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE) {
        ok = true;
    } ਨਹੀਂਤਾਂ ਜੇ (new_ਬਹੁਲਕ == RISCV_IOMMU_DDTP_ਬਹੁਲਕ_1_LVL ||
               new_ਬਹੁਲਕ == RISCV_IOMMU_DDTP_ਬਹੁਲਕ_2_LVL ||
               new_ਬਹੁਲਕ == RISCV_IOMMU_DDTP_ਬਹੁਲਕ_3_LVL) {
        ok = old_ਬਹੁਲਕ == RISCV_IOMMU_DDTP_ਬਹੁਲਕ_OFF ||
             old_ਬਹੁਲਕ == RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE;
    }

    ਜੇ (ok) {
        /* clear reserved and busy bits, report back sanitized version */
        new_ddtp = ਸੈੱਟ_ਖੇਤਰ(new_ddtp & RISCV_IOMMU_DDTP_PPN,
                             RISCV_IOMMU_DDTP_ਬਹੁਲਕ, new_ਬਹੁਲਕ);
    } ਨਹੀਂਤਾਂ {
        new_ddtp = old_ddtp;
    }
    s->ddtp = new_ddtp;

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, new_ddtp);
}

/* Command function and opcode field. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD(func, op) (((func) << 7) | (op))

ਸਥਿਰ ਖਾਲੀ riscv_iommu_process_cq_ਪੂਛ(RISCVIOMMUState *s)
{
    ਬਣਤਰ riscv_iommu_command cmd;
    Mem_Tx_ਨਤੀਜਾ res;
    dma_addr_t addr;
    uint32_t ਪੂਛ, ਸਿਰ, ctrl;
    uint64_t cmd_opcode;
    GHFunc func;

    ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    ਪੂਛ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQT) & s->cq_ਨਕਾਬ;
    ਸਿਰ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQH) & s->cq_ਨਕਾਬ;

    /* Check for pending error or queue processing disabled */
    ਜੇ (!(ctrl & RISCV_IOMMU_CQCSR_CQON) ||
        !!(ctrl & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        ਵਾਪਸ;
    }

    ਜਦੋਂ (ਪੂਛ != ਸਿਰ) {
        addr = s->cq_addr  + ਸਿਰ * ਆਕਾਰ(cmd);
        res = dma_ਸਮ੍ਰਿਤੀ_read(s->ਨਿਸ਼ਾਨਾ_as, addr, &cmd, ਆਕਾਰ(cmd),
                              MEMTXATTRS_UNSPECIFIED);

        ਜੇ (res != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                  RISCV_IOMMU_CQCSR_CQMF, 0);
            ਜਾਓ fault;
        }

        trace_riscv_iommu_cmd(s->ਮਾਪਾ_obj.id, cmd.dword0, cmd.dword1);

        cmd_opcode = get_ਖੇਤਰ(cmd.dword0,
                               RISCV_IOMMU_CMD_OPCODE | RISCV_IOMMU_CMD_FUNC);

        ਚੋਣ (cmd_opcode) {
        ਹਾਲਤ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOFENCE_FUNC_C,
                             RISCV_IOMMU_CMD_IOFENCE_OPCODE):
            ਜੇ (cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_RESERVED) {
                ਜਾਓ cmd_ill;
            }

            res = riscv_iommu_iofence(s,
                cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_AV, cmd.dword1 << 2,
                get_ਖੇਤਰ(cmd.dword0, RISCV_IOMMU_CMD_IOFENCE_ਅੰਕੜੇ));

            ਜੇ (res != MEMTX_OK) {
                riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                      RISCV_IOMMU_CQCSR_CQMF, 0);
                ਜਾਓ fault;
            }
            ਰੋਕੋ;

        ਹਾਲਤ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_ਖੇਤਰ(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_ਖੇਤਰ(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;

            ਜੇ (pscv) {
                /* illegal command arguments IOTINVAL.GVMA & PSCV == 1 */
                ਜਾਓ cmd_ill;
            }

            func = riscv_iommu_iot_inval_all;

            ਜੇ (gv) {
                func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                              riscv_iommu_iot_inval_gscid;
            }

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_ਟੈਗ_VG);

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_ਟੈਗ_VN);
            ਰੋਕੋ;
        }

        ਹਾਲਤ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_ਖੇਤਰ(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_ਖੇਤਰ(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & ਨਿਸ਼ਾਨਾ_ਪੰਨਾ_ਨਕਾਬ;
            RISCVIOMMUTrans_ਟੈਗ transtag;

            ਜੇ (gv) {
                transtag = RISCV_IOMMU_TRANS_ਟੈਗ_VN;
                ਜੇ (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_gscid_pscid_iova :
                                  riscv_iommu_iot_inval_gscid_pscid;
                } ਨਹੀਂਤਾਂ {
                    func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                                  riscv_iommu_iot_inval_gscid;
                }
            } ਨਹੀਂਤਾਂ {
                transtag = RISCV_IOMMU_TRANS_ਟੈਗ_SS;
                ਜੇ (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_pscid_iova :
                                  riscv_iommu_iot_inval_pscid;
                } ਨਹੀਂਤਾਂ {
                    func = (av) ? riscv_iommu_iot_inval_iova :
                                  riscv_iommu_iot_inval_all;
                }
            }

            riscv_iommu_iot_inval(s, func, gscid, pscid, iova, transtag);
            ਰੋਕੋ;
        }

        ਹਾਲਤ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            ਜੇ (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* invalidate all device context cache mappings */
                func = riscv_iommu_ctx_inval_all;
            } ਨਹੀਂਤਾਂ {
                /* invalidate all device context matching DID */
                func = riscv_iommu_ctx_inval_devid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_ਖੇਤਰ(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID), 0);
            ਰੋਕੋ;

        ਹਾਲਤ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            ਜੇ (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* illegal command arguments IODIR_PDT & DV == 0 */
                ਜਾਓ cmd_ill;
            } ਨਹੀਂਤਾਂ {
                func = riscv_iommu_ctx_inval_devid_procid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_ਖੇਤਰ(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID),
                get_ਖੇਤਰ(cmd.dword0, RISCV_IOMMU_CMD_IODIR_PID));
            ਰੋਕੋ;

        /* ATS commands */
        ਹਾਲਤ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_INVAL,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            ਜੇ (!s->enable_ats) {
                ਜਾਓ cmd_ill;
            }

            riscv_iommu_ats_inval(s, &cmd);
            ਰੋਕੋ;

        ਹਾਲਤ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_PRGR,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            ਜੇ (!s->enable_ats) {
                ਜਾਓ cmd_ill;
            }

            riscv_iommu_ats_prgr(s, &cmd);
            ਰੋਕੋ;

        ਮੂਲ:
        cmd_ill:
            /* Invalid instruction, do not advance instruction index. */
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                RISCV_IOMMU_CQCSR_CMD_ILL, 0);
            ਜਾਓ fault;
        }

        /* Advance and update head pointer after command completes. */
        ਸਿਰ = (ਸਿਰ + 1) & s->cq_ਨਕਾਬ;
        riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_CQH, ਸਿਰ);
    }
    ਵਾਪਸ;

fault:
    ਜੇ (ctrl & RISCV_IOMMU_CQCSR_CIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_CQ);
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_process_cq_control(RISCVIOMMUState *s)
{
    uint64_t ਅਧਾਰ;
    uint32_t ctrl_ਸੈੱਟ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_ਸੈੱਟ & RISCV_IOMMU_CQCSR_CQEN);
    bool ਸਰਗਰਮ = !!(ctrl_ਸੈੱਟ & RISCV_IOMMU_CQCSR_CQON);

    ਜੇ (enable && !ਸਰਗਰਮ) {
        ਅਧਾਰ = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_CQB);
        s->cq_ਨਕਾਬ = (2ULL << get_ਖੇਤਰ(ਅਧਾਰ, RISCV_IOMMU_CQB_LOG2SZ)) - 1;
        s->cq_addr = PPN_PHYS(get_ਖੇਤਰ(ਅਧਾਰ, RISCV_IOMMU_CQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~s->cq_ਨਕਾਬ);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQT], 0);
        ctrl_ਸੈੱਟ = RISCV_IOMMU_CQCSR_CQON;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQMF |
                   RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CMD_ਮੰਜ਼ਿਲ |
                   RISCV_IOMMU_CQCSR_FENCE_W_IP;
    } ਨਹੀਂਤਾਂ ਜੇ (!enable && ਸਰਗਰਮ) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~0);
        ctrl_ਸੈੱਟ = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQON;
    } ਨਹੀਂਤਾਂ {
        ctrl_ਸੈੱਟ = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, ctrl_ਸੈੱਟ, ctrl_clr);

    /*
     * After clearing error bits (CMD_ILL, CQMF), if queue is still active,
     * re-process pending command.
     */
    ctrl_ਸੈੱਟ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    ਜੇ ((ctrl_ਸੈੱਟ & RISCV_IOMMU_CQCSR_CQON) &&
        !(ctrl_ਸੈੱਟ & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        riscv_iommu_process_cq_ਪੂਛ(s);
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_process_fq_control(RISCVIOMMUState *s)
{
    uint64_t ਅਧਾਰ;
    uint32_t ctrl_ਸੈੱਟ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_ਸੈੱਟ & RISCV_IOMMU_FQCSR_FQEN);
    bool ਸਰਗਰਮ = !!(ctrl_ਸੈੱਟ & RISCV_IOMMU_FQCSR_FQON);

    ਜੇ (enable && !ਸਰਗਰਮ) {
        ਅਧਾਰ = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_FQB);
        s->fq_ਨਕਾਬ = (2ULL << get_ਖੇਤਰ(ਅਧਾਰ, RISCV_IOMMU_FQB_LOG2SZ)) - 1;
        s->fq_addr = PPN_PHYS(get_ਖੇਤਰ(ਅਧਾਰ, RISCV_IOMMU_FQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~s->fq_ਨਕਾਬ);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQT], 0);
        ctrl_ਸੈੱਟ = RISCV_IOMMU_FQCSR_FQON;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQMF |
            RISCV_IOMMU_FQCSR_FQOF;
    } ਨਹੀਂਤਾਂ ਜੇ (!enable && ਸਰਗਰਮ) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~0);
        ctrl_ਸੈੱਟ = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQON;
    } ਨਹੀਂਤਾਂ {
        ctrl_ਸੈੱਟ = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, ctrl_ਸੈੱਟ, ctrl_clr);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_process_pq_control(RISCVIOMMUState *s)
{
    uint64_t ਅਧਾਰ;
    uint32_t ctrl_ਸੈੱਟ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_ਸੈੱਟ & RISCV_IOMMU_PQCSR_PQEN);
    bool ਸਰਗਰਮ = !!(ctrl_ਸੈੱਟ & RISCV_IOMMU_PQCSR_PQON);

    ਜੇ (enable && !ਸਰਗਰਮ) {
        ਅਧਾਰ = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_PQB);
        s->pq_ਨਕਾਬ = (2ULL << get_ਖੇਤਰ(ਅਧਾਰ, RISCV_IOMMU_PQB_LOG2SZ)) - 1;
        s->pq_addr = PPN_PHYS(get_ਖੇਤਰ(ਅਧਾਰ, RISCV_IOMMU_PQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~s->pq_ਨਕਾਬ);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQT], 0);
        ctrl_ਸੈੱਟ = RISCV_IOMMU_PQCSR_PQON;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQMF |
            RISCV_IOMMU_PQCSR_PQOF;
    } ਨਹੀਂਤਾਂ ਜੇ (!enable && ਸਰਗਰਮ) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~0);
        ctrl_ਸੈੱਟ = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQON;
    } ਨਹੀਂਤਾਂ {
        ctrl_ਸੈੱਟ = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, ctrl_ਸੈੱਟ, ctrl_clr);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_process_dbg(RISCVIOMMUState *s)
{
    uint64_t iova = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_IOVA);
    uint64_t ctrl = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_CTL);
    ਬਿਨਾਨਿਸ਼ਾਨੀ devid = get_ਖੇਤਰ(ctrl, RISCV_IOMMU_TR_REQ_CTL_DID);
    ਬਿਨਾਨਿਸ਼ਾਨੀ pid = get_ਖੇਤਰ(ctrl, RISCV_IOMMU_TR_REQ_CTL_PID);
    IOMMUAccessFlags perm = ctrl & RISCV_IOMMU_TR_REQ_CTL_NW
                            ? IOMMU_RO : IOMMU_RW;
    RISCVIOMMUContext *ctx;
    ਖਾਲੀ *ref;

    ਜੇ (!(ctrl & RISCV_IOMMU_TR_REQ_CTL_GO_BUSY)) {
        ਵਾਪਸ;
    }

    ctx = riscv_iommu_ctx(s, devid, pid, perm, iova, &ref);
    ਜੇ (ctx == ਨਿਰਾ) {
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE,
                                 RISCV_IOMMU_TR_RESPONSE_FAULT |
                                 (RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED << 10));
    } ਨਹੀਂਤਾਂ {
        IOMMUTLBEntry iotlb = {
            .iova = iova,
            .perm = perm,
            .addr_ਨਕਾਬ = ~0,
            .ਨਿਸ਼ਾਨਾ_as = ਨਿਰਾ,
        };
        ਪੂਰਨ fault = riscv_iommu_translate(s, ctx, &iotlb, false);
        ਜੇ (fault) {
            iova = RISCV_IOMMU_TR_RESPONSE_FAULT | (((uint64_t) fault) << 10);
        } ਨਹੀਂਤਾਂ {
            iova = iotlb.translated_addr & ~iotlb.addr_ਨਕਾਬ;
            iova = ਸੈੱਟ_ਖੇਤਰ(0, RISCV_IOMMU_TR_RESPONSE_PPN, PPN_ਥੱਲੇ(iova));
        }
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE, iova);
    }

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
        RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    riscv_iommu_ctx_put(s, ref);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ riscv_iommu_process_fn(RISCVIOMMUState *s);

ਸਥਿਰ ਖਾਲੀ riscv_iommu_update_icvec(RISCVIOMMUState *s, uint64_t ਅੰਕੜੇ)
{
    uint64_t icvec = 0;

    icvec |= MIN(ਅੰਕੜੇ & RISCV_IOMMU_ICVEC_CIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_CIV);

    icvec |= MIN(ਅੰਕੜੇ & RISCV_IOMMU_ICVEC_FIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_FIV);

    icvec |= MIN(ਅੰਕੜੇ & RISCV_IOMMU_ICVEC_PMIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PMIV);

    icvec |= MIN(ਅੰਕੜੇ & RISCV_IOMMU_ICVEC_PIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PIV);

    trace_riscv_iommu_icvec_write(ਅੰਕੜੇ, icvec);

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_ICVEC, icvec);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_update_ipsr(RISCVIOMMUState *s, uint64_t ਅੰਕੜੇ)
{
    uint32_t cqcsr, fqcsr, pqcsr;
    uint32_t ipsr_ਸੈੱਟ = 0;
    uint32_t ipsr_clr = 0;

    ਜੇ (ਅੰਕੜੇ & RISCV_IOMMU_IPSR_CIP) {
        cqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);

        ਜੇ (cqcsr & RISCV_IOMMU_CQCSR_CIE &&
            (cqcsr & RISCV_IOMMU_CQCSR_FENCE_W_IP ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ILL ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ਮੰਜ਼ਿਲ ||
             cqcsr & RISCV_IOMMU_CQCSR_CQMF)) {
            ipsr_ਸੈੱਟ |= RISCV_IOMMU_IPSR_CIP;
        } ਨਹੀਂਤਾਂ {
            ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
        }
    } ਨਹੀਂਤਾਂ {
        ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
    }

    ਜੇ (ਅੰਕੜੇ & RISCV_IOMMU_IPSR_FIP) {
        fqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);

        ਜੇ (fqcsr & RISCV_IOMMU_FQCSR_FIE &&
            (fqcsr & RISCV_IOMMU_FQCSR_FQOF ||
             fqcsr & RISCV_IOMMU_FQCSR_FQMF)) {
            ipsr_ਸੈੱਟ |= RISCV_IOMMU_IPSR_FIP;
        } ਨਹੀਂਤਾਂ {
            ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
        }
    } ਨਹੀਂਤਾਂ {
        ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
    }

    ਜੇ (!(ਅੰਕੜੇ & RISCV_IOMMU_IPSR_PMIP)) {
        ipsr_clr |= RISCV_IOMMU_IPSR_PMIP;
    }

    ਜੇ (ਅੰਕੜੇ & RISCV_IOMMU_IPSR_PIP) {
        pqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);

        ਜੇ (pqcsr & RISCV_IOMMU_PQCSR_PIE &&
            (pqcsr & RISCV_IOMMU_PQCSR_PQOF ||
             pqcsr & RISCV_IOMMU_PQCSR_PQMF)) {
            ipsr_ਸੈੱਟ |= RISCV_IOMMU_IPSR_PIP;
        } ਨਹੀਂਤਾਂ {
            ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
        }
    } ਨਹੀਂਤਾਂ {
        ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, ipsr_ਸੈੱਟ, ipsr_clr);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_process_hpm_writes(RISCVIOMMUState *s,
                                           uint32_t regb,
                                           bool prev_cy_inh)
{
    ਚੋਣ (regb) {
    ਹਾਲਤ RISCV_IOMMU_REG_IOCOUNTINH:
        riscv_iommu_process_iocntinh_cy(s, prev_cy_inh);
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_REG_IOHPMCYCLES:
    ਹਾਲਤ RISCV_IOMMU_REG_IOHPMCYCLES + 4:
        riscv_iommu_process_hpmcycle_write(s);
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_REG_IOHPMEVT_ਅਧਾਰ ...
        RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_ਅੰਕ) + 4:
        riscv_iommu_process_hpmevt_write(s, regb & ~7);
        ਰੋਕੋ;
    }
}

/*
 * Write the resulting value of 'data' for the reg specified
 * by 'reg_addr', after considering read-only/read-write/write-clear
 * bits, in the pointer 'dest'.
 *
 * The result is written in little-endian.
 */
ਸਥਿਰ ਖਾਲੀ riscv_iommu_write_reg_val(RISCVIOMMUState *s,
                                      ਖਾਲੀ *dest, hwaddr reg_addr,
                                      ਪੂਰਨ ਮਾਪ, uint64_t ਅੰਕੜੇ)
{
    uint64_t ro = ldn_le_p(&s->regs_ro[reg_addr], ਮਾਪ);
    uint64_t wc = ldn_le_p(&s->regs_wc[reg_addr], ਮਾਪ);
    uint64_t curr_val = ldn_le_p(&s->regs[reg_addr], ਮਾਪ);

    stn_le_p(dest, ਮਾਪ, ((curr_val & ro) | (ਅੰਕੜੇ & ~ro)) & ~(ਅੰਕੜੇ & wc));
}

ਸਥਿਰ Mem_Tx_ਨਤੀਜਾ riscv_iommu_mmio_write(ਖਾਲੀ *opaque, hwaddr addr,
                                          uint64_t ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ,
                                          MemTxAttrs attrs)
{
    riscv_iommu_process_fn *process_fn = ਨਿਰਾ;
    RISCVIOMMUState *s = opaque;
    uint32_t regb = addr & ~3;
    uint32_t busy = 0;
    uint64_t val = 0;
    bool cy_inh = false;

    ਜੇ ((addr & (ਮਾਪ - 1)) != 0) {
        /* Unsupported MMIO alignment or access size */
        ਵਾਪਸ MEMTX_ERROR;
    }

    ਜੇ (addr + ਮਾਪ > RISCV_IOMMU_REG_MSI_CONFIG) {
        /* Unsupported MMIO access location. */
        ਵਾਪਸ MEMTX_ACCESS_ERROR;
    }

    /* Track actionable MMIO write. */
    ਚੋਣ (regb) {
    ਹਾਲਤ RISCV_IOMMU_REG_DDTP:
    ਹਾਲਤ RISCV_IOMMU_REG_DDTP + 4:
        process_fn = riscv_iommu_process_ddtp;
        regb = RISCV_IOMMU_REG_DDTP;
        busy = RISCV_IOMMU_DDTP_BUSY;
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_REG_CQT:
        process_fn = riscv_iommu_process_cq_ਪੂਛ;
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_REG_CQCSR:
        process_fn = riscv_iommu_process_cq_control;
        busy = RISCV_IOMMU_CQCSR_BUSY;
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_REG_FQCSR:
        process_fn = riscv_iommu_process_fq_control;
        busy = RISCV_IOMMU_FQCSR_BUSY;
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_REG_PQCSR:
        process_fn = riscv_iommu_process_pq_control;
        busy = RISCV_IOMMU_PQCSR_BUSY;
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_REG_ICVEC:
    ਹਾਲਤ RISCV_IOMMU_REG_IPSR:
        /*
         * ICVEC and IPSR have special read/write procedures. We'll
         * call their respective helpers and exit.
         */
        riscv_iommu_write_reg_val(s, &val, addr, ਮਾਪ, ਅੰਕੜੇ);

        /*
         * 'val' is stored as LE. Switch to host endianess
         * before using it.
         */
        val = le64_ਮੰਜ਼ਿਲ_cpu(val);

        ਜੇ (regb == RISCV_IOMMU_REG_ICVEC) {
            riscv_iommu_update_icvec(s, val);
        } ਨਹੀਂਤਾਂ {
            riscv_iommu_update_ipsr(s, val);
        }

        ਵਾਪਸ MEMTX_OK;

    ਹਾਲਤ RISCV_IOMMU_REG_TR_REQ_CTL:
        process_fn = riscv_iommu_process_dbg;
        regb = RISCV_IOMMU_REG_TR_REQ_CTL;
        busy = RISCV_IOMMU_TR_REQ_CTL_GO_BUSY;
        ਰੋਕੋ;

    ਹਾਲਤ RISCV_IOMMU_REG_IOCOUNTINH:
        ਜੇ (addr != RISCV_IOMMU_REG_IOCOUNTINH) {
            ਰੋਕੋ;
        }
        /* Store previous value of CY bit. */
        cy_inh = !!(riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTINH) &
            RISCV_IOMMU_IOCOUNTINH_CY);
        ਰੋਕੋ;


    ਮੂਲ:
        ਰੋਕੋ;
    }

    /*
     * Registers update might be not synchronized with core logic.
     * If system software updates register when relevant BUSY bit
     * is set IOMMU behavior of additional writes to the register
     * is UNSPECIFIED.
     */
    riscv_iommu_write_reg_val(s, &s->regs[addr], addr, ਮਾਪ, ਅੰਕੜੇ);

    /* Busy flag update, MSB 4-byte register. */
    ਜੇ (busy) {
        uint32_t rw = ldl_le_p(&s->regs[regb]);
        stl_le_p(&s->regs[regb], rw | busy);
    }

    /* Process HPM writes and update any internal state if needed. */
    ਜੇ (regb >= RISCV_IOMMU_REG_IOCOUNTOVF &&
        regb <= (RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_ਅੰਕ) + 4)) {
        riscv_iommu_process_hpm_writes(s, regb, cy_inh);
    }

    ਜੇ (process_fn) {
        process_fn(s);
    }

    ਵਾਪਸ MEMTX_OK;
}

ਸਥਿਰ Mem_Tx_ਨਤੀਜਾ riscv_iommu_mmio_read(ਖਾਲੀ *opaque, hwaddr addr,
    uint64_t *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ, MemTxAttrs attrs)
{
    RISCVIOMMUState *s = opaque;
    uint64_t val = -1;
    uint8_t *ptr;

    ਜੇ ((addr & (ਮਾਪ - 1)) != 0) {
        /* Unsupported MMIO alignment. */
        ਵਾਪਸ MEMTX_ERROR;
    }

    ਜੇ (addr + ਮਾਪ > RISCV_IOMMU_REG_MSI_CONFIG) {
        ਵਾਪਸ MEMTX_ACCESS_ERROR;
    }

    /* Compute cycle register value. */
    ਜੇ ((addr & ~7) == RISCV_IOMMU_REG_IOHPMCYCLES) {
        val = riscv_iommu_hpmcycle_read(s);
        ptr = (uint8_t *)&val + (addr & 7);
    } ਨਹੀਂਤਾਂ ਜੇ ((addr & ~3) == RISCV_IOMMU_REG_IOCOUNTOVF) {
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
    } ਨਹੀਂਤਾਂ {
        ptr = &s->regs[addr];
    }

    val = ldn_le_p(ptr, ਮਾਪ);

    *ਅੰਕੜੇ = val;

    ਵਾਪਸ MEMTX_OK;
}

ਸਥਿਰ ਅਚਲ ਸਮ੍ਰਿਤੀ_Region_Ops riscv_iommu_mmio_ops = {
    .read_with_attrs = riscv_iommu_mmio_read,
    .write_with_attrs = riscv_iommu_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_ਮਾਪ = 4,
        .max_access_ਮਾਪ = 8,
        .unaligned = false,
    },
    .valid = {
        .min_access_ਮਾਪ = 4,
        .max_access_ਮਾਪ = 8,
    }
};

/*
 * Translations matching MSI pattern check are redirected to "riscv-iommu-trap"
 * memory region as untranslated address, for additional MSI/MRIF interception
 * by IOMMU interrupt remapping implementation.
 * Note: Device emulation code generating an MSI is expected to provide a valid
 * memory transaction attributes with requested_id set.
 */
ਸਥਿਰ Mem_Tx_ਨਤੀਜਾ riscv_iommu_trap_write(ਖਾਲੀ *opaque, hwaddr addr,
    uint64_t ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ, MemTxAttrs attrs)
{
    RISCVIOMMUState* s = (RISCVIOMMUState *)opaque;
    RISCVIOMMUContext *ctx;
    Mem_Tx_ਨਤੀਜਾ res;
    ਖਾਲੀ *ref;
    uint32_t devid = attrs.requester_id;

    ਜੇ (attrs.unspecified) {
        ਵਾਪਸ MEMTX_ACCESS_ERROR;
    }

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= s->bus << 8;

    ctx = riscv_iommu_ctx(s, devid, 0, IOMMU_RW, addr, &ref);
    ਜੇ (ctx == ਨਿਰਾ) {
        res = MEMTX_ACCESS_ERROR;
    } ਨਹੀਂਤਾਂ {
        res = riscv_iommu_msi_write(s, ctx, addr, ਅੰਕੜੇ, ਮਾਪ, attrs);
    }
    riscv_iommu_ctx_put(s, ref);
    ਵਾਪਸ res;
}

ਸਥਿਰ Mem_Tx_ਨਤੀਜਾ riscv_iommu_trap_read(ਖਾਲੀ *opaque, hwaddr addr,
    uint64_t *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ, MemTxAttrs attrs)
{
    ਵਾਪਸ MEMTX_ACCESS_ERROR;
}

ਸਥਿਰ ਅਚਲ ਸਮ੍ਰਿਤੀ_Region_Ops riscv_iommu_trap_ops = {
    .read_with_attrs = riscv_iommu_trap_read,
    .write_with_attrs = riscv_iommu_trap_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_ਮਾਪ = 4,
        .max_access_ਮਾਪ = 8,
    },
    .valid = {
        .min_access_ਮਾਪ = 4,
        .max_access_ਮਾਪ = 8,
    }
};

ਖਾਲੀ riscv_iommu_ਸੈੱਟ_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_ਬਹੁਲਕ ਬਹੁਲਕ)
{
    s->cap = ਸੈੱਟ_ਖੇਤਰ(s->cap, RISCV_IOMMU_CAP_IGS, ਬਹੁਲਕ);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_instance_init(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    /* Enable translation debug interface */
    s->cap = RISCV_IOMMU_CAP_DBG;

    /* TODO: method to report supported PID bits */
    s->pid_ਬਿੱਟ = 8; /* restricted to size of MemTxAttrs.pid */
    s->cap |= RISCV_IOMMU_CAP_PD8;

    /* register storage */
    s->regs = g_new0(uint8_t, RISCV_IOMMU_REG_ਮਾਪ);
    s->regs_ro = g_new0(uint8_t, RISCV_IOMMU_REG_ਮਾਪ);
    s->regs_wc = g_new0(uint8_t, RISCV_IOMMU_REG_ਮਾਪ);

     /* Mark all registers read-only */
    memset(s->regs_ro, 0xff, RISCV_IOMMU_REG_ਮਾਪ);

    /* Device translation context cache */
    s->ctx_cache = g_hash_ਸਾਰਣੀ_new_ਭਰਿਆ(riscv_iommu_ctx_hash,
                                         riscv_iommu_ctx_equal,
                                         g_free, ਨਿਰਾ);

    s->iot_cache = g_hash_ਸਾਰਣੀ_new_ਭਰਿਆ(riscv_iommu_iot_hash,
                                         riscv_iommu_iot_equal,
                                         g_free, ਨਿਰਾ);

    s->iommus.le_ਅਗਲਾ = ਨਿਰਾ;
    s->iommus.le_prev = ਨਿਰਾ;
    QLIST_INIT(&s->spaces);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_instance_finalize(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    g_free(s->regs);
    g_free(s->regs_ro);
    g_free(s->regs_wc);

    g_hash_ਸਾਰਣੀ_unref(s->ctx_cache);
    g_hash_ਸਾਰਣੀ_unref(s->iot_cache);
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    /* Report QEMU target physical address space limits. */
    s->cap = ਸੈੱਟ_ਖੇਤਰ(s->cap, RISCV_IOMMU_CAP_PAS, s->pas_ਬਿੱਟ);

    s->cap |= s->ਸੰਸਕਰਣ & RISCV_IOMMU_CAP_ਸੰਸਕਰਣ;
    ਜੇ (s->enable_msi) {
        s->cap |= RISCV_IOMMU_CAP_MSI_FLAT | RISCV_IOMMU_CAP_MSI_MRIF;
    }
    ਜੇ (s->enable_ats) {
        s->cap |= RISCV_IOMMU_CAP_ATS;
    }
    ਜੇ (s->enable_s_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32 | RISCV_IOMMU_CAP_SV39 |
                  RISCV_IOMMU_CAP_SV48 | RISCV_IOMMU_CAP_SV57;
    }
    ਜੇ (s->enable_g_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32X4 | RISCV_IOMMU_CAP_SV39X4 |
                  RISCV_IOMMU_CAP_SV48X4 | RISCV_IOMMU_CAP_SV57X4 |
                  RISCV_IOMMU_CAP_SVRSW60T59B;
    }

    ਜੇ (s->hpm_cntrs > 0) {
        /* Clip number of HPM counters to maximum supported (31). */
        ਜੇ (s->hpm_cntrs > RISCV_IOMMU_IOCOUNT_ਅੰਕ) {
            s->hpm_cntrs = RISCV_IOMMU_IOCOUNT_ਅੰਕ;
        }
        /* Enable hardware performance monitor interface */
        s->cap |= RISCV_IOMMU_CAP_HPM;
    }

    /* Out-of-reset translation mode: OFF (DMA disabled) BARE (passthrough) */
    s->ddtp = ਸੈੱਟ_ਖੇਤਰ(0, RISCV_IOMMU_DDTP_ਬਹੁਲਕ, s->enable_off ?
                        RISCV_IOMMU_DDTP_ਬਹੁਲਕ_OFF : RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE);

    /*
     * Register complete MMIO space, including MSI/PBA registers.
     * Note, PCIDevice implementation will add overlapping MR for MSI/PBA,
     * managed directly by the PCIDevice implementation.
     */
    ਸਮ੍ਰਿਤੀ_region_init_io(&s->regs_mr, OBJECT(dev), &riscv_iommu_mmio_ops, s,
        "riscv-iommu-regs", RISCV_IOMMU_REG_ਮਾਪ);

    /* Set power-on register state */
    stq_le_p(&s->regs[RISCV_IOMMU_REG_CAP], s->cap);

    stq_le_p(&s->regs[RISCV_IOMMU_REG_FCTL], 0);
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FCTL],
             ~(RISCV_IOMMU_FCTL_GXL | RISCV_IOMMU_FCTL_WSI));

    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_DDTP],
        ~(RISCV_IOMMU_DDTP_PPN | RISCV_IOMMU_DDTP_ਬਹੁਲਕ));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQB],
        ~(RISCV_IOMMU_CQB_LOG2SZ | RISCV_IOMMU_CQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQB],
        ~(RISCV_IOMMU_FQB_LOG2SZ | RISCV_IOMMU_FQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQB],
        ~(RISCV_IOMMU_PQB_LOG2SZ | RISCV_IOMMU_PQB_PPN));
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_CQCSR], RISCV_IOMMU_CQCSR_CQMF |
        RISCV_IOMMU_CQCSR_CMD_ਮੰਜ਼ਿਲ | RISCV_IOMMU_CQCSR_CMD_ILL);
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
    ਜੇ (s->cap & RISCV_IOMMU_CAP_DBG) {
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_IOVA], 0);
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_CTL],
            RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    }

    /* If HPM registers are enabled. */
    ਜੇ (s->cap & RISCV_IOMMU_CAP_HPM) {
        /* +1 for cycle counter bit. */
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOCOUNTINH],
                 ~((2 << s->hpm_cntrs) - 1));
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCYCLES], 0);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCTR_ਅਧਾਰ],
               0x00, s->hpm_cntrs * 8);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMEVT_ਅਧਾਰ],
               0x00, s->hpm_cntrs * 8);
    }

    /* Memory region for downstream access, if specified. */
    ਜੇ (s->ਨਿਸ਼ਾਨਾ_mr) {
        s->ਨਿਸ਼ਾਨਾ_as = g_new0(ਪਤਾ_Space, 1);
        ਪਤਾ_space_init(s->ਨਿਸ਼ਾਨਾ_as, s->ਨਿਸ਼ਾਨਾ_mr,
            "riscv-iommu-downstream");
    } ਨਹੀਂਤਾਂ {
        /* Fallback to global system memory. */
        s->ਨਿਸ਼ਾਨਾ_as = &ਪਤਾ_space_ਸਮ੍ਰਿਤੀ;
    }

    /* Memory region for untranslated MRIF/MSI writes */
    ਸਮ੍ਰਿਤੀ_region_init_io(&s->trap_mr, OBJECT(dev), &riscv_iommu_trap_ops, s,
            "riscv-iommu-trap", ~0ULL);
    ਪਤਾ_space_init(&s->trap_as, &s->trap_mr, "riscv-iommu-trap-as");

    ਜੇ (s->cap & RISCV_IOMMU_CAP_HPM) {
        s->hpm_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL, riscv_iommu_hpm_timer_cb, s);
        s->hpm_event_ctr_map = g_hash_ਸਾਰਣੀ_new(g_ਸਿੱਧਾ_hash, g_ਸਿੱਧਾ_equal);
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_unrealize(DeviceState *dev)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    ਜੇ (s->cap & RISCV_IOMMU_CAP_HPM) {
        g_hash_ਸਾਰਣੀ_unref(s->hpm_event_ctr_map);
        timer_free(s->hpm_timer);
    }
}

ਖਾਲੀ riscv_iommu_reset(RISCVIOMMUState *s)
{
    uint32_t reg_clr;
    ਪੂਰਨ ddtp_ਬਹੁਲਕ;

    /*
     * Clear DDTP while setting DDTP_mode back to user
     * initial setting.
     */
    ddtp_ਬਹੁਲਕ = s->enable_off ?
                RISCV_IOMMU_DDTP_ਬਹੁਲਕ_OFF : RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE;
    s->ddtp = ਸੈੱਟ_ਖੇਤਰ(0, RISCV_IOMMU_DDTP_ਬਹੁਲਕ, ddtp_ਬਹੁਲਕ);
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

    g_hash_ਸਾਰਣੀ_remove_all(s->ctx_cache);
    g_hash_ਸਾਰਣੀ_remove_all(s->iot_cache);
}

ਸਥਿਰ ਅਚਲ Property riscv_iommu_properties[] = {
    DEFINE_PROP_UINT32("version", RISCVIOMMUState, ਸੰਸਕਰਣ,
        RISCV_IOMMU_SPEC_DOT_VER),
    DEFINE_PROP_UINT32("pas-bits", RISCVIOMMUState, pas_ਬਿੱਟ, 0),
    DEFINE_PROP_UINT32("bus", RISCVIOMMUState, bus, 0x0),
    DEFINE_PROP_UINT32("ioatc-limit", RISCVIOMMUState, iot_ਹੱਦਬੰਦੀ,
        ਹੱਦਬੰਦੀ_CACHE_IOT),
    DEFINE_PROP_BOOL("intremap", RISCVIOMMUState, enable_msi, TRUE),
    DEFINE_PROP_BOOL("ats", RISCVIOMMUState, enable_ats, TRUE),
    DEFINE_PROP_BOOL("off", RISCVIOMMUState, enable_off, TRUE),
    DEFINE_PROP_BOOL("s-stage", RISCVIOMMUState, enable_s_stage, TRUE),
    DEFINE_PROP_BOOL("g-stage", RISCVIOMMUState, enable_g_stage, TRUE),
    DEFINE_PROP_LINK("downstream-mr", RISCVIOMMUState, ਨਿਸ਼ਾਨਾ_mr,
        TYPE_ਸਮ੍ਰਿਤੀ_REGION, ਸਮ੍ਰਿਤੀ_Region *),
    DEFINE_PROP_UINT8("hpm-counters", RISCVIOMMUState, hpm_cntrs,
                      RISCV_IOMMU_IOCOUNT_ਅੰਕ),
};

ਸਥਿਰ ਖਾਲੀ riscv_iommu_class_init(ObjectClass *klass, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    /* internal device for riscv-iommu-{pci/sys}, not user-creatable */
    dc->user_creatable = false;
    dc->realize = riscv_iommu_realize;
    dc->unrealize = riscv_iommu_unrealize;
    device_class_ਸੈੱਟ_props(dc, riscv_iommu_properties);
}

ਸਥਿਰ ਅਚਲ TypeInfo riscv_iommu_info = {
    .name = TYPE_RISCV_IOMMU,
    .ਮਾਪਾ = TYPE_DEVICE,
    .instance_ਮਾਪ = ਆਕਾਰ(RISCVIOMMUState),
    .instance_init = riscv_iommu_instance_init,
    .instance_finalize = riscv_iommu_instance_finalize,
    .class_init = riscv_iommu_class_init,
};

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *IOMMU_FLAG_STR[] = {
    "NA",
    "RO",
    "WR",
    "RW",
};

/* RISC-V IOMMU Memory Region - Address Translation Space */
ਸਥਿਰ IOMMUTLBEntry riscv_iommu_ਸਮ੍ਰਿਤੀ_region_translate(
    IOMMUMemoryRegion *iommu_mr, hwaddr addr,
    IOMMUAccessFlags flag, ਪੂਰਨ iommu_idx)
{
    RISCVIOMMUSpace *as = container_ਦਾ(iommu_mr, RISCVIOMMUSpace, iova_mr);
    RISCVIOMMUContext *ctx;
    ਖਾਲੀ *ref;
    IOMMUTLBEntry iotlb = {
        .iova = addr,
        .ਨਿਸ਼ਾਨਾ_as = as->iommu->ਨਿਸ਼ਾਨਾ_as,
        .addr_ਨਕਾਬ = ~0ULL,
        .perm = flag,
    };
    uint32_t devid = riscv_iommu_space_devid(as);

    ctx = riscv_iommu_ctx(as->iommu, devid, iommu_idx, flag, addr, &ref);
    ਜੇ (ctx == ਨਿਰਾ) {
        /* Translation disabled or invalid. */
        iotlb.addr_ਨਕਾਬ = 0;
        iotlb.perm = IOMMU_NONE;
    } ਨਹੀਂਤਾਂ ਜੇ (riscv_iommu_translate(as->iommu, ctx, &iotlb, true)) {
        /* Translation disabled or fault reported. */
        iotlb.addr_ਨਕਾਬ = 0;
        iotlb.perm = IOMMU_NONE;
    }

    /* Trace all dma translations with original access flags. */
    trace_riscv_iommu_dma(as->iommu->ਮਾਪਾ_obj.id, PCI_BUS_ਅੰਕ(devid),
                          PCI_ਥਾਂ(devid), PCI_FUNC(devid), iommu_idx,
                          IOMMU_FLAG_STR[flag & IOMMU_RW], iotlb.iova,
                          iotlb.translated_addr);

    riscv_iommu_ctx_put(as->iommu, ref);

    ਵਾਪਸ iotlb;
}

ਸਥਿਰ ਪੂਰਨ riscv_iommu_ਸਮ੍ਰਿਤੀ_region_notify(
    IOMMUMemoryRegion *iommu_mr, IOMMUNotifierFlag old,
    IOMMUNotifierFlag new, Error **errp)
{
    RISCVIOMMUSpace *as = container_ਦਾ(iommu_mr, RISCVIOMMUSpace, iova_mr);

    ਜੇ (old == IOMMU_NOTIFIER_NONE) {
        as->notifier = true;
        trace_riscv_iommu_notifier_ਜੋੜੋ(iommu_mr->ਮਾਪਾ_obj.name);
    } ਨਹੀਂਤਾਂ ਜੇ (new == IOMMU_NOTIFIER_NONE) {
        as->notifier = false;
        trace_riscv_iommu_notifier_del(iommu_mr->ਮਾਪਾ_obj.name);
    }

    ਵਾਪਸ 0;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool pci_is_iommu(PCIDevice *pdev)
{
    ਵਾਪਸ pci_get_ਸ਼ਬਦ(pdev->config + PCI_CLASS_DEVICE) == 0x0806;
}

ਸਥਿਰ ਪਤਾ_Space *riscv_iommu_ਲੱਭੋ_as(PCIBus *bus, ਖਾਲੀ *opaque, ਪੂਰਨ devfn)
{
    RISCVIOMMUState *s = (RISCVIOMMUState *) opaque;
    PCIDevice *pdev = pci_ਲੱਭੋ_device(bus, pci_bus_ਅੰਕ(bus), devfn);
    ਪਤਾ_Space *as = ਨਿਰਾ;

    ਜੇ (pdev && pci_is_iommu(pdev)) {
        ਵਾਪਸ s->ਨਿਸ਼ਾਨਾ_as;
    }

    /* Find first registered IOMMU device */
    ਜਦੋਂ (s->iommus.le_prev) {
        s = *(s->iommus.le_prev);
    }

    /* Find first matching IOMMU */
    ਜਦੋਂ (s != ਨਿਰਾ && as == ਨਿਰਾ) {
        as = riscv_iommu_space(s, bus, devfn);
        s = s->iommus.le_ਅਗਲਾ;
    }

    ਵਾਪਸ as ? as : &ਪਤਾ_space_ਸਮ੍ਰਿਤੀ;
}

ਸਥਿਰ ਅਚਲ PCIIOMMUOps riscv_iommu_ops = {
    .get_ਪਤਾ_space = riscv_iommu_ਲੱਭੋ_as,
};

ਖਾਲੀ riscv_iommu_pci_ਤਿਆਰੀ_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
        Error **errp)
{
    ਜੇ (bus->iommu_ops &&
        bus->iommu_ops->get_ਪਤਾ_space == riscv_iommu_ਲੱਭੋ_as) {
        /* Allow multiple IOMMUs on the same PCIe bus, link known devices */
        RISCVIOMMUState *ਆਖਰੀ = (RISCVIOMMUState *)bus->iommu_opaque;
        QLIST_ਪਾਓ_AFTER(ਆਖਰੀ, iommu, iommus);
    } ਨਹੀਂਤਾਂ ਜੇ (!bus->iommu_ops && !bus->iommu_opaque) {
        pci_ਤਿਆਰੀ_iommu(bus, &riscv_iommu_ops, iommu);
    } ਨਹੀਂਤਾਂ {
        error_setg(errp, "can't register secondary IOMMU for PCI bus #%d",
            pci_bus_ਅੰਕ(bus));
    }
}

ਸਥਿਰ ਪੂਰਨ riscv_iommu_ਸਮ੍ਰਿਤੀ_region_ਸੂਚਕਾਂਕ(IOMMUMemoryRegion *iommu_mr,
    MemTxAttrs attrs)
{
    ਵਾਪਸ attrs.unspecified ? RISCV_IOMMU_NOPROCID : (ਪੂਰਨ)attrs.pid;
}

ਸਥਿਰ ਪੂਰਨ riscv_iommu_ਸਮ੍ਰਿਤੀ_region_ਸੂਚਕਾਂਕ_len(IOMMUMemoryRegion *iommu_mr)
{
    RISCVIOMMUSpace *as = container_ਦਾ(iommu_mr, RISCVIOMMUSpace, iova_mr);
    ਵਾਪਸ 1 << as->iommu->pid_ਬਿੱਟ;
}

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ਸਮ੍ਰਿਤੀ_region_init(ObjectClass *klass, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    IOMMUMemoryRegionClass *imrc = IOMMU_ਸਮ੍ਰਿਤੀ_REGION_CLASS(klass);

    imrc->translate = riscv_iommu_ਸਮ੍ਰਿਤੀ_region_translate;
    imrc->notify_flag_ਬਦਲਿਆ = riscv_iommu_ਸਮ੍ਰਿਤੀ_region_notify;
    imrc->attrs_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ = riscv_iommu_ਸਮ੍ਰਿਤੀ_region_ਸੂਚਕਾਂਕ;
    imrc->ਅੰਕ_indexes = riscv_iommu_ਸਮ੍ਰਿਤੀ_region_ਸੂਚਕਾਂਕ_len;
}

ਸਥਿਰ ਅਚਲ TypeInfo riscv_iommu_ਸਮ੍ਰਿਤੀ_region_info = {
    .ਮਾਪਾ = TYPE_IOMMU_ਸਮ੍ਰਿਤੀ_REGION,
    .name = TYPE_RISCV_IOMMU_ਸਮ੍ਰਿਤੀ_REGION,
    .class_init = riscv_iommu_ਸਮ੍ਰਿਤੀ_region_init,
};

ਸਥਿਰ ਖਾਲੀ riscv_iommu_ਰਜਿਸਟਰ_mr_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&riscv_iommu_ਸਮ੍ਰਿਤੀ_region_info);
    type_ਰਜਿਸਟਰ_static(&riscv_iommu_info);
}

type_init(riscv_iommu_ਰਜਿਸਟਰ_mr_types);
