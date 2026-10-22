/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#অন্তর্ভাৱ "qemu/osdep.h"
#অন্তর্ভাৱ "system/kvm.h"
#অন্তর্ভাৱ "hw/intc/riscv_aplic.h"
#অন্তর্ভাৱ "hw/intc/riscv_imsic.h"

#অন্তর্ভাৱ "aia.h"

uint32_t imsic_অঙ্ক_bits(uint32_t গুনতি)
{
    uint32_t ret = 0;

    যেতিয়ালৈ (BIT(ret) < গুনতি) {
        ret++;
    }

    ঘূৰাই ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, পূর্ণাঙ্ক aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t অঙ্ক_sources,
                             ধ্রুৱ MemMapEntry *aplic_m,
                             ধ্রুৱ MemMapEntry *aplic_s,
                             ধ্রুৱ MemMapEntry *imsic_m,
                             ধ্রুৱ MemMapEntry *imsic_s,
                             পূর্ণাঙ্ক socket, পূর্ণাঙ্ক base_hartid, পূর্ণাঙ্ক hart_গুনতি,
                             uint32_t অঙ্ক_msis, uint32_t অঙ্ক_prio_bits)
{
    পূর্ণাঙ্ক i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = রিক্ত;
    DeviceState *aplic_m_dev = রিক্ত;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(অঙ্ক_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_অঙ্ক_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    যদি (msimode) {
        যদি (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            বাবে (i = 0; i < hart_গুনতি; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   অঙ্ক_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        বাবে (i = 0; i < hart_গুনতি; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               অঙ্ক_msis);
        }
    }

    যদি (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_গুনতি,
                                     অঙ্ক_sources,
                                     অঙ্ক_prio_bits,
                                     msimode, true, রিক্ত);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_গুনতি,
                                 অঙ্ক_sources,
                                 অঙ্ক_prio_bits,
                                 msimode, false, aplic_m_dev);

    যদি (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    ঘূৰাই kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
