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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qom/object.h"
#ಅನ್ತರ್ಭಾವ "exec/target_page.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pci_bus.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pci_device.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "migration/vmstate.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/timer.h"
#ಅನ್ತರ್ಭಾವ "qemu/target-info.h"
#ಅನ್ತರ್ಭಾವ "qemu/bitops.h"

#ಅನ್ತರ್ಭಾವ "cpu_bits.h"
#ಅನ್ತರ್ಭಾವ "riscv-iommu.h"
#ಅನ್ತರ್ಭಾವ "riscv-iommu-bits.h"
#ಅನ್ತರ್ಭಾವ "riscv-iommu-hpm.h"
#ಅನ್ತರ್ಭಾವ "trace.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ಎಲ್ಲೆ_CACHE_CTX               (1U << 7)
#ವ್ಯಾಖ್ಯಾನಿಸು ಎಲ್ಲೆ_CACHE_IOT               (1U << 20)

/* Physical page number coversions */
#ವ್ಯಾಖ್ಯಾನಿಸು PPN_PHYS(ppn)                 ((ppn) << ಗುರಿ_ಪುಟ_ಬಿಟ್ಟುಗಳು)
#ವ್ಯಾಖ್ಯಾನಿಸು PPN_ಕೆಳಕ್ಕೆ(phy)                 ((phy) >> ಗುರಿ_ಪುಟ_ಬಿಟ್ಟುಗಳು)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ RISCVIOMMUEntry RISCVIOMMUEntry;

/* Device assigned I/O address space */
ರಚನೆ RISCVIOMMUSpace {
    IOMMUMemoryRegion iova_mr;  /* IOVA memory region for attached device */
    ವಿಳಾಸ_Space iova_as;       /* IOVA address space for attached device */
    RISCVIOMMUState *iommu;     /* Managing IOMMU device state */
    PCIBus *bus;                /* PCI bus of the requester */
    uint8_t devfn;              /* Requester identifier, AKA device_id */
    bool notifier;              /* IOMMU unmap notifier enabled */
    QLIST_ನಮೂದು(RISCVIOMMUSpace) list;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ RISCVIOMMUTrans_ಟ್ಯಾಗ್ {
    RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_BY,  /* Bypass */
    RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_SS,  /* Single Stage */
    RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_VG,  /* G-stage only */
    RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_VN,  /* Nested translation */
} RISCVIOMMUTrans_ಟ್ಯಾಗ್;

/* Address translation cache entry */
ರಚನೆ RISCVIOMMUEntry {
    RISCVIOMMUTrans_ಟ್ಯಾಗ್ ಟ್ಯಾಗ್;     /* Translation Tag */
    uint64_t iova:44;           /* IOVA Page Number */
    uint64_t pscid:20;          /* Process Soft-Context identifier */
    uint64_t phys:44;           /* Physical Page Number */
    uint64_t gscid:16;          /* Guest Soft-Context identifier */
    uint64_t perm:2;            /* IOMMU_RW flags */
};

/* IOMMU index for transactions without process_id specified. */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_NOPROCID 0

ಸ್ಥಿರ uint32_t riscv_iommu_space_devid(RISCVIOMMUSpace *as)
{
    uint32_t devid = PCI_BUILD_BDF(pci_bus_ಅಂಕೆ(as->bus), as->devfn);

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= as->iommu->bus << 8;
    ಹಿಂತಿರುಗಿ devid;
}

ಸ್ಥಿರ uint8_t riscv_iommu_get_icvec_ಸದಿಶ(uint32_t icvec, uint32_t vec_type)
{
    ಆಯ್ಕೆ (vec_type) {
    ಸ್ಥಿತಿ RISCV_IOMMU_INTR_CQ:
        ಹಿಂತಿರುಗಿ icvec & RISCV_IOMMU_ICVEC_CIV;
    ಸ್ಥಿತಿ RISCV_IOMMU_INTR_FQ:
        ಹಿಂತಿರುಗಿ (icvec & RISCV_IOMMU_ICVEC_FIV) >> 4;
    ಸ್ಥಿತಿ RISCV_IOMMU_INTR_PM:
        ಹಿಂತಿರುಗಿ (icvec & RISCV_IOMMU_ICVEC_PMIV) >> 8;
    ಸ್ಥಿತಿ RISCV_IOMMU_INTR_PQ:
        ಹಿಂತಿರುಗಿ (icvec & RISCV_IOMMU_ICVEC_PIV) >> 12;
    ಪೂರ್ವನಿಯೋಜಿತ:
        g_assert_not_reached();
    }
}

ಶೂನ್ಯ riscv_iommu_notify(RISCVIOMMUState *s, ಪೂರ್ಣಾಂಕ vec_type)
{
    uint32_t ipsr, icvec, ಸದಿಶ;

    ಆದರೆ (!s->notify) {
        ಹಿಂತಿರುಗಿ;
    }

    icvec = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_ICVEC);
    ipsr = riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, (1 << vec_type), 0);

    ಆದರೆ (!(ipsr & (1 << vec_type))) {
        ಸದಿಶ = riscv_iommu_get_icvec_ಸದಿಶ(icvec, vec_type);
        s->notify(s, ಸದಿಶ);
        trace_riscv_iommu_notify_int_ಸದಿಶ(vec_type, ಸದಿಶ);
    }
}

