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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qemu/guest-random.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/char/serial-mm.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus-fdt.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/iommu.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv-iommu.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv-iommu-bits.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/virt.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/device-common.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/fdt-common.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/numa.h"
#ಅನ್ತರ್ಭಾವ "kvm/kvm_riscv.h"
#ಅನ್ತರ್ಭಾವ "hw/firmware/smbios.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aplic.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/sifive_plic.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/sifive_test.h"
#ಅನ್ತರ್ಭಾವ "hw/core/platform-bus.h"
#ಅನ್ತರ್ಭಾವ "chardev/char.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "system/tcg.h"
#ಅನ್ತರ್ಭಾವ "system/kvm.h"
#ಅನ್ತರ್ಭಾವ "system/tpm.h"
#ಅನ್ತರ್ಭಾವ "system/qtest.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pci.h"
#ಅನ್ತರ್ಭಾವ "hw/pci-host/gpex.h"
#ಅನ್ತರ್ಭಾವ "hw/display/ramfb.h"
#ಅನ್ತರ್ಭಾವ "hw/acpi/aml-build.h"
#ಅನ್ತರ್ಭಾವ "qapi/qapi-visit-common.h"
#ಅನ್ತರ್ಭಾವ "hw/virtio/virtio-iommu.h"
#ಅನ್ತರ್ಭಾವ "hw/uefi/var-service-api.h"

#ಅನ್ತರ್ಭಾವ "aia.h"

/* KVM AIA only supports APLIC MSI. APLIC Wired is always emulated by QEMU. */
ಸ್ಥಿರ bool virt_use_kvm_aia_aplic_imsic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    ಹಿಂತಿರುಗಿ riscv_is_kvm_aia_aplic_imsic(msimode);
}

ಸ್ಥಿರ bool virt_use_emulated_aplic(RISCVVirtAIAType aia_type)
{
    bool msimode = aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;

    ಹಿಂತಿರುಗಿ riscv_use_emulated_aplic(msimode);
}

