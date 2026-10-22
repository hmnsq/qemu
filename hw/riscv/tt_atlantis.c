/*
 * Tenstorrent Atlantis RISC-V System on Chip
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright 2025 Tenstorrent, Joel Stanley <joel@jms.id.au>
 */

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qemu/cutils.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qemu/guest-random.h"
#అన్తర్భావ "qemu/units.h"

#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/core/sysbus.h"

#అన్తర్భావ "target/riscv/cpu.h"

#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/riscv/fdt-common.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"

#అన్తర్భావ "hw/char/serial-mm.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/misc/unimp.h"
#అన్తర్భావ "hw/misc/tt_atlantis_prcm.h"

#అన్తర్భావ "system/system.h"
#అన్తర్భావ "system/device_tree.h"

#అన్తర్భావ "hw/riscv/tt_atlantis.h"

#అన్తర్భావ "aia.h"

#నిర్వచించు TT_IRQCHIP_అంకె_MSIS       255
#నిర్వచించు TT_IRQCHIP_అంకె_SOURCES    128
#నిర్వచించు TT_IRQCHIP_అంకె_PRIO_బిట్లు  3
#నిర్వచించు TT_IMSIC_GUESTS           5
#నిర్వచించు TT_IMSIC_STRIDE           0x40000 /* Same stride for M and S */
#నిర్వచించు TT_IMSIC_GUEST_బిట్లు       6

/* Stride is fixed by hardware, check it's consistent with guest bits. */
QEMU_BUILD_BUG_ON(TT_IMSIC_STRIDE != (0x1000 << TT_IMSIC_GUEST_బిట్లు));

#నిర్వచించు TT_ACLINT_MTIME_కొలత    0x8050
#నిర్వచించు TT_ACLINT_MTIME         0x0
#నిర్వచించు TT_ACLINT_MTIMECMP      0x8000
#నిర్వచించు TT_ACLINT_TIMEBASE_FREQ 1000000000

స్థిర స్థిరరాశి Mem_Map_నమోదు tt_atlantis_memmap[] = {
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

స్థిర I2CBus *i2c_get_bus(TTAtlantisSoCState *s, గుర్తులేని busnr)
{
    ప్రతిజ్ఞా(busnr < TT_ATL_అంకె_I2_C);

    తిరిగి s->i2c[busnr].bus;
}

స్థిర uint32_t fdt_phandle = 1;
స్థిర uint32_t తదుపరి_phandle(శూన్య)
{
    తిరిగి fdt_phandle++;
}

స్థిర శూన్య create_fdt_జ్ఞాపకం(శూన్య *fdt, TTAtlantisSoCState *s)
{
    hwaddr ram_కొలత = జ్ఞాపకం_region_కొలత(s->dram);
    hwaddr కొలత_lo = ram_కొలత;
    hwaddr కొలత_hi = 0;

    ఒకవేళ (కొలత_lo > s->memmap[TT_ATL_DDR_LO].కొలత) {
        కొలత_lo = s->memmap[TT_ATL_DDR_LO].కొలత;
        కొలత_hi = ram_కొలత - కొలత_lo;
    }

    riscv_create_fdt_socket_జ్ఞాపకం(fdt, s->memmap[TT_ATL_DDR_LO].పునాది,
                                   కొలత_lo, 0, false);
    ఒకవేళ (కొలత_hi) {
        /*
         * The first part of the HI address is aliased at the LO address
         * so do not include that as usable memory. Is there any way
         * (or good reason) to describe that aliasing 2GB with DT?
         */
        riscv_create_fdt_socket_జ్ఞాపకం(fdt,
                                       s->memmap[TT_ATL_DDR_HI].పునాది + కొలత_lo,
                                       కొలత_hi, 0, false);
    }
}

స్థిర శూన్య create_fdt_aclint(శూన్య *fdt, TTAtlantisSoCState *s,
                              uint32_t *intc_phandles)
{
    g_autofree అక్షరం *name = ఖాళీ;
    g_autofree uint32_t *aclint_mtimer_cells = ఖాళీ;
    uint32_t aclint_cells_కొలత;
    hwaddr addr;

    aclint_mtimer_cells = g_new0(uint32_t, s->cpus.అంకె_harts * 2);

    కొరకు (పూర్ణాంకం cpu = 0; cpu < s->cpus.అంకె_harts; cpu++) {
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(IRQ_M_TIMER);
    }
    aclint_cells_కొలత = s->cpus.అంకె_harts * పరిమాణం(uint32_t) * 2;

    addr = s->memmap[TT_ATL_ACLINT].పునాది;

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIX, addr);
    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr + TT_ACLINT_MTIME,
                                 2, 0x1000,
                                 2, addr + TT_ACLINT_MTIMECMP,
                                 2, 0x1000);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_కొలత);
}

