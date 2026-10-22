/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HW_RISCV_AIA_H
#ವ್ಯಾಖ್ಯಾನಿಸು HW_RISCV_AIA_H

#ಅನ್ತರ್ಭಾವ "exec/hwaddr.h"

uint32_t imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(uint32_t ಎಣಿಕೆ_2);

DeviceState *riscv_create_aia(ಸ್ಮರಣೆ_Region *container,
                             bool msimode, ಪೂರ್ಣಾಂಕ aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t ಅಂಕೆ_sources,
                             ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *aplic_m,
                             ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *aplic_s,
                             ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *imsic_m,
                             ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *imsic_s,
                             ಪೂರ್ಣಾಂಕ socket, ಪೂರ್ಣಾಂಕ ತಳ_hartid, ಪೂರ್ಣಾಂಕ hart_ಎಣಿಕೆ,
                             uint32_t ಅಂಕೆ_msis, uint32_t ಅಂಕೆ_prio_ಬಿಟ್ಟುಗಳು);

#ಆದರೆ_ಅಂತ್ಯ
