/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#شامل "qemu/osdep.h"
#شامل "system/kvm.h"
#شامل "hw/intc/riscv_aplic.h"
#شامل "hw/intc/riscv_imsic.h"

#شامل "aia.h"

uint32_t imsic_عدد_بٹ(uint32_t گنتی)
{
    uint32_t ret = 0;

    جب (ثنائی_ہندسہ(ret) < گنتی) {
        ret++;
    }

    واپس ret;
}

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
                             uint32_t عدد_msis, uint32_t عدد_prio_بٹ)
{
    صحیح i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = تہی;
    DeviceState *aplic_m_dev = تہی;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(عدد_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_سائز(imsic_عدد_بٹ(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_سائز(0));

    اگر (msimode) {
        اگر (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->بنیاد + socket * (1U << IMSIC_MMIO_گروہ_MIN_سرکاؤ);
            برائے (i = 0; i < hart_گنتی; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   بنیاد_hartid + i, true, 1,
                                   عدد_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->بنیاد + socket * (1U << IMSIC_MMIO_گروہ_MIN_سرکاؤ);
        برائے (i = 0; i < hart_گنتی; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               بنیاد_hartid + i, false, 1 + aia_guests,
                               عدد_msis);
        }
    }

    اگر (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->بنیاد + socket * aplic_m->سائز,
                                     aplic_m->سائز,
                                     (msimode) ? 0 : بنیاد_hartid,
                                     (msimode) ? 0 : hart_گنتی,
                                     عدد_sources,
                                     عدد_prio_بٹ,
                                     msimode, true, تہی);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->بنیاد + socket * aplic_s->سائز,
                                 aplic_s->سائز,
                                 (msimode) ? 0 : بنیاد_hartid,
                                 (msimode) ? 0 : hart_گنتی,
                                 عدد_sources,
                                 عدد_prio_بٹ,
                                 msimode, false, aplic_m_dev);

    اگر (kvm_enabled() && msimode) {
        riscv_aplic_سیٹ_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    واپس kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
