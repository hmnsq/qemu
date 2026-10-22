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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/units.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/guest-random.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/loader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/char/serial-mm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus-fdt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv_hart.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv-iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv-iommu-bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/virt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/device-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/fdt-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/numa.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kvm/kvm_riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/firmware/smbios.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aplic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/sifive_plic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/misc/sifive_test.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/platform-bus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chardev/char.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/tcg.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/kvm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/tpm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/qtest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci-host/gpex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/display/ramfb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/acpi/aml-build.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/qapi-visit-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/virtio/virtio-iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/uefi/var-service-api.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "aia.h"

/* KVM AIA only supports APLIC MSI. APLIC Wired is always emulated by QEMU. */
ਸਥਿਰ bool virt_use_kvm_aia_aplic_imsic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    ਵਾਪਸ riscv_is_kvm_aia_aplic_imsic(msimode);
}

ਸਥਿਰ bool virt_use_emulated_aplic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    ਵਾਪਸ riscv_use_emulated_aplic(msimode);
}

ਸਥਿਰ bool virt_aclint_allowed(ਖਾਲੀ)
{
    ਵਾਪਸ tcg_enabled() || qtest_enabled();
}

ਸਥਿਰ ਅਚਲ Mem_Map_ਇੰਦਰਾਜ virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_IOMMU_SYS] =    {  0x3010000,        0x1000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_ਮਾਪ(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_ਮਾਪ(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_ਮਾਪ(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    /* UART1 supports page isolation from UART0 */
    [VIRT_UART1] =        { 0x1000a000,         0x100 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_ਮਾਪ },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_ਮਾਪ },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    [VIRT_DRAM] =         { 0x80000000,           0x0 },
};

/* PCIe high mmio is fixed for RV32 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRT32_ਉੱਚਾ_PCIE_MMIO_ਅਧਾਰ  0x300000000ULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRT32_ਉੱਚਾ_PCIE_MMIO_ਮਾਪ  (4 * GiB)

/* PCIe high mmio for RV64, size is fixed but base depends on top of RAM */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRT64_ਉੱਚਾ_PCIE_MMIO_ਮਾਪ  (16 * GiB)

ਸਥਿਰ Mem_Map_ਇੰਦਰਾਜ virt_ਉੱਚਾ_pcie_memmap;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIRT_FLASH_SECTOR_ਮਾਪ (256 * KiB)

ਸਥਿਰ ਖਾਲੀ virt_flash_create(RISCVVirtState *s)
{
    s->flash[0] = riscv_flash_create(OBJECT(s), "virt.flash0", "pflash0",
                                     VIRT_FLASH_SECTOR_ਮਾਪ);
    s->flash[1] = riscv_flash_create(OBJECT(s), "virt.flash1", "pflash1",
                                     VIRT_FLASH_SECTOR_ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ virt_flash_map(RISCVVirtState *s,
                           ਸਮ੍ਰਿਤੀ_Region *sysmem)
{
    hwaddr flashsize = s->memmap[VIRT_FLASH].ਮਾਪ / 2;
    hwaddr flashbase = s->memmap[VIRT_FLASH].ਅਧਾਰ;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_ਮਾਪ);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_socket_plic(RISCVVirtState *s,
                                   ਪੂਰਨ socket,
                                   uint32_t *phandle, uint32_t *intc_phandles,
                                   uint32_t *plic_phandles)
{
    ਪੂਰਨ cpu;
    g_autofree ਅੱਖਰ *plic_name = ਨਿਰਾ;
    g_autofree uint32_t *plic_cells;
    MachineState *ms = MACHINE(s);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ plic_addr = s->memmap[VIRT_PLIC].ਅਧਾਰ +
                              (s->memmap[VIRT_PLIC].ਮਾਪ * socket);
    bool numa_enabled = riscv_numa_enabled(MACHINE(s));
    uint32_t cells_ਲੰਮਾਈ;

    plic_name = g_strdup_printf("/soc/interrupt-controller@%lx", plic_addr);

    ਜੇ (kvm_enabled()) {
        cells_ਲੰਮਾਈ = s->soc[socket].ਅੰਕ_harts * 2;
        plic_cells = g_new0(uint32_t, cells_ਲੰਮਾਈ);

        ਲਈ (cpu = 0; cpu < s->soc[socket].ਅੰਕ_harts; cpu++) {
            plic_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
            plic_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_S_EXT);
        }
   } ਨਹੀਂਤਾਂ {
        cells_ਲੰਮਾਈ = s->soc[socket].ਅੰਕ_harts * 4;
        plic_cells = g_new0(uint32_t, cells_ਲੰਮਾਈ);

        ਲਈ (cpu = 0; cpu < s->soc[socket].ਅੰਕ_harts; cpu++) {
            plic_cells[cpu * 4 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_M_EXT);
            plic_cells[cpu * 4 + 2] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 3] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_S_EXT);
        }
    }

    plic_phandles[socket] = (*phandle)++;

    riscv_create_fdt_plic(ms->fdt, plic_addr, s->memmap[VIRT_PLIC].ਮਾਪ,
                          plic_phandles[socket], FDT_PLIC_INT_CELLS,
                          FDT_PLIC_ADDR_CELLS, plic_cells,
                          cells_ਲੰਮਾਈ * ਆਕਾਰ(uint32_t),
                          VIRT_IRQCHIP_ਅੰਕ_SOURCES - 1,
                          numa_enabled, socket);

    ਜੇ (!socket) {
        platform_bus_ਜੋੜੋ_all_fdt_nodes(ms->fdt, plic_name,
                                       s->memmap[VIRT_PLATFORM_BUS].ਅਧਾਰ,
                                       s->memmap[VIRT_PLATFORM_BUS].ਮਾਪ,
                                       VIRT_PLATFORM_BUS_IRQ);
    }
}

ਸਥਿਰ ਖਾਲੀ create_fdt_pmu(RISCVVirtState *s)
{
    g_autofree ਅੱਖਰ *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_ਗੰਢ(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_sockets(RISCVVirtState *s,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
                               uint32_t *irq_virtio_phandle,
                               uint32_t *msi_pcie_phandle)
{
    ਪੂਰਨ socket, phandle_pos;
    MachineState *ms = MACHINE(s);
    uint32_t msi_m_phandle = 0, msi_s_phandle = 0;
    uint32_t xplic_phandles[MAX_NODES];
    g_autofree uint32_t *intc_phandles = ਨਿਰਾ;
    ਪੂਰਨ socket_ਗਿਣਤੀ = riscv_socket_ਗਿਣਤੀ(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_ਦੋਆਧਾਰੀ_ਅੰਕ = riscv_is_32bit(&s->soc[0]);
    APLICFdtProps aplic_props;
    ACLINTFdtProps aclint_props;

    riscv_fdt_create_cpu_socket_subnode(ms->fdt,
        kvm_enabled() ? kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    intc_phandles = g_new0(uint32_t, ms->smp.cpus);

    ਜੇ (virt_aclint_allowed() && s->have_aclint) {
        aclint_props.clint = &s->memmap[VIRT_CLINT];
        aclint_props.aclint_sswi = &s->memmap[VIRT_ACLINT_SSWI];
        aclint_props.aia_type = s->aia_type;
        aclint_props.numa_enabled = numa_enabled;
    }

    phandle_pos = ms->smp.cpus;
    ਲਈ (socket = (socket_ਗਿਣਤੀ - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[VIRT_DRAM].ਅਧਾਰ +
                         riscv_socket_mem_ਵਿੱਥ(ms, socket);
        uint64_t memsize = riscv_socket_mem_ਮਾਪ(ms, socket);

        phandle_pos -= s->soc[socket].ਅੰਕ_harts;

        riscv_create_fdt_socket_cpus(ms->fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].ਅੰਕ_harts,
                                     s->soc[socket].hartid_ਅਧਾਰ,
                                     phandle, &intc_phandles[phandle_pos],
                                     numa_enabled, is_32_ਦੋਆਧਾਰੀ_ਅੰਕ);

        riscv_create_fdt_socket_ਸਮ੍ਰਿਤੀ(ms->fdt, memaddr, memsize,
                                       socket, riscv_numa_enabled(ms));

        ਜੇ (virt_aclint_allowed() && s->have_aclint) {
            aclint_props.socket = socket;
            aclint_props.ਅੰਕ_harts = s->soc[socket].ਅੰਕ_harts;
            riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                           &intc_phandles[phandle_pos]);
        } ਨਹੀਂਤਾਂ ਜੇ (tcg_enabled()) {
            hwaddr clintaddr = s->memmap[VIRT_CLINT].ਅਧਾਰ +
                               s->memmap[VIRT_CLINT].ਮਾਪ * socket;

            riscv_create_fdt_socket_clint(ms->fdt, clintaddr,
                                          s->memmap[VIRT_CLINT].ਮਾਪ,
                                          socket, &intc_phandles[phandle_pos],
                                          s->soc[socket].ਅੰਕ_harts,
                                          numa_enabled);
        }
    }

    ਜੇ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        IMSICFdtProps props = {
            .soc = &s->soc,
            .socket_ਗਿਣਤੀ = riscv_socket_ਗਿਣਤੀ(ms),
            .smp_cpus = ms->smp.cpus,
            .imsic_m_ਅਧਾਰ = !kvm_enabled() ? s->memmap[VIRT_IMSIC_M].ਅਧਾਰ : 0,
            .imsic_s_ਅਧਾਰ = s->memmap[VIRT_IMSIC_S].ਅਧਾਰ,
            .imsic_ਸਮੂਹ_max_ਮਾਪ = VIRT_IMSIC_ਸਮੂਹ_MAX_ਮਾਪ,
            .irqchip_ਅੰਕ_msis = VIRT_IRQCHIP_ਅੰਕ_MSIS,
            .aia_guests = s->aia_guests
        };

        riscv_create_fdt_imsic(ms->fdt, &props, phandle, intc_phandles,
                               &msi_m_phandle, &msi_s_phandle);

        *msi_pcie_phandle = msi_s_phandle;
    }

    ਜੇ (s->aia_type != VIRT_AIA_TYPE_NONE) {
        aplic_props.aplic_m = !kvm_enabled() ? &s->memmap[VIRT_APLIC_M] : ਨਿਰਾ;
        aplic_props.aplic_s = &s->memmap[VIRT_APLIC_S];
        aplic_props.platform_bus = &s->memmap[VIRT_PLATFORM_BUS];
        aplic_props.platform_bus_irq = VIRT_PLATFORM_BUS_IRQ;
        aplic_props.socket = 0;
        aplic_props.ਅੰਕ_harts = ms->smp.cpus;
        aplic_props.numa_enabled = numa_enabled;
        aplic_props.irqchip_ਅੰਕ_sources = VIRT_IRQCHIP_ਅੰਕ_SOURCES;
        aplic_props.aia_type = s->aia_type;
    }

    /*
     * With KVM AIA aplic-imsic, using an irqchip without split
     * mode, we'll use only one APLIC instance.
     */
    ਜੇ (!virt_use_emulated_aplic(s->aia_type)) {
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[0], xplic_phandles);

        *irq_mmio_phandle = xplic_phandles[0];
        *irq_virtio_phandle = xplic_phandles[0];
        *irq_pcie_phandle = xplic_phandles[0];
    } ਨਹੀਂਤਾਂ {
        phandle_pos = ms->smp.cpus;
        ਲਈ (socket = (socket_ਗਿਣਤੀ - 1); socket >= 0; socket--) {
            phandle_pos -= s->soc[socket].ਅੰਕ_harts;

            ਜੇ (s->aia_type == VIRT_AIA_TYPE_NONE) {
                create_fdt_socket_plic(s, socket, phandle,
                                       &intc_phandles[phandle_pos],
                                       xplic_phandles);
            } ਨਹੀਂਤਾਂ {
                aplic_props.socket = socket;
                aplic_props.ਅੰਕ_harts = s->soc[socket].ਅੰਕ_harts;
                riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                              msi_m_phandle, msi_s_phandle,
                                              phandle,
                                              &intc_phandles[phandle_pos],
                                              xplic_phandles);
            }
        }

        ਲਈ (socket = 0; socket < socket_ਗਿਣਤੀ; socket++) {
            ਜੇ (socket == 0) {
                *irq_mmio_phandle = xplic_phandles[socket];
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            ਜੇ (socket == 1) {
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            ਜੇ (socket == 2) {
                *irq_pcie_phandle = xplic_phandles[socket];
            }
        }
    }

    riscv_socket_fdt_write_ਦੂਰੀ_ਮੈਟ੍ਰਿਕਸ(ms);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_virtio(RISCVVirtState *s, uint32_t irq_virtio_phandle)
{
    ਪੂਰਨ i;
    MachineState *ms = MACHINE(s);
    hwaddr virtio_ਅਧਾਰ = s->memmap[VIRT_VIRTIO].ਅਧਾਰ;

    ਲਈ (i = 0; i < VIRTIO_ਗਿਣਤੀ; i++) {
        g_autofree ਅੱਖਰ *name = ਨਿਰਾ;
        uint64_t ਮਾਪ = s->memmap[VIRT_VIRTIO].ਮਾਪ;
        hwaddr addr = virtio_ਅਧਾਰ + i * ਮਾਪ;

        name = g_strdup_printf("/soc/virtio_mmio@%"HWADDR_PRIx, addr);

        qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, name);
        qemu_fdt_setprop_string(ms->fdt, name, "compatible", "virtio,mmio");
        qemu_fdt_setprop_sized_cells(ms->fdt, name, "reg", 2, addr, 2, ਮਾਪ);
        qemu_fdt_setprop_cell(ms->fdt, name, "interrupt-parent",
            irq_virtio_phandle);
        ਜੇ (s->aia_type == VIRT_AIA_TYPE_NONE) {
            qemu_fdt_setprop_cell(ms->fdt, name, "interrupts",
                                  VIRTIO_IRQ + i);
        } ਨਹੀਂਤਾਂ {
            qemu_fdt_setprop_cells(ms->fdt, name, "interrupts",
                                   VIRTIO_IRQ + i, 0x4);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ create_fdt_uarts(RISCVVirtState *s, uint32_t irq_mmio_phandle)
{
    g_autofree ਅੱਖਰ *name = ਨਿਰਾ;
    MachineState *ms = MACHINE(s);

    ਜੇ (s->uart1_present) {
        riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART1],
                              UART1_IRQ, s->aia_type, irq_mmio_phandle);
    }

    riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART0], UART0_IRQ,
                          s->aia_type, irq_mmio_phandle);

    name = riscv_fdt_get_uart_nodename(s->memmap[VIRT_UART0].ਅਧਾਰ);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_fw_cfg(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    hwaddr ਅਧਾਰ = s->memmap[VIRT_FW_CFG].ਅਧਾਰ;
    hwaddr ਮਾਪ = s->memmap[VIRT_FW_CFG].ਮਾਪ;
    g_autofree ਅੱਖਰ *nodename = g_strdup_printf("/fw-cfg@%" PRIx64, ਅਧਾਰ);

    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, nodename);
    qemu_fdt_setprop_string(ms->fdt, nodename,
                            "compatible", "qemu,fw-cfg-mmio");
    qemu_fdt_setprop_sized_cells(ms->fdt, nodename, "reg",
                                 2, ਅਧਾਰ, 2, ਮਾਪ);
    qemu_fdt_setprop(ms->fdt, nodename, "dma-coherent", ਨਿਰਾ, 0);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_virtio_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ਅਚਲ ਅੱਖਰ compat[] = "virtio,pci-iommu\0pci1af4,1057";
    ਖਾਲੀ *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree ਅੱਖਰ *iommu_ਗੰਢ = ਨਿਰਾ;
    g_autofree ਅੱਖਰ *pci_ਗੰਢ = ਨਿਰਾ;

    pci_ਗੰਢ = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].ਅਧਾਰ);
    iommu_ਗੰਢ = g_strdup_printf("%s/virtio_iommu@%x,%x", pci_ਗੰਢ,
                                 PCI_ਥਾਂ(bdf), PCI_FUNC(bdf));
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, iommu_ਗੰਢ);

    qemu_fdt_setprop(fdt, iommu_ਗੰਢ, "compatible", compat, ਆਕਾਰ(compat));
    qemu_fdt_setprop_sized_cells(fdt, iommu_ਗੰਢ, "reg",
                                 1, bdf << 8, 1, 0, 1, 0,
                                 1, 0, 1, 0);
    qemu_fdt_setprop_cell(fdt, iommu_ਗੰਢ, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_ਗੰਢ, "phandle", iommu_phandle);

    qemu_fdt_setprop_cells(fdt, pci_ਗੰਢ, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ਅਚਲ ਅੱਖਰ comp[] = "riscv,pci-iommu";
    ਖਾਲੀ *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree ਅੱਖਰ *iommu_ਗੰਢ = ਨਿਰਾ;
    g_autofree ਅੱਖਰ *pci_ਗੰਢ = ਨਿਰਾ;

    pci_ਗੰਢ = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].ਅਧਾਰ);
    iommu_ਗੰਢ = g_strdup_printf("%s/iommu@%x", pci_ਗੰਢ, bdf);
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, iommu_ਗੰਢ);

    qemu_fdt_setprop(fdt, iommu_ਗੰਢ, "compatible", comp, ਆਕਾਰ(comp));
    qemu_fdt_setprop_cell(fdt, iommu_ਗੰਢ, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_ਗੰਢ, "phandle", iommu_phandle);
    qemu_fdt_setprop_cells(fdt, iommu_ਗੰਢ, "reg",
                           bdf << 8, 0, 0, 0, 0);
    qemu_fdt_setprop_cells(fdt, pci_ਗੰਢ, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
    s->pci_iommu_bdf = bdf;
}

