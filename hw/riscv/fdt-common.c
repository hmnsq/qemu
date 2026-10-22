/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#అన్తర్భావ "qemu/osdep.h"

#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/sysbus-fdt.h"
#అన్తర్భావ "hw/riscv/fdt-common.h"
#అన్తర్భావ "target/riscv/cpu_bits.h"
#అన్తర్భావ "hw/riscv/riscv-iommu-bits.h"
#అన్తర్భావ "hw/riscv/iommu.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/riscv_imsic.h"
#అన్తర్భావ "hw/pci/pci.h"
#అన్తర్భావ "hw/pci/pcie_host.h"
#అన్తర్భావ "hw/riscv/aia.h"

#నిర్వచించు UART_STD_CLOCK_FREQ 3686400 /* 3.6864 MHz */

శూన్య *riscv_create_board_device_వృక్షం(స్థిరరాశి అక్షరం *model, స్థిరరాశి అక్షరం *compatible,
                                     పూర్ణాంకం *fdt_కొలత)
{
    శూన్య *fdt = create_device_వృక్షం(fdt_కొలత);

    ఒకవేళ (!fdt) {
        error_report("create_device_tree() failed");
        నిష్క్రమించు(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", model);
    qemu_fdt_setprop_string(fdt, "/", "compatible", compatible);
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_కూడు_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", ఖాళీ, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    తిరిగి fdt;
}

శూన్య riscv_create_fdt_socket_జ్ఞాపకం(శూన్య *fdt, hwaddr addr, uint64_t కొలత,
                                    పూర్ణాంకం socket_id, bool numa_enabled)
{
    g_autofree అక్షరం *mem_name = g_strdup_printf("/memory@%"HWADDR_PRIx, addr);

    qemu_fdt_కూడు_subnode(fdt, mem_name);
    qemu_fdt_setprop_sized_cells(fdt, mem_name, "reg", 2, addr, 2, కొలత);
    qemu_fdt_setprop_string(fdt, mem_name, "device_type", "memory");

    ఒకవేళ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, mem_name, "numa-node-id", socket_id);
    }
}

శూన్య riscv_create_fdt_socket_clint(శూన్య *fdt, hwaddr addr, uint64_t కొలత,
                                   పూర్ణాంకం socket_id, uint32_t *intc_phandles,
                                   పూర్ణాంకం అంకె_harts, bool numa_enabled)
{
    g_autofree uint32_t *clint_cells = g_new0(uint32_t, అంకె_harts * 4);
    g_autofree అక్షరం *clint_name = ఖాళీ;
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి clint_compat[2] = {
        "sifive,clint0", "riscv,clint0"
    };

    కొరకు (పూర్ణాంకం cpu = 0; cpu < అంకె_harts; cpu++) {
        clint_cells[cpu * 4 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 1] = cpu_గమ్యం_be32(IRQ_M_SOFT);
        clint_cells[cpu * 4 + 2] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 3] = cpu_గమ్యం_be32(IRQ_M_TIMER);
    }

    clint_name = g_strdup_printf("/soc/clint@%"HWADDR_PRIx, addr);
    qemu_fdt_కూడు_subnode(fdt, clint_name);
    qemu_fdt_setprop_string_array(fdt, clint_name, "compatible",
                                  (అక్షరం **)&clint_compat,
                                  ARRAY_కొలత(clint_compat));
    qemu_fdt_setprop_sized_cells(fdt, clint_name, "reg",
                                 2, addr, 2, కొలత);
    qemu_fdt_setprop(fdt, clint_name, "interrupts-extended",
                     clint_cells, అంకె_harts * పరిమాణం(uint32_t) * 4);

    ఒకవేళ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, clint_name, "numa-node-id", socket_id);
    }
}

శూన్య riscv_fdt_create_cpu_socket_subnode(శూన్య *fdt,
                                         uint64_t timebase_frequency)
{
    qemu_fdt_కూడు_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency",
                          timebase_frequency);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);
    qemu_fdt_కూడు_subnode(fdt, "/cpus/cpu-map");
}

