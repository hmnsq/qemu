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

#నిర్వచితం_కాకపోతే HW_RISCV_IOMMU_బిట్లు_H
#నిర్వచించు HW_RISCV_IOMMU_బిట్లు_H

#నిర్వచించు RISCV_IOMMU_SPEC_DOT_VER 0x010

#నిర్వచితం_కాకపోతే GENMASK_ULL
#నిర్వచించు GENMASK_ULL(h, l) (((~0ULL) >> (63 - (h) + (l))) << (l))
#అయితే_ముగింపు

/*
 * struct riscv_iommu_fq_record - Fault/Event Queue Record
 * See section 3.2 for more info.
 */
నిర్మాణం riscv_iommu_fq_రికార్డు {
    uint64_t hdr;
    uint64_t _reserved;
    uint64_t iotval;
    uint64_t iotval2;
};
/* Header fields */
#నిర్వచించు RISCV_IOMMU_FQ_HDR_CAUSE        GENMASK_ULL(11, 0)
#నిర్వచించు RISCV_IOMMU_FQ_HDR_PID          GENMASK_ULL(31, 12)
#నిర్వచించు RISCV_IOMMU_FQ_HDR_PV           బిట్టు_ULL(32)
#నిర్వచించు RISCV_IOMMU_FQ_HDR_TTYPE        GENMASK_ULL(39, 34)
#నిర్వచించు RISCV_IOMMU_FQ_HDR_DID          GENMASK_ULL(63, 40)

/*
 * struct riscv_iommu_pq_record - PCIe Page Request record
 * For more infos on the PCIe Page Request queue see chapter 3.3.
 */
నిర్మాణం riscv_iommu_pq_రికార్డు {
      uint64_t hdr;
      uint64_t payload;
};
/* Header fields */
#నిర్వచించు RISCV_IOMMU_PREQ_HDR_PID        GENMASK_ULL(31, 12)
#నిర్వచించు RISCV_IOMMU_PREQ_HDR_PV         బిట్టు_ULL(32)
#నిర్వచించు RISCV_IOMMU_PREQ_HDR_PRIV       బిట్టు_ULL(33)
#నిర్వచించు RISCV_IOMMU_PREQ_HDR_EXEC       బిట్టు_ULL(34)
#నిర్వచించు RISCV_IOMMU_PREQ_HDR_DID        GENMASK_ULL(63, 40)

/* Payload fields */
#నిర్వచించు RISCV_IOMMU_PREQ_PAYLOAD_R      బిట్టు_ULL(0)
#నిర్వచించు RISCV_IOMMU_PREQ_PAYLOAD_W      బిట్టు_ULL(1)
#నిర్వచించు RISCV_IOMMU_PREQ_PAYLOAD_L      బిట్టు_ULL(2)
#నిర్వచించు RISCV_IOMMU_PREQ_PAYLOAD_M      GENMASK_ULL(2, 0)
#నిర్వచించు RISCV_IOMMU_PREQ_PRG_సూచీ      GENMASK_ULL(11, 3)
#నిర్వచించు RISCV_IOMMU_PREQ_UADDR          GENMASK_ULL(63, 12)

/* Common field positions */
#నిర్వచించు RISCV_IOMMU_PPN_క్షేత్రం           GENMASK_ULL(53, 10)
#నిర్వచించు RISCV_IOMMU_వరుస_LOGSZ_క్షేత్రం   GENMASK_ULL(4, 0)
#నిర్వచించు RISCV_IOMMU_వరుస_సూచీ_క్షేత్రం   GENMASK_ULL(31, 0)
#నిర్వచించు RISCV_IOMMU_వరుస_ENABLE        బిట్టు(0)
#నిర్వచించు RISCV_IOMMU_వరుస_INTR_ENABLE   బిట్టు(1)
#నిర్వచించు RISCV_IOMMU_వరుస_MEM_FAULT     బిట్టు(8)
#నిర్వచించు RISCV_IOMMU_వరుస_అధికప్రవాహం      బిట్టు(9)
#నిర్వచించు RISCV_IOMMU_వరుస_చురుకైన        బిట్టు(16)
#నిర్వచించు RISCV_IOMMU_వరుస_BUSY          బిట్టు(17)
#నిర్వచించు RISCV_IOMMU_ATP_PPN_క్షేత్రం       GENMASK_ULL(43, 0)
#నిర్వచించు RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం      GENMASK_ULL(63, 60)

