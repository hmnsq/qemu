/*
 * Tenstorrent Atlantis RISC-V System on Chip
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright 2025 Tenstorrent, Joel Stanley <joel@jms.id.au>
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/cutils.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qemu/guest-random.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"

#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"

#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"

#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/fdt-common.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"

#ಅನ್ತರ್ಭಾವ "hw/char/serial-mm.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/unimp.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/tt_atlantis_prcm.h"

#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"

#ಅನ್ತರ್ಭಾವ "hw/riscv/tt_atlantis.h"

#ಅನ್ತರ್ಭಾವ "aia.h"

#ವ್ಯಾಖ್ಯಾನಿಸು TT_IRQCHIP_ಅಂಕೆ_MSIS       255
#ವ್ಯಾಖ್ಯಾನಿಸು TT_IRQCHIP_ಅಂಕೆ_SOURCES    128
#ವ್ಯಾಖ್ಯಾನಿಸು TT_IRQCHIP_ಅಂಕೆ_PRIO_ಬಿಟ್ಟುಗಳು  3
#ವ್ಯಾಖ್ಯಾನಿಸು TT_IMSIC_GUESTS           5
#ವ್ಯಾಖ್ಯಾನಿಸು TT_IMSIC_STRIDE           0x40000 /* Same stride for M and S */
#ವ್ಯಾಖ್ಯಾನಿಸು TT_IMSIC_GUEST_ಬಿಟ್ಟುಗಳು       6

/* Stride is fixed by hardware, check it's consistent with guest bits. */
QEMU_BUILD_BUG_ON(TT_IMSIC_STRIDE != (0x1000 << TT_IMSIC_GUEST_ಬಿಟ್ಟುಗಳು));

#ವ್ಯಾಖ್ಯಾನಿಸು TT_ACLINT_MTIME_ಗಾತ್ರ    0x8050
#ವ್ಯಾಖ್ಯಾನಿಸು TT_ACLINT_MTIME         0x0
#ವ್ಯಾಖ್ಯಾನಿಸು TT_ACLINT_MTIMECMP      0x8000
#ವ್ಯಾಖ್ಯಾನಿಸು TT_ACLINT_TIMEBASE_FREQ 1000000000

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು tt_atlantis_memmap[] = {
    /* Keep sorted with :'<,'>!sort -g -k 4 */
    [TT_ATL_DDR_LO] =           { 0x00000000,    0x80000000 },
    [TT_ATL_BOOTROM] =          { 0x80000000,        0x2000 },
    [TT_ATL_MIMSIC] =           { 0xa0000000,      0x200000 },
    [TT_ATL_ACLINT] =           { 0xa2180000,       0x10000 },
    [TT_ATL_SIMSIC] =           { 0xa4000000,      0x200000 },
    [TT_ATL_MAPLIC] =           { 0xcc000000,     0x4000000 },
    [TT_ATL_PRCM_RCPU] =        { 0xd0000000,       0x10000 },
    [TT_ATL_I2C0] =             { 0xd4040000,       0x10000 },
    [TT_ATL_I2C1] =             { 0xd4050000,       0x10000 },
    [TT_ATL_I2C2] =             { 0xd4060000,       0x10000 },
    [TT_ATL_I2C3] =             { 0xd4070000,       0x10000 },
    [TT_ATL_I2C4] =             { 0xd4080000,       0x10000 },
    [TT_ATL_UART1] =            { 0xd4110000,       0x10000 },
    [TT_ATL_PRCM_PCIE] =        { 0xd8000000,         0x100 },
    [TT_ATL_PRCM_MM] =          { 0xdc000000,        0x1000 },
    [TT_ATL_PRCM_HSIO] =        { 0xe00c0000,         0x510 },
    [TT_ATL_SAPLIC] =           { 0xe8000000,     0x4000000 },
    [TT_ATL_DDR_HI] =          { 0x100000000,  0x1000000000 },
};

ಸ್ಥಿರ I2CBus *i2c_get_bus(TTAtlantisSoCState *s, ಚಿಹ್ನೆರಹಿತ busnr)
{
    ಪ್ರತಿಜ್ಞಾ(busnr < TT_ATL_ಅಂಕೆ_I2_C);

    ಹಿಂತಿರುಗಿ s->i2c[busnr].bus;
}

