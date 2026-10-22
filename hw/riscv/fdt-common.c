/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus-fdt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/fdt-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu_bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv-iommu-bits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/iommu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_imsic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci/pcie_host.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/aia.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_STD_CLOCK_FREQ 3686400 /* 3.6864 MHz */

ਖਾਲੀ *riscv_create_board_device_ਰੁੱਖ(ਅਚਲ ਅੱਖਰ *model, ਅਚਲ ਅੱਖਰ *compatible,
                                     ਪੂਰਨ *fdt_ਮਾਪ)
{
    ਖਾਲੀ *fdt = create_device_ਰੁੱਖ(fdt_ਮਾਪ);

    ਜੇ (!fdt) {
        error_report("create_device_tree() failed");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", model);
    qemu_fdt_setprop_string(fdt, "/", "compatible", compatible);
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", ਨਿਰਾ, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    ਵਾਪਸ fdt;
}

ਖਾਲੀ riscv_create_fdt_socket_ਸਮ੍ਰਿਤੀ(ਖਾਲੀ *fdt, hwaddr addr, uint64_t ਮਾਪ,
                                    ਪੂਰਨ socket_id, bool numa_enabled)
{
    g_autofree ਅੱਖਰ *mem_name = g_strdup_printf("/memory@%"HWADDR_PRIx, addr);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, mem_name);
    qemu_fdt_setprop_sized_cells(fdt, mem_name, "reg", 2, addr, 2, ਮਾਪ);
    qemu_fdt_setprop_string(fdt, mem_name, "device_type", "memory");

    ਜੇ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, mem_name, "numa-node-id", socket_id);
    }
}

ਖਾਲੀ riscv_create_fdt_socket_clint(ਖਾਲੀ *fdt, hwaddr addr, uint64_t ਮਾਪ,
                                   ਪੂਰਨ socket_id, uint32_t *intc_phandles,
                                   ਪੂਰਨ ਅੰਕ_harts, bool numa_enabled)
{
    g_autofree uint32_t *clint_cells = g_new0(uint32_t, ਅੰਕ_harts * 4);
    g_autofree ਅੱਖਰ *clint_name = ਨਿਰਾ;
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ clint_compat[2] = {
        "sifive,clint0", "riscv,clint0"
    };

    ਲਈ (ਪੂਰਨ cpu = 0; cpu < ਅੰਕ_harts; cpu++) {
        clint_cells[cpu * 4 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_M_SOFT);
        clint_cells[cpu * 4 + 2] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 3] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_M_TIMER);
    }

    clint_name = g_strdup_printf("/soc/clint@%"HWADDR_PRIx, addr);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, clint_name);
    qemu_fdt_setprop_string_array(fdt, clint_name, "compatible",
                                  (ਅੱਖਰ **)&clint_compat,
                                  ARRAY_ਮਾਪ(clint_compat));
    qemu_fdt_setprop_sized_cells(fdt, clint_name, "reg",
                                 2, addr, 2, ਮਾਪ);
    qemu_fdt_setprop(fdt, clint_name, "interrupts-extended",
                     clint_cells, ਅੰਕ_harts * ਆਕਾਰ(uint32_t) * 4);

    ਜੇ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, clint_name, "numa-node-id", socket_id);
    }
}

ਖਾਲੀ riscv_fdt_create_cpu_socket_subnode(ਖਾਲੀ *fdt,
                                         uint64_t timebase_frequency)
{
    qemu_fdt_ਜੋੜੋ_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency",
                          timebase_frequency);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, "/cpus/cpu-map");
}

