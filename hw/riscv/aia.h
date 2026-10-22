/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#अगर_अघोषित HW_RISCV_AIA_H
#घोषणा HW_RISCV_AIA_H

#समावेश "exec/hwaddr.h"

uint32_t imsic_अंक_bits(uint32_t गिनती);

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, पूर्णांक aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t अंक_sources,
                             स्थिर MemMapEntry *aplic_m,
                             स्थिर MemMapEntry *aplic_s,
                             स्थिर MemMapEntry *imsic_m,
                             स्थिर MemMapEntry *imsic_s,
                             पूर्णांक socket, पूर्णांक base_hartid, पूर्णांक hart_गिनती,
                             uint32_t अंक_msis, uint32_t अंक_prio_bits);

#पूर्ण_अगर
