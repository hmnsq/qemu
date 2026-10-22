/*
 * Tenstorrent Atlantis RISC-V System on Chip
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright 2025 Tenstorrent, Joel Stanley <joel@jms.id.au>
 */

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/cutils.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "qemu/guest-random.h"
#अन्तर्भाव "qemu/units.h"

#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/core/loader.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "hw/core/sysbus.h"

#अन्तर्भाव "target/riscv/cpu.h"

#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "hw/riscv/fdt-common.h"
#अन्तर्भाव "hw/riscv/riscv_hart.h"

#अन्तर्भाव "hw/char/serial-mm.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "hw/misc/unimp.h"
#अन्तर्भाव "hw/misc/tt_atlantis_prcm.h"

#अन्तर्भाव "system/system.h"
#अन्तर्भाव "system/device_tree.h"

#अन्तर्भाव "hw/riscv/tt_atlantis.h"

#अन्तर्भाव "aia.h"

#define TT_IRQCHIP_अङ्कः_MSIS       255
#define TT_IRQCHIP_अङ्कः_SOURCES    128
#define TT_IRQCHIP_अङ्कः_PRIO_BITS  3
#define TT_IMSIC_GUESTS           5
#define TT_IMSIC_STRIDE           0x40000 /* Same stride for M and S */
#define TT_IMSIC_GUEST_BITS       6

/* Stride is fixed by hardware, check it's consistent with guest bits. */
QEMU_BUILD_BUG_ON(TT_IMSIC_STRIDE != (0x1000 << TT_IMSIC_GUEST_BITS));

#define TT_ACLINT_MTIME_SIZE    0x8050
#define TT_ACLINT_MTIME         0x0
#define TT_ACLINT_MTIMECMP      0x8000
#define TT_ACLINT_TIMEBASE_FREQ 1000000000

स्थिर ध्रुव MemMapEntry tt_atlantis_memmap[] = {
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

स्थिर I2CBus *i2c_get_bus(TTAtlantisSoCState *s, unsigned busnr)
{
    प्रतिज्ञा(busnr < TT_ATL_अङ्कः_I2_C);

    पटिनिवत्त s->i2c[busnr].bus;
}

स्थिर uint32_t fdt_phandle = 1;
स्थिर uint32_t next_phandle(शून्य)
{
    पटिनिवत्त fdt_phandle++;
}

स्थिर शून्य create_fdt_memory(शून्य *fdt, TTAtlantisSoCState *s)
{
    hwaddr ram_size = memory_region_size(s->dram);
    hwaddr size_lo = ram_size;
    hwaddr size_hi = 0;

    सचे (size_lo > s->memmap[TT_ATL_DDR_LO].size) {
        size_lo = s->memmap[TT_ATL_DDR_LO].size;
        size_hi = ram_size - size_lo;
    }

    riscv_create_fdt_socket_memory(fdt, s->memmap[TT_ATL_DDR_LO].base,
                                   size_lo, 0, false);
    सचे (size_hi) {
        /*
         * The first part of the HI address is aliased at the LO address
         * so do not include that as usable memory. Is there any way
         * (or good reason) to describe that aliasing 2GB with DT?
         */
        riscv_create_fdt_socket_memory(fdt,
                                       s->memmap[TT_ATL_DDR_HI].base + size_lo,
                                       size_hi, 0, false);
    }
}

स्थिर शून्य create_fdt_aclint(शून्य *fdt, TTAtlantisSoCState *s,
                              uint32_t *intc_phandles)
{
    g_autofree वर्ण *name = रिक्त;
    g_autofree uint32_t *aclint_mtimer_cells = रिक्त;
    uint32_t aclint_cells_size;
    hwaddr addr;

    aclint_mtimer_cells = g_new0(uint32_t, s->cpus.अङ्कः_harts * 2);

    अत्थाय (पूर्णाङ्क cpu = 0; cpu < s->cpus.अङ्कः_harts; cpu++) {
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_to_be32(IRQ_M_TIMER);
    }
    aclint_cells_size = s->cpus.अङ्कः_harts * परिमाण(uint32_t) * 2;

    addr = s->memmap[TT_ATL_ACLINT].base;

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIX, addr);
    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr + TT_ACLINT_MTIME,
                                 2, 0x1000,
                                 2, addr + TT_ACLINT_MTIMECMP,
                                 2, 0x1000);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_size);
}

