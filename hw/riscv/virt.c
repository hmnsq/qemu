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

#ضمن "qemu/osdep.h"
#ضمن "qemu/units.h"
#ضمن "qemu/error-report.h"
#ضمن "qemu/guest-random.h"
#ضمن "qapi/error.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/core/sysbus.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/char/serial-mm.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/core/sysbus-fdt.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "hw/riscv/iommu.h"
#ضمن "hw/riscv/riscv-iommu.h"
#ضمن "hw/riscv/riscv-iommu-bits.h"
#ضمن "hw/riscv/virt.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/riscv/device-common.h"
#ضمن "hw/riscv/fdt-common.h"
#ضمن "hw/riscv/numa.h"
#ضمن "kvm/kvm_riscv.h"
#ضمن "hw/firmware/smbios.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/riscv_aplic.h"
#ضمن "hw/intc/sifive_plic.h"
#ضمن "hw/misc/sifive_test.h"
#ضمن "hw/core/platform-bus.h"
#ضمن "chardev/char.h"
#ضمن "system/device_tree.h"
#ضمن "system/system.h"
#ضمن "system/tcg.h"
#ضمن "system/kvm.h"
#ضمن "system/tpm.h"
#ضمن "system/qtest.h"
#ضمن "hw/pci/pci.h"
#ضمن "hw/pci-host/gpex.h"
#ضمن "hw/display/ramfb.h"
#ضمن "hw/acpi/aml-build.h"
#ضمن "qapi/qapi-visit-common.h"
#ضمن "hw/virtio/virtio-iommu.h"
#ضمن "hw/uefi/var-service-api.h"

#ضمن "aia.h"

/* KVM AIA only supports APLIC MSI. APLIC Wired is always emulated by QEMU. */
ساكن bool virt_use_kvm_aia_aplic_imsic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    أرجع riscv_is_kvm_aia_aplic_imsic(msimode);
}

ساكن bool virt_use_emulated_aplic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    أرجع riscv_use_emulated_aplic(msimode);
}

ساكن bool virt_aclint_allowed(فراغ)
{
    أرجع tcg_enabled() || qtest_enabled();
}

ساكن ثابت Mem_Map_مدخل virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_IOMMU_SYS] =    {  0x3010000,        0x1000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_اندازه(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_اندازه(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_اندازه(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    /* UART1 supports page isolation from UART0 */
    [VIRT_UART1] =        { 0x1000a000,         0x100 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_اندازه },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_اندازه },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    [VIRT_DRAM] =         { 0x80000000,           0x0 },
};

/* PCIe high mmio is fixed for RV32 */
#تعریف_کن VIRT32_بالا_PCIE_MMIO_پایه  0x300000000ULL
#تعریف_کن VIRT32_بالا_PCIE_MMIO_اندازه  (4 * GiB)

/* PCIe high mmio for RV64, size is fixed but base depends on top of RAM */
#تعریف_کن VIRT64_بالا_PCIE_MMIO_اندازه  (16 * GiB)

ساكن Mem_Map_مدخل virt_بالا_pcie_memmap;

#تعریف_کن VIRT_FLASH_SECTOR_اندازه (256 * KiB)

ساكن فراغ virt_flash_create(RISCVVirtState *s)
{
    s->flash[0] = riscv_flash_create(OBJECT(s), "virt.flash0", "pflash0",
                                     VIRT_FLASH_SECTOR_اندازه);
    s->flash[1] = riscv_flash_create(OBJECT(s), "virt.flash1", "pflash1",
                                     VIRT_FLASH_SECTOR_اندازه);
}

ساكن فراغ virt_flash_map(RISCVVirtState *s,
                           حافظه_Region *sysmem)
{
    hwaddr flashsize = s->memmap[VIRT_FLASH].اندازه / 2;
    hwaddr flashbase = s->memmap[VIRT_FLASH].پایه;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_اندازه);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_اندازه);
}

ساكن فراغ create_fdt_socket_plic(RISCVVirtState *s,
                                   صحيح socket,
                                   uint32_t *phandle, uint32_t *intc_phandles,
                                   uint32_t *plic_phandles)
{
    صحيح cpu;
    g_autofree حرف *plic_name = عدم;
    g_autofree uint32_t *plic_cells;
    MachineState *ms = MACHINE(s);
    بیعلامت دراز plic_addr = s->memmap[VIRT_PLIC].پایه +
                              (s->memmap[VIRT_PLIC].اندازه * socket);
    bool numa_enabled = riscv_numa_enabled(MACHINE(s));
    uint32_t cells_درازی;

    plic_name = g_strdup_printf("/soc/interrupt-controller@%lx", plic_addr);

    إذا (kvm_enabled()) {
        cells_درازی = s->soc[socket].رقم_harts * 2;
        plic_cells = g_new0(uint32_t, cells_درازی);

        لكل (cpu = 0; cpu < s->soc[socket].رقم_harts; cpu++) {
            plic_cells[cpu * 2 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
            plic_cells[cpu * 2 + 1] = cpu_مقصد_be32(IRQ_S_EXT);
        }
   } وإلا {
        cells_درازی = s->soc[socket].رقم_harts * 4;
        plic_cells = g_new0(uint32_t, cells_درازی);

        لكل (cpu = 0; cpu < s->soc[socket].رقم_harts; cpu++) {
            plic_cells[cpu * 4 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 1] = cpu_مقصد_be32(IRQ_M_EXT);
            plic_cells[cpu * 4 + 2] = cpu_مقصد_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 3] = cpu_مقصد_be32(IRQ_S_EXT);
        }
    }

    plic_phandles[socket] = (*phandle)++;

    riscv_create_fdt_plic(ms->fdt, plic_addr, s->memmap[VIRT_PLIC].اندازه,
                          plic_phandles[socket], FDT_PLIC_INT_CELLS,
                          FDT_PLIC_ADDR_CELLS, plic_cells,
                          cells_درازی * حجم(uint32_t),
                          VIRT_IRQCHIP_رقم_SOURCES - 1,
                          numa_enabled, socket);

    إذا (!socket) {
        platform_bus_جمع_کن_all_fdt_nodes(ms->fdt, plic_name,
                                       s->memmap[VIRT_PLATFORM_BUS].پایه,
                                       s->memmap[VIRT_PLATFORM_BUS].اندازه,
                                       VIRT_PLATFORM_BUS_IRQ);
    }
}

