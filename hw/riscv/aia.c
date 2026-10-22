/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "system/kvm.h"
#అన్తర్భావ "hw/intc/riscv_aplic.h"
#అన్తర్భావ "hw/intc/riscv_imsic.h"

#అన్తర్భావ "aia.h"

uint32_t imsic_అంకె_బిట్లు(uint32_t లెక్క)
{
    uint32_t ret = 0;

    అయితే (బిట్టు(ret) < లెక్క) {
        ret++;
    }

    తిరిగి ret;
}

DeviceState *riscv_create_aia(జ్ఞాపకం_Region *container,
                             bool msimode, పూర్ణాంకం aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t అంకె_sources,
                             స్థిరరాశి Mem_Map_నమోదు *aplic_m,
                             స్థిరరాశి Mem_Map_నమోదు *aplic_s,
                             స్థిరరాశి Mem_Map_నమోదు *imsic_m,
                             స్థిరరాశి Mem_Map_నమోదు *imsic_s,
                             పూర్ణాంకం socket, పూర్ణాంకం పునాది_hartid, పూర్ణాంకం hart_లెక్క,
                             uint32_t అంకె_msis, uint32_t అంకె_prio_బిట్లు)
{
    పూర్ణాంకం i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = ఖాళీ;
    DeviceState *aplic_m_dev = ఖాళీ;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(అంకె_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_కొలత(imsic_అంకె_బిట్లు(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_కొలత(0));

    ఒకవేళ (msimode) {
        ఒకవేళ (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->పునాది + socket * (1U << IMSIC_MMIO_సమూహం_MIN_జరుపు);
            కొరకు (i = 0; i < hart_లెక్క; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   పునాది_hartid + i, true, 1,
                                   అంకె_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->పునాది + socket * (1U << IMSIC_MMIO_సమూహం_MIN_జరుపు);
        కొరకు (i = 0; i < hart_లెక్క; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               పునాది_hartid + i, false, 1 + aia_guests,
                               అంకె_msis);
        }
    }

    ఒకవేళ (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->పునాది + socket * aplic_m->కొలత,
                                     aplic_m->కొలత,
                                     (msimode) ? 0 : పునాది_hartid,
                                     (msimode) ? 0 : hart_లెక్క,
                                     అంకె_sources,
                                     అంకె_prio_బిట్లు,
                                     msimode, true, ఖాళీ);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->పునాది + socket * aplic_s->కొలత,
                                 aplic_s->కొలత,
                                 (msimode) ? 0 : పునాది_hartid,
                                 (msimode) ? 0 : hart_లెక్క,
                                 అంకె_sources,
                                 అంకె_prio_బిట్లు,
                                 msimode, false, aplic_m_dev);

    ఒకవేళ (kvm_enabled() && msimode) {
        riscv_aplic_సమితి_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    తిరిగి kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
