/*
 * QEMU RISC-V Board Compatible with SiFive Freedom U SDK
 *
 * Copyright (c) 2016-2017 Sagar Karandikar, sagark@eecs.berkeley.edu
 * Copyright (c) 2017 SiFive, Inc.
 * Copyright (c) 2019 Bin Meng <bmeng.cn@gmail.com>
 *
 * Provides a board compatible with the SiFive Freedom U SDK:
 *
 * 0) UART
 * 1) CLINT (Core Level Interruptor)
 * 2) PLIC (Platform Level Interrupt Controller)
 * 3) PRCI (Power, Reset, Clock, Interrupt)
 * 4) GPIO (General Purpose Input/Output Controller)
 * 5) OTP (One-Time Programmable) memory with stored serial number
 * 6) GEM (Gigabit Ethernet Controller) and management block
 * 7) DMA (Direct Memory Access Controller)
 * 8) SPI0 connected to an SPI flash
 * 9) SPI2 connected to an SD card
 * 10) PWM0 and PWM1
 *
 * This board currently generates devicetree dynamically that indicates at least
 * two harts and up to five harts.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ضمن "qemu/osdep.h"
#ضمن "qemu/error-report.h"
#ضمن "qapi/error.h"
#ضمن "qapi/visitor.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/irq.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/core/sysbus.h"
#ضمن "hw/cpu/cluster.h"
#ضمن "hw/misc/unimp.h"
#ضمن "hw/sd/sd.h"
#ضمن "hw/ssi/ssi.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "hw/riscv/sifive_u.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/riscv/fdt-common.h"
#ضمن "hw/char/sifive_uart.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/sifive_plic.h"
#ضمن "chardev/char.h"
#ضمن "net/eth.h"
#ضمن "system/device_tree.h"
#ضمن "system/runstate.h"
#ضمن "system/system.h"

#ضمن <libfdt.h>

/* CLINT timebase frequency */
#تعریف_کړه CLINT_TIMEBASE_FREQ 1000000

ساكن ثابت Mem_Map_ثبت sifive_u_memmap[] = {
    [SIFIVE_U_DEV_DEBUG] =    {        0x0,      0x100 },
    [SIFIVE_U_DEV_MROM] =     {     0x1000,     0xf000 },
    [SIFIVE_U_DEV_CLINT] =    {  0x2000000,    0x10000 },
    [SIFIVE_U_DEV_L2CC] =     {  0x2010000,     0x1000 },
    [SIFIVE_U_DEV_PDMA] =     {  0x3000000,   0x100000 },
    [SIFIVE_U_DEV_L2LIM] =    {  0x8000000,  0x2000000 },
    [SIFIVE_U_DEV_PLIC] =     {  0xc000000,  0x4000000 },
    [SIFIVE_U_DEV_PRCI] =     { 0x10000000,     0x1000 },
    [SIFIVE_U_DEV_UART0] =    { 0x10010000,     0x1000 },
    [SIFIVE_U_DEV_UART1] =    { 0x10011000,     0x1000 },
    [SIFIVE_U_DEV_PWM0] =     { 0x10020000,     0x1000 },
    [SIFIVE_U_DEV_PWM1] =     { 0x10021000,     0x1000 },
    [SIFIVE_U_DEV_QSPI0] =    { 0x10040000,     0x1000 },
    [SIFIVE_U_DEV_QSPI2] =    { 0x10050000,     0x1000 },
    [SIFIVE_U_DEV_GPIO] =     { 0x10060000,     0x1000 },
    [SIFIVE_U_DEV_OTP] =      { 0x10070000,     0x1000 },
    [SIFIVE_U_DEV_GEM] =      { 0x10090000,     0x2000 },
    [SIFIVE_U_DEV_GEM_MGMT] = { 0x100a0000,     0x1000 },
    [SIFIVE_U_DEV_DMC] =      { 0x100b0000,    0x10000 },
    [SIFIVE_U_DEV_FLASH0] =   { 0x20000000, 0x10000000 },
    [SIFIVE_U_DEV_DRAM] =     { 0x80000000,        0x0 },
};

#تعریف_کړه OTP_SERIAL          1
#تعریف_کړه GEM_REVISION        0x10070109

