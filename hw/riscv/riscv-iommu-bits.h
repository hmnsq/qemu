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

#اگر_غیر_تعریف_شدہ HW_RISCV_IOMMU_بٹ_H
#تعریف_کرو HW_RISCV_IOMMU_بٹ_H

#تعریف_کرو RISCV_IOMMU_SPEC_DOT_VER 0x010

#اگر_غیر_تعریف_شدہ GENMASK_ULL
#تعریف_کرو GENMASK_ULL(h, l) (((~0ULL) >> (63 - (h) + (l))) << (l))
#اگر_ختم

/*
 * struct riscv_iommu_fq_record - Fault/Event Queue Record
 * See section 3.2 for more info.
 */
ساخت riscv_iommu_fq_ریکارڈ {
    uint64_t hdr;
    uint64_t _reserved;
    uint64_t iotval;
    uint64_t iotval2;
};
/* Header fields */
#تعریف_کرو RISCV_IOMMU_FQ_HDR_CAUSE        GENMASK_ULL(11, 0)
#تعریف_کرو RISCV_IOMMU_FQ_HDR_PID          GENMASK_ULL(31, 12)
#تعریف_کرو RISCV_IOMMU_FQ_HDR_PV           ثنائی_ہندسہ_ULL(32)
#تعریف_کرو RISCV_IOMMU_FQ_HDR_TTYPE        GENMASK_ULL(39, 34)
#تعریف_کرو RISCV_IOMMU_FQ_HDR_DID          GENMASK_ULL(63, 40)

/*
 * struct riscv_iommu_pq_record - PCIe Page Request record
 * For more infos on the PCIe Page Request queue see chapter 3.3.
 */
ساخت riscv_iommu_pq_ریکارڈ {
      uint64_t hdr;
      uint64_t payload;
};
/* Header fields */
#تعریف_کرو RISCV_IOMMU_PREQ_HDR_PID        GENMASK_ULL(31, 12)
#تعریف_کرو RISCV_IOMMU_PREQ_HDR_PV         ثنائی_ہندسہ_ULL(32)
#تعریف_کرو RISCV_IOMMU_PREQ_HDR_PRIV       ثنائی_ہندسہ_ULL(33)
#تعریف_کرو RISCV_IOMMU_PREQ_HDR_EXEC       ثنائی_ہندسہ_ULL(34)
#تعریف_کرو RISCV_IOMMU_PREQ_HDR_DID        GENMASK_ULL(63, 40)

/* Payload fields */
#تعریف_کرو RISCV_IOMMU_PREQ_PAYLOAD_R      ثنائی_ہندسہ_ULL(0)
#تعریف_کرو RISCV_IOMMU_PREQ_PAYLOAD_W      ثنائی_ہندسہ_ULL(1)
#تعریف_کرو RISCV_IOMMU_PREQ_PAYLOAD_L      ثنائی_ہندسہ_ULL(2)
#تعریف_کرو RISCV_IOMMU_PREQ_PAYLOAD_M      GENMASK_ULL(2, 0)
#تعریف_کرو RISCV_IOMMU_PREQ_PRG_اشاریہ      GENMASK_ULL(11, 3)
#تعریف_کرو RISCV_IOMMU_PREQ_UADDR          GENMASK_ULL(63, 12)

/* Common field positions */
#تعریف_کرو RISCV_IOMMU_PPN_میدان           GENMASK_ULL(53, 10)
#تعریف_کرو RISCV_IOMMU_قطار_LOGSZ_میدان   GENMASK_ULL(4, 0)
#تعریف_کرو RISCV_IOMMU_قطار_اشاریہ_میدان   GENMASK_ULL(31, 0)
#تعریف_کرو RISCV_IOMMU_قطار_ENABLE        ثنائی_ہندسہ(0)
#تعریف_کرو RISCV_IOMMU_قطار_INTR_ENABLE   ثنائی_ہندسہ(1)
#تعریف_کرو RISCV_IOMMU_قطار_MEM_FAULT     ثنائی_ہندسہ(8)
#تعریف_کرو RISCV_IOMMU_قطار_حد_سے_تجاوز      ثنائی_ہندسہ(9)
#تعریف_کرو RISCV_IOMMU_قطار_فعال        ثنائی_ہندسہ(16)
#تعریف_کرو RISCV_IOMMU_قطار_BUSY          ثنائی_ہندسہ(17)
#تعریف_کرو RISCV_IOMMU_ATP_PPN_میدان       GENMASK_ULL(43, 0)
#تعریف_کرو RISCV_IOMMU_ATP_منوال_میدان      GENMASK_ULL(63, 60)

