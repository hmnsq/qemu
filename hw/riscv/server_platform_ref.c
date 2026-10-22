/*
 * QEMU RISC-V Server Platform Reference Board (riscv-server-ref)
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qemu/guest-random.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qapi/qapi-visit-common.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/platform-bus.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/char/serial.h"
#ಅನ್ತರ್ಭಾವ "hw/block/flash.h"
#ಅನ್ತರ್ಭಾವ "hw/ide/pci.h"
#ಅನ್ತರ್ಭಾವ "hw/ide/ahci-pci.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pci.h"
#ಅನ್ತರ್ಭಾವ "hw/pci-host/gpex.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus-fdt.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/device-common.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/fdt-common.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/numa.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/iommu.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv-iommu.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv-iommu-bits.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aplic.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_imsic.h"
#ಅನ್ತರ್ಭಾವ "chardev/char.h"
#ಅನ್ತರ್ಭಾವ "hw/char/serial-mm.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "system/runstate.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "system/kvm.h"
#ಅನ್ತರ್ಭಾವ "system/qtest.h"
#ಅನ್ತರ್ಭಾವ "system/tcg.h"
#ಅನ್ತರ್ಭಾವ "kvm/kvm_riscv.h"
#ಅನ್ತರ್ಭಾವ "system/tpm.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "net/net.h"

#ಅನ್ತರ್ಭಾವ "aia.h"

#define RVSERVER_ACLINT_TIMEBASE_FREQ          1000000000 /* 1Ghz */
#define RVSERVER_CPUS_MAX_BITS                 9
#define RVSERVER_CPUS_MAX                      (1 << RVSERVER_CPUS_MAX_BITS)
#define RVSERVER_IRQCHIP_ಅಂಕೆ_MSIS              255
#define RVSERVER_IRQCHIP_ಅಂಕೆ_SOURCES           96
#define RVSERVER_IRQCHIP_ಅಂಕೆ_PRIO_BITS         3
#define RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS    5
#define RVSERVER_IRQCHIP_MAX_GUESTS_BITS       3
#define RVSERVER_IRQCHIP_MAX_GUESTS \
        ((1U << RVSERVER_IRQCHIP_MAX_GUESTS_BITS) - 1U)
#define RVSERVER_SOCKETS_MAX_BITS              2
#define RVSERVER_SOCKETS_MAX                   (1 << RVSERVER_SOCKETS_MAX_BITS)

#define SYSCON_RESET                           0x1
#define SYSCON_POWEROFF                        0x2

#define RVSERVER_PLATFORM_BUS_ಅಂಕೆ_IRQS         8

#define TYPE_RISCV_SERVER_REF_MACHINE MACHINE_TYPE_NAME("riscv-server-ref")
OBJECT_DECLARE_SIMPLE_TYPE(RISCVServerRefMachineState, RISCV_SERVER_REF_MACHINE)

ಸಂರಚನಾ RISCVServerRefMachineState {
    /*< private >*/
    MachineState parent;

    /*< public >*/
    Notifier machine_done;
    RISCVHartArrayState soc[RVSERVER_SOCKETS_MAX];
    DeviceState *irqchip[RVSERVER_SOCKETS_MAX];
    PFlashCFI01 *flash[2];

    ಪೂರ್ಣಾಙ್ಕ fdt_size;
    ಪೂರ್ಣಾಙ್ಕ aia_guests;
    ಧ್ರುವ MemMapEntry *memmap;

    DeviceState *platform_bus_dev;
};

ಗಣನಾ {
    RVSERVER_MROM,
    RVSERVER_RESET_SYSCON,
    RVSERVER_RTC,
    RVSERVER_IOMMU_SYS,
    RVSERVER_ACLINT,
    RVSERVER_APLIC_M,
    RVSERVER_APLIC_S,
    RVSERVER_UART0,
    RVSERVER_IMSIC_M,
    RVSERVER_IMSIC_S,
    RVSERVER_FLASH,
    RVSERVER_DRAM,
    RVSERVER_PCIE_MMIO,
    RVSERVER_PCIE_PIO,
    RVSERVER_PLATFORM_BUS,
    RVSERVER_PCIE_ECAM,
    RVSERVER_PCIE_MMIO_ಮೇಲಿನ
};