स्थिर शून्य create_fdt_one_imsic(शून्य *fdt, ध्रुव MemMapEntry *mem, पूर्णाङ्क cpus,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 पूर्णाङ्क irq_पङ्क्तिः, uint32_t imsic_guest_bits)
{
    g_autofree वर्ण *name = रिक्त;
    g_autofree uint32_t *imsic_cells = g_new0(uint32_t, cpus * 2);

    अत्थाय (पूर्णाङ्क cpu = 0; cpu < cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_to_be32(irq_पङ्क्तिः);
    }

    name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->base);
    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,imsics");

    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", रिक्त, 0);
    qemu_fdt_setprop(fdt, name, "msi-controller", रिक्त, 0);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     imsic_cells, परिमाण(uint32_t) * cpus * 2);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->base, 2, mem->size);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-ids", TT_IRQCHIP_अङ्कः_MSIS);

    सचे (imsic_guest_bits) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,guest-index-bits",
                              imsic_guest_bits);
    }
    qemu_fdt_setprop_cell(fdt, name, "phandle", msi_phandle);
}

स्थिर शून्य create_fdt_one_aplic(शून्य *fdt,
                                 ध्रुव MemMapEntry *mem,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_child_phandle,
                                 पूर्णाङ्क irq_पङ्क्तिः, पूर्णाङ्क अङ्कः_harts)
{
    g_autofree वर्ण *name =
        g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->base);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, अङ्कः_harts * 2);

    अत्थाय (पूर्णाङ्क cpu = 0; cpu < अङ्कः_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_to_be32(irq_पङ्क्तिः);
    }

    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aplic");
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 2);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", रिक्त, 0);

    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aplic_cells, अङ्कः_harts * परिमाण(uint32_t) * 2);
    qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_phandle);

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->base, 2, mem->size);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-sources",
                          TT_IRQCHIP_अङ्कः_SOURCES);

    सचे (aplic_child_phandle) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,children",
                              aplic_child_phandle);
        qemu_fdt_setprop_cells(fdt, name, "riscv,delegation",
                               aplic_child_phandle, 1, TT_IRQCHIP_अङ्कः_SOURCES);
    }

    qemu_fdt_setprop_cell(fdt, name, "phandle", aplic_phandle);
}

स्थिर शून्य create_fdt_pmu(शून्य *fdt, TTAtlantisSoCState *s)
{
    वर्ण pmu_name[] = "/pmu";
    RISCVCPU *hart = &s->cpus.harts[0];

    qemu_fdt_add_subnode(fdt, pmu_name);
    qemu_fdt_setprop_string(fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_ग्रन्थिः(fdt, hart->pmu_avail_ctrs, pmu_name);
}

स्थिर शून्य create_fdt_cpu(शून्य *fdt, TTAtlantisSoCState *s,
                           uint32_t aplic_s_phandle,
                           uint32_t imsic_s_phandle)
{
    g_autofree uint32_t *intc_phandles = g_new0(uint32_t, s->cpus.अङ्कः_harts);
    पूर्णाङ्क अङ्कः_harts = s->cpus.अङ्कः_harts;

    riscv_fdt_create_cpu_socket_subnode(fdt, TT_ACLINT_TIMEBASE_FREQ);

    riscv_create_fdt_socket_cpus(fdt, s->cpus.harts, 0, अङ्कः_harts,
                                 s->cpus.hartid_base, &fdt_phandle,
                                 intc_phandles, false, false);

    create_fdt_aclint(fdt, s, intc_phandles);

    /* M-level IMSIC node */
    uint32_t msi_m_phandle = next_phandle();
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_MIMSIC], अङ्कः_harts,
                         intc_phandles, msi_m_phandle,
                         IRQ_M_EXT, TT_IMSIC_GUEST_BITS);

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_SIMSIC], अङ्कः_harts,
                         intc_phandles, imsic_s_phandle,
                         IRQ_S_EXT, TT_IMSIC_GUEST_BITS);

    uint32_t aplic_m_phandle = next_phandle();

    /* M-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_MAPLIC],
                         msi_m_phandle, intc_phandles,
                         aplic_m_phandle, aplic_s_phandle,
                         IRQ_M_EXT, अङ्कः_harts);

    /* S-level APLIC node */
    create_fdt_one_aplic(fdt, &s->memmap[TT_ATL_SAPLIC],
                         imsic_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         IRQ_S_EXT, अङ्कः_harts);
}

