/*
 * Tenstorrent Atlantis RISC-V System on Chip
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright 2025 Tenstorrent, Joel Stanley <joel@jms.id.au>
 */

#ضمن "qemu/osdep.h"
#ضمن "qapi/error.h"
#ضمن "qemu/cutils.h"
#ضمن "qemu/error-report.h"
#ضمن "qemu/guest-random.h"
#ضمن "qemu/units.h"

#ضمن "hw/core/boards.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/core/sysbus.h"

#ضمن "target/riscv/cpu.h"

#ضمن "hw/riscv/boot.h"
#ضمن "hw/riscv/fdt-common.h"
#ضمن "hw/riscv/riscv_hart.h"

#ضمن "hw/char/serial-mm.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/misc/unimp.h"
#ضمن "hw/misc/tt_atlantis_prcm.h"

#ضمن "system/system.h"
#ضمن "system/device_tree.h"

#ضمن "hw/riscv/tt_atlantis.h"

#ضمن "aia.h"

#تعریف_کرو TT_IRQCHIP_عدد_MSIS       255
#تعریف_کرو TT_IRQCHIP_عدد_SOURCES    128
#تعریف_کرو TT_IRQCHIP_عدد_PRIO_بٹ  3
#تعریف_کرو TT_IMSIC_GUESTS           5
#تعریف_کرو TT_IMSIC_STRIDE           0x40000 /* Same stride for M and S */
#تعریف_کرو TT_IMSIC_GUEST_بٹ       6

/* Stride is fixed by hardware, check it's consistent with guest bits. */
QEMU_BUILD_BUG_ON(TT_IMSIC_STRIDE != (0x1000 << TT_IMSIC_GUEST_بٹ));

#تعریف_کرو TT_ACLINT_MTIME_ناپ    0x8050
#تعریف_کرو TT_ACLINT_MTIME         0x0
#تعریف_کرو TT_ACLINT_MTIMECMP      0x8000
#تعریف_کرو TT_ACLINT_TIMEBASE_FREQ 1000000000

ساكن ثابت Mem_Map_اندراج tt_atlantis_memmap[] = {
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

ساكن I2CBus *i2c_get_bus(TTAtlantisSoCState *s, بنانشانی busnr)
{
    أكد(busnr < TT_ATL_عدد_I2_C);

    أرجع s->i2c[busnr].bus;
}

ساكن uint32_t fdt_phandle = 1;
ساكن uint32_t اگلا_phandle(فراغ)
{
    أرجع fdt_phandle++;
}

ساكن فراغ create_fdt_حافظہ(فراغ *fdt, TTAtlantisSoCState *s)
{
    hwaddr ram_ناپ = حافظہ_region_ناپ(s->dram);
    hwaddr ناپ_lo = ram_ناپ;
    hwaddr ناپ_hi = 0;

    إذا (ناپ_lo > s->memmap[TT_ATL_DDR_LO].ناپ) {
        ناپ_lo = s->memmap[TT_ATL_DDR_LO].ناپ;
        ناپ_hi = ram_ناپ - ناپ_lo;
    }

    riscv_create_fdt_socket_حافظہ(fdt, s->memmap[TT_ATL_DDR_LO].ادھار,
                                   ناپ_lo, 0, false);
    إذا (ناپ_hi) {
        /*
         * The first part of the HI address is aliased at the LO address
         * so do not include that as usable memory. Is there any way
         * (or good reason) to describe that aliasing 2GB with DT?
         */
        riscv_create_fdt_socket_حافظہ(fdt,
                                       s->memmap[TT_ATL_DDR_HI].ادھار + ناپ_lo,
                                       ناپ_hi, 0, false);
    }
}

ساكن فراغ create_fdt_aclint(فراغ *fdt, TTAtlantisSoCState *s,
                              uint32_t *intc_phandles)
{
    g_autofree حرف *name = عدم;
    g_autofree uint32_t *aclint_mtimer_cells = عدم;
    uint32_t aclint_cells_ناپ;
    hwaddr addr;

    aclint_mtimer_cells = g_new0(uint32_t, s->cpus.عدد_harts * 2);

    لكل (صحيح cpu = 0; cpu < s->cpus.عدد_harts; cpu++) {
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_منزل_be32(IRQ_M_TIMER);
    }
    aclint_cells_ناپ = s->cpus.عدد_harts * حجم(uint32_t) * 2;

    addr = s->memmap[TT_ATL_ACLINT].ادھار;

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIX, addr);
    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr + TT_ACLINT_MTIME,
                                 2, 0x1000,
                                 2, addr + TT_ACLINT_MTIMECMP,
                                 2, 0x1000);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_ناپ);
}

