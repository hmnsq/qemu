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

#تعریف_کن RVSERVER_ACLINT_TIMEBASE_FREQ          1000000000 /* 1Ghz */
#تعریف_کن RVSERVER_CPUS_MAX_بیتها                 9
#تعریف_کن RVSERVER_CPUS_MAX                      (1 << RVSERVER_CPUS_MAX_بیتها)
#تعریف_کن RVSERVER_IRQCHIP_رقم_MSIS              255
#تعریف_کن RVSERVER_IRQCHIP_رقم_SOURCES           96
#تعریف_کن RVSERVER_IRQCHIP_رقم_PRIO_بیتها         3
#تعریف_کن RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS    5
#تعریف_کن RVSERVER_IRQCHIP_MAX_GUESTS_بیتها       3
#تعریف_کن RVSERVER_IRQCHIP_MAX_GUESTS \
        ((1U << RVSERVER_IRQCHIP_MAX_GUESTS_بیتها) - 1U)
#تعریف_کن RVSERVER_SOCKETS_MAX_بیتها              2
#تعریف_کن RVSERVER_SOCKETS_MAX                   (1 << RVSERVER_SOCKETS_MAX_بیتها)

#تعریف_کن SYSCON_RESET                           0x1
#تعریف_کن SYSCON_POWEROFF                        0x2

#تعریف_کن RVSERVER_PLATFORM_BUS_رقم_IRQS         8

#تعریف_کن TYPE_RISCV_SERVER_REF_MACHINE MACHINE_TYPE_NAME("riscv-server-ref")
OBJECT_DECLARE_SIMPLE_TYPE(RISCVServerRefMachineState, RISCV_SERVER_REF_MACHINE)