स्थिर शून्य create_fdt_uart(शून्य *fdt, ध्रुव MemMapEntry *mem, पूर्णाङ्क irq,
                            पूर्णाङ्क irqchip_phandle, uint32_t clk_parent_phandle,
                            uint32_t clk_phandle)
{
    g_autofree वर्ण *name = g_strdup_printf("/soc/serial@%"HWADDR_PRIX,
                                            mem->base);

    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->base, 2, mem->size);
    qemu_fdt_setprop_cell(fdt, name, "reg-shift", 2);
    qemu_fdt_setprop_cell(fdt, name, "reg-io-width", 4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_parent_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 3686400);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);

    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(fdt, "/aliases", "serial0", name);
}

स्थिर शून्य create_fdt_rng(शून्य *fdt)
{
    uint8_t rng_seed[32];

    qemu_guest_getrandom_nofail(rng_seed, परिमाण(rng_seed));
    qemu_fdt_setprop(fdt, "/chosen", "rng-seed", rng_seed, परिमाण(rng_seed));
}

स्थिर शून्य create_fdt_clk(शून्य *fdt, ध्रुव वर्ण *clock_name,
                           uint32_t freq, uint32_t phandle)
{
    g_autofree वर्ण *name = g_strdup_printf("/clocks/%s", clock_name);

    qemu_fdt_add_path(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "fixed-clock");
    qemu_fdt_setprop_string(fdt, name, "clock-output-names", clock_name);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", freq);
    qemu_fdt_setprop_cell(fdt, name, "phandle", phandle);
}

स्थिर शून्य create_fdt_i2c(शून्य *fdt, ध्रुव MemMapEntry *mem, uint32_t irq,
                           uint32_t irqchip_phandle,
                           uint32_t clk_parent_phandle,
                           uint32_t clk_phandle)
{
    g_autofree वर्ण *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX, mem->base);

    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "snps,designware-i2c");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->base, 2, mem->size);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irqchip_phandle);
    qemu_fdt_setprop_cells(fdt, name, "interrupts", irq, 0x4);
    qemu_fdt_setprop_cells(fdt, name, "clocks", clk_parent_phandle,
                           clk_phandle);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", 100000);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0);
}

स्थिर शून्य create_fdt_i2c_device(शून्य *fdt, TTAtlantisSoCState *s, पूर्णाङ्क bus,
                                  ध्रुव वर्ण *compat, पूर्णाङ्क addr)
{
    hwaddr base = s->memmap[TT_ATL_I2C0 + bus].base;
    g_autofree वर्ण *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX"/sensor@%x",
                                            base, addr);

    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compat);
    qemu_fdt_setprop_cell(fdt, name, "reg", addr);
}

स्थिर वर्ण *create_fdt_prcm(शून्य *fdt, ध्रुव MemMapEntry *mem,
                             ध्रुव वर्ण *prcm_name)
{
    hwaddr base = mem->base;
    hwaddr size = mem->size;
    वर्ण *name = g_strdup_printf("/soc/clock-controller@%" PRIx64,
                                 base);
    g_autofree वर्ण *compatible =
        g_strdup_printf("tenstorrent,atlantis-prcm-%s", prcm_name);

    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compatible);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, base, 2, size);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#reset-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "phandle", next_phandle());

    पटिनिवत्त name;
}