ਸਥਿਰ ਖਾਲੀ
create_fdt_socket_cpu_ਅੰਦਰੂਨੀ(ਖਾਲੀ *fdt, ਅੱਖਰ *clust_name, RISCVCPU *cpu_ptr,
                               ਪੂਰਨ cpu, ਪੂਰਨ socket_id, ਪੂਰਨ socket_hartid_ਅਧਾਰ,
                               uint32_t *phandle, uint32_t *intc_phandles,
                               bool numa_enabled, bool is_32_ਦੋਆਧਾਰੀ_ਅੰਕ)
{
    g_autofree ਅੱਖਰ *cpu_name = ਨਿਰਾ;
    g_autofree ਅੱਖਰ *core_name = ਨਿਰਾ;
    g_autofree ਅੱਖਰ *intc_name = ਨਿਰਾ;
    uint32_t cpu_phandle = (*phandle)++;
    bool is_sifive_u = cpu_ptr == ਨਿਰਾ;

    cpu_name = g_strdup_printf("/cpus/cpu@%d", socket_hartid_ਅਧਾਰ + cpu);

    /*
     * The sifive_u board has an exclusive satp and riscv,isa
     * schema that can't be shared with other boards, so part
     * of the CPU FDT creation (i.e. the /cpus/cpu@N subnode)
     * is still being done by the board.
     */
    ਜੇ (!is_sifive_u) {
        int8_t satp_ਬਹੁਲਕ_max = cpu_ptr->cfg.max_satp_ਬਹੁਲਕ;

        qemu_fdt_ਜੋੜੋ_subnode(fdt, cpu_name);

        ਜੇ (satp_ਬਹੁਲਕ_max != -1) {
            g_autofree ਅੱਖਰ *sv_name = ਨਿਰਾ;
            sv_name = g_strdup_printf("riscv,%s",
                                      satp_ਬਹੁਲਕ_str(satp_ਬਹੁਲਕ_max, is_32_ਦੋਆਧਾਰੀ_ਅੰਕ));
            qemu_fdt_setprop_string(fdt, cpu_name, "mmu-type", sv_name);
        }
        riscv_isa_write_fdt(cpu_ptr, fdt, cpu_name);

        ਜੇ (cpu_ptr->cfg.ext_zicbom) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbom-block-size",
                                  cpu_ptr->cfg.cbom_blocksize);
        }

        ਜੇ (cpu_ptr->cfg.ext_zicboz) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cboz-block-size",
                                  cpu_ptr->cfg.cboz_blocksize);
        }

        ਜੇ (cpu_ptr->cfg.ext_zicbop) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbop-block-size",
                                  cpu_ptr->cfg.cbop_blocksize);
        }
    }

    qemu_fdt_setprop_string(fdt, cpu_name, "compatible", "riscv");
    qemu_fdt_setprop_string(fdt, cpu_name, "status", "okay");
    qemu_fdt_setprop_cell(fdt, cpu_name, "reg",
                          socket_hartid_ਅਧਾਰ + cpu);
    qemu_fdt_setprop_string(fdt, cpu_name, "device_type", "cpu");
    ਜੇ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, cpu_name, "numa-node-id", socket_id);
    }
    qemu_fdt_setprop_cell(fdt, cpu_name, "phandle", cpu_phandle);

    intc_phandles[cpu] = (*phandle)++;

    intc_name = g_strdup_printf("%s/interrupt-controller", cpu_name);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, intc_name);
    qemu_fdt_setprop_cell(fdt, intc_name, "phandle",
                          intc_phandles[cpu]);
    qemu_fdt_setprop_string(fdt, intc_name, "compatible",
                            "riscv,cpu-intc");
    qemu_fdt_setprop(fdt, intc_name, "interrupt-controller", ਨਿਰਾ, 0);
    qemu_fdt_setprop_cell(fdt, intc_name, "#interrupt-cells", 1);

    core_name = g_strdup_printf("%s/core%d", clust_name, cpu);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, core_name);
    qemu_fdt_setprop_cell(fdt, core_name, "cpu", cpu_phandle);
}

ਖਾਲੀ riscv_create_fdt_socket_cpus(ਖਾਲੀ *fdt, RISCVCPU *socket_harts,
                                  ਪੂਰਨ socket_id, ਪੂਰਨ ਅੰਕ_harts_socket,
                                  ਪੂਰਨ socket_hartid_ਅਧਾਰ, uint32_t *phandle,
                                  uint32_t *intc_phandles, bool numa_enabled,
                                  bool is_32_ਦੋਆਧਾਰੀ_ਅੰਕ)
{
    g_autofree ਅੱਖਰ *clust_name = ਨਿਰਾ;

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", socket_id);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, clust_name);

    ਲਈ (ਪੂਰਨ cpu = ਅੰਕ_harts_socket - 1; cpu >= 0; cpu--) {
        RISCVCPU *cpu_ptr = &socket_harts[cpu];

        create_fdt_socket_cpu_ਅੰਦਰੂਨੀ(fdt, clust_name, cpu_ptr, cpu,
                                       socket_id, socket_hartid_ਅਧਾਰ,
                                       phandle, intc_phandles, numa_enabled,
                                       is_32_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }
}

ਖਾਲੀ
riscv_create_fdt_socket_cpu_sifive(ਖਾਲੀ *fdt, ਅੱਖਰ *clust_name,
                                   ਪੂਰਨ cpu_id, ਪੂਰਨ socket_id,
                                   ਪੂਰਨ socket_hartid_ਅਧਾਰ, uint32_t *phandle,
                                   uint32_t *intc_phandles)
{
    create_fdt_socket_cpu_ਅੰਦਰੂਨੀ(fdt, clust_name, ਨਿਰਾ, cpu_id,
                                   socket_id, socket_hartid_ਅਧਾਰ,
                                   phandle, intc_phandles, false, false);
}

ਖਾਲੀ riscv_create_fdt_plic(ਖਾਲੀ *fdt, hwaddr addr, uint64_t ਮਾਪ,
                           uint32_t plic_phandle, uint32_t int_cells,
                           uint32_t addr_cells, uint32_t *plic_cells,
                           uint32_t cells_ਮਾਪ, uint32_t ndev_sources,
                           bool numa_enabled, ਪੂਰਨ socket_id)
{
    g_autofree ਅੱਖਰ *nodename = ਨਿਰਾ;
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ plic_compat[2] = {
        "sifive,plic-1.0.0", "riscv,plic0"
    };

    nodename = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx, addr);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", int_cells);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", addr_cells);
    qemu_fdt_setprop_string_array(fdt, nodename, "compatible",
        (ਅੱਖਰ **)&plic_compat, ARRAY_ਮਾਪ(plic_compat));
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", ਨਿਰਾ, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
                     plic_cells, cells_ਮਾਪ);
    qemu_fdt_setprop_sized_cells(fdt, nodename, "reg",
                                 2, addr, 2, ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", ndev_sources);
    ਜੇ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, nodename, "numa-node-id", socket_id);
    }
    qemu_fdt_setprop_cell(fdt, nodename, "phandle", plic_phandle);
}

