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

#ضمن "qemu/osdep.h"
#ضمن "qom/object.h"
#ضمن "exec/target_page.h"
#ضمن "hw/pci/pci_bus.h"
#ضمن "hw/pci/pci_device.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "migration/vmstate.h"
#ضمن "qapi/error.h"
#ضمن "qemu/timer.h"
#ضمن "qemu/target-info.h"
#ضمن "qemu/bitops.h"

#ضمن "cpu_bits.h"
#ضمن "riscv-iommu.h"
#ضمن "riscv-iommu-bits.h"
#ضمن "riscv-iommu-hpm.h"
#ضمن "trace.h"

#تعریف_کن حد_CACHE_CTX               (1U << 7)
#تعریف_کن حد_CACHE_IOT               (1U << 20)

/* Physical page number coversions */
#تعریف_کن PPN_PHYS(ppn)                 ((ppn) << هدف_صفحه_بیتها)
#تعریف_کن PPN_پایین(phy)                 ((phy) >> هدف_صفحه_بیتها)

تعريفنوع بنية RISCVIOMMUEntry RISCVIOMMUEntry;

/* Device assigned I/O address space */
بنية RISCVIOMMUSpace {
    IOMMUMemoryRegion iova_mr;  /* IOVA memory region for attached device */
    آدرس_Space iova_as;       /* IOVA address space for attached device */
    RISCVIOMMUState *iommu;     /* Managing IOMMU device state */
    PCIBus *bus;                /* PCI bus of the requester */
    uint8_t devfn;              /* Requester identifier, AKA device_id */
    bool notifier;              /* IOMMU unmap notifier enabled */
    QLIST_مدخل(RISCVIOMMUSpace) list;
};

تعريفنوع تعداد RISCVIOMMUTrans_برچسب {
    RISCV_IOMMU_TRANS_برچسب_BY,  /* Bypass */
    RISCV_IOMMU_TRANS_برچسب_SS,  /* Single Stage */
    RISCV_IOMMU_TRANS_برچسب_VG,  /* G-stage only */
    RISCV_IOMMU_TRANS_برچسب_VN,  /* Nested translation */
} RISCVIOMMUTrans_برچسب;

/* Address translation cache entry */
بنية RISCVIOMMUEntry {
    RISCVIOMMUTrans_برچسب برچسب;     /* Translation Tag */
    uint64_t iova:44;           /* IOVA Page Number */
    uint64_t pscid:20;          /* Process Soft-Context identifier */
    uint64_t phys:44;           /* Physical Page Number */
    uint64_t gscid:16;          /* Guest Soft-Context identifier */
    uint64_t perm:2;            /* IOMMU_RW flags */
};

/* IOMMU index for transactions without process_id specified. */
#تعریف_کن RISCV_IOMMU_NOPROCID 0

ساكن uint32_t riscv_iommu_space_devid(RISCVIOMMUSpace *as)
{
    uint32_t devid = PCI_BUILD_BDF(pci_bus_رقم(as->bus), as->devfn);

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= as->iommu->bus << 8;
    أرجع devid;
}

ساكن uint8_t riscv_iommu_get_icvec_وکتور(uint32_t icvec, uint32_t vec_type)
{
    بدل (vec_type) {
    حالة RISCV_IOMMU_INTR_CQ:
        أرجع icvec & RISCV_IOMMU_ICVEC_CIV;
    حالة RISCV_IOMMU_INTR_FQ:
        أرجع (icvec & RISCV_IOMMU_ICVEC_FIV) >> 4;
    حالة RISCV_IOMMU_INTR_PM:
        أرجع (icvec & RISCV_IOMMU_ICVEC_PMIV) >> 8;
    حالة RISCV_IOMMU_INTR_PQ:
        أرجع (icvec & RISCV_IOMMU_ICVEC_PIV) >> 12;
    افتراضي:
        g_assert_not_reached();
    }
}

فراغ riscv_iommu_notify(RISCVIOMMUState *s, صحيح vec_type)
{
    uint32_t ipsr, icvec, وکتور;

    إذا (!s->notify) {
        أرجع;
    }

    icvec = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_ICVEC);
    ipsr = riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, (1 << vec_type), 0);

    إذا (!(ipsr & (1 << vec_type))) {
        وکتور = riscv_iommu_get_icvec_وکتور(icvec, vec_type);
        s->notify(s, وکتور);
        trace_riscv_iommu_notify_int_وکتور(vec_type, وکتور);
    }
}

فراغ riscv_iommu_fault(RISCVIOMMUState *s, بنية riscv_iommu_fq_رکورد *ev)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t سر = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQH) & s->fq_ماسک;
    uint32_t دم = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQT) & s->fq_ماسک;
    uint32_t بعدی = (دم + 1) & s->fq_ماسک;
    uint32_t devid = get_فیلد(ev->hdr, RISCV_IOMMU_FQ_HDR_DID);

    trace_riscv_iommu_flt(s->والد_obj.id, PCI_BUS_رقم(devid), PCI_جایگاه(devid),
                          PCI_FUNC(devid), ev->hdr, ev->iotval);

    إذا (!(ctrl & RISCV_IOMMU_FQCSR_FQON) ||
        !!(ctrl & (RISCV_IOMMU_FQCSR_FQOF | RISCV_IOMMU_FQCSR_FQMF))) {
        أرجع;
    }

    إذا (سر == بعدی) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                              RISCV_IOMMU_FQCSR_FQOF, 0);
    } وإلا {
        dma_addr_t addr = s->fq_addr + دم * حجم(*ev);
        إذا (dma_حافظه_write(s->هدف_as, addr, ev, حجم(*ev),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR,
                                  RISCV_IOMMU_FQCSR_FQMF, 0);
        } وإلا {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_FQT, بعدی);
        }
    }

    إذا (ctrl & RISCV_IOMMU_FQCSR_FIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_FQ);
    }
}

ساكن فراغ riscv_iommu_pri(RISCVIOMMUState *s,
    بنية riscv_iommu_pq_رکورد *pr)
{
    uint32_t ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t سر = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQH) & s->pq_ماسک;
    uint32_t دم = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQT) & s->pq_ماسک;
    uint32_t بعدی = (دم + 1) & s->pq_ماسک;
    uint32_t devid = get_فیلد(pr->hdr, RISCV_IOMMU_PREQ_HDR_DID);

    trace_riscv_iommu_pri(s->والد_obj.id, PCI_BUS_رقم(devid), PCI_جایگاه(devid),
                          PCI_FUNC(devid), pr->payload);

    إذا (!(ctrl & RISCV_IOMMU_PQCSR_PQON) ||
        !!(ctrl & (RISCV_IOMMU_PQCSR_PQOF | RISCV_IOMMU_PQCSR_PQMF))) {
        أرجع;
    }

    إذا (سر == بعدی) {
        riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                              RISCV_IOMMU_PQCSR_PQOF, 0);
    } وإلا {
        dma_addr_t addr = s->pq_addr + دم * حجم(*pr);
        إذا (dma_حافظه_write(s->هدف_as, addr, pr, حجم(*pr),
                             MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR,
                                  RISCV_IOMMU_PQCSR_PQMF, 0);
        } وإلا {
            riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_PQT, بعدی);
        }
    }

    إذا (ctrl & RISCV_IOMMU_PQCSR_PIE) {
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
ساكن uint64_t riscv_iommu_pext_u64(uint64_t val, uint64_t ext)
{
    uint64_t ret = 0;
    uint64_t rot = 1;

    طالما (ext) {
        إذا (ext & 1) {
            إذا (val & 1) {
                ret |= rot;
            }
            rot <<= 1;
        }
        val >>= 1;
        ext >>= 1;
    }

    أرجع ret;
}

/* Check if GPA matches MSI/MRIF pattern. */
ساكن bool riscv_iommu_msi_check(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    dma_addr_t gpa)
{
    إذا (!s->enable_msi) {
        أرجع false;
    }

    إذا (get_فیلد(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_نما) !=
        RISCV_IOMMU_DC_MSIPTP_نما_FLAT) {
        أرجع false; /* Invalid MSI/MRIF mode */
    }

    إذا ((PPN_پایین(gpa) ^ ctx->msi_addr_الگو) & ~ctx->msi_addr_ماسک) {
        أرجع false; /* GPA not in MSI range defined by AIA IMSIC rules. */
    }

    أرجع true;
}

/* Returns the NAPOT page mask, or 0 for reserved encodings. */
ساكن hwaddr riscv_iommu_napot_صفحه_ماسک(hwaddr ppn, hwaddr addr, hwaddr *بیرون)
{
    صحيح napot_بیتها = ctz64(ppn) + 1;
    hwaddr napot_ماسک, صفحه_ماسک;

    /* The spec only defines 64KB (napot_bits == 4) */
    إذا (napot_بیتها != 4) {
        أرجع 0;
    }

    napot_ماسک = (1ULL << napot_بیتها) - 1;
    صفحه_ماسک = PPN_PHYS(napot_ماسک) | (هدف_صفحه_اندازه - 1);

    *بیرون = PPN_PHYS(ppn & ~napot_ماسک) | (addr & صفحه_ماسک);

    أرجع صفحه_ماسک;
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
ساكن صحيح riscv_iommu_spa_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb)
{
    IOMMUAccessFlags trans_perm = IOMMU_NONE;
    dma_addr_t addr, پایه;
    uint64_t satp, gatp, pte;
    bool en_s, en_g;
    بنية {
        بیعلامت حرف گام;
        بیعلامت حرف سطوح;
        بیعلامت حرف ptidxbits;
        بیعلامت حرف ptesize;
    } sc[2];
    /* Translation stage phase */
    تعداد {
        S_STAGE = 0,
        G_STAGE = 1,
    } pass;
    Mem_Tx_نتیجه ret;
    bool pv = !!ctx->process_id;
    /*
     * Keep the request permission separate from iotlb->perm.  G-stage
     * walks translate S-stage PTE addresses before the real leaf is
     * reached, but permission checks and fault types must still use the
     * original request.  A successful walk leaves iotlb->perm with the
     * effective leaf permission for the translation cache.
     */
    ثابت IOMMUAccessFlags req_perm = iotlb->perm;

    satp = get_فیلد(ctx->satp, RISCV_IOMMU_ATP_نما_فیلد);
    gatp = get_فیلد(ctx->gatp, RISCV_IOMMU_ATP_نما_فیلد);

    en_s = satp != RISCV_IOMMU_DC_FSC_نما_BARE;
    en_g = gatp != RISCV_IOMMU_DC_IOHGATP_نما_BARE;

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
    إذا (!en_s && (req_perm & IOMMU_WO) &&
        riscv_iommu_msi_check(s, ctx, iotlb->iova)) {
        iotlb->هدف_as = &s->trap_as;
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_ماسک = ~هدف_صفحه_ماسک;
        أرجع 0;
    }

    /* Exit early for pass-through mode. */
    إذا (!(en_s || en_g)) {
        iotlb->translated_addr = iotlb->iova;
        iotlb->addr_ماسک = ~هدف_صفحه_ماسک;
        /* Allow R/W in pass-through mode */
        iotlb->perm = IOMMU_RW;
        أرجع 0;
    }

    /* S/G translation parameters. */
    لكل (pass = 0; pass < 2; pass++) {
        uint32_t sv_نما;

        sc[pass].گام = 0;
        إذا (pass ? (s->fctl & RISCV_IOMMU_FCTL_GXL) :
            (ctx->tc & RISCV_IOMMU_DC_TC_SXL)) {
            /* 32bit mode for GXL/SXL == 1 */
            بدل (pass ? gatp : satp) {
            حالة RISCV_IOMMU_DC_IOHGATP_نما_BARE:
                sc[pass].سطوح    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                اكسر;
            حالة RISCV_IOMMU_DC_IOHGATP_نما_SV32_X4:
                sv_نما = pass ? RISCV_IOMMU_CAP_SV32X4 : RISCV_IOMMU_CAP_SV32;
                إذا (!(s->cap & sv_نما)) {
                    أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].سطوح    = 2;
                sc[pass].ptidxbits = 10;
                sc[pass].ptesize   = 4;
                اكسر;
            افتراضي:
                أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        } وإلا {
            /* 64bit mode for GXL/SXL == 0 */
            بدل (pass ? gatp : satp) {
            حالة RISCV_IOMMU_DC_IOHGATP_نما_BARE:
                sc[pass].سطوح    = 0;
                sc[pass].ptidxbits = 0;
                sc[pass].ptesize   = 0;
                اكسر;
            حالة RISCV_IOMMU_DC_IOHGATP_نما_SV39_X4:
                sv_نما = pass ? RISCV_IOMMU_CAP_SV39X4 : RISCV_IOMMU_CAP_SV39;
                إذا (!(s->cap & sv_نما)) {
                    أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].سطوح    = 3;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                اكسر;
            حالة RISCV_IOMMU_DC_IOHGATP_نما_SV48_X4:
                sv_نما = pass ? RISCV_IOMMU_CAP_SV48X4 : RISCV_IOMMU_CAP_SV48;
                إذا (!(s->cap & sv_نما)) {
                    أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].سطوح    = 4;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                اكسر;
            حالة RISCV_IOMMU_DC_IOHGATP_نما_SV57_X4:
                sv_نما = pass ? RISCV_IOMMU_CAP_SV57X4 : RISCV_IOMMU_CAP_SV57;
                إذا (!(s->cap & sv_نما)) {
                    أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
                }
                sc[pass].سطوح    = 5;
                sc[pass].ptidxbits = 9;
                sc[pass].ptesize   = 8;
                اكسر;
            افتراضي:
                أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
            }
        }
    };

    /* S/G stages translation tables root pointers */
    gatp = PPN_PHYS(get_فیلد(ctx->gatp, RISCV_IOMMU_ATP_PPN_فیلد));
    satp = PPN_PHYS(get_فیلد(ctx->satp, RISCV_IOMMU_ATP_PPN_فیلد));
    addr = (en_s && en_g) ? satp : iotlb->iova;
    پایه = en_g ? gatp : satp;
    pass = en_g ? G_STAGE : S_STAGE;

    افعل {
        ثابت بیعلامت widened = (pass && !sc[pass].گام) ? 2 : 0;
        ثابت بیعلامت va_بیتها = widened + sc[pass].ptidxbits;
        ثابت بیعلامت va_skip = هدف_صفحه_بیتها + sc[pass].ptidxbits *
                                 (sc[pass].سطوح - 1 - sc[pass].گام);
        ثابت بیعلامت idx = (addr >> va_skip) & ((1 << va_بیتها) - 1);
        ثابت dma_addr_t pte_addr = پایه + idx * sc[pass].ptesize;
        ثابت bool ade =
            ctx->tc & (pass ? RISCV_IOMMU_DC_TC_GADE : RISCV_IOMMU_DC_TC_SADE);

        إذا (ade && !(s->cap & RISCV_IOMMU_CAP_AMO_HWAD)) {
            /* GADE/SADE are reserved bits if AMO_HWAD is cleared.  */
            أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }

        /* Address range check before first level lookup */
        إذا (!sc[pass].گام) {
            ثابت uint64_t va_len = va_skip + va_بیتها;
            ثابت uint64_t va_ماسک = (1ULL << va_len) - 1;

            إذا (pass == S_STAGE && va_len > 32) {
                uint64_t ماسک, masked_msbs;

                ماسک = MAKE_64_بیت_ماسک(0, هدف_long_بیتها() - va_len + 1);
                masked_msbs = (addr >> (va_len - 1)) & ماسک;

                إذا (masked_msbs != 0 && masked_msbs != ماسک) {
                    أرجع (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S;
                }
            } وإلا {
                إذا ((addr & va_ماسک) != addr) {
                    أرجع (req_perm & IOMMU_WO) ?
                                RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                                RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS;
                }
            }
        }


        إذا (pass == S_STAGE) {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_S_VS_WALKS);
        } وإلا {
            riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_G_WALKS);
        }

        /* Read page table entry */
        إذا (sc[pass].ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->هدف_as, pte_addr, &pte32,
                             MEMTXATTRS_UNSPECIFIED);
            pte = pte32;
        } وإلا {
            ret = ldq_le_dma(s->هدف_as, pte_addr, &pte,
                             MEMTXATTRS_UNSPECIFIED);
        }
        إذا (ret != MEMTX_OK) {
            أرجع (req_perm & IOMMU_WO) ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT
                                         : RISCV_IOMMU_FQ_CAUSE_RD_FAULT;
        }

        sc[pass].گام++;
        hwaddr ppn = pte >> PTE_PPN_لغزش;

        إذا (!(pte & PTE_V)) {
            اكسر;                /* Invalid PTE */
        } وإلا إذا (pte & PTE_RESERVED(false)) {
            اكسر;                /* Reserved PTE bits set */
        } وإلا إذا (!(pte & (PTE_R | PTE_W | PTE_X))) {
            پایه = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } وإلا إذا ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            اكسر;                /* Reserved leaf PTE flags: PTE_W */
        } وإلا إذا ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            اكسر;                /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } وإلا إذا (ppn & ((1ULL << (va_skip - هدف_صفحه_بیتها)) - 1)) {
            اكسر;                /* Misaligned PPN */
        } وإلا إذا (!(pte & PTE_U) && !pv) {
            /*
             * All accesses are assumed to be User mode unless
             * process_id is valid (pv).  In case we have a
             * non-user mode leaf PTE and !pv we need to fault.
             */
            اكسر;
        } وإلا إذا ((req_perm & IOMMU_RO) && !(pte & PTE_R)) {
            اكسر;                /* Read access check failed */
        } وإلا إذا ((req_perm & IOMMU_WO) && !(pte & PTE_W)) {
            اكسر;                /* Write access check failed */
        } وإلا إذا (!ade && !(pte & PTE_A)) {
            اكسر;                /* Access bit not set */
        } وإلا إذا ((req_perm & IOMMU_WO) && !ade && !(pte & PTE_D)) {
            اكسر;                /* Dirty bit not set */
        } وإلا إذا (pass == G_STAGE && !(pte & PTE_U)) {
            /*
             * riscv-iommu spec 1.0: "When checking the U bit in a
             * second-stage PTE, the transaction is treated as
             * not requesting supervisor privilege."
             *
             * I.e. we need to fault if this is a non-user PTE since
             * we are always in user mode at this point.
             */
            اكسر;
        } وإلا {
            /* Leaf PTE, translation completed. */
            sc[pass].گام = sc[pass].سطوح;

            إذا (pte & PTE_N) {
                hwaddr ماسک = riscv_iommu_napot_صفحه_ماسک(ppn, addr, &پایه);

                إذا (!ماسک) {
                    اكسر;
                }
                iotlb->addr_ماسک &= ماسک;
            } وإلا {
                پایه = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
                /* Update address mask based on smallest translation granularity */
                iotlb->addr_ماسک &= (1ULL << va_skip) - 1;
            }

            /* Continue with S-Stage translation? */
            إذا (pass && sc[0].گام != sc[0].سطوح) {
                pass = S_STAGE;
                addr = iotlb->iova;
                استمر;
            }

            /* Cache the effective permission, not this request's subset. */
            IOMMUAccessFlags برگ_perm = (pte & PTE_W) ?
                                         ((pte & PTE_R) ? IOMMU_RW : IOMMU_WO) :
                                         IOMMU_RO;

            trans_perm = trans_perm == IOMMU_NONE ?
                         برگ_perm : trans_perm & برگ_perm;

            /* Translation phase completed (GPA or SPA) */
            iotlb->translated_addr = پایه;

            /* Check MSI GPA address match */
            إذا (pass == S_STAGE && (req_perm & IOMMU_WO) &&
                riscv_iommu_msi_check(s, ctx, پایه)) {
                /* Trap MSI writes and return GPA address. */
                iotlb->هدف_as = &s->trap_as;
                iotlb->addr_ماسک = ~هدف_صفحه_ماسک;
                أرجع 0;
            }

            /* Continue with G-Stage translation? */
            إذا (!pass && en_g) {
                pass = G_STAGE;
                addr = پایه;
                پایه = gatp;
                sc[pass].گام = 0;
                استمر;
            }

            iotlb->perm = trans_perm;
            أرجع 0;
        }

        إذا (sc[pass].گام == sc[pass].سطوح) {
            اكسر; /* Can't find leaf PTE */
        }

        /* Continue with G-Stage translation? */
        إذا (!pass && en_g) {
            pass = G_STAGE;
            addr = پایه;
            پایه = gatp;
            sc[pass].گام = 0;
        }
    } طالما (1);

    /*
     * riscv_iommu_translate() will receive a fault and then call
     * riscv_iommu_report_fault() using iotlb->translated_addr
     * as iotval2.  Update translated_addr it with the latest
     * translated addr we have.
     */
    iotlb->translated_addr = addr;

    أرجع (req_perm & IOMMU_WO) ?
                (pass ? RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S) :
                (pass ? RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS :
                        RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S);
}

