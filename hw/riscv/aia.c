/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#समावेश "qemu/osdep.h"
#समावेश "system/kvm.h"
#समावेश "hw/intc/riscv_aplic.h"
#समावेश "hw/intc/riscv_imsic.h"

#समावेश "aia.h"

uint32_t imsic_अंक_bits(uint32_t गिनती)
{
    uint32_t ret = 0;

    जबतक (BIT(ret) < गिनती) {
        ret++;
    }

    वापस ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, पूर्णांक aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t अंक_sources,
                             स्थिर MemMapEntry *aplic_m,
                             स्थिर MemMapEntry *aplic_s,
                             स्थिर MemMapEntry *imsic_m,
                             स्थिर MemMapEntry *imsic_s,
                             पूर्णांक socket, पूर्णांक base_hartid, पूर्णांक hart_गिनती,
                             uint32_t अंक_msis, uint32_t अंक_prio_bits)
{
    पूर्णांक i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = शून्य;
    DeviceState *aplic_m_dev = शून्य;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(अंक_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_आकार(imsic_अंक_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_आकार(0));

    अगर (msimode) {
        अगर (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            क्रम (i = 0; i < hart_गिनती; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   अंक_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        क्रम (i = 0; i < hart_गिनती; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               अंक_msis);
        }
    }

    अगर (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->आकार,
                                     aplic_m->आकार,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_गिनती,
                                     अंक_sources,
                                     अंक_prio_bits,
                                     msimode, true, शून्य);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->आकार,
                                 aplic_s->आकार,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_गिनती,
                                 अंक_sources,
                                 अंक_prio_bits,
                                 msimode, false, aplic_m_dev);

    अगर (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    वापस kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
