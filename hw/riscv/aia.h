/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#ଅନ୍ତର୍ଭାଵ "exec/hwaddr.h"

uint32_t imsic_ଅଙ୍କ_bits(uint32_t ଗଣନା_2);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t ଅଙ୍କ_sources,
                             ଅଚଳ MemMapEntry *aplic_m,
                             ଅଚଳ MemMapEntry *aplic_s,
                             ଅଚଳ MemMapEntry *imsic_m,
                             ଅଚଳ MemMapEntry *imsic_s,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_hartid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart_ଗଣନା,
                             uint32_t ଅଙ୍କ_msis, uint32_t ଅଙ୍କ_prio_bits);

#endif