స్థిర శూన్య
create_fdt_socket_cpu_అంతర్గత(శూన్య *fdt, అక్షరం *clust_name, RISCVCPU *cpu_ptr,
                               పూర్ణాంకం cpu, పూర్ణాంకం socket_id, పూర్ణాంకం socket_hartid_పునాది,
                               uint32_t *phandle, uint32_t *intc_phandles,
                               bool numa_enabled, bool is_32_బిట్టు)
{
    g_autofree అక్షరం *cpu_name = ఖాళీ;
    g_autofree అక్షరం *core_name = ఖాళీ;
    g_autofree అక్షరం *intc_name = ఖాళీ;
    uint32_t cpu_phandle = (*phandle)++;
    bool is_sifive_u = cpu_ptr == ఖాళీ;

    cpu_name = g_strdup_printf("/cpus/cpu@%d", socket_hartid_పునాది + cpu);

    /*
     * The sifive_u board has an exclusive satp and riscv,isa
     * schema that can't be shared with other boards, so part
     * of the CPU FDT creation (i.e. the /cpus/cpu@N subnode)
     * is still being done by the board.
     */
    ఒకవేళ (!is_sifive_u) {
        int8_t satp_బహుళకం_max = cpu_ptr->cfg.max_satp_బహుళకం;

        qemu_fdt_కూడు_subnode(fdt, cpu_name);

        ఒకవేళ (satp_బహుళకం_max != -1) {
            g_autofree అక్షరం *sv_name = ఖాళీ;
            sv_name = g_strdup_printf("riscv,%s",
                                      satp_బహుళకం_str(satp_బహుళకం_max, is_32_బిట్టు));
            qemu_fdt_setprop_string(fdt, cpu_name, "mmu-type", sv_name);
        }
        riscv_isa_write_fdt(cpu_ptr, fdt, cpu_name);

        ఒకవేళ (cpu_ptr->cfg.ext_zicbom) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbom-block-size",
                                  cpu_ptr->cfg.cbom_blocksize);
        }

        ఒకవేళ (cpu_ptr->cfg.ext_zicboz) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cboz-block-size",
                                  cpu_ptr->cfg.cboz_blocksize);
        }

        ఒకవేళ (cpu_ptr->cfg.ext_zicbop) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbop-block-size",
                                  cpu_ptr->cfg.cbop_blocksize);
        }
    }

    qemu_fdt_setprop_string(fdt, cpu_name, "compatible", "riscv");
    qemu_fdt_setprop_string(fdt, cpu_name, "status", "okay");
    qemu_fdt_setprop_cell(fdt, cpu_name, "reg",
                          socket_hartid_పునాది + cpu);
    qemu_fdt_setprop_string(fdt, cpu_name, "device_type", "cpu");
    ఒకవేళ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, cpu_name, "numa-node-id", socket_id);
    }
    qemu_fdt_setprop_cell(fdt, cpu_name, "phandle", cpu_phandle);

    intc_phandles[cpu] = (*phandle)++;

    intc_name = g_strdup_printf("%s/interrupt-controller", cpu_name);
    qemu_fdt_కూడు_subnode(fdt, intc_name);
    qemu_fdt_setprop_cell(fdt, intc_name, "phandle",
                          intc_phandles[cpu]);
    qemu_fdt_setprop_string(fdt, intc_name, "compatible",
                            "riscv,cpu-intc");
    qemu_fdt_setprop(fdt, intc_name, "interrupt-controller", ఖాళీ, 0);
    qemu_fdt_setprop_cell(fdt, intc_name, "#interrupt-cells", 1);

    core_name = g_strdup_printf("%s/core%d", clust_name, cpu);
    qemu_fdt_కూడు_subnode(fdt, core_name);
    qemu_fdt_setprop_cell(fdt, core_name, "cpu", cpu_phandle);
}

శూన్య riscv_create_fdt_socket_cpus(శూన్య *fdt, RISCVCPU *socket_harts,
                                  పూర్ణాంకం socket_id, పూర్ణాంకం అంకె_harts_socket,
                                  పూర్ణాంకం socket_hartid_పునాది, uint32_t *phandle,
                                  uint32_t *intc_phandles, bool numa_enabled,
                                  bool is_32_బిట్టు)
{
    g_autofree అక్షరం *clust_name = ఖాళీ;

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", socket_id);
    qemu_fdt_కూడు_subnode(fdt, clust_name);

    కొరకు (పూర్ణాంకం cpu = అంకె_harts_socket - 1; cpu >= 0; cpu--) {
        RISCVCPU *cpu_ptr = &socket_harts[cpu];

        create_fdt_socket_cpu_అంతర్గత(fdt, clust_name, cpu_ptr, cpu,
                                       socket_id, socket_hartid_పునాది,
                                       phandle, intc_phandles, numa_enabled,
                                       is_32_బిట్టు);
    }
}

