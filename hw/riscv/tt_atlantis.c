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

#परिभाषय TT_IRQCHIP_अङ्कः_MSIS       255
#परिभाषय TT_IRQCHIP_अङ्कः_SOURCES    128
#परिभाषय TT_IRQCHIP_अङ्कः_PRIO_द्वयङ्काः  3
#परिभाषय TT_IMSIC_GUESTS           5
#परिभाषय TT_IMSIC_STRIDE           0x40000 /* Same stride for M and S */
#परिभाषय TT_IMSIC_GUEST_द्वयङ्काः       6

/* Stride is fixed by hardware, check it's consistent with guest bits. */
QEMU_BUILD_BUG_ON(TT_IMSIC_STRIDE != (0x1000 << TT_IMSIC_GUEST_द्वयङ्काः));

#परिभाषय TT_ACLINT_MTIME_आकार    0x8050
#परिभाषय TT_ACLINT_MTIME         0x0
#परिभाषय TT_ACLINT_MTIMECMP      0x8000
#परिभाषय TT_ACLINT_TIMEBASE_FREQ 1000000000

स्थिर ध्रुव Mem_Map_प्रविष्टिः tt_atlantis_memmap[] = {
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

स्थिर I2CBus *i2c_get_bus(TTAtlantisSoCState *s, अचिह्नित busnr)
{
    प्रतिज्ञा(busnr < TT_ATL_अङ्कः_I2_C);

    प्रत्यागम s->i2c[busnr].bus;
}

स्थिर uint32_t fdt_phandle = 1;
स्थिर uint32_t अग्र_phandle(शून्य)
{
    प्रत्यागम fdt_phandle++;
}

स्थिर शून्य create_fdt_स्मृतिः(शून्य *fdt, TTAtlantisSoCState *s)
{
    hwaddr ram_आकार = स्मृतिः_region_आकार(s->dram);
    hwaddr आकार_lo = ram_आकार;
    hwaddr आकार_hi = 0;

    यदि (आकार_lo > s->memmap[TT_ATL_DDR_LO].आकार) {
        आकार_lo = s->memmap[TT_ATL_DDR_LO].आकार;
        आकार_hi = ram_आकार - आकार_lo;
    }

    riscv_create_fdt_socket_स्मृतिः(fdt, s->memmap[TT_ATL_DDR_LO].आधारः,
                                   आकार_lo, 0, false);
    यदि (आकार_hi) {
        /*
         * The first part of the HI address is aliased at the LO address
         * so do not include that as usable memory. Is there any way
         * (or good reason) to describe that aliasing 2GB with DT?
         */
        riscv_create_fdt_socket_स्मृतिः(fdt,
                                       s->memmap[TT_ATL_DDR_HI].आधारः + आकार_lo,
                                       आकार_hi, 0, false);
    }
}

स्थिर शून्य create_fdt_aclint(शून्य *fdt, TTAtlantisSoCState *s,
                              uint32_t *intc_phandles)
{
    g_autofree वर्ण *name = रिक्त;
    g_autofree uint32_t *aclint_mtimer_cells = रिक्त;
    uint32_t aclint_cells_आकार;
    hwaddr addr;

    aclint_mtimer_cells = g_new0(uint32_t, s->cpus.अङ्कः_harts * 2);

    प्रति (पूर्णाङ्क cpu = 0; cpu < s->cpus.अङ्कः_harts; cpu++) {
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_गन्तव्यम्_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_गन्तव्यम्_be32(IRQ_M_TIMER);
    }
    aclint_cells_आकार = s->cpus.अङ्कः_harts * परिमाण(uint32_t) * 2;

    addr = s->memmap[TT_ATL_ACLINT].आधारः;

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIX, addr);
    qemu_fdt_सङ्कलय_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr + TT_ACLINT_MTIME,
                                 2, 0x1000,
                                 2, addr + TT_ACLINT_MTIMECMP,
                                 2, 0x1000);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_आकार);
}