ساكن فراغ riscv_iommu_report_fault(RISCVIOMMUState *s,
                                     RISCVIOMMUContext *ctx,
                                     uint32_t fault_type, uint32_t cause,
                                     bool pv,
                                     uint64_t iotval, uint64_t iotval2)
{
    بنية riscv_iommu_fq_رکورد ev = { 0 };

    إذا (ctx->tc & RISCV_IOMMU_DC_TC_DTF) {
        بدل (cause) {
        حالة RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED:
        حالة RISCV_IOMMU_FQ_CAUSE_DDT_بارگیری_FAULT:
        حالة RISCV_IOMMU_FQ_CAUSE_DDT_INVALID:
        حالة RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED:
        حالة RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED:
        حالة RISCV_IOMMU_FQ_CAUSE_داخلی_DP_ERROR:
        حالة RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT:
            اكسر;
        افتراضي:
            /* DTF prevents reporting a fault for this given cause */
            أرجع;
        }
    }

    ev.hdr = مجموعه_فیلد(ev.hdr, RISCV_IOMMU_FQ_HDR_CAUSE, cause);
    ev.hdr = مجموعه_فیلد(ev.hdr, RISCV_IOMMU_FQ_HDR_TTYPE, fault_type);
    ev.hdr = مجموعه_فیلد(ev.hdr, RISCV_IOMMU_FQ_HDR_DID, ctx->devid);
    ev.hdr = مجموعه_فیلد(ev.hdr, RISCV_IOMMU_FQ_HDR_PV, pv);

    إذا (pv) {
        ev.hdr = مجموعه_فیلد(ev.hdr, RISCV_IOMMU_FQ_HDR_PID, ctx->process_id);
    }

    ev.iotval = iotval;
    ev.iotval2 = iotval2;

    riscv_iommu_fault(s, &ev);
}

/* Redirect MSI write for given GPA. */
ساكن Mem_Tx_نتیجه riscv_iommu_msi_write(RISCVIOMMUState *s,
    RISCVIOMMUContext *ctx, uint64_t gpa, uint64_t داده,
    بیعلامت اندازه, MemTxAttrs attrs)
{
    Mem_Tx_نتیجه res;
    dma_addr_t addr;
    uint64_t intn;
    اندازه_t جابه_جایی;
    uint32_t n190;
    uint64_t pte[2];
    صحيح fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    صحيح cause;

    /* Interrupt File Number */
    intn = riscv_iommu_pext_u64(PPN_پایین(gpa), ctx->msi_addr_ماسک);
    جابه_جایی = intn * حجم(pte);

    /* fetch MSI PTE */
    addr = PPN_PHYS(get_فیلد(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_PPN));
    إذا (addr & جابه_جایی) {
        /* Interrupt file number out of range */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_بارگیری_FAULT;
        اذهب err;
    }

    addr |= جابه_جایی;
    res = dma_حافظه_read(s->هدف_as, addr, &pte, حجم(pte),
            MEMTXATTRS_UNSPECIFIED);
    إذا (res != MEMTX_OK) {
        إذا (res == MEMTX_DECODE_ERROR) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED;
        } وإلا {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_بارگیری_FAULT;
        }
        اذهب err;
    }

    le64_مقصد_cpus(&pte[0]);
    le64_مقصد_cpus(&pte[1]);

    إذا (!(pte[0] & RISCV_IOMMU_MSI_PTE_V) || (pte[0] & RISCV_IOMMU_MSI_PTE_C)) {
        /*
         * The spec mentions that: "If msipte.C == 1, then further
         * processing to interpret the PTE is implementation
         * defined.". We'll abort with cause = 262 for this
         * case too.
         */
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_INVALID;
        اذهب err;
    }

    بدل (get_فیلد(pte[0], RISCV_IOMMU_MSI_PTE_M)) {
    حالة RISCV_IOMMU_MSI_PTE_M_BASIC:
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
        إذا (pte[0] & (GENMASK_ULL(62, 54) | GENMASK_ULL(9, 3))) {
            res = MEMTX_DECODE_ERROR;
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
            اذهب err;
        }

        /* MSI Pass-through mode */
        addr = PPN_PHYS(get_فیلد(pte[0], RISCV_IOMMU_MSI_PTE_PPN));

        trace_riscv_iommu_msi(s->والد_obj.id, PCI_BUS_رقم(ctx->devid),
                              PCI_جایگاه(ctx->devid), PCI_FUNC(ctx->devid),
                              gpa, addr);

        res = dma_حافظه_write(s->هدف_as, addr, &داده, اندازه, attrs);
        إذا (res != MEMTX_OK) {
            cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
            اذهب err;
        }

        أرجع MEMTX_OK;
    حالة RISCV_IOMMU_MSI_PTE_M_MRIF:
        /* MRIF mode, continue. */
        اكسر;
    افتراضي:
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        اذهب err;
    }

    /*
     * Report an error for interrupt identities exceeding the maximum allowed
     * for an IMSIC interrupt file (2047) or destination address is not 32-bit
     * aligned. See IOMMU Specification, Chapter 2.3. MSI page tables.
     */
    إذا ((داده > 2047) || (gpa & 3)) {
        res = MEMTX_ACCESS_ERROR;
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED;
        اذهب err;
    }

    /* MSI MRIF mode, non atomic pending bit update */

    /* MRIF pending bit address */
    addr = get_فیلد(pte[0], RISCV_IOMMU_MSI_PTE_MRIF_ADDR) << 9;
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
    addr = addr | ((داده & 0x7c0) >> 2);

    trace_riscv_iommu_msi(s->والد_obj.id, PCI_BUS_رقم(ctx->devid),
                          PCI_جایگاه(ctx->devid), PCI_FUNC(ctx->devid),
                          gpa, addr);

    /* MRIF pending bit mask */
    داده = 1ULL << (داده & 0x03f);
    res = dma_حافظه_read(s->هدف_as, addr, &intn, حجم(intn), attrs);
    إذا (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_بارگیری_FAULT;
        اذهب err;
    }

    intn = intn | داده;
    res = dma_حافظه_write(s->هدف_as, addr, &intn, حجم(intn), attrs);
    إذا (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        اذهب err;
    }

    /* Get MRIF enable bits */
    addr = addr + حجم(intn);
    res = dma_حافظه_read(s->هدف_as, addr, &intn, حجم(intn), attrs);
    إذا (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_بارگیری_FAULT;
        اذهب err;
    }

    إذا (!(intn & داده)) {
        /* notification disabled, MRIF update completed. */
        أرجع MEMTX_OK;
    }

    /* Send notification message */
    addr = PPN_PHYS(get_فیلد(pte[1], RISCV_IOMMU_MSI_MRIF_NPPN));
    n190 = get_فیلد(pte[1], RISCV_IOMMU_MSI_MRIF_NID) |
          (get_فیلد(pte[1], RISCV_IOMMU_MSI_MRIF_NID_MSB) << 10);

    res = dma_حافظه_write(s->هدف_as, addr, &n190, حجم(n190), attrs);
    إذا (res != MEMTX_OK) {
        cause = RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT;
        اذهب err;
    }

    trace_riscv_iommu_mrif_notification(s->والد_obj.id, n190, addr);

    أرجع MEMTX_OK;