శూన్య
riscv_create_fdt_socket_cpu_sifive(శూన్య *fdt, అక్షరం *clust_name,
                                   పూర్ణాంకం cpu_id, పూర్ణాంకం socket_id,
                                   పూర్ణాంకం socket_hartid_పునాది, uint32_t *phandle,
                                   uint32_t *intc_phandles)
{
    create_fdt_socket_cpu_అంతర్గత(fdt, clust_name, ఖాళీ, cpu_id,
                                   socket_id, socket_hartid_పునాది,
                                   phandle, intc_phandles, false, false);
}

శూన్య riscv_create_fdt_plic(శూన్య *fdt, hwaddr addr, uint64_t కొలత,
                           uint32_t plic_phandle, uint32_t int_cells,
                           uint32_t addr_cells, uint32_t *plic_cells,
                           uint32_t cells_కొలత, uint32_t ndev_sources,
                           bool numa_enabled, పూర్ణాంకం socket_id)
{
    g_autofree అక్షరం *nodename = ఖాళీ;
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి plic_compat[2] = {
        "sifive,plic-1.0.0", "riscv,plic0"
    };

    nodename = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx, addr);

    qemu_fdt_కూడు_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", int_cells);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", addr_cells);
    qemu_fdt_setprop_string_array(fdt, nodename, "compatible",
        (అక్షరం **)&plic_compat, ARRAY_కొలత(plic_compat));
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", ఖాళీ, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
                     plic_cells, cells_కొలత);
    qemu_fdt_setprop_sized_cells(fdt, nodename, "reg",
                                 2, addr, 2, కొలత);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", ndev_sources);
    ఒకవేళ (numa_enabled) {
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
శూన్య riscv_pmu_generate_fdt_కణుపు(శూన్య *fdt, uint32_t cmask, అక్షరం *pmu_name)
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
   fdt_event_ctr_map[0] = cpu_గమ్యం_be32(0x00000001);
   fdt_event_ctr_map[1] = cpu_గమ్యం_be32(0x00000001);
   fdt_event_ctr_map[2] = cpu_గమ్యం_be32(cmask | 1 << 0);

   /* SBI_PMU_HW_INSTRUCTIONS: 0x02 : type(0x00) */
   fdt_event_ctr_map[3] = cpu_గమ్యం_be32(0x00000002);
   fdt_event_ctr_map[4] = cpu_గమ్యం_be32(0x00000002);
   fdt_event_ctr_map[5] = cpu_గమ్యం_be32(cmask | 1 << 2);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[6] = cpu_గమ్యం_be32(0x00010019);
   fdt_event_ctr_map[7] = cpu_గమ్యం_be32(0x00010019);
   fdt_event_ctr_map[8] = cpu_గమ్యం_be32(cmask);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 WRITE : 0x01 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[9] = cpu_గమ్యం_be32(0x0001001B);
   fdt_event_ctr_map[10] = cpu_గమ్యం_be32(0x0001001B);
   fdt_event_ctr_map[11] = cpu_గమ్యం_be32(cmask);

   /* SBI_PMU_HW_CACHE_ITLB : 0x04 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[12] = cpu_గమ్యం_be32(0x00010021);
   fdt_event_ctr_map[13] = cpu_గమ్యం_be32(0x00010021);
   fdt_event_ctr_map[14] = cpu_గమ్యం_be32(cmask);

   /* This a OpenSBI specific DT property documented in OpenSBI docs */
   qemu_fdt_setprop(fdt, pmu_name, "riscv,event-to-mhpmcounters",
                    fdt_event_ctr_map, పరిమాణం(fdt_event_ctr_map));
}

