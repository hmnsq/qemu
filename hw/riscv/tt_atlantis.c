/*
 * Tenstorrent Atlantis RISC-V System on Chip
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright 2025 Tenstorrent, Joel Stanley <joel@jms.id.au>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/cutils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/guest-random.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/units.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/loader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/fdt-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv_hart.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hw/char/serial-mm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/misc/unimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/misc/tt_atlantis_prcm.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/tt_atlantis.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "aia.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_IRQCHIP_ਅੰਕ_MSIS       255
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_IRQCHIP_ਅੰਕ_SOURCES    128
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_IRQCHIP_ਅੰਕ_PRIO_ਬਿੱਟ  3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_IMSIC_GUESTS           5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_IMSIC_STRIDE           0x40000 /* Same stride for M and S */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_IMSIC_GUEST_ਬਿੱਟ       6

/* Stride is fixed by hardware, check it's consistent with guest bits. */
QEMU_BUILD_BUG_ON(TT_IMSIC_STRIDE != (0x1000 << TT_IMSIC_GUEST_ਬਿੱਟ));

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_ACLINT_MTIME_ਮਾਪ    0x8050
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_ACLINT_MTIME         0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_ACLINT_MTIMECMP      0x8000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TT_ACLINT_TIMEBASE_FREQ 1000000000

ਸਥਿਰ ਅਚਲ Mem_Map_ਇੰਦਰਾਜ tt_atlantis_memmap[] = {
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

ਸਥਿਰ I2CBus *i2c_get_bus(TTAtlantisSoCState *s, ਬਿਨਾਨਿਸ਼ਾਨੀ busnr)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(busnr < TT_ATL_ਅੰਕ_I2_C);

    ਵਾਪਸ s->i2c[busnr].bus;
}

ਸਥਿਰ uint32_t fdt_phandle = 1;
ਸਥਿਰ uint32_t ਅਗਲਾ_phandle(ਖਾਲੀ)
{
    ਵਾਪਸ fdt_phandle++;
}

ਸਥਿਰ ਖਾਲੀ create_fdt_ਸਮ੍ਰਿਤੀ(ਖਾਲੀ *fdt, TTAtlantisSoCState *s)
{
    hwaddr ram_ਮਾਪ = ਸਮ੍ਰਿਤੀ_region_ਮਾਪ(s->dram);
    hwaddr ਮਾਪ_lo = ram_ਮਾਪ;
    hwaddr ਮਾਪ_hi = 0;

    ਜੇ (ਮਾਪ_lo > s->memmap[TT_ATL_DDR_LO].ਮਾਪ) {
        ਮਾਪ_lo = s->memmap[TT_ATL_DDR_LO].ਮਾਪ;
        ਮਾਪ_hi = ram_ਮਾਪ - ਮਾਪ_lo;
    }

    riscv_create_fdt_socket_ਸਮ੍ਰਿਤੀ(fdt, s->memmap[TT_ATL_DDR_LO].ਅਧਾਰ,
                                   ਮਾਪ_lo, 0, false);
    ਜੇ (ਮਾਪ_hi) {
        /*
         * The first part of the HI address is aliased at the LO address
         * so do not include that as usable memory. Is there any way
         * (or good reason) to describe that aliasing 2GB with DT?
         */
        riscv_create_fdt_socket_ਸਮ੍ਰਿਤੀ(fdt,
                                       s->memmap[TT_ATL_DDR_HI].ਅਧਾਰ + ਮਾਪ_lo,
                                       ਮਾਪ_hi, 0, false);
    }
}