ساكن فراغ create_fdt(SiFiveUState *s, ثابت Mem_Map_ثبت *memmap,
                       bool is_32_بټ)
{
    MachineState *ms = MACHINE(s);
    فراغ *fdt;
    صحيح cpu;
    uint32_t *cells, cells_اوږدتیا;
    حرف *nodename;
    uint32_t plic_phandle, prci_phandle, gpio_phandle, phandle = 1;
    uint32_t hfclk_phandle, rtcclk_phandle, phy_phandle;
    ساكن ثابت حرف * ثابت ethclk_نومونه[2] = { "pclk", "hclk" };
    g_autofree uint32_t *intc_phandles = g_new0(uint32_t, ms->smp.cpus);
    g_autofree حرف *clust_name = عدم;

    fdt = ms->fdt = riscv_create_board_device_ونه(
        "SiFive HiFive Unleashed A00",
        "sifive,hifive-unleashed-a00",
        &s->fdt_اندازه);

    hfclk_phandle = phandle++;
    nodename = g_strdup_printf("/hfclk");
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", hfclk_phandle);
    qemu_fdt_setprop_string(fdt, nodename, "clock-output-names", "hfclk");
    qemu_fdt_setprop_cell(fdt, nodename, "clock-frequency",
        SIFIVE_U_HFCLK_FREQ);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "fixed-clock");
    qemu_fdt_setprop_cell(fdt, nodename, "#clock-cells", 0x0);
    g_free(nodename);

    rtcclk_phandle = phandle++;
    nodename = g_strdup_printf("/rtcclk");
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", rtcclk_phandle);
    qemu_fdt_setprop_string(fdt, nodename, "clock-output-names", "rtcclk");
    qemu_fdt_setprop_cell(fdt, nodename, "clock-frequency",
        SIFIVE_U_RTCCLK_FREQ);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "fixed-clock");
    qemu_fdt_setprop_cell(fdt, nodename, "#clock-cells", 0x0);
    g_free(nodename);

    riscv_create_fdt_socket_حافظه(fdt, memmap[SIFIVE_U_DEV_DRAM].بنسټ,
                                   ms->ram_اندازه, 0, false);

    riscv_fdt_create_cpu_socket_subnode(fdt, CLINT_TIMEBASE_FREQ);

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", 0);
    qemu_fdt_جمع_کړه_subnode(fdt, clust_name);

    لكل (cpu = ms->smp.cpus - 1; cpu >= 0; cpu--) {
        nodename = g_strdup_printf("/cpus/cpu@%d", cpu);
        qemu_fdt_جمع_کړه_subnode(fdt, nodename);

        /* cpu 0 is the management hart that does not have mmu */
        إذا (cpu != 0) {
            إذا (is_32_بټ) {
                qemu_fdt_setprop_string(fdt, nodename, "mmu-type", "riscv,sv32");
            } وإلا {
                qemu_fdt_setprop_string(fdt, nodename, "mmu-type", "riscv,sv39");
            }
            riscv_isa_write_fdt(&s->soc.u_cpus.harts[cpu - 1], fdt, nodename);
        } وإلا {
            riscv_isa_write_fdt(&s->soc.e_cpus.harts[0], fdt, nodename);
        }

        riscv_create_fdt_socket_cpu_sifive(fdt, clust_name, cpu, 0, 0,
                                           &phandle, intc_phandles);

        g_free(nodename);
    }

    riscv_create_fdt_socket_clint(fdt, memmap[SIFIVE_U_DEV_CLINT].بنسټ,
                                  memmap[SIFIVE_U_DEV_CLINT].اندازه, 0,
                                  intc_phandles, ms->smp.cpus, false);

    nodename = g_strdup_printf("/soc/otp@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_OTP].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "fuse-count", SIFIVE_U_OTP_REG_اندازه);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_OTP].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_OTP].اندازه);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
        "sifive,fu540-c000-otp");
    g_free(nodename);

    prci_phandle = phandle++;
    nodename = g_strdup_printf("/soc/clock-controller@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_PRCI].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", prci_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "#clock-cells", 0x1);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        hfclk_phandle, rtcclk_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_PRCI].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_PRCI].اندازه);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
        "sifive,fu540-c000-prci");
    g_free(nodename);

    plic_phandle = phandle++;
    cells_اوږدتیا = ms->smp.cpus * 4 - 2;
    cells =  g_new0(uint32_t, cells_اوږدتیا);
    لكل (cpu = 0; cpu < ms->smp.cpus; cpu++) {
        /* cpu 0 is the management hart that does not have S-mode */
        إذا (cpu == 0) {
            cells[0] = cpu_منزل_be32(intc_phandles[cpu]);
            cells[1] = cpu_منزل_be32(IRQ_M_EXT);
        } وإلا {
            cells[cpu * 4 - 2] = cpu_منزل_be32(intc_phandles[cpu]);
            cells[cpu * 4 - 1] = cpu_منزل_be32(IRQ_M_EXT);
            cells[cpu * 4 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
            cells[cpu * 4 + 1] = cpu_منزل_be32(IRQ_S_EXT);
        }
    }

    riscv_create_fdt_plic(fdt, memmap[SIFIVE_U_DEV_PLIC].بنسټ,
                          memmap[SIFIVE_U_DEV_PLIC].اندازه,
                          plic_phandle, SIFIVE_U_PLIC_INT_CELLS,
                          SIFIVE_U_PLIC_ADDR_CELLS, cells,
                          cells_اوږدتیا * حجم(uint32_t),
                          SIFIVE_U_PLIC_عدد_SOURCES - 1, false, 0);
    g_free(cells);

    gpio_phandle = phandle++;
    nodename = g_strdup_printf("/soc/gpio@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_GPIO].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", gpio_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", 2);
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", عدم, 0);
    qemu_fdt_setprop_cell(fdt, nodename, "#gpio-cells", 2);
    qemu_fdt_setprop(fdt, nodename, "gpio-controller", عدم, 0);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_GPIO].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_GPIO].اندازه);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts", SIFIVE_U_GPIO_IRQ0,
        SIFIVE_U_GPIO_IRQ1, SIFIVE_U_GPIO_IRQ2, SIFIVE_U_GPIO_IRQ3,
        SIFIVE_U_GPIO_IRQ4, SIFIVE_U_GPIO_IRQ5, SIFIVE_U_GPIO_IRQ6,
        SIFIVE_U_GPIO_IRQ7, SIFIVE_U_GPIO_IRQ8, SIFIVE_U_GPIO_IRQ9,
        SIFIVE_U_GPIO_IRQ10, SIFIVE_U_GPIO_IRQ11, SIFIVE_U_GPIO_IRQ12,
        SIFIVE_U_GPIO_IRQ13, SIFIVE_U_GPIO_IRQ14, SIFIVE_U_GPIO_IRQ15);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,gpio0");
    g_free(nodename);

    nodename = g_strdup_printf("/gpio-restart");
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cells(fdt, nodename, "gpios", gpio_phandle, 10, 1);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "gpio-restart");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/dma@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_PDMA].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#dma-cells", 1);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
        SIFIVE_U_PDMA_IRQ0, SIFIVE_U_PDMA_IRQ1, SIFIVE_U_PDMA_IRQ2,
        SIFIVE_U_PDMA_IRQ3, SIFIVE_U_PDMA_IRQ4, SIFIVE_U_PDMA_IRQ5,
        SIFIVE_U_PDMA_IRQ6, SIFIVE_U_PDMA_IRQ7);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_PDMA].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_PDMA].اندازه);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
                            "sifive,fu540-c000-pdma");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/cache-controller@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_L2CC].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_L2CC].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_L2CC].اندازه);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
        SIFIVE_U_L2CC_IRQ0, SIFIVE_U_L2CC_IRQ1, SIFIVE_U_L2CC_IRQ2);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop(fdt, nodename, "cache-unified", عدم, 0);
    qemu_fdt_setprop_cell(fdt, nodename, "cache-size", 2097152);
    qemu_fdt_setprop_cell(fdt, nodename, "cache-sets", 1024);
    qemu_fdt_setprop_cell(fdt, nodename, "cache-level", 2);
    qemu_fdt_setprop_cell(fdt, nodename, "cache-block-size", 64);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
                            "sifive,fu540-c000-ccache");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/spi@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_QSPI2].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#size-cells", 0);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", 1);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_QSPI2_IRQ);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_QSPI2].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_QSPI2].اندازه);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,spi0");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/spi@%lx/mmc@0",
        (اوږد)memmap[SIFIVE_U_DEV_QSPI2].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop(fdt, nodename, "disable-wp", عدم, 0);
    qemu_fdt_setprop_cells(fdt, nodename, "voltage-ranges", 3300, 3300);
    qemu_fdt_setprop_cell(fdt, nodename, "spi-max-frequency", 20000000);
    qemu_fdt_setprop_cell(fdt, nodename, "reg", 0);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "mmc-spi-slot");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/spi@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_QSPI0].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#size-cells", 0);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", 1);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_QSPI0_IRQ);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_QSPI0].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_QSPI0].اندازه);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,spi0");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/spi@%lx/flash@0",
        (اوږد)memmap[SIFIVE_U_DEV_QSPI0].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "spi-rx-bus-width", 4);
    qemu_fdt_setprop_cell(fdt, nodename, "spi-tx-bus-width", 4);
    qemu_fdt_setprop(fdt, nodename, "m25p,fast-read", عدم, 0);
    qemu_fdt_setprop_cell(fdt, nodename, "spi-max-frequency", 50000000);
    qemu_fdt_setprop_cell(fdt, nodename, "reg", 0);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "jedec,spi-nor");
    g_free(nodename);

    phy_phandle = phandle++;
    nodename = g_strdup_printf("/soc/ethernet@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_GEM].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
        "sifive,fu540-c000-gem");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_GEM].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_GEM].اندازه,
        0x0, memmap[SIFIVE_U_DEV_GEM_MGMT].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_GEM_MGMT].اندازه);
    qemu_fdt_setprop_string(fdt, nodename, "reg-names", "control");
    qemu_fdt_setprop_string(fdt, nodename, "phy-mode", "gmii");
    qemu_fdt_setprop_cell(fdt, nodename, "phy-handle", phy_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_GEM_IRQ);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_GEMGXLPLL, prci_phandle, PRCI_CLK_GEMGXLPLL);
    qemu_fdt_setprop_string_array(fdt, nodename, "clock-names",
        (حرف **)&ethclk_نومونه, ARRAY_اندازه(ethclk_نومونه));
    qemu_fdt_setprop(fdt, nodename, "local-mac-address",
        s->soc.gem.conf.macaddr.a, ETH_ALEN);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", 1);
    qemu_fdt_setprop_cell(fdt, nodename, "#size-cells", 0);

    qemu_fdt_جمع_کړه_subnode(fdt, "/aliases");
    qemu_fdt_setprop_string(fdt, "/aliases", "ethernet0", nodename);

    g_free(nodename);

    nodename = g_strdup_printf("/soc/ethernet@%lx/ethernet-phy@0",
        (اوږد)memmap[SIFIVE_U_DEV_GEM].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", phy_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "reg", 0x0);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/pwm@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_PWM0].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,pwm0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_PWM0].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_PWM0].اندازه);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
                           SIFIVE_U_PWM0_IRQ0, SIFIVE_U_PWM0_IRQ1,
                           SIFIVE_U_PWM0_IRQ2, SIFIVE_U_PWM0_IRQ3);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
                           prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "#pwm-cells", 0);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/pwm@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_PWM1].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,pwm0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_PWM1].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_PWM1].اندازه);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
                           SIFIVE_U_PWM1_IRQ0, SIFIVE_U_PWM1_IRQ1,
                           SIFIVE_U_PWM1_IRQ2, SIFIVE_U_PWM1_IRQ3);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
                           prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "#pwm-cells", 0);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/serial@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_UART1].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,uart0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_UART1].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_UART1].اندازه);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_UART1_IRQ);

    qemu_fdt_setprop_string(fdt, "/aliases", "serial1", nodename);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/serial@%lx",
        (اوږد)memmap[SIFIVE_U_DEV_UART0].بنسټ);
    qemu_fdt_جمع_کړه_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,uart0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_UART0].بنسټ,
        0x0, memmap[SIFIVE_U_DEV_UART0].اندازه);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_UART0_IRQ);

    qemu_fdt_جمع_کړه_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", nodename);
    qemu_fdt_setprop_string(fdt, "/aliases", "serial0", nodename);

    g_free(nodename);
}

