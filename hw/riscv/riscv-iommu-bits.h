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

#यदि_अपरिभाषितम् HW_RISCV_IOMMU_द्वयङ्काः_H
#परिभाषय HW_RISCV_IOMMU_द्वयङ्काः_H

#परिभाषय RISCV_IOMMU_SPEC_DOT_VER 0x010

#यदि_अपरिभाषितम् GENMASK_ULL
#परिभाषय GENMASK_ULL(h, l) (((~0ULL) >> (63 - (h) + (l))) << (l))
#यदि_अन्तः

/*
 * struct riscv_iommu_fq_record - Fault/Event Queue Record
 * See section 3.2 for more info.
 */
संरचना riscv_iommu_fq_लेखः {
    uint64_t hdr;
    uint64_t _reserved;
    uint64_t iotval;
    uint64_t iotval2;
};
/* Header fields */
#परिभाषय RISCV_IOMMU_FQ_HDR_CAUSE        GENMASK_ULL(11, 0)
#परिभाषय RISCV_IOMMU_FQ_HDR_PID          GENMASK_ULL(31, 12)
#परिभाषय RISCV_IOMMU_FQ_HDR_PV           BIT_ULL(32)
#परिभाषय RISCV_IOMMU_FQ_HDR_TTYPE        GENMASK_ULL(39, 34)
#परिभाषय RISCV_IOMMU_FQ_HDR_DID          GENMASK_ULL(63, 40)

/*
 * struct riscv_iommu_pq_record - PCIe Page Request record
 * For more infos on the PCIe Page Request queue see chapter 3.3.
 */
संरचना riscv_iommu_pq_लेखः {
      uint64_t hdr;
      uint64_t payload;
};
/* Header fields */
#परिभाषय RISCV_IOMMU_PREQ_HDR_PID        GENMASK_ULL(31, 12)
#परिभाषय RISCV_IOMMU_PREQ_HDR_PV         BIT_ULL(32)
#परिभाषय RISCV_IOMMU_PREQ_HDR_PRIV       BIT_ULL(33)
#परिभाषय RISCV_IOMMU_PREQ_HDR_EXEC       BIT_ULL(34)
#परिभाषय RISCV_IOMMU_PREQ_HDR_DID        GENMASK_ULL(63, 40)

/* Payload fields */
#परिभाषय RISCV_IOMMU_PREQ_PAYLOAD_R      BIT_ULL(0)
#परिभाषय RISCV_IOMMU_PREQ_PAYLOAD_W      BIT_ULL(1)
#परिभाषय RISCV_IOMMU_PREQ_PAYLOAD_L      BIT_ULL(2)
#परिभाषय RISCV_IOMMU_PREQ_PAYLOAD_M      GENMASK_ULL(2, 0)
#परिभाषय RISCV_IOMMU_PREQ_PRG_सूचकांक      GENMASK_ULL(11, 3)
#परिभाषय RISCV_IOMMU_PREQ_UADDR          GENMASK_ULL(63, 12)

/* Common field positions */
#परिभाषय RISCV_IOMMU_PPN_क्षेत्रम्           GENMASK_ULL(53, 10)
#परिभाषय RISCV_IOMMU_QUEUE_LOGSZ_क्षेत्रम्   GENMASK_ULL(4, 0)
#परिभाषय RISCV_IOMMU_QUEUE_सूचकांक_क्षेत्रम्   GENMASK_ULL(31, 0)
#परिभाषय RISCV_IOMMU_QUEUE_ENABLE        BIT(0)
#परिभाषय RISCV_IOMMU_QUEUE_INTR_ENABLE   BIT(1)
#परिभाषय RISCV_IOMMU_QUEUE_MEM_FAULT     BIT(8)
#परिभाषय RISCV_IOMMU_QUEUE_अतिप्रवाहः      BIT(9)
#परिभाषय RISCV_IOMMU_QUEUE_सक्रिय        BIT(16)
#परिभाषय RISCV_IOMMU_QUEUE_BUSY          BIT(17)
#परिभाषय RISCV_IOMMU_ATP_PPN_क्षेत्रम्       GENMASK_ULL(43, 0)
#परिभाषय RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्      GENMASK_ULL(63, 60)

