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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qapi/visitor.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/irq.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "hw/cpu/cluster.h"
#అన్తర్భావ "hw/misc/unimp.h"
#అన్తర్భావ "hw/sd/sd.h"
#అన్తర్భావ "hw/ssi/ssi.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "hw/riscv/sifive_u.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/riscv/fdt-common.h"
#అన్తర్భావ "hw/char/sifive_uart.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/sifive_plic.h"
#అన్తర్భావ "chardev/char.h"
#అన్తర్భావ "net/eth.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "system/runstate.h"
#అన్తర్భావ "system/system.h"

#అన్తర్భావ <libfdt.h>

/* CLINT timebase frequency */
#నిర్వచించు CLINT_TIMEBASE_FREQ 1000000

స్థిర స్థిరరాశి Mem_Map_నమోదు sifive_u_memmap[] = {
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

#నిర్వచించు OTP_SERIAL          1
#నిర్వచించు GEM_REVISION        0x10070109

స్థిర శూన్య create_fdt(SiFiveUState *s, స్థిరరాశి Mem_Map_నమోదు *memmap,
                       bool is_32_బిట్టు)
{
    MachineState *ms = MACHINE(s);
    శూన్య *fdt;
    పూర్ణాంకం cpu;
    uint32_t *cells, cells_నిడివి;
    అక్షరం *nodename;
    uint32_t plic_phandle, prci_phandle, gpio_phandle, phandle = 1;
    uint32_t hfclk_phandle, rtcclk_phandle, phy_phandle;
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి ethclk_పేర్లు[2] = { "pclk", "hclk" };
    g_autofree uint32_t *intc_phandles = g_new0(uint32_t, ms->smp.cpus);
    g_autofree అక్షరం *clust_name = ఖాళీ;

    fdt = ms->fdt = riscv_create_board_device_వృక్షం(
        "SiFive HiFive Unleashed A00",
        "sifive,hifive-unleashed-a00",
        &s->fdt_కొలత);

    hfclk_phandle = phandle++;
    nodename = g_strdup_printf("/hfclk");
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", hfclk_phandle);
    qemu_fdt_setprop_string(fdt, nodename, "clock-output-names", "hfclk");
    qemu_fdt_setprop_cell(fdt, nodename, "clock-frequency",
        SIFIVE_U_HFCLK_FREQ);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "fixed-clock");
    qemu_fdt_setprop_cell(fdt, nodename, "#clock-cells", 0x0);
    g_free(nodename);

    rtcclk_phandle = phandle++;
    nodename = g_strdup_printf("/rtcclk");
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", rtcclk_phandle);
    qemu_fdt_setprop_string(fdt, nodename, "clock-output-names", "rtcclk");
    qemu_fdt_setprop_cell(fdt, nodename, "clock-frequency",
        SIFIVE_U_RTCCLK_FREQ);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "fixed-clock");
    qemu_fdt_setprop_cell(fdt, nodename, "#clock-cells", 0x0);
    g_free(nodename);

    riscv_create_fdt_socket_జ్ఞాపకం(fdt, memmap[SIFIVE_U_DEV_DRAM].పునాది,
                                   ms->ram_కొలత, 0, false);

    riscv_fdt_create_cpu_socket_subnode(fdt, CLINT_TIMEBASE_FREQ);

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", 0);
    qemu_fdt_కూడు_subnode(fdt, clust_name);

    కొరకు (cpu = ms->smp.cpus - 1; cpu >= 0; cpu--) {
        nodename = g_strdup_printf("/cpus/cpu@%d", cpu);
        qemu_fdt_కూడు_subnode(fdt, nodename);

        /* cpu 0 is the management hart that does not have mmu */
        ఒకవేళ (cpu != 0) {
            ఒకవేళ (is_32_బిట్టు) {
                qemu_fdt_setprop_string(fdt, nodename, "mmu-type", "riscv,sv32");
            } లేకపోతే {
                qemu_fdt_setprop_string(fdt, nodename, "mmu-type", "riscv,sv39");
            }
            riscv_isa_write_fdt(&s->soc.u_cpus.harts[cpu - 1], fdt, nodename);
        } లేకపోతే {
            riscv_isa_write_fdt(&s->soc.e_cpus.harts[0], fdt, nodename);
        }

        riscv_create_fdt_socket_cpu_sifive(fdt, clust_name, cpu, 0, 0,
                                           &phandle, intc_phandles);

        g_free(nodename);
    }

    riscv_create_fdt_socket_clint(fdt, memmap[SIFIVE_U_DEV_CLINT].పునాది,
                                  memmap[SIFIVE_U_DEV_CLINT].కొలత, 0,
                                  intc_phandles, ms->smp.cpus, false);

    nodename = g_strdup_printf("/soc/otp@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_OTP].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "fuse-count", SIFIVE_U_OTP_REG_కొలత);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_OTP].పునాది,
        0x0, memmap[SIFIVE_U_DEV_OTP].కొలత);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
        "sifive,fu540-c000-otp");
    g_free(nodename);

    prci_phandle = phandle++;
    nodename = g_strdup_printf("/soc/clock-controller@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_PRCI].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", prci_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "#clock-cells", 0x1);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        hfclk_phandle, rtcclk_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_PRCI].పునాది,
        0x0, memmap[SIFIVE_U_DEV_PRCI].కొలత);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
        "sifive,fu540-c000-prci");
    g_free(nodename);

    plic_phandle = phandle++;
    cells_నిడివి = ms->smp.cpus * 4 - 2;
    cells =  g_new0(uint32_t, cells_నిడివి);
    కొరకు (cpu = 0; cpu < ms->smp.cpus; cpu++) {
        /* cpu 0 is the management hart that does not have S-mode */
        ఒకవేళ (cpu == 0) {
            cells[0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
            cells[1] = cpu_గమ్యం_be32(IRQ_M_EXT);
        } లేకపోతే {
            cells[cpu * 4 - 2] = cpu_గమ్యం_be32(intc_phandles[cpu]);
            cells[cpu * 4 - 1] = cpu_గమ్యం_be32(IRQ_M_EXT);
            cells[cpu * 4 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
            cells[cpu * 4 + 1] = cpu_గమ్యం_be32(IRQ_S_EXT);
        }
    }

    riscv_create_fdt_plic(fdt, memmap[SIFIVE_U_DEV_PLIC].పునాది,
                          memmap[SIFIVE_U_DEV_PLIC].కొలత,
                          plic_phandle, SIFIVE_U_PLIC_INT_CELLS,
                          SIFIVE_U_PLIC_ADDR_CELLS, cells,
                          cells_నిడివి * పరిమాణం(uint32_t),
                          SIFIVE_U_PLIC_అంకె_SOURCES - 1, false, 0);
    g_free(cells);

    gpio_phandle = phandle++;
    nodename = g_strdup_printf("/soc/gpio@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_GPIO].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", gpio_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", 2);
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", ఖాళీ, 0);
    qemu_fdt_setprop_cell(fdt, nodename, "#gpio-cells", 2);
    qemu_fdt_setprop(fdt, nodename, "gpio-controller", ఖాళీ, 0);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_GPIO].పునాది,
        0x0, memmap[SIFIVE_U_DEV_GPIO].కొలత);
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
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cells(fdt, nodename, "gpios", gpio_phandle, 10, 1);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "gpio-restart");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/dma@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_PDMA].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#dma-cells", 1);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
        SIFIVE_U_PDMA_IRQ0, SIFIVE_U_PDMA_IRQ1, SIFIVE_U_PDMA_IRQ2,
        SIFIVE_U_PDMA_IRQ3, SIFIVE_U_PDMA_IRQ4, SIFIVE_U_PDMA_IRQ5,
        SIFIVE_U_PDMA_IRQ6, SIFIVE_U_PDMA_IRQ7);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_PDMA].పునాది,
        0x0, memmap[SIFIVE_U_DEV_PDMA].కొలత);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
                            "sifive,fu540-c000-pdma");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/cache-controller@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_L2CC].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_L2CC].పునాది,
        0x0, memmap[SIFIVE_U_DEV_L2CC].కొలత);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
        SIFIVE_U_L2CC_IRQ0, SIFIVE_U_L2CC_IRQ1, SIFIVE_U_L2CC_IRQ2);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop(fdt, nodename, "cache-unified", ఖాళీ, 0);
    qemu_fdt_setprop_cell(fdt, nodename, "cache-size", 2097152);
    qemu_fdt_setprop_cell(fdt, nodename, "cache-sets", 1024);
    qemu_fdt_setprop_cell(fdt, nodename, "cache-level", 2);
    qemu_fdt_setprop_cell(fdt, nodename, "cache-block-size", 64);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
                            "sifive,fu540-c000-ccache");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/spi@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_QSPI2].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#size-cells", 0);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", 1);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_QSPI2_IRQ);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_QSPI2].పునాది,
        0x0, memmap[SIFIVE_U_DEV_QSPI2].కొలత);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,spi0");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/spi@%lx/mmc@0",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_QSPI2].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop(fdt, nodename, "disable-wp", ఖాళీ, 0);
    qemu_fdt_setprop_cells(fdt, nodename, "voltage-ranges", 3300, 3300);
    qemu_fdt_setprop_cell(fdt, nodename, "spi-max-frequency", 20000000);
    qemu_fdt_setprop_cell(fdt, nodename, "reg", 0);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "mmc-spi-slot");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/spi@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_QSPI0].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#size-cells", 0);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", 1);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_QSPI0_IRQ);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_QSPI0].పునాది,
        0x0, memmap[SIFIVE_U_DEV_QSPI0].కొలత);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,spi0");
    g_free(nodename);

    nodename = g_strdup_printf("/soc/spi@%lx/flash@0",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_QSPI0].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "spi-rx-bus-width", 4);
    qemu_fdt_setprop_cell(fdt, nodename, "spi-tx-bus-width", 4);
    qemu_fdt_setprop(fdt, nodename, "m25p,fast-read", ఖాళీ, 0);
    qemu_fdt_setprop_cell(fdt, nodename, "spi-max-frequency", 50000000);
    qemu_fdt_setprop_cell(fdt, nodename, "reg", 0);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "jedec,spi-nor");
    g_free(nodename);

    phy_phandle = phandle++;
    nodename = g_strdup_printf("/soc/ethernet@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_GEM].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible",
        "sifive,fu540-c000-gem");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_GEM].పునాది,
        0x0, memmap[SIFIVE_U_DEV_GEM].కొలత,
        0x0, memmap[SIFIVE_U_DEV_GEM_MGMT].పునాది,
        0x0, memmap[SIFIVE_U_DEV_GEM_MGMT].కొలత);
    qemu_fdt_setprop_string(fdt, nodename, "reg-names", "control");
    qemu_fdt_setprop_string(fdt, nodename, "phy-mode", "gmii");
    qemu_fdt_setprop_cell(fdt, nodename, "phy-handle", phy_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_GEM_IRQ);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_GEMGXLPLL, prci_phandle, PRCI_CLK_GEMGXLPLL);
    qemu_fdt_setprop_string_array(fdt, nodename, "clock-names",
        (అక్షరం **)&ethclk_పేర్లు, ARRAY_కొలత(ethclk_పేర్లు));
    qemu_fdt_setprop(fdt, nodename, "local-mac-address",
        s->soc.gem.conf.macaddr.a, ETH_ALEN);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", 1);
    qemu_fdt_setprop_cell(fdt, nodename, "#size-cells", 0);

    qemu_fdt_కూడు_subnode(fdt, "/aliases");
    qemu_fdt_setprop_string(fdt, "/aliases", "ethernet0", nodename);

    g_free(nodename);

    nodename = g_strdup_printf("/soc/ethernet@%lx/ethernet-phy@0",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_GEM].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", phy_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "reg", 0x0);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/pwm@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_PWM0].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,pwm0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_PWM0].పునాది,
        0x0, memmap[SIFIVE_U_DEV_PWM0].కొలత);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
                           SIFIVE_U_PWM0_IRQ0, SIFIVE_U_PWM0_IRQ1,
                           SIFIVE_U_PWM0_IRQ2, SIFIVE_U_PWM0_IRQ3);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
                           prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "#pwm-cells", 0);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/pwm@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_PWM1].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,pwm0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_PWM1].పునాది,
        0x0, memmap[SIFIVE_U_DEV_PWM1].కొలత);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts",
                           SIFIVE_U_PWM1_IRQ0, SIFIVE_U_PWM1_IRQ1,
                           SIFIVE_U_PWM1_IRQ2, SIFIVE_U_PWM1_IRQ3);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
                           prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "#pwm-cells", 0);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/serial@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_UART1].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,uart0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_UART1].పునాది,
        0x0, memmap[SIFIVE_U_DEV_UART1].కొలత);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_UART1_IRQ);

    qemu_fdt_setprop_string(fdt, "/aliases", "serial1", nodename);
    g_free(nodename);

    nodename = g_strdup_printf("/soc/serial@%lx",
        (దీర్ఘ)memmap[SIFIVE_U_DEV_UART0].పునాది);
    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_string(fdt, nodename, "compatible", "sifive,uart0");
    qemu_fdt_setprop_cells(fdt, nodename, "reg",
        0x0, memmap[SIFIVE_U_DEV_UART0].పునాది,
        0x0, memmap[SIFIVE_U_DEV_UART0].కొలత);
    qemu_fdt_setprop_cells(fdt, nodename, "clocks",
        prci_phandle, PRCI_CLK_TLCLK);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupt-parent", plic_phandle);
    qemu_fdt_setprop_cell(fdt, nodename, "interrupts", SIFIVE_U_UART0_IRQ);

    qemu_fdt_కూడు_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", nodename);
    qemu_fdt_setprop_string(fdt, "/aliases", "serial0", nodename);

    g_free(nodename);
}

