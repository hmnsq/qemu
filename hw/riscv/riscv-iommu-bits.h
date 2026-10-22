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

#جيڪڏهن_وصف_نه_ٿيل HW_RISCV_IOMMU_بٽ_H
#وصف_ڪريو HW_RISCV_IOMMU_بٽ_H

#وصف_ڪريو RISCV_IOMMU_SPEC_DOT_VER 0x010

#جيڪڏهن_وصف_نه_ٿيل GENMASK_ULL
#وصف_ڪريو GENMASK_ULL(h, l) (((~0ULL) >> (63 - (h) + (l))) << (l))
#جيڪڏهن_پورو

/*
 * struct riscv_iommu_fq_record - Fault/Event Queue Record
 * See section 3.2 for more info.
 */
بنية riscv_iommu_fq_رڪارڊ {
    uint64_t hdr;
    uint64_t _reserved;
    uint64_t iotval;
    uint64_t iotval2;
};
/* Header fields */
#وصف_ڪريو RISCV_IOMMU_FQ_HDR_CAUSE        GENMASK_ULL(11, 0)
#وصف_ڪريو RISCV_IOMMU_FQ_HDR_PID          GENMASK_ULL(31, 12)
#وصف_ڪريو RISCV_IOMMU_FQ_HDR_PV           ٻٽو_انگ_ULL(32)
#وصف_ڪريو RISCV_IOMMU_FQ_HDR_TTYPE        GENMASK_ULL(39, 34)
#وصف_ڪريو RISCV_IOMMU_FQ_HDR_DID          GENMASK_ULL(63, 40)

/*
 * struct riscv_iommu_pq_record - PCIe Page Request record
 * For more infos on the PCIe Page Request queue see chapter 3.3.
 */
بنية riscv_iommu_pq_رڪارڊ {
      uint64_t hdr;
      uint64_t payload;
};
/* Header fields */
#وصف_ڪريو RISCV_IOMMU_PREQ_HDR_PID        GENMASK_ULL(31, 12)
#وصف_ڪريو RISCV_IOMMU_PREQ_HDR_PV         ٻٽو_انگ_ULL(32)
#وصف_ڪريو RISCV_IOMMU_PREQ_HDR_PRIV       ٻٽو_انگ_ULL(33)
#وصف_ڪريو RISCV_IOMMU_PREQ_HDR_EXEC       ٻٽو_انگ_ULL(34)
#وصف_ڪريو RISCV_IOMMU_PREQ_HDR_DID        GENMASK_ULL(63, 40)

/* Payload fields */
#وصف_ڪريو RISCV_IOMMU_PREQ_PAYLOAD_R      ٻٽو_انگ_ULL(0)
#وصف_ڪريو RISCV_IOMMU_PREQ_PAYLOAD_W      ٻٽو_انگ_ULL(1)
#وصف_ڪريو RISCV_IOMMU_PREQ_PAYLOAD_L      ٻٽو_انگ_ULL(2)
#وصف_ڪريو RISCV_IOMMU_PREQ_PAYLOAD_M      GENMASK_ULL(2, 0)
#وصف_ڪريو RISCV_IOMMU_PREQ_PRG_اشاريو      GENMASK_ULL(11, 3)
#وصف_ڪريو RISCV_IOMMU_PREQ_UADDR          GENMASK_ULL(63, 12)

/* Common field positions */
#وصف_ڪريو RISCV_IOMMU_PPN_ميدان           GENMASK_ULL(53, 10)
#وصف_ڪريو RISCV_IOMMU_قطار_LOGSZ_ميدان   GENMASK_ULL(4, 0)
#وصف_ڪريو RISCV_IOMMU_قطار_اشاريو_ميدان   GENMASK_ULL(31, 0)
#وصف_ڪريو RISCV_IOMMU_قطار_ENABLE        ٻٽو_انگ(0)
#وصف_ڪريو RISCV_IOMMU_قطار_INTR_ENABLE   ٻٽو_انگ(1)
#وصف_ڪريو RISCV_IOMMU_قطار_MEM_FAULT     ٻٽو_انگ(8)
#وصف_ڪريو RISCV_IOMMU_قطار_حد_کان_مٿي      ٻٽو_انگ(9)
#وصف_ڪريو RISCV_IOMMU_قطار_سرگرم        ٻٽو_انگ(16)
#وصف_ڪريو RISCV_IOMMU_قطار_BUSY          ٻٽو_انگ(17)
#وصف_ڪريو RISCV_IOMMU_ATP_PPN_ميدان       GENMASK_ULL(43, 0)
#وصف_ڪريو RISCV_IOMMU_ATP_گهڻائي_ميدان      GENMASK_ULL(63, 60)