err:
    riscv_iommu_report_fault(s, ctx, fault_type, cause,
                             !!ctx->process_id, 0, 0);
    أرجع res;
}

/*
 * Check device context configuration as described by the
 * riscv-iommu spec section "Device-context configuration
 * checks".
 */
ساكن bool riscv_iommu_validate_device_ctx(RISCVIOMMUState *s,
                                            RISCVIOMMUContext *ctx)
{
    uint32_t fsc_نما, msi_نما;
    uint64_t gatp;

    إذا (ctx->tc & RISCV_IOMMU_DC_TC_RESERVED) {
        أرجع false;
    }

    إذا (!(s->cap & RISCV_IOMMU_CAP_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI ||
         ctx->tc & RISCV_IOMMU_DC_TC_PRPR)) {
        أرجع false;
    }

    إذا (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS) &&
        (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA ||
         ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI)) {
        أرجع false;
    }

    إذا (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI) &&
        ctx->tc & RISCV_IOMMU_DC_TC_PRPR) {
        أرجع false;
    }

    إذا (!(s->cap & RISCV_IOMMU_CAP_T2GPA) &&
        ctx->tc & RISCV_IOMMU_DC_TC_T2GPA) {
        أرجع false;
    }

    إذا (s->cap & RISCV_IOMMU_CAP_MSI_FLAT) {
        msi_نما = get_فیلد(ctx->msiptp, RISCV_IOMMU_DC_MSIPTP_نما);

        إذا (msi_نما != RISCV_IOMMU_DC_MSIPTP_نما_OFF &&
            msi_نما != RISCV_IOMMU_DC_MSIPTP_نما_FLAT) {
            أرجع false;
        }
    }

    gatp = get_فیلد(ctx->gatp, RISCV_IOMMU_ATP_نما_فیلد);
    إذا (ctx->tc & RISCV_IOMMU_DC_TC_T2GPA &&
        gatp == RISCV_IOMMU_DC_IOHGATP_نما_BARE) {
        أرجع false;
    }

    إذا (gatp != RISCV_IOMMU_DC_IOHGATP_نما_BARE) {
        uint64_t iohgatp_ppn = get_فیلد(ctx->gatp,
                                         RISCV_IOMMU_DC_IOHGATP_PPN);

        /*
         * One of the conditions for a misconfigured DDT entry
         * according to the riscv-spec: "DC.iohgatp.MODE is not
         * Bare and the root page table (address) determined by
         * DC.iohgatp.PPN is not aligned to a 16-KiB boundary."
         */
        إذا (PPN_PHYS(iohgatp_ppn) & ((1ULL << 14) - 1)) {
            أرجع false;
        }
    }

    fsc_نما = get_فیلد(ctx->satp, RISCV_IOMMU_DC_FSC_نما);

    إذا (ctx->tc & RISCV_IOMMU_DC_TC_PDTV) {
        بدل (fsc_نما) {
        حالة RISCV_IOMMU_DC_FSC_PDTP_نما_PD8:
            إذا (!(s->cap & RISCV_IOMMU_CAP_PD8)) {
                أرجع false;
            }
            اكسر;
        حالة RISCV_IOMMU_DC_FSC_PDTP_نما_PD17:
            إذا (!(s->cap & RISCV_IOMMU_CAP_PD17)) {
                أرجع false;
            }
            اكسر;
        حالة RISCV_IOMMU_DC_FSC_PDTP_نما_PD20:
            إذا (!(s->cap & RISCV_IOMMU_CAP_PD20)) {
                أرجع false;
            }
            اكسر;
        }
    } وإلا {
        /* DC.tc.PDTV is 0 */
        إذا (ctx->tc & RISCV_IOMMU_DC_TC_DPE) {
            أرجع false;
        }

        إذا (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
            إذا (fsc_نما == RISCV_IOMMU_DC_FSC_IOSATP_نما_SV32 &&
                !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                أرجع false;
            }
        } وإلا {
            بدل (fsc_نما) {
            حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV39:
                إذا (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                    أرجع false;
                }
                اكسر;
            حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV48:
                إذا (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                    أرجع false;
                }
            اكسر;
            حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV57:
                إذا (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                    أرجع false;
                }
                اكسر;
            }
        }
    }

    /*
     * CAP_END is always zero (only one endianess). FCTL_BE is
     * always zero (little-endian accesses). Thus TC_SBE must
     * always be LE, i.e. zero.
     */
    إذا (ctx->tc & RISCV_IOMMU_DC_TC_SBE) {
        أرجع false;
    }

    أرجع true;
}

/*
 * Validate process context (PC) according to section
 * "Process-context configuration checks".
 */