ਸਥਿਰ ਖਾਲੀ create_fdt_aclint(ਖਾਲੀ *fdt, TTAtlantisSoCState *s,
                              uint32_t *intc_phandles)
{
    g_autofree ਅੱਖਰ *name = ਨਿਰਾ;
    g_autofree uint32_t *aclint_mtimer_cells = ਨਿਰਾ;
    uint32_t aclint_cells_ਮਾਪ;
    hwaddr addr;

    aclint_mtimer_cells = g_new0(uint32_t, s->cpus.ਅੰਕ_harts * 2);

    ਲਈ (ਪੂਰਨ cpu = 0; cpu < s->cpus.ਅੰਕ_harts; cpu++) {
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_M_TIMER);
    }
    aclint_cells_ਮਾਪ = s->cpus.ਅੰਕ_harts * ਆਕਾਰ(uint32_t) * 2;

    addr = s->memmap[TT_ATL_ACLINT].ਅਧਾਰ;

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIX, addr);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr + TT_ACLINT_MTIME,
                                 2, 0x1000,
                                 2, addr + TT_ACLINT_MTIMECMP,
                                 2, 0x1000);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_one_imsic(ਖਾਲੀ *fdt, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *mem, ਪੂਰਨ cpus,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 ਪੂਰਨ irq_ਸਤਰ, uint32_t imsic_guest_ਬਿੱਟ)
{
    g_autofree ਅੱਖਰ *name = ਨਿਰਾ;
    g_autofree uint32_t *imsic_cells = g_new0(uint32_t, cpus * 2);

    ਲਈ (ਪੂਰਨ cpu = 0; cpu < cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(irq_ਸਤਰ);
    }

    name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->ਅਧਾਰ);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,imsics");

    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", ਨਿਰਾ, 0);
    qemu_fdt_setprop(fdt, name, "msi-controller", ਨਿਰਾ, 0);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     imsic_cells, ਆਕਾਰ(uint32_t) * cpus * 2);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ਅਧਾਰ, 2, mem->ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-ids", TT_IRQCHIP_ਅੰਕ_MSIS);

    ਜੇ (imsic_guest_ਬਿੱਟ) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,guest-index-bits",
                              imsic_guest_ਬਿੱਟ);
    }
    qemu_fdt_setprop_cell(fdt, name, "phandle", msi_phandle);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_one_aplic(ਖਾਲੀ *fdt,
                                 ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *mem,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_ਬੱਚਾ_phandle,
                                 ਪੂਰਨ irq_ਸਤਰ, ਪੂਰਨ ਅੰਕ_harts)
{
    g_autofree ਅੱਖਰ *name =
        g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->ਅਧਾਰ);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, ਅੰਕ_harts * 2);

    ਲਈ (ਪੂਰਨ cpu = 0; cpu < ਅੰਕ_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(irq_ਸਤਰ);
    }

    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aplic");
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 2);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", ਨਿਰਾ, 0);

    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aplic_cells, ਅੰਕ_harts * ਆਕਾਰ(uint32_t) * 2);
    qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_phandle);

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ਅਧਾਰ, 2, mem->ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-sources",
                          TT_IRQCHIP_ਅੰਕ_SOURCES);

    ਜੇ (aplic_ਬੱਚਾ_phandle) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,children",
                              aplic_ਬੱਚਾ_phandle);
        qemu_fdt_setprop_cells(fdt, name, "riscv,delegation",
                               aplic_ਬੱਚਾ_phandle, 1, TT_IRQCHIP_ਅੰਕ_SOURCES);
    }

    qemu_fdt_setprop_cell(fdt, name, "phandle", aplic_phandle);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_pmu(ਖਾਲੀ *fdt, TTAtlantisSoCState *s)
{
    ਅੱਖਰ pmu_name[] = "/pmu";
    RISCVCPU *hart = &s->cpus.harts[0];

    qemu_fdt_ਜੋੜੋ_subnode(fdt, pmu_name);
    qemu_fdt_setprop_string(fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_ਗੰਢ(fdt, hart->pmu_avail_ctrs, pmu_name);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_cpu(ਖਾਲੀ *fdt, TTAtlantisSoCState *s,
                           uint32_t aplic_s_phandle,
                           uint32_t imsic_s_phandle)
{
    g_autofree uint32_t *intc_phandles = g_new0(uint32_t, s->cpus.ਅੰਕ_harts);
    ਪੂਰਨ ਅੰਕ_harts = s->cpus.ਅੰਕ_harts;

    riscv_fdt_create_cpu_socket_subnode(fdt, TT_ACLINT_TIMEBASE_FREQ);

    riscv_create_fdt_socket_cpus(fdt, s->cpus.harts, 0, ਅੰਕ_harts,
                                 s->cpus.hartid_ਅਧਾਰ, &fdt_phandle,
                                 intc_phandles, false, false);

    create_fdt_aclint(fdt, s, intc_phandles);

    /* M-level IMSIC node */
    uint32_t msi_m_phandle = ਅਗਲਾ_phandle();
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_MIMSIC], ਅੰਕ_harts,
                         intc_phandles, msi_m_phandle,
                         IRQ_M_EXT, TT_IMSIC_GUEST_ਬਿੱਟ);

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_SIMSIC], ਅੰਕ_harts,
                         intc_phandles, imsic_s_phandle,
                         IRQ_S_EXT, TT_IMSIC_GUEST_ਬਿੱਟ);

    uint32_t aplic_m_phandle = ਅਗਲਾ_phandle();

    /* M-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_MAPLIC],
                         msi_m_phandle, intc_phandles,
                         aplic_m_phandle, aplic_s_phandle,
                         IRQ_M_EXT, ਅੰਕ_harts);

    /* S-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_SAPLIC],
                         imsic_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         IRQ_S_EXT, ਅੰਕ_harts);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_uart(ਖਾਲੀ *fdt, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *mem, ਪੂਰਨ irq,
                            ਪੂਰਨ irqchip_phandle, uint32_t clk_ਮਾਪਾ_phandle,
                            uint32_t clk_phandle)
{
    g_autofree ਅੱਖਰ *name = g_strdup_printf("/soc/serial@%"HWADDR_PRIX,
                                            mem->ਅਧਾਰ);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ਅਧਾਰ, 2, mem->ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, name, "reg-shift", 2);
    qemu_fdt_setprop_cell(fdt, name, "reg-io-width", 4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_ਮਾਪਾ_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 3686400);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);

    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(fdt, "/aliases", "serial0", name);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_rng(ਖਾਲੀ *fdt)
{
    uint8_t rng_ਬੀਜ[32];

    qemu_guest_getrandom_nofail(rng_ਬੀਜ, ਆਕਾਰ(rng_ਬੀਜ));
    qemu_fdt_setprop(fdt, "/chosen", "rng-seed", rng_ਬੀਜ, ਆਕਾਰ(rng_ਬੀਜ));
}

ਸਥਿਰ ਖਾਲੀ create_fdt_clk(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *clock_name,
                           uint32_t freq, uint32_t phandle)
{
    g_autofree ਅੱਖਰ *name = g_strdup_printf("/clocks/%s", clock_name);

    qemu_fdt_ਜੋੜੋ_ਰਸਤਾ(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "fixed-clock");
    qemu_fdt_setprop_string(fdt, name, "clock-output-names", clock_name);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", freq);
    qemu_fdt_setprop_cell(fdt, name, "phandle", phandle);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_i2c(ਖਾਲੀ *fdt, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *mem, uint32_t irq,
                           uint32_t irqchip_phandle,
                           uint32_t clk_ਮਾਪਾ_phandle,
                           uint32_t clk_phandle)
{
    g_autofree ਅੱਖਰ *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX, mem->ਅਧਾਰ);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "snps,designware-i2c");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ਅਧਾਰ, 2, mem->ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_ਮਾਪਾ_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 100000);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_i2c_device(ਖਾਲੀ *fdt, TTAtlantisSoCState *s, ਪੂਰਨ bus,
                                  ਅਚਲ ਅੱਖਰ *compat, ਪੂਰਨ addr)
{
    hwaddr ਅਧਾਰ = s->memmap[TT_ATL_I2C0 + bus].ਅਧਾਰ;
    g_autofree ਅੱਖਰ *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX"/sensor@%x",
                                            ਅਧਾਰ, addr);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compat);
    qemu_fdt_setprop_cell(fdt, name, "reg", addr);
}

ਸਥਿਰ ਅੱਖਰ *create_fdt_prcm(ਖਾਲੀ *fdt, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *mem,
                             ਅਚਲ ਅੱਖਰ *prcm_name)
{
    hwaddr ਅਧਾਰ = mem->ਅਧਾਰ;
    hwaddr ਮਾਪ = mem->ਮਾਪ;
    ਅੱਖਰ *name = g_strdup_printf("/soc/clock-controller@%" PRIx64,
                                 ਅਧਾਰ);
    g_autofree ਅੱਖਰ *compatible =
        g_strdup_printf("tenstorrent,atlantis-prcm-%s", prcm_name);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compatible);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, ਅਧਾਰ, 2, ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#reset-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "phandle", ਅਗਲਾ_phandle());

    ਵਾਪਸ name;
}

ਸਥਿਰ ਖਾਲੀ finalize_fdt(ਖਾਲੀ *fdt, TTAtlantisSoCState *s)
{
    uint32_t aplic_s_phandle = ਅਗਲਾ_phandle();
    uint32_t imsic_s_phandle = ਅਗਲਾ_phandle();
    uint32_t osc_24m_phandle = ਅਗਲਾ_phandle();
    uint32_t prcm_rcpu_phandle;
    g_autofree ਅੱਖਰ *rcpu_name, *hsio_name, *pcie_name, *mm_name;

    create_fdt_cpu(fdt, s, aplic_s_phandle, imsic_s_phandle);

    create_fdt_ਸਮ੍ਰਿਤੀ(fdt, s);

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
                           prcm_rcpu_phandle, TT_ATL_CLK_RCPU_ਜੜ੍ਹ,
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

    ਲਈ (ਪੂਰਨ i = 0; i < TT_ATL_ਅੰਕ_I2_C; i++) {
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

ਸਥਿਰ ਖਾਲੀ create_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    ਪੂਰਨ fdt_ਮਾਪ = 0;

    ms->fdt = riscv_create_board_device_ਰੁੱਖ("Tenstorrent Atlantis RISC-V Machine",
                                             "tenstorrent,atlantis", &fdt_ਮਾਪ);

    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, "/chosen");

    create_fdt_rng(ms->fdt);

    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, "/aliases");

    create_fdt_pmu(ms->fdt, &ams->soc);
}

ਸਥਿਰ ਖਾਲੀ ਲੱਦੋ_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    ਅੱਖਰ **ਗੰਢ_ਰਸਤਾ;
    Error *err = ਨਿਰਾ;
    ਪੂਰਨ fdt_ਮਾਪ = 0;

    ms->fdt = ਲੱਦੋ_device_ਰੁੱਖ(ms->dtb, &fdt_ਮਾਪ);
    ਜੇ (!ms->fdt) {
        error_report("load_device_tree() failed");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    qemu_fdt_ਜੋੜੋ_ਰਸਤਾ(ms->fdt, "/chosen");

    /* Clear memory nodes and update with the specified RAM size */
    ਗੰਢ_ਰਸਤਾ = qemu_fdt_ਗੰਢ_unit_ਰਸਤਾ(ms->fdt, "memory", &err);
    ਜੇ (err) {
        warn_report_err(err);
    } ਨਹੀਂਤਾਂ {
        ਲਈ (ਪੂਰਨ i = 0; ਗੰਢ_ਰਸਤਾ[i]; i++) {
            warn_report("Replacing device tree %s with the requested RAM size",
                        ਗੰਢ_ਰਸਤਾ[i]);
            qemu_fdt_nop_ਗੰਢ(ms->fdt, ਗੰਢ_ਰਸਤਾ[i]);
        }
        g_strfreev(ਗੰਢ_ਰਸਤਾ);
    }

    create_fdt_ਸਮ੍ਰਿਤੀ(ms->fdt, &ams->soc);
}