ಸ್ಥಿರ uint32_t fdt_phandle = 1;
ಸ್ಥಿರ uint32_t ಮುಂದಿನ_phandle(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ fdt_phandle++;
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_ಸ್ಮರಣೆ(ಶೂನ್ಯ *fdt, TTAtlantisSoCState *s)
{
    hwaddr ram_ಗಾತ್ರ = ಸ್ಮರಣೆ_region_ಗಾತ್ರ(s->dram);
    hwaddr ಗಾತ್ರ_lo = ram_ಗಾತ್ರ;
    hwaddr ಗಾತ್ರ_hi = 0;

    ಆದರೆ (ಗಾತ್ರ_lo > s->memmap[TT_ATL_DDR_LO].ಗಾತ್ರ_2) {
        ಗಾತ್ರ_lo = s->memmap[TT_ATL_DDR_LO].ಗಾತ್ರ_2;
        ಗಾತ್ರ_hi = ram_ಗಾತ್ರ - ಗಾತ್ರ_lo;
    }

    riscv_create_fdt_socket_ಸ್ಮರಣೆ(fdt, s->memmap[TT_ATL_DDR_LO].ತಳ,
                                   ಗಾತ್ರ_lo, 0, false);
    ಆದರೆ (ಗಾತ್ರ_hi) {
        /*
         * The first part of the HI address is aliased at the LO address
         * so do not include that as usable memory. Is there any way
         * (or good reason) to describe that aliasing 2GB with DT?
         */
        riscv_create_fdt_socket_ಸ್ಮರಣೆ(fdt,
                                       s->memmap[TT_ATL_DDR_HI].ತಳ + ಗಾತ್ರ_lo,
                                       ಗಾತ್ರ_hi, 0, false);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_aclint(ಶೂನ್ಯ *fdt, TTAtlantisSoCState *s,
                              uint32_t *intc_phandles)
{
    g_autofree ಅಕ್ಷರ *name = ಖಾಲಿ;
    g_autofree uint32_t *aclint_mtimer_cells = ಖಾಲಿ;
    uint32_t aclint_cells_ಗಾತ್ರ;
    hwaddr addr;

    aclint_mtimer_cells = g_new0(uint32_t, s->cpus.ಅಂಕೆ_harts * 2);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cpu = 0; cpu < s->cpus.ಅಂಕೆ_harts; cpu++) {
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(IRQ_M_TIMER);
    }
    aclint_cells_ಗಾತ್ರ = s->cpus.ಅಂಕೆ_harts * ಗಾತ್ರ(uint32_t) * 2;

    addr = s->memmap[TT_ATL_ACLINT].ತಳ;

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIX, addr);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr + TT_ACLINT_MTIME,
                                 2, 0x1000,
                                 2, addr + TT_ACLINT_MTIMECMP,
                                 2, 0x1000);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_ಗಾತ್ರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_one_imsic(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *mem, ಪೂರ್ಣಾಂಕ cpus,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 ಪೂರ್ಣಾಂಕ irq_ಸಾಲು, uint32_t imsic_guest_ಬಿಟ್ಟುಗಳು)
{
    g_autofree ಅಕ್ಷರ *name = ಖಾಲಿ;
    g_autofree uint32_t *imsic_cells = g_new0(uint32_t, cpus * 2);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cpu = 0; cpu < cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(irq_ಸಾಲು);
    }

    name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->ತಳ);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,imsics");

    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", ಖಾಲಿ, 0);
    qemu_fdt_setprop(fdt, name, "msi-controller", ಖಾಲಿ, 0);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     imsic_cells, ಗಾತ್ರ(uint32_t) * cpus * 2);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ತಳ, 2, mem->ಗಾತ್ರ_2);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-ids", TT_IRQCHIP_ಅಂಕೆ_MSIS);

    ಆದರೆ (imsic_guest_ಬಿಟ್ಟುಗಳು) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,guest-index-bits",
                              imsic_guest_ಬಿಟ್ಟುಗಳು);
    }
    qemu_fdt_setprop_cell(fdt, name, "phandle", msi_phandle);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_one_aplic(ಶೂನ್ಯ *fdt,
                                 ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *mem,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_ಮಗು_phandle,
                                 ಪೂರ್ಣಾಂಕ irq_ಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_harts)
{
    g_autofree ಅಕ್ಷರ *name =
        g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->ತಳ);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, ಅಂಕೆ_harts * 2);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cpu = 0; cpu < ಅಂಕೆ_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(irq_ಸಾಲು);
    }

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aplic");
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 2);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", ಖಾಲಿ, 0);

    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aplic_cells, ಅಂಕೆ_harts * ಗಾತ್ರ(uint32_t) * 2);
    qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_phandle);

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ತಳ, 2, mem->ಗಾತ್ರ_2);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-sources",
                          TT_IRQCHIP_ಅಂಕೆ_SOURCES);

    ಆದರೆ (aplic_ಮಗು_phandle) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,children",
                              aplic_ಮಗು_phandle);
        qemu_fdt_setprop_cells(fdt, name, "riscv,delegation",
                               aplic_ಮಗು_phandle, 1, TT_IRQCHIP_ಅಂಕೆ_SOURCES);
    }

    qemu_fdt_setprop_cell(fdt, name, "phandle", aplic_phandle);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_pmu(ಶೂನ್ಯ *fdt, TTAtlantisSoCState *s)
{
    ಅಕ್ಷರ pmu_name[] = "/pmu";
    RISCVCPU *hart = &s->cpus.harts[0];

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, pmu_name);
    qemu_fdt_setprop_string(fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_ಗಂಟು(fdt, hart->pmu_avail_ctrs, pmu_name);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_cpu(ಶೂನ್ಯ *fdt, TTAtlantisSoCState *s,
                           uint32_t aplic_s_phandle,
                           uint32_t imsic_s_phandle)
{
    g_autofree uint32_t *intc_phandles = g_new0(uint32_t, s->cpus.ಅಂಕೆ_harts);
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_harts = s->cpus.ಅಂಕೆ_harts;

    riscv_fdt_create_cpu_socket_subnode(fdt, TT_ACLINT_TIMEBASE_FREQ);

    riscv_create_fdt_socket_cpus(fdt, s->cpus.harts, 0, ಅಂಕೆ_harts,
                                 s->cpus.hartid_ತಳ, &fdt_phandle,
                                 intc_phandles, false, false);

    create_fdt_aclint(fdt, s, intc_phandles);

    /* M-level IMSIC node */
    uint32_t msi_m_phandle = ಮುಂದಿನ_phandle();
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_MIMSIC], ಅಂಕೆ_harts,
                         intc_phandles, msi_m_phandle,
                         IRQ_M_EXT, TT_IMSIC_GUEST_ಬಿಟ್ಟುಗಳು);

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_SIMSIC], ಅಂಕೆ_harts,
                         intc_phandles, imsic_s_phandle,
                         IRQ_S_EXT, TT_IMSIC_GUEST_ಬಿಟ್ಟುಗಳು);

    uint32_t aplic_m_phandle = ಮುಂದಿನ_phandle();

    /* M-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_MAPLIC],
                         msi_m_phandle, intc_phandles,
                         aplic_m_phandle, aplic_s_phandle,
                         IRQ_M_EXT, ಅಂಕೆ_harts);

    /* S-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_SAPLIC],
                         imsic_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         IRQ_S_EXT, ಅಂಕೆ_harts);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_uart(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *mem, ಪೂರ್ಣಾಂಕ irq,
                            ಪೂರ್ಣಾಂಕ irqchip_phandle, uint32_t clk_ಪೋಷಕ_phandle,
                            uint32_t clk_phandle)
{
    g_autofree ಅಕ್ಷರ *name = g_strdup_printf("/soc/serial@%"HWADDR_PRIX,
                                            mem->ತಳ);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ತಳ, 2, mem->ಗಾತ್ರ_2);
    qemu_fdt_setprop_cell(fdt, name, "reg-shift", 2);
    qemu_fdt_setprop_cell(fdt, name, "reg-io-width", 4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_ಪೋಷಕ_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 3686400);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);

    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(fdt, "/aliases", "serial0", name);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_rng(ಶೂನ್ಯ *fdt)
{
    uint8_t rng_ಬೀಜ[32];

    qemu_guest_getrandom_nofail(rng_ಬೀಜ, ಗಾತ್ರ(rng_ಬೀಜ));
    qemu_fdt_setprop(fdt, "/chosen", "rng-seed", rng_ಬೀಜ, ಗಾತ್ರ(rng_ಬೀಜ));
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_clk(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clock_name,
                           uint32_t freq, uint32_t phandle)
{
    g_autofree ಅಕ್ಷರ *name = g_strdup_printf("/clocks/%s", clock_name);

    qemu_fdt_ಕೂಡಿಸು_ಮಾರ್ಗ(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "fixed-clock");
    qemu_fdt_setprop_string(fdt, name, "clock-output-names", clock_name);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", freq);
    qemu_fdt_setprop_cell(fdt, name, "phandle", phandle);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_i2c(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *mem, uint32_t irq,
                           uint32_t irqchip_phandle,
                           uint32_t clk_ಪೋಷಕ_phandle,
                           uint32_t clk_phandle)
{
    g_autofree ಅಕ್ಷರ *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX, mem->ತಳ);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "snps,designware-i2c");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ತಳ, 2, mem->ಗಾತ್ರ_2);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_ಪೋಷಕ_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 100000);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_i2c_device(ಶೂನ್ಯ *fdt, TTAtlantisSoCState *s, ಪೂರ್ಣಾಂಕ bus,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compat, ಪೂರ್ಣಾಂಕ addr)
{
    hwaddr ತಳ = s->memmap[TT_ATL_I2C0 + bus].ತಳ;
    g_autofree ಅಕ್ಷರ *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX"/sensor@%x",
                                            ತಳ, addr);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compat);
    qemu_fdt_setprop_cell(fdt, name, "reg", addr);
}

ಸ್ಥಿರ ಅಕ್ಷರ *create_fdt_prcm(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *mem,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prcm_name)
{
    hwaddr ತಳ = mem->ತಳ;
    hwaddr ಗಾತ್ರ_2 = mem->ಗಾತ್ರ_2;
    ಅಕ್ಷರ *name = g_strdup_printf("/soc/clock-controller@%" PRIx64,
                                 ತಳ);
    g_autofree ಅಕ್ಷರ *compatible =
        g_strdup_printf("tenstorrent,atlantis-prcm-%s", prcm_name);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compatible);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, ತಳ, 2, ಗಾತ್ರ_2);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#reset-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "phandle", ಮುಂದಿನ_phandle());

    ಹಿಂತಿರುಗಿ name;
}

ಸ್ಥಿರ ಶೂನ್ಯ finalize_fdt(ಶೂನ್ಯ *fdt, TTAtlantisSoCState *s)
{
    uint32_t aplic_s_phandle = ಮುಂದಿನ_phandle();
    uint32_t imsic_s_phandle = ಮುಂದಿನ_phandle();
    uint32_t osc_24m_phandle = ಮುಂದಿನ_phandle();
    uint32_t prcm_rcpu_phandle;
    g_autofree ಅಕ್ಷರ *rcpu_name, *hsio_name, *pcie_name, *mm_name;

    create_fdt_cpu(fdt, s, aplic_s_phandle, imsic_s_phandle);

    create_fdt_ಸ್ಮರಣೆ(fdt, s);

    /*
     * We want to do this, but the Linux aplic driver was broken before v6.16
     *
     * qemu_fdt_setprop_cell(MACHINE(s)->fdt, "/soc", "interrupt-parent",
     *                       aplic_s_phandle);
     */

    create_fdt_clk(fdt, "osc-24m", 24000000, osc_24m_phandle);

    rcpu_name = create_fdt_prcm(fdt, &s->memmap[TT_ATL_PRCM_RCPU], "rcpu");
    prcm_rcpu_phandle = qemu_fdt_get_phandle(fdt, rcpu_name);
    qemu_fdt_setprop_cells(fdt, rcpu_name, "clocks", osc_24m_phandle);
    qemu_fdt_setprop_cells(fdt, rcpu_name, "assigned-clocks",
                           prcm_rcpu_phandle, TT_ATL_CLK_RCPU_ಬೇರು,
                           prcm_rcpu_phandle, TT_ATL_CLK_NOCC_CLK);
    qemu_fdt_setprop_cells(fdt, rcpu_name, "assigned-clock-parents",
                           prcm_rcpu_phandle, TT_ATL_CLK_RCPU_PLL,
                           prcm_rcpu_phandle, TT_ATL_CLK_NOC_PLL);

    hsio_name = create_fdt_prcm(fdt, &s->memmap[TT_ATL_PRCM_HSIO], "hsio");
    qemu_fdt_setprop_cells(fdt, hsio_name, "clocks", osc_24m_phandle,
                           prcm_rcpu_phandle, TT_ATL_CLK_HSIO_PLL);

    pcie_name = create_fdt_prcm(fdt, &s->memmap[TT_ATL_PRCM_PCIE], "pcie");
    qemu_fdt_setprop_cells(fdt, pcie_name, "clocks", osc_24m_phandle,
                           prcm_rcpu_phandle, TT_ATL_CLK_PCIE_PLL);

    mm_name = create_fdt_prcm(fdt, &s->memmap[TT_ATL_PRCM_MM], "mm");
    qemu_fdt_setprop_cells(fdt, mm_name, "clocks", osc_24m_phandle,
                           prcm_rcpu_phandle, TT_ATL_CLK_MM_PLL0,
                           prcm_rcpu_phandle, TT_ATL_CLK_MM_PLL1);

    create_fdt_uart(fdt, &s->memmap[TT_ATL_UART1], TT_ATL_UART1_IRQ,
                    aplic_s_phandle, prcm_rcpu_phandle, TT_ATL_CLK_UART1_PCLK);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < TT_ATL_ಅಂಕೆ_I2_C; i++) {
        create_fdt_i2c(fdt,
                       &s->memmap[TT_ATL_I2C0 + i],
                       TT_ATL_I2C0_IRQ + i,
                       aplic_s_phandle, prcm_rcpu_phandle,
                       TT_ATL_CLK_I2C0_PCLK + i);
    }

    /* I2C peripherals: qemu specific */
    create_fdt_i2c_device(fdt, s, 0, "dallas,ds1338", 0x6f);
    create_fdt_i2c_device(fdt, s, 4, "ti,tmp105", 0x48);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    ಪೂರ್ಣಾಂಕ fdt_ಗಾತ್ರ = 0;

    ms->fdt = riscv_create_board_device_ವೃಕ್ಷ("Tenstorrent Atlantis RISC-V Machine",
                                             "tenstorrent,atlantis", &fdt_ಗಾತ್ರ);

    qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, "/chosen");

    create_fdt_rng(ms->fdt);

    qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, "/aliases");

    create_fdt_pmu(ms->fdt, &ams->soc);
}