స్థిర శూన్య sifive_u_machine_reset(శూన్య *opaque, పూర్ణాంకం n, పూర్ణాంకం level)
{
    /* gpio pin active low triggers reset */
    ఒకవేళ (!level) {
        qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
    }
}

స్థిర శూన్య sifive_u_machine_init(MachineState *machine)
{
    స్థిరరాశి Mem_Map_నమోదు *memmap = sifive_u_memmap;
    SiFiveUState *s = RISCV_U_MACHINE(machine);
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    జ్ఞాపకం_Region *flash0 = g_new(జ్ఞాపకం_Region, 1);
    hwaddr ప్రారంభం_addr = memmap[SIFIVE_U_DEV_DRAM].పునాది;
    hwaddr firmware_చివర_addr;
    vaddr kernel_ప్రారంభం_addr;
    స్థిరరాశి అక్షరం *firmware_name;
    uint32_t ప్రారంభం_addr_hi32 = 0x00000000;
    uint32_t fdt_ఎక్కించు_addr_hi32 = 0x00000000;
    పూర్ణాంకం i;
    uint64_t fdt_ఎక్కించు_addr;
    uint64_t kernel_నమోదు;
    DriveInfo *dinfo;
    ఖండం_Backend *blk;
    DeviceState *flash_dev, *sd_dev, *card_dev;
    qemu_irq flash_cs, sd_cs;
    RISCVBootInfo boot_info;

    /* Initialize SoC */
    object_initialize_బిడ్డ(OBJECT(machine), "soc", &s->soc, TYPE_RISCV_U_SOC);
    object_property_సమితి_uint(OBJECT(&s->soc), "serial", s->serial,
                             &error_abort);
    object_property_సమితి_str(OBJECT(&s->soc), "cpu-type", machine->cpu_type,
                             &error_abort);
    qdev_realize(DEVICE(&s->soc), ఖాళీ, &error_fatal);

    /* register RAM */
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, memmap[SIFIVE_U_DEV_DRAM].పునాది,
                                machine->ram);

    /* register QSPI0 Flash */
    జ్ఞాపకం_region_init_ram(flash0, ఖాళీ, "riscv.sifive.u.flash0",
                           memmap[SIFIVE_U_DEV_FLASH0].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, memmap[SIFIVE_U_DEV_FLASH0].పునాది,
                                flash0);

    /* register gpio-restart */
    qdev_connect_gpio_బహిర్గతం(DEVICE(&(s->soc.gpio)), 10,
                          qemu_allocate_irq(sifive_u_machine_reset, ఖాళీ, 0));

    /* load/create device tree */
    ఒకవేళ (machine->dtb) {
        machine->fdt = ఎక్కించు_device_వృక్షం(machine->dtb, &s->fdt_కొలత);
        ఒకవేళ (!machine->fdt) {
            error_report("load_device_tree() failed");
            నిష్క్రమించు(1);
        }
    } లేకపోతే {
        create_fdt(s, memmap, riscv_is_32bit(&s->soc.u_cpus));
    }

    ఒకవేళ (s->ప్రారంభం_లోపల_flash) {
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

    ఎంపిక (s->msel) {
    స్థితి MSEL_MEMMAP_QSPI0_FLASH:
        ప్రారంభం_addr = memmap[SIFIVE_U_DEV_FLASH0].పునాది;
        ఆపు;
    స్థితి MSEL_L2LIM_QSPI0_FLASH:
    స్థితి MSEL_L2LIM_QSPI2_SD:
        ప్రారంభం_addr = memmap[SIFIVE_U_DEV_L2LIM].పునాది;
        ఆపు;
    అప్రమేయ:
        ప్రారంభం_addr = memmap[SIFIVE_U_DEV_DRAM].పునాది;
        ఆపు;
    }

    riscv_boot_info_init(&boot_info, &s->soc.u_cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.u_cpus);
    firmware_చివర_addr = riscv_కనుగొను_and_ఎక్కించు_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ప్రారంభం_addr, ఖాళీ);

    ఒకవేళ (machine->kernel_filename) {
        kernel_ప్రారంభం_addr = riscv_calc_kernel_ప్రారంభం_addr(&boot_info,
                                                         firmware_చివర_addr);
        riscv_ఎక్కించు_kernel(machine, &boot_info, kernel_ప్రారంభం_addr,
                          true, ఖాళీ);
        kernel_నమోదు = boot_info.image_దిగువ_addr;
    } లేకపోతే {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_నమోదు = 0;
    }

    fdt_ఎక్కించు_addr = riscv_compute_fdt_addr(memmap[SIFIVE_U_DEV_DRAM].పునాది,
                                           memmap[SIFIVE_U_DEV_DRAM].కొలత,
                                           machine, &boot_info);
    riscv_ఎక్కించు_fdt(fdt_ఎక్కించు_addr, machine->fdt);

    ఒకవేళ (!riscv_is_32bit(&s->soc.u_cpus)) {
        ప్రారంభం_addr_hi32 = (uint64_t)ప్రారంభం_addr >> 32;
        fdt_ఎక్కించు_addr_hi32 = fdt_ఎక్కించు_addr >> 32;
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
        ప్రారంభం_addr,                    /* start: .dword */
        ప్రారంభం_addr_hi32,
        fdt_ఎక్కించు_addr,                 /* fdt_laddr: .dword */
        fdt_ఎక్కించు_addr_hi32,
                                       /* fw_dyn: */
    };

    ఒకవేళ (riscv_is_32bit(&s->soc.u_cpus)) {
        reset_vec[4] = 0x0242a583;     /*     lw     a1, 36(t0) */
        reset_vec[5] = 0x01c2a283;     /*     lw     t0, 28(t0) */
    } లేకపోతే {
        reset_vec[4] = 0x0242b583;     /*     ld     a1, 36(t0) */
        reset_vec[5] = 0x01c2b283;     /*     ld     t0, 28(t0) */
    }


    /* copy in the reset vector in little_endian byte order */
    కొరకు (i = 0; i < ARRAY_కొలత(reset_vec); i++) {
        reset_vec[i] = cpu_గమ్యం_le32(reset_vec[i]);
    }
    rom_కూడు_blob_fixed_as("mrom.reset", reset_vec, పరిమాణం(reset_vec),
                          memmap[SIFIVE_U_DEV_MROM].పునాది, &చిరునామా_space_జ్ఞాపకం);

    riscv_rom_copy_firmware_info(machine, &s->soc.u_cpus,
                                 memmap[SIFIVE_U_DEV_MROM].పునాది,
                                 memmap[SIFIVE_U_DEV_MROM].కొలత,
                                 పరిమాణం(reset_vec), kernel_నమోదు);

    /* Connect an SPI flash to SPI0 */
    flash_dev = qdev_new("is25wp256");
    dinfo = drive_get(IF_MTD, 0, 0);
    ఒకవేళ (dinfo) {
        qdev_prop_సమితి_drive_err(flash_dev, "drive",
                                blk_by_legacy_dinfo(dinfo),
                                &error_fatal);
    }
    qdev_realize_and_unref(flash_dev, BUS(s->soc.spi0.spi), &error_fatal);

    flash_cs = qdev_get_gpio_లోపల_named(flash_dev, SSI_GPIO_CS, 0);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->soc.spi0), 1, flash_cs);

    /* Connect an SD card to SPI2 */
    sd_dev = ssi_create_peripheral(s->soc.spi2.spi, "ssi-sd");

    sd_cs = qdev_get_gpio_లోపల_named(sd_dev, SSI_GPIO_CS, 0);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->soc.spi2), 1, sd_cs);

    dinfo = drive_get(IF_SD, 0, 0);
    blk = dinfo ? blk_by_legacy_dinfo(dinfo) : ఖాళీ;
    card_dev = qdev_new(TYPE_SD_CARD_SPI);
    qdev_prop_సమితి_drive_err(card_dev, "drive", blk, &error_fatal);
    qdev_realize_and_unref(card_dev,
                           qdev_get_బిడ్డ_bus(sd_dev, "sd-bus"),
                           &error_fatal);
}