/* 5.3 IOMMU Capabilities (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_CAP             0x0000
#تعریف_کرو RISCV_IOMMU_CAP_نسخہ         GENMASK_ULL(7, 0)
#تعریف_کرو RISCV_IOMMU_CAP_SV32            ثنائی_ہندسہ_ULL(8)
#تعریف_کرو RISCV_IOMMU_CAP_SV39            ثنائی_ہندسہ_ULL(9)
#تعریف_کرو RISCV_IOMMU_CAP_SV48            ثنائی_ہندسہ_ULL(10)
#تعریف_کرو RISCV_IOMMU_CAP_SV57            ثنائی_ہندسہ_ULL(11)
#تعریف_کرو RISCV_IOMMU_CAP_SVRSW60T59B     ثنائی_ہندسہ_ULL(14)
#تعریف_کرو RISCV_IOMMU_CAP_SV32X4          ثنائی_ہندسہ_ULL(16)
#تعریف_کرو RISCV_IOMMU_CAP_SV39X4          ثنائی_ہندسہ_ULL(17)
#تعریف_کرو RISCV_IOMMU_CAP_SV48X4          ثنائی_ہندسہ_ULL(18)
#تعریف_کرو RISCV_IOMMU_CAP_SV57X4          ثنائی_ہندسہ_ULL(19)
#تعریف_کرو RISCV_IOMMU_CAP_MSI_FLAT        ثنائی_ہندسہ_ULL(22)
#تعریف_کرو RISCV_IOMMU_CAP_MSI_MRIF        ثنائی_ہندسہ_ULL(23)
#تعریف_کرو RISCV_IOMMU_CAP_AMO_HWAD        ثنائی_ہندسہ_ULL(24)
#تعریف_کرو RISCV_IOMMU_CAP_ATS             ثنائی_ہندسہ_ULL(25)
#تعریف_کرو RISCV_IOMMU_CAP_T2GPA           ثنائی_ہندسہ_ULL(26)
#تعریف_کرو RISCV_IOMMU_CAP_IGS             GENMASK_ULL(29, 28)
#تعریف_کرو RISCV_IOMMU_CAP_HPM             ثنائی_ہندسہ_ULL(30)
#تعریف_کرو RISCV_IOMMU_CAP_DBG             ثنائی_ہندسہ_ULL(31)
#تعریف_کرو RISCV_IOMMU_CAP_PAS             GENMASK_ULL(37, 32)
#تعریف_کرو RISCV_IOMMU_CAP_PD8             ثنائی_ہندسہ_ULL(38)
#تعریف_کرو RISCV_IOMMU_CAP_PD17            ثنائی_ہندسہ_ULL(39)
#تعریف_کرو RISCV_IOMMU_CAP_PD20            ثنائی_ہندسہ_ULL(40)

شمار riscv_iommu_igs_modes {
    RISCV_IOMMU_CAP_IGS_MSI = 0,
    RISCV_IOMMU_CAP_IGS_WSI,
    RISCV_IOMMU_CAP_IGS_BOTH
};

/* 5.4 Features control register (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_FCTL            0x0008
#تعریف_کرو RISCV_IOMMU_FCTL_BE             ثنائی_ہندسہ(0)
#تعریف_کرو RISCV_IOMMU_FCTL_WSI            ثنائی_ہندسہ(1)
#تعریف_کرو RISCV_IOMMU_FCTL_GXL            ثنائی_ہندسہ(2)

/* 5.5 Device-directory-table pointer (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_DDTP            0x0010
#تعریف_کرو RISCV_IOMMU_DDTP_منوال           GENMASK_ULL(3, 0)
#تعریف_کرو RISCV_IOMMU_DDTP_BUSY           ثنائی_ہندسہ_ULL(4)
#تعریف_کرو RISCV_IOMMU_DDTP_PPN            RISCV_IOMMU_PPN_میدان

شمار riscv_iommu_ddtp_modes {
    RISCV_IOMMU_DDTP_منوال_OFF = 0,
    RISCV_IOMMU_DDTP_منوال_BARE = 1,
    RISCV_IOMMU_DDTP_منوال_1_LVL = 2,
    RISCV_IOMMU_DDTP_منوال_2_LVL = 3,
    RISCV_IOMMU_DDTP_منوال_3_LVL = 4,
    RISCV_IOMMU_DDTP_منوال_MAX = 4
};

/* 5.6 Command Queue Base (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_CQB             0x0018
#تعریف_کرو RISCV_IOMMU_CQB_LOG2SZ          RISCV_IOMMU_قطار_LOGSZ_میدان
#تعریف_کرو RISCV_IOMMU_CQB_PPN             RISCV_IOMMU_PPN_میدان

/* 5.7 Command Queue head (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_CQH             0x0020

/* 5.8 Command Queue tail (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_CQT             0x0024

/* 5.9 Fault Queue Base (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_FQB             0x0028
#تعریف_کرو RISCV_IOMMU_FQB_LOG2SZ          RISCV_IOMMU_قطار_LOGSZ_میدان
#تعریف_کرو RISCV_IOMMU_FQB_PPN             RISCV_IOMMU_PPN_میدان

/* 5.10 Fault Queue Head (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_FQH             0x0030

/* 5.11 Fault Queue tail (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_FQT             0x0034

/* 5.12 Page Request Queue base (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_PQB             0x0038
#تعریف_کرو RISCV_IOMMU_PQB_LOG2SZ          RISCV_IOMMU_قطار_LOGSZ_میدان
#تعریف_کرو RISCV_IOMMU_PQB_PPN             RISCV_IOMMU_PPN_میدان

/* 5.13 Page Request Queue head (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_PQH             0x0040

/* 5.14 Page Request Queue tail (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_PQT             0x0044

/* 5.15 Command Queue CSR (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_CQCSR           0x0048
#تعریف_کرو RISCV_IOMMU_CQCSR_CQEN          RISCV_IOMMU_قطار_ENABLE
#تعریف_کرو RISCV_IOMMU_CQCSR_CIE           RISCV_IOMMU_قطار_INTR_ENABLE
#تعریف_کرو RISCV_IOMMU_CQCSR_CQMF          RISCV_IOMMU_قطار_MEM_FAULT
#تعریف_کرو RISCV_IOMMU_CQCSR_CMD_مقصد        ثنائی_ہندسہ(9)
#تعریف_کرو RISCV_IOMMU_CQCSR_CMD_ILL       ثنائی_ہندسہ(10)
#تعریف_کرو RISCV_IOMMU_CQCSR_FENCE_W_IP    ثنائی_ہندسہ(11)
#تعریف_کرو RISCV_IOMMU_CQCSR_CQON          RISCV_IOMMU_قطار_فعال
#تعریف_کرو RISCV_IOMMU_CQCSR_BUSY          RISCV_IOMMU_قطار_BUSY

/* 5.16 Fault Queue CSR (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_FQCSR           0x004C
#تعریف_کرو RISCV_IOMMU_FQCSR_FQEN          RISCV_IOMMU_قطار_ENABLE
#تعریف_کرو RISCV_IOMMU_FQCSR_FIE           RISCV_IOMMU_قطار_INTR_ENABLE
#تعریف_کرو RISCV_IOMMU_FQCSR_FQMF          RISCV_IOMMU_قطار_MEM_FAULT
#تعریف_کرو RISCV_IOMMU_FQCSR_FQOF          RISCV_IOMMU_قطار_حد_سے_تجاوز
#تعریف_کرو RISCV_IOMMU_FQCSR_FQON          RISCV_IOMMU_قطار_فعال
#تعریف_کرو RISCV_IOMMU_FQCSR_BUSY          RISCV_IOMMU_قطار_BUSY

/* 5.17 Page Request Queue CSR (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_PQCSR           0x0050
#تعریف_کرو RISCV_IOMMU_PQCSR_PQEN          RISCV_IOMMU_قطار_ENABLE
#تعریف_کرو RISCV_IOMMU_PQCSR_PIE           RISCV_IOMMU_قطار_INTR_ENABLE
#تعریف_کرو RISCV_IOMMU_PQCSR_PQMF          RISCV_IOMMU_قطار_MEM_FAULT
#تعریف_کرو RISCV_IOMMU_PQCSR_PQOF          RISCV_IOMMU_قطار_حد_سے_تجاوز
#تعریف_کرو RISCV_IOMMU_PQCSR_PQON          RISCV_IOMMU_قطار_فعال
#تعریف_کرو RISCV_IOMMU_PQCSR_BUSY          RISCV_IOMMU_قطار_BUSY

/* 5.18 Interrupt Pending Status (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_IPSR            0x0054
#تعریف_کرو RISCV_IOMMU_IPSR_CIP            ثنائی_ہندسہ(0)
#تعریف_کرو RISCV_IOMMU_IPSR_FIP            ثنائی_ہندسہ(1)
#تعریف_کرو RISCV_IOMMU_IPSR_PMIP           ثنائی_ہندسہ(2)
#تعریف_کرو RISCV_IOMMU_IPSR_PIP            ثنائی_ہندسہ(3)

شمار {
    RISCV_IOMMU_INTR_CQ,
    RISCV_IOMMU_INTR_FQ,
    RISCV_IOMMU_INTR_PM,
    RISCV_IOMMU_INTR_PQ,
    RISCV_IOMMU_INTR_گنتی
};

#تعریف_کرو RISCV_IOMMU_IOCOUNT_عدد         31

/* 5.19 Performance monitoring counter overflow status (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_IOCOUNTOVF      0x0058
#تعریف_کرو RISCV_IOMMU_IOCOUNTOVF_CY       ثنائی_ہندسہ(0)

/* 5.20 Performance monitoring counter inhibits (32bits) */
#تعریف_کرو RISCV_IOMMU_REG_IOCOUNTINH      0x005C
#تعریف_کرو RISCV_IOMMU_IOCOUNTINH_CY       ثنائی_ہندسہ(0)

