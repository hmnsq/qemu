/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "system/kvm.h"
#अन्तर्भाव "hw/intc/riscv_aplic.h"
#अन्तर्भाव "hw/intc/riscv_imsic.h"

#अन्तर्भाव "aia.h"

uint32_t imsic_अङ्कः_bits(uint32_t गणनम्)
{
    uint32_t ret = 0;

    याव (BIT(ret) < गणनम्) {
        ret++;
    }

    पटिनिवत्त ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, पूर्णाङ्क aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t अङ्कः_sources,
                             ध्रुव MemMapEntry *aplic_m,
                             ध्रुव MemMapEntry *aplic_s,
                             ध्रुव MemMapEntry *imsic_m,
                             ध्रुव MemMapEntry *imsic_s,
                             पूर्णाङ्क socket, पूर्णाङ्क base_hartid, पूर्णाङ्क hart_गणनम्,
                             uint32_t अङ्कः_msis, uint32_t अङ्कः_prio_bits)
{
    पूर्णाङ्क i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = रिक्त;
    DeviceState *aplic_m_dev = रिक्त;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(अङ्कः_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_अङ्कः_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    सचे (msimode) {
        सचे (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            अत्थाय (i = 0; i < hart_गणनम्; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   अङ्कः_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        अत्थाय (i = 0; i < hart_गणनम्; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               अङ्कः_msis);
        }
    }

    सचे (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_गणनम्,
                                     अङ्कः_sources,
                                     अङ्कः_prio_bits,
                                     msimode, true, रिक्त);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_गणनम्,
                                 अङ्कः_sources,
                                 अङ्कः_prio_bits,
                                 msimode, false, aplic_m_dev);

    सचे (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    पटिनिवत्त kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
