/*
 * QEMU RISC-V VirtIO Board
 *
 * Copyright (c) 2017 SiFive, Inc.
 *
 * RISC-V machine with 16550a UART and VirtIO MMIO
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
#అన్తర్భావ "qemu/units.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qemu/guest-random.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/char/serial-mm.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/core/sysbus-fdt.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "hw/riscv/iommu.h"
#అన్తర్భావ "hw/riscv/riscv-iommu.h"
#అన్తర్భావ "hw/riscv/riscv-iommu-bits.h"
#అన్తర్భావ "hw/riscv/virt.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/riscv/device-common.h"
#అన్తర్భావ "hw/riscv/fdt-common.h"
#అన్తర్భావ "hw/riscv/numa.h"
#అన్తర్భావ "kvm/kvm_riscv.h"
#అన్తర్భావ "hw/firmware/smbios.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/riscv_aplic.h"
#అన్తర్భావ "hw/intc/sifive_plic.h"
#అన్తర్భావ "hw/misc/sifive_test.h"
#అన్తర్భావ "hw/core/platform-bus.h"
#అన్తర్భావ "chardev/char.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "system/system.h"
#అన్తర్భావ "system/tcg.h"
#అన్తర్భావ "system/kvm.h"
#అన్తర్భావ "system/tpm.h"
#అన్తర్భావ "system/qtest.h"
#అన్తర్భావ "hw/pci/pci.h"
#అన్తర్భావ "hw/pci-host/gpex.h"
#అన్తర్భావ "hw/display/ramfb.h"
#అన్తర్భావ "hw/acpi/aml-build.h"
#అన్తర్భావ "qapi/qapi-visit-common.h"
#అన్తర్భావ "hw/virtio/virtio-iommu.h"
#అన్తర్భావ "hw/uefi/var-service-api.h"

#అన్తర్భావ "aia.h"

/* KVM AIA only supports APLIC MSI. APLIC Wired is always emulated by QEMU. */
స్థిర bool virt_use_kvm_aia_aplic_imsic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    తిరిగి riscv_is_kvm_aia_aplic_imsic(msimode);
}

స్థిర bool virt_use_emulated_aplic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    తిరిగి riscv_use_emulated_aplic(msimode);
}

స్థిర bool virt_aclint_allowed(శూన్య)
{
    తిరిగి tcg_enabled() || qtest_enabled();
}

స్థిర స్థిరరాశి Mem_Map_నమోదు virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_IOMMU_SYS] =    {  0x3010000,        0x1000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_కొలత(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_కొలత(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_కొలత(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    /* UART1 supports page isolation from UART0 */
    [VIRT_UART1] =        { 0x1000a000,         0x100 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_కొలత },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_కొలత },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    [VIRT_DRAM] =         { 0x80000000,           0x0 },
};

/* PCIe high mmio is fixed for RV32 */
#నిర్వచించు VIRT32_ఎగువ_PCIE_MMIO_పునాది  0x300000000ULL
#నిర్వచించు VIRT32_ఎగువ_PCIE_MMIO_కొలత  (4 * GiB)

/* PCIe high mmio for RV64, size is fixed but base depends on top of RAM */
#నిర్వచించు VIRT64_ఎగువ_PCIE_MMIO_కొలత  (16 * GiB)

స్థిర Mem_Map_నమోదు virt_ఎగువ_pcie_memmap;

#నిర్వచించు VIRT_FLASH_SECTOR_కొలత (256 * KiB)

స్థిర శూన్య virt_flash_create(RISCVVirtState *s)
{
    s->flash[0] = riscv_flash_create(OBJECT(s), "virt.flash0", "pflash0",
                                     VIRT_FLASH_SECTOR_కొలత);
    s->flash[1] = riscv_flash_create(OBJECT(s), "virt.flash1", "pflash1",
                                     VIRT_FLASH_SECTOR_కొలత);
}

స్థిర శూన్య virt_flash_map(RISCVVirtState *s,
                           జ్ఞాపకం_Region *sysmem)
{
    hwaddr flashsize = s->memmap[VIRT_FLASH].కొలత / 2;
    hwaddr flashbase = s->memmap[VIRT_FLASH].పునాది;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_కొలత);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_కొలత);
}

స్థిర శూన్య create_fdt_socket_plic(RISCVVirtState *s,
                                   పూర్ణాంకం socket,
                                   uint32_t *phandle, uint32_t *intc_phandles,
                                   uint32_t *plic_phandles)
{
    పూర్ణాంకం cpu;
    g_autofree అక్షరం *plic_name = ఖాళీ;
    g_autofree uint32_t *plic_cells;
    MachineState *ms = MACHINE(s);
    గుర్తులేని దీర్ఘ plic_addr = s->memmap[VIRT_PLIC].పునాది +
                              (s->memmap[VIRT_PLIC].కొలత * socket);
    bool numa_enabled = riscv_numa_enabled(MACHINE(s));
    uint32_t cells_నిడివి;

    plic_name = g_strdup_printf("/soc/interrupt-controller@%lx", plic_addr);

    ఒకవేళ (kvm_enabled()) {
        cells_నిడివి = s->soc[socket].అంకె_harts * 2;
        plic_cells = g_new0(uint32_t, cells_నిడివి);

        కొరకు (cpu = 0; cpu < s->soc[socket].అంకె_harts; cpu++) {
            plic_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
            plic_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(IRQ_S_EXT);
        }
   } లేకపోతే {
        cells_నిడివి = s->soc[socket].అంకె_harts * 4;
        plic_cells = g_new0(uint32_t, cells_నిడివి);

        కొరకు (cpu = 0; cpu < s->soc[socket].అంకె_harts; cpu++) {
            plic_cells[cpu * 4 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 1] = cpu_గమ్యం_be32(IRQ_M_EXT);
            plic_cells[cpu * 4 + 2] = cpu_గమ్యం_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 3] = cpu_గమ్యం_be32(IRQ_S_EXT);
        }
    }

    plic_phandles[socket] = (*phandle)++;

    riscv_create_fdt_plic(ms->fdt, plic_addr, s->memmap[VIRT_PLIC].కొలత,
                          plic_phandles[socket], FDT_PLIC_INT_CELLS,
                          FDT_PLIC_ADDR_CELLS, plic_cells,
                          cells_నిడివి * పరిమాణం(uint32_t),
                          VIRT_IRQCHIP_అంకె_SOURCES - 1,
                          numa_enabled, socket);

    ఒకవేళ (!socket) {
        platform_bus_కూడు_all_fdt_nodes(ms->fdt, plic_name,
                                       s->memmap[VIRT_PLATFORM_BUS].పునాది,
                                       s->memmap[VIRT_PLATFORM_BUS].కొలత,
                                       VIRT_PLATFORM_BUS_IRQ);
    }
}

