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
                             bool msimode, পূর্ণাঙ্ক aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t অঙ্ক_sources,
                             ধ্রুব MemMapEntry *aplic_m,
                             ধ্রুব MemMapEntry *aplic_s,
                             ধ্রুব MemMapEntry *imsic_m,
                             ধ্রুব MemMapEntry *imsic_s,
                             পূর্ণাঙ্ক socket, পূর্ণাঙ্ক base_hartid, পূর্ণাঙ্ক hart_গুনতি,
                             uint32_t অঙ্ক_msis, uint32_t অঙ্ক_prio_bits);

#endif
