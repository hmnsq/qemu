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

#شامل "qemu/osdep.h"
#شامل "qemu/units.h"
#شامل "qemu/error-report.h"
#شامل "qemu/guest-random.h"
#شامل "qapi/error.h"
#شامل "hw/core/boards.h"
#شامل "hw/core/loader.h"
#شامل "hw/core/sysbus.h"
#شامل "hw/core/qdev-properties.h"
#شامل "hw/char/serial-mm.h"
#شامل "target/riscv/cpu.h"
#شامل "hw/core/sysbus-fdt.h"
#شامل "hw/riscv/riscv_hart.h"
#شامل "hw/riscv/iommu.h"
#شامل "hw/riscv/riscv-iommu.h"
#شامل "hw/riscv/riscv-iommu-bits.h"
#شامل "hw/riscv/virt.h"
#شامل "hw/riscv/boot.h"
#شامل "hw/riscv/device-common.h"
#شامل "hw/riscv/fdt-common.h"
#شامل "hw/riscv/numa.h"
#شامل "kvm/kvm_riscv.h"
#شامل "hw/firmware/smbios.h"
#شامل "hw/intc/riscv_aclint.h"
#شامل "hw/intc/riscv_aplic.h"
#شامل "hw/intc/sifive_plic.h"
#شامل "hw/misc/sifive_test.h"
#شامل "hw/core/platform-bus.h"
#شامل "chardev/char.h"
#شامل "system/device_tree.h"
#شامل "system/system.h"
#شامل "system/tcg.h"
#شامل "system/kvm.h"
#شامل "system/tpm.h"
#شامل "system/qtest.h"
#شامل "hw/pci/pci.h"
#شامل "hw/pci-host/gpex.h"
#شامل "hw/display/ramfb.h"
#شامل "hw/acpi/aml-build.h"
#شامل "qapi/qapi-visit-common.h"
#شامل "hw/virtio/virtio-iommu.h"
#شامل "hw/uefi/var-service-api.h"

#شامل "aia.h"

/* KVM AIA only supports APLIC MSI. APLIC Wired is always emulated by QEMU. */
ساکن bool virt_use_kvm_aia_aplic_imsic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    واپس riscv_is_kvm_aia_aplic_imsic(msimode);
}

ساکن bool virt_use_emulated_aplic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    واپس riscv_use_emulated_aplic(msimode);
}

ساکن bool virt_aclint_allowed(خالی)
{
    واپس tcg_enabled() || qtest_enabled();
}

ساکن ثابت Mem_Map_اندراج virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_IOMMU_SYS] =    {  0x3010000,        0x1000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_سائز(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_سائز(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_سائز(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    /* UART1 supports page isolation from UART0 */
    [VIRT_UART1] =        { 0x1000a000,         0x100 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_سائز },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_سائز },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    [VIRT_DRAM] =         { 0x80000000,           0x0 },
};

/* PCIe high mmio is fixed for RV32 */
#تعریف_کرو VIRT32_بلند_PCIE_MMIO_بنیاد  0x300000000ULL
#تعریف_کرو VIRT32_بلند_PCIE_MMIO_سائز  (4 * GiB)

/* PCIe high mmio for RV64, size is fixed but base depends on top of RAM */
#تعریف_کرو VIRT64_بلند_PCIE_MMIO_سائز  (16 * GiB)

ساکن Mem_Map_اندراج virt_بلند_pcie_memmap;

#تعریف_کرو VIRT_FLASH_SECTOR_سائز (256 * KiB)

ساکن خالی virt_flash_create(RISCVVirtState *s)
{
    s->flash[0] = riscv_flash_create(OBJECT(s), "virt.flash0", "pflash0",
                                     VIRT_FLASH_SECTOR_سائز);
    s->flash[1] = riscv_flash_create(OBJECT(s), "virt.flash1", "pflash1",
                                     VIRT_FLASH_SECTOR_سائز);
}

ساکن خالی virt_flash_map(RISCVVirtState *s,
                           حافظہ_Region *sysmem)
{
    hwaddr flashsize = s->memmap[VIRT_FLASH].سائز / 2;
    hwaddr flashbase = s->memmap[VIRT_FLASH].بنیاد;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_سائز);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_سائز);
}