స్థిర శూన్య create_fdt_one_imsic(శూన్య *fdt, స్థిరరాశి Mem_Map_నమోదు *mem, పూర్ణాంకం cpus,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 పూర్ణాంకం irq_పంక్తి, uint32_t imsic_guest_బిట్లు)
{
    g_autofree అక్షరం *name = ఖాళీ;
    g_autofree uint32_t *imsic_cells = g_new0(uint32_t, cpus * 2);

    కొరకు (పూర్ణాంకం cpu = 0; cpu < cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(irq_పంక్తి);
    }

    name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->పునాది);
    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,imsics");

    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", ఖాళీ, 0);
    qemu_fdt_setprop(fdt, name, "msi-controller", ఖాళీ, 0);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     imsic_cells, పరిమాణం(uint32_t) * cpus * 2);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->పునాది, 2, mem->కొలత);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-ids", TT_IRQCHIP_అంకె_MSIS);

    ఒకవేళ (imsic_guest_బిట్లు) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,guest-index-bits",
                              imsic_guest_బిట్లు);
    }
    qemu_fdt_setprop_cell(fdt, name, "phandle", msi_phandle);
}

స్థిర శూన్య create_fdt_one_aplic(శూన్య *fdt,
                                 స్థిరరాశి Mem_Map_నమోదు *mem,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_బిడ్డ_phandle,
                                 పూర్ణాంకం irq_పంక్తి, పూర్ణాంకం అంకె_harts)
{
    g_autofree అక్షరం *name =
        g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->పునాది);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, అంకె_harts * 2);

    కొరకు (పూర్ణాంకం cpu = 0; cpu < అంకె_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(irq_పంక్తి);
    }

    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aplic");
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 2);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", ఖాళీ, 0);

    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aplic_cells, అంకె_harts * పరిమాణం(uint32_t) * 2);
    qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_phandle);

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->పునాది, 2, mem->కొలత);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-sources",
                          TT_IRQCHIP_అంకె_SOURCES);

    ఒకవేళ (aplic_బిడ్డ_phandle) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,children",
                              aplic_బిడ్డ_phandle);
        qemu_fdt_setprop_cells(fdt, name, "riscv,delegation",
                               aplic_బిడ్డ_phandle, 1, TT_IRQCHIP_అంకె_SOURCES);
    }

    qemu_fdt_setprop_cell(fdt, name, "phandle", aplic_phandle);
}