ಸ್ಥಿರ bool virt_aclint_allowed(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ tcg_enabled() || qtest_enabled();
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_IOMMU_SYS] =    {  0x3010000,        0x1000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_ಗಾತ್ರ(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_ಗಾತ್ರ(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_ಗಾತ್ರ(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    /* UART1 supports page isolation from UART0 */
    [VIRT_UART1] =        { 0x1000a000,         0x100 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_ಗಾತ್ರ },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_ಗಾತ್ರ },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    [VIRT_DRAM] =         { 0x80000000,           0x0 },
};

/* PCIe high mmio is fixed for RV32 */
#ವ್ಯಾಖ್ಯಾನಿಸು VIRT32_ಮೇಲಿನ_PCIE_MMIO_ತಳ  0x300000000ULL
#ವ್ಯಾಖ್ಯಾನಿಸು VIRT32_ಮೇಲಿನ_PCIE_MMIO_ಗಾತ್ರ  (4 * GiB)

/* PCIe high mmio for RV64, size is fixed but base depends on top of RAM */
#ವ್ಯಾಖ್ಯಾನಿಸು VIRT64_ಮೇಲಿನ_PCIE_MMIO_ಗಾತ್ರ  (16 * GiB)

ಸ್ಥಿರ Mem_Map_ನಮೂದು virt_ಮೇಲಿನ_pcie_memmap;

#ವ್ಯಾಖ್ಯಾನಿಸು VIRT_FLASH_SECTOR_ಗಾತ್ರ (256 * KiB)

ಸ್ಥಿರ ಶೂನ್ಯ virt_flash_create(RISCVVirtState *s)
{
    s->flash[0] = riscv_flash_create(OBJECT(s), "virt.flash0", "pflash0",
                                     VIRT_FLASH_SECTOR_ಗಾತ್ರ);
    s->flash[1] = riscv_flash_create(OBJECT(s), "virt.flash1", "pflash1",
                                     VIRT_FLASH_SECTOR_ಗಾತ್ರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_flash_map(RISCVVirtState *s,
                           ಸ್ಮರಣೆ_Region *sysmem)
{
    hwaddr flashsize = s->memmap[VIRT_FLASH].ಗಾತ್ರ_2 / 2;
    hwaddr flashbase = s->memmap[VIRT_FLASH].ತಳ;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_ಗಾತ್ರ);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, VIRT_FLASH_SECTOR_ಗಾತ್ರ);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_socket_plic(RISCVVirtState *s,
                                   ಪೂರ್ಣಾಂಕ socket,
                                   uint32_t *phandle, uint32_t *intc_phandles,
                                   uint32_t *plic_phandles)
{
    ಪೂರ್ಣಾಂಕ cpu;
    g_autofree ಅಕ್ಷರ *plic_name = ಖಾಲಿ;
    g_autofree uint32_t *plic_cells;
    MachineState *ms = MACHINE(s);
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ plic_addr = s->memmap[VIRT_PLIC].ತಳ +
                              (s->memmap[VIRT_PLIC].ಗಾತ್ರ_2 * socket);
    bool numa_enabled = riscv_numa_enabled(MACHINE(s));
    uint32_t cells_ಉದ್ದಳತೆ;

    plic_name = g_strdup_printf("/soc/interrupt-controller@%lx", plic_addr);

    ಆದರೆ (kvm_enabled()) {
        cells_ಉದ್ದಳತೆ = s->soc[socket].ಅಂಕೆ_harts * 2;
        plic_cells = g_new0(uint32_t, cells_ಉದ್ದಳತೆ);

        ಗಾಗಿ (cpu = 0; cpu < s->soc[socket].ಅಂಕೆ_harts; cpu++) {
            plic_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
            plic_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(IRQ_S_EXT);
        }
   } ಇಲ್ಲದಿದ್ದರೆ {
        cells_ಉದ್ದಳತೆ = s->soc[socket].ಅಂಕೆ_harts * 4;
        plic_cells = g_new0(uint32_t, cells_ಉದ್ದಳತೆ);

        ಗಾಗಿ (cpu = 0; cpu < s->soc[socket].ಅಂಕೆ_harts; cpu++) {
            plic_cells[cpu * 4 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 1] = cpu_ಗಮ್ಯ_be32(IRQ_M_EXT);
            plic_cells[cpu * 4 + 2] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
            plic_cells[cpu * 4 + 3] = cpu_ಗಮ್ಯ_be32(IRQ_S_EXT);
        }
    }

    plic_phandles[socket] = (*phandle)++;

    riscv_create_fdt_plic(ms->fdt, plic_addr, s->memmap[VIRT_PLIC].ಗಾತ್ರ_2,
                          plic_phandles[socket], FDT_PLIC_INT_CELLS,
                          FDT_PLIC_ADDR_CELLS, plic_cells,
                          cells_ಉದ್ದಳತೆ * ಗಾತ್ರ(uint32_t),
                          VIRT_IRQCHIP_ಅಂಕೆ_SOURCES - 1,
                          numa_enabled, socket);

    ಆದರೆ (!socket) {
        platform_bus_ಕೂಡಿಸು_all_fdt_nodes(ms->fdt, plic_name,
                                       s->memmap[VIRT_PLATFORM_BUS].ತಳ,
                                       s->memmap[VIRT_PLATFORM_BUS].ಗಾತ್ರ_2,
                                       VIRT_PLATFORM_BUS_IRQ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_pmu(RISCVVirtState *s)
{
    g_autofree ಅಕ್ಷರ *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_ಗಂಟು(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_sockets(RISCVVirtState *s,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
                               uint32_t *irq_virtio_phandle,
                               uint32_t *msi_pcie_phandle)
{
    ಪೂರ್ಣಾಂಕ socket, phandle_pos;
    MachineState *ms = MACHINE(s);
    uint32_t msi_m_phandle = 0, msi_s_phandle = 0;
    uint32_t xplic_phandles[MAX_NODES];
    g_autofree uint32_t *intc_phandles = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ socket_ಎಣಿಕೆ = riscv_socket_ಎಣಿಕೆ(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_ಬಿಟ್ = riscv_is_32bit(&s->soc[0]);
    APLICFdtProps aplic_props;
    ACLINTFdtProps aclint_props;

    riscv_fdt_create_cpu_socket_subnode(ms->fdt,
        kvm_enabled() ? kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    intc_phandles = g_new0(uint32_t, ms->smp.cpus);

    ಆದರೆ (virt_aclint_allowed() && s->have_aclint) {
        aclint_props.clint = &s->memmap[VIRT_CLINT];
        aclint_props.aclint_sswi = &s->memmap[VIRT_ACLINT_SSWI];
        aclint_props.aia_type = s->aia_type;
        aclint_props.numa_enabled = numa_enabled;
    }

    phandle_pos = ms->smp.cpus;
    ಗಾಗಿ (socket = (socket_ಎಣಿಕೆ - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[VIRT_DRAM].ತಳ +
                         riscv_socket_mem_ಸ್ಥಾನಾಂತರ(ms, socket);
        uint64_t memsize = riscv_socket_mem_ಗಾತ್ರ(ms, socket);

        phandle_pos -= s->soc[socket].ಅಂಕೆ_harts;

        riscv_create_fdt_socket_cpus(ms->fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].ಅಂಕೆ_harts,
                                     s->soc[socket].hartid_ತಳ,
                                     phandle, &intc_phandles[phandle_pos],
                                     numa_enabled, is_32_ಬಿಟ್);

        riscv_create_fdt_socket_ಸ್ಮರಣೆ(ms->fdt, memaddr, memsize,
                                       socket, riscv_numa_enabled(ms));

        ಆದರೆ (virt_aclint_allowed() && s->have_aclint) {
            aclint_props.socket = socket;
            aclint_props.ಅಂಕೆ_harts = s->soc[socket].ಅಂಕೆ_harts;
            riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                           &intc_phandles[phandle_pos]);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tcg_enabled()) {
            hwaddr clintaddr = s->memmap[VIRT_CLINT].ತಳ +
                               s->memmap[VIRT_CLINT].ಗಾತ್ರ_2 * socket;

            riscv_create_fdt_socket_clint(ms->fdt, clintaddr,
                                          s->memmap[VIRT_CLINT].ಗಾತ್ರ_2,
                                          socket, &intc_phandles[phandle_pos],
                                          s->soc[socket].ಅಂಕೆ_harts,
                                          numa_enabled);
        }
    }

    ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        IMSICFdtProps props = {
            .soc = &s->soc,
            .socket_ಎಣಿಕೆ = riscv_socket_ಎಣಿಕೆ(ms),
            .smp_cpus = ms->smp.cpus,
            .imsic_m_ತಳ = !kvm_enabled() ? s->memmap[VIRT_IMSIC_M].ತಳ : 0,
            .imsic_s_ತಳ = s->memmap[VIRT_IMSIC_S].ತಳ,
            .imsic_ಗುಂಪು_max_ಗಾತ್ರ = VIRT_IMSIC_ಗುಂಪು_MAX_ಗಾತ್ರ,
            .irqchip_ಅಂಕೆ_msis = VIRT_IRQCHIP_ಅಂಕೆ_MSIS,
            .aia_guests = s->aia_guests
        };

        riscv_create_fdt_imsic(ms->fdt, &props, phandle, intc_phandles,
                               &msi_m_phandle, &msi_s_phandle);

        *msi_pcie_phandle = msi_s_phandle;
    }

    ಆದರೆ (s->aia_type != VIRT_AIA_TYPE_NONE) {
        aplic_props.aplic_m = !kvm_enabled() ? &s->memmap[VIRT_APLIC_M] : ಖಾಲಿ;
        aplic_props.aplic_s = &s->memmap[VIRT_APLIC_S];
        aplic_props.platform_bus = &s->memmap[VIRT_PLATFORM_BUS];
        aplic_props.platform_bus_irq = VIRT_PLATFORM_BUS_IRQ;
        aplic_props.socket = 0;
        aplic_props.ಅಂಕೆ_harts = ms->smp.cpus;
        aplic_props.numa_enabled = numa_enabled;
        aplic_props.irqchip_ಅಂಕೆ_sources = VIRT_IRQCHIP_ಅಂಕೆ_SOURCES;
        aplic_props.aia_type = s->aia_type;
    }

    /*
     * With KVM AIA aplic-imsic, using an irqchip without split
     * mode, we'll use only one APLIC instance.
     */
    ಆದರೆ (!virt_use_emulated_aplic(s->aia_type)) {
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[0], xplic_phandles);

        *irq_mmio_phandle = xplic_phandles[0];
        *irq_virtio_phandle = xplic_phandles[0];
        *irq_pcie_phandle = xplic_phandles[0];
    } ಇಲ್ಲದಿದ್ದರೆ {
        phandle_pos = ms->smp.cpus;
        ಗಾಗಿ (socket = (socket_ಎಣಿಕೆ - 1); socket >= 0; socket--) {
            phandle_pos -= s->soc[socket].ಅಂಕೆ_harts;

            ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_NONE) {
                create_fdt_socket_plic(s, socket, phandle,
                                       &intc_phandles[phandle_pos],
                                       xplic_phandles);
            } ಇಲ್ಲದಿದ್ದರೆ {
                aplic_props.socket = socket;
                aplic_props.ಅಂಕೆ_harts = s->soc[socket].ಅಂಕೆ_harts;
                riscv_create_fdt_socket_aplic(ms->fdt, &aplic_props,
                                              msi_m_phandle, msi_s_phandle,
                                              phandle,
                                              &intc_phandles[phandle_pos],
                                              xplic_phandles);
            }
        }

        ಗಾಗಿ (socket = 0; socket < socket_ಎಣಿಕೆ; socket++) {
            ಆದರೆ (socket == 0) {
                *irq_mmio_phandle = xplic_phandles[socket];
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            ಆದರೆ (socket == 1) {
                *irq_virtio_phandle = xplic_phandles[socket];
                *irq_pcie_phandle = xplic_phandles[socket];
            }
            ಆದರೆ (socket == 2) {
                *irq_pcie_phandle = xplic_phandles[socket];
            }
        }
    }

    riscv_socket_fdt_write_ದೂರ_ಮಾತೃಕೆ(ms);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_virtio(RISCVVirtState *s, uint32_t irq_virtio_phandle)
{
    ಪೂರ್ಣಾಂಕ i;
    MachineState *ms = MACHINE(s);
    hwaddr virtio_ತಳ = s->memmap[VIRT_VIRTIO].ತಳ;

    ಗಾಗಿ (i = 0; i < VIRTIO_ಎಣಿಕೆ; i++) {
        g_autofree ಅಕ್ಷರ *name = ಖಾಲಿ;
        uint64_t ಗಾತ್ರ_2 = s->memmap[VIRT_VIRTIO].ಗಾತ್ರ_2;
        hwaddr addr = virtio_ತಳ + i * ಗಾತ್ರ_2;

        name = g_strdup_printf("/soc/virtio_mmio@%"HWADDR_PRIx, addr);

        qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, name);
        qemu_fdt_setprop_string(ms->fdt, name, "compatible", "virtio,mmio");
        qemu_fdt_setprop_sized_cells(ms->fdt, name, "reg", 2, addr, 2, ಗಾತ್ರ_2);
        qemu_fdt_setprop_cell(ms->fdt, name, "interrupt-parent",
            irq_virtio_phandle);
        ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_NONE) {
            qemu_fdt_setprop_cell(ms->fdt, name, "interrupts",
                                  VIRTIO_IRQ + i);
        } ಇಲ್ಲದಿದ್ದರೆ {
            qemu_fdt_setprop_cells(ms->fdt, name, "interrupts",
                                   VIRTIO_IRQ + i, 0x4);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_uarts(RISCVVirtState *s, uint32_t irq_mmio_phandle)
{
    g_autofree ಅಕ್ಷರ *name = ಖಾಲಿ;
    MachineState *ms = MACHINE(s);

    ಆದರೆ (s->uart1_present) {
        riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART1],
                              UART1_IRQ, s->aia_type, irq_mmio_phandle);
    }

    riscv_create_fdt_uart(ms->fdt, &s->memmap[VIRT_UART0], UART0_IRQ,
                          s->aia_type, irq_mmio_phandle);

    name = riscv_fdt_get_uart_nodename(s->memmap[VIRT_UART0].ತಳ);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_fw_cfg(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    hwaddr ತಳ = s->memmap[VIRT_FW_CFG].ತಳ;
    hwaddr ಗಾತ್ರ_2 = s->memmap[VIRT_FW_CFG].ಗಾತ್ರ_2;
    g_autofree ಅಕ್ಷರ *nodename = g_strdup_printf("/fw-cfg@%" PRIx64, ತಳ);

    qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, nodename);
    qemu_fdt_setprop_string(ms->fdt, nodename,
                            "compatible", "qemu,fw-cfg-mmio");
    qemu_fdt_setprop_sized_cells(ms->fdt, nodename, "reg",
                                 2, ತಳ, 2, ಗಾತ್ರ_2);
    qemu_fdt_setprop(ms->fdt, nodename, "dma-coherent", ಖಾಲಿ, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_virtio_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ compat[] = "virtio,pci-iommu\0pci1af4,1057";
    ಶೂನ್ಯ *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree ಅಕ್ಷರ *iommu_ಗಂಟು = ಖಾಲಿ;
    g_autofree ಅಕ್ಷರ *pci_ಗಂಟು = ಖಾಲಿ;

    pci_ಗಂಟು = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].ತಳ);
    iommu_ಗಂಟು = g_strdup_printf("%s/virtio_iommu@%x,%x", pci_ಗಂಟು,
                                 PCI_ಸ್ಥಾನ(bdf), PCI_FUNC(bdf));
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, iommu_ಗಂಟು);

    qemu_fdt_setprop(fdt, iommu_ಗಂಟು, "compatible", compat, ಗಾತ್ರ(compat));
    qemu_fdt_setprop_sized_cells(fdt, iommu_ಗಂಟು, "reg",
                                 1, bdf << 8, 1, 0, 1, 0,
                                 1, 0, 1, 0);
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "phandle", iommu_phandle);

    qemu_fdt_setprop_cells(fdt, pci_ಗಂಟು, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_iommu(RISCVVirtState *s, uint16_t bdf)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ comp[] = "riscv,pci-iommu";
    ಶೂನ್ಯ *fdt = MACHINE(s)->fdt;
    uint32_t iommu_phandle;
    g_autofree ಅಕ್ಷರ *iommu_ಗಂಟು = ಖಾಲಿ;
    g_autofree ಅಕ್ಷರ *pci_ಗಂಟು = ಖಾಲಿ;

    pci_ಗಂಟು = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                               s->memmap[VIRT_PCIE_ECAM].ತಳ);
    iommu_ಗಂಟು = g_strdup_printf("%s/iommu@%x", pci_ಗಂಟು, bdf);
    iommu_phandle = qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, iommu_ಗಂಟು);

    qemu_fdt_setprop(fdt, iommu_ಗಂಟು, "compatible", comp, ಗಾತ್ರ(comp));
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "#iommu-cells", 1);
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "phandle", iommu_phandle);
    qemu_fdt_setprop_cells(fdt, iommu_ಗಂಟು, "reg",
                           bdf << 8, 0, 0, 0, 0);
    qemu_fdt_setprop_cells(fdt, pci_ಗಂಟು, "iommu-map",
                           0, iommu_phandle, 0, bdf,
                           bdf + 1, iommu_phandle, bdf + 1, 0xffff - bdf);
    s->pci_iommu_bdf = bdf;
}

ಸ್ಥಿರ ಶೂನ್ಯ finalize_fdt(RISCVVirtState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, irq_virtio_phandle = 1;
    uint32_t iommu_sys_phandle = 1;

    create_fdt_sockets(s, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &irq_virtio_phandle,
                       &msi_pcie_phandle);

    create_fdt_virtio(s, irq_virtio_phandle);

    ಆದರೆ (virt_is_iommu_sys_enabled(s)) {
        iommu_sys_phandle =
            riscv_create_fdt_riscv_iommu_sys(MACHINE(s)->fdt,
                                             s->memmap[VIRT_IOMMU_SYS].ತಳ,
                                             s->memmap[VIRT_IOMMU_SYS].ಗಾತ್ರ_2,
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
                          &virt_ಮೇಲಿನ_pcie_memmap,
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, PCIE_IRQ);

    riscv_create_fdt_syscon(MACHINE(s)->fdt, &phandle,
                            s->memmap[VIRT_TEST].ತಳ,
                            s->memmap[VIRT_TEST].ಗಾತ್ರ_2,
                            FINISHER_RESET, FINISHER_PASS, true);

    create_fdt_uarts(s, irq_mmio_phandle);

    riscv_create_fdt_rtc(MACHINE(s)->fdt, &s->memmap[VIRT_RTC], RTC_IRQ,
                         s->aia_type, irq_mmio_phandle);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt(RISCVVirtState *s)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_ಬೀಜ[32];
    g_autofree ಅಕ್ಷರ *name = ಖಾಲಿ;

    ms->fdt = riscv_create_board_device_ವೃಕ್ಷ("riscv-virtio,qemu",
                                             "riscv-virtio",
                                             &s->fdt_ಗಾತ್ರ);

    /*
     * The "/soc/pci@..." node is needed for PCIE hotplugs
     * that might happen before finalize_fdt().
     */
    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                           s->memmap[VIRT_PCIE_ECAM].ತಳ);
    qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, name);

    qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, "/chosen");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_ಬೀಜ, ಗಾತ್ರ(rng_ಬೀಜ));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_ಬೀಜ, ಗಾತ್ರ(rng_ಬೀಜ));

    qemu_fdt_ಕೂಡಿಸು_subnode(ms->fdt, "/aliases");

    riscv_create_fdt_flash(ms->fdt, s->memmap[VIRT_FLASH].ತಳ,
                           s->memmap[VIRT_FLASH].ಗಾತ್ರ_2 / 2);
    create_fdt_fw_cfg(s);
    create_fdt_pmu(s);
}

