/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#அந்தர்பாவ "qemu/osdep.h"
#அந்தர்பாவ "system/kvm.h"
#அந்தர்பாவ "hw/intc/riscv_aplic.h"
#அந்தர்பாவ "hw/intc/riscv_imsic.h"

#அந்தர்பாவ "aia.h"

uint32_t imsic_இலக்கம்_bits(uint32_t மொத்தம்)
{
    uint32_t ret = 0;

    போது (BIT(ret) < மொத்தம்) {
        ret++;
    }

    திரும்பு ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, முழு aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t இலக்கம்_sources,
                             மாறிலி MemMapEntry *aplic_m,
                             மாறிலி MemMapEntry *aplic_s,
                             மாறிலி MemMapEntry *imsic_m,
                             மாறிலி MemMapEntry *imsic_s,
                             முழு socket, முழு base_hartid, முழு hart_மொத்தம்,
                             uint32_t இலக்கம்_msis, uint32_t இலக்கம்_prio_bits)
{
    முழு i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = வெற்று;
    DeviceState *aplic_m_dev = வெற்று;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(இலக்கம்_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_இலக்கம்_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    எனில் (msimode) {
        எனில் (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            க்கு (i = 0; i < hart_மொத்தம்; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   இலக்கம்_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        க்கு (i = 0; i < hart_மொத்தம்; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               இலக்கம்_msis);
        }
    }

    எனில் (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_மொத்தம்,
                                     இலக்கம்_sources,
                                     இலக்கம்_prio_bits,
                                     msimode, true, வெற்று);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_மொத்தம்,
                                 இலக்கம்_sources,
                                 இலக்கம்_prio_bits,
                                 msimode, false, aplic_m_dev);

    எனில் (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    திரும்பு kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
