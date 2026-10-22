/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#اگر_تعریف_نشده HW_RISCV_AIA_H
#تعریف_کن HW_RISCV_AIA_H

#ضمن "exec/hwaddr.h"

uint32_t imsic_رقم_بیتها(uint32_t شمار);

DeviceState *riscv_create_aia(حافظه_Region *container,
                             bool msimode, صحيح aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t رقم_sources,
                             ثابت Mem_Map_مدخل *aplic_m,
                             ثابت Mem_Map_مدخل *aplic_s,
                             ثابت Mem_Map_مدخل *imsic_m,
                             ثابت Mem_Map_مدخل *imsic_s,
                             صحيح socket, صحيح پایه_hartid, صحيح hart_شمار,
                             uint32_t رقم_msis, uint32_t رقم_prio_بیتها);

#پایان_اگر
