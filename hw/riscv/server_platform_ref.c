/*
 * QEMU RISC-V Server Platform Reference Board (riscv-server-ref)
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/units.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qemu/guest-random.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qapi/qapi-visit-common.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/platform-bus.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/char/serial.h"
#అన్తర్భావ "hw/block/flash.h"
#అన్తర్భావ "hw/ide/pci.h"
#అన్తర్భావ "hw/ide/ahci-pci.h"
#అన్తర్భావ "hw/pci/pci.h"
#అన్తర్భావ "hw/pci-host/gpex.h"
#అన్తర్భావ "hw/core/sysbus-fdt.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/riscv/device-common.h"
#అన్తర్భావ "hw/riscv/fdt-common.h"
#అన్తర్భావ "hw/riscv/numa.h"
#అన్తర్భావ "hw/riscv/iommu.h"
#అన్తర్భావ "hw/riscv/riscv-iommu.h"
#అన్తర్భావ "hw/riscv/riscv-iommu-bits.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/riscv_aplic.h"
#అన్తర్భావ "hw/intc/riscv_imsic.h"
#అన్తర్భావ "chardev/char.h"
#అన్తర్భావ "hw/char/serial-mm.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "system/runstate.h"
#అన్తర్భావ "system/system.h"
#అన్తర్భావ "system/kvm.h"
#అన్తర్భావ "system/qtest.h"
#అన్తర్భావ "system/tcg.h"
#అన్తర్భావ "kvm/kvm_riscv.h"
#అన్తర్భావ "system/tpm.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "net/net.h"

#అన్తర్భావ "aia.h"

#నిర్వచించు RVSERVER_ACLINT_TIMEBASE_FREQ          1000000000 /* 1Ghz */
#నిర్వచించు RVSERVER_CPUS_MAX_బిట్లు                 9
#నిర్వచించు RVSERVER_CPUS_MAX                      (1 << RVSERVER_CPUS_MAX_బిట్లు)
#నిర్వచించు RVSERVER_IRQCHIP_అంకె_MSIS              255
#నిర్వచించు RVSERVER_IRQCHIP_అంకె_SOURCES           96
#నిర్వచించు RVSERVER_IRQCHIP_అంకె_PRIO_బిట్లు         3
#నిర్వచించు RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS    5
#నిర్వచించు RVSERVER_IRQCHIP_MAX_GUESTS_బిట్లు       3
#నిర్వచించు RVSERVER_IRQCHIP_MAX_GUESTS \
        ((1U << RVSERVER_IRQCHIP_MAX_GUESTS_బిట్లు) - 1U)
#నిర్వచించు RVSERVER_SOCKETS_MAX_బిట్లు              2
#నిర్వచించు RVSERVER_SOCKETS_MAX                   (1 << RVSERVER_SOCKETS_MAX_బిట్లు)

#నిర్వచించు SYSCON_RESET                           0x1
#నిర్వచించు SYSCON_POWEROFF                        0x2

#నిర్వచించు RVSERVER_PLATFORM_BUS_అంకె_IRQS         8

#నిర్వచించు TYPE_RISCV_SERVER_REF_MACHINE MACHINE_TYPE_NAME("riscv-server-ref")
OBJECT_DECLARE_SIMPLE_TYPE(RISCVServerRefMachineState, RISCV_SERVER_REF_MACHINE)

నిర్మాణం RISCVServerRefMachineState {
    /*< private >*/
    MachineState మాతృ;

    /*< public >*/
    Notifier machine_పూర్తి;
    RISCVHartArrayState soc[RVSERVER_SOCKETS_MAX];
    DeviceState *irqchip[RVSERVER_SOCKETS_MAX];
    PFlashCFI01 *flash[2];

    పూర్ణాంకం fdt_కొలత;
    పూర్ణాంకం aia_guests;
    స్థిరరాశి Mem_Map_నమోదు *memmap;

    DeviceState *platform_bus_dev;
};

గణన {
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
    RVSERVER_PCIE_MMIO_ఎగువ
};