స్థిర bool sifive_u_machine_get_ప్రారంభం_లోపల_flash(Object *obj, Error **errp)
{
    SiFiveUState *s = RISCV_U_MACHINE(obj);

    తిరిగి s->ప్రారంభం_లోపల_flash;
}

స్థిర శూన్య sifive_u_machine_సమితి_ప్రారంభం_లోపల_flash(Object *obj, bool విలువ, Error **errp)
{
    SiFiveUState *s = RISCV_U_MACHINE(obj);

    s->ప్రారంభం_లోపల_flash = విలువ;
}

స్థిర శూన్య sifive_u_machine_instance_init(Object *obj)
{
    SiFiveUState *s = RISCV_U_MACHINE(obj);

    s->ప్రారంభం_లోపల_flash = false;
    s->msel = 0;
    object_property_కూడు_uint32_ptr(obj, "msel", &s->msel,
                                   OBJ_PROP_FLAG_READWRITE);
    object_property_సమితి_description(obj, "msel",
                                    "Mode Select (MSEL[3:0]) pin state");

    s->serial = OTP_SERIAL;
    object_property_కూడు_uint32_ptr(obj, "serial", &s->serial,
                                   OBJ_PROP_FLAG_READWRITE);
    object_property_సమితి_description(obj, "serial", "Board serial number");
}