స్థిర శూన్య create_fdt_pmu(RISCVVirtState *s)
{
    g_autofree అక్షరం *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_కూడు_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_కణుపు(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

స్థిర శూన్య create_fdt_sockets(RISCVVirtState *s,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
                               uint32_t *irq_virtio_phandle,
                               uint32_t *msi_pcie_phandle)
{
    పూర్ణాంకం socket, phandle_pos;
    MachineState *ms = MACHINE(s);
    uint32_t msi_m_phandle = 0, msi_s_phandle = 0;
    uint32_t xplic_phandles[MAX_NODES];
    g_autofree uint32_t *intc_phandles = ఖాళీ;
    పూర్ణాంకం socket_లెక్క = riscv_socket_లెక్క(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_బిట్టు = riscv_is_32bit(&s->soc[0]);
    APLICFdtProps aplic_props;
    ACLINTFdtProps aclint_props;

    riscv_fdt_create_cpu_socket_subnode(ms->fdt,
        kvm_enabled() ? kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    intc_phandles = g_new0(uint32_t, ms->smp.cpus);

    ఒకవేళ (virt_aclint_allowed() && s->have_aclint) {
        aclint_props.clint = &s->memmap[VIRT_CLINT];
        aclint_props.aclint_sswi = &s->memmap[VIRT_ACLINT_SSWI];
        aclint_props.aia_type = s->aia_type;
        aclint_props.numa_enabled = numa_enabled;
    }

    phandle_pos = ms->smp.cpus;
    కొరకు (socket = (socket_లెక్క - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[VIRT_DRAM].పునాది +
                         riscv_socket_mem_స్థానభ్రంశం(ms, socket);
        uint64_t memsize = riscv_socket_mem_కొలత(ms, socket);

        phandle_pos -= s->soc[socket].అంకె_harts;

        riscv_create_fdt_socket_cpus(ms->fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].అంకె_harts,
                                     s->soc[socket].hartid_పునాది,
                                     phandle, &intc_phandles[phandle_pos],
                                     numa_enabled, is_32_బిట్టు);

        riscv_create_fdt_socket_జ్ఞాపకం(ms->fdt, memaddr, memsize,
                                       socket, riscv_numa_enabled(ms));

        ఒకవేళ (virt_aclint_allowed() && s->have_aclint) {
            aclint_props.socket = socket;
            aclint_props.అంకె_harts = s->soc[socket].అంకె_harts;
            riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                           &intc_phandles[phandle_pos]);
        } లేకపోతే ఒకవేళ (tcg_enabled()) {
            hwaddr clintaddr = s->memmap[VIRT_CLINT].పునాది +
                               s->memmap[VIRT_CLINT].కొలత * socket;

            riscv_create_fdt_socket_clint(ms->fdt, clintaddr,
                                          s->memmap[VIRT_CLINT].కొలత,
                                          socket, &intc_phandles[phandle_pos],
                                          s->soc[socket].అంకె_harts,
                                          numa_enabled);
        }
    }

    ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        IMSICFdtProps props = {
            .soc = &s->soc,
            .socket_లెక్క = riscv_socket_లెక్క(ms),
            .smp_cpus = ms->smp.cpus,
            .imsic_m_పునాది = !kvm_enabled() ? s->memmap[VIRT_IMSIC_M].పునాది : 0,
            .imsic_s_పునాది = s->memmap[VIRT_IMSIC_S].పునాది,
            .imsic_సమూహం_max_కొలత = VIRT_IMSIC_సమూహం_MAX_కొలత,
            .irqchip_అంకె_msis = VIRT_IRQCHIP_అంకె_MSIS,
            .aia_guests = s->aia_guests
        };

        riscv_create_fdt_imsic(ms->fdt, &props, phandle, intc_phandles,
                               &msi_m_phandle, &msi_s_phandle);

        *msi_pcie_phandle = msi_s_phandle;
    }

    ఒకవేళ (s->aia_type != VIRT_AIA_TYPE_NONE) {
        aplic_props.aplic_m = !kvm_enabled() ? &s->memmap[VIRT_APLIC_M] : ఖాళీ;
        aplic_props.aplic_s = &s->memmap[VIRT_APLIC_S];
        aplic_props.platform_bus = &s->memmap[VIRT_PLATFORM_BUS];
        aplic_props.platform_bus_irq = VIRT_PLATFORM_BUS_IRQ;
        aplic_props.socket = 0;
        aplic_props.అంకె_harts = ms->smp.cpus;
        aplic_props.numa_enabled = numa_enabled;
        aplic_props.irqchip_అంకె_sources = VIRT_IRQCHIP_అంకె_SOURCES;
        aplic_props.aia_type = s->aia_type;
    }

    /*
     * With KVM AIA aplic-imsic, using an irqchip without split
     * mode, we'll use only one APLIC instance.
     */
    ఒకవేళ (!virt_use_emulated_aplic(s->aia_type)) {
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[0], xplic_phandles);

        *irq_mmio_phandle = xplic_phandles[0];
        *irq_virtio_phandle = xplic_phandles[0];
        *irq_pcie_phandle = xplic_phandles[0];
    } లేకపోతే {
        phandle_pos = ms->smp.cpus;
        కొరకు (socket = (socket_లెక్క - 1); socket >= 0; socket--) {
            phandle_pos -= s->soc[socket].అంకె_harts;

            ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_NONE) {
                create_fdt_socket_plic(s, socket, phandle,
                                       &intc_phandles[phandle_pos],
                                       xplic_phandles);
            } లేకపోతే {
                aplic_props.socket = socket;
                aplic_props.అంకె_harts = s->soc[socket].అంకె_harts;
                riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                              msi_m_phandle, msi_s_phandle,
                                              phandle,
                                              &intc_phandles[phandle_pos],
                                              xplic_phandles);
            }
        }

        కొరకు (socket = 0; socket < socket_లెక్క; socket++) {
            ఒకవేళ (socket == 0) {
                *irq_mmio_phandle = xplic_phandles[socket];
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            ఒకవేళ (socket == 1) {
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            ఒకవేళ (socket == 2) {
                *irq_pcie_phandle = xplic_phandles[socket];
            }
        }
    }

    riscv_socket_fdt_write_దూరం_మాతృక(ms);
}

స్థిర శూన్య create_fdt_virtio(RISCVVirtState *s, uint32_t irq_virtio_phandle)
{
    పూర్ణాంకం i;
    MachineState *ms = MACHINE(s);
    hwaddr virtio_పునాది = s->memmap[VIRT_VIRTIO].పునాది;

    కొరకు (i = 0; i < VIRTIO_లెక్క; i++) {
        g_autofree అక్షరం *name = ఖాళీ;
        uint64_t కొలత = s->memmap[VIRT_VIRTIO].కొలత;
        hwaddr addr = virtio_పునాది + i * కొలత;

        name = g_strdup_printf("/soc/virtio_mmio@%"HWADDR_PRIx, addr);

        qemu_fdt_కూడు_subnode(ms->fdt, name);
        qemu_fdt_setprop_string(ms->fdt, name, "compatible", "virtio,mmio");
        qemu_fdt_setprop_sized_cells(ms->fdt, name, "reg", 2, addr, 2, కొలత);
        qemu_fdt_setprop_cell(ms->fdt, name, "interrupt-parent",
            irq_virtio_phandle);
        ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_NONE) {
            qemu_fdt_setprop_cell(ms->fdt, name, "interrupts",
                                  VIRTIO_IRQ + i);
        } లేకపోతే {
            qemu_fdt_setprop_cells(ms->fdt, name, "interrupts",
                                   VIRTIO_IRQ + i, 0x4);
        }
    }
}