بنية RISCVServerRefMachineState {
    /*< private >*/
    MachineState والد;

    /*< public >*/
    Notifier machine_انجامشده;
    RISCVHartArrayState soc[RVSERVER_SOCKETS_MAX];
    DeviceState *irqchip[RVSERVER_SOCKETS_MAX];
    PFlashCFI01 *flash[2];

    صحيح fdt_اندازه;
    صحيح aia_guests;
    ثابت Mem_Map_مدخل *memmap;

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
    RVSERVER_PCIE_MMIO_بالا
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

#تعریف_کن RVSERVER_IMSIC_گروه_MAX_اندازه      (1U << IMSIC_MMIO_گروه_MIN_لغزش)
#إذا RVSERVER_IMSIC_گروه_MAX_اندازه < \
    IMSIC_گروه_اندازه(RVSERVER_CPUS_MAX_بیتها, RVSERVER_IRQCHIP_MAX_GUESTS_بیتها)
#خطا "Can't accommodate single IMSIC group in address space"
#پایان_اگر

#تعریف_کن RVSERVER_IMSIC_MAX_اندازه \
    (RVSERVER_SOCKETS_MAX * RVSERVER_IMSIC_گروه_MAX_اندازه)
#إذا 0x4000000 < RVSERVER_IMSIC_MAX_اندازه
#خطا "Can't accommodate all IMSIC groups in address space"
#پایان_اگر

ساكن ثابت Mem_Map_مدخل rvserver_ref_memmap[] = {
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
    [RVSERVER_PCIE_MMIO_بالا] = { 0x10000000000ull, 0x10000000000ull },
};

#تعریف_کن RVSERVER_FLASH_SECTOR_اندازه (256 * KiB)

ساكن فراغ rvserver_flash_maps(RISCVServerRefMachineState *s,
                                حافظه_Region *sysmem)
{
    hwaddr flashsize = rvserver_ref_memmap[RVSERVER_FLASH].اندازه / 2;
    hwaddr flashbase = rvserver_ref_memmap[RVSERVER_FLASH].پایه;

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

    qemu_fdt_جمع_کن_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_گره(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

ساكن فراغ create_fdt_sockets(RISCVServerRefMachineState *s,
                               ثابت Mem_Map_مدخل *memmap,
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
    صحيح socket_شمار = riscv_socket_شمار(ms);
    bool numa_enabled = riscv_numa_enabled(ms);
    bool is_32_بیت = riscv_is_32bit(&s->soc[0]);
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
    لكل (socket = (socket_شمار - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[RVSERVER_DRAM].پایه +
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

        aclint_props.socket = socket;
        aclint_props.رقم_harts = s->soc[socket].رقم_harts;
        riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                       &intc_phandles[phandle_pos]);
    }

    imsic_fdt_props.soc = &s->soc;
    imsic_fdt_props.socket_شمار = socket_شمار;
    imsic_fdt_props.smp_cpus = ms->smp.cpus;
    imsic_fdt_props.imsic_m_پایه = memmap[RVSERVER_IMSIC_M].پایه;
    imsic_fdt_props.imsic_s_پایه = memmap[RVSERVER_IMSIC_S].پایه;
    imsic_fdt_props.imsic_گروه_max_اندازه = RVSERVER_IMSIC_گروه_MAX_اندازه;
    imsic_fdt_props.irqchip_رقم_msis = RVSERVER_IRQCHIP_رقم_MSIS;
    imsic_fdt_props.aia_guests = s->aia_guests;

    riscv_create_fdt_imsic(ms->fdt, &imsic_fdt_props, phandle, intc_phandles,
                           &msi_m_phandle, &msi_s_phandle);
    *msi_pcie_phandle = msi_s_phandle;

    aplic_fdt_props.aplic_m = !kvm_enabled()
                              ? &memmap[RVSERVER_APLIC_M] : عدم;
    aplic_fdt_props.aplic_s = &memmap[RVSERVER_APLIC_S];
    aplic_fdt_props.platform_bus = &memmap[RVSERVER_PLATFORM_BUS];
    aplic_fdt_props.platform_bus_irq = RVSERVER_PLATFORM_BUS_IRQ;
    aplic_fdt_props.رقم_harts = ms->smp.cpus;
    aplic_fdt_props.numa_enabled = numa_enabled;
    aplic_fdt_props.irqchip_رقم_sources = RVSERVER_IRQCHIP_رقم_SOURCES;
    aplic_fdt_props.aia_type = AIA_TYPE_APLIC_IMSIC;

    phandle_pos = ms->smp.cpus;
    لكل (socket = (socket_شمار - 1); socket >= 0; socket--) {
        phandle_pos -= s->soc[socket].رقم_harts;

        aplic_fdt_props.socket = socket;
        aplic_fdt_props.رقم_harts = s->soc[socket].رقم_harts;
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_fdt_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[phandle_pos],
                                      xplic_phandles);
    }

    لكل (socket = 0; socket < socket_شمار; socket++) {
        إذا (socket == 0) {
            *irq_mmio_phandle = xplic_phandles[socket];
            *irq_pcie_phandle = xplic_phandles[socket];
        }
        إذا (socket == 1) {
            *irq_pcie_phandle = xplic_phandles[socket];
        }
    }

    riscv_socket_fdt_write_فاصله_ماتریس(ms);
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
                                         s->memmap[RVSERVER_IOMMU_SYS].پایه,
                                         s->memmap[RVSERVER_IOMMU_SYS].اندازه,
                                         &phandle, irq_mmio_phandle,
                                         msi_pcie_phandle, IOMMU_SYS_IRQ);

    riscv_create_fdt_pcie(ms->fdt, AIA_TYPE_APLIC_IMSIC, true,
                          &s->memmap[RVSERVER_PCIE_ECAM],
                          &s->memmap[RVSERVER_PCIE_PIO],
                          &s->memmap[RVSERVER_PCIE_MMIO],
                          &s->memmap[RVSERVER_PCIE_MMIO_بالا],
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, RVSERVER_PCIE_IRQ);

    riscv_create_fdt_syscon(ms->fdt, &phandle,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].پایه,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].اندازه,
                            SYSCON_RESET, SYSCON_POWEROFF, false);

    riscv_create_fdt_uart(ms->fdt, &rvserver_ref_memmap[RVSERVER_UART0],
                          RVSERVER_UART0_IRQ, AIA_TYPE_APLIC_IMSIC,
                          irq_mmio_phandle);
    name = riscv_fdt_get_uart_nodename(rvserver_ref_memmap[RVSERVER_UART0].پایه);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);

    riscv_create_fdt_rtc(ms->fdt, &rvserver_ref_memmap[RVSERVER_RTC],
                         RVSERVER_RTC_IRQ, AIA_TYPE_APLIC_IMSIC,
                         irq_mmio_phandle);
}

ساكن فراغ create_fdt(RISCVServerRefMachineState *s,
                       ثابت Mem_Map_مدخل *memmap)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_دانه[32];
    g_autofree حرف *name = عدم;

    ms->fdt = riscv_create_board_device_درخت(MACHINE_GET_CLASS(s)->desc,
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
                           s->memmap[RVSERVER_PCIE_ECAM].پایه);
    qemu_fdt_جمع_کن_subnode(ms->fdt, name);

    qemu_fdt_جمع_کن_subnode(ms->fdt, "/chosen");
    qemu_fdt_جمع_کن_subnode(ms->fdt, "/aliases");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_دانه, حجم(rng_دانه));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_دانه, حجم(rng_دانه));

    riscv_create_fdt_flash(ms->fdt, rvserver_ref_memmap[RVSERVER_FLASH].پایه,
                           rvserver_ref_memmap[RVSERVER_FLASH].اندازه / 2);
    create_fdt_pmu(s);

}