ಗಣನಾ {
    RVSERVER_UART0_IRQ = 10,
    RVSERVER_RTC_IRQ   = 11,
    RVSERVER_PCIE_IRQ  = 32, /* 32 to 35 */
    IOMMU_SYS_IRQ      = 36, /* 36 to 39 */
    RVSERVER_PLATFORM_BUS_IRQ = 40, /* 40 to 47 */
};

/*
 * The server soc reference machine physical address space used by some of the
 * devices namely ACLINT, APLIC and IMSIC depend on number of Sockets, number
 * of CPUs, and number of IMSIC guest files.
 *
 * Various limits defined by RVSERVER_SOCKETS_MAX_BITS, RVSERVER_CPUS_MAX_BITS,
 * and RVSERVER_IRQCHIP_MAX_GUESTS_BITS are tuned for maximum utilization of
 * server reference machine physical address space.
 */

#define RVSERVER_IMSIC_GROUP_MAX_SIZE      (1U << IMSIC_MMIO_GROUP_MIN_SHIFT)
#ಯದಿ RVSERVER_IMSIC_GROUP_MAX_SIZE < \
    IMSIC_GROUP_SIZE(RVSERVER_CPUS_MAX_BITS, RVSERVER_IRQCHIP_MAX_GUESTS_BITS)
#error "Can't accommodate single IMSIC group in address space"
#endif

#define RVSERVER_IMSIC_MAX_SIZE \
    (RVSERVER_SOCKETS_MAX * RVSERVER_IMSIC_GROUP_MAX_SIZE)
#ಯದಿ 0x4000000 < RVSERVER_IMSIC_MAX_SIZE
#error "Can't accommodate all IMSIC groups in address space"
#endif

ಸ್ಥಿರ ಧ್ರುವ MemMapEntry rvserver_ref_memmap[] = {
    [RVSERVER_MROM] =           {     0x1000,        0xf000 },
    [RVSERVER_RESET_SYSCON] =   {   0x100000,        0x1000 },
    [RVSERVER_RTC] =            {   0x101000,        0x1000 },
    [RVSERVER_IOMMU_SYS] =      {   0x102000,        0x1000 },
    [RVSERVER_ACLINT] =         {  0x2000000,       0x10000 },
    [RVSERVER_PCIE_PIO] =       {  0x3000000,       0x10000 },
    [RVSERVER_PLATFORM_BUS] =   {  0x4000000,     0x2000000 },
    [RVSERVER_APLIC_M] =        {  0xc000000, APLIC_SIZE(RVSERVER_CPUS_MAX) },
    [RVSERVER_APLIC_S] =        {  0xd000000, APLIC_SIZE(RVSERVER_CPUS_MAX) },
    [RVSERVER_UART0] =          { 0x10000000,         0x100 },
    [RVSERVER_FLASH] =          { 0x20000000,     0x4000000 },
    [RVSERVER_IMSIC_M] =        { 0x24000000, RVSERVER_IMSIC_MAX_SIZE },
    [RVSERVER_IMSIC_S] =        { 0x28000000, RVSERVER_IMSIC_MAX_SIZE },
    [RVSERVER_PCIE_ECAM] =      { 0x30000000,    0x10000000 },
    [RVSERVER_PCIE_MMIO] =      { 0x40000000,    0x40000000 },
    [RVSERVER_DRAM] =           { 0x80000000, 0xff80000000ull },
    [RVSERVER_PCIE_MMIO_ಮೇಲಿನ] = { 0x10000000000ull, 0x10000000000ull },
};

