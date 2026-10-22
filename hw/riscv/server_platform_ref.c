/*
 * QEMU RISC-V Server Platform Reference Board (riscv-server-ref)
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن "qemu/osdep.h"
#ضمن "qemu/units.h"
#ضمن "qemu/error-report.h"
#ضمن "qemu/guest-random.h"
#ضمن "qapi/error.h"
#ضمن "qapi/qapi-visit-common.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/platform-bus.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/core/sysbus.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/char/serial.h"
#ضمن "hw/block/flash.h"
#ضمن "hw/ide/pci.h"
#ضمن "hw/ide/ahci-pci.h"
#ضمن "hw/pci/pci.h"
#ضمن "hw/pci-host/gpex.h"
#ضمن "hw/core/sysbus-fdt.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/riscv/device-common.h"
#ضمن "hw/riscv/fdt-common.h"
#ضمن "hw/riscv/numa.h"
#ضمن "hw/riscv/iommu.h"
#ضمن "hw/riscv/riscv-iommu.h"
#ضمن "hw/riscv/riscv-iommu-bits.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/riscv_aplic.h"
#ضمن "hw/intc/riscv_imsic.h"
#ضمن "chardev/char.h"
#ضمن "hw/char/serial-mm.h"
#ضمن "system/device_tree.h"
#ضمن "system/runstate.h"
#ضمن "system/system.h"
#ضمن "system/kvm.h"
#ضمن "system/qtest.h"
#ضمن "system/tcg.h"
#ضمن "kvm/kvm_riscv.h"
#ضمن "system/tpm.h"
#ضمن "target/riscv/cpu.h"
#ضمن "net/net.h"

#ضمن "aia.h"

#تعریف_کړه RVSERVER_ACLINT_TIMEBASE_FREQ          1000000000 /* 1Ghz */
#تعریف_کړه RVSERVER_CPUS_MAX_بټونه                 9
#تعریف_کړه RVSERVER_CPUS_MAX                      (1 << RVSERVER_CPUS_MAX_بټونه)
#تعریف_کړه RVSERVER_IRQCHIP_عدد_MSIS              255
#تعریف_کړه RVSERVER_IRQCHIP_عدد_SOURCES           96
#تعریف_کړه RVSERVER_IRQCHIP_عدد_PRIO_بټونه         3
#تعریف_کړه RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS    5
#تعریف_کړه RVSERVER_IRQCHIP_MAX_GUESTS_بټونه       3
#تعریف_کړه RVSERVER_IRQCHIP_MAX_GUESTS \
        ((1U << RVSERVER_IRQCHIP_MAX_GUESTS_بټونه) - 1U)
#تعریف_کړه RVSERVER_SOCKETS_MAX_بټونه              2
#تعریف_کړه RVSERVER_SOCKETS_MAX                   (1 << RVSERVER_SOCKETS_MAX_بټونه)

#تعریف_کړه SYSCON_RESET                           0x1
#تعریف_کړه SYSCON_POWEROFF                        0x2

#تعریف_کړه RVSERVER_PLATFORM_BUS_عدد_IRQS         8

#تعریف_کړه TYPE_RISCV_SERVER_REF_MACHINE MACHINE_TYPE_NAME("riscv-server-ref")
OBJECT_DECLARE_SIMPLE_TYPE(RISCVServerRefMachineState, RISCV_SERVER_REF_MACHINE)

بنية RISCVServerRefMachineState {
    /*< private >*/
    MachineState پلار;

    /*< public >*/
    Notifier machine_بشپړ;
    RISCVHartArrayState soc[RVSERVER_SOCKETS_MAX];
    DeviceState *irqchip[RVSERVER_SOCKETS_MAX];
    PFlashCFI01 *flash[2];

    صحيح fdt_اندازه;
    صحيح aia_guests;
    ثابت Mem_Map_ثبت *memmap;

    DeviceState *platform_bus_dev;
};

تعداد {
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
    RVSERVER_PCIE_MMIO_لوړ
};