ساکن خالی create_fdt_socket_plic(RISCVVirtState *s,
                                   صحیح socket,
                                   uint32_t *phandle, uint32_t *intc_phandles,
                                   uint32_t *plic_phandles)
{
    صحیح cpu;
    g_autofree حرف *plic_name = تہی;
    g_autofree uint32_t *plic_cells;
    MachineState *ms = MACHINE(s);
    غیرعلامتی طویل plic_addr = s->memmap[VIRT_PLIC].بنیاد +
                              (s->memmap[VIRT_PLIC].سائز * socket);
    bool numa_enabled = riscv_numa_enabled(MACHINE(s));
    uint32_t cells_لمبائی;

    plic_name = g_strdup_printf("/soc/interrupt-controller@%lx", plic_addr);

    اگر (kvm_enabled()) {
        cells_لمبائی = s->soc[socket].عدد_harts * 2;
        plic_cells = g_new0(uint32_t, cells_لمبائی);

        برائے (cpu = 0; cpu < s->soc[socket].عدد_harts; cpu++) {
            plic_cells[cpu * 2 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
            plic_cells[cpu * 2 + 1] = cpu_مقصد_be32(IRQ_S_EXT);
        }
   } ورنہ {
        cells_لمبائی = s->soc[socket].عدد_harts * 4;
        plic_cells = g_new0(uint32_t, cells_لمبائی);

        برائے (cpu = 0; cpu < s->soc[socket].عدد_harts; cpu++) {
            plic_cells[cpu * 4 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 1] = cpu_مقصد_be32(IRQ_M_EXT);
            plic_cells[cpu * 4 + 2] = cpu_مقصد_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 3] = cpu_مقصد_be32(IRQ_S_EXT);
        }
    }

    plic_phandles[socket] = (*phandle)++;

    riscv_create_fdt_plic(ms->fdt, plic_addr, s->memmap[VIRT_PLIC].سائز,
                          plic_phandles[socket], FDT_PLIC_INT_CELLS,
                          FDT_PLIC_ADDR_CELLS, plic_cells,
                          cells_لمبائی * حجم(uint32_t),
                          VIRT_IRQCHIP_عدد_SOURCES - 1,
                          numa_enabled, socket);

    اگر (!socket) {
        platform_bus_جوڑو_all_fdt_nodes(ms->fdt, plic_name,
                                       s->memmap[VIRT_PLATFORM_BUS].بنیاد,
                                       s->memmap[VIRT_PLATFORM_BUS].سائز,
                                       VIRT_PLATFORM_BUS_IRQ);
    }
}

ساکن خالی create_fdt_pmu(RISCVVirtState *s)
{
    g_autofree حرف *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_جوڑو_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_گرہ(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

ساکن خالی create_fdt_sockets(RISCVVirtState *s,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
                               uint32_t *irq_virtio_phandle,
                               uint32_t *msi_pcie_phandle)
{
    صحیح socket, phandle_pos;
    MachineState *ms = MACHINE(s);
    uint32_t msi_m_phandle = 0, msi_s_phandle = 0;
    uint32_t xplic_phandles[MAX_NODES];
    g_autofree uint32_t *intc_phandles = تہی;
    صحیح socket_گنتی = riscv_socket_گنتی(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_ثنائی_ہندسہ = riscv_is_32bit(&s->soc[0]);
    APLICFdtProps aplic_props;
    ACLINTFdtProps aclint_props;

    riscv_fdt_create_cpu_socket_subnode(ms->fdt,
        kvm_enabled() ? kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    intc_phandles = g_new0(uint32_t, ms->smp.cpus);

    اگر (virt_aclint_allowed() && s->have_aclint) {
        aclint_props.clint = &s->memmap[VIRT_CLINT];
        aclint_props.aclint_sswi = &s->memmap[VIRT_ACLINT_SSWI];
        aclint_props.aia_type = s->aia_type;
        aclint_props.numa_enabled = numa_enabled;
    }

    phandle_pos = ms->smp.cpus;
    برائے (socket = (socket_گنتی - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[VIRT_DRAM].بنیاد +
                         riscv_socket_mem_انحراف(ms, socket);
        uint64_t memsize = riscv_socket_mem_سائز(ms, socket);

        phandle_pos -= s->soc[socket].عدد_harts;

        riscv_create_fdt_socket_cpus(ms->fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].عدد_harts,
                                     s->soc[socket].hartid_بنیاد,
                                     phandle, &intc_phandles[phandle_pos],
                                     numa_enabled, is_32_ثنائی_ہندسہ);

        riscv_create_fdt_socket_حافظہ(ms->fdt, memaddr, memsize,
                                       socket, riscv_numa_enabled(ms));

        اگر (virt_aclint_allowed() && s->have_aclint) {
            aclint_props.socket = socket;
            aclint_props.عدد_harts = s->soc[socket].عدد_harts;
            riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                           &intc_phandles[phandle_pos]);
        } ورنہ اگر (tcg_enabled()) {
            hwaddr clintaddr = s->memmap[VIRT_CLINT].بنیاد +
                               s->memmap[VIRT_CLINT].سائز * socket;

            riscv_create_fdt_socket_clint(ms->fdt, clintaddr,
                                          s->memmap[VIRT_CLINT].سائز,
                                          socket, &intc_phandles[phandle_pos],
                                          s->soc[socket].عدد_harts,
                                          numa_enabled);
        }
    }

    اگر (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        IMSICFdtProps props = {
            .soc = &s->soc,
            .socket_گنتی = riscv_socket_گنتی(ms),
            .smp_cpus = ms->smp.cpus,
            .imsic_m_بنیاد = !kvm_enabled() ? s->memmap[VIRT_IMSIC_M].بنیاد : 0,
            .imsic_s_بنیاد = s->memmap[VIRT_IMSIC_S].بنیاد,
            .imsic_گروہ_max_سائز = VIRT_IMSIC_گروہ_MAX_سائز,
            .irqchip_عدد_msis = VIRT_IRQCHIP_عدد_MSIS,
            .aia_guests = s->aia_guests
        };

        riscv_create_fdt_imsic(ms->fdt, &props, phandle, intc_phandles,
                               &msi_m_phandle, &msi_s_phandle);

        *msi_pcie_phandle = msi_s_phandle;
    }

    اگر (s->aia_type != VIRT_AIA_TYPE_NONE) {
        aplic_props.aplic_m = !kvm_enabled() ? &s->memmap[VIRT_APLIC_M] : تہی;
        aplic_props.aplic_s = &s->memmap[VIRT_APLIC_S];
        aplic_props.platform_bus = &s->memmap[VIRT_PLATFORM_BUS];
        aplic_props.platform_bus_irq = VIRT_PLATFORM_BUS_IRQ;
        aplic_props.socket = 0;
        aplic_props.عدد_harts = ms->smp.cpus;
        aplic_props.numa_enabled = numa_enabled;
        aplic_props.irqchip_عدد_sources = VIRT_IRQCHIP_عدد_SOURCES;
        aplic_props.aia_type = s->aia_type;
    }

    /*
     * With KVM AIA aplic-imsic, using an irqchip without split
     * mode, we'll use only one APLIC instance.
     */
    اگر (!virt_use_emulated_aplic(s->aia_type)) {
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[0], xplic_phandles);

        *irq_mmio_phandle = xplic_phandles[0];
        *irq_virtio_phandle = xplic_phandles[0];
        *irq_pcie_phandle = xplic_phandles[0];
    } ورنہ {
        phandle_pos = ms->smp.cpus;
        برائے (socket = (socket_گنتی - 1); socket >= 0; socket--) {
            phandle_pos -= s->soc[socket].عدد_harts;

            اگر (s->aia_type == VIRT_AIA_TYPE_NONE) {
                create_fdt_socket_plic(s, socket, phandle,
                                       &intc_phandles[phandle_pos],
                                       xplic_phandles);
            } ورنہ {
                aplic_props.socket = socket;
                aplic_props.عدد_harts = s->soc[socket].عدد_harts;
                riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                              msi_m_phandle, msi_s_phandle,
                                              phandle,
                                              &intc_phandles[phandle_pos],
                                              xplic_phandles);
            }
        }

        برائے (socket = 0; socket < socket_گنتی; socket++) {
            اگر (socket == 0) {
                *irq_mmio_phandle = xplic_phandles[socket];
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            اگر (socket == 1) {
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            اگر (socket == 2) {
                *irq_pcie_phandle = xplic_phandles[socket];
            }
        }
    }

    riscv_socket_fdt_write_فاصلہ_میٹرکس(ms);
}

ساکن خالی create_fdt_virtio(RISCVVirtState *s, uint32_t irq_virtio_phandle)
{
    صحیح i;
    MachineState *ms = MACHINE(s);
    hwaddr virtio_بنیاد = s->memmap[VIRT_VIRTIO].بنیاد;

    برائے (i = 0; i < VIRTIO_گنتی; i++) {
        g_autofree حرف *name = تہی;
        uint64_t سائز = s->memmap[VIRT_VIRTIO].سائز;
        hwaddr addr = virtio_بنیاد + i * سائز;

        name = g_strdup_printf("/soc/virtio_mmio@%"HWADDR_PRIx, addr);

        qemu_fdt_جوڑو_subnode(ms->fdt, name);
        qemu_fdt_setprop_string(ms->fdt, name, "compatible", "virtio,mmio");
        qemu_fdt_setprop_sized_cells(ms->fdt, name, "reg", 2, addr, 2, سائز);
        qemu_fdt_setprop_cell(ms->fdt, name, "interrupt-parent",
            irq_virtio_phandle);
        اگر (s->aia_type == VIRT_AIA_TYPE_NONE) {
            qemu_fdt_setprop_cell(ms->fdt, name, "interrupts",
                                  VIRTIO_IRQ + i);
        } ورنہ {
            qemu_fdt_setprop_cells(ms->fdt, name, "interrupts",
                                   VIRTIO_IRQ + i, 0x4);
        }
    }
}

ساکن خالی create_fdt_uarts(RISCVVirtState *s, uint32_t irq_mmio_phandle)
{
    g_autofree حرف *name = تہی;
    MachineState *ms = MACHINE(s);

    اگر (s->uart1_present) {
        riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART1],
                              UART1_IRQ, s->aia_type, irq_mmio_phandle);
    }

    riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART0], UART0_IRQ,
                          s->aia_type, irq_mmio_phandle);

    name = riscv_fdt_get_uart_nodename(s->memmap[VIRT_UART0].بنیاد);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);
}