స్థిర శూన్య create_fdt_pmu(శూన్య *fdt, TTAtlantisSoCState *s)
{
    అక్షరం pmu_name[] = "/pmu";
    RISCVCPU *hart = &s->cpus.harts[0];

    qemu_fdt_కూడు_subnode(fdt, pmu_name);
    qemu_fdt_setprop_string(fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_కణుపు(fdt, hart->pmu_avail_ctrs, pmu_name);
}

స్థిర శూన్య create_fdt_cpu(శూన్య *fdt, TTAtlantisSoCState *s,
                           uint32_t aplic_s_phandle,
                           uint32_t imsic_s_phandle)
{
    g_autofree uint32_t *intc_phandles = g_new0(uint32_t, s->cpus.అంకె_harts);
    పూర్ణాంకం అంకె_harts = s->cpus.అంకె_harts;

    riscv_fdt_create_cpu_socket_subnode(fdt, TT_ACLINT_TIMEBASE_FREQ);

    riscv_create_fdt_socket_cpus(fdt, s->cpus.harts, 0, అంకె_harts,
                                 s->cpus.hartid_పునాది, &fdt_phandle,
                                 intc_phandles, false, false);

    create_fdt_aclint(fdt, s, intc_phandles);

    /* M-level IMSIC node */
    uint32_t msi_m_phandle = తదుపరి_phandle();
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_MIMSIC], అంకె_harts,
                         intc_phandles, msi_m_phandle,
                         IRQ_M_EXT, TT_IMSIC_GUEST_బిట్లు);

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_SIMSIC], అంకె_harts,
                         intc_phandles, imsic_s_phandle,
                         IRQ_S_EXT, TT_IMSIC_GUEST_బిట్లు);

    uint32_t aplic_m_phandle = తదుపరి_phandle();

    /* M-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_MAPLIC],
                         msi_m_phandle, intc_phandles,
                         aplic_m_phandle, aplic_s_phandle,
                         IRQ_M_EXT, అంకె_harts);

    /* S-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_SAPLIC],
                         imsic_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         IRQ_S_EXT, అంకె_harts);
}

స్థిర శూన్య create_fdt_uart(శూన్య *fdt, స్థిరరాశి Mem_Map_నమోదు *mem, పూర్ణాంకం irq,
                            పూర్ణాంకం irqchip_phandle, uint32_t clk_మాతృ_phandle,
                            uint32_t clk_phandle)
{
    g_autofree అక్షరం *name = g_strdup_printf("/soc/serial@%"HWADDR_PRIX,
                                            mem->పునాది);

    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->పునాది, 2, mem->కొలత);
    qemu_fdt_setprop_cell(fdt, name, "reg-shift", 2);
    qemu_fdt_setprop_cell(fdt, name, "reg-io-width", 4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_మాతృ_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 3686400);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);

    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(fdt, "/aliases", "serial0", name);
}

స్థిర శూన్య create_fdt_rng(శూన్య *fdt)
{
    uint8_t rng_విత్తనం[32];

    qemu_guest_getrandom_nofail(rng_విత్తనం, పరిమాణం(rng_విత్తనం));
    qemu_fdt_setprop(fdt, "/chosen", "rng-seed", rng_విత్తనం, పరిమాణం(rng_విత్తనం));
}

స్థిర శూన్య create_fdt_clk(శూన్య *fdt, స్థిరరాశి అక్షరం *clock_name,
                           uint32_t freq, uint32_t phandle)
{
    g_autofree అక్షరం *name = g_strdup_printf("/clocks/%s", clock_name);

    qemu_fdt_కూడు_మార్గం(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "fixed-clock");
    qemu_fdt_setprop_string(fdt, name, "clock-output-names", clock_name);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", freq);
    qemu_fdt_setprop_cell(fdt, name, "phandle", phandle);
}

స్థిర శూన్య create_fdt_i2c(శూన్య *fdt, స్థిరరాశి Mem_Map_నమోదు *mem, uint32_t irq,
                           uint32_t irqchip_phandle,
                           uint32_t clk_మాతృ_phandle,
                           uint32_t clk_phandle)
{
    g_autofree అక్షరం *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX, mem->పునాది);

    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "snps,designware-i2c");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->పునాది, 2, mem->కొలత);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_మాతృ_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 100000);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0);
}

స్థిర శూన్య create_fdt_i2c_device(శూన్య *fdt, TTAtlantisSoCState *s, పూర్ణాంకం bus,
                                  స్థిరరాశి అక్షరం *compat, పూర్ణాంకం addr)
{
    hwaddr పునాది = s->memmap[TT_ATL_I2C0 + bus].పునాది;
    g_autofree అక్షరం *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX"/sensor@%x",
                                            పునాది, addr);

    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compat);
    qemu_fdt_setprop_cell(fdt, name, "reg", addr);
}

స్థిర అక్షరం *create_fdt_prcm(శూన్య *fdt, స్థిరరాశి Mem_Map_నమోదు *mem,
                             స్థిరరాశి అక్షరం *prcm_name)
{
    hwaddr పునాది = mem->పునాది;
    hwaddr కొలత = mem->కొలత;
    అక్షరం *name = g_strdup_printf("/soc/clock-controller@%" PRIx64,
                                 పునాది);
    g_autofree అక్షరం *compatible =
        g_strdup_printf("tenstorrent,atlantis-prcm-%s", prcm_name);

    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compatible);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, పునాది, 2, కొలత);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#reset-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "phandle", తదుపరి_phandle());

    తిరిగి name;
}

స్థిర శూన్య finalize_fdt(శూన్య *fdt, TTAtlantisSoCState *s)
{
    uint32_t aplic_s_phandle = తదుపరి_phandle();
    uint32_t imsic_s_phandle = తదుపరి_phandle();
    uint32_t osc_24m_phandle = తదుపరి_phandle();
    uint32_t prcm_rcpu_phandle;
    g_autofree అక్షరం *rcpu_name, *hsio_name, *pcie_name, *mm_name;

    create_fdt_cpu(fdt, s, aplic_s_phandle, imsic_s_phandle);

    create_fdt_జ్ఞాపకం(fdt, s);

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
                           prcm_rcpu_phandle, TT_ATL_CLK_RCPU_మూలం,
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

    కొరకు (పూర్ణాంకం i = 0; i < TT_ATL_అంకె_I2_C; i++) {
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

స్థిర శూన్య create_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    పూర్ణాంకం fdt_కొలత = 0;

    ms->fdt = riscv_create_board_device_వృక్షం("Tenstorrent Atlantis RISC-V Machine",
                                             "tenstorrent,atlantis", &fdt_కొలత);

    qemu_fdt_కూడు_subnode(ms->fdt, "/chosen");

    create_fdt_rng(ms->fdt);

    qemu_fdt_కూడు_subnode(ms->fdt, "/aliases");

    create_fdt_pmu(ms->fdt, &ams->soc);
}

స్థిర శూన్య ఎక్కించు_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    అక్షరం **కణుపు_మార్గం;
    Error *err = ఖాళీ;
    పూర్ణాంకం fdt_కొలత = 0;

    ms->fdt = ఎక్కించు_device_వృక్షం(ms->dtb, &fdt_కొలత);
    ఒకవేళ (!ms->fdt) {
        error_report("load_device_tree() failed");
        నిష్క్రమించు(1);
    }

    qemu_fdt_కూడు_మార్గం(ms->fdt, "/chosen");

    /* Clear memory nodes and update with the specified RAM size */
    కణుపు_మార్గం = qemu_fdt_కణుపు_unit_మార్గం(ms->fdt, "memory", &err);
    ఒకవేళ (err) {
        warn_report_err(err);
    } లేకపోతే {
        కొరకు (పూర్ణాంకం i = 0; కణుపు_మార్గం[i]; i++) {
            warn_report("Replacing device tree %s with the requested RAM size",
                        కణుపు_మార్గం[i]);
            qemu_fdt_nop_కణుపు(ms->fdt, కణుపు_మార్గం[i]);
        }
        g_strfreev(కణుపు_మార్గం);
    }

    create_fdt_జ్ఞాపకం(ms->fdt, &ams->soc);
}