تعداد {
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

#تعریف_کړه RVSERVER_IMSIC_ډله_MAX_اندازه      (1U << IMSIC_MMIO_ډله_MIN_ښويدنه)
#إذا RVSERVER_IMSIC_ډله_MAX_اندازه < \
    IMSIC_ډله_اندازه(RVSERVER_CPUS_MAX_بټونه, RVSERVER_IRQCHIP_MAX_GUESTS_بټونه)
#تېروتنه "Can't accommodate single IMSIC group in address space"
#د_که_پای

#تعریف_کړه RVSERVER_IMSIC_MAX_اندازه \
    (RVSERVER_SOCKETS_MAX * RVSERVER_IMSIC_ډله_MAX_اندازه)
#إذا 0x4000000 < RVSERVER_IMSIC_MAX_اندازه
#تېروتنه "Can't accommodate all IMSIC groups in address space"
#د_که_پای

ساكن ثابت Mem_Map_ثبت rvserver_ref_memmap[] = {
    [RVSERVER_MROM] =           {     0x1000,        0xf000 },
    [RVSERVER_RESET_SYSCON] =   {   0x100000,        0x1000 },
    [RVSERVER_RTC] =            {   0x101000,        0x1000 },
    [RVSERVER_IOMMU_SYS] =      {   0x102000,        0x1000 },
    [RVSERVER_ACLINT] =         {  0x2000000,       0x10000 },
    [RVSERVER_PCIE_PIO] =       {  0x3000000,       0x10000 },
    [RVSERVER_PLATFORM_BUS] =   {  0x4000000,     0x2000000 },
    [RVSERVER_APLIC_M] =        {  0xc000000, APLIC_اندازه(RVSERVER_CPUS_MAX) },
    [RVSERVER_APLIC_S] =        {  0xd000000, APLIC_اندازه(RVSERVER_CPUS_MAX) },
    [RVSERVER_UART0] =          { 0x10000000,         0x100 },
    [RVSERVER_FLASH] =          { 0x20000000,     0x4000000 },
    [RVSERVER_IMSIC_M] =        { 0x24000000, RVSERVER_IMSIC_MAX_اندازه },
    [RVSERVER_IMSIC_S] =        { 0x28000000, RVSERVER_IMSIC_MAX_اندازه },
    [RVSERVER_PCIE_ECAM] =      { 0x30000000,    0x10000000 },
    [RVSERVER_PCIE_MMIO] =      { 0x40000000,    0x40000000 },
    [RVSERVER_DRAM] =           { 0x80000000, 0xff80000000ull },
    [RVSERVER_PCIE_MMIO_لوړ] = { 0x10000000000ull, 0x10000000000ull },
};

#تعریف_کړه RVSERVER_FLASH_SECTOR_اندازه (256 * KiB)

ساكن فراغ rvserver_flash_maps(RISCVServerRefMachineState *s,
                                حافظه_Region *sysmem)
{
    hwaddr flashsize = rvserver_ref_memmap[RVSERVER_FLASH].اندازه / 2;
    hwaddr flashbase = rvserver_ref_memmap[RVSERVER_FLASH].بنسټ;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_اندازه);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_اندازه);
}