/* 5.3 IOMMU Capabilities (64bits) */
#परिभाषय RISCV_IOMMU_REG_CAP             0x0000
#परिभाषय RISCV_IOMMU_CAP_संस्करणम्         GENMASK_ULL(7, 0)
#परिभाषय RISCV_IOMMU_CAP_SV32            BIT_ULL(8)
#परिभाषय RISCV_IOMMU_CAP_SV39            BIT_ULL(9)
#परिभाषय RISCV_IOMMU_CAP_SV48            BIT_ULL(10)
#परिभाषय RISCV_IOMMU_CAP_SV57            BIT_ULL(11)
#परिभाषय RISCV_IOMMU_CAP_SVRSW60T59B     BIT_ULL(14)
#परिभाषय RISCV_IOMMU_CAP_SV32X4          BIT_ULL(16)
#परिभाषय RISCV_IOMMU_CAP_SV39X4          BIT_ULL(17)
#परिभाषय RISCV_IOMMU_CAP_SV48X4          BIT_ULL(18)
#परिभाषय RISCV_IOMMU_CAP_SV57X4          BIT_ULL(19)
#परिभाषय RISCV_IOMMU_CAP_MSI_FLAT        BIT_ULL(22)
#परिभाषय RISCV_IOMMU_CAP_MSI_MRIF        BIT_ULL(23)
#परिभाषय RISCV_IOMMU_CAP_AMO_HWAD        BIT_ULL(24)
#परिभाषय RISCV_IOMMU_CAP_ATS             BIT_ULL(25)
#परिभाषय RISCV_IOMMU_CAP_T2GPA           BIT_ULL(26)
#परिभाषय RISCV_IOMMU_CAP_IGS             GENMASK_ULL(29, 28)
#परिभाषय RISCV_IOMMU_CAP_HPM             BIT_ULL(30)
#परिभाषय RISCV_IOMMU_CAP_DBG             BIT_ULL(31)
#परिभाषय RISCV_IOMMU_CAP_PAS             GENMASK_ULL(37, 32)
#परिभाषय RISCV_IOMMU_CAP_PD8             BIT_ULL(38)
#परिभाषय RISCV_IOMMU_CAP_PD17            BIT_ULL(39)
#परिभाषय RISCV_IOMMU_CAP_PD20            BIT_ULL(40)

गणना riscv_iommu_igs_modes {
    RISCV_IOMMU_CAP_IGS_MSI = 0,
    RISCV_IOMMU_CAP_IGS_WSI,
    RISCV_IOMMU_CAP_IGS_BOTH
};

/* 5.4 Features control register (32bits) */
#परिभाषय RISCV_IOMMU_REG_FCTL            0x0008
#परिभाषय RISCV_IOMMU_FCTL_BE             BIT(0)
#परिभाषय RISCV_IOMMU_FCTL_WSI            BIT(1)
#परिभाषय RISCV_IOMMU_FCTL_GXL            BIT(2)

/* 5.5 Device-directory-table pointer (64bits) */
#परिभाषय RISCV_IOMMU_REG_DDTP            0x0010
#परिभाषय RISCV_IOMMU_DDTP_बहुलकम्           GENMASK_ULL(3, 0)
#परिभाषय RISCV_IOMMU_DDTP_BUSY           BIT_ULL(4)
#परिभाषय RISCV_IOMMU_DDTP_PPN            RISCV_IOMMU_PPN_क्षेत्रम्

गणना riscv_iommu_ddtp_modes {
    RISCV_IOMMU_DDTP_बहुलकम्_OFF = 0,
    RISCV_IOMMU_DDTP_बहुलकम्_BARE = 1,
    RISCV_IOMMU_DDTP_बहुलकम्_1_LVL = 2,
    RISCV_IOMMU_DDTP_बहुलकम्_2_LVL = 3,
    RISCV_IOMMU_DDTP_बहुलकम्_3_LVL = 4,
    RISCV_IOMMU_DDTP_बहुलकम्_MAX = 4
};