/*
 * To keep it simple, any event can be mapped to any programmable counters in
 * QEMU. The generic cycle & instruction count events can also be monitored
 * using programmable counters. In that case, mcycle & minstret must continue
 * to provide the correct value as well. Heterogeneous PMU per hart is not
 * supported yet. Thus, number of counters are same across all harts.
 */
ਖਾਲੀ riscv_pmu_generate_fdt_ਗੰਢ(ਖਾਲੀ *fdt, uint32_t cmask, ਅੱਖਰ *pmu_name)
{
    uint32_t fdt_event_ctr_map[15] = {};

   /*
    * The event encoding is specified in the SBI specification
    * Event idx is a 20bits wide number encoded as follows:
    * event_idx[19:16] = type
    * event_idx[15:0] = code
    * The code field in cache events are encoded as follows:
    * event_idx.code[15:3] = cache_id
    * event_idx.code[2:1] = op_id
    * event_idx.code[0:0] = result_id
    */

   /* SBI_PMU_HW_CPU_CYCLES: 0x01 : type(0x00) */
   fdt_event_ctr_map[0] = cpu_ਮੰਜ਼ਿਲ_be32(0x00000001);
   fdt_event_ctr_map[1] = cpu_ਮੰਜ਼ਿਲ_be32(0x00000001);
   fdt_event_ctr_map[2] = cpu_ਮੰਜ਼ਿਲ_be32(cmask | 1 << 0);

   /* SBI_PMU_HW_INSTRUCTIONS: 0x02 : type(0x00) */
   fdt_event_ctr_map[3] = cpu_ਮੰਜ਼ਿਲ_be32(0x00000002);
   fdt_event_ctr_map[4] = cpu_ਮੰਜ਼ਿਲ_be32(0x00000002);
   fdt_event_ctr_map[5] = cpu_ਮੰਜ਼ਿਲ_be32(cmask | 1 << 2);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[6] = cpu_ਮੰਜ਼ਿਲ_be32(0x00010019);
   fdt_event_ctr_map[7] = cpu_ਮੰਜ਼ਿਲ_be32(0x00010019);
   fdt_event_ctr_map[8] = cpu_ਮੰਜ਼ਿਲ_be32(cmask);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 WRITE : 0x01 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[9] = cpu_ਮੰਜ਼ਿਲ_be32(0x0001001B);
   fdt_event_ctr_map[10] = cpu_ਮੰਜ਼ਿਲ_be32(0x0001001B);
   fdt_event_ctr_map[11] = cpu_ਮੰਜ਼ਿਲ_be32(cmask);

   /* SBI_PMU_HW_CACHE_ITLB : 0x04 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[12] = cpu_ਮੰਜ਼ਿਲ_be32(0x00010021);
   fdt_event_ctr_map[13] = cpu_ਮੰਜ਼ਿਲ_be32(0x00010021);
   fdt_event_ctr_map[14] = cpu_ਮੰਜ਼ਿਲ_be32(cmask);

   /* This a OpenSBI specific DT property documented in OpenSBI docs */
   qemu_fdt_setprop(fdt, pmu_name, "riscv,event-to-mhpmcounters",
                    fdt_event_ctr_map, ਆਕਾਰ(fdt_event_ctr_map));
}

ਖਾਲੀ riscv_create_fdt_flash(ਖਾਲੀ *fdt, hwaddr flashbase, hwaddr flashsize)
{
    g_autofree ਅੱਖਰ *name = g_strdup_printf("/flash@%" PRIx64, flashbase);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "cfi-flash");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, flashbase, 2, flashsize,
                                 2, flashbase + flashsize, 2, flashsize);
    qemu_fdt_setprop_cell(fdt, name, "bank-width", 4);
}

/*
 * @sifive_test_compat is used to create a FDT that declares
 * compat with "sifive,test1" and "sifive,test0".  This happens
 * to be the case for the 'virt' machine that also creates a
 * 'sifive_test' syscon device.
 */