స్థిర శూన్య create_fdt_uarts(RISCVVirtState *s, uint32_t irq_mmio_phandle)
{
    g_autofree అక్షరం *name = ఖాళీ;
    MachineState *ms = MACHINE(s);

    ఒకవేళ (s->uart1_present) {
        riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART1],
                              UART1_IRQ, s->aia_type, irq_mmio_phandle);
    }

    riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART0], UART0_IRQ,
                          s->aia_type, irq_mmio_phandle);

    name = riscv_fdt_get_uart_nodename(s->memmap[VIRT_UART0].పునాది);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);
}

స్థిర శూన్య create_fdt_fw_cfg(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    hwaddr పునాది = s->memmap[VIRT_FW_CFG].పునాది;
    hwaddr కొలత = s->memmap[VIRT_FW_CFG].కొలత;
    g_autofree అక్షరం *nodename = g_strdup_printf("/fw-cfg@%" PRIx64, పునాది);

    qemu_fdt_కూడు_subnode(ms->fdt, nodename);
    qemu_fdt_setprop_string(ms->fdt, nodename,
                            "compatible", "qemu,fw-cfg-mmio");
    qemu_fdt_setprop_sized_cells(ms->fdt, nodename, "reg",
                                 2, పునాది, 2, కొలత);
    qemu_fdt_setprop(ms->fdt, nodename, "dma-coherent", ఖాళీ, 0);
}

స్థిర శూన్య create_fdt_virtio_iommu(RISCVVirtState *s, uint16_t bdf)
{
    స్థిరరాశి అక్షరం compat[] = "virtio,pci-iommu\0pci1af4,1057";
    శూన్య *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree అక్షరం *iommu_కణుపు = ఖాళీ;
    g_autofree అక్షరం *pci_కణుపు = ఖాళీ;

    pci_కణుపు = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].పునాది);
    iommu_కణుపు = g_strdup_printf("%s/virtio_iommu@%x,%x", pci_కణుపు,
                                 PCI_స్థానం(bdf), PCI_FUNC(bdf));
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);

    qemu_fdt_కూడు_subnode(fdt, iommu_కణుపు);

    qemu_fdt_setprop(fdt, iommu_కణుపు, "compatible", compat, పరిమాణం(compat));
    qemu_fdt_setprop_sized_cells(fdt, iommu_కణుపు, "reg",
                                 1, bdf << 8, 1, 0, 1, 0,
                                 1, 0, 1, 0);
    qemu_fdt_setprop_cell(fdt, iommu_కణుపు, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_కణుపు, "phandle", iommu_phandle);

    qemu_fdt_setprop_cells(fdt, pci_కణుపు, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
}

