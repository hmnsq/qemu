/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#ضمن "exec/hwaddr.h"

uint32_t imsic_رقم_bits(uint32_t تعداد_2);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, صحيح aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t رقم_sources,
                             ثابت MemMapEntry *aplic_m,
                             ثابت MemMapEntry *aplic_s,
                             ثابت MemMapEntry *imsic_m,
                             ثابت MemMapEntry *imsic_s,
                             صحيح socket, صحيح base_hartid, صحيح hart_تعداد,
                             uint32_t رقم_msis, uint32_t رقم_prio_bits);

#endif