స్థిర శూన్య mmio_map_unimplemented(జ్ఞాపకం_Region *జ్ఞాపకం, SysBusDevice *dev,
                                   స్థిరరాశి అక్షరం *name, hwaddr addr, uint64_t కొలత)
{
    qdev_prop_సమితి_string(DEVICE(dev), "name", name);
    qdev_prop_సమితి_uint64(DEVICE(dev), "size", కొలత);
    sysbus_realize(dev, &error_abort);

    జ్ఞాపకం_region_కూడు_subregion_overlap(జ్ఞాపకం, addr,
                                        sysbus_mmio_get_region(dev, 0), -1000);
}

స్థిర శూన్య tt_atlantis_machine_పూర్తి(Notifier *n, శూన్య *దత్తాంశం)
{
    TTAtlantisState *ams = container_యొక్క(n, TTAtlantisState, machine_పూర్తి);
    TTAtlantisSoCState *s = &ams->soc;
    MachineState *machine = MACHINE(ams);
    hwaddr ప్రారంభం_addr = s->memmap[TT_ATL_DDR_LO].పునాది;
    hwaddr mem_కొలత;
    లక్ష్యం_ulong firmware_చివర_addr, kernel_ప్రారంభం_addr;
    స్థిరరాశి అక్షరం *firmware_name = riscv_default_firmware_name(&s->cpus);
    uint64_t fdt_ఎక్కించు_addr;
    uint64_t kernel_నమోదు;
    RISCVBootInfo boot_info;

    /*
     * A user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    ఒకవేళ (machine->dtb == ఖాళీ) {
        finalize_fdt(machine->fdt, s);
    }

    mem_కొలత = machine->ram_కొలత;
    ఒకవేళ (mem_కొలత > s->memmap[TT_ATL_DDR_LO].కొలత) {
        mem_కొలత = s->memmap[TT_ATL_DDR_LO].కొలత;
    }
    riscv_boot_info_init_discontig_mem(&boot_info, &s->cpus,
                                       s->memmap[TT_ATL_DDR_LO].పునాది,
                                       mem_కొలత);

    firmware_చివర_addr = riscv_కనుగొను_and_ఎక్కించు_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ప్రారంభం_addr, ఖాళీ);

    kernel_ప్రారంభం_addr = riscv_calc_kernel_ప్రారంభం_addr(&boot_info,
                                                     firmware_చివర_addr);
    ఒకవేళ (machine->kernel_filename) {
        riscv_ఎక్కించు_kernel(machine, &boot_info, kernel_ప్రారంభం_addr,
                          true, ఖాళీ);
        kernel_నమోదు = boot_info.image_దిగువ_addr;
    } లేకపోతే {
        /* If we aren't loading a payload, OpenSBI thinks we are trying to boot
         * address 0, which fails `sbi_domain_check_addr()` as that is where
         * OpenSBI is running. Instead point OpenSBI to the end of the region
         * where it was loaded, which avoids the early hang, allowing the
         * system to proceed with the OpenSBI boot output.
         */
        kernel_నమోదు = kernel_ప్రారంభం_addr;
    }

    fdt_ఎక్కించు_addr = riscv_compute_fdt_addr(s->memmap[TT_ATL_DDR_LO].పునాది,
                                           s->memmap[TT_ATL_DDR_LO].కొలత,
                                           machine, &boot_info);
    riscv_ఎక్కించు_fdt(fdt_ఎక్కించు_addr, machine->fdt);

    /* load the reset vector */
    riscv_అమరిక_rom_reset_vec(machine, &s->cpus, ప్రారంభం_addr,
                              s->memmap[TT_ATL_BOOTROM].పునాది,
                              s->memmap[TT_ATL_BOOTROM].కొలత,
                              kernel_నమోదు,
                              fdt_ఎక్కించు_addr);
}