/* 5.3 IOMMU Capabilities (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_CAP             0x0000
#وصف_ڪريو RISCV_IOMMU_CAP_نسخو         GENMASK_ULL(7, 0)
#وصف_ڪريو RISCV_IOMMU_CAP_SV32            ٻٽو_انگ_ULL(8)
#وصف_ڪريو RISCV_IOMMU_CAP_SV39            ٻٽو_انگ_ULL(9)
#وصف_ڪريو RISCV_IOMMU_CAP_SV48            ٻٽو_انگ_ULL(10)
#وصف_ڪريو RISCV_IOMMU_CAP_SV57            ٻٽو_انگ_ULL(11)
#وصف_ڪريو RISCV_IOMMU_CAP_SVRSW60T59B     ٻٽو_انگ_ULL(14)
#وصف_ڪريو RISCV_IOMMU_CAP_SV32X4          ٻٽو_انگ_ULL(16)
#وصف_ڪريو RISCV_IOMMU_CAP_SV39X4          ٻٽو_انگ_ULL(17)
#وصف_ڪريو RISCV_IOMMU_CAP_SV48X4          ٻٽو_انگ_ULL(18)
#وصف_ڪريو RISCV_IOMMU_CAP_SV57X4          ٻٽو_انگ_ULL(19)
#وصف_ڪريو RISCV_IOMMU_CAP_MSI_FLAT        ٻٽو_انگ_ULL(22)
#وصف_ڪريو RISCV_IOMMU_CAP_MSI_MRIF        ٻٽو_انگ_ULL(23)
#وصف_ڪريو RISCV_IOMMU_CAP_AMO_HWAD        ٻٽو_انگ_ULL(24)
#وصف_ڪريو RISCV_IOMMU_CAP_ATS             ٻٽو_انگ_ULL(25)
#وصف_ڪريو RISCV_IOMMU_CAP_T2GPA           ٻٽو_انگ_ULL(26)
#وصف_ڪريو RISCV_IOMMU_CAP_IGS             GENMASK_ULL(29, 28)
#وصف_ڪريو RISCV_IOMMU_CAP_HPM             ٻٽو_انگ_ULL(30)
#وصف_ڪريو RISCV_IOMMU_CAP_DBG             ٻٽو_انگ_ULL(31)
#وصف_ڪريو RISCV_IOMMU_CAP_PAS             GENMASK_ULL(37, 32)
#وصف_ڪريو RISCV_IOMMU_CAP_PD8             ٻٽو_انگ_ULL(38)
#وصف_ڪريو RISCV_IOMMU_CAP_PD17            ٻٽو_انگ_ULL(39)
#وصف_ڪريو RISCV_IOMMU_CAP_PD20            ٻٽو_انگ_ULL(40)

تعداد riscv_iommu_igs_modes {
    RISCV_IOMMU_CAP_IGS_MSI = 0,
    RISCV_IOMMU_CAP_IGS_WSI,
    RISCV_IOMMU_CAP_IGS_BOTH
};

/* 5.4 Features control register (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_FCTL            0x0008
#وصف_ڪريو RISCV_IOMMU_FCTL_BE             ٻٽو_انگ(0)
#وصف_ڪريو RISCV_IOMMU_FCTL_WSI            ٻٽو_انگ(1)
#وصف_ڪريو RISCV_IOMMU_FCTL_GXL            ٻٽو_انگ(2)

/* 5.5 Device-directory-table pointer (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_DDTP            0x0010
#وصف_ڪريو RISCV_IOMMU_DDTP_گهڻائي           GENMASK_ULL(3, 0)
#وصف_ڪريو RISCV_IOMMU_DDTP_BUSY           ٻٽو_انگ_ULL(4)
#وصف_ڪريو RISCV_IOMMU_DDTP_PPN            RISCV_IOMMU_PPN_ميدان

تعداد riscv_iommu_ddtp_modes {
    RISCV_IOMMU_DDTP_گهڻائي_OFF = 0,
    RISCV_IOMMU_DDTP_گهڻائي_BARE = 1,
    RISCV_IOMMU_DDTP_گهڻائي_1_LVL = 2,
    RISCV_IOMMU_DDTP_گهڻائي_2_LVL = 3,
    RISCV_IOMMU_DDTP_گهڻائي_3_LVL = 4,
    RISCV_IOMMU_DDTP_گهڻائي_MAX = 4
};

/* 5.6 Command Queue Base (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_CQB             0x0018
#وصف_ڪريو RISCV_IOMMU_CQB_LOG2SZ          RISCV_IOMMU_قطار_LOGSZ_ميدان
#وصف_ڪريو RISCV_IOMMU_CQB_PPN             RISCV_IOMMU_PPN_ميدان

/* 5.7 Command Queue head (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_CQH             0x0020

/* 5.8 Command Queue tail (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_CQT             0x0024

/* 5.9 Fault Queue Base (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_FQB             0x0028
#وصف_ڪريو RISCV_IOMMU_FQB_LOG2SZ          RISCV_IOMMU_قطار_LOGSZ_ميدان
#وصف_ڪريو RISCV_IOMMU_FQB_PPN             RISCV_IOMMU_PPN_ميدان

/* 5.10 Fault Queue Head (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_FQH             0x0030

/* 5.11 Fault Queue tail (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_FQT             0x0034

/* 5.12 Page Request Queue base (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_PQB             0x0038
#وصف_ڪريو RISCV_IOMMU_PQB_LOG2SZ          RISCV_IOMMU_قطار_LOGSZ_ميدان
#وصف_ڪريو RISCV_IOMMU_PQB_PPN             RISCV_IOMMU_PPN_ميدان

/* 5.13 Page Request Queue head (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_PQH             0x0040

/* 5.14 Page Request Queue tail (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_PQT             0x0044

/* 5.15 Command Queue CSR (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_CQCSR           0x0048
#وصف_ڪريو RISCV_IOMMU_CQCSR_CQEN          RISCV_IOMMU_قطار_ENABLE
#وصف_ڪريو RISCV_IOMMU_CQCSR_CIE           RISCV_IOMMU_قطار_INTR_ENABLE
#وصف_ڪريو RISCV_IOMMU_CQCSR_CQMF          RISCV_IOMMU_قطار_MEM_FAULT
#وصف_ڪريو RISCV_IOMMU_CQCSR_CMD_منزل        ٻٽو_انگ(9)
#وصف_ڪريو RISCV_IOMMU_CQCSR_CMD_ILL       ٻٽو_انگ(10)
#وصف_ڪريو RISCV_IOMMU_CQCSR_FENCE_W_IP    ٻٽو_انگ(11)
#وصف_ڪريو RISCV_IOMMU_CQCSR_CQON          RISCV_IOMMU_قطار_سرگرم
#وصف_ڪريو RISCV_IOMMU_CQCSR_BUSY          RISCV_IOMMU_قطار_BUSY

/* 5.16 Fault Queue CSR (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_FQCSR           0x004C
#وصف_ڪريو RISCV_IOMMU_FQCSR_FQEN          RISCV_IOMMU_قطار_ENABLE
#وصف_ڪريو RISCV_IOMMU_FQCSR_FIE           RISCV_IOMMU_قطار_INTR_ENABLE
#وصف_ڪريو RISCV_IOMMU_FQCSR_FQMF          RISCV_IOMMU_قطار_MEM_FAULT
#وصف_ڪريو RISCV_IOMMU_FQCSR_FQOF          RISCV_IOMMU_قطار_حد_کان_مٿي
#وصف_ڪريو RISCV_IOMMU_FQCSR_FQON          RISCV_IOMMU_قطار_سرگرم
#وصف_ڪريو RISCV_IOMMU_FQCSR_BUSY          RISCV_IOMMU_قطار_BUSY

/* 5.17 Page Request Queue CSR (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_PQCSR           0x0050
#وصف_ڪريو RISCV_IOMMU_PQCSR_PQEN          RISCV_IOMMU_قطار_ENABLE
#وصف_ڪريو RISCV_IOMMU_PQCSR_PIE           RISCV_IOMMU_قطار_INTR_ENABLE
#وصف_ڪريو RISCV_IOMMU_PQCSR_PQMF          RISCV_IOMMU_قطار_MEM_FAULT
#وصف_ڪريو RISCV_IOMMU_PQCSR_PQOF          RISCV_IOMMU_قطار_حد_کان_مٿي
#وصف_ڪريو RISCV_IOMMU_PQCSR_PQON          RISCV_IOMMU_قطار_سرگرم
#وصف_ڪريو RISCV_IOMMU_PQCSR_BUSY          RISCV_IOMMU_قطار_BUSY

/* 5.18 Interrupt Pending Status (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_IPSR            0x0054
#وصف_ڪريو RISCV_IOMMU_IPSR_CIP            ٻٽو_انگ(0)
#وصف_ڪريو RISCV_IOMMU_IPSR_FIP            ٻٽو_انگ(1)
#وصف_ڪريو RISCV_IOMMU_IPSR_PMIP           ٻٽو_انگ(2)
#وصف_ڪريو RISCV_IOMMU_IPSR_PIP            ٻٽو_انگ(3)

تعداد {
    RISCV_IOMMU_INTR_CQ,
    RISCV_IOMMU_INTR_FQ,
    RISCV_IOMMU_INTR_PM,
    RISCV_IOMMU_INTR_PQ,
    RISCV_IOMMU_INTR_ڳڻپ
};

#وصف_ڪريو RISCV_IOMMU_IOCOUNT_انگ         31

/* 5.19 Performance monitoring counter overflow status (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_IOCOUNTOVF      0x0058
#وصف_ڪريو RISCV_IOMMU_IOCOUNTOVF_CY       ٻٽو_انگ(0)

/* 5.20 Performance monitoring counter inhibits (32bits) */
#وصف_ڪريو RISCV_IOMMU_REG_IOCOUNTINH      0x005C
#وصف_ڪريو RISCV_IOMMU_IOCOUNTINH_CY       ٻٽو_انگ(0)