ಶೂನ್ಯ riscv_iommu_fault(RISCVIOMMUState *s, ರಚನೆ riscv_iommu_fq_ದಾಖಲೆ *ev)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ತಲೆ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQH) & s->fq_ಮುಖವಾಡ;
    uint32_t ಬಾಲ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQT) & s->fq_ಮುಖವಾಡ;
    uint32_t ಮುಂದಿನ = (ಬಾಲ + 1) & s->fq_ಮುಖವಾಡ;
    uint32_t devid = get_ಕ್ಷೇತ್ರ(ev->hdr, RISCV_IOMMU_FQ_HDR_DID);

    trace_riscv_iommu_flt(s->ಪೋಷಕ_obj.id, PCI_BUS_ಅಂಕೆ(devid), PCI_ಸ್ಥಾನ(devid),
                          PCI_FUNC(devid), ev->hdr, ev->iotval);

    ಆದರೆ (!(ctrl & RISCV_IOMMU_FQCSR_FQON) ||
        !!(ctrl & (RISCV_IOMMU_FQCSR_FQOF | RISCV_IOMMU_FQCSR_FQMF))) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ತಲೆ == ಮುಂದಿನ) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                              RISCV_IOMMU_FQCSR_FQOF, 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        dma_addr_t addr = s->fq_addr + ಬಾಲ * ಗಾತ್ರ(*ev);
        ಆದರೆ (dma_ಸ್ಮರಣೆ_write(s->ಗುರಿ_as, addr, ev, ಗಾತ್ರ(*ev),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                                  RISCV_IOMMU_FQCSR_FQMF, 0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_FQT, ಮುಂದಿನ);
        }
    }

    ಆದರೆ (ctrl & RISCV_IOMMU_FQCSR_FIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_FQ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_pri(RISCVIOMMUState *s,
    ರಚನೆ riscv_iommu_pq_ದಾಖಲೆ *pr)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ತಲೆ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQH) & s->pq_ಮುಖವಾಡ;
    uint32_t ಬಾಲ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQT) & s->pq_ಮುಖವಾಡ;
    uint32_t ಮುಂದಿನ = (ಬಾಲ + 1) & s->pq_ಮುಖವಾಡ;
    uint32_t devid = get_ಕ್ಷೇತ್ರ(pr->hdr, RISCV_IOMMU_PREQ_HDR_DID);

    trace_riscv_iommu_pri(s->ಪೋಷಕ_obj.id, PCI_BUS_ಅಂಕೆ(devid), PCI_ಸ್ಥಾನ(devid),
                          PCI_FUNC(devid), pr->payload);

    ಆದರೆ (!(ctrl & RISCV_IOMMU_PQCSR_PQON) ||
        !!(ctrl & (RISCV_IOMMU_PQCSR_PQOF | RISCV_IOMMU_PQCSR_PQMF))) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ತಲೆ == ಮುಂದಿನ) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                              RISCV_IOMMU_PQCSR_PQOF, 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        dma_addr_t addr = s->pq_addr + ಬಾಲ * ಗಾತ್ರ(*pr);
        ಆದರೆ (dma_ಸ್ಮರಣೆ_write(s->ಗುರಿ_as, addr, pr, ಗಾತ್ರ(*pr),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                                  RISCV_IOMMU_PQCSR_PQMF, 0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_PQT, ಮುಂದಿನ);
        }
    }

    ಆದರೆ (ctrl & RISCV_IOMMU_PQCSR_PIE) {
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
ಸ್ಥಿರ uint64_t riscv_iommu_pext_u64(uint64_t val, uint64_t ext)
{
    uint64_t ret = 0;
    uint64_t rot = 1;

    ಆಗ (ext) {
        ಆದರೆ (ext & 1) {
            ಆದರೆ (val & 1) {
                ret |= rot;
            }
            rot <<= 1;
        }
        val >>= 1;
        ext >>= 1;
    }

    ಹಿಂತಿರುಗಿ ret;
}

/* Check if GPA matches MSI/MRIF pattern. */
ಸ್ಥಿರ bool riscv_iommu_msi_check(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    dma_addr_t gpa)
{
    ಆದರೆ (!s->enable_msi) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (get_ಕ್ಷೇತ್ರ(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_ಬಹುಲಕ) !=
        RISCV_IOMMU_DC_MSIPTP_ಬಹುಲಕ_FLAT) {
        ಹಿಂತಿರುಗಿ false; /* Invalid MSI/MRIF mode */
    }

    ಆದರೆ ((PPN_ಕೆಳಕ್ಕೆ(gpa) ^ ctx->msi_addr_ನಮೂನೆ) & ~ctx->msi_addr_ಮುಖವಾಡ) {
        ಹಿಂತಿರುಗಿ false; /* GPA not in MSI range defined by AIA IMSIC rules. */
    }

    ಹಿಂತಿರುಗಿ true;
}

/* Returns the NAPOT page mask, or 0 for reserved encodings. */
ಸ್ಥಿರ hwaddr riscv_iommu_napot_ಪುಟ_ಮುಖವಾಡ(hwaddr ppn, hwaddr addr, hwaddr *ಹೊರಹೋಗು)
{
    ಪೂರ್ಣಾಂಕ napot_ಬಿಟ್ಟುಗಳು = ctz64(ppn) + 1;
    hwaddr napot_ಮುಖವಾಡ, ಪುಟ_ಮುಖವಾಡ;

    /* The spec only defines 64KB (napot_bits == 4) */
    ಆದರೆ (napot_ಬಿಟ್ಟುಗಳು != 4) {
        ಹಿಂತಿರುಗಿ 0;
    }

    napot_ಮುಖವಾಡ = (1ULL << napot_ಬಿಟ್ಟುಗಳು) - 1;
    ಪುಟ_ಮುಖವಾಡ = PPN_PHYS(napot_ಮುಖವಾಡ) | (ಗುರಿ_ಪುಟ_ಗಾತ್ರ - 1);

    *ಹೊರಹೋಗು = PPN_PHYS(ppn & ~napot_ಮುಖವಾಡ) | (addr & ಪುಟ_ಮುಖವಾಡ);

    ಹಿಂತಿರುಗಿ ಪುಟ_ಮುಖವಾಡ;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ riscv_iommu_spa_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb)
{
    IOMMUAccessFlags trans_perm = IOMMU_NONE;
    dma_addr_t addr, ತಳ;
    uint64_t satp, gatp, pte;
    bool en_s, en_g;
    ರಚನೆ {
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಹೆಜ್ಜೆ;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಮಟ್ಟಗಳು;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ptidxbits;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ptesize;
    } sc[2];
    /* Translation stage phase */
    ಎಣಿಕೆ {
        S_STAGE = 0,
        G_STAGE = 1,
    } pass;
    Mem_Tx_ಫಲಿತಾಂಶ ret;
    bool pv = !!ctx->process_id;
    /*
     * Keep the request permission separate from iotlb->perm.  G-stage
     * walks translate S-stage PTE addresses before the real leaf is
     * reached, but permission checks and fault types must still use the
     * original request.  A successful walk leaves iotlb->perm with the
     * effective leaf permission for the translation cache.
     */
    ಸ್ಥಿರಾಂಕ IOMMUAccessFlags req_perm = iotlb->perm;

    satp = get_ಕ್ಷೇತ್ರ(ctx->satp, RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ);
    gatp = get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ);

    en_s = satp != RISCV_IOMMU_DC_FSC_ಬಹುಲಕ_BARE;
    en_g = gatp != RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE;

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
    ಆದರೆ (!en_s && (req_perm & IOMMU_WO) &&
        riscv_iommu_msi_check(s, ctx, iotlb->iova)) {
        iotlb->ಗುರಿ_as = &s->trap_as;
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_ಮುಖವಾಡ = ~ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* Exit early for pass-through mode. */
    ಆದರೆ (!(en_s || en_g)) {
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_ಮುಖವಾಡ = ~ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;
        /* Allow R/W in pass-through mode */
        iotlb->perm = IOMMU_RW;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* S/G translation parameters. */
    ಗಾಗಿ (pass = 0; pass < 2; pass++) {
        uint32_t sv_ಬಹುಲಕ;

        sc[pass].ಹೆಜ್ಜೆ = 0;
        ಆದರೆ (pass ? (s->fctl & RISCV_IOMMU_FCTL_GXL) :
            (ctx->tc & RISCV_IOMMU_DC_TC_SXL)) {
            /* 32bit mode for GXL/SXL == 1 */
            ಆಯ್ಕೆ (pass ? gatp : satp) {
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE:
                sc[pass].ಮಟ್ಟಗಳು    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV32_X4:
                sv_ಬಹುಲಕ = pass ? RISCV_IOMMU_CAP_SV32X4 : RISCV_IOMMU_CAP_SV32;
                ಆದರೆ (!(s->cap & sv_ಬಹುಲಕ)) {
                    ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].ಮಟ್ಟಗಳು    = 2;
                sc[pass].ptidxbits = 10;
                sc[pass].ptesize   = 4;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* 64bit mode for GXL/SXL == 0 */
            ಆಯ್ಕೆ (pass ? gatp : satp) {
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE:
                sc[pass].ಮಟ್ಟಗಳು    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV39_X4:
                sv_ಬಹುಲಕ = pass ? RISCV_IOMMU_CAP_SV39X4 : RISCV_IOMMU_CAP_SV39;
                ಆದರೆ (!(s->cap & sv_ಬಹುಲಕ)) {
                    ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].ಮಟ್ಟಗಳು    = 3;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV48_X4:
                sv_ಬಹುಲಕ = pass ? RISCV_IOMMU_CAP_SV48X4 : RISCV_IOMMU_CAP_SV48;
                ಆದರೆ (!(s->cap & sv_ಬಹುಲಕ)) {
                    ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].ಮಟ್ಟಗಳು    = 4;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV57_X4:
                sv_ಬಹುಲಕ = pass ? RISCV_IOMMU_CAP_SV57X4 : RISCV_IOMMU_CAP_SV57;
                ಆದರೆ (!(s->cap & sv_ಬಹುಲಕ)) {
                    ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].ಮಟ್ಟಗಳು    = 5;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        }
    };

    /* S/G stages translation tables root pointers */
    gatp = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_ATP_PPN_ಕ್ಷೇತ್ರ));
    satp = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ctx->satp, RISCV_IOMMU_ATP_PPN_ಕ್ಷೇತ್ರ));
    addr = (en_s && en_g) ? satp : iotlb->iova;
    ತಳ = en_g ? gatp : satp;
    pass = en_g ? G_STAGE : S_STAGE;

    ಮಾಡು {
        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ widened = (pass && !sc[pass].ಹೆಜ್ಜೆ) ? 2 : 0;
        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ va_ಬಿಟ್ಟುಗಳು = widened + sc[pass].ptidxbits;
        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ va_skip = ಗುರಿ_ಪುಟ_ಬಿಟ್ಟುಗಳು + sc[pass].ptidxbits *
                                 (sc[pass].ಮಟ್ಟಗಳು - 1 - sc[pass].ಹೆಜ್ಜೆ);
        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ idx = (addr >> va_skip) & ((1 << va_ಬಿಟ್ಟುಗಳು) - 1);
        ಸ್ಥಿರಾಂಕ dma_addr_t pte_addr = ತಳ + idx * sc[pass].ptesize;
        ಸ್ಥಿರಾಂಕ bool ade =
            ctx->tc & (pass ? RISCV_IOMMU_DC_TC_GADE : RISCV_IOMMU_DC_TC_SADE);

        ಆದರೆ (ade && !(s->cap & RISCV_IOMMU_CAP_AMO_HWAD)) {
            /* GADE/SADE are reserved bits if AMO_HWAD is cleared.  */
            ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }

        /* Address range check before first level lookup */
        ಆದರೆ (!sc[pass].ಹೆಜ್ಜೆ) {
            ಸ್ಥಿರಾಂಕ uint64_t va_len = va_skip + va_ಬಿಟ್ಟುಗಳು;
            ಸ್ಥಿರಾಂಕ uint64_t va_ಮುಖವಾಡ = (1ULL << va_len) - 1;

            ಆದರೆ (pass == S_STAGE && va_len > 32) {
                uint64_t ಮುಖವಾಡ, masked_msbs;

                ಮುಖವಾಡ = MAKE_64_ಬಿಟ್_ಮುಖವಾಡ(0, ಗುರಿ_long_ಬಿಟ್ಟುಗಳು() - va_len + 1);
                masked_msbs = (addr >> (va_len - 1)) & ಮುಖವಾಡ;

                ಆದರೆ (masked_msbs != 0 && masked_msbs != ಮುಖವಾಡ) {
                    ಹಿಂತಿರುಗಿ (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ ((addr & va_ಮುಖವಾಡ) != addr) {
                    ಹಿಂತಿರುಗಿ (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS;
                }
            }
        }


        ಆದರೆ (pass == S_STAGE) {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_S_VS_WALKS);
        } ಇಲ್ಲದಿದ್ದರೆ {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_G_WALKS);
        }

        /* Read page table entry */
        ಆದರೆ (sc[pass].ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->ಗುರಿ_as, pte_addr, &pte32,
                             MEMTXATTRS_UNSPECIFIED);
            pte = pte32;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ret = ldq_le_dma(s->ಗುರಿ_as, pte_addr, &pte,
                             MEMTXATTRS_UNSPECIFIED);
        }
        ಆದರೆ (ret != MEMTX_OK) {
            ಹಿಂತಿರುಗಿ (req_perm & IOMMU_WO) ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT
                                         : RISCV_IOMMU_FQ_CAUSE_RD_FAULT;
        }

        sc[pass].ಹೆಜ್ಜೆ++;
        hwaddr ppn = pte >> PTE_PPN_ಸರಿಸು;

        ಆದರೆ (!(pte & PTE_V)) {
            ನಿಲ್ಲಿಸು;                /* Invalid PTE */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pte & PTE_RESERVED(false)) {
            ನಿಲ್ಲಿಸು;                /* Reserved PTE bits set */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(pte & (PTE_R | PTE_W | PTE_X))) {
            ತಳ = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            ನಿಲ್ಲಿಸು;                /* Reserved leaf PTE flags: PTE_W */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            ನಿಲ್ಲಿಸು;                /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ppn & ((1ULL << (va_skip - ಗುರಿ_ಪುಟ_ಬಿಟ್ಟುಗಳು)) - 1)) {
            ನಿಲ್ಲಿಸು;                /* Misaligned PPN */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(pte & PTE_U) && !pv) {
            /*
             * All accesses are assumed to be User mode unless
             * process_id is valid (pv).  In case we have a
             * non-user mode leaf PTE and !pv we need to fault.
             */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((req_perm & IOMMU_RO) && !(pte & PTE_R)) {
            ನಿಲ್ಲಿಸು;                /* Read access check failed */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((req_perm & IOMMU_WO) && !(pte & PTE_W)) {
            ನಿಲ್ಲಿಸು;                /* Write access check failed */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ade && !(pte & PTE_A)) {
            ನಿಲ್ಲಿಸು;                /* Access bit not set */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((req_perm & IOMMU_WO) && !ade && !(pte & PTE_D)) {
            ನಿಲ್ಲಿಸು;                /* Dirty bit not set */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pass == G_STAGE && !(pte & PTE_U)) {
            /*
             * riscv-iommu spec 1.0: "When checking the U bit in a
             * second-stage PTE, the transaction is treated as
             * not requesting supervisor privilege."
             *
             * I.e. we need to fault if this is a non-user PTE since
             * we are always in user mode at this point.
             */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* Leaf PTE, translation completed. */
            sc[pass].ಹೆಜ್ಜೆ = sc[pass].ಮಟ್ಟಗಳು;

            ಆದರೆ (pte & PTE_N) {
                hwaddr ಮುಖವಾಡ = riscv_iommu_napot_ಪುಟ_ಮುಖವಾಡ(ppn, addr, &ತಳ);

                ಆದರೆ (!ಮುಖವಾಡ) {
                    ನಿಲ್ಲಿಸು;
                }
                iotlb->addr_ಮುಖವಾಡ &= ಮುಖವಾಡ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ತಳ = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
                /* Update address mask based on smallest translation granularity */
                iotlb->addr_ಮುಖವಾಡ &= (1ULL << va_skip) - 1;
            }

            /* Continue with S-Stage translation? */
            ಆದರೆ (pass && sc[0].ಹೆಜ್ಜೆ != sc[0].ಮಟ್ಟಗಳು) {
                pass = S_STAGE;
                addr = iotlb->iova;
                ಮುಂದುವರಿ;
            }

            /* Cache the effective permission, not this request's subset. */
            IOMMUAccessFlags ಎಲೆ_perm = (pte & PTE_W) ?
                                         ((pte & PTE_R) ? IOMMU_RW : IOMMU_WO) :
                                         IOMMU_RO;

            trans_perm = trans_perm == IOMMU_NONE ?
                         ಎಲೆ_perm : trans_perm & ಎಲೆ_perm;

            /* Translation phase completed (GPA or SPA) */
            iotlb->translated_addr = ತಳ;

            /* Check MSI GPA address match */
            ಆದರೆ (pass == S_STAGE && (req_perm & IOMMU_WO) &&
                riscv_iommu_msi_check(s, ctx, ತಳ)) {
                /* Trap MSI writes and return GPA address. */
                iotlb->ಗುರಿ_as = &s->trap_as;
                iotlb->addr_ಮುಖವಾಡ = ~ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;
                ಹಿಂತಿರುಗಿ 0;
            }

            /* Continue with G-Stage translation? */
            ಆದರೆ (!pass && en_g) {
                pass = G_STAGE;
                addr = ತಳ;
                ತಳ = gatp;
                sc[pass].ಹೆಜ್ಜೆ = 0;
                ಮುಂದುವರಿ;
            }

            iotlb->perm = trans_perm;
            ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (sc[pass].ಹೆಜ್ಜೆ == sc[pass].ಮಟ್ಟಗಳು) {
            ನಿಲ್ಲಿಸು; /* Can't find leaf PTE */
        }

        /* Continue with G-Stage translation? */
        ಆದರೆ (!pass && en_g) {
            pass = G_STAGE;
            addr = ತಳ;
            ತಳ = gatp;
            sc[pass].ಹೆಜ್ಜೆ = 0;
        }
    } ಆಗ (1);

    /*
     * riscv_iommu_translate() will receive a fault and then call
     * riscv_iommu_report_fault() using iotlb->translated_addr
     * as iotval2.  Update translated_addr it with the latest
     * translated addr we have.
     */
    iotlb->translated_addr = addr;

    ಹಿಂತಿರುಗಿ (req_perm & IOMMU_WO) ?
                (pass ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S) :
                (pass ? RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_report_fault(RISCVIOMMUState *s,
                                     RISCVIOMMUContext *ctx,
                                     uint32_t fault_type, uint32_t cause,
                                     bool pv,
                                     uint64_t iotval, uint64_t iotval2)
{
    ರಚನೆ riscv_iommu_fq_ದಾಖಲೆ ev = { 0 };

    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_DTF) {
        ಆಯ್ಕೆ (cause) {
        ಸ್ಥಿತಿ RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED:
        ಸ್ಥಿತಿ RISCV_IOMMU_FQ_CAUSE_DDT_ತುಂಬು_FAULT:
        ಸ್ಥಿತಿ RISCV_IOMMU_FQ_CAUSE_DDT_INVALID:
        ಸ್ಥಿತಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED:
        ಸ್ಥಿತಿ RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED:
        ಸ್ಥಿತಿ RISCV_IOMMU_FQ_CAUSE_ಆಂತರಿಕ_DP_ERROR:
        ಸ್ಥಿತಿ RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT:
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* DTF prevents reporting a fault for this given cause */
            ಹಿಂತಿರುಗಿ;
        }
    }

    ev.hdr = ಗಣ_ಕ್ಷೇತ್ರ(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE, cause);
    ev.hdr = ಗಣ_ಕ್ಷೇತ್ರ(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE, fault_type);
    ev.hdr = ಗಣ_ಕ್ಷೇತ್ರ(ev.hdr, RISCV_IOMMU_FQ_HDR_DID, ctx->devid);
    ev.hdr = ಗಣ_ಕ್ಷೇತ್ರ(ev.hdr, RISCV_IOMMU_FQ_HDR_PV, pv);

    ಆದರೆ (pv) {
        ev.hdr = ಗಣ_ಕ್ಷೇತ್ರ(ev.hdr, RISCV_IOMMU_FQ_HDR_PID, ctx->process_id);
    }

    ev.iotval = iotval;
    ev.iotval2 = iotval2;

    riscv_iommu_fault(s, &ev);
}

/* Redirect MSI write for given GPA. */
ಸ್ಥಿರ Mem_Tx_ಫಲಿತಾಂಶ riscv_iommu_msi_write(RISCVIOMMUState *s,
    RISCVIOMMUContext *ctx, uint64_t gpa, uint64_t ದತ್ತಾಂಶ,
    ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2, MemTxAttrs attrs)
{
    Mem_Tx_ಫಲಿತಾಂಶ res;
    dma_addr_t addr;
    uint64_t intn;
    ಗಾತ್ರ_t ಸ್ಥಾನಾಂತರ;
    uint32_t n190;
    uint64_t pte[2];
    ಪೂರ್ಣಾಂಕ fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    ಪೂರ್ಣಾಂಕ cause;

    /* Interrupt File Number */
    intn = riscv_iommu_pext_u64(PPN_ಕೆಳಕ್ಕೆ(gpa), ctx->msi_addr_ಮುಖವಾಡ);
    ಸ್ಥಾನಾಂತರ = intn * ಗಾತ್ರ(pte);

    /* fetch MSI PTE */
    addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_PPN));
    ಆದರೆ (addr & ಸ್ಥಾನಾಂತರ) {
        /* Interrupt file number out of range */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ತುಂಬು_FAULT;
        ಹೋಗು err;
    }

    addr |= ಸ್ಥಾನಾಂತರ;
    res = dma_ಸ್ಮರಣೆ_read(s->ಗುರಿ_as, addr, &pte, ಗಾತ್ರ(pte),
            MEMTXATTRS_UNSPECIFIED);
    ಆದರೆ (res != MEMTX_OK) {
        ಆದರೆ (res == MEMTX_DECODE_ERROR) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED;
        } ಇಲ್ಲದಿದ್ದರೆ {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_ತುಂಬು_FAULT;
        }
        ಹೋಗು err;
    }

    le64_ಗಮ್ಯ_cpus(&pte[0]);
    le64_ಗಮ್ಯ_cpus(&pte[1]);

    ಆದರೆ (!(pte[0] & RISCV_IOMMU_MSI_PTE_V) || (pte[0] & RISCV_IOMMU_MSI_PTE_C)) {
        /*
         * The spec mentions that: "If msipte.C == 1, then further
         * processing to interpret the PTE is implementation
         * defined.". We'll abort with cause = 262 for this
         * case too.
         */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_INVALID;
        ಹೋಗು err;
    }

    ಆಯ್ಕೆ (get_ಕ್ಷೇತ್ರ(pte[0], RISCV_IOMMU_MSI_PTE_M)) {
    ಸ್ಥಿತಿ RISCV_IOMMU_MSI_PTE_M_BASIC:
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
        ಆದರೆ (pte[0] & (GENMASK_ULL(62, 54) | GENMASK_ULL(9, 3))) {
            res = MEMTX_DECODE_ERROR;
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
            ಹೋಗು err;
        }

        /* MSI Pass-through mode */
        addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(pte[0], RISCV_IOMMU_MSI_PTE_PPN));

        trace_riscv_iommu_msi(s->ಪೋಷಕ_obj.id, PCI_BUS_ಅಂಕೆ(ctx->devid),
                              PCI_ಸ್ಥಾನ(ctx->devid), PCI_FUNC(ctx->devid),
                              gpa, addr);

        res = dma_ಸ್ಮರಣೆ_write(s->ಗುರಿ_as, addr, &ದತ್ತಾಂಶ, ಗಾತ್ರ_2, attrs);
        ಆದರೆ (res != MEMTX_OK) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
            ಹೋಗು err;
        }

        ಹಿಂತಿರುಗಿ MEMTX_OK;
    ಸ್ಥಿತಿ RISCV_IOMMU_MSI_PTE_M_MRIF:
        /* MRIF mode, continue. */
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        ಹೋಗು err;
    }

    /*
     * Report an error for interrupt identities exceeding the maximum allowed
     * for an IMSIC interrupt file (2047) or destination address is not 32-bit
     * aligned. See IOMMU Specification, Chapter 2.3. MSI page tables.
     */
    ಆದರೆ ((ದತ್ತಾಂಶ > 2047) || (gpa & 3)) {
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        ಹೋಗು err;
    }

    /* MSI MRIF mode, non atomic pending bit update */

    /* MRIF pending bit address */
    addr = get_ಕ್ಷೇತ್ರ(pte[0], RISCV_IOMMU_MSI_PTE_MRIF_ADDR) << 9;
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
    addr = addr | ((ದತ್ತಾಂಶ & 0x7c0) >> 2);

    trace_riscv_iommu_msi(s->ಪೋಷಕ_obj.id, PCI_BUS_ಅಂಕೆ(ctx->devid),
                          PCI_ಸ್ಥಾನ(ctx->devid), PCI_FUNC(ctx->devid),
                          gpa, addr);

    /* MRIF pending bit mask */
    ದತ್ತಾಂಶ = 1ULL << (ದತ್ತಾಂಶ & 0x03f);
    res = dma_ಸ್ಮರಣೆ_read(s->ಗುರಿ_as, addr, &intn, ಗಾತ್ರ(intn), attrs);
    ಆದರೆ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ತುಂಬು_FAULT;
        ಹೋಗು err;
    }

    intn = intn | ದತ್ತಾಂಶ;
    res = dma_ಸ್ಮರಣೆ_write(s->ಗುರಿ_as, addr, &intn, ಗಾತ್ರ(intn), attrs);
    ಆದರೆ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        ಹೋಗು err;
    }

    /* Get MRIF enable bits */
    addr = addr + ಗಾತ್ರ(intn);
    res = dma_ಸ್ಮರಣೆ_read(s->ಗುರಿ_as, addr, &intn, ಗಾತ್ರ(intn), attrs);
    ಆದರೆ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_ತುಂಬು_FAULT;
        ಹೋಗು err;
    }

    ಆದರೆ (!(intn & ದತ್ತಾಂಶ)) {
        /* notification disabled, MRIF update completed. */
        ಹಿಂತಿರುಗಿ MEMTX_OK;
    }

    /* Send notification message */
    addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(pte[1], RISCV_IOMMU_MSI_MRIF_NPPN));
    n190 = get_ಕ್ಷೇತ್ರ(pte[1], RISCV_IOMMU_MSI_MRIF_NID) |
          (get_ಕ್ಷೇತ್ರ(pte[1], RISCV_IOMMU_MSI_MRIF_NID_MSB) << 10);

    res = dma_ಸ್ಮರಣೆ_write(s->ಗುರಿ_as, addr, &n190, ಗಾತ್ರ(n190), attrs);
    ಆದರೆ (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        ಹೋಗು err;
    }

    trace_riscv_iommu_mrif_notification(s->ಪೋಷಕ_obj.id, n190, addr);

    ಹಿಂತಿರುಗಿ MEMTX_OK;

