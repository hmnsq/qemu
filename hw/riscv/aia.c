/*
 * QEMU RISC-V Advanced Interrupt Architecture (AIA)
 *
 * Copyright (C) 2019 Western Digital Corporation or its affiliates.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن "qemu/osdep.h"
#ضمن "system/kvm.h"
#ضمن "hw/intc/riscv_aplic.h"
#ضمن "hw/intc/riscv_imsic.h"

#ضمن "aia.h"

uint32_t imsic_لمب_bits(uint32_t كدي)
{
    uint32_t ret = 0;

    طالما (BIT(ret) < كدي) {
        ret++;
    }

    أرجع ret;
}

DeviceState *riscv_create_aia(MemoryRegion *container,
                             bool msimode, صحيح aia_guests,
                             uint32_t m_imsic_stride,
                             uint32_t s_imsic_stride,
                             uint16_t لمب_sources,
                             ثابت MemMapEntry *aplic_m,
                             ثابت MemMapEntry *aplic_s,
                             ثابت MemMapEntry *imsic_m,
                             ثابت MemMapEntry *imsic_s,
                             صحيح socket, صحيح base_hartid, صحيح hart_كدي,
                             uint32_t لمب_msis, uint32_t لمب_prio_bits)
{
    صحيح i;
    hwaddr addr = 0;
    DeviceState *aplic_s_dev = عدم;
    DeviceState *aplic_m_dev = عدم;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(لمب_sources <= 1023);

    /* Prevent IMSIC MMIO regions from silently overlapping */
    g_assert(s_imsic_stride >= IMSIC_HART_SIZE(imsic_لمب_bits(1 + aia_guests)));
    g_assert(m_imsic_stride >= IMSIC_HART_SIZE(0));

    إذا (msimode) {
        إذا (!kvm_enabled()) {
            /* Per-socket M-level IMSICs */
            addr = imsic_m->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
            لكل (i = 0; i < hart_كدي; i++) {
                riscv_imsic_create(container,
                                   addr + i * m_imsic_stride,
                                   base_hartid + i, true, 1,
                                   لمب_msis);
            }
        }

        /* Per-socket S-level IMSICs */
        addr = imsic_s->base + socket * (1U << IMSIC_MMIO_GROUP_MIN_SHIFT);
        لكل (i = 0; i < hart_كدي; i++) {
            riscv_imsic_create(container,
                               addr + i * s_imsic_stride,
                               base_hartid + i, false, 1 + aia_guests,
                               لمب_msis);
        }
    }

    إذا (!kvm_enabled()) {
        /* Per-socket M-level APLIC */
        aplic_m_dev = riscv_aplic_create(container,
                                     aplic_m->base + socket * aplic_m->size,
                                     aplic_m->size,
                                     (msimode) ? 0 : base_hartid,
                                     (msimode) ? 0 : hart_كدي,
                                     لمب_sources,
                                     لمب_prio_bits,
                                     msimode, true, عدم);
    }

    /* Per-socket S-level APLIC */
    aplic_s_dev = riscv_aplic_create(container,
                                 aplic_s->base + socket * aplic_s->size,
                                 aplic_s->size,
                                 (msimode) ? 0 : base_hartid,
                                 (msimode) ? 0 : hart_كدي,
                                 لمب_sources,
                                 لمب_prio_bits,
                                 msimode, false, aplic_m_dev);

    إذا (kvm_enabled() && msimode) {
        riscv_aplic_set_kvm_msicfgaddr(RISCV_APLIC(aplic_s_dev), addr);
    }

    أرجع kvm_enabled() ? aplic_s_dev : aplic_m_dev;
}