ساكن فراغ create_fdt_one_imsic(فراغ *fdt, ثابت Mem_Map_اندراج *mem, صحيح cpus,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 صحيح irq_سطر, uint32_t imsic_guest_بٹ)
{
    g_autofree حرف *name = عدم;
    g_autofree uint32_t *imsic_cells = g_new0(uint32_t, cpus * 2);

    لكل (صحيح cpu = 0; cpu < cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_منزل_be32(irq_سطر);
    }

    name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->ادھار);
    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,imsics");

    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", عدم, 0);
    qemu_fdt_setprop(fdt, name, "msi-controller", عدم, 0);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     imsic_cells, حجم(uint32_t) * cpus * 2);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ادھار, 2, mem->ناپ);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-ids", TT_IRQCHIP_عدد_MSIS);

    إذا (imsic_guest_بٹ) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,guest-index-bits",
                              imsic_guest_بٹ);
    }
    qemu_fdt_setprop_cell(fdt, name, "phandle", msi_phandle);
}

ساكن فراغ create_fdt_one_aplic(فراغ *fdt,
                                 ثابت Mem_Map_اندراج *mem,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_بچہ_phandle,
                                 صحيح irq_سطر, صحيح عدد_harts)
{
    g_autofree حرف *name =
        g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->ادھار);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, عدد_harts * 2);

    لكل (صحيح cpu = 0; cpu < عدد_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_منزل_be32(irq_سطر);
    }

    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aplic");
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 2);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", عدم, 0);

    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aplic_cells, عدد_harts * حجم(uint32_t) * 2);
    qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_phandle);

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ادھار, 2, mem->ناپ);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-sources",
                          TT_IRQCHIP_عدد_SOURCES);

    إذا (aplic_بچہ_phandle) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,children",
                              aplic_بچہ_phandle);
        qemu_fdt_setprop_cells(fdt, name, "riscv,delegation",
                               aplic_بچہ_phandle, 1, TT_IRQCHIP_عدد_SOURCES);
    }

    qemu_fdt_setprop_cell(fdt, name, "phandle", aplic_phandle);
}