ਸਥਿਰ ਖਾਲੀ mmio_map_unimplemented(ਸਮ੍ਰਿਤੀ_Region *ਸਮ੍ਰਿਤੀ, SysBusDevice *dev,
                                   ਅਚਲ ਅੱਖਰ *name, hwaddr addr, uint64_t ਮਾਪ)
{
    qdev_prop_ਸੈੱਟ_string(DEVICE(dev), "name", name);
    qdev_prop_ਸੈੱਟ_uint64(DEVICE(dev), "size", ਮਾਪ);
    sysbus_realize(dev, &error_abort);

    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion_overlap(ਸਮ੍ਰਿਤੀ, addr,
                                        sysbus_mmio_get_region(dev, 0), -1000);
}

ਸਥਿਰ ਖਾਲੀ tt_atlantis_machine_ਪੂਰਾ(Notifier *n, ਖਾਲੀ *ਅੰਕੜੇ)
{
    TTAtlantisState *ams = container_ਦਾ(n, TTAtlantisState, machine_ਪੂਰਾ);
    TTAtlantisSoCState *s = &ams->soc;
    MachineState *machine = MACHINE(ams);
    hwaddr ਸ਼ੁਰੂ_addr = s->memmap[TT_ATL_DDR_LO].ਅਧਾਰ;
    hwaddr mem_ਮਾਪ;
    ਨਿਸ਼ਾਨਾ_ulong firmware_ਸਿਰਾ_addr, kernel_ਸ਼ੁਰੂ_addr;
    ਅਚਲ ਅੱਖਰ *firmware_name = riscv_default_firmware_name(&s->cpus);
    uint64_t fdt_ਲੱਦੋ_addr;
    uint64_t kernel_ਇੰਦਰਾਜ;
    RISCVBootInfo boot_info;

    /*
     * A user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    ਜੇ (machine->dtb == ਨਿਰਾ) {
        finalize_fdt(machine->fdt, s);
    }

    mem_ਮਾਪ = machine->ram_ਮਾਪ;
    ਜੇ (mem_ਮਾਪ > s->memmap[TT_ATL_DDR_LO].ਮਾਪ) {
        mem_ਮਾਪ = s->memmap[TT_ATL_DDR_LO].ਮਾਪ;
    }
    riscv_boot_info_init_discontig_mem(&boot_info, &s->cpus,
                                       s->memmap[TT_ATL_DDR_LO].ਅਧਾਰ,
                                       mem_ਮਾਪ);

    firmware_ਸਿਰਾ_addr = riscv_ਲੱਭੋ_and_ਲੱਦੋ_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ਸ਼ੁਰੂ_addr, ਨਿਰਾ);

    kernel_ਸ਼ੁਰੂ_addr = riscv_calc_kernel_ਸ਼ੁਰੂ_addr(&boot_info,
                                                     firmware_ਸਿਰਾ_addr);
    ਜੇ (machine->kernel_filename) {
        riscv_ਲੱਦੋ_kernel(machine, &boot_info, kernel_ਸ਼ੁਰੂ_addr,
                          true, ਨਿਰਾ);
        kernel_ਇੰਦਰਾਜ = boot_info.image_ਨੀਵਾਂ_addr;
    } ਨਹੀਂਤਾਂ {
        /* If we aren't loading a payload, OpenSBI thinks we are trying to boot
         * address 0, which fails `sbi_domain_check_addr()` as that is where
         * OpenSBI is running. Instead point OpenSBI to the end of the region
         * where it was loaded, which avoids the early hang, allowing the
         * system to proceed with the OpenSBI boot output.
         */
        kernel_ਇੰਦਰਾਜ = kernel_ਸ਼ੁਰੂ_addr;
    }

    fdt_ਲੱਦੋ_addr = riscv_compute_fdt_addr(s->memmap[TT_ATL_DDR_LO].ਅਧਾਰ,
                                           s->memmap[TT_ATL_DDR_LO].ਮਾਪ,
                                           machine, &boot_info);
    riscv_ਲੱਦੋ_fdt(fdt_ਲੱਦੋ_addr, machine->fdt);

    /* load the reset vector */
    riscv_ਤਿਆਰੀ_rom_reset_vec(machine, &s->cpus, ਸ਼ੁਰੂ_addr,
                              s->memmap[TT_ATL_BOOTROM].ਅਧਾਰ,
                              s->memmap[TT_ATL_BOOTROM].ਮਾਪ,
                              kernel_ਇੰਦਰਾਜ,
                              fdt_ਲੱਦੋ_addr);
}