ساکن خالی create_fdt_fw_cfg(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    hwaddr بنیاد = s->memmap[VIRT_FW_CFG].بنیاد;
    hwaddr سائز = s->memmap[VIRT_FW_CFG].سائز;
    g_autofree حرف *nodename = g_strdup_printf("/fw-cfg@%" PRIx64, بنیاد);

    qemu_fdt_جوڑو_subnode(ms->fdt, nodename);
    qemu_fdt_setprop_string(ms->fdt, nodename,
                            "compatible", "qemu,fw-cfg-mmio");
    qemu_fdt_setprop_sized_cells(ms->fdt, nodename, "reg",
                                 2, بنیاد, 2, سائز);
    qemu_fdt_setprop(ms->fdt, nodename, "dma-coherent", تہی, 0);
}

ساکن خالی create_fdt_virtio_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ثابت حرف compat[] = "virtio,pci-iommu\0pci1af4,1057";
    خالی *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree حرف *iommu_گرہ = تہی;
    g_autofree حرف *pci_گرہ = تہی;

    pci_گرہ = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].بنیاد);
    iommu_گرہ = g_strdup_printf("%s/virtio_iommu@%x,%x", pci_گرہ,
                                 PCI_خانہ(bdf), PCI_FUNC(bdf));
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);

    qemu_fdt_جوڑو_subnode(fdt, iommu_گرہ);

    qemu_fdt_setprop(fdt, iommu_گرہ, "compatible", compat, حجم(compat));
    qemu_fdt_setprop_sized_cells(fdt, iommu_گرہ, "reg",
                                 1, bdf << 8, 1, 0, 1, 0,
                                 1, 0, 1, 0);
    qemu_fdt_setprop_cell(fdt, iommu_گرہ, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_گرہ, "phandle", iommu_phandle);

    qemu_fdt_setprop_cells(fdt, pci_گرہ, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
}