ساكن فراغ create_fdt_pmu(RISCVServerRefMachineState *s)
{
    g_autofree حرف *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_جمع_کړه_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_غوټه(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

ساكن فراغ create_fdt_sockets(RISCVServerRefMachineState *s,
                               ثابت Mem_Map_ثبت *memmap,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
                               uint32_t *msi_pcie_phandle)
{
    صحيح socket, phandle_pos;
    MachineState *ms = MACHINE(s);
    uint32_t msi_m_phandle = 0, msi_s_phandle = 0;
    uint32_t xplic_phandles[MAX_NODES];
    g_autofree uint32_t *intc_phandles = عدم;
    صحيح socket_شمېر = riscv_socket_شمېر(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_بټ = riscv_is_32bit(&s->soc[0]);
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
    لكل (socket = (socket_شمېر - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[RVSERVER_DRAM].بنسټ +
                         riscv_socket_mem_دننه_واټن(ms, socket);
        uint64_t memsize = riscv_socket_mem_اندازه(ms, socket);

        phandle_pos -= s->soc[socket].عدد_harts;

        riscv_create_fdt_socket_cpus(ms->fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].عدد_harts,
                                     s->soc[socket].hartid_بنسټ,
                                     phandle, &intc_phandles[phandle_pos],
                                     numa_enabled, is_32_بټ);

        riscv_create_fdt_socket_حافظه(ms->fdt, memaddr, memsize,
                                       socket, riscv_numa_enabled(ms));

        aclint_props.socket = socket;
        aclint_props.عدد_harts = s->soc[socket].عدد_harts;
        riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                       &intc_phandles[phandle_pos]);
    }

    imsic_fdt_props.soc = &s->soc;
    imsic_fdt_props.socket_شمېر = socket_شمېر;
    imsic_fdt_props.smp_cpus = ms->smp.cpus;
    imsic_fdt_props.imsic_m_بنسټ = memmap[RVSERVER_IMSIC_M].بنسټ;
    imsic_fdt_props.imsic_s_بنسټ = memmap[RVSERVER_IMSIC_S].بنسټ;
    imsic_fdt_props.imsic_ډله_max_اندازه = RVSERVER_IMSIC_ډله_MAX_اندازه;
    imsic_fdt_props.irqchip_عدد_msis = RVSERVER_IRQCHIP_عدد_MSIS;
    imsic_fdt_props.aia_guests = s->aia_guests;

    riscv_create_fdt_imsic(ms->fdt, &imsic_fdt_props, phandle, intc_phandles,
                           &msi_m_phandle, &msi_s_phandle);
    *msi_pcie_phandle = msi_s_phandle;

    aplic_fdt_props.aplic_m = !kvm_enabled()
                              ? &memmap[RVSERVER_APLIC_M] : عدم;
    aplic_fdt_props.aplic_s = &memmap[RVSERVER_APLIC_S];
    aplic_fdt_props.platform_bus = &memmap[RVSERVER_PLATFORM_BUS];
    aplic_fdt_props.platform_bus_irq = RVSERVER_PLATFORM_BUS_IRQ;
    aplic_fdt_props.عدد_harts = ms->smp.cpus;
    aplic_fdt_props.numa_enabled = numa_enabled;
    aplic_fdt_props.irqchip_عدد_sources = RVSERVER_IRQCHIP_عدد_SOURCES;
    aplic_fdt_props.aia_type = AIA_TYPE_APLIC_IMSIC;

    phandle_pos = ms->smp.cpus;
    لكل (socket = (socket_شمېر - 1); socket >= 0; socket--) {
        phandle_pos -= s->soc[socket].عدد_harts;

        aplic_fdt_props.socket = socket;
        aplic_fdt_props.عدد_harts = s->soc[socket].عدد_harts;
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_fdt_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[phandle_pos],
                                      xplic_phandles);
    }

    لكل (socket = 0; socket < socket_شمېر; socket++) {
        إذا (socket == 0) {
            *irq_mmio_phandle = xplic_phandles[socket];
            *irq_pcie_phandle = xplic_phandles[socket];
        }
        إذا (socket == 1) {
            *irq_pcie_phandle = xplic_phandles[socket];
        }
    }

    riscv_socket_fdt_write_واټن_ماتریکس(ms);
}

ساكن فراغ finalize_fdt(RISCVServerRefMachineState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, iommu_sys_phandle;
    g_autofree حرف *name = عدم;
    MachineState *ms = MACHINE(s);

    create_fdt_sockets(s, rvserver_ref_memmap, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &msi_pcie_phandle);

    iommu_sys_phandle = riscv_create_fdt_riscv_iommu_sys(ms->fdt,
                                         s->memmap[RVSERVER_IOMMU_SYS].بنسټ,
                                         s->memmap[RVSERVER_IOMMU_SYS].اندازه,
                                         &phandle, irq_mmio_phandle,
                                         msi_pcie_phandle, IOMMU_SYS_IRQ);

    riscv_create_fdt_pcie(ms->fdt, AIA_TYPE_APLIC_IMSIC, true,
                          &s->memmap[RVSERVER_PCIE_ECAM],
                          &s->memmap[RVSERVER_PCIE_PIO],
                          &s->memmap[RVSERVER_PCIE_MMIO],
                          &s->memmap[RVSERVER_PCIE_MMIO_لوړ],
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, RVSERVER_PCIE_IRQ);

    riscv_create_fdt_syscon(ms->fdt, &phandle,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].بنسټ,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].اندازه,
                            SYSCON_RESET, SYSCON_POWEROFF, false);

    riscv_create_fdt_uart(ms->fdt, &rvserver_ref_memmap[RVSERVER_UART0],
                          RVSERVER_UART0_IRQ, AIA_TYPE_APLIC_IMSIC,
                          irq_mmio_phandle);
    name = riscv_fdt_get_uart_nodename(rvserver_ref_memmap[RVSERVER_UART0].بنسټ);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);

    riscv_create_fdt_rtc(ms->fdt, &rvserver_ref_memmap[RVSERVER_RTC],
                         RVSERVER_RTC_IRQ, AIA_TYPE_APLIC_IMSIC,
                         irq_mmio_phandle);
}