/* 5.6 Command Queue Base (64bits) */
#परिभाषय RISCV_IOMMU_REG_CQB             0x0018
#परिभाषय RISCV_IOMMU_CQB_LOG2SZ          RISCV_IOMMU_QUEUE_LOGSZ_क्षेत्रम्
#परिभाषय RISCV_IOMMU_CQB_PPN             RISCV_IOMMU_PPN_क्षेत्रम्

/* 5.7 Command Queue head (32bits) */
#परिभाषय RISCV_IOMMU_REG_CQH             0x0020

/* 5.8 Command Queue tail (32bits) */
#परिभाषय RISCV_IOMMU_REG_CQT             0x0024

/* 5.9 Fault Queue Base (64bits) */
#परिभाषय RISCV_IOMMU_REG_FQB             0x0028
#परिभाषय RISCV_IOMMU_FQB_LOG2SZ          RISCV_IOMMU_QUEUE_LOGSZ_क्षेत्रम्
#परिभाषय RISCV_IOMMU_FQB_PPN             RISCV_IOMMU_PPN_क्षेत्रम्

/* 5.10 Fault Queue Head (32bits) */
#परिभाषय RISCV_IOMMU_REG_FQH             0x0030

/* 5.11 Fault Queue tail (32bits) */
#परिभाषय RISCV_IOMMU_REG_FQT             0x0034

/* 5.12 Page Request Queue base (64bits) */
#परिभाषय RISCV_IOMMU_REG_PQB             0x0038
#परिभाषय RISCV_IOMMU_PQB_LOG2SZ          RISCV_IOMMU_QUEUE_LOGSZ_क्षेत्रम्
#परिभाषय RISCV_IOMMU_PQB_PPN             RISCV_IOMMU_PPN_क्षेत्रम्

/* 5.13 Page Request Queue head (32bits) */
#परिभाषय RISCV_IOMMU_REG_PQH             0x0040

/* 5.14 Page Request Queue tail (32bits) */
#परिभाषय RISCV_IOMMU_REG_PQT             0x0044

/* 5.15 Command Queue CSR (32bits) */
#परिभाषय RISCV_IOMMU_REG_CQCSR           0x0048
#परिभाषय RISCV_IOMMU_CQCSR_CQEN          RISCV_IOMMU_QUEUE_ENABLE
#परिभाषय RISCV_IOMMU_CQCSR_CIE           RISCV_IOMMU_QUEUE_INTR_ENABLE
#परिभाषय RISCV_IOMMU_CQCSR_CQMF          RISCV_IOMMU_QUEUE_MEM_FAULT
#परिभाषय RISCV_IOMMU_CQCSR_CMD_गन्तव्यम्        BIT(9)
#परिभाषय RISCV_IOMMU_CQCSR_CMD_ILL       BIT(10)
#परिभाषय RISCV_IOMMU_CQCSR_FENCE_W_IP    BIT(11)
#परिभाषय RISCV_IOMMU_CQCSR_CQON          RISCV_IOMMU_QUEUE_सक्रिय
#परिभाषय RISCV_IOMMU_CQCSR_BUSY          RISCV_IOMMU_QUEUE_BUSY

/* 5.16 Fault Queue CSR (32bits) */
#परिभाषय RISCV_IOMMU_REG_FQCSR           0x004C
#परिभाषय RISCV_IOMMU_FQCSR_FQEN          RISCV_IOMMU_QUEUE_ENABLE
#परिभाषय RISCV_IOMMU_FQCSR_FIE           RISCV_IOMMU_QUEUE_INTR_ENABLE
#परिभाषय RISCV_IOMMU_FQCSR_FQMF          RISCV_IOMMU_QUEUE_MEM_FAULT
#परिभाषय RISCV_IOMMU_FQCSR_FQOF          RISCV_IOMMU_QUEUE_अतिप्रवाहः
#परिभाषय RISCV_IOMMU_FQCSR_FQON          RISCV_IOMMU_QUEUE_सक्रिय
#परिभाषय RISCV_IOMMU_FQCSR_BUSY          RISCV_IOMMU_QUEUE_BUSY