स्थिर शून्य create_fdt_one_imsic(शून्य *fdt, ध्रुव Mem_Map_प्रविष्टिः *mem, पूर्णाङ्क cpus,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 पूर्णाङ्क irq_पङ्क्तिः, uint32_t imsic_guest_द्वयङ्काः)
{
    g_autofree वर्ण *name = रिक्त;
    g_autofree uint32_t *imsic_cells = g_new0(uint32_t, cpus * 2);

    प्रति (पूर्णाङ्क cpu = 0; cpu < cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_गन्तव्यम्_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_गन्तव्यम्_be32(irq_पङ्क्तिः);
    }

    name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->आधारः);
    qemu_fdt_सङ्कलय_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,imsics");

    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", रिक्त, 0);
    qemu_fdt_setprop(fdt, name, "msi-controller", रिक्त, 0);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     imsic_cells, परिमाण(uint32_t) * cpus * 2);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->आधारः, 2, mem->आकार);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-ids", TT_IRQCHIP_अङ्कः_MSIS);

    यदि (imsic_guest_द्वयङ्काः) {
        qemu_fdt_setprop_cell(fdt, name, "riscv,guest-index-bits",
                              imsic_guest_द्वयङ्काः);
    }
    qemu_fdt_setprop_cell(fdt, name, "phandle", msi_phandle);
}

स्थिर शून्य create_fdt_one_aplic(शून्य *fdt,
                                 ध्रुव Mem_Map_प्रविष्टिः *mem,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_child_phandle,
                                 पूर्णाङ्क irq_पङ्क्तिः, पूर्णाङ्क अङ्कः_harts)
{
    g_autofree वर्ण *name =
        g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIX, mem->आधारः);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, अङ्कः_harts * 2);

    प्रति (पूर्णाङ्क cpu = 0; cpu < अङ्कः_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_गन्तव्यम्_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_गन्तव्यम्_be32(irq_पङ्क्तिः);
    }

    qemu_fdt_सङ्कलय_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aplic");
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 2);
    qemu_fdt_setprop(fdt, name, "interrupt-controller", रिक्त, 0);

    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aplic_cells, अङ्कः_harts * परिमाण(uint32_t) * 2);
    qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_phandle);

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->आधारः, 2, mem->आकार);
    qemu_fdt_setprop_cell(fdt, name, "riscv,num-sources",
                          TT_IRQCHIP_अङ्कः_SOURCES);

    यदि (aplic_child_phandle) {
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

    qemu_fdt_सङ्कलय_subnode(fdt, pmu_name);
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
                                 s->cpus.hartid_आधारः, &fdt_phandle,
                                 intc_phandles, false, false);

    create_fdt_aclint(fdt, s, intc_phandles);

    /* M-level IMSIC node */
    uint32_t msi_m_phandle = अग्र_phandle();
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_MIMSIC], अङ्कः_harts,
                         intc_phandles, msi_m_phandle,
                         IRQ_M_EXT, TT_IMSIC_GUEST_द्वयङ्काः);

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, &s->memmap[TT_ATL_SIMSIC], अङ्कः_harts,
                         intc_phandles, imsic_s_phandle,
                         IRQ_S_EXT, TT_IMSIC_GUEST_द्वयङ्काः);

    uint32_t aplic_m_phandle = अग्र_phandle();

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

स्थिर शून्य create_fdt_uart(शून्य *fdt, ध्रुव Mem_Map_प्रविष्टिः *mem, पूर्णाङ्क irq,
                            पूर्णाङ्क irqchip_phandle, uint32_t clk_parent_phandle,
                            uint32_t clk_phandle)
{
    g_autofree वर्ण *name = g_strdup_printf("/soc/serial@%"HWADDR_PRIX,
                                            mem->आधारः);

    qemu_fdt_सङ्कलय_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->आधारः, 2, mem->आकार);
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
    uint8_t rng_बीजम्[32];

    qemu_guest_getrandom_nofail(rng_बीजम्, परिमाण(rng_बीजम्));
    qemu_fdt_setprop(fdt, "/chosen", "rng-seed", rng_बीजम्, परिमाण(rng_बीजम्));
}

स्थिर शून्य create_fdt_clk(शून्य *fdt, ध्रुव वर्ण *clock_name,
                           uint32_t freq, uint32_t phandle)
{
    g_autofree वर्ण *name = g_strdup_printf("/clocks/%s", clock_name);

    qemu_fdt_सङ्कलय_मार्गः(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "fixed-clock");
    qemu_fdt_setprop_string(fdt, name, "clock-output-names", clock_name);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 0);
    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", freq);
    qemu_fdt_setprop_cell(fdt, name, "phandle", phandle);
}