ساكن فراغ create_fdt(RISCVServerRefMachineState *s,
                       ثابت Mem_Map_ثبت *memmap)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_تخم[32];
    g_autofree حرف *name = عدم;

    ms->fdt = riscv_create_board_device_ونه(MACHINE_GET_CLASS(s)->desc,
                                             "qemu,riscv-server-ref",
                                             &s->fdt_اندازه);

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
                           s->memmap[RVSERVER_PCIE_ECAM].بنسټ);
    qemu_fdt_جمع_کړه_subnode(ms->fdt, name);

    qemu_fdt_جمع_کړه_subnode(ms->fdt, "/chosen");
    qemu_fdt_جمع_کړه_subnode(ms->fdt, "/aliases");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_تخم, حجم(rng_تخم));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_تخم, حجم(rng_تخم));

    riscv_create_fdt_flash(ms->fdt, rvserver_ref_memmap[RVSERVER_FLASH].بنسټ,
                           rvserver_ref_memmap[RVSERVER_FLASH].اندازه / 2);
    create_fdt_pmu(s);

}

ساكن uint64_t rvserver_reset_syscon_read(فراغ *opaque, hwaddr addr,
                                           بینښه اندازه)
{
    أرجع 0;
}

ساكن فراغ rvserver_reset_syscon_write(فراغ *opaque, hwaddr addr,
                                        uint64_t val64, بینښه صحيح اندازه)
{
    بدل (val64) {
    حالة SYSCON_POWEROFF:
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_GUEST_SHUTDOWN);
        أرجع;
    حالة SYSCON_RESET:
        qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        أرجع;
    افتراضي:
        اكسر;
    }
}

ساكن ثابت حافظه_Region_Ops rvserver_reset_syscon_ops = {
    .اقرأ = rvserver_reset_syscon_read,
    .اكتب = rvserver_reset_syscon_write,
    .endianness = DEVICE_LITTLE_ENDIAN
};