ساکن خالی create_fdt_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ثابت حرف comp[] = "riscv,pci-iommu";
    خالی *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree حرف *iommu_گرہ = تہی;
    g_autofree حرف *pci_گرہ = تہی;

    pci_گرہ = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].بنیاد);
    iommu_گرہ = g_strdup_printf("%s/iommu@%x", pci_گرہ, bdf);
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_جوڑو_subnode(fdt, iommu_گرہ);

    qemu_fdt_setprop(fdt, iommu_گرہ, "compatible", comp, حجم(comp));
    qemu_fdt_setprop_cell(fdt, iommu_گرہ, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_گرہ, "phandle", iommu_phandle);
    qemu_fdt_setprop_cells(fdt, iommu_گرہ, "reg",
                           bdf << 8, 0, 0, 0, 0);
    qemu_fdt_setprop_cells(fdt, pci_گرہ, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
    s->pci_iommu_bdf = bdf;
}

ساکن خالی finalize_fdt(RISCVVirtState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, irq_virtio_phandle = 1;
    uint32_t iommu_sys_phandle = 1;

    create_fdt_sockets(s, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &irq_virtio_phandle,
                       &msi_pcie_phandle);

    create_fdt_virtio(s, irq_virtio_phandle);

    اگر (virt_is_iommu_sys_enabled(s)) {
        iommu_sys_phandle =
            riscv_create_fdt_riscv_iommu_sys(MACHINE(s)->fdt,
                                             s->memmap[VIRT_IOMMU_SYS].بنیاد,
                                             s->memmap[VIRT_IOMMU_SYS].سائز,
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
                          &virt_بلند_pcie_memmap,
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, PCIE_IRQ);

    riscv_create_fdt_syscon(MACHINE(s)->fdt, &phandle,
                            s->memmap[VIRT_TEST].بنیاد,
                            s->memmap[VIRT_TEST].سائز,
                            FINISHER_RESET, FINISHER_PASS, true);

    create_fdt_uarts(s, irq_mmio_phandle);

    riscv_create_fdt_rtc(MACHINE(s)->fdt, &s->memmap[VIRT_RTC], RTC_IRQ,
                         s->aia_type, irq_mmio_phandle);
}

ساکن خالی create_fdt(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_بیج[32];
    g_autofree حرف *name = تہی;

    ms->fdt = riscv_create_board_device_درخت("riscv-virtio,qemu",
                                             "riscv-virtio",
                                             &s->fdt_سائز);

    /*
     * The "/soc/pci@..." node is needed for PCIE hotplugs
     * that might happen before finalize_fdt().
     */
    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                           s->memmap[VIRT_PCIE_ECAM].بنیاد);
    qemu_fdt_جوڑو_subnode(ms->fdt, name);

    qemu_fdt_جوڑو_subnode(ms->fdt, "/chosen");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_بیج, حجم(rng_بیج));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_بیج, حجم(rng_بیج));

    qemu_fdt_جوڑو_subnode(ms->fdt, "/aliases");

    riscv_create_fdt_flash(ms->fdt, s->memmap[VIRT_FLASH].بنیاد,
                           s->memmap[VIRT_FLASH].سائز / 2);
    create_fdt_fw_cfg(s);
    create_fdt_pmu(s);
}

ساکن FWCfgState *create_fw_cfg(ثابت MachineState *ms, hwaddr بنیاد)
{
    FWCfgState *fw_cfg;

    fw_cfg = fw_cfg_init_mem_dma(بنیاد, &پتہ_space_حافظہ);
    fw_cfg_جوڑو_i16(fw_cfg, FW_CFG_NB_CPUS, (uint16_t)ms->smp.cpus);

    واپس fw_cfg;
}

ساکن DeviceState *virt_create_plic(ثابت Mem_Map_اندراج *memmap, صحیح socket,
                                     صحیح بنیاد_hartid, صحیح hart_گنتی)
{
    g_autofree حرف *plic_hart_config = تہی;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hart_گنتی);

    /* Per-socket PLIC */
    واپس sifive_plic_create(
             get_system_حافظہ(),
             memmap[VIRT_PLIC].بنیاد + socket * memmap[VIRT_PLIC].سائز,
             plic_hart_config, hart_گنتی, بنیاد_hartid,
             VIRT_IRQCHIP_عدد_SOURCES,
             ((1U << VIRT_IRQCHIP_عدد_PRIO_بٹ) - 1),
             VIRT_PLIC_ترجیح_بنیاد, VIRT_PLIC_PENDING_بنیاد,
             VIRT_PLIC_ENABLE_بنیاد, VIRT_PLIC_ENABLE_STRIDE,
             VIRT_PLIC_CONTEXT_بنیاد,
             VIRT_PLIC_CONTEXT_STRIDE,
             memmap[VIRT_PLIC].سائز);
}

