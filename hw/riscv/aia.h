/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#அந்தர்பாவ "exec/hwaddr.h"

uint32_t imsic_இலக்கம்_bits(uint32_t மொத்தம்);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, முழு aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t இலக்கம்_sources,
                             மாறிலி MemMapEntry *aplic_m,
                             மாறிலி MemMapEntry *aplic_s,
                             மாறிலி MemMapEntry *imsic_m,
                             மாறிலி MemMapEntry *imsic_s,
                             முழு socket, முழு base_hartid, முழு hart_மொத்தம்,
                             uint32_t இலக்கம்_msis, uint32_t இலக்கம்_prio_bits);

#endif