స్థిర శూన్య sifive_u_machine_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with SiFive U SDK";
    mc->init = sifive_u_machine_init;
    mc->max_cpus = SIFIVE_U_MANAGEMENT_CPU_లెక్క + SIFIVE_U_COMPUTE_CPU_లెక్క;
    mc->min_cpus = SIFIVE_U_MANAGEMENT_CPU_లెక్క + 1;
    mc->default_cpu_type = SIFIVE_U_CPU;
    mc->default_cpus = mc->min_cpus;
    mc->default_ram_id = "riscv.sifive.u.ram";
    mc->auto_create_sdcard = true;

    object_class_property_కూడు_bool(oc, "start-in-flash",
                                   sifive_u_machine_get_ప్రారంభం_లోపల_flash,
                                   sifive_u_machine_సమితి_ప్రారంభం_లోపల_flash);
    object_class_property_సమితి_description(oc, "start-in-flash",
                                          "Set on to tell QEMU's ROM to jump to "
                                          "flash. Otherwise QEMU will jump to DRAM "
                                          "or L2LIM depending on the msel value");
}

స్థిర స్థిరరాశి TypeInfo sifive_u_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("sifive_u"),
    .మాతృ     = TYPE_MACHINE,
    .class_init = sifive_u_machine_class_init,
    .instance_init = sifive_u_machine_instance_init,
    .instance_కొలత = పరిమాణం(SiFiveUState),
};

