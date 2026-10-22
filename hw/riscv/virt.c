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

#অন্তর্ভাব "qemu/osdep.h"
#অন্তর্ভাব "qemu/units.h"
#অন্তর্ভাব "qemu/error-report.h"
#অন্তর্ভাব "qemu/guest-random.h"
#অন্তর্ভাব "qapi/error.h"
#অন্তর্ভাব "hw/core/boards.h"
#অন্তর্ভাব "hw/core/loader.h"
#অন্তর্ভাব "hw/core/sysbus.h"
#অন্তর্ভাব "hw/core/qdev-properties.h"
#অন্তর্ভাব "hw/char/serial-mm.h"
#অন্তর্ভাব "target/riscv/cpu.h"
#অন্তর্ভাব "hw/core/sysbus-fdt.h"
#অন্তর্ভাব "hw/riscv/riscv_hart.h"
#অন্তর্ভাব "hw/riscv/iommu.h"
#অন্তর্ভাব "hw/riscv/riscv-iommu.h"
#অন্তর্ভাব "hw/riscv/riscv-iommu-bits.h"
#অন্তর্ভাব "hw/riscv/virt.h"
#অন্তর্ভাব "hw/riscv/boot.h"
#অন্তর্ভাব "hw/riscv/device-common.h"
#অন্তর্ভাব "hw/riscv/fdt-common.h"
#অন্তর্ভাব "hw/riscv/numa.h"
#অন্তর্ভাব "kvm/kvm_riscv.h"
#অন্তর্ভাব "hw/firmware/smbios.h"
#অন্তর্ভাব "hw/intc/riscv_aclint.h"
#অন্তর্ভাব "hw/intc/riscv_aplic.h"
#অন্তর্ভাব "hw/intc/sifive_plic.h"
#অন্তর্ভাব "hw/misc/sifive_test.h"
#অন্তর্ভাব "hw/core/platform-bus.h"
#অন্তর্ভাব "chardev/char.h"
#অন্তর্ভাব "system/device_tree.h"
#অন্তর্ভাব "system/system.h"
#অন্তর্ভাব "system/tcg.h"
#অন্তর্ভাব "system/kvm.h"
#অন্তর্ভাব "system/tpm.h"
#অন্তর্ভাব "system/qtest.h"
#অন্তর্ভাব "hw/pci/pci.h"
#অন্তর্ভাব "hw/pci-host/gpex.h"
#অন্তর্ভাব "hw/display/ramfb.h"
#অন্তর্ভাব "hw/acpi/aml-build.h"
#অন্তর্ভাব "qapi/qapi-visit-common.h"
#অন্তর্ভাব "hw/virtio/virtio-iommu.h"
#অন্তর্ভাব "hw/uefi/var-service-api.h"

#অন্তর্ভাব "aia.h"

/* KVM AIA only supports APLIC MSI. APLIC Wired is always emulated by QEMU. */
স্থির bool virt_use_kvm_aia_aplic_imsic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    ফেরত riscv_is_kvm_aia_aplic_imsic(msimode);
}

স্থির bool virt_use_emulated_aplic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    ফেরত riscv_use_emulated_aplic(msimode);
}

স্থির bool virt_aclint_allowed(শূন্য)
{
    ফেরত tcg_enabled() || qtest_enabled();
}

স্থির ধ্রুবক MemMapEntry virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_IOMMU_SYS] =    {  0x3010000,        0x1000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_আকার(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_আকার(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_আকার(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    /* UART1 supports page isolation from UART0 */
    [VIRT_UART1] =        { 0x1000a000,         0x100 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_আকার },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_আকার },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    [VIRT_DRAM] =         { 0x80000000,           0x0 },
};

/* PCIe high mmio is fixed for RV32 */
#define VIRT32_উপরের_PCIE_MMIO_BASE  0x300000000ULL
#define VIRT32_উপরের_PCIE_MMIO_আকার  (4 * GiB)

/* PCIe high mmio for RV64, size is fixed but base depends on top of RAM */
#define VIRT64_উপরের_PCIE_MMIO_আকার  (16 * GiB)

স্থির MemMapEntry virt_উপরের_pcie_memmap;

#define VIRT_FLASH_SECTOR_আকার (256 * KiB)

স্থির শূন্য virt_flash_create(RISCVVirtState *s)
{
    s->flash[0] = riscv_flash_create(OBJECT(s), "virt.flash0", "pflash0",
                                     VIRT_FLASH_SECTOR_আকার);
    s->flash[1] = riscv_flash_create(OBJECT(s), "virt.flash1", "pflash1",
                                     VIRT_FLASH_SECTOR_আকার);
}

স্থির শূন্য virt_flash_map(RISCVVirtState *s,
                           MemoryRegion *sysmem)
{
    hwaddr flashsize = s->memmap[VIRT_FLASH].আকার_2 / 2;
    hwaddr flashbase = s->memmap[VIRT_FLASH].base;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_আকার);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_আকার);
}