స్థిర శూన్య create_fdt_iommu(RISCVVirtState *s, uint16_t bdf)
{
    స్థిరరాశి అక్షరం comp[] = "riscv,pci-iommu";
    శూన్య *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree అక్షరం *iommu_కణుపు = ఖాళీ;
    g_autofree అక్షరం *pci_కణుపు = ఖాళీ;

    pci_కణుపు = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].పునాది);
    iommu_కణుపు = g_strdup_printf("%s/iommu@%x", pci_కణుపు, bdf);
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_కూడు_subnode(fdt, iommu_కణుపు);

    qemu_fdt_setprop(fdt, iommu_కణుపు, "compatible", comp, పరిమాణం(comp));
    qemu_fdt_setprop_cell(fdt, iommu_కణుపు, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_కణుపు, "phandle", iommu_phandle);
    qemu_fdt_setprop_cells(fdt, iommu_కణుపు, "reg",
                           bdf << 8, 0, 0, 0, 0);
    qemu_fdt_setprop_cells(fdt, pci_కణుపు, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
    s->pci_iommu_bdf = bdf;
}

స్థిర శూన్య finalize_fdt(RISCVVirtState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, irq_virtio_phandle = 1;
    uint32_t iommu_sys_phandle = 1;

    create_fdt_sockets(s, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &irq_virtio_phandle,
                       &msi_pcie_phandle);

    create_fdt_virtio(s, irq_virtio_phandle);

    ఒకవేళ (virt_is_iommu_sys_enabled(s)) {
        iommu_sys_phandle =
            riscv_create_fdt_riscv_iommu_sys(MACHINE(s)->fdt,
                                             s->memmap[VIRT_IOMMU_SYS].పునాది,
                                             s->memmap[VIRT_IOMMU_SYS].కొలత,
                                             &phandle,
                                             irq_mmio_phandle,
                                             msi_pcie_phandle,
                                             IOMMU_SYS_IRQ);
    }

    riscv_create_fdt_pcie(MACHINE(s)->fdt, s->aia_type,
                          virt_is_iommu_sys_enabled(s),
                          &s->memmap[VIRT_PCIE_ECAM],
                          &s->memmap[VIRT_PCIE_PIO],
                          &s->memmap[VIRT_PCIE_MMIO],
                          &virt_ఎగువ_pcie_memmap,
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, PCIE_IRQ);

    riscv_create_fdt_syscon(MACHINE(s)->fdt, &phandle,
                            s->memmap[VIRT_TEST].పునాది,
                            s->memmap[VIRT_TEST].కొలత,
                            FINISHER_RESET, FINISHER_PASS, true);

    create_fdt_uarts(s, irq_mmio_phandle);

    riscv_create_fdt_rtc(MACHINE(s)->fdt, &s->memmap[VIRT_RTC], RTC_IRQ,
                         s->aia_type, irq_mmio_phandle);
}

స్థిర శూన్య create_fdt(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_విత్తనం[32];
    g_autofree అక్షరం *name = ఖాళీ;

    ms->fdt = riscv_create_board_device_వృక్షం("riscv-virtio,qemu",
                                             "riscv-virtio",
                                             &s->fdt_కొలత);

    /*
     * The "/soc/pci@..." node is needed for PCIE hotplugs
     * that might happen before finalize_fdt().
     */
    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                           s->memmap[VIRT_PCIE_ECAM].పునాది);
    qemu_fdt_కూడు_subnode(ms->fdt, name);

    qemu_fdt_కూడు_subnode(ms->fdt, "/chosen");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_విత్తనం, పరిమాణం(rng_విత్తనం));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_విత్తనం, పరిమాణం(rng_విత్తనం));

    qemu_fdt_కూడు_subnode(ms->fdt, "/aliases");

    riscv_create_fdt_flash(ms->fdt, s->memmap[VIRT_FLASH].పునాది,
                           s->memmap[VIRT_FLASH].కొలత / 2);
    create_fdt_fw_cfg(s);
    create_fdt_pmu(s);
}

స్థిర FWCfgState *create_fw_cfg(స్థిరరాశి MachineState *ms, hwaddr పునాది)
{
    FWCfgState *fw_cfg;

    fw_cfg = fw_cfg_init_mem_dma(పునాది, &చిరునామా_space_జ్ఞాపకం);
    fw_cfg_కూడు_i16(fw_cfg, FW_CFG_NB_CPUS, (uint16_t)ms->smp.cpus);

    తిరిగి fw_cfg;
}

స్థిర DeviceState *virt_create_plic(స్థిరరాశి Mem_Map_నమోదు *memmap, పూర్ణాంకం socket,
                                     పూర్ణాంకం పునాది_hartid, పూర్ణాంకం hart_లెక్క)
{
    g_autofree అక్షరం *plic_hart_config = ఖాళీ;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hart_లెక్క);

    /* Per-socket PLIC */
    తిరిగి sifive_plic_create(
             get_system_జ్ఞాపకం(),
             memmap[VIRT_PLIC].పునాది + socket * memmap[VIRT_PLIC].కొలత,
             plic_hart_config, hart_లెక్క, పునాది_hartid,
             VIRT_IRQCHIP_అంకె_SOURCES,
             ((1U << VIRT_IRQCHIP_అంకె_PRIO_బిట్లు) - 1),
             VIRT_PLIC_ప్రాధాన్యత_పునాది, VIRT_PLIC_PENDING_పునాది,
             VIRT_PLIC_ENABLE_పునాది, VIRT_PLIC_ENABLE_STRIDE,
             VIRT_PLIC_CONTEXT_పునాది,
             VIRT_PLIC_CONTEXT_STRIDE,
             memmap[VIRT_PLIC].కొలత);
}

