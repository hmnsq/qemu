/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright © 2022-2023 Rivos Inc.
 * Copyright © 2023 FORTH-ICS/CARV
 * Copyright © 2023 RISC-V IOMMU Task Group
 *
 * RISC-V IOMMU - Register Layout and Data Structures.
 *
 * Based on the IOMMU spec version 1.0, 3/2023
 * https://github.com/riscv-non-isa/riscv-iommu
 */

#که_نه_تعریف_شوی HW_RISCV_IOMMU_بټونه_H
#تعریف_کړه HW_RISCV_IOMMU_بټونه_H

#تعریف_کړه RISCV_IOMMU_SPEC_DOT_VER 0x010

#که_نه_تعریف_شوی GENMASK_ULL
#تعریف_کړه GENMASK_ULL(h, l) (((~0ULL) >> (63 - (h) + (l))) << (l))
#د_که_پای

/*
 * struct riscv_iommu_fq_record - Fault/Event Queue Record
 * See section 3.2 for more info.
 */
بنية riscv_iommu_fq_ریکارډ {
    uint64_t hdr;
    uint64_t _reserved;
    uint64_t iotval;
    uint64_t iotval2;
};
/* Header fields */
#تعریف_کړه RISCV_IOMMU_FQ_HDR_CAUSE        GENMASK_ULL(11, 0)
#تعریف_کړه RISCV_IOMMU_FQ_HDR_PID          GENMASK_ULL(31, 12)
#تعریف_کړه RISCV_IOMMU_FQ_HDR_PV           بټ_ULL(32)
#تعریف_کړه RISCV_IOMMU_FQ_HDR_TTYPE        GENMASK_ULL(39, 34)
#تعریف_کړه RISCV_IOMMU_FQ_HDR_DID          GENMASK_ULL(63, 40)

/*
 * struct riscv_iommu_pq_record - PCIe Page Request record
 * For more infos on the PCIe Page Request queue see chapter 3.3.
 */
بنية riscv_iommu_pq_ریکارډ {
      uint64_t hdr;
      uint64_t payload;
};
/* Header fields */
#تعریف_کړه RISCV_IOMMU_PREQ_HDR_PID        GENMASK_ULL(31, 12)
#تعریف_کړه RISCV_IOMMU_PREQ_HDR_PV         بټ_ULL(32)
#تعریف_کړه RISCV_IOMMU_PREQ_HDR_PRIV       بټ_ULL(33)
#تعریف_کړه RISCV_IOMMU_PREQ_HDR_EXEC       بټ_ULL(34)
#تعریف_کړه RISCV_IOMMU_PREQ_HDR_DID        GENMASK_ULL(63, 40)

/* Payload fields */
#تعریف_کړه RISCV_IOMMU_PREQ_PAYLOAD_R      بټ_ULL(0)
#تعریف_کړه RISCV_IOMMU_PREQ_PAYLOAD_W      بټ_ULL(1)
#تعریف_کړه RISCV_IOMMU_PREQ_PAYLOAD_L      بټ_ULL(2)
#تعریف_کړه RISCV_IOMMU_PREQ_PAYLOAD_M      GENMASK_ULL(2, 0)
#تعریف_کړه RISCV_IOMMU_PREQ_PRG_شاخص      GENMASK_ULL(11, 3)
#تعریف_کړه RISCV_IOMMU_PREQ_UADDR          GENMASK_ULL(63, 12)

/* Common field positions */
#تعریف_کړه RISCV_IOMMU_PPN_ډګر           GENMASK_ULL(53, 10)
#تعریف_کړه RISCV_IOMMU_کتار_LOGSZ_ډګر   GENMASK_ULL(4, 0)
#تعریف_کړه RISCV_IOMMU_کتار_شاخص_ډګر   GENMASK_ULL(31, 0)
#تعریف_کړه RISCV_IOMMU_کتار_ENABLE        بټ(0)
#تعریف_کړه RISCV_IOMMU_کتار_INTR_ENABLE   بټ(1)
#تعریف_کړه RISCV_IOMMU_کتار_MEM_FAULT     بټ(8)
#تعریف_کړه RISCV_IOMMU_کتار_ډکېدنه      بټ(9)
#تعریف_کړه RISCV_IOMMU_کتار_فعال        بټ(16)
#تعریف_کړه RISCV_IOMMU_کتار_BUSY          بټ(17)
#تعریف_کړه RISCV_IOMMU_ATP_PPN_ډګر       GENMASK_ULL(43, 0)
#تعریف_کړه RISCV_IOMMU_ATP_ډېرپېښ_ډګر      GENMASK_ULL(63, 60)