/* 5.3 IOMMU Capabilities (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_CAP             0x0000
#నిర్వచించు RISCV_IOMMU_CAP_సంస్కరణ         GENMASK_ULL(7, 0)
#నిర్వచించు RISCV_IOMMU_CAP_SV32            బిట్టు_ULL(8)
#నిర్వచించు RISCV_IOMMU_CAP_SV39            బిట్టు_ULL(9)
#నిర్వచించు RISCV_IOMMU_CAP_SV48            బిట్టు_ULL(10)
#నిర్వచించు RISCV_IOMMU_CAP_SV57            బిట్టు_ULL(11)
#నిర్వచించు RISCV_IOMMU_CAP_SVRSW60T59B     బిట్టు_ULL(14)
#నిర్వచించు RISCV_IOMMU_CAP_SV32X4          బిట్టు_ULL(16)
#నిర్వచించు RISCV_IOMMU_CAP_SV39X4          బిట్టు_ULL(17)
#నిర్వచించు RISCV_IOMMU_CAP_SV48X4          బిట్టు_ULL(18)
#నిర్వచించు RISCV_IOMMU_CAP_SV57X4          బిట్టు_ULL(19)
#నిర్వచించు RISCV_IOMMU_CAP_MSI_FLAT        బిట్టు_ULL(22)
#నిర్వచించు RISCV_IOMMU_CAP_MSI_MRIF        బిట్టు_ULL(23)
#నిర్వచించు RISCV_IOMMU_CAP_AMO_HWAD        బిట్టు_ULL(24)
#నిర్వచించు RISCV_IOMMU_CAP_ATS             బిట్టు_ULL(25)
#నిర్వచించు RISCV_IOMMU_CAP_T2GPA           బిట్టు_ULL(26)
#నిర్వచించు RISCV_IOMMU_CAP_IGS             GENMASK_ULL(29, 28)
#నిర్వచించు RISCV_IOMMU_CAP_HPM             బిట్టు_ULL(30)
#నిర్వచించు RISCV_IOMMU_CAP_DBG             బిట్టు_ULL(31)
#నిర్వచించు RISCV_IOMMU_CAP_PAS             GENMASK_ULL(37, 32)
#నిర్వచించు RISCV_IOMMU_CAP_PD8             బిట్టు_ULL(38)
#నిర్వచించు RISCV_IOMMU_CAP_PD17            బిట్టు_ULL(39)
#నిర్వచించు RISCV_IOMMU_CAP_PD20            బిట్టు_ULL(40)

గణన riscv_iommu_igs_modes {
    RISCV_IOMMU_CAP_IGS_MSI = 0,
    RISCV_IOMMU_CAP_IGS_WSI,
    RISCV_IOMMU_CAP_IGS_BOTH
};

/* 5.4 Features control register (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_FCTL            0x0008
#నిర్వచించు RISCV_IOMMU_FCTL_BE             బిట్టు(0)
#నిర్వచించు RISCV_IOMMU_FCTL_WSI            బిట్టు(1)
#నిర్వచించు RISCV_IOMMU_FCTL_GXL            బిట్టు(2)

/* 5.5 Device-directory-table pointer (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_DDTP            0x0010
#నిర్వచించు RISCV_IOMMU_DDTP_బహుళకం           GENMASK_ULL(3, 0)
#నిర్వచించు RISCV_IOMMU_DDTP_BUSY           బిట్టు_ULL(4)
#నిర్వచించు RISCV_IOMMU_DDTP_PPN            RISCV_IOMMU_PPN_క్షేత్రం

గణన riscv_iommu_ddtp_modes {
    RISCV_IOMMU_DDTP_బహుళకం_OFF = 0,
    RISCV_IOMMU_DDTP_బహుళకం_BARE = 1,
    RISCV_IOMMU_DDTP_బహుళకం_1_LVL = 2,
    RISCV_IOMMU_DDTP_బహుళకం_2_LVL = 3,
    RISCV_IOMMU_DDTP_బహుళకం_3_LVL = 4,
    RISCV_IOMMU_DDTP_బహుళకం_MAX = 4
};

/* 5.6 Command Queue Base (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_CQB             0x0018
#నిర్వచించు RISCV_IOMMU_CQB_LOG2SZ          RISCV_IOMMU_వరుస_LOGSZ_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_CQB_PPN             RISCV_IOMMU_PPN_క్షేత్రం

/* 5.7 Command Queue head (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_CQH             0x0020

/* 5.8 Command Queue tail (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_CQT             0x0024

/* 5.9 Fault Queue Base (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_FQB             0x0028
#నిర్వచించు RISCV_IOMMU_FQB_LOG2SZ          RISCV_IOMMU_వరుస_LOGSZ_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_FQB_PPN             RISCV_IOMMU_PPN_క్షేత్రం

/* 5.10 Fault Queue Head (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_FQH             0x0030

/* 5.11 Fault Queue tail (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_FQT             0x0034

/* 5.12 Page Request Queue base (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_PQB             0x0038
#నిర్వచించు RISCV_IOMMU_PQB_LOG2SZ          RISCV_IOMMU_వరుస_LOGSZ_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_PQB_PPN             RISCV_IOMMU_PPN_క్షేత్రం

/* 5.13 Page Request Queue head (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_PQH             0x0040

/* 5.14 Page Request Queue tail (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_PQT             0x0044

/* 5.15 Command Queue CSR (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_CQCSR           0x0048
#నిర్వచించు RISCV_IOMMU_CQCSR_CQEN          RISCV_IOMMU_వరుస_ENABLE
#నిర్వచించు RISCV_IOMMU_CQCSR_CIE           RISCV_IOMMU_వరుస_INTR_ENABLE
#నిర్వచించు RISCV_IOMMU_CQCSR_CQMF          RISCV_IOMMU_వరుస_MEM_FAULT
#నిర్వచించు RISCV_IOMMU_CQCSR_CMD_గమ్యం        బిట్టు(9)
#నిర్వచించు RISCV_IOMMU_CQCSR_CMD_ILL       బిట్టు(10)
#నిర్వచించు RISCV_IOMMU_CQCSR_FENCE_W_IP    బిట్టు(11)
#నిర్వచించు RISCV_IOMMU_CQCSR_CQON          RISCV_IOMMU_వరుస_చురుకైన
#నిర్వచించు RISCV_IOMMU_CQCSR_BUSY          RISCV_IOMMU_వరుస_BUSY

/* 5.16 Fault Queue CSR (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_FQCSR           0x004C
#నిర్వచించు RISCV_IOMMU_FQCSR_FQEN          RISCV_IOMMU_వరుస_ENABLE
#నిర్వచించు RISCV_IOMMU_FQCSR_FIE           RISCV_IOMMU_వరుస_INTR_ENABLE
#నిర్వచించు RISCV_IOMMU_FQCSR_FQMF          RISCV_IOMMU_వరుస_MEM_FAULT
#నిర్వచించు RISCV_IOMMU_FQCSR_FQOF          RISCV_IOMMU_వరుస_అధికప్రవాహం
#నిర్వచించు RISCV_IOMMU_FQCSR_FQON          RISCV_IOMMU_వరుస_చురుకైన
#నిర్వచించు RISCV_IOMMU_FQCSR_BUSY          RISCV_IOMMU_వరుస_BUSY

/* 5.17 Page Request Queue CSR (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_PQCSR           0x0050
#నిర్వచించు RISCV_IOMMU_PQCSR_PQEN          RISCV_IOMMU_వరుస_ENABLE
#నిర్వచించు RISCV_IOMMU_PQCSR_PIE           RISCV_IOMMU_వరుస_INTR_ENABLE
#నిర్వచించు RISCV_IOMMU_PQCSR_PQMF          RISCV_IOMMU_వరుస_MEM_FAULT
#నిర్వచించు RISCV_IOMMU_PQCSR_PQOF          RISCV_IOMMU_వరుస_అధికప్రవాహం
#నిర్వచించు RISCV_IOMMU_PQCSR_PQON          RISCV_IOMMU_వరుస_చురుకైన
#నిర్వచించు RISCV_IOMMU_PQCSR_BUSY          RISCV_IOMMU_వరుస_BUSY

/* 5.18 Interrupt Pending Status (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_IPSR            0x0054
#నిర్వచించు RISCV_IOMMU_IPSR_CIP            బిట్టు(0)
#నిర్వచించు RISCV_IOMMU_IPSR_FIP            బిట్టు(1)
#నిర్వచించు RISCV_IOMMU_IPSR_PMIP           బిట్టు(2)
#నిర్వచించు RISCV_IOMMU_IPSR_PIP            బిట్టు(3)

గణన {
    RISCV_IOMMU_INTR_CQ,
    RISCV_IOMMU_INTR_FQ,
    RISCV_IOMMU_INTR_PM,
    RISCV_IOMMU_INTR_PQ,
    RISCV_IOMMU_INTR_లెక్క
};

#నిర్వచించు RISCV_IOMMU_IOCOUNT_అంకె         31

/* 5.19 Performance monitoring counter overflow status (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_IOCOUNTOVF      0x0058
#నిర్వచించు RISCV_IOMMU_IOCOUNTOVF_CY       బిట్టు(0)

/* 5.20 Performance monitoring counter inhibits (32bits) */
#నిర్వచించు RISCV_IOMMU_REG_IOCOUNTINH      0x005C
#నిర్వచించు RISCV_IOMMU_IOCOUNTINH_CY       బిట్టు(0)