గణన {
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

#నిర్వచించు RVSERVER_IMSIC_సమూహం_MAX_కొలత      (1U << IMSIC_MMIO_సమూహం_MIN_జరుపు)
#ఒకవేళ RVSERVER_IMSIC_సమూహం_MAX_కొలత < \
    IMSIC_సమూహం_కొలత(RVSERVER_CPUS_MAX_బిట్లు, RVSERVER_IRQCHIP_MAX_GUESTS_బిట్లు)
#దోషం "Can't accommodate single IMSIC group in address space"
#అయితే_ముగింపు

#నిర్వచించు RVSERVER_IMSIC_MAX_కొలత \
    (RVSERVER_SOCKETS_MAX * RVSERVER_IMSIC_సమూహం_MAX_కొలత)
#ఒకవేళ 0x4000000 < RVSERVER_IMSIC_MAX_కొలత
#దోషం "Can't accommodate all IMSIC groups in address space"
#అయితే_ముగింపు

స్థిర స్థిరరాశి Mem_Map_నమోదు rvserver_ref_memmap[] = {
    [RVSERVER_MROM] =           {     0x1000,        0xf000 },
    [RVSERVER_RESET_SYSCON] =   {   0x100000,        0x1000 },
    [RVSERVER_RTC] =            {   0x101000,        0x1000 },
    [RVSERVER_IOMMU_SYS] =      {   0x102000,        0x1000 },
    [RVSERVER_ACLINT] =         {  0x2000000,       0x10000 },
    [RVSERVER_PCIE_PIO] =       {  0x3000000,       0x10000 },
    [RVSERVER_PLATFORM_BUS] =   {  0x4000000,     0x2000000 },
    [RVSERVER_APLIC_M] =        {  0xc000000, APLIC_కొలత(RVSERVER_CPUS_MAX) },
    [RVSERVER_APLIC_S] =        {  0xd000000, APLIC_కొలత(RVSERVER_CPUS_MAX) },
    [RVSERVER_UART0] =          { 0x10000000,         0x100 },
    [RVSERVER_FLASH] =          { 0x20000000,     0x4000000 },
    [RVSERVER_IMSIC_M] =        { 0x24000000, RVSERVER_IMSIC_MAX_కొలత },
    [RVSERVER_IMSIC_S] =        { 0x28000000, RVSERVER_IMSIC_MAX_కొలత },
    [RVSERVER_PCIE_ECAM] =      { 0x30000000,    0x10000000 },
    [RVSERVER_PCIE_MMIO] =      { 0x40000000,    0x40000000 },
    [RVSERVER_DRAM] =           { 0x80000000, 0xff80000000ull },
    [RVSERVER_PCIE_MMIO_ఎగువ] = { 0x10000000000ull, 0x10000000000ull },
};

#నిర్వచించు RVSERVER_FLASH_SECTOR_కొలత (256 * KiB)

స్థిర శూన్య rvserver_flash_maps(RISCVServerRefMachineState *s,
                                జ్ఞాపకం_Region *sysmem)
{
    hwaddr flashsize = rvserver_ref_memmap[RVSERVER_FLASH].కొలత / 2;
    hwaddr flashbase = rvserver_ref_memmap[RVSERVER_FLASH].పునాది;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_కొలత);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_కొలత);
}