/* 5.21 Performance monitoring cycles counter (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_IOHPMCYCLES     0x0060
#تعریف_کرو RISCV_IOMMU_IOHPMCYCLES_شمار GENMASK_ULL(62, 0)
#تعریف_کرو RISCV_IOMMU_IOHPMCYCLES_OVF     ثنائی_ہندسہ_ULL(63)

/* 5.22 Performance monitoring event counters (31 * 64bits) */
#تعریف_کرو RISCV_IOMMU_REG_IOHPMCTR_بنیاد   0x0068
#تعریف_کرو RISCV_IOMMU_REG_IOHPMCTR(_n)    \
    (RISCV_IOMMU_REG_IOHPMCTR_بنیاد + (_n * 0x8))

/* 5.23 Performance monitoring event selectors (31 * 64bits) */
#تعریف_کرو RISCV_IOMMU_REG_IOHPMEVT_بنیاد   0x0160
#تعریف_کرو RISCV_IOMMU_REG_IOHPMEVT(_n)    \
    (RISCV_IOMMU_REG_IOHPMEVT_بنیاد + (_n * 0x8))
#تعریف_کرو RISCV_IOMMU_IOHPMEVT_EVENT_ID   GENMASK_ULL(14, 0)
#تعریف_کرو RISCV_IOMMU_IOHPMEVT_DMASK      ثنائی_ہندسہ_ULL(15)
#تعریف_کرو RISCV_IOMMU_IOHPMEVT_PID_PSCID  GENMASK_ULL(35, 16)
#تعریف_کرو RISCV_IOMMU_IOHPMEVT_DID_GSCID  GENMASK_ULL(59, 36)
#تعریف_کرو RISCV_IOMMU_IOHPMEVT_PV_PSCV    ثنائی_ہندسہ_ULL(60)
#تعریف_کرو RISCV_IOMMU_IOHPMEVT_DV_GSCV    ثنائی_ہندسہ_ULL(61)
#تعریف_کرو RISCV_IOMMU_IOHPMEVT_IDT        ثنائی_ہندسہ_ULL(62)
#تعریف_کرو RISCV_IOMMU_IOHPMEVT_کا         ثنائی_ہندسہ_ULL(63)