ساكن فراغ create_fdt_pmu(فراغ *fdt, TTAtlantisSoCState *s)
{
    حرف pmu_name[] = "/pmu";
    RISCVCPU *hart = &s->cpus.harts[0];

    qemu_fdt_جوڑو_subnode(fdt, pmu_name);
    qemu_fdt_setprop_string(fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_گرہ(fdt, hart->pmu_avail_ctrs, pmu_name);
}

ساكن فراغ create_fdt_cpu(فراغ *fdt, TTAtlantisSoCState *s,
                           uint32_t aplic_s_phandle,
                           uint32_t imsic_s_phandle)
{
    g_autofree uint32_t *intc_phandles = g_new0(uint32_t, s->cpus.عدد_harts);
    صحيح عدد_harts = s->cpus.عدد_harts;

    riscv_fdt_create_cpu_socket_subnode(fdt, TT_ACLINT_TIMEBASE_FREQ);

    riscv_create_fdt_socket_cpus(fdt, s->cpus.harts, 0, عدد_harts,
                                 s->cpus.hartid_ادھار, &fdt_phandle,
                                 intc_phandles, false, false);

    create_fdt_aclint(fdt, s, intc_phandles);

    /* M-level IMSIC node */
    uint32_t msi_m_phandle = اگلا_phandle();
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_MIMSIC], عدد_harts,
                         intc_phandles, msi_m_phandle,
                         IRQ_M_EXT, TT_IMSIC_GUEST_بٹ);

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_SIMSIC], عدد_harts,
                         intc_phandles, imsic_s_phandle,
                         IRQ_S_EXT, TT_IMSIC_GUEST_بٹ);

    uint32_t aplic_m_phandle = اگلا_phandle();

    /* M-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_MAPLIC],
                         msi_m_phandle, intc_phandles,
                         aplic_m_phandle, aplic_s_phandle,
                         IRQ_M_EXT, عدد_harts);

    /* S-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_SAPLIC],
                         imsic_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         IRQ_S_EXT, عدد_harts);
}

ساكن فراغ create_fdt_uart(فراغ *fdt, ثابت Mem_Map_اندراج *mem, صحيح irq,
                            صحيح irqchip_phandle, uint32_t clk_ماپا_phandle,
                            uint32_t clk_phandle)
{
    g_autofree حرف *name = g_strdup_printf("/soc/serial@%"HWADDR_PRIX,
                                            mem->ادھار);

    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ادھار, 2, mem->ناپ);
    qemu_fdt_setprop_cell(fdt, name, "reg-shift", 2);
    qemu_fdt_setprop_cell(fdt, name, "reg-io-width", 4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_ماپا_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 3686400);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);

    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(fdt, "/aliases", "serial0", name);
}

ساكن فراغ create_fdt_rng(فراغ *fdt)
{
    uint8_t rng_بیج[32];

    qemu_guest_getrandom_nofail(rng_بیج, حجم(rng_بیج));
    qemu_fdt_setprop(fdt, "/chosen", "rng-seed", rng_بیج, حجم(rng_بیج));
}

ساكن فراغ create_fdt_clk(فراغ *fdt, ثابت حرف *clock_name,
                           uint32_t freq, uint32_t phandle)
{
    g_autofree حرف *name = g_strdup_printf("/clocks/%s", clock_name);

    qemu_fdt_جوڑو_رستہ(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "fixed-clock");
    qemu_fdt_setprop_string(fdt, name, "clock-output-names", clock_name);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", freq);
    qemu_fdt_setprop_cell(fdt, name, "phandle", phandle);
}

ساكن فراغ create_fdt_i2c(فراغ *fdt, ثابت Mem_Map_اندراج *mem, uint32_t irq,
                           uint32_t irqchip_phandle,
                           uint32_t clk_ماپا_phandle,
                           uint32_t clk_phandle)
{
    g_autofree حرف *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX, mem->ادھار);

    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "snps,designware-i2c");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->ادھار, 2, mem->ناپ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_ماپا_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 100000);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0);
}

ساكن فراغ create_fdt_i2c_device(فراغ *fdt, TTAtlantisSoCState *s, صحيح bus,
                                  ثابت حرف *compat, صحيح addr)
{
    hwaddr ادھار = s->memmap[TT_ATL_I2C0 + bus].ادھار;
    g_autofree حرف *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX"/sensor@%x",
                                            ادھار, addr);

    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compat);
    qemu_fdt_setprop_cell(fdt, name, "reg", addr);
}

ساكن حرف *create_fdt_prcm(فراغ *fdt, ثابت Mem_Map_اندراج *mem,
                             ثابت حرف *prcm_name)
{
    hwaddr ادھار = mem->ادھار;
    hwaddr ناپ = mem->ناپ;
    حرف *name = g_strdup_printf("/soc/clock-controller@%" PRIx64,
                                 ادھار);
    g_autofree حرف *compatible =
        g_strdup_printf("tenstorrent,atlantis-prcm-%s", prcm_name);

    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compatible);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, ادھار, 2, ناپ);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#reset-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "phandle", اگلا_phandle());

    أرجع name;
}

ساكن فراغ finalize_fdt(فراغ *fdt, TTAtlantisSoCState *s)
{
    uint32_t aplic_s_phandle = اگلا_phandle();
    uint32_t imsic_s_phandle = اگلا_phandle();
    uint32_t osc_24m_phandle = اگلا_phandle();
    uint32_t prcm_rcpu_phandle;
    g_autofree حرف *rcpu_name, *hsio_name, *pcie_name, *mm_name;

    create_fdt_cpu(fdt, s, aplic_s_phandle, imsic_s_phandle);

    create_fdt_حافظہ(fdt, s);

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
                           prcm_rcpu_phandle, TT_ATL_CLK_RCPU_جڑ,
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

    لكل (صحيح i = 0; i < TT_ATL_عدد_I2_C; i++) {
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

ساكن فراغ create_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    صحيح fdt_ناپ = 0;

    ms->fdt = riscv_create_board_device_درخت("Tenstorrent Atlantis RISC-V Machine",
                                             "tenstorrent,atlantis", &fdt_ناپ);

    qemu_fdt_جوڑو_subnode(ms->fdt, "/chosen");

    create_fdt_rng(ms->fdt);

    qemu_fdt_جوڑو_subnode(ms->fdt, "/aliases");

    create_fdt_pmu(ms->fdt, &ams->soc);
}

ساكن فراغ لدو_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    حرف **گرہ_رستہ;
    Error *err = عدم;
    صحيح fdt_ناپ = 0;

    ms->fdt = لدو_device_درخت(ms->dtb, &fdt_ناپ);
    إذا (!ms->fdt) {
        error_report("load_device_tree() failed");
        باہر_نکلو(1);
    }

    qemu_fdt_جوڑو_رستہ(ms->fdt, "/chosen");

    /* Clear memory nodes and update with the specified RAM size */
    گرہ_رستہ = qemu_fdt_گرہ_unit_رستہ(ms->fdt, "memory", &err);
    إذا (err) {
        warn_report_err(err);
    } وإلا {
        لكل (صحيح i = 0; گرہ_رستہ[i]; i++) {
            warn_report("Replacing device tree %s with the requested RAM size",
                        گرہ_رستہ[i]);
            qemu_fdt_nop_گرہ(ms->fdt, گرہ_رستہ[i]);
        }
        g_strfreev(گرہ_رستہ);
    }

    create_fdt_حافظہ(ms->fdt, &ams->soc);
}