ساكن فراغ create_fdt_pmu(RISCVVirtState *s)
{
    g_autofree حرف *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_جمع_کن_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_گره(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

ساكن فراغ create_fdt_sockets(RISCVVirtState *s,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
                               uint32_t *irq_virtio_phandle,
                               uint32_t *msi_pcie_phandle)
{
    صحيح socket, phandle_pos;
    MachineState *ms = MACHINE(s);
    uint32_t msi_m_phandle = 0, msi_s_phandle = 0;
    uint32_t xplic_phandles[MAX_NODES];
    g_autofree uint32_t *intc_phandles = عدم;
    صحيح socket_شمار = riscv_socket_شمار(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_بیت = riscv_is_32bit(&s->soc[0]);
    APLICFdtProps aplic_props;
    ACLINTFdtProps aclint_props;

    riscv_fdt_create_cpu_socket_subnode(ms->fdt,
        kvm_enabled() ? kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    intc_phandles = g_new0(uint32_t, ms->smp.cpus);

    إذا (virt_aclint_allowed() && s->have_aclint) {
        aclint_props.clint = &s->memmap[VIRT_CLINT];
        aclint_props.aclint_sswi = &s->memmap[VIRT_ACLINT_SSWI];
        aclint_props.aia_type = s->aia_type;
        aclint_props.numa_enabled = numa_enabled;
    }

    phandle_pos = ms->smp.cpus;
    لكل (socket = (socket_شمار - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[VIRT_DRAM].پایه +
                         riscv_socket_mem_جابه_جایی(ms, socket);
        uint64_t memsize = riscv_socket_mem_اندازه(ms, socket);

        phandle_pos -= s->soc[socket].رقم_harts;

        riscv_create_fdt_socket_cpus(ms->fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].رقم_harts,
                                     s->soc[socket].hartid_پایه,
                                     phandle, &intc_phandles[phandle_pos],
                                     numa_enabled, is_32_بیت);

        riscv_create_fdt_socket_حافظه(ms->fdt, memaddr, memsize,
                                       socket, riscv_numa_enabled(ms));

        إذا (virt_aclint_allowed() && s->have_aclint) {
            aclint_props.socket = socket;
            aclint_props.رقم_harts = s->soc[socket].رقم_harts;
            riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                           &intc_phandles[phandle_pos]);
        } وإلا إذا (tcg_enabled()) {
            hwaddr clintaddr = s->memmap[VIRT_CLINT].پایه +
                               s->memmap[VIRT_CLINT].اندازه * socket;

            riscv_create_fdt_socket_clint(ms->fdt, clintaddr,
                                          s->memmap[VIRT_CLINT].اندازه,
                                          socket, &intc_phandles[phandle_pos],
                                          s->soc[socket].رقم_harts,
                                          numa_enabled);
        }
    }

    إذا (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        IMSICFdtProps props = {
            .soc = &s->soc,
            .socket_شمار = riscv_socket_شمار(ms),
            .smp_cpus = ms->smp.cpus,
            .imsic_m_پایه = !kvm_enabled() ? s->memmap[VIRT_IMSIC_M].پایه : 0,
            .imsic_s_پایه = s->memmap[VIRT_IMSIC_S].پایه,
            .imsic_گروه_max_اندازه = VIRT_IMSIC_گروه_MAX_اندازه,
            .irqchip_رقم_msis = VIRT_IRQCHIP_رقم_MSIS,
            .aia_guests = s->aia_guests
        };

        riscv_create_fdt_imsic(ms->fdt, &props, phandle, intc_phandles,
                               &msi_m_phandle, &msi_s_phandle);

        *msi_pcie_phandle = msi_s_phandle;
    }

    إذا (s->aia_type != VIRT_AIA_TYPE_NONE) {
        aplic_props.aplic_m = !kvm_enabled() ? &s->memmap[VIRT_APLIC_M] : عدم;
        aplic_props.aplic_s = &s->memmap[VIRT_APLIC_S];
        aplic_props.platform_bus = &s->memmap[VIRT_PLATFORM_BUS];
        aplic_props.platform_bus_irq = VIRT_PLATFORM_BUS_IRQ;
        aplic_props.socket = 0;
        aplic_props.رقم_harts = ms->smp.cpus;
        aplic_props.numa_enabled = numa_enabled;
        aplic_props.irqchip_رقم_sources = VIRT_IRQCHIP_رقم_SOURCES;
        aplic_props.aia_type = s->aia_type;
    }

    /*
     * With KVM AIA aplic-imsic, using an irqchip without split
     * mode, we'll use only one APLIC instance.
     */
    إذا (!virt_use_emulated_aplic(s->aia_type)) {
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[0], xplic_phandles);

        *irq_mmio_phandle = xplic_phandles[0];
        *irq_virtio_phandle = xplic_phandles[0];
        *irq_pcie_phandle = xplic_phandles[0];
    } وإلا {
        phandle_pos = ms->smp.cpus;
        لكل (socket = (socket_شمار - 1); socket >= 0; socket--) {
            phandle_pos -= s->soc[socket].رقم_harts;

            إذا (s->aia_type == VIRT_AIA_TYPE_NONE) {
                create_fdt_socket_plic(s, socket, phandle,
                                       &intc_phandles[phandle_pos],
                                       xplic_phandles);
            } وإلا {
                aplic_props.socket = socket;
                aplic_props.رقم_harts = s->soc[socket].رقم_harts;
                riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                              msi_m_phandle, msi_s_phandle,
                                              phandle,
                                              &intc_phandles[phandle_pos],
                                              xplic_phandles);
            }
        }

        لكل (socket = 0; socket < socket_شمار; socket++) {
            إذا (socket == 0) {
                *irq_mmio_phandle = xplic_phandles[socket];
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            إذا (socket == 1) {
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            إذا (socket == 2) {
                *irq_pcie_phandle = xplic_phandles[socket];
            }
        }
    }

    riscv_socket_fdt_write_فاصله_ماتریس(ms);
}

ساكن فراغ create_fdt_virtio(RISCVVirtState *s, uint32_t irq_virtio_phandle)
{
    صحيح i;
    MachineState *ms = MACHINE(s);
    hwaddr virtio_پایه = s->memmap[VIRT_VIRTIO].پایه;

    لكل (i = 0; i < VIRTIO_شمار; i++) {
        g_autofree حرف *name = عدم;
        uint64_t اندازه = s->memmap[VIRT_VIRTIO].اندازه;
        hwaddr addr = virtio_پایه + i * اندازه;

        name = g_strdup_printf("/soc/virtio_mmio@%"HWADDR_PRIx, addr);

        qemu_fdt_جمع_کن_subnode(ms->fdt, name);
        qemu_fdt_setprop_string(ms->fdt, name, "compatible", "virtio,mmio");
        qemu_fdt_setprop_sized_cells(ms->fdt, name, "reg", 2, addr, 2, اندازه);
        qemu_fdt_setprop_cell(ms->fdt, name, "interrupt-parent",
            irq_virtio_phandle);
        إذا (s->aia_type == VIRT_AIA_TYPE_NONE) {
            qemu_fdt_setprop_cell(ms->fdt, name, "interrupts",
                                  VIRTIO_IRQ + i);
        } وإلا {
            qemu_fdt_setprop_cells(ms->fdt, name, "interrupts",
                                   VIRTIO_IRQ + i, 0x4);
        }
    }
}

ساكن فراغ create_fdt_uarts(RISCVVirtState *s, uint32_t irq_mmio_phandle)
{
    g_autofree حرف *name = عدم;
    MachineState *ms = MACHINE(s);

    إذا (s->uart1_present) {
        riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART1],
                              UART1_IRQ, s->aia_type, irq_mmio_phandle);
    }

    riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART0], UART0_IRQ,
                          s->aia_type, irq_mmio_phandle);

    name = riscv_fdt_get_uart_nodename(s->memmap[VIRT_UART0].پایه);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);
}