شمار RISCV_IOMMU_HPMEVENT_id {
    RISCV_IOMMU_HPMEVENT_INVALID    = 0,
    RISCV_IOMMU_HPMEVENT_URQ        = 1,
    RISCV_IOMMU_HPMEVENT_TRQ        = 2,
    RISCV_IOMMU_HPMEVENT_ATS_RQ     = 3,
    RISCV_IOMMU_HPMEVENT_TLB_ناکامی   = 4,
    RISCV_IOMMU_HPMEVENT_DD_WALK    = 5,
    RISCV_IOMMU_HPMEVENT_PD_WALK    = 6,
    RISCV_IOMMU_HPMEVENT_S_VS_WALKS = 7,
    RISCV_IOMMU_HPMEVENT_G_WALKS    = 8,
    RISCV_IOMMU_HPMEVENT_MAX        = 9
};

/* 5.24 Translation request IOVA (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_TR_REQ_IOVA     0x0258

/* 5.25 Translation request control (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_TR_REQ_CTL      0x0260
#تعریف_کرو RISCV_IOMMU_TR_REQ_CTL_GO_BUSY  ثنائی_ہندسہ_ULL(0)
#تعریف_کرو RISCV_IOMMU_TR_REQ_CTL_NW       ثنائی_ہندسہ_ULL(3)
#تعریف_کرو RISCV_IOMMU_TR_REQ_CTL_PID      GENMASK_ULL(31, 12)
#تعریف_کرو RISCV_IOMMU_TR_REQ_CTL_DID      GENMASK_ULL(63, 40)

/* 5.26 Translation request response (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_TR_RESPONSE     0x0268
#تعریف_کرو RISCV_IOMMU_TR_RESPONSE_FAULT   ثنائی_ہندسہ_ULL(0)
#تعریف_کرو RISCV_IOMMU_TR_RESPONSE_S       ثنائی_ہندسہ_ULL(9)
#تعریف_کرو RISCV_IOMMU_TR_RESPONSE_PPN     RISCV_IOMMU_PPN_میدان

/* 5.27 Interrupt cause to vector (64bits) */
#تعریف_کرو RISCV_IOMMU_REG_ICVEC           0x02F8
#تعریف_کرو RISCV_IOMMU_ICVEC_CIV           GENMASK_ULL(3, 0)
#تعریف_کرو RISCV_IOMMU_ICVEC_FIV           GENMASK_ULL(7, 4)
#تعریف_کرو RISCV_IOMMU_ICVEC_PMIV          GENMASK_ULL(11, 8)
#تعریف_کرو RISCV_IOMMU_ICVEC_PIV           GENMASK_ULL(15, 12)