/* 5.21 Performance monitoring cycles counter (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_IOHPMCYCLES     0x0060
#నిర్వచించు RISCV_IOMMU_IOHPMCYCLES_COUNTER GENMASK_ULL(62, 0)
#నిర్వచించు RISCV_IOMMU_IOHPMCYCLES_OVF     బిట్టు_ULL(63)

/* 5.22 Performance monitoring event counters (31 * 64bits) */
#నిర్వచించు RISCV_IOMMU_REG_IOHPMCTR_పునాది   0x0068
#నిర్వచించు RISCV_IOMMU_REG_IOHPMCTR(_n)    \
    (RISCV_IOMMU_REG_IOHPMCTR_పునాది + (_n * 0x8))

/* 5.23 Performance monitoring event selectors (31 * 64bits) */
#నిర్వచించు RISCV_IOMMU_REG_IOHPMEVT_పునాది   0x0160
#నిర్వచించు RISCV_IOMMU_REG_IOHPMEVT(_n)    \
    (RISCV_IOMMU_REG_IOHPMEVT_పునాది + (_n * 0x8))
#నిర్వచించు RISCV_IOMMU_IOHPMEVT_EVENT_ID   GENMASK_ULL(14, 0)
#నిర్వచించు RISCV_IOMMU_IOHPMEVT_DMASK      బిట్టు_ULL(15)
#నిర్వచించు RISCV_IOMMU_IOHPMEVT_PID_PSCID  GENMASK_ULL(35, 16)
#నిర్వచించు RISCV_IOMMU_IOHPMEVT_DID_GSCID  GENMASK_ULL(59, 36)
#నిర్వచించు RISCV_IOMMU_IOHPMEVT_PV_PSCV    బిట్టు_ULL(60)
#నిర్వచించు RISCV_IOMMU_IOHPMEVT_DV_GSCV    బిట్టు_ULL(61)
#నిర్వచించు RISCV_IOMMU_IOHPMEVT_IDT        బిట్టు_ULL(62)
#నిర్వచించు RISCV_IOMMU_IOHPMEVT_యొక్క         బిట్టు_ULL(63)