శూన్య riscv_create_fdt_flash(శూన్య *fdt, hwaddr flashbase, hwaddr flashsize)
{
    g_autofree అక్షరం *name = g_strdup_printf("/flash@%" PRIx64, flashbase);

    qemu_fdt_కూడు_subnode(fdt, name);
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
శూన్య riscv_create_fdt_syscon(శూన్య *fdt, uint32_t *తదుపరి_phandle,
                             hwaddr addr, hwaddr కొలత,
                             uint32_t reboot, uint32_t poweroff,
                             bool sifive_test_compat)
{
    uint32_t syscon_phandle;
    అక్షరం *name;

    name = g_strdup_printf("/soc/syscon@%"HWADDR_PRIx, addr);
    qemu_fdt_కూడు_subnode(fdt, name);

    ఒకవేళ (sifive_test_compat) {
        స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి compat[3] = {
            "sifive,test1", "sifive,test0", "syscon"
        };

        qemu_fdt_setprop_string_array(fdt, name, "compatible",
                                      (అక్షరం **)&compat, ARRAY_కొలత(compat));
    } లేకపోతే {
        qemu_fdt_setprop_string(fdt, name, "compatible", "syscon");
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr,
                                 2, కొలత);

    syscon_phandle = తదుపరి_phandle ? (*తదుపరి_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, name, "phandle", syscon_phandle);

    g_free(name);

    name = g_strdup_printf("/reboot");
    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-reboot");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", reboot);
    g_free(name);

    name = g_strdup_printf("/poweroff");
    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-poweroff");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", poweroff);
    g_free(name);
}

uint32_t riscv_create_fdt_riscv_iommu_sys(శూన్య *fdt, hwaddr addr, hwaddr కొలత,
                                          uint32_t *తదుపరి_phandle,
                                          uint32_t irq_chip,
                                          uint32_t msi_phandle,
                                          uint32_t iommu_sys_irq)
{
    స్థిరరాశి అక్షరం comp[] = "riscv,iommu";
    g_autofree అక్షరం *iommu_కణుపు = ఖాళీ;
    uint32_t iommu_phandle;

    iommu_కణుపు = g_strdup_printf("/soc/iommu@%"HWADDR_PRIx, addr);
    qemu_fdt_కూడు_subnode(fdt, iommu_కణుపు);

    qemu_fdt_setprop(fdt, iommu_కణుపు, "compatible", comp, పరిమాణం(comp));
    qemu_fdt_setprop_cell(fdt, iommu_కణుపు, "#iommu-cells", 1);

    iommu_phandle = తదుపరి_phandle ? (*తదుపరి_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, iommu_కణుపు, "phandle", iommu_phandle);

    qemu_fdt_setprop_sized_cells(fdt, iommu_కణుపు, "reg", 2, addr, 2, కొలత);
    qemu_fdt_setprop_cell(fdt, iommu_కణుపు, "interrupt-parent", irq_chip);

    qemu_fdt_setprop_cells(fdt, iommu_కణుపు, "interrupts",
        iommu_sys_irq + RISCV_IOMMU_INTR_CQ, FDT_IRQ_TYPE_అంచు_దిగువ,
        iommu_sys_irq + RISCV_IOMMU_INTR_FQ, FDT_IRQ_TYPE_అంచు_దిగువ,
        iommu_sys_irq + RISCV_IOMMU_INTR_PM, FDT_IRQ_TYPE_అంచు_దిగువ,
        iommu_sys_irq + RISCV_IOMMU_INTR_PQ, FDT_IRQ_TYPE_అంచు_దిగువ);

    qemu_fdt_setprop_cell(fdt, iommu_కణుపు, "msi-parent", msi_phandle);

    తిరిగి iommu_phandle;
}

స్థిర శూన్య create_pcie_irq_map(శూన్య *fdt, అక్షరం *nodename,
                                uint32_t irqchip_phandle,
                                RISCVAIAType aia_type, uint32_t pcie_irq)
{
    పూర్ణాంకం pin, dev;
    uint32_t irq_map_stride = 0;
    uint32_t నిండిన_irq_map[PCI_అంకె_PINS * PCI_అంకె_PINS *
                          FDT_MAX_INT_MAP_వెడల్పు] = {};
    uint32_t *irq_map = నిండిన_irq_map;

    /*
     * This code creates a standard swizzle of interrupts such that
     * each device's first interrupt is based on it's PCI_SLOT number.
     * (See pci_swizzle_map_irq_fn())
     *
     * We only need one entry per interrupt in the table (not one per
     * possible slot) seeing the interrupt-map-mask will allow the table
     * to wrap to any number of devices.
     */
    కొరకు (dev = 0; dev < PCI_అంకె_PINS; dev++) {
        పూర్ణాంకం devfn = dev * 0x8;

        కొరకు (pin = 0; pin < PCI_అంకె_PINS; pin++) {
            పూర్ణాంకం irq_nr = pcie_irq + ((pin + PCI_స్థానం(devfn)) % PCI_అంకె_PINS);
            పూర్ణాంకం i = 0;

            /* Fill PCI address cells */
            irq_map[i] = cpu_గమ్యం_be32(devfn << 8);
            i += FDT_PCI_ADDR_CELLS;

            /* Fill PCI Interrupt cells */
            irq_map[i] = cpu_గమ్యం_be32(pin + 1);
            i += FDT_PCI_INT_CELLS;

            /* Fill interrupt controller phandle and cells */
            irq_map[i++] = cpu_గమ్యం_be32(irqchip_phandle);
            irq_map[i++] = cpu_గమ్యం_be32(irq_nr);

            ఒకవేళ (aia_type != AIA_TYPE_NONE) {
                irq_map[i++] = cpu_గమ్యం_be32(0x4);
            }

            ఒకవేళ (!irq_map_stride) {
                irq_map_stride = i;
            }
            irq_map += irq_map_stride;
        }
    }

    qemu_fdt_setprop(fdt, nodename, "interrupt-map", నిండిన_irq_map,
                     PCI_అంకె_PINS * PCI_అంకె_PINS *
                     irq_map_stride * పరిమాణం(uint32_t));

    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-map-mask",
                           0x1800, 0, 0, 0x7);
}

