/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef HW_RISCV_AIA_H
#define HW_RISCV_AIA_H

#അന്തര്ഭാവ "exec/hwaddr.h"

uint32_t imsic_അക്കം_bits(uint32_t എണ്ണം);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, പൂർണ്ണസംഖ്യ aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t അക്കം_sources,
                             സ്ഥിരം MemMapEntry *aplic_m,
                             സ്ഥിരം MemMapEntry *aplic_s,
                             സ്ഥിരം MemMapEntry *imsic_m,
                             സ്ഥിരം MemMapEntry *imsic_s,
                             പൂർണ്ണസംഖ്യ socket, പൂർണ്ണസംഖ്യ base_hartid, പൂർണ്ണസംഖ്യ hart_എണ്ണം,
                             uint32_t അക്കം_msis, uint32_t അക്കം_prio_bits);

#endif