ਖਾਲੀ riscv_create_fdt_syscon(ਖਾਲੀ *fdt, uint32_t *ਅਗਲਾ_phandle,
                             hwaddr addr, hwaddr ਮਾਪ,
                             uint32_t reboot, uint32_t poweroff,
                             bool sifive_test_compat)
{
    uint32_t syscon_phandle;
    ਅੱਖਰ *name;

    name = g_strdup_printf("/soc/syscon@%"HWADDR_PRIx, addr);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);

    ਜੇ (sifive_test_compat) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ compat[3] = {
            "sifive,test1", "sifive,test0", "syscon"
        };

        qemu_fdt_setprop_string_array(fdt, name, "compatible",
                                      (ਅੱਖਰ **)&compat, ARRAY_ਮਾਪ(compat));
    } ਨਹੀਂਤਾਂ {
        qemu_fdt_setprop_string(fdt, name, "compatible", "syscon");
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr,
                                 2, ਮਾਪ);

    syscon_phandle = ਅਗਲਾ_phandle ? (*ਅਗਲਾ_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, name, "phandle", syscon_phandle);

    g_free(name);

    name = g_strdup_printf("/reboot");
    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-reboot");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", reboot);
    g_free(name);

    name = g_strdup_printf("/poweroff");
    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-poweroff");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", poweroff);
    g_free(name);
}

uint32_t riscv_create_fdt_riscv_iommu_sys(ਖਾਲੀ *fdt, hwaddr addr, hwaddr ਮਾਪ,
                                          uint32_t *ਅਗਲਾ_phandle,
                                          uint32_t irq_chip,
                                          uint32_t msi_phandle,
                                          uint32_t iommu_sys_irq)
{
    ਅਚਲ ਅੱਖਰ comp[] = "riscv,iommu";
    g_autofree ਅੱਖਰ *iommu_ਗੰਢ = ਨਿਰਾ;
    uint32_t iommu_phandle;

    iommu_ਗੰਢ = g_strdup_printf("/soc/iommu@%"HWADDR_PRIx, addr);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, iommu_ਗੰਢ);

    qemu_fdt_setprop(fdt, iommu_ਗੰਢ, "compatible", comp, ਆਕਾਰ(comp));
    qemu_fdt_setprop_cell(fdt, iommu_ਗੰਢ, "#iommu-cells", 1);

    iommu_phandle = ਅਗਲਾ_phandle ? (*ਅਗਲਾ_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, iommu_ਗੰਢ, "phandle", iommu_phandle);

    qemu_fdt_setprop_sized_cells(fdt, iommu_ਗੰਢ, "reg", 2, addr, 2, ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, iommu_ਗੰਢ, "interrupt-parent", irq_chip);

    qemu_fdt_setprop_cells(fdt, iommu_ਗੰਢ, "interrupts",
        iommu_sys_irq + RISCV_IOMMU_INTR_CQ, FDT_IRQ_TYPE_ਕਿਨਾਰਾ_ਨੀਵਾਂ,
        iommu_sys_irq + RISCV_IOMMU_INTR_FQ, FDT_IRQ_TYPE_ਕਿਨਾਰਾ_ਨੀਵਾਂ,
        iommu_sys_irq + RISCV_IOMMU_INTR_PM, FDT_IRQ_TYPE_ਕਿਨਾਰਾ_ਨੀਵਾਂ,
        iommu_sys_irq + RISCV_IOMMU_INTR_PQ, FDT_IRQ_TYPE_ਕਿਨਾਰਾ_ਨੀਵਾਂ);

    qemu_fdt_setprop_cell(fdt, iommu_ਗੰਢ, "msi-parent", msi_phandle);

    ਵਾਪਸ iommu_phandle;
}