స్థిర శూన్య virt_build_smbios(RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    uint8_t *smbios_tables, *smbios_anchor;
    కొలత_t smbios_tables_len, smbios_anchor_len;
    నిర్మాణం smbios_phys_mem_area mem_array;
    స్థిరరాశి అక్షరం *లబ్ధం = "QEMU Virtual Machine";

    ఒకవేళ (kvm_enabled()) {
        లబ్ధం = "KVM Virtual Machine";
    }

    smbios_సమితి_defaults("QEMU", లబ్ధం, mc->name);

    ఒకవేళ (riscv_is_32bit(&s->soc[0])) {
        smbios_సమితి_default_processor_family(0x200);
    } లేకపోతే {
        smbios_సమితి_default_processor_family(0x201);
    }

    /* build the array of physical mem area from base_memmap */
    mem_array.చిరునామా = s->memmap[VIRT_DRAM].పునాది;
    mem_array.నిడివి = ms->ram_కొలత;

    smbios_get_tables(ms, SMBIOS_నమోదు_బిందువు_TYPE_64,
                      &mem_array, 1,
                      &smbios_tables, &smbios_tables_len,
                      &smbios_anchor, &smbios_anchor_len,
                      &error_fatal);

    ఒకవేళ (smbios_anchor) {
        fw_cfg_కూడు_file(s->fw_cfg, "etc/smbios/smbios-tables",
                        smbios_tables, smbios_tables_len);
        fw_cfg_కూడు_file(s->fw_cfg, "etc/smbios/smbios-anchor",
                        smbios_anchor, smbios_anchor_len);
    }
}

స్థిర శూన్య virt_machine_పూర్తి(Notifier *notifier, శూన్య *దత్తాంశం)
{
    RISCVVirtState *s = container_యొక్క(notifier, RISCVVirtState,
                                     machine_పూర్తి);
    MachineState *machine = MACHINE(s);
    hwaddr ప్రారంభం_addr = s->memmap[VIRT_DRAM].పునాది;
    hwaddr firmware_చివర_addr;
    vaddr kernel_ప్రారంభం_addr;
    స్థిరరాశి అక్షరం *firmware_name = riscv_default_firmware_name(&s->soc[0]);
    uint64_t fdt_ఎక్కించు_addr;
    uint64_t kernel_నమోదు = 0;
    ఖండం_Backend *pflash_blk0;
    RISCVBootInfo boot_info;

    /*
     * An user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    ఒకవేళ (machine->dtb == ఖాళీ) {
        finalize_fdt(s);
    }

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * so the "-bios" parameter is not supported when KVM is enabled.
     */
    ఒకవేళ (kvm_enabled()) {
        ఒకవేళ (machine->firmware) {
            ఒకవేళ (strcmp(machine->firmware, "none")) {
                error_report("Machine mode firmware is not supported in "
                             "combination with KVM.");
                నిష్క్రమించు(1);
            }
        } లేకపోతే {
            machine->firmware = g_strdup("none");
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_చివర_addr = riscv_కనుగొను_and_ఎక్కించు_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ప్రారంభం_addr, ఖాళీ);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    ఒకవేళ (pflash_blk0) {
        ఒకవేళ (machine->firmware && !strcmp(machine->firmware, "none") &&
            !kvm_enabled()) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            ప్రారంభం_addr = s->memmap[VIRT_FLASH].పునాది;
        } లేకపోతే {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_అమరిక_firmware_boot(machine);
            kernel_నమోదు = s->memmap[VIRT_FLASH].పునాది;
        }
    }

    ఒకవేళ (machine->kernel_filename && !kernel_నమోదు) {
        kernel_ప్రారంభం_addr = riscv_calc_kernel_ప్రారంభం_addr(&boot_info,
                                                         firmware_చివర_addr);
        riscv_ఎక్కించు_kernel(machine, &boot_info, kernel_ప్రారంభం_addr,
                          true, ఖాళీ);
        kernel_నమోదు = boot_info.image_దిగువ_addr;
    }

    fdt_ఎక్కించు_addr = riscv_compute_fdt_addr(s->memmap[VIRT_DRAM].పునాది,
                                           s->memmap[VIRT_DRAM].కొలత,
                                           machine, &boot_info);
    riscv_ఎక్కించు_fdt(fdt_ఎక్కించు_addr, machine->fdt);

    /* load the reset vector */
    riscv_అమరిక_rom_reset_vec(machine, &s->soc[0], ప్రారంభం_addr,
                              s->memmap[VIRT_MROM].పునాది,
                              s->memmap[VIRT_MROM].కొలత, kernel_నమోదు,
                              fdt_ఎక్కించు_addr);

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * So here setup kernel start address and fdt address.
     * TODO:Support firmware loading and integrate to TCG start
     */
    ఒకవేళ (kvm_enabled()) {
        riscv_అమరిక_ప్రత్యక్ష_kernel(kernel_నమోదు, fdt_ఎక్కించు_addr);
    }

    virt_build_smbios(s);

    ఒకవేళ (virt_is_acpi_enabled(s)) {
        virt_acpi_అమరిక(s);
    }
}