ساكن فراغ sifive_u_machine_reset(فراغ *opaque, صحيح n, صحيح level)
{
    /* gpio pin active low triggers reset */
    إذا (!level) {
        qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
    }
}

ساكن فراغ sifive_u_machine_init(MachineState *machine)
{
    ثابت Mem_Map_ثبت *memmap = sifive_u_memmap;
    SiFiveUState *s = RISCV_U_MACHINE(machine);
    حافظه_Region *system_حافظه = get_system_حافظه();
    حافظه_Region *flash0 = g_new(حافظه_Region, 1);
    hwaddr پیل_addr = memmap[SIFIVE_U_DEV_DRAM].بنسټ;
    hwaddr firmware_پای_ټکی_addr;
    vaddr kernel_پیل_addr;
    ثابت حرف *firmware_name;
    uint32_t پیل_addr_hi32 = 0x00000000;
    uint32_t fdt_بارول_addr_hi32 = 0x00000000;
    صحيح i;
    uint64_t fdt_بارول_addr;
    uint64_t kernel_ثبت;
    DriveInfo *dinfo;
    بلاک_Backend *blk;
    DeviceState *flash_dev, *sd_dev, *card_dev;
    qemu_irq flash_cs, sd_cs;
    RISCVBootInfo boot_info;

    /* Initialize SoC */
    object_initialize_ماشوم(OBJECT(machine), "soc", &s->soc, TYPE_RISCV_U_SOC);
    object_property_سېټ_uint(OBJECT(&s->soc), "serial", s->serial,
                             &error_abort);
    object_property_سېټ_str(OBJECT(&s->soc), "cpu-type", machine->cpu_type,
                             &error_abort);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    /* register RAM */
    حافظه_region_جمع_کړه_subregion(system_حافظه, memmap[SIFIVE_U_DEV_DRAM].بنسټ,
                                machine->ram);

    /* register QSPI0 Flash */
    حافظه_region_init_ram(flash0, عدم, "riscv.sifive.u.flash0",
                           memmap[SIFIVE_U_DEV_FLASH0].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(system_حافظه, memmap[SIFIVE_U_DEV_FLASH0].بنسټ,
                                flash0);

    /* register gpio-restart */
    qdev_connect_gpio_بهر(DEVICE(&(s->soc.gpio)), 10,
                          qemu_allocate_irq(sifive_u_machine_reset, عدم, 0));

    /* load/create device tree */
    إذا (machine->dtb) {
        machine->fdt = بارول_device_ونه(machine->dtb, &s->fdt_اندازه);
        إذا (!machine->fdt) {
            error_report("load_device_tree() failed");
            وتل(1);
        }
    } وإلا {
        create_fdt(s, memmap, riscv_is_32bit(&s->soc.u_cpus));
    }

    إذا (s->پیل_دننه_flash) {
        /*
         * If start_in_flash property is given, assign s->msel to a value
         * that representing booting from QSPI0 memory-mapped flash.
         *
         * This also means that when both start_in_flash and msel properties
         * are given, start_in_flash takes the precedence over msel.
         *
         * Note this is to keep backward compatibility not to break existing
         * users that use start_in_flash property.
         */
        s->msel = MSEL_MEMMAP_QSPI0_FLASH;
    }

    بدل (s->msel) {
    حالة MSEL_MEMMAP_QSPI0_FLASH:
        پیل_addr = memmap[SIFIVE_U_DEV_FLASH0].بنسټ;
        اكسر;
    حالة MSEL_L2LIM_QSPI0_FLASH:
    حالة MSEL_L2LIM_QSPI2_SD:
        پیل_addr = memmap[SIFIVE_U_DEV_L2LIM].بنسټ;
        اكسر;
    افتراضي:
        پیل_addr = memmap[SIFIVE_U_DEV_DRAM].بنسټ;
        اكسر;
    }

    riscv_boot_info_init(&boot_info, &s->soc.u_cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.u_cpus);
    firmware_پای_ټکی_addr = riscv_پیدا_کړه_and_بارول_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &پیل_addr, عدم);

    إذا (machine->kernel_filename) {
        kernel_پیل_addr = riscv_calc_kernel_پیل_addr(&boot_info,
                                                         firmware_پای_ټکی_addr);
        riscv_بارول_kernel(machine, &boot_info, kernel_پیل_addr,
                          true, عدم);
        kernel_ثبت = boot_info.image_ټیټ_addr;
    } وإلا {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_ثبت = 0;
    }

    fdt_بارول_addr = riscv_compute_fdt_addr(memmap[SIFIVE_U_DEV_DRAM].بنسټ,
                                           memmap[SIFIVE_U_DEV_DRAM].اندازه,
                                           machine, &boot_info);
    riscv_بارول_fdt(fdt_بارول_addr, machine->fdt);

    إذا (!riscv_is_32bit(&s->soc.u_cpus)) {
        پیل_addr_hi32 = (uint64_t)پیل_addr >> 32;
        fdt_بارول_addr_hi32 = fdt_بارول_addr >> 32;
    }

    /* reset vector */
    uint32_t reset_vec[12] = {
        s->msel,                       /* MSEL pin state */
        0x00000297,                    /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
        0x02c28613,                    /*     addi   a2, t0, %pcrel_lo(1b) */
        0xf1402573,                    /*     csrr   a0, mhartid  */
        0,
        0,
        0x00028067,                    /*     jr     t0 */
        0x00000000,                    /* padding for alignment */
        پیل_addr,                    /* start: .dword */
        پیل_addr_hi32,
        fdt_بارول_addr,                 /* fdt_laddr: .dword */
        fdt_بارول_addr_hi32,
                                       /* fw_dyn: */
    };

    إذا (riscv_is_32bit(&s->soc.u_cpus)) {
        reset_vec[4] = 0x0242a583;     /*     lw     a1, 36(t0) */
        reset_vec[5] = 0x01c2a283;     /*     lw     t0, 28(t0) */
    } وإلا {
        reset_vec[4] = 0x0242b583;     /*     ld     a1, 36(t0) */
        reset_vec[5] = 0x01c2b283;     /*     ld     t0, 28(t0) */
    }


    /* copy in the reset vector in little_endian byte order */
    لكل (i = 0; i < ARRAY_اندازه(reset_vec); i++) {
        reset_vec[i] = cpu_منزل_le32(reset_vec[i]);
    }
    rom_جمع_کړه_blob_fixed_as("mrom.reset", reset_vec, حجم(reset_vec),
                          memmap[SIFIVE_U_DEV_MROM].بنسټ, &پته_space_حافظه);

    riscv_rom_copy_firmware_info(machine, &s->soc.u_cpus,
                                 memmap[SIFIVE_U_DEV_MROM].بنسټ,
                                 memmap[SIFIVE_U_DEV_MROM].اندازه,
                                 حجم(reset_vec), kernel_ثبت);

    /* Connect an SPI flash to SPI0 */
    flash_dev = qdev_new("is25wp256");
    dinfo = drive_get(IF_MTD, 0, 0);
    إذا (dinfo) {
        qdev_prop_سېټ_drive_err(flash_dev, "drive",
                                blk_by_legacy_dinfo(dinfo),
                                &error_fatal);
    }
    qdev_realize_and_unref(flash_dev, BUS(s->soc.spi0.spi), &error_fatal);

    flash_cs = qdev_get_gpio_دننه_named(flash_dev, SSI_GPIO_CS, 0);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->soc.spi0), 1, flash_cs);

    /* Connect an SD card to SPI2 */
    sd_dev = ssi_create_peripheral(s->soc.spi2.spi, "ssi-sd");

    sd_cs = qdev_get_gpio_دننه_named(sd_dev, SSI_GPIO_CS, 0);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->soc.spi2), 1, sd_cs);

    dinfo = drive_get(IF_SD, 0, 0);
    blk = dinfo ? blk_by_legacy_dinfo(dinfo) : عدم;
    card_dev = qdev_new(TYPE_SD_CARD_SPI);
    qdev_prop_سېټ_drive_err(card_dev, "drive", blk, &error_fatal);
    qdev_realize_and_unref(card_dev,
                           qdev_get_ماشوم_bus(sd_dev, "sd-bus"),
                           &error_fatal);
}