/*
 * NOTE: this function uses a "/soc/pci@..." FDT subnode that
 * should be created beforehand.
 */
శూన్య riscv_create_fdt_pcie(శూన్య *fdt, పూర్ణాంకం aia_type, bool has_iommu_sys,
                           స్థిరరాశి Mem_Map_నమోదు *pcie_ecam,
                           స్థిరరాశి Mem_Map_నమోదు *pcie_pio,
                           స్థిరరాశి Mem_Map_నమోదు *pcie_mmio,
                           స్థిరరాశి Mem_Map_నమోదు *ఎగువ_pcie,
                           uint32_t irq_pcie_phandle,
                           uint32_t msi_pcie_phandle,
                           uint32_t iommu_sys_phandle, uint32_t pcie_irq)
{
    g_autofree అక్షరం *name = ఖాళీ;

    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx, pcie_ecam->పునాది);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", FDT_PCI_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", FDT_PCI_INT_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0x2);
    qemu_fdt_setprop_string(fdt, name, "compatible", "pci-host-ecam-generic");
    qemu_fdt_setprop_string(fdt, name, "device_type", "pci");
    qemu_fdt_setprop_cell(fdt, name, "linux,pci-domain", 0);

    qemu_fdt_setprop_cells(fdt, name, "bus-range", 0,
                           pcie_ecam->కొలత / PCIE_MMCFG_కొలత_MIN - 1);
    qemu_fdt_setprop(fdt, name, "dma-coherent", ఖాళీ, 0);

    ఒకవేళ (aia_type == AIA_TYPE_APLIC_IMSIC) {
        qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_pcie_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2,
                                 pcie_ecam->పునాది, 2, pcie_ecam->కొలత);

    qemu_fdt_setprop_sized_cells(fdt, name, "ranges",
        1, FDT_PCI_RANGE_IOPORT, 2, 0,
        2, pcie_pio->పునాది, 2, pcie_pio->కొలత,
        1, FDT_PCI_RANGE_MMIO,
        2, pcie_mmio->పునాది,
        2, pcie_mmio->పునాది, 2, pcie_mmio->కొలత,
        1, FDT_PCI_RANGE_MMIO_64_బిట్టు,
        2, ఎగువ_pcie->పునాది,
        2, ఎగువ_pcie->పునాది, 2, ఎగువ_pcie->కొలత);

    ఒకవేళ (has_iommu_sys) {
        qemu_fdt_setprop_cells(fdt, name, "iommu-map",
                               0, iommu_sys_phandle, 0, 0x10000);
    }

    create_pcie_irq_map(fdt, name, irq_pcie_phandle, aia_type, pcie_irq);
}

