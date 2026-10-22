/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن "qemu/osdep.h"
#ضمن "system/kvm.h"
#ضمن "hw/intc/riscv_aplic.h"
#ضمن "hw/intc/riscv_imsic.h"

#ضمن "aia.h"

uint32_t imsic_عدد_بټونه(uint32_t شمېر)
{
    uint32_t ret = 0;

    طالما (بټ(ret) < شمېر) {
        ret++;
    }

    أرجع ret;
}

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
                             uint32_t عدد_msis, uint32_t عدد_prio_بټونه)
{
    صحيح i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = عدم;
    DeviceState *aplic_m_dev = عدم;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(عدد_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_اندازه(imsic_عدد_بټونه(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_اندازه(0));

    إذا (msimode) {
        إذا (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->بنسټ + socket * (1U << IMSIC_MMIO_ډله_MIN_ښويدنه);
            لكل (i = 0; i < hart_شمېر; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   بنسټ_hartid + i, true, 1,
                                   عدد_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->بنسټ + socket * (1U << IMSIC_MMIO_ډله_MIN_ښويدنه);
        لكل (i = 0; i < hart_شمېر; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               بنسټ_hartid + i, false, 1 + aia_guests,
                               عدد_msis);
        }
    }

    إذا (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->بنسټ + socket * aplic_m->اندازه,
                                     aplic_m->اندازه,
                                     (msimode) ? 0 : بنسټ_hartid,
                                     (msimode) ? 0 : hart_شمېر,
                                     عدد_sources,
                                     عدد_prio_بټونه,
                                     msimode, true, عدم);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->بنسټ + socket * aplic_s->اندازه,
                                 aplic_s->اندازه,
                                 (msimode) ? 0 : بنسټ_hartid,
                                 (msimode) ? 0 : hart_شمېر,
                                 عدد_sources,
                                 عدد_prio_بټونه,
                                 msimode, false, aplic_m_dev);

    إذا (kvm_enabled() && msimode) {
        riscv_aplic_سېټ_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    أرجع kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