ساكن bool riscv_iommu_validate_process_ctx(RISCVIOMMUState *s,
                                             RISCVIOMMUContext *ctx)
{
    uint32_t نما;

    إذا (get_فیلد(ctx->ta, RISCV_IOMMU_PC_TA_RESERVED)) {
        أرجع false;
    }

    إذا (get_فیلد(ctx->satp, RISCV_IOMMU_PC_FSC_RESERVED)) {
        أرجع false;
    }

    نما = get_فیلد(ctx->satp, RISCV_IOMMU_DC_FSC_نما);
    بدل (نما) {
    حالة RISCV_IOMMU_DC_FSC_نما_BARE:
    /* sv39 and sv32 modes have the same value (8) */
    حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV39:
    حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV48:
    حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV57:
        اكسر;
    افتراضي:
        أرجع false;
    }

    إذا (ctx->tc & RISCV_IOMMU_DC_TC_SXL) {
        إذا (نما == RISCV_IOMMU_DC_FSC_IOSATP_نما_SV32 &&
            !(s->cap & RISCV_IOMMU_CAP_SV32)) {
                أرجع false;
        }
    } وإلا {
        بدل (نما) {
        حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV39:
            إذا (!(s->cap & RISCV_IOMMU_CAP_SV39)) {
                أرجع false;
            }
            اكسر;
        حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV48:
            إذا (!(s->cap & RISCV_IOMMU_CAP_SV48)) {
                أرجع false;
            }
            اكسر;
        حالة RISCV_IOMMU_DC_FSC_IOSATP_نما_SV57:
            إذا (!(s->cap & RISCV_IOMMU_CAP_SV57)) {
                أرجع false;
            }
            اكسر;
        }
    }

    أرجع true;
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
ساكن Mem_Tx_نتیجه pdt_حافظه_read(RISCVIOMMUState *s,
                                   RISCVIOMMUContext *ctx,
                                   dma_addr_t addr,
                                   فراغ *buf, dma_addr_t len,
                                   MemTxAttrs attrs)
{
    uint64_t gatp_نما, pte;
    بنية {
        بیعلامت حرف گام;
        بیعلامت حرف سطوح;
        بیعلامت حرف ptidxbits;
        بیعلامت حرف ptesize;
    } sc;
    Mem_Tx_نتیجه ret;
    dma_addr_t پایه = addr;

    /* G stages translation mode */
    gatp_نما = get_فیلد(ctx->gatp, RISCV_IOMMU_ATP_نما_فیلد);
    إذا (gatp_نما == RISCV_IOMMU_DC_IOHGATP_نما_BARE) {
        اذهب بیرون;
    }

    /* G stages translation tables root pointer */
    پایه = PPN_PHYS(get_فیلد(ctx->gatp, RISCV_IOMMU_ATP_PPN_فیلد));

    /* Start at step 0 */
    sc.گام = 0;

    إذا (s->fctl & RISCV_IOMMU_FCTL_GXL) {
        /* 32bit mode for GXL == 1 */
        بدل (gatp_نما) {
        حالة RISCV_IOMMU_DC_IOHGATP_نما_SV32_X4:
            إذا (!(s->cap & RISCV_IOMMU_CAP_SV32X4)) {
                أرجع MEMTX_ACCESS_ERROR;
            }
            sc.سطوح    = 2;
            sc.ptidxbits = 10;
            sc.ptesize   = 4;
            اكسر;
        افتراضي:
            أرجع MEMTX_ACCESS_ERROR;
        }
    } وإلا {
        /* 64bit mode for GXL == 0 */
        بدل (gatp_نما) {
        حالة RISCV_IOMMU_DC_IOHGATP_نما_SV39_X4:
            إذا (!(s->cap & RISCV_IOMMU_CAP_SV39X4)) {
                أرجع MEMTX_ACCESS_ERROR;
            }
            sc.سطوح    = 3;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            اكسر;
        حالة RISCV_IOMMU_DC_IOHGATP_نما_SV48_X4:
            إذا (!(s->cap & RISCV_IOMMU_CAP_SV48X4)) {
                أرجع MEMTX_ACCESS_ERROR;
            }
            sc.سطوح    = 4;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            اكسر;
        حالة RISCV_IOMMU_DC_IOHGATP_نما_SV57_X4:
            إذا (!(s->cap & RISCV_IOMMU_CAP_SV57X4)) {
                أرجع MEMTX_ACCESS_ERROR;
            }
            sc.سطوح    = 5;
            sc.ptidxbits = 9;
            sc.ptesize   = 8;
            اكسر;
        افتراضي:
            أرجع MEMTX_ACCESS_ERROR;
        }
    }

    افعل {
        ثابت بیعلامت va_بیتها = (sc.گام ? 0 : 2) + sc.ptidxbits;
        ثابت بیعلامت va_skip = هدف_صفحه_بیتها + sc.ptidxbits *
                                 (sc.سطوح - 1 - sc.گام);
        ثابت بیعلامت idx = (addr >> va_skip) & ((1 << va_بیتها) - 1);
        ثابت dma_addr_t pte_addr = پایه + idx * sc.ptesize;

        /* Address range check before first level lookup */
        إذا (!sc.گام) {
            ثابت uint64_t va_ماسک = (1ULL << (va_skip + va_بیتها)) - 1;
            إذا ((addr & va_ماسک) != addr) {
                أرجع MEMTX_ACCESS_ERROR;
            }
        }

        /* Read page table entry */
        إذا (sc.ptesize == 4) {
            uint32_t pte32 = 0;
            ret = ldl_le_dma(s->هدف_as, pte_addr, &pte32, attrs);
            pte = pte32;
        } وإلا {
            ret = ldq_le_dma(s->هدف_as, pte_addr, &pte, attrs);
        }
        إذا (ret != MEMTX_OK) {
            أرجع ret;
        }

        sc.گام++;
        hwaddr ppn = pte >> PTE_PPN_لغزش;

        إذا (!(pte & PTE_V)) {
            أرجع MEMTX_ACCESS_ERROR; /* Invalid PTE */
        } وإلا إذا (!(pte & (PTE_R | PTE_W | PTE_X))) {
            پایه = PPN_PHYS(ppn); /* Inner PTE, continue walking */
        } وإلا إذا ((pte & (PTE_R | PTE_W | PTE_X)) == PTE_W) {
            أرجع MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W */
        } وإلا إذا ((pte & (PTE_R | PTE_W | PTE_X)) == (PTE_W | PTE_X)) {
            أرجع MEMTX_ACCESS_ERROR; /* Reserved leaf PTE flags: PTE_W + PTE_X */
        } وإلا إذا (ppn & ((1ULL << (va_skip - هدف_صفحه_بیتها)) - 1)) {
            أرجع MEMTX_ACCESS_ERROR; /* Misaligned PPN */
        } وإلا {
            /* Leaf PTE, translation completed. */
            إذا (pte & PTE_N) {
                إذا (!riscv_iommu_napot_صفحه_ماسک(ppn, addr, &پایه)) {
                    أرجع MEMTX_ACCESS_ERROR;
                }
            } وإلا {
                پایه = PPN_PHYS(ppn) | (addr & ((1ULL << va_skip) - 1));
            }
            اكسر;
        }

        إذا (sc.گام == sc.سطوح) {
            أرجع MEMTX_ACCESS_ERROR; /* Can't find leaf PTE */
        }
    } طالما (1);

بیرون:
    أرجع dma_حافظه_read(s->هدف_as, پایه, buf, len, attrs);
}

/*
 * RISC-V IOMMU Device Context Loopkup - Device Directory Tree Walk
 *
 * @s         : IOMMU Device State
 * @ctx       : Device Translation Context with devid and process_id set.
 * @return    : success or fault code.
 */
ساكن صحيح riscv_iommu_ctx_fetch(RISCVIOMMUState *s, RISCVIOMMUContext *ctx)
{
    ثابت uint64_t ddtp = s->ddtp;
    بیعلامت نما = get_فیلد(ddtp, RISCV_IOMMU_DDTP_نما);
    dma_addr_t addr = PPN_PHYS(get_فیلد(ddtp, RISCV_IOMMU_DDTP_PPN));
    بنية riscv_iommu_dc dc;
    /* Device Context format: 0: extended (64 bytes) | 1: base (32 bytes) */
    ثابت صحيح dc_fmt = !s->enable_msi;
    ثابت اندازه_t dc_len = حجم(dc) >> dc_fmt;
    صحيح عمق;
    uint64_t de;

    بدل (نما) {
    حالة RISCV_IOMMU_DDTP_نما_OFF:
        أرجع RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED;

    حالة RISCV_IOMMU_DDTP_نما_BARE:
        /* mock up pass-through translation context */
        ctx->gatp = مجموعه_فیلد(0, RISCV_IOMMU_ATP_نما_فیلد,
            RISCV_IOMMU_DC_IOHGATP_نما_BARE);
        ctx->satp = مجموعه_فیلد(0, RISCV_IOMMU_ATP_نما_فیلد,
            RISCV_IOMMU_DC_FSC_نما_BARE);

        ctx->tc = RISCV_IOMMU_DC_TC_V;
        إذا (s->enable_ats) {
            ctx->tc |= RISCV_IOMMU_DC_TC_EN_ATS;
        }

        ctx->ta = 0;
        ctx->msiptp = 0;
        أرجع 0;

    حالة RISCV_IOMMU_DDTP_نما_1_LVL:
        عمق = 0;
        اكسر;

    حالة RISCV_IOMMU_DDTP_نما_2_LVL:
        عمق = 1;
        اكسر;

    حالة RISCV_IOMMU_DDTP_نما_3_LVL:
        عمق = 2;
        اكسر;

    افتراضي:
        أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /*
     * Check supported device id width (in bits).
     * See IOMMU Specification, Chapter 6. Software guidelines.
     * - if extended device-context format is used:
     *   1LVL: 6, 2LVL: 15, 3LVL: 24
     * - if base device-context format is used:
     *   1LVL: 7, 2LVL: 16, 3LVL: 24
     */
    إذا (ctx->devid >= (1 << (عمق * 9 + 6 + (dc_fmt && عمق != 2)))) {
        أرجع RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
    }

    /* Device directory tree walk */
    لكل (; عمق-- > 0; ) {
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
        ثابت صحيح تقسیم = عمق * 9 + 6 + dc_fmt;
        addr |= ((ctx->devid >> تقسیم) << 3) & ~هدف_صفحه_ماسک;
        إذا (dma_حافظه_read(s->هدف_as, addr, &de, حجم(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            أرجع RISCV_IOMMU_FQ_CAUSE_DDT_بارگیری_FAULT;
        }
        le64_مقصد_cpus(&de);
        إذا (!(de & RISCV_IOMMU_DDTE_VALID)) {
            /* invalid directory entry */
            أرجع RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        إذا (de & ~(RISCV_IOMMU_DDTE_PPN | RISCV_IOMMU_DDTE_VALID)) {
            /* reserved bits set */
            أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
        }
        addr = PPN_PHYS(get_فیلد(de, RISCV_IOMMU_DDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_DD_WALK);

    /* index into device context entry page */
    addr |= (ctx->devid * dc_len) & ~هدف_صفحه_ماسک;

    memset(&dc, 0, حجم(dc));
    إذا (dma_حافظه_read(s->هدف_as, addr, &dc, dc_len,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        أرجع RISCV_IOMMU_FQ_CAUSE_DDT_بارگیری_FAULT;
    }

    /* Set translation context. */
    ctx->tc = le64_مقصد_cpu(dc.tc);
    ctx->gatp = le64_مقصد_cpu(dc.iohgatp);
    ctx->satp = le64_مقصد_cpu(dc.fsc);
    ctx->ta = le64_مقصد_cpu(dc.ta);
    ctx->msiptp = le64_مقصد_cpu(dc.msiptp);
    ctx->msi_addr_ماسک = le64_مقصد_cpu(dc.msi_addr_ماسک);
    ctx->msi_addr_الگو = le64_مقصد_cpu(dc.msi_addr_الگو);

    إذا (!(ctx->tc & RISCV_IOMMU_DC_TC_V)) {
        أرجع RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
    }

    إذا (!riscv_iommu_validate_device_ctx(s, ctx)) {
        أرجع RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED;
    }

    /* FSC field checks */
    نما = get_فیلد(ctx->satp, RISCV_IOMMU_DC_FSC_نما);
    addr = PPN_PHYS(get_فیلد(ctx->satp, RISCV_IOMMU_DC_FSC_PPN));

    إذا (!(ctx->tc & RISCV_IOMMU_DC_TC_PDTV)) {
        إذا (ctx->process_id != RISCV_IOMMU_NOPROCID) {
            /* PID is disabled */
            أرجع RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
        }
        إذا (نما > RISCV_IOMMU_DC_FSC_IOSATP_نما_SV57) {
            /* Invalid translation mode */
            أرجع RISCV_IOMMU_FQ_CAUSE_DDT_INVALID;
        }
        أرجع 0;
    }

    إذا (ctx->process_id == RISCV_IOMMU_NOPROCID) {
        إذا (!(ctx->tc & RISCV_IOMMU_DC_TC_DPE)) {
            /* No default process_id enabled, set BARE mode */
            ctx->satp = 0ULL;
            أرجع 0;
        } وإلا {
            /* Use default process_id #0 */
            ctx->process_id = 0;
        }
    }

    إذا (نما == RISCV_IOMMU_DC_FSC_نما_BARE) {
        /* No S-Stage translation, done. */
        أرجع 0;
    }

    /* FSC.TC.PDTV enabled */
    إذا (نما > RISCV_IOMMU_DC_FSC_PDTP_نما_PD20) {
        /* Invalid PDTP.MODE */
        أرجع RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    لكل (عمق = نما - RISCV_IOMMU_DC_FSC_PDTP_نما_PD8; عمق-- > 0; ) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

        /*
         * Select process id index bits based on process directory tree
         * level. See IOMMU Specification, 2.2. Process-Directory-Table.
         */
        ثابت صحيح تقسیم = عمق * 9 + 8;
        addr |= ((ctx->process_id >> تقسیم) << 3) & ~هدف_صفحه_ماسک;
        إذا (pdt_حافظه_read(s, ctx, addr, &de, حجم(de),
                            MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
            أرجع RISCV_IOMMU_FQ_CAUSE_PDT_بارگیری_FAULT;
        }
        le64_مقصد_cpus(&de);
        إذا (!(de & RISCV_IOMMU_PDTE_VALID)) {
            أرجع RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
        }
        addr = PPN_PHYS(get_فیلد(de, RISCV_IOMMU_PDTE_PPN));
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_PD_WALK);

    /* Leaf entry in PDT */
    addr |= (ctx->process_id << 4) & ~هدف_صفحه_ماسک;
    إذا (pdt_حافظه_read(s, ctx, addr, &dc.ta, حجم(uint64_t) * 2,
                        MEMTXATTRS_UNSPECIFIED) != MEMTX_OK) {
        أرجع RISCV_IOMMU_FQ_CAUSE_PDT_بارگیری_FAULT;
    }

    /* Use FSC and TA from process directory entry. */
    ctx->ta = le64_مقصد_cpu(dc.ta);
    ctx->satp = le64_مقصد_cpu(dc.fsc);

    إذا (!(ctx->ta & RISCV_IOMMU_PC_TA_V)) {
        أرجع RISCV_IOMMU_FQ_CAUSE_PDT_INVALID;
    }

    إذا (!riscv_iommu_validate_process_ctx(s, ctx)) {
        أرجع RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED;
    }

    أرجع 0;
}

/* Translation Context cache support */
ساكن gboolean riscv_iommu_ctx_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUContext *c1 = (RISCVIOMMUContext *) v1;
    RISCVIOMMUContext *c2 = (RISCVIOMMUContext *) v2;
    أرجع c1->devid == c2->devid &&
           c1->process_id == c2->process_id;
}

ساكن guint riscv_iommu_ctx_hash(gconstpointer v)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) v;
    /*
     * Generate simple hash of (process_id, devid)
     * assuming 24-bit wide devid.
     */
    أرجع (guint)(ctx->devid) + ((guint)(ctx->process_id) << 24);
}

ساكن فراغ riscv_iommu_ctx_inval_devid_procid(gpointer کلید, gpointer مقدار,
                                               gpointer داده)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) مقدار;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) داده;
    إذا (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid &&
        ctx->process_id == arg->process_id) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ساكن فراغ riscv_iommu_ctx_inval_devid(gpointer کلید, gpointer مقدار,
                                        gpointer داده)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) مقدار;
    RISCVIOMMUContext *arg = (RISCVIOMMUContext *) داده;
    إذا (ctx->tc & RISCV_IOMMU_DC_TC_V &&
        ctx->devid == arg->devid) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ساكن فراغ riscv_iommu_ctx_inval_all(gpointer کلید, gpointer مقدار,
                                      gpointer داده)
{
    RISCVIOMMUContext *ctx = (RISCVIOMMUContext *) مقدار;
    إذا (ctx->tc & RISCV_IOMMU_DC_TC_V) {
        ctx->tc &= ~RISCV_IOMMU_DC_TC_V;
    }
}

ساكن فراغ riscv_iommu_ctx_inval(RISCVIOMMUState *s, GHFunc func,
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
ساكن RISCVIOMMUContext *riscv_iommu_ctx(RISCVIOMMUState *s,
                                          بیعلامت devid, بیعلامت process_id,
                                          IOMMUAccessFlags perm, uint64_t iova,
                                          فراغ **ref)
{
    GHash_جدول *ctx_cache;
    RISCVIOMMUContext *ctx;
    RISCVIOMMUContext کلید = {
        .devid = devid,
        .process_id = process_id,
    };
    بیعلامت نما = get_فیلد(s->ddtp, RISCV_IOMMU_DDTP_نما);
    uint32_t fault_type;

    ctx_cache = g_hash_جدول_ref(s->ctx_cache);

    إذا (نما != RISCV_IOMMU_DDTP_نما_OFF &&
        نما != RISCV_IOMMU_DDTP_نما_BARE) {
        ctx = g_hash_جدول_lookup(ctx_cache, &کلید);

        إذا (ctx && (ctx->tc & RISCV_IOMMU_DC_TC_V)) {
            *ref = ctx_cache;
            أرجع ctx;
        }
    }

    ctx = g_new0(RISCVIOMMUContext, 1);
    ctx->devid = devid;
    ctx->process_id = process_id;

    صحيح fault = riscv_iommu_ctx_fetch(s, ctx);
    إذا (!fault) {
        إذا (نما != RISCV_IOMMU_DDTP_نما_BARE) {
            إذا (g_hash_جدول_اندازه(ctx_cache) >= حد_CACHE_CTX) {
                g_hash_جدول_unref(ctx_cache);
                ctx_cache = g_hash_جدول_new_پر(riscv_iommu_ctx_hash,
                                                  riscv_iommu_ctx_equal,
                                                  g_free, عدم);
                g_hash_جدول_ref(ctx_cache);
                g_hash_جدول_unref(qatomic_xchg(&s->ctx_cache, ctx_cache));
            }

            g_hash_جدول_جمع_کن(ctx_cache, ctx);
            *ref = ctx_cache;
        } وإلا {
            g_hash_جدول_unref(ctx_cache);
            /* Remember ctx so it can be freed */
            *ref = ctx;
        }
        أرجع ctx;
    }

    g_hash_جدول_unref(ctx_cache);
    *ref = عدم;

    /*
     * TODO: (1) do we need to distinguish other fault types
     * for ctx fetching and (2) evaluate putting the 'fault_type'
     * logic inside riscv_iommu_report_fault() - there's at
     * least one other place (end of riscv_iommu_translate())
     * that does something similar.
     */
    إذا (perm & IOMMU_RO) {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
    } وإلا {
        fault_type = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
    }

    riscv_iommu_report_fault(s, ctx, fault_type, fault,
                             !!process_id, iova, 0);

    g_free(ctx);
    أرجع عدم;
}

ساكن فراغ riscv_iommu_ctx_put(RISCVIOMMUState *s, فراغ *ref)
{
    بیعلامت نما = get_فیلد(s->ddtp, RISCV_IOMMU_DDTP_نما);

    إذا (!ref) {
        أرجع;
    }

    /* ref is pointing to ctx in Bare mode. Bare mode ctx is not cached */
    إذا (نما == RISCV_IOMMU_DDTP_نما_BARE) {
        g_free(ref);
    } وإلا {
        g_hash_جدول_unref((GHash_جدول *)ref);
    }
}

/* Find or allocate address space for a given device */
ساكن آدرس_Space *riscv_iommu_space(RISCVIOMMUState *s, PCIBus *bus,
                                            صحيح devfn)
{
    RISCVIOMMUSpace *as;

    QLIST_FOREACH(as, &s->spaces, list) {
        إذا (as->bus == bus && as->devfn == devfn) {
            اكسر;
        }
    }

    إذا (as == عدم) {
        حرف name[64];
        as = g_new0(RISCVIOMMUSpace, 1);

        as->iommu = s;
        as->bus = bus;
        as->devfn = devfn;

        snprintf(name, حجم(name), "riscv-iommu-%04x:%02x.%d-iova",
            pci_bus_رقم(bus), PCI_جایگاه(devfn), PCI_FUNC(devfn));

        /* IOVA address space, untranslated addresses */
        حافظه_region_init_iommu(&as->iova_mr, حجم(as->iova_mr),
            TYPE_RISCV_IOMMU_حافظه_REGION,
            OBJECT(s), "riscv_iommu", UINT64_MAX);
        آدرس_space_init(&as->iova_as, حافظه_REGION(&as->iova_mr), name);

        QLIST_درج_سر(&s->spaces, as, list);

        trace_riscv_iommu_new(s->والد_obj.id, pci_bus_رقم(bus),
                PCI_جایگاه(devfn), PCI_FUNC(devfn));
    }
    أرجع &as->iova_as;
}

/* Translation Object cache support */
ساكن gboolean riscv_iommu_iot_equal(gconstpointer v1, gconstpointer v2)
{
    RISCVIOMMUEntry *t1 = (RISCVIOMMUEntry *) v1;
    RISCVIOMMUEntry *t2 = (RISCVIOMMUEntry *) v2;
    أرجع t1->gscid == t2->gscid && t1->pscid == t2->pscid &&
           t1->iova == t2->iova && t1->برچسب == t2->برچسب;
}

ساكن guint riscv_iommu_iot_hash(gconstpointer v)
{
    RISCVIOMMUEntry *t = (RISCVIOMMUEntry *) v;
    أرجع (guint)t->iova;
}

/* GV: 0 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 0 AV: 0 GVMA: 1 */
ساكن
فراغ riscv_iommu_iot_inval_all(gpointer کلید, gpointer مقدار, gpointer داده)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) مقدار;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) داده;
    إذا (iot->برچسب == arg->برچسب) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 0 PSCV: 1 GVMA: 0 */
ساكن
فراغ riscv_iommu_iot_inval_pscid(gpointer کلید, gpointer مقدار, gpointer داده)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) مقدار;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) داده;
    إذا (iot->برچسب == arg->برچسب &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 0 GVMA: 0 */
ساكن
فراغ riscv_iommu_iot_inval_iova(gpointer کلید, gpointer مقدار, gpointer داده)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) مقدار;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) داده;
    إذا (iot->برچسب == arg->برچسب &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 0 AV: 1 PSCV: 1 GVMA: 0 */
ساكن فراغ riscv_iommu_iot_inval_pscid_iova(gpointer کلید, gpointer مقدار,
                                             gpointer داده)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) مقدار;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) داده;
    إذا (iot->برچسب == arg->برچسب &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 0 GVMA: 1 */
ساكن
فراغ riscv_iommu_iot_inval_gscid(gpointer کلید, gpointer مقدار, gpointer داده)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) مقدار;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) داده;
    إذا (iot->برچسب == arg->برچسب &&
        iot->gscid == arg->gscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 0 PSCV: 1 GVMA: 0 */
ساكن فراغ riscv_iommu_iot_inval_gscid_pscid(gpointer کلید, gpointer مقدار,
                                              gpointer داده)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) مقدار;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) داده;
    إذا (iot->برچسب == arg->برچسب &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 0 GVMA: 0 */
/* GV: 1 AV: 1 GVMA: 1 */
ساكن فراغ riscv_iommu_iot_inval_gscid_iova(gpointer کلید, gpointer مقدار,
                                             gpointer داده)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) مقدار;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) داده;
    إذا (iot->برچسب == arg->برچسب &&
        iot->gscid == arg->gscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* GV: 1 AV: 1 PSCV: 1 GVMA: 0 */
ساكن فراغ riscv_iommu_iot_inval_gscid_pscid_iova(gpointer کلید, gpointer مقدار,
                                                   gpointer داده)
{
    RISCVIOMMUEntry *iot = (RISCVIOMMUEntry *) مقدار;
    RISCVIOMMUEntry *arg = (RISCVIOMMUEntry *) داده;
    إذا (iot->برچسب == arg->برچسب &&
        iot->gscid == arg->gscid &&
        iot->pscid == arg->pscid &&
        iot->iova == arg->iova) {
        iot->perm = IOMMU_NONE;
    }
}

/* caller should keep ref-count for iot_cache object */
ساكن RISCVIOMMUEntry *riscv_iommu_iot_lookup(RISCVIOMMUContext *ctx,
    GHash_جدول *iot_cache, hwaddr iova, RISCVIOMMUTrans_برچسب transtag)
{
    RISCVIOMMUEntry کلید = {
        .برچسب   = transtag,
        .gscid = get_فیلد(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID),
        .pscid = get_فیلد(ctx->ta, RISCV_IOMMU_DC_TA_PSCID),
        .iova  = PPN_پایین(iova),
    };
    أرجع g_hash_جدول_lookup(iot_cache, &کلید);
}

/* caller should keep ref-count for iot_cache object */
ساكن فراغ riscv_iommu_iot_update(RISCVIOMMUState *s,
    GHash_جدول *iot_cache, RISCVIOMMUEntry *iot)
{
    إذا (!s->iot_حد) {
        أرجع;
    }

    إذا (g_hash_جدول_اندازه(s->iot_cache) >= s->iot_حد) {
        iot_cache = g_hash_جدول_new_پر(riscv_iommu_iot_hash,
                                          riscv_iommu_iot_equal,
                                          g_free, عدم);
        g_hash_جدول_unref(qatomic_xchg(&s->iot_cache, iot_cache));
    }
    g_hash_جدول_جمع_کن(iot_cache, iot);
}

ساكن فراغ riscv_iommu_iot_inval(RISCVIOMMUState *s, GHFunc func,
    uint32_t gscid, uint32_t pscid, hwaddr iova, RISCVIOMMUTrans_برچسب transtag)
{
    GHash_جدول *iot_cache;
    RISCVIOMMUEntry کلید = {
        .برچسب = transtag,
        .gscid = gscid,
        .pscid = pscid,
        .iova  = PPN_پایین(iova),
    };

    iot_cache = g_hash_جدول_ref(s->iot_cache);
    g_hash_جدول_foreach(iot_cache, func, &کلید);
    g_hash_جدول_unref(iot_cache);
}

ساكن RISCVIOMMUTrans_برچسب riscv_iommu_get_transtag(RISCVIOMMUContext *ctx)
{
    uint64_t satp = get_فیلد(ctx->satp, RISCV_IOMMU_ATP_نما_فیلد);
    uint64_t gatp = get_فیلد(ctx->gatp, RISCV_IOMMU_ATP_نما_فیلد);

    إذا (satp == RISCV_IOMMU_DC_FSC_نما_BARE) {
        أرجع (gatp == RISCV_IOMMU_DC_IOHGATP_نما_BARE) ?
            RISCV_IOMMU_TRANS_برچسب_BY : RISCV_IOMMU_TRANS_برچسب_VG;
    } وإلا {
        أرجع (gatp == RISCV_IOMMU_DC_IOHGATP_نما_BARE) ?
            RISCV_IOMMU_TRANS_برچسب_SS : RISCV_IOMMU_TRANS_برچسب_VN;
    }
}

ساكن صحيح riscv_iommu_translate(RISCVIOMMUState *s, RISCVIOMMUContext *ctx,
    IOMMUTLBEntry *iotlb, bool enable_cache)
{
    RISCVIOMMUTrans_برچسب transtag = riscv_iommu_get_transtag(ctx);
    RISCVIOMMUEntry *iot;
    IOMMUAccessFlags perm;
    bool enable_pid;
    bool enable_pri;
    GHash_جدول *iot_cache;
    صحيح fault;

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_URQ);

    iot_cache = g_hash_جدول_ref(s->iot_cache);
    enable_pri = (iotlb->perm == IOMMU_NONE) &&
                 (ctx->tc & RISCV_IOMMU_DC_TC_EN_PRI);
    enable_pid = (ctx->tc & RISCV_IOMMU_DC_TC_PDTV);

    /* Check for ATS request. */
    إذا (iotlb->perm == IOMMU_NONE) {
        riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_ATS_RQ);
        /* Check if ATS is disabled. */
        إذا (!(ctx->tc & RISCV_IOMMU_DC_TC_EN_ATS)) {
            enable_pri = false;
            fault = RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED;
            اذهب انجامشده;
        }
    }

    iot = riscv_iommu_iot_lookup(ctx, iot_cache, iotlb->iova, transtag);
    perm = iot ? iot->perm : IOMMU_NONE;
    إذا (perm != IOMMU_NONE) {
        iotlb->translated_addr = PPN_PHYS(iot->phys);
        iotlb->addr_ماسک = ~هدف_صفحه_ماسک;
        iotlb->perm = perm;
        fault = 0;
        اذهب انجامشده;
    }

    riscv_iommu_hpm_incr_ctr(s, ctx, RISCV_IOMMU_HPMEVENT_TLB_نیافتن);

    /* Translate using device directory / page table information. */
    fault = riscv_iommu_spa_fetch(s, ctx, iotlb);

    إذا (!fault && iotlb->هدف_as == &s->trap_as) {
        /* Do not cache trapped MSI translations */
        اذهب انجامشده;
    }

    /*
     * We made an implementation choice to not cache identity-mapped
     * translations, as allowed by the specification, to avoid
     * translation cache evictions for other devices sharing the
     * IOMMU hardware model.
     */
    إذا (!fault && iotlb->translated_addr != iotlb->iova && enable_cache) {
        iot = g_new0(RISCVIOMMUEntry, 1);
        iot->iova = PPN_پایین(iotlb->iova);
        iot->phys = PPN_پایین(iotlb->translated_addr);
        iot->gscid = get_فیلد(ctx->gatp, RISCV_IOMMU_DC_IOHGATP_GSCID);
        iot->pscid = get_فیلد(ctx->ta, RISCV_IOMMU_DC_TA_PSCID);
        iot->perm = iotlb->perm;
        iot->برچسب = transtag;
        riscv_iommu_iot_update(s, iot_cache, iot);
    }