స్థిర శూన్య create_fdt_one_imsic(శూన్య *fdt, IMSICFdtProps *props,
                                 hwaddr పునాది_addr,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 bool m_బహుళకం, uint32_t imsic_guest_బిట్లు)
{
    RISCVHartArrayState *soc = (RISCVHartArrayState *)props->soc;
    g_autofree అక్షరం *imsic_name = ఖాళీ;
    g_autofree uint32_t *imsic_cells = ఖాళీ;
    g_autofree uint32_t *imsic_regs = ఖాళీ;
    uint32_t imsic_max_hart_చొప్పున_socket = 0;
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి imsic_compat[2] = {
        "qemu,imsics", "riscv,imsics"
    };

    imsic_cells = g_new0(uint32_t, props->smp_cpus * 2);
    imsic_regs = g_new0(uint32_t, props->socket_లెక్క * 4);

    కొరకు (పూర్ణాంకం cpu = 0; cpu < props->smp_cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(m_బహుళకం ? IRQ_M_EXT : IRQ_S_EXT);
    }

    కొరకు (పూర్ణాంకం socket = 0; socket < props->socket_లెక్క; socket++) {
        hwaddr imsic_addr = పునాది_addr + socket * props->imsic_సమూహం_max_కొలత;
        uint32_t imsic_కొలత = IMSIC_HART_కొలత(imsic_guest_బిట్లు) *
                              soc[socket].అంకె_harts;

        imsic_regs[socket * 4 + 0] = cpu_గమ్యం_be32(imsic_addr >> 32);
        imsic_regs[socket * 4 + 1] = cpu_గమ్యం_be32(imsic_addr);
        imsic_regs[socket * 4 + 2] = 0;
        imsic_regs[socket * 4 + 3] = cpu_గమ్యం_be32(imsic_కొలత);
        ఒకవేళ (imsic_max_hart_చొప్పున_socket < soc[socket].అంకె_harts) {
            imsic_max_hart_చొప్పున_socket = soc[socket].అంకె_harts;
        }
    }

    imsic_name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                                 పునాది_addr);
    qemu_fdt_కూడు_subnode(fdt, imsic_name);

    qemu_fdt_setprop_string_array(fdt, imsic_name, "compatible",
                                  (అక్షరం **)&imsic_compat,
                                  ARRAY_కొలత(imsic_compat));

    qemu_fdt_setprop_cell(fdt, imsic_name, "#interrupt-cells",
                          FDT_IMSIC_INT_CELLS);
    qemu_fdt_setprop(fdt, imsic_name, "interrupt-controller", ఖాళీ, 0);
    qemu_fdt_setprop(fdt, imsic_name, "msi-controller", ఖాళీ, 0);
    qemu_fdt_setprop(fdt, imsic_name, "interrupts-extended",
                     imsic_cells, props->smp_cpus * పరిమాణం(uint32_t) * 2);
    qemu_fdt_setprop(fdt, imsic_name, "reg", imsic_regs,
                     props->socket_లెక్క * పరిమాణం(uint32_t) * 4);
    qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,num-ids",
                          props->irqchip_అంకె_msis);

    ఒకవేళ (imsic_guest_బిట్లు) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,guest-index-bits",
                              imsic_guest_బిట్లు);
    }

    ఒకవేళ (props->socket_లెక్క > 1) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,hart-index-bits",
                              imsic_అంకె_బిట్లు(imsic_max_hart_చొప్పున_socket));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-bits",
                              imsic_అంకె_బిట్లు(props->socket_లెక్క));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-shift",
                              IMSIC_MMIO_సమూహం_MIN_జరుపు);
    }
    qemu_fdt_setprop_cell(fdt, imsic_name, "phandle", msi_phandle);
}

శూన్య riscv_create_fdt_imsic(శూన్య *fdt, IMSICFdtProps *props,
                            uint32_t *తదుపరి_phandle, uint32_t *intc_phandles,
                            uint32_t *msi_m_phandle, uint32_t *msi_s_phandle)
{
    ఒకవేళ (తదుపరి_phandle) {
        *msi_m_phandle = (*తదుపరి_phandle)++;
        *msi_s_phandle = (*తదుపరి_phandle)++;
    } లేకపోతే {
        *msi_m_phandle = qemu_fdt_alloc_phandle(fdt);
        *msi_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    ఒకవేళ (props->imsic_m_పునాది) {
        /* M-level IMSIC node */
        create_fdt_one_imsic(fdt, props, props->imsic_m_పునాది,
                             intc_phandles, *msi_m_phandle, true, 0);
    }

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, props, props->imsic_s_పునాది,
                         intc_phandles, *msi_s_phandle, false,
                         imsic_అంకె_బిట్లు(props->aia_guests + 1));

}

/* Caller must free string after use */
స్థిర అక్షరం *fdt_get_aplic_nodename(hwaddr aplic_addr)
{
    తిరిగి g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                           aplic_addr);
}

