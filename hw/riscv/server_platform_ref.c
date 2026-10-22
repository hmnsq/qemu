/*
 * QEMU RISC-V Server Platform Reference Board (riscv-server-ref)
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#অন্তর্ভাব "qemu/osdep.h"
#অন্তর্ভাব "qemu/units.h"
#অন্তর্ভাব "qemu/error-report.h"
#অন্তর্ভাব "qemu/guest-random.h"
#অন্তর্ভাব "qapi/error.h"
#অন্তর্ভাব "qapi/qapi-visit-common.h"
#অন্তর্ভাব "hw/core/boards.h"
#অন্তর্ভাব "hw/core/platform-bus.h"
#অন্তর্ভাব "hw/core/loader.h"
#অন্তর্ভাব "hw/core/sysbus.h"
#অন্তর্ভাব "hw/core/qdev-properties.h"
#অন্তর্ভাব "hw/char/serial.h"
#অন্তর্ভাব "hw/block/flash.h"
#অন্তর্ভাব "hw/ide/pci.h"
#অন্তর্ভাব "hw/ide/ahci-pci.h"
#অন্তর্ভাব "hw/pci/pci.h"
#অন্তর্ভাব "hw/pci-host/gpex.h"
#অন্তর্ভাব "hw/core/sysbus-fdt.h"
#অন্তর্ভাব "hw/riscv/riscv_hart.h"
#অন্তর্ভাব "hw/riscv/boot.h"
#অন্তর্ভাব "hw/riscv/device-common.h"
#অন্তর্ভাব "hw/riscv/fdt-common.h"
#অন্তর্ভাব "hw/riscv/numa.h"
#অন্তর্ভাব "hw/riscv/iommu.h"
#অন্তর্ভাব "hw/riscv/riscv-iommu.h"
#অন্তর্ভাব "hw/riscv/riscv-iommu-bits.h"
#অন্তর্ভাব "hw/intc/riscv_aclint.h"
#অন্তর্ভাব "hw/intc/riscv_aplic.h"
#অন্তর্ভাব "hw/intc/riscv_imsic.h"
#অন্তর্ভাব "chardev/char.h"
#অন্তর্ভাব "hw/char/serial-mm.h"
#অন্তর্ভাব "system/device_tree.h"
#অন্তর্ভাব "system/runstate.h"
#অন্তর্ভাব "system/system.h"
#অন্তর্ভাব "system/kvm.h"
#অন্তর্ভাব "system/qtest.h"
#অন্তর্ভাব "system/tcg.h"
#অন্তর্ভাব "kvm/kvm_riscv.h"
#অন্তর্ভাব "system/tpm.h"
#অন্তর্ভাব "target/riscv/cpu.h"
#অন্তর্ভাব "net/net.h"

#অন্তর্ভাব "aia.h"

#define RVSERVER_ACLINT_TIMEBASE_FREQ          1000000000 /* 1Ghz */
#define RVSERVER_CPUS_MAX_BITS                 9
#define RVSERVER_CPUS_MAX                      (1 << RVSERVER_CPUS_MAX_BITS)
#define RVSERVER_IRQCHIP_অঙ্ক_MSIS              255
#define RVSERVER_IRQCHIP_অঙ্ক_SOURCES           96
#define RVSERVER_IRQCHIP_অঙ্ক_PRIO_BITS         3
#define RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS    5
#define RVSERVER_IRQCHIP_MAX_GUESTS_BITS       3
#define RVSERVER_IRQCHIP_MAX_GUESTS \
        ((1U << RVSERVER_IRQCHIP_MAX_GUESTS_BITS) - 1U)
#define RVSERVER_SOCKETS_MAX_BITS              2
#define RVSERVER_SOCKETS_MAX                   (1 << RVSERVER_SOCKETS_MAX_BITS)

#define SYSCON_RESET                           0x1
#define SYSCON_POWEROFF                        0x2

#define RVSERVER_PLATFORM_BUS_অঙ্ক_IRQS         8