err:
    riscv_iommu_report_fault(s, ctx, fault_type, cause,
                             !!ctx->process_id, 0, 0);
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Check device context configuration as described by the
 * riscv-iommu spec section "Device-context configuration
 * checks".
 */
ಸ್ಥಿರ bool riscv_iommu_validate_device_ctx(RISCVIOMMUState *s,
                                            RISCVIOMMUContext *ctx)
{
    uint32_t fsc_ಬಹುಲಕ, msi_ಬಹುಲಕ;
    uint64_t gatp;

    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_RESERVED) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI ||
         ctx->tc & RISCV_IOMMU_DC_TC_PRPR)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI) &&
        ctx->tc & RISCV_IOMMU_DC_TC_PRPR) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_T2GPA) &&
        ctx->tc & RISCV_IOMMU_DC_TC_T2GPA) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (s->cap & RISCV_IOMMU_CAP_MSI_FLAT) {
        msi_ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_ಬಹುಲಕ);

        ಆದರೆ (msi_ಬಹುಲಕ != RISCV_IOMMU_DC_MSIPTP_ಬಹುಲಕ_OFF &&
            msi_ಬಹುಲಕ != RISCV_IOMMU_DC_MSIPTP_ಬಹುಲಕ_FLAT) {
            ಹಿಂತಿರುಗಿ false;
        }
    }

    gatp = get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ);
    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA &&
        gatp == RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (gatp != RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE) {
        uint64_t iohgatp_ppn = get_ಕ್ಷೇತ್ರ(ctx->gatp,
                                         RISCV_IOMMU_DC_IOHGATP_PPN);

        /*
         * One of the conditions for a misconfigured DDT entry
         * according to the riscv-spec: "DC.iohgatp.MODE is not
         * Bare and the root page table (address) determined by
         * DC.iohgatp.PPN is not aligned to a 16-KiB boundary."
         */
        ಆದರೆ (PPN_PHYS(iohgatp_ppn) & ((1ULL << 14) - 1)) {
            ಹಿಂತಿರುಗಿ false;
        }
    }

    fsc_ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(ctx->satp, RISCV_IOMMU_DC_FSC_ಬಹುಲಕ);

    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_PDTV) {
        ಆಯ್ಕೆ (fsc_ಬಹುಲಕ) {
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_PDTP_ಬಹುಲಕ_PD8:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_PD8)) {
                ಹಿಂತಿರುಗಿ false;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_PDTP_ಬಹುಲಕ_PD17:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_PD17)) {
                ಹಿಂತಿರುಗಿ false;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_PDTP_ಬಹುಲಕ_PD20:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_PD20)) {
                ಹಿಂತಿರುಗಿ false;
            }
            ನಿಲ್ಲಿಸು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* DC.tc.PDTV is 0 */
        ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_DPE) {
            ಹಿಂತಿರುಗಿ false;
        }

        ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
            ಆದರೆ (fsc_ಬಹುಲಕ == RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV32 &&
                !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                ಹಿಂತಿರುಗಿ false;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆಯ್ಕೆ (fsc_ಬಹುಲಕ) {
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV39:
                ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                    ಹಿಂತಿರುಗಿ false;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV48:
                ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                    ಹಿಂತಿರುಗಿ false;
                }
            ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV57:
                ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                    ಹಿಂತಿರುಗಿ false;
                }
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    /*
     * CAP_END is always zero (only one endianess). FCTL_BE is
     * always zero (little-endian accesses). Thus TC_SBE must
     * always be LE, i.e. zero.
     */
    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_SBE) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಹಿಂತಿರುಗಿ true;
}

/*
 * Validate process context (PC) according to section
 * "Process-context configuration checks".
 */
ಸ್ಥಿರ bool riscv_iommu_validate_process_ctx(RISCVIOMMUState *s,
                                             RISCVIOMMUContext *ctx)
{
    uint32_t ಬಹುಲಕ;

    ಆದರೆ (get_ಕ್ಷೇತ್ರ(ctx->ta, RISCV_IOMMU_PC_TA_RESERVED)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (get_ಕ್ಷೇತ್ರ(ctx->satp, RISCV_IOMMU_PC_FSC_RESERVED)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(ctx->satp, RISCV_IOMMU_DC_FSC_ಬಹುಲಕ);
    ಆಯ್ಕೆ (ಬಹುಲಕ) {
    ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_ಬಹುಲಕ_BARE:
    /* sv39 and sv32 modes have the same value (8) */
    ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV39:
    ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV48:
    ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV57:
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
        ಆದರೆ (ಬಹುಲಕ == RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV32 &&
            !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                ಹಿಂತಿರುಗಿ false;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಯ್ಕೆ (ಬಹುಲಕ) {
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV39:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                ಹಿಂತಿರುಗಿ false;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV48:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                ಹಿಂತಿರುಗಿ false;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV57:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                ಹಿಂತಿರುಗಿ false;
            }
            ನಿಲ್ಲಿಸು;
        }
    }

    ಹಿಂತಿರುಗಿ true;
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
ಸ್ಥಿರ Mem_Tx_ಫಲಿತಾಂಶ pdt_ಸ್ಮರಣೆ_read(RISCVIOMMUState *s,
                                   RISCVIOMMUContext *ctx,
                                   dma_addr_t addr,
                                   ಶೂನ್ಯ *buf, dma_addr_t len,
                                   MemTxAttrs attrs)
{
    uint64_t gatp_ಬಹುಲಕ, pte;
    ರಚನೆ {
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಹೆಜ್ಜೆ;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಮಟ್ಟಗಳು;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ptidxbits;
        ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ptesize;
    } sc;
    Mem_Tx_ಫಲಿತಾಂಶ ret;
    dma_addr_t ತಳ = addr;

    /* G stages translation mode */
    gatp_ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ);
    ಆದರೆ (gatp_ಬಹುಲಕ == RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE) {
        ಹೋಗು ಹೊರಹೋಗು;
    }

    /* G stages translation tables root pointer */
    ತಳ = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_ATP_PPN_ಕ್ಷೇತ್ರ));

    /* Start at step 0 */
    sc.ಹೆಜ್ಜೆ = 0;

    ಆದರೆ (s->fctl & RISCV_IOMMU_FCTL_GXL) {
        /* 32bit mode for GXL == 1 */
        ಆಯ್ಕೆ (gatp_ಬಹುಲಕ) {
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV32_X4:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV32X4)) {
                ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
            }
            sc.ಮಟ್ಟಗಳು    = 2;
            sc.ptidxbits = 10;
            sc.ptesize   = 4;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* 64bit mode for GXL == 0 */
        ಆಯ್ಕೆ (gatp_ಬಹುಲಕ) {
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV39_X4:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV39X4)) {
                ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
            }
            sc.ಮಟ್ಟಗಳು    = 3;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV48_X4:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV48X4)) {
                ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
            }
            sc.ಮಟ್ಟಗಳು    = 4;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV57_X4:
            ಆದರೆ (!(s->cap & RISCV_IOMMU_CAP_SV57X4)) {
                ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
            }
            sc.ಮಟ್ಟಗಳು    = 5;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
        }
    }

    ಮಾಡು {
        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ va_ಬಿಟ್ಟುಗಳು = (sc.ಹೆಜ್ಜೆ ? 0 : 2) + sc.ptidxbits;
        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ va_skip = ಗುರಿ_ಪುಟ_ಬಿಟ್ಟುಗಳು + sc.ptidxbits *
                                 (sc.ಮಟ್ಟಗಳು - 1 - sc.ಹೆಜ್ಜೆ);
        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ idx = (addr >> va_skip) & ((1 << va_ಬಿಟ್ಟುಗಳು) - 1);
        ಸ್ಥಿರಾಂಕ dma_addr_t pte_addr = ತಳ + idx * sc.ptesize;

        /* Address range check before first level lookup */
        ಆದರೆ (!sc.ಹೆಜ್ಜೆ) {
            ಸ್ಥಿರಾಂಕ uint64_t va_ಮುಖವಾಡ = (1ULL << (va_skip + va_ಬಿಟ್ಟುಗಳು)) - 1;
            ಆದರೆ ((addr & va_ಮುಖವಾಡ) != addr) {
                ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
            }
        }

        /* Read page table entry */
        ಆದರೆ (sc.ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->ಗುರಿ_as, pte_addr, &pte32, attrs);
            pte = pte32;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ret = ldq_le_dma(s->ಗುರಿ_as, pte_addr, &pte, attrs);
        }
        ಆದರೆ (ret != MEMTX_OK) {
            ಹಿಂತಿರುಗಿ ret;
        }

        sc.ಹೆಜ್ಜೆ++;
        hwaddr ppn = pte >> PTE_PPN_ಸರಿಸು;

        ಆದರೆ (!(pte & PTE_V)) {
            ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR; /* Invalid PTE */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(pte & (PTE_R | PTE_W | PTE_X))) {
            ತಳ = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ppn & ((1ULL << (va_skip - ಗುರಿ_ಪುಟ_ಬಿಟ್ಟುಗಳು)) - 1)) {
            ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR; /* Misaligned PPN */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* Leaf PTE, translation completed. */
            ಆದರೆ (pte & PTE_N) {
                ಆದರೆ (!riscv_iommu_napot_ಪುಟ_ಮುಖವಾಡ(ppn, addr, &ತಳ)) {
                    ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ತಳ = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
            }
            ನಿಲ್ಲಿಸು;
        }

        ಆದರೆ (sc.ಹೆಜ್ಜೆ == sc.ಮಟ್ಟಗಳು) {
            ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR; /* Can't find leaf PTE */
        }
    } ಆಗ (1);

ಹೊರಹೋಗು:
    ಹಿಂತಿರುಗಿ dma_ಸ್ಮರಣೆ_read(s->ಗುರಿ_as, ತಳ, buf, len, attrs);
}