గణన RISCV_IOMMU_HPMEVENT_id {
    RISCV_IOMMU_HPMEVENT_INVALID    = 0,
    RISCV_IOMMU_HPMEVENT_URQ        = 1,
    RISCV_IOMMU_HPMEVENT_TRQ        = 2,
    RISCV_IOMMU_HPMEVENT_ATS_RQ     = 3,
    RISCV_IOMMU_HPMEVENT_TLB_తప్పిపోవు   = 4,
    RISCV_IOMMU_HPMEVENT_DD_WALK    = 5,
    RISCV_IOMMU_HPMEVENT_PD_WALK    = 6,
    RISCV_IOMMU_HPMEVENT_S_VS_WALKS = 7,
    RISCV_IOMMU_HPMEVENT_G_WALKS    = 8,
    RISCV_IOMMU_HPMEVENT_MAX        = 9
};

/* 5.24 Translation request IOVA (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_TR_REQ_IOVA     0x0258

/* 5.25 Translation request control (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_TR_REQ_CTL      0x0260
#నిర్వచించు RISCV_IOMMU_TR_REQ_CTL_GO_BUSY  బిట్టు_ULL(0)
#నిర్వచించు RISCV_IOMMU_TR_REQ_CTL_NW       బిట్టు_ULL(3)
#నిర్వచించు RISCV_IOMMU_TR_REQ_CTL_PID      GENMASK_ULL(31, 12)
#నిర్వచించు RISCV_IOMMU_TR_REQ_CTL_DID      GENMASK_ULL(63, 40)

/* 5.26 Translation request response (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_TR_RESPONSE     0x0268
#నిర్వచించు RISCV_IOMMU_TR_RESPONSE_FAULT   బిట్టు_ULL(0)
#నిర్వచించు RISCV_IOMMU_TR_RESPONSE_S       బిట్టు_ULL(9)
#నిర్వచించు RISCV_IOMMU_TR_RESPONSE_PPN     RISCV_IOMMU_PPN_క్షేత్రం

/* 5.27 Interrupt cause to vector (64bits) */
#నిర్వచించు RISCV_IOMMU_REG_ICVEC           0x02F8
#నిర్వచించు RISCV_IOMMU_ICVEC_CIV           GENMASK_ULL(3, 0)
#నిర్వచించు RISCV_IOMMU_ICVEC_FIV           GENMASK_ULL(7, 4)
#నిర్వచించు RISCV_IOMMU_ICVEC_PMIV          GENMASK_ULL(11, 8)
#నిర్వచించు RISCV_IOMMU_ICVEC_PIV           GENMASK_ULL(15, 12)