#define TYPE_RISCV_SERVER_REF_MACHINE MACHINE_TYPE_NAME("riscv-server-ref")
OBJECT_DECLARE_SIMPLE_TYPE(RISCVServerRefMachineState, RISCV_SERVER_REF_MACHINE)

কাঠামো RISCVServerRefMachineState {
    /*< private >*/
    MachineState parent;

    /*< public >*/
    Notifier machine_done;
    RISCVHartArrayState soc[RVSERVER_SOCKETS_MAX];
    DeviceState *irqchip[RVSERVER_SOCKETS_MAX];
    PFlashCFI01 *flash[2];

    পূর্ণসংখ্যা fdt_আকার;
    পূর্ণসংখ্যা aia_guests;
    ধ্রুবক MemMapEntry *memmap;

    DeviceState *platform_bus_dev;
};

গণনা {
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
    RVSERVER_PCIE_MMIO_উপরের
};

গণনা {
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

#define RVSERVER_IMSIC_GROUP_MAX_আকার      (1U << IMSIC_MMIO_GROUP_MIN_SHIFT)
#যদি RVSERVER_IMSIC_GROUP_MAX_আকার < \
    IMSIC_GROUP_আকার(RVSERVER_CPUS_MAX_BITS, RVSERVER_IRQCHIP_MAX_GUESTS_BITS)
#error "Can't accommodate single IMSIC group in address space"
#endif

#define RVSERVER_IMSIC_MAX_আকার \
    (RVSERVER_SOCKETS_MAX * RVSERVER_IMSIC_GROUP_MAX_আকার)
#যদি 0x4000000 < RVSERVER_IMSIC_MAX_আকার
#error "Can't accommodate all IMSIC groups in address space"
#endif

স্থির ধ্রুবক MemMapEntry rvserver_ref_memmap[] = {
    [RVSERVER_MROM] =           {     0x1000,        0xf000 },
    [RVSERVER_RESET_SYSCON] =   {   0x100000,        0x1000 },
    [RVSERVER_RTC] =            {   0x101000,        0x1000 },
    [RVSERVER_IOMMU_SYS] =      {   0x102000,        0x1000 },
    [RVSERVER_ACLINT] =         {  0x2000000,       0x10000 },
    [RVSERVER_PCIE_PIO] =       {  0x3000000,       0x10000 },
    [RVSERVER_PLATFORM_BUS] =   {  0x4000000,     0x2000000 },
    [RVSERVER_APLIC_M] =        {  0xc000000, APLIC_আকার(RVSERVER_CPUS_MAX) },
    [RVSERVER_APLIC_S] =        {  0xd000000, APLIC_আকার(RVSERVER_CPUS_MAX) },
    [RVSERVER_UART0] =          { 0x10000000,         0x100 },
    [RVSERVER_FLASH] =          { 0x20000000,     0x4000000 },
    [RVSERVER_IMSIC_M] =        { 0x24000000, RVSERVER_IMSIC_MAX_আকার },
    [RVSERVER_IMSIC_S] =        { 0x28000000, RVSERVER_IMSIC_MAX_আকার },
    [RVSERVER_PCIE_ECAM] =      { 0x30000000,    0x10000000 },
    [RVSERVER_PCIE_MMIO] =      { 0x40000000,    0x40000000 },
    [RVSERVER_DRAM] =           { 0x80000000, 0xff80000000ull },
    [RVSERVER_PCIE_MMIO_উপরের] = { 0x10000000000ull, 0x10000000000ull },
};

#define RVSERVER_FLASH_SECTOR_আকার (256 * KiB)

স্থির শূন্য rvserver_flash_maps(RISCVServerRefMachineState *s,
                                MemoryRegion *sysmem)
{
    hwaddr flashsize = rvserver_ref_memmap[RVSERVER_FLASH].আকার_2 / 2;
    hwaddr flashbase = rvserver_ref_memmap[RVSERVER_FLASH].base;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_আকার);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_আকার);
}