ਸਥਿਰ ਖਾਲੀ finalize_fdt(RISCVVirtState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, irq_virtio_phandle = 1;
    uint32_t iommu_sys_phandle = 1;

    create_fdt_sockets(s, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &irq_virtio_phandle,
                       &msi_pcie_phandle);

    create_fdt_virtio(s, irq_virtio_phandle);

    ਜੇ (virt_is_iommu_sys_enabled(s)) {
        iommu_sys_phandle =
            riscv_create_fdt_riscv_iommu_sys(MACHINE(s)->fdt,
                                             s->memmap[VIRT_IOMMU_SYS].ਅਧਾਰ,
                                             s->memmap[VIRT_IOMMU_SYS].ਮਾਪ,
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
                          &virt_ਉੱਚਾ_pcie_memmap,
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, PCIE_IRQ);

    riscv_create_fdt_syscon(MACHINE(s)->fdt, &phandle,
                            s->memmap[VIRT_TEST].ਅਧਾਰ,
                            s->memmap[VIRT_TEST].ਮਾਪ,
                            FINISHER_RESET, FINISHER_PASS, true);

    create_fdt_uarts(s, irq_mmio_phandle);

    riscv_create_fdt_rtc(MACHINE(s)->fdt, &s->memmap[VIRT_RTC], RTC_IRQ,
                         s->aia_type, irq_mmio_phandle);
}

ਸਥਿਰ ਖਾਲੀ create_fdt(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_ਬੀਜ[32];
    g_autofree ਅੱਖਰ *name = ਨਿਰਾ;

    ms->fdt = riscv_create_board_device_ਰੁੱਖ("riscv-virtio,qemu",
                                             "riscv-virtio",
                                             &s->fdt_ਮਾਪ);

    /*
     * The "/soc/pci@..." node is needed for PCIE hotplugs
     * that might happen before finalize_fdt().
     */
    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                           s->memmap[VIRT_PCIE_ECAM].ਅਧਾਰ);
    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, name);

    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, "/chosen");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_ਬੀਜ, ਆਕਾਰ(rng_ਬੀਜ));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_ਬੀਜ, ਆਕਾਰ(rng_ਬੀਜ));

    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, "/aliases");

    riscv_create_fdt_flash(ms->fdt, s->memmap[VIRT_FLASH].ਅਧਾਰ,
                           s->memmap[VIRT_FLASH].ਮਾਪ / 2);
    create_fdt_fw_cfg(s);
    create_fdt_pmu(s);
}