/* 5.28 MSI Configuration table (32 * 64bits) */
#నిర్వచించు RISCV_IOMMU_REG_MSI_CONFIG      0x0300

#నిర్వచించు RISCV_IOMMU_REG_కొలత            0x1000

#నిర్వచించు RISCV_IOMMU_DDTE_VALID          బిట్టు_ULL(0)
#నిర్వచించు RISCV_IOMMU_DDTE_PPN            RISCV_IOMMU_PPN_క్షేత్రం

/* Struct riscv_iommu_dc - Device Context - section 2.1 */
నిర్మాణం riscv_iommu_dc {
      uint64_t tc;
      uint64_t iohgatp;
      uint64_t ta;
      uint64_t fsc;
      uint64_t msiptp;
      uint64_t msi_addr_ముసుగు;
      uint64_t msi_addr_నమూనా;
      uint64_t _reserved;
};

/* Translation control fields */
#నిర్వచించు RISCV_IOMMU_DC_TC_V             బిట్టు_ULL(0)
#నిర్వచించు RISCV_IOMMU_DC_TC_EN_ATS        బిట్టు_ULL(1)
#నిర్వచించు RISCV_IOMMU_DC_TC_EN_PRI        బిట్టు_ULL(2)
#నిర్వచించు RISCV_IOMMU_DC_TC_T2GPA         బిట్టు_ULL(3)
#నిర్వచించు RISCV_IOMMU_DC_TC_DTF           బిట్టు_ULL(4)
#నిర్వచించు RISCV_IOMMU_DC_TC_PDTV          బిట్టు_ULL(5)
#నిర్వచించు RISCV_IOMMU_DC_TC_PRPR          బిట్టు_ULL(6)
#నిర్వచించు RISCV_IOMMU_DC_TC_GADE          బిట్టు_ULL(7)
#నిర్వచించు RISCV_IOMMU_DC_TC_SADE          బిట్టు_ULL(8)
#నిర్వచించు RISCV_IOMMU_DC_TC_DPE           బిట్టు_ULL(9)
#నిర్వచించు RISCV_IOMMU_DC_TC_SBE           బిట్టు_ULL(10)
#నిర్వచించు RISCV_IOMMU_DC_TC_SXL           బిట్టు_ULL(11)