ساكن فراغ create_fdt_fw_cfg(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    hwaddr پایه = s->memmap[VIRT_FW_CFG].پایه;
    hwaddr اندازه = s->memmap[VIRT_FW_CFG].اندازه;
    g_autofree حرف *nodename = g_strdup_printf("/fw-cfg@%" PRIx64, پایه);

    qemu_fdt_جمع_کن_subnode(ms->fdt, nodename);
    qemu_fdt_setprop_string(ms->fdt, nodename,
                            "compatible", "qemu,fw-cfg-mmio");
    qemu_fdt_setprop_sized_cells(ms->fdt, nodename, "reg",
                                 2, پایه, 2, اندازه);
    qemu_fdt_setprop(ms->fdt, nodename, "dma-coherent", عدم, 0);
}

ساكن فراغ create_fdt_virtio_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ثابت حرف compat[] = "virtio,pci-iommu\0pci1af4,1057";
    فراغ *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree حرف *iommu_گره = عدم;
    g_autofree حرف *pci_گره = عدم;

    pci_گره = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].پایه);
    iommu_گره = g_strdup_printf("%s/virtio_iommu@%x,%x", pci_گره,
                                 PCI_جایگاه(bdf), PCI_FUNC(bdf));
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);

    qemu_fdt_جمع_کن_subnode(fdt, iommu_گره);

    qemu_fdt_setprop(fdt, iommu_گره, "compatible", compat, حجم(compat));
    qemu_fdt_setprop_sized_cells(fdt, iommu_گره, "reg",
                                 1, bdf << 8, 1, 0, 1, 0,
                                 1, 0, 1, 0);
    qemu_fdt_setprop_cell(fdt, iommu_گره, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_گره, "phandle", iommu_phandle);

    qemu_fdt_setprop_cells(fdt, pci_گره, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
}