స్థిర శూన్య create_fdt_pmu(RISCVServerRefMachineState *s)
{
    g_autofree అక్షరం *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_కూడు_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_కణుపు(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

స్థిర శూన్య create_fdt_sockets(RISCVServerRefMachineState *s,
                               స్థిరరాశి Mem_Map_నమోదు *memmap,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
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
    కొరకు (socket = (socket_లెక్క - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[RVSERVER_DRAM].పునాది +
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

        aclint_props.socket = socket;
        aclint_props.అంకె_harts = s->soc[socket].అంకె_harts;
        riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                       &intc_phandles[phandle_pos]);
    }

    imsic_fdt_props.soc = &s->soc;
    imsic_fdt_props.socket_లెక్క = socket_లెక్క;
    imsic_fdt_props.smp_cpus = ms->smp.cpus;
    imsic_fdt_props.imsic_m_పునాది = memmap[RVSERVER_IMSIC_M].పునాది;
    imsic_fdt_props.imsic_s_పునాది = memmap[RVSERVER_IMSIC_S].పునాది;
    imsic_fdt_props.imsic_సమూహం_max_కొలత = RVSERVER_IMSIC_సమూహం_MAX_కొలత;
    imsic_fdt_props.irqchip_అంకె_msis = RVSERVER_IRQCHIP_అంకె_MSIS;
    imsic_fdt_props.aia_guests = s->aia_guests;

    riscv_create_fdt_imsic(ms->fdt, &imsic_fdt_props, phandle, intc_phandles,
                           &msi_m_phandle, &msi_s_phandle);
    *msi_pcie_phandle = msi_s_phandle;

    aplic_fdt_props.aplic_m = !kvm_enabled()
                              ? &memmap[RVSERVER_APLIC_M] : ఖాళీ;
    aplic_fdt_props.aplic_s = &memmap[RVSERVER_APLIC_S];
    aplic_fdt_props.platform_bus = &memmap[RVSERVER_PLATFORM_BUS];
    aplic_fdt_props.platform_bus_irq = RVSERVER_PLATFORM_BUS_IRQ;
    aplic_fdt_props.అంకె_harts = ms->smp.cpus;
    aplic_fdt_props.numa_enabled = numa_enabled;
    aplic_fdt_props.irqchip_అంకె_sources = RVSERVER_IRQCHIP_అంకె_SOURCES;
    aplic_fdt_props.aia_type = AIA_TYPE_APLIC_IMSIC;

    phandle_pos = ms->smp.cpus;
    కొరకు (socket = (socket_లెక్క - 1); socket >= 0; socket--) {
        phandle_pos -= s->soc[socket].అంకె_harts;

        aplic_fdt_props.socket = socket;
        aplic_fdt_props.అంకె_harts = s->soc[socket].అంకె_harts;
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_fdt_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[phandle_pos],
                                      xplic_phandles);
    }

    కొరకు (socket = 0; socket < socket_లెక్క; socket++) {
        ఒకవేళ (socket == 0) {
            *irq_mmio_phandle = xplic_phandles[socket];
            *irq_pcie_phandle = xplic_phandles[socket];
        }
        ఒకవేళ (socket == 1) {
            *irq_pcie_phandle = xplic_phandles[socket];
        }
    }

    riscv_socket_fdt_write_దూరం_మాతృక(ms);
}

స్థిర శూన్య finalize_fdt(RISCVServerRefMachineState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, iommu_sys_phandle;
    g_autofree అక్షరం *name = ఖాళీ;
    MachineState *ms = MACHINE(s);

    create_fdt_sockets(s, rvserver_ref_memmap, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &msi_pcie_phandle);

    iommu_sys_phandle = riscv_create_fdt_riscv_iommu_sys(ms->fdt,
                                         s->memmap[RVSERVER_IOMMU_SYS].పునాది,
                                         s->memmap[RVSERVER_IOMMU_SYS].కొలత,
                                         &phandle, irq_mmio_phandle,
                                         msi_pcie_phandle, IOMMU_SYS_IRQ);

    riscv_create_fdt_pcie(ms->fdt, AIA_TYPE_APLIC_IMSIC, true,
                          &s->memmap[RVSERVER_PCIE_ECAM],
                          &s->memmap[RVSERVER_PCIE_PIO],
                          &s->memmap[RVSERVER_PCIE_MMIO],
                          &s->memmap[RVSERVER_PCIE_MMIO_ఎగువ],
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, RVSERVER_PCIE_IRQ);

    riscv_create_fdt_syscon(ms->fdt, &phandle,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].పునాది,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].కొలత,
                            SYSCON_RESET, SYSCON_POWEROFF, false);

    riscv_create_fdt_uart(ms->fdt, &rvserver_ref_memmap[RVSERVER_UART0],
                          RVSERVER_UART0_IRQ, AIA_TYPE_APLIC_IMSIC,
                          irq_mmio_phandle);
    name = riscv_fdt_get_uart_nodename(rvserver_ref_memmap[RVSERVER_UART0].పునాది);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);

    riscv_create_fdt_rtc(ms->fdt, &rvserver_ref_memmap[RVSERVER_RTC],
                         RVSERVER_RTC_IRQ, AIA_TYPE_APLIC_IMSIC,
                         irq_mmio_phandle);
}

స్థిర శూన్య create_fdt(RISCVServerRefMachineState *s,
                       స్థిరరాశి Mem_Map_నమోదు *memmap)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_విత్తనం[32];
    g_autofree అక్షరం *name = ఖాళీ;

    ms->fdt = riscv_create_board_device_వృక్షం(MACHINE_GET_CLASS(s)->desc,
                                             "qemu,riscv-server-ref",
                                             &s->fdt_కొలత);

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
                           s->memmap[RVSERVER_PCIE_ECAM].పునాది);
    qemu_fdt_కూడు_subnode(ms->fdt, name);

    qemu_fdt_కూడు_subnode(ms->fdt, "/chosen");
    qemu_fdt_కూడు_subnode(ms->fdt, "/aliases");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_విత్తనం, పరిమాణం(rng_విత్తనం));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_విత్తనం, పరిమాణం(rng_విత్తనం));

    riscv_create_fdt_flash(ms->fdt, rvserver_ref_memmap[RVSERVER_FLASH].పునాది,
                           rvserver_ref_memmap[RVSERVER_FLASH].కొలత / 2);
    create_fdt_pmu(s);

}

