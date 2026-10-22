/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#അന്തര്ഭാവ "qemu/osdep.h"
#അന്തര്ഭാവ "system/kvm.h"
#അന്തര്ഭാവ "hw/intc/riscv_aplic.h"
#അന്തര്ഭാവ "hw/intc/riscv_imsic.h"

#അന്തര്ഭാവ "aia.h"

uint32_t imsic_അക്കം_bits(uint32_t എണ്ണം)
{
    uint32_t ret = 0;

    ആകുമ്പോൾ (BIT(ret) < എണ്ണം) {
        ret++;
    }

    തിരികെ ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, പൂർണ്ണസംഖ്യ aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t അക്കം_sources,
                             സ്ഥിരം MemMapEntry *aplic_m,
                             സ്ഥിരം MemMapEntry *aplic_s,
                             സ്ഥിരം MemMapEntry *imsic_m,
                             സ്ഥിരം MemMapEntry *imsic_s,
                             പൂർണ്ണസംഖ്യ socket, പൂർണ്ണസംഖ്യ base_hartid, പൂർണ്ണസംഖ്യ hart_എണ്ണം,
                             uint32_t അക്കം_msis, uint32_t അക്കം_prio_bits)
{
    പൂർണ്ണസംഖ്യ i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = ശൂന്യ;
    DeviceState *aplic_m_dev = ശൂന്യ;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(അക്കം_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_അക്കം_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    എങ്കിൽ (msimode) {
        എങ്കിൽ (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            വേണ്ടി (i = 0; i < hart_എണ്ണം; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   അക്കം_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        വേണ്ടി (i = 0; i < hart_എണ്ണം; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               അക്കം_msis);
        }
    }

    എങ്കിൽ (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_എണ്ണം,
                                     അക്കം_sources,
                                     അക്കം_prio_bits,
                                     msimode, true, ശൂന്യ);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_എണ്ണം,
                                 അക്കം_sources,
                                 അക്കം_prio_bits,
                                 msimode, false, aplic_m_dev);

    എങ്കിൽ (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    തിരികെ kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