স্থির শূন্য create_fdt_socket_plic(RISCVVirtState *s,
                                   পূর্ণসংখ্যা socket,
                                   uint32_t *phandle, uint32_t *intc_phandles,
                                   uint32_t *plic_phandles)
{
    পূর্ণসংখ্যা cpu;
    g_autofree অক্ষর *plic_name = ফাঁকা;
    g_autofree uint32_t *plic_cells;
    MachineState *ms = MACHINE(s);
    unsigned long plic_addr = s->memmap[VIRT_PLIC].base +
                              (s->memmap[VIRT_PLIC].আকার_2 * socket);
    bool numa_enabled = riscv_numa_enabled(MACHINE(s));
    uint32_t cells_দৈর্ঘ্য;

    plic_name = g_strdup_printf("/soc/interrupt-controller@%lx", plic_addr);

    যদি (kvm_enabled()) {
        cells_দৈর্ঘ্য = s->soc[socket].অঙ্ক_harts * 2;
        plic_cells = g_new0(uint32_t, cells_দৈর্ঘ্য);

        জন্য (cpu = 0; cpu < s->soc[socket].অঙ্ক_harts; cpu++) {
            plic_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
            plic_cells[cpu * 2 + 1] = cpu_to_be32(IRQ_S_EXT);
        }
   } অন্যথা {
        cells_দৈর্ঘ্য = s->soc[socket].অঙ্ক_harts * 4;
        plic_cells = g_new0(uint32_t, cells_দৈর্ঘ্য);

        জন্য (cpu = 0; cpu < s->soc[socket].অঙ্ক_harts; cpu++) {
            plic_cells[cpu * 4 + 0] = cpu_to_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 1] = cpu_to_be32(IRQ_M_EXT);
            plic_cells[cpu * 4 + 2] = cpu_to_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 3] = cpu_to_be32(IRQ_S_EXT);
        }
    }

    plic_phandles[socket] = (*phandle)++;

    riscv_create_fdt_plic(ms->fdt, plic_addr, s->memmap[VIRT_PLIC].আকার_2,
                          plic_phandles[socket], FDT_PLIC_INT_CELLS,
                          FDT_PLIC_ADDR_CELLS, plic_cells,
                          cells_দৈর্ঘ্য * আকার(uint32_t),
                          VIRT_IRQCHIP_অঙ্ক_SOURCES - 1,
                          numa_enabled, socket);

    যদি (!socket) {
        platform_bus_add_all_fdt_nodes(ms->fdt, plic_name,
                                       s->memmap[VIRT_PLATFORM_BUS].base,
                                       s->memmap[VIRT_PLATFORM_BUS].আকার_2,
                                       VIRT_PLATFORM_BUS_IRQ);
    }
}