/* 5.21 Performance monitoring cycles counter (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_IOHPMCYCLES     0x0060
#وصف_ڪريو RISCV_IOMMU_IOHPMCYCLES_COUNTER GENMASK_ULL(62, 0)
#وصف_ڪريو RISCV_IOMMU_IOHPMCYCLES_OVF     ٻٽو_انگ_ULL(63)

/* 5.22 Performance monitoring event counters (31 * 64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_IOHPMCTR_بنياد   0x0068
#وصف_ڪريو RISCV_IOMMU_REG_IOHPMCTR(_n)    \
    (RISCV_IOMMU_REG_IOHPMCTR_بنياد + (_n * 0x8))

/* 5.23 Performance monitoring event selectors (31 * 64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_IOHPMEVT_بنياد   0x0160
#وصف_ڪريو RISCV_IOMMU_REG_IOHPMEVT(_n)    \
    (RISCV_IOMMU_REG_IOHPMEVT_بنياد + (_n * 0x8))
#وصف_ڪريو RISCV_IOMMU_IOHPMEVT_EVENT_ID   GENMASK_ULL(14, 0)
#وصف_ڪريو RISCV_IOMMU_IOHPMEVT_DMASK      ٻٽو_انگ_ULL(15)
#وصف_ڪريو RISCV_IOMMU_IOHPMEVT_PID_PSCID  GENMASK_ULL(35, 16)
#وصف_ڪريو RISCV_IOMMU_IOHPMEVT_DID_GSCID  GENMASK_ULL(59, 36)
#وصف_ڪريو RISCV_IOMMU_IOHPMEVT_PV_PSCV    ٻٽو_انگ_ULL(60)
#وصف_ڪريو RISCV_IOMMU_IOHPMEVT_DV_GSCV    ٻٽو_انگ_ULL(61)
#وصف_ڪريو RISCV_IOMMU_IOHPMEVT_IDT        ٻٽو_انگ_ULL(62)
#وصف_ڪريو RISCV_IOMMU_IOHPMEVT_جو         ٻٽو_انگ_ULL(63)

تعداد RISCV_IOMMU_HPMEVENT_id {
    RISCV_IOMMU_HPMEVENT_INVALID    = 0,
    RISCV_IOMMU_HPMEVENT_URQ        = 1,
    RISCV_IOMMU_HPMEVENT_TRQ        = 2,
    RISCV_IOMMU_HPMEVENT_ATS_RQ     = 3,
    RISCV_IOMMU_HPMEVENT_TLB_ناڪامي   = 4,
    RISCV_IOMMU_HPMEVENT_DD_WALK    = 5,
    RISCV_IOMMU_HPMEVENT_PD_WALK    = 6,
    RISCV_IOMMU_HPMEVENT_S_VS_WALKS = 7,
    RISCV_IOMMU_HPMEVENT_G_WALKS    = 8,
    RISCV_IOMMU_HPMEVENT_MAX        = 9
};

/* 5.24 Translation request IOVA (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_TR_REQ_IOVA     0x0258

/* 5.25 Translation request control (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_TR_REQ_CTL      0x0260
#وصف_ڪريو RISCV_IOMMU_TR_REQ_CTL_GO_BUSY  ٻٽو_انگ_ULL(0)
#وصف_ڪريو RISCV_IOMMU_TR_REQ_CTL_NW       ٻٽو_انگ_ULL(3)
#وصف_ڪريو RISCV_IOMMU_TR_REQ_CTL_PID      GENMASK_ULL(31, 12)
#وصف_ڪريو RISCV_IOMMU_TR_REQ_CTL_DID      GENMASK_ULL(63, 40)

/* 5.26 Translation request response (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_TR_RESPONSE     0x0268
#وصف_ڪريو RISCV_IOMMU_TR_RESPONSE_FAULT   ٻٽو_انگ_ULL(0)
#وصف_ڪريو RISCV_IOMMU_TR_RESPONSE_S       ٻٽو_انگ_ULL(9)
#وصف_ڪريو RISCV_IOMMU_TR_RESPONSE_PPN     RISCV_IOMMU_PPN_ميدان

/* 5.27 Interrupt cause to vector (64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_ICVEC           0x02F8
#وصف_ڪريو RISCV_IOMMU_ICVEC_CIV           GENMASK_ULL(3, 0)
#وصف_ڪريو RISCV_IOMMU_ICVEC_FIV           GENMASK_ULL(7, 4)
#وصف_ڪريو RISCV_IOMMU_ICVEC_PMIV          GENMASK_ULL(11, 8)
#وصف_ڪريو RISCV_IOMMU_ICVEC_PIV           GENMASK_ULL(15, 12)

/* 5.28 MSI Configuration table (32 * 64bits) */
#وصف_ڪريو RISCV_IOMMU_REG_MSI_CONFIG      0x0300

