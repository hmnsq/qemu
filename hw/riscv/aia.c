/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/kvm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aplic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_imsic.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "aia.h"

uint32_t imsic_ਅੰਕ_ਬਿੱਟ(uint32_t ਗਿਣਤੀ_2)
{
    uint32_t ret = 0;

    ਜਦੋਂ (ਦੋਆਧਾਰੀ_ਅੰਕ(ret) < ਗਿਣਤੀ_2) {
        ret++;
    }

    ਵਾਪਸ ret;
}

DeviceState *riscv_create_aia(ਸਮ੍ਰਿਤੀ_Region *container,
                             bool msimode, ਪੂਰਨ aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t ਅੰਕ_sources,
                             ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *aplic_m,
                             ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *aplic_s,
                             ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *imsic_m,
                             ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *imsic_s,
                             ਪੂਰਨ socket, ਪੂਰਨ ਅਧਾਰ_hartid, ਪੂਰਨ hart_ਗਿਣਤੀ,
                             uint32_t ਅੰਕ_msis, uint32_t ਅੰਕ_prio_ਬਿੱਟ)
{
    ਪੂਰਨ i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = ਨਿਰਾ;
    DeviceState *aplic_m_dev = ਨਿਰਾ;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(ਅੰਕ_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_ਮਾਪ(imsic_ਅੰਕ_ਬਿੱਟ(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_ਮਾਪ(0));

    ਜੇ (msimode) {
        ਜੇ (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->ਅਧਾਰ + socket * (1U << IMSIC_MMIO_ਸਮੂਹ_MIN_ਖਿਸਕਾਓ);
            ਲਈ (i = 0; i < hart_ਗਿਣਤੀ; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   ਅਧਾਰ_hartid + i, true, 1,
                                   ਅੰਕ_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->ਅਧਾਰ + socket * (1U << IMSIC_MMIO_ਸਮੂਹ_MIN_ਖਿਸਕਾਓ);
        ਲਈ (i = 0; i < hart_ਗਿਣਤੀ; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               ਅਧਾਰ_hartid + i, false, 1 + aia_guests,
                               ਅੰਕ_msis);
        }
    }

    ਜੇ (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->ਅਧਾਰ + socket * aplic_m->ਮਾਪ,
                                     aplic_m->ਮਾਪ,
                                     (msimode) ? 0 : ਅਧਾਰ_hartid,
                                     (msimode) ? 0 : hart_ਗਿਣਤੀ,
                                     ਅੰਕ_sources,
                                     ਅੰਕ_prio_ਬਿੱਟ,
                                     msimode, true, ਨਿਰਾ);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->ਅਧਾਰ + socket * aplic_s->ਮਾਪ,
                                 aplic_s->ਮਾਪ,
                                 (msimode) ? 0 : ਅਧਾਰ_hartid,
                                 (msimode) ? 0 : hart_ਗਿਣਤੀ,
                                 ਅੰਕ_sources,
                                 ਅੰਕ_prio_ਬਿੱਟ,
                                 msimode, false, aplic_m_dev);

    ਜੇ (kvm_enabled() && msimode) {
        riscv_aplic_ਸੈੱਟ_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    ਵਾਪਸ kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