స్థిర uint64_t rvserver_reset_syscon_read(శూన్య *opaque, hwaddr addr,
                                           గుర్తులేని కొలత)
{
    తిరిగి 0;
}

స్థిర శూన్య rvserver_reset_syscon_write(శూన్య *opaque, hwaddr addr,
                                        uint64_t val64, గుర్తులేని పూర్ణాంకం కొలత)
{
    ఎంపిక (val64) {
    స్థితి SYSCON_POWEROFF:
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_GUEST_SHUTDOWN);
        తిరిగి;
    స్థితి SYSCON_RESET:
        qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        తిరిగి;
    అప్రమేయ:
        ఆపు;
    }
}

స్థిర స్థిరరాశి జ్ఞాపకం_Region_Ops rvserver_reset_syscon_ops = {
    .పఠ = rvserver_reset_syscon_read,
    .లిఖ = rvserver_reset_syscon_write,
    .endianness = DEVICE_LITTLE_ENDIAN
};

స్థిర శూన్య rvserver_ref_machine_పూర్తి(Notifier *notifier, శూన్య *దత్తాంశం)
{
    RISCVServerRefMachineState *s = container_యొక్క(
        notifier, RISCVServerRefMachineState, machine_పూర్తి);
    స్థిరరాశి Mem_Map_నమోదు *memmap = rvserver_ref_memmap;
    MachineState *machine = MACHINE(s);
    hwaddr ప్రారంభం_addr = memmap[RVSERVER_DRAM].పునాది;
    లక్ష్యం_ulong firmware_చివర_addr, kernel_ప్రారంభం_addr;
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

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_చివర_addr = riscv_కనుగొను_and_ఎక్కించు_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ప్రారంభం_addr, ఖాళీ);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    ఒకవేళ (pflash_blk0) {
        ఒకవేళ (machine->firmware && !strcmp(machine->firmware, "none")) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            ప్రారంభం_addr = rvserver_ref_memmap[RVSERVER_FLASH].పునాది;
        } లేకపోతే {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_అమరిక_firmware_boot(machine);
            kernel_నమోదు = rvserver_ref_memmap[RVSERVER_FLASH].పునాది;
        }
    }

    ఒకవేళ (machine->kernel_filename && !kernel_నమోదు) {
        kernel_ప్రారంభం_addr = riscv_calc_kernel_ప్రారంభం_addr(&boot_info,
                                                         firmware_చివర_addr);
        riscv_ఎక్కించు_kernel(machine, &boot_info, kernel_ప్రారంభం_addr, true, ఖాళీ);
        kernel_నమోదు = boot_info.image_దిగువ_addr;
    }

    fdt_ఎక్కించు_addr = riscv_compute_fdt_addr(memmap[RVSERVER_DRAM].పునాది,
                                           memmap[RVSERVER_DRAM].కొలత,
                                           machine, &boot_info);

    riscv_ఎక్కించు_fdt(fdt_ఎక్కించు_addr, machine->fdt);

    /* load the reset vector */
    riscv_అమరిక_rom_reset_vec(machine, &s->soc[0], ప్రారంభం_addr,
                              rvserver_ref_memmap[RVSERVER_MROM].పునాది,
                              rvserver_ref_memmap[RVSERVER_MROM].కొలత,
                              kernel_నమోదు,
                              fdt_ఎక్కించు_addr);
}

స్థిర bool rvserver_aclint_allowed(శూన్య)
{
    తిరిగి tcg_enabled() || qtest_enabled();
}