स्थिर शून्य finalize_fdt(शून्य *fdt, TTAtlantisSoCState *s)
{
    uint32_t aplic_s_phandle = next_phandle();
    uint32_t imsic_s_phandle = next_phandle();
    uint32_t osc_24m_phandle = next_phandle();
    uint32_t prcm_rcpu_phandle;
    g_autofree वर्ण *rcpu_name, *hsio_name, *pcie_name, *mm_name;

    create_fdt_cpu(fdt, s, aplic_s_phandle, imsic_s_phandle);

    create_fdt_memory(fdt, s);

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
                           prcm_rcpu_phandle, TT_ATL_CLK_RCPU_मूलम्,
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

    अत्थाय (पूर्णाङ्क i = 0; i < TT_ATL_अङ्कः_I2_C; i++) {
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

स्थिर शून्य create_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    पूर्णाङ्क fdt_size = 0;

    ms->fdt = riscv_create_board_device_वृक्षः("Tenstorrent Atlantis RISC-V Machine",
                                             "tenstorrent,atlantis", &fdt_size);

    qemu_fdt_add_subnode(ms->fdt, "/chosen");

    create_fdt_rng(ms->fdt);

    qemu_fdt_add_subnode(ms->fdt, "/aliases");

    create_fdt_pmu(ms->fdt, &ams->soc);
}

स्थिर शून्य load_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    वर्ण **ग्रन्थिः_path;
    Error *err = रिक्त;
    पूर्णाङ्क fdt_size = 0;

    ms->fdt = load_device_वृक्षः(ms->dtb, &fdt_size);
    सचे (!ms->fdt) {
        error_report("load_device_tree() failed");
        exit(1);
    }

    qemu_fdt_add_path(ms->fdt, "/chosen");

    /* Clear memory nodes and update with the specified RAM size */
    ग्रन्थिः_path = qemu_fdt_ग्रन्थिः_unit_path(ms->fdt, "memory", &err);
    सचे (err) {
        warn_report_err(err);
    } अञ्ञथा {
        अत्थाय (पूर्णाङ्क i = 0; ग्रन्थिः_path[i]; i++) {
            warn_report("Replacing device tree %s with the requested RAM size",
                        ग्रन्थिः_path[i]);
            qemu_fdt_nop_ग्रन्थिः(ms->fdt, ग्रन्थिः_path[i]);
        }
        g_strfreev(ग्रन्थिः_path);
    }

    create_fdt_memory(ms->fdt, &ams->soc);
}

स्थिर शून्य mmio_map_unimplemented(MemoryRegion *memory, SysBusDevice *dev,
                                   ध्रुव वर्ण *name, hwaddr addr, uint64_t size)
{
    qdev_prop_set_string(DEVICE(dev), "name", name);
    qdev_prop_set_uint64(DEVICE(dev), "size", size);
    sysbus_realize(dev, &error_abort);

    memory_region_add_subregion_overlap(memory, addr,
                                        sysbus_mmio_get_region(dev, 0), -1000);
}