/* 5.28 MSI Configuration table (32 * 64bits) */
#تعریف_کرو RISCV_IOMMU_REG_MSI_CONFIG      0x0300

#تعریف_کرو RISCV_IOMMU_REG_سائز            0x1000

#تعریف_کرو RISCV_IOMMU_DDTE_VALID          ثنائی_ہندسہ_ULL(0)
#تعریف_کرو RISCV_IOMMU_DDTE_PPN            RISCV_IOMMU_PPN_میدان

/* Struct riscv_iommu_dc - Device Context - section 2.1 */
ساخت riscv_iommu_dc {
      uint64_t tc;
      uint64_t iohgatp;
      uint64_t ta;
      uint64_t fsc;
      uint64_t msiptp;
      uint64_t msi_addr_نقاب;
      uint64_t msi_addr_نمونہ;
      uint64_t _reserved;
};

/* Translation control fields */
#تعریف_کرو RISCV_IOMMU_DC_TC_V             ثنائی_ہندسہ_ULL(0)
#تعریف_کرو RISCV_IOMMU_DC_TC_EN_ATS        ثنائی_ہندسہ_ULL(1)
#تعریف_کرو RISCV_IOMMU_DC_TC_EN_PRI        ثنائی_ہندسہ_ULL(2)
#تعریف_کرو RISCV_IOMMU_DC_TC_T2GPA         ثنائی_ہندسہ_ULL(3)
#تعریف_کرو RISCV_IOMMU_DC_TC_DTF           ثنائی_ہندسہ_ULL(4)
#تعریف_کرو RISCV_IOMMU_DC_TC_PDTV          ثنائی_ہندسہ_ULL(5)
#تعریف_کرو RISCV_IOMMU_DC_TC_PRPR          ثنائی_ہندسہ_ULL(6)
#تعریف_کرو RISCV_IOMMU_DC_TC_GADE          ثنائی_ہندسہ_ULL(7)
#تعریف_کرو RISCV_IOMMU_DC_TC_SADE          ثنائی_ہندسہ_ULL(8)
#تعریف_کرو RISCV_IOMMU_DC_TC_DPE           ثنائی_ہندسہ_ULL(9)
#تعریف_کرو RISCV_IOMMU_DC_TC_SBE           ثنائی_ہندسہ_ULL(10)
#تعریف_کرو RISCV_IOMMU_DC_TC_SXL           ثنائی_ہندسہ_ULL(11)