స్థిర శూన్య rvserver_ref_machine_init(MachineState *machine)
{
    స్థిరరాశి Mem_Map_నమోదు *memmap = rvserver_ref_memmap;
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(machine);
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    జ్ఞాపకం_Region *ముసుగు_rom = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *reset_syscon_io = g_new(జ్ఞాపకం_Region, 1);
    DeviceState *mmio_irqchip, *pcie_irqchip;
    పూర్ణాంకం i, పునాది_hartid, hart_లెక్క;
    పూర్ణాంకం socket_లెక్క = riscv_socket_లెక్క(machine);
    పూర్ణాంకం imsic_బిట్లు;

    /* Check socket count limit */
    ఒకవేళ (RVSERVER_SOCKETS_MAX < socket_లెక్క) {
        error_report("number of sockets/nodes should be less than %d",
            RVSERVER_SOCKETS_MAX);
        నిష్క్రమించు(1);
    }

    ఒకవేళ (!rvserver_aclint_allowed()) {
        error_report("'aclint' is only available with TCG acceleration");
        నిష్క్రమించు(1);
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
    ఒకవేళ (qtest_enabled()) {
        s->aia_guests = 0;
    }

    imsic_బిట్లు = imsic_అంకె_బిట్లు(s->aia_guests + 1);

    /* Initialize sockets */
    mmio_irqchip = pcie_irqchip = ఖాళీ;
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

        /* Per-socket ACLINT MTIMER */
        riscv_aclint_mtimer_create(system_జ్ఞాపకం,
                memmap[RVSERVER_ACLINT].పునాది + i * RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                పునాది_hartid, hart_లెక్క,
                RISCV_ACLINT_DEFAULT_MTIMECMP,
                RISCV_ACLINT_DEFAULT_MTIME,
                RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

        /* Per-socket interrupt controller */
        s->irqchip[i] = riscv_create_aia(system_జ్ఞాపకం, true, s->aia_guests,
                                         IMSIC_HART_కొలత(0),
                                         IMSIC_HART_కొలత(imsic_బిట్లు),
                                         RVSERVER_IRQCHIP_అంకె_SOURCES,
                                         &s->memmap[RVSERVER_APLIC_M],
                                         &s->memmap[RVSERVER_APLIC_S],
                                         &s->memmap[RVSERVER_IMSIC_M],
                                         &s->memmap[RVSERVER_IMSIC_S],
                                         i, పునాది_hartid, hart_లెక్క,
                                         RVSERVER_IRQCHIP_అంకె_MSIS,
                                         RVSERVER_IRQCHIP_అంకె_PRIO_బిట్లు);

        /* Try to use different IRQCHIP instance based device type */
        ఒకవేళ (i == 0) {
            mmio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ఒకవేళ (i == 1) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    /* register system main memory (actual RAM) */
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, memmap[RVSERVER_DRAM].పునాది,
        machine->ram);

    /* boot rom */
    జ్ఞాపకం_region_init_rom(ముసుగు_rom, ఖాళీ, "riscv_rvserver_ref_board.mrom",
                           memmap[RVSERVER_MROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, memmap[RVSERVER_MROM].పునాది,
                                ముసుగు_rom);

    జ్ఞాపకం_region_init_io(reset_syscon_io, ఖాళీ, &rvserver_reset_syscon_ops,
                          ఖాళీ, "reset_syscon_io",
                          memmap[RVSERVER_RESET_SYSCON].కొలత);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[RVSERVER_RESET_SYSCON].పునాది,
                                reset_syscon_io);

    riscv_gpex_pcie_init(system_జ్ఞాపకం, pcie_irqchip,
                         &rvserver_ref_memmap[RVSERVER_PCIE_ECAM],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO_ఎగువ],
                         &rvserver_ref_memmap[RVSERVER_PCIE_PIO],
                         RVSERVER_PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
                                                    &s->memmap[RVSERVER_PLATFORM_BUS],
                                                    RVSERVER_PLATFORM_BUS_IRQ,
                                                    RVSERVER_PLATFORM_BUS_అంకె_IRQS);

    serial_mm_init(system_జ్ఞాపకం, memmap[RVSERVER_UART0].పునాది,
        0, qdev_get_gpio_లోపల(mmio_irqchip, RVSERVER_UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    sysbus_create_simple("goldfish_rtc", memmap[RVSERVER_RTC].పునాది,
        qdev_get_gpio_లోపల(mmio_irqchip, RVSERVER_RTC_IRQ));

    కొరకు (i = 0; i < ARRAY_కొలత(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    rvserver_flash_maps(s, system_జ్ఞాపకం);

    /* load/create device tree */
    ఒకవేళ (machine->dtb) {
        machine->fdt = ఎక్కించు_device_వృక్షం(machine->dtb, &s->fdt_కొలత);
        ఒకవేళ (!machine->fdt) {
            error_report("load_device_tree() failed");
            నిష్క్రమించు(1);
        }
    } లేకపోతే {
        create_fdt(s, memmap);
    }

    riscv_create_iommu_sys(mmio_irqchip, s->memmap[RVSERVER_IOMMU_SYS].పునాది,
                           IOMMU_SYS_IRQ, false);

    s->machine_పూర్తి.notify = rvserver_ref_machine_పూర్తి;
    qemu_కూడు_machine_init_పూర్తి_notifier(&s->machine_పూర్తి);
}

స్థిర శూన్య rvserver_ref_machine_instance_init(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->flash[0] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash0",
                                     "pflash0", RVSERVER_FLASH_SECTOR_కొలత);
    s->flash[1] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash1",
                                     "pflash1", RVSERVER_FLASH_SECTOR_కొలత);

    s->aia_guests = RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS;
}

స్థిర శూన్య rvserver_ref_machine_instance_finalize(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    కొరకు (పూర్ణాంకం i = 0; i < ARRAY_కొలత(s->flash); i++) {
        ఒకవేళ (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
}

స్థిర అక్షరం *rvserver_ref_get_aia_guests(Object *obj, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);
    అక్షరం val[32];

    sprintf(val, "%d", s->aia_guests);
    తిరిగి g_strdup(val);
}

స్థిర శూన్య rvserver_ref_సమితి_aia_guests(Object *obj, స్థిరరాశి అక్షరం *val,
                                        Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->aia_guests = atoi(val);
    ఒకవేళ (s->aia_guests < 0 || s->aia_guests > RVSERVER_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          RVSERVER_IRQCHIP_MAX_GUESTS);
    }
}

స్థిర స్థిరరాశి HotplugHandler *
rvserver_machine_get_hotplug_handler(MachineState *ms, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);

    ఒకవేళ (device_is_dynamic_sysbus(mc, dev)) {
        తిరిగి HOTPLUG_HANDLER(ms);
    }

    తిరిగి ఖాళీ;
}

స్థిర శూన్య rvserver_machine_device_plug_cb(స్థిరరాశి HotplugHandler *hotplug_dev,
                                            DeviceState *dev, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(hotplug_dev);

    ఒకవేళ (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        ఒకవేళ (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }
}

స్థిర శూన్య rvserver_ref_machine_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    అక్షరం str[128];
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి valid_cpu_types[] = {
        TYPE_RISCV_CPU_RVSERVER_REF,
        ఖాళీ
    };

    /* Note: this is used as /model in the FDT.  */
    mc->desc = "RISC-V Server Platform Reference Board v1.0";
    mc->init = rvserver_ref_machine_init;
    mc->max_cpus = RVSERVER_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_RVSERVER_REF;
    mc->valid_cpu_types = valid_cpu_types;
    mc->pci_allow_0_చిరునామా = true;
    mc->default_nic = "e1000e";
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_సూచీ_గమ్యం_instance_props = riscv_numa_cpu_సూచీ_గమ్యం_props;
    mc->get_default_cpu_కణుపు_id = riscv_numa_get_default_cpu_కణుపు_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_rvserver_ref_board.ram";

    object_class_property_కూడు_str(oc, "aia-guests",
                                  rvserver_ref_get_aia_guests,
                                  rvserver_ref_సమితి_aia_guests);
    sprintf(str, "Set number of guest MMIO pages for AIA IMSIC. Valid value "
                 "should be between 0 and %d.", RVSERVER_IRQCHIP_MAX_GUESTS);
    object_class_property_సమితి_description(oc, "aia-guests", str);

    ప్రతిజ్ఞా(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = rvserver_machine_get_hotplug_handler;
    hc->plug = rvserver_machine_device_plug_cb;
#నిర్వచితమైతే CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#అయితే_ముగింపు
}

స్థిర స్థిరరాశి TypeInfo rvserver_ref_typeinfo = {
    .name       = TYPE_RISCV_SERVER_REF_MACHINE,
    .మాతృ     = TYPE_MACHINE,
    .class_init = rvserver_ref_machine_class_init,
    .instance_init = rvserver_ref_machine_instance_init,
    .instance_finalize = rvserver_ref_machine_instance_finalize,
    .instance_కొలత = పరిమాణం(RISCVServerRefMachineState),
    .interfaces = (స్థిరరాశి InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

స్థిర శూన్య rvserver_ref_init_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&rvserver_ref_typeinfo);
}

type_init(rvserver_ref_init_రిజిస్టరు_types)
