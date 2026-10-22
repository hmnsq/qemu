/*
 * QEMU RISC-V Server Platform Reference Board (riscv-server-ref)
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/units.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/guest-random.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/qapi-visit-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/platform-bus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/loader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/char/serial.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/block/flash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/ide/pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/ide/ahci-pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci-host/gpex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus-fdt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv_hart.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/device-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/fdt-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/numa.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv-iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv-iommu-bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aplic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_imsic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chardev/char.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/char/serial-mm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/runstate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/kvm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/qtest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/tcg.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kvm/kvm_riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/tpm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "net/net.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "aia.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_ACLINT_TIMEBASE_FREQ          1000000000 /* 1Ghz */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_CPUS_MAX_ਬਿੱਟ                 9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_CPUS_MAX                      (1 << RVSERVER_CPUS_MAX_ਬਿੱਟ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_IRQCHIP_ਅੰਕ_MSIS              255
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_IRQCHIP_ਅੰਕ_SOURCES           96
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_IRQCHIP_ਅੰਕ_PRIO_ਬਿੱਟ         3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS    5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_IRQCHIP_MAX_GUESTS_ਬਿੱਟ       3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_IRQCHIP_MAX_GUESTS \
        ((1U << RVSERVER_IRQCHIP_MAX_GUESTS_ਬਿੱਟ) - 1U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_SOCKETS_MAX_ਬਿੱਟ              2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_SOCKETS_MAX                   (1 << RVSERVER_SOCKETS_MAX_ਬਿੱਟ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSCON_RESET                           0x1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSCON_POWEROFF                        0x2

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_PLATFORM_BUS_ਅੰਕ_IRQS         8

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TYPE_RISCV_SERVER_REF_MACHINE MACHINE_TYPE_NAME("riscv-server-ref")
OBJECT_DECLARE_SIMPLE_TYPE(RISCVServerRefMachineState, RISCV_SERVER_REF_MACHINE)

ਬਣਤਰ RISCVServerRefMachineState {
    /*< private >*/
    MachineState ਮਾਪਾ;

    /*< public >*/
    Notifier machine_ਪੂਰਾ;
    RISCVHartArrayState soc[RVSERVER_SOCKETS_MAX];
    DeviceState *irqchip[RVSERVER_SOCKETS_MAX];
    PFlashCFI01 *flash[2];

    ਪੂਰਨ fdt_ਮਾਪ;
    ਪੂਰਨ aia_guests;
    ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap;

    DeviceState *platform_bus_dev;
};

ਗਿਣਤੀ {
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
    RVSERVER_PCIE_MMIO_ਉੱਚਾ
};

ਗਿਣਤੀ {
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_IMSIC_ਸਮੂਹ_MAX_ਮਾਪ      (1U << IMSIC_MMIO_ਸਮੂਹ_MIN_ਖਿਸਕਾਓ)
#ਜੇ RVSERVER_IMSIC_ਸਮੂਹ_MAX_ਮਾਪ < \
    IMSIC_ਸਮੂਹ_ਮਾਪ(RVSERVER_CPUS_MAX_ਬਿੱਟ, RVSERVER_IRQCHIP_MAX_GUESTS_ਬਿੱਟ)
#ਗਲਤੀ "Can't accommodate single IMSIC group in address space"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_IMSIC_MAX_ਮਾਪ \
    (RVSERVER_SOCKETS_MAX * RVSERVER_IMSIC_ਸਮੂਹ_MAX_ਮਾਪ)
#ਜੇ 0x4000000 < RVSERVER_IMSIC_MAX_ਮਾਪ
#ਗਲਤੀ "Can't accommodate all IMSIC groups in address space"
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ Mem_Map_ਇੰਦਰਾਜ rvserver_ref_memmap[] = {
    [RVSERVER_MROM] =           {     0x1000,        0xf000 },
    [RVSERVER_RESET_SYSCON] =   {   0x100000,        0x1000 },
    [RVSERVER_RTC] =            {   0x101000,        0x1000 },
    [RVSERVER_IOMMU_SYS] =      {   0x102000,        0x1000 },
    [RVSERVER_ACLINT] =         {  0x2000000,       0x10000 },
    [RVSERVER_PCIE_PIO] =       {  0x3000000,       0x10000 },
    [RVSERVER_PLATFORM_BUS] =   {  0x4000000,     0x2000000 },
    [RVSERVER_APLIC_M] =        {  0xc000000, APLIC_ਮਾਪ(RVSERVER_CPUS_MAX) },
    [RVSERVER_APLIC_S] =        {  0xd000000, APLIC_ਮਾਪ(RVSERVER_CPUS_MAX) },
    [RVSERVER_UART0] =          { 0x10000000,         0x100 },
    [RVSERVER_FLASH] =          { 0x20000000,     0x4000000 },
    [RVSERVER_IMSIC_M] =        { 0x24000000, RVSERVER_IMSIC_MAX_ਮਾਪ },
    [RVSERVER_IMSIC_S] =        { 0x28000000, RVSERVER_IMSIC_MAX_ਮਾਪ },
    [RVSERVER_PCIE_ECAM] =      { 0x30000000,    0x10000000 },
    [RVSERVER_PCIE_MMIO] =      { 0x40000000,    0x40000000 },
    [RVSERVER_DRAM] =           { 0x80000000, 0xff80000000ull },
    [RVSERVER_PCIE_MMIO_ਉੱਚਾ] = { 0x10000000000ull, 0x10000000000ull },
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RVSERVER_FLASH_SECTOR_ਮਾਪ (256 * KiB)

ਸਥਿਰ ਖਾਲੀ rvserver_flash_maps(RISCVServerRefMachineState *s,
                                ਸਮ੍ਰਿਤੀ_Region *sysmem)
{
    hwaddr flashsize = rvserver_ref_memmap[RVSERVER_FLASH].ਮਾਪ / 2;
    hwaddr flashbase = rvserver_ref_memmap[RVSERVER_FLASH].ਅਧਾਰ;

    riscv_init_flash_map(s->flash[0], flashbase, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_ਮਾਪ);
    riscv_init_flash_map(s->flash[1], flashbase + flashsize, flashsize,
                         sysmem, RVSERVER_FLASH_SECTOR_ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_pmu(RISCVServerRefMachineState *s)
{
    g_autofree ਅੱਖਰ *pmu_name = g_strdup_printf("/pmu");
    MachineState *ms = MACHINE(s);
    RISCVCPU *hart = &s->soc[0].harts[0];

    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, pmu_name);
    qemu_fdt_setprop_string(ms->fdt, pmu_name, "compatible", "riscv,pmu");
    riscv_pmu_generate_fdt_ਗੰਢ(ms->fdt, hart->pmu_avail_ctrs, pmu_name);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_sockets(RISCVServerRefMachineState *s,
                               ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap,
                               uint32_t *phandle,
                               uint32_t *irq_mmio_phandle,
                               uint32_t *irq_pcie_phandle,
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
    ਲਈ (socket = (socket_ਗਿਣਤੀ - 1); socket >= 0; socket--) {
        hwaddr memaddr = s->memmap[RVSERVER_DRAM].ਅਧਾਰ +
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

        aclint_props.socket = socket;
        aclint_props.ਅੰਕ_harts = s->soc[socket].ਅੰਕ_harts;
        riscv_create_fdt_socket_aclint(ms->fdt, &aclint_props,
                                       &intc_phandles[phandle_pos]);
    }

    imsic_fdt_props.soc = &s->soc;
    imsic_fdt_props.socket_ਗਿਣਤੀ = socket_ਗਿਣਤੀ;
    imsic_fdt_props.smp_cpus = ms->smp.cpus;
    imsic_fdt_props.imsic_m_ਅਧਾਰ = memmap[RVSERVER_IMSIC_M].ਅਧਾਰ;
    imsic_fdt_props.imsic_s_ਅਧਾਰ = memmap[RVSERVER_IMSIC_S].ਅਧਾਰ;
    imsic_fdt_props.imsic_ਸਮੂਹ_max_ਮਾਪ = RVSERVER_IMSIC_ਸਮੂਹ_MAX_ਮਾਪ;
    imsic_fdt_props.irqchip_ਅੰਕ_msis = RVSERVER_IRQCHIP_ਅੰਕ_MSIS;
    imsic_fdt_props.aia_guests = s->aia_guests;

    riscv_create_fdt_imsic(ms->fdt, &imsic_fdt_props, phandle, intc_phandles,
                           &msi_m_phandle, &msi_s_phandle);
    *msi_pcie_phandle = msi_s_phandle;

    aplic_fdt_props.aplic_m = !kvm_enabled()
                              ? &memmap[RVSERVER_APLIC_M] : ਨਿਰਾ;
    aplic_fdt_props.aplic_s = &memmap[RVSERVER_APLIC_S];
    aplic_fdt_props.platform_bus = &memmap[RVSERVER_PLATFORM_BUS];
    aplic_fdt_props.platform_bus_irq = RVSERVER_PLATFORM_BUS_IRQ;
    aplic_fdt_props.ਅੰਕ_harts = ms->smp.cpus;
    aplic_fdt_props.numa_enabled = numa_enabled;
    aplic_fdt_props.irqchip_ਅੰਕ_sources = RVSERVER_IRQCHIP_ਅੰਕ_SOURCES;
    aplic_fdt_props.aia_type = AIA_TYPE_APLIC_IMSIC;

    phandle_pos = ms->smp.cpus;
    ਲਈ (socket = (socket_ਗਿਣਤੀ - 1); socket >= 0; socket--) {
        phandle_pos -= s->soc[socket].ਅੰਕ_harts;

        aplic_fdt_props.socket = socket;
        aplic_fdt_props.ਅੰਕ_harts = s->soc[socket].ਅੰਕ_harts;
        riscv_create_fdt_socket_aplic(ms->fdt, &aplic_fdt_props,
                                      msi_m_phandle, msi_s_phandle, phandle,
                                      &intc_phandles[phandle_pos],
                                      xplic_phandles);
    }

    ਲਈ (socket = 0; socket < socket_ਗਿਣਤੀ; socket++) {
        ਜੇ (socket == 0) {
            *irq_mmio_phandle = xplic_phandles[socket];
            *irq_pcie_phandle = xplic_phandles[socket];
        }
        ਜੇ (socket == 1) {
            *irq_pcie_phandle = xplic_phandles[socket];
        }
    }

    riscv_socket_fdt_write_ਦੂਰੀ_ਮੈਟ੍ਰਿਕਸ(ms);
}

ਸਥਿਰ ਖਾਲੀ finalize_fdt(RISCVServerRefMachineState *s)
{
    uint32_t phandle = 1, irq_mmio_phandle = 1, msi_pcie_phandle = 1;
    uint32_t irq_pcie_phandle = 1, iommu_sys_phandle;
    g_autofree ਅੱਖਰ *name = ਨਿਰਾ;
    MachineState *ms = MACHINE(s);

    create_fdt_sockets(s, rvserver_ref_memmap, &phandle, &irq_mmio_phandle,
                       &irq_pcie_phandle, &msi_pcie_phandle);

    iommu_sys_phandle = riscv_create_fdt_riscv_iommu_sys(ms->fdt,
                                         s->memmap[RVSERVER_IOMMU_SYS].ਅਧਾਰ,
                                         s->memmap[RVSERVER_IOMMU_SYS].ਮਾਪ,
                                         &phandle, irq_mmio_phandle,
                                         msi_pcie_phandle, IOMMU_SYS_IRQ);

    riscv_create_fdt_pcie(ms->fdt, AIA_TYPE_APLIC_IMSIC, true,
                          &s->memmap[RVSERVER_PCIE_ECAM],
                          &s->memmap[RVSERVER_PCIE_PIO],
                          &s->memmap[RVSERVER_PCIE_MMIO],
                          &s->memmap[RVSERVER_PCIE_MMIO_ਉੱਚਾ],
                          irq_pcie_phandle, msi_pcie_phandle,
                          iommu_sys_phandle, RVSERVER_PCIE_IRQ);

    riscv_create_fdt_syscon(ms->fdt, &phandle,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].ਅਧਾਰ,
                            rvserver_ref_memmap[RVSERVER_RESET_SYSCON].ਮਾਪ,
                            SYSCON_RESET, SYSCON_POWEROFF, false);

    riscv_create_fdt_uart(ms->fdt, &rvserver_ref_memmap[RVSERVER_UART0],
                          RVSERVER_UART0_IRQ, AIA_TYPE_APLIC_IMSIC,
                          irq_mmio_phandle);
    name = riscv_fdt_get_uart_nodename(rvserver_ref_memmap[RVSERVER_UART0].ਅਧਾਰ);
    qemu_fdt_setprop_string(ms->fdt, "/chosen", "stdout-path", name);
    qemu_fdt_setprop_string(ms->fdt, "/aliases", "serial0", name);

    riscv_create_fdt_rtc(ms->fdt, &rvserver_ref_memmap[RVSERVER_RTC],
                         RVSERVER_RTC_IRQ, AIA_TYPE_APLIC_IMSIC,
                         irq_mmio_phandle);
}

ਸਥਿਰ ਖਾਲੀ create_fdt(RISCVServerRefMachineState *s,
                       ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap)
{
    MachineState *ms = MACHINE(s);
    uint8_t rng_ਬੀਜ[32];
    g_autofree ਅੱਖਰ *name = ਨਿਰਾ;

    ms->fdt = riscv_create_board_device_ਰੁੱਖ(MACHINE_GET_CLASS(s)->desc,
                                             "qemu,riscv-server-ref",
                                             &s->fdt_ਮਾਪ);

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
                           s->memmap[RVSERVER_PCIE_ECAM].ਅਧਾਰ);
    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, name);

    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, "/chosen");
    qemu_fdt_ਜੋੜੋ_subnode(ms->fdt, "/aliases");

    /* Pass seed to RNG */
    qemu_guest_getrandom_nofail(rng_ਬੀਜ, ਆਕਾਰ(rng_ਬੀਜ));
    qemu_fdt_setprop(ms->fdt, "/chosen", "rng-seed",
                     rng_ਬੀਜ, ਆਕਾਰ(rng_ਬੀਜ));

    riscv_create_fdt_flash(ms->fdt, rvserver_ref_memmap[RVSERVER_FLASH].ਅਧਾਰ,
                           rvserver_ref_memmap[RVSERVER_FLASH].ਮਾਪ / 2);
    create_fdt_pmu(s);

}

ਸਥਿਰ uint64_t rvserver_reset_syscon_read(ਖਾਲੀ *opaque, hwaddr addr,
                                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ)
{
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ rvserver_reset_syscon_write(ਖਾਲੀ *opaque, hwaddr addr,
                                        uint64_t val64, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮਾਪ)
{
    ਚੋਣ (val64) {
    ਹਾਲਤ SYSCON_POWEROFF:
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_GUEST_SHUTDOWN);
        ਵਾਪਸ;
    ਹਾਲਤ SYSCON_RESET:
        qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);
        ਵਾਪਸ;
    ਮੂਲ:
        ਰੋਕੋ;
    }
}

ਸਥਿਰ ਅਚਲ ਸਮ੍ਰਿਤੀ_Region_Ops rvserver_reset_syscon_ops = {
    .ਪਠ = rvserver_reset_syscon_read,
    .ਲਿਖ = rvserver_reset_syscon_write,
    .endianness = DEVICE_LITTLE_ENDIAN
};

ਸਥਿਰ ਖਾਲੀ rvserver_ref_machine_ਪੂਰਾ(Notifier *notifier, ਖਾਲੀ *ਅੰਕੜੇ)
{
    RISCVServerRefMachineState *s = container_ਦਾ(
        notifier, RISCVServerRefMachineState, machine_ਪੂਰਾ);
    ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap = rvserver_ref_memmap;
    MachineState *machine = MACHINE(s);
    hwaddr ਸ਼ੁਰੂ_addr = memmap[RVSERVER_DRAM].ਅਧਾਰ;
    ਨਿਸ਼ਾਨਾ_ulong firmware_ਸਿਰਾ_addr, kernel_ਸ਼ੁਰੂ_addr;
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

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    firmware_ਸਿਰਾ_addr = riscv_ਲੱਭੋ_and_ਲੱਦੋ_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &ਸ਼ੁਰੂ_addr, ਨਿਰਾ);

    pflash_blk0 = pflash_cfi01_get_blk(s->flash[0]);
    ਜੇ (pflash_blk0) {
        ਜੇ (machine->firmware && !strcmp(machine->firmware, "none")) {
            /*
             * Pflash was supplied but bios is none and not KVM guest,
             * let's overwrite the address we jump to after reset to
             * the base of the flash.
             */
            ਸ਼ੁਰੂ_addr = rvserver_ref_memmap[RVSERVER_FLASH].ਅਧਾਰ;
        } ਨਹੀਂਤਾਂ {
            /*
             * Pflash was supplied but either KVM guest or bios is not none.
             * In this case, base of the flash would contain S-mode payload.
             */
            riscv_ਤਿਆਰੀ_firmware_boot(machine);
            kernel_ਇੰਦਰਾਜ = rvserver_ref_memmap[RVSERVER_FLASH].ਅਧਾਰ;
        }
    }

    ਜੇ (machine->kernel_filename && !kernel_ਇੰਦਰਾਜ) {
        kernel_ਸ਼ੁਰੂ_addr = riscv_calc_kernel_ਸ਼ੁਰੂ_addr(&boot_info,
                                                         firmware_ਸਿਰਾ_addr);
        riscv_ਲੱਦੋ_kernel(machine, &boot_info, kernel_ਸ਼ੁਰੂ_addr, true, ਨਿਰਾ);
        kernel_ਇੰਦਰਾਜ = boot_info.image_ਨੀਵਾਂ_addr;
    }

    fdt_ਲੱਦੋ_addr = riscv_compute_fdt_addr(memmap[RVSERVER_DRAM].ਅਧਾਰ,
                                           memmap[RVSERVER_DRAM].ਮਾਪ,
                                           machine, &boot_info);

    riscv_ਲੱਦੋ_fdt(fdt_ਲੱਦੋ_addr, machine->fdt);

    /* load the reset vector */
    riscv_ਤਿਆਰੀ_rom_reset_vec(machine, &s->soc[0], ਸ਼ੁਰੂ_addr,
                              rvserver_ref_memmap[RVSERVER_MROM].ਅਧਾਰ,
                              rvserver_ref_memmap[RVSERVER_MROM].ਮਾਪ,
                              kernel_ਇੰਦਰਾਜ,
                              fdt_ਲੱਦੋ_addr);
}

ਸਥਿਰ bool rvserver_aclint_allowed(ਖਾਲੀ)
{
    ਵਾਪਸ tcg_enabled() || qtest_enabled();
}

ਸਥਿਰ ਖਾਲੀ rvserver_ref_machine_init(MachineState *machine)
{
    ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap = rvserver_ref_memmap;
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(machine);
    ਸਮ੍ਰਿਤੀ_Region *system_ਸਮ੍ਰਿਤੀ = get_system_ਸਮ੍ਰਿਤੀ();
    ਸਮ੍ਰਿਤੀ_Region *ਨਕਾਬ_rom = g_new(ਸਮ੍ਰਿਤੀ_Region, 1);
    ਸਮ੍ਰਿਤੀ_Region *reset_syscon_io = g_new(ਸਮ੍ਰਿਤੀ_Region, 1);
    DeviceState *mmio_irqchip, *pcie_irqchip;
    ਪੂਰਨ i, ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ;
    ਪੂਰਨ socket_ਗਿਣਤੀ = riscv_socket_ਗਿਣਤੀ(machine);
    ਪੂਰਨ imsic_ਬਿੱਟ;

    /* Check socket count limit */
    ਜੇ (RVSERVER_SOCKETS_MAX < socket_ਗਿਣਤੀ) {
        error_report("number of sockets/nodes should be less than %d",
            RVSERVER_SOCKETS_MAX);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਜੇ (!rvserver_aclint_allowed()) {
        error_report("'aclint' is only available with TCG acceleration");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
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
    ਜੇ (qtest_enabled()) {
        s->aia_guests = 0;
    }

    imsic_ਬਿੱਟ = imsic_ਅੰਕ_ਬਿੱਟ(s->aia_guests + 1);

    /* Initialize sockets */
    mmio_irqchip = pcie_irqchip = ਨਿਰਾ;
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

        /* Per-socket ACLINT MTIMER */
        riscv_aclint_mtimer_create(system_ਸਮ੍ਰਿਤੀ,
                memmap[RVSERVER_ACLINT].ਅਧਾਰ + i * RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ,
                RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ,
                ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ,
                RISCV_ACLINT_DEFAULT_MTIMECMP,
                RISCV_ACLINT_DEFAULT_MTIME,
                RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, true);

        /* Per-socket interrupt controller */
        s->irqchip[i] = riscv_create_aia(system_ਸਮ੍ਰਿਤੀ, true, s->aia_guests,
                                         IMSIC_HART_ਮਾਪ(0),
                                         IMSIC_HART_ਮਾਪ(imsic_ਬਿੱਟ),
                                         RVSERVER_IRQCHIP_ਅੰਕ_SOURCES,
                                         &s->memmap[RVSERVER_APLIC_M],
                                         &s->memmap[RVSERVER_APLIC_S],
                                         &s->memmap[RVSERVER_IMSIC_M],
                                         &s->memmap[RVSERVER_IMSIC_S],
                                         i, ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ,
                                         RVSERVER_IRQCHIP_ਅੰਕ_MSIS,
                                         RVSERVER_IRQCHIP_ਅੰਕ_PRIO_ਬਿੱਟ);

        /* Try to use different IRQCHIP instance based device type */
        ਜੇ (i == 0) {
            mmio_irqchip = s->irqchip[i];
            pcie_irqchip = s->irqchip[i];
        }
        ਜੇ (i == 1) {
            pcie_irqchip = s->irqchip[i];
        }
    }

    /* register system main memory (actual RAM) */
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ, memmap[RVSERVER_DRAM].ਅਧਾਰ,
        machine->ram);

    /* boot rom */
    ਸਮ੍ਰਿਤੀ_region_init_rom(ਨਕਾਬ_rom, ਨਿਰਾ, "riscv_rvserver_ref_board.mrom",
                           memmap[RVSERVER_MROM].ਮਾਪ, &error_fatal);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ, memmap[RVSERVER_MROM].ਅਧਾਰ,
                                ਨਕਾਬ_rom);

    ਸਮ੍ਰਿਤੀ_region_init_io(reset_syscon_io, ਨਿਰਾ, &rvserver_reset_syscon_ops,
                          ਨਿਰਾ, "reset_syscon_io",
                          memmap[RVSERVER_RESET_SYSCON].ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ,
                                memmap[RVSERVER_RESET_SYSCON].ਅਧਾਰ,
                                reset_syscon_io);

    riscv_gpex_pcie_init(system_ਸਮ੍ਰਿਤੀ, pcie_irqchip,
                         &rvserver_ref_memmap[RVSERVER_PCIE_ECAM],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO],
                         &rvserver_ref_memmap[RVSERVER_PCIE_MMIO_ਉੱਚਾ],
                         &rvserver_ref_memmap[RVSERVER_PCIE_PIO],
                         RVSERVER_PCIE_IRQ);

    s->platform_bus_dev = riscv_create_platform_bus(mmio_irqchip,
                                                    &s->memmap[RVSERVER_PLATFORM_BUS],
                                                    RVSERVER_PLATFORM_BUS_IRQ,
                                                    RVSERVER_PLATFORM_BUS_ਅੰਕ_IRQS);

    serial_mm_init(system_ਸਮ੍ਰਿਤੀ, memmap[RVSERVER_UART0].ਅਧਾਰ,
        0, qdev_get_gpio_ਵਿੱਚ(mmio_irqchip, RVSERVER_UART0_IRQ), 399193,
        serial_hd(0), DEVICE_LITTLE_ENDIAN);

    sysbus_create_simple("goldfish_rtc", memmap[RVSERVER_RTC].ਅਧਾਰ,
        qdev_get_gpio_ਵਿੱਚ(mmio_irqchip, RVSERVER_RTC_IRQ));

    ਲਈ (i = 0; i < ARRAY_ਮਾਪ(s->flash); i++) {
        /* Map legacy -drive if=pflash to machine properties */
        pflash_cfi01_legacy_drive(s->flash[i],
                                  drive_get(IF_PFLASH, 0, i));
    }
    rvserver_flash_maps(s, system_ਸਮ੍ਰਿਤੀ);

    /* load/create device tree */
    ਜੇ (machine->dtb) {
        machine->fdt = ਲੱਦੋ_device_ਰੁੱਖ(machine->dtb, &s->fdt_ਮਾਪ);
        ਜੇ (!machine->fdt) {
            error_report("load_device_tree() failed");
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
    } ਨਹੀਂਤਾਂ {
        create_fdt(s, memmap);
    }

    riscv_create_iommu_sys(mmio_irqchip, s->memmap[RVSERVER_IOMMU_SYS].ਅਧਾਰ,
                           IOMMU_SYS_IRQ, false);

    s->machine_ਪੂਰਾ.notify = rvserver_ref_machine_ਪੂਰਾ;
    qemu_ਜੋੜੋ_machine_init_ਪੂਰਾ_notifier(&s->machine_ਪੂਰਾ);
}