స్థిర శూన్య sifive_u_machine_init_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&sifive_u_machine_typeinfo);
}

type_init(sifive_u_machine_init_రిజిస్టరు_types)

స్థిర శూన్య sifive_u_soc_instance_init(Object *obj)
{
    SiFiveUSoCState *s = RISCV_U_SOC(obj);

    object_initialize_బిడ్డ(obj, "e-cluster", &s->e_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_సమితి_uint32(DEVICE(&s->e_cluster), "cluster-id", 0);

    object_initialize_బిడ్డ(OBJECT(&s->e_cluster), "e-cpus", &s->e_cpus,
                            TYPE_RISCV_HART_ARRAY);
    qdev_prop_సమితి_uint32(DEVICE(&s->e_cpus), "num-harts", 1);
    qdev_prop_సమితి_uint32(DEVICE(&s->e_cpus), "hartid-base", 0);
    qdev_prop_సమితి_string(DEVICE(&s->e_cpus), "cpu-type", SIFIVE_E_CPU);
    qdev_prop_సమితి_uint64(DEVICE(&s->e_cpus), "resetvec", 0x1004);

    object_initialize_బిడ్డ(obj, "u-cluster", &s->u_cluster, TYPE_CPU_CLUSTER);
    qdev_prop_సమితి_uint32(DEVICE(&s->u_cluster), "cluster-id", 1);

    object_initialize_బిడ్డ(OBJECT(&s->u_cluster), "u-cpus", &s->u_cpus,
                            TYPE_RISCV_HART_ARRAY);

    object_initialize_బిడ్డ(obj, "prci", &s->prci, TYPE_SIFIVE_U_PRCI);
    object_initialize_బిడ్డ(obj, "otp", &s->otp, TYPE_SIFIVE_U_OTP);
    object_initialize_బిడ్డ(obj, "gem", &s->gem, TYPE_CADENCE_GEM);
    object_initialize_బిడ్డ(obj, "gpio", &s->gpio, TYPE_SIFIVE_GPIO);
    object_initialize_బిడ్డ(obj, "pdma", &s->dma, TYPE_SIFIVE_PDMA);
    object_initialize_బిడ్డ(obj, "spi0", &s->spi0, TYPE_SIFIVE_SPI);
    object_initialize_బిడ్డ(obj, "spi2", &s->spi2, TYPE_SIFIVE_SPI);
    object_initialize_బిడ్డ(obj, "pwm0", &s->pwm[0], TYPE_SIFIVE_PWM);
    object_initialize_బిడ్డ(obj, "pwm1", &s->pwm[1], TYPE_SIFIVE_PWM);
}

స్థిర శూన్య sifive_u_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    SiFiveUSoCState *s = RISCV_U_SOC(dev);
    స్థిరరాశి Mem_Map_నమోదు *memmap = sifive_u_memmap;
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    జ్ఞాపకం_Region *ముసుగు_rom = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *l2lim_mem = g_new(జ్ఞాపకం_Region, 1);
    అక్షరం *plic_hart_config;
    పూర్ణాంకం i, j;

    qdev_prop_సమితి_uint32(DEVICE(&s->u_cpus), "num-harts", ms->smp.cpus - 1);
    qdev_prop_సమితి_uint32(DEVICE(&s->u_cpus), "hartid-base", 1);
    qdev_prop_సమితి_string(DEVICE(&s->u_cpus), "cpu-type", s->cpu_type);
    qdev_prop_సమితి_uint64(DEVICE(&s->u_cpus), "resetvec", 0x1004);

    sysbus_realize(SYS_BUS_DEVICE(&s->e_cpus), &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->u_cpus), &error_fatal);
    /*
     * The cluster must be realized after the RISC-V hart array container,
     * as the container's CPU object is only created on realize, and the
     * CPU must exist and have been parented into the cluster before the
     * cluster is realized.
     */
    qdev_realize(DEVICE(&s->e_cluster), ఖాళీ, &error_abort);
    qdev_realize(DEVICE(&s->u_cluster), ఖాళీ, &error_abort);

    /* boot rom */
    జ్ఞాపకం_region_init_rom(ముసుగు_rom, OBJECT(dev), "riscv.sifive.u.mrom",
                           memmap[SIFIVE_U_DEV_MROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, memmap[SIFIVE_U_DEV_MROM].పునాది,
                                ముసుగు_rom);

    /*
     * Add L2-LIM at reset size.
     * This should be reduced in size as the L2 Cache Controller WayEnable
     * register is incremented. Unfortunately I don't see a nice (or any) way
     * to handle reducing or blocking out the L2 LIM while still allowing it
     * be re returned to all enabled after a reset. For the time being, just
     * leave it enabled all the time. This won't break anything, but will be
     * too generous to misbehaving guests.
     */
    జ్ఞాపకం_region_init_ram(l2lim_mem, ఖాళీ, "riscv.sifive.u.l2lim",
                           memmap[SIFIVE_U_DEV_L2LIM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, memmap[SIFIVE_U_DEV_L2LIM].పునాది,
                                l2lim_mem);

    /* create PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(ms->smp.cpus);

    /* MMIO */
    s->plic = sifive_plic_create(system_జ్ఞాపకం,
        memmap[SIFIVE_U_DEV_PLIC].పునాది,
        plic_hart_config, ms->smp.cpus, 0,
        SIFIVE_U_PLIC_అంకె_SOURCES,
        SIFIVE_U_PLIC_అంకె_PRIORITIES,
        SIFIVE_U_PLIC_ప్రాధాన్యత_పునాది,
        SIFIVE_U_PLIC_PENDING_పునాది,
        SIFIVE_U_PLIC_ENABLE_పునాది,
        SIFIVE_U_PLIC_ENABLE_STRIDE,
        SIFIVE_U_PLIC_CONTEXT_పునాది,
        SIFIVE_U_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_U_DEV_PLIC].కొలత);
    g_free(plic_hart_config);
    sifive_uart_create(system_జ్ఞాపకం, memmap[SIFIVE_U_DEV_UART0].పునాది,
        serial_hd(0), qdev_get_gpio_లోపల(DEVICE(s->plic), SIFIVE_U_UART0_IRQ));
    sifive_uart_create(system_జ్ఞాపకం, memmap[SIFIVE_U_DEV_UART1].పునాది,
        serial_hd(1), qdev_get_gpio_లోపల(DEVICE(s->plic), SIFIVE_U_UART1_IRQ));
    riscv_aclint_swi_create(system_జ్ఞాపకం,
        memmap[SIFIVE_U_DEV_CLINT].పునాది, 0,
        ms->smp.cpus, false);
    riscv_aclint_mtimer_create(system_జ్ఞాపకం,
        memmap[SIFIVE_U_DEV_CLINT].పునాది + RISCV_ACLINT_SWI_కొలత,
        RISCV_ACLINT_DEFAULT_MTIMER_కొలత, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        CLINT_TIMEBASE_FREQ, false);

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->prci), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->prci), 0, memmap[SIFIVE_U_DEV_PRCI].పునాది);

    qdev_prop_సమితి_uint32(DEVICE(&s->gpio), "ngpio", 16);
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_U_DEV_GPIO].పునాది);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, ఖాళీ);

    /* Connect GPIO interrupts to the PLIC */
    కొరకు (i = 0; i < 16; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i,
                           qdev_get_gpio_లోపల(DEVICE(s->plic),
                                            SIFIVE_U_GPIO_IRQ0 + i));
    }

    /* PDMA */
    sysbus_realize(SYS_BUS_DEVICE(&s->dma), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->dma), 0, memmap[SIFIVE_U_DEV_PDMA].పునాది);

    /* Connect PDMA interrupts to the PLIC */
    కొరకు (i = 0; i < SIFIVE_PDMA_IRQS; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->dma), i,
                           qdev_get_gpio_లోపల(DEVICE(s->plic),
                                            SIFIVE_U_PDMA_IRQ0 + i));
    }

    qdev_prop_సమితి_uint32(DEVICE(&s->otp), "serial", s->serial);
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->otp), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->otp), 0, memmap[SIFIVE_U_DEV_OTP].పునాది);

    qemu_configure_nic_device(DEVICE(&s->gem), true, ఖాళీ);
    object_property_సమితి_int(OBJECT(&s->gem), "revision", GEM_REVISION,
                            &error_abort);
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->gem), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gem), 0, memmap[SIFIVE_U_DEV_GEM].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->gem), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->plic), SIFIVE_U_GEM_IRQ));

    /* PWM */
    కొరకు (i = 0; i < 2; i++) {
        ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->pwm[i]), errp)) {
            తిరిగి;
        }
        sysbus_mmio_map(SYS_BUS_DEVICE(&s->pwm[i]), 0,
                                memmap[SIFIVE_U_DEV_PWM0].పునాది + (0x1000 * i));

        /* Connect PWM interrupts to the PLIC */
        కొరకు (j = 0; j < SIFIVE_PWM_IRQS; j++) {
            sysbus_connect_irq(SYS_BUS_DEVICE(&s->pwm[i]), j,
                               qdev_get_gpio_లోపల(DEVICE(s->plic),
                                        SIFIVE_U_PWM0_IRQ0 + (i * 4) + j));
        }
    }

    create_unimplemented_device("riscv.sifive.u.gem-mgmt",
        memmap[SIFIVE_U_DEV_GEM_MGMT].పునాది, memmap[SIFIVE_U_DEV_GEM_MGMT].కొలత);

    create_unimplemented_device("riscv.sifive.u.dmc",
        memmap[SIFIVE_U_DEV_DMC].పునాది, memmap[SIFIVE_U_DEV_DMC].కొలత);

    create_unimplemented_device("riscv.sifive.u.l2cc",
        memmap[SIFIVE_U_DEV_L2CC].పునాది, memmap[SIFIVE_U_DEV_L2CC].కొలత);

    sysbus_realize(SYS_BUS_DEVICE(&s->spi0), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->spi0), 0,
                    memmap[SIFIVE_U_DEV_QSPI0].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->spi0), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->plic), SIFIVE_U_QSPI0_IRQ));
    sysbus_realize(SYS_BUS_DEVICE(&s->spi2), errp);
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->spi2), 0,
                    memmap[SIFIVE_U_DEV_QSPI2].పునాది);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->spi2), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->plic), SIFIVE_U_QSPI2_IRQ));
}

స్థిర స్థిరరాశి Property sifive_u_soc_props[] = {
    DEFINE_PROP_UINT32("serial", SiFiveUSoCState, serial, OTP_SERIAL),
    DEFINE_PROP_STRING("cpu-type", SiFiveUSoCState, cpu_type),
};

స్థిర శూన్య sifive_u_soc_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_సమితి_props(dc, sifive_u_soc_props);
    dc->realize = sifive_u_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

స్థిర స్థిరరాశి TypeInfo sifive_u_soc_type_info = {
    .name = TYPE_RISCV_U_SOC,
    .మాతృ = TYPE_DEVICE,
    .instance_కొలత = పరిమాణం(SiFiveUSoCState),
    .instance_init = sifive_u_soc_instance_init,
    .class_init = sifive_u_soc_class_init,
};

స్థిర శూన్య sifive_u_soc_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&sifive_u_soc_type_info);
}

type_init(sifive_u_soc_రిజిస్టరు_types)
