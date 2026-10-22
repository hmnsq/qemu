/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HW_RISCV_AIA_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HW_RISCV_AIA_H

#ਅਨ੍ਤਰ੍ਭਾਵ "exec/hwaddr.h"

uint32_t imsic_ਅੰਕ_ਬਿੱਟ(uint32_t ਗਿਣਤੀ_2);

DeviceState *riscv_create_aia(ਸਮ੍ਰਿਤੀ_Region *container,
                             bool msimode, ਪੂਰਨ aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t ਅੰਕ_sources,
                             ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *aplic_m,
                             ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *aplic_s,
                             ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *imsic_m,
                             ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *imsic_s,
                             ਪੂਰਨ socket, ਪੂਰਨ ਅਧਾਰ_hartid, ਪੂਰਨ hart_ਗਿਣਤੀ,
                             uint32_t ਅੰਕ_msis, uint32_t ਅੰਕ_prio_ਬਿੱਟ);

#ਜੇ_ਅੰਤ