#define RVSERVER_FLASH_SECTOR_SIZE (256 * KiB)

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_flash_maps(RISCVServerRefMachineState *s,
                                MemoryRegion *sysmem)
{
    hwaddr flashsize = rvserver_ref_memmap[RVSERVER_FLASH].size / 2;
    hwaddr flashbase = rvserver_ref_memmap[RVSERVER_FLASH].base;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_SIZE);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_SIZE);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_pmu(RISCVServerRefMachineState *s)
{
    g_autofree ವರ್ಣ *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_add_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_ಗಂಟು(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_sockets(RISCVServerRefMachineState *s,
                               ಧ್ರುವ MemMapEntry *memmap,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
                               uint32_t *msi_pcie_phandle)
{
    ಪೂರ್ಣಾಙ್ಕ socket, phandle_pos;
    MachineState *ms = MACHINE(s);
    uint32_t msi_m_phandle = 0, msi_s_phandle = 0;
    uint32_t xplic_phandles[MAX_NODES];
    g_autofree uint32_t *intc_phandles = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ socket_ಎಣಿಕೆ = riscv_socket_ಎಣಿಕೆ(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_bit = riscv_is_32bit(&s->soc[0]);
    IMSICFdtProps imsic_fdt_props;
    APLICFdtProps aplic_fdt_props;
    ACLINTFdtProps aclint_props;

    riscv_fdt_create_cpu_socket_subnode(ms->fdt,
        kvm_enabled() ? kvm_riscv_get_timebase_frequency(&s->soc->harts[0])
                        : RVSERVER_ACLINT_TIMEBASE_FREQ);

    intc_phandles = g_new0(uint32_t, ms->smp.cpus);

    aclint_props.clint = &memmap[RVSERVER_ACLINT];
    aclint_props.aia_type = AIA_TYPE_APLIC_IMSIC;
    aclint_props.numa_enabled = numa_enabled;

    phandle_pos = ms->smp.cpus;
    ಪ್ರತಿ (socket = (socket_ಎಣಿಕೆ - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[RVSERVER_DRAM].base +
                         riscv_socket_mem_offset(ms, socket);
        uint64_t memsize = riscv_socket_mem_size(ms, socket);

        phandle_pos -= s->soc[socket].ಅಂಕೆ_harts;

        riscv_create_fdt_socket_cpus(ms->fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].ಅಂಕೆ_harts,
                                     s->soc[socket].hartid_base,
                                     phandle, &intc_phandles[phandle_pos],
                                     numa_enabled, is_32_bit);

        riscv_create_fdt_socket_memory(ms->fdt, memaddr, memsize,
                                       socket, riscv_numa_enabled(ms));

        aclint_props.socket = socket;
        aclint_props.ಅಂಕೆ_harts = s->soc[socket].ಅಂಕೆ_harts;
        riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                       &intc_phandles[phandle_pos]);
    }

    imsic_fdt_props.soc = &s->soc;
    imsic_fdt_props.socket_ಎಣಿಕೆ = socket_ಎಣಿಕೆ;
    imsic_fdt_props.smp_cpus = ms->smp.cpus;
    imsic_fdt_props.imsic_m_base = memmap[RVSERVER_IMSIC_M].base;
    imsic_fdt_props.imsic_s_base = memmap[RVSERVER_IMSIC_S].base;
    imsic_fdt_props.imsic_group_max_size = RVSERVER_IMSIC_GROUP_MAX_SIZE;
    imsic_fdt_props.irqchip_ಅಂಕೆ_msis = RVSERVER_IRQCHIP_ಅಂಕೆ_MSIS;
    imsic_fdt_props.aia_guests = s->aia_guests;

    riscv_create_fdt_imsic(ms->fdt, &imsic_fdt_props, phandle, intc_phandles,
                           &msi_m_phandle, &msi_s_phandle);
    *msi_pcie_phandle = msi_s_phandle;

    aplic_fdt_props.aplic_m = !kvm_enabled()
                              ? &memmap[RVSERVER_APLIC_M] : ರಿಕ್ತ;
    aplic_fdt_props.aplic_s = &memmap[RVSERVER_APLIC_S];
    aplic_fdt_props.platform_bus = &memmap[RVSERVER_PLATFORM_BUS];
    aplic_fdt_props.platform_bus_irq = RVSERVER_PLATFORM_BUS_IRQ;
    aplic_fdt_props.ಅಂಕೆ_harts = ms->smp.cpus;
    aplic_fdt_props.numa_enabled = numa_enabled;
    aplic_fdt_props.irqchip_ಅಂಕೆ_sources = RVSERVER_IRQCHIP_ಅಂಕೆ_SOURCES;
    aplic_fdt_props.aia_type = AIA_TYPE_APLIC_IMSIC;

    phandle_pos = ms->smp.cpus;
    ಪ್ರತಿ (socket = (socket_ಎಣಿಕೆ - 1); socket >= 0; socket--) {
        phandle_pos -= s->soc[socket].ಅಂಕೆ_harts;

        aplic_fdt_props.socket = socket;
        aplic_fdt_props.ಅಂಕೆ_harts = s->soc[socket].ಅಂಕೆ_harts;
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_fdt_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[phandle_pos],
                                      xplic_phandles);
    }

    ಪ್ರತಿ (socket = 0; socket < socket_ಎಣಿಕೆ; socket++) {
        ಯದಿ (socket == 0) {
            *irq_mmio_phandle = xplic_phandles[socket];
            *irq_pcie_phandle = xplic_phandles[socket];
        }
        ಯದಿ (socket == 1) {
            *irq_pcie_phandle = xplic_phandles[socket];
        }
    }

    riscv_socket_fdt_write_distance_matrix(ms);
}

ಸ್ಥಿರ ಶೂನ್ಯ finalize_fdt(RISCVServerRefMachineState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, iommu_sys_phandle;
    g_autofree ವರ್ಣ *name = ರಿಕ್ತ;
    MachineState *ms = MACHINE(s);

    create_fdt_sockets(s, rvserver_ref_memmap, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &msi_pcie_phandle);

    iommu_sys_phandle = riscv_create_fdt_riscv_iommu_sys(ms->fdt,
                                         s->memmap[RVSERVER_IOMMU_SYS].base,
                                         s->memmap[RVSERVER_IOMMU_SYS].size,
                                         &phandle, irq_mmio_phandle,
                                         msi_pcie_phandle, IOMMU_SYS_IRQ);

    riscv_create_fdt_pcie(ms->fdt, AIA_TYPE_APLIC_IMSIC, true,
                          &s->memmap[RVSERVER_PCIE_ECAM],
                          &s->memmap[RVSERVER_PCIE_PIO],
                          &s->memmap[RVSERVER_PCIE_MMIO],
                          &s->memmap[RVSERVER_PCIE_MMIO_ಮೇಲಿನ],
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, RVSERVER_PCIE_IRQ);

    riscv_create_fdt_syscon(ms->fdt, &phandle,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].base,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].size,
                            SYSCON_RESET, SYSCON_POWEROFF, false);

    riscv_create_fdt_uart(ms->fdt, &rvserver_ref_memmap[RVSERVER_UART0],
                          RVSERVER_UART0_IRQ, AIA_TYPE_APLIC_IMSIC,
                          irq_mmio_phandle);
    name = riscv_fdt_get_uart_nodename(rvserver_ref_memmap[RVSERVER_UART0].base);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);

    riscv_create_fdt_rtc(ms->fdt, &rvserver_ref_memmap[RVSERVER_RTC],
                         RVSERVER_RTC_IRQ, AIA_TYPE_APLIC_IMSIC,
                         irq_mmio_phandle);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt(RISCVServerRefMachineState *s,
                       ಧ್ರುವ MemMapEntry *memmap)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_seed[32];
    g_autofree ವರ್ಣ *name = ರಿಕ್ತ;

    ms->fdt = riscv_create_board_device_ವೃಕ್ಷ(MACHINE_GET_CLASS(s)->desc,
                                             "qemu,riscv-server-ref",
                                             &s->fdt_size);

    /*
     * This versioning scheme is for informing platform fw only. It is neither:
     * - A QEMU versioned machine type; a given version of QEMU will emulate
     *   a given version of the platform.
     * - A reflection of level of server platform support provided.
     *
     * machine-version-major: updated when changes breaking fw compatibility
     *                        are introduced.
     * machine-version-minor: updated when features are added that don't break
     *                        fw compatibility.
     *
     * It's the same as the scheme in arm sbsa-ref.
     */
    qemu_fdt_setprop_cell(ms->fdt, "/", "machine-version-major", 0);
    qemu_fdt_setprop_cell(ms->fdt, "/", "machine-version-minor", 0);

    /*
     * The "/soc/pci@..." node is needed for PCIE hotplugs
     * that might happen before finalize_fdt().
     */
    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx,
                           s->memmap[RVSERVER_PCIE_ECAM].base);
    qemu_fdt_add_subnode(ms->fdt, name);

    qemu_fdt_add_subnode(ms->fdt, "/chosen");
    qemu_fdt_add_subnode(ms->fdt, "/aliases");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_seed, ಪರಿಮಾಣ(rng_seed));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_seed, ಪರಿಮಾಣ(rng_seed));

    riscv_create_fdt_flash(ms->fdt, rvserver_ref_memmap[RVSERVER_FLASH].base,
                           rvserver_ref_memmap[RVSERVER_FLASH].size / 2);
    create_fdt_pmu(s);

}