ಸ್ಥಿರ FWCfgState *create_fw_cfg(ಸ್ಥಿರಾಂಕ MachineState *ms, hwaddr ತಳ)
{
    FWCfgState *fw_cfg;

    fw_cfg = fw_cfg_init_mem_dma(ತಳ, &ವಿಳಾಸ_space_ಸ್ಮರಣೆ);
    fw_cfg_ಕೂಡಿಸು_i16(fw_cfg, FW_CFG_NB_CPUS, (uint16_t)ms->smp.cpus);

    ಹಿಂತಿರುಗಿ fw_cfg;
}

ಸ್ಥಿರ DeviceState *virt_create_plic(ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap, ಪೂರ್ಣಾಂಕ socket,
                                     ಪೂರ್ಣಾಂಕ ತಳ_hartid, ಪೂರ್ಣಾಂಕ hart_ಎಣಿಕೆ)
{
    g_autofree ಅಕ್ಷರ *plic_hart_config = ಖಾಲಿ;

    /* Per-socket PLIC hart topology configuration string */
    plic_hart_config = riscv_plic_hart_config_string(hart_ಎಣಿಕೆ);

    /* Per-socket PLIC */
    ಹಿಂತಿರುಗಿ sifive_plic_create(
             get_system_ಸ್ಮರಣೆ(),
             memmap[VIRT_PLIC].ತಳ + socket * memmap[VIRT_PLIC].ಗಾತ್ರ_2,
             plic_hart_config, hart_ಎಣಿಕೆ, ತಳ_hartid,
             VIRT_IRQCHIP_ಅಂಕೆ_SOURCES,
             ((1U << VIRT_IRQCHIP_ಅಂಕೆ_PRIO_ಬಿಟ್ಟುಗಳು) - 1),
             VIRT_PLIC_ಆದ್ಯತೆ_ತಳ, VIRT_PLIC_PENDING_ತಳ,
             VIRT_PLIC_ENABLE_ತಳ, VIRT_PLIC_ENABLE_STRIDE,
             VIRT_PLIC_CONTEXT_ತಳ,
             VIRT_PLIC_CONTEXT_STRIDE,
             memmap[VIRT_PLIC].ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_build_smbios(RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    uint8_t *smbios_tables, *smbios_anchor;
    ಗಾತ್ರ_t smbios_tables_len, smbios_anchor_len;
    ರಚನೆ smbios_phys_mem_area mem_array;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುಣಲಬ್ಧ = "QEMU Virtual Machine";

    ಆದರೆ (kvm_enabled()) {
        ಗುಣಲಬ್ಧ = "KVM Virtual Machine";
    }

    smbios_ಗಣ_defaults("QEMU", ಗುಣಲಬ್ಧ, mc->name);

    ಆದರೆ (riscv_is_32bit(&s->soc[0])) {
        smbios_ಗಣ_default_processor_family(0x200);
    } ಇಲ್ಲದಿದ್ದರೆ {
        smbios_ಗಣ_default_processor_family(0x201);
    }

    /* build the array of physical mem area from base_memmap */
    mem_array.ವಿಳಾಸ = s->memmap[VIRT_DRAM].ತಳ;
    mem_array.ಉದ್ದಳತೆ = ms->ram_ಗಾತ್ರ;

    smbios_get_tables(ms, SMBIOS_ನಮೂದು_ಬಿಂದು_TYPE_64,
                      &mem_array, 1,
                      &smbios_tables, &smbios_tables_len,
                      &smbios_anchor, &smbios_anchor_len,
                      &error_fatal);

    ಆದರೆ (smbios_anchor) {
        fw_cfg_ಕೂಡಿಸು_file(s->fw_cfg, "etc/smbios/smbios-tables",
                        smbios_tables, smbios_tables_len);
        fw_cfg_ಕೂಡಿಸು_file(s->fw_cfg, "etc/smbios/smbios-anchor",
                        smbios_anchor, smbios_anchor_len);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_machine_ಮುಗಿದ(Notifier *notifier, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    RISCVVirtState *s = container_ದ(notifier, RISCVVirtState,
                                     machine_ಮುಗಿದ);
    MachineState *machine = MACHINE(s);
    hwaddr ಆರಂಭ_addr = s->memmap[VIRT_DRAM].ತಳ;
    hwaddr firmware_ಕೊನೆ_addr;
    vaddr kernel_ಆರಂಭ_addr;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *firmware_name = riscv_default_firmware_name(&s->soc[0]);
    uint64_t fdt_ತುಂಬು_addr;
    uint64_t kernel_ನಮೂದು = 0;
    ಖಂಡ_Backend *pflash_blk0;
    RISCVBootInfo boot_info;

    /*
     * An user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    ಆದರೆ (machine->dtb == ಖಾಲಿ) {
        finalize_fdt(s);
    }

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * so the "-bios" parameter is not supported when KVM is enabled.
     */
    ಆದರೆ (kvm_enabled()) {
        ಆದರೆ (machine->firmware) {
            ಆದರೆ (strcmp(machine->firmware, "none")) {
                error_report("Machine mode firmware is not supported in "
                             "combination with KVM.");
                ನಿರ್ಗಮಿಸು(1);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            machine->firmware = g_strdup("none");
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_ಕೊನೆ_addr = riscv_ಕಂಡುಹಿಡಿ_and_ತುಂಬು_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ಆರಂಭ_addr, ಖಾಲಿ);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    ಆದರೆ (pflash_blk0) {
        ಆದರೆ (machine->firmware && !strcmp(machine->firmware, "none") &&
            !kvm_enabled()) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            ಆರಂಭ_addr = s->memmap[VIRT_FLASH].ತಳ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_ಸಿದ್ಧತೆ_firmware_boot(machine);
            kernel_ನಮೂದು = s->memmap[VIRT_FLASH].ತಳ;
        }
    }

    ಆದರೆ (machine->kernel_filename && !kernel_ನಮೂದು) {
        kernel_ಆರಂಭ_addr = riscv_calc_kernel_ಆರಂಭ_addr(&boot_info,
                                                         firmware_ಕೊನೆ_addr);
        riscv_ತುಂಬು_kernel(machine, &boot_info, kernel_ಆರಂಭ_addr,
                          true, ಖಾಲಿ);
        kernel_ನಮೂದು = boot_info.image_ಕೆಳಗಿನ_addr;
    }

    fdt_ತುಂಬು_addr = riscv_compute_fdt_addr(s->memmap[VIRT_DRAM].ತಳ,
                                           s->memmap[VIRT_DRAM].ಗಾತ್ರ_2,
                                           machine, &boot_info);
    riscv_ತುಂಬು_fdt(fdt_ತುಂಬು_addr, machine->fdt);

    /* load the reset vector */
    riscv_ಸಿದ್ಧತೆ_rom_reset_vec(machine, &s->soc[0], ಆರಂಭ_addr,
                              s->memmap[VIRT_MROM].ತಳ,
                              s->memmap[VIRT_MROM].ಗಾತ್ರ_2, kernel_ನಮೂದು,
                              fdt_ತುಂಬು_addr);

    /*
     * Only direct boot kernel is currently supported for KVM VM,
     * So here setup kernel start address and fdt address.
     * TODO:Support firmware loading and integrate to TCG start
     */
    ಆದರೆ (kvm_enabled()) {
        riscv_ಸಿದ್ಧತೆ_ನೇರ_kernel(kernel_ನಮೂದು, fdt_ತುಂಬು_addr);
    }

    virt_build_smbios(s);

    ಆದರೆ (virt_is_acpi_enabled(s)) {
        virt_acpi_ಸಿದ್ಧತೆ(s);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_machine_init(MachineState *machine)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    ಸ್ಮರಣೆ_Region *ಮುಖವಾಡ_rom = g_new(ಸ್ಮರಣೆ_Region, 1);
    DeviceState *mmio_irqchip, *virtio_irqchip, *pcie_irqchip;
    ಪೂರ್ಣಾಂಕ i, ತಳ_hartid, hart_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ socket_ಎಣಿಕೆ = riscv_socket_ಎಣಿಕೆ(machine);

    s->memmap = virt_memmap;

    /* Check socket count limit */
    ಆದರೆ (VIRT_SOCKETS_MAX < socket_ಎಣಿಕೆ) {
        error_report("number of sockets/nodes should be less than %d",
            VIRT_SOCKETS_MAX);
        ನಿರ್ಗಮಿಸು(1);
    }

    ಆದರೆ (!virt_aclint_allowed() && s->have_aclint) {
        error_report("'aclint' is only available with TCG acceleration");
        ನಿರ್ಗಮಿಸು(1);
    }

    /* Initialize sockets */
    mmio_irqchip = virtio_irqchip = pcie_irqchip = ಖಾಲಿ;
    ಗಾಗಿ (i = 0; i < socket_ಎಣಿಕೆ; i++) {
        g_autofree ಅಕ್ಷರ *soc_name = g_strdup_printf("soc%d", i);

        ಆದರೆ (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            ನಿರ್ಗಮಿಸು(1);
        }

        ತಳ_hartid = riscv_socket_ಮೊದಲ_hartid(machine, i);
        ಆದರೆ (ತಳ_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            ನಿರ್ಗಮಿಸು(1);
        }

        hart_ಎಣಿಕೆ = riscv_socket_hart_ಎಣಿಕೆ(machine, i);
        ಆದರೆ (hart_ಎಣಿಕೆ < 0) {
            error_report("can't find hart count for socket%d", i);
            ನಿರ್ಗಮಿಸು(1);
        }

        object_initialize_ಮಗು(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        object_property_ಗಣ_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_ಗಣ_int(OBJECT(&s->soc[i]), "hartid-base",
                                ತಳ_hartid, &error_abort);
        object_property_ಗಣ_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_ಎಣಿಕೆ, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        ಆದರೆ (virt_aclint_allowed() && s->have_aclint) {
            ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
                /* Per-socket ACLINT MTIMER */
                riscv_aclint_mtimer_create(system_ಸ್ಮರಣೆ,
                        s->memmap[VIRT_CLINT].ತಳ +
                            i * RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ,
                        RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ,
                        ತಳ_hartid, hart_ಎಣಿಕೆ,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* Per-socket ACLINT MSWI, MTIMER, and SSWI */
                riscv_aclint_swi_create(system_ಸ್ಮರಣೆ,
                        s->memmap[VIRT_CLINT].ತಳ +
                            i * s->memmap[VIRT_CLINT].ಗಾತ್ರ_2,
                        ತಳ_hartid, hart_ಎಣಿಕೆ, false);
                riscv_aclint_mtimer_create(system_ಸ್ಮರಣೆ,
                        s->memmap[VIRT_CLINT].ತಳ +
                            i * s->memmap[VIRT_CLINT].ಗಾತ್ರ_2 +
                            RISCV_ACLINT_SWI_ಗಾತ್ರ,
                        RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ,
                        ತಳ_hartid, hart_ಎಣಿಕೆ,
                        RISCV_ACLINT_DEFAULT_MTIMECMP,
                        RISCV_ACLINT_DEFAULT_MTIME,
                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
                riscv_aclint_swi_create(system_ಸ್ಮರಣೆ,
                        s->memmap[VIRT_ACLINT_SSWI].ತಳ +
                            i * s->memmap[VIRT_ACLINT_SSWI].ಗಾತ್ರ_2,
                        ತಳ_hartid, hart_ಎಣಿಕೆ, true);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tcg_enabled()) {
            /* Per-socket SiFive CLINT */
            riscv_aclint_swi_create(system_ಸ್ಮರಣೆ,
                    s->memmap[VIRT_CLINT].ತಳ + i * s->memmap[VIRT_CLINT].ಗಾತ್ರ_2,
                    ತಳ_hartid, hart_ಎಣಿಕೆ, false);
            riscv_aclint_mtimer_create(system_ಸ್ಮರಣೆ,
                    s->memmap[VIRT_CLINT].ತಳ +
                    i * s->memmap[VIRT_CLINT].ಗಾತ್ರ_2 + RISCV_ACLINT_SWI_ಗಾತ್ರ,
                    RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ, ತಳ_hartid, hart_ಎಣಿಕೆ,
                    RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
                    RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);
        }

        /* Per-socket interrupt controller */
        ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_NONE) {
            s->irqchip[i] = virt_create_plic(s->memmap, i,
                                             ತಳ_hartid, hart_ಎಣಿಕೆ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            bool msimode = s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC;
            ಪೂರ್ಣಾಂಕ imsic_ಬಿಟ್ಟುಗಳು = imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(s->aia_guests + 1);
            s->irqchip[i] = riscv_create_aia(system_ಸ್ಮರಣೆ, msimode,
                                             s->aia_guests,
                                             IMSIC_HART_ಗಾತ್ರ(0),
                                             IMSIC_HART_ಗಾತ್ರ(imsic_ಬಿಟ್ಟುಗಳು),
                                             s->ಅಂಕೆ_sources,
                                             &s->memmap[VIRT_APLIC_M],
                                             &s->memmap[VIRT_APLIC_S],
                                             &s->memmap[VIRT_IMSIC_M],
                                             &s->memmap[VIRT_IMSIC_S],
                                             i, ತಳ_hartid, hart_ಎಣಿಕೆ,
                                             VIRT_IRQCHIP_ಅಂಕೆ_MSIS,
                                             VIRT_IRQCHIP_ಅಂಕೆ_PRIO_ಬಿಟ್ಟುಗಳು);
        }

        /* Try to use different IRQCHIP instance based device type */
        ಆದರೆ (i == 0) {
            mmio_irqchip = s->irqchip[i];
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ಆದರೆ (i == 1) {
            virtio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ಆದರೆ (i == 2) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    ಆದರೆ (kvm_enabled() && virt_use_kvm_aia_aplic_imsic(s->aia_type)) {
        kvm_riscv_aia_create(machine, IMSIC_MMIO_ಗುಂಪು_MIN_ಸರಿಸು,
                             VIRT_IRQCHIP_ಅಂಕೆ_SOURCES, VIRT_IRQCHIP_ಅಂಕೆ_MSIS,
                             s->memmap[VIRT_APLIC_S].ತಳ,
                             s->memmap[VIRT_IMSIC_S].ತಳ,
                             s->aia_guests);
    }

    ಆದರೆ (riscv_is_32bit(&s->soc[0])) {
#ಆದರೆ HOST_LONG_ಬಿಟ್ಟುಗಳು == 64
        /* limit RAM size in a 32-bit system */
        ಆದರೆ (machine->ram_ಗಾತ್ರ > 10 * GiB) {
            machine->ram_ಗಾತ್ರ = 10 * GiB;
            error_report("Limiting RAM size to 10 GiB");
        }
#ಆದರೆ_ಅಂತ್ಯ
        virt_ಮೇಲಿನ_pcie_memmap.ತಳ = VIRT32_ಮೇಲಿನ_PCIE_MMIO_ತಳ;
        virt_ಮೇಲಿನ_pcie_memmap.ಗಾತ್ರ_2 = VIRT32_ಮೇಲಿನ_PCIE_MMIO_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        virt_ಮೇಲಿನ_pcie_memmap.ಗಾತ್ರ_2 = VIRT64_ಮೇಲಿನ_PCIE_MMIO_ಗಾತ್ರ;
        virt_ಮೇಲಿನ_pcie_memmap.ತಳ = s->memmap[VIRT_DRAM].ತಳ +
                                     machine->ram_ಗಾತ್ರ;
        virt_ಮೇಲಿನ_pcie_memmap.ತಳ =
            ಸುತ್ತು_ಮೇಲೆ(virt_ಮೇಲಿನ_pcie_memmap.ತಳ, virt_ಮೇಲಿನ_pcie_memmap.ಗಾತ್ರ_2);
    }

    /* register system main memory (actual RAM) */
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ, s->memmap[VIRT_DRAM].ತಳ,
                                machine->ram);

    /* boot rom */
    ಸ್ಮರಣೆ_region_init_rom(ಮುಖವಾಡ_rom, ಖಾಲಿ, "riscv_virt_board.mrom",
                           s->memmap[VIRT_MROM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ, s->memmap[VIRT_MROM].ತಳ,
                                ಮುಖವಾಡ_rom);

    /*
     * Init fw_cfg. Must be done before riscv_load_fdt, otherwise the
     * device tree cannot be altered and we get FDT_ERR_NOSPACE.
     */
    s->fw_cfg = create_fw_cfg(machine, s->memmap[VIRT_FW_CFG].ತಳ);
    rom_ಗಣ_fw(s->fw_cfg);

    /* SiFive Test MMIO device */
    sifive_test_create(s->memmap[VIRT_TEST].ತಳ);

    /* VirtIO MMIO devices */
    ಗಾಗಿ (i = 0; i < VIRTIO_ಎಣಿಕೆ; i++) {
        sysbus_create_simple("virtio-mmio",
            s->memmap[VIRT_VIRTIO].ತಳ + i * s->memmap[VIRT_VIRTIO].ಗಾತ್ರ_2,
            qdev_get_gpio_ಒಳ(virtio_irqchip, VIRTIO_IRQ + i));
    }

    riscv_gpex_pcie_init(system_ಸ್ಮರಣೆ, pcie_irqchip,
                   &s->memmap[VIRT_PCIE_ECAM], &s->memmap[VIRT_PCIE_MMIO],
                   &virt_ಮೇಲಿನ_pcie_memmap, &s->memmap[VIRT_PCIE_PIO],
                   PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
        &s->memmap[VIRT_PLATFORM_BUS], VIRT_PLATFORM_BUS_IRQ,
        VIRT_PLATFORM_BUS_ಅಂಕೆ_IRQS);

    serial_mm_init(system_ಸ್ಮರಣೆ, s->memmap[VIRT_UART0].ತಳ,
        0, qdev_get_gpio_ಒಳ(mmio_irqchip, UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    ಆದರೆ (serial_hd(1)) {
        serial_mm_init(system_ಸ್ಮರಣೆ, s->memmap[VIRT_UART1].ತಳ,
            0, qdev_get_gpio_ಒಳ(mmio_irqchip, UART1_IRQ), 399193,
            serial_hd(1), DEVICE_LITTLE_ENDIAN);
        s->uart1_present = true;
    }

    sysbus_create_simple("goldfish_rtc", s->memmap[VIRT_RTC].ತಳ,
        qdev_get_gpio_ಒಳ(mmio_irqchip, RTC_IRQ));

    ಗಾಗಿ (i = 0; i < ARRAY_ಗಾತ್ರ(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    virt_flash_map(s, system_ಸ್ಮರಣೆ);

    /* load/create device tree */
    ಆದರೆ (machine->dtb) {
        machine->fdt = ತುಂಬು_device_ವೃಕ್ಷ(machine->dtb, &s->fdt_ಗಾತ್ರ);
        ಆದರೆ (!machine->fdt) {
            error_report("load_device_tree() failed");
            ನಿರ್ಗಮಿಸು(1);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        create_fdt(s);
    }

    ಆದರೆ (virt_is_iommu_sys_enabled(s)) {
        riscv_create_iommu_sys(mmio_irqchip, s->memmap[VIRT_IOMMU_SYS].ತಳ,
                               IOMMU_SYS_IRQ, riscv_is_32bit(&s->soc[0]));
    }

    s->machine_ಮುಗಿದ.notify = virt_machine_ಮುಗಿದ;
    qemu_ಕೂಡಿಸು_machine_init_ಮುಗಿದ_notifier(&s->machine_ಮುಗಿದ);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_machine_instance_finalize(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ARRAY_ಗಾತ್ರ(s->flash); i++) {
        ಆದರೆ (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
    g_free(s->oem_id);
    g_free(s->oem_ಕೋಷ್ಟಕ_id);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_machine_instance_init(Object *obj)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    virt_flash_create(s);

    s->oem_id = g_strndup(ACPI_BUILD_APPNAME6, 6);
    s->oem_ಕೋಷ್ಟಕ_id = g_strndup(ACPI_BUILD_APPNAME8, 8);
    s->acpi = ON_OFF_AUTO_AUTO;
    s->iommu_sys = ON_OFF_AUTO_AUTO;
    s->ಅಂಕೆ_sources = VIRT_IRQCHIP_ಅಂಕೆ_SOURCES;
}

ಸ್ಥಿರ ಅಕ್ಷರ *virt_get_aia_guests(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ಹಿಂತಿರುಗಿ g_strdup_printf("%d", s->aia_guests);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_ಗಣ_aia_guests(Object *obj, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->aia_guests = atoi(val);
    ಆದರೆ (s->aia_guests < 0 || s->aia_guests > VIRT_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          VIRT_IRQCHIP_MAX_GUESTS);
    }
}

ಸ್ಥಿರ ಅಕ್ಷರ *virt_get_aia(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val;

    ಆಯ್ಕೆ (s->aia_type) {
    ಸ್ಥಿತಿ VIRT_AIA_TYPE_APLIC:
        val = "aplic";
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VIRT_AIA_TYPE_APLIC_IMSIC:
        val = "aplic-imsic";
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        val = "none";
        ನಿಲ್ಲಿಸು;
    };

    ಹಿಂತಿರುಗಿ g_strdup(val);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_ಗಣ_aia(Object *obj, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ಆದರೆ (!strcmp(val, "none")) {
        s->aia_type = VIRT_AIA_TYPE_NONE;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strcmp(val, "aplic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strcmp(val, "aplic-imsic")) {
        s->aia_type = VIRT_AIA_TYPE_APLIC_IMSIC;
    } ಇಲ್ಲದಿದ್ದರೆ {
        error_setg(errp, "Invalid AIA interrupt controller type");
        error_append_hint(errp, "Valid values are none, aplic, and "
                          "aplic-imsic.\n");
    }
}

ಸ್ಥಿರ bool virt_get_aclint(Object *obj, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ಹಿಂತಿರುಗಿ s->have_aclint;
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_ಗಣ_aclint(Object *obj, bool ಮೌಲ್ಯ, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    s->have_aclint = ಮೌಲ್ಯ;
}

bool virt_is_iommu_sys_enabled(RISCVVirtState *s)
{
    ಹಿಂತಿರುಗಿ s->iommu_sys == ON_OFF_AUTO_ON;
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_get_iommu_sys(Object *obj, Visitor *v, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                               ಶೂನ್ಯ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto iommu_sys = s->iommu_sys;

    ಭೇಟಿ_type_On_Off_Auto(v, name, &iommu_sys, errp);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_ಗಣ_iommu_sys(Object *obj, Visitor *v, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                               ಶೂನ್ಯ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ಭೇಟಿ_type_On_Off_Auto(v, name, &s->iommu_sys, errp);
}

bool virt_is_acpi_enabled(RISCVVirtState *s)
{
    ಹಿಂತಿರುಗಿ s->acpi != ON_OFF_AUTO_OFF;
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_get_acpi(Object *obj, Visitor *v, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                          ಶೂನ್ಯ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);
    OnOffAuto acpi = s->acpi;

    ಭೇಟಿ_type_On_Off_Auto(v, name, &acpi, errp);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_ಗಣ_acpi(Object *obj, Visitor *v, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                          ಶೂನ್ಯ *opaque, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(obj);

    ಭೇಟಿ_type_On_Off_Auto(v, name, &s->acpi, errp);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ HotplugHandler *
virt_machine_get_hotplug_handler(MachineState *machine, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    RISCVVirtState *s = RISCV_VIRT_MACHINE(machine);

    ಆದರೆ (device_is_dynamic_sysbus(mc, dev) ||
        object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI) ||
        object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        s->iommu_sys = ON_OFF_AUTO_OFF;
        ಹಿಂತಿರುಗಿ HOTPLUG_HANDLER(machine);
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_machine_device_plug_cb(ಸ್ಥಿರಾಂಕ HotplugHandler *hotplug_dev,
                                        DeviceState *dev, Error **errp)
{
    RISCVVirtState *s = RISCV_VIRT_MACHINE(hotplug_dev);

    ಆದರೆ (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        ಆದರೆ (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }

    ಆದರೆ (object_dynamic_cast(OBJECT(dev), TYPE_VIRTIO_IOMMU_PCI)) {
        create_fdt_virtio_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
    }

    ಆದರೆ (object_dynamic_cast(OBJECT(dev), TYPE_RISCV_IOMMU_PCI)) {
        create_fdt_iommu(s, pci_get_bdf(PCI_DEVICE(dev)));
        s->iommu_sys = ON_OFF_AUTO_OFF;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_machine_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);

    mc->desc = "RISC-V VirtIO board";
    mc->init = virt_machine_init;
    mc->max_cpus = VIRT_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_ತಳ;
    mc->ಖಂಡ_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->pci_allow_0_ವಿಳಾಸ = true;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_instance_props = riscv_numa_cpu_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_props;
    mc->get_default_cpu_ಗಂಟು_id = riscv_numa_get_default_cpu_ಗಂಟು_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_virt_board.ram";
    ಪ್ರತಿಜ್ಞಾ(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = virt_machine_get_hotplug_handler;

    hc->plug = virt_machine_device_plug_cb;

    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_RAMFB_DEVICE);
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_UEFI_VARS_SYSBUS);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#ಆದರೆ_ಅಂತ್ಯ

    object_class_property_ಕೂಡಿಸು_bool(oc, "aclint", virt_get_aclint,
                                   virt_ಗಣ_aclint);
    object_class_property_ಗಣ_description(oc, "aclint",
                                          "(TCG only) Set on/off to "
                                          "enable/disable emulating "
                                          "ACLINT devices");

    object_class_property_ಕೂಡಿಸು_str(oc, "aia", virt_get_aia,
                                  virt_ಗಣ_aia);
    object_class_property_ಗಣ_description(oc, "aia",
                                          "Set type of AIA interrupt "
                                          "controller. Valid values are "
                                          "none, aplic, and aplic-imsic.");

    object_class_property_ಕೂಡಿಸು_str(oc, "aia-guests",
                                  virt_get_aia_guests,
                                  virt_ಗಣ_aia_guests);
    {
        g_autofree ಅಕ್ಷರ *str =
            g_strdup_printf("Set number of guest MMIO pages for AIA IMSIC. "
                            "Valid value should be between 0 and %d.",
                            VIRT_IRQCHIP_MAX_GUESTS);
        object_class_property_ಗಣ_description(oc, "aia-guests", str);
    }

    object_class_property_ಕೂಡಿಸು(oc, "acpi", "OnOffAuto",
                              virt_get_acpi, virt_ಗಣ_acpi,
                              ಖಾಲಿ, ಖಾಲಿ);
    object_class_property_ಗಣ_description(oc, "acpi",
                                          "Enable ACPI");

    object_class_property_ಕೂಡಿಸು(oc, "iommu-sys", "OnOffAuto",
                              virt_get_iommu_sys, virt_ಗಣ_iommu_sys,
                              ಖಾಲಿ, ಖಾಲಿ);
    object_class_property_ಗಣ_description(oc, "iommu-sys",
                                          "Enable IOMMU platform device");
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo virt_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("virt"),
    .ಪೋಷಕ     = TYPE_MACHINE,
    .class_init = virt_machine_class_init,
    .instance_init = virt_machine_instance_init,
    .instance_finalize = virt_machine_instance_finalize,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(RISCVVirtState),
    .interfaces = (ಸ್ಥಿರಾಂಕ InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

ಸ್ಥಿರ ಶೂನ್ಯ virt_machine_init_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&virt_machine_typeinfo);
}

type_init(virt_machine_init_ರಿಜಿಸ್ಟರ್_types)