#وصف_ڪريو RISCV_IOMMU_REG_سائيز            0x1000

#وصف_ڪريو RISCV_IOMMU_DDTE_VALID          ٻٽو_انگ_ULL(0)
#وصف_ڪريو RISCV_IOMMU_DDTE_PPN            RISCV_IOMMU_PPN_ميدان

/* Struct riscv_iommu_dc - Device Context - section 2.1 */
بنية riscv_iommu_dc {
      uint64_t tc;
      uint64_t iohgatp;
      uint64_t ta;
      uint64_t fsc;
      uint64_t msiptp;
      uint64_t msi_addr_نقاب;
      uint64_t msi_addr_نمونو;
      uint64_t _reserved;
};

/* Translation control fields */
#وصف_ڪريو RISCV_IOMMU_DC_TC_V             ٻٽو_انگ_ULL(0)
#وصف_ڪريو RISCV_IOMMU_DC_TC_EN_ATS        ٻٽو_انگ_ULL(1)
#وصف_ڪريو RISCV_IOMMU_DC_TC_EN_PRI        ٻٽو_انگ_ULL(2)
#وصف_ڪريو RISCV_IOMMU_DC_TC_T2GPA         ٻٽو_انگ_ULL(3)
#وصف_ڪريو RISCV_IOMMU_DC_TC_DTF           ٻٽو_انگ_ULL(4)
#وصف_ڪريو RISCV_IOMMU_DC_TC_PDTV          ٻٽو_انگ_ULL(5)
#وصف_ڪريو RISCV_IOMMU_DC_TC_PRPR          ٻٽو_انگ_ULL(6)
#وصف_ڪريو RISCV_IOMMU_DC_TC_GADE          ٻٽو_انگ_ULL(7)
#وصف_ڪريو RISCV_IOMMU_DC_TC_SADE          ٻٽو_انگ_ULL(8)
#وصف_ڪريو RISCV_IOMMU_DC_TC_DPE           ٻٽو_انگ_ULL(9)
#وصف_ڪريو RISCV_IOMMU_DC_TC_SBE           ٻٽو_انگ_ULL(10)
#وصف_ڪريو RISCV_IOMMU_DC_TC_SXL           ٻٽو_انگ_ULL(11)