انجامشده:
    g_hash_جدول_unref(iot_cache);

    إذا (enable_pri && fault) {
        بنية riscv_iommu_pq_رکورد pr = {0};
        إذا (enable_pid) {
            pr.hdr = مجموعه_فیلد(RISCV_IOMMU_PREQ_HDR_PV,
                               RISCV_IOMMU_PREQ_HDR_PID, ctx->process_id);
        }
        pr.hdr = مجموعه_فیلد(pr.hdr, RISCV_IOMMU_PREQ_HDR_DID, ctx->devid);
        pr.payload = (iotlb->iova & هدف_صفحه_ماسک) |
                     RISCV_IOMMU_PREQ_PAYLOAD_M;
        riscv_iommu_pri(s, &pr);
        أرجع fault;
    }

    إذا (fault) {
        بیعلامت ttype = RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ;

        إذا ((iotlb->perm & IOMMU_RW) == IOMMU_RW
            || iotlb->perm & IOMMU_WO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_WR;
        } وإلا إذا (iotlb->perm & IOMMU_RO) {
            ttype = RISCV_IOMMU_FQ_TTYPE_UADDR_RD;
        }

        riscv_iommu_report_fault(s, ctx, ttype, fault, enable_pid,
                                 iotlb->iova, iotlb->translated_addr);
        أرجع fault;
    }

    أرجع 0;
}

/* IOMMU Command Interface */
ساكن Mem_Tx_نتیجه riscv_iommu_iofence(RISCVIOMMUState *s, bool notify,
    uint64_t addr, uint32_t داده)
{
    /*
     * ATS processing in this implementation of the IOMMU is synchronous,
     * no need to wait for completions here.
     */
    إذا (!notify) {
        أرجع MEMTX_OK;
    }

    أرجع dma_حافظه_write(s->هدف_as, addr, &داده, حجم(داده),
        MEMTXATTRS_UNSPECIFIED);
}

ساكن فراغ riscv_iommu_ats(RISCVIOMMUState *s,
    بنية riscv_iommu_command *cmd, IOMMUNotifierFlag flag,
    IOMMUAccessFlags perm,
    فراغ (*trace_fn)(ثابت حرف *id))
{
    RISCVIOMMUSpace *as;
    IOMMUNotifier *n;
    IOMMUTLBEvent event;
    uint32_t pid;
    uint32_t devid;
    ثابت bool pv = cmd->dword0 & RISCV_IOMMU_CMD_ATS_PV;

    إذا (cmd->dword0 & RISCV_IOMMU_CMD_ATS_DSV) {
        /* Use device segment and requester id */
        devid = get_فیلد(cmd->dword0,
            RISCV_IOMMU_CMD_ATS_DSEG | RISCV_IOMMU_CMD_ATS_RID);
    } وإلا {
        devid = get_فیلد(cmd->dword0, RISCV_IOMMU_CMD_ATS_RID);
    }

    pid = get_فیلد(cmd->dword0, RISCV_IOMMU_CMD_ATS_PID);

    QLIST_FOREACH(as, &s->spaces, list) {
        إذا (riscv_iommu_space_devid(as) == devid) {
            اكسر;
        }
    }

    إذا (!as || !as->notifier) {
        أرجع;
    }

    event.type = flag;
    event.مدخل.perm = perm;
    event.مدخل.هدف_as = s->هدف_as;

    IOMMU_NOTIFIER_FOREACH(n, &as->iova_mr) {
        إذا (!pv || n->iommu_idx == pid) {
            event.مدخل.iova = n->شروع;
            event.مدخل.addr_ماسک = n->انتها - n->شروع;
            trace_fn(as->iova_mr.والد_obj.name);
            حافظه_region_notify_iommu_one(n, &event);
        }
    }
}

ساكن فراغ riscv_iommu_ats_inval(RISCVIOMMUState *s,
    بنية riscv_iommu_command *cmd)
{
    أرجع riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_DEVIOTLB_UNMAP, IOMMU_NONE,
                           trace_riscv_iommu_ats_inval);
}

ساكن فراغ riscv_iommu_ats_prgr(RISCVIOMMUState *s,
    بنية riscv_iommu_command *cmd)
{
    بیعلامت resp_کد = get_فیلد(cmd->dword1,
                                   RISCV_IOMMU_CMD_ATS_PRGR_RESP_کد);

    /* Using the access flag to carry response code information */
    IOMMUAccessFlags perm = resp_کد ? IOMMU_NONE : IOMMU_RW;
    أرجع riscv_iommu_ats(s, cmd, IOMMU_NOTIFIER_MAP, perm,
                           trace_riscv_iommu_ats_prgr);
}

ساكن فراغ riscv_iommu_process_ddtp(RISCVIOMMUState *s)
{
    uint64_t old_ddtp = s->ddtp;
    uint64_t new_ddtp = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_DDTP);
    بیعلامت new_نما = get_فیلد(new_ddtp, RISCV_IOMMU_DDTP_نما);
    بیعلامت old_نما = get_فیلد(old_ddtp, RISCV_IOMMU_DDTP_نما);
    bool ok = false;

    /*
     * Check for allowed DDTP.MODE transitions:
     * {OFF, BARE}        -> {OFF, BARE, 1LVL, 2LVL, 3LVL}
     * {1LVL, 2LVL, 3LVL} -> {OFF, BARE}
     */
    إذا (new_نما == old_نما ||
        new_نما == RISCV_IOMMU_DDTP_نما_OFF ||
        new_نما == RISCV_IOMMU_DDTP_نما_BARE) {
        ok = true;
    } وإلا إذا (new_نما == RISCV_IOMMU_DDTP_نما_1_LVL ||
               new_نما == RISCV_IOMMU_DDTP_نما_2_LVL ||
               new_نما == RISCV_IOMMU_DDTP_نما_3_LVL) {
        ok = old_نما == RISCV_IOMMU_DDTP_نما_OFF ||
             old_نما == RISCV_IOMMU_DDTP_نما_BARE;
    }

    إذا (ok) {
        /* clear reserved and busy bits, report back sanitized version */
        new_ddtp = مجموعه_فیلد(new_ddtp & RISCV_IOMMU_DDTP_PPN,
                             RISCV_IOMMU_DDTP_نما, new_نما);
    } وإلا {
        new_ddtp = old_ddtp;
    }
    s->ddtp = new_ddtp;

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_DDTP, new_ddtp);
}