ਸਥਿਰ ਖਾਲੀ create_pcie_irq_map(ਖਾਲੀ *fdt, ਅੱਖਰ *nodename,
                                uint32_t irqchip_phandle,
                                RISCVAIAType aia_type, uint32_t pcie_irq)
{
    ਪੂਰਨ pin, dev;
    uint32_t irq_map_stride = 0;
    uint32_t ਭਰਿਆ_irq_map[PCI_ਅੰਕ_PINS * PCI_ਅੰਕ_PINS *
                          FDT_MAX_INT_MAP_ਚੌੜਾਈ] = {};
    uint32_t *irq_map = ਭਰਿਆ_irq_map;

    /*
     * This code creates a standard swizzle of interrupts such that
     * each device's first interrupt is based on it's PCI_SLOT number.
     * (See pci_swizzle_map_irq_fn())
     *
     * We only need one entry per interrupt in the table (not one per
     * possible slot) seeing the interrupt-map-mask will allow the table
     * to wrap to any number of devices.
     */
    ਲਈ (dev = 0; dev < PCI_ਅੰਕ_PINS; dev++) {
        ਪੂਰਨ devfn = dev * 0x8;

        ਲਈ (pin = 0; pin < PCI_ਅੰਕ_PINS; pin++) {
            ਪੂਰਨ irq_nr = pcie_irq + ((pin + PCI_ਥਾਂ(devfn)) % PCI_ਅੰਕ_PINS);
            ਪੂਰਨ i = 0;

            /* Fill PCI address cells */
            irq_map[i] = cpu_ਮੰਜ਼ਿਲ_be32(devfn << 8);
            i += FDT_PCI_ADDR_CELLS;

            /* Fill PCI Interrupt cells */
            irq_map[i] = cpu_ਮੰਜ਼ਿਲ_be32(pin + 1);
            i += FDT_PCI_INT_CELLS;

            /* Fill interrupt controller phandle and cells */
            irq_map[i++] = cpu_ਮੰਜ਼ਿਲ_be32(irqchip_phandle);
            irq_map[i++] = cpu_ਮੰਜ਼ਿਲ_be32(irq_nr);

            ਜੇ (aia_type != AIA_TYPE_NONE) {
                irq_map[i++] = cpu_ਮੰਜ਼ਿਲ_be32(0x4);
            }

            ਜੇ (!irq_map_stride) {
                irq_map_stride = i;
            }
            irq_map += irq_map_stride;
        }
    }

    qemu_fdt_setprop(fdt, nodename, "interrupt-map", ਭਰਿਆ_irq_map,
                     PCI_ਅੰਕ_PINS * PCI_ਅੰਕ_PINS *
                     irq_map_stride * ਆਕਾਰ(uint32_t));

    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-map-mask",
                           0x1800, 0, 0, 0x7);
}

/*
 * NOTE: this function uses a "/soc/pci@..." FDT subnode that
 * should be created beforehand.
 */