ساکن خالی virt_build_smbios(RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    uint8_t *smbios_tables, *smbios_anchor;
    سائز_t smbios_tables_len, smbios_anchor_len;
    ساخت smbios_phys_mem_area mem_array;
    ثابت حرف *حاصل_ضرب = "QEMU Virtual Machine";

    اگر (kvm_enabled()) {
        حاصل_ضرب = "KVM Virtual Machine";
    }

    smbios_سیٹ_defaults("QEMU", حاصل_ضرب, mc->name);

    اگر (riscv_is_32bit(&s->soc[0])) {
        smbios_سیٹ_default_processor_family(0x200);
    } ورنہ {
        smbios_سیٹ_default_processor_family(0x201);
    }

    /* build the array of physical mem area from base_memmap */
    mem_array.پتہ = s->memmap[VIRT_DRAM].بنیاد;
    mem_array.لمبائی = ms->ram_سائز;

    smbios_get_tables(ms, SMBIOS_اندراج_نقطہ_TYPE_64,
                      &mem_array, 1,
                      &smbios_tables, &smbios_tables_len,
                      &smbios_anchor, &smbios_anchor_len,
                      &error_fatal);

    اگر (smbios_anchor) {
        fw_cfg_جوڑو_file(s->fw_cfg, "etc/smbios/smbios-tables",
                        smbios_tables, smbios_tables_len);
        fw_cfg_جوڑو_file(s->fw_cfg, "etc/smbios/smbios-anchor",
                        smbios_anchor, smbios_anchor_len);
    }
}

ساکن خالی virt_machine_مکمل(Notifier *notifier, خالی *ڈیٹا)
{
    RISCVVirtState *s = container_کا(notifier, RISCVVirtState,
                                     machine_مکمل);
    MachineState *machine = MACHINE(s);
    hwaddr آغاز_addr = s->memmap[VIRT_DRAM].بنیاد;
    hwaddr firmware_انتہا_addr;
    vaddr kernel_آغاز_addr;
    ثابت حرف *firmware_name = riscv_default_firmware_name(&s->soc[0]);
    uint64_t fdt_بھرو_addr;
    uint64_t kernel_اندراج = 0;
    بلاک_Backend *pflash_blk0;
    RISCVBootInfo boot_info;

    /*
     * An user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    اگر (machine->dtb == تہی) {
        finalize_fdt(s);
    }

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * so the "-bios" parameter is not supported when KVM is enabled.
     */
    اگر (kvm_enabled()) {
        اگر (machine->firmware) {
            اگر (strcmp(machine->firmware, "none")) {
                error_report("Machine mode firmware is not supported in "
                             "combination with KVM.");
                باہر_نکلو(1);
            }
        } ورنہ {
            machine->firmware = g_strdup("none");
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_انتہا_addr = riscv_ڈھونڈو_and_بھرو_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &آغاز_addr, تہی);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    اگر (pflash_blk0) {
        اگر (machine->firmware && !strcmp(machine->firmware, "none") &&
            !kvm_enabled()) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            آغاز_addr = s->memmap[VIRT_FLASH].بنیاد;
        } ورنہ {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_تیاری_firmware_boot(machine);
            kernel_اندراج = s->memmap[VIRT_FLASH].بنیاد;
        }
    }

    اگر (machine->kernel_filename && !kernel_اندراج) {
        kernel_آغاز_addr = riscv_calc_kernel_آغاز_addr(&boot_info,
                                                         firmware_انتہا_addr);
        riscv_بھرو_kernel(machine, &boot_info, kernel_آغاز_addr,
                          true, تہی);
        kernel_اندراج = boot_info.image_کم_addr;
    }

    fdt_بھرو_addr = riscv_compute_fdt_addr(s->memmap[VIRT_DRAM].بنیاد,
                                           s->memmap[VIRT_DRAM].سائز,
                                           machine, &boot_info);
    riscv_بھرو_fdt(fdt_بھرو_addr, machine->fdt);

    /* load the reset vector */
    riscv_تیاری_rom_reset_vec(machine, &s->soc[0], آغاز_addr,
                              s->memmap[VIRT_MROM].بنیاد,
                              s->memmap[VIRT_MROM].سائز, kernel_اندراج,
                              fdt_بھرو_addr);

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * So here setup kernel start address and fdt address.
     * TODO:Support firmware loading and integrate to TCG start
     */
    اگر (kvm_enabled()) {
        riscv_تیاری_براہ_راست_kernel(kernel_اندراج, fdt_بھرو_addr);
    }

    virt_build_smbios(s);

    اگر (virt_is_acpi_enabled(s)) {
        virt_acpi_تیاری(s);
    }
}