/*
 * RISC-V IOMMU Device Context Loopkup - Device Directory Tree Walk
 *
 * @s         : IOMMU Device State
 * @ctx       : Device Translation Context with devid and process_id set.
 * @return    : success or fault code.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ riscv_iommu_ctx_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx)
{
    ಸ್ಥಿರಾಂಕ uint64_t ddtp = s->ddtp;
    ಚಿಹ್ನೆರಹಿತ ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(ddtp, RISCV_IOMMU_DDTP_ಬಹುಲಕ);
    dma_addr_t addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ddtp, RISCV_IOMMU_DDTP_PPN));
    ರಚನೆ riscv_iommu_dc dc;
    /* Device Context format: 0: extended (64 bytes) | 1: base (32 bytes) */
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ dc_fmt = !s->enable_msi;
    ಸ್ಥಿರಾಂಕ ಗಾತ್ರ_t dc_len = ಗಾತ್ರ(dc) >> dc_fmt;
    ಪೂರ್ಣಾಂಕ ಆಳ;
    uint64_t de;

    ಆಯ್ಕೆ (ಬಹುಲಕ) {
    ಸ್ಥಿತಿ RISCV_IOMMU_DDTP_ಬಹುಲಕ_OFF:
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED;

    ಸ್ಥಿತಿ RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE:
        /* mock up pass-through translation context */
        ctx->gatp = ಗಣ_ಕ್ಷೇತ್ರ(0, RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ,
            RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE);
        ctx->satp = ಗಣ_ಕ್ಷೇತ್ರ(0, RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ,
            RISCV_IOMMU_DC_FSC_ಬಹುಲಕ_BARE);

        ctx->tc = RISCV_IOMMU_DC_TC_V;
        ಆದರೆ (s->enable_ats) {
            ctx->tc |= RISCV_IOMMU_DC_TC_EN_ATS;
        }

        ctx->ta = 0;
        ctx->msiptp = 0;
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ RISCV_IOMMU_DDTP_ಬಹುಲಕ_1_LVL:
        ಆಳ = 0;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_DDTP_ಬಹುಲಕ_2_LVL:
        ಆಳ = 1;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_DDTP_ಬಹುಲಕ_3_LVL:
        ಆಳ = 2;
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /*
     * Check supported device id width (in bits).
     * See IOMMU Specification, Chapter 6. Software guidelines.
     * - if extended device-context format is used:
     *   1LVL: 6, 2LVL: 15, 3LVL: 24
     * - if base device-context format is used:
     *   1LVL: 7, 2LVL: 16, 3LVL: 24
     */
    ಆದರೆ (ctx->devid >= (1 << (ಆಳ * 9 + 6 + (dc_fmt && ಆಳ != 2)))) {
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
    }

    /* Device directory tree walk */
    ಗಾಗಿ (; ಆಳ-- > 0; ) {
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
        ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ವಿಭಜಿಸು = ಆಳ * 9 + 6 + dc_fmt;
        addr |= ((ctx->devid >> ವಿಭಜಿಸು) << 3) & ~ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;
        ಆದರೆ (dma_ಸ್ಮರಣೆ_read(s->ಗುರಿ_as, addr, &de, ಗಾತ್ರ(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_ತುಂಬು_FAULT;
        }
        le64_ಗಮ್ಯ_cpus(&de);
        ಆದರೆ (!(de & RISCV_IOMMU_DDTE_VALID)) {
            /* invalid directory entry */
            ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        ಆದರೆ (de & ~(RISCV_IOMMU_DDTE_PPN | RISCV_IOMMU_DDTE_VALID)) {
            /* reserved bits set */
            ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }
        addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(de, RISCV_IOMMU_DDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);

    /* index into device context entry page */
    addr |= (ctx->devid * dc_len) & ~ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;

    memset(&dc, 0, ಗಾತ್ರ(dc));
    ಆದರೆ (dma_ಸ್ಮರಣೆ_read(s->ಗುರಿ_as, addr, &dc, dc_len,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_ತುಂಬು_FAULT;
    }

    /* Set translation context. */
    ctx->tc = le64_ಗಮ್ಯ_cpu(dc.tc);
    ctx->gatp = le64_ಗಮ್ಯ_cpu(dc.iohgatp);
    ctx->satp = le64_ಗಮ್ಯ_cpu(dc.fsc);
    ctx->ta = le64_ಗಮ್ಯ_cpu(dc.ta);
    ctx->msiptp = le64_ಗಮ್ಯ_cpu(dc.msiptp);
    ctx->msi_addr_ಮುಖವಾಡ = le64_ಗಮ್ಯ_cpu(dc.msi_addr_ಮುಖವಾಡ);
    ctx->msi_addr_ನಮೂನೆ = le64_ಗಮ್ಯ_cpu(dc.msi_addr_ನಮೂನೆ);

    ಆದರೆ (!(ctx->tc & RISCV_IOMMU_DC_TC_V)) {
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
    }

    ಆದರೆ (!riscv_iommu_validate_device_ctx(s, ctx)) {
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /* FSC field checks */
    ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(ctx->satp, RISCV_IOMMU_DC_FSC_ಬಹುಲಕ);
    addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ctx->satp, RISCV_IOMMU_DC_FSC_PPN));

    ಆದರೆ (!(ctx->tc & RISCV_IOMMU_DC_TC_PDTV)) {
        ಆದರೆ (ctx->process_id != RISCV_IOMMU_NOPROCID) {
            /* PID is disabled */
            ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
        }
        ಆದರೆ (ಬಹುಲಕ > RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV57) {
            /* Invalid translation mode */
            ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (ctx->process_id == RISCV_IOMMU_NOPROCID) {
        ಆದರೆ (!(ctx->tc & RISCV_IOMMU_DC_TC_DPE)) {
            /* No default process_id enabled, set BARE mode */
            ctx->satp = 0ULL;
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* Use default process_id #0 */
            ctx->process_id = 0;
        }
    }

    ಆದರೆ (ಬಹುಲಕ == RISCV_IOMMU_DC_FSC_ಬಹುಲಕ_BARE) {
        /* No S-Stage translation, done. */
        ಹಿಂತಿರುಗಿ 0;
    }

    /* FSC.TC.PDTV enabled */
    ಆದರೆ (ಬಹುಲಕ > RISCV_IOMMU_DC_FSC_PDTP_ಬಹುಲಕ_PD20) {
        /* Invalid PDTP.MODE */
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    ಗಾಗಿ (ಆಳ = ಬಹುಲಕ - RISCV_IOMMU_DC_FSC_PDTP_ಬಹುಲಕ_PD8; ಆಳ-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

        /*
         * Select process id index bits based on process directory tree
         * level. See IOMMU Specification, 2.2. Process-Directory-Table.
         */
        ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ವಿಭಜಿಸು = ಆಳ * 9 + 8;
        addr |= ((ctx->process_id >> ವಿಭಜಿಸು) << 3) & ~ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;
        ಆದರೆ (pdt_ಸ್ಮರಣೆ_read(s, ctx, addr, &de, ಗಾತ್ರ(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_PDT_ತುಂಬು_FAULT;
        }
        le64_ಗಮ್ಯ_cpus(&de);
        ಆದರೆ (!(de & RISCV_IOMMU_PDTE_VALID)) {
            ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
        }
        addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(de, RISCV_IOMMU_PDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

    /* Leaf entry in PDT */
    addr |= (ctx->process_id << 4) & ~ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;
    ಆದರೆ (pdt_ಸ್ಮರಣೆ_read(s, ctx, addr, &dc.ta, ಗಾತ್ರ(uint64_t) * 2,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_PDT_ತುಂಬು_FAULT;
    }

    /* Use FSC and TA from process directory entry. */
    ctx->ta = le64_ಗಮ್ಯ_cpu(dc.ta);
    ctx->satp = le64_ಗಮ್ಯ_cpu(dc.fsc);

    ಆದರೆ (!(ctx->ta & RISCV_IOMMU_PC_TA_V)) {
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
    }

    ಆದರೆ (!riscv_iommu_validate_process_ctx(s, ctx)) {
        ಹಿಂತಿರುಗಿ RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    ಹಿಂತಿರುಗಿ 0;
}

/* Translation Context cache support */
ಸ್ಥಿರ gboolean riscv_iommu_ctx_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUContext *c1 = (RISCVIOMMUContext *) v1;
    RISCVIOMMUContext *c2 = (RISCVIOMMUContext *) v2;
    ಹಿಂತಿರುಗಿ c1->devid == c2->devid &&
           c1->process_id == c2->process_id;
}

ಸ್ಥಿರ guint riscv_iommu_ctx_hash(gconstpointer v)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) v;
    /*
     * Generate simple hash of (process_id, devid)
     * assuming 24-bit wide devid.
     */
    ಹಿಂತಿರುಗಿ (guint)(ctx->devid) + ((guint)(ctx->process_id) << 24);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ctx_inval_devid_procid(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ,
                                               gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) ಮೌಲ್ಯ;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) ದತ್ತಾಂಶ;
    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid &&
        ctx->process_id == arg->process_id) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ctx_inval_devid(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ,
                                        gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) ಮೌಲ್ಯ;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) ದತ್ತಾಂಶ;
    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ctx_inval_all(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ,
                                      gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) ಮೌಲ್ಯ;
    ಆದರೆ (ctx->tc & RISCV_IOMMU_DC_TC_V) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ctx_inval(RISCVIOMMUState *s, GHFunc func,
                                  uint32_t devid, uint32_t process_id)
{
    GHash_ಕೋಷ್ಟಕ *ctx_cache;
    RISCVIOMMUContext ಕೀಲಿ = {
        .devid = devid,
        .process_id = process_id,
    };
    ctx_cache = g_hash_ಕೋಷ್ಟಕ_ref(s->ctx_cache);
    g_hash_ಕೋಷ್ಟಕ_foreach(ctx_cache, func, &ಕೀಲಿ);
    g_hash_ಕೋಷ್ಟಕ_unref(ctx_cache);
}

/* Find or allocate translation context for a given {device_id, process_id} */
ಸ್ಥಿರ RISCVIOMMUContext *riscv_iommu_ctx(RISCVIOMMUState *s,
                                          ಚಿಹ್ನೆರಹಿತ devid, ಚಿಹ್ನೆರಹಿತ process_id,
                                          IOMMUAccessFlags perm, uint64_t iova,
                                          ಶೂನ್ಯ **ref)
{
    GHash_ಕೋಷ್ಟಕ *ctx_cache;
    RISCVIOMMUContext *ctx;
    RISCVIOMMUContext ಕೀಲಿ = {
        .devid = devid,
        .process_id = process_id,
    };
    ಚಿಹ್ನೆರಹಿತ ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(s->ddtp, RISCV_IOMMU_DDTP_ಬಹುಲಕ);
    uint32_t fault_type;

    ctx_cache = g_hash_ಕೋಷ್ಟಕ_ref(s->ctx_cache);

    ಆದರೆ (ಬಹುಲಕ != RISCV_IOMMU_DDTP_ಬಹುಲಕ_OFF &&
        ಬಹುಲಕ != RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE) {
        ctx = g_hash_ಕೋಷ್ಟಕ_lookup(ctx_cache, &ಕೀಲಿ);

        ಆದರೆ (ctx && (ctx->tc & RISCV_IOMMU_DC_TC_V)) {
            *ref = ctx_cache;
            ಹಿಂತಿರುಗಿ ctx;
        }
    }

    ctx = g_new0(RISCVIOMMUContext, 1);
    ctx->devid = devid;
    ctx->process_id = process_id;

    ಪೂರ್ಣಾಂಕ fault = riscv_iommu_ctx_fetch(s, ctx);
    ಆದರೆ (!fault) {
        ಆದರೆ (ಬಹುಲಕ != RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE) {
            ಆದರೆ (g_hash_ಕೋಷ್ಟಕ_ಗಾತ್ರ(ctx_cache) >= ಎಲ್ಲೆ_CACHE_CTX) {
                g_hash_ಕೋಷ್ಟಕ_unref(ctx_cache);
                ctx_cache = g_hash_ಕೋಷ್ಟಕ_new_ತುಂಬಿದ(riscv_iommu_ctx_hash,
                                                  riscv_iommu_ctx_equal,
                                                  g_free, ಖಾಲಿ);
                g_hash_ಕೋಷ್ಟಕ_ref(ctx_cache);
                g_hash_ಕೋಷ್ಟಕ_unref(qatomic_xchg(&s->ctx_cache, ctx_cache));
            }

            g_hash_ಕೋಷ್ಟಕ_ಕೂಡಿಸು(ctx_cache, ctx);
            *ref = ctx_cache;
        } ಇಲ್ಲದಿದ್ದರೆ {
            g_hash_ಕೋಷ್ಟಕ_unref(ctx_cache);
            /* Remember ctx so it can be freed */
            *ref = ctx;
        }
        ಹಿಂತಿರುಗಿ ctx;
    }

    g_hash_ಕೋಷ್ಟಕ_unref(ctx_cache);
    *ref = ಖಾಲಿ;

    /*
     * TODO: (1) do we need to distinguish other fault types
     * for ctx fetching and (2) evaluate putting the 'fault_type'
     * logic inside riscv_iommu_report_fault() - there's at
     * least one other place (end of riscv_iommu_translate())
     * that does something similar.
     */
    ಆದರೆ (perm & IOMMU_RO) {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
    } ಇಲ್ಲದಿದ್ದರೆ {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    }

    riscv_iommu_report_fault(s, ctx, fault_type, fault,
                             !!process_id, iova, 0);

    g_free(ctx);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ctx_put(RISCVIOMMUState *s, ಶೂನ್ಯ *ref)
{
    ಚಿಹ್ನೆರಹಿತ ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(s->ddtp, RISCV_IOMMU_DDTP_ಬಹುಲಕ);

    ಆದರೆ (!ref) {
        ಹಿಂತಿರುಗಿ;
    }

    /* ref is pointing to ctx in Bare mode. Bare mode ctx is not cached */
    ಆದರೆ (ಬಹುಲಕ == RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE) {
        g_free(ref);
    } ಇಲ್ಲದಿದ್ದರೆ {
        g_hash_ಕೋಷ್ಟಕ_unref((GHash_ಕೋಷ್ಟಕ *)ref);
    }
}

/* Find or allocate address space for a given device */
ಸ್ಥಿರ ವಿಳಾಸ_Space *riscv_iommu_space(RISCVIOMMUState *s, PCIBus *bus,
                                            ಪೂರ್ಣಾಂಕ devfn)
{
    RISCVIOMMUSpace *as;

    QLIST_FOREACH(as, &s->spaces, list) {
        ಆದರೆ (as->bus == bus && as->devfn == devfn) {
            ನಿಲ್ಲಿಸು;
        }
    }

    ಆದರೆ (as == ಖಾಲಿ) {
        ಅಕ್ಷರ name[64];
        as = g_new0(RISCVIOMMUSpace, 1);

        as->iommu = s;
        as->bus = bus;
        as->devfn = devfn;

        snprintf(name, ಗಾತ್ರ(name), "riscv-iommu-%04x:%02x.%d-iova",
            pci_bus_ಅಂಕೆ(bus), PCI_ಸ್ಥಾನ(devfn), PCI_FUNC(devfn));

        /* IOVA address space, untranslated addresses */
        ಸ್ಮರಣೆ_region_init_iommu(&as->iova_mr, ಗಾತ್ರ(as->iova_mr),
            TYPE_RISCV_IOMMU_ಸ್ಮರಣೆ_REGION,
            OBJECT(s), "riscv_iommu", UINT64_MAX);
        ವಿಳಾಸ_space_init(&as->iova_as, ಸ್ಮರಣೆ_REGION(&as->iova_mr), name);

        QLIST_ಸೇರಿಸು_ತಲೆ(&s->spaces, as, list);

        trace_riscv_iommu_new(s->ಪೋಷಕ_obj.id, pci_bus_ಅಂಕೆ(bus),
                PCI_ಸ್ಥಾನ(devfn), PCI_FUNC(devfn));
    }
    ಹಿಂತಿರುಗಿ &as->iova_as;
}

/* Translation Object cache support */
ಸ್ಥಿರ gboolean riscv_iommu_iot_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUEntry *t1 = (RISCVIOMMUEntry *) v1;
    RISCVIOMMUEntry *t2 = (RISCVIOMMUEntry *) v2;
    ಹಿಂತಿರುಗಿ t1->gscid == t2->gscid && t1->pscid == t2->pscid &&
           t1->iova == t2->iova && t1->ಟ್ಯಾಗ್ == t2->ಟ್ಯಾಗ್;
}

ಸ್ಥಿರ guint riscv_iommu_iot_hash(gconstpointer v)
{
    RISCVIOMMUEntry *t = (RISCVIOMMUEntry *) v;
    ಹಿಂತಿರುಗಿ (guint)t->iova;
}

/* GV: 0 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 0 AV: 0 GVMA: 1 */
ಸ್ಥಿರ
ಶೂನ್ಯ riscv_iommu_iot_inval_all(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ, gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ಮೌಲ್ಯ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ದತ್ತಾಂಶ;
    ಆದರೆ (iot->ಟ್ಯಾಗ್ == arg->ಟ್ಯಾಗ್) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 0 PSCV: 1 GVMA: 0 */
ಸ್ಥಿರ
ಶೂನ್ಯ riscv_iommu_iot_inval_pscid(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ, gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ಮೌಲ್ಯ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ದತ್ತಾಂಶ;
    ಆದರೆ (iot->ಟ್ಯಾಗ್ == arg->ಟ್ಯಾಗ್ &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 0 GVMA: 0 */
ಸ್ಥಿರ
ಶೂನ್ಯ riscv_iommu_iot_inval_iova(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ, gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ಮೌಲ್ಯ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ದತ್ತಾಂಶ;
    ಆದರೆ (iot->ಟ್ಯಾಗ್ == arg->ಟ್ಯಾಗ್ &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 1 GVMA: 0 */
ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_iot_inval_pscid_iova(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ,
                                             gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ಮೌಲ್ಯ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ದತ್ತಾಂಶ;
    ಆದರೆ (iot->ಟ್ಯಾಗ್ == arg->ಟ್ಯಾಗ್ &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 0 GVMA: 1 */
ಸ್ಥಿರ
ಶೂನ್ಯ riscv_iommu_iot_inval_gscid(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ, gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ಮೌಲ್ಯ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ದತ್ತಾಂಶ;
    ಆದರೆ (iot->ಟ್ಯಾಗ್ == arg->ಟ್ಯಾಗ್ &&
        iot->gscid == arg->gscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 1 GVMA: 0 */
ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_iot_inval_gscid_pscid(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ,
                                              gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ಮೌಲ್ಯ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ದತ್ತಾಂಶ;
    ಆದರೆ (iot->ಟ್ಯಾಗ್ == arg->ಟ್ಯಾಗ್ &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 1 GVMA: 1 */
ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_iot_inval_gscid_iova(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ,
                                             gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ಮೌಲ್ಯ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ದತ್ತಾಂಶ;
    ಆದರೆ (iot->ಟ್ಯಾಗ್ == arg->ಟ್ಯಾಗ್ &&
        iot->gscid == arg->gscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 1 GVMA: 0 */
ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_iot_inval_gscid_pscid_iova(gpointer ಕೀಲಿ, gpointer ಮೌಲ್ಯ,
                                                   gpointer ದತ್ತಾಂಶ)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) ಮೌಲ್ಯ;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) ದತ್ತಾಂಶ;
    ಆದರೆ (iot->ಟ್ಯಾಗ್ == arg->ಟ್ಯಾಗ್ &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* caller should keep ref-count for iot_cache object */
ಸ್ಥಿರ RISCVIOMMUEntry *riscv_iommu_iot_lookup(RISCVIOMMUContext *ctx,
    GHash_ಕೋಷ್ಟಕ *iot_cache, hwaddr iova, RISCVIOMMUTrans_ಟ್ಯಾಗ್ transtag)
{
    RISCVIOMMUEntry ಕೀಲಿ = {
        .ಟ್ಯಾಗ್   = transtag,
        .gscid = get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID),
        .pscid = get_ಕ್ಷೇತ್ರ(ctx->ta, RISCV_IOMMU_DC_TA_PSCID),
        .iova  = PPN_ಕೆಳಕ್ಕೆ(iova),
    };
    ಹಿಂತಿರುಗಿ g_hash_ಕೋಷ್ಟಕ_lookup(iot_cache, &ಕೀಲಿ);
}

/* caller should keep ref-count for iot_cache object */
ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_iot_update(RISCVIOMMUState *s,
    GHash_ಕೋಷ್ಟಕ *iot_cache, RISCVIOMMUEntry *iot)
{
    ಆದರೆ (!s->iot_ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (g_hash_ಕೋಷ್ಟಕ_ಗಾತ್ರ(s->iot_cache) >= s->iot_ಎಲ್ಲೆ) {
        iot_cache = g_hash_ಕೋಷ್ಟಕ_new_ತುಂಬಿದ(riscv_iommu_iot_hash,
                                          riscv_iommu_iot_equal,
                                          g_free, ಖಾಲಿ);
        g_hash_ಕೋಷ್ಟಕ_unref(qatomic_xchg(&s->iot_cache, iot_cache));
    }
    g_hash_ಕೋಷ್ಟಕ_ಕೂಡಿಸು(iot_cache, iot);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_iot_inval(RISCVIOMMUState *s, GHFunc func,
    uint32_t gscid, uint32_t pscid, hwaddr iova, RISCVIOMMUTrans_ಟ್ಯಾಗ್ transtag)
{
    GHash_ಕೋಷ್ಟಕ *iot_cache;
    RISCVIOMMUEntry ಕೀಲಿ = {
        .ಟ್ಯಾಗ್ = transtag,
        .gscid = gscid,
        .pscid = pscid,
        .iova  = PPN_ಕೆಳಕ್ಕೆ(iova),
    };

    iot_cache = g_hash_ಕೋಷ್ಟಕ_ref(s->iot_cache);
    g_hash_ಕೋಷ್ಟಕ_foreach(iot_cache, func, &ಕೀಲಿ);
    g_hash_ಕೋಷ್ಟಕ_unref(iot_cache);
}

ಸ್ಥಿರ RISCVIOMMUTrans_ಟ್ಯಾಗ್ riscv_iommu_get_transtag(RISCVIOMMUContext *ctx)
{
    uint64_t satp = get_ಕ್ಷೇತ್ರ(ctx->satp, RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ);
    uint64_t gatp = get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ);

    ಆದರೆ (satp == RISCV_IOMMU_DC_FSC_ಬಹುಲಕ_BARE) {
        ಹಿಂತಿರುಗಿ (gatp == RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE) ?
            RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_BY : RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_VG;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ (gatp == RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE) ?
            RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_SS : RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_VN;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ riscv_iommu_translate(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb, bool enable_cache)
{
    RISCVIOMMUTrans_ಟ್ಯಾಗ್ transtag = riscv_iommu_get_transtag(ctx);
    RISCVIOMMUEntry *iot;
    IOMMUAccessFlags perm;
    bool enable_pid;
    bool enable_pri;
    GHash_ಕೋಷ್ಟಕ *iot_cache;
    ಪೂರ್ಣಾಂಕ fault;

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_URQ);

    iot_cache = g_hash_ಕೋಷ್ಟಕ_ref(s->iot_cache);
    enable_pri = (iotlb->perm == IOMMU_NONE) &&
                 (ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI);
    enable_pid = (ctx->tc & RISCV_IOMMU_DC_TC_PDTV);

    /* Check for ATS request. */
    ಆದರೆ (iotlb->perm == IOMMU_NONE) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_ATS_RQ);
        /* Check if ATS is disabled. */
        ಆದರೆ (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS)) {
            enable_pri = false;
            fault = RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
            ಹೋಗು ಮುಗಿದ;
        }
    }

    iot = riscv_iommu_iot_lookup(ctx, iot_cache, iotlb->iova, transtag);
    perm = iot ? iot->perm : IOMMU_NONE;
    ಆದರೆ (perm != IOMMU_NONE) {
        iotlb->translated_addr = PPN_PHYS(iot->phys);
        iotlb->addr_ಮುಖವಾಡ = ~ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;
        iotlb->perm = perm;
        fault = 0;
        ಹೋಗು ಮುಗಿದ;
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_TLB_ತಪ್ಪಿದ);

    /* Translate using device directory / page table information. */
    fault = riscv_iommu_spa_fetch(s, ctx, iotlb);

    ಆದರೆ (!fault && iotlb->ಗುರಿ_as == &s->trap_as) {
        /* Do not cache trapped MSI translations */
        ಹೋಗು ಮುಗಿದ;
    }

    /*
     * We made an implementation choice to not cache identity-mapped
     * translations, as allowed by the specification, to avoid
     * translation cache evictions for other devices sharing the
     * IOMMU hardware model.
     */
    ಆದರೆ (!fault && iotlb->translated_addr != iotlb->iova && enable_cache) {
        iot = g_new0(RISCVIOMMUEntry, 1);
        iot->iova = PPN_ಕೆಳಕ್ಕೆ(iotlb->iova);
        iot->phys = PPN_ಕೆಳಕ್ಕೆ(iotlb->translated_addr);
        iot->gscid = get_ಕ್ಷೇತ್ರ(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
        iot->pscid = get_ಕ್ಷೇತ್ರ(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        iot->perm = iotlb->perm;
        iot->ಟ್ಯಾಗ್ = transtag;
        riscv_iommu_iot_update(s, iot_cache, iot);
    }

ಮುಗಿದ:
    g_hash_ಕೋಷ್ಟಕ_unref(iot_cache);

    ಆದರೆ (enable_pri && fault) {
        ರಚನೆ riscv_iommu_pq_ದಾಖಲೆ pr = {0};
        ಆದರೆ (enable_pid) {
            pr.hdr = ಗಣ_ಕ್ಷೇತ್ರ(RISCV_IOMMU_PREQ_HDR_PV,
                               RISCV_IOMMU_PREQ_HDR_PID, ctx->process_id);
        }
        pr.hdr = ಗಣ_ಕ್ಷೇತ್ರ(pr.hdr, RISCV_IOMMU_PREQ_HDR_DID, ctx->devid);
        pr.payload = (iotlb->iova & ಗುರಿ_ಪುಟ_ಮುಖವಾಡ) |
                     RISCV_IOMMU_PREQ_PAYLOAD_M;
        riscv_iommu_pri(s, &pr);
        ಹಿಂತಿರುಗಿ fault;
    }

    ಆದರೆ (fault) {
        ಚಿಹ್ನೆರಹಿತ ttype = RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ;

        ಆದರೆ ((iotlb->perm & IOMMU_RW) == IOMMU_RW
            || iotlb->perm & IOMMU_WO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iotlb->perm & IOMMU_RO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
        }

        riscv_iommu_report_fault(s, ctx, ttype, fault, enable_pid,
                                 iotlb->iova, iotlb->translated_addr);
        ಹಿಂತಿರುಗಿ fault;
    }

    ಹಿಂತಿರುಗಿ 0;
}

/* IOMMU Command Interface */
ಸ್ಥಿರ Mem_Tx_ಫಲಿತಾಂಶ riscv_iommu_iofence(RISCVIOMMUState *s, bool notify,
    uint64_t addr, uint32_t ದತ್ತಾಂಶ)
{
    /*
     * ATS processing in this implementation of the IOMMU is synchronous,
     * no need to wait for completions here.
     */
    ಆದರೆ (!notify) {
        ಹಿಂತಿರುಗಿ MEMTX_OK;
    }

    ಹಿಂತಿರುಗಿ dma_ಸ್ಮರಣೆ_write(s->ಗುರಿ_as, addr, &ದತ್ತಾಂಶ, ಗಾತ್ರ(ದತ್ತಾಂಶ),
        MEMTXATTRS_UNSPECIFIED);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ats(RISCVIOMMUState *s,
    ರಚನೆ riscv_iommu_command *cmd, IOMMUNotifierFlag flag,
    IOMMUAccessFlags perm,
    ಶೂನ್ಯ (*trace_fn)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id))
{
    RISCVIOMMUSpace *as;
    IOMMUNotifier *n;
    IOMMUTLBEvent event;
    uint32_t pid;
    uint32_t devid;
    ಸ್ಥಿರಾಂಕ bool pv = cmd->dword0 & RISCV_IOMMU_CMD_ATS_PV;

    ಆದರೆ (cmd->dword0 & RISCV_IOMMU_CMD_ATS_DSV) {
        /* Use device segment and requester id */
        devid = get_ಕ್ಷೇತ್ರ(cmd->dword0,
            RISCV_IOMMU_CMD_ATS_DSEG | RISCV_IOMMU_CMD_ATS_RID);
    } ಇಲ್ಲದಿದ್ದರೆ {
        devid = get_ಕ್ಷೇತ್ರ(cmd->dword0, RISCV_IOMMU_CMD_ATS_RID);
    }

    pid = get_ಕ್ಷೇತ್ರ(cmd->dword0, RISCV_IOMMU_CMD_ATS_PID);

    QLIST_FOREACH(as, &s->spaces, list) {
        ಆದರೆ (riscv_iommu_space_devid(as) == devid) {
            ನಿಲ್ಲಿಸು;
        }
    }

    ಆದರೆ (!as || !as->notifier) {
        ಹಿಂತಿರುಗಿ;
    }

    event.type = flag;
    event.ನಮೂದು.perm = perm;
    event.ನಮೂದು.ಗುರಿ_as = s->ಗುರಿ_as;

    IOMMU_NOTIFIER_FOREACH(n, &as->iova_mr) {
        ಆದರೆ (!pv || n->iommu_idx == pid) {
            event.ನಮೂದು.iova = n->ಆರಂಭ;
            event.ನಮೂದು.addr_ಮುಖವಾಡ = n->ಕೊನೆ - n->ಆರಂಭ;
            trace_fn(as->iova_mr.ಪೋಷಕ_obj.name);
            ಸ್ಮರಣೆ_region_notify_iommu_one(n, &event);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ats_inval(RISCVIOMMUState *s,
    ರಚನೆ riscv_iommu_command *cmd)
{
    ಹಿಂತಿರುಗಿ riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_DEVIOTLB_UNMAP, IOMMU_NONE,
                           trace_riscv_iommu_ats_inval);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ats_prgr(RISCVIOMMUState *s,
    ರಚನೆ riscv_iommu_command *cmd)
{
    ಚಿಹ್ನೆರಹಿತ resp_ಸಂಕೇತ = get_ಕ್ಷೇತ್ರ(cmd->dword1,
                                   RISCV_IOMMU_CMD_ATS_PRGR_RESP_ಸಂಕೇತ);

    /* Using the access flag to carry response code information */
    IOMMUAccessFlags perm = resp_ಸಂಕೇತ ? IOMMU_NONE : IOMMU_RW;
    ಹಿಂತಿರುಗಿ riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_MAP, perm,
                           trace_riscv_iommu_ats_prgr);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_process_ddtp(RISCVIOMMUState *s)
{
    uint64_t old_ddtp = s->ddtp;
    uint64_t new_ddtp = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_DDTP);
    ಚಿಹ್ನೆರಹಿತ new_ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(new_ddtp, RISCV_IOMMU_DDTP_ಬಹುಲಕ);
    ಚಿಹ್ನೆರಹಿತ old_ಬಹುಲಕ = get_ಕ್ಷೇತ್ರ(old_ddtp, RISCV_IOMMU_DDTP_ಬಹುಲಕ);
    bool ok = false;

    /*
     * Check for allowed DDTP.MODE transitions:
     * {OFF, BARE}        -> {OFF, BARE, 1LVL, 2LVL, 3LVL}
     * {1LVL, 2LVL, 3LVL} -> {OFF, BARE}
     */
    ಆದರೆ (new_ಬಹುಲಕ == old_ಬಹುಲಕ ||
        new_ಬಹುಲಕ == RISCV_IOMMU_DDTP_ಬಹುಲಕ_OFF ||
        new_ಬಹುಲಕ == RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE) {
        ok = true;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (new_ಬಹುಲಕ == RISCV_IOMMU_DDTP_ಬಹುಲಕ_1_LVL ||
               new_ಬಹುಲಕ == RISCV_IOMMU_DDTP_ಬಹುಲಕ_2_LVL ||
               new_ಬಹುಲಕ == RISCV_IOMMU_DDTP_ಬಹುಲಕ_3_LVL) {
        ok = old_ಬಹುಲಕ == RISCV_IOMMU_DDTP_ಬಹುಲಕ_OFF ||
             old_ಬಹುಲಕ == RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE;
    }

    ಆದರೆ (ok) {
        /* clear reserved and busy bits, report back sanitized version */
        new_ddtp = ಗಣ_ಕ್ಷೇತ್ರ(new_ddtp & RISCV_IOMMU_DDTP_PPN,
                             RISCV_IOMMU_DDTP_ಬಹುಲಕ, new_ಬಹುಲಕ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        new_ddtp = old_ddtp;
    }
    s->ddtp = new_ddtp;

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, new_ddtp);
}

/* Command function and opcode field. */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD(func, op) (((func) << 7) | (op))

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_process_cq_ಬಾಲ(RISCVIOMMUState *s)
{
    ರಚನೆ riscv_iommu_command cmd;
    Mem_Tx_ಫಲಿತಾಂಶ res;
    dma_addr_t addr;
    uint32_t ಬಾಲ, ತಲೆ, ctrl;
    uint64_t cmd_opcode;
    GHFunc func;

    ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    ಬಾಲ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQT) & s->cq_ಮುಖವಾಡ;
    ತಲೆ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQH) & s->cq_ಮುಖವಾಡ;

    /* Check for pending error or queue processing disabled */
    ಆದರೆ (!(ctrl & RISCV_IOMMU_CQCSR_CQON) ||
        !!(ctrl & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆಗ (ಬಾಲ != ತಲೆ) {
        addr = s->cq_addr  + ತಲೆ * ಗಾತ್ರ(cmd);
        res = dma_ಸ್ಮರಣೆ_read(s->ಗುರಿ_as, addr, &cmd, ಗಾತ್ರ(cmd),
                              MEMTXATTRS_UNSPECIFIED);

        ಆದರೆ (res != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                  RISCV_IOMMU_CQCSR_CQMF, 0);
            ಹೋಗು fault;
        }

        trace_riscv_iommu_cmd(s->ಪೋಷಕ_obj.id, cmd.dword0, cmd.dword1);

        cmd_opcode = get_ಕ್ಷೇತ್ರ(cmd.dword0,
                               RISCV_IOMMU_CMD_OPCODE | RISCV_IOMMU_CMD_FUNC);

        ಆಯ್ಕೆ (cmd_opcode) {
        ಸ್ಥಿತಿ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOFENCE_FUNC_C,
                             RISCV_IOMMU_CMD_IOFENCE_OPCODE):
            ಆದರೆ (cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_RESERVED) {
                ಹೋಗು cmd_ill;
            }

            res = riscv_iommu_iofence(s,
                cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_AV, cmd.dword1 << 2,
                get_ಕ್ಷೇತ್ರ(cmd.dword0, RISCV_IOMMU_CMD_IOFENCE_ದತ್ತಾಂಶ));

            ಆದರೆ (res != MEMTX_OK) {
                riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                      RISCV_IOMMU_CQCSR_CQMF, 0);
                ಹೋಗು fault;
            }
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_ಕ್ಷೇತ್ರ(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_ಕ್ಷೇತ್ರ(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;

            ಆದರೆ (pscv) {
                /* illegal command arguments IOTINVAL.GVMA & PSCV == 1 */
                ಹೋಗು cmd_ill;
            }

            func = riscv_iommu_iot_inval_all;

            ಆದರೆ (gv) {
                func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                              riscv_iommu_iot_inval_gscid;
            }

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_VG);

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_VN);
            ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_ಕ್ಷೇತ್ರ(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_ಕ್ಷೇತ್ರ(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & ಗುರಿ_ಪುಟ_ಮುಖವಾಡ;
            RISCVIOMMUTrans_ಟ್ಯಾಗ್ transtag;

            ಆದರೆ (gv) {
                transtag = RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_VN;
                ಆದರೆ (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_gscid_pscid_iova :
                                  riscv_iommu_iot_inval_gscid_pscid;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                                  riscv_iommu_iot_inval_gscid;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                transtag = RISCV_IOMMU_TRANS_ಟ್ಯಾಗ್_SS;
                ಆದರೆ (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_pscid_iova :
                                  riscv_iommu_iot_inval_pscid;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    func = (av) ? riscv_iommu_iot_inval_iova :
                                  riscv_iommu_iot_inval_all;
                }
            }

            riscv_iommu_iot_inval(s, func, gscid, pscid, iova, transtag);
            ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            ಆದರೆ (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* invalidate all device context cache mappings */
                func = riscv_iommu_ctx_inval_all;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* invalidate all device context matching DID */
                func = riscv_iommu_ctx_inval_devid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_ಕ್ಷೇತ್ರ(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID), 0);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            ಆದರೆ (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* illegal command arguments IODIR_PDT & DV == 0 */
                ಹೋಗು cmd_ill;
            } ಇಲ್ಲದಿದ್ದರೆ {
                func = riscv_iommu_ctx_inval_devid_procid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_ಕ್ಷೇತ್ರ(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID),
                get_ಕ್ಷೇತ್ರ(cmd.dword0, RISCV_IOMMU_CMD_IODIR_PID));
            ನಿಲ್ಲಿಸು;

        /* ATS commands */
        ಸ್ಥಿತಿ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_INVAL,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            ಆದರೆ (!s->enable_ats) {
                ಹೋಗು cmd_ill;
            }

            riscv_iommu_ats_inval(s, &cmd);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_PRGR,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            ಆದರೆ (!s->enable_ats) {
                ಹೋಗು cmd_ill;
            }

            riscv_iommu_ats_prgr(s, &cmd);
            ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
        cmd_ill:
            /* Invalid instruction, do not advance instruction index. */
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                RISCV_IOMMU_CQCSR_CMD_ILL, 0);
            ಹೋಗು fault;
        }

        /* Advance and update head pointer after command completes. */
        ತಲೆ = (ತಲೆ + 1) & s->cq_ಮುಖವಾಡ;
        riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_CQH, ತಲೆ);
    }
    ಹಿಂತಿರುಗಿ;

fault:
    ಆದರೆ (ctrl & RISCV_IOMMU_CQCSR_CIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_CQ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_process_cq_control(RISCVIOMMUState *s)
{
    uint64_t ತಳ;
    uint32_t ctrl_ಗಣ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_ಗಣ & RISCV_IOMMU_CQCSR_CQEN);
    bool ಸಕ್ರಿಯ = !!(ctrl_ಗಣ & RISCV_IOMMU_CQCSR_CQON);

    ಆದರೆ (enable && !ಸಕ್ರಿಯ) {
        ತಳ = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_CQB);
        s->cq_ಮುಖವಾಡ = (2ULL << get_ಕ್ಷೇತ್ರ(ತಳ, RISCV_IOMMU_CQB_LOG2SZ)) - 1;
        s->cq_addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ತಳ, RISCV_IOMMU_CQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~s->cq_ಮುಖವಾಡ);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQT], 0);
        ctrl_ಗಣ = RISCV_IOMMU_CQCSR_CQON;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQMF |
                   RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CMD_ಗಮ್ಯ |
                   RISCV_IOMMU_CQCSR_FENCE_W_IP;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!enable && ಸಕ್ರಿಯ) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~0);
        ctrl_ಗಣ = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQON;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ctrl_ಗಣ = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, ctrl_ಗಣ, ctrl_clr);

    /*
     * After clearing error bits (CMD_ILL, CQMF), if queue is still active,
     * re-process pending command.
     */
    ctrl_ಗಣ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    ಆದರೆ ((ctrl_ಗಣ & RISCV_IOMMU_CQCSR_CQON) &&
        !(ctrl_ಗಣ & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        riscv_iommu_process_cq_ಬಾಲ(s);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_process_fq_control(RISCVIOMMUState *s)
{
    uint64_t ತಳ;
    uint32_t ctrl_ಗಣ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_ಗಣ & RISCV_IOMMU_FQCSR_FQEN);
    bool ಸಕ್ರಿಯ = !!(ctrl_ಗಣ & RISCV_IOMMU_FQCSR_FQON);

    ಆದರೆ (enable && !ಸಕ್ರಿಯ) {
        ತಳ = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_FQB);
        s->fq_ಮುಖವಾಡ = (2ULL << get_ಕ್ಷೇತ್ರ(ತಳ, RISCV_IOMMU_FQB_LOG2SZ)) - 1;
        s->fq_addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ತಳ, RISCV_IOMMU_FQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~s->fq_ಮುಖವಾಡ);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQT], 0);
        ctrl_ಗಣ = RISCV_IOMMU_FQCSR_FQON;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQMF |
            RISCV_IOMMU_FQCSR_FQOF;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!enable && ಸಕ್ರಿಯ) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~0);
        ctrl_ಗಣ = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQON;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ctrl_ಗಣ = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, ctrl_ಗಣ, ctrl_clr);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_process_pq_control(RISCVIOMMUState *s)
{
    uint64_t ತಳ;
    uint32_t ctrl_ಗಣ = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_ಗಣ & RISCV_IOMMU_PQCSR_PQEN);
    bool ಸಕ್ರಿಯ = !!(ctrl_ಗಣ & RISCV_IOMMU_PQCSR_PQON);

    ಆದರೆ (enable && !ಸಕ್ರಿಯ) {
        ತಳ = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_PQB);
        s->pq_ಮುಖವಾಡ = (2ULL << get_ಕ್ಷೇತ್ರ(ತಳ, RISCV_IOMMU_PQB_LOG2SZ)) - 1;
        s->pq_addr = PPN_PHYS(get_ಕ್ಷೇತ್ರ(ತಳ, RISCV_IOMMU_PQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~s->pq_ಮುಖವಾಡ);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQT], 0);
        ctrl_ಗಣ = RISCV_IOMMU_PQCSR_PQON;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQMF |
            RISCV_IOMMU_PQCSR_PQOF;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!enable && ಸಕ್ರಿಯ) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~0);
        ctrl_ಗಣ = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQON;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ctrl_ಗಣ = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, ctrl_ಗಣ, ctrl_clr);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_process_dbg(RISCVIOMMUState *s)
{
    uint64_t iova = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_IOVA);
    uint64_t ctrl = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_CTL);
    ಚಿಹ್ನೆರಹಿತ devid = get_ಕ್ಷೇತ್ರ(ctrl, RISCV_IOMMU_TR_REQ_CTL_DID);
    ಚಿಹ್ನೆರಹಿತ pid = get_ಕ್ಷೇತ್ರ(ctrl, RISCV_IOMMU_TR_REQ_CTL_PID);
    IOMMUAccessFlags perm = ctrl & RISCV_IOMMU_TR_REQ_CTL_NW
                            ? IOMMU_RO : IOMMU_RW;
    RISCVIOMMUContext *ctx;
    ಶೂನ್ಯ *ref;

    ಆದರೆ (!(ctrl & RISCV_IOMMU_TR_REQ_CTL_GO_BUSY)) {
        ಹಿಂತಿರುಗಿ;
    }

    ctx = riscv_iommu_ctx(s, devid, pid, perm, iova, &ref);
    ಆದರೆ (ctx == ಖಾಲಿ) {
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE,
                                 RISCV_IOMMU_TR_RESPONSE_FAULT |
                                 (RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED << 10));
    } ಇಲ್ಲದಿದ್ದರೆ {
        IOMMUTLBEntry iotlb = {
            .iova = iova,
            .perm = perm,
            .addr_ಮುಖವಾಡ = ~0,
            .ಗುರಿ_as = ಖಾಲಿ,
        };
        ಪೂರ್ಣಾಂಕ fault = riscv_iommu_translate(s, ctx, &iotlb, false);
        ಆದರೆ (fault) {
            iova = RISCV_IOMMU_TR_RESPONSE_FAULT | (((uint64_t) fault) << 10);
        } ಇಲ್ಲದಿದ್ದರೆ {
            iova = iotlb.translated_addr & ~iotlb.addr_ಮುಖವಾಡ;
            iova = ಗಣ_ಕ್ಷೇತ್ರ(0, RISCV_IOMMU_TR_RESPONSE_PPN, PPN_ಕೆಳಕ್ಕೆ(iova));
        }
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE, iova);
    }

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
        RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    riscv_iommu_ctx_put(s, ref);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ riscv_iommu_process_fn(RISCVIOMMUState *s);

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_update_icvec(RISCVIOMMUState *s, uint64_t ದತ್ತಾಂಶ)
{
    uint64_t icvec = 0;

    icvec |= MIN(ದತ್ತಾಂಶ & RISCV_IOMMU_ICVEC_CIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_CIV);

    icvec |= MIN(ದತ್ತಾಂಶ & RISCV_IOMMU_ICVEC_FIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_FIV);

    icvec |= MIN(ದತ್ತಾಂಶ & RISCV_IOMMU_ICVEC_PMIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PMIV);

    icvec |= MIN(ದತ್ತಾಂಶ & RISCV_IOMMU_ICVEC_PIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PIV);

    trace_riscv_iommu_icvec_write(ದತ್ತಾಂಶ, icvec);

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_ICVEC, icvec);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_update_ipsr(RISCVIOMMUState *s, uint64_t ದತ್ತಾಂಶ)
{
    uint32_t cqcsr, fqcsr, pqcsr;
    uint32_t ipsr_ಗಣ = 0;
    uint32_t ipsr_clr = 0;

    ಆದರೆ (ದತ್ತಾಂಶ & RISCV_IOMMU_IPSR_CIP) {
        cqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);

        ಆದರೆ (cqcsr & RISCV_IOMMU_CQCSR_CIE &&
            (cqcsr & RISCV_IOMMU_CQCSR_FENCE_W_IP ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ILL ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ಗಮ್ಯ ||
             cqcsr & RISCV_IOMMU_CQCSR_CQMF)) {
            ipsr_ಗಣ |= RISCV_IOMMU_IPSR_CIP;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
    }

    ಆದರೆ (ದತ್ತಾಂಶ & RISCV_IOMMU_IPSR_FIP) {
        fqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);

        ಆದರೆ (fqcsr & RISCV_IOMMU_FQCSR_FIE &&
            (fqcsr & RISCV_IOMMU_FQCSR_FQOF ||
             fqcsr & RISCV_IOMMU_FQCSR_FQMF)) {
            ipsr_ಗಣ |= RISCV_IOMMU_IPSR_FIP;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
    }

    ಆದರೆ (!(ದತ್ತಾಂಶ & RISCV_IOMMU_IPSR_PMIP)) {
        ipsr_clr |= RISCV_IOMMU_IPSR_PMIP;
    }

    ಆದರೆ (ದತ್ತಾಂಶ & RISCV_IOMMU_IPSR_PIP) {
        pqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);

        ಆದರೆ (pqcsr & RISCV_IOMMU_PQCSR_PIE &&
            (pqcsr & RISCV_IOMMU_PQCSR_PQOF ||
             pqcsr & RISCV_IOMMU_PQCSR_PQMF)) {
            ipsr_ಗಣ |= RISCV_IOMMU_IPSR_PIP;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, ipsr_ಗಣ, ipsr_clr);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_process_hpm_writes(RISCVIOMMUState *s,
                                           uint32_t regb,
                                           bool prev_cy_inh)
{
    ಆಯ್ಕೆ (regb) {
    ಸ್ಥಿತಿ RISCV_IOMMU_REG_IOCOUNTINH:
        riscv_iommu_process_iocntinh_cy(s, prev_cy_inh);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_IOHPMCYCLES:
    ಸ್ಥಿತಿ RISCV_IOMMU_REG_IOHPMCYCLES + 4:
        riscv_iommu_process_hpmcycle_write(s);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_IOHPMEVT_ತಳ ...
        RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_ಅಂಕೆ) + 4:
        riscv_iommu_process_hpmevt_write(s, regb & ~7);
        ನಿಲ್ಲಿಸು;
    }
}

/*
 * Write the resulting value of 'data' for the reg specified
 * by 'reg_addr', after considering read-only/read-write/write-clear
 * bits, in the pointer 'dest'.
 *
 * The result is written in little-endian.
 */
ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_write_reg_val(RISCVIOMMUState *s,
                                      ಶೂನ್ಯ *dest, hwaddr reg_addr,
                                      ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, uint64_t ದತ್ತಾಂಶ)
{
    uint64_t ro = ldn_le_p(&s->regs_ro[reg_addr], ಗಾತ್ರ_2);
    uint64_t wc = ldn_le_p(&s->regs_wc[reg_addr], ಗಾತ್ರ_2);
    uint64_t curr_val = ldn_le_p(&s->regs[reg_addr], ಗಾತ್ರ_2);

    stn_le_p(dest, ಗಾತ್ರ_2, ((curr_val & ro) | (ದತ್ತಾಂಶ & ~ro)) & ~(ದತ್ತಾಂಶ & wc));
}

ಸ್ಥಿರ Mem_Tx_ಫಲಿತಾಂಶ riscv_iommu_mmio_write(ಶೂನ್ಯ *opaque, hwaddr addr,
                                          uint64_t ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2,
                                          MemTxAttrs attrs)
{
    riscv_iommu_process_fn *process_fn = ಖಾಲಿ;
    RISCVIOMMUState *s = opaque;
    uint32_t regb = addr & ~3;
    uint32_t busy = 0;
    uint64_t val = 0;
    bool cy_inh = false;

    ಆದರೆ ((addr & (ಗಾತ್ರ_2 - 1)) != 0) {
        /* Unsupported MMIO alignment or access size */
        ಹಿಂತಿರುಗಿ MEMTX_ERROR;
    }

    ಆದರೆ (addr + ಗಾತ್ರ_2 > RISCV_IOMMU_REG_MSI_CONFIG) {
        /* Unsupported MMIO access location. */
        ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
    }

    /* Track actionable MMIO write. */
    ಆಯ್ಕೆ (regb) {
    ಸ್ಥಿತಿ RISCV_IOMMU_REG_DDTP:
    ಸ್ಥಿತಿ RISCV_IOMMU_REG_DDTP + 4:
        process_fn = riscv_iommu_process_ddtp;
        regb = RISCV_IOMMU_REG_DDTP;
        busy = RISCV_IOMMU_DDTP_BUSY;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_CQT:
        process_fn = riscv_iommu_process_cq_ಬಾಲ;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_CQCSR:
        process_fn = riscv_iommu_process_cq_control;
        busy = RISCV_IOMMU_CQCSR_BUSY;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_FQCSR:
        process_fn = riscv_iommu_process_fq_control;
        busy = RISCV_IOMMU_FQCSR_BUSY;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_PQCSR:
        process_fn = riscv_iommu_process_pq_control;
        busy = RISCV_IOMMU_PQCSR_BUSY;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_ICVEC:
    ಸ್ಥಿತಿ RISCV_IOMMU_REG_IPSR:
        /*
         * ICVEC and IPSR have special read/write procedures. We'll
         * call their respective helpers and exit.
         */
        riscv_iommu_write_reg_val(s, &val, addr, ಗಾತ್ರ_2, ದತ್ತಾಂಶ);

        /*
         * 'val' is stored as LE. Switch to host endianess
         * before using it.
         */
        val = le64_ಗಮ್ಯ_cpu(val);

        ಆದರೆ (regb == RISCV_IOMMU_REG_ICVEC) {
            riscv_iommu_update_icvec(s, val);
        } ಇಲ್ಲದಿದ್ದರೆ {
            riscv_iommu_update_ipsr(s, val);
        }

        ಹಿಂತಿರುಗಿ MEMTX_OK;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_TR_REQ_CTL:
        process_fn = riscv_iommu_process_dbg;
        regb = RISCV_IOMMU_REG_TR_REQ_CTL;
        busy = RISCV_IOMMU_TR_REQ_CTL_GO_BUSY;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ RISCV_IOMMU_REG_IOCOUNTINH:
        ಆದರೆ (addr != RISCV_IOMMU_REG_IOCOUNTINH) {
            ನಿಲ್ಲಿಸು;
        }
        /* Store previous value of CY bit. */
        cy_inh = !!(riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTINH) &
            RISCV_IOMMU_IOCOUNTINH_CY);
        ನಿಲ್ಲಿಸು;


    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    /*
     * Registers update might be not synchronized with core logic.
     * If system software updates register when relevant BUSY bit
     * is set IOMMU behavior of additional writes to the register
     * is UNSPECIFIED.
     */
    riscv_iommu_write_reg_val(s, &s->regs[addr], addr, ಗಾತ್ರ_2, ದತ್ತಾಂಶ);

    /* Busy flag update, MSB 4-byte register. */
    ಆದರೆ (busy) {
        uint32_t rw = ldl_le_p(&s->regs[regb]);
        stl_le_p(&s->regs[regb], rw | busy);
    }

    /* Process HPM writes and update any internal state if needed. */
    ಆದರೆ (regb >= RISCV_IOMMU_REG_IOCOUNTOVF &&
        regb <= (RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_ಅಂಕೆ) + 4)) {
        riscv_iommu_process_hpm_writes(s, regb, cy_inh);
    }

    ಆದರೆ (process_fn) {
        process_fn(s);
    }

    ಹಿಂತಿರುಗಿ MEMTX_OK;
}

ಸ್ಥಿರ Mem_Tx_ಫಲಿತಾಂಶ riscv_iommu_mmio_read(ಶೂನ್ಯ *opaque, hwaddr addr,
    uint64_t *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2, MemTxAttrs attrs)
{
    RISCVIOMMUState *s = opaque;
    uint64_t val = -1;
    uint8_t *ptr;

    ಆದರೆ ((addr & (ಗಾತ್ರ_2 - 1)) != 0) {
        /* Unsupported MMIO alignment. */
        ಹಿಂತಿರುಗಿ MEMTX_ERROR;
    }

    ಆದರೆ (addr + ಗಾತ್ರ_2 > RISCV_IOMMU_REG_MSI_CONFIG) {
        ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
    }

    /* Compute cycle register value. */
    ಆದರೆ ((addr & ~7) == RISCV_IOMMU_REG_IOHPMCYCLES) {
        val = riscv_iommu_hpmcycle_read(s);
        ptr = (uint8_t *)&val + (addr & 7);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((addr & ~3) == RISCV_IOMMU_REG_IOCOUNTOVF) {
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
    } ಇಲ್ಲದಿದ್ದರೆ {
        ptr = &s->regs[addr];
    }

    val = ldn_le_p(ptr, ಗಾತ್ರ_2);

    *ದತ್ತಾಂಶ = val;

    ಹಿಂತಿರುಗಿ MEMTX_OK;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಸ್ಮರಣೆ_Region_Ops riscv_iommu_mmio_ops = {
    .read_with_attrs = riscv_iommu_mmio_read,
    .write_with_attrs = riscv_iommu_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_ಗಾತ್ರ = 4,
        .max_access_ಗಾತ್ರ = 8,
        .unaligned = false,
    },
    .valid = {
        .min_access_ಗಾತ್ರ = 4,
        .max_access_ಗಾತ್ರ = 8,
    }
};

/*
 * Translations matching MSI pattern check are redirected to "riscv-iommu-trap"
 * memory region as untranslated address, for additional MSI/MRIF interception
 * by IOMMU interrupt remapping implementation.
 * Note: Device emulation code generating an MSI is expected to provide a valid
 * memory transaction attributes with requested_id set.
 */
ಸ್ಥಿರ Mem_Tx_ಫಲಿತಾಂಶ riscv_iommu_trap_write(ಶೂನ್ಯ *opaque, hwaddr addr,
    uint64_t ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2, MemTxAttrs attrs)
{
    RISCVIOMMUState* s = (RISCVIOMMUState *)opaque;
    RISCVIOMMUContext *ctx;
    Mem_Tx_ಫಲಿತಾಂಶ res;
    ಶೂನ್ಯ *ref;
    uint32_t devid = attrs.requester_id;

    ಆದರೆ (attrs.unspecified) {
        ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
    }

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= s->bus << 8;

    ctx = riscv_iommu_ctx(s, devid, 0, IOMMU_RW, addr, &ref);
    ಆದರೆ (ctx == ಖಾಲಿ) {
        res = MEMTX_ACCESS_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        res = riscv_iommu_msi_write(s, ctx, addr, ದತ್ತಾಂಶ, ಗಾತ್ರ_2, attrs);
    }
    riscv_iommu_ctx_put(s, ref);
    ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ Mem_Tx_ಫಲಿತಾಂಶ riscv_iommu_trap_read(ಶೂನ್ಯ *opaque, hwaddr addr,
    uint64_t *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2, MemTxAttrs attrs)
{
    ಹಿಂತಿರುಗಿ MEMTX_ACCESS_ERROR;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಸ್ಮರಣೆ_Region_Ops riscv_iommu_trap_ops = {
    .read_with_attrs = riscv_iommu_trap_read,
    .write_with_attrs = riscv_iommu_trap_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_ಗಾತ್ರ = 4,
        .max_access_ಗಾತ್ರ = 8,
    },
    .valid = {
        .min_access_ಗಾತ್ರ = 4,
        .max_access_ಗಾತ್ರ = 8,
    }
};

ಶೂನ್ಯ riscv_iommu_ಗಣ_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_ಬಹುಲಕ ಬಹುಲಕ)
{
    s->cap = ಗಣ_ಕ್ಷೇತ್ರ(s->cap, RISCV_IOMMU_CAP_IGS, ಬಹುಲಕ);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_instance_init(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    /* Enable translation debug interface */
    s->cap = RISCV_IOMMU_CAP_DBG;

    /* TODO: method to report supported PID bits */
    s->pid_ಬಿಟ್ಟುಗಳು = 8; /* restricted to size of MemTxAttrs.pid */
    s->cap |= RISCV_IOMMU_CAP_PD8;

    /* register storage */
    s->regs = g_new0(uint8_t, RISCV_IOMMU_REG_ಗಾತ್ರ);
    s->regs_ro = g_new0(uint8_t, RISCV_IOMMU_REG_ಗಾತ್ರ);
    s->regs_wc = g_new0(uint8_t, RISCV_IOMMU_REG_ಗಾತ್ರ);

     /* Mark all registers read-only */
    memset(s->regs_ro, 0xff, RISCV_IOMMU_REG_ಗಾತ್ರ);

    /* Device translation context cache */
    s->ctx_cache = g_hash_ಕೋಷ್ಟಕ_new_ತುಂಬಿದ(riscv_iommu_ctx_hash,
                                         riscv_iommu_ctx_equal,
                                         g_free, ಖಾಲಿ);

    s->iot_cache = g_hash_ಕೋಷ್ಟಕ_new_ತುಂಬಿದ(riscv_iommu_iot_hash,
                                         riscv_iommu_iot_equal,
                                         g_free, ಖಾಲಿ);

    s->iommus.le_ಮುಂದಿನ = ಖಾಲಿ;
    s->iommus.le_prev = ಖಾಲಿ;
    QLIST_INIT(&s->spaces);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_instance_finalize(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    g_free(s->regs);
    g_free(s->regs_ro);
    g_free(s->regs_wc);

    g_hash_ಕೋಷ್ಟಕ_unref(s->ctx_cache);
    g_hash_ಕೋಷ್ಟಕ_unref(s->iot_cache);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    /* Report QEMU target physical address space limits. */
    s->cap = ಗಣ_ಕ್ಷೇತ್ರ(s->cap, RISCV_IOMMU_CAP_PAS, s->pas_ಬಿಟ್ಟುಗಳು);

    s->cap |= s->ಆವೃತ್ತಿ & RISCV_IOMMU_CAP_ಆವೃತ್ತಿ;
    ಆದರೆ (s->enable_msi) {
        s->cap |= RISCV_IOMMU_CAP_MSI_FLAT | RISCV_IOMMU_CAP_MSI_MRIF;
    }
    ಆದರೆ (s->enable_ats) {
        s->cap |= RISCV_IOMMU_CAP_ATS;
    }
    ಆದರೆ (s->enable_s_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32 | RISCV_IOMMU_CAP_SV39 |
                  RISCV_IOMMU_CAP_SV48 | RISCV_IOMMU_CAP_SV57;
    }
    ಆದರೆ (s->enable_g_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32X4 | RISCV_IOMMU_CAP_SV39X4 |
                  RISCV_IOMMU_CAP_SV48X4 | RISCV_IOMMU_CAP_SV57X4 |
                  RISCV_IOMMU_CAP_SVRSW60T59B;
    }

    ಆದರೆ (s->hpm_cntrs > 0) {
        /* Clip number of HPM counters to maximum supported (31). */
        ಆದರೆ (s->hpm_cntrs > RISCV_IOMMU_IOCOUNT_ಅಂಕೆ) {
            s->hpm_cntrs = RISCV_IOMMU_IOCOUNT_ಅಂಕೆ;
        }
        /* Enable hardware performance monitor interface */
        s->cap |= RISCV_IOMMU_CAP_HPM;
    }

    /* Out-of-reset translation mode: OFF (DMA disabled) BARE (passthrough) */
    s->ddtp = ಗಣ_ಕ್ಷೇತ್ರ(0, RISCV_IOMMU_DDTP_ಬಹುಲಕ, s->enable_off ?
                        RISCV_IOMMU_DDTP_ಬಹುಲಕ_OFF : RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE);

    /*
     * Register complete MMIO space, including MSI/PBA registers.
     * Note, PCIDevice implementation will add overlapping MR for MSI/PBA,
     * managed directly by the PCIDevice implementation.
     */
    ಸ್ಮರಣೆ_region_init_io(&s->regs_mr, OBJECT(dev), &riscv_iommu_mmio_ops, s,
        "riscv-iommu-regs", RISCV_IOMMU_REG_ಗಾತ್ರ);

    /* Set power-on register state */
    stq_le_p(&s->regs[RISCV_IOMMU_REG_CAP], s->cap);

    stq_le_p(&s->regs[RISCV_IOMMU_REG_FCTL], 0);
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FCTL],
             ~(RISCV_IOMMU_FCTL_GXL | RISCV_IOMMU_FCTL_WSI));

    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_DDTP],
        ~(RISCV_IOMMU_DDTP_PPN | RISCV_IOMMU_DDTP_ಬಹುಲಕ));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQB],
        ~(RISCV_IOMMU_CQB_LOG2SZ | RISCV_IOMMU_CQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQB],
        ~(RISCV_IOMMU_FQB_LOG2SZ | RISCV_IOMMU_FQB_PPN));
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQB],
        ~(RISCV_IOMMU_PQB_LOG2SZ | RISCV_IOMMU_PQB_PPN));
    stl_le_p(&s->regs_wc[RISCV_IOMMU_REG_CQCSR], RISCV_IOMMU_CQCSR_CQMF |
        RISCV_IOMMU_CQCSR_CMD_ಗಮ್ಯ | RISCV_IOMMU_CQCSR_CMD_ILL);
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
    ಆದರೆ (s->cap & RISCV_IOMMU_CAP_DBG) {
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_IOVA], 0);
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_CTL],
            RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    }

    /* If HPM registers are enabled. */
    ಆದರೆ (s->cap & RISCV_IOMMU_CAP_HPM) {
        /* +1 for cycle counter bit. */
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOCOUNTINH],
                 ~((2 << s->hpm_cntrs) - 1));
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCYCLES], 0);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCTR_ತಳ],
               0x00, s->hpm_cntrs * 8);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMEVT_ತಳ],
               0x00, s->hpm_cntrs * 8);
    }

    /* Memory region for downstream access, if specified. */
    ಆದರೆ (s->ಗುರಿ_mr) {
        s->ಗುರಿ_as = g_new0(ವಿಳಾಸ_Space, 1);
        ವಿಳಾಸ_space_init(s->ಗುರಿ_as, s->ಗುರಿ_mr,
            "riscv-iommu-downstream");
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Fallback to global system memory. */
        s->ಗುರಿ_as = &ವಿಳಾಸ_space_ಸ್ಮರಣೆ;
    }

    /* Memory region for untranslated MRIF/MSI writes */
    ಸ್ಮರಣೆ_region_init_io(&s->trap_mr, OBJECT(dev), &riscv_iommu_trap_ops, s,
            "riscv-iommu-trap", ~0ULL);
    ವಿಳಾಸ_space_init(&s->trap_as, &s->trap_mr, "riscv-iommu-trap-as");

    ಆದರೆ (s->cap & RISCV_IOMMU_CAP_HPM) {
        s->hpm_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL, riscv_iommu_hpm_timer_cb, s);
        s->hpm_event_ctr_map = g_hash_ಕೋಷ್ಟಕ_new(g_ನೇರ_hash, g_ನೇರ_equal);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_unrealize(DeviceState *dev)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    ಆದರೆ (s->cap & RISCV_IOMMU_CAP_HPM) {
        g_hash_ಕೋಷ್ಟಕ_unref(s->hpm_event_ctr_map);
        timer_free(s->hpm_timer);
    }
}