/* 5.17 Page Request Queue CSR (32bits) */
#परिभाषय RISCV_IOMMU_REG_PQCSR           0x0050
#परिभाषय RISCV_IOMMU_PQCSR_PQEN          RISCV_IOMMU_QUEUE_ENABLE
#परिभाषय RISCV_IOMMU_PQCSR_PIE           RISCV_IOMMU_QUEUE_INTR_ENABLE
#परिभाषय RISCV_IOMMU_PQCSR_PQMF          RISCV_IOMMU_QUEUE_MEM_FAULT
#परिभाषय RISCV_IOMMU_PQCSR_PQOF          RISCV_IOMMU_QUEUE_अतिप्रवाहः
#परिभाषय RISCV_IOMMU_PQCSR_PQON          RISCV_IOMMU_QUEUE_सक्रिय
#परिभाषय RISCV_IOMMU_PQCSR_BUSY          RISCV_IOMMU_QUEUE_BUSY

/* 5.18 Interrupt Pending Status (32bits) */
#परिभाषय RISCV_IOMMU_REG_IPSR            0x0054
#परिभाषय RISCV_IOMMU_IPSR_CIP            BIT(0)
#परिभाषय RISCV_IOMMU_IPSR_FIP            BIT(1)
#परिभाषय RISCV_IOMMU_IPSR_PMIP           BIT(2)
#परिभाषय RISCV_IOMMU_IPSR_PIP            BIT(3)

गणना {
    RISCV_IOMMU_INTR_CQ,
    RISCV_IOMMU_INTR_FQ,
    RISCV_IOMMU_INTR_PM,
    RISCV_IOMMU_INTR_PQ,
    RISCV_IOMMU_INTR_गणनम्
};

#परिभाषय RISCV_IOMMU_IOCOUNT_अङ्कः         31

/* 5.19 Performance monitoring counter overflow status (32bits) */
#परिभाषय RISCV_IOMMU_REG_IOCOUNTOVF      0x0058
#परिभाषय RISCV_IOMMU_IOCOUNTOVF_CY       BIT(0)

/* 5.20 Performance monitoring counter inhibits (32bits) */
#परिभाषय RISCV_IOMMU_REG_IOCOUNTINH      0x005C
#परिभाषय RISCV_IOMMU_IOCOUNTINH_CY       BIT(0)

/* 5.21 Performance monitoring cycles counter (64bits) */
#परिभाषय RISCV_IOMMU_REG_IOHPMCYCLES     0x0060
#परिभाषय RISCV_IOMMU_IOHPMCYCLES_गणक GENMASK_ULL(62, 0)
#परिभाषय RISCV_IOMMU_IOHPMCYCLES_OVF     BIT_ULL(63)

/* 5.22 Performance monitoring event counters (31 * 64bits) */
#परिभाषय RISCV_IOMMU_REG_IOHPMCTR_आधारः   0x0068
#परिभाषय RISCV_IOMMU_REG_IOHPMCTR(_n)    \
    (RISCV_IOMMU_REG_IOHPMCTR_आधारः + (_n * 0x8))

/* 5.23 Performance monitoring event selectors (31 * 64bits) */
#परिभाषय RISCV_IOMMU_REG_IOHPMEVT_आधारः   0x0160
#परिभाषय RISCV_IOMMU_REG_IOHPMEVT(_n)    \
    (RISCV_IOMMU_REG_IOHPMEVT_आधारः + (_n * 0x8))