/* 5.3 IOMMU Capabilities (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_CAP             0x0000
#تعریف_کړه RISCV_IOMMU_CAP_نسخه         GENMASK_ULL(7, 0)
#تعریف_کړه RISCV_IOMMU_CAP_SV32            بټ_ULL(8)
#تعریف_کړه RISCV_IOMMU_CAP_SV39            بټ_ULL(9)
#تعریف_کړه RISCV_IOMMU_CAP_SV48            بټ_ULL(10)
#تعریف_کړه RISCV_IOMMU_CAP_SV57            بټ_ULL(11)
#تعریف_کړه RISCV_IOMMU_CAP_SVRSW60T59B     بټ_ULL(14)
#تعریف_کړه RISCV_IOMMU_CAP_SV32X4          بټ_ULL(16)
#تعریف_کړه RISCV_IOMMU_CAP_SV39X4          بټ_ULL(17)
#تعریف_کړه RISCV_IOMMU_CAP_SV48X4          بټ_ULL(18)
#تعریف_کړه RISCV_IOMMU_CAP_SV57X4          بټ_ULL(19)
#تعریف_کړه RISCV_IOMMU_CAP_MSI_FLAT        بټ_ULL(22)
#تعریف_کړه RISCV_IOMMU_CAP_MSI_MRIF        بټ_ULL(23)
#تعریف_کړه RISCV_IOMMU_CAP_AMO_HWAD        بټ_ULL(24)
#تعریف_کړه RISCV_IOMMU_CAP_ATS             بټ_ULL(25)
#تعریف_کړه RISCV_IOMMU_CAP_T2GPA           بټ_ULL(26)
#تعریف_کړه RISCV_IOMMU_CAP_IGS             GENMASK_ULL(29, 28)
#تعریف_کړه RISCV_IOMMU_CAP_HPM             بټ_ULL(30)
#تعریف_کړه RISCV_IOMMU_CAP_DBG             بټ_ULL(31)
#تعریف_کړه RISCV_IOMMU_CAP_PAS             GENMASK_ULL(37, 32)
#تعریف_کړه RISCV_IOMMU_CAP_PD8             بټ_ULL(38)
#تعریف_کړه RISCV_IOMMU_CAP_PD17            بټ_ULL(39)
#تعریف_کړه RISCV_IOMMU_CAP_PD20            بټ_ULL(40)

تعداد riscv_iommu_igs_modes {
    RISCV_IOMMU_CAP_IGS_MSI = 0,
    RISCV_IOMMU_CAP_IGS_WSI,
    RISCV_IOMMU_CAP_IGS_BOTH
};

/* 5.4 Features control register (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_FCTL            0x0008
#تعریف_کړه RISCV_IOMMU_FCTL_BE             بټ(0)
#تعریف_کړه RISCV_IOMMU_FCTL_WSI            بټ(1)
#تعریف_کړه RISCV_IOMMU_FCTL_GXL            بټ(2)

/* 5.5 Device-directory-table pointer (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_DDTP            0x0010
#تعریف_کړه RISCV_IOMMU_DDTP_ډېرپېښ           GENMASK_ULL(3, 0)
#تعریف_کړه RISCV_IOMMU_DDTP_BUSY           بټ_ULL(4)
#تعریف_کړه RISCV_IOMMU_DDTP_PPN            RISCV_IOMMU_PPN_ډګر

تعداد riscv_iommu_ddtp_modes {
    RISCV_IOMMU_DDTP_ډېرپېښ_OFF = 0,
    RISCV_IOMMU_DDTP_ډېرپېښ_BARE = 1,
    RISCV_IOMMU_DDTP_ډېرپېښ_1_LVL = 2,
    RISCV_IOMMU_DDTP_ډېرپېښ_2_LVL = 3,
    RISCV_IOMMU_DDTP_ډېرپېښ_3_LVL = 4,
    RISCV_IOMMU_DDTP_ډېرپېښ_MAX = 4
};

/* 5.6 Command Queue Base (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_CQB             0x0018
#تعریف_کړه RISCV_IOMMU_CQB_LOG2SZ          RISCV_IOMMU_کتار_LOGSZ_ډګر
#تعریف_کړه RISCV_IOMMU_CQB_PPN             RISCV_IOMMU_PPN_ډګر

/* 5.7 Command Queue head (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_CQH             0x0020

/* 5.8 Command Queue tail (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_CQT             0x0024

/* 5.9 Fault Queue Base (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_FQB             0x0028
#تعریف_کړه RISCV_IOMMU_FQB_LOG2SZ          RISCV_IOMMU_کتار_LOGSZ_ډګر
#تعریف_کړه RISCV_IOMMU_FQB_PPN             RISCV_IOMMU_PPN_ډګر

/* 5.10 Fault Queue Head (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_FQH             0x0030

/* 5.11 Fault Queue tail (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_FQT             0x0034

/* 5.12 Page Request Queue base (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_PQB             0x0038
#تعریف_کړه RISCV_IOMMU_PQB_LOG2SZ          RISCV_IOMMU_کتار_LOGSZ_ډګر
#تعریف_کړه RISCV_IOMMU_PQB_PPN             RISCV_IOMMU_PPN_ډګر

/* 5.13 Page Request Queue head (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_PQH             0x0040

/* 5.14 Page Request Queue tail (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_PQT             0x0044

/* 5.15 Command Queue CSR (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_CQCSR           0x0048
#تعریف_کړه RISCV_IOMMU_CQCSR_CQEN          RISCV_IOMMU_کتار_ENABLE
#تعریف_کړه RISCV_IOMMU_CQCSR_CIE           RISCV_IOMMU_کتار_INTR_ENABLE
#تعریف_کړه RISCV_IOMMU_CQCSR_CQMF          RISCV_IOMMU_کتار_MEM_FAULT
#تعریف_کړه RISCV_IOMMU_CQCSR_CMD_منزل        بټ(9)
#تعریف_کړه RISCV_IOMMU_CQCSR_CMD_ILL       بټ(10)
#تعریف_کړه RISCV_IOMMU_CQCSR_FENCE_W_IP    بټ(11)
#تعریف_کړه RISCV_IOMMU_CQCSR_CQON          RISCV_IOMMU_کتار_فعال
#تعریف_کړه RISCV_IOMMU_CQCSR_BUSY          RISCV_IOMMU_کتار_BUSY

/* 5.16 Fault Queue CSR (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_FQCSR           0x004C
#تعریف_کړه RISCV_IOMMU_FQCSR_FQEN          RISCV_IOMMU_کتار_ENABLE
#تعریف_کړه RISCV_IOMMU_FQCSR_FIE           RISCV_IOMMU_کتار_INTR_ENABLE
#تعریف_کړه RISCV_IOMMU_FQCSR_FQMF          RISCV_IOMMU_کتار_MEM_FAULT
#تعریف_کړه RISCV_IOMMU_FQCSR_FQOF          RISCV_IOMMU_کتار_ډکېدنه
#تعریف_کړه RISCV_IOMMU_FQCSR_FQON          RISCV_IOMMU_کتار_فعال
#تعریف_کړه RISCV_IOMMU_FQCSR_BUSY          RISCV_IOMMU_کتار_BUSY

/* 5.17 Page Request Queue CSR (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_PQCSR           0x0050
#تعریف_کړه RISCV_IOMMU_PQCSR_PQEN          RISCV_IOMMU_کتار_ENABLE
#تعریف_کړه RISCV_IOMMU_PQCSR_PIE           RISCV_IOMMU_کتار_INTR_ENABLE
#تعریف_کړه RISCV_IOMMU_PQCSR_PQMF          RISCV_IOMMU_کتار_MEM_FAULT
#تعریف_کړه RISCV_IOMMU_PQCSR_PQOF          RISCV_IOMMU_کتار_ډکېدنه
#تعریف_کړه RISCV_IOMMU_PQCSR_PQON          RISCV_IOMMU_کتار_فعال
#تعریف_کړه RISCV_IOMMU_PQCSR_BUSY          RISCV_IOMMU_کتار_BUSY

/* 5.18 Interrupt Pending Status (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_IPSR            0x0054
#تعریف_کړه RISCV_IOMMU_IPSR_CIP            بټ(0)
#تعریف_کړه RISCV_IOMMU_IPSR_FIP            بټ(1)
#تعریف_کړه RISCV_IOMMU_IPSR_PMIP           بټ(2)
#تعریف_کړه RISCV_IOMMU_IPSR_PIP            بټ(3)

تعداد {
    RISCV_IOMMU_INTR_CQ,
    RISCV_IOMMU_INTR_FQ,
    RISCV_IOMMU_INTR_PM,
    RISCV_IOMMU_INTR_PQ,
    RISCV_IOMMU_INTR_شمېر
};

#تعریف_کړه RISCV_IOMMU_IOCOUNT_عدد         31

/* 5.19 Performance monitoring counter overflow status (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_IOCOUNTOVF      0x0058
#تعریف_کړه RISCV_IOMMU_IOCOUNTOVF_CY       بټ(0)

/* 5.20 Performance monitoring counter inhibits (32bits) */
#تعریف_کړه RISCV_IOMMU_REG_IOCOUNTINH      0x005C
#تعریف_کړه RISCV_IOMMU_IOCOUNTINH_CY       بټ(0)

