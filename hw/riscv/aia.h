/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#جيڪڏهن_وصف_نه_ٿيل HW_RISCV_AIA_H
#وصف_ڪريو HW_RISCV_AIA_H

#ضمن "exec/hwaddr.h"

uint32_t imsic_انگ_بٽ(uint32_t ڳڻپ);

DeviceState *riscv_create_aia(يادگيري_Region *container,
                             bool msimode, صحيح aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t انگ_sources,
                             ثابت Mem_Map_داخلا *aplic_m,
                             ثابت Mem_Map_داخلا *aplic_s,
                             ثابت Mem_Map_داخلا *imsic_m,
                             ثابت Mem_Map_داخلا *imsic_s,
                             صحيح socket, صحيح بنياد_hartid, صحيح hart_ڳڻپ,
                             uint32_t انگ_msis, uint32_t انگ_prio_بٽ);

#جيڪڏهن_پورو
