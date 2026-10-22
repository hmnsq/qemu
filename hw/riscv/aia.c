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

uint32_t imsic_انگ_بٽ(uint32_t ڳڻپ)
{
    uint32_t ret = 0;

    طالما (ٻٽو_انگ(ret) < ڳڻپ) {
        ret++;
    }

    أرجع ret;
}

DeviceState *riscv_create_aia(يادگيري_Region *container,
                             bool msimode, صحيح aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t انگ_sources,
                             ثابت Mem_Map_داخلا *aplic_m,
                             ثابت Mem_Map_داخلا *aplic_s,
                             ثابت Mem_Map_داخلا *imsic_m,
                             ثابت Mem_Map_داخلا *imsic_s,
                             صحيح socket, صحيح بنياد_hartid, صحيح hart_ڳڻپ,
                             uint32_t انگ_msis, uint32_t انگ_prio_بٽ)
{
    صحيح i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = عدم;
    DeviceState *aplic_m_dev = عدم;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(انگ_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_سائيز(imsic_انگ_بٽ(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_سائيز(0));

    إذا (msimode) {
        إذا (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->بنياد + socket * (1U << IMSIC_MMIO_گروهه_MIN_سرڪايو);
            لكل (i = 0; i < hart_ڳڻپ; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   بنياد_hartid + i, true, 1,
                                   انگ_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->بنياد + socket * (1U << IMSIC_MMIO_گروهه_MIN_سرڪايو);
        لكل (i = 0; i < hart_ڳڻپ; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               بنياد_hartid + i, false, 1 + aia_guests,
                               انگ_msis);
        }
    }

    إذا (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->بنياد + socket * aplic_m->سائيز,
                                     aplic_m->سائيز,
                                     (msimode) ? 0 : بنياد_hartid,
                                     (msimode) ? 0 : hart_ڳڻپ,
                                     انگ_sources,
                                     انگ_prio_بٽ,
                                     msimode, true, عدم);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->بنياد + socket * aplic_s->سائيز,
                                 aplic_s->سائيز,
                                 (msimode) ? 0 : بنياد_hartid,
                                 (msimode) ? 0 : hart_ڳڻپ,
                                 انگ_sources,
                                 انگ_prio_بٽ,
                                 msimode, false, aplic_m_dev);

    إذا (kvm_enabled() && msimode) {
        riscv_aplic_سيٽ_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    أرجع kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