/* 5.21 Performance monitoring cycles counter (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_IOHPMCYCLES     0x0060
#تعریف_کړه RISCV_IOMMU_IOHPMCYCLES_COUNTER GENMASK_ULL(62, 0)
#تعریف_کړه RISCV_IOMMU_IOHPMCYCLES_OVF     بټ_ULL(63)

/* 5.22 Performance monitoring event counters (31 * 64bits) */
#تعریف_کړه RISCV_IOMMU_REG_IOHPMCTR_بنسټ   0x0068
#تعریف_کړه RISCV_IOMMU_REG_IOHPMCTR(_n)    \
    (RISCV_IOMMU_REG_IOHPMCTR_بنسټ + (_n * 0x8))

/* 5.23 Performance monitoring event selectors (31 * 64bits) */
#تعریف_کړه RISCV_IOMMU_REG_IOHPMEVT_بنسټ   0x0160
#تعریف_کړه RISCV_IOMMU_REG_IOHPMEVT(_n)    \
    (RISCV_IOMMU_REG_IOHPMEVT_بنسټ + (_n * 0x8))
#تعریف_کړه RISCV_IOMMU_IOHPMEVT_EVENT_ID   GENMASK_ULL(14, 0)
#تعریف_کړه RISCV_IOMMU_IOHPMEVT_DMASK      بټ_ULL(15)
#تعریف_کړه RISCV_IOMMU_IOHPMEVT_PID_PSCID  GENMASK_ULL(35, 16)
#تعریف_کړه RISCV_IOMMU_IOHPMEVT_DID_GSCID  GENMASK_ULL(59, 36)
#تعریف_کړه RISCV_IOMMU_IOHPMEVT_PV_PSCV    بټ_ULL(60)
#تعریف_کړه RISCV_IOMMU_IOHPMEVT_DV_GSCV    بټ_ULL(61)
#تعریف_کړه RISCV_IOMMU_IOHPMEVT_IDT        بټ_ULL(62)
#تعریف_کړه RISCV_IOMMU_IOHPMEVT_د         بټ_ULL(63)