ਖਾਲੀ riscv_create_fdt_pcie(ਖਾਲੀ *fdt, ਪੂਰਨ aia_type, bool has_iommu_sys,
                           ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *pcie_ecam,
                           ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *pcie_pio,
                           ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *pcie_mmio,
                           ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *ਉੱਚਾ_pcie,
                           uint32_t irq_pcie_phandle,
                           uint32_t msi_pcie_phandle,
                           uint32_t iommu_sys_phandle, uint32_t pcie_irq)
{
    g_autofree ਅੱਖਰ *name = ਨਿਰਾ;

    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx, pcie_ecam->ਅਧਾਰ);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", FDT_PCI_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", FDT_PCI_INT_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0x2);
    qemu_fdt_setprop_string(fdt, name, "compatible", "pci-host-ecam-generic");
    qemu_fdt_setprop_string(fdt, name, "device_type", "pci");
    qemu_fdt_setprop_cell(fdt, name, "linux,pci-domain", 0);

    qemu_fdt_setprop_cells(fdt, name, "bus-range", 0,
                           pcie_ecam->ਮਾਪ / PCIE_MMCFG_ਮਾਪ_MIN - 1);
    qemu_fdt_setprop(fdt, name, "dma-coherent", ਨਿਰਾ, 0);

    ਜੇ (aia_type == AIA_TYPE_APLIC_IMSIC) {
        qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_pcie_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2,
                                 pcie_ecam->ਅਧਾਰ, 2, pcie_ecam->ਮਾਪ);

    qemu_fdt_setprop_sized_cells(fdt, name, "ranges",
        1, FDT_PCI_RANGE_IOPORT, 2, 0,
        2, pcie_pio->ਅਧਾਰ, 2, pcie_pio->ਮਾਪ,
        1, FDT_PCI_RANGE_MMIO,
        2, pcie_mmio->ਅਧਾਰ,
        2, pcie_mmio->ਅਧਾਰ, 2, pcie_mmio->ਮਾਪ,
        1, FDT_PCI_RANGE_MMIO_64_ਦੋਆਧਾਰੀ_ਅੰਕ,
        2, ਉੱਚਾ_pcie->ਅਧਾਰ,
        2, ਉੱਚਾ_pcie->ਅਧਾਰ, 2, ਉੱਚਾ_pcie->ਮਾਪ);

    ਜੇ (has_iommu_sys) {
        qemu_fdt_setprop_cells(fdt, name, "iommu-map",
                               0, iommu_sys_phandle, 0, 0x10000);
    }

    create_pcie_irq_map(fdt, name, irq_pcie_phandle, aia_type, pcie_irq);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_one_imsic(ਖਾਲੀ *fdt, IMSICFdtProps *props,
                                 hwaddr ਅਧਾਰ_addr,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 bool m_ਬਹੁਲਕ, uint32_t imsic_guest_ਬਿੱਟ)
{
    RISCVHartArrayState *soc = (RISCVHartArrayState *)props->soc;
    g_autofree ਅੱਖਰ *imsic_name = ਨਿਰਾ;
    g_autofree uint32_t *imsic_cells = ਨਿਰਾ;
    g_autofree uint32_t *imsic_regs = ਨਿਰਾ;
    uint32_t imsic_max_hart_ਪ੍ਰਤੀ_socket = 0;
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ imsic_compat[2] = {
        "qemu,imsics", "riscv,imsics"
    };

    imsic_cells = g_new0(uint32_t, props->smp_cpus * 2);
    imsic_regs = g_new0(uint32_t, props->socket_ਗਿਣਤੀ * 4);

    ਲਈ (ਪੂਰਨ cpu = 0; cpu < props->smp_cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(m_ਬਹੁਲਕ ? IRQ_M_EXT : IRQ_S_EXT);
    }

    ਲਈ (ਪੂਰਨ socket = 0; socket < props->socket_ਗਿਣਤੀ; socket++) {
        hwaddr imsic_addr = ਅਧਾਰ_addr + socket * props->imsic_ਸਮੂਹ_max_ਮਾਪ;
        uint32_t imsic_ਮਾਪ = IMSIC_HART_ਮਾਪ(imsic_guest_ਬਿੱਟ) *
                              soc[socket].ਅੰਕ_harts;

        imsic_regs[socket * 4 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(imsic_addr >> 32);
        imsic_regs[socket * 4 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(imsic_addr);
        imsic_regs[socket * 4 + 2] = 0;
        imsic_regs[socket * 4 + 3] = cpu_ਮੰਜ਼ਿਲ_be32(imsic_ਮਾਪ);
        ਜੇ (imsic_max_hart_ਪ੍ਰਤੀ_socket < soc[socket].ਅੰਕ_harts) {
            imsic_max_hart_ਪ੍ਰਤੀ_socket = soc[socket].ਅੰਕ_harts;
        }
    }

    imsic_name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                                 ਅਧਾਰ_addr);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, imsic_name);

    qemu_fdt_setprop_string_array(fdt, imsic_name, "compatible",
                                  (ਅੱਖਰ **)&imsic_compat,
                                  ARRAY_ਮਾਪ(imsic_compat));

    qemu_fdt_setprop_cell(fdt, imsic_name, "#interrupt-cells",
                          FDT_IMSIC_INT_CELLS);
    qemu_fdt_setprop(fdt, imsic_name, "interrupt-controller", ਨਿਰਾ, 0);
    qemu_fdt_setprop(fdt, imsic_name, "msi-controller", ਨਿਰਾ, 0);
    qemu_fdt_setprop(fdt, imsic_name, "interrupts-extended",
                     imsic_cells, props->smp_cpus * ਆਕਾਰ(uint32_t) * 2);
    qemu_fdt_setprop(fdt, imsic_name, "reg", imsic_regs,
                     props->socket_ਗਿਣਤੀ * ਆਕਾਰ(uint32_t) * 4);
    qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,num-ids",
                          props->irqchip_ਅੰਕ_msis);

    ਜੇ (imsic_guest_ਬਿੱਟ) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,guest-index-bits",
                              imsic_guest_ਬਿੱਟ);
    }

    ਜੇ (props->socket_ਗਿਣਤੀ > 1) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,hart-index-bits",
                              imsic_ਅੰਕ_ਬਿੱਟ(imsic_max_hart_ਪ੍ਰਤੀ_socket));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-bits",
                              imsic_ਅੰਕ_ਬਿੱਟ(props->socket_ਗਿਣਤੀ));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-shift",
                              IMSIC_MMIO_ਸਮੂਹ_MIN_ਖਿਸਕਾਓ);
    }
    qemu_fdt_setprop_cell(fdt, imsic_name, "phandle", msi_phandle);
}

ਖਾਲੀ riscv_create_fdt_imsic(ਖਾਲੀ *fdt, IMSICFdtProps *props,
                            uint32_t *ਅਗਲਾ_phandle, uint32_t *intc_phandles,
                            uint32_t *msi_m_phandle, uint32_t *msi_s_phandle)
{
    ਜੇ (ਅਗਲਾ_phandle) {
        *msi_m_phandle = (*ਅਗਲਾ_phandle)++;
        *msi_s_phandle = (*ਅਗਲਾ_phandle)++;
    } ਨਹੀਂਤਾਂ {
        *msi_m_phandle = qemu_fdt_alloc_phandle(fdt);
        *msi_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    ਜੇ (props->imsic_m_ਅਧਾਰ) {
        /* M-level IMSIC node */
        create_fdt_one_imsic(fdt, props, props->imsic_m_ਅਧਾਰ,
                             intc_phandles, *msi_m_phandle, true, 0);
    }

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, props, props->imsic_s_ਅਧਾਰ,
                         intc_phandles, *msi_s_phandle, false,
                         imsic_ਅੰਕ_ਬਿੱਟ(props->aia_guests + 1));

}