#परिभाषय RISCV_IOMMU_IOHPMEVT_EVENT_ID   GENMASK_ULL(14, 0)
#परिभाषय RISCV_IOMMU_IOHPMEVT_DMASK      BIT_ULL(15)
#परिभाषय RISCV_IOMMU_IOHPMEVT_PID_PSCID  GENMASK_ULL(35, 16)
#परिभाषय RISCV_IOMMU_IOHPMEVT_DID_GSCID  GENMASK_ULL(59, 36)
#परिभाषय RISCV_IOMMU_IOHPMEVT_PV_PSCV    BIT_ULL(60)
#परिभाषय RISCV_IOMMU_IOHPMEVT_DV_GSCV    BIT_ULL(61)
#परिभाषय RISCV_IOMMU_IOHPMEVT_IDT        BIT_ULL(62)
#परिभाषय RISCV_IOMMU_IOHPMEVT_सम्बन्धि         BIT_ULL(63)

गणना RISCV_IOMMU_HPMEVENT_id {
    RISCV_IOMMU_HPMEVENT_INVALID    = 0,
    RISCV_IOMMU_HPMEVENT_URQ        = 1,
    RISCV_IOMMU_HPMEVENT_TRQ        = 2,
    RISCV_IOMMU_HPMEVENT_ATS_RQ     = 3,
    RISCV_IOMMU_HPMEVENT_TLB_अलब्धिः   = 4,
    RISCV_IOMMU_HPMEVENT_DD_WALK    = 5,
    RISCV_IOMMU_HPMEVENT_PD_WALK    = 6,
    RISCV_IOMMU_HPMEVENT_S_VS_WALKS = 7,
    RISCV_IOMMU_HPMEVENT_G_WALKS    = 8,
    RISCV_IOMMU_HPMEVENT_MAX        = 9
};

/* 5.24 Translation request IOVA (64bits) */
#परिभाषय RISCV_IOMMU_REG_TR_REQ_IOVA     0x0258

/* 5.25 Translation request control (64bits) */
#परिभाषय RISCV_IOMMU_REG_TR_REQ_CTL      0x0260
#परिभाषय RISCV_IOMMU_TR_REQ_CTL_GO_BUSY  BIT_ULL(0)
#परिभाषय RISCV_IOMMU_TR_REQ_CTL_NW       BIT_ULL(3)
#परिभाषय RISCV_IOMMU_TR_REQ_CTL_PID      GENMASK_ULL(31, 12)
#परिभाषय RISCV_IOMMU_TR_REQ_CTL_DID      GENMASK_ULL(63, 40)

/* 5.26 Translation request response (64bits) */
#परिभाषय RISCV_IOMMU_REG_TR_RESPONSE     0x0268
#परिभाषय RISCV_IOMMU_TR_RESPONSE_FAULT   BIT_ULL(0)
#परिभाषय RISCV_IOMMU_TR_RESPONSE_S       BIT_ULL(9)
#परिभाषय RISCV_IOMMU_TR_RESPONSE_PPN     RISCV_IOMMU_PPN_क्षेत्रम्

/* 5.27 Interrupt cause to vector (64bits) */
#परिभाषय RISCV_IOMMU_REG_ICVEC           0x02F8
#परिभाषय RISCV_IOMMU_ICVEC_CIV           GENMASK_ULL(3, 0)
#परिभाषय RISCV_IOMMU_ICVEC_FIV           GENMASK_ULL(7, 4)
#परिभाषय RISCV_IOMMU_ICVEC_PMIV          GENMASK_ULL(11, 8)
#परिभाषय RISCV_IOMMU_ICVEC_PIV           GENMASK_ULL(15, 12)

/* 5.28 MSI Configuration table (32 * 64bits) */
#परिभाषय RISCV_IOMMU_REG_MSI_CONFIG      0x0300

#परिभाषय RISCV_IOMMU_REG_आकार            0x1000

#परिभाषय RISCV_IOMMU_DDTE_VALID          BIT_ULL(0)
#परिभाषय RISCV_IOMMU_DDTE_PPN            RISCV_IOMMU_PPN_क्षेत्रम्