ਸਥਿਰ ਖਾਲੀ tt_atlantis_soc_init(Object *obj)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(obj);

    object_initialize_ਬੱਚਾ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);

    object_initialize_ਬੱਚਾ(obj, "uart1", &s->uart1,
                            TYPE_UNIMPLEMENTED_DEVICE);

    object_initialize_ਬੱਚਾ(obj, "prcm-rcpu",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU],
                            TYPE_TT_ATLANTIS_PRCM_RCPU);
    object_initialize_ਬੱਚਾ(obj, "prcm-hsio",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO],
                            TYPE_TT_ATLANTIS_PRCM_HSIO);
    object_initialize_ਬੱਚਾ(obj, "prcm-pcie",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE],
                            TYPE_TT_ATLANTIS_PRCM_PCIE);
    object_initialize_ਬੱਚਾ(obj, "prcm-mm",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM],
                            TYPE_TT_ATLANTIS_PRCM_MM);

    ਲਈ (ਪੂਰਨ i = 0; i < TT_ATL_ਅੰਕ_I2_C; i++) {
        object_initialize_ਬੱਚਾ(obj, "i2c[*]", &s->i2c[i],
                                TYPE_DESIGNWARE_I2C);
    }
}

ਸਥਿਰ ਖਾਲੀ tt_atlantis_soc_realize(DeviceState *dev, Error **errp)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(dev);
    ram_addr_t lo_ram_ਮਾਪ, ram_ਮਾਪ;
    ਪੂਰਨ hart_ਗਿਣਤੀ = s->ਅੰਕ_harts;

    ਜੇ (!s->ਸਮ੍ਰਿਤੀ) {
        error_setg(errp, "'memory' link is not set");
        ਵਾਪਸ;
    }
    ਜੇ (!s->dram) {
        error_setg(errp, "'dram' link is not set");
        ਵਾਪਸ;
    }
    ram_ਮਾਪ = ਸਮ੍ਰਿਤੀ_region_ਮਾਪ(s->dram);

    s->memmap = tt_atlantis_memmap;

    /* CPUs */
    object_property_ਸੈੱਟ_str(OBJECT(&s->cpus), "cpu-type", s->cpu_type,
                            &error_abort);
    object_property_ਸੈੱਟ_int(OBJECT(&s->cpus), "hartid-base", 0,
                            &error_abort);
    object_property_ਸੈੱਟ_int(OBJECT(&s->cpus), "num-harts", hart_ਗਿਣਤੀ,
                            &error_abort);
    object_property_ਸੈੱਟ_int(OBJECT(&s->cpus), "resetvec",
                            s->memmap[TT_ATL_BOOTROM].ਅਧਾਰ,
                            &error_abort);
    object_property_ਸੈੱਟ_link(OBJECT(&s->cpus), "memory", OBJECT(s->ਸਮ੍ਰਿਤੀ),
                             &error_abort);
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        ਵਾਪਸ;
    }

    s->irqchip = riscv_create_aia(s->ਸਮ੍ਰਿਤੀ,
                                  true, TT_IMSIC_GUESTS,
                                  TT_IMSIC_STRIDE,
                                  TT_IMSIC_STRIDE,
                                  TT_IRQCHIP_ਅੰਕ_SOURCES,
                                  &s->memmap[TT_ATL_MAPLIC],
                                  &s->memmap[TT_ATL_SAPLIC],
                                  &s->memmap[TT_ATL_MIMSIC],
                                  &s->memmap[TT_ATL_SIMSIC],
                                  0, 0, hart_ਗਿਣਤੀ,
                                  TT_IRQCHIP_ਅੰਕ_MSIS,
                                  TT_IRQCHIP_ਅੰਕ_PRIO_ਬਿੱਟ);

    riscv_aclint_mtimer_create(s->ਸਮ੍ਰਿਤੀ,
            s->memmap[TT_ATL_ACLINT].ਅਧਾਰ,
            TT_ACLINT_MTIME_ਮਾਪ,
            0, hart_ਗਿਣਤੀ,
            TT_ACLINT_MTIMECMP,
            TT_ACLINT_MTIME,
            TT_ACLINT_TIMEBASE_FREQ, true);

    /*
     * DDR
     *
     * The high address is where RAM lives. It is always present and may be
     * up to 64GB. The low address is an alias of the first 2GB of that RAM.
     */
    ਜੇ (ram_ਮਾਪ > s->memmap[TT_ATL_DDR_HI].ਮਾਪ) {
        g_autofree ਅੱਖਰ *sz = ਮਾਪ_ਮੰਜ਼ਿਲ_str(s->memmap[TT_ATL_DDR_HI].ਮਾਪ);
        error_setg(errp, "RAM size is too large, maximum is %s", sz);
        ਵਾਪਸ;
    }

    ਸਮ੍ਰਿਤੀ_region_init_alias(&s->ram_hi, OBJECT(s), "ram.high", s->dram,
                             0, ram_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(s->ਸਮ੍ਰਿਤੀ,
                                s->memmap[TT_ATL_DDR_HI].ਅਧਾਰ, &s->ram_hi);

    lo_ram_ਮਾਪ = MIN(ram_ਮਾਪ, s->memmap[TT_ATL_DDR_LO].ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_init_alias(&s->ram_lo, OBJECT(s), "ram.low", s->dram,
                             0, lo_ram_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(s->ਸਮ੍ਰਿਤੀ,
                                s->memmap[TT_ATL_DDR_LO].ਅਧਾਰ, &s->ram_lo);

    /* Boot ROM */
    ਜੇ (!ਸਮ੍ਰਿਤੀ_region_init_rom(&s->bootrom, OBJECT(s), "tt-atlantis.bootrom",
                                s->memmap[TT_ATL_BOOTROM].ਮਾਪ, errp)) {
        ਵਾਪਸ;
    }
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(s->ਸਮ੍ਰਿਤੀ, s->memmap[TT_ATL_BOOTROM].ਅਧਾਰ,
                                &s->bootrom);

    /* UART1, the soc console (UART0 is for the boot microcontroller) */
    serial_mm_init(s->ਸਮ੍ਰਿਤੀ, s->memmap[TT_ATL_UART1].ਅਧਾਰ, 2,
                   qdev_get_gpio_ਵਿੱਚ(s->irqchip, TT_ATL_UART1_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* 4 PRCM blocks */
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]),
                        errp)) {
        ਵਾਪਸ;
    }
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(
        s->ਸਮ੍ਰਿਤੀ, s->memmap[TT_ATL_PRCM_RCPU].ਅਧਾਰ,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]), 0));

    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]),
                        errp)) {
        ਵਾਪਸ;
    }
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(
        s->ਸਮ੍ਰਿਤੀ, s->memmap[TT_ATL_PRCM_HSIO].ਅਧਾਰ,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]), 0));

    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]),
                        errp)) {
        ਵਾਪਸ;
    }
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(
        s->ਸਮ੍ਰਿਤੀ, s->memmap[TT_ATL_PRCM_PCIE].ਅਧਾਰ,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]), 0));

    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM]),
                        errp)) {
        ਵਾਪਸ;
    }
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(
        s->ਸਮ੍ਰਿਤੀ, s->memmap[TT_ATL_PRCM_MM].ਅਧਾਰ,
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
    mmio_map_unimplemented(s->ਸਮ੍ਰਿਤੀ, SYS_BUS_DEVICE(&s->uart1),
                           "tt-atlantis.uart1", s->memmap[TT_ATL_UART1].ਅਧਾਰ,
                           s->memmap[TT_ATL_UART1].ਮਾਪ);

    /* I2C */
    ਲਈ (ਪੂਰਨ i = 0; i < TT_ATL_ਅੰਕ_I2_C; i++) {
        SysBusDevice *sbd = SYS_BUS_DEVICE(&s->i2c[i]);

        ਜੇ (!sysbus_realize(sbd, errp)) {
            ਵਾਪਸ;
        }
        ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(s->ਸਮ੍ਰਿਤੀ,
                                    s->memmap[TT_ATL_I2C0 + i].ਅਧਾਰ,
                                    sysbus_mmio_get_region(sbd, 0));
        sysbus_connect_irq(sbd, 0,
                           qdev_get_gpio_ਵਿੱਚ(s->irqchip, TT_ATL_I2C0_IRQ + i));
    }
}