/* Caller must free string after use */
ਸਥਿਰ ਅੱਖਰ *fdt_get_aplic_nodename(hwaddr aplic_addr)
{
    ਵਾਪਸ g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                           aplic_addr);
}

ਸਥਿਰ ਖਾਲੀ create_fdt_one_aplic(ਖਾਲੀ *fdt, APLICFdtProps *props,
                                 hwaddr aplic_addr, uint32_t aplic_ਮਾਪ,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_ਬੱਚਾ_phandle,
                                 bool m_ਬਹੁਲਕ)
{
    g_autofree ਅੱਖਰ *aplic_name = fdt_get_aplic_nodename(aplic_addr);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, props->ਅੰਕ_harts * 2);
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ aplic_compat[2] = {
        "qemu,aplic", "riscv,aplic"
    };

    ਲਈ (ਪੂਰਨ cpu = 0; cpu < props->ਅੰਕ_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(m_ਬਹੁਲਕ ? IRQ_M_EXT : IRQ_S_EXT);
    }

    qemu_fdt_ਜੋੜੋ_subnode(fdt, aplic_name);
    qemu_fdt_setprop_string_array(fdt, aplic_name, "compatible",
                                  (ਅੱਖਰ **)&aplic_compat,
                                  ARRAY_ਮਾਪ(aplic_compat));
    qemu_fdt_setprop_cell(fdt, aplic_name, "#address-cells",
                          FDT_APLIC_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, aplic_name,
                          "#interrupt-cells", FDT_APLIC_INT_CELLS);
    qemu_fdt_setprop(fdt, aplic_name, "interrupt-controller", ਨਿਰਾ, 0);

    ਜੇ (props->aia_type == AIA_TYPE_APLIC) {
        qemu_fdt_setprop(fdt, aplic_name, "interrupts-extended",
                         aplic_cells, props->ਅੰਕ_harts * ਆਕਾਰ(uint32_t) * 2);
    } ਨਹੀਂਤਾਂ {
        qemu_fdt_setprop_cell(fdt, aplic_name, "msi-parent", msi_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, aplic_name, "reg",
                                 2, aplic_addr, 2, aplic_ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,num-sources",
                          props->irqchip_ਅੰਕ_sources);

    ਜੇ (aplic_ਬੱਚਾ_phandle) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,children",
                              aplic_ਬੱਚਾ_phandle);
        qemu_fdt_setprop_cells(fdt, aplic_name, "riscv,delegation",
                               aplic_ਬੱਚਾ_phandle, 0x1,
                               props->irqchip_ਅੰਕ_sources);
    }

    ਜੇ (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "numa-node-id", props->socket);
    }

    qemu_fdt_setprop_cell(fdt, aplic_name, "phandle", aplic_phandle);
}

ਖਾਲੀ riscv_create_fdt_socket_aplic(ਖਾਲੀ *fdt, APLICFdtProps *props,
                                   uint32_t msi_m_phandle,
                                   uint32_t msi_s_phandle,
                                   uint32_t *ਅਗਲਾ_phandle,
                                   uint32_t *intc_phandles,
                                   uint32_t *aplic_phandles)
{
    uint32_t aplic_m_phandle, aplic_s_phandle;
    hwaddr aplic_addr;

    ਜੇ (ਅਗਲਾ_phandle) {
        aplic_m_phandle = (*ਅਗਲਾ_phandle)++;
        aplic_s_phandle = (*ਅਗਲਾ_phandle)++;
    } ਨਹੀਂਤਾਂ {
        aplic_m_phandle = qemu_fdt_alloc_phandle(fdt);
        aplic_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    ਜੇ (props->aplic_m) {
        /* M-level APLIC node */
        aplic_addr = props->aplic_m->ਅਧਾਰ + (props->aplic_m->ਮਾਪ * props->socket);
        create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_m->ਮਾਪ,
                             msi_m_phandle, intc_phandles,
                             aplic_m_phandle, aplic_s_phandle,
                             true);
    }

    /* S-level APLIC node */
    aplic_addr = props->aplic_s->ਅਧਾਰ + (props->aplic_s->ਮਾਪ * props->socket);
    create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_s->ਮਾਪ,
                         msi_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         false);

    ਜੇ (!props->socket && props->platform_bus_irq) {
        g_autofree ਅੱਖਰ *aplic_name = fdt_get_aplic_nodename(aplic_addr);
        platform_bus_ਜੋੜੋ_all_fdt_nodes(fdt, aplic_name,
                                       props->platform_bus->ਅਧਾਰ,
                                       props->platform_bus->ਮਾਪ,
                                       props->platform_bus_irq);
    }

    aplic_phandles[props->socket] = aplic_s_phandle;
}