स्थिर शून्य tt_atlantis_machine_done(Notifier *n, शून्य *data)
{
    TTAtlantisState *ams = container_of(n, TTAtlantisState, machine_done);
    TTAtlantisSoCState *s = &ams->soc;
    MachineState *machine = MACHINE(ams);
    hwaddr start_addr = s->memmap[TT_ATL_DDR_LO].base;
    hwaddr mem_size;
    target_ulong firmware_end_addr, kernel_start_addr;
    ध्रुव वर्ण *firmware_name = riscv_default_firmware_name(&s->cpus);
    uint64_t fdt_load_addr;
    uint64_t kernel_entry;
    RISCVBootInfo boot_info;

    /*
     * A user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    सचे (machine->dtb == रिक्त) {
        finalize_fdt(machine->fdt, s);
    }

    mem_size = machine->ram_size;
    सचे (mem_size > s->memmap[TT_ATL_DDR_LO].size) {
        mem_size = s->memmap[TT_ATL_DDR_LO].size;
    }
    riscv_boot_info_init_discontig_mem(&boot_info, &s->cpus,
                                       s->memmap[TT_ATL_DDR_LO].base,
                                       mem_size);

    firmware_end_addr = riscv_find_and_load_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &start_addr, रिक्त);

    kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                     firmware_end_addr);
    सचे (machine->kernel_filename) {
        riscv_load_kernel(machine, &boot_info, kernel_start_addr,
                          true, रिक्त);
        kernel_entry = boot_info.image_नीचः_addr;
    } अञ्ञथा {
        /* If we aren't loading a payload, OpenSBI thinks we are trying to boot
         * address 0, which fails `sbi_domain_check_addr()` as that is where
         * OpenSBI is running. Instead point OpenSBI to the end of the region
         * where it was loaded, which avoids the early hang, allowing the
         * system to proceed with the OpenSBI boot output.
         */
        kernel_entry = kernel_start_addr;
    }

    fdt_load_addr = riscv_compute_fdt_addr(s->memmap[TT_ATL_DDR_LO].base,
                                           s->memmap[TT_ATL_DDR_LO].size,
                                           machine, &boot_info);
    riscv_load_fdt(fdt_load_addr, machine->fdt);

    /* load the reset vector */
    riscv_setup_rom_reset_vec(machine, &s->cpus, start_addr,
                              s->memmap[TT_ATL_BOOTROM].base,
                              s->memmap[TT_ATL_BOOTROM].size,
                              kernel_entry,
                              fdt_load_addr);
}

स्थिर शून्य tt_atlantis_soc_init(Object *obj)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);

    object_initialize_child(obj, "uart1", &s->uart1,
                            TYPE_UNIMPLEMENTED_DEVICE);

    object_initialize_child(obj, "prcm-rcpu",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU],
                            TYPE_TT_ATLANTIS_PRCM_RCPU);
    object_initialize_child(obj, "prcm-hsio",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO],
                            TYPE_TT_ATLANTIS_PRCM_HSIO);
    object_initialize_child(obj, "prcm-pcie",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE],
                            TYPE_TT_ATLANTIS_PRCM_PCIE);
    object_initialize_child(obj, "prcm-mm",
                            &s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM],
                            TYPE_TT_ATLANTIS_PRCM_MM);

    अत्थाय (पूर्णाङ्क i = 0; i < TT_ATL_अङ्कः_I2_C; i++) {
        object_initialize_child(obj, "i2c[*]", &s->i2c[i],
                                TYPE_DESIGNWARE_I2C);
    }
}