ಸ್ಥಿರ uint64_t rvserver_reset_syscon_read(ಶೂನ್ಯ *opaque, hwaddr addr,
                                           unsigned size)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_reset_syscon_write(ಶೂನ್ಯ *opaque, hwaddr addr,
                                        uint64_t val64, unsigned ಪೂರ್ಣಾಙ್ಕ size)
{
    ವಿಕಲ್ಪ (val64) {
    ಪಕ್ಷ SYSCON_POWEROFF:
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_GUEST_SHUTDOWN);
        ಪ್ರತ್ಯಾಗಮ;
    ಪಕ್ಷ SYSCON_RESET:
        qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        ಪ್ರತ್ಯಾಗಮ;
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }
}

ಸ್ಥಿರ ಧ್ರುವ MemoryRegionOps rvserver_reset_syscon_ops = {
    .ಪಠ = rvserver_reset_syscon_read,
    .ಲಿಖ = rvserver_reset_syscon_write,
    .endianness = DEVICE_LITTLE_ENDIAN
};

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_ref_machine_done(Notifier *notifier, ಶೂನ್ಯ *data)
{
    RISCVServerRefMachineState *s = container_of(
        notifier, RISCVServerRefMachineState, machine_done);
    ಧ್ರುವ MemMapEntry *memmap = rvserver_ref_memmap;
    MachineState *machine = MACHINE(s);
    hwaddr start_addr = memmap[RVSERVER_DRAM].base;
    target_ulong firmware_end_addr, kernel_start_addr;
    ಧ್ರುವ ವರ್ಣ *firmware_name = riscv_default_firmware_name(&s->soc[0]);
    uint64_t fdt_load_addr;
    uint64_t kernel_entry = 0;
    BlockBackend *pflash_blk0;
    RISCVBootInfo boot_info;

    /*
     * An user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    ಯದಿ (machine->dtb == ರಿಕ್ತ) {
        finalize_fdt(s);
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_end_addr = riscv_find_and_load_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &start_addr, ರಿಕ್ತ);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    ಯದಿ (pflash_blk0) {
        ಯದಿ (machine->firmware && !strcmp(machine->firmware, "none")) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            start_addr = rvserver_ref_memmap[RVSERVER_FLASH].base;
        } ಅನ್ಯಥಾ {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_setup_firmware_boot(machine);
            kernel_entry = rvserver_ref_memmap[RVSERVER_FLASH].base;
        }
    }

    ಯದಿ (machine->kernel_filename && !kernel_entry) {
        kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                         firmware_end_addr);
        riscv_load_kernel(machine, &boot_info, kernel_start_addr, true, ರಿಕ್ತ);
        kernel_entry = boot_info.image_ಕೆಳಗಿನ_addr;
    }

    fdt_load_addr = riscv_compute_fdt_addr(memmap[RVSERVER_DRAM].base,
                                           memmap[RVSERVER_DRAM].size,
                                           machine, &boot_info);

    riscv_load_fdt(fdt_load_addr, machine->fdt);

    /* load the reset vector */
    riscv_setup_rom_reset_vec(machine, &s->soc[0], start_addr,
                              rvserver_ref_memmap[RVSERVER_MROM].base,
                              rvserver_ref_memmap[RVSERVER_MROM].size,
                              kernel_entry,
                              fdt_load_addr);
}