ਸਥਿਰ ਅਚਲ Property tt_atlantis_soc_props[] = {
    DEFINE_PROP_STRING("cpu-type", TTAtlantisSoCState, cpu_type),
    DEFINE_PROP_UINT32("num-harts", TTAtlantisSoCState, ਅੰਕ_harts, 8),
    DEFINE_PROP_LINK("memory", TTAtlantisSoCState, ਸਮ੍ਰਿਤੀ,
                     TYPE_ਸਮ੍ਰਿਤੀ_REGION, ਸਮ੍ਰਿਤੀ_Region *),
    DEFINE_PROP_LINK("dram", TTAtlantisSoCState, dram,
                     TYPE_ਸਮ੍ਰਿਤੀ_REGION, ਸਮ੍ਰਿਤੀ_Region *),
};

ਸਥਿਰ ਖਾਲੀ tt_atlantis_soc_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = tt_atlantis_soc_realize;
    device_class_ਸੈੱਟ_props(dc, tt_atlantis_soc_props);
    /* The SoC can only be instantiated from the machine */
    dc->user_creatable = false;
}

ਸਥਿਰ ਖਾਲੀ tt_atlantis_machine_init(MachineState *machine)
{
    TTAtlantisState *ams = TT_ATLANTIS_MACHINE(machine);
    TTAtlantisSoCState *s = &ams->soc;

    ਸਮ੍ਰਿਤੀ_region_init(&ams->soc_ਸਮ੍ਰਿਤੀ, OBJECT(machine),
                       "tt-atlantis.soc-memory", UINT64_MAX);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(get_system_ਸਮ੍ਰਿਤੀ(), 0, &ams->soc_ਸਮ੍ਰਿਤੀ);

    object_initialize_ਬੱਚਾ(OBJECT(machine), "soc", &ams->soc,
                            TYPE_TT_ATLANTIS_SOC);
    object_property_ਸੈੱਟ_str(OBJECT(&ams->soc), "cpu-type", machine->cpu_type,
                            &error_abort);
    object_property_ਸੈੱਟ_int(OBJECT(&ams->soc), "num-harts", machine->smp.cpus,
                            &error_abort);

    object_property_ਸੈੱਟ_link(OBJECT(&ams->soc), "memory",
                             OBJECT(&ams->soc_ਸਮ੍ਰਿਤੀ), &error_abort);
    object_property_ਸੈੱਟ_link(OBJECT(&ams->soc), "dram", OBJECT(machine->ram),
                             &error_abort);

    qdev_realize(DEVICE(&ams->soc), ਨਿਰਾ, &error_fatal);

    /* I2C peripherals: qemu specific */
    i2c_slave_create_simple(i2c_get_bus(s, 0), "ds1338", 0x6f);
    i2c_slave_create_simple(i2c_get_bus(s, 4), "tmp105", 0x48);

    /* Load or create device tree */
    ਜੇ (machine->dtb) {
        ਲੱਦੋ_fdt(ams);
    } ਨਹੀਂਤਾਂ {
        create_fdt(ams);
    }

    ams->machine_ਪੂਰਾ.notify = tt_atlantis_machine_ਪੂਰਾ;
    qemu_ਜੋੜੋ_machine_init_ਪੂਰਾ_notifier(&ams->machine_ਪੂਰਾ);
}