تعداد RISCV_IOMMU_HPMEVENT_id {
    RISCV_IOMMU_HPMEVENT_INVALID    = 0,
    RISCV_IOMMU_HPMEVENT_URQ        = 1,
    RISCV_IOMMU_HPMEVENT_TRQ        = 2,
    RISCV_IOMMU_HPMEVENT_ATS_RQ     = 3,
    RISCV_IOMMU_HPMEVENT_TLB_نه_موندنه   = 4,
    RISCV_IOMMU_HPMEVENT_DD_WALK    = 5,
    RISCV_IOMMU_HPMEVENT_PD_WALK    = 6,
    RISCV_IOMMU_HPMEVENT_S_VS_WALKS = 7,
    RISCV_IOMMU_HPMEVENT_G_WALKS    = 8,
    RISCV_IOMMU_HPMEVENT_MAX        = 9
};

/* 5.24 Translation request IOVA (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_TR_REQ_IOVA     0x0258

/* 5.25 Translation request control (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_TR_REQ_CTL      0x0260
#تعریف_کړه RISCV_IOMMU_TR_REQ_CTL_GO_BUSY  بټ_ULL(0)
#تعریف_کړه RISCV_IOMMU_TR_REQ_CTL_NW       بټ_ULL(3)
#تعریف_کړه RISCV_IOMMU_TR_REQ_CTL_PID      GENMASK_ULL(31, 12)
#تعریف_کړه RISCV_IOMMU_TR_REQ_CTL_DID      GENMASK_ULL(63, 40)

/* 5.26 Translation request response (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_TR_RESPONSE     0x0268
#تعریف_کړه RISCV_IOMMU_TR_RESPONSE_FAULT   بټ_ULL(0)
#تعریف_کړه RISCV_IOMMU_TR_RESPONSE_S       بټ_ULL(9)
#تعریف_کړه RISCV_IOMMU_TR_RESPONSE_PPN     RISCV_IOMMU_PPN_ډګر

/* 5.27 Interrupt cause to vector (64bits) */
#تعریف_کړه RISCV_IOMMU_REG_ICVEC           0x02F8
#تعریف_کړه RISCV_IOMMU_ICVEC_CIV           GENMASK_ULL(3, 0)
#تعریف_کړه RISCV_IOMMU_ICVEC_FIV           GENMASK_ULL(7, 4)
#تعریف_کړه RISCV_IOMMU_ICVEC_PMIV          GENMASK_ULL(11, 8)
#تعریف_کړه RISCV_IOMMU_ICVEC_PIV           GENMASK_ULL(15, 12)