ಸ್ಥಿರ bool rvserver_aclint_allowed(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ tcg_enabled() || qtest_enabled();
}

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_ref_machine_init(MachineState *machine)
{
    ಧ್ರುವ MemMapEntry *memmap = rvserver_ref_memmap;
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *mask_rom = g_new(MemoryRegion, 1);
    MemoryRegion *reset_syscon_io = g_new(MemoryRegion, 1);
    DeviceState *mmio_irqchip, *pcie_irqchip;
    ಪೂರ್ಣಾಙ್ಕ i, base_hartid, hart_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ socket_ಎಣಿಕೆ = riscv_socket_ಎಣಿಕೆ(machine);
    ಪೂರ್ಣಾಙ್ಕ imsic_bits;

    /* Check socket count limit */
    ಯದಿ (RVSERVER_SOCKETS_MAX < socket_ಎಣಿಕೆ) {
        error_report("number of sockets/nodes should be less than %d",
            RVSERVER_SOCKETS_MAX);
        exit(1);
    }

    ಯದಿ (!rvserver_aclint_allowed()) {
        error_report("'aclint' is only available with TCG acceleration");
        exit(1);
    }

    s->memmap = rvserver_ref_memmap;

    /*
     * We can't set aia_guests with 'qtest' because it'll skip the
     * TCG exclusive init code, setting a blank CPU without RVH
     * (among other things).  If aia_guests is not zero we'll
     * enter riscv_imsic_reset_enter(), which will reset its
     * external irqs via qemu_irq_lower(), and qtest will hit an
     * assert in riscv_cpu_set_irq() because we don't have RVH
     * for handling guest local interrupts.
     *
     * qtest just want to inspect realized devices using QOM and
     * HMP, so do not set aia_guests in that case.
     */
    ಯದಿ (qtest_enabled()) {
        s->aia_guests = 0;
    }

    imsic_bits = imsic_ಅಂಕೆ_bits(s->aia_guests + 1);

    /* Initialize sockets */
    mmio_irqchip = pcie_irqchip = ರಿಕ್ತ;
    ಪ್ರತಿ (i = 0; i < socket_ಎಣಿಕೆ; i++) {
        g_autofree ವರ್ಣ *soc_name = g_strdup_printf("soc%d", i);

        ಯದಿ (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            exit(1);
        }

        base_hartid = riscv_socket_first_hartid(machine, i);
        ಯದಿ (base_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            exit(1);
        }

        hart_ಎಣಿಕೆ = riscv_socket_hart_ಎಣಿಕೆ(machine, i);
        ಯದಿ (hart_ಎಣಿಕೆ < 0) {
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
                                hart_ಎಣಿಕೆ, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Per-socket ACLINT MTIMER */
        riscv_aclint_mtimer_create(system_memory,
                memmap[RVSERVER_ACLINT].base + i * RISCV_ACLINT_DEFAULT_MTIMER_SIZE,
                RISCV_ACLINT_DEFAULT_MTIMER_SIZE,
                base_hartid, hart_ಎಣಿಕೆ,
                RISCV_ACLINT_DEFAULT_MTIMECMP,
                RISCV_ACLINT_DEFAULT_MTIME,
                RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

        /* Per-socket interrupt controller */
        s->irqchip[i] = riscv_create_aia(system_memory, true, s->aia_guests,
                                         IMSIC_HART_SIZE(0),
                                         IMSIC_HART_SIZE(imsic_bits),
                                         RVSERVER_IRQCHIP_ಅಂಕೆ_SOURCES,
                                         &s->memmap[RVSERVER_APLIC_M],
                                         &s->memmap[RVSERVER_APLIC_S],
                                         &s->memmap[RVSERVER_IMSIC_M],
                                         &s->memmap[RVSERVER_IMSIC_S],
                                         i, base_hartid, hart_ಎಣಿಕೆ,
                                         RVSERVER_IRQCHIP_ಅಂಕೆ_MSIS,
                                         RVSERVER_IRQCHIP_ಅಂಕೆ_PRIO_BITS);

        /* Try to use different IRQCHIP instance based device type */
        ಯದಿ (i == 0) {
            mmio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ಯದಿ (i == 1) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    /* register system main memory (actual RAM) */
    memory_region_add_subregion(system_memory, memmap[RVSERVER_DRAM].base,
        machine->ram);

    /* boot rom */
    memory_region_init_rom(mask_rom, ರಿಕ್ತ, "riscv_rvserver_ref_board.mrom",
                           memmap[RVSERVER_MROM].size, &error_fatal);
    memory_region_add_subregion(system_memory, memmap[RVSERVER_MROM].base,
                                mask_rom);

    memory_region_init_io(reset_syscon_io, ರಿಕ್ತ, &rvserver_reset_syscon_ops,
                          ರಿಕ್ತ, "reset_syscon_io",
                          memmap[RVSERVER_RESET_SYSCON].size);
    memory_region_add_subregion(system_memory,
                                memmap[RVSERVER_RESET_SYSCON].base,
                                reset_syscon_io);

    riscv_gpex_pcie_init(system_memory, pcie_irqchip,
                         &rvserver_ref_memmap[RVSERVER_PCIE_ECAM],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO_ಮೇಲಿನ],
                         &rvserver_ref_memmap[RVSERVER_PCIE_PIO],
                         RVSERVER_PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
                                                    &s->memmap[RVSERVER_PLATFORM_BUS],
                                                    RVSERVER_PLATFORM_BUS_IRQ,
                                                    RVSERVER_PLATFORM_BUS_ಅಂಕೆ_IRQS);

    serial_mm_init(system_memory, memmap[RVSERVER_UART0].base,
        0, qdev_get_gpio_in(mmio_irqchip, RVSERVER_UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    sysbus_create_simple("goldfish_rtc", memmap[RVSERVER_RTC].base,
        qdev_get_gpio_in(mmio_irqchip, RVSERVER_RTC_IRQ));

    ಪ್ರತಿ (i = 0; i < ARRAY_SIZE(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    rvserver_flash_maps(s, system_memory);

    /* load/create device tree */
    ಯದಿ (machine->dtb) {
        machine->fdt = load_device_ವೃಕ್ಷ(machine->dtb, &s->fdt_size);
        ಯದಿ (!machine->fdt) {
            error_report("load_device_tree() failed");
            exit(1);
        }
    } ಅನ್ಯಥಾ {
        create_fdt(s, memmap);
    }

    riscv_create_iommu_sys(mmio_irqchip, s->memmap[RVSERVER_IOMMU_SYS].base,
                           IOMMU_SYS_IRQ, false);

    s->machine_done.notify = rvserver_ref_machine_done;
    qemu_add_machine_init_done_notifier(&s->machine_done);
}

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_ref_machine_instance_init(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->flash[0] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash0",
                                     "pflash0", RVSERVER_FLASH_SECTOR_SIZE);
    s->flash[1] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash1",
                                     "pflash1", RVSERVER_FLASH_SECTOR_SIZE);

    s->aia_guests = RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS;
}

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_ref_machine_instance_finalize(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ARRAY_SIZE(s->flash); i++) {
        ಯದಿ (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
}

ಸ್ಥಿರ ವರ್ಣ *rvserver_ref_get_aia_guests(Object *obj, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);
    ವರ್ಣ val[32];

    sprintf(val, "%d", s->aia_guests);
    ಪ್ರತ್ಯಾಗಮ g_strdup(val);
}

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_ref_set_aia_guests(Object *obj, ಧ್ರುವ ವರ್ಣ *val,
                                        Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->aia_guests = atoi(val);
    ಯದಿ (s->aia_guests < 0 || s->aia_guests > RVSERVER_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          RVSERVER_IRQCHIP_MAX_GUESTS);
    }
}

ಸ್ಥಿರ ಧ್ರುವ HotplugHandler *
rvserver_machine_get_hotplug_handler(MachineState *ms, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);

    ಯದಿ (device_is_dynamic_sysbus(mc, dev)) {
        ಪ್ರತ್ಯಾಗಮ HOTPLUG_HANDLER(ms);
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_machine_device_plug_cb(ಧ್ರುವ HotplugHandler *hotplug_dev,
                                            DeviceState *dev, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(hotplug_dev);

    ಯದಿ (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        ಯದಿ (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_ref_machine_class_init(ObjectClass *oc, ಧ್ರುವ ಶೂನ್ಯ *data)
{
    ವರ್ಣ str[128];
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ valid_cpu_types[] = {
        TYPE_RISCV_CPU_RVSERVER_REF,
        ರಿಕ್ತ
    };

    /* Note: this is used as /model in the FDT.  */
    mc->desc = "RISC-V Server Platform Reference Board v1.0";
    mc->init = rvserver_ref_machine_init;
    mc->max_cpus = RVSERVER_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_RVSERVER_REF;
    mc->valid_cpu_types = valid_cpu_types;
    mc->pci_allow_0_address = true;
    mc->default_nic = "e1000e";
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_index_to_instance_props = riscv_numa_cpu_index_to_props;
    mc->get_default_cpu_ಗಂಟು_id = riscv_numa_get_default_cpu_ಗಂಟು_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_rvserver_ref_board.ram";

    object_class_property_add_str(oc, "aia-guests",
                                  rvserver_ref_get_aia_guests,
                                  rvserver_ref_set_aia_guests);
    sprintf(str, "Set number of guest MMIO pages for AIA IMSIC. Valid value "
                 "should be between 0 and %d.", RVSERVER_IRQCHIP_MAX_GUESTS);
    object_class_property_set_description(oc, "aia-guests", str);

    ಪ್ರತಿಜ್ಞಾ(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = rvserver_machine_get_hotplug_handler;
    hc->plug = rvserver_machine_device_plug_cb;
#ifdef CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#endif
}

ಸ್ಥಿರ ಧ್ರುವ TypeInfo rvserver_ref_typeinfo = {
    .name       = TYPE_RISCV_SERVER_REF_MACHINE,
    .parent     = TYPE_MACHINE,
    .class_init = rvserver_ref_machine_class_init,
    .instance_init = rvserver_ref_machine_instance_init,
    .instance_finalize = rvserver_ref_machine_instance_finalize,
    .instance_size = ಪರಿಮಾಣ(RISCVServerRefMachineState),
    .interfaces = (ಧ್ರುವ InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

ಸ್ಥಿರ ಶೂನ್ಯ rvserver_ref_init_register_types(ಶೂನ್ಯ)
{
    type_register_static(&rvserver_ref_typeinfo);
}

type_init(rvserver_ref_init_register_types)
