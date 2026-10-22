/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#ಅನ್ತರ್ಭಾವ "exec/hwaddr.h"

uint32_t imsic_ಅಂಕೆ_bits(uint32_t ಎಣಿಕೆ);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, ಪೂರ್ಣಾಙ್ಕ aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t ಅಂಕೆ_sources,
                             ಧ್ರುವ MemMapEntry *aplic_m,
                             ಧ್ರುವ MemMapEntry *aplic_s,
                             ಧ್ರುವ MemMapEntry *imsic_m,
                             ಧ್ರುವ MemMapEntry *imsic_s,
                             ಪೂರ್ಣಾಙ್ಕ socket, ಪೂರ್ಣಾಙ್ಕ base_hartid, ಪೂರ್ಣಾಙ್ಕ hart_ಎಣಿಕೆ,
                             uint32_t ಅಂಕೆ_msis, uint32_t ಅಂಕೆ_prio_bits);

#endif