স্থির শূন্য create_fdt_pmu(RISCVVirtState *s)
{
    g_autofree অক্ষর *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_add_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_গ্রন্থি(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

স্থির শূন্য create_fdt_sockets(RISCVVirtState *s,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
                               uint32_t *irq_virtio_phandle,
                               uint32_t *msi_pcie_phandle)
{
    পূর্ণসংখ্যা socket, phandle_pos;
    MachineState *ms = MACHINE(s);
    uint32_t msi_m_phandle = 0, msi_s_phandle = 0;
    uint32_t xplic_phandles[MAX_NODES];
    g_autofree uint32_t *intc_phandles = ফাঁকা;
    পূর্ণসংখ্যা socket_গুনতি = riscv_socket_গুনতি(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_bit = riscv_is_32bit(&s->soc[0]);
    APLICFdtProps aplic_props;
    ACLINTFdtProps aclint_props;

    riscv_fdt_create_cpu_socket_subnode(ms->fdt,
        kvm_enabled() ? kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    intc_phandles = g_new0(uint32_t, ms->smp.cpus);

    যদি (virt_aclint_allowed() && s->have_aclint) {
        aclint_props.clint = &s->memmap[VIRT_CLINT];
        aclint_props.aclint_sswi = &s->memmap[VIRT_ACLINT_SSWI];
        aclint_props.aia_type = s->aia_type;
        aclint_props.numa_enabled = numa_enabled;
    }

    phandle_pos = ms->smp.cpus;
    জন্য (socket = (socket_গুনতি - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[VIRT_DRAM].base +
                         riscv_socket_mem_offset(ms, socket);
        uint64_t memsize = riscv_socket_mem_আকার(ms, socket);

        phandle_pos -= s->soc[socket].অঙ্ক_harts;

        riscv_create_fdt_socket_cpus(ms->fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].অঙ্ক_harts,
                                     s->soc[socket].hartid_base,
                                     phandle, &intc_phandles[phandle_pos],
                                     numa_enabled, is_32_bit);

        riscv_create_fdt_socket_memory(ms->fdt, memaddr, memsize,
                                       socket, riscv_numa_enabled(ms));

        যদি (virt_aclint_allowed() && s->have_aclint) {
            aclint_props.socket = socket;
            aclint_props.অঙ্ক_harts = s->soc[socket].অঙ্ক_harts;
            riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                           &intc_phandles[phandle_pos]);
        } অন্যথা যদি (tcg_enabled()) {
            hwaddr clintaddr = s->memmap[VIRT_CLINT].base +
                               s->memmap[VIRT_CLINT].আকার_2 * socket;

            riscv_create_fdt_socket_clint(ms->fdt, clintaddr,
                                          s->memmap[VIRT_CLINT].আকার_2,
                                          socket, &intc_phandles[phandle_pos],
                                          s->soc[socket].অঙ্ক_harts,
                                          numa_enabled);
        }
    }

    যদি (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        IMSICFdtProps props = {
            .soc = &s->soc,
            .socket_গুনতি = riscv_socket_গুনতি(ms),
            .smp_cpus = ms->smp.cpus,
            .imsic_m_base = !kvm_enabled() ? s->memmap[VIRT_IMSIC_M].base : 0,
            .imsic_s_base = s->memmap[VIRT_IMSIC_S].base,
            .imsic_group_max_আকার = VIRT_IMSIC_GROUP_MAX_আকার,
            .irqchip_অঙ্ক_msis = VIRT_IRQCHIP_অঙ্ক_MSIS,
            .aia_guests = s->aia_guests
        };

        riscv_create_fdt_imsic(ms->fdt, &props, phandle, intc_phandles,
                               &msi_m_phandle, &msi_s_phandle);

        *msi_pcie_phandle = msi_s_phandle;
    }

    যদি (s->aia_type != VIRT_AIA_TYPE_NONE) {
        aplic_props.aplic_m = !kvm_enabled() ? &s->memmap[VIRT_APLIC_M] : ফাঁকা;
        aplic_props.aplic_s = &s->memmap[VIRT_APLIC_S];
        aplic_props.platform_bus = &s->memmap[VIRT_PLATFORM_BUS];
        aplic_props.platform_bus_irq = VIRT_PLATFORM_BUS_IRQ;
        aplic_props.socket = 0;
        aplic_props.অঙ্ক_harts = ms->smp.cpus;
        aplic_props.numa_enabled = numa_enabled;
        aplic_props.irqchip_অঙ্ক_sources = VIRT_IRQCHIP_অঙ্ক_SOURCES;
        aplic_props.aia_type = s->aia_type;
    }

    /*
     * With KVM AIA aplic-imsic, using an irqchip without split
     * mode, we'll use only one APLIC instance.
     */
    যদি (!virt_use_emulated_aplic(s->aia_type)) {
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[0], xplic_phandles);

        *irq_mmio_phandle = xplic_phandles[0];
        *irq_virtio_phandle = xplic_phandles[0];
        *irq_pcie_phandle = xplic_phandles[0];
    } অন্যথা {
        phandle_pos = ms->smp.cpus;
        জন্য (socket = (socket_গুনতি - 1); socket >= 0; socket--) {
            phandle_pos -= s->soc[socket].অঙ্ক_harts;

            যদি (s->aia_type == VIRT_AIA_TYPE_NONE) {
                create_fdt_socket_plic(s, socket, phandle,
                                       &intc_phandles[phandle_pos],
                                       xplic_phandles);
            } অন্যথা {
                aplic_props.socket = socket;
                aplic_props.অঙ্ক_harts = s->soc[socket].অঙ্ক_harts;
                riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                              msi_m_phandle, msi_s_phandle,
                                              phandle,
                                              &intc_phandles[phandle_pos],
                                              xplic_phandles);
            }
        }

        জন্য (socket = 0; socket < socket_গুনতি; socket++) {
            যদি (socket == 0) {
                *irq_mmio_phandle = xplic_phandles[socket];
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            যদি (socket == 1) {
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            যদি (socket == 2) {
                *irq_pcie_phandle = xplic_phandles[socket];
            }
        }
    }

    riscv_socket_fdt_write_distance_matrix(ms);
}

স্থির শূন্য create_fdt_virtio(RISCVVirtState *s, uint32_t irq_virtio_phandle)
{
    পূর্ণসংখ্যা i;
    MachineState *ms = MACHINE(s);
    hwaddr virtio_base = s->memmap[VIRT_VIRTIO].base;

    জন্য (i = 0; i < VIRTIO_গুনতি; i++) {
        g_autofree অক্ষর *name = ফাঁকা;
        uint64_t আকার_2 = s->memmap[VIRT_VIRTIO].আকার_2;
        hwaddr addr = virtio_base + i * আকার_2;

        name = g_strdup_printf("/soc/virtio_mmio@%"HWADDR_PRIx, addr);

        qemu_fdt_add_subnode(ms->fdt, name);
        qemu_fdt_setprop_string(ms->fdt, name, "compatible", "virtio,mmio");
        qemu_fdt_setprop_sized_cells(ms->fdt, name, "reg", 2, addr, 2, আকার_2);
        qemu_fdt_setprop_cell(ms->fdt, name, "interrupt-parent",
            irq_virtio_phandle);
        যদি (s->aia_type == VIRT_AIA_TYPE_NONE) {
            qemu_fdt_setprop_cell(ms->fdt, name, "interrupts",
                                  VIRTIO_IRQ + i);
        } অন্যথা {
            qemu_fdt_setprop_cells(ms->fdt, name, "interrupts",
                                   VIRTIO_IRQ + i, 0x4);
        }
    }
}

স্থির শূন্য create_fdt_uarts(RISCVVirtState *s, uint32_t irq_mmio_phandle)
{
    g_autofree অক্ষর *name = ফাঁকা;
    MachineState *ms = MACHINE(s);

    যদি (s->uart1_present) {
        riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART1],
                              UART1_IRQ, s->aia_type, irq_mmio_phandle);
    }

    riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART0], UART0_IRQ,
                          s->aia_type, irq_mmio_phandle);

    name = riscv_fdt_get_uart_nodename(s->memmap[VIRT_UART0].base);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);
}

স্থির শূন্য create_fdt_fw_cfg(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    hwaddr base = s->memmap[VIRT_FW_CFG].base;
    hwaddr আকার_2 = s->memmap[VIRT_FW_CFG].আকার_2;
    g_autofree অক্ষর *nodename = g_strdup_printf("/fw-cfg@%" PRIx64, base);

    qemu_fdt_add_subnode(ms->fdt, nodename);
    qemu_fdt_setprop_string(ms->fdt, nodename,
                            "compatible", "qemu,fw-cfg-mmio");
    qemu_fdt_setprop_sized_cells(ms->fdt, nodename, "reg",
                                 2, base, 2, আকার_2);
    qemu_fdt_setprop(ms->fdt, nodename, "dma-coherent", ফাঁকা, 0);
}