ਸਥਿਰ ਖਾਲੀ rvserver_ref_machine_instance_init(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->flash[0] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash0",
                                     "pflash0", RVSERVER_FLASH_SECTOR_ਮਾਪ);
    s->flash[1] = riscv_flash_create(OBJECT(s), "riscv-server-ref.flash1",
                                     "pflash1", RVSERVER_FLASH_SECTOR_ਮਾਪ);

    s->aia_guests = RVSERVER_IRQCHIP_DEFAULT_AIA_GUESTS;
}

ਸਥਿਰ ਖਾਲੀ rvserver_ref_machine_instance_finalize(Object *obj)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    ਲਈ (ਪੂਰਨ i = 0; i < ARRAY_ਮਾਪ(s->flash); i++) {
        ਜੇ (s->flash[i] && !qdev_is_realized(DEVICE(s->flash[i]))) {
            object_unref(OBJECT(s->flash[i]));
        }
    }
}

ਸਥਿਰ ਅੱਖਰ *rvserver_ref_get_aia_guests(Object *obj, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);
    ਅੱਖਰ val[32];

    sprintf(val, "%d", s->aia_guests);
    ਵਾਪਸ g_strdup(val);
}

ਸਥਿਰ ਖਾਲੀ rvserver_ref_ਸੈੱਟ_aia_guests(Object *obj, ਅਚਲ ਅੱਖਰ *val,
                                        Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(obj);

    s->aia_guests = atoi(val);
    ਜੇ (s->aia_guests < 0 || s->aia_guests > RVSERVER_IRQCHIP_MAX_GUESTS) {
        error_setg(errp, "Invalid number of AIA IMSIC guests");
        error_append_hint(errp, "Valid values be between 0 and %d.\n",
                          RVSERVER_IRQCHIP_MAX_GUESTS);
    }
}