/* Command function and opcode field. */
#تعریف_کن RISCV_IOMMU_CMD(func, op) (((func) << 7) | (op))

ساكن فراغ riscv_iommu_process_cq_دم(RISCVIOMMUState *s)
{
    بنية riscv_iommu_command cmd;
    Mem_Tx_نتیجه res;
    dma_addr_t addr;
    uint32_t دم, سر, ctrl;
    uint64_t cmd_opcode;
    GHFunc func;

    ctrl = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    دم = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQT) & s->cq_ماسک;
    سر = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQH) & s->cq_ماسک;

    /* Check for pending error or queue processing disabled */
    إذا (!(ctrl & RISCV_IOMMU_CQCSR_CQON) ||
        !!(ctrl & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        أرجع;
    }

    طالما (دم != سر) {
        addr = s->cq_addr  + سر * حجم(cmd);
        res = dma_حافظه_read(s->هدف_as, addr, &cmd, حجم(cmd),
                              MEMTXATTRS_UNSPECIFIED);

        إذا (res != MEMTX_OK) {
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                  RISCV_IOMMU_CQCSR_CQMF, 0);
            اذهب fault;
        }

        trace_riscv_iommu_cmd(s->والد_obj.id, cmd.dword0, cmd.dword1);

        cmd_opcode = get_فیلد(cmd.dword0,
                               RISCV_IOMMU_CMD_OPCODE | RISCV_IOMMU_CMD_FUNC);

        بدل (cmd_opcode) {
        حالة RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOFENCE_FUNC_C,
                             RISCV_IOMMU_CMD_IOFENCE_OPCODE):
            إذا (cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_RESERVED) {
                اذهب cmd_ill;
            }

            res = riscv_iommu_iofence(s,
                cmd.dword0 & RISCV_IOMMU_CMD_IOFENCE_AV, cmd.dword1 << 2,
                get_فیلد(cmd.dword0, RISCV_IOMMU_CMD_IOFENCE_داده));

            إذا (res != MEMTX_OK) {
                riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                                      RISCV_IOMMU_CQCSR_CQMF, 0);
                اذهب fault;
            }
            اكسر;

        حالة RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_فیلد(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_فیلد(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & هدف_صفحه_ماسک;

            إذا (pscv) {
                /* illegal command arguments IOTINVAL.GVMA & PSCV == 1 */
                اذهب cmd_ill;
            }

            func = riscv_iommu_iot_inval_all;

            إذا (gv) {
                func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                              riscv_iommu_iot_inval_gscid;
            }

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_برچسب_VG);

            riscv_iommu_iot_inval(
                s, func, gscid, pscid, iova, RISCV_IOMMU_TRANS_برچسب_VN);
            اكسر;
        }

        حالة RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA,
                             RISCV_IOMMU_CMD_IOTINVAL_OPCODE):
        {
            bool gv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_GV);
            bool av = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_AV);
            bool pscv = !!(cmd.dword0 & RISCV_IOMMU_CMD_IOTINVAL_PSCV);
            uint32_t gscid = get_فیلد(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_GSCID);
            uint32_t pscid = get_فیلد(cmd.dword0,
                                       RISCV_IOMMU_CMD_IOTINVAL_PSCID);
            hwaddr iova = (cmd.dword1 << 2) & هدف_صفحه_ماسک;
            RISCVIOMMUTrans_برچسب transtag;

            إذا (gv) {
                transtag = RISCV_IOMMU_TRANS_برچسب_VN;
                إذا (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_gscid_pscid_iova :
                                  riscv_iommu_iot_inval_gscid_pscid;
                } وإلا {
                    func = (av) ? riscv_iommu_iot_inval_gscid_iova :
                                  riscv_iommu_iot_inval_gscid;
                }
            } وإلا {
                transtag = RISCV_IOMMU_TRANS_برچسب_SS;
                إذا (pscv) {
                    func = (av) ? riscv_iommu_iot_inval_pscid_iova :
                                  riscv_iommu_iot_inval_pscid;
                } وإلا {
                    func = (av) ? riscv_iommu_iot_inval_iova :
                                  riscv_iommu_iot_inval_all;
                }
            }

            riscv_iommu_iot_inval(s, func, gscid, pscid, iova, transtag);
            اكسر;
        }

        حالة RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            إذا (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* invalidate all device context cache mappings */
                func = riscv_iommu_ctx_inval_all;
            } وإلا {
                /* invalidate all device context matching DID */
                func = riscv_iommu_ctx_inval_devid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_فیلد(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID), 0);
            اكسر;

        حالة RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT,
                             RISCV_IOMMU_CMD_IODIR_OPCODE):
            إذا (!(cmd.dword0 & RISCV_IOMMU_CMD_IODIR_DV)) {
                /* illegal command arguments IODIR_PDT & DV == 0 */
                اذهب cmd_ill;
            } وإلا {
                func = riscv_iommu_ctx_inval_devid_procid;
            }
            riscv_iommu_ctx_inval(s, func,
                get_فیلد(cmd.dword0, RISCV_IOMMU_CMD_IODIR_DID),
                get_فیلد(cmd.dword0, RISCV_IOMMU_CMD_IODIR_PID));
            اكسر;

        /* ATS commands */
        حالة RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_INVAL,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            إذا (!s->enable_ats) {
                اذهب cmd_ill;
            }

            riscv_iommu_ats_inval(s, &cmd);
            اكسر;

        حالة RISCV_IOMMU_CMD(RISCV_IOMMU_CMD_ATS_FUNC_PRGR,
                             RISCV_IOMMU_CMD_ATS_OPCODE):
            إذا (!s->enable_ats) {
                اذهب cmd_ill;
            }

            riscv_iommu_ats_prgr(s, &cmd);
            اكسر;

        افتراضي:
        cmd_ill:
            /* Invalid instruction, do not advance instruction index. */
            riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR,
                RISCV_IOMMU_CQCSR_CMD_ILL, 0);
            اذهب fault;
        }

        /* Advance and update head pointer after command completes. */
        سر = (سر + 1) & s->cq_ماسک;
        riscv_iommu_reg_set32(s, RISCV_IOMMU_REG_CQH, سر);
    }
    أرجع;

fault:
    إذا (ctrl & RISCV_IOMMU_CQCSR_CIE) {
        riscv_iommu_notify(s, RISCV_IOMMU_INTR_CQ);
    }
}

ساكن فراغ riscv_iommu_process_cq_control(RISCVIOMMUState *s)
{
    uint64_t پایه;
    uint32_t ctrl_مجموعه = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_مجموعه & RISCV_IOMMU_CQCSR_CQEN);
    bool فعال = !!(ctrl_مجموعه & RISCV_IOMMU_CQCSR_CQON);

    إذا (enable && !فعال) {
        پایه = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_CQB);
        s->cq_ماسک = (2ULL << get_فیلد(پایه, RISCV_IOMMU_CQB_LOG2SZ)) - 1;
        s->cq_addr = PPN_PHYS(get_فیلد(پایه, RISCV_IOMMU_CQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~s->cq_ماسک);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_CQT], 0);
        ctrl_مجموعه = RISCV_IOMMU_CQCSR_CQON;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQMF |
                   RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CMD_مقصد |
                   RISCV_IOMMU_CQCSR_FENCE_W_IP;
    } وإلا إذا (!enable && فعال) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_CQT], ~0);
        ctrl_مجموعه = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY | RISCV_IOMMU_CQCSR_CQON;
    } وإلا {
        ctrl_مجموعه = 0;
        ctrl_clr = RISCV_IOMMU_CQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_CQCSR, ctrl_مجموعه, ctrl_clr);

    /*
     * After clearing error bits (CMD_ILL, CQMF), if queue is still active,
     * re-process pending command.
     */
    ctrl_مجموعه = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);
    إذا ((ctrl_مجموعه & RISCV_IOMMU_CQCSR_CQON) &&
        !(ctrl_مجموعه & (RISCV_IOMMU_CQCSR_CMD_ILL | RISCV_IOMMU_CQCSR_CQMF))) {
        riscv_iommu_process_cq_دم(s);
    }
}

ساكن فراغ riscv_iommu_process_fq_control(RISCVIOMMUState *s)
{
    uint64_t پایه;
    uint32_t ctrl_مجموعه = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_مجموعه & RISCV_IOMMU_FQCSR_FQEN);
    bool فعال = !!(ctrl_مجموعه & RISCV_IOMMU_FQCSR_FQON);

    إذا (enable && !فعال) {
        پایه = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_FQB);
        s->fq_ماسک = (2ULL << get_فیلد(پایه, RISCV_IOMMU_FQB_LOG2SZ)) - 1;
        s->fq_addr = PPN_PHYS(get_فیلد(پایه, RISCV_IOMMU_FQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~s->fq_ماسک);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_FQT], 0);
        ctrl_مجموعه = RISCV_IOMMU_FQCSR_FQON;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQMF |
            RISCV_IOMMU_FQCSR_FQOF;
    } وإلا إذا (!enable && فعال) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_FQH], ~0);
        ctrl_مجموعه = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY | RISCV_IOMMU_FQCSR_FQON;
    } وإلا {
        ctrl_مجموعه = 0;
        ctrl_clr = RISCV_IOMMU_FQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_FQCSR, ctrl_مجموعه, ctrl_clr);
}

ساكن فراغ riscv_iommu_process_pq_control(RISCVIOMMUState *s)
{
    uint64_t پایه;
    uint32_t ctrl_مجموعه = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);
    uint32_t ctrl_clr;
    bool enable = !!(ctrl_مجموعه & RISCV_IOMMU_PQCSR_PQEN);
    bool فعال = !!(ctrl_مجموعه & RISCV_IOMMU_PQCSR_PQON);

    إذا (enable && !فعال) {
        پایه = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_PQB);
        s->pq_ماسک = (2ULL << get_فیلد(پایه, RISCV_IOMMU_PQB_LOG2SZ)) - 1;
        s->pq_addr = PPN_PHYS(get_فیلد(پایه, RISCV_IOMMU_PQB_PPN));
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~s->pq_ماسک);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQH], 0);
        stl_le_p(&s->regs[RISCV_IOMMU_REG_PQT], 0);
        ctrl_مجموعه = RISCV_IOMMU_PQCSR_PQON;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQMF |
            RISCV_IOMMU_PQCSR_PQOF;
    } وإلا إذا (!enable && فعال) {
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_PQH], ~0);
        ctrl_مجموعه = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY | RISCV_IOMMU_PQCSR_PQON;
    } وإلا {
        ctrl_مجموعه = 0;
        ctrl_clr = RISCV_IOMMU_PQCSR_BUSY;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_PQCSR, ctrl_مجموعه, ctrl_clr);
}

ساكن فراغ riscv_iommu_process_dbg(RISCVIOMMUState *s)
{
    uint64_t iova = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_IOVA);
    uint64_t ctrl = riscv_iommu_reg_get64(s, RISCV_IOMMU_REG_TR_REQ_CTL);
    بیعلامت devid = get_فیلد(ctrl, RISCV_IOMMU_TR_REQ_CTL_DID);
    بیعلامت pid = get_فیلد(ctrl, RISCV_IOMMU_TR_REQ_CTL_PID);
    IOMMUAccessFlags perm = ctrl & RISCV_IOMMU_TR_REQ_CTL_NW
                            ? IOMMU_RO : IOMMU_RW;
    RISCVIOMMUContext *ctx;
    فراغ *ref;

    إذا (!(ctrl & RISCV_IOMMU_TR_REQ_CTL_GO_BUSY)) {
        أرجع;
    }

    ctx = riscv_iommu_ctx(s, devid, pid, perm, iova, &ref);
    إذا (ctx == عدم) {
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE,
                                 RISCV_IOMMU_TR_RESPONSE_FAULT |
                                 (RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED << 10));
    } وإلا {
        IOMMUTLBEntry iotlb = {
            .iova = iova,
            .perm = perm,
            .addr_ماسک = ~0,
            .هدف_as = عدم,
        };
        صحيح fault = riscv_iommu_translate(s, ctx, &iotlb, false);
        إذا (fault) {
            iova = RISCV_IOMMU_TR_RESPONSE_FAULT | (((uint64_t) fault) << 10);
        } وإلا {
            iova = iotlb.translated_addr & ~iotlb.addr_ماسک;
            iova = مجموعه_فیلد(0, RISCV_IOMMU_TR_RESPONSE_PPN, PPN_پایین(iova));
        }
        riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_TR_RESPONSE, iova);
    }

    riscv_iommu_reg_mod64(s, RISCV_IOMMU_REG_TR_REQ_CTL, 0,
        RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    riscv_iommu_ctx_put(s, ref);
}

تعريفنوع فراغ riscv_iommu_process_fn(RISCVIOMMUState *s);

ساكن فراغ riscv_iommu_update_icvec(RISCVIOMMUState *s, uint64_t داده)
{
    uint64_t icvec = 0;

    icvec |= MIN(داده & RISCV_IOMMU_ICVEC_CIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_CIV);

    icvec |= MIN(داده & RISCV_IOMMU_ICVEC_FIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_FIV);

    icvec |= MIN(داده & RISCV_IOMMU_ICVEC_PMIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PMIV);

    icvec |= MIN(داده & RISCV_IOMMU_ICVEC_PIV,
                 s->icvec_avail_vectors & RISCV_IOMMU_ICVEC_PIV);

    trace_riscv_iommu_icvec_write(داده, icvec);

    riscv_iommu_reg_set64(s, RISCV_IOMMU_REG_ICVEC, icvec);
}