/* Second-stage (aka G-stage) context fields */
#وصف_ڪريو RISCV_IOMMU_DC_IOHGATP_PPN      RISCV_IOMMU_ATP_PPN_ميدان
#وصف_ڪريو RISCV_IOMMU_DC_IOHGATP_GSCID    GENMASK_ULL(59, 44)
#وصف_ڪريو RISCV_IOMMU_DC_IOHGATP_گهڻائي     RISCV_IOMMU_ATP_گهڻائي_ميدان

#وصف_ڪريو RISCV_IOMMU_DC_TC_RESERVED      (GENMASK_ULL(23, 12) \
                                         | GENMASK_ULL(63, 32))

تعداد riscv_iommu_dc_iohgatp_modes {
    RISCV_IOMMU_DC_IOHGATP_گهڻائي_BARE = 0,
    RISCV_IOMMU_DC_IOHGATP_گهڻائي_SV32_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_گهڻائي_SV39_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_گهڻائي_SV48_X4 = 9,
    RISCV_IOMMU_DC_IOHGATP_گهڻائي_SV57_X4 = 10
};

/* Translation attributes fields */
#وصف_ڪريو RISCV_IOMMU_DC_TA_PSCID         GENMASK_ULL(31, 12)

/* First-stage context fields */
#وصف_ڪريو RISCV_IOMMU_DC_FSC_PPN          RISCV_IOMMU_ATP_PPN_ميدان
#وصف_ڪريو RISCV_IOMMU_DC_FSC_گهڻائي         RISCV_IOMMU_ATP_گهڻائي_ميدان