/* Struct riscv_iommu_dc - Device Context - section 2.1 */
संरचना riscv_iommu_dc {
      uint64_t tc;
      uint64_t iohgatp;
      uint64_t ta;
      uint64_t fsc;
      uint64_t msiptp;
      uint64_t msi_addr_आवरणम्;
      uint64_t msi_addr_आदर्शः;
      uint64_t _reserved;
};

/* Translation control fields */
#परिभाषय RISCV_IOMMU_DC_TC_V             BIT_ULL(0)
#परिभाषय RISCV_IOMMU_DC_TC_EN_ATS        BIT_ULL(1)
#परिभाषय RISCV_IOMMU_DC_TC_EN_PRI        BIT_ULL(2)
#परिभाषय RISCV_IOMMU_DC_TC_T2GPA         BIT_ULL(3)
#परिभाषय RISCV_IOMMU_DC_TC_DTF           BIT_ULL(4)
#परिभाषय RISCV_IOMMU_DC_TC_PDTV          BIT_ULL(5)
#परिभाषय RISCV_IOMMU_DC_TC_PRPR          BIT_ULL(6)
#परिभाषय RISCV_IOMMU_DC_TC_GADE          BIT_ULL(7)
#परिभाषय RISCV_IOMMU_DC_TC_SADE          BIT_ULL(8)
#परिभाषय RISCV_IOMMU_DC_TC_DPE           BIT_ULL(9)
#परिभाषय RISCV_IOMMU_DC_TC_SBE           BIT_ULL(10)
#परिभाषय RISCV_IOMMU_DC_TC_SXL           BIT_ULL(11)

/* Second-stage (aka G-stage) context fields */
#परिभाषय RISCV_IOMMU_DC_IOHGATP_PPN      RISCV_IOMMU_ATP_PPN_क्षेत्रम्
#परिभाषय RISCV_IOMMU_DC_IOHGATP_GSCID    GENMASK_ULL(59, 44)
#परिभाषय RISCV_IOMMU_DC_IOHGATP_बहुलकम्     RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्

#परिभाषय RISCV_IOMMU_DC_TC_RESERVED      (GENMASK_ULL(23, 12) \
                                         | GENMASK_ULL(63, 32))

गणना riscv_iommu_dc_iohgatp_modes {
    RISCV_IOMMU_DC_IOHGATP_बहुलकम्_BARE = 0,
    RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV32_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV39_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV48_X4 = 9,
    RISCV_IOMMU_DC_IOHGATP_बहुलकम्_SV57_X4 = 10
};

/* Translation attributes fields */
#परिभाषय RISCV_IOMMU_DC_TA_PSCID         GENMASK_ULL(31, 12)

/* First-stage context fields */
#परिभाषय RISCV_IOMMU_DC_FSC_PPN          RISCV_IOMMU_ATP_PPN_क्षेत्रम्
#परिभाषय RISCV_IOMMU_DC_FSC_बहुलकम्         RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्

/* Generic I/O MMU command structure - check section 3.1 */
संरचना riscv_iommu_command {
    uint64_t dword0;
    uint64_t dword1;
};

#परिभाषय RISCV_IOMMU_CMD_OPCODE          GENMASK_ULL(6, 0)
#परिभाषय RISCV_IOMMU_CMD_FUNC            GENMASK_ULL(9, 7)

#परिभाषय RISCV_IOMMU_CMD_IOTINVAL_OPCODE         1
#परिभाषय RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA       0
#परिभाषय RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA      1
#परिभाषय RISCV_IOMMU_CMD_IOTINVAL_AV     BIT_ULL(10)
#परिभाषय RISCV_IOMMU_CMD_IOTINVAL_PSCID  GENMASK_ULL(31, 12)
#परिभाषय RISCV_IOMMU_CMD_IOTINVAL_PSCV   BIT_ULL(32)
#परिभाषय RISCV_IOMMU_CMD_IOTINVAL_GV     BIT_ULL(33)
#परिभाषय RISCV_IOMMU_CMD_IOTINVAL_GSCID  GENMASK_ULL(59, 44)