ساکن خالی virt_machine_init(MachineState *machine)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);
    حافظہ_Region *system_حافظہ = get_system_حافظہ();
    حافظہ_Region *نقاب_rom = g_new(حافظہ_Region, 1);
    DeviceState *mmio_irqchip, *virtio_irqchip, *pcie_irqchip;
    صحیح i, بنیاد_hartid, hart_گنتی;
    صحیح socket_گنتی = riscv_socket_گنتی(machine);

    s->memmap = virt_memmap;

    /* Check socket count limit */
    اگر (VIRT_SOCKETS_MAX < socket_گنتی) {
        error_report("number of sockets/nodes should be less than %d",
            VIRT_SOCKETS_MAX);
        باہر_نکلو(1);
    }

    اگر (!virt_aclint_allowed() && s->have_aclint) {
        error_report("'aclint' is only available with TCG acceleration");
        باہر_نکلو(1);
    }

    /* Initialize sockets */
    mmio_irqchip = virtio_irqchip = pcie_irqchip = تہی;
    برائے (i = 0; i < socket_گنتی; i++) {
        g_autofree حرف *soc_name = g_strdup_printf("soc%d", i);

        اگر (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            باہر_نکلو(1);
        }

        بنیاد_hartid = riscv_socket_پہلا_hartid(machine, i);
        اگر (بنیاد_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            باہر_نکلو(1);
        }

        hart_گنتی = riscv_socket_hart_گنتی(machine, i);
        اگر (hart_گنتی < 0) {
            error_report("can't find hart count for socket%d", i);
            باہر_نکلو(1);
        }

        object_initialize_بچہ(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        object_property_سیٹ_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_سیٹ_int(OBJECT(&s->soc[i]), "hartid-base",
                                بنیاد_hartid, &error_abort);
        object_property_سیٹ_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_گنتی, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        اگر (virt_aclint_allowed() && s->have_aclint) {
            اگر (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
                /* Per-socket ACLINT MTIMER */
                riscv_aclint_mtimer_create(system_حافظہ,
                        s->memmap[VIRT_CLINT].بنیاد +
                            i * RISCV_ACLINT_DEFAULT_MTIMER_سائز,
                        RISCV_ACLINT_DEFAULT_MTIMER_سائز,
                        بنیاد_hartid, hart_گنتی,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
            } ورنہ {
                /* Per-socket ACLINT MSWI, MTIMER, and SSWI */
                riscv_aclint_swi_create(system_حافظہ,
                        s->memmap[VIRT_CLINT].بنیاد +
                            i * s->memmap[VIRT_CLINT].سائز,
                        بنیاد_hartid, hart_گنتی, false);
                riscv_aclint_mtimer_create(system_حافظہ,
                        s->memmap[VIRT_CLINT].بنیاد +
                            i * s->memmap[VIRT_CLINT].سائز +
                            RISCV_ACLINT_SWI_سائز,
                        RISCV_ACLINT_DEFAULT_MTIMER_سائز,
                        بنیاد_hartid, hart_گنتی,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
                riscv_aclint_swi_create(system_حافظہ,
                        s->memmap[VIRT_ACLINT_SSWI].بنیاد +
                            i * s->memmap[VIRT_ACLINT_SSWI].سائز,
                        بنیاد_hartid, hart_گنتی, true);
            }
        } ورنہ اگر (tcg_enabled()) {
            /* Per-socket SiFive CLINT */
            riscv_aclint_swi_create(system_حافظہ,
                    s->memmap[VIRT_CLINT].بنیاد + i * s->memmap[VIRT_CLINT].سائز,
                    بنیاد_hartid, hart_گنتی, false);
            riscv_aclint_mtimer_create(system_حافظہ,
                    s->memmap[VIRT_CLINT].بنیاد +
                    i * s->memmap[VIRT_CLINT].سائز + RISCV_ACLINT_SWI_سائز,
                    RISCV_ACLINT_DEFAULT_MTIMER_سائز, بنیاد_hartid, hart_گنتی,
                    RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
                    RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
        }

        /* Per-socket interrupt controller */
        اگر (s->aia_type == VIRT_AIA_TYPE_NONE) {
            s->irqchip[i] = virt_create_plic(s->memmap, i,
                                             بنیاد_hartid, hart_گنتی);
        } ورنہ {
            bool msimode = s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;
            صحیح imsic_بٹ = imsic_عدد_بٹ(s->aia_guests + 1);
            s->irqchip[i] = riscv_create_aia(system_حافظہ, msimode,
                                             s->aia_guests,
                                             IMSIC_HART_سائز(0),
                                             IMSIC_HART_سائز(imsic_بٹ),
                                             s->عدد_sources,
                                             &s->memmap[VIRT_APLIC_M],
                                             &s->memmap[VIRT_APLIC_S],
                                             &s->memmap[VIRT_IMSIC_M],
                                             &s->memmap[VIRT_IMSIC_S],
                                             i, بنیاد_hartid, hart_گنتی,
                                             VIRT_IRQCHIP_عدد_MSIS,
                                             VIRT_IRQCHIP_عدد_PRIO_بٹ);
        }

        /* Try to use different IRQCHIP instance based device type */
        اگر (i == 0) {
            mmio_irqchip = s->irqchip[i];
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        اگر (i == 1) {
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        اگر (i == 2) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    اگر (kvm_enabled() && virt_use_kvm_aia_aplic_imsic(s->aia_type)) {
        kvm_riscv_aia_create(machine, IMSIC_MMIO_گروہ_MIN_سرکاؤ,
                             VIRT_IRQCHIP_عدد_SOURCES, VIRT_IRQCHIP_عدد_MSIS,
                             s->memmap[VIRT_APLIC_S].بنیاد,
                             s->memmap[VIRT_IMSIC_S].بنیاد,
                             s->aia_guests);
    }

    اگر (riscv_is_32bit(&s->soc[0])) {
#اگر HOST_LONG_بٹ == 64
        /* limit RAM size in a 32-bit system */
        اگر (machine->ram_سائز > 10 * GiB) {
            machine->ram_سائز = 10 * GiB;
            error_report("Limiting RAM size to 10 GiB");
        }
#اگر_ختم
        virt_بلند_pcie_memmap.بنیاد = VIRT32_بلند_PCIE_MMIO_بنیاد;
        virt_بلند_pcie_memmap.سائز = VIRT32_بلند_PCIE_MMIO_سائز;
    } ورنہ {
        virt_بلند_pcie_memmap.سائز = VIRT64_بلند_PCIE_MMIO_سائز;
        virt_بلند_pcie_memmap.بنیاد = s->memmap[VIRT_DRAM].بنیاد +
                                     machine->ram_سائز;
        virt_بلند_pcie_memmap.بنیاد =
            باری_اوپر(virt_بلند_pcie_memmap.بنیاد, virt_بلند_pcie_memmap.سائز);
    }

    /* register system main memory (actual RAM) */
    حافظہ_region_جوڑو_subregion(system_حافظہ, s->memmap[VIRT_DRAM].بنیاد,
                                machine->ram);

    /* boot rom */
    حافظہ_region_init_rom(نقاب_rom, تہی, "riscv_virt_board.mrom",
                           s->memmap[VIRT_MROM].سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion(system_حافظہ, s->memmap[VIRT_MROM].بنیاد,
                                نقاب_rom);

    /*
     * Init fw_cfg. Must be done before riscv_load_fdt, otherwise the
     * device tree cannot be altered and we get FDT_ERR_NOSPACE.
     */
    s->fw_cfg = create_fw_cfg(machine, s->memmap[VIRT_FW_CFG].بنیاد);
    rom_سیٹ_fw(s->fw_cfg);

    /* SiFive Test MMIO device */
    sifive_test_create(s->memmap[VIRT_TEST].بنیاد);

    /* VirtIO MMIO devices */
    برائے (i = 0; i < VIRTIO_گنتی; i++) {
        sysbus_create_simple("virtio-mmio",
            s->memmap[VIRT_VIRTIO].بنیاد + i * s->memmap[VIRT_VIRTIO].سائز,
            qdev_get_gpio_اندر(virtio_irqchip, VIRTIO_IRQ + i));
    }

    riscv_gpex_pcie_init(system_حافظہ, pcie_irqchip,
                   &s->memmap[VIRT_PCIE_ECAM], &s->memmap[VIRT_PCIE_MMIO],
                   &virt_بلند_pcie_memmap, &s->memmap[VIRT_PCIE_PIO],
                   PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
        &s->memmap[VIRT_PLATFORM_BUS], VIRT_PLATFORM_BUS_IRQ,
        VIRT_PLATFORM_BUS_عدد_IRQS);

    serial_mm_init(system_حافظہ, s->memmap[VIRT_UART0].بنیاد,
        0, qdev_get_gpio_اندر(mmio_irqchip, UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    اگر (serial_hd(1)) {
        serial_mm_init(system_حافظہ, s->memmap[VIRT_UART1].بنیاد,
            0, qdev_get_gpio_اندر(mmio_irqchip, UART1_IRQ), 399193,
            serial_hd(1), DEVICE_LITTLE_ENDIAN);
        s->uart1_present = true;
    }

    sysbus_create_simple("goldfish_rtc", s->memmap[VIRT_RTC].بنیاد,
        qdev_get_gpio_اندر(mmio_irqchip, RTC_IRQ));

    برائے (i = 0; i < ARRAY_سائز(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    virt_flash_map(s, system_حافظہ);

    /* load/create device tree */
    اگر (machine->dtb) {
        machine->fdt = بھرو_device_درخت(machine->dtb, &s->fdt_سائز);
        اگر (!machine->fdt) {
            error_report("load_device_tree() failed");
            باہر_نکلو(1);
        }
    } ورنہ {
        create_fdt(s);
    }

    اگر (virt_is_iommu_sys_enabled(s)) {
        riscv_create_iommu_sys(mmio_irqchip, s->memmap[VIRT_IOMMU_SYS].بنیاد,
                               IOMMU_SYS_IRQ, riscv_is_32bit(&s->soc[0]));
    }

    s->machine_مکمل.notify = virt_machine_مکمل;
    qemu_جوڑو_machine_init_مکمل_notifier(&s->machine_مکمل);
}

ساکن خالی virt_machine_instance_finalize(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    برائے (صحیح i = 0; i < ARRAY_سائز(s->flash); i++) {
        اگر (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
    g_free(s->oem_id);
    g_free(s->oem_جدول_id);
}

ساکن خالی virt_machine_instance_init(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    virt_flash_create(s);

    s->oem_id = g_strndup(ACPI_BUILD_APPNAME6, 6);
    s->oem_جدول_id = g_strndup(ACPI_BUILD_APPNAME8, 8);
    s->acpi = ON_OFF_AUTO_AUTO;
    s->iommu_sys = ON_OFF_AUTO_AUTO;
    s->عدد_sources = VIRT_IRQCHIP_عدد_SOURCES;
}

ساکن حرف *virt_get_aia_guests(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    واپس g_strdup_printf("%d", s->aia_guests);
}

ساکن خالی virt_سیٹ_aia_guests(Object *obj, ثابت حرف *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->aia_guests = atoi(val);
    اگر (s->aia_guests < 0 || s->aia_guests > VIRT_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          VIRT_IRQCHIP_MAX_GUESTS);
    }
}

ساکن حرف *virt_get_aia(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    ثابت حرف *val;

    انتخاب (s->aia_type) {
    حالت VIRT_AIA_TYPE_APLIC:
        val = "aplic";
        توڑو;
    حالت VIRT_AIA_TYPE_APLIC_IMSIC:
        val = "aplic-imsic";
        توڑو;
    طےشدہ:
        val = "none";
        توڑو;
    };

    واپس g_strdup(val);
}

ساکن خالی virt_سیٹ_aia(Object *obj, ثابت حرف *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    اگر (!strcmp(val, "none")) {
        s->aia_type = VIRT_AIA_TYPE_NONE;
    } ورنہ اگر (!strcmp(val, "aplic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC;
    } ورنہ اگر (!strcmp(val, "aplic-imsic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC_IMSIC;
    } ورنہ {
        error_setg(errp, "Invalid AIA interrupt controller type");
        error_append_hint(errp, "Valid values are none, aplic, and "
                          "aplic-imsic.\n");
    }
}

ساکن bool virt_get_aclint(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    واپس s->have_aclint;
}

ساکن خالی virt_سیٹ_aclint(Object *obj, bool قدر, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->have_aclint = قدر;
}

bool virt_is_iommu_sys_enabled(RISCVVirtState *s)
{
    واپس s->iommu_sys == ON_OFF_AUTO_ON;
}

ساکن خالی virt_get_iommu_sys(Object *obj, Visitor *v, ثابت حرف *name,
                               خالی *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto iommu_sys = s->iommu_sys;

    دورہ_type_On_Off_Auto(v, name, &iommu_sys, errp);
}

ساکن خالی virt_سیٹ_iommu_sys(Object *obj, Visitor *v, ثابت حرف *name,
                               خالی *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    دورہ_type_On_Off_Auto(v, name, &s->iommu_sys, errp);
}

bool virt_is_acpi_enabled(RISCVVirtState *s)
{
    واپس s->acpi != ON_OFF_AUTO_OFF;
}

ساکن خالی virt_get_acpi(Object *obj, Visitor *v, ثابت حرف *name,
                          خالی *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto acpi = s->acpi;

    دورہ_type_On_Off_Auto(v, name, &acpi, errp);
}

ساکن خالی virt_سیٹ_acpi(Object *obj, Visitor *v, ثابت حرف *name,
                          خالی *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    دورہ_type_On_Off_Auto(v, name, &s->acpi, errp);
}

ساکن ثابت HotplugHandler *
virt_machine_get_hotplug_handler(MachineState *machine, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);

    اگر (device_is_dynamic_sysbus(mc, dev) ||
        object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI) ||
        object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        s->iommu_sys = ON_OFF_AUTO_OFF;
        واپس HOTPLUG_HANDLER(machine);
    }

    واپس تہی;
}

ساکن خالی virt_machine_device_plug_cb(ثابت HotplugHandler *hotplug_dev,
                                        DeviceState *dev, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(hotplug_dev);

    اگر (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        اگر (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }

    اگر (object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI)) {
        create_fdt_virtio_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
    }

    اگر (object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        create_fdt_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
        s->iommu_sys = ON_OFF_AUTO_OFF;
    }
}

ساکن خالی virt_machine_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);

    mc->desc = "RISC-V VirtIO board";
    mc->init = virt_machine_init;
    mc->max_cpus = VIRT_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_بنیاد;
    mc->بلاک_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->pci_allow_0_پتہ = true;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_اشاریہ_مقصد_instance_props = riscv_numa_cpu_اشاریہ_مقصد_props;
    mc->get_default_cpu_گرہ_id = riscv_numa_get_default_cpu_گرہ_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_virt_board.ram";
    أكد(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = virt_machine_get_hotplug_handler;

    hc->plug = virt_machine_device_plug_cb;

    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_RAMFB_DEVICE);
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_UEFI_VARS_SYSBUS);
#اگر_تعریف_شدہ CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#اگر_ختم

    object_class_property_جوڑو_bool(oc, "aclint", virt_get_aclint,
                                   virt_سیٹ_aclint);
    object_class_property_سیٹ_description(oc, "aclint",
                                          "(TCG only) Set on/off to "
                                          "enable/disable emulating "
                                          "ACLINT devices");

    object_class_property_جوڑو_str(oc, "aia", virt_get_aia,
                                  virt_سیٹ_aia);
    object_class_property_سیٹ_description(oc, "aia",
                                          "Set type of AIA interrupt "
                                          "controller. Valid values are "
                                          "none, aplic, and aplic-imsic.");

    object_class_property_جوڑو_str(oc, "aia-guests",
                                  virt_get_aia_guests,
                                  virt_سیٹ_aia_guests);
    {
        g_autofree حرف *str =
            g_strdup_printf("Set number of guest MMIO pages for AIA IMSIC. "
                            "Valid value should be between 0 and %d.",
                            VIRT_IRQCHIP_MAX_GUESTS);
        object_class_property_سیٹ_description(oc, "aia-guests", str);
    }

    object_class_property_جوڑو(oc, "acpi", "OnOffAuto",
                              virt_get_acpi, virt_سیٹ_acpi,
                              تہی, تہی);
    object_class_property_سیٹ_description(oc, "acpi",
                                          "Enable ACPI");

    object_class_property_جوڑو(oc, "iommu-sys", "OnOffAuto",
                              virt_get_iommu_sys, virt_سیٹ_iommu_sys,
                              تہی, تہی);
    object_class_property_سیٹ_description(oc, "iommu-sys",
                                          "Enable IOMMU platform device");
}

ساکن ثابت TypeInfo virt_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("virt"),
    .والد     = TYPE_MACHINE,
    .class_init = virt_machine_class_init,
    .instance_init = virt_machine_instance_init,
    .instance_finalize = virt_machine_instance_finalize,
    .instance_سائز = حجم(RISCVVirtState),
    .interfaces = (ثابت InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

ساکن خالی virt_machine_init_رجسٹر_types(خالی)
{
    type_رجسٹر_static(&virt_machine_typeinfo);
}

type_init(virt_machine_init_رجسٹر_types)