স্থির শূন্য create_fdt_pmu(RISCVServerRefMachineState *s)
{
    g_autofree অক্ষর *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_add_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_গ্রন্থি(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

স্থির শূন্য create_fdt_sockets(RISCVServerRefMachineState *s,
                               ধ্রুবক MemMapEntry *memmap,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
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
    জন্য (socket = (socket_গুনতি - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[RVSERVER_DRAM].base +
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

        aclint_props.socket = socket;
        aclint_props.অঙ্ক_harts = s->soc[socket].অঙ্ক_harts;
        riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                       &intc_phandles[phandle_pos]);
    }

    imsic_fdt_props.soc = &s->soc;
    imsic_fdt_props.socket_গুনতি = socket_গুনতি;
    imsic_fdt_props.smp_cpus = ms->smp.cpus;
    imsic_fdt_props.imsic_m_base = memmap[RVSERVER_IMSIC_M].base;
    imsic_fdt_props.imsic_s_base = memmap[RVSERVER_IMSIC_S].base;
    imsic_fdt_props.imsic_group_max_আকার = RVSERVER_IMSIC_GROUP_MAX_আকার;
    imsic_fdt_props.irqchip_অঙ্ক_msis = RVSERVER_IRQCHIP_অঙ্ক_MSIS;
    imsic_fdt_props.aia_guests = s->aia_guests;

    riscv_create_fdt_imsic(ms->fdt, &imsic_fdt_props, phandle, intc_phandles,
                           &msi_m_phandle, &msi_s_phandle);
    *msi_pcie_phandle = msi_s_phandle;

    aplic_fdt_props.aplic_m = !kvm_enabled()
                              ? &memmap[RVSERVER_APLIC_M] : ফাঁকা;
    aplic_fdt_props.aplic_s = &memmap[RVSERVER_APLIC_S];
    aplic_fdt_props.platform_bus = &memmap[RVSERVER_PLATFORM_BUS];
    aplic_fdt_props.platform_bus_irq = RVSERVER_PLATFORM_BUS_IRQ;
    aplic_fdt_props.অঙ্ক_harts = ms->smp.cpus;
    aplic_fdt_props.numa_enabled = numa_enabled;
    aplic_fdt_props.irqchip_অঙ্ক_sources = RVSERVER_IRQCHIP_অঙ্ক_SOURCES;
    aplic_fdt_props.aia_type = AIA_TYPE_APLIC_IMSIC;

    phandle_pos = ms->smp.cpus;
    জন্য (socket = (socket_গুনতি - 1); socket >= 0; socket--) {
        phandle_pos -= s->soc[socket].অঙ্ক_harts;

        aplic_fdt_props.socket = socket;
        aplic_fdt_props.অঙ্ক_harts = s->soc[socket].অঙ্ক_harts;
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_fdt_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[phandle_pos],
                                      xplic_phandles);
    }

    জন্য (socket = 0; socket < socket_গুনতি; socket++) {
        যদি (socket == 0) {
            *irq_mmio_phandle = xplic_phandles[socket];
            *irq_pcie_phandle = xplic_phandles[socket];
        }
        যদি (socket == 1) {
            *irq_pcie_phandle = xplic_phandles[socket];
        }
    }

    riscv_socket_fdt_write_distance_matrix(ms);
}

স্থির শূন্য finalize_fdt(RISCVServerRefMachineState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, iommu_sys_phandle;
    g_autofree অক্ষর *name = ফাঁকা;
    MachineState *ms = MACHINE(s);

    create_fdt_sockets(s, rvserver_ref_memmap, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &msi_pcie_phandle);

    iommu_sys_phandle = riscv_create_fdt_riscv_iommu_sys(ms->fdt,
                                         s->memmap[RVSERVER_IOMMU_SYS].base,
                                         s->memmap[RVSERVER_IOMMU_SYS].আকার_2,
                                         &phandle, irq_mmio_phandle,
                                         msi_pcie_phandle, IOMMU_SYS_IRQ);

    riscv_create_fdt_pcie(ms->fdt, AIA_TYPE_APLIC_IMSIC, true,
                          &s->memmap[RVSERVER_PCIE_ECAM],
                          &s->memmap[RVSERVER_PCIE_PIO],
                          &s->memmap[RVSERVER_PCIE_MMIO],
                          &s->memmap[RVSERVER_PCIE_MMIO_উপরের],
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, RVSERVER_PCIE_IRQ);

    riscv_create_fdt_syscon(ms->fdt, &phandle,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].base,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].আকার_2,
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

স্থির শূন্য create_fdt(RISCVServerRefMachineState *s,
                       ধ্রুবক MemMapEntry *memmap)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_seed[32];
    g_autofree অক্ষর *name = ফাঁকা;

    ms->fdt = riscv_create_board_device_বৃক্ষ(MACHINE_GET_CLASS(s)->desc,
                                             "qemu,riscv-server-ref",
                                             &s->fdt_আকার);

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
    qemu_guest_getrandom_nofail(rng_seed, আকার(rng_seed));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_seed, আকার(rng_seed));

    riscv_create_fdt_flash(ms->fdt, rvserver_ref_memmap[RVSERVER_FLASH].base,
                           rvserver_ref_memmap[RVSERVER_FLASH].আকার_2 / 2);
    create_fdt_pmu(s);

}