ساكن uint64_t rvserver_reset_syscon_read(فراغ *opaque, hwaddr addr,
                                           بیعلامت اندازه)
{
    أرجع 0;
}

ساكن فراغ rvserver_reset_syscon_write(فراغ *opaque, hwaddr addr,
                                        uint64_t val64, بیعلامت صحيح اندازه)
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

ساكن فراغ rvserver_ref_machine_انجامشده(Notifier *notifier, فراغ *داده)
{
    RISCVServerRefMachineState *s = container_از(
        notifier, RISCVServerRefMachineState, machine_انجامشده);
    ثابت Mem_Map_مدخل *memmap = rvserver_ref_memmap;
    MachineState *machine = MACHINE(s);
    hwaddr شروع_addr = memmap[RVSERVER_DRAM].پایه;
    هدف_ulong firmware_انتها_addr, kernel_شروع_addr;
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

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_انتها_addr = riscv_پیدا_کن_and_بارگیری_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &شروع_addr, عدم);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    إذا (pflash_blk0) {
        إذا (machine->firmware && !strcmp(machine->firmware, "none")) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            شروع_addr = rvserver_ref_memmap[RVSERVER_FLASH].پایه;
        } وإلا {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_راهاندازی_firmware_boot(machine);
            kernel_مدخل = rvserver_ref_memmap[RVSERVER_FLASH].پایه;
        }
    }

    إذا (machine->kernel_filename && !kernel_مدخل) {
        kernel_شروع_addr = riscv_calc_kernel_شروع_addr(&boot_info,
                                                         firmware_انتها_addr);
        riscv_بارگیری_kernel(machine, &boot_info, kernel_شروع_addr, true, عدم);
        kernel_مدخل = boot_info.image_کم_addr;
    }

    fdt_بارگیری_addr = riscv_compute_fdt_addr(memmap[RVSERVER_DRAM].پایه,
                                           memmap[RVSERVER_DRAM].اندازه,
                                           machine, &boot_info);

    riscv_بارگیری_fdt(fdt_بارگیری_addr, machine->fdt);

    /* load the reset vector */
    riscv_راهاندازی_rom_reset_vec(machine, &s->soc[0], شروع_addr,
                              rvserver_ref_memmap[RVSERVER_MROM].پایه,
                              rvserver_ref_memmap[RVSERVER_MROM].اندازه,
                              kernel_مدخل,
                              fdt_بارگیری_addr);
}

ساكن bool rvserver_aclint_allowed(فراغ)
{
    أرجع tcg_enabled() || qtest_enabled();
}