ಸ್ಥಿರ ಶೂನ್ಯ ತುಂಬು_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    ಅಕ್ಷರ **ಗಂಟು_ಮಾರ್ಗ;
    Error *err = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ fdt_ಗಾತ್ರ = 0;

    ms->fdt = ತುಂಬು_device_ವೃಕ್ಷ(ms->dtb, &fdt_ಗಾತ್ರ);
    ಆದರೆ (!ms->fdt) {
        error_report("load_device_tree() failed");
        ನಿರ್ಗಮಿಸು(1);
    }

    qemu_fdt_ಕೂಡಿಸು_ಮಾರ್ಗ(ms->fdt, "/chosen");

    /* Clear memory nodes and update with the specified RAM size */
    ಗಂಟು_ಮಾರ್ಗ = qemu_fdt_ಗಂಟು_unit_ಮಾರ್ಗ(ms->fdt, "memory", &err);
    ಆದರೆ (err) {
        warn_report_err(err);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; ಗಂಟು_ಮಾರ್ಗ[i]; i++) {
            warn_report("Replacing device tree %s with the requested RAM size",
                        ಗಂಟು_ಮಾರ್ಗ[i]);
            qemu_fdt_nop_ಗಂಟು(ms->fdt, ಗಂಟು_ಮಾರ್ಗ[i]);
        }
        g_strfreev(ಗಂಟು_ಮಾರ್ಗ);
    }

    create_fdt_ಸ್ಮರಣೆ(ms->fdt, &ams->soc);
}