/* Generic I/O MMU command structure - check section 3.1 */
بنية riscv_iommu_command {
    uint64_t dword0;
    uint64_t dword1;
};

#وصف_ڪريو RISCV_IOMMU_CMD_OPCODE          GENMASK_ULL(6, 0)
#وصف_ڪريو RISCV_IOMMU_CMD_FUNC            GENMASK_ULL(9, 7)

#وصف_ڪريو RISCV_IOMMU_CMD_IOTINVAL_OPCODE         1
#وصف_ڪريو RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA       0
#وصف_ڪريو RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA      1
#وصف_ڪريو RISCV_IOMMU_CMD_IOTINVAL_AV     ٻٽو_انگ_ULL(10)
#وصف_ڪريو RISCV_IOMMU_CMD_IOTINVAL_PSCID  GENMASK_ULL(31, 12)
#وصف_ڪريو RISCV_IOMMU_CMD_IOTINVAL_PSCV   ٻٽو_انگ_ULL(32)
#وصف_ڪريو RISCV_IOMMU_CMD_IOTINVAL_GV     ٻٽو_انگ_ULL(33)
#وصف_ڪريو RISCV_IOMMU_CMD_IOTINVAL_GSCID  GENMASK_ULL(59, 44)

#وصف_ڪريو RISCV_IOMMU_CMD_IOFENCE_OPCODE          2
#وصف_ڪريو RISCV_IOMMU_CMD_IOFENCE_FUNC_C          0
#وصف_ڪريو RISCV_IOMMU_CMD_IOFENCE_AV      ٻٽو_انگ_ULL(10)
#وصف_ڪريو RISCV_IOMMU_CMD_IOFENCE_RESERVED GENMASK_ULL(31, 14)
#وصف_ڪريو RISCV_IOMMU_CMD_IOFENCE_معلومات    GENMASK_ULL(63, 32)