ਖਾਲੀ riscv_create_fdt_socket_aclint(ਖਾਲੀ *fdt, ACLINTFdtProps *props,
                                    uint32_t *intc_phandles)
{
    uint32_t aclint_cells_ਮਾਪ = props->ਅੰਕ_harts * ਆਕਾਰ(uint32_t) * 2;
    g_autofree uint32_t *aclint_mswi_cells = ਨਿਰਾ;
    g_autofree uint32_t *aclint_sswi_cells = ਨਿਰਾ;
    g_autofree uint32_t *aclint_mtimer_cells = ਨਿਰਾ;
    hwaddr addr, ਮਾਪ;
    ਅੱਖਰ *name;
    ਪੂਰਨ cpu;

    aclint_mswi_cells = g_new0(uint32_t, props->ਅੰਕ_harts * 2);
    aclint_mtimer_cells = g_new0(uint32_t, props->ਅੰਕ_harts * 2);
    aclint_sswi_cells = g_new0(uint32_t, props->ਅੰਕ_harts * 2);

    ਲਈ (cpu = 0; cpu < props->ਅੰਕ_harts; cpu++) {
        aclint_mswi_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        aclint_mswi_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_M_SOFT);
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_M_TIMER);
        aclint_sswi_cells[cpu * 2 + 0] = cpu_ਮੰਜ਼ਿਲ_be32(intc_phandles[cpu]);
        aclint_sswi_cells[cpu * 2 + 1] = cpu_ਮੰਜ਼ਿਲ_be32(IRQ_S_SOFT);
    }

    ਜੇ (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->ਅਧਾਰ + (props->clint->ਮਾਪ * props->socket);
        name = g_strdup_printf("/soc/mswi@%"HWADDR_PRIx, addr);

        qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, RISCV_ACLINT_SWI_ਮਾਪ);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_mswi_cells, aclint_cells_ਮਾਪ);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ਨਿਰਾ, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        ਜੇ (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }

    ਜੇ (props->aia_type == AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->ਅਧਾਰ +
               (RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ * props->socket);
        ਮਾਪ = RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ;
    } ਨਹੀਂਤਾਂ {
        addr = props->clint->ਅਧਾਰ + RISCV_ACLINT_SWI_ਮਾਪ +
               (props->clint->ਮਾਪ * props->socket);
        ਮਾਪ = props->clint->ਮਾਪ - RISCV_ACLINT_SWI_ਮਾਪ;
    }

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIx,
                           addr + RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible",
                            "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
        2, addr + RISCV_ACLINT_DEFAULT_MTIME,
        2, ਮਾਪ - RISCV_ACLINT_DEFAULT_MTIME,
        2, addr + RISCV_ACLINT_DEFAULT_MTIMECMP,
        2, RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_ਮਾਪ);

    ਜੇ (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
    }

    g_free(name);

    ਜੇ (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->aclint_sswi->ਅਧਾਰ
               + (props->aclint_sswi->ਮਾਪ * props->socket);

        name = g_strdup_printf("/soc/sswi@%"HWADDR_PRIx, addr);
        qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible",
            "riscv,aclint-sswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, props->aclint_sswi->ਮਾਪ);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_sswi_cells, aclint_cells_ਮਾਪ);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ਨਿਰਾ, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        ਜੇ (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }
}

ਅੱਖਰ *riscv_fdt_get_uart_nodename(hwaddr addr)
{
    ਵਾਪਸ g_strdup_printf("/soc/serial@%"HWADDR_PRIx, addr);
}

ਖਾਲੀ riscv_create_fdt_uart(ਖਾਲੀ *fdt, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *uart_mem,
                           ਪੂਰਨ uart_irq, ਪੂਰਨ aia_type,
                           uint32_t irq_phandle)
{
    g_autofree ਅੱਖਰ *name = riscv_fdt_get_uart_nodename(uart_mem->ਅਧਾਰ);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, uart_mem->ਅਧਾਰ,
                                 2, uart_mem->ਮਾਪ);

    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", UART_STD_CLOCK_FREQ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);

    ਜੇ (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", uart_irq);
    } ਨਹੀਂਤਾਂ {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", uart_irq, 0x4);
    }
}

ਖਾਲੀ riscv_create_fdt_rtc(ਖਾਲੀ *fdt, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *rtc_mem,
                          ਪੂਰਨ rtc_irq, ਪੂਰਨ aia_type,
                          uint32_t irq_phandle)
{
    g_autofree ਅੱਖਰ *name = ਨਿਰਾ;

    name = g_strdup_printf("/soc/rtc@%"HWADDR_PRIx,
                           rtc_mem->ਅਧਾਰ);
    qemu_fdt_ਜੋੜੋ_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "google,goldfish-rtc");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, rtc_mem->ਅਧਾਰ,
                                 2, rtc_mem->ਮਾਪ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);
    ਜੇ (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", rtc_irq);
    } ਨਹੀਂਤਾਂ {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", rtc_irq, 0x4);
    }
}
