/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#અન્તર્ભાવ "qemu/osdep.h"
#અન્તર્ભાવ "system/kvm.h"
#અન્તર્ભાવ "hw/intc/riscv_aplic.h"
#અન્તર્ભાવ "hw/intc/riscv_imsic.h"

#અન્તર્ભાવ "aia.h"

uint32_t imsic_અંક_bits(uint32_t ગણતરી)
{
    uint32_t ret = 0;

    જ્યારે (BIT(ret) < ગણતરી) {
        ret++;
    }

    પાછા ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, પૂર્ણાંક aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t અંક_sources,
                             અચલ MemMapEntry *aplic_m,
                             અચલ MemMapEntry *aplic_s,
                             અચલ MemMapEntry *imsic_m,
                             અચલ MemMapEntry *imsic_s,
                             પૂર્ણાંક socket, પૂર્ણાંક base_hartid, પૂર્ણાંક hart_ગણતરી,
                             uint32_t અંક_msis, uint32_t અંક_prio_bits)
{
    પૂર્ણાંક i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = ખાલી;
    DeviceState *aplic_m_dev = ખાલી;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(અંક_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_અંક_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    જો (msimode) {
        જો (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            માટે (i = 0; i < hart_ગણતરી; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   અંક_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        માટે (i = 0; i < hart_ગણતરી; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               અંક_msis);
        }
    }

    જો (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_ગણતરી,
                                     અંક_sources,
                                     અંક_prio_bits,
                                     msimode, true, ખાલી);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_ગણતરી,
                                 અંક_sources,
                                 અંક_prio_bits,
                                 msimode, false, aplic_m_dev);

    જો (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    પાછા kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