/* Second-stage (aka G-stage) context fields */
#నిర్వచించు RISCV_IOMMU_DC_IOHGATP_PPN      RISCV_IOMMU_ATP_PPN_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_DC_IOHGATP_GSCID    GENMASK_ULL(59, 44)
#నిర్వచించు RISCV_IOMMU_DC_IOHGATP_బహుళకం     RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం

#నిర్వచించు RISCV_IOMMU_DC_TC_RESERVED      (GENMASK_ULL(23, 12) \
                                         | GENMASK_ULL(63, 32))

గణన riscv_iommu_dc_iohgatp_modes {
    RISCV_IOMMU_DC_IOHGATP_బహుళకం_BARE = 0,
    RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV32_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV39_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV48_X4 = 9,
    RISCV_IOMMU_DC_IOHGATP_బహుళకం_SV57_X4 = 10
};

/* Translation attributes fields */
#నిర్వచించు RISCV_IOMMU_DC_TA_PSCID         GENMASK_ULL(31, 12)

/* First-stage context fields */
#నిర్వచించు RISCV_IOMMU_DC_FSC_PPN          RISCV_IOMMU_ATP_PPN_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_DC_FSC_బహుళకం         RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం

/* Generic I/O MMU command structure - check section 3.1 */
నిర్మాణం riscv_iommu_command {
    uint64_t dword0;
    uint64_t dword1;
};

#నిర్వచించు RISCV_IOMMU_CMD_OPCODE          GENMASK_ULL(6, 0)
#నిర్వచించు RISCV_IOMMU_CMD_FUNC            GENMASK_ULL(9, 7)

#నిర్వచించు RISCV_IOMMU_CMD_IOTINVAL_OPCODE         1
#నిర్వచించు RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA       0
#నిర్వచించు RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA      1
#నిర్వచించు RISCV_IOMMU_CMD_IOTINVAL_AV     బిట్టు_ULL(10)
#నిర్వచించు RISCV_IOMMU_CMD_IOTINVAL_PSCID  GENMASK_ULL(31, 12)
#నిర్వచించు RISCV_IOMMU_CMD_IOTINVAL_PSCV   బిట్టు_ULL(32)
#నిర్వచించు RISCV_IOMMU_CMD_IOTINVAL_GV     బిట్టు_ULL(33)
#నిర్వచించు RISCV_IOMMU_CMD_IOTINVAL_GSCID  GENMASK_ULL(59, 44)