#परिभाषय RISCV_IOMMU_CMD_IOFENCE_OPCODE          2
#परिभाषय RISCV_IOMMU_CMD_IOFENCE_FUNC_C          0
#परिभाषय RISCV_IOMMU_CMD_IOFENCE_AV      BIT_ULL(10)
#परिभाषय RISCV_IOMMU_CMD_IOFENCE_RESERVED GENMASK_ULL(31, 14)
#परिभाषय RISCV_IOMMU_CMD_IOFENCE_दत्त    GENMASK_ULL(63, 32)

#परिभाषय RISCV_IOMMU_CMD_IODIR_OPCODE            3
#परिभाषय RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT    0
#परिभाषय RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT    1
#परिभाषय RISCV_IOMMU_CMD_IODIR_PID       GENMASK_ULL(31, 12)
#परिभाषय RISCV_IOMMU_CMD_IODIR_DV        BIT_ULL(33)
#परिभाषय RISCV_IOMMU_CMD_IODIR_DID       GENMASK_ULL(63, 40)

/* 3.1.4 I/O MMU PCIe ATS */
#परिभाषय RISCV_IOMMU_CMD_ATS_OPCODE              4
#परिभाषय RISCV_IOMMU_CMD_ATS_FUNC_INVAL          0
#परिभाषय RISCV_IOMMU_CMD_ATS_FUNC_PRGR           1
#परिभाषय RISCV_IOMMU_CMD_ATS_PID         GENMASK_ULL(31, 12)
#परिभाषय RISCV_IOMMU_CMD_ATS_PV          BIT_ULL(32)
#परिभाषय RISCV_IOMMU_CMD_ATS_DSV         BIT_ULL(33)
#परिभाषय RISCV_IOMMU_CMD_ATS_RID         GENMASK_ULL(55, 40)
#परिभाषय RISCV_IOMMU_CMD_ATS_DSEG        GENMASK_ULL(63, 56)
/* dword1 is the ATS payload, two different payload types for INVAL and PRGR */

/* ATS.PRGR payload */
#परिभाषय RISCV_IOMMU_CMD_ATS_PRGR_RESP_संहिता      GENMASK_ULL(47, 44)

गणना riscv_iommu_dc_fsc_atp_modes {
    RISCV_IOMMU_DC_FSC_बहुलकम्_BARE = 0,
    RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV32 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV39 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV48 = 9,
    RISCV_IOMMU_DC_FSC_IOSATP_बहुलकम्_SV57 = 10,
    RISCV_IOMMU_DC_FSC_PDTP_बहुलकम्_PD8 = 1,
    RISCV_IOMMU_DC_FSC_PDTP_बहुलकम्_PD17 = 2,
    RISCV_IOMMU_DC_FSC_PDTP_बहुलकम्_PD20 = 3
};

गणना riscv_iommu_fq_causes {
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
    RISCV_IOMMU_FQ_CAUSE_DDT_आरोपय_FAULT       = 257,
    RISCV_IOMMU_FQ_CAUSE_DDT_INVALID          = 258,
    RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED    = 259,
    RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED        = 260,
    RISCV_IOMMU_FQ_CAUSE_MSI_आरोपय_FAULT       = 261,
    RISCV_IOMMU_FQ_CAUSE_MSI_INVALID          = 262,
    RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED    = 263,
    RISCV_IOMMU_FQ_CAUSE_MRIF_FAULT           = 264,
    RISCV_IOMMU_FQ_CAUSE_PDT_आरोपय_FAULT       = 265,
    RISCV_IOMMU_FQ_CAUSE_PDT_INVALID          = 266,
    RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED    = 267,
    RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED        = 268,
    RISCV_IOMMU_FQ_CAUSE_PDT_CORRUPTED        = 269,
    RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED     = 270,
    RISCV_IOMMU_FQ_CAUSE_MRIF_CORRUIPTED      = 271,
    RISCV_IOMMU_FQ_CAUSE_आन्तरिकम्_DP_ERROR    = 272,
    RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT         = 273,
    RISCV_IOMMU_FQ_CAUSE_PT_CORRUPTED         = 274
};