ساكن فراغ rvserver_ref_machine_بشپړ(Notifier *notifier, فراغ *معلومات)
{
    RISCVServerRefMachineState *s = container_د(
        notifier, RISCVServerRefMachineState, machine_بشپړ);
    ثابت Mem_Map_ثبت *memmap = rvserver_ref_memmap;
    MachineState *machine = MACHINE(s);
    hwaddr پیل_addr = memmap[RVSERVER_DRAM].بنسټ;
    موخه_ulong firmware_پای_ټکی_addr, kernel_پیل_addr;
    ثابت حرف *firmware_name = riscv_default_firmware_name(&s->soc[0]);
    uint64_t fdt_بارول_addr;
    uint64_t kernel_ثبت = 0;
    بلاک_Backend *pflash_blk0;
    RISCVBootInfo boot_info;

    /*
     * An user provided dtb must include everything, including
     * dynamic sysbus devices. Our FDT needs to be finalized.
     */
    إذا (machine->dtb == عدم) {
        finalize_fdt(s);
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_پای_ټکی_addr = riscv_پیدا_کړه_and_بارول_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &پیل_addr, عدم);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    إذا (pflash_blk0) {
        إذا (machine->firmware && !strcmp(machine->firmware, "none")) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            پیل_addr = rvserver_ref_memmap[RVSERVER_FLASH].بنسټ;
        } وإلا {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_تیاری_firmware_boot(machine);
            kernel_ثبت = rvserver_ref_memmap[RVSERVER_FLASH].بنسټ;
        }
    }

    إذا (machine->kernel_filename && !kernel_ثبت) {
        kernel_پیل_addr = riscv_calc_kernel_پیل_addr(&boot_info,
                                                         firmware_پای_ټکی_addr);
        riscv_بارول_kernel(machine, &boot_info, kernel_پیل_addr, true, عدم);
        kernel_ثبت = boot_info.image_ټیټ_addr;
    }

    fdt_بارول_addr = riscv_compute_fdt_addr(memmap[RVSERVER_DRAM].بنسټ,
                                           memmap[RVSERVER_DRAM].اندازه,
                                           machine, &boot_info);

    riscv_بارول_fdt(fdt_بارول_addr, machine->fdt);

    /* load the reset vector */
    riscv_تیاری_rom_reset_vec(machine, &s->soc[0], پیل_addr,
                              rvserver_ref_memmap[RVSERVER_MROM].بنسټ,
                              rvserver_ref_memmap[RVSERVER_MROM].اندازه,
                              kernel_ثبت,
                              fdt_بارول_addr);
}

ساكن bool rvserver_aclint_allowed(فراغ)
{
    أرجع tcg_enabled() || qtest_enabled();
}