ساكن فراغ riscv_iommu_update_ipsr(RISCVIOMMUState *s, uint64_t داده)
{
    uint32_t cqcsr, fqcsr, pqcsr;
    uint32_t ipsr_مجموعه = 0;
    uint32_t ipsr_clr = 0;

    إذا (داده & RISCV_IOMMU_IPSR_CIP) {
        cqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_CQCSR);

        إذا (cqcsr & RISCV_IOMMU_CQCSR_CIE &&
            (cqcsr & RISCV_IOMMU_CQCSR_FENCE_W_IP ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_ILL ||
             cqcsr & RISCV_IOMMU_CQCSR_CMD_مقصد ||
             cqcsr & RISCV_IOMMU_CQCSR_CQMF)) {
            ipsr_مجموعه |= RISCV_IOMMU_IPSR_CIP;
        } وإلا {
            ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
        }
    } وإلا {
        ipsr_clr |= RISCV_IOMMU_IPSR_CIP;
    }

    إذا (داده & RISCV_IOMMU_IPSR_FIP) {
        fqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_FQCSR);

        إذا (fqcsr & RISCV_IOMMU_FQCSR_FIE &&
            (fqcsr & RISCV_IOMMU_FQCSR_FQOF ||
             fqcsr & RISCV_IOMMU_FQCSR_FQMF)) {
            ipsr_مجموعه |= RISCV_IOMMU_IPSR_FIP;
        } وإلا {
            ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
        }
    } وإلا {
        ipsr_clr |= RISCV_IOMMU_IPSR_FIP;
    }

    إذا (!(داده & RISCV_IOMMU_IPSR_PMIP)) {
        ipsr_clr |= RISCV_IOMMU_IPSR_PMIP;
    }

    إذا (داده & RISCV_IOMMU_IPSR_PIP) {
        pqcsr = riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_PQCSR);

        إذا (pqcsr & RISCV_IOMMU_PQCSR_PIE &&
            (pqcsr & RISCV_IOMMU_PQCSR_PQOF ||
             pqcsr & RISCV_IOMMU_PQCSR_PQMF)) {
            ipsr_مجموعه |= RISCV_IOMMU_IPSR_PIP;
        } وإلا {
            ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
        }
    } وإلا {
        ipsr_clr |= RISCV_IOMMU_IPSR_PIP;
    }

    riscv_iommu_reg_mod32(s, RISCV_IOMMU_REG_IPSR, ipsr_مجموعه, ipsr_clr);
}

ساكن فراغ riscv_iommu_process_hpm_writes(RISCVIOMMUState *s,
                                           uint32_t regb,
                                           bool prev_cy_inh)
{
    بدل (regb) {
    حالة RISCV_IOMMU_REG_IOCOUNTINH:
        riscv_iommu_process_iocntinh_cy(s, prev_cy_inh);
        اكسر;

    حالة RISCV_IOMMU_REG_IOHPMCYCLES:
    حالة RISCV_IOMMU_REG_IOHPMCYCLES + 4:
        riscv_iommu_process_hpmcycle_write(s);
        اكسر;

    حالة RISCV_IOMMU_REG_IOHPMEVT_پایه ...
        RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_رقم) + 4:
        riscv_iommu_process_hpmevt_write(s, regb & ~7);
        اكسر;
    }
}

/*
 * Write the resulting value of 'data' for the reg specified
 * by 'reg_addr', after considering read-only/read-write/write-clear
 * bits, in the pointer 'dest'.
 *
 * The result is written in little-endian.
 */
ساكن فراغ riscv_iommu_write_reg_val(RISCVIOMMUState *s,
                                      فراغ *dest, hwaddr reg_addr,
                                      صحيح اندازه, uint64_t داده)
{
    uint64_t ro = ldn_le_p(&s->regs_ro[reg_addr], اندازه);
    uint64_t wc = ldn_le_p(&s->regs_wc[reg_addr], اندازه);
    uint64_t curr_val = ldn_le_p(&s->regs[reg_addr], اندازه);

    stn_le_p(dest, اندازه, ((curr_val & ro) | (داده & ~ro)) & ~(داده & wc));
}

ساكن Mem_Tx_نتیجه riscv_iommu_mmio_write(فراغ *opaque, hwaddr addr,
                                          uint64_t داده, بیعلامت اندازه,
                                          MemTxAttrs attrs)
{
    riscv_iommu_process_fn *process_fn = عدم;
    RISCVIOMMUState *s = opaque;
    uint32_t regb = addr & ~3;
    uint32_t busy = 0;
    uint64_t val = 0;
    bool cy_inh = false;

    إذا ((addr & (اندازه - 1)) != 0) {
        /* Unsupported MMIO alignment or access size */
        أرجع MEMTX_ERROR;
    }

    إذا (addr + اندازه > RISCV_IOMMU_REG_MSI_CONFIG) {
        /* Unsupported MMIO access location. */
        أرجع MEMTX_ACCESS_ERROR;
    }

    /* Track actionable MMIO write. */
    بدل (regb) {
    حالة RISCV_IOMMU_REG_DDTP:
    حالة RISCV_IOMMU_REG_DDTP + 4:
        process_fn = riscv_iommu_process_ddtp;
        regb = RISCV_IOMMU_REG_DDTP;
        busy = RISCV_IOMMU_DDTP_BUSY;
        اكسر;

    حالة RISCV_IOMMU_REG_CQT:
        process_fn = riscv_iommu_process_cq_دم;
        اكسر;

    حالة RISCV_IOMMU_REG_CQCSR:
        process_fn = riscv_iommu_process_cq_control;
        busy = RISCV_IOMMU_CQCSR_BUSY;
        اكسر;

    حالة RISCV_IOMMU_REG_FQCSR:
        process_fn = riscv_iommu_process_fq_control;
        busy = RISCV_IOMMU_FQCSR_BUSY;
        اكسر;

    حالة RISCV_IOMMU_REG_PQCSR:
        process_fn = riscv_iommu_process_pq_control;
        busy = RISCV_IOMMU_PQCSR_BUSY;
        اكسر;

    حالة RISCV_IOMMU_REG_ICVEC:
    حالة RISCV_IOMMU_REG_IPSR:
        /*
         * ICVEC and IPSR have special read/write procedures. We'll
         * call their respective helpers and exit.
         */
        riscv_iommu_write_reg_val(s, &val, addr, اندازه, داده);

        /*
         * 'val' is stored as LE. Switch to host endianess
         * before using it.
         */
        val = le64_مقصد_cpu(val);

        إذا (regb == RISCV_IOMMU_REG_ICVEC) {
            riscv_iommu_update_icvec(s, val);
        } وإلا {
            riscv_iommu_update_ipsr(s, val);
        }

        أرجع MEMTX_OK;

    حالة RISCV_IOMMU_REG_TR_REQ_CTL:
        process_fn = riscv_iommu_process_dbg;
        regb = RISCV_IOMMU_REG_TR_REQ_CTL;
        busy = RISCV_IOMMU_TR_REQ_CTL_GO_BUSY;
        اكسر;

    حالة RISCV_IOMMU_REG_IOCOUNTINH:
        إذا (addr != RISCV_IOMMU_REG_IOCOUNTINH) {
            اكسر;
        }
        /* Store previous value of CY bit. */
        cy_inh = !!(riscv_iommu_reg_get32(s, RISCV_IOMMU_REG_IOCOUNTINH) &
            RISCV_IOMMU_IOCOUNTINH_CY);
        اكسر;


    افتراضي:
        اكسر;
    }

    /*
     * Registers update might be not synchronized with core logic.
     * If system software updates register when relevant BUSY bit
     * is set IOMMU behavior of additional writes to the register
     * is UNSPECIFIED.
     */
    riscv_iommu_write_reg_val(s, &s->regs[addr], addr, اندازه, داده);

    /* Busy flag update, MSB 4-byte register. */
    إذا (busy) {
        uint32_t rw = ldl_le_p(&s->regs[regb]);
        stl_le_p(&s->regs[regb], rw | busy);
    }

    /* Process HPM writes and update any internal state if needed. */
    إذا (regb >= RISCV_IOMMU_REG_IOCOUNTOVF &&
        regb <= (RISCV_IOMMU_REG_IOHPMEVT(RISCV_IOMMU_IOCOUNT_رقم) + 4)) {
        riscv_iommu_process_hpm_writes(s, regb, cy_inh);
    }

    إذا (process_fn) {
        process_fn(s);
    }

    أرجع MEMTX_OK;
}

ساكن Mem_Tx_نتیجه riscv_iommu_mmio_read(فراغ *opaque, hwaddr addr,
    uint64_t *داده, بیعلامت اندازه, MemTxAttrs attrs)
{
    RISCVIOMMUState *s = opaque;
    uint64_t val = -1;
    uint8_t *ptr;

    إذا ((addr & (اندازه - 1)) != 0) {
        /* Unsupported MMIO alignment. */
        أرجع MEMTX_ERROR;
    }

    إذا (addr + اندازه > RISCV_IOMMU_REG_MSI_CONFIG) {
        أرجع MEMTX_ACCESS_ERROR;
    }

    /* Compute cycle register value. */
    إذا ((addr & ~7) == RISCV_IOMMU_REG_IOHPMCYCLES) {
        val = riscv_iommu_hpmcycle_read(s);
        ptr = (uint8_t *)&val + (addr & 7);
    } وإلا إذا ((addr & ~3) == RISCV_IOMMU_REG_IOCOUNTOVF) {
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
    } وإلا {
        ptr = &s->regs[addr];
    }

    val = ldn_le_p(ptr, اندازه);

    *داده = val;

    أرجع MEMTX_OK;
}

ساكن ثابت حافظه_Region_Ops riscv_iommu_mmio_ops = {
    .read_with_attrs = riscv_iommu_mmio_read,
    .write_with_attrs = riscv_iommu_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_اندازه = 4,
        .max_access_اندازه = 8,
        .unaligned = false,
    },
    .valid = {
        .min_access_اندازه = 4,
        .max_access_اندازه = 8,
    }
};

/*
 * Translations matching MSI pattern check are redirected to "riscv-iommu-trap"
 * memory region as untranslated address, for additional MSI/MRIF interception
 * by IOMMU interrupt remapping implementation.
 * Note: Device emulation code generating an MSI is expected to provide a valid
 * memory transaction attributes with requested_id set.
 */
ساكن Mem_Tx_نتیجه riscv_iommu_trap_write(فراغ *opaque, hwaddr addr,
    uint64_t داده, بیعلامت اندازه, MemTxAttrs attrs)
{
    RISCVIOMMUState* s = (RISCVIOMMUState *)opaque;
    RISCVIOMMUContext *ctx;
    Mem_Tx_نتیجه res;
    فراغ *ref;
    uint32_t devid = attrs.requester_id;

    إذا (attrs.unspecified) {
        أرجع MEMTX_ACCESS_ERROR;
    }

    /* FIXME: PCIe bus remapping for attached endpoints. */
    devid |= s->bus << 8;

    ctx = riscv_iommu_ctx(s, devid, 0, IOMMU_RW, addr, &ref);
    إذا (ctx == عدم) {
        res = MEMTX_ACCESS_ERROR;
    } وإلا {
        res = riscv_iommu_msi_write(s, ctx, addr, داده, اندازه, attrs);
    }
    riscv_iommu_ctx_put(s, ref);
    أرجع res;
}

ساكن Mem_Tx_نتیجه riscv_iommu_trap_read(فراغ *opaque, hwaddr addr,
    uint64_t *داده, بیعلامت اندازه, MemTxAttrs attrs)
{
    أرجع MEMTX_ACCESS_ERROR;
}

ساكن ثابت حافظه_Region_Ops riscv_iommu_trap_ops = {
    .read_with_attrs = riscv_iommu_trap_read,
    .write_with_attrs = riscv_iommu_trap_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_اندازه = 4,
        .max_access_اندازه = 8,
    },
    .valid = {
        .min_access_اندازه = 4,
        .max_access_اندازه = 8,
    }
};

فراغ riscv_iommu_مجموعه_cap_igs(RISCVIOMMUState *s, riscv_iommu_igs_نما نما)
{
    s->cap = مجموعه_فیلد(s->cap, RISCV_IOMMU_CAP_IGS, نما);
}

ساكن فراغ riscv_iommu_instance_init(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    /* Enable translation debug interface */
    s->cap = RISCV_IOMMU_CAP_DBG;

    /* TODO: method to report supported PID bits */
    s->pid_بیتها = 8; /* restricted to size of MemTxAttrs.pid */
    s->cap |= RISCV_IOMMU_CAP_PD8;

    /* register storage */
    s->regs = g_new0(uint8_t, RISCV_IOMMU_REG_اندازه);
    s->regs_ro = g_new0(uint8_t, RISCV_IOMMU_REG_اندازه);
    s->regs_wc = g_new0(uint8_t, RISCV_IOMMU_REG_اندازه);

     /* Mark all registers read-only */
    memset(s->regs_ro, 0xff, RISCV_IOMMU_REG_اندازه);

    /* Device translation context cache */
    s->ctx_cache = g_hash_جدول_new_پر(riscv_iommu_ctx_hash,
                                         riscv_iommu_ctx_equal,
                                         g_free, عدم);

    s->iot_cache = g_hash_جدول_new_پر(riscv_iommu_iot_hash,
                                         riscv_iommu_iot_equal,
                                         g_free, عدم);

    s->iommus.le_بعدی = عدم;
    s->iommus.le_prev = عدم;
    QLIST_INIT(&s->spaces);
}

ساكن فراغ riscv_iommu_instance_finalize(Object *obj)
{
    RISCVIOMMUState *s = RISCV_IOMMU(obj);

    g_free(s->regs);
    g_free(s->regs_ro);
    g_free(s->regs_wc);

    g_hash_جدول_unref(s->ctx_cache);
    g_hash_جدول_unref(s->iot_cache);
}