स्थिर शून्य create_fdt_i2c(शून्य *fdt, ध्रुव Mem_Map_प्रविष्टिः *mem, uint32_t irq,
                           uint32_t irqchip_phandle,
                           uint32_t clk_parent_phandle,
                           uint32_t clk_phandle)
{
    g_autofree वर्ण *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX, mem->आधारः);

    qemu_fdt_सङ्कलय_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "snps,designware-i2c");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, mem->आधारः, 2, mem->आकार);
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
    hwaddr आधारः = s->memmap[TT_ATL_I2C0 + bus].आधारः;
    g_autofree वर्ण *name = g_strdup_printf("/soc/i2c@%"HWADDR_PRIX"/sensor@%x",
                                            आधारः, addr);

    qemu_fdt_सङ्कलय_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compat);
    qemu_fdt_setprop_cell(fdt, name, "reg", addr);
}

स्थिर वर्ण *create_fdt_prcm(शून्य *fdt, ध्रुव Mem_Map_प्रविष्टिः *mem,
                             ध्रुव वर्ण *prcm_name)
{
    hwaddr आधारः = mem->आधारः;
    hwaddr आकार = mem->आकार;
    वर्ण *name = g_strdup_printf("/soc/clock-controller@%" PRIx64,
                                 आधारः);
    g_autofree वर्ण *compatible =
        g_strdup_printf("tenstorrent,atlantis-prcm-%s", prcm_name);

    qemu_fdt_सङ्कलय_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", compatible);
    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2, आधारः, 2, आकार);
    qemu_fdt_setprop_cell(fdt, name, "#clock-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "#reset-cells", 1);
    qemu_fdt_setprop_cell(fdt, name, "phandle", अग्र_phandle());

    प्रत्यागम name;
}