/* Second-stage (aka G-stage) context fields */
#تعریف_کرو RISCV_IOMMU_DC_IOHGATP_PPN      RISCV_IOMMU_ATP_PPN_میدان
#تعریف_کرو RISCV_IOMMU_DC_IOHGATP_GSCID    GENMASK_ULL(59, 44)
#تعریف_کرو RISCV_IOMMU_DC_IOHGATP_منوال     RISCV_IOMMU_ATP_منوال_میدان

#تعریف_کرو RISCV_IOMMU_DC_TC_RESERVED      (GENMASK_ULL(23, 12) \
                                         | GENMASK_ULL(63, 32))

شمار riscv_iommu_dc_iohgatp_modes {
    RISCV_IOMMU_DC_IOHGATP_منوال_BARE = 0,
    RISCV_IOMMU_DC_IOHGATP_منوال_SV32_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_منوال_SV39_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_منوال_SV48_X4 = 9,
    RISCV_IOMMU_DC_IOHGATP_منوال_SV57_X4 = 10
};

/* Translation attributes fields */
#تعریف_کرو RISCV_IOMMU_DC_TA_PSCID         GENMASK_ULL(31, 12)

/* First-stage context fields */
#تعریف_کرو RISCV_IOMMU_DC_FSC_PPN          RISCV_IOMMU_ATP_PPN_میدان
#تعریف_کرو RISCV_IOMMU_DC_FSC_منوال         RISCV_IOMMU_ATP_منوال_میدان

/* Generic I/O MMU command structure - check section 3.1 */
ساخت riscv_iommu_command {
    uint64_t dword0;
    uint64_t dword1;
};

#تعریف_کرو RISCV_IOMMU_CMD_OPCODE          GENMASK_ULL(6, 0)
#تعریف_کرو RISCV_IOMMU_CMD_FUNC            GENMASK_ULL(9, 7)