/* 5.28 MSI Configuration table (32 * 64bits) */
#تعریف_کړه RISCV_IOMMU_REG_MSI_CONFIG      0x0300

#تعریف_کړه RISCV_IOMMU_REG_اندازه            0x1000

#تعریف_کړه RISCV_IOMMU_DDTE_VALID          بټ_ULL(0)
#تعریف_کړه RISCV_IOMMU_DDTE_PPN            RISCV_IOMMU_PPN_ډګر

/* Struct riscv_iommu_dc - Device Context - section 2.1 */
بنية riscv_iommu_dc {
      uint64_t tc;
      uint64_t iohgatp;
      uint64_t ta;
      uint64_t fsc;
      uint64_t msiptp;
      uint64_t msi_addr_ماسک;
      uint64_t msi_addr_نمونه;
      uint64_t _reserved;
};

/* Translation control fields */
#تعریف_کړه RISCV_IOMMU_DC_TC_V             بټ_ULL(0)
#تعریف_کړه RISCV_IOMMU_DC_TC_EN_ATS        بټ_ULL(1)
#تعریف_کړه RISCV_IOMMU_DC_TC_EN_PRI        بټ_ULL(2)
#تعریف_کړه RISCV_IOMMU_DC_TC_T2GPA         بټ_ULL(3)
#تعریف_کړه RISCV_IOMMU_DC_TC_DTF           بټ_ULL(4)
#تعریف_کړه RISCV_IOMMU_DC_TC_PDTV          بټ_ULL(5)
#تعریف_کړه RISCV_IOMMU_DC_TC_PRPR          بټ_ULL(6)
#تعریف_کړه RISCV_IOMMU_DC_TC_GADE          بټ_ULL(7)
#تعریف_کړه RISCV_IOMMU_DC_TC_SADE          بټ_ULL(8)
#تعریف_کړه RISCV_IOMMU_DC_TC_DPE           بټ_ULL(9)
#تعریف_کړه RISCV_IOMMU_DC_TC_SBE           بټ_ULL(10)
#تعریف_کړه RISCV_IOMMU_DC_TC_SXL           بټ_ULL(11)