స్థిర శూన్య create_fdt_one_aplic(శూన్య *fdt, APLICFdtProps *props,
                                 hwaddr aplic_addr, uint32_t aplic_కొలత,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_బిడ్డ_phandle,
                                 bool m_బహుళకం)
{
    g_autofree అక్షరం *aplic_name = fdt_get_aplic_nodename(aplic_addr);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, props->అంకె_harts * 2);
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి aplic_compat[2] = {
        "qemu,aplic", "riscv,aplic"
    };

    కొరకు (పూర్ణాంకం cpu = 0; cpu < props->అంకె_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(m_బహుళకం ? IRQ_M_EXT : IRQ_S_EXT);
    }

    qemu_fdt_కూడు_subnode(fdt, aplic_name);
    qemu_fdt_setprop_string_array(fdt, aplic_name, "compatible",
                                  (అక్షరం **)&aplic_compat,
                                  ARRAY_కొలత(aplic_compat));
    qemu_fdt_setprop_cell(fdt, aplic_name, "#address-cells",
                          FDT_APLIC_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, aplic_name,
                          "#interrupt-cells", FDT_APLIC_INT_CELLS);
    qemu_fdt_setprop(fdt, aplic_name, "interrupt-controller", ఖాళీ, 0);

    ఒకవేళ (props->aia_type == AIA_TYPE_APLIC) {
        qemu_fdt_setprop(fdt, aplic_name, "interrupts-extended",
                         aplic_cells, props->అంకె_harts * పరిమాణం(uint32_t) * 2);
    } లేకపోతే {
        qemu_fdt_setprop_cell(fdt, aplic_name, "msi-parent", msi_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, aplic_name, "reg",
                                 2, aplic_addr, 2, aplic_కొలత);
    qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,num-sources",
                          props->irqchip_అంకె_sources);

    ఒకవేళ (aplic_బిడ్డ_phandle) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,children",
                              aplic_బిడ్డ_phandle);
        qemu_fdt_setprop_cells(fdt, aplic_name, "riscv,delegation",
                               aplic_బిడ్డ_phandle, 0x1,
                               props->irqchip_అంకె_sources);
    }

    ఒకవేళ (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "numa-node-id", props->socket);
    }

    qemu_fdt_setprop_cell(fdt, aplic_name, "phandle", aplic_phandle);
}

శూన్య riscv_create_fdt_socket_aplic(శూన్య *fdt, APLICFdtProps *props,
                                   uint32_t msi_m_phandle,
                                   uint32_t msi_s_phandle,
                                   uint32_t *తదుపరి_phandle,
                                   uint32_t *intc_phandles,
                                   uint32_t *aplic_phandles)
{
    uint32_t aplic_m_phandle, aplic_s_phandle;
    hwaddr aplic_addr;

    ఒకవేళ (తదుపరి_phandle) {
        aplic_m_phandle = (*తదుపరి_phandle)++;
        aplic_s_phandle = (*తదుపరి_phandle)++;
    } లేకపోతే {
        aplic_m_phandle = qemu_fdt_alloc_phandle(fdt);
        aplic_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    ఒకవేళ (props->aplic_m) {
        /* M-level APLIC node */
        aplic_addr = props->aplic_m->పునాది + (props->aplic_m->కొలత * props->socket);
        create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_m->కొలత,
                             msi_m_phandle, intc_phandles,
                             aplic_m_phandle, aplic_s_phandle,
                             true);
    }

    /* S-level APLIC node */
    aplic_addr = props->aplic_s->పునాది + (props->aplic_s->కొలత * props->socket);
    create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_s->కొలత,
                         msi_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         false);

    ఒకవేళ (!props->socket && props->platform_bus_irq) {
        g_autofree అక్షరం *aplic_name = fdt_get_aplic_nodename(aplic_addr);
        platform_bus_కూడు_all_fdt_nodes(fdt, aplic_name,
                                       props->platform_bus->పునాది,
                                       props->platform_bus->కొలత,
                                       props->platform_bus_irq);
    }

    aplic_phandles[props->socket] = aplic_s_phandle;
}