ಸ್ಥಿರ ಶೂನ್ಯ mmio_map_unimplemented(ಸ್ಮರಣೆ_Region *ಸ್ಮರಣೆ, SysBusDevice *dev,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, hwaddr addr, uint64_t ಗಾತ್ರ_2)
{
    qdev_prop_ಗಣ_string(DEVICE(dev), "name", name);
    qdev_prop_ಗಣ_uint64(DEVICE(dev), "size", ಗಾತ್ರ_2);
    sysbus_realize(dev, &error_abort);

    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion_overlap(ಸ್ಮರಣೆ, addr,
                                        sysbus_mmio_get_region(dev, 0), -1000);
}

ಸ್ಥಿರ ಶೂನ್ಯ tt_atlantis_machine_ಮುಗಿದ(Notifier *n, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    TTAtlantisState *ams = container_ದ(n, TTAtlantisState, machine_ಮುಗಿದ);
    TTAtlantisSoCState *s = &ams->soc;
    MachineState *machine = MACHINE(ams);
    hwaddr ಆರಂಭ_addr = s->memmap[TT_ATL_DDR_LO].ತಳ;
    hwaddr mem_ಗಾತ್ರ;
    ಗುರಿ_ulong firmware_ಕೊನೆ_addr, kernel_ಆರಂಭ_addr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *firmware_name = riscv_default_firmware_name(&s->cpus);
    uint64_t fdt_ತುಂಬು_addr;
    uint64_t kernel_ನಮೂದು;
    RISCVBootInfo boot_info;

    /*
     * A user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    ಆದರೆ (machine->dtb == ಖಾಲಿ) {
        finalize_fdt(machine->fdt, s);
    }

    mem_ಗಾತ್ರ = machine->ram_ಗಾತ್ರ;
    ಆದರೆ (mem_ಗಾತ್ರ > s->memmap[TT_ATL_DDR_LO].ಗಾತ್ರ_2) {
        mem_ಗಾತ್ರ = s->memmap[TT_ATL_DDR_LO].ಗಾತ್ರ_2;
    }
    riscv_boot_info_init_discontig_mem(&boot_info, &s->cpus,
                                       s->memmap[TT_ATL_DDR_LO].ತಳ,
                                       mem_ಗಾತ್ರ);

    firmware_ಕೊನೆ_addr = riscv_ಕಂಡುಹಿಡಿ_and_ತುಂಬು_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ಆರಂಭ_addr, ಖಾಲಿ);

    kernel_ಆರಂಭ_addr = riscv_calc_kernel_ಆರಂಭ_addr(&boot_info,
                                                     firmware_ಕೊನೆ_addr);
    ಆದರೆ (machine->kernel_filename) {
        riscv_ತುಂಬು_kernel(machine, &boot_info, kernel_ಆರಂಭ_addr,
                          true, ಖಾಲಿ);
        kernel_ನಮೂದು = boot_info.image_ಕೆಳಗಿನ_addr;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* If we aren't loading a payload, OpenSBI thinks we are trying to boot
         * address 0, which fails `sbi_domain_check_addr()` as that is where
         * OpenSBI is running. Instead point OpenSBI to the end of the region
         * where it was loaded, which avoids the early hang, allowing the
         * system to proceed with the OpenSBI boot output.
         */
        kernel_ನಮೂದು = kernel_ಆರಂಭ_addr;
    }

    fdt_ತುಂಬು_addr = riscv_compute_fdt_addr(s->memmap[TT_ATL_DDR_LO].ತಳ,
                                           s->memmap[TT_ATL_DDR_LO].ಗಾತ್ರ_2,
                                           machine, &boot_info);
    riscv_ತುಂಬು_fdt(fdt_ತುಂಬು_addr, machine->fdt);

    /* load the reset vector */
    riscv_ಸಿದ್ಧತೆ_rom_reset_vec(machine, &s->cpus, ಆರಂಭ_addr,
                              s->memmap[TT_ATL_BOOTROM].ತಳ,
                              s->memmap[TT_ATL_BOOTROM].ಗಾತ್ರ_2,
                              kernel_ನಮೂದು,
                              fdt_ತುಂಬು_addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ tt_atlantis_soc_init(Object *obj)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(obj);

    object_initialize_ಮಗು(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);

    object_initialize_ಮಗು(obj, "uart1", &s->uart1,
                            TYPE_UNIMPLEMENTED_DEVICE);

    object_initialize_ಮಗು(obj, "prcm-rcpu",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU],
                            TYPE_TT_ATLANTIS_PRCM_RCPU);
    object_initialize_ಮಗು(obj, "prcm-hsio",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO],
                            TYPE_TT_ATLANTIS_PRCM_HSIO);
    object_initialize_ಮಗು(obj, "prcm-pcie",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE],
                            TYPE_TT_ATLANTIS_PRCM_PCIE);
    object_initialize_ಮಗು(obj, "prcm-mm",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM],
                            TYPE_TT_ATLANTIS_PRCM_MM);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < TT_ATL_ಅಂಕೆ_I2_C; i++) {
        object_initialize_ಮಗು(obj, "i2c[*]", &s->i2c[i],
                                TYPE_DESIGNWARE_I2C);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ tt_atlantis_soc_realize(DeviceState *dev, Error **errp)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(dev);
    ram_addr_t lo_ram_ಗಾತ್ರ, ram_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ hart_ಎಣಿಕೆ = s->ಅಂಕೆ_harts;

    ಆದರೆ (!s->ಸ್ಮರಣೆ) {
        error_setg(errp, "'memory' link is not set");
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!s->dram) {
        error_setg(errp, "'dram' link is not set");
        ಹಿಂತಿರುಗಿ;
    }
    ram_ಗಾತ್ರ = ಸ್ಮರಣೆ_region_ಗಾತ್ರ(s->dram);

    s->memmap = tt_atlantis_memmap;

    /* CPUs */
    object_property_ಗಣ_str(OBJECT(&s->cpus), "cpu-type", s->cpu_type,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&s->cpus), "hartid-base", 0,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&s->cpus), "num-harts", hart_ಎಣಿಕೆ,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&s->cpus), "resetvec",
                            s->memmap[TT_ATL_BOOTROM].ತಳ,
                            &error_abort);
    object_property_ಗಣ_link(OBJECT(&s->cpus), "memory", OBJECT(s->ಸ್ಮರಣೆ),
                             &error_abort);
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        ಹಿಂತಿರುಗಿ;
    }

    s->irqchip = riscv_create_aia(s->ಸ್ಮರಣೆ,
                                  true, TT_IMSIC_GUESTS,
                                  TT_IMSIC_STRIDE,
                                  TT_IMSIC_STRIDE,
                                  TT_IRQCHIP_ಅಂಕೆ_SOURCES,
                                  &s->memmap[TT_ATL_MAPLIC],
                                  &s->memmap[TT_ATL_SAPLIC],
                                  &s->memmap[TT_ATL_MIMSIC],
                                  &s->memmap[TT_ATL_SIMSIC],
                                  0, 0, hart_ಎಣಿಕೆ,
                                  TT_IRQCHIP_ಅಂಕೆ_MSIS,
                                  TT_IRQCHIP_ಅಂಕೆ_PRIO_ಬಿಟ್ಟುಗಳು);

    riscv_aclint_mtimer_create(s->ಸ್ಮರಣೆ,
            s->memmap[TT_ATL_ACLINT].ತಳ,
            TT_ACLINT_MTIME_ಗಾತ್ರ,
            0, hart_ಎಣಿಕೆ,
            TT_ACLINT_MTIMECMP,
            TT_ACLINT_MTIME,
            TT_ACLINT_TIMEBASE_FREQ, true);

    /*
     * DDR
     *
     * The high address is where RAM lives. It is always present and may be
     * up to 64GB. The low address is an alias of the first 2GB of that RAM.
     */
    ಆದರೆ (ram_ಗಾತ್ರ > s->memmap[TT_ATL_DDR_HI].ಗಾತ್ರ_2) {
        g_autofree ಅಕ್ಷರ *sz = ಗಾತ್ರ_ಗಮ್ಯ_str(s->memmap[TT_ATL_DDR_HI].ಗಾತ್ರ_2);
        error_setg(errp, "RAM size is too large, maximum is %s", sz);
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಮರಣೆ_region_init_alias(&s->ram_hi, OBJECT(s), "ram.high", s->dram,
                             0, ram_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(s->ಸ್ಮರಣೆ,
                                s->memmap[TT_ATL_DDR_HI].ತಳ, &s->ram_hi);

    lo_ram_ಗಾತ್ರ = MIN(ram_ಗಾತ್ರ, s->memmap[TT_ATL_DDR_LO].ಗಾತ್ರ_2);
    ಸ್ಮರಣೆ_region_init_alias(&s->ram_lo, OBJECT(s), "ram.low", s->dram,
                             0, lo_ram_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(s->ಸ್ಮರಣೆ,
                                s->memmap[TT_ATL_DDR_LO].ತಳ, &s->ram_lo);

    /* Boot ROM */
    ಆದರೆ (!ಸ್ಮರಣೆ_region_init_rom(&s->bootrom, OBJECT(s), "tt-atlantis.bootrom",
                                s->memmap[TT_ATL_BOOTROM].ಗಾತ್ರ_2, errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(s->ಸ್ಮರಣೆ, s->memmap[TT_ATL_BOOTROM].ತಳ,
                                &s->bootrom);

    /* UART1, the soc console (UART0 is for the boot microcontroller) */
    serial_mm_init(s->ಸ್ಮರಣೆ, s->memmap[TT_ATL_UART1].ತಳ, 2,
                   qdev_get_gpio_ಒಳ(s->irqchip, TT_ATL_UART1_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* 4 PRCM blocks */
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]),
                        errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(
        s->ಸ್ಮರಣೆ, s->memmap[TT_ATL_PRCM_RCPU].ತಳ,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]), 0));

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]),
                        errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(
        s->ಸ್ಮರಣೆ, s->memmap[TT_ATL_PRCM_HSIO].ತಳ,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]), 0));

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]),
                        errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(
        s->ಸ್ಮರಣೆ, s->memmap[TT_ATL_PRCM_PCIE].ತಳ,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]), 0));

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM]),
                        errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(
        s->ಸ್ಮರಣೆ, s->memmap[TT_ATL_PRCM_MM].ತಳ,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM]), 0));

    /*
     * Atlantis contains a DesignWare uart while the QEMU machine
     * uses the serial_mm model with the base ns16550 register set.
     * Linux's dw driver writes outside of serial_mm's 0x20 sized
     * mapping and faults.
     *
     * Create an unimplemented device region so writes don't fault
     * and reads return zero, which keeps Linux happy.
     */
    mmio_map_unimplemented(s->ಸ್ಮರಣೆ, SYS_BUS_DEVICE(&s->uart1),
                           "tt-atlantis.uart1", s->memmap[TT_ATL_UART1].ತಳ,
                           s->memmap[TT_ATL_UART1].ಗಾತ್ರ_2);

    /* I2C */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < TT_ATL_ಅಂಕೆ_I2_C; i++) {
        SysBusDevice *sbd = SYS_BUS_DEVICE(&s->i2c[i]);

        ಆದರೆ (!sysbus_realize(sbd, errp)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(s->ಸ್ಮರಣೆ,
                                    s->memmap[TT_ATL_I2C0 + i].ತಳ,
                                    sysbus_mmio_get_region(sbd, 0));
        sysbus_connect_irq(sbd, 0,
                           qdev_get_gpio_ಒಳ(s->irqchip, TT_ATL_I2C0_IRQ + i));
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Property tt_atlantis_soc_props[] = {
    DEFINE_PROP_STRING("cpu-type", TTAtlantisSoCState, cpu_type),
    DEFINE_PROP_UINT32("num-harts", TTAtlantisSoCState, ಅಂಕೆ_harts, 8),
    DEFINE_PROP_LINK("memory", TTAtlantisSoCState, ಸ್ಮರಣೆ,
                     TYPE_ಸ್ಮರಣೆ_REGION, ಸ್ಮರಣೆ_Region *),
    DEFINE_PROP_LINK("dram", TTAtlantisSoCState, dram,
                     TYPE_ಸ್ಮರಣೆ_REGION, ಸ್ಮರಣೆ_Region *),
};

ಸ್ಥಿರ ಶೂನ್ಯ tt_atlantis_soc_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = tt_atlantis_soc_realize;
    device_class_ಗಣ_props(dc, tt_atlantis_soc_props);
    /* The SoC can only be instantiated from the machine */
    dc->user_creatable = false;
}

ಸ್ಥಿರ ಶೂನ್ಯ tt_atlantis_machine_init(MachineState *machine)
{
    TTAtlantisState *ams = TT_ATLANTIS_MACHINE(machine);
    TTAtlantisSoCState *s = &ams->soc;

    ಸ್ಮರಣೆ_region_init(&ams->soc_ಸ್ಮರಣೆ, OBJECT(machine),
                       "tt-atlantis.soc-memory", UINT64_MAX);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(get_system_ಸ್ಮರಣೆ(), 0, &ams->soc_ಸ್ಮರಣೆ);

    object_initialize_ಮಗು(OBJECT(machine), "soc", &ams->soc,
                            TYPE_TT_ATLANTIS_SOC);
    object_property_ಗಣ_str(OBJECT(&ams->soc), "cpu-type", machine->cpu_type,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&ams->soc), "num-harts", machine->smp.cpus,
                            &error_abort);

    object_property_ಗಣ_link(OBJECT(&ams->soc), "memory",
                             OBJECT(&ams->soc_ಸ್ಮರಣೆ), &error_abort);
    object_property_ಗಣ_link(OBJECT(&ams->soc), "dram", OBJECT(machine->ram),
                             &error_abort);

    qdev_realize(DEVICE(&ams->soc), ಖಾಲಿ, &error_fatal);

    /* I2C peripherals: qemu specific */
    i2c_slave_create_simple(i2c_get_bus(s, 0), "ds1338", 0x6f);
    i2c_slave_create_simple(i2c_get_bus(s, 4), "tmp105", 0x48);

    /* Load or create device tree */
    ಆದರೆ (machine->dtb) {
        ತುಂಬು_fdt(ams);
    } ಇಲ್ಲದಿದ್ದರೆ {
        create_fdt(ams);
    }

    ams->machine_ಮುಗಿದ.notify = tt_atlantis_machine_ಮುಗಿದ;
    qemu_ಕೂಡಿಸು_machine_init_ಮುಗಿದ_notifier(&ams->machine_ಮುಗಿದ);
}