ساكن فراغ riscv_iommu_realize(DeviceState *dev, Error **errp)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    /* Report QEMU target physical address space limits. */
    s->cap = مجموعه_فیلد(s->cap, RISCV_IOMMU_CAP_PAS, s->pas_بیتها);

    s->cap |= s->نسخه & RISCV_IOMMU_CAP_نسخه;
    إذا (s->enable_msi) {
        s->cap |= RISCV_IOMMU_CAP_MSI_FLAT | RISCV_IOMMU_CAP_MSI_MRIF;
    }
    إذا (s->enable_ats) {
        s->cap |= RISCV_IOMMU_CAP_ATS;
    }
    إذا (s->enable_s_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32 | RISCV_IOMMU_CAP_SV39 |
                  RISCV_IOMMU_CAP_SV48 | RISCV_IOMMU_CAP_SV57;
    }
    إذا (s->enable_g_stage) {
        s->cap |= RISCV_IOMMU_CAP_SV32X4 | RISCV_IOMMU_CAP_SV39X4 |
                  RISCV_IOMMU_CAP_SV48X4 | RISCV_IOMMU_CAP_SV57X4 |
                  RISCV_IOMMU_CAP_SVRSW60T59B;
    }

    إذا (s->hpm_cntrs > 0) {
        /* Clip number of HPM counters to maximum supported (31). */
        إذا (s->hpm_cntrs > RISCV_IOMMU_IOCOUNT_رقم) {
            s->hpm_cntrs = RISCV_IOMMU_IOCOUNT_رقم;
        }
        /* Enable hardware performance monitor interface */
        s->cap |= RISCV_IOMMU_CAP_HPM;
    }

    /* Out-of-reset translation mode: OFF (DMA disabled) BARE (passthrough) */
    s->ddtp = مجموعه_فیلد(0, RISCV_IOMMU_DDTP_نما, s->enable_off ?
                        RISCV_IOMMU_DDTP_نما_OFF : RISCV_IOMMU_DDTP_نما_BARE);

    /*
     * Register complete MMIO space, including MSI/PBA registers.
     * Note, PCIDevice implementation will add overlapping MR for MSI/PBA,
     * managed directly by the PCIDevice implementation.
     */
    حافظه_region_init_io(&s->regs_mr, OBJECT(dev), &riscv_iommu_mmio_ops, s,
        "riscv-iommu-regs", RISCV_IOMMU_REG_اندازه);

    /* Set power-on register state */
    stq_le_p(&s->regs[RISCV_IOMMU_REG_CAP], s->cap);

    stq_le_p(&s->regs[RISCV_IOMMU_REG_FCTL], 0);
    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_FCTL],
             ~(RISCV_IOMMU_FCTL_GXL | RISCV_IOMMU_FCTL_WSI));

    stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_DDTP],
        ~(RISCV_IOMMU_DDTP_PPN | RISCV_IOMMU_DDTP_نما));
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
    إذا (s->cap & RISCV_IOMMU_CAP_DBG) {
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_IOVA], 0);
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_TR_REQ_CTL],
            RISCV_IOMMU_TR_REQ_CTL_GO_BUSY);
    }

    /* If HPM registers are enabled. */
    إذا (s->cap & RISCV_IOMMU_CAP_HPM) {
        /* +1 for cycle counter bit. */
        stl_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOCOUNTINH],
                 ~((2 << s->hpm_cntrs) - 1));
        stq_le_p(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCYCLES], 0);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMCTR_پایه],
               0x00, s->hpm_cntrs * 8);
        memset(&s->regs_ro[RISCV_IOMMU_REG_IOHPMEVT_پایه],
               0x00, s->hpm_cntrs * 8);
    }

    /* Memory region for downstream access, if specified. */
    إذا (s->هدف_mr) {
        s->هدف_as = g_new0(آدرس_Space, 1);
        آدرس_space_init(s->هدف_as, s->هدف_mr,
            "riscv-iommu-downstream");
    } وإلا {
        /* Fallback to global system memory. */
        s->هدف_as = &آدرس_space_حافظه;
    }

    /* Memory region for untranslated MRIF/MSI writes */
    حافظه_region_init_io(&s->trap_mr, OBJECT(dev), &riscv_iommu_trap_ops, s,
            "riscv-iommu-trap", ~0ULL);
    آدرس_space_init(&s->trap_as, &s->trap_mr, "riscv-iommu-trap-as");

    إذا (s->cap & RISCV_IOMMU_CAP_HPM) {
        s->hpm_timer =
            timer_new_ns(QEMU_CLOCK_VIRTUAL, riscv_iommu_hpm_timer_cb, s);
        s->hpm_event_ctr_map = g_hash_جدول_new(g_مستقیم_hash, g_مستقیم_equal);
    }
}

ساكن فراغ riscv_iommu_unrealize(DeviceState *dev)
{
    RISCVIOMMUState *s = RISCV_IOMMU(dev);

    إذا (s->cap & RISCV_IOMMU_CAP_HPM) {
        g_hash_جدول_unref(s->hpm_event_ctr_map);
        timer_free(s->hpm_timer);
    }
}

فراغ riscv_iommu_reset(RISCVIOMMUState *s)
{
    uint32_t reg_clr;
    صحيح ddtp_نما;

    /*
     * Clear DDTP while setting DDTP_mode back to user
     * initial setting.
     */
    ddtp_نما = s->enable_off ?
                RISCV_IOMMU_DDTP_نما_OFF : RISCV_IOMMU_DDTP_نما_BARE;
    s->ddtp = مجموعه_فیلد(0, RISCV_IOMMU_DDTP_نما, ddtp_نما);
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

ساكن ثابت Property riscv_iommu_properties[] = {
    DEFINE_PROP_UINT32("version", RISCVIOMMUState, نسخه,
        RISCV_IOMMU_SPEC_DOT_VER),
    DEFINE_PROP_UINT32("pas-bits", RISCVIOMMUState, pas_بیتها, 0),
    DEFINE_PROP_UINT32("bus", RISCVIOMMUState, bus, 0x0),
    DEFINE_PROP_UINT32("ioatc-limit", RISCVIOMMUState, iot_حد,
        حد_CACHE_IOT),
    DEFINE_PROP_BOOL("intremap", RISCVIOMMUState, enable_msi, TRUE),
    DEFINE_PROP_BOOL("ats", RISCVIOMMUState, enable_ats, TRUE),
    DEFINE_PROP_BOOL("off", RISCVIOMMUState, enable_off, TRUE),
    DEFINE_PROP_BOOL("s-stage", RISCVIOMMUState, enable_s_stage, TRUE),
    DEFINE_PROP_BOOL("g-stage", RISCVIOMMUState, enable_g_stage, TRUE),
    DEFINE_PROP_LINK("downstream-mr", RISCVIOMMUState, هدف_mr,
        TYPE_حافظه_REGION, حافظه_Region *),
    DEFINE_PROP_UINT8("hpm-counters", RISCVIOMMUState, hpm_cntrs,
                      RISCV_IOMMU_IOCOUNT_رقم),
};

ساكن فراغ riscv_iommu_class_init(ObjectClass *klass, ثابت فراغ *داده)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    /* internal device for riscv-iommu-{pci/sys}, not user-creatable */
    dc->user_creatable = false;
    dc->realize = riscv_iommu_realize;
    dc->unrealize = riscv_iommu_unrealize;
    device_class_مجموعه_props(dc, riscv_iommu_properties);
}

ساكن ثابت TypeInfo riscv_iommu_info = {
    .name = TYPE_RISCV_IOMMU,
    .والد = TYPE_DEVICE,
    .instance_اندازه = حجم(RISCVIOMMUState),
    .instance_init = riscv_iommu_instance_init,
    .instance_finalize = riscv_iommu_instance_finalize,
    .class_init = riscv_iommu_class_init,
};

ساكن ثابت حرف *IOMMU_FLAG_STR[] = {
    "NA",
    "RO",
    "WR",
    "RW",
};

/* RISC-V IOMMU Memory Region - Address Translation Space */
ساكن IOMMUTLBEntry riscv_iommu_حافظه_region_translate(
    IOMMUMemoryRegion *iommu_mr, hwaddr addr,
    IOMMUAccessFlags flag, صحيح iommu_idx)
{
    RISCVIOMMUSpace *as = container_از(iommu_mr, RISCVIOMMUSpace, iova_mr);
    RISCVIOMMUContext *ctx;
    فراغ *ref;
    IOMMUTLBEntry iotlb = {
        .iova = addr,
        .هدف_as = as->iommu->هدف_as,
        .addr_ماسک = ~0ULL,
        .perm = flag,
    };
    uint32_t devid = riscv_iommu_space_devid(as);

    ctx = riscv_iommu_ctx(as->iommu, devid, iommu_idx, flag, addr, &ref);
    إذا (ctx == عدم) {
        /* Translation disabled or invalid. */
        iotlb.addr_ماسک = 0;
        iotlb.perm = IOMMU_NONE;
    } وإلا إذا (riscv_iommu_translate(as->iommu, ctx, &iotlb, true)) {
        /* Translation disabled or fault reported. */
        iotlb.addr_ماسک = 0;
        iotlb.perm = IOMMU_NONE;
    }

    /* Trace all dma translations with original access flags. */
    trace_riscv_iommu_dma(as->iommu->والد_obj.id, PCI_BUS_رقم(devid),
                          PCI_جایگاه(devid), PCI_FUNC(devid), iommu_idx,
                          IOMMU_FLAG_STR[flag & IOMMU_RW], iotlb.iova,
                          iotlb.translated_addr);

    riscv_iommu_ctx_put(as->iommu, ref);

    أرجع iotlb;
}

ساكن صحيح riscv_iommu_حافظه_region_notify(
    IOMMUMemoryRegion *iommu_mr, IOMMUNotifierFlag old,
    IOMMUNotifierFlag new, Error **errp)
{
    RISCVIOMMUSpace *as = container_از(iommu_mr, RISCVIOMMUSpace, iova_mr);

    إذا (old == IOMMU_NOTIFIER_NONE) {
        as->notifier = true;
        trace_riscv_iommu_notifier_جمع_کن(iommu_mr->والد_obj.name);
    } وإلا إذا (new == IOMMU_NOTIFIER_NONE) {
        as->notifier = false;
        trace_riscv_iommu_notifier_del(iommu_mr->والد_obj.name);
    }

    أرجع 0;
}

ساكن مضمن bool pci_is_iommu(PCIDevice *pdev)
{
    أرجع pci_get_کلمه(pdev->config + PCI_CLASS_DEVICE) == 0x0806;
}

ساكن آدرس_Space *riscv_iommu_پیدا_کن_as(PCIBus *bus, فراغ *opaque, صحيح devfn)
{
    RISCVIOMMUState *s = (RISCVIOMMUState *) opaque;
    PCIDevice *pdev = pci_پیدا_کن_device(bus, pci_bus_رقم(bus), devfn);
    آدرس_Space *as = عدم;

    إذا (pdev && pci_is_iommu(pdev)) {
        أرجع s->هدف_as;
    }

    /* Find first registered IOMMU device */
    طالما (s->iommus.le_prev) {
        s = *(s->iommus.le_prev);
    }

    /* Find first matching IOMMU */
    طالما (s != عدم && as == عدم) {
        as = riscv_iommu_space(s, bus, devfn);
        s = s->iommus.le_بعدی;
    }

    أرجع as ? as : &آدرس_space_حافظه;
}

ساكن ثابت PCIIOMMUOps riscv_iommu_ops = {
    .get_آدرس_space = riscv_iommu_پیدا_کن_as,
};

فراغ riscv_iommu_pci_راهاندازی_iommu(RISCVIOMMUState *iommu, PCIBus *bus,
        Error **errp)
{
    إذا (bus->iommu_ops &&
        bus->iommu_ops->get_آدرس_space == riscv_iommu_پیدا_کن_as) {
        /* Allow multiple IOMMUs on the same PCIe bus, link known devices */
        RISCVIOMMUState *آخرین = (RISCVIOMMUState *)bus->iommu_opaque;
        QLIST_درج_AFTER(آخرین, iommu, iommus);
    } وإلا إذا (!bus->iommu_ops && !bus->iommu_opaque) {
        pci_راهاندازی_iommu(bus, &riscv_iommu_ops, iommu);
    } وإلا {
        error_setg(errp, "can't register secondary IOMMU for PCI bus #%d",
            pci_bus_رقم(bus));
    }
}

ساكن صحيح riscv_iommu_حافظه_region_شاخص(IOMMUMemoryRegion *iommu_mr,
    MemTxAttrs attrs)
{
    أرجع attrs.unspecified ? RISCV_IOMMU_NOPROCID : (صحيح)attrs.pid;
}

ساكن صحيح riscv_iommu_حافظه_region_شاخص_len(IOMMUMemoryRegion *iommu_mr)
{
    RISCVIOMMUSpace *as = container_از(iommu_mr, RISCVIOMMUSpace, iova_mr);
    أرجع 1 << as->iommu->pid_بیتها;
}

ساكن فراغ riscv_iommu_حافظه_region_init(ObjectClass *klass, ثابت فراغ *داده)
{
    IOMMUMemoryRegionClass *imrc = IOMMU_حافظه_REGION_CLASS(klass);

    imrc->translate = riscv_iommu_حافظه_region_translate;
    imrc->notify_flag_تغییریافته = riscv_iommu_حافظه_region_notify;
    imrc->attrs_مقصد_شاخص = riscv_iommu_حافظه_region_شاخص;
    imrc->رقم_indexes = riscv_iommu_حافظه_region_شاخص_len;
}

ساكن ثابت TypeInfo riscv_iommu_حافظه_region_info = {
    .والد = TYPE_IOMMU_حافظه_REGION,
    .name = TYPE_RISCV_IOMMU_حافظه_REGION,
    .class_init = riscv_iommu_حافظه_region_init,
};

ساكن فراغ riscv_iommu_ثبات_mr_types(فراغ)
{
    type_ثبات_static(&riscv_iommu_حافظه_region_info);
    type_ثبات_static(&riscv_iommu_info);
}

type_init(riscv_iommu_ثبات_mr_types);