स्थिर शून्य finalize_fdt(शून्य *fdt, TTAtlantisSoCState *s)
{
    uint32_t aplic_s_phandle = अग्र_phandle();
    uint32_t imsic_s_phandle = अग्र_phandle();
    uint32_t osc_24m_phandle = अग्र_phandle();
    uint32_t prcm_rcpu_phandle;
    g_autofree वर्ण *rcpu_name, *hsio_name, *pcie_name, *mm_name;

    create_fdt_cpu(fdt, s, aplic_s_phandle, imsic_s_phandle);

    create_fdt_स्मृतिः(fdt, s);

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

    प्रति (पूर्णाङ्क i = 0; i < TT_ATL_अङ्कः_I2_C; i++) {
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
    पूर्णाङ्क fdt_आकार = 0;

    ms->fdt = riscv_create_board_device_वृक्षः("Tenstorrent Atlantis RISC-V Machine",
                                             "tenstorrent,atlantis", &fdt_आकार);

    qemu_fdt_सङ्कलय_subnode(ms->fdt, "/chosen");

    create_fdt_rng(ms->fdt);

    qemu_fdt_सङ्कलय_subnode(ms->fdt, "/aliases");

    create_fdt_pmu(ms->fdt, &ams->soc);
}

स्थिर शून्य आरोपय_fdt(TTAtlantisState *ams)
{
    MachineState *ms = MACHINE(ams);
    वर्ण **ग्रन्थिः_मार्गः;
    Error *err = रिक्त;
    पूर्णाङ्क fdt_आकार = 0;

    ms->fdt = आरोपय_device_वृक्षः(ms->dtb, &fdt_आकार);
    यदि (!ms->fdt) {
        error_report("load_device_tree() failed");
        निर्गच्छ(1);
    }

    qemu_fdt_सङ्कलय_मार्गः(ms->fdt, "/chosen");

    /* Clear memory nodes and update with the specified RAM size */
    ग्रन्थिः_मार्गः = qemu_fdt_ग्रन्थिः_unit_मार्गः(ms->fdt, "memory", &err);
    यदि (err) {
        warn_report_err(err);
    } अन्यथा {
        प्रति (पूर्णाङ्क i = 0; ग्रन्थिः_मार्गः[i]; i++) {
            warn_report("Replacing device tree %s with the requested RAM size",
                        ग्रन्थिः_मार्गः[i]);
            qemu_fdt_nop_ग्रन्थिः(ms->fdt, ग्रन्थिः_मार्गः[i]);
        }
        g_strfreev(ग्रन्थिः_मार्गः);
    }

    create_fdt_स्मृतिः(ms->fdt, &ams->soc);
}

स्थिर शून्य mmio_map_unimplemented(स्मृतिः_Region *स्मृतिः, SysBusDevice *dev,
                                   ध्रुव वर्ण *name, hwaddr addr, uint64_t आकार)
{
    qdev_prop_समुच्चयः_string(DEVICE(dev), "name", name);
    qdev_prop_समुच्चयः_uint64(DEVICE(dev), "size", आकार);
    sysbus_realize(dev, &error_abort);

    स्मृतिः_region_सङ्कलय_subregion_overlap(स्मृतिः, addr,
                                        sysbus_mmio_get_region(dev, 0), -1000);
}

स्थिर शून्य tt_atlantis_machine_समाप्तम्(Notifier *n, शून्य *दत्त)
{
    TTAtlantisState *ams = container_सम्बन्धि(n, TTAtlantisState, machine_समाप्तम्);
    TTAtlantisSoCState *s = &ams->soc;
    MachineState *machine = MACHINE(ams);
    hwaddr start_addr = s->memmap[TT_ATL_DDR_LO].आधारः;
    hwaddr mem_आकार;
    target_ulong firmware_अन्तः_addr, kernel_start_addr;
    ध्रुव वर्ण *firmware_name = riscv_default_firmware_name(&s->cpus);
    uint64_t fdt_आरोपय_addr;
    uint64_t kernel_प्रविष्टिः;
    RISCVBootInfo boot_info;

    /*
     * A user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    यदि (machine->dtb == रिक्त) {
        finalize_fdt(machine->fdt, s);
    }

    mem_आकार = machine->ram_आकार;
    यदि (mem_आकार > s->memmap[TT_ATL_DDR_LO].आकार) {
        mem_आकार = s->memmap[TT_ATL_DDR_LO].आकार;
    }
    riscv_boot_info_init_discontig_mem(&boot_info, &s->cpus,
                                       s->memmap[TT_ATL_DDR_LO].आधारः,
                                       mem_आकार);

    firmware_अन्तः_addr = riscv_find_and_आरोपय_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &start_addr, रिक्त);

    kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                     firmware_अन्तः_addr);
    यदि (machine->kernel_filename) {
        riscv_आरोपय_kernel(machine, &boot_info, kernel_start_addr,
                          true, रिक्त);
        kernel_प्रविष्टिः = boot_info.image_नीचः_addr;
    } अन्यथा {
        /* If we aren't loading a payload, OpenSBI thinks we are trying to boot
         * address 0, which fails `sbi_domain_check_addr()` as that is where
         * OpenSBI is running. Instead point OpenSBI to the end of the region
         * where it was loaded, which avoids the early hang, allowing the
         * system to proceed with the OpenSBI boot output.
         */
        kernel_प्रविष्टिः = kernel_start_addr;
    }

    fdt_आरोपय_addr = riscv_compute_fdt_addr(s->memmap[TT_ATL_DDR_LO].आधारः,
                                           s->memmap[TT_ATL_DDR_LO].आकार,
                                           machine, &boot_info);
    riscv_आरोपय_fdt(fdt_आरोपय_addr, machine->fdt);

    /* load the reset vector */
    riscv_सज्जता_rom_reset_vec(machine, &s->cpus, start_addr,
                              s->memmap[TT_ATL_BOOTROM].आधारः,
                              s->memmap[TT_ATL_BOOTROM].आकार,
                              kernel_प्रविष्टिः,
                              fdt_आरोपय_addr);
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

    प्रति (पूर्णाङ्क i = 0; i < TT_ATL_अङ्कः_I2_C; i++) {
        object_initialize_child(obj, "i2c[*]", &s->i2c[i],
                                TYPE_DESIGNWARE_I2C);
    }
}