#تعریف_کرو RISCV_IOMMU_CMD_IOTINVAL_OPCODE         1
#تعریف_کرو RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA       0
#تعریف_کرو RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA      1
#تعریف_کرو RISCV_IOMMU_CMD_IOTINVAL_AV     ثنائی_ہندسہ_ULL(10)
#تعریف_کرو RISCV_IOMMU_CMD_IOTINVAL_PSCID  GENMASK_ULL(31, 12)
#تعریف_کرو RISCV_IOMMU_CMD_IOTINVAL_PSCV   ثنائی_ہندسہ_ULL(32)
#تعریف_کرو RISCV_IOMMU_CMD_IOTINVAL_GV     ثنائی_ہندسہ_ULL(33)
#تعریف_کرو RISCV_IOMMU_CMD_IOTINVAL_GSCID  GENMASK_ULL(59, 44)

#تعریف_کرو RISCV_IOMMU_CMD_IOFENCE_OPCODE          2
#تعریف_کرو RISCV_IOMMU_CMD_IOFENCE_FUNC_C          0
#تعریف_کرو RISCV_IOMMU_CMD_IOFENCE_AV      ثنائی_ہندسہ_ULL(10)
#تعریف_کرو RISCV_IOMMU_CMD_IOFENCE_RESERVED GENMASK_ULL(31, 14)
#تعریف_کرو RISCV_IOMMU_CMD_IOFENCE_ڈیٹا    GENMASK_ULL(63, 32)

#تعریف_کرو RISCV_IOMMU_CMD_IODIR_OPCODE            3
#تعریف_کرو RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT    0
#تعریف_کرو RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT    1
#تعریف_کرو RISCV_IOMMU_CMD_IODIR_PID       GENMASK_ULL(31, 12)
#تعریف_کرو RISCV_IOMMU_CMD_IODIR_DV        ثنائی_ہندسہ_ULL(33)
#تعریف_کرو RISCV_IOMMU_CMD_IODIR_DID       GENMASK_ULL(63, 40)

/* 3.1.4 I/O MMU PCIe ATS */
#تعریف_کرو RISCV_IOMMU_CMD_ATS_OPCODE              4
#تعریف_کرو RISCV_IOMMU_CMD_ATS_FUNC_INVAL          0
#تعریف_کرو RISCV_IOMMU_CMD_ATS_FUNC_PRGR           1
#تعریف_کرو RISCV_IOMMU_CMD_ATS_PID         GENMASK_ULL(31, 12)
#تعریف_کرو RISCV_IOMMU_CMD_ATS_PV          ثنائی_ہندسہ_ULL(32)
#تعریف_کرو RISCV_IOMMU_CMD_ATS_DSV         ثنائی_ہندسہ_ULL(33)
#تعریف_کرو RISCV_IOMMU_CMD_ATS_RID         GENMASK_ULL(55, 40)
#تعریف_کرو RISCV_IOMMU_CMD_ATS_DSEG        GENMASK_ULL(63, 56)
/* dword1 is the ATS payload, two different payload types for INVAL and PRGR */

/* ATS.PRGR payload */
#تعریف_کرو RISCV_IOMMU_CMD_ATS_PRGR_RESP_رمز      GENMASK_ULL(47, 44)

شمار riscv_iommu_dc_fsc_atp_modes {
    RISCV_IOMMU_DC_FSC_منوال_BARE = 0,
    RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV32 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV39 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV48 = 9,
    RISCV_IOMMU_DC_FSC_IOSATP_منوال_SV57 = 10,
    RISCV_IOMMU_DC_FSC_PDTP_منوال_PD8 = 1,
    RISCV_IOMMU_DC_FSC_PDTP_منوال_PD17 = 2,
    RISCV_IOMMU_DC_FSC_PDTP_منوال_PD20 = 3
};