#وصف_ڪريو RISCV_IOMMU_CMD_IODIR_OPCODE            3
#وصف_ڪريو RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT    0
#وصف_ڪريو RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT    1
#وصف_ڪريو RISCV_IOMMU_CMD_IODIR_PID       GENMASK_ULL(31, 12)
#وصف_ڪريو RISCV_IOMMU_CMD_IODIR_DV        ٻٽو_انگ_ULL(33)
#وصف_ڪريو RISCV_IOMMU_CMD_IODIR_DID       GENMASK_ULL(63, 40)

/* 3.1.4 I/O MMU PCIe ATS */
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_OPCODE              4
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_FUNC_INVAL          0
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_FUNC_PRGR           1
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_PID         GENMASK_ULL(31, 12)
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_PV          ٻٽو_انگ_ULL(32)
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_DSV         ٻٽو_انگ_ULL(33)
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_RID         GENMASK_ULL(55, 40)
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_DSEG        GENMASK_ULL(63, 56)
/* dword1 is the ATS payload, two different payload types for INVAL and PRGR */

/* ATS.PRGR payload */
#وصف_ڪريو RISCV_IOMMU_CMD_ATS_PRGR_RESP_ڪوڊ      GENMASK_ULL(47, 44)

تعداد riscv_iommu_dc_fsc_atp_modes {
    RISCV_IOMMU_DC_FSC_گهڻائي_BARE = 0,
    RISCV_IOMMU_DC_FSC_IOSATP_گهڻائي_SV32 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_گهڻائي_SV39 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_گهڻائي_SV48 = 9,
    RISCV_IOMMU_DC_FSC_IOSATP_گهڻائي_SV57 = 10,
    RISCV_IOMMU_DC_FSC_PDTP_گهڻائي_PD8 = 1,
    RISCV_IOMMU_DC_FSC_PDTP_گهڻائي_PD17 = 2,
    RISCV_IOMMU_DC_FSC_PDTP_گهڻائي_PD20 = 3
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
    RISCV_IOMMU_FQ_CAUSE_DDT_ڀريو_FAULT       = 257,
    RISCV_IOMMU_FQ_CAUSE_DDT_INVALID          = 258,
    RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED    = 259,
    RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED        = 260,
    RISCV_IOMMU_FQ_CAUSE_MSI_ڀريو_FAULT       = 261,
    RISCV_IOMMU_FQ_CAUSE_MSI_INVALID          = 262,
    RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED    = 263,
    RISCV_IOMMU_FQ_CAUSE_MRIF_FAULT           = 264,
    RISCV_IOMMU_FQ_CAUSE_PDT_ڀريو_FAULT       = 265,
    RISCV_IOMMU_FQ_CAUSE_PDT_INVALID          = 266,
    RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED    = 267,
    RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED        = 268,
    RISCV_IOMMU_FQ_CAUSE_PDT_CORRUPTED        = 269,
    RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED     = 270,
    RISCV_IOMMU_FQ_CAUSE_MRIF_CORRUIPTED      = 271,
    RISCV_IOMMU_FQ_CAUSE_اندروني_DP_ERROR    = 272,
    RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT         = 273,
    RISCV_IOMMU_FQ_CAUSE_PT_CORRUPTED         = 274
};