ساكن bool sifive_u_machine_get_پیل_دننه_flash(Object *obj, Error **errp)
{
    SiFiveUState *s = RISCV_U_MACHINE(obj);

    أرجع s->پیل_دننه_flash;
}

ساكن فراغ sifive_u_machine_سېټ_پیل_دننه_flash(Object *obj, bool ارزښت, Error **errp)
{
    SiFiveUState *s = RISCV_U_MACHINE(obj);

    s->پیل_دننه_flash = ارزښت;
}

ساكن فراغ sifive_u_machine_instance_init(Object *obj)
{
    SiFiveUState *s = RISCV_U_MACHINE(obj);

    s->پیل_دننه_flash = false;
    s->msel = 0;
    object_property_جمع_کړه_uint32_ptr(obj, "msel", &s->msel,
                                   OBJ_PROP_FLAG_READWRITE);
    object_property_سېټ_description(obj, "msel",
                                    "Mode Select (MSEL[3:0]) pin state");

    s->serial = OTP_SERIAL;
    object_property_جمع_کړه_uint32_ptr(obj, "serial", &s->serial,
                                   OBJ_PROP_FLAG_READWRITE);
    object_property_سېټ_description(obj, "serial", "Board serial number");
}

ساكن فراغ sifive_u_machine_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with SiFive U SDK";
    mc->init = sifive_u_machine_init;
    mc->max_cpus = SIFIVE_U_MANAGEMENT_CPU_شمېر + SIFIVE_U_COMPUTE_CPU_شمېر;
    mc->min_cpus = SIFIVE_U_MANAGEMENT_CPU_شمېر + 1;
    mc->default_cpu_type = SIFIVE_U_CPU;
    mc->default_cpus = mc->min_cpus;
    mc->default_ram_id = "riscv.sifive.u.ram";
    mc->auto_create_sdcard = true;

    object_class_property_جمع_کړه_bool(oc, "start-in-flash",
                                   sifive_u_machine_get_پیل_دننه_flash,
                                   sifive_u_machine_سېټ_پیل_دننه_flash);
    object_class_property_سېټ_description(oc, "start-in-flash",
                                          "Set on to tell QEMU's ROM to jump to "
                                          "flash. Otherwise QEMU will jump to DRAM "
                                          "or L2LIM depending on the msel value");
}