شمار riscv_iommu_fq_causes {
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
    RISCV_IOMMU_FQ_CAUSE_DDT_بھرو_FAULT       = 257,
    RISCV_IOMMU_FQ_CAUSE_DDT_INVALID          = 258,
    RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED    = 259,
    RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED        = 260,
    RISCV_IOMMU_FQ_CAUSE_MSI_بھرو_FAULT       = 261,
    RISCV_IOMMU_FQ_CAUSE_MSI_INVALID          = 262,
    RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED    = 263,
    RISCV_IOMMU_FQ_CAUSE_MRIF_FAULT           = 264,
    RISCV_IOMMU_FQ_CAUSE_PDT_بھرو_FAULT       = 265,
    RISCV_IOMMU_FQ_CAUSE_PDT_INVALID          = 266,
    RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED    = 267,
    RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED        = 268,
    RISCV_IOMMU_FQ_CAUSE_PDT_CORRUPTED        = 269,
    RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED     = 270,
    RISCV_IOMMU_FQ_CAUSE_MRIF_CORRUIPTED      = 271,
    RISCV_IOMMU_FQ_CAUSE_اندرونی_DP_ERROR    = 272,
    RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT         = 273,
    RISCV_IOMMU_FQ_CAUSE_PT_CORRUPTED         = 274
};

/* MSI page table pointer */
#تعریف_کرو RISCV_IOMMU_DC_MSIPTP_PPN       RISCV_IOMMU_ATP_PPN_میدان
#تعریف_کرو RISCV_IOMMU_DC_MSIPTP_منوال      RISCV_IOMMU_ATP_منوال_میدان
#تعریف_کرو RISCV_IOMMU_DC_MSIPTP_منوال_OFF  0
#تعریف_کرو RISCV_IOMMU_DC_MSIPTP_منوال_FLAT 1

/* 2.2 Process Directory Table */
#تعریف_کرو RISCV_IOMMU_PDTE_VALID          ثنائی_ہندسہ_ULL(0)
#تعریف_کرو RISCV_IOMMU_PDTE_PPN            RISCV_IOMMU_PPN_میدان

/* Translation attributes fields */
#تعریف_کرو RISCV_IOMMU_PC_TA_V             ثنائی_ہندسہ_ULL(0)
#تعریف_کرو RISCV_IOMMU_PC_TA_RESERVED      GENMASK_ULL(63, 32)

/* First stage context fields */
#تعریف_کرو RISCV_IOMMU_PC_FSC_PPN          RISCV_IOMMU_ATP_PPN_میدان
#تعریف_کرو RISCV_IOMMU_PC_FSC_RESERVED     GENMASK_ULL(59, 44)

شمار riscv_iommu_fq_ttypes {
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
ساخت riscv_iommu_msi_pte {
      uint64_t pte;
      uint64_t mrif_info;
};

/* Fields on pte */
#تعریف_کرو RISCV_IOMMU_MSI_PTE_V           ثنائی_ہندسہ_ULL(0)
#تعریف_کرو RISCV_IOMMU_MSI_PTE_M           GENMASK_ULL(2, 1)

#تعریف_کرو RISCV_IOMMU_MSI_PTE_M_MRIF      1
#تعریف_کرو RISCV_IOMMU_MSI_PTE_M_BASIC     3

/* When M == 1 (MRIF mode) */
#تعریف_کرو RISCV_IOMMU_MSI_PTE_MRIF_ADDR   GENMASK_ULL(53, 7)
/* When M == 3 (basic mode) */
#تعریف_کرو RISCV_IOMMU_MSI_PTE_PPN         RISCV_IOMMU_PPN_میدان
#تعریف_کرو RISCV_IOMMU_MSI_PTE_C           ثنائی_ہندسہ_ULL(63)

/* Fields on mrif_info */
#تعریف_کرو RISCV_IOMMU_MSI_MRIF_NID        GENMASK_ULL(9, 0)
#تعریف_کرو RISCV_IOMMU_MSI_MRIF_NPPN       RISCV_IOMMU_PPN_میدان
#تعریف_کرو RISCV_IOMMU_MSI_MRIF_NID_MSB    ثنائی_ہندسہ_ULL(60)

#اگر_ختم /* _RISCV_IOMMU_BITS_H_ */