स्थिर शून्य tt_atlantis_soc_realize(DeviceState *dev, Error **errp)
{
    TTAtlantisSoCState *s = TT_ATLANTIS_SOC(dev);
    ram_addr_t lo_ram_आकार, ram_आकार;
    पूर्णाङ्क hart_गणनम् = s->अङ्कः_harts;

    यदि (!s->स्मृतिः) {
        error_setg(errp, "'memory' link is not set");
        प्रत्यागम;
    }
    यदि (!s->dram) {
        error_setg(errp, "'dram' link is not set");
        प्रत्यागम;
    }
    ram_आकार = स्मृतिः_region_आकार(s->dram);

    s->memmap = tt_atlantis_memmap;

    /* CPUs */
    object_property_समुच्चयः_str(OBJECT(&s->cpus), "cpu-type", s->cpu_type,
                            &error_abort);
    object_property_समुच्चयः_int(OBJECT(&s->cpus), "hartid-base", 0,
                            &error_abort);
    object_property_समुच्चयः_int(OBJECT(&s->cpus), "num-harts", hart_गणनम्,
                            &error_abort);
    object_property_समुच्चयः_int(OBJECT(&s->cpus), "resetvec",
                            s->memmap[TT_ATL_BOOTROM].आधारः,
                            &error_abort);
    object_property_समुच्चयः_link(OBJECT(&s->cpus), "memory", OBJECT(s->स्मृतिः),
                             &error_abort);
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->cpus), errp)) {
        प्रत्यागम;
    }

    s->irqchip = riscv_create_aia(s->स्मृतिः,
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
                                  TT_IRQCHIP_अङ्कः_PRIO_द्वयङ्काः);

    riscv_aclint_mtimer_create(s->स्मृतिः,
            s->memmap[TT_ATL_ACLINT].आधारः,
            TT_ACLINT_MTIME_आकार,
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
    यदि (ram_आकार > s->memmap[TT_ATL_DDR_HI].आकार) {
        g_autofree वर्ण *sz = आकार_गन्तव्यम्_str(s->memmap[TT_ATL_DDR_HI].आकार);
        error_setg(errp, "RAM size is too large, maximum is %s", sz);
        प्रत्यागम;
    }

    स्मृतिः_region_init_alias(&s->ram_hi, OBJECT(s), "ram.high", s->dram,
                             0, ram_आकार);
    स्मृतिः_region_सङ्कलय_subregion(s->स्मृतिः,
                                s->memmap[TT_ATL_DDR_HI].आधारः, &s->ram_hi);

    lo_ram_आकार = MIN(ram_आकार, s->memmap[TT_ATL_DDR_LO].आकार);
    स्मृतिः_region_init_alias(&s->ram_lo, OBJECT(s), "ram.low", s->dram,
                             0, lo_ram_आकार);
    स्मृतिः_region_सङ्कलय_subregion(s->स्मृतिः,
                                s->memmap[TT_ATL_DDR_LO].आधारः, &s->ram_lo);

    /* Boot ROM */
    यदि (!स्मृतिः_region_init_rom(&s->bootrom, OBJECT(s), "tt-atlantis.bootrom",
                                s->memmap[TT_ATL_BOOTROM].आकार, errp)) {
        प्रत्यागम;
    }
    स्मृतिः_region_सङ्कलय_subregion(s->स्मृतिः, s->memmap[TT_ATL_BOOTROM].आधारः,
                                &s->bootrom);

    /* UART1, the soc console (UART0 is for the boot microcontroller) */
    serial_mm_init(s->स्मृतिः, s->memmap[TT_ATL_UART1].आधारः, 2,
                   qdev_get_gpio_अन्तर्(s->irqchip, TT_ATL_UART1_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* 4 PRCM blocks */
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]),
                        errp)) {
        प्रत्यागम;
    }
    स्मृतिः_region_सङ्कलय_subregion(
        s->स्मृतिः, s->memmap[TT_ATL_PRCM_RCPU].आधारः,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_RCPU]), 0));

    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]),
                        errp)) {
        प्रत्यागम;
    }
    स्मृतिः_region_सङ्कलय_subregion(
        s->स्मृतिः, s->memmap[TT_ATL_PRCM_HSIO].आधारः,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_HSIO]), 0));

    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]),
                        errp)) {
        प्रत्यागम;
    }
    स्मृतिः_region_सङ्कलय_subregion(
        s->स्मृतिः, s->memmap[TT_ATL_PRCM_PCIE].आधारः,
        sysbus_mmio_get_region(
            SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_PCIE]), 0));

    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->prcm[TT_ATLANTIS_PRCM_DOMAIN_MM]),
                        errp)) {
        प्रत्यागम;
    }
    स्मृतिः_region_सङ्कलय_subregion(
        s->स्मृतिः, s->memmap[TT_ATL_PRCM_MM].आधारः,
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
    mmio_map_unimplemented(s->स्मृतिः, SYS_BUS_DEVICE(&s->uart1),
                           "tt-atlantis.uart1", s->memmap[TT_ATL_UART1].आधारः,
                           s->memmap[TT_ATL_UART1].आकार);

    /* I2C */
    प्रति (पूर्णाङ्क i = 0; i < TT_ATL_अङ्कः_I2_C; i++) {
        SysBusDevice *sbd = SYS_BUS_DEVICE(&s->i2c[i]);

        यदि (!sysbus_realize(sbd, errp)) {
            प्रत्यागम;
        }
        स्मृतिः_region_सङ्कलय_subregion(s->स्मृतिः,
                                    s->memmap[TT_ATL_I2C0 + i].आधारः,
                                    sysbus_mmio_get_region(sbd, 0));
        sysbus_connect_irq(sbd, 0,
                           qdev_get_gpio_अन्तर्(s->irqchip, TT_ATL_I2C0_IRQ + i));
    }
}