ಶೂನ್ಯ riscv_iommu_reset(RISCVIOMMUState *s)
{
    uint32_t reg_clr;
    ಪೂರ್ಣಾಂಕ ddtp_ಬಹುಲಕ;

    /*
     * Clear DDTP while setting DDTP_mode back to user
     * initial setting.
     */
    ddtp_ಬಹುಲಕ = s->enable_off ?
                RISCV_IOMMU_DDTP_ಬಹುಲಕ_OFF : RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE;
    s->ddtp = ಗಣ_ಕ್ಷೇತ್ರ(0, RISCV_IOMMU_DDTP_ಬಹುಲಕ, ddtp_ಬಹುಲಕ);
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

    g_hash_ಕೋಷ್ಟಕ_remove_all(s->ctx_cache);
    g_hash_ಕೋಷ್ಟಕ_remove_all(s->iot_cache);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Property riscv_iommu_properties[] = {
    DEFINE_PROP_UINT32("version", RISCVIOMMUState, ಆವೃತ್ತಿ,
        RISCV_IOMMU_SPEC_DOT_VER),
    DEFINE_PROP_UINT32("pas-bits", RISCVIOMMUState, pas_ಬಿಟ್ಟುಗಳು, 0),
    DEFINE_PROP_UINT32("bus", RISCVIOMMUState, bus, 0x0),
    DEFINE_PROP_UINT32("ioatc-limit", RISCVIOMMUState, iot_ಎಲ್ಲೆ,
        ಎಲ್ಲೆ_CACHE_IOT),
    DEFINE_PROP_BOOL("intremap", RISCVIOMMUState, enable_msi, TRUE),
    DEFINE_PROP_BOOL("ats", RISCVIOMMUState, enable_ats, TRUE),
    DEFINE_PROP_BOOL("off", RISCVIOMMUState, enable_off, TRUE),
    DEFINE_PROP_BOOL("s-stage", RISCVIOMMUState, enable_s_stage, TRUE),
    DEFINE_PROP_BOOL("g-stage", RISCVIOMMUState, enable_g_stage, TRUE),
    DEFINE_PROP_LINK("downstream-mr", RISCVIOMMUState, ಗುರಿ_mr,
        TYPE_ಸ್ಮರಣೆ_REGION, ಸ್ಮರಣೆ_Region *),
    DEFINE_PROP_UINT8("hpm-counters", RISCVIOMMUState, hpm_cntrs,
                      RISCV_IOMMU_IOCOUNT_ಅಂಕೆ),
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_class_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    /* internal device for riscv-iommu-{pci/sys}, not user-creatable */
    dc->user_creatable = false;
    dc->realize = riscv_iommu_realize;
    dc->unrealize = riscv_iommu_unrealize;
    device_class_ಗಣ_props(dc, riscv_iommu_properties);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo riscv_iommu_info = {
    .name = TYPE_RISCV_IOMMU,
    .ಪೋಷಕ = TYPE_DEVICE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(RISCVIOMMUState),
    .instance_init = riscv_iommu_instance_init,
    .instance_finalize = riscv_iommu_instance_finalize,
    .class_init = riscv_iommu_class_init,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *IOMMU_FLAG_STR[] = {
    "NA",
    "RO",
    "WR",
    "RW",
};

/* RISC-V IOMMU Memory Region - Address Translation Space */
ಸ್ಥಿರ IOMMUTLBEntry riscv_iommu_ಸ್ಮರಣೆ_region_translate(
    IOMMUMemoryRegion *iommu_mr, hwaddr addr,
    IOMMUAccessFlags flag, ಪೂರ್ಣಾಂಕ iommu_idx)
{
    RISCVIOMMUSpace *as = container_ದ(iommu_mr, RISCVIOMMUSpace, iova_mr);
    RISCVIOMMUContext *ctx;
    ಶೂನ್ಯ *ref;
    IOMMUTLBEntry iotlb = {
        .iova = addr,
        .ಗುರಿ_as = as->iommu->ಗುರಿ_as,
        .addr_ಮುಖವಾಡ = ~0ULL,
        .perm = flag,
    };
    uint32_t devid = riscv_iommu_space_devid(as);

    ctx = riscv_iommu_ctx(as->iommu, devid, iommu_idx, flag, addr, &ref);
    ಆದರೆ (ctx == ಖಾಲಿ) {
        /* Translation disabled or invalid. */
        iotlb.addr_ಮುಖವಾಡ = 0;
        iotlb.perm = IOMMU_NONE;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (riscv_iommu_translate(as->iommu, ctx, &iotlb, true)) {
        /* Translation disabled or fault reported. */
        iotlb.addr_ಮುಖವಾಡ = 0;
        iotlb.perm = IOMMU_NONE;
    }

    /* Trace all dma translations with original access flags. */
    trace_riscv_iommu_dma(as->iommu->ಪೋಷಕ_obj.id, PCI_BUS_ಅಂಕೆ(devid),
                          PCI_ಸ್ಥಾನ(devid), PCI_FUNC(devid), iommu_idx,
                          IOMMU_FLAG_STR[flag & IOMMU_RW], iotlb.iova,
                          iotlb.translated_addr);

    riscv_iommu_ctx_put(as->iommu, ref);

    ಹಿಂತಿರುಗಿ iotlb;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ riscv_iommu_ಸ್ಮರಣೆ_region_notify(
    IOMMUMemoryRegion *iommu_mr, IOMMUNotifierFlag old,
    IOMMUNotifierFlag new, Error **errp)
{
    RISCVIOMMUSpace *as = container_ದ(iommu_mr, RISCVIOMMUSpace, iova_mr);

    ಆದರೆ (old == IOMMU_NOTIFIER_NONE) {
        as->notifier = true;
        trace_riscv_iommu_notifier_ಕೂಡಿಸು(iommu_mr->ಪೋಷಕ_obj.name);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (new == IOMMU_NOTIFIER_NONE) {
        as->notifier = false;
        trace_riscv_iommu_notifier_del(iommu_mr->ಪೋಷಕ_obj.name);
    }

    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool pci_is_iommu(PCIDevice *pdev)
{
    ಹಿಂತಿರುಗಿ pci_get_ಶಬ್ದ(pdev->config + PCI_CLASS_DEVICE) == 0x0806;
}

ಸ್ಥಿರ ವಿಳಾಸ_Space *riscv_iommu_ಕಂಡುಹಿಡಿ_as(PCIBus *bus, ಶೂನ್ಯ *opaque, ಪೂರ್ಣಾಂಕ devfn)
{
    RISCVIOMMUState *s = (RISCVIOMMUState *) opaque;
    PCIDevice *pdev = pci_ಕಂಡುಹಿಡಿ_device(bus, pci_bus_ಅಂಕೆ(bus), devfn);
    ವಿಳಾಸ_Space *as = ಖಾಲಿ;

    ಆದರೆ (pdev && pci_is_iommu(pdev)) {
        ಹಿಂತಿರುಗಿ s->ಗುರಿ_as;
    }

    /* Find first registered IOMMU device */
    ಆಗ (s->iommus.le_prev) {
        s = *(s->iommus.le_prev);
    }

    /* Find first matching IOMMU */
    ಆಗ (s != ಖಾಲಿ && as == ಖಾಲಿ) {
        as = riscv_iommu_space(s, bus, devfn);
        s = s->iommus.le_ಮುಂದಿನ;
    }

    ಹಿಂತಿರುಗಿ as ? as : &ವಿಳಾಸ_space_ಸ್ಮರಣೆ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ PCIIOMMUOps riscv_iommu_ops = {
    .get_ವಿಳಾಸ_space = riscv_iommu_ಕಂಡುಹಿಡಿ_as,
};

ಶೂನ್ಯ riscv_iommu_pci_ಸಿದ್ಧತೆ_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
        Error **errp)
{
    ಆದರೆ (bus->iommu_ops &&
        bus->iommu_ops->get_ವಿಳಾಸ_space == riscv_iommu_ಕಂಡುಹಿಡಿ_as) {
        /* Allow multiple IOMMUs on the same PCIe bus, link known devices */
        RISCVIOMMUState *ಕೊನೆಯ = (RISCVIOMMUState *)bus->iommu_opaque;
        QLIST_ಸೇರಿಸು_AFTER(ಕೊನೆಯ, iommu, iommus);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!bus->iommu_ops && !bus->iommu_opaque) {
        pci_ಸಿದ್ಧತೆ_iommu(bus, &riscv_iommu_ops, iommu);
    } ಇಲ್ಲದಿದ್ದರೆ {
        error_setg(errp, "can't register secondary IOMMU for PCI bus #%d",
            pci_bus_ಅಂಕೆ(bus));
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ riscv_iommu_ಸ್ಮರಣೆ_region_ಸೂಚ್ಯಂಕ(IOMMUMemoryRegion *iommu_mr,
    MemTxAttrs attrs)
{
    ಹಿಂತಿರುಗಿ attrs.unspecified ? RISCV_IOMMU_NOPROCID : (ಪೂರ್ಣಾಂಕ)attrs.pid;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ riscv_iommu_ಸ್ಮರಣೆ_region_ಸೂಚ್ಯಂಕ_len(IOMMUMemoryRegion *iommu_mr)
{
    RISCVIOMMUSpace *as = container_ದ(iommu_mr, RISCVIOMMUSpace, iova_mr);
    ಹಿಂತಿರುಗಿ 1 << as->iommu->pid_ಬಿಟ್ಟುಗಳು;
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ಸ್ಮರಣೆ_region_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    IOMMUMemoryRegionClass *imrc = IOMMU_ಸ್ಮರಣೆ_REGION_CLASS(klass);

    imrc->translate = riscv_iommu_ಸ್ಮರಣೆ_region_translate;
    imrc->notify_flag_ಬದಲಾಯಿತು = riscv_iommu_ಸ್ಮರಣೆ_region_notify;
    imrc->attrs_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ = riscv_iommu_ಸ್ಮರಣೆ_region_ಸೂಚ್ಯಂಕ;
    imrc->ಅಂಕೆ_indexes = riscv_iommu_ಸ್ಮರಣೆ_region_ಸೂಚ್ಯಂಕ_len;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo riscv_iommu_ಸ್ಮರಣೆ_region_info = {
    .ಪೋಷಕ = TYPE_IOMMU_ಸ್ಮರಣೆ_REGION,
    .name = TYPE_RISCV_IOMMU_ಸ್ಮರಣೆ_REGION,
    .class_init = riscv_iommu_ಸ್ಮರಣೆ_region_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ riscv_iommu_ರಿಜಿಸ್ಟರ್_mr_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&riscv_iommu_ಸ್ಮರಣೆ_region_info);
    type_ರಿಜಿಸ್ಟರ್_static(&riscv_iommu_info);
}

type_init(riscv_iommu_ರಿಜಿಸ್ಟರ್_mr_types);