#నిర్వచించు RISCV_IOMMU_CMD_IOFENCE_OPCODE          2
#నిర్వచించు RISCV_IOMMU_CMD_IOFENCE_FUNC_C          0
#నిర్వచించు RISCV_IOMMU_CMD_IOFENCE_AV      బిట్టు_ULL(10)
#నిర్వచించు RISCV_IOMMU_CMD_IOFENCE_RESERVED GENMASK_ULL(31, 14)
#నిర్వచించు RISCV_IOMMU_CMD_IOFENCE_దత్తాంశం    GENMASK_ULL(63, 32)

#నిర్వచించు RISCV_IOMMU_CMD_IODIR_OPCODE            3
#నిర్వచించు RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT    0
#నిర్వచించు RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT    1
#నిర్వచించు RISCV_IOMMU_CMD_IODIR_PID       GENMASK_ULL(31, 12)
#నిర్వచించు RISCV_IOMMU_CMD_IODIR_DV        బిట్టు_ULL(33)
#నిర్వచించు RISCV_IOMMU_CMD_IODIR_DID       GENMASK_ULL(63, 40)

/* 3.1.4 I/O MMU PCIe ATS */
#నిర్వచించు RISCV_IOMMU_CMD_ATS_OPCODE              4
#నిర్వచించు RISCV_IOMMU_CMD_ATS_FUNC_INVAL          0
#నిర్వచించు RISCV_IOMMU_CMD_ATS_FUNC_PRGR           1
#నిర్వచించు RISCV_IOMMU_CMD_ATS_PID         GENMASK_ULL(31, 12)
#నిర్వచించు RISCV_IOMMU_CMD_ATS_PV          బిట్టు_ULL(32)
#నిర్వచించు RISCV_IOMMU_CMD_ATS_DSV         బిట్టు_ULL(33)
#నిర్వచించు RISCV_IOMMU_CMD_ATS_RID         GENMASK_ULL(55, 40)
#నిర్వచించు RISCV_IOMMU_CMD_ATS_DSEG        GENMASK_ULL(63, 56)
/* dword1 is the ATS payload, two different payload types for INVAL and PRGR */

/* ATS.PRGR payload */
#నిర్వచించు RISCV_IOMMU_CMD_ATS_PRGR_RESP_సంకేతం      GENMASK_ULL(47, 44)

గణన riscv_iommu_dc_fsc_atp_modes {
    RISCV_IOMMU_DC_FSC_బహుళకం_BARE = 0,
    RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV32 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV39 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV48 = 9,
    RISCV_IOMMU_DC_FSC_IOSATP_బహుళకం_SV57 = 10,
    RISCV_IOMMU_DC_FSC_PDTP_బహుళకం_PD8 = 1,
    RISCV_IOMMU_DC_FSC_PDTP_బహుళకం_PD17 = 2,
    RISCV_IOMMU_DC_FSC_PDTP_బహుళకం_PD20 = 3
};

గణన riscv_iommu_fq_causes {
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
    RISCV_IOMMU_FQ_CAUSE_DDT_ఎక్కించు_FAULT       = 257,
    RISCV_IOMMU_FQ_CAUSE_DDT_INVALID          = 258,
    RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED    = 259,
    RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED        = 260,
    RISCV_IOMMU_FQ_CAUSE_MSI_ఎక్కించు_FAULT       = 261,
    RISCV_IOMMU_FQ_CAUSE_MSI_INVALID          = 262,
    RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED    = 263,
    RISCV_IOMMU_FQ_CAUSE_MRIF_FAULT           = 264,
    RISCV_IOMMU_FQ_CAUSE_PDT_ఎక్కించు_FAULT       = 265,
    RISCV_IOMMU_FQ_CAUSE_PDT_INVALID          = 266,
    RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED    = 267,
    RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED        = 268,
    RISCV_IOMMU_FQ_CAUSE_PDT_CORRUPTED        = 269,
    RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED     = 270,
    RISCV_IOMMU_FQ_CAUSE_MRIF_CORRUIPTED      = 271,
    RISCV_IOMMU_FQ_CAUSE_అంతర్గత_DP_ERROR    = 272,
    RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT         = 273,
    RISCV_IOMMU_FQ_CAUSE_PT_CORRUPTED         = 274
};

