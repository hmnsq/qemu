/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#నిర్వచితం_కాకపోతే HW_RISCV_AIA_H
#నిర్వచించు HW_RISCV_AIA_H

#అన్తర్భావ "exec/hwaddr.h"

uint32_t imsic_అంకె_బిట్లు(uint32_t లెక్క);

DeviceState *riscv_create_aia(జ్ఞాపకం_Region *container,
                             bool msimode, పూర్ణాంకం aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t అంకె_sources,
                             స్థిరరాశి Mem_Map_నమోదు *aplic_m,
                             స్థిరరాశి Mem_Map_నమోదు *aplic_s,
                             స్థిరరాశి Mem_Map_నమోదు *imsic_m,
                             స్థిరరాశి Mem_Map_నమోదు *imsic_s,
                             పూర్ణాంకం socket, పూర్ణాంకం పునాది_hartid, పూర్ణాంకం hart_లెక్క,
                             uint32_t అంకె_msis, uint32_t అంకె_prio_బిట్లు);

#అయితే_ముగింపు