స్థిర శూన్య tt_atlantis_soc_init(Object *obj)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(obj);

    object_initialize_బిడ్డ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);

    object_initialize_బిడ్డ(obj, "uart1", &s->uart1,
                            TYPE_UNIMPLEMENTED_DEVICE);

    object_initialize_బిడ్డ(obj, "prcm-rcpu",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU],
                            TYPE_TT_ATLANTIS_PRCM_RCPU);
    object_initialize_బిడ్డ(obj, "prcm-hsio",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO],
                            TYPE_TT_ATLANTIS_PRCM_HSIO);
    object_initialize_బిడ్డ(obj, "prcm-pcie",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE],
                            TYPE_TT_ATLANTIS_PRCM_PCIE);
    object_initialize_బిడ్డ(obj, "prcm-mm",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM],
                            TYPE_TT_ATLANTIS_PRCM_MM);

    కొరకు (పూర్ణాంకం i = 0; i < TT_ATL_అంకె_I2_C; i++) {
        object_initialize_బిడ్డ(obj, "i2c[*]", &s->i2c[i],
                                TYPE_DESIGNWARE_I2C);
    }
}

స్థిర శూన్య tt_atlantis_soc_realize(DeviceState *dev, Error **errp)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(dev);
    ram_addr_t lo_ram_కొలత, ram_కొలత;
    పూర్ణాంకం hart_లెక్క = s->అంకె_harts;

    ఒకవేళ (!s->జ్ఞాపకం) {
        error_setg(errp, "'memory' link is not set");
        తిరిగి;
    }
    ఒకవేళ (!s->dram) {
        error_setg(errp, "'dram' link is not set");
        తిరిగి;
    }
    ram_కొలత = జ్ఞాపకం_region_కొలత(s->dram);

    s->memmap = tt_atlantis_memmap;

    /* CPUs */
    object_property_సమితి_str(OBJECT(&s->cpus), "cpu-type", s->cpu_type,
                            &error_abort);
    object_property_సమితి_int(OBJECT(&s->cpus), "hartid-base", 0,
                            &error_abort);
    object_property_సమితి_int(OBJECT(&s->cpus), "num-harts", hart_లెక్క,
                            &error_abort);
    object_property_సమితి_int(OBJECT(&s->cpus), "resetvec",
                            s->memmap[TT_ATL_BOOTROM].పునాది,
                            &error_abort);
    object_property_సమితి_link(OBJECT(&s->cpus), "memory", OBJECT(s->జ్ఞాపకం),
                             &error_abort);
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        తిరిగి;
    }

    s->irqchip = riscv_create_aia(s->జ్ఞాపకం,
                                  true, TT_IMSIC_GUESTS,
                                  TT_IMSIC_STRIDE,
                                  TT_IMSIC_STRIDE,
                                  TT_IRQCHIP_అంకె_SOURCES,
                                  &s->memmap[TT_ATL_MAPLIC],
                                  &s->memmap[TT_ATL_SAPLIC],
                                  &s->memmap[TT_ATL_MIMSIC],
                                  &s->memmap[TT_ATL_SIMSIC],
                                  0, 0, hart_లెక్క,
                                  TT_IRQCHIP_అంకె_MSIS,
                                  TT_IRQCHIP_అంకె_PRIO_బిట్లు);

    riscv_aclint_mtimer_create(s->జ్ఞాపకం,
            s->memmap[TT_ATL_ACLINT].పునాది,
            TT_ACLINT_MTIME_కొలత,
            0, hart_లెక్క,
            TT_ACLINT_MTIMECMP,
            TT_ACLINT_MTIME,
            TT_ACLINT_TIMEBASE_FREQ, true);

    /*
     * DDR
     *
     * The high address is where RAM lives. It is always present and may be
     * up to 64GB. The low address is an alias of the first 2GB of that RAM.
     */
    ఒకవేళ (ram_కొలత > s->memmap[TT_ATL_DDR_HI].కొలత) {
        g_autofree అక్షరం *sz = కొలత_గమ్యం_str(s->memmap[TT_ATL_DDR_HI].కొలత);
        error_setg(errp, "RAM size is too large, maximum is %s", sz);
        తిరిగి;
    }

    జ్ఞాపకం_region_init_alias(&s->ram_hi, OBJECT(s), "ram.high", s->dram,
                             0, ram_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(s->జ్ఞాపకం,
                                s->memmap[TT_ATL_DDR_HI].పునాది, &s->ram_hi);

    lo_ram_కొలత = MIN(ram_కొలత, s->memmap[TT_ATL_DDR_LO].కొలత);
    జ్ఞాపకం_region_init_alias(&s->ram_lo, OBJECT(s), "ram.low", s->dram,
                             0, lo_ram_కొలత);
    జ్ఞాపకం_region_కూడు_subregion(s->జ్ఞాపకం,
                                s->memmap[TT_ATL_DDR_LO].పునాది, &s->ram_lo);

    /* Boot ROM */
    ఒకవేళ (!జ్ఞాపకం_region_init_rom(&s->bootrom, OBJECT(s), "tt-atlantis.bootrom",
                                s->memmap[TT_ATL_BOOTROM].కొలత, errp)) {
        తిరిగి;
    }
    జ్ఞాపకం_region_కూడు_subregion(s->జ్ఞాపకం, s->memmap[TT_ATL_BOOTROM].పునాది,
                                &s->bootrom);

    /* UART1, the soc console (UART0 is for the boot microcontroller) */
    serial_mm_init(s->జ్ఞాపకం, s->memmap[TT_ATL_UART1].పునాది, 2,
                   qdev_get_gpio_లోపల(s->irqchip, TT_ATL_UART1_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* 4 PRCM blocks */
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]),
                        errp)) {
        తిరిగి;
    }
    జ్ఞాపకం_region_కూడు_subregion(
        s->జ్ఞాపకం, s->memmap[TT_ATL_PRCM_RCPU].పునాది,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]), 0));

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]),
                        errp)) {
        తిరిగి;
    }
    జ్ఞాపకం_region_కూడు_subregion(
        s->జ్ఞాపకం, s->memmap[TT_ATL_PRCM_HSIO].పునాది,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]), 0));

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]),
                        errp)) {
        తిరిగి;
    }
    జ్ఞాపకం_region_కూడు_subregion(
        s->జ్ఞాపకం, s->memmap[TT_ATL_PRCM_PCIE].పునాది,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]), 0));

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM]),
                        errp)) {
        తిరిగి;
    }
    జ్ఞాపకం_region_కూడు_subregion(
        s->జ్ఞాపకం, s->memmap[TT_ATL_PRCM_MM].పునాది,
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
    mmio_map_unimplemented(s->జ్ఞాపకం, SYS_BUS_DEVICE(&s->uart1),
                           "tt-atlantis.uart1", s->memmap[TT_ATL_UART1].పునాది,
                           s->memmap[TT_ATL_UART1].కొలత);

    /* I2C */
    కొరకు (పూర్ణాంకం i = 0; i < TT_ATL_అంకె_I2_C; i++) {
        SysBusDevice *sbd = SYS_BUS_DEVICE(&s->i2c[i]);

        ఒకవేళ (!sysbus_realize(sbd, errp)) {
            తిరిగి;
        }
        జ్ఞాపకం_region_కూడు_subregion(s->జ్ఞాపకం,
                                    s->memmap[TT_ATL_I2C0 + i].పునాది,
                                    sysbus_mmio_get_region(sbd, 0));
        sysbus_connect_irq(sbd, 0,
                           qdev_get_gpio_లోపల(s->irqchip, TT_ATL_I2C0_IRQ + i));
    }
}