স্থির uint64_t rvserver_reset_syscon_read(শূন্য *opaque, hwaddr addr,
                                           unsigned আকার_2)
{
    ফেরত 0;
}

স্থির শূন্য rvserver_reset_syscon_write(শূন্য *opaque, hwaddr addr,
                                        uint64_t val64, unsigned পূর্ণসংখ্যা আকার_2)
{
    নির্বাচন (val64) {
    ক্ষেত্র SYSCON_POWEROFF:
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_GUEST_SHUTDOWN);
        ফেরত;
    ক্ষেত্র SYSCON_RESET:
        qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        ফেরত;
    পূর্বনির্ধারিত:
        বিরতি;
    }
}

স্থির ধ্রুবক MemoryRegionOps rvserver_reset_syscon_ops = {
    .পঠ = rvserver_reset_syscon_read,
    .লিখ = rvserver_reset_syscon_write,
    .endianness = DEVICE_LITTLE_ENDIAN
};

স্থির শূন্য rvserver_ref_machine_done(Notifier *notifier, শূন্য *তথ্য)
{
    RISCVServerRefMachineState *s = container_of(
        notifier, RISCVServerRefMachineState, machine_done);
    ধ্রুবক MemMapEntry *memmap = rvserver_ref_memmap;
    MachineState *machine = MACHINE(s);
    hwaddr start_addr = memmap[RVSERVER_DRAM].base;
    target_ulong firmware_end_addr, kernel_start_addr;
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

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_end_addr = riscv_find_and_load_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &start_addr, ফাঁকা);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    যদি (pflash_blk0) {
        যদি (machine->firmware && !strcmp(machine->firmware, "none")) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            start_addr = rvserver_ref_memmap[RVSERVER_FLASH].base;
        } অন্যথা {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_setup_firmware_boot(machine);
            kernel_entry = rvserver_ref_memmap[RVSERVER_FLASH].base;
        }
    }

    যদি (machine->kernel_filename && !kernel_entry) {
        kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                         firmware_end_addr);
        riscv_load_kernel(machine, &boot_info, kernel_start_addr, true, ফাঁকা);
        kernel_entry = boot_info.image_নিচের_addr;
    }

    fdt_load_addr = riscv_compute_fdt_addr(memmap[RVSERVER_DRAM].base,
                                           memmap[RVSERVER_DRAM].আকার_2,
                                           machine, &boot_info);

    riscv_load_fdt(fdt_load_addr, machine->fdt);

    /* load the reset vector */
    riscv_setup_rom_reset_vec(machine, &s->soc[0], start_addr,
                              rvserver_ref_memmap[RVSERVER_MROM].base,
                              rvserver_ref_memmap[RVSERVER_MROM].আকার_2,
                              kernel_entry,
                              fdt_load_addr);
}

স্থির bool rvserver_aclint_allowed(শূন্য)
{
    ফেরত tcg_enabled() || qtest_enabled();
}

