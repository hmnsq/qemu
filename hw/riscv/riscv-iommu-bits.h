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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HW_RISCV_IOMMU_ਬਿੱਟ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HW_RISCV_IOMMU_ਬਿੱਟ_H

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_SPEC_DOT_VER 0x010

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GENMASK_ULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GENMASK_ULL(h, l) (((~0ULL) >> (63 - (h) + (l))) << (l))
#ਜੇ_ਅੰਤ

/*
 * struct riscv_iommu_fq_record - Fault/Event Queue Record
 * See section 3.2 for more info.
 */
ਬਣਤਰ riscv_iommu_fq_ਰਿਕਾਰਡ {
    uint64_t hdr;
    uint64_t _reserved;
    uint64_t iotval;
    uint64_t iotval2;
};
/* Header fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQ_HDR_CAUSE        GENMASK_ULL(11, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQ_HDR_PID          GENMASK_ULL(31, 12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQ_HDR_PV           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQ_HDR_TTYPE        GENMASK_ULL(39, 34)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQ_HDR_DID          GENMASK_ULL(63, 40)

/*
 * struct riscv_iommu_pq_record - PCIe Page Request record
 * For more infos on the PCIe Page Request queue see chapter 3.3.
 */
ਬਣਤਰ riscv_iommu_pq_ਰਿਕਾਰਡ {
      uint64_t hdr;
      uint64_t payload;
};
/* Header fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_HDR_PID        GENMASK_ULL(31, 12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_HDR_PV         ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_HDR_PRIV       ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(33)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_HDR_EXEC       ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(34)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_HDR_DID        GENMASK_ULL(63, 40)

/* Payload fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_PAYLOAD_R      ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_PAYLOAD_W      ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_PAYLOAD_L      ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_PAYLOAD_M      GENMASK_ULL(2, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_PRG_ਸੂਚਕਾਂਕ      GENMASK_ULL(11, 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PREQ_UADDR          GENMASK_ULL(63, 12)

/* Common field positions */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PPN_ਖੇਤਰ           GENMASK_ULL(53, 10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ਕਤਾਰ_LOGSZ_ਖੇਤਰ   GENMASK_ULL(4, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ਕਤਾਰ_ਸੂਚਕਾਂਕ_ਖੇਤਰ   GENMASK_ULL(31, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ਕਤਾਰ_ENABLE        ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ਕਤਾਰ_INTR_ENABLE   ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ਕਤਾਰ_MEM_FAULT     ਦੋਆਧਾਰੀ_ਅੰਕ(8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ਕਤਾਰ_ਉਛਾਲ      ਦੋਆਧਾਰੀ_ਅੰਕ(9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ਕਤਾਰ_ਸਰਗਰਮ        ਦੋਆਧਾਰੀ_ਅੰਕ(16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ਕਤਾਰ_BUSY          ਦੋਆਧਾਰੀ_ਅੰਕ(17)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ATP_PPN_ਖੇਤਰ       GENMASK_ULL(43, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ      GENMASK_ULL(63, 60)

/* 5.3 IOMMU Capabilities (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_CAP             0x0000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_ਸੰਸਕਰਣ         GENMASK_ULL(7, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SV32            ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SV39            ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SV48            ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SV57            ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SVRSW60T59B     ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SV32X4          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SV39X4          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(17)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SV48X4          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_SV57X4          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(19)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_MSI_FLAT        ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(22)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_MSI_MRIF        ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(23)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_AMO_HWAD        ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(24)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_ATS             ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(25)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_T2GPA           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(26)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_IGS             GENMASK_ULL(29, 28)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_HPM             ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(30)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_DBG             ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_PAS             GENMASK_ULL(37, 32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_PD8             ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(38)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_PD17            ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(39)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CAP_PD20            ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(40)

ਗਿਣਤੀ riscv_iommu_igs_modes {
    RISCV_IOMMU_CAP_IGS_MSI = 0,
    RISCV_IOMMU_CAP_IGS_WSI,
    RISCV_IOMMU_CAP_IGS_BOTH
};

/* 5.4 Features control register (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_FCTL            0x0008
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FCTL_BE             ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FCTL_WSI            ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FCTL_GXL            ਦੋਆਧਾਰੀ_ਅੰਕ(2)

/* 5.5 Device-directory-table pointer (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_DDTP            0x0010
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DDTP_ਬਹੁਲਕ           GENMASK_ULL(3, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DDTP_BUSY           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DDTP_PPN            RISCV_IOMMU_PPN_ਖੇਤਰ

ਗਿਣਤੀ riscv_iommu_ddtp_modes {
    RISCV_IOMMU_DDTP_ਬਹੁਲਕ_OFF = 0,
    RISCV_IOMMU_DDTP_ਬਹੁਲਕ_BARE = 1,
    RISCV_IOMMU_DDTP_ਬਹੁਲਕ_1_LVL = 2,
    RISCV_IOMMU_DDTP_ਬਹੁਲਕ_2_LVL = 3,
    RISCV_IOMMU_DDTP_ਬਹੁਲਕ_3_LVL = 4,
    RISCV_IOMMU_DDTP_ਬਹੁਲਕ_MAX = 4
};

/* 5.6 Command Queue Base (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_CQB             0x0018
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQB_LOG2SZ          RISCV_IOMMU_ਕਤਾਰ_LOGSZ_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQB_PPN             RISCV_IOMMU_PPN_ਖੇਤਰ

/* 5.7 Command Queue head (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_CQH             0x0020

/* 5.8 Command Queue tail (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_CQT             0x0024

/* 5.9 Fault Queue Base (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_FQB             0x0028
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQB_LOG2SZ          RISCV_IOMMU_ਕਤਾਰ_LOGSZ_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQB_PPN             RISCV_IOMMU_PPN_ਖੇਤਰ

/* 5.10 Fault Queue Head (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_FQH             0x0030

/* 5.11 Fault Queue tail (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_FQT             0x0034

/* 5.12 Page Request Queue base (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_PQB             0x0038
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PQB_LOG2SZ          RISCV_IOMMU_ਕਤਾਰ_LOGSZ_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PQB_PPN             RISCV_IOMMU_PPN_ਖੇਤਰ

/* 5.13 Page Request Queue head (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_PQH             0x0040

/* 5.14 Page Request Queue tail (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_PQT             0x0044

/* 5.15 Command Queue CSR (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_CQCSR           0x0048
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQCSR_CQEN          RISCV_IOMMU_ਕਤਾਰ_ENABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQCSR_CIE           RISCV_IOMMU_ਕਤਾਰ_INTR_ENABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQCSR_CQMF          RISCV_IOMMU_ਕਤਾਰ_MEM_FAULT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQCSR_CMD_ਮੰਜ਼ਿਲ        ਦੋਆਧਾਰੀ_ਅੰਕ(9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQCSR_CMD_ILL       ਦੋਆਧਾਰੀ_ਅੰਕ(10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQCSR_FENCE_W_IP    ਦੋਆਧਾਰੀ_ਅੰਕ(11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQCSR_CQON          RISCV_IOMMU_ਕਤਾਰ_ਸਰਗਰਮ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CQCSR_BUSY          RISCV_IOMMU_ਕਤਾਰ_BUSY

/* 5.16 Fault Queue CSR (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_FQCSR           0x004C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQCSR_FQEN          RISCV_IOMMU_ਕਤਾਰ_ENABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQCSR_FIE           RISCV_IOMMU_ਕਤਾਰ_INTR_ENABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQCSR_FQMF          RISCV_IOMMU_ਕਤਾਰ_MEM_FAULT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQCSR_FQOF          RISCV_IOMMU_ਕਤਾਰ_ਉਛਾਲ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQCSR_FQON          RISCV_IOMMU_ਕਤਾਰ_ਸਰਗਰਮ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_FQCSR_BUSY          RISCV_IOMMU_ਕਤਾਰ_BUSY

/* 5.17 Page Request Queue CSR (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_PQCSR           0x0050
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PQCSR_PQEN          RISCV_IOMMU_ਕਤਾਰ_ENABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PQCSR_PIE           RISCV_IOMMU_ਕਤਾਰ_INTR_ENABLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PQCSR_PQMF          RISCV_IOMMU_ਕਤਾਰ_MEM_FAULT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PQCSR_PQOF          RISCV_IOMMU_ਕਤਾਰ_ਉਛਾਲ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PQCSR_PQON          RISCV_IOMMU_ਕਤਾਰ_ਸਰਗਰਮ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PQCSR_BUSY          RISCV_IOMMU_ਕਤਾਰ_BUSY

/* 5.18 Interrupt Pending Status (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_IPSR            0x0054
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IPSR_CIP            ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IPSR_FIP            ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IPSR_PMIP           ਦੋਆਧਾਰੀ_ਅੰਕ(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IPSR_PIP            ਦੋਆਧਾਰੀ_ਅੰਕ(3)

ਗਿਣਤੀ {
    RISCV_IOMMU_INTR_CQ,
    RISCV_IOMMU_INTR_FQ,
    RISCV_IOMMU_INTR_PM,
    RISCV_IOMMU_INTR_PQ,
    RISCV_IOMMU_INTR_ਗਿਣਤੀ
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOCOUNT_ਅੰਕ         31

/* 5.19 Performance monitoring counter overflow status (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_IOCOUNTOVF      0x0058
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOCOUNTOVF_CY       ਦੋਆਧਾਰੀ_ਅੰਕ(0)

/* 5.20 Performance monitoring counter inhibits (32bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_IOCOUNTINH      0x005C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOCOUNTINH_CY       ਦੋਆਧਾਰੀ_ਅੰਕ(0)

/* 5.21 Performance monitoring cycles counter (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_IOHPMCYCLES     0x0060
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMCYCLES_COUNTER GENMASK_ULL(62, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMCYCLES_OVF     ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(63)

/* 5.22 Performance monitoring event counters (31 * 64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_IOHPMCTR_ਅਧਾਰ   0x0068
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_IOHPMCTR(_n)    \
    (RISCV_IOMMU_REG_IOHPMCTR_ਅਧਾਰ + (_n * 0x8))

/* 5.23 Performance monitoring event selectors (31 * 64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_IOHPMEVT_ਅਧਾਰ   0x0160
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_IOHPMEVT(_n)    \
    (RISCV_IOMMU_REG_IOHPMEVT_ਅਧਾਰ + (_n * 0x8))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMEVT_EVENT_ID   GENMASK_ULL(14, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMEVT_DMASK      ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(15)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMEVT_PID_PSCID  GENMASK_ULL(35, 16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMEVT_DID_GSCID  GENMASK_ULL(59, 36)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMEVT_PV_PSCV    ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(60)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMEVT_DV_GSCV    ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(61)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMEVT_IDT        ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(62)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_IOHPMEVT_ਦਾ         ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(63)

ਗਿਣਤੀ RISCV_IOMMU_HPMEVENT_id {
    RISCV_IOMMU_HPMEVENT_INVALID    = 0,
    RISCV_IOMMU_HPMEVENT_URQ        = 1,
    RISCV_IOMMU_HPMEVENT_TRQ        = 2,
    RISCV_IOMMU_HPMEVENT_ATS_RQ     = 3,
    RISCV_IOMMU_HPMEVENT_TLB_ਖੁੰਝ   = 4,
    RISCV_IOMMU_HPMEVENT_DD_WALK    = 5,
    RISCV_IOMMU_HPMEVENT_PD_WALK    = 6,
    RISCV_IOMMU_HPMEVENT_S_VS_WALKS = 7,
    RISCV_IOMMU_HPMEVENT_G_WALKS    = 8,
    RISCV_IOMMU_HPMEVENT_MAX        = 9
};

/* 5.24 Translation request IOVA (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_TR_REQ_IOVA     0x0258

/* 5.25 Translation request control (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_TR_REQ_CTL      0x0260
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_TR_REQ_CTL_GO_BUSY  ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_TR_REQ_CTL_NW       ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_TR_REQ_CTL_PID      GENMASK_ULL(31, 12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_TR_REQ_CTL_DID      GENMASK_ULL(63, 40)

/* 5.26 Translation request response (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_TR_RESPONSE     0x0268
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_TR_RESPONSE_FAULT   ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_TR_RESPONSE_S       ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_TR_RESPONSE_PPN     RISCV_IOMMU_PPN_ਖੇਤਰ

/* 5.27 Interrupt cause to vector (64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_ICVEC           0x02F8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ICVEC_CIV           GENMASK_ULL(3, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ICVEC_FIV           GENMASK_ULL(7, 4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ICVEC_PMIV          GENMASK_ULL(11, 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_ICVEC_PIV           GENMASK_ULL(15, 12)

/* 5.28 MSI Configuration table (32 * 64bits) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_MSI_CONFIG      0x0300

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_REG_ਮਾਪ            0x1000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DDTE_VALID          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DDTE_PPN            RISCV_IOMMU_PPN_ਖੇਤਰ

/* Struct riscv_iommu_dc - Device Context - section 2.1 */
ਬਣਤਰ riscv_iommu_dc {
      uint64_t tc;
      uint64_t iohgatp;
      uint64_t ta;
      uint64_t fsc;
      uint64_t msiptp;
      uint64_t msi_addr_ਨਕਾਬ;
      uint64_t msi_addr_ਨਮੂਨਾ;
      uint64_t _reserved;
};

/* Translation control fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_V             ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_EN_ATS        ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_EN_PRI        ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_T2GPA         ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_DTF           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_PDTV          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_PRPR          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_GADE          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(7)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_SADE          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_DPE           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_SBE           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_SXL           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(11)

/* Second-stage (aka G-stage) context fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_IOHGATP_PPN      RISCV_IOMMU_ATP_PPN_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_IOHGATP_GSCID    GENMASK_ULL(59, 44)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ     RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TC_RESERVED      (GENMASK_ULL(23, 12) \
                                         | GENMASK_ULL(63, 32))

ਗਿਣਤੀ riscv_iommu_dc_iohgatp_modes {
    RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_BARE = 0,
    RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV32_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV39_X4 = 8,
    RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV48_X4 = 9,
    RISCV_IOMMU_DC_IOHGATP_ਬਹੁਲਕ_SV57_X4 = 10
};

/* Translation attributes fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_TA_PSCID         GENMASK_ULL(31, 12)

/* First-stage context fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_FSC_PPN          RISCV_IOMMU_ATP_PPN_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ         RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ

/* Generic I/O MMU command structure - check section 3.1 */
ਬਣਤਰ riscv_iommu_command {
    uint64_t dword0;
    uint64_t dword1;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_OPCODE          GENMASK_ULL(6, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_FUNC            GENMASK_ULL(9, 7)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOTINVAL_OPCODE         1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOTINVAL_FUNC_VMA       0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOTINVAL_FUNC_GVMA      1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOTINVAL_AV     ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOTINVAL_PSCID  GENMASK_ULL(31, 12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOTINVAL_PSCV   ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOTINVAL_GV     ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(33)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOTINVAL_GSCID  GENMASK_ULL(59, 44)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOFENCE_OPCODE          2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOFENCE_FUNC_C          0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOFENCE_AV      ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOFENCE_RESERVED GENMASK_ULL(31, 14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IOFENCE_ਅੰਕੜੇ    GENMASK_ULL(63, 32)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IODIR_OPCODE            3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_DDT    0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IODIR_FUNC_INVAL_PDT    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IODIR_PID       GENMASK_ULL(31, 12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IODIR_DV        ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(33)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_IODIR_DID       GENMASK_ULL(63, 40)

/* 3.1.4 I/O MMU PCIe ATS */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_OPCODE              4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_FUNC_INVAL          0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_FUNC_PRGR           1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_PID         GENMASK_ULL(31, 12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_PV          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_DSV         ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(33)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_RID         GENMASK_ULL(55, 40)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_DSEG        GENMASK_ULL(63, 56)
/* dword1 is the ATS payload, two different payload types for INVAL and PRGR */

/* ATS.PRGR payload */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_CMD_ATS_PRGR_RESP_ਕੋਡ      GENMASK_ULL(47, 44)

ਗਿਣਤੀ riscv_iommu_dc_fsc_atp_modes {
    RISCV_IOMMU_DC_FSC_ਬਹੁਲਕ_BARE = 0,
    RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV32 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV39 = 8,
    RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV48 = 9,
    RISCV_IOMMU_DC_FSC_IOSATP_ਬਹੁਲਕ_SV57 = 10,
    RISCV_IOMMU_DC_FSC_PDTP_ਬਹੁਲਕ_PD8 = 1,
    RISCV_IOMMU_DC_FSC_PDTP_ਬਹੁਲਕ_PD17 = 2,
    RISCV_IOMMU_DC_FSC_PDTP_ਬਹੁਲਕ_PD20 = 3
};

ਗਿਣਤੀ riscv_iommu_fq_causes {
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
    RISCV_IOMMU_FQ_CAUSE_DDT_ਲੱਦੋ_FAULT       = 257,
    RISCV_IOMMU_FQ_CAUSE_DDT_INVALID          = 258,
    RISCV_IOMMU_FQ_CAUSE_DDT_MISCONFIGURED    = 259,
    RISCV_IOMMU_FQ_CAUSE_TTYPE_BLOCKED        = 260,
    RISCV_IOMMU_FQ_CAUSE_MSI_ਲੱਦੋ_FAULT       = 261,
    RISCV_IOMMU_FQ_CAUSE_MSI_INVALID          = 262,
    RISCV_IOMMU_FQ_CAUSE_MSI_MISCONFIGURED    = 263,
    RISCV_IOMMU_FQ_CAUSE_MRIF_FAULT           = 264,
    RISCV_IOMMU_FQ_CAUSE_PDT_ਲੱਦੋ_FAULT       = 265,
    RISCV_IOMMU_FQ_CAUSE_PDT_INVALID          = 266,
    RISCV_IOMMU_FQ_CAUSE_PDT_MISCONFIGURED    = 267,
    RISCV_IOMMU_FQ_CAUSE_DDT_CORRUPTED        = 268,
    RISCV_IOMMU_FQ_CAUSE_PDT_CORRUPTED        = 269,
    RISCV_IOMMU_FQ_CAUSE_MSI_PT_CORRUPTED     = 270,
    RISCV_IOMMU_FQ_CAUSE_MRIF_CORRUIPTED      = 271,
    RISCV_IOMMU_FQ_CAUSE_ਅੰਦਰੂਨੀ_DP_ERROR    = 272,
    RISCV_IOMMU_FQ_CAUSE_MSI_WR_FAULT         = 273,
    RISCV_IOMMU_FQ_CAUSE_PT_CORRUPTED         = 274
};

/* MSI page table pointer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_MSIPTP_PPN       RISCV_IOMMU_ATP_PPN_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_MSIPTP_ਬਹੁਲਕ      RISCV_IOMMU_ATP_ਬਹੁਲਕ_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_MSIPTP_ਬਹੁਲਕ_OFF  0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_DC_MSIPTP_ਬਹੁਲਕ_FLAT 1

/* 2.2 Process Directory Table */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PDTE_VALID          ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PDTE_PPN            RISCV_IOMMU_PPN_ਖੇਤਰ

/* Translation attributes fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PC_TA_V             ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PC_TA_RESERVED      GENMASK_ULL(63, 32)

/* First stage context fields */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PC_FSC_PPN          RISCV_IOMMU_ATP_PPN_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_PC_FSC_RESERVED     GENMASK_ULL(59, 44)

ਗਿਣਤੀ riscv_iommu_fq_ttypes {
    RISCV_IOMMU_FQ_TTYPE_NONE = 0,
    RISCV_IOMMU_FQ_TTYPE_UADDR_INST_FETCH = 1,
    RISCV_IOMMU_FQ_TTYPE_UADDR_RD = 2,
    RISCV_IOMMU_FQ_TTYPE_UADDR_WR = 3,
    RISCV_IOMMU_FQ_TTYPE_TADDR_INST_FETCH = 5,
    RISCV_IOMMU_FQ_TTYPE_TADDR_RD = 6,
    RISCV_IOMMU_FQ_TTYPE_TADDR_WR = 7,
    RISCV_IOMMU_FQ_TTYPE_PCIE_ATS_REQ = 8,
    RISCV_IOMMU_FW_TTYPE_PCIE_ਸੁਨੇਹਾ_REQ = 9,
};

/*
 * struct riscv_iommu_msi_pte - MSI Page Table Entry
 */
ਬਣਤਰ riscv_iommu_msi_pte {
      uint64_t pte;
      uint64_t mrif_info;
};

/* Fields on pte */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_PTE_V           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_PTE_M           GENMASK_ULL(2, 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_PTE_M_MRIF      1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_PTE_M_BASIC     3

/* When M == 1 (MRIF mode) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_PTE_MRIF_ADDR   GENMASK_ULL(53, 7)
/* When M == 3 (basic mode) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_PTE_PPN         RISCV_IOMMU_PPN_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_PTE_C           ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(63)

/* Fields on mrif_info */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_MRIF_NID        GENMASK_ULL(9, 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_MRIF_NPPN       RISCV_IOMMU_PPN_ਖੇਤਰ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_IOMMU_MSI_MRIF_NID_MSB    ਦੋਆਧਾਰੀ_ਅੰਕ_ULL(60)

#ਜੇ_ਅੰਤ /* _RISCV_IOMMU_BITS_H_ */
