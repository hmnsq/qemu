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

uint32_t imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(uint32_t ಎಣಿಕೆ_2)
{
    uint32_t ret = 0;

    ಆಗ (ಬಿಟ್(ret) < ಎಣಿಕೆ_2) {
        ret++;
    }

    ಹಿಂತಿರುಗಿ ret;
}

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
                             uint32_t ಅಂಕೆ_msis, uint32_t ಅಂಕೆ_prio_ಬಿಟ್ಟುಗಳು)
{
    ಪೂರ್ಣಾಂಕ i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = ಖಾಲಿ;
    DeviceState *aplic_m_dev = ಖಾಲಿ;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(ಅಂಕೆ_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_ಗಾತ್ರ(imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_ಗಾತ್ರ(0));

    ಆದರೆ (msimode) {
        ಆದರೆ (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->ತಳ + socket * (1U << IMSIC_MMIO_ಗುಂಪು_MIN_ಸರಿಸು);
            ಗಾಗಿ (i = 0; i < hart_ಎಣಿಕೆ; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   ತಳ_hartid + i, true, 1,
                                   ಅಂಕೆ_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->ತಳ + socket * (1U << IMSIC_MMIO_ಗುಂಪು_MIN_ಸರಿಸು);
        ಗಾಗಿ (i = 0; i < hart_ಎಣಿಕೆ; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               ತಳ_hartid + i, false, 1 + aia_guests,
                               ಅಂಕೆ_msis);
        }
    }

    ಆದರೆ (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->ತಳ + socket * aplic_m->ಗಾತ್ರ_2,
                                     aplic_m->ಗಾತ್ರ_2,
                                     (msimode) ? 0 : ತಳ_hartid,
                                     (msimode) ? 0 : hart_ಎಣಿಕೆ,
                                     ಅಂಕೆ_sources,
                                     ಅಂಕೆ_prio_ಬಿಟ್ಟುಗಳು,
                                     msimode, true, ಖಾಲಿ);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->ತಳ + socket * aplic_s->ಗಾತ್ರ_2,
                                 aplic_s->ಗಾತ್ರ_2,
                                 (msimode) ? 0 : ತಳ_hartid,
                                 (msimode) ? 0 : hart_ಎಣಿಕೆ,
                                 ಅಂಕೆ_sources,
                                 ಅಂಕೆ_prio_ಬಿಟ್ಟುಗಳು,
                                 msimode, false, aplic_m_dev);

    ಆದರೆ (kvm_enabled() && msimode) {
        riscv_aplic_ಗಣ_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    ಹಿಂತಿರುಗಿ kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
