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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HW_RISCV_IOMMU_ಬಿಟ್ಟುಗಳು_H
#ವ್ಯಾಖ್ಯಾನಿಸು HW_RISCV_IOMMU_ಬಿಟ್ಟುಗಳು_H

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_SPEC_DOT_VER 0x010

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GENMASK_ULL
#ವ್ಯಾಖ್ಯಾನಿಸು GENMASK_ULL(h, l) (((~0ULL) >> (63 - (h) + (l))) << (l))
#ಆದರೆ_ಅಂತ್ಯ

/*
 * struct riscv_iommu_fq_record - Fault/Event Queue Record
 * See section 3.2 for more info.
 */
ರಚನೆ riscv_iommu_fq_ದಾಖಲೆ {
    uint64_t hdr;
    uint64_t _reserved;
    uint64_t iotval;
    uint64_t iotval2;
};
/* Header fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQ_HDR_CAUSE        GENMASK_ULL(11, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQ_HDR_PID          GENMASK_ULL(31, 12)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQ_HDR_PV           ಬಿಟ್_ULL(32)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQ_HDR_TTYPE        GENMASK_ULL(39, 34)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQ_HDR_DID          GENMASK_ULL(63, 40)

/*
 * struct riscv_iommu_pq_record - PCIe Page Request record
 * For more infos on the PCIe Page Request queue see chapter 3.3.
 */