স্থির শূন্য create_fdt_virtio_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ধ্রুবক অক্ষর compat[] = "virtio,pci-iommu\0pci1af4,1057";
    শূন্য *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree অক্ষর *iommu_গ্রন্থি = ফাঁকা;
    g_autofree অক্ষর *pci_গ্রন্থি = ফাঁকা;

    pci_গ্রন্থি = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].base);
    iommu_গ্রন্থি = g_strdup_printf("%s/virtio_iommu@%x,%x", pci_গ্রন্থি,
                                 PCI_SLOT(bdf), PCI_FUNC(bdf));
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);

    qemu_fdt_add_subnode(fdt, iommu_গ্রন্থি);

    qemu_fdt_setprop(fdt, iommu_গ্রন্থি, "compatible", compat, আকার(compat));
    qemu_fdt_setprop_sized_cells(fdt, iommu_গ্রন্থি, "reg",
                                 1, bdf << 8, 1, 0, 1, 0,
                                 1, 0, 1, 0);
    qemu_fdt_setprop_cell(fdt, iommu_গ্রন্থি, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_গ্রন্থি, "phandle", iommu_phandle);

    qemu_fdt_setprop_cells(fdt, pci_গ্রন্থি, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
}

স্থির শূন্য create_fdt_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ধ্রুবক অক্ষর comp[] = "riscv,pci-iommu";
    শূন্য *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree অক্ষর *iommu_গ্রন্থি = ফাঁকা;
    g_autofree অক্ষর *pci_গ্রন্থি = ফাঁকা;

    pci_গ্রন্থি = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].base);
    iommu_গ্রন্থি = g_strdup_printf("%s/iommu@%x", pci_গ্রন্থি, bdf);
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_add_subnode(fdt, iommu_গ্রন্থি);

    qemu_fdt_setprop(fdt, iommu_গ্রন্থি, "compatible", comp, আকার(comp));
    qemu_fdt_setprop_cell(fdt, iommu_গ্রন্থি, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_গ্রন্থি, "phandle", iommu_phandle);
    qemu_fdt_setprop_cells(fdt, iommu_গ্রন্থি, "reg",
                           bdf << 8, 0, 0, 0, 0);
    qemu_fdt_setprop_cells(fdt, pci_গ্রন্থি, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
    s->pci_iommu_bdf = bdf;
}

স্থির শূন্য finalize_fdt(RISCVVirtState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, irq_virtio_phandle = 1;
    uint32_t iommu_sys_phandle = 1;

    create_fdt_sockets(s, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &irq_virtio_phandle,
                       &msi_pcie_phandle);

    create_fdt_virtio(s, irq_virtio_phandle);

    যদি (virt_is_iommu_sys_enabled(s)) {
        iommu_sys_phandle =
            riscv_create_fdt_riscv_iommu_sys(MACHINE(s)->fdt,
                                             s->memmap[VIRT_IOMMU_SYS].base,
                                             s->memmap[VIRT_IOMMU_SYS].আকার_2,
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
                          &virt_উপরের_pcie_memmap,
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, PCIE_IRQ);

    riscv_create_fdt_syscon(MACHINE(s)->fdt, &phandle,
                            s->memmap[VIRT_TEST].base,
                            s->memmap[VIRT_TEST].আকার_2,
                            FINISHER_RESET, FINISHER_PASS, true);

    create_fdt_uarts(s, irq_mmio_phandle);

    riscv_create_fdt_rtc(MACHINE(s)->fdt, &s->memmap[VIRT_RTC], RTC_IRQ,
                         s->aia_type, irq_mmio_phandle);
}

স্থির শূন্য create_fdt(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_seed[32];
    g_autofree অক্ষর *name = ফাঁকা;

    ms->fdt = riscv_create_board_device_বৃক্ষ("riscv-virtio,qemu",
                                             "riscv-virtio",
                                             &s->fdt_আকার);

    /*
     * The "/soc/pci@..." node is needed for PCIE hotplugs
     * that might happen before finalize_fdt().
     */
    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                           s->memmap[VIRT_PCIE_ECAM].base);
    qemu_fdt_add_subnode(ms->fdt, name);

    qemu_fdt_add_subnode(ms->fdt, "/chosen");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_seed, আকার(rng_seed));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_seed, আকার(rng_seed));

    qemu_fdt_add_subnode(ms->fdt, "/aliases");

    riscv_create_fdt_flash(ms->fdt, s->memmap[VIRT_FLASH].base,
                           s->memmap[VIRT_FLASH].আকার_2 / 2);
    create_fdt_fw_cfg(s);
    create_fdt_pmu(s);
}

স্থির FWCfgState *create_fw_cfg(ধ্রুবক MachineState *ms, hwaddr base)
{
    FWCfgState *fw_cfg;

    fw_cfg = fw_cfg_init_mem_dma(base, &address_space_memory);
    fw_cfg_add_i16(fw_cfg, FW_CFG_NB_CPUS, (uint16_t)ms->smp.cpus);

    ফেরত fw_cfg;
}

