/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#અન્તર્ભાવ "exec/hwaddr.h"

uint32_t imsic_અંક_bits(uint32_t ગણતરી);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, પૂર્ણાંક aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t અંક_sources,
                             અચલ MemMapEntry *aplic_m,
                             અચલ MemMapEntry *aplic_s,
                             અચલ MemMapEntry *imsic_m,
                             અચલ MemMapEntry *imsic_s,
                             પૂર્ણાંક socket, પૂર્ણાંક base_hartid, પૂર્ણાંક hart_ગણતરી,
                             uint32_t અંક_msis, uint32_t અંક_prio_bits);

#endif