/* MSI page table pointer */
#परिभाषय RISCV_IOMMU_DC_MSIPTP_PPN       RISCV_IOMMU_ATP_PPN_क्षेत्रम्
#परिभाषय RISCV_IOMMU_DC_MSIPTP_बहुलकम्      RISCV_IOMMU_ATP_बहुलकम्_क्षेत्रम्
#परिभाषय RISCV_IOMMU_DC_MSIPTP_बहुलकम्_OFF  0
#परिभाषय RISCV_IOMMU_DC_MSIPTP_बहुलकम्_FLAT 1

/* 2.2 Process Directory Table */
#परिभाषय RISCV_IOMMU_PDTE_VALID          BIT_ULL(0)
#परिभाषय RISCV_IOMMU_PDTE_PPN            RISCV_IOMMU_PPN_क्षेत्रम्

/* Translation attributes fields */
#परिभाषय RISCV_IOMMU_PC_TA_V             BIT_ULL(0)
#परिभाषय RISCV_IOMMU_PC_TA_RESERVED      GENMASK_ULL(63, 32)

/* First stage context fields */
#परिभाषय RISCV_IOMMU_PC_FSC_PPN          RISCV_IOMMU_ATP_PPN_क्षेत्रम्
#परिभाषय RISCV_IOMMU_PC_FSC_RESERVED     GENMASK_ULL(59, 44)

गणना riscv_iommu_fq_ttypes {
    RISCV_IOMMU_FQ_TTYPE_NONE = 0,
    RISCV_IOMMU_FQ_TTYPE_UADDR_INST_FETCH = 1,
    RISCV_IOMMU_FQ_TTYPE_UADDR_RD = 2,
    RISCV_IOMMU_FQ_TTYPE_UADDR_WR = 3,
    RISCV_IOMMU_FQ_TTYPE_TADDR_INST_FETCH = 5,
    RISCV_IOMMU_FQ_TTYPE_TADDR_RD = 6,
    RISCV_IOMMU_FQ_TTYPE_TADDR_WR = 7,
    RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ = 8,
    RISCV_IOMMU_FW_TTYPE_PCIE_सन्देशः_REQ = 9,
};

/*
 * struct riscv_iommu_msi_pte - MSI Page Table Entry
 */
संरचना riscv_iommu_msi_pte {
      uint64_t pte;
      uint64_t mrif_info;
};

/* Fields on pte */
#परिभाषय RISCV_IOMMU_MSI_PTE_V           BIT_ULL(0)
#परिभाषय RISCV_IOMMU_MSI_PTE_M           GENMASK_ULL(2, 1)

#परिभाषय RISCV_IOMMU_MSI_PTE_M_MRIF      1
#परिभाषय RISCV_IOMMU_MSI_PTE_M_BASIC     3

/* When M == 1 (MRIF mode) */
#परिभाषय RISCV_IOMMU_MSI_PTE_MRIF_ADDR   GENMASK_ULL(53, 7)
/* When M == 3 (basic mode) */
#परिभाषय RISCV_IOMMU_MSI_PTE_PPN         RISCV_IOMMU_PPN_क्षेत्रम्
#परिभाषय RISCV_IOMMU_MSI_PTE_C           BIT_ULL(63)

/* Fields on mrif_info */
#परिभाषय RISCV_IOMMU_MSI_MRIF_NID        GENMASK_ULL(9, 0)
#परिभाषय RISCV_IOMMU_MSI_MRIF_NPPN       RISCV_IOMMU_PPN_क्षेत्रम्
#परिभाषय RISCV_IOMMU_MSI_MRIF_NID_MSB    BIT_ULL(60)

#यदि_अन्तः /* _RISCV_IOMMU_BITS_H_ */
