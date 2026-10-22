/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#که_نه_تعریف_شوی HW_RISCV_AIA_H
#تعریف_کړه HW_RISCV_AIA_H

#ضمن "exec/hwaddr.h"

uint32_t imsic_عدد_بټونه(uint32_t شمېر);

DeviceState *riscv_create_aia(حافظه_Region *container,
                             bool msimode, صحيح aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t عدد_sources,
                             ثابت Mem_Map_ثبت *aplic_m,
                             ثابت Mem_Map_ثبت *aplic_s,
                             ثابت Mem_Map_ثبت *imsic_m,
                             ثابت Mem_Map_ثبت *imsic_s,
                             صحيح socket, صحيح بنسټ_hartid, صحيح hart_شمېر,
                             uint32_t عدد_msis, uint32_t عدد_prio_بټونه);

#د_که_پای
