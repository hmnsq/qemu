/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ଅନ୍ତର୍ଭାଵ "qemu/osdep.h"
#ଅନ୍ତର୍ଭାଵ "system/kvm.h"
#ଅନ୍ତର୍ଭାଵ "hw/intc/riscv_aplic.h"
#ଅନ୍ତର୍ଭାଵ "hw/intc/riscv_imsic.h"

#ଅନ୍ତର୍ଭାଵ "aia.h"

uint32_t imsic_ଅଙ୍କ_bits(uint32_t ଗଣନା_2)
{
    uint32_t ret = 0;

    ଯେତେବେଳେ (BIT(ret) < ଗଣନା_2) {
        ret++;
    }

    ଫେରନ୍ତୁ ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t ଅଙ୍କ_sources,
                             ଅଚଳ MemMapEntry *aplic_m,
                             ଅଚଳ MemMapEntry *aplic_s,
                             ଅଚଳ MemMapEntry *imsic_m,
                             ଅଚଳ MemMapEntry *imsic_s,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_hartid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart_ଗଣନା,
                             uint32_t ଅଙ୍କ_msis, uint32_t ଅଙ୍କ_prio_bits)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = ଖାଲି;
    DeviceState *aplic_m_dev = ଖାଲି;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(ଅଙ୍କ_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_ଅଙ୍କ_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    ଯଦି (msimode) {
        ଯଦି (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            ପାଇଁ (i = 0; i < hart_ଗଣନା; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   ଅଙ୍କ_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        ପାଇଁ (i = 0; i < hart_ଗଣନା; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               ଅଙ୍କ_msis);
        }
    }

    ଯଦି (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_ଗଣନା,
                                     ଅଙ୍କ_sources,
                                     ଅଙ୍କ_prio_bits,
                                     msimode, true, ଖାଲି);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_ଗଣନା,
                                 ଅଙ୍କ_sources,
                                 ଅଙ୍କ_prio_bits,
                                 msimode, false, aplic_m_dev);

    ଯଦି (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    ଫେରନ୍ତୁ kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