স্থির শূন্য rvserver_ref_machine_init(MachineState *machine)
{
    ধ্রুবক MemMapEntry *memmap = rvserver_ref_memmap;
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *mask_rom = g_new(MemoryRegion, 1);
    MemoryRegion *reset_syscon_io = g_new(MemoryRegion, 1);
    DeviceState *mmio_irqchip, *pcie_irqchip;
    পূর্ণসংখ্যা i, base_hartid, hart_গুনতি;
    পূর্ণসংখ্যা socket_গুনতি = riscv_socket_গুনতি(machine);
    পূর্ণসংখ্যা imsic_bits;

    /* Check socket count limit */
    যদি (RVSERVER_SOCKETS_MAX < socket_গুনতি) {
        error_report("number of sockets/nodes should be less than %d",
            RVSERVER_SOCKETS_MAX);
        exit(1);
    }

    যদি (!rvserver_aclint_allowed()) {
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
    যদি (qtest_enabled()) {
        s->aia_guests = 0;
    }

    imsic_bits = imsic_অঙ্ক_bits(s->aia_guests + 1);

    /* Initialize sockets */
    mmio_irqchip = pcie_irqchip = ফাঁকা;
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

        /* Per-socket ACLINT MTIMER */
        riscv_aclint_mtimer_create(system_memory,
                memmap[RVSERVER_ACLINT].base + i * RISCV_ACLINT_DEFAULT_MTIMER_আকার,
                RISCV_ACLINT_DEFAULT_MTIMER_আকার,
                base_hartid, hart_গুনতি,
                RISCV_ACLINT_DEFAULT_MTIMECMP,
                RISCV_ACLINT_DEFAULT_MTIME,
                RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

        /* Per-socket interrupt controller */
        s->irqchip[i] = riscv_create_aia(system_memory, true, s->aia_guests,
                                         IMSIC_HART_আকার(0),
                                         IMSIC_HART_আকার(imsic_bits),
                                         RVSERVER_IRQCHIP_অঙ্ক_SOURCES,
                                         &s->memmap[RVSERVER_APLIC_M],
                                         &s->memmap[RVSERVER_APLIC_S],
                                         &s->memmap[RVSERVER_IMSIC_M],
                                         &s->memmap[RVSERVER_IMSIC_S],
                                         i, base_hartid, hart_গুনতি,
                                         RVSERVER_IRQCHIP_অঙ্ক_MSIS,
                                         RVSERVER_IRQCHIP_অঙ্ক_PRIO_BITS);

        /* Try to use different IRQCHIP instance based device type */
        যদি (i == 0) {
            mmio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        যদি (i == 1) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    /* register system main memory (actual RAM) */
    memory_region_add_subregion(system_memory, memmap[RVSERVER_DRAM].base,
        machine->ram);

    /* boot rom */
    memory_region_init_rom(mask_rom, ফাঁকা, "riscv_rvserver_ref_board.mrom",
                           memmap[RVSERVER_MROM].আকার_2, &error_fatal);
    memory_region_add_subregion(system_memory, memmap[RVSERVER_MROM].base,
                                mask_rom);

    memory_region_init_io(reset_syscon_io, ফাঁকা, &rvserver_reset_syscon_ops,
                          ফাঁকা, "reset_syscon_io",
                          memmap[RVSERVER_RESET_SYSCON].আকার_2);
    memory_region_add_subregion(system_memory,
                                memmap[RVSERVER_RESET_SYSCON].base,
                                reset_syscon_io);

    riscv_gpex_pcie_init(system_memory, pcie_irqchip,
                         &rvserver_ref_memmap[RVSERVER_PCIE_ECAM],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO_উপরের],
                         &rvserver_ref_memmap[RVSERVER_PCIE_PIO],
                         RVSERVER_PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
                                                    &s->memmap[RVSERVER_PLATFORM_BUS],
                                                    RVSERVER_PLATFORM_BUS_IRQ,
                                                    RVSERVER_PLATFORM_BUS_অঙ্ক_IRQS);

    serial_mm_init(system_memory, memmap[RVSERVER_UART0].base,
        0, qdev_get_gpio_in(mmio_irqchip, RVSERVER_UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    sysbus_create_simple("goldfish_rtc", memmap[RVSERVER_RTC].base,
        qdev_get_gpio_in(mmio_irqchip, RVSERVER_RTC_IRQ));

    জন্য (i = 0; i < ARRAY_আকার(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    rvserver_flash_maps(s, system_memory);

    /* load/create device tree */
    যদি (machine->dtb) {
        machine->fdt = load_device_বৃক্ষ(machine->dtb, &s->fdt_আকার);
        যদি (!machine->fdt) {
            error_report("load_device_tree() failed");
            exit(1);
        }
    } অন্যথা {
        create_fdt(s, memmap);
    }

    riscv_create_iommu_sys(mmio_irqchip, s->memmap[RVSERVER_IOMMU_SYS].base,
                           IOMMU_SYS_IRQ, false);

    s->machine_done.notify = rvserver_ref_machine_done;
    qemu_add_machine_init_done_notifier(&s->machine_done);
}

স্থির শূন্য rvserver_ref_machine_instance_init(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->flash[0] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash0",
                                     "pflash0", RVSERVER_FLASH_SECTOR_আকার);
    s->flash[1] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash1",
                                     "pflash1", RVSERVER_FLASH_SECTOR_আকার);

    s->aia_guests = RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS;
}

স্থির শূন্য rvserver_ref_machine_instance_finalize(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    জন্য (পূর্ণসংখ্যা i = 0; i < ARRAY_আকার(s->flash); i++) {
        যদি (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
}

স্থির অক্ষর *rvserver_ref_get_aia_guests(Object *obj, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);
    অক্ষর val[32];

    sprintf(val, "%d", s->aia_guests);
    ফেরত g_strdup(val);
}

স্থির শূন্য rvserver_ref_set_aia_guests(Object *obj, ধ্রুবক অক্ষর *val,
                                        Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->aia_guests = atoi(val);
    যদি (s->aia_guests < 0 || s->aia_guests > RVSERVER_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          RVSERVER_IRQCHIP_MAX_GUESTS);
    }
}

স্থির ধ্রুবক HotplugHandler *
rvserver_machine_get_hotplug_handler(MachineState *ms, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);

    যদি (device_is_dynamic_sysbus(mc, dev)) {
        ফেরত HOTPLUG_HANDLER(ms);
    }

    ফেরত ফাঁকা;
}

স্থির শূন্য rvserver_machine_device_plug_cb(ধ্রুবক HotplugHandler *hotplug_dev,
                                            DeviceState *dev, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(hotplug_dev);

    যদি (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        যদি (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }
}

স্থির শূন্য rvserver_ref_machine_class_init(ObjectClass *oc, ধ্রুবক শূন্য *তথ্য)
{
    অক্ষর str[128];
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);
    স্থির ধ্রুবক অক্ষর * ধ্রুবক valid_cpu_types[] = {
        TYPE_RISCV_CPU_RVSERVER_REF,
        ফাঁকা
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
    mc->cpu_সূচক_to_instance_props = riscv_numa_cpu_সূচক_to_props;
    mc->get_default_cpu_গ্রন্থি_id = riscv_numa_get_default_cpu_গ্রন্থি_id;
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

    প্রতিজ্ঞা(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = rvserver_machine_get_hotplug_handler;
    hc->plug = rvserver_machine_device_plug_cb;
#ifdef CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#endif
}

স্থির ধ্রুবক TypeInfo rvserver_ref_typeinfo = {
    .name       = TYPE_RISCV_SERVER_REF_MACHINE,
    .parent     = TYPE_MACHINE,
    .class_init = rvserver_ref_machine_class_init,
    .instance_init = rvserver_ref_machine_instance_init,
    .instance_finalize = rvserver_ref_machine_instance_finalize,
    .instance_আকার = আকার(RISCVServerRefMachineState),
    .interfaces = (ধ্রুবক InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

স্থির শূন্য rvserver_ref_init_register_types(শূন্য)
{
    type_register_static(&rvserver_ref_typeinfo);
}

type_init(rvserver_ref_init_register_types)