ساكن فراغ create_fdt_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ثابت حرف comp[] = "riscv,pci-iommu";
    فراغ *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree حرف *iommu_گره = عدم;
    g_autofree حرف *pci_گره = عدم;

    pci_گره = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].پایه);
    iommu_گره = g_strdup_printf("%s/iommu@%x", pci_گره, bdf);
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_جمع_کن_subnode(fdt, iommu_گره);

    qemu_fdt_setprop(fdt, iommu_گره, "compatible", comp, حجم(comp));
    qemu_fdt_setprop_cell(fdt, iommu_گره, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_گره, "phandle", iommu_phandle);
    qemu_fdt_setprop_cells(fdt, iommu_گره, "reg",
                           bdf << 8, 0, 0, 0, 0);
    qemu_fdt_setprop_cells(fdt, pci_گره, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
    s->pci_iommu_bdf = bdf;
}

ساكن فراغ finalize_fdt(RISCVVirtState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, irq_virtio_phandle = 1;
    uint32_t iommu_sys_phandle = 1;

    create_fdt_sockets(s, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &irq_virtio_phandle,
                       &msi_pcie_phandle);

    create_fdt_virtio(s, irq_virtio_phandle);

    إذا (virt_is_iommu_sys_enabled(s)) {
        iommu_sys_phandle =
            riscv_create_fdt_riscv_iommu_sys(MACHINE(s)->fdt,
                                             s->memmap[VIRT_IOMMU_SYS].پایه,
                                             s->memmap[VIRT_IOMMU_SYS].اندازه,
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
                          &virt_بالا_pcie_memmap,
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, PCIE_IRQ);

    riscv_create_fdt_syscon(MACHINE(s)->fdt, &phandle,
                            s->memmap[VIRT_TEST].پایه,
                            s->memmap[VIRT_TEST].اندازه,
                            FINISHER_RESET, FINISHER_PASS, true);

    create_fdt_uarts(s, irq_mmio_phandle);

    riscv_create_fdt_rtc(MACHINE(s)->fdt, &s->memmap[VIRT_RTC], RTC_IRQ,
                         s->aia_type, irq_mmio_phandle);
}

ساكن فراغ create_fdt(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_دانه[32];
    g_autofree حرف *name = عدم;

    ms->fdt = riscv_create_board_device_درخت("riscv-virtio,qemu",
                                             "riscv-virtio",
                                             &s->fdt_اندازه);

    /*
     * The "/soc/pci@..." node is needed for PCIE hotplugs
     * that might happen before finalize_fdt().
     */
    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                           s->memmap[VIRT_PCIE_ECAM].پایه);
    qemu_fdt_جمع_کن_subnode(ms->fdt, name);

    qemu_fdt_جمع_کن_subnode(ms->fdt, "/chosen");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_دانه, حجم(rng_دانه));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_دانه, حجم(rng_دانه));

    qemu_fdt_جمع_کن_subnode(ms->fdt, "/aliases");

    riscv_create_fdt_flash(ms->fdt, s->memmap[VIRT_FLASH].پایه,
                           s->memmap[VIRT_FLASH].اندازه / 2);
    create_fdt_fw_cfg(s);
    create_fdt_pmu(s);
}

ساكن FWCfgState *create_fw_cfg(ثابت MachineState *ms, hwaddr پایه)
{
    FWCfgState *fw_cfg;

    fw_cfg = fw_cfg_init_mem_dma(پایه, &آدرس_space_حافظه);
    fw_cfg_جمع_کن_i16(fw_cfg, FW_CFG_NB_CPUS, (uint16_t)ms->smp.cpus);

    أرجع fw_cfg;
}

ساكن DeviceState *virt_create_plic(ثابت Mem_Map_مدخل *memmap, صحيح socket,
                                     صحيح پایه_hartid, صحيح hart_شمار)
{
    g_autofree حرف *plic_hart_config = عدم;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hart_شمار);

    /* Per-socket PLIC */
    أرجع sifive_plic_create(
             get_system_حافظه(),
             memmap[VIRT_PLIC].پایه + socket * memmap[VIRT_PLIC].اندازه,
             plic_hart_config, hart_شمار, پایه_hartid,
             VIRT_IRQCHIP_رقم_SOURCES,
             ((1U << VIRT_IRQCHIP_رقم_PRIO_بیتها) - 1),
             VIRT_PLIC_اولویت_پایه, VIRT_PLIC_PENDING_پایه,
             VIRT_PLIC_ENABLE_پایه, VIRT_PLIC_ENABLE_STRIDE,
             VIRT_PLIC_CONTEXT_پایه,
             VIRT_PLIC_CONTEXT_STRIDE,
             memmap[VIRT_PLIC].اندازه);
}