ರಚನೆ riscv_iommu_pq_ದಾಖಲೆ {
      uint64_t hdr;
      uint64_t payload;
};
/* Header fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_HDR_PID        GENMASK_ULL(31, 12)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_HDR_PV         ಬಿಟ್_ULL(32)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_HDR_PRIV       ಬಿಟ್_ULL(33)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_HDR_EXEC       ಬಿಟ್_ULL(34)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_HDR_DID        GENMASK_ULL(63, 40)

/* Payload fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_PAYLOAD_R      ಬಿಟ್_ULL(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_PAYLOAD_W      ಬಿಟ್_ULL(1)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_PAYLOAD_L      ಬಿಟ್_ULL(2)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_PAYLOAD_M      GENMASK_ULL(2, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_PRG_ಸೂಚ್ಯಂಕ      GENMASK_ULL(11, 3)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PREQ_UADDR          GENMASK_ULL(63, 12)

/* Common field positions */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ           GENMASK_ULL(53, 10)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ಸರತಿ_LOGSZ_ಕ್ಷೇತ್ರ   GENMASK_ULL(4, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ಸರತಿ_ಸೂಚ್ಯಂಕ_ಕ್ಷೇತ್ರ   GENMASK_ULL(31, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ಸರತಿ_ENABLE        ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ಸರತಿ_INTR_ENABLE   ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ಸರತಿ_MEM_FAULT     ಬಿಟ್(8)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ಸರತಿ_ಅತಿಪ್ರವಾಹ      ಬಿಟ್(9)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ಸರತಿ_ಸಕ್ರಿಯ        ಬಿಟ್(16)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ಸರತಿ_BUSY          ಬಿಟ್(17)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ATP_PPN_ಕ್ಷೇತ್ರ       GENMASK_ULL(43, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ      GENMASK_ULL(63, 60)

/* 5.3 IOMMU Capabilities (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_CAP             0x0000
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_ಆವೃತ್ತಿ         GENMASK_ULL(7, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SV32            ಬಿಟ್_ULL(8)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SV39            ಬಿಟ್_ULL(9)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SV48            ಬಿಟ್_ULL(10)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SV57            ಬಿಟ್_ULL(11)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SVRSW60T59B     ಬಿಟ್_ULL(14)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SV32X4          ಬಿಟ್_ULL(16)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SV39X4          ಬಿಟ್_ULL(17)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SV48X4          ಬಿಟ್_ULL(18)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_SV57X4          ಬಿಟ್_ULL(19)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_MSI_FLAT        ಬಿಟ್_ULL(22)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_MSI_MRIF        ಬಿಟ್_ULL(23)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_AMO_HWAD        ಬಿಟ್_ULL(24)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_ATS             ಬಿಟ್_ULL(25)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_T2GPA           ಬಿಟ್_ULL(26)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_IGS             GENMASK_ULL(29, 28)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_HPM             ಬಿಟ್_ULL(30)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_DBG             ಬಿಟ್_ULL(31)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_PAS             GENMASK_ULL(37, 32)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_PD8             ಬಿಟ್_ULL(38)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_PD17            ಬಿಟ್_ULL(39)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CAP_PD20            ಬಿಟ್_ULL(40)

ಎಣಿಕೆ riscv_iommu_igs_modes {
    RISCV_IOMMU_CAP_IGS_MSI = 0,
    RISCV_IOMMU_CAP_IGS_WSI,
    RISCV_IOMMU_CAP_IGS_BOTH
};

/* 5.4 Features control register (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_FCTL            0x0008
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FCTL_BE             ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FCTL_WSI            ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FCTL_GXL            ಬಿಟ್(2)

/* 5.5 Device-directory-table pointer (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_DDTP            0x0010
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DDTP_ಬಹುಲಕ           GENMASK_ULL(3, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DDTP_BUSY           ಬಿಟ್_ULL(4)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DDTP_PPN            RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ

ಎಣಿಕೆ riscv_iommu_ddtp_modes {
    RISCV_IOMMU_DDTP_ಬಹುಲಕ_OFF = 0,
    RISCV_IOMMU_DDTP_ಬಹುಲಕ_BARE = 1,
    RISCV_IOMMU_DDTP_ಬಹುಲಕ_1_LVL = 2,
    RISCV_IOMMU_DDTP_ಬಹುಲಕ_2_LVL = 3,
    RISCV_IOMMU_DDTP_ಬಹುಲಕ_3_LVL = 4,
    RISCV_IOMMU_DDTP_ಬಹುಲಕ_MAX = 4
};

/* 5.6 Command Queue Base (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_CQB             0x0018
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQB_LOG2SZ          RISCV_IOMMU_ಸರತಿ_LOGSZ_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQB_PPN             RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ

/* 5.7 Command Queue head (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_CQH             0x0020

/* 5.8 Command Queue tail (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_CQT             0x0024

/* 5.9 Fault Queue Base (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_FQB             0x0028
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQB_LOG2SZ          RISCV_IOMMU_ಸರತಿ_LOGSZ_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQB_PPN             RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ

/* 5.10 Fault Queue Head (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_FQH             0x0030

/* 5.11 Fault Queue tail (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_FQT             0x0034

/* 5.12 Page Request Queue base (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_PQB             0x0038
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PQB_LOG2SZ          RISCV_IOMMU_ಸರತಿ_LOGSZ_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PQB_PPN             RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ

/* 5.13 Page Request Queue head (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_PQH             0x0040

/* 5.14 Page Request Queue tail (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_PQT             0x0044

/* 5.15 Command Queue CSR (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_CQCSR           0x0048
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQCSR_CQEN          RISCV_IOMMU_ಸರತಿ_ENABLE
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQCSR_CIE           RISCV_IOMMU_ಸರತಿ_INTR_ENABLE
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQCSR_CQMF          RISCV_IOMMU_ಸರತಿ_MEM_FAULT
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQCSR_CMD_ಗಮ್ಯ        ಬಿಟ್(9)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQCSR_CMD_ILL       ಬಿಟ್(10)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQCSR_FENCE_W_IP    ಬಿಟ್(11)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQCSR_CQON          RISCV_IOMMU_ಸರತಿ_ಸಕ್ರಿಯ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CQCSR_BUSY          RISCV_IOMMU_ಸರತಿ_BUSY

/* 5.16 Fault Queue CSR (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_FQCSR           0x004C
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQCSR_FQEN          RISCV_IOMMU_ಸರತಿ_ENABLE
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQCSR_FIE           RISCV_IOMMU_ಸರತಿ_INTR_ENABLE
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQCSR_FQMF          RISCV_IOMMU_ಸರತಿ_MEM_FAULT
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQCSR_FQOF          RISCV_IOMMU_ಸರತಿ_ಅತಿಪ್ರವಾಹ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQCSR_FQON          RISCV_IOMMU_ಸರತಿ_ಸಕ್ರಿಯ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_FQCSR_BUSY          RISCV_IOMMU_ಸರತಿ_BUSY

/* 5.17 Page Request Queue CSR (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_PQCSR           0x0050
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PQCSR_PQEN          RISCV_IOMMU_ಸರತಿ_ENABLE
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PQCSR_PIE           RISCV_IOMMU_ಸರತಿ_INTR_ENABLE
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PQCSR_PQMF          RISCV_IOMMU_ಸರತಿ_MEM_FAULT
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PQCSR_PQOF          RISCV_IOMMU_ಸರತಿ_ಅತಿಪ್ರವಾಹ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PQCSR_PQON          RISCV_IOMMU_ಸರತಿ_ಸಕ್ರಿಯ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PQCSR_BUSY          RISCV_IOMMU_ಸರತಿ_BUSY

/* 5.18 Interrupt Pending Status (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_IPSR            0x0054
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IPSR_CIP            ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IPSR_FIP            ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IPSR_PMIP           ಬಿಟ್(2)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IPSR_PIP            ಬಿಟ್(3)

ಎಣಿಕೆ {
    RISCV_IOMMU_INTR_CQ,
    RISCV_IOMMU_INTR_FQ,
    RISCV_IOMMU_INTR_PM,
    RISCV_IOMMU_INTR_PQ,
    RISCV_IOMMU_INTR_ಎಣಿಕೆ
};

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOCOUNT_ಅಂಕೆ         31

/* 5.19 Performance monitoring counter overflow status (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_IOCOUNTOVF      0x0058
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOCOUNTOVF_CY       ಬಿಟ್(0)

/* 5.20 Performance monitoring counter inhibits (32bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_IOCOUNTINH      0x005C
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOCOUNTINH_CY       ಬಿಟ್(0)

/* 5.21 Performance monitoring cycles counter (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_IOHPMCYCLES     0x0060
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMCYCLES_COUNTER GENMASK_ULL(62, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMCYCLES_OVF     ಬಿಟ್_ULL(63)

/* 5.22 Performance monitoring event counters (31 * 64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_IOHPMCTR_ತಳ   0x0068
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_IOHPMCTR(_n)    \
    (RISCV_IOMMU_REG_IOHPMCTR_ತಳ + (_n * 0x8))

/* 5.23 Performance monitoring event selectors (31 * 64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_IOHPMEVT_ತಳ   0x0160
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_IOHPMEVT(_n)    \
    (RISCV_IOMMU_REG_IOHPMEVT_ತಳ + (_n * 0x8))
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMEVT_EVENT_ID   GENMASK_ULL(14, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMEVT_DMASK      ಬಿಟ್_ULL(15)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMEVT_PID_PSCID  GENMASK_ULL(35, 16)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMEVT_DID_GSCID  GENMASK_ULL(59, 36)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMEVT_PV_PSCV    ಬಿಟ್_ULL(60)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMEVT_DV_GSCV    ಬಿಟ್_ULL(61)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMEVT_IDT        ಬಿಟ್_ULL(62)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_IOHPMEVT_ದ         ಬಿಟ್_ULL(63)

ಎಣಿಕೆ RISCV_IOMMU_HPMEVENT_id {
    RISCV_IOMMU_HPMEVENT_INVALID    = 0,
    RISCV_IOMMU_HPMEVENT_URQ        = 1,
    RISCV_IOMMU_HPMEVENT_TRQ        = 2,
    RISCV_IOMMU_HPMEVENT_ATS_RQ     = 3,
    RISCV_IOMMU_HPMEVENT_TLB_ತಪ್ಪಿದ   = 4,
    RISCV_IOMMU_HPMEVENT_DD_WALK    = 5,
    RISCV_IOMMU_HPMEVENT_PD_WALK    = 6,
    RISCV_IOMMU_HPMEVENT_S_VS_WALKS = 7,
    RISCV_IOMMU_HPMEVENT_G_WALKS    = 8,
    RISCV_IOMMU_HPMEVENT_MAX        = 9
};

/* 5.24 Translation request IOVA (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_TR_REQ_IOVA     0x0258

/* 5.25 Translation request control (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_TR_REQ_CTL      0x0260
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_TR_REQ_CTL_GO_BUSY  ಬಿಟ್_ULL(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_TR_REQ_CTL_NW       ಬಿಟ್_ULL(3)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_TR_REQ_CTL_PID      GENMASK_ULL(31, 12)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_TR_REQ_CTL_DID      GENMASK_ULL(63, 40)

/* 5.26 Translation request response (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_TR_RESPONSE     0x0268
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_TR_RESPONSE_FAULT   ಬಿಟ್_ULL(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_TR_RESPONSE_S       ಬಿಟ್_ULL(9)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_TR_RESPONSE_PPN     RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ

/* 5.27 Interrupt cause to vector (64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_ICVEC           0x02F8
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ICVEC_CIV           GENMASK_ULL(3, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ICVEC_FIV           GENMASK_ULL(7, 4)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ICVEC_PMIV          GENMASK_ULL(11, 8)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_ICVEC_PIV           GENMASK_ULL(15, 12)

/* 5.28 MSI Configuration table (32 * 64bits) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_MSI_CONFIG      0x0300

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_REG_ಗಾತ್ರ            0x1000

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DDTE_VALID          ಬಿಟ್_ULL(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DDTE_PPN            RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ

/* Struct riscv_iommu_dc - Device Context - section 2.1 */
ರಚನೆ riscv_iommu_dc {
      uint64_t tc;
      uint64_t iohgatp;
      uint64_t ta;
      uint64_t fsc;
      uint64_t msiptp;
      uint64_t msi_addr_ಮುಖವಾಡ;
      uint64_t msi_addr_ನಮೂನೆ;
      uint64_t _reserved;
};

/* Translation control fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_V             ಬಿಟ್_ULL(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_EN_ATS        ಬಿಟ್_ULL(1)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_EN_PRI        ಬಿಟ್_ULL(2)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_T2GPA         ಬಿಟ್_ULL(3)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_DTF           ಬಿಟ್_ULL(4)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_PDTV          ಬಿಟ್_ULL(5)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_PRPR          ಬಿಟ್_ULL(6)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_GADE          ಬಿಟ್_ULL(7)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_SADE          ಬಿಟ್_ULL(8)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_DPE           ಬಿಟ್_ULL(9)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_SBE           ಬಿಟ್_ULL(10)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_SXL           ಬಿಟ್_ULL(11)

/* Second-stage (aka G-stage) context fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_IOHGATP_PPN      RISCV_IOMMU_ATP_PPN_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_IOHGATP_GSCID    GENMASK_ULL(59, 44)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ     RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TC_RESERVED      (GENMASK_ULL(23, 12) \
                                         | GENMASK_ULL(63, 32))

ಎಣಿಕೆ riscv_iommu_dc_iohgatp_modes {
    RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_BARE = 0,
    RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV32_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV39_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV48_X4 = 9,
    RISCV_IOMMU_DC_IOHGATP_ಬಹುಲಕ_SV57_X4 = 10
};

/* Translation attributes fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_TA_PSCID         GENMASK_ULL(31, 12)

/* First-stage context fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_FSC_PPN          RISCV_IOMMU_ATP_PPN_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_FSC_ಬಹುಲಕ         RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ

/* Generic I/O MMU command structure - check section 3.1 */
ರಚನೆ riscv_iommu_command {
    uint64_t dword0;
    uint64_t dword1;
};

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_OPCODE          GENMASK_ULL(6, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_FUNC            GENMASK_ULL(9, 7)

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOTINVAL_OPCODE         1
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA       0
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA      1
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOTINVAL_AV     ಬಿಟ್_ULL(10)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOTINVAL_PSCID  GENMASK_ULL(31, 12)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOTINVAL_PSCV   ಬಿಟ್_ULL(32)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOTINVAL_GV     ಬಿಟ್_ULL(33)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOTINVAL_GSCID  GENMASK_ULL(59, 44)

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOFENCE_OPCODE          2
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOFENCE_FUNC_C          0
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOFENCE_AV      ಬಿಟ್_ULL(10)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOFENCE_RESERVED GENMASK_ULL(31, 14)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IOFENCE_ದತ್ತಾಂಶ    GENMASK_ULL(63, 32)

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IODIR_OPCODE            3
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT    0
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT    1
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IODIR_PID       GENMASK_ULL(31, 12)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IODIR_DV        ಬಿಟ್_ULL(33)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_IODIR_DID       GENMASK_ULL(63, 40)

/* 3.1.4 I/O MMU PCIe ATS */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_OPCODE              4
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_FUNC_INVAL          0
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_FUNC_PRGR           1
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_PID         GENMASK_ULL(31, 12)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_PV          ಬಿಟ್_ULL(32)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_DSV         ಬಿಟ್_ULL(33)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_RID         GENMASK_ULL(55, 40)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_DSEG        GENMASK_ULL(63, 56)
/* dword1 is the ATS payload, two different payload types for INVAL and PRGR */

/* ATS.PRGR payload */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_CMD_ATS_PRGR_RESP_ಸಂಕೇತ      GENMASK_ULL(47, 44)

ಎಣಿಕೆ riscv_iommu_dc_fsc_atp_modes {
    RISCV_IOMMU_DC_FSC_ಬಹುಲಕ_BARE = 0,
    RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV32 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV39 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV48 = 9,
    RISCV_IOMMU_DC_FSC_IOSATP_ಬಹುಲಕ_SV57 = 10,
    RISCV_IOMMU_DC_FSC_PDTP_ಬಹುಲಕ_PD8 = 1,
    RISCV_IOMMU_DC_FSC_PDTP_ಬಹುಲಕ_PD17 = 2,
    RISCV_IOMMU_DC_FSC_PDTP_ಬಹುಲಕ_PD20 = 3
};

ಎಣಿಕೆ riscv_iommu_fq_causes {
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
    RISCV_IOMMU_FQ_CAUSE_DDT_ತುಂಬು_FAULT       = 257,
    RISCV_IOMMU_FQ_CAUSE_DDT_INVALID          = 258,
    RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED    = 259,
    RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED        = 260,
    RISCV_IOMMU_FQ_CAUSE_MSI_ತುಂಬು_FAULT       = 261,
    RISCV_IOMMU_FQ_CAUSE_MSI_INVALID          = 262,
    RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED    = 263,
    RISCV_IOMMU_FQ_CAUSE_MRIF_FAULT           = 264,
    RISCV_IOMMU_FQ_CAUSE_PDT_ತುಂಬು_FAULT       = 265,
    RISCV_IOMMU_FQ_CAUSE_PDT_INVALID          = 266,
    RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED    = 267,
    RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED        = 268,
    RISCV_IOMMU_FQ_CAUSE_PDT_CORRUPTED        = 269,
    RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED     = 270,
    RISCV_IOMMU_FQ_CAUSE_MRIF_CORRUIPTED      = 271,
    RISCV_IOMMU_FQ_CAUSE_ಆಂತರಿಕ_DP_ERROR    = 272,
    RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT         = 273,
    RISCV_IOMMU_FQ_CAUSE_PT_CORRUPTED         = 274
};

/* MSI page table pointer */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_MSIPTP_PPN       RISCV_IOMMU_ATP_PPN_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_MSIPTP_ಬಹುಲಕ      RISCV_IOMMU_ATP_ಬಹುಲಕ_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_MSIPTP_ಬಹುಲಕ_OFF  0
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_DC_MSIPTP_ಬಹುಲಕ_FLAT 1

/* 2.2 Process Directory Table */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PDTE_VALID          ಬಿಟ್_ULL(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PDTE_PPN            RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ

/* Translation attributes fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PC_TA_V             ಬಿಟ್_ULL(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PC_TA_RESERVED      GENMASK_ULL(63, 32)

/* First stage context fields */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PC_FSC_PPN          RISCV_IOMMU_ATP_PPN_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_PC_FSC_RESERVED     GENMASK_ULL(59, 44)

ಎಣಿಕೆ riscv_iommu_fq_ttypes {
    RISCV_IOMMU_FQ_TTYPE_NONE = 0,
    RISCV_IOMMU_FQ_TTYPE_UADDR_INST_FETCH = 1,
    RISCV_IOMMU_FQ_TTYPE_UADDR_RD = 2,
    RISCV_IOMMU_FQ_TTYPE_UADDR_WR = 3,
    RISCV_IOMMU_FQ_TTYPE_TADDR_INST_FETCH = 5,
    RISCV_IOMMU_FQ_TTYPE_TADDR_RD = 6,
    RISCV_IOMMU_FQ_TTYPE_TADDR_WR = 7,
    RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ = 8,
    RISCV_IOMMU_FW_TTYPE_PCIE_ಸಂದೇಶ_REQ = 9,
};

/*
 * struct riscv_iommu_msi_pte - MSI Page Table Entry
 */
ರಚನೆ riscv_iommu_msi_pte {
      uint64_t pte;
      uint64_t mrif_info;
};

/* Fields on pte */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_PTE_V           ಬಿಟ್_ULL(0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_PTE_M           GENMASK_ULL(2, 1)

#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_PTE_M_MRIF      1
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_PTE_M_BASIC     3

/* When M == 1 (MRIF mode) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_PTE_MRIF_ADDR   GENMASK_ULL(53, 7)
/* When M == 3 (basic mode) */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_PTE_PPN         RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_PTE_C           ಬಿಟ್_ULL(63)

/* Fields on mrif_info */
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_MRIF_NID        GENMASK_ULL(9, 0)
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_MRIF_NPPN       RISCV_IOMMU_PPN_ಕ್ಷೇತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_IOMMU_MSI_MRIF_NID_MSB    ಬಿಟ್_ULL(60)

#ಆದರೆ_ಅಂತ್ಯ /* _RISCV_IOMMU_BITS_H_ */
