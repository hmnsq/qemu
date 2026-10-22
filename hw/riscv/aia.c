/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#شامل "qemu/osdep.h"
#شامل "system/kvm.h"
#شامل "hw/intc/riscv_aplic.h"
#شامل "hw/intc/riscv_imsic.h"

#شامل "aia.h"

uint32_t imsic_رقم_bits(uint32_t شمار)
{
    uint32_t ret = 0;

    وقتی (BIT(ret) < شمار) {
        ret++;
    }

    بازگشت ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, صحیح aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t رقم_sources,
                             ثابت MemMapEntry *aplic_m,
                             ثابت MemMapEntry *aplic_s,
                             ثابت MemMapEntry *imsic_m,
                             ثابت MemMapEntry *imsic_s,
                             صحیح socket, صحیح base_hartid, صحیح hart_شمار,
                             uint32_t رقم_msis, uint32_t رقم_prio_bits)
{
    صحیح i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = پوچ;
    DeviceState *aplic_m_dev = پوچ;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(رقم_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_رقم_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    اگر (msimode) {
        اگر (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            برای (i = 0; i < hart_شمار; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   رقم_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        برای (i = 0; i < hart_شمار; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               رقم_msis);
        }
    }

    اگر (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_شمار,
                                     رقم_sources,
                                     رقم_prio_bits,
                                     msimode, true, پوچ);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_شمار,
                                 رقم_sources,
                                 رقم_prio_bits,
                                 msimode, false, aplic_m_dev);

    اگر (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    بازگشت kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