ساكن فراغ rvserver_ref_machine_init(MachineState *machine)
{
    ثابت Mem_Map_مدخل *memmap = rvserver_ref_memmap;
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(machine);
    حافظه_Region *system_حافظه = get_system_حافظه();
    حافظه_Region *ماسک_rom = g_new(حافظه_Region, 1);
    حافظه_Region *reset_syscon_io = g_new(حافظه_Region, 1);
    DeviceState *mmio_irqchip, *pcie_irqchip;
    صحيح i, پایه_hartid, hart_شمار;
    صحيح socket_شمار = riscv_socket_شمار(machine);
    صحيح imsic_بیتها;

    /* Check socket count limit */
    إذا (RVSERVER_SOCKETS_MAX < socket_شمار) {
        error_report("number of sockets/nodes should be less than %d",
            RVSERVER_SOCKETS_MAX);
        خروج(1);
    }

    إذا (!rvserver_aclint_allowed()) {
        error_report("'aclint' is only available with TCG acceleration");
        خروج(1);
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

    imsic_بیتها = imsic_رقم_بیتها(s->aia_guests + 1);

    /* Initialize sockets */
    mmio_irqchip = pcie_irqchip = عدم;
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

        /* Per-socket ACLINT MTIMER */
        riscv_aclint_mtimer_create(system_حافظه,
                memmap[RVSERVER_ACLINT].پایه + i * RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                RISCV_ACLINT_DEFAULT_MTIMER_اندازه,
                پایه_hartid, hart_شمار,
                RISCV_ACLINT_DEFAULT_MTIMECMP,
                RISCV_ACLINT_DEFAULT_MTIME,
                RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

        /* Per-socket interrupt controller */
        s->irqchip[i] = riscv_create_aia(system_حافظه, true, s->aia_guests,
                                         IMSIC_HART_اندازه(0),
                                         IMSIC_HART_اندازه(imsic_بیتها),
                                         RVSERVER_IRQCHIP_رقم_SOURCES,
                                         &s->memmap[RVSERVER_APLIC_M],
                                         &s->memmap[RVSERVER_APLIC_S],
                                         &s->memmap[RVSERVER_IMSIC_M],
                                         &s->memmap[RVSERVER_IMSIC_S],
                                         i, پایه_hartid, hart_شمار,
                                         RVSERVER_IRQCHIP_رقم_MSIS,
                                         RVSERVER_IRQCHIP_رقم_PRIO_بیتها);

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
    حافظه_region_جمع_کن_subregion(system_حافظه, memmap[RVSERVER_DRAM].پایه,
        machine->ram);

    /* boot rom */
    حافظه_region_init_rom(ماسک_rom, عدم, "riscv_rvserver_ref_board.mrom",
                           memmap[RVSERVER_MROM].اندازه, &error_fatal);
    حافظه_region_جمع_کن_subregion(system_حافظه, memmap[RVSERVER_MROM].پایه,
                                ماسک_rom);

    حافظه_region_init_io(reset_syscon_io, عدم, &rvserver_reset_syscon_ops,
                          عدم, "reset_syscon_io",
                          memmap[RVSERVER_RESET_SYSCON].اندازه);
    حافظه_region_جمع_کن_subregion(system_حافظه,
                                memmap[RVSERVER_RESET_SYSCON].پایه,
                                reset_syscon_io);

    riscv_gpex_pcie_init(system_حافظه, pcie_irqchip,
                         &rvserver_ref_memmap[RVSERVER_PCIE_ECAM],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO_بالا],
                         &rvserver_ref_memmap[RVSERVER_PCIE_PIO],
                         RVSERVER_PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
                                                    &s->memmap[RVSERVER_PLATFORM_BUS],
                                                    RVSERVER_PLATFORM_BUS_IRQ,
                                                    RVSERVER_PLATFORM_BUS_رقم_IRQS);

    serial_mm_init(system_حافظه, memmap[RVSERVER_UART0].پایه,
        0, qdev_get_gpio_درون(mmio_irqchip, RVSERVER_UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    sysbus_create_simple("goldfish_rtc", memmap[RVSERVER_RTC].پایه,
        qdev_get_gpio_درون(mmio_irqchip, RVSERVER_RTC_IRQ));

    لكل (i = 0; i < ARRAY_اندازه(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    rvserver_flash_maps(s, system_حافظه);

    /* load/create device tree */
    إذا (machine->dtb) {
        machine->fdt = بارگیری_device_درخت(machine->dtb, &s->fdt_اندازه);
        إذا (!machine->fdt) {
            error_report("load_device_tree() failed");
            خروج(1);
        }
    } وإلا {
        create_fdt(s, memmap);
    }

    riscv_create_iommu_sys(mmio_irqchip, s->memmap[RVSERVER_IOMMU_SYS].پایه,
                           IOMMU_SYS_IRQ, false);

    s->machine_انجامشده.notify = rvserver_ref_machine_انجامشده;
    qemu_جمع_کن_machine_init_انجامشده_notifier(&s->machine_انجامشده);
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

ساكن فراغ rvserver_ref_مجموعه_aia_guests(Object *obj, ثابت حرف *val,
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

ساكن فراغ rvserver_ref_machine_class_init(ObjectClass *oc, ثابت فراغ *داده)
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
    mc->pci_allow_0_آدرس = true;
    mc->default_nic = "e1000e";
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_شاخص_مقصد_instance_props = riscv_numa_cpu_شاخص_مقصد_props;
    mc->get_default_cpu_گره_id = riscv_numa_get_default_cpu_گره_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_rvserver_ref_board.ram";

    object_class_property_جمع_کن_str(oc, "aia-guests",
                                  rvserver_ref_get_aia_guests,
                                  rvserver_ref_مجموعه_aia_guests);
    sprintf(str, "Set number of guest MMIO pages for AIA IMSIC. Valid value "
                 "should be between 0 and %d.", RVSERVER_IRQCHIP_MAX_GUESTS);
    object_class_property_مجموعه_description(oc, "aia-guests", str);

    أكد(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = rvserver_machine_get_hotplug_handler;
    hc->plug = rvserver_machine_device_plug_cb;
#اگر_تعریف_شده CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#پایان_اگر
}

ساكن ثابت TypeInfo rvserver_ref_typeinfo = {
    .name       = TYPE_RISCV_SERVER_REF_MACHINE,
    .والد     = TYPE_MACHINE,
    .class_init = rvserver_ref_machine_class_init,
    .instance_init = rvserver_ref_machine_instance_init,
    .instance_finalize = rvserver_ref_machine_instance_finalize,
    .instance_اندازه = حجم(RISCVServerRefMachineState),
    .interfaces = (ثابت InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

ساكن فراغ rvserver_ref_init_ثبات_types(فراغ)
{
    type_ثبات_static(&rvserver_ref_typeinfo);
}

type_init(rvserver_ref_init_ثبات_types)