/* MSI page table pointer */
#وصف_ڪريو RISCV_IOMMU_DC_MSIPTP_PPN       RISCV_IOMMU_ATP_PPN_ميدان
#وصف_ڪريو RISCV_IOMMU_DC_MSIPTP_گهڻائي      RISCV_IOMMU_ATP_گهڻائي_ميدان
#وصف_ڪريو RISCV_IOMMU_DC_MSIPTP_گهڻائي_OFF  0
#وصف_ڪريو RISCV_IOMMU_DC_MSIPTP_گهڻائي_FLAT 1

/* 2.2 Process Directory Table */
#وصف_ڪريو RISCV_IOMMU_PDTE_VALID          ٻٽو_انگ_ULL(0)
#وصف_ڪريو RISCV_IOMMU_PDTE_PPN            RISCV_IOMMU_PPN_ميدان

/* Translation attributes fields */
#وصف_ڪريو RISCV_IOMMU_PC_TA_V             ٻٽو_انگ_ULL(0)
#وصف_ڪريو RISCV_IOMMU_PC_TA_RESERVED      GENMASK_ULL(63, 32)

/* First stage context fields */
#وصف_ڪريو RISCV_IOMMU_PC_FSC_PPN          RISCV_IOMMU_ATP_PPN_ميدان
#وصف_ڪريو RISCV_IOMMU_PC_FSC_RESERVED     GENMASK_ULL(59, 44)

تعداد riscv_iommu_fq_ttypes {
    RISCV_IOMMU_FQ_TTYPE_NONE = 0,
    RISCV_IOMMU_FQ_TTYPE_UADDR_INST_FETCH = 1,
    RISCV_IOMMU_FQ_TTYPE_UADDR_RD = 2,
    RISCV_IOMMU_FQ_TTYPE_UADDR_WR = 3,
    RISCV_IOMMU_FQ_TTYPE_TADDR_INST_FETCH = 5,
    RISCV_IOMMU_FQ_TTYPE_TADDR_RD = 6,
    RISCV_IOMMU_FQ_TTYPE_TADDR_WR = 7,
    RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ = 8,
    RISCV_IOMMU_FW_TTYPE_PCIE_نياپو_REQ = 9,
};

/*
 * struct riscv_iommu_msi_pte - MSI Page Table Entry
 */
بنية riscv_iommu_msi_pte {
      uint64_t pte;
      uint64_t mrif_info;
};

/* Fields on pte */
#وصف_ڪريو RISCV_IOMMU_MSI_PTE_V           ٻٽو_انگ_ULL(0)
#وصف_ڪريو RISCV_IOMMU_MSI_PTE_M           GENMASK_ULL(2, 1)

#وصف_ڪريو RISCV_IOMMU_MSI_PTE_M_MRIF      1
#وصف_ڪريو RISCV_IOMMU_MSI_PTE_M_BASIC     3

/* When M == 1 (MRIF mode) */
#وصف_ڪريو RISCV_IOMMU_MSI_PTE_MRIF_ADDR   GENMASK_ULL(53, 7)
/* When M == 3 (basic mode) */
#وصف_ڪريو RISCV_IOMMU_MSI_PTE_PPN         RISCV_IOMMU_PPN_ميدان
#وصف_ڪريو RISCV_IOMMU_MSI_PTE_C           ٻٽو_انگ_ULL(63)

/* Fields on mrif_info */
#وصف_ڪريو RISCV_IOMMU_MSI_MRIF_NID        GENMASK_ULL(9, 0)
#وصف_ڪريو RISCV_IOMMU_MSI_MRIF_NPPN       RISCV_IOMMU_PPN_ميدان
#وصف_ڪريو RISCV_IOMMU_MSI_MRIF_NID_MSB    ٻٽو_انگ_ULL(60)

#جيڪڏهن_پورو /* _RISCV_IOMMU_BITS_H_ */
