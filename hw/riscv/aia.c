/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "system/kvm.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aplic.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_imsic.h"

#ಅನ್ತರ್ಭಾವ "aia.h"

uint32_t imsic_ಅಂಕೆ_bits(uint32_t ಎಣಿಕೆ)
{
    uint32_t ret = 0;

    ಯಾವತ್ (BIT(ret) < ಎಣಿಕೆ) {
        ret++;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, ಪೂರ್ಣಾಙ್ಕ aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t ಅಂಕೆ_sources,
                             ಧ್ರುವ MemMapEntry *aplic_m,
                             ಧ್ರುವ MemMapEntry *aplic_s,
                             ಧ್ರುವ MemMapEntry *imsic_m,
                             ಧ್ರುವ MemMapEntry *imsic_s,
                             ಪೂರ್ಣಾಙ್ಕ socket, ಪೂರ್ಣಾಙ್ಕ base_hartid, ಪೂರ್ಣಾಙ್ಕ hart_ಎಣಿಕೆ,
                             uint32_t ಅಂಕೆ_msis, uint32_t ಅಂಕೆ_prio_bits)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = ರಿಕ್ತ;
    DeviceState *aplic_m_dev = ರಿಕ್ತ;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(ಅಂಕೆ_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_ಅಂಕೆ_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    ಯದಿ (msimode) {
        ಯದಿ (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            ಪ್ರತಿ (i = 0; i < hart_ಎಣಿಕೆ; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   ಅಂಕೆ_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        ಪ್ರತಿ (i = 0; i < hart_ಎಣಿಕೆ; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               ಅಂಕೆ_msis);
        }
    }

    ಯದಿ (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_ಎಣಿಕೆ,
                                     ಅಂಕೆ_sources,
                                     ಅಂಕೆ_prio_bits,
                                     msimode, true, ರಿಕ್ತ);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_ಎಣಿಕೆ,
                                 ಅಂಕೆ_sources,
                                 ಅಂಕೆ_prio_bits,
                                 msimode, false, aplic_m_dev);

    ಯದಿ (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    ಪ್ರತ್ಯಾಗಮ kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