ساكن فراغ mmio_map_unimplemented(حافظہ_Region *حافظہ, SysBusDevice *dev,
                                   ثابت حرف *name, hwaddr addr, uint64_t ناپ)
{
    qdev_prop_سیٹ_string(DEVICE(dev), "name", name);
    qdev_prop_سیٹ_uint64(DEVICE(dev), "size", ناپ);
    sysbus_realize(dev, &error_abort);

    حافظہ_region_جوڑو_subregion_overlap(حافظہ, addr,
                                        sysbus_mmio_get_region(dev, 0), -1000);
}

ساكن فراغ tt_atlantis_machine_پورا(Notifier *n, فراغ *انکڑے)
{
    TTAtlantisState *ams = container_دا(n, TTAtlantisState, machine_پورا);
    TTAtlantisSoCState *s = &ams->soc;
    MachineState *machine = MACHINE(ams);
    hwaddr شروع_addr = s->memmap[TT_ATL_DDR_LO].ادھار;
    hwaddr mem_ناپ;
    نشانہ_ulong firmware_سرا_addr, kernel_شروع_addr;
    ثابت حرف *firmware_name = riscv_default_firmware_name(&s->cpus);
    uint64_t fdt_لدو_addr;
    uint64_t kernel_اندراج;
    RISCVBootInfo boot_info;

    /*
     * A user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    إذا (machine->dtb == عدم) {
        finalize_fdt(machine->fdt, s);
    }

    mem_ناپ = machine->ram_ناپ;
    إذا (mem_ناپ > s->memmap[TT_ATL_DDR_LO].ناپ) {
        mem_ناپ = s->memmap[TT_ATL_DDR_LO].ناپ;
    }
    riscv_boot_info_init_discontig_mem(&boot_info, &s->cpus,
                                       s->memmap[TT_ATL_DDR_LO].ادھار,
                                       mem_ناپ);

    firmware_سرا_addr = riscv_لبھو_and_لدو_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &شروع_addr, عدم);

    kernel_شروع_addr = riscv_calc_kernel_شروع_addr(&boot_info,
                                                     firmware_سرا_addr);
    إذا (machine->kernel_filename) {
        riscv_لدو_kernel(machine, &boot_info, kernel_شروع_addr,
                          true, عدم);
        kernel_اندراج = boot_info.image_کم_addr;
    } وإلا {
        /* If we aren't loading a payload, OpenSBI thinks we are trying to boot
         * address 0, which fails `sbi_domain_check_addr()` as that is where
         * OpenSBI is running. Instead point OpenSBI to the end of the region
         * where it was loaded, which avoids the early hang, allowing the
         * system to proceed with the OpenSBI boot output.
         */
        kernel_اندراج = kernel_شروع_addr;
    }

    fdt_لدو_addr = riscv_compute_fdt_addr(s->memmap[TT_ATL_DDR_LO].ادھار,
                                           s->memmap[TT_ATL_DDR_LO].ناپ,
                                           machine, &boot_info);
    riscv_لدو_fdt(fdt_لدو_addr, machine->fdt);

    /* load the reset vector */
    riscv_تیاری_rom_reset_vec(machine, &s->cpus, شروع_addr,
                              s->memmap[TT_ATL_BOOTROM].ادھار,
                              s->memmap[TT_ATL_BOOTROM].ناپ,
                              kernel_اندراج,
                              fdt_لدو_addr);
}