శూన్య riscv_create_fdt_socket_aclint(శూన్య *fdt, ACLINTFdtProps *props,
                                    uint32_t *intc_phandles)
{
    uint32_t aclint_cells_కొలత = props->అంకె_harts * పరిమాణం(uint32_t) * 2;
    g_autofree uint32_t *aclint_mswi_cells = ఖాళీ;
    g_autofree uint32_t *aclint_sswi_cells = ఖాళీ;
    g_autofree uint32_t *aclint_mtimer_cells = ఖాళీ;
    hwaddr addr, కొలత;
    అక్షరం *name;
    పూర్ణాంకం cpu;

    aclint_mswi_cells = g_new0(uint32_t, props->అంకె_harts * 2);
    aclint_mtimer_cells = g_new0(uint32_t, props->అంకె_harts * 2);
    aclint_sswi_cells = g_new0(uint32_t, props->అంకె_harts * 2);

    కొరకు (cpu = 0; cpu < props->అంకె_harts; cpu++) {
        aclint_mswi_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        aclint_mswi_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(IRQ_M_SOFT);
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(IRQ_M_TIMER);
        aclint_sswi_cells[cpu * 2 + 0] = cpu_గమ్యం_be32(intc_phandles[cpu]);
        aclint_sswi_cells[cpu * 2 + 1] = cpu_గమ్యం_be32(IRQ_S_SOFT);
    }

    ఒకవేళ (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->పునాది + (props->clint->కొలత * props->socket);
        name = g_strdup_printf("/soc/mswi@%"HWADDR_PRIx, addr);

        qemu_fdt_కూడు_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, RISCV_ACLINT_SWI_కొలత);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_mswi_cells, aclint_cells_కొలత);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ఖాళీ, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        ఒకవేళ (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }

    ఒకవేళ (props->aia_type == AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->పునాది +
               (RISCV_ACLINT_DEFAULT_MTIMER_కొలత * props->socket);
        కొలత = RISCV_ACLINT_DEFAULT_MTIMER_కొలత;
    } లేకపోతే {
        addr = props->clint->పునాది + RISCV_ACLINT_SWI_కొలత +
               (props->clint->కొలత * props->socket);
        కొలత = props->clint->కొలత - RISCV_ACLINT_SWI_కొలత;
    }

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIx,
                           addr + RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible",
                            "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
        2, addr + RISCV_ACLINT_DEFAULT_MTIME,
        2, కొలత - RISCV_ACLINT_DEFAULT_MTIME,
        2, addr + RISCV_ACLINT_DEFAULT_MTIMECMP,
        2, RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_కొలత);

    ఒకవేళ (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
    }

    g_free(name);

    ఒకవేళ (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->aclint_sswi->పునాది
               + (props->aclint_sswi->కొలత * props->socket);

        name = g_strdup_printf("/soc/sswi@%"HWADDR_PRIx, addr);
        qemu_fdt_కూడు_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible",
            "riscv,aclint-sswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, props->aclint_sswi->కొలత);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_sswi_cells, aclint_cells_కొలత);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ఖాళీ, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        ఒకవేళ (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }
}

అక్షరం *riscv_fdt_get_uart_nodename(hwaddr addr)
{
    తిరిగి g_strdup_printf("/soc/serial@%"HWADDR_PRIx, addr);
}

శూన్య riscv_create_fdt_uart(శూన్య *fdt, స్థిరరాశి Mem_Map_నమోదు *uart_mem,
                           పూర్ణాంకం uart_irq, పూర్ణాంకం aia_type,
                           uint32_t irq_phandle)
{
    g_autofree అక్షరం *name = riscv_fdt_get_uart_nodename(uart_mem->పునాది);

    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, uart_mem->పునాది,
                                 2, uart_mem->కొలత);

    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", UART_STD_CLOCK_FREQ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);

    ఒకవేళ (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", uart_irq);
    } లేకపోతే {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", uart_irq, 0x4);
    }
}

శూన్య riscv_create_fdt_rtc(శూన్య *fdt, స్థిరరాశి Mem_Map_నమోదు *rtc_mem,
                          పూర్ణాంకం rtc_irq, పూర్ణాంకం aia_type,
                          uint32_t irq_phandle)
{
    g_autofree అక్షరం *name = ఖాళీ;

    name = g_strdup_printf("/soc/rtc@%"HWADDR_PRIx,
                           rtc_mem->పునాది);
    qemu_fdt_కూడు_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "google,goldfish-rtc");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, rtc_mem->పునాది,
                                 2, rtc_mem->కొలత);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);
    ఒకవేళ (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", rtc_irq);
    } లేకపోతే {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", rtc_irq, 0x4);
    }
}
