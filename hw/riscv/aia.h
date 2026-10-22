/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#অন্তর্ভাব "exec/hwaddr.h"

uint32_t imsic_অঙ্ক_bits(uint32_t গুনতি);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, পূর্ণসংখ্যা aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t অঙ্ক_sources,
                             ধ্রুবক MemMapEntry *aplic_m,
                             ধ্রুবক MemMapEntry *aplic_s,
                             ধ্রুবক MemMapEntry *imsic_m,
                             ধ্রুবক MemMapEntry *imsic_s,
                             পূর্ণসংখ্যা socket, পূর্ণসংখ্যা base_hartid, পূর্ণসংখ্যা hart_গুনতি,
                             uint32_t অঙ্ক_msis, uint32_t অঙ্ক_prio_bits);

#endif
