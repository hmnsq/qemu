/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#यदि_अपरिभाषितम् HW_RISCV_AIA_H
#परिभाषय HW_RISCV_AIA_H

#अन्तर्भाव "exec/hwaddr.h"

uint32_t imsic_अङ्कः_द्वयङ्काः(uint32_t गणनम्);

DeviceState *riscv_create_aia(स्मृतिः_Region *container,
                             bool msimode, पूर्णाङ्क aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t अङ्कः_sources,
                             ध्रुव Mem_Map_प्रविष्टिः *aplic_m,
                             ध्रुव Mem_Map_प्रविष्टिः *aplic_s,
                             ध्रुव Mem_Map_प्रविष्टिः *imsic_m,
                             ध्रुव Mem_Map_प्रविष्टिः *imsic_s,
                             पूर्णाङ्क socket, पूर्णाङ्क आधारः_hartid, पूर्णाङ्क hart_गणनम्,
                             uint32_t अङ्कः_msis, uint32_t अङ्कः_prio_द्वयङ्काः);

#यदि_अन्तः