స్థిర శూన్య virt_machine_init(MachineState *machine)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    జ్ఞాపకం_Region *ముసుగు_rom = g_new(జ్ఞాపకం_Region, 1);
    DeviceState *mmio_irqchip, *virtio_irqchip, *pcie_irqchip;
    పూర్ణాంకం i, పునాది_hartid, hart_లెక్క;
    పూర్ణాంకం socket_లెక్క = riscv_socket_లెక్క(machine);

    s->memmap = virt_memmap;

    /* Check socket count limit */
    ఒకవేళ (VIRT_SOCKETS_MAX < socket_లెక్క) {
        error_report("number of sockets/nodes should be less than %d",
            VIRT_SOCKETS_MAX);
        నిష్క్రమించు(1);
    }

    ఒకవేళ (!virt_aclint_allowed() && s->have_aclint) {
        error_report("'aclint' is only available with TCG acceleration");
        నిష్క్రమించు(1);
    }

    /* Initialize sockets */
    mmio_irqchip = virtio_irqchip = pcie_irqchip = ఖాళీ;
    కొరకు (i = 0; i < socket_లెక్క; i++) {
        g_autofree అక్షరం *soc_name = g_strdup_printf("soc%d", i);

        ఒకవేళ (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            నిష్క్రమించు(1);
        }

        పునాది_hartid = riscv_socket_మొదటి_hartid(machine, i);
        ఒకవేళ (పునాది_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            నిష్క్రమించు(1);
        }

        hart_లెక్క = riscv_socket_hart_లెక్క(machine, i);
        ఒకవేళ (hart_లెక్క < 0) {
            error_report("can't find hart count for socket%d", i);
            నిష్క్రమించు(1);
        }

        object_initialize_బిడ్డ(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        object_property_సమితి_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_సమితి_int(OBJECT(&s->soc[i]), "hartid-base",
                                పునాది_hartid, &error_abort);
        object_property_సమితి_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_లెక్క, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        ఒకవేళ (virt_aclint_allowed() && s->have_aclint) {
            ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
                /* Per-socket ACLINT MTIMER */
                riscv_aclint_mtimer_create(system_జ్ఞాపకం,
                        s->memmap[VIRT_CLINT].పునాది +
                            i * RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                        RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                        పునాది_hartid, hart_లెక్క,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
            } లేకపోతే {
                /* Per-socket ACLINT MSWI, MTIMER, and SSWI */
                riscv_aclint_swi_create(system_జ్ఞాపకం,
                        s->memmap[VIRT_CLINT].పునాది +
                            i * s->memmap[VIRT_CLINT].కొలత,
                        పునాది_hartid, hart_లెక్క, false);
                riscv_aclint_mtimer_create(system_జ్ఞాపకం,
                        s->memmap[VIRT_CLINT].పునాది +
                            i * s->memmap[VIRT_CLINT].కొలత +
                            RISCV_ACLINT_SWI_కొలత,
                        RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                        పునాది_hartid, hart_లెక్క,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
                riscv_aclint_swi_create(system_జ్ఞాపకం,
                        s->memmap[VIRT_ACLINT_SSWI].పునాది +
                            i * s->memmap[VIRT_ACLINT_SSWI].కొలత,
                        పునాది_hartid, hart_లెక్క, true);
            }
        } లేకపోతే ఒకవేళ (tcg_enabled()) {
            /* Per-socket SiFive CLINT */
            riscv_aclint_swi_create(system_జ్ఞాపకం,
                    s->memmap[VIRT_CLINT].పునాది + i * s->memmap[VIRT_CLINT].కొలత,
                    పునాది_hartid, hart_లెక్క, false);
            riscv_aclint_mtimer_create(system_జ్ఞాపకం,
                    s->memmap[VIRT_CLINT].పునాది +
                    i * s->memmap[VIRT_CLINT].కొలత + RISCV_ACLINT_SWI_కొలత,
                    RISCV_ACLINT_DEFAULT_MTIMER_కొలత, పునాది_hartid, hart_లెక్క,
                    RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
                    RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
        }

        /* Per-socket interrupt controller */
        ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_NONE) {
            s->irqchip[i] = virt_create_plic(s->memmap, i,
                                             పునాది_hartid, hart_లెక్క);
        } లేకపోతే {
            bool msimode = s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;
            పూర్ణాంకం imsic_బిట్లు = imsic_అంకె_బిట్లు(s->aia_guests + 1);
            s->irqchip[i] = riscv_create_aia(system_జ్ఞాపకం, msimode,
                                             s->aia_guests,
                                             IMSIC_HART_కొలత(0),
                                             IMSIC_HART_కొలత(imsic_బిట్లు),
                                             s->అంకె_sources,
                                             &s->memmap[VIRT_APLIC_M],
                                             &s->memmap[VIRT_APLIC_S],
                                             &s->memmap[VIRT_IMSIC_M],
                                             &s->memmap[VIRT_IMSIC_S],
                                             i, పునాది_hartid, hart_లెక్క,
                                             VIRT_IRQCHIP_అంకె_MSIS,
                                             VIRT_IRQCHIP_అంకె_PRIO_బిట్లు);
        }

        /* Try to use different IRQCHIP instance based device type */
        ఒకవేళ (i == 0) {
            mmio_irqchip = s->irqchip[i];
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ఒకవేళ (i == 1) {
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ఒకవేళ (i == 2) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    ఒకవేళ (kvm_enabled() && virt_use_kvm_aia_aplic_imsic(s->aia_type)) {
        kvm_riscv_aia_create(machine, IMSIC_MMIO_సమూహం_MIN_జరుపు,
                             VIRT_IRQCHIP_అంకె_SOURCES, VIRT_IRQCHIP_అంకె_MSIS,
                             s->memmap[VIRT_APLIC_S].పునాది,
                             s->memmap[VIRT_IMSIC_S].పునాది,
                             s->aia_guests);
    }

    ఒకవేళ (riscv_is_32bit(&s->soc[0])) {
#ఒకవేళ HOST_LONG_బిట్లు == 64
        /* limit RAM size in a 32-bit system */
        ఒకవేళ (machine->ram_కొలత > 10 * GiB) {
            machine->ram_కొలత = 10 * GiB;
            error_report("Limiting RAM size to 10 GiB");
        }
#అయితే_ముగింపు
        virt_ఎగువ_pcie_memmap.పునాది = VIRT32_ఎగువ_PCIE_MMIO_పునాది;
        virt_ఎగువ_pcie_memmap.కొలత = VIRT32_ఎగువ_PCIE_MMIO_కొలత;
    } లేకపోతే {
        virt_ఎగువ_pcie_memmap.కొలత = VIRT64_ఎగువ_PCIE_MMIO_కొలత;
        virt_ఎగువ_pcie_memmap.పునాది = s->memmap[VIRT_DRAM].పునాది +
                                     machine->ram_కొలత;
        virt_ఎగువ_pcie_memmap.పునాది =
            విడత_పైకి(virt_ఎగువ_pcie_memmap.పునాది, virt_ఎగువ_pcie_memmap.కొలత);
    }

    /* register system main memory (actual RAM) */
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, s->memmap[VIRT_DRAM].పునాది,
                                machine->ram);

    /* boot rom */
    జ్ఞాపకం_region_init_rom(ముసుగు_rom, ఖాళీ, "riscv_virt_board.mrom",
                           s->memmap[VIRT_MROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, s->memmap[VIRT_MROM].పునాది,
                                ముసుగు_rom);

    /*
     * Init fw_cfg. Must be done before riscv_load_fdt, otherwise the
     * device tree cannot be altered and we get FDT_ERR_NOSPACE.
     */
    s->fw_cfg = create_fw_cfg(machine, s->memmap[VIRT_FW_CFG].పునాది);
    rom_సమితి_fw(s->fw_cfg);

    /* SiFive Test MMIO device */
    sifive_test_create(s->memmap[VIRT_TEST].పునాది);

    /* VirtIO MMIO devices */
    కొరకు (i = 0; i < VIRTIO_లెక్క; i++) {
        sysbus_create_simple("virtio-mmio",
            s->memmap[VIRT_VIRTIO].పునాది + i * s->memmap[VIRT_VIRTIO].కొలత,
            qdev_get_gpio_లోపల(virtio_irqchip, VIRTIO_IRQ + i));
    }

    riscv_gpex_pcie_init(system_జ్ఞాపకం, pcie_irqchip,
                   &s->memmap[VIRT_PCIE_ECAM], &s->memmap[VIRT_PCIE_MMIO],
                   &virt_ఎగువ_pcie_memmap, &s->memmap[VIRT_PCIE_PIO],
                   PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
        &s->memmap[VIRT_PLATFORM_BUS], VIRT_PLATFORM_BUS_IRQ,
        VIRT_PLATFORM_BUS_అంకె_IRQS);

    serial_mm_init(system_జ్ఞాపకం, s->memmap[VIRT_UART0].పునాది,
        0, qdev_get_gpio_లోపల(mmio_irqchip, UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    ఒకవేళ (serial_hd(1)) {
        serial_mm_init(system_జ్ఞాపకం, s->memmap[VIRT_UART1].పునాది,
            0, qdev_get_gpio_లోపల(mmio_irqchip, UART1_IRQ), 399193,
            serial_hd(1), DEVICE_LITTLE_ENDIAN);
        s->uart1_present = true;
    }

    sysbus_create_simple("goldfish_rtc", s->memmap[VIRT_RTC].పునాది,
        qdev_get_gpio_లోపల(mmio_irqchip, RTC_IRQ));

    కొరకు (i = 0; i < ARRAY_కొలత(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    virt_flash_map(s, system_జ్ఞాపకం);

    /* load/create device tree */
    ఒకవేళ (machine->dtb) {
        machine->fdt = ఎక్కించు_device_వృక్షం(machine->dtb, &s->fdt_కొలత);
        ఒకవేళ (!machine->fdt) {
            error_report("load_device_tree() failed");
            నిష్క్రమించు(1);
        }
    } లేకపోతే {
        create_fdt(s);
    }

    ఒకవేళ (virt_is_iommu_sys_enabled(s)) {
        riscv_create_iommu_sys(mmio_irqchip, s->memmap[VIRT_IOMMU_SYS].పునాది,
                               IOMMU_SYS_IRQ, riscv_is_32bit(&s->soc[0]));
    }

    s->machine_పూర్తి.notify = virt_machine_పూర్తి;
    qemu_కూడు_machine_init_పూర్తి_notifier(&s->machine_పూర్తి);
}

స్థిర శూన్య virt_machine_instance_finalize(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    కొరకు (పూర్ణాంకం i = 0; i < ARRAY_కొలత(s->flash); i++) {
        ఒకవేళ (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
    g_free(s->oem_id);
    g_free(s->oem_పట్టిక_id);
}

స్థిర శూన్య virt_machine_instance_init(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    virt_flash_create(s);

    s->oem_id = g_strndup(ACPI_BUILD_APPNAME6, 6);
    s->oem_పట్టిక_id = g_strndup(ACPI_BUILD_APPNAME8, 8);
    s->acpi = ON_OFF_AUTO_AUTO;
    s->iommu_sys = ON_OFF_AUTO_AUTO;
    s->అంకె_sources = VIRT_IRQCHIP_అంకె_SOURCES;
}

స్థిర అక్షరం *virt_get_aia_guests(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    తిరిగి g_strdup_printf("%d", s->aia_guests);
}

స్థిర శూన్య virt_సమితి_aia_guests(Object *obj, స్థిరరాశి అక్షరం *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->aia_guests = atoi(val);
    ఒకవేళ (s->aia_guests < 0 || s->aia_guests > VIRT_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          VIRT_IRQCHIP_MAX_GUESTS);
    }
}

స్థిర అక్షరం *virt_get_aia(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    స్థిరరాశి అక్షరం *val;

    ఎంపిక (s->aia_type) {
    స్థితి VIRT_AIA_TYPE_APLIC:
        val = "aplic";
        ఆపు;
    స్థితి VIRT_AIA_TYPE_APLIC_IMSIC:
        val = "aplic-imsic";
        ఆపు;
    అప్రమేయ:
        val = "none";
        ఆపు;
    };

    తిరిగి g_strdup(val);
}

స్థిర శూన్య virt_సమితి_aia(Object *obj, స్థిరరాశి అక్షరం *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ఒకవేళ (!strcmp(val, "none")) {
        s->aia_type = VIRT_AIA_TYPE_NONE;
    } లేకపోతే ఒకవేళ (!strcmp(val, "aplic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC;
    } లేకపోతే ఒకవేళ (!strcmp(val, "aplic-imsic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC_IMSIC;
    } లేకపోతే {
        error_setg(errp, "Invalid AIA interrupt controller type");
        error_append_hint(errp, "Valid values are none, aplic, and "
                          "aplic-imsic.\n");
    }
}

స్థిర bool virt_get_aclint(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    తిరిగి s->have_aclint;
}

స్థిర శూన్య virt_సమితి_aclint(Object *obj, bool విలువ, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->have_aclint = విలువ;
}

bool virt_is_iommu_sys_enabled(RISCVVirtState *s)
{
    తిరిగి s->iommu_sys == ON_OFF_AUTO_ON;
}

స్థిర శూన్య virt_get_iommu_sys(Object *obj, Visitor *v, స్థిరరాశి అక్షరం *name,
                               శూన్య *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto iommu_sys = s->iommu_sys;

    సందర్శించు_type_On_Off_Auto(v, name, &iommu_sys, errp);
}

స్థిర శూన్య virt_సమితి_iommu_sys(Object *obj, Visitor *v, స్థిరరాశి అక్షరం *name,
                               శూన్య *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    సందర్శించు_type_On_Off_Auto(v, name, &s->iommu_sys, errp);
}

bool virt_is_acpi_enabled(RISCVVirtState *s)
{
    తిరిగి s->acpi != ON_OFF_AUTO_OFF;
}

స్థిర శూన్య virt_get_acpi(Object *obj, Visitor *v, స్థిరరాశి అక్షరం *name,
                          శూన్య *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto acpi = s->acpi;

    సందర్శించు_type_On_Off_Auto(v, name, &acpi, errp);
}

స్థిర శూన్య virt_సమితి_acpi(Object *obj, Visitor *v, స్థిరరాశి అక్షరం *name,
                          శూన్య *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    సందర్శించు_type_On_Off_Auto(v, name, &s->acpi, errp);
}

స్థిర స్థిరరాశి HotplugHandler *
virt_machine_get_hotplug_handler(MachineState *machine, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);

    ఒకవేళ (device_is_dynamic_sysbus(mc, dev) ||
        object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI) ||
        object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        s->iommu_sys = ON_OFF_AUTO_OFF;
        తిరిగి HOTPLUG_HANDLER(machine);
    }

    తిరిగి ఖాళీ;
}

స్థిర శూన్య virt_machine_device_plug_cb(స్థిరరాశి HotplugHandler *hotplug_dev,
                                        DeviceState *dev, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(hotplug_dev);

    ఒకవేళ (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        ఒకవేళ (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }

    ఒకవేళ (object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI)) {
        create_fdt_virtio_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
    }

    ఒకవేళ (object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        create_fdt_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
        s->iommu_sys = ON_OFF_AUTO_OFF;
    }
}

స్థిర శూన్య virt_machine_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);

    mc->desc = "RISC-V VirtIO board";
    mc->init = virt_machine_init;
    mc->max_cpus = VIRT_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_పునాది;
    mc->ఖండం_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->pci_allow_0_చిరునామా = true;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_సూచీ_గమ్యం_instance_props = riscv_numa_cpu_సూచీ_గమ్యం_props;
    mc->get_default_cpu_కణుపు_id = riscv_numa_get_default_cpu_కణుపు_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_virt_board.ram";
    ప్రతిజ్ఞా(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = virt_machine_get_hotplug_handler;

    hc->plug = virt_machine_device_plug_cb;

    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_RAMFB_DEVICE);
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_UEFI_VARS_SYSBUS);
#నిర్వచితమైతే CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#అయితే_ముగింపు

    object_class_property_కూడు_bool(oc, "aclint", virt_get_aclint,
                                   virt_సమితి_aclint);
    object_class_property_సమితి_description(oc, "aclint",
                                          "(TCG only) Set on/off to "
                                          "enable/disable emulating "
                                          "ACLINT devices");

    object_class_property_కూడు_str(oc, "aia", virt_get_aia,
                                  virt_సమితి_aia);
    object_class_property_సమితి_description(oc, "aia",
                                          "Set type of AIA interrupt "
                                          "controller. Valid values are "
                                          "none, aplic, and aplic-imsic.");

    object_class_property_కూడు_str(oc, "aia-guests",
                                  virt_get_aia_guests,
                                  virt_సమితి_aia_guests);
    {
        g_autofree అక్షరం *str =
            g_strdup_printf("Set number of guest MMIO pages for AIA IMSIC. "
                            "Valid value should be between 0 and %d.",
                            VIRT_IRQCHIP_MAX_GUESTS);
        object_class_property_సమితి_description(oc, "aia-guests", str);
    }

    object_class_property_కూడు(oc, "acpi", "OnOffAuto",
                              virt_get_acpi, virt_సమితి_acpi,
                              ఖాళీ, ఖాళీ);
    object_class_property_సమితి_description(oc, "acpi",
                                          "Enable ACPI");

    object_class_property_కూడు(oc, "iommu-sys", "OnOffAuto",
                              virt_get_iommu_sys, virt_సమితి_iommu_sys,
                              ఖాళీ, ఖాళీ);
    object_class_property_సమితి_description(oc, "iommu-sys",
                                          "Enable IOMMU platform device");
}

స్థిర స్థిరరాశి TypeInfo virt_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("virt"),
    .మాతృ     = TYPE_MACHINE,
    .class_init = virt_machine_class_init,
    .instance_init = virt_machine_instance_init,
    .instance_finalize = virt_machine_instance_finalize,
    .instance_కొలత = పరిమాణం(RISCVVirtState),
    .interfaces = (స్థిరరాశి InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

స్థిర శూన్య virt_machine_init_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&virt_machine_typeinfo);
}

type_init(virt_machine_init_రిజిస్టరు_types)