/* Second-stage (aka G-stage) context fields */
#تعریف_کړه RISCV_IOMMU_DC_IOHGATP_PPN      RISCV_IOMMU_ATP_PPN_ډګر
#تعریف_کړه RISCV_IOMMU_DC_IOHGATP_GSCID    GENMASK_ULL(59, 44)
#تعریف_کړه RISCV_IOMMU_DC_IOHGATP_ډېرپېښ     RISCV_IOMMU_ATP_ډېرپېښ_ډګر

#تعریف_کړه RISCV_IOMMU_DC_TC_RESERVED      (GENMASK_ULL(23, 12) \
                                         | GENMASK_ULL(63, 32))

تعداد riscv_iommu_dc_iohgatp_modes {
    RISCV_IOMMU_DC_IOHGATP_ډېرپېښ_BARE = 0,
    RISCV_IOMMU_DC_IOHGATP_ډېرپېښ_SV32_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_ډېرپېښ_SV39_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_ډېرپېښ_SV48_X4 = 9,
    RISCV_IOMMU_DC_IOHGATP_ډېرپېښ_SV57_X4 = 10
};

/* Translation attributes fields */
#تعریف_کړه RISCV_IOMMU_DC_TA_PSCID         GENMASK_ULL(31, 12)

/* First-stage context fields */
#تعریف_کړه RISCV_IOMMU_DC_FSC_PPN          RISCV_IOMMU_ATP_PPN_ډګر
#تعریف_کړه RISCV_IOMMU_DC_FSC_ډېرپېښ         RISCV_IOMMU_ATP_ډېرپېښ_ډګر

/* Generic I/O MMU command structure - check section 3.1 */
بنية riscv_iommu_command {
    uint64_t dword0;
    uint64_t dword1;
};

#تعریف_کړه RISCV_IOMMU_CMD_OPCODE          GENMASK_ULL(6, 0)
#تعریف_کړه RISCV_IOMMU_CMD_FUNC            GENMASK_ULL(9, 7)

#تعریف_کړه RISCV_IOMMU_CMD_IOTINVAL_OPCODE         1
#تعریف_کړه RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA       0
#تعریف_کړه RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA      1
#تعریف_کړه RISCV_IOMMU_CMD_IOTINVAL_AV     بټ_ULL(10)
#تعریف_کړه RISCV_IOMMU_CMD_IOTINVAL_PSCID  GENMASK_ULL(31, 12)
#تعریف_کړه RISCV_IOMMU_CMD_IOTINVAL_PSCV   بټ_ULL(32)
#تعریف_کړه RISCV_IOMMU_CMD_IOTINVAL_GV     بټ_ULL(33)
#تعریف_کړه RISCV_IOMMU_CMD_IOTINVAL_GSCID  GENMASK_ULL(59, 44)

#تعریف_کړه RISCV_IOMMU_CMD_IOFENCE_OPCODE          2
#تعریف_کړه RISCV_IOMMU_CMD_IOFENCE_FUNC_C          0
#تعریف_کړه RISCV_IOMMU_CMD_IOFENCE_AV      بټ_ULL(10)
#تعریف_کړه RISCV_IOMMU_CMD_IOFENCE_RESERVED GENMASK_ULL(31, 14)
#تعریف_کړه RISCV_IOMMU_CMD_IOFENCE_معلومات    GENMASK_ULL(63, 32)

#تعریف_کړه RISCV_IOMMU_CMD_IODIR_OPCODE            3
#تعریف_کړه RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT    0
#تعریف_کړه RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT    1
#تعریف_کړه RISCV_IOMMU_CMD_IODIR_PID       GENMASK_ULL(31, 12)
#تعریف_کړه RISCV_IOMMU_CMD_IODIR_DV        بټ_ULL(33)
#تعریف_کړه RISCV_IOMMU_CMD_IODIR_DID       GENMASK_ULL(63, 40)