स्थिर ध्रुव Property tt_atlantis_soc_props[] = {
    DEFINE_PROP_STRING("cpu-type", TTAtlantisSoCState, cpu_type),
    DEFINE_PROP_UINT32("num-harts", TTAtlantisSoCState, अङ्कः_harts, 8),
    DEFINE_PROP_LINK("memory", TTAtlantisSoCState, स्मृतिः,
                     TYPE_स्मृतिः_REGION, स्मृतिः_Region *),
    DEFINE_PROP_LINK("dram", TTAtlantisSoCState, dram,
                     TYPE_स्मृतिः_REGION, स्मृतिः_Region *),
};

स्थिर शून्य tt_atlantis_soc_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = tt_atlantis_soc_realize;
    device_class_समुच्चयः_props(dc, tt_atlantis_soc_props);
    /* The SoC can only be instantiated from the machine */
    dc->user_creatable = false;
}

स्थिर शून्य tt_atlantis_machine_init(MachineState *machine)
{
    TTAtlantisState *ams = TT_ATLANTIS_MACHINE(machine);
    TTAtlantisSoCState *s = &ams->soc;

    स्मृतिः_region_init(&ams->soc_स्मृतिः, OBJECT(machine),
                       "tt-atlantis.soc-memory", UINT64_MAX);
    स्मृतिः_region_सङ्कलय_subregion(get_system_स्मृतिः(), 0, &ams->soc_स्मृतिः);

    object_initialize_child(OBJECT(machine), "soc", &ams->soc,
                            TYPE_TT_ATLANTIS_SOC);
    object_property_समुच्चयः_str(OBJECT(&ams->soc), "cpu-type", machine->cpu_type,
                            &error_abort);
    object_property_समुच्चयः_int(OBJECT(&ams->soc), "num-harts", machine->smp.cpus,
                            &error_abort);

    object_property_समुच्चयः_link(OBJECT(&ams->soc), "memory",
                             OBJECT(&ams->soc_स्मृतिः), &error_abort);
    object_property_समुच्चयः_link(OBJECT(&ams->soc), "dram", OBJECT(machine->ram),
                             &error_abort);

    qdev_realize(DEVICE(&ams->soc), रिक्त, &error_fatal);

    /* I2C peripherals: qemu specific */
    i2c_slave_create_simple(i2c_get_bus(s, 0), "ds1338", 0x6f);
    i2c_slave_create_simple(i2c_get_bus(s, 4), "tmp105", 0x48);

    /* Load or create device tree */
    यदि (machine->dtb) {
        आरोपय_fdt(ams);
    } अन्यथा {
        create_fdt(ams);
    }

    ams->machine_समाप्तम्.notify = tt_atlantis_machine_समाप्तम्;
    qemu_सङ्कलय_machine_init_समाप्तम्_notifier(&ams->machine_समाप्तम्);
}

स्थिर शून्य tt_atlantis_machine_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Tenstorrent Atlantis RISC-V SoC (Experimental)";
    mc->init = tt_atlantis_machine_init;
    mc->max_cpus = 8;
    mc->default_cpus = 8;
    mc->default_ram_आकार = 4 * GiB;
    mc->default_cpu_type = TYPE_RISCV_CPU_TT_ASCALON;
    mc->खण्डम्_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->default_ram_id = "tt_atlantis.ram";
}

स्थिर ध्रुव TypeInfo tt_atlantis_types[] = {
    {
        .name       = TYPE_TT_ATLANTIS_SOC,
        .parent     = TYPE_DEVICE,
        .instance_आकार = परिमाण(TTAtlantisSoCState),
        .instance_init = tt_atlantis_soc_init,
        .class_init = tt_atlantis_soc_class_init,
    }, {
        .name       = MACHINE_TYPE_NAME("tt-atlantis"),
        .parent     = TYPE_MACHINE,
        .class_init = tt_atlantis_machine_class_init,
        .instance_आकार = परिमाण(TTAtlantisState),
    },
};

DEFINE_TYPES(tt_atlantis_types)