स्थिर शून्य tt_atlantis_soc_realize(DeviceState *dev, Error **errp)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(dev);
    ram_addr_t lo_ram_size, ram_size;
    पूर्णाङ्क hart_गणनम् = s->अङ्कः_harts;

    सचे (!s->memory) {
        error_setg(errp, "'memory' link is not set");
        पटिनिवत्त;
    }
    सचे (!s->dram) {
        error_setg(errp, "'dram' link is not set");
        पटिनिवत्त;
    }
    ram_size = memory_region_size(s->dram);

    s->memmap = tt_atlantis_memmap;

    /* CPUs */
    object_property_set_str(OBJECT(&s->cpus), "cpu-type", s->cpu_type,
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), "hartid-base", 0,
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), "num-harts", hart_गणनम्,
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), "resetvec",
                            s->memmap[TT_ATL_BOOTROM].base,
                            &error_abort);
    object_property_set_link(OBJECT(&s->cpus), "memory", OBJECT(s->memory),
                             &error_abort);
    सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        पटिनिवत्त;
    }

    s->irqchip = riscv_create_aia(s->memory,
                                  true, TT_IMSIC_GUESTS,
                                  TT_IMSIC_STRIDE,
                                  TT_IMSIC_STRIDE,
                                  TT_IRQCHIP_अङ्कः_SOURCES,
                                  &s->memmap[TT_ATL_MAPLIC],
                                  &s->memmap[TT_ATL_SAPLIC],
                                  &s->memmap[TT_ATL_MIMSIC],
                                  &s->memmap[TT_ATL_SIMSIC],
                                  0, 0, hart_गणनम्,
                                  TT_IRQCHIP_अङ्कः_MSIS,
                                  TT_IRQCHIP_अङ्कः_PRIO_BITS);

    riscv_aclint_mtimer_create(s->memory,
            s->memmap[TT_ATL_ACLINT].base,
            TT_ACLINT_MTIME_SIZE,
            0, hart_गणनम्,
            TT_ACLINT_MTIMECMP,
            TT_ACLINT_MTIME,
            TT_ACLINT_TIMEBASE_FREQ, true);

    /*
     * DDR
     *
     * The high address is where RAM lives. It is always present and may be
     * up to 64GB. The low address is an alias of the first 2GB of that RAM.
     */
    सचे (ram_size > s->memmap[TT_ATL_DDR_HI].size) {
        g_autofree वर्ण *sz = size_to_str(s->memmap[TT_ATL_DDR_HI].size);
        error_setg(errp, "RAM size is too large, maximum is %s", sz);
        पटिनिवत्त;
    }

    memory_region_init_alias(&s->ram_hi, OBJECT(s), "ram.high", s->dram,
                             0, ram_size);
    memory_region_add_subregion(s->memory,
                                s->memmap[TT_ATL_DDR_HI].base, &s->ram_hi);

    lo_ram_size = MIN(ram_size, s->memmap[TT_ATL_DDR_LO].size);
    memory_region_init_alias(&s->ram_lo, OBJECT(s), "ram.low", s->dram,
                             0, lo_ram_size);
    memory_region_add_subregion(s->memory,
                                s->memmap[TT_ATL_DDR_LO].base, &s->ram_lo);

    /* Boot ROM */
    सचे (!memory_region_init_rom(&s->bootrom, OBJECT(s), "tt-atlantis.bootrom",
                                s->memmap[TT_ATL_BOOTROM].size, errp)) {
        पटिनिवत्त;
    }
    memory_region_add_subregion(s->memory, s->memmap[TT_ATL_BOOTROM].base,
                                &s->bootrom);

    /* UART1, the soc console (UART0 is for the boot microcontroller) */
    serial_mm_init(s->memory, s->memmap[TT_ATL_UART1].base, 2,
                   qdev_get_gpio_in(s->irqchip, TT_ATL_UART1_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* 4 PRCM blocks */
    सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]),
                        errp)) {
        पटिनिवत्त;
    }
    memory_region_add_subregion(
        s->memory, s->memmap[TT_ATL_PRCM_RCPU].base,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]), 0));

    सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]),
                        errp)) {
        पटिनिवत्त;
    }
    memory_region_add_subregion(
        s->memory, s->memmap[TT_ATL_PRCM_HSIO].base,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]), 0));

    सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]),
                        errp)) {
        पटिनिवत्त;
    }
    memory_region_add_subregion(
        s->memory, s->memmap[TT_ATL_PRCM_PCIE].base,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]), 0));

    सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM]),
                        errp)) {
        पटिनिवत्त;
    }
    memory_region_add_subregion(
        s->memory, s->memmap[TT_ATL_PRCM_MM].base,
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
    mmio_map_unimplemented(s->memory, SYS_BUS_DEVICE(&s->uart1),
                           "tt-atlantis.uart1", s->memmap[TT_ATL_UART1].base,
                           s->memmap[TT_ATL_UART1].size);

    /* I2C */
    अत्थाय (पूर्णाङ्क i = 0; i < TT_ATL_अङ्कः_I2_C; i++) {
        SysBusDevice *sbd = SYS_BUS_DEVICE(&s->i2c[i]);

        सचे (!sysbus_realize(sbd, errp)) {
            पटिनिवत्त;
        }
        memory_region_add_subregion(s->memory,
                                    s->memmap[TT_ATL_I2C0 + i].base,
                                    sysbus_mmio_get_region(sbd, 0));
        sysbus_connect_irq(sbd, 0,
                           qdev_get_gpio_in(s->irqchip, TT_ATL_I2C0_IRQ + i));
    }
}