ساكن فراغ tt_atlantis_soc_init(Object *obj)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(obj);

    object_initialize_بچہ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);

    object_initialize_بچہ(obj, "uart1", &s->uart1,
                            TYPE_UNIMPLEMENTED_DEVICE);

    object_initialize_بچہ(obj, "prcm-rcpu",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU],
                            TYPE_TT_ATLANTIS_PRCM_RCPU);
    object_initialize_بچہ(obj, "prcm-hsio",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO],
                            TYPE_TT_ATLANTIS_PRCM_HSIO);
    object_initialize_بچہ(obj, "prcm-pcie",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE],
                            TYPE_TT_ATLANTIS_PRCM_PCIE);
    object_initialize_بچہ(obj, "prcm-mm",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM],
                            TYPE_TT_ATLANTIS_PRCM_MM);

    لكل (صحيح i = 0; i < TT_ATL_عدد_I2_C; i++) {
        object_initialize_بچہ(obj, "i2c[*]", &s->i2c[i],
                                TYPE_DESIGNWARE_I2C);
    }
}

ساكن فراغ tt_atlantis_soc_realize(DeviceState *dev, Error **errp)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(dev);
    ram_addr_t lo_ram_ناپ, ram_ناپ;
    صحيح hart_گنتی = s->عدد_harts;

    إذا (!s->حافظہ) {
        error_setg(errp, "'memory' link is not set");
        أرجع;
    }
    إذا (!s->dram) {
        error_setg(errp, "'dram' link is not set");
        أرجع;
    }
    ram_ناپ = حافظہ_region_ناپ(s->dram);

    s->memmap = tt_atlantis_memmap;

    /* CPUs */
    object_property_سیٹ_str(OBJECT(&s->cpus), "cpu-type", s->cpu_type,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&s->cpus), "hartid-base", 0,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&s->cpus), "num-harts", hart_گنتی,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&s->cpus), "resetvec",
                            s->memmap[TT_ATL_BOOTROM].ادھار,
                            &error_abort);
    object_property_سیٹ_link(OBJECT(&s->cpus), "memory", OBJECT(s->حافظہ),
                             &error_abort);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        أرجع;
    }

    s->irqchip = riscv_create_aia(s->حافظہ,
                                  true, TT_IMSIC_GUESTS,
                                  TT_IMSIC_STRIDE,
                                  TT_IMSIC_STRIDE,
                                  TT_IRQCHIP_عدد_SOURCES,
                                  &s->memmap[TT_ATL_MAPLIC],
                                  &s->memmap[TT_ATL_SAPLIC],
                                  &s->memmap[TT_ATL_MIMSIC],
                                  &s->memmap[TT_ATL_SIMSIC],
                                  0, 0, hart_گنتی,
                                  TT_IRQCHIP_عدد_MSIS,
                                  TT_IRQCHIP_عدد_PRIO_بٹ);

    riscv_aclint_mtimer_create(s->حافظہ,
            s->memmap[TT_ATL_ACLINT].ادھار,
            TT_ACLINT_MTIME_ناپ,
            0, hart_گنتی,
            TT_ACLINT_MTIMECMP,
            TT_ACLINT_MTIME,
            TT_ACLINT_TIMEBASE_FREQ, true);

    /*
     * DDR
     *
     * The high address is where RAM lives. It is always present and may be
     * up to 64GB. The low address is an alias of the first 2GB of that RAM.
     */
    إذا (ram_ناپ > s->memmap[TT_ATL_DDR_HI].ناپ) {
        g_autofree حرف *sz = ناپ_منزل_str(s->memmap[TT_ATL_DDR_HI].ناپ);
        error_setg(errp, "RAM size is too large, maximum is %s", sz);
        أرجع;
    }

    حافظہ_region_init_alias(&s->ram_hi, OBJECT(s), "ram.high", s->dram,
                             0, ram_ناپ);
    حافظہ_region_جوڑو_subregion(s->حافظہ,
                                s->memmap[TT_ATL_DDR_HI].ادھار, &s->ram_hi);

    lo_ram_ناپ = MIN(ram_ناپ, s->memmap[TT_ATL_DDR_LO].ناپ);
    حافظہ_region_init_alias(&s->ram_lo, OBJECT(s), "ram.low", s->dram,
                             0, lo_ram_ناپ);
    حافظہ_region_جوڑو_subregion(s->حافظہ,
                                s->memmap[TT_ATL_DDR_LO].ادھار, &s->ram_lo);

    /* Boot ROM */
    إذا (!حافظہ_region_init_rom(&s->bootrom, OBJECT(s), "tt-atlantis.bootrom",
                                s->memmap[TT_ATL_BOOTROM].ناپ, errp)) {
        أرجع;
    }
    حافظہ_region_جوڑو_subregion(s->حافظہ, s->memmap[TT_ATL_BOOTROM].ادھار,
                                &s->bootrom);

    /* UART1, the soc console (UART0 is for the boot microcontroller) */
    serial_mm_init(s->حافظہ, s->memmap[TT_ATL_UART1].ادھار, 2,
                   qdev_get_gpio_وچ(s->irqchip, TT_ATL_UART1_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* 4 PRCM blocks */
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]),
                        errp)) {
        أرجع;
    }
    حافظہ_region_جوڑو_subregion(
        s->حافظہ, s->memmap[TT_ATL_PRCM_RCPU].ادھار,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]), 0));

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]),
                        errp)) {
        أرجع;
    }
    حافظہ_region_جوڑو_subregion(
        s->حافظہ, s->memmap[TT_ATL_PRCM_HSIO].ادھار,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]), 0));

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]),
                        errp)) {
        أرجع;
    }
    حافظہ_region_جوڑو_subregion(
        s->حافظہ, s->memmap[TT_ATL_PRCM_PCIE].ادھار,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]), 0));

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM]),
                        errp)) {
        أرجع;
    }
    حافظہ_region_جوڑو_subregion(
        s->حافظہ, s->memmap[TT_ATL_PRCM_MM].ادھار,
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
    mmio_map_unimplemented(s->حافظہ, SYS_BUS_DEVICE(&s->uart1),
                           "tt-atlantis.uart1", s->memmap[TT_ATL_UART1].ادھار,
                           s->memmap[TT_ATL_UART1].ناپ);

    /* I2C */
    لكل (صحيح i = 0; i < TT_ATL_عدد_I2_C; i++) {
        SysBusDevice *sbd = SYS_BUS_DEVICE(&s->i2c[i]);

        إذا (!sysbus_realize(sbd, errp)) {
            أرجع;
        }
        حافظہ_region_جوڑو_subregion(s->حافظہ,
                                    s->memmap[TT_ATL_I2C0 + i].ادھار,
                                    sysbus_mmio_get_region(sbd, 0));
        sysbus_connect_irq(sbd, 0,
                           qdev_get_gpio_وچ(s->irqchip, TT_ATL_I2C0_IRQ + i));
    }
}