ਸਥਿਰ FWCfgState *create_fw_cfg(ਅਚਲ MachineState *ms, hwaddr ਅਧਾਰ)
{
    FWCfgState *fw_cfg;

    fw_cfg = fw_cfg_init_mem_dma(ਅਧਾਰ, &ਪਤਾ_space_ਸਮ੍ਰਿਤੀ);
    fw_cfg_ਜੋੜੋ_i16(fw_cfg, FW_CFG_NB_CPUS, (uint16_t)ms->smp.cpus);

    ਵਾਪਸ fw_cfg;
}

ਸਥਿਰ DeviceState *virt_create_plic(ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap, ਪੂਰਨ socket,
                                     ਪੂਰਨ ਅਧਾਰ_hartid, ਪੂਰਨ hart_ਗਿਣਤੀ)
{
    g_autofree ਅੱਖਰ *plic_hart_config = ਨਿਰਾ;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hart_ਗਿਣਤੀ);

    /* Per-socket PLIC */
    ਵਾਪਸ sifive_plic_create(
             get_system_ਸਮ੍ਰਿਤੀ(),
             memmap[VIRT_PLIC].ਅਧਾਰ + socket * memmap[VIRT_PLIC].ਮਾਪ,
             plic_hart_config, hart_ਗਿਣਤੀ, ਅਧਾਰ_hartid,
             VIRT_IRQCHIP_ਅੰਕ_SOURCES,
             ((1U << VIRT_IRQCHIP_ਅੰਕ_PRIO_ਬਿੱਟ) - 1),
             VIRT_PLIC_ਤਰਜੀਹ_ਅਧਾਰ, VIRT_PLIC_PENDING_ਅਧਾਰ,
             VIRT_PLIC_ENABLE_ਅਧਾਰ, VIRT_PLIC_ENABLE_STRIDE,
             VIRT_PLIC_CONTEXT_ਅਧਾਰ,
             VIRT_PLIC_CONTEXT_STRIDE,
             memmap[VIRT_PLIC].ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ virt_build_smbios(RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    uint8_t *smbios_tables, *smbios_anchor;
    ਮਾਪ_t smbios_tables_len, smbios_anchor_len;
    ਬਣਤਰ smbios_phys_mem_area mem_array;
    ਅਚਲ ਅੱਖਰ *ਗੁਣਨਫਲ = "QEMU Virtual Machine";

    ਜੇ (kvm_enabled()) {
        ਗੁਣਨਫਲ = "KVM Virtual Machine";
    }

    smbios_ਸੈੱਟ_defaults("QEMU", ਗੁਣਨਫਲ, mc->name);

    ਜੇ (riscv_is_32bit(&s->soc[0])) {
        smbios_ਸੈੱਟ_default_processor_family(0x200);
    } ਨਹੀਂਤਾਂ {
        smbios_ਸੈੱਟ_default_processor_family(0x201);
    }

    /* build the array of physical mem area from base_memmap */
    mem_array.ਪਤਾ = s->memmap[VIRT_DRAM].ਅਧਾਰ;
    mem_array.ਲੰਮਾਈ = ms->ram_ਮਾਪ;

    smbios_get_tables(ms, SMBIOS_ਇੰਦਰਾਜ_ਬਿੰਦੂ_TYPE_64,
                      &mem_array, 1,
                      &smbios_tables, &smbios_tables_len,
                      &smbios_anchor, &smbios_anchor_len,
                      &error_fatal);

    ਜੇ (smbios_anchor) {
        fw_cfg_ਜੋੜੋ_file(s->fw_cfg, "etc/smbios/smbios-tables",
                        smbios_tables, smbios_tables_len);
        fw_cfg_ਜੋੜੋ_file(s->fw_cfg, "etc/smbios/smbios-anchor",
                        smbios_anchor, smbios_anchor_len);
    }
}

ਸਥਿਰ ਖਾਲੀ virt_machine_ਪੂਰਾ(Notifier *notifier, ਖਾਲੀ *ਅੰਕੜੇ)
{
    RISCVVirtState *s = container_ਦਾ(notifier, RISCVVirtState,
                                     machine_ਪੂਰਾ);
    MachineState *machine = MACHINE(s);
    hwaddr ਸ਼ੁਰੂ_addr = s->memmap[VIRT_DRAM].ਅਧਾਰ;
    hwaddr firmware_ਸਿਰਾ_addr;
    vaddr kernel_ਸ਼ੁਰੂ_addr;
    ਅਚਲ ਅੱਖਰ *firmware_name = riscv_default_firmware_name(&s->soc[0]);
    uint64_t fdt_ਲੱਦੋ_addr;
    uint64_t kernel_ਇੰਦਰਾਜ = 0;
    ਖੰਡ_Backend *pflash_blk0;
    RISCVBootInfo boot_info;

    /*
     * An user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    ਜੇ (machine->dtb == ਨਿਰਾ) {
        finalize_fdt(s);
    }

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * so the "-bios" parameter is not supported when KVM is enabled.
     */
    ਜੇ (kvm_enabled()) {
        ਜੇ (machine->firmware) {
            ਜੇ (strcmp(machine->firmware, "none")) {
                error_report("Machine mode firmware is not supported in "
                             "combination with KVM.");
                ਬਾਹਰ_ਨਿਕਲੋ(1);
            }
        } ਨਹੀਂਤਾਂ {
            machine->firmware = g_strdup("none");
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_ਸਿਰਾ_addr = riscv_ਲੱਭੋ_and_ਲੱਦੋ_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ਸ਼ੁਰੂ_addr, ਨਿਰਾ);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    ਜੇ (pflash_blk0) {
        ਜੇ (machine->firmware && !strcmp(machine->firmware, "none") &&
            !kvm_enabled()) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            ਸ਼ੁਰੂ_addr = s->memmap[VIRT_FLASH].ਅਧਾਰ;
        } ਨਹੀਂਤਾਂ {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_ਤਿਆਰੀ_firmware_boot(machine);
            kernel_ਇੰਦਰਾਜ = s->memmap[VIRT_FLASH].ਅਧਾਰ;
        }
    }

    ਜੇ (machine->kernel_filename && !kernel_ਇੰਦਰਾਜ) {
        kernel_ਸ਼ੁਰੂ_addr = riscv_calc_kernel_ਸ਼ੁਰੂ_addr(&boot_info,
                                                         firmware_ਸਿਰਾ_addr);
        riscv_ਲੱਦੋ_kernel(machine, &boot_info, kernel_ਸ਼ੁਰੂ_addr,
                          true, ਨਿਰਾ);
        kernel_ਇੰਦਰਾਜ = boot_info.image_ਨੀਵਾਂ_addr;
    }

    fdt_ਲੱਦੋ_addr = riscv_compute_fdt_addr(s->memmap[VIRT_DRAM].ਅਧਾਰ,
                                           s->memmap[VIRT_DRAM].ਮਾਪ,
                                           machine, &boot_info);
    riscv_ਲੱਦੋ_fdt(fdt_ਲੱਦੋ_addr, machine->fdt);

    /* load the reset vector */
    riscv_ਤਿਆਰੀ_rom_reset_vec(machine, &s->soc[0], ਸ਼ੁਰੂ_addr,
                              s->memmap[VIRT_MROM].ਅਧਾਰ,
                              s->memmap[VIRT_MROM].ਮਾਪ, kernel_ਇੰਦਰਾਜ,
                              fdt_ਲੱਦੋ_addr);

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * So here setup kernel start address and fdt address.
     * TODO:Support firmware loading and integrate to TCG start
     */
    ਜੇ (kvm_enabled()) {
        riscv_ਤਿਆਰੀ_ਸਿੱਧਾ_kernel(kernel_ਇੰਦਰਾਜ, fdt_ਲੱਦੋ_addr);
    }

    virt_build_smbios(s);

    ਜੇ (virt_is_acpi_enabled(s)) {
        virt_acpi_ਤਿਆਰੀ(s);
    }
}

ਸਥਿਰ ਖਾਲੀ virt_machine_init(MachineState *machine)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);
    ਸਮ੍ਰਿਤੀ_Region *system_ਸਮ੍ਰਿਤੀ = get_system_ਸਮ੍ਰਿਤੀ();
    ਸਮ੍ਰਿਤੀ_Region *ਨਕਾਬ_rom = g_new(ਸਮ੍ਰਿਤੀ_Region, 1);
    DeviceState *mmio_irqchip, *virtio_irqchip, *pcie_irqchip;
    ਪੂਰਨ i, ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ;
    ਪੂਰਨ socket_ਗਿਣਤੀ = riscv_socket_ਗਿਣਤੀ(machine);

    s->memmap = virt_memmap;

    /* Check socket count limit */
    ਜੇ (VIRT_SOCKETS_MAX < socket_ਗਿਣਤੀ) {
        error_report("number of sockets/nodes should be less than %d",
            VIRT_SOCKETS_MAX);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਜੇ (!virt_aclint_allowed() && s->have_aclint) {
        error_report("'aclint' is only available with TCG acceleration");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    /* Initialize sockets */
    mmio_irqchip = virtio_irqchip = pcie_irqchip = ਨਿਰਾ;
    ਲਈ (i = 0; i < socket_ਗਿਣਤੀ; i++) {
        g_autofree ਅੱਖਰ *soc_name = g_strdup_printf("soc%d", i);

        ਜੇ (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }

        ਅਧਾਰ_hartid = riscv_socket_ਪਹਿਲਾ_hartid(machine, i);
        ਜੇ (ਅਧਾਰ_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }

        hart_ਗਿਣਤੀ = riscv_socket_hart_ਗਿਣਤੀ(machine, i);
        ਜੇ (hart_ਗਿਣਤੀ < 0) {
            error_report("can't find hart count for socket%d", i);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }

        object_initialize_ਬੱਚਾ(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        object_property_ਸੈੱਟ_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_ਸੈੱਟ_int(OBJECT(&s->soc[i]), "hartid-base",
                                ਅਧਾਰ_hartid, &error_abort);
        object_property_ਸੈੱਟ_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_ਗਿਣਤੀ, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        ਜੇ (virt_aclint_allowed() && s->have_aclint) {
            ਜੇ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
                /* Per-socket ACLINT MTIMER */
                riscv_aclint_mtimer_create(system_ਸਮ੍ਰਿਤੀ,
                        s->memmap[VIRT_CLINT].ਅਧਾਰ +
                            i * RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ,
                        RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ,
                        ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
            } ਨਹੀਂਤਾਂ {
                /* Per-socket ACLINT MSWI, MTIMER, and SSWI */
                riscv_aclint_swi_create(system_ਸਮ੍ਰਿਤੀ,
                        s->memmap[VIRT_CLINT].ਅਧਾਰ +
                            i * s->memmap[VIRT_CLINT].ਮਾਪ,
                        ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ, false);
                riscv_aclint_mtimer_create(system_ਸਮ੍ਰਿਤੀ,
                        s->memmap[VIRT_CLINT].ਅਧਾਰ +
                            i * s->memmap[VIRT_CLINT].ਮਾਪ +
                            RISCV_ACLINT_SWI_ਮਾਪ,
                        RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ,
                        ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
                riscv_aclint_swi_create(system_ਸਮ੍ਰਿਤੀ,
                        s->memmap[VIRT_ACLINT_SSWI].ਅਧਾਰ +
                            i * s->memmap[VIRT_ACLINT_SSWI].ਮਾਪ,
                        ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ, true);
            }
        } ਨਹੀਂਤਾਂ ਜੇ (tcg_enabled()) {
            /* Per-socket SiFive CLINT */
            riscv_aclint_swi_create(system_ਸਮ੍ਰਿਤੀ,
                    s->memmap[VIRT_CLINT].ਅਧਾਰ + i * s->memmap[VIRT_CLINT].ਮਾਪ,
                    ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ, false);
            riscv_aclint_mtimer_create(system_ਸਮ੍ਰਿਤੀ,
                    s->memmap[VIRT_CLINT].ਅਧਾਰ +
                    i * s->memmap[VIRT_CLINT].ਮਾਪ + RISCV_ACLINT_SWI_ਮਾਪ,
                    RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ, ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ,
                    RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
                    RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
        }

        /* Per-socket interrupt controller */
        ਜੇ (s->aia_type == VIRT_AIA_TYPE_NONE) {
            s->irqchip[i] = virt_create_plic(s->memmap, i,
                                             ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ);
        } ਨਹੀਂਤਾਂ {
            bool msimode = s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;
            ਪੂਰਨ imsic_ਬਿੱਟ = imsic_ਅੰਕ_ਬਿੱਟ(s->aia_guests + 1);
            s->irqchip[i] = riscv_create_aia(system_ਸਮ੍ਰਿਤੀ, msimode,
                                             s->aia_guests,
                                             IMSIC_HART_ਮਾਪ(0),
                                             IMSIC_HART_ਮਾਪ(imsic_ਬਿੱਟ),
                                             s->ਅੰਕ_sources,
                                             &s->memmap[VIRT_APLIC_M],
                                             &s->memmap[VIRT_APLIC_S],
                                             &s->memmap[VIRT_IMSIC_M],
                                             &s->memmap[VIRT_IMSIC_S],
                                             i, ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ,
                                             VIRT_IRQCHIP_ਅੰਕ_MSIS,
                                             VIRT_IRQCHIP_ਅੰਕ_PRIO_ਬਿੱਟ);
        }

        /* Try to use different IRQCHIP instance based device type */
        ਜੇ (i == 0) {
            mmio_irqchip = s->irqchip[i];
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ਜੇ (i == 1) {
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ਜੇ (i == 2) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    ਜੇ (kvm_enabled() && virt_use_kvm_aia_aplic_imsic(s->aia_type)) {
        kvm_riscv_aia_create(machine, IMSIC_MMIO_ਸਮੂਹ_MIN_ਖਿਸਕਾਓ,
                             VIRT_IRQCHIP_ਅੰਕ_SOURCES, VIRT_IRQCHIP_ਅੰਕ_MSIS,
                             s->memmap[VIRT_APLIC_S].ਅਧਾਰ,
                             s->memmap[VIRT_IMSIC_S].ਅਧਾਰ,
                             s->aia_guests);
    }

    ਜੇ (riscv_is_32bit(&s->soc[0])) {
#ਜੇ HOST_LONG_ਬਿੱਟ == 64
        /* limit RAM size in a 32-bit system */
        ਜੇ (machine->ram_ਮਾਪ > 10 * GiB) {
            machine->ram_ਮਾਪ = 10 * GiB;
            error_report("Limiting RAM size to 10 GiB");
        }
#ਜੇ_ਅੰਤ
        virt_ਉੱਚਾ_pcie_memmap.ਅਧਾਰ = VIRT32_ਉੱਚਾ_PCIE_MMIO_ਅਧਾਰ;
        virt_ਉੱਚਾ_pcie_memmap.ਮਾਪ = VIRT32_ਉੱਚਾ_PCIE_MMIO_ਮਾਪ;
    } ਨਹੀਂਤਾਂ {
        virt_ਉੱਚਾ_pcie_memmap.ਮਾਪ = VIRT64_ਉੱਚਾ_PCIE_MMIO_ਮਾਪ;
        virt_ਉੱਚਾ_pcie_memmap.ਅਧਾਰ = s->memmap[VIRT_DRAM].ਅਧਾਰ +
                                     machine->ram_ਮਾਪ;
        virt_ਉੱਚਾ_pcie_memmap.ਅਧਾਰ =
            ਗੇੜਾ_ਉੱਪਰ(virt_ਉੱਚਾ_pcie_memmap.ਅਧਾਰ, virt_ਉੱਚਾ_pcie_memmap.ਮਾਪ);
    }

    /* register system main memory (actual RAM) */
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ, s->memmap[VIRT_DRAM].ਅਧਾਰ,
                                machine->ram);

    /* boot rom */
    ਸਮ੍ਰਿਤੀ_region_init_rom(ਨਕਾਬ_rom, ਨਿਰਾ, "riscv_virt_board.mrom",
                           s->memmap[VIRT_MROM].ਮਾਪ, &error_fatal);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ, s->memmap[VIRT_MROM].ਅਧਾਰ,
                                ਨਕਾਬ_rom);

    /*
     * Init fw_cfg. Must be done before riscv_load_fdt, otherwise the
     * device tree cannot be altered and we get FDT_ERR_NOSPACE.
     */
    s->fw_cfg = create_fw_cfg(machine, s->memmap[VIRT_FW_CFG].ਅਧਾਰ);
    rom_ਸੈੱਟ_fw(s->fw_cfg);

    /* SiFive Test MMIO device */
    sifive_test_create(s->memmap[VIRT_TEST].ਅਧਾਰ);

    /* VirtIO MMIO devices */
    ਲਈ (i = 0; i < VIRTIO_ਗਿਣਤੀ; i++) {
        sysbus_create_simple("virtio-mmio",
            s->memmap[VIRT_VIRTIO].ਅਧਾਰ + i * s->memmap[VIRT_VIRTIO].ਮਾਪ,
            qdev_get_gpio_ਵਿੱਚ(virtio_irqchip, VIRTIO_IRQ + i));
    }

    riscv_gpex_pcie_init(system_ਸਮ੍ਰਿਤੀ, pcie_irqchip,
                   &s->memmap[VIRT_PCIE_ECAM], &s->memmap[VIRT_PCIE_MMIO],
                   &virt_ਉੱਚਾ_pcie_memmap, &s->memmap[VIRT_PCIE_PIO],
                   PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
        &s->memmap[VIRT_PLATFORM_BUS], VIRT_PLATFORM_BUS_IRQ,
        VIRT_PLATFORM_BUS_ਅੰਕ_IRQS);

    serial_mm_init(system_ਸਮ੍ਰਿਤੀ, s->memmap[VIRT_UART0].ਅਧਾਰ,
        0, qdev_get_gpio_ਵਿੱਚ(mmio_irqchip, UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    ਜੇ (serial_hd(1)) {
        serial_mm_init(system_ਸਮ੍ਰਿਤੀ, s->memmap[VIRT_UART1].ਅਧਾਰ,
            0, qdev_get_gpio_ਵਿੱਚ(mmio_irqchip, UART1_IRQ), 399193,
            serial_hd(1), DEVICE_LITTLE_ENDIAN);
        s->uart1_present = true;
    }

    sysbus_create_simple("goldfish_rtc", s->memmap[VIRT_RTC].ਅਧਾਰ,
        qdev_get_gpio_ਵਿੱਚ(mmio_irqchip, RTC_IRQ));

    ਲਈ (i = 0; i < ARRAY_ਮਾਪ(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    virt_flash_map(s, system_ਸਮ੍ਰਿਤੀ);

    /* load/create device tree */
    ਜੇ (machine->dtb) {
        machine->fdt = ਲੱਦੋ_device_ਰੁੱਖ(machine->dtb, &s->fdt_ਮਾਪ);
        ਜੇ (!machine->fdt) {
            error_report("load_device_tree() failed");
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
    } ਨਹੀਂਤਾਂ {
        create_fdt(s);
    }

    ਜੇ (virt_is_iommu_sys_enabled(s)) {
        riscv_create_iommu_sys(mmio_irqchip, s->memmap[VIRT_IOMMU_SYS].ਅਧਾਰ,
                               IOMMU_SYS_IRQ, riscv_is_32bit(&s->soc[0]));
    }

    s->machine_ਪੂਰਾ.notify = virt_machine_ਪੂਰਾ;
    qemu_ਜੋੜੋ_machine_init_ਪੂਰਾ_notifier(&s->machine_ਪੂਰਾ);
}

ਸਥਿਰ ਖਾਲੀ virt_machine_instance_finalize(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ਲਈ (ਪੂਰਨ i = 0; i < ARRAY_ਮਾਪ(s->flash); i++) {
        ਜੇ (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
    g_free(s->oem_id);
    g_free(s->oem_ਸਾਰਣੀ_id);
}

ਸਥਿਰ ਖਾਲੀ virt_machine_instance_init(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    virt_flash_create(s);

    s->oem_id = g_strndup(ACPI_BUILD_APPNAME6, 6);
    s->oem_ਸਾਰਣੀ_id = g_strndup(ACPI_BUILD_APPNAME8, 8);
    s->acpi = ON_OFF_AUTO_AUTO;
    s->iommu_sys = ON_OFF_AUTO_AUTO;
    s->ਅੰਕ_sources = VIRT_IRQCHIP_ਅੰਕ_SOURCES;
}

ਸਥਿਰ ਅੱਖਰ *virt_get_aia_guests(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ਵਾਪਸ g_strdup_printf("%d", s->aia_guests);
}

ਸਥਿਰ ਖਾਲੀ virt_ਸੈੱਟ_aia_guests(Object *obj, ਅਚਲ ਅੱਖਰ *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->aia_guests = atoi(val);
    ਜੇ (s->aia_guests < 0 || s->aia_guests > VIRT_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          VIRT_IRQCHIP_MAX_GUESTS);
    }
}

ਸਥਿਰ ਅੱਖਰ *virt_get_aia(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    ਅਚਲ ਅੱਖਰ *val;

    ਚੋਣ (s->aia_type) {
    ਹਾਲਤ VIRT_AIA_TYPE_APLIC:
        val = "aplic";
        ਰੋਕੋ;
    ਹਾਲਤ VIRT_AIA_TYPE_APLIC_IMSIC:
        val = "aplic-imsic";
        ਰੋਕੋ;
    ਮੂਲ:
        val = "none";
        ਰੋਕੋ;
    };

    ਵਾਪਸ g_strdup(val);
}

ਸਥਿਰ ਖਾਲੀ virt_ਸੈੱਟ_aia(Object *obj, ਅਚਲ ਅੱਖਰ *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ਜੇ (!strcmp(val, "none")) {
        s->aia_type = VIRT_AIA_TYPE_NONE;
    } ਨਹੀਂਤਾਂ ਜੇ (!strcmp(val, "aplic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC;
    } ਨਹੀਂਤਾਂ ਜੇ (!strcmp(val, "aplic-imsic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC_IMSIC;
    } ਨਹੀਂਤਾਂ {
        error_setg(errp, "Invalid AIA interrupt controller type");
        error_append_hint(errp, "Valid values are none, aplic, and "
                          "aplic-imsic.\n");
    }
}

ਸਥਿਰ bool virt_get_aclint(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ਵਾਪਸ s->have_aclint;
}

ਸਥਿਰ ਖਾਲੀ virt_ਸੈੱਟ_aclint(Object *obj, bool ਮੁੱਲ, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->have_aclint = ਮੁੱਲ;
}

bool virt_is_iommu_sys_enabled(RISCVVirtState *s)
{
    ਵਾਪਸ s->iommu_sys == ON_OFF_AUTO_ON;
}

ਸਥਿਰ ਖਾਲੀ virt_get_iommu_sys(Object *obj, Visitor *v, ਅਚਲ ਅੱਖਰ *name,
                               ਖਾਲੀ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto iommu_sys = s->iommu_sys;

    ਦੇਖੋ_type_On_Off_Auto(v, name, &iommu_sys, errp);
}

ਸਥਿਰ ਖਾਲੀ virt_ਸੈੱਟ_iommu_sys(Object *obj, Visitor *v, ਅਚਲ ਅੱਖਰ *name,
                               ਖਾਲੀ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ਦੇਖੋ_type_On_Off_Auto(v, name, &s->iommu_sys, errp);
}

bool virt_is_acpi_enabled(RISCVVirtState *s)
{
    ਵਾਪਸ s->acpi != ON_OFF_AUTO_OFF;
}

ਸਥਿਰ ਖਾਲੀ virt_get_acpi(Object *obj, Visitor *v, ਅਚਲ ਅੱਖਰ *name,
                          ਖਾਲੀ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto acpi = s->acpi;

    ਦੇਖੋ_type_On_Off_Auto(v, name, &acpi, errp);
}

ਸਥਿਰ ਖਾਲੀ virt_ਸੈੱਟ_acpi(Object *obj, Visitor *v, ਅਚਲ ਅੱਖਰ *name,
                          ਖਾਲੀ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ਦੇਖੋ_type_On_Off_Auto(v, name, &s->acpi, errp);
}

ਸਥਿਰ ਅਚਲ HotplugHandler *
virt_machine_get_hotplug_handler(MachineState *machine, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);

    ਜੇ (device_is_dynamic_sysbus(mc, dev) ||
        object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI) ||
        object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        s->iommu_sys = ON_OFF_AUTO_OFF;
        ਵਾਪਸ HOTPLUG_HANDLER(machine);
    }

    ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਖਾਲੀ virt_machine_device_plug_cb(ਅਚਲ HotplugHandler *hotplug_dev,
                                        DeviceState *dev, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(hotplug_dev);

    ਜੇ (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        ਜੇ (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }

    ਜੇ (object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI)) {
        create_fdt_virtio_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
    }

    ਜੇ (object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        create_fdt_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
        s->iommu_sys = ON_OFF_AUTO_OFF;
    }
}

ਸਥਿਰ ਖਾਲੀ virt_machine_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);

    mc->desc = "RISC-V VirtIO board";
    mc->init = virt_machine_init;
    mc->max_cpus = VIRT_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_ਅਧਾਰ;
    mc->ਖੰਡ_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->pci_allow_0_ਪਤਾ = true;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_instance_props = riscv_numa_cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_props;
    mc->get_default_cpu_ਗੰਢ_id = riscv_numa_get_default_cpu_ਗੰਢ_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_virt_board.ram";
    ਪ੍ਰਤਿਜ੍ਞਾ(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = virt_machine_get_hotplug_handler;

    hc->plug = virt_machine_device_plug_cb;

    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_RAMFB_DEVICE);
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_UEFI_VARS_SYSBUS);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#ਜੇ_ਅੰਤ

    object_class_property_ਜੋੜੋ_bool(oc, "aclint", virt_get_aclint,
                                   virt_ਸੈੱਟ_aclint);
    object_class_property_ਸੈੱਟ_description(oc, "aclint",
                                          "(TCG only) Set on/off to "
                                          "enable/disable emulating "
                                          "ACLINT devices");

    object_class_property_ਜੋੜੋ_str(oc, "aia", virt_get_aia,
                                  virt_ਸੈੱਟ_aia);
    object_class_property_ਸੈੱਟ_description(oc, "aia",
                                          "Set type of AIA interrupt "
                                          "controller. Valid values are "
                                          "none, aplic, and aplic-imsic.");

    object_class_property_ਜੋੜੋ_str(oc, "aia-guests",
                                  virt_get_aia_guests,
                                  virt_ਸੈੱਟ_aia_guests);
    {
        g_autofree ਅੱਖਰ *str =
            g_strdup_printf("Set number of guest MMIO pages for AIA IMSIC. "
                            "Valid value should be between 0 and %d.",
                            VIRT_IRQCHIP_MAX_GUESTS);
        object_class_property_ਸੈੱਟ_description(oc, "aia-guests", str);
    }

    object_class_property_ਜੋੜੋ(oc, "acpi", "OnOffAuto",
                              virt_get_acpi, virt_ਸੈੱਟ_acpi,
                              ਨਿਰਾ, ਨਿਰਾ);
    object_class_property_ਸੈੱਟ_description(oc, "acpi",
                                          "Enable ACPI");

    object_class_property_ਜੋੜੋ(oc, "iommu-sys", "OnOffAuto",
                              virt_get_iommu_sys, virt_ਸੈੱਟ_iommu_sys,
                              ਨਿਰਾ, ਨਿਰਾ);
    object_class_property_ਸੈੱਟ_description(oc, "iommu-sys",
                                          "Enable IOMMU platform device");
}

ਸਥਿਰ ਅਚਲ TypeInfo virt_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("virt"),
    .ਮਾਪਾ     = TYPE_MACHINE,
    .class_init = virt_machine_class_init,
    .instance_init = virt_machine_instance_init,
    .instance_finalize = virt_machine_instance_finalize,
    .instance_ਮਾਪ = ਆਕਾਰ(RISCVVirtState),
    .interfaces = (ਅਚਲ InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

ਸਥਿਰ ਖਾਲੀ virt_machine_init_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&virt_machine_typeinfo);
}

type_init(virt_machine_init_ਰਜਿਸਟਰ_types)