ਸਥਿਰ ਖਾਲੀ tt_atlantis_machine_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Tenstorrent Atlantis RISC-V SoC (Experimental)";
    mc->init = tt_atlantis_machine_init;
    mc->max_cpus = 8;
    mc->default_cpus = 8;
    mc->default_ram_ਮਾਪ = 4 * GiB;
    mc->default_cpu_type = TYPE_RISCV_CPU_TT_ASCALON;
    mc->ਖੰਡ_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->default_ram_id = "tt_atlantis.ram";
}

ਸਥਿਰ ਅਚਲ TypeInfo tt_atlantis_types[] = {
    {
        .name       = TYPE_TT_ATLANTIS_SOC,
        .ਮਾਪਾ     = TYPE_DEVICE,
        .instance_ਮਾਪ = ਆਕਾਰ(TTAtlantisSoCState),
        .instance_init = tt_atlantis_soc_init,
        .class_init = tt_atlantis_soc_class_init,
    }, {
        .name       = MACHINE_TYPE_NAME("tt-atlantis"),
        .ਮਾਪਾ     = TYPE_MACHINE,
        .class_init = tt_atlantis_machine_class_init,
        .instance_ਮਾਪ = ਆਕਾਰ(TTAtlantisState),
    },
};

DEFINE_TYPES(tt_atlantis_types)
