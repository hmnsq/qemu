/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#अन्तर्भाव "exec/hwaddr.h"

uint32_t imsic_अङ्क_bits(uint32_t गन्ती);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, पूर्णाङ्क aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t अङ्क_sources,
                             अचर MemMapEntry *aplic_m,
                             अचर MemMapEntry *aplic_s,
                             अचर MemMapEntry *imsic_m,
                             अचर MemMapEntry *imsic_s,
                             पूर्णाङ्क socket, पूर्णाङ्क base_hartid, पूर्णाङ्क hart_गन्ती,
                             uint32_t अङ्क_msis, uint32_t अङ्क_prio_bits);

#endif