ساكن ثابت Property tt_atlantis_soc_props[] = {
    DEFINE_PROP_STRING("cpu-type", TTAtlantisSoCState, cpu_type),
    DEFINE_PROP_UINT32("num-harts", TTAtlantisSoCState, عدد_harts, 8),
    DEFINE_PROP_LINK("memory", TTAtlantisSoCState, حافظہ,
                     TYPE_حافظہ_REGION, حافظہ_Region *),
    DEFINE_PROP_LINK("dram", TTAtlantisSoCState, dram,
                     TYPE_حافظہ_REGION, حافظہ_Region *),
};

ساكن فراغ tt_atlantis_soc_class_init(ObjectClass *oc, ثابت فراغ *انکڑے)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = tt_atlantis_soc_realize;
    device_class_سیٹ_props(dc, tt_atlantis_soc_props);
    /* The SoC can only be instantiated from the machine */
    dc->user_creatable = false;
}

ساكن فراغ tt_atlantis_machine_init(MachineState *machine)
{
    TTAtlantisState *ams = TT_ATLANTIS_MACHINE(machine);
    TTAtlantisSoCState *s = &ams->soc;

    حافظہ_region_init(&ams->soc_حافظہ, OBJECT(machine),
                       "tt-atlantis.soc-memory", UINT64_MAX);
    حافظہ_region_جوڑو_subregion(get_system_حافظہ(), 0, &ams->soc_حافظہ);

    object_initialize_بچہ(OBJECT(machine), "soc", &ams->soc,
                            TYPE_TT_ATLANTIS_SOC);
    object_property_سیٹ_str(OBJECT(&ams->soc), "cpu-type", machine->cpu_type,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&ams->soc), "num-harts", machine->smp.cpus,
                            &error_abort);

    object_property_سیٹ_link(OBJECT(&ams->soc), "memory",
                             OBJECT(&ams->soc_حافظہ), &error_abort);
    object_property_سیٹ_link(OBJECT(&ams->soc), "dram", OBJECT(machine->ram),
                             &error_abort);

    qdev_realize(DEVICE(&ams->soc), عدم, &error_fatal);

    /* I2C peripherals: qemu specific */
    i2c_slave_create_simple(i2c_get_bus(s, 0), "ds1338", 0x6f);
    i2c_slave_create_simple(i2c_get_bus(s, 4), "tmp105", 0x48);

    /* Load or create device tree */
    إذا (machine->dtb) {
        لدو_fdt(ams);
    } وإلا {
        create_fdt(ams);
    }

    ams->machine_پورا.notify = tt_atlantis_machine_پورا;
    qemu_جوڑو_machine_init_پورا_notifier(&ams->machine_پورا);
}

ساكن فراغ tt_atlantis_machine_class_init(ObjectClass *oc, ثابت فراغ *انکڑے)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Tenstorrent Atlantis RISC-V SoC (Experimental)";
    mc->init = tt_atlantis_machine_init;
    mc->max_cpus = 8;
    mc->default_cpus = 8;
    mc->default_ram_ناپ = 4 * GiB;
    mc->default_cpu_type = TYPE_RISCV_CPU_TT_ASCALON;
    mc->بلاک_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->default_ram_id = "tt_atlantis.ram";
}

ساكن ثابت TypeInfo tt_atlantis_types[] = {
    {
        .name       = TYPE_TT_ATLANTIS_SOC,
        .ماپا     = TYPE_DEVICE,
        .instance_ناپ = حجم(TTAtlantisSoCState),
        .instance_init = tt_atlantis_soc_init,
        .class_init = tt_atlantis_soc_class_init,
    }, {
        .name       = MACHINE_TYPE_NAME("tt-atlantis"),
        .ماپا     = TYPE_MACHINE,
        .class_init = tt_atlantis_machine_class_init,
        .instance_ناپ = حجم(TTAtlantisState),
    },
};

DEFINE_TYPES(tt_atlantis_types)