/* 3.1.4 I/O MMU PCIe ATS */
#تعریف_کړه RISCV_IOMMU_CMD_ATS_OPCODE              4
#تعریف_کړه RISCV_IOMMU_CMD_ATS_FUNC_INVAL          0
#تعریف_کړه RISCV_IOMMU_CMD_ATS_FUNC_PRGR           1
#تعریف_کړه RISCV_IOMMU_CMD_ATS_PID         GENMASK_ULL(31, 12)
#تعریف_کړه RISCV_IOMMU_CMD_ATS_PV          بټ_ULL(32)
#تعریف_کړه RISCV_IOMMU_CMD_ATS_DSV         بټ_ULL(33)
#تعریف_کړه RISCV_IOMMU_CMD_ATS_RID         GENMASK_ULL(55, 40)
#تعریف_کړه RISCV_IOMMU_CMD_ATS_DSEG        GENMASK_ULL(63, 56)
/* dword1 is the ATS payload, two different payload types for INVAL and PRGR */

/* ATS.PRGR payload */
#تعریف_کړه RISCV_IOMMU_CMD_ATS_PRGR_RESP_کوډ      GENMASK_ULL(47, 44)

تعداد riscv_iommu_dc_fsc_atp_modes {
    RISCV_IOMMU_DC_FSC_ډېرپېښ_BARE = 0,
    RISCV_IOMMU_DC_FSC_IOSATP_ډېرپېښ_SV32 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_ډېرپېښ_SV39 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_ډېرپېښ_SV48 = 9,
    RISCV_IOMMU_DC_FSC_IOSATP_ډېرپېښ_SV57 = 10,
    RISCV_IOMMU_DC_FSC_PDTP_ډېرپېښ_PD8 = 1,
    RISCV_IOMMU_DC_FSC_PDTP_ډېرپېښ_PD17 = 2,
    RISCV_IOMMU_DC_FSC_PDTP_ډېرپېښ_PD20 = 3
};

تعداد riscv_iommu_fq_causes {
    RISCV_IOMMU_FQ_CAUSE_INST_FAULT           = 1,
    RISCV_IOMMU_FQ_CAUSE_RD_ADDR_MISALIGNED   = 4,
    RISCV_IOMMU_FQ_CAUSE_RD_FAULT             = 5,
    RISCV_IOMMU_FQ_CAUSE_WR_ADDR_MISALIGNED   = 6,
    RISCV_IOMMU_FQ_CAUSE_WR_FAULT             = 7,
    RISCV_IOMMU_FQ_CAUSE_INST_FAULT_S         = 12,
    RISCV_IOMMU_FQ_CAUSE_RD_FAULT_S           = 13,
    RISCV_IOMMU_FQ_CAUSE_WR_FAULT_S           = 15,
    RISCV_IOMMU_FQ_CAUSE_INST_FAULT_VS        = 20,
    RISCV_IOMMU_FQ_CAUSE_RD_FAULT_VS          = 21,
    RISCV_IOMMU_FQ_CAUSE_WR_FAULT_VS          = 23,
    RISCV_IOMMU_FQ_CAUSE_DMA_DISABLED         = 256,
    RISCV_IOMMU_FQ_CAUSE_DDT_بارول_FAULT       = 257,
    RISCV_IOMMU_FQ_CAUSE_DDT_INVALID          = 258,
    RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED    = 259,
    RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED        = 260,
    RISCV_IOMMU_FQ_CAUSE_MSI_بارول_FAULT       = 261,
    RISCV_IOMMU_FQ_CAUSE_MSI_INVALID          = 262,
    RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED    = 263,
    RISCV_IOMMU_FQ_CAUSE_MRIF_FAULT           = 264,
    RISCV_IOMMU_FQ_CAUSE_PDT_بارول_FAULT       = 265,
    RISCV_IOMMU_FQ_CAUSE_PDT_INVALID          = 266,
    RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED    = 267,
    RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED        = 268,
    RISCV_IOMMU_FQ_CAUSE_PDT_CORRUPTED        = 269,
    RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED     = 270,
    RISCV_IOMMU_FQ_CAUSE_MRIF_CORRUIPTED      = 271,
    RISCV_IOMMU_FQ_CAUSE_دننی_DP_ERROR    = 272,
    RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT         = 273,
    RISCV_IOMMU_FQ_CAUSE_PT_CORRUPTED         = 274
};