ਸਥਿਰ ਅਚਲ HotplugHandler *
rvserver_machine_get_hotplug_handler(MachineState *ms, DeviceState *dev)
{
    MachineClass *mc = MACHINE_GET_CLASS(ms);

    ਜੇ (device_is_dynamic_sysbus(mc, dev)) {
        ਵਾਪਸ HOTPLUG_HANDLER(ms);
    }

    ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਖਾਲੀ rvserver_machine_device_plug_cb(ਅਚਲ HotplugHandler *hotplug_dev,
                                            DeviceState *dev, Error **errp)
{
    RISCVServerRefMachineState *s = RISCV_SERVER_REF_MACHINE(hotplug_dev);

    ਜੇ (s->platform_bus_dev) {
        MachineClass *mc = MACHINE_GET_CLASS(s);

        ਜੇ (device_is_dynamic_sysbus(mc, dev)) {
            platform_bus_link_device(PLATFORM_BUS_DEVICE(s->platform_bus_dev),
                                     SYS_BUS_DEVICE(dev));
        }
    }
}

ਸਥਿਰ ਖਾਲੀ rvserver_ref_machine_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    ਅੱਖਰ str[128];
    MachineClass *mc = MACHINE_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ valid_cpu_types[] = {
        TYPE_RISCV_CPU_RVSERVER_REF,
        ਨਿਰਾ
    };

    /* Note: this is used as /model in the FDT.  */
    mc->desc = "RISC-V Server Platform Reference Board v1.0";
    mc->init = rvserver_ref_machine_init;
    mc->max_cpus = RVSERVER_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_RVSERVER_REF;
    mc->valid_cpu_types = valid_cpu_types;
    mc->pci_allow_0_ਪਤਾ = true;
    mc->default_nic = "e1000e";
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_instance_props = riscv_numa_cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_props;
    mc->get_default_cpu_ਗੰਢ_id = riscv_numa_get_default_cpu_ਗੰਢ_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv_rvserver_ref_board.ram";

    object_class_property_ਜੋੜੋ_str(oc, "aia-guests",
                                  rvserver_ref_get_aia_guests,
                                  rvserver_ref_ਸੈੱਟ_aia_guests);
    sprintf(str, "Set number of guest MMIO pages for AIA IMSIC. Valid value "
                 "should be between 0 and %d.", RVSERVER_IRQCHIP_MAX_GUESTS);
    object_class_property_ਸੈੱਟ_description(oc, "aia-guests", str);

    ਪ੍ਰਤਿਜ੍ਞਾ(!mc->get_hotplug_handler);
    mc->get_hotplug_handler = rvserver_machine_get_hotplug_handler;
    hc->plug = rvserver_machine_device_plug_cb;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TPM
    machine_class_allow_dynamic_sysbus_dev(mc, TYPE_TPM_TIS_SYSBUS);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਅਚਲ TypeInfo rvserver_ref_typeinfo = {
    .name       = TYPE_RISCV_SERVER_REF_MACHINE,
    .ਮਾਪਾ     = TYPE_MACHINE,
    .class_init = rvserver_ref_machine_class_init,
    .instance_init = rvserver_ref_machine_instance_init,
    .instance_finalize = rvserver_ref_machine_instance_finalize,
    .instance_ਮਾਪ = ਆਕਾਰ(RISCVServerRefMachineState),
    .interfaces = (ਅਚਲ InterfaceInfo[]) {
         { TYPE_HOTPLUG_HANDLER },
         { }
    },
};

ਸਥਿਰ ਖਾਲੀ rvserver_ref_init_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&rvserver_ref_typeinfo);
}

type_init(rvserver_ref_init_ਰਜਿਸਟਰ_types)