/* MSI page table pointer */
#నిర్వచించు RISCV_IOMMU_DC_MSIPTP_PPN       RISCV_IOMMU_ATP_PPN_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_DC_MSIPTP_బహుళకం      RISCV_IOMMU_ATP_బహుళకం_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_DC_MSIPTP_బహుళకం_OFF  0
#నిర్వచించు RISCV_IOMMU_DC_MSIPTP_బహుళకం_FLAT 1

/* 2.2 Process Directory Table */
#నిర్వచించు RISCV_IOMMU_PDTE_VALID          బిట్టు_ULL(0)
#నిర్వచించు RISCV_IOMMU_PDTE_PPN            RISCV_IOMMU_PPN_క్షేత్రం

/* Translation attributes fields */
#నిర్వచించు RISCV_IOMMU_PC_TA_V             బిట్టు_ULL(0)
#నిర్వచించు RISCV_IOMMU_PC_TA_RESERVED      GENMASK_ULL(63, 32)

/* First stage context fields */
#నిర్వచించు RISCV_IOMMU_PC_FSC_PPN          RISCV_IOMMU_ATP_PPN_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_PC_FSC_RESERVED     GENMASK_ULL(59, 44)

గణన riscv_iommu_fq_ttypes {
    RISCV_IOMMU_FQ_TTYPE_NONE = 0,
    RISCV_IOMMU_FQ_TTYPE_UADDR_INST_FETCH = 1,
    RISCV_IOMMU_FQ_TTYPE_UADDR_RD = 2,
    RISCV_IOMMU_FQ_TTYPE_UADDR_WR = 3,
    RISCV_IOMMU_FQ_TTYPE_TADDR_INST_FETCH = 5,
    RISCV_IOMMU_FQ_TTYPE_TADDR_RD = 6,
    RISCV_IOMMU_FQ_TTYPE_TADDR_WR = 7,
    RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ = 8,
    RISCV_IOMMU_FW_TTYPE_PCIE_సందేశం_REQ = 9,
};

/*
 * struct riscv_iommu_msi_pte - MSI Page Table Entry
 */
నిర్మాణం riscv_iommu_msi_pte {
      uint64_t pte;
      uint64_t mrif_info;
};

/* Fields on pte */
#నిర్వచించు RISCV_IOMMU_MSI_PTE_V           బిట్టు_ULL(0)
#నిర్వచించు RISCV_IOMMU_MSI_PTE_M           GENMASK_ULL(2, 1)

#నిర్వచించు RISCV_IOMMU_MSI_PTE_M_MRIF      1
#నిర్వచించు RISCV_IOMMU_MSI_PTE_M_BASIC     3

/* When M == 1 (MRIF mode) */
#నిర్వచించు RISCV_IOMMU_MSI_PTE_MRIF_ADDR   GENMASK_ULL(53, 7)
/* When M == 3 (basic mode) */
#నిర్వచించు RISCV_IOMMU_MSI_PTE_PPN         RISCV_IOMMU_PPN_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_MSI_PTE_C           బిట్టు_ULL(63)

/* Fields on mrif_info */
#నిర్వచించు RISCV_IOMMU_MSI_MRIF_NID        GENMASK_ULL(9, 0)
#నిర్వచించు RISCV_IOMMU_MSI_MRIF_NPPN       RISCV_IOMMU_PPN_క్షేత్రం
#నిర్వచించు RISCV_IOMMU_MSI_MRIF_NID_MSB    బిట్టు_ULL(60)

#అయితే_ముగింపు /* _RISCV_IOMMU_BITS_H_ */