/* MSI page table pointer */
#تعریف_کړه RISCV_IOMMU_DC_MSIPTP_PPN       RISCV_IOMMU_ATP_PPN_ډګر
#تعریف_کړه RISCV_IOMMU_DC_MSIPTP_ډېرپېښ      RISCV_IOMMU_ATP_ډېرپېښ_ډګر
#تعریف_کړه RISCV_IOMMU_DC_MSIPTP_ډېرپېښ_OFF  0
#تعریف_کړه RISCV_IOMMU_DC_MSIPTP_ډېرپېښ_FLAT 1

/* 2.2 Process Directory Table */
#تعریف_کړه RISCV_IOMMU_PDTE_VALID          بټ_ULL(0)
#تعریف_کړه RISCV_IOMMU_PDTE_PPN            RISCV_IOMMU_PPN_ډګر

/* Translation attributes fields */
#تعریف_کړه RISCV_IOMMU_PC_TA_V             بټ_ULL(0)
#تعریف_کړه RISCV_IOMMU_PC_TA_RESERVED      GENMASK_ULL(63, 32)

/* First stage context fields */
#تعریف_کړه RISCV_IOMMU_PC_FSC_PPN          RISCV_IOMMU_ATP_PPN_ډګر
#تعریف_کړه RISCV_IOMMU_PC_FSC_RESERVED     GENMASK_ULL(59, 44)

تعداد riscv_iommu_fq_ttypes {
    RISCV_IOMMU_FQ_TTYPE_NONE = 0,
    RISCV_IOMMU_FQ_TTYPE_UADDR_INST_FETCH = 1,
    RISCV_IOMMU_FQ_TTYPE_UADDR_RD = 2,
    RISCV_IOMMU_FQ_TTYPE_UADDR_WR = 3,
    RISCV_IOMMU_FQ_TTYPE_TADDR_INST_FETCH = 5,
    RISCV_IOMMU_FQ_TTYPE_TADDR_RD = 6,
    RISCV_IOMMU_FQ_TTYPE_TADDR_WR = 7,
    RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ = 8,
    RISCV_IOMMU_FW_TTYPE_PCIE_پیغام_REQ = 9,
};

/*
 * struct riscv_iommu_msi_pte - MSI Page Table Entry
 */
بنية riscv_iommu_msi_pte {
      uint64_t pte;
      uint64_t mrif_info;
};

/* Fields on pte */
#تعریف_کړه RISCV_IOMMU_MSI_PTE_V           بټ_ULL(0)
#تعریف_کړه RISCV_IOMMU_MSI_PTE_M           GENMASK_ULL(2, 1)

#تعریف_کړه RISCV_IOMMU_MSI_PTE_M_MRIF      1
#تعریف_کړه RISCV_IOMMU_MSI_PTE_M_BASIC     3

/* When M == 1 (MRIF mode) */
#تعریف_کړه RISCV_IOMMU_MSI_PTE_MRIF_ADDR   GENMASK_ULL(53, 7)
/* When M == 3 (basic mode) */
#تعریف_کړه RISCV_IOMMU_MSI_PTE_PPN         RISCV_IOMMU_PPN_ډګر
#تعریف_کړه RISCV_IOMMU_MSI_PTE_C           بټ_ULL(63)

/* Fields on mrif_info */
#تعریف_کړه RISCV_IOMMU_MSI_MRIF_NID        GENMASK_ULL(9, 0)
#تعریف_کړه RISCV_IOMMU_MSI_MRIF_NPPN       RISCV_IOMMU_PPN_ډګر
#تعریف_کړه RISCV_IOMMU_MSI_MRIF_NID_MSB    بټ_ULL(60)

#د_که_پای /* _RISCV_IOMMU_BITS_H_ */