ساكن ثابت TypeInfo sifive_u_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("sifive_u"),
    .پلار     = TYPE_MACHINE,
    .class_init = sifive_u_machine_class_init,
    .instance_init = sifive_u_machine_instance_init,
    .instance_اندازه = حجم(SiFiveUState),
};

ساكن فراغ sifive_u_machine_init_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&sifive_u_machine_typeinfo);
}

type_init(sifive_u_machine_init_ثبتوونکی_types)

ساكن فراغ sifive_u_soc_instance_init(Object *obj)
{
    SiFiveUSoCState *s = RISCV_U_SOC(obj);

    object_initialize_ماشوم(obj, "e-cluster", &s->e_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_سېټ_uint32(DEVICE(&s->e_cluster), "cluster-id", 0);

    object_initialize_ماشوم(OBJECT(&s->e_cluster), "e-cpus", &s->e_cpus,
                            TYPE_RISCV_HART_ARRAY);
    qdev_prop_سېټ_uint32(DEVICE(&s->e_cpus), "num-harts", 1);
    qdev_prop_سېټ_uint32(DEVICE(&s->e_cpus), "hartid-base", 0);
    qdev_prop_سېټ_string(DEVICE(&s->e_cpus), "cpu-type", SIFIVE_E_CPU);
    qdev_prop_سېټ_uint64(DEVICE(&s->e_cpus), "resetvec", 0x1004);

    object_initialize_ماشوم(obj, "u-cluster", &s->u_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_سېټ_uint32(DEVICE(&s->u_cluster), "cluster-id", 1);

    object_initialize_ماشوم(OBJECT(&s->u_cluster), "u-cpus", &s->u_cpus,
                            TYPE_RISCV_HART_ARRAY);

    object_initialize_ماشوم(obj, "prci", &s->prci, TYPE_SIFIVE_U_PRCI);
    object_initialize_ماشوم(obj, "otp", &s->otp, TYPE_SIFIVE_U_OTP);
    object_initialize_ماشوم(obj, "gem", &s->gem, TYPE_CADENCE_GEM);
    object_initialize_ماشوم(obj, "gpio", &s->gpio, TYPE_SIFIVE_GPIO);
    object_initialize_ماشوم(obj, "pdma", &s->dma, TYPE_SIFIVE_PDMA);
    object_initialize_ماشوم(obj, "spi0", &s->spi0, TYPE_SIFIVE_SPI);
    object_initialize_ماشوم(obj, "spi2", &s->spi2, TYPE_SIFIVE_SPI);
    object_initialize_ماشوم(obj, "pwm0", &s->pwm[0], TYPE_SIFIVE_PWM);
    object_initialize_ماشوم(obj, "pwm1", &s->pwm[1], TYPE_SIFIVE_PWM);
}

ساكن فراغ sifive_u_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    SiFiveUSoCState *s = RISCV_U_SOC(dev);
    ثابت Mem_Map_ثبت *memmap = sifive_u_memmap;
    حافظه_Region *system_حافظه = get_system_حافظه();
    حافظه_Region *ماسک_rom = g_new(حافظه_Region, 1);
    حافظه_Region *l2lim_mem = g_new(حافظه_Region, 1);
    حرف *plic_hart_config;
    صحيح i, j;

    qdev_prop_سېټ_uint32(DEVICE(&s->u_cpus), "num-harts", ms->smp.cpus - 1);
    qdev_prop_سېټ_uint32(DEVICE(&s->u_cpus), "hartid-base", 1);
    qdev_prop_سېټ_string(DEVICE(&s->u_cpus), "cpu-type", s->cpu_type);
    qdev_prop_سېټ_uint64(DEVICE(&s->u_cpus), "resetvec", 0x1004);

    sysbus_realize(SYS_BUS_DEVICE(&s->e_cpus), &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->u_cpus), &error_fatal);
    /*
     * The cluster must be realized after the RISC-V hart array container,
     * as the container's CPU object is only created on realize, and the
     * CPU must exist and have been parented into the cluster before the
     * cluster is realized.
     */
    qdev_realize(DEVICE(&s->e_cluster), عدم, &error_abort);
    qdev_realize(DEVICE(&s->u_cluster), عدم, &error_abort);

    /* boot rom */
    حافظه_region_init_rom(ماسک_rom, OBJECT(dev), "riscv.sifive.u.mrom",
                           memmap[SIFIVE_U_DEV_MROM].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(system_حافظه, memmap[SIFIVE_U_DEV_MROM].بنسټ,
                                ماسک_rom);

    /*
     * Add L2-LIM at reset size.
     * This should be reduced in size as the L2 Cache Controller WayEnable
     * register is incremented. Unfortunately I don't see a nice (or any) way
     * to handle reducing or blocking out the L2 LIM while still allowing it
     * be re returned to all enabled after a reset. For the time being, just
     * leave it enabled all the time. This won't break anything, but will be
     * too generous to misbehaving guests.
     */
    حافظه_region_init_ram(l2lim_mem, عدم, "riscv.sifive.u.l2lim",
                           memmap[SIFIVE_U_DEV_L2LIM].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(system_حافظه, memmap[SIFIVE_U_DEV_L2LIM].بنسټ,
                                l2lim_mem);

    /* create PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(ms->smp.cpus);

    /* MMIO */
    s->plic = sifive_plic_create(system_حافظه,
        memmap[SIFIVE_U_DEV_PLIC].بنسټ,
        plic_hart_config, ms->smp.cpus, 0,
        SIFIVE_U_PLIC_عدد_SOURCES,
        SIFIVE_U_PLIC_عدد_PRIORITIES,
        SIFIVE_U_PLIC_لومړیتوب_بنسټ,
        SIFIVE_U_PLIC_PENDING_بنسټ,
        SIFIVE_U_PLIC_ENABLE_بنسټ,
        SIFIVE_U_PLIC_ENABLE_STRIDE,
        SIFIVE_U_PLIC_CONTEXT_بنسټ,
        SIFIVE_U_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_U_DEV_PLIC].اندازه);
    g_free(plic_hart_config);
    sifive_uart_create(system_حافظه, memmap[SIFIVE_U_DEV_UART0].بنسټ,
        serial_hd(0), qdev_get_gpio_دننه(DEVICE(s->plic), SIFIVE_U_UART0_IRQ));
    sifive_uart_create(system_حافظه, memmap[SIFIVE_U_DEV_UART1].بنسټ,
        serial_hd(1), qdev_get_gpio_دننه(DEVICE(s->plic), SIFIVE_U_UART1_IRQ));
    riscv_aclint_swi_create(system_حافظه,
        memmap[SIFIVE_U_DEV_CLINT].بنسټ, 0,
        ms->smp.cpus, false);
    riscv_aclint_mtimer_create(system_حافظه,
        memmap[SIFIVE_U_DEV_CLINT].بنسټ + RISCV_ACLINT_SWI_اندازه,
        RISCV_ACLINT_DEFAULT_MTIMER_اندازه, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        CLINT_TIMEBASE_FREQ, false);

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->prci), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->prci), 0, memmap[SIFIVE_U_DEV_PRCI].بنسټ);

    qdev_prop_سېټ_uint32(DEVICE(&s->gpio), "ngpio", 16);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_U_DEV_GPIO].بنسټ);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, عدم);

    /* Connect GPIO interrupts to the PLIC */
    لكل (i = 0; i < 16; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i,
                           qdev_get_gpio_دننه(DEVICE(s->plic),
                                            SIFIVE_U_GPIO_IRQ0 + i));
    }

    /* PDMA */
    sysbus_realize(SYS_BUS_DEVICE(&s->dma), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->dma), 0, memmap[SIFIVE_U_DEV_PDMA].بنسټ);

    /* Connect PDMA interrupts to the PLIC */
    لكل (i = 0; i < SIFIVE_PDMA_IRQS; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->dma), i,
                           qdev_get_gpio_دننه(DEVICE(s->plic),
                                            SIFIVE_U_PDMA_IRQ0 + i));
    }

    qdev_prop_سېټ_uint32(DEVICE(&s->otp), "serial", s->serial);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->otp), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->otp), 0, memmap[SIFIVE_U_DEV_OTP].بنسټ);

    qemu_configure_nic_device(DEVICE(&s->gem), true, عدم);
    object_property_سېټ_int(OBJECT(&s->gem), "revision", GEM_REVISION,
                            &error_abort);
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->gem), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem), 0, memmap[SIFIVE_U_DEV_GEM].بنسټ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem), 0,
                       qdev_get_gpio_دننه(DEVICE(s->plic), SIFIVE_U_GEM_IRQ));

    /* PWM */
    لكل (i = 0; i < 2; i++) {
        إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->pwm[i]), errp)) {
            أرجع;
        }
        sysbus_mmio_map(SYS_BUS_DEVICE(&s->pwm[i]), 0,
                                memmap[SIFIVE_U_DEV_PWM0].بنسټ + (0x1000 * i));

        /* Connect PWM interrupts to the PLIC */
        لكل (j = 0; j < SIFIVE_PWM_IRQS; j++) {
            sysbus_connect_irq(SYS_BUS_DEVICE(&s->pwm[i]), j,
                               qdev_get_gpio_دننه(DEVICE(s->plic),
                                        SIFIVE_U_PWM0_IRQ0 + (i * 4) + j));
        }
    }

    create_unimplemented_device("riscv.sifive.u.gem-mgmt",
        memmap[SIFIVE_U_DEV_GEM_MGMT].بنسټ, memmap[SIFIVE_U_DEV_GEM_MGMT].اندازه);

    create_unimplemented_device("riscv.sifive.u.dmc",
        memmap[SIFIVE_U_DEV_DMC].بنسټ, memmap[SIFIVE_U_DEV_DMC].اندازه);

    create_unimplemented_device("riscv.sifive.u.l2cc",
        memmap[SIFIVE_U_DEV_L2CC].بنسټ, memmap[SIFIVE_U_DEV_L2CC].اندازه);

    sysbus_realize(SYS_BUS_DEVICE(&s->spi0), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->spi0), 0,
                    memmap[SIFIVE_U_DEV_QSPI0].بنسټ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->spi0), 0,
                       qdev_get_gpio_دننه(DEVICE(s->plic), SIFIVE_U_QSPI0_IRQ));
    sysbus_realize(SYS_BUS_DEVICE(&s->spi2), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->spi2), 0,
                    memmap[SIFIVE_U_DEV_QSPI2].بنسټ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->spi2), 0,
                       qdev_get_gpio_دننه(DEVICE(s->plic), SIFIVE_U_QSPI2_IRQ));
}

ساكن ثابت Property sifive_u_soc_props[] = {
    DEFINE_PROP_UINT32("serial", SiFiveUSoCState, serial, OTP_SERIAL),
    DEFINE_PROP_STRING("cpu-type", SiFiveUSoCState, cpu_type),
};

ساكن فراغ sifive_u_soc_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_سېټ_props(dc, sifive_u_soc_props);
    dc->realize = sifive_u_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

ساكن ثابت TypeInfo sifive_u_soc_type_info = {
    .name = TYPE_RISCV_U_SOC,
    .پلار = TYPE_DEVICE,
    .instance_اندازه = حجم(SiFiveUSoCState),
    .instance_init = sifive_u_soc_instance_init,
    .class_init = sifive_u_soc_class_init,
};

ساكن فراغ sifive_u_soc_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&sifive_u_soc_type_info);
}

type_init(sifive_u_soc_ثبتوونکی_types)