ಸ್ಥಿರ ಶೂನ್ಯ tt_atlantis_machine_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Tenstorrent Atlantis RISC-V SoC (Experimental)";
    mc->init = tt_atlantis_machine_init;
    mc->max_cpus = 8;
    mc->default_cpus = 8;
    mc->default_ram_ಗಾತ್ರ = 4 * GiB;
    mc->default_cpu_type = TYPE_RISCV_CPU_TT_ASCALON;
    mc->ಖಂಡ_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->default_ram_id = "tt_atlantis.ram";
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo tt_atlantis_types[] = {
    {
        .name       = TYPE_TT_ATLANTIS_SOC,
        .ಪೋಷಕ     = TYPE_DEVICE,
        .instance_ಗಾತ್ರ = ಗಾತ್ರ(TTAtlantisSoCState),
        .instance_init = tt_atlantis_soc_init,
        .class_init = tt_atlantis_soc_class_init,
    }, {
        .name       = MACHINE_TYPE_NAME("tt-atlantis"),
        .ಪೋಷಕ     = TYPE_MACHINE,
        .class_init = tt_atlantis_machine_class_init,
        .instance_ಗಾತ್ರ = ಗಾತ್ರ(TTAtlantisState),
    },
};

DEFINE_TYPES(tt_atlantis_types)