स्थिर ध्रुव Property tt_atlantis_soc_props[] = {
    DEFINE_PROP_STRING("cpu-type", TTAtlantisSoCState, cpu_type),
    DEFINE_PROP_UINT32("num-harts", TTAtlantisSoCState, अङ्कः_harts, 8),
    DEFINE_PROP_LINK("memory", TTAtlantisSoCState, memory,
                     TYPE_MEMORY_REGION, MemoryRegion *),
    DEFINE_PROP_LINK("dram", TTAtlantisSoCState, dram,
                     TYPE_MEMORY_REGION, MemoryRegion *),
};

स्थिर शून्य tt_atlantis_soc_class_init(ObjectClass *oc, ध्रुव शून्य *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = tt_atlantis_soc_realize;
    device_class_set_props(dc, tt_atlantis_soc_props);
    /* The SoC can only be instantiated from the machine */
    dc->user_creatable = false;
}

स्थिर शून्य tt_atlantis_machine_init(MachineState *machine)
{
    TTAtlantisState *ams = TT_ATLANTIS_MACHINE(machine);
    TTAtlantisSoCState *s = &ams->soc;

    memory_region_init(&ams->soc_memory, OBJECT(machine),
                       "tt-atlantis.soc-memory", UINT64_MAX);
    memory_region_add_subregion(get_system_memory(), 0, &ams->soc_memory);

    object_initialize_child(OBJECT(machine), "soc", &ams->soc,
                            TYPE_TT_ATLANTIS_SOC);
    object_property_set_str(OBJECT(&ams->soc), "cpu-type", machine->cpu_type,
                            &error_abort);
    object_property_set_int(OBJECT(&ams->soc), "num-harts", machine->smp.cpus,
                            &error_abort);

    object_property_set_link(OBJECT(&ams->soc), "memory",
                             OBJECT(&ams->soc_memory), &error_abort);
    object_property_set_link(OBJECT(&ams->soc), "dram", OBJECT(machine->ram),
                             &error_abort);

    qdev_realize(DEVICE(&ams->soc), रिक्त, &error_fatal);

    /* I2C peripherals: qemu specific */
    i2c_slave_create_simple(i2c_get_bus(s, 0), "ds1338", 0x6f);
    i2c_slave_create_simple(i2c_get_bus(s, 4), "tmp105", 0x48);

    /* Load or create device tree */
    सचे (machine->dtb) {
        load_fdt(ams);
    } अञ्ञथा {
        create_fdt(ams);
    }

    ams->machine_done.notify = tt_atlantis_machine_done;
    qemu_add_machine_init_done_notifier(&ams->machine_done);
}

स्थिर शून्य tt_atlantis_machine_class_init(ObjectClass *oc, ध्रुव शून्य *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Tenstorrent Atlantis RISC-V SoC (Experimental)";
    mc->init = tt_atlantis_machine_init;
    mc->max_cpus = 8;
    mc->default_cpus = 8;
    mc->default_ram_size = 4 * GiB;
    mc->default_cpu_type = TYPE_RISCV_CPU_TT_ASCALON;
    mc->block_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->default_ram_id = "tt_atlantis.ram";
}

स्थिर ध्रुव TypeInfo tt_atlantis_types[] = {
    {
        .name       = TYPE_TT_ATLANTIS_SOC,
        .parent     = TYPE_DEVICE,
        .instance_size = परिमाण(TTAtlantisSoCState),
        .instance_init = tt_atlantis_soc_init,
        .class_init = tt_atlantis_soc_class_init,
    }, {
        .name       = MACHINE_TYPE_NAME("tt-atlantis"),
        .parent     = TYPE_MACHINE,
        .class_init = tt_atlantis_machine_class_init,
        .instance_size = परिमाण(TTAtlantisState),
    },
};

DEFINE_TYPES(tt_atlantis_types)
