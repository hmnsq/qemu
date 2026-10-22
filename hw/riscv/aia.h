/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#include "exec/hwaddr.h"

uint32_t imsic_नुमेरो_bits(uint32_t बिलांग);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, int aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t नुमेरो_sources,
                             const MemMapEntry *aplic_m,
                             const MemMapEntry *aplic_s,
                             const MemMapEntry *imsic_m,
                             const MemMapEntry *imsic_s,
                             int socket, int base_hartid, int hart_बिलांग,
                             uint32_t नुमेरो_msis, uint32_t नुमेरो_prio_bits);

#endif