ساكن فراغ virt_build_smbios(RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    uint8_t *smbios_tables, *smbios_anchor;
    اندازه_t smbios_tables_len, smbios_anchor_len;
    بنية smbios_phys_mem_area mem_array;
    ثابت حرف *حاصلضرب = "QEMU Virtual Machine";

    إذا (kvm_enabled()) {
        حاصلضرب = "KVM Virtual Machine";
    }

    smbios_مجموعه_defaults("QEMU", حاصلضرب, mc->name);

    إذا (riscv_is_32bit(&s->soc[0])) {
        smbios_مجموعه_default_processor_family(0x200);
    } وإلا {
        smbios_مجموعه_default_processor_family(0x201);
    }

    /* build the array of physical mem area from base_memmap */
    mem_array.آدرس = s->memmap[VIRT_DRAM].پایه;
    mem_array.درازی = ms->ram_اندازه;

    smbios_get_tables(ms, SMBIOS_مدخل_نقطه_TYPE_64,
                      &mem_array, 1,
                      &smbios_tables, &smbios_tables_len,
                      &smbios_anchor, &smbios_anchor_len,
                      &error_fatal);

    إذا (smbios_anchor) {
        fw_cfg_جمع_کن_file(s->fw_cfg, "etc/smbios/smbios-tables",
                        smbios_tables, smbios_tables_len);
        fw_cfg_جمع_کن_file(s->fw_cfg, "etc/smbios/smbios-anchor",
                        smbios_anchor, smbios_anchor_len);
    }
}

ساكن فراغ virt_machine_انجامشده(Notifier *notifier, فراغ *داده)
{
    RISCVVirtState *s = container_از(notifier, RISCVVirtState,
                                     machine_انجامشده);
    MachineState *machine = MACHINE(s);
    hwaddr شروع_addr = s->memmap[VIRT_DRAM].پایه;
    hwaddr firmware_انتها_addr;
    vaddr kernel_شروع_addr;
    ثابت حرف *firmware_name = riscv_default_firmware_name(&s->soc[0]);
    uint64_t fdt_بارگیری_addr;
    uint64_t kernel_مدخل = 0;
    بلوک_Backend *pflash_blk0;
    RISCVBootInfo boot_info;

    /*
     * An user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    إذا (machine->dtb == عدم) {
        finalize_fdt(s);
    }

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * so the "-bios" parameter is not supported when KVM is enabled.
     */
    إذا (kvm_enabled()) {
        إذا (machine->firmware) {
            إذا (strcmp(machine->firmware, "none")) {
                error_report("Machine mode firmware is not supported in "
                             "combination with KVM.");
                خروج(1);
            }
        } وإلا {
            machine->firmware = g_strdup("none");
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_انتها_addr = riscv_پیدا_کن_and_بارگیری_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &شروع_addr, عدم);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    إذا (pflash_blk0) {
        إذا (machine->firmware && !strcmp(machine->firmware, "none") &&
            !kvm_enabled()) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            شروع_addr = s->memmap[VIRT_FLASH].پایه;
        } وإلا {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_راهاندازی_firmware_boot(machine);
            kernel_مدخل = s->memmap[VIRT_FLASH].پایه;
        }
    }

    إذا (machine->kernel_filename && !kernel_مدخل) {
        kernel_شروع_addr = riscv_calc_kernel_شروع_addr(&boot_info,
                                                         firmware_انتها_addr);
        riscv_بارگیری_kernel(machine, &boot_info, kernel_شروع_addr,
                          true, عدم);
        kernel_مدخل = boot_info.image_کم_addr;
    }

    fdt_بارگیری_addr = riscv_compute_fdt_addr(s->memmap[VIRT_DRAM].پایه,
                                           s->memmap[VIRT_DRAM].اندازه,
                                           machine, &boot_info);
    riscv_بارگیری_fdt(fdt_بارگیری_addr, machine->fdt);

    /* load the reset vector */
    riscv_راهاندازی_rom_reset_vec(machine, &s->soc[0], شروع_addr,
                              s->memmap[VIRT_MROM].پایه,
                              s->memmap[VIRT_MROM].اندازه, kernel_مدخل,
                              fdt_بارگیری_addr);

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * So here setup kernel start address and fdt address.
     * TODO:Support firmware loading and integrate to TCG start
     */
    إذا (kvm_enabled()) {
        riscv_راهاندازی_مستقیم_kernel(kernel_مدخل, fdt_بارگیری_addr);
    }

    virt_build_smbios(s);

    إذا (virt_is_acpi_enabled(s)) {
        virt_acpi_راهاندازی(s);
    }
}