స్థిర స్థిరరాశి Property tt_atlantis_soc_props[] = {
    DEFINE_PROP_STRING("cpu-type", TTAtlantisSoCState, cpu_type),
    DEFINE_PROP_UINT32("num-harts", TTAtlantisSoCState, అంకె_harts, 8),
    DEFINE_PROP_LINK("memory", TTAtlantisSoCState, జ్ఞాపకం,
                     TYPE_జ్ఞాపకం_REGION, జ్ఞాపకం_Region *),
    DEFINE_PROP_LINK("dram", TTAtlantisSoCState, dram,
                     TYPE_జ్ఞాపకం_REGION, జ్ఞాపకం_Region *),
};

స్థిర శూన్య tt_atlantis_soc_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = tt_atlantis_soc_realize;
    device_class_సమితి_props(dc, tt_atlantis_soc_props);
    /* The SoC can only be instantiated from the machine */
    dc->user_creatable = false;
}

స్థిర శూన్య tt_atlantis_machine_init(MachineState *machine)
{
    TTAtlantisState *ams = TT_ATLANTIS_MACHINE(machine);
    TTAtlantisSoCState *s = &ams->soc;

    జ్ఞాపకం_region_init(&ams->soc_జ్ఞాపకం, OBJECT(machine),
                       "tt-atlantis.soc-memory", UINT64_MAX);
    జ్ఞాపకం_region_కూడు_subregion(get_system_జ్ఞాపకం(), 0, &ams->soc_జ్ఞాపకం);

    object_initialize_బిడ్డ(OBJECT(machine), "soc", &ams->soc,
                            TYPE_TT_ATLANTIS_SOC);
    object_property_సమితి_str(OBJECT(&ams->soc), "cpu-type", machine->cpu_type,
                            &error_abort);
    object_property_సమితి_int(OBJECT(&ams->soc), "num-harts", machine->smp.cpus,
                            &error_abort);

    object_property_సమితి_link(OBJECT(&ams->soc), "memory",
                             OBJECT(&ams->soc_జ్ఞాపకం), &error_abort);
    object_property_సమితి_link(OBJECT(&ams->soc), "dram", OBJECT(machine->ram),
                             &error_abort);

    qdev_realize(DEVICE(&ams->soc), ఖాళీ, &error_fatal);

    /* I2C peripherals: qemu specific */
    i2c_slave_create_simple(i2c_get_bus(s, 0), "ds1338", 0x6f);
    i2c_slave_create_simple(i2c_get_bus(s, 4), "tmp105", 0x48);

    /* Load or create device tree */
    ఒకవేళ (machine->dtb) {
        ఎక్కించు_fdt(ams);
    } లేకపోతే {
        create_fdt(ams);
    }

    ams->machine_పూర్తి.notify = tt_atlantis_machine_పూర్తి;
    qemu_కూడు_machine_init_పూర్తి_notifier(&ams->machine_పూర్తి);
}

స్థిర శూన్య tt_atlantis_machine_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Tenstorrent Atlantis RISC-V SoC (Experimental)";
    mc->init = tt_atlantis_machine_init;
    mc->max_cpus = 8;
    mc->default_cpus = 8;
    mc->default_ram_కొలత = 4 * GiB;
    mc->default_cpu_type = TYPE_RISCV_CPU_TT_ASCALON;
    mc->ఖండం_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->default_ram_id = "tt_atlantis.ram";
}

స్థిర స్థిరరాశి TypeInfo tt_atlantis_types[] = {
    {
        .name       = TYPE_TT_ATLANTIS_SOC,
        .మాతృ     = TYPE_DEVICE,
        .instance_కొలత = పరిమాణం(TTAtlantisSoCState),
        .instance_init = tt_atlantis_soc_init,
        .class_init = tt_atlantis_soc_class_init,
    }, {
        .name       = MACHINE_TYPE_NAME("tt-atlantis"),
        .మాతృ     = TYPE_MACHINE,
        .class_init = tt_atlantis_machine_class_init,
        .instance_కొలత = పరిమాణం(TTAtlantisState),
    },
};

DEFINE_TYPES(tt_atlantis_types)