ساكن فراغ rvserver_ref_machine_init(MachineState *machine)
{
    ثابت Mem_Map_ثبت *memmap = rvserver_ref_memmap;
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(machine);
    حافظه_Region *system_حافظه = get_system_حافظه();
    حافظه_Region *ماسک_rom = g_new(حافظه_Region, 1);
    حافظه_Region *reset_syscon_io = g_new(حافظه_Region, 1);
    DeviceState *mmio_irqchip, *pcie_irqchip;
    صحيح i, بنسټ_hartid, hart_شمېر;
    صحيح socket_شمېر = riscv_socket_شمېر(machine);
    صحيح imsic_بټونه;

    /* Check socket count limit */
    إذا (RVSERVER_SOCKETS_MAX < socket_شمېر) {
        error_report("number of sockets/nodes should be less than %d",
            RVSERVER_SOCKETS_MAX);
        وتل(1);
    }

    إذا (!rvserver_aclint_allowed()) {
        error_report("'aclint' is only available with TCG acceleration");
        وتل(1);
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
    إذا (qtest_enabled()) {
        s->aia_guests = 0;
    }

    imsic_بټونه = imsic_عدد_بټونه(s->aia_guests + 1);

    /* Initialize sockets */
    mmio_irqchip = pcie_irqchip = عدم;
    لكل (i = 0; i < socket_شمېر; i++) {
        g_autofree حرف *soc_name = g_strdup_printf("soc%d", i);

        إذا (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            وتل(1);
        }

        بنسټ_hartid = riscv_socket_لومړی_hartid(machine, i);
        إذا (بنسټ_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            وتل(1);
        }

        hart_شمېر = riscv_socket_hart_شمېر(machine, i);
        إذا (hart_شمېر < 0) {
            error_report("can't find hart count for socket%d", i);
            وتل(1);
        }

        object_initialize_ماشوم(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        object_property_سېټ_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_سېټ_int(OBJECT(&s->soc[i]), "hartid-base",
                                بنسټ_hartid, &error_abort);
        object_property_سېټ_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_شمېر, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Per-socket ACLINT MTIMER */
        riscv_aclint_mtimer_create(system_حافظه,
                memmap[RVSERVER_ACLINT].بنسټ + i * RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                بنسټ_hartid, hart_شمېر,
                RISCV_ACLINT_DEFAULT_MTIMECMP,
                RISCV_ACLINT_DEFAULT_MTIME,
                RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

        /* Per-socket interrupt controller */
        s->irqchip[i] = riscv_create_aia(system_حافظه, true, s->aia_guests,
                                         IMSIC_HART_اندازه(0),
                                         IMSIC_HART_اندازه(imsic_بټونه),
                                         RVSERVER_IRQCHIP_عدد_SOURCES,
                                         &s->memmap[RVSERVER_APLIC_M],
                                         &s->memmap[RVSERVER_APLIC_S],
                                         &s->memmap[RVSERVER_IMSIC_M],
                                         &s->memmap[RVSERVER_IMSIC_S],
                                         i, بنسټ_hartid, hart_شمېر,
                                         RVSERVER_IRQCHIP_عدد_MSIS,
                                         RVSERVER_IRQCHIP_عدد_PRIO_بټونه);

        /* Try to use different IRQCHIP instance based device type */
        إذا (i == 0) {
            mmio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        إذا (i == 1) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    /* register system main memory (actual RAM) */
    حافظه_region_جمع_کړه_subregion(system_حافظه, memmap[RVSERVER_DRAM].بنسټ,
        machine->ram);

    /* boot rom */
    حافظه_region_init_rom(ماسک_rom, عدم, "riscv_rvserver_ref_board.mrom",
                           memmap[RVSERVER_MROM].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(system_حافظه, memmap[RVSERVER_MROM].بنسټ,
                                ماسک_rom);

    حافظه_region_init_io(reset_syscon_io, عدم, &rvserver_reset_syscon_ops,
                          عدم, "reset_syscon_io",
                          memmap[RVSERVER_RESET_SYSCON].اندازه);
    حافظه_region_جمع_کړه_subregion(system_حافظه,
                                memmap[RVSERVER_RESET_SYSCON].بنسټ,
                                reset_syscon_io);

    riscv_gpex_pcie_init(system_حافظه, pcie_irqchip,
                         &rvserver_ref_memmap[RVSERVER_PCIE_ECAM],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO_لوړ],
                         &rvserver_ref_memmap[RVSERVER_PCIE_PIO],
                         RVSERVER_PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
                                                    &s->memmap[RVSERVER_PLATFORM_BUS],
                                                    RVSERVER_PLATFORM_BUS_IRQ,
                                                    RVSERVER_PLATFORM_BUS_عدد_IRQS);

    serial_mm_init(system_حافظه, memmap[RVSERVER_UART0].بنسټ,
        0, qdev_get_gpio_دننه(mmio_irqchip, RVSERVER_UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    sysbus_create_simple("goldfish_rtc", memmap[RVSERVER_RTC].بنسټ,
        qdev_get_gpio_دننه(mmio_irqchip, RVSERVER_RTC_IRQ));

    لكل (i = 0; i < ARRAY_اندازه(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    rvserver_flash_maps(s, system_حافظه);

    /* load/create device tree */
    إذا (machine->dtb) {
        machine->fdt = بارول_device_ونه(machine->dtb, &s->fdt_اندازه);
        إذا (!machine->fdt) {
            error_report("load_device_tree() failed");
            وتل(1);
        }
    } وإلا {
        create_fdt(s, memmap);
    }

    riscv_create_iommu_sys(mmio_irqchip, s->memmap[RVSERVER_IOMMU_SYS].بنسټ,
                           IOMMU_SYS_IRQ, false);

    s->machine_بشپړ.notify = rvserver_ref_machine_بشپړ;
    qemu_جمع_کړه_machine_init_بشپړ_notifier(&s->machine_بشپړ);
}

ساكن فراغ rvserver_ref_machine_instance_init(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->flash[0] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash0",
                                     "pflash0", RVSERVER_FLASH_SECTOR_اندازه);
    s->flash[1] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash1",
                                     "pflash1", RVSERVER_FLASH_SECTOR_اندازه);

    s->aia_guests = RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS;
}

ساكن فراغ rvserver_ref_machine_instance_finalize(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    لكل (صحيح i = 0; i < ARRAY_اندازه(s->flash); i++) {
        إذا (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
}

ساكن حرف *rvserver_ref_get_aia_guests(Object *obj, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);
    حرف val[32];

    sprintf(val, "%d", s->aia_guests);
    أرجع g_strdup(val);
}

ساكن فراغ rvserver_ref_سېټ_aia_guests(Object *obj, ثابت حرف *val,
                                        Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->aia_guests = atoi(val);
    إذا (s->aia_guests < 0 || s->aia_guests > RVSERVER_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          RVSERVER_IRQCHIP_MAX_GUESTS);
    }
}

ساكن ثابت HotplugHandler *
rvserver_machine_get_hotplug_handler(MachineState *ms, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);

    إذا (device_is_dynamic_sysbus(mc, dev)) {
        أرجع HOTPLUG_HANDLER(ms);
    }

    أرجع عدم;
}

ساكن فراغ rvserver_machine_device_plug_cb(ثابت HotplugHandler *hotplug_dev,
                                            DeviceState *dev, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(hotplug_dev);

    إذا (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        إذا (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }
}

ساكن فراغ rvserver_ref_machine_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    حرف str[128];
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);
    ساكن ثابت حرف * ثابت valid_cpu_types[] = {
        TYPE_RISCV_CPU_RVSERVER_REF,
        عدم
    };

    /* Note: this is used as /model in the FDT.  */
    mc->desc = "RISC-V Server Platform Reference Board v1.0";
    mc->init = rvserver_ref_machine_init;
    mc->max_cpus = RVSERVER_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_RVSERVER_REF;
    mc->valid_cpu_types = valid_cpu_types;
    mc->pci_allow_0_پته = true;
    mc->default_nic = "e1000e";
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_شاخص_منزل_instance_props = riscv_numa_cpu_شاخص_منزل_props;
    mc->get_default_cpu_غوټه_id = riscv_numa_get_default_cpu_غوټه_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_rvserver_ref_board.ram";

    object_class_property_جمع_کړه_str(oc, "aia-guests",
                                  rvserver_ref_get_aia_guests,
                                  rvserver_ref_سېټ_aia_guests);
    sprintf(str, "Set number of guest MMIO pages for AIA IMSIC. Valid value "
                 "should be between 0 and %d.", RVSERVER_IRQCHIP_MAX_GUESTS);
    object_class_property_سېټ_description(oc, "aia-guests", str);

    أكد(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = rvserver_machine_get_hotplug_handler;
    hc->plug = rvserver_machine_device_plug_cb;
#که_تعریف_شوی CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#د_که_پای
}

ساكن ثابت TypeInfo rvserver_ref_typeinfo = {
    .name       = TYPE_RISCV_SERVER_REF_MACHINE,
    .پلار     = TYPE_MACHINE,
    .class_init = rvserver_ref_machine_class_init,
    .instance_init = rvserver_ref_machine_instance_init,
    .instance_finalize = rvserver_ref_machine_instance_finalize,
    .instance_اندازه = حجم(RISCVServerRefMachineState),
    .interfaces = (ثابت InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

ساكن فراغ rvserver_ref_init_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&rvserver_ref_typeinfo);
}

type_init(rvserver_ref_init_ثبتوونکی_types)