ساكن فراغ virt_machine_init(MachineState *machine)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);
    حافظه_Region *system_حافظه = get_system_حافظه();
    حافظه_Region *ماسک_rom = g_new(حافظه_Region, 1);
    DeviceState *mmio_irqchip, *virtio_irqchip, *pcie_irqchip;
    صحيح i, پایه_hartid, hart_شمار;
    صحيح socket_شمار = riscv_socket_شمار(machine);

    s->memmap = virt_memmap;

    /* Check socket count limit */
    إذا (VIRT_SOCKETS_MAX < socket_شمار) {
        error_report("number of sockets/nodes should be less than %d",
            VIRT_SOCKETS_MAX);
        خروج(1);
    }

    إذا (!virt_aclint_allowed() && s->have_aclint) {
        error_report("'aclint' is only available with TCG acceleration");
        خروج(1);
    }

    /* Initialize sockets */
    mmio_irqchip = virtio_irqchip = pcie_irqchip = عدم;
    لكل (i = 0; i < socket_شمار; i++) {
        g_autofree حرف *soc_name = g_strdup_printf("soc%d", i);

        إذا (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            خروج(1);
        }

        پایه_hartid = riscv_socket_اولی_hartid(machine, i);
        إذا (پایه_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            خروج(1);
        }

        hart_شمار = riscv_socket_hart_شمار(machine, i);
        إذا (hart_شمار < 0) {
            error_report("can't find hart count for socket%d", i);
            خروج(1);
        }

        object_initialize_فرزند(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        object_property_مجموعه_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_مجموعه_int(OBJECT(&s->soc[i]), "hartid-base",
                                پایه_hartid, &error_abort);
        object_property_مجموعه_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_شمار, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        إذا (virt_aclint_allowed() && s->have_aclint) {
            إذا (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
                /* Per-socket ACLINT MTIMER */
                riscv_aclint_mtimer_create(system_حافظه,
                        s->memmap[VIRT_CLINT].پایه +
                            i * RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                        RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                        پایه_hartid, hart_شمار,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
            } وإلا {
                /* Per-socket ACLINT MSWI, MTIMER, and SSWI */
                riscv_aclint_swi_create(system_حافظه,
                        s->memmap[VIRT_CLINT].پایه +
                            i * s->memmap[VIRT_CLINT].اندازه,
                        پایه_hartid, hart_شمار, false);
                riscv_aclint_mtimer_create(system_حافظه,
                        s->memmap[VIRT_CLINT].پایه +
                            i * s->memmap[VIRT_CLINT].اندازه +
                            RISCV_ACLINT_SWI_اندازه,
                        RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                        پایه_hartid, hart_شمار,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
                riscv_aclint_swi_create(system_حافظه,
                        s->memmap[VIRT_ACLINT_SSWI].پایه +
                            i * s->memmap[VIRT_ACLINT_SSWI].اندازه,
                        پایه_hartid, hart_شمار, true);
            }
        } وإلا إذا (tcg_enabled()) {
            /* Per-socket SiFive CLINT */
            riscv_aclint_swi_create(system_حافظه,
                    s->memmap[VIRT_CLINT].پایه + i * s->memmap[VIRT_CLINT].اندازه,
                    پایه_hartid, hart_شمار, false);
            riscv_aclint_mtimer_create(system_حافظه,
                    s->memmap[VIRT_CLINT].پایه +
                    i * s->memmap[VIRT_CLINT].اندازه + RISCV_ACLINT_SWI_اندازه,
                    RISCV_ACLINT_DEFAULT_MTIMER_اندازه, پایه_hartid, hart_شمار,
                    RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
                    RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
        }

        /* Per-socket interrupt controller */
        إذا (s->aia_type == VIRT_AIA_TYPE_NONE) {
            s->irqchip[i] = virt_create_plic(s->memmap, i,
                                             پایه_hartid, hart_شمار);
        } وإلا {
            bool msimode = s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;
            صحيح imsic_بیتها = imsic_رقم_بیتها(s->aia_guests + 1);
            s->irqchip[i] = riscv_create_aia(system_حافظه, msimode,
                                             s->aia_guests,
                                             IMSIC_HART_اندازه(0),
                                             IMSIC_HART_اندازه(imsic_بیتها),
                                             s->رقم_sources,
                                             &s->memmap[VIRT_APLIC_M],
                                             &s->memmap[VIRT_APLIC_S],
                                             &s->memmap[VIRT_IMSIC_M],
                                             &s->memmap[VIRT_IMSIC_S],
                                             i, پایه_hartid, hart_شمار,
                                             VIRT_IRQCHIP_رقم_MSIS,
                                             VIRT_IRQCHIP_رقم_PRIO_بیتها);
        }

        /* Try to use different IRQCHIP instance based device type */
        إذا (i == 0) {
            mmio_irqchip = s->irqchip[i];
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        إذا (i == 1) {
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        إذا (i == 2) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    إذا (kvm_enabled() && virt_use_kvm_aia_aplic_imsic(s->aia_type)) {
        kvm_riscv_aia_create(machine, IMSIC_MMIO_گروه_MIN_لغزش,
                             VIRT_IRQCHIP_رقم_SOURCES, VIRT_IRQCHIP_رقم_MSIS,
                             s->memmap[VIRT_APLIC_S].پایه,
                             s->memmap[VIRT_IMSIC_S].پایه,
                             s->aia_guests);
    }

    إذا (riscv_is_32bit(&s->soc[0])) {
#إذا HOST_LONG_بیتها == 64
        /* limit RAM size in a 32-bit system */
        إذا (machine->ram_اندازه > 10 * GiB) {
            machine->ram_اندازه = 10 * GiB;
            error_report("Limiting RAM size to 10 GiB");
        }
#پایان_اگر
        virt_بالا_pcie_memmap.پایه = VIRT32_بالا_PCIE_MMIO_پایه;
        virt_بالا_pcie_memmap.اندازه = VIRT32_بالا_PCIE_MMIO_اندازه;
    } وإلا {
        virt_بالا_pcie_memmap.اندازه = VIRT64_بالا_PCIE_MMIO_اندازه;
        virt_بالا_pcie_memmap.پایه = s->memmap[VIRT_DRAM].پایه +
                                     machine->ram_اندازه;
        virt_بالا_pcie_memmap.پایه =
            دور_به_بالا(virt_بالا_pcie_memmap.پایه, virt_بالا_pcie_memmap.اندازه);
    }

    /* register system main memory (actual RAM) */
    حافظه_region_جمع_کن_subregion(system_حافظه, s->memmap[VIRT_DRAM].پایه,
                                machine->ram);

    /* boot rom */
    حافظه_region_init_rom(ماسک_rom, عدم, "riscv_virt_board.mrom",
                           s->memmap[VIRT_MROM].اندازه, &error_fatal);
    حافظه_region_جمع_کن_subregion(system_حافظه, s->memmap[VIRT_MROM].پایه,
                                ماسک_rom);

    /*
     * Init fw_cfg. Must be done before riscv_load_fdt, otherwise the
     * device tree cannot be altered and we get FDT_ERR_NOSPACE.
     */
    s->fw_cfg = create_fw_cfg(machine, s->memmap[VIRT_FW_CFG].پایه);
    rom_مجموعه_fw(s->fw_cfg);

    /* SiFive Test MMIO device */
    sifive_test_create(s->memmap[VIRT_TEST].پایه);

    /* VirtIO MMIO devices */
    لكل (i = 0; i < VIRTIO_شمار; i++) {
        sysbus_create_simple("virtio-mmio",
            s->memmap[VIRT_VIRTIO].پایه + i * s->memmap[VIRT_VIRTIO].اندازه,
            qdev_get_gpio_درون(virtio_irqchip, VIRTIO_IRQ + i));
    }

    riscv_gpex_pcie_init(system_حافظه, pcie_irqchip,
                   &s->memmap[VIRT_PCIE_ECAM], &s->memmap[VIRT_PCIE_MMIO],
                   &virt_بالا_pcie_memmap, &s->memmap[VIRT_PCIE_PIO],
                   PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
        &s->memmap[VIRT_PLATFORM_BUS], VIRT_PLATFORM_BUS_IRQ,
        VIRT_PLATFORM_BUS_رقم_IRQS);

    serial_mm_init(system_حافظه, s->memmap[VIRT_UART0].پایه,
        0, qdev_get_gpio_درون(mmio_irqchip, UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    إذا (serial_hd(1)) {
        serial_mm_init(system_حافظه, s->memmap[VIRT_UART1].پایه,
            0, qdev_get_gpio_درون(mmio_irqchip, UART1_IRQ), 399193,
            serial_hd(1), DEVICE_LITTLE_ENDIAN);
        s->uart1_present = true;
    }

    sysbus_create_simple("goldfish_rtc", s->memmap[VIRT_RTC].پایه,
        qdev_get_gpio_درون(mmio_irqchip, RTC_IRQ));

    لكل (i = 0; i < ARRAY_اندازه(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    virt_flash_map(s, system_حافظه);

    /* load/create device tree */
    إذا (machine->dtb) {
        machine->fdt = بارگیری_device_درخت(machine->dtb, &s->fdt_اندازه);
        إذا (!machine->fdt) {
            error_report("load_device_tree() failed");
            خروج(1);
        }
    } وإلا {
        create_fdt(s);
    }

    إذا (virt_is_iommu_sys_enabled(s)) {
        riscv_create_iommu_sys(mmio_irqchip, s->memmap[VIRT_IOMMU_SYS].پایه,
                               IOMMU_SYS_IRQ, riscv_is_32bit(&s->soc[0]));
    }

    s->machine_انجامشده.notify = virt_machine_انجامشده;
    qemu_جمع_کن_machine_init_انجامشده_notifier(&s->machine_انجامشده);
}

ساكن فراغ virt_machine_instance_finalize(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    لكل (صحيح i = 0; i < ARRAY_اندازه(s->flash); i++) {
        إذا (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
    g_free(s->oem_id);
    g_free(s->oem_جدول_id);
}

ساكن فراغ virt_machine_instance_init(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    virt_flash_create(s);

    s->oem_id = g_strndup(ACPI_BUILD_APPNAME6, 6);
    s->oem_جدول_id = g_strndup(ACPI_BUILD_APPNAME8, 8);
    s->acpi = ON_OFF_AUTO_AUTO;
    s->iommu_sys = ON_OFF_AUTO_AUTO;
    s->رقم_sources = VIRT_IRQCHIP_رقم_SOURCES;
}

ساكن حرف *virt_get_aia_guests(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    أرجع g_strdup_printf("%d", s->aia_guests);
}

ساكن فراغ virt_مجموعه_aia_guests(Object *obj, ثابت حرف *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->aia_guests = atoi(val);
    إذا (s->aia_guests < 0 || s->aia_guests > VIRT_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          VIRT_IRQCHIP_MAX_GUESTS);
    }
}

ساكن حرف *virt_get_aia(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    ثابت حرف *val;

    بدل (s->aia_type) {
    حالة VIRT_AIA_TYPE_APLIC:
        val = "aplic";
        اكسر;
    حالة VIRT_AIA_TYPE_APLIC_IMSIC:
        val = "aplic-imsic";
        اكسر;
    افتراضي:
        val = "none";
        اكسر;
    };

    أرجع g_strdup(val);
}

ساكن فراغ virt_مجموعه_aia(Object *obj, ثابت حرف *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    إذا (!strcmp(val, "none")) {
        s->aia_type = VIRT_AIA_TYPE_NONE;
    } وإلا إذا (!strcmp(val, "aplic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC;
    } وإلا إذا (!strcmp(val, "aplic-imsic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC_IMSIC;
    } وإلا {
        error_setg(errp, "Invalid AIA interrupt controller type");
        error_append_hint(errp, "Valid values are none, aplic, and "
                          "aplic-imsic.\n");
    }
}

ساكن bool virt_get_aclint(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    أرجع s->have_aclint;
}

ساكن فراغ virt_مجموعه_aclint(Object *obj, bool مقدار, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->have_aclint = مقدار;
}

bool virt_is_iommu_sys_enabled(RISCVVirtState *s)
{
    أرجع s->iommu_sys == ON_OFF_AUTO_ON;
}

ساكن فراغ virt_get_iommu_sys(Object *obj, Visitor *v, ثابت حرف *name,
                               فراغ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto iommu_sys = s->iommu_sys;

    بازدید_type_On_Off_Auto(v, name, &iommu_sys, errp);
}

ساكن فراغ virt_مجموعه_iommu_sys(Object *obj, Visitor *v, ثابت حرف *name,
                               فراغ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    بازدید_type_On_Off_Auto(v, name, &s->iommu_sys, errp);
}

bool virt_is_acpi_enabled(RISCVVirtState *s)
{
    أرجع s->acpi != ON_OFF_AUTO_OFF;
}

ساكن فراغ virt_get_acpi(Object *obj, Visitor *v, ثابت حرف *name,
                          فراغ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto acpi = s->acpi;

    بازدید_type_On_Off_Auto(v, name, &acpi, errp);
}

ساكن فراغ virt_مجموعه_acpi(Object *obj, Visitor *v, ثابت حرف *name,
                          فراغ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    بازدید_type_On_Off_Auto(v, name, &s->acpi, errp);
}

ساكن ثابت HotplugHandler *
virt_machine_get_hotplug_handler(MachineState *machine, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);

    إذا (device_is_dynamic_sysbus(mc, dev) ||
        object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI) ||
        object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        s->iommu_sys = ON_OFF_AUTO_OFF;
        أرجع HOTPLUG_HANDLER(machine);
    }

    أرجع عدم;
}

ساكن فراغ virt_machine_device_plug_cb(ثابت HotplugHandler *hotplug_dev,
                                        DeviceState *dev, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(hotplug_dev);

    إذا (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        إذا (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }

    إذا (object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI)) {
        create_fdt_virtio_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
    }

    إذا (object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        create_fdt_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
        s->iommu_sys = ON_OFF_AUTO_OFF;
    }
}

ساكن فراغ virt_machine_class_init(ObjectClass *oc, ثابت فراغ *داده)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);

    mc->desc = "RISC-V VirtIO board";
    mc->init = virt_machine_init;
    mc->max_cpus = VIRT_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_پایه;
    mc->بلوک_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->pci_allow_0_آدرس = true;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_شاخص_مقصد_instance_props = riscv_numa_cpu_شاخص_مقصد_props;
    mc->get_default_cpu_گره_id = riscv_numa_get_default_cpu_گره_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_virt_board.ram";
    أكد(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = virt_machine_get_hotplug_handler;

    hc->plug = virt_machine_device_plug_cb;

    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_RAMFB_DEVICE);
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_UEFI_VARS_SYSBUS);
#اگر_تعریف_شده CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#پایان_اگر

    object_class_property_جمع_کن_bool(oc, "aclint", virt_get_aclint,
                                   virt_مجموعه_aclint);
    object_class_property_مجموعه_description(oc, "aclint",
                                          "(TCG only) Set on/off to "
                                          "enable/disable emulating "
                                          "ACLINT devices");

    object_class_property_جمع_کن_str(oc, "aia", virt_get_aia,
                                  virt_مجموعه_aia);
    object_class_property_مجموعه_description(oc, "aia",
                                          "Set type of AIA interrupt "
                                          "controller. Valid values are "
                                          "none, aplic, and aplic-imsic.");

    object_class_property_جمع_کن_str(oc, "aia-guests",
                                  virt_get_aia_guests,
                                  virt_مجموعه_aia_guests);
    {
        g_autofree حرف *str =
            g_strdup_printf("Set number of guest MMIO pages for AIA IMSIC. "
                            "Valid value should be between 0 and %d.",
                            VIRT_IRQCHIP_MAX_GUESTS);
        object_class_property_مجموعه_description(oc, "aia-guests", str);
    }

    object_class_property_جمع_کن(oc, "acpi", "OnOffAuto",
                              virt_get_acpi, virt_مجموعه_acpi,
                              عدم, عدم);
    object_class_property_مجموعه_description(oc, "acpi",
                                          "Enable ACPI");

    object_class_property_جمع_کن(oc, "iommu-sys", "OnOffAuto",
                              virt_get_iommu_sys, virt_مجموعه_iommu_sys,
                              عدم, عدم);
    object_class_property_مجموعه_description(oc, "iommu-sys",
                                          "Enable IOMMU platform device");
}

ساكن ثابت TypeInfo virt_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("virt"),
    .والد     = TYPE_MACHINE,
    .class_init = virt_machine_class_init,
    .instance_init = virt_machine_instance_init,
    .instance_finalize = virt_machine_instance_finalize,
    .instance_اندازه = حجم(RISCVVirtState),
    .interfaces = (ثابت InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

ساكن فراغ virt_machine_init_ثبات_types(فراغ)
{
    type_ثبات_static(&virt_machine_typeinfo);
}

type_init(virt_machine_init_ثبات_types)