স্থির DeviceState *virt_create_plic(ধ্রুবক MemMapEntry *memmap, পূর্ণসংখ্যা socket,
                                     পূর্ণসংখ্যা base_hartid, পূর্ণসংখ্যা hart_গুনতি)
{
    g_autofree অক্ষর *plic_hart_config = ফাঁকা;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hart_গুনতি);

    /* Per-socket PLIC */
    ফেরত sifive_plic_create(
             get_system_memory(),
             memmap[VIRT_PLIC].base + socket * memmap[VIRT_PLIC].আকার_2,
             plic_hart_config, hart_গুনতি, base_hartid,
             VIRT_IRQCHIP_অঙ্ক_SOURCES,
             ((1U << VIRT_IRQCHIP_অঙ্ক_PRIO_BITS) - 1),
             VIRT_PLIC_PRIORITY_BASE, VIRT_PLIC_PENDING_BASE,
             VIRT_PLIC_ENABLE_BASE, VIRT_PLIC_ENABLE_STRIDE,
             VIRT_PLIC_CONTEXT_BASE,
             VIRT_PLIC_CONTEXT_STRIDE,
             memmap[VIRT_PLIC].আকার_2);
}

স্থির শূন্য virt_build_smbios(RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    uint8_t *smbios_tables, *smbios_anchor;
    আকার_t smbios_tables_len, smbios_anchor_len;
    কাঠামো smbios_phys_mem_area mem_array;
    ধ্রুবক অক্ষর *product = "QEMU Virtual Machine";

    যদি (kvm_enabled()) {
        product = "KVM Virtual Machine";
    }

    smbios_set_defaults("QEMU", product, mc->name);

    যদি (riscv_is_32bit(&s->soc[0])) {
        smbios_set_default_processor_family(0x200);
    } অন্যথা {
        smbios_set_default_processor_family(0x201);
    }

    /* build the array of physical mem area from base_memmap */
    mem_array.address = s->memmap[VIRT_DRAM].base;
    mem_array.দৈর্ঘ্য = ms->ram_আকার;

    smbios_get_tables(ms, SMBIOS_ENTRY_POINT_TYPE_64,
                      &mem_array, 1,
                      &smbios_tables, &smbios_tables_len,
                      &smbios_anchor, &smbios_anchor_len,
                      &error_fatal);

    যদি (smbios_anchor) {
        fw_cfg_add_file(s->fw_cfg, "etc/smbios/smbios-tables",
                        smbios_tables, smbios_tables_len);
        fw_cfg_add_file(s->fw_cfg, "etc/smbios/smbios-anchor",
                        smbios_anchor, smbios_anchor_len);
    }
}

স্থির শূন্য virt_machine_done(Notifier *notifier, শূন্য *তথ্য)
{
    RISCVVirtState *s = container_of(notifier, RISCVVirtState,
                                     machine_done);
    MachineState *machine = MACHINE(s);
    hwaddr start_addr = s->memmap[VIRT_DRAM].base;
    hwaddr firmware_end_addr;
    vaddr kernel_start_addr;
    ধ্রুবক অক্ষর *firmware_name = riscv_default_firmware_name(&s->soc[0]);
    uint64_t fdt_load_addr;
    uint64_t kernel_entry = 0;
    BlockBackend *pflash_blk0;
    RISCVBootInfo boot_info;

    /*
     * An user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    যদি (machine->dtb == ফাঁকা) {
        finalize_fdt(s);
    }

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * so the "-bios" parameter is not supported when KVM is enabled.
     */
    যদি (kvm_enabled()) {
        যদি (machine->firmware) {
            যদি (strcmp(machine->firmware, "none")) {
                error_report("Machine mode firmware is not supported in "
                             "combination with KVM.");
                exit(1);
            }
        } অন্যথা {
            machine->firmware = g_strdup("none");
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_end_addr = riscv_find_and_load_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &start_addr, ফাঁকা);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    যদি (pflash_blk0) {
        যদি (machine->firmware && !strcmp(machine->firmware, "none") &&
            !kvm_enabled()) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            start_addr = s->memmap[VIRT_FLASH].base;
        } অন্যথা {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_setup_firmware_boot(machine);
            kernel_entry = s->memmap[VIRT_FLASH].base;
        }
    }

    যদি (machine->kernel_filename && !kernel_entry) {
        kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                         firmware_end_addr);
        riscv_load_kernel(machine, &boot_info, kernel_start_addr,
                          true, ফাঁকা);
        kernel_entry = boot_info.image_নিচের_addr;
    }

    fdt_load_addr = riscv_compute_fdt_addr(s->memmap[VIRT_DRAM].base,
                                           s->memmap[VIRT_DRAM].আকার_2,
                                           machine, &boot_info);
    riscv_load_fdt(fdt_load_addr, machine->fdt);

    /* load the reset vector */
    riscv_setup_rom_reset_vec(machine, &s->soc[0], start_addr,
                              s->memmap[VIRT_MROM].base,
                              s->memmap[VIRT_MROM].আকার_2, kernel_entry,
                              fdt_load_addr);

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * So here setup kernel start address and fdt address.
     * TODO:Support firmware loading and integrate to TCG start
     */
    যদি (kvm_enabled()) {
        riscv_setup_direct_kernel(kernel_entry, fdt_load_addr);
    }

    virt_build_smbios(s);

    যদি (virt_is_acpi_enabled(s)) {
        virt_acpi_setup(s);
    }
}

