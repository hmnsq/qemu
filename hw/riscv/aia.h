/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#اگر_غیر_تعریف_شدہ HW_RISCV_AIA_H
#تعریف_کرو HW_RISCV_AIA_H

#شامل "exec/hwaddr.h"

uint32_t imsic_عدد_بٹ(uint32_t گنتی);

DeviceState *riscv_create_aia(حافظہ_Region *container,
                             bool msimode, صحیح aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t عدد_sources,
                             ثابت Mem_Map_اندراج *aplic_m,
                             ثابت Mem_Map_اندراج *aplic_s,
                             ثابت Mem_Map_اندراج *imsic_m,
                             ثابت Mem_Map_اندراج *imsic_s,
                             صحیح socket, صحیح بنیاد_hartid, صحیح hart_گنتی,
                             uint32_t عدد_msis, uint32_t عدد_prio_بٹ);

#اگر_ختم