স্থির শূন্য virt_machine_init(MachineState *machine)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *mask_rom = g_new(MemoryRegion, 1);
    DeviceState *mmio_irqchip, *virtio_irqchip, *pcie_irqchip;
    পূর্ণসংখ্যা i, base_hartid, hart_গুনতি;
    পূর্ণসংখ্যা socket_গুনতি = riscv_socket_গুনতি(machine);

    s->memmap = virt_memmap;

    /* Check socket count limit */
    যদি (VIRT_SOCKETS_MAX < socket_গুনতি) {
        error_report("number of sockets/nodes should be less than %d",
            VIRT_SOCKETS_MAX);
        exit(1);
    }

    যদি (!virt_aclint_allowed() && s->have_aclint) {
        error_report("'aclint' is only available with TCG acceleration");
        exit(1);
    }

    /* Initialize sockets */
    mmio_irqchip = virtio_irqchip = pcie_irqchip = ফাঁকা;
    জন্য (i = 0; i < socket_গুনতি; i++) {
        g_autofree অক্ষর *soc_name = g_strdup_printf("soc%d", i);

        যদি (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            exit(1);
        }

        base_hartid = riscv_socket_first_hartid(machine, i);
        যদি (base_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            exit(1);
        }

        hart_গুনতি = riscv_socket_hart_গুনতি(machine, i);
        যদি (hart_গুনতি < 0) {
            error_report("can't find hart count for socket%d", i);
            exit(1);
        }

        object_initialize_child(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        object_property_set_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_set_int(OBJECT(&s->soc[i]), "hartid-base",
                                base_hartid, &error_abort);
        object_property_set_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_গুনতি, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        যদি (virt_aclint_allowed() && s->have_aclint) {
            যদি (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
                /* Per-socket ACLINT MTIMER */
                riscv_aclint_mtimer_create(system_memory,
                        s->memmap[VIRT_CLINT].base +
                            i * RISCV_ACLINT_DEFAULT_MTIMER_আকার,
                        RISCV_ACLINT_DEFAULT_MTIMER_আকার,
                        base_hartid, hart_গুনতি,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
            } অন্যথা {
                /* Per-socket ACLINT MSWI, MTIMER, and SSWI */
                riscv_aclint_swi_create(system_memory,
                        s->memmap[VIRT_CLINT].base +
                            i * s->memmap[VIRT_CLINT].আকার_2,
                        base_hartid, hart_গুনতি, false);
                riscv_aclint_mtimer_create(system_memory,
                        s->memmap[VIRT_CLINT].base +
                            i * s->memmap[VIRT_CLINT].আকার_2 +
                            RISCV_ACLINT_SWI_আকার,
                        RISCV_ACLINT_DEFAULT_MTIMER_আকার,
                        base_hartid, hart_গুনতি,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
                riscv_aclint_swi_create(system_memory,
                        s->memmap[VIRT_ACLINT_SSWI].base +
                            i * s->memmap[VIRT_ACLINT_SSWI].আকার_2,
                        base_hartid, hart_গুনতি, true);
            }
        } অন্যথা যদি (tcg_enabled()) {
            /* Per-socket SiFive CLINT */
            riscv_aclint_swi_create(system_memory,
                    s->memmap[VIRT_CLINT].base + i * s->memmap[VIRT_CLINT].আকার_2,
                    base_hartid, hart_গুনতি, false);
            riscv_aclint_mtimer_create(system_memory,
                    s->memmap[VIRT_CLINT].base +
                    i * s->memmap[VIRT_CLINT].আকার_2 + RISCV_ACLINT_SWI_আকার,
                    RISCV_ACLINT_DEFAULT_MTIMER_আকার, base_hartid, hart_গুনতি,
                    RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
                    RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
        }

        /* Per-socket interrupt controller */
        যদি (s->aia_type == VIRT_AIA_TYPE_NONE) {
            s->irqchip[i] = virt_create_plic(s->memmap, i,
                                             base_hartid, hart_গুনতি);
        } অন্যথা {
            bool msimode = s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;
            পূর্ণসংখ্যা imsic_bits = imsic_অঙ্ক_bits(s->aia_guests + 1);
            s->irqchip[i] = riscv_create_aia(system_memory, msimode,
                                             s->aia_guests,
                                             IMSIC_HART_আকার(0),
                                             IMSIC_HART_আকার(imsic_bits),
                                             s->অঙ্ক_sources,
                                             &s->memmap[VIRT_APLIC_M],
                                             &s->memmap[VIRT_APLIC_S],
                                             &s->memmap[VIRT_IMSIC_M],
                                             &s->memmap[VIRT_IMSIC_S],
                                             i, base_hartid, hart_গুনতি,
                                             VIRT_IRQCHIP_অঙ্ক_MSIS,
                                             VIRT_IRQCHIP_অঙ্ক_PRIO_BITS);
        }

        /* Try to use different IRQCHIP instance based device type */
        যদি (i == 0) {
            mmio_irqchip = s->irqchip[i];
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        যদি (i == 1) {
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        যদি (i == 2) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    যদি (kvm_enabled() && virt_use_kvm_aia_aplic_imsic(s->aia_type)) {
        kvm_riscv_aia_create(machine, IMSIC_MMIO_GROUP_MIN_SHIFT,
                             VIRT_IRQCHIP_অঙ্ক_SOURCES, VIRT_IRQCHIP_অঙ্ক_MSIS,
                             s->memmap[VIRT_APLIC_S].base,
                             s->memmap[VIRT_IMSIC_S].base,
                             s->aia_guests);
    }

    যদি (riscv_is_32bit(&s->soc[0])) {
#যদি HOST_LONG_BITS == 64
        /* limit RAM size in a 32-bit system */
        যদি (machine->ram_আকার > 10 * GiB) {
            machine->ram_আকার = 10 * GiB;
            error_report("Limiting RAM size to 10 GiB");
        }
#endif
        virt_উপরের_pcie_memmap.base = VIRT32_উপরের_PCIE_MMIO_BASE;
        virt_উপরের_pcie_memmap.আকার_2 = VIRT32_উপরের_PCIE_MMIO_আকার;
    } অন্যথা {
        virt_উপরের_pcie_memmap.আকার_2 = VIRT64_উপরের_PCIE_MMIO_আকার;
        virt_উপরের_pcie_memmap.base = s->memmap[VIRT_DRAM].base +
                                     machine->ram_আকার;
        virt_উপরের_pcie_memmap.base =
            ROUND_UP(virt_উপরের_pcie_memmap.base, virt_উপরের_pcie_memmap.আকার_2);
    }

    /* register system main memory (actual RAM) */
    memory_region_add_subregion(system_memory, s->memmap[VIRT_DRAM].base,
                                machine->ram);

    /* boot rom */
    memory_region_init_rom(mask_rom, ফাঁকা, "riscv_virt_board.mrom",
                           s->memmap[VIRT_MROM].আকার_2, &error_fatal);
    memory_region_add_subregion(system_memory, s->memmap[VIRT_MROM].base,
                                mask_rom);

    /*
     * Init fw_cfg. Must be done before riscv_load_fdt, otherwise the
     * device tree cannot be altered and we get FDT_ERR_NOSPACE.
     */
    s->fw_cfg = create_fw_cfg(machine, s->memmap[VIRT_FW_CFG].base);
    rom_set_fw(s->fw_cfg);

    /* SiFive Test MMIO device */
    sifive_test_create(s->memmap[VIRT_TEST].base);

    /* VirtIO MMIO devices */
    জন্য (i = 0; i < VIRTIO_গুনতি; i++) {
        sysbus_create_simple("virtio-mmio",
            s->memmap[VIRT_VIRTIO].base + i * s->memmap[VIRT_VIRTIO].আকার_2,
            qdev_get_gpio_in(virtio_irqchip, VIRTIO_IRQ + i));
    }

    riscv_gpex_pcie_init(system_memory, pcie_irqchip,
                   &s->memmap[VIRT_PCIE_ECAM], &s->memmap[VIRT_PCIE_MMIO],
                   &virt_উপরের_pcie_memmap, &s->memmap[VIRT_PCIE_PIO],
                   PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
        &s->memmap[VIRT_PLATFORM_BUS], VIRT_PLATFORM_BUS_IRQ,
        VIRT_PLATFORM_BUS_অঙ্ক_IRQS);

    serial_mm_init(system_memory, s->memmap[VIRT_UART0].base,
        0, qdev_get_gpio_in(mmio_irqchip, UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    যদি (serial_hd(1)) {
        serial_mm_init(system_memory, s->memmap[VIRT_UART1].base,
            0, qdev_get_gpio_in(mmio_irqchip, UART1_IRQ), 399193,
            serial_hd(1), DEVICE_LITTLE_ENDIAN);
        s->uart1_present = true;
    }

    sysbus_create_simple("goldfish_rtc", s->memmap[VIRT_RTC].base,
        qdev_get_gpio_in(mmio_irqchip, RTC_IRQ));

    জন্য (i = 0; i < ARRAY_আকার(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    virt_flash_map(s, system_memory);

    /* load/create device tree */
    যদি (machine->dtb) {
        machine->fdt = load_device_বৃক্ষ(machine->dtb, &s->fdt_আকার);
        যদি (!machine->fdt) {
            error_report("load_device_tree() failed");
            exit(1);
        }
    } অন্যথা {
        create_fdt(s);
    }

    যদি (virt_is_iommu_sys_enabled(s)) {
        riscv_create_iommu_sys(mmio_irqchip, s->memmap[VIRT_IOMMU_SYS].base,
                               IOMMU_SYS_IRQ, riscv_is_32bit(&s->soc[0]));
    }

    s->machine_done.notify = virt_machine_done;
    qemu_add_machine_init_done_notifier(&s->machine_done);
}

স্থির শূন্য virt_machine_instance_finalize(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    জন্য (পূর্ণসংখ্যা i = 0; i < ARRAY_আকার(s->flash); i++) {
        যদি (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
    g_free(s->oem_id);
    g_free(s->oem_table_id);
}

স্থির শূন্য virt_machine_instance_init(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    virt_flash_create(s);

    s->oem_id = g_strndup(ACPI_BUILD_APPNAME6, 6);
    s->oem_table_id = g_strndup(ACPI_BUILD_APPNAME8, 8);
    s->acpi = ON_OFF_AUTO_AUTO;
    s->iommu_sys = ON_OFF_AUTO_AUTO;
    s->অঙ্ক_sources = VIRT_IRQCHIP_অঙ্ক_SOURCES;
}

স্থির অক্ষর *virt_get_aia_guests(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ফেরত g_strdup_printf("%d", s->aia_guests);
}

স্থির শূন্য virt_set_aia_guests(Object *obj, ধ্রুবক অক্ষর *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->aia_guests = atoi(val);
    যদি (s->aia_guests < 0 || s->aia_guests > VIRT_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          VIRT_IRQCHIP_MAX_GUESTS);
    }
}

স্থির অক্ষর *virt_get_aia(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    ধ্রুবক অক্ষর *val;

    নির্বাচন (s->aia_type) {
    ক্ষেত্র VIRT_AIA_TYPE_APLIC:
        val = "aplic";
        বিরতি;
    ক্ষেত্র VIRT_AIA_TYPE_APLIC_IMSIC:
        val = "aplic-imsic";
        বিরতি;
    পূর্বনির্ধারিত:
        val = "none";
        বিরতি;
    };

    ফেরত g_strdup(val);
}

স্থির শূন্য virt_set_aia(Object *obj, ধ্রুবক অক্ষর *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    যদি (!strcmp(val, "none")) {
        s->aia_type = VIRT_AIA_TYPE_NONE;
    } অন্যথা যদি (!strcmp(val, "aplic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC;
    } অন্যথা যদি (!strcmp(val, "aplic-imsic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC_IMSIC;
    } অন্যথা {
        error_setg(errp, "Invalid AIA interrupt controller type");
        error_append_hint(errp, "Valid values are none, aplic, and "
                          "aplic-imsic.\n");
    }
}

স্থির bool virt_get_aclint(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ফেরত s->have_aclint;
}

স্থির শূন্য virt_set_aclint(Object *obj, bool মান, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->have_aclint = মান;
}

bool virt_is_iommu_sys_enabled(RISCVVirtState *s)
{
    ফেরত s->iommu_sys == ON_OFF_AUTO_ON;
}

স্থির শূন্য virt_get_iommu_sys(Object *obj, Visitor *v, ধ্রুবক অক্ষর *name,
                               শূন্য *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto iommu_sys = s->iommu_sys;

    visit_type_OnOffAuto(v, name, &iommu_sys, errp);
}

স্থির শূন্য virt_set_iommu_sys(Object *obj, Visitor *v, ধ্রুবক অক্ষর *name,
                               শূন্য *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    visit_type_OnOffAuto(v, name, &s->iommu_sys, errp);
}

bool virt_is_acpi_enabled(RISCVVirtState *s)
{
    ফেরত s->acpi != ON_OFF_AUTO_OFF;
}

স্থির শূন্য virt_get_acpi(Object *obj, Visitor *v, ধ্রুবক অক্ষর *name,
                          শূন্য *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto acpi = s->acpi;

    visit_type_OnOffAuto(v, name, &acpi, errp);
}

স্থির শূন্য virt_set_acpi(Object *obj, Visitor *v, ধ্রুবক অক্ষর *name,
                          শূন্য *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    visit_type_OnOffAuto(v, name, &s->acpi, errp);
}

স্থির ধ্রুবক HotplugHandler *
virt_machine_get_hotplug_handler(MachineState *machine, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);

    যদি (device_is_dynamic_sysbus(mc, dev) ||
        object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI) ||
        object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        s->iommu_sys = ON_OFF_AUTO_OFF;
        ফেরত HOTPLUG_HANDLER(machine);
    }

    ফেরত ফাঁকা;
}

স্থির শূন্য virt_machine_device_plug_cb(ধ্রুবক HotplugHandler *hotplug_dev,
                                        DeviceState *dev, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(hotplug_dev);

    যদি (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        যদি (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }

    যদি (object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI)) {
        create_fdt_virtio_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
    }

    যদি (object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        create_fdt_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
        s->iommu_sys = ON_OFF_AUTO_OFF;
    }
}

স্থির শূন্য virt_machine_class_init(ObjectClass *oc, ধ্রুবক শূন্য *তথ্য)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);

    mc->desc = "RISC-V VirtIO board";
    mc->init = virt_machine_init;
    mc->max_cpus = VIRT_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_BASE;
    mc->block_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->pci_allow_0_address = true;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_সূচক_to_instance_props = riscv_numa_cpu_সূচক_to_props;
    mc->get_default_cpu_গ্রন্থি_id = riscv_numa_get_default_cpu_গ্রন্থি_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_virt_board.ram";
    প্রতিজ্ঞা(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = virt_machine_get_hotplug_handler;

    hc->plug = virt_machine_device_plug_cb;

    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_RAMFB_DEVICE);
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_UEFI_VARS_SYSBUS);
#ifdef CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#endif

    object_class_property_add_bool(oc, "aclint", virt_get_aclint,
                                   virt_set_aclint);
    object_class_property_set_description(oc, "aclint",
                                          "(TCG only) Set on/off to "
                                          "enable/disable emulating "
                                          "ACLINT devices");

    object_class_property_add_str(oc, "aia", virt_get_aia,
                                  virt_set_aia);
    object_class_property_set_description(oc, "aia",
                                          "Set type of AIA interrupt "
                                          "controller. Valid values are "
                                          "none, aplic, and aplic-imsic.");

    object_class_property_add_str(oc, "aia-guests",
                                  virt_get_aia_guests,
                                  virt_set_aia_guests);
    {
        g_autofree অক্ষর *str =
            g_strdup_printf("Set number of guest MMIO pages for AIA IMSIC. "
                            "Valid value should be between 0 and %d.",
                            VIRT_IRQCHIP_MAX_GUESTS);
        object_class_property_set_description(oc, "aia-guests", str);
    }

    object_class_property_add(oc, "acpi", "OnOffAuto",
                              virt_get_acpi, virt_set_acpi,
                              ফাঁকা, ফাঁকা);
    object_class_property_set_description(oc, "acpi",
                                          "Enable ACPI");

    object_class_property_add(oc, "iommu-sys", "OnOffAuto",
                              virt_get_iommu_sys, virt_set_iommu_sys,
                              ফাঁকা, ফাঁকা);
    object_class_property_set_description(oc, "iommu-sys",
                                          "Enable IOMMU platform device");
}

স্থির ধ্রুবক TypeInfo virt_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("virt"),
    .parent     = TYPE_MACHINE,
    .class_init = virt_machine_class_init,
    .instance_init = virt_machine_instance_init,
    .instance_finalize = virt_machine_instance_finalize,
    .instance_আকার = আকার(RISCVVirtState),
    .interfaces = (ধ্রুবক InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

স্থির শূন্য virt_machine_init_register_types(শূন্য)
{
    type_register_static(&virt_machine_typeinfo);
}

type_init(virt_machine_init_register_types)
