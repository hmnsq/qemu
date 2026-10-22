/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#অন্তর্ভাব "qemu/osdep.h"

#অন্তর্ভাব "qemu/error-report.h"
#অন্তর্ভাব "system/device_tree.h"
#অন্তর্ভাব "hw/core/boards.h"
#অন্তর্ভাব "hw/core/sysbus-fdt.h"
#অন্তর্ভাব "hw/riscv/fdt-common.h"
#অন্তর্ভাব "target/riscv/cpu_bits.h"
#অন্তর্ভাব "hw/riscv/riscv-iommu-bits.h"
#অন্তর্ভাব "hw/riscv/iommu.h"
#অন্তর্ভাব "hw/intc/riscv_aclint.h"
#অন্তর্ভাব "hw/intc/riscv_imsic.h"
#অন্তর্ভাব "hw/pci/pci.h"
#অন্তর্ভাব "hw/pci/pcie_host.h"
#অন্তর্ভাব "hw/riscv/aia.h"

#define UART_STD_CLOCK_FREQ 3686400 /* 3.6864 MHz */

শূন্য *riscv_create_board_device_বৃক্ষ(ধ্রুবক অক্ষর *model, ধ্রুবক অক্ষর *compatible,
                                     পূর্ণসংখ্যা *fdt_আকার)
{
    শূন্য *fdt = create_device_বৃক্ষ(fdt_আকার);

    যদি (!fdt) {
        error_report("create_device_tree() failed");
        exit(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", model);
    qemu_fdt_setprop_string(fdt, "/", "compatible", compatible);
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_add_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", ফাঁকা, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    ফেরত fdt;
}

শূন্য riscv_create_fdt_socket_memory(শূন্য *fdt, hwaddr addr, uint64_t আকার_2,
                                    পূর্ণসংখ্যা socket_id, bool numa_enabled)
{
    g_autofree অক্ষর *mem_name = g_strdup_printf("/memory@%"HWADDR_PRIx, addr);

    qemu_fdt_add_subnode(fdt, mem_name);
    qemu_fdt_setprop_sized_cells(fdt, mem_name, "reg", 2, addr, 2, আকার_2);
    qemu_fdt_setprop_string(fdt, mem_name, "device_type", "memory");

    যদি (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, mem_name, "numa-node-id", socket_id);
    }
}

শূন্য riscv_create_fdt_socket_clint(শূন্য *fdt, hwaddr addr, uint64_t আকার_2,
                                   পূর্ণসংখ্যা socket_id, uint32_t *intc_phandles,
                                   পূর্ণসংখ্যা অঙ্ক_harts, bool numa_enabled)
{
    g_autofree uint32_t *clint_cells = g_new0(uint32_t, অঙ্ক_harts * 4);
    g_autofree অক্ষর *clint_name = ফাঁকা;
    স্থির ধ্রুবক অক্ষর * ধ্রুবক clint_compat[2] = {
        "sifive,clint0", "riscv,clint0"
    };

    জন্য (পূর্ণসংখ্যা cpu = 0; cpu < অঙ্ক_harts; cpu++) {
        clint_cells[cpu * 4 + 0] = cpu_to_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 1] = cpu_to_be32(IRQ_M_SOFT);
        clint_cells[cpu * 4 + 2] = cpu_to_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 3] = cpu_to_be32(IRQ_M_TIMER);
    }

    clint_name = g_strdup_printf("/soc/clint@%"HWADDR_PRIx, addr);
    qemu_fdt_add_subnode(fdt, clint_name);
    qemu_fdt_setprop_string_array(fdt, clint_name, "compatible",
                                  (অক্ষর **)&clint_compat,
                                  ARRAY_আকার(clint_compat));
    qemu_fdt_setprop_sized_cells(fdt, clint_name, "reg",
                                 2, addr, 2, আকার_2);
    qemu_fdt_setprop(fdt, clint_name, "interrupts-extended",
                     clint_cells, অঙ্ক_harts * আকার(uint32_t) * 4);

    যদি (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, clint_name, "numa-node-id", socket_id);
    }
}

শূন্য riscv_fdt_create_cpu_socket_subnode(শূন্য *fdt,
                                         uint64_t timebase_frequency)
{
    qemu_fdt_add_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency",
                          timebase_frequency);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);
    qemu_fdt_add_subnode(fdt, "/cpus/cpu-map");
}

স্থির শূন্য
create_fdt_socket_cpu_internal(শূন্য *fdt, অক্ষর *clust_name, RISCVCPU *cpu_ptr,
                               পূর্ণসংখ্যা cpu, পূর্ণসংখ্যা socket_id, পূর্ণসংখ্যা socket_hartid_base,
                               uint32_t *phandle, uint32_t *intc_phandles,
                               bool numa_enabled, bool is_32_bit)
{
    g_autofree অক্ষর *cpu_name = ফাঁকা;
    g_autofree অক্ষর *core_name = ফাঁকা;
    g_autofree অক্ষর *intc_name = ফাঁকা;
    uint32_t cpu_phandle = (*phandle)++;
    bool is_sifive_u = cpu_ptr == ফাঁকা;

    cpu_name = g_strdup_printf("/cpus/cpu@%d", socket_hartid_base + cpu);

    /*
     * The sifive_u board has an exclusive satp and riscv,isa
     * schema that can't be shared with other boards, so part
     * of the CPU FDT creation (i.e. the /cpus/cpu@N subnode)
     * is still being done by the board.
     */
    যদি (!is_sifive_u) {
        int8_t satp_mode_max = cpu_ptr->cfg.max_satp_mode;

        qemu_fdt_add_subnode(fdt, cpu_name);

        যদি (satp_mode_max != -1) {
            g_autofree অক্ষর *sv_name = ফাঁকা;
            sv_name = g_strdup_printf("riscv,%s",
                                      satp_mode_str(satp_mode_max, is_32_bit));
            qemu_fdt_setprop_string(fdt, cpu_name, "mmu-type", sv_name);
        }
        riscv_isa_write_fdt(cpu_ptr, fdt, cpu_name);

        যদি (cpu_ptr->cfg.ext_zicbom) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbom-block-size",
                                  cpu_ptr->cfg.cbom_blocksize);
        }

        যদি (cpu_ptr->cfg.ext_zicboz) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cboz-block-size",
                                  cpu_ptr->cfg.cboz_blocksize);
        }

        যদি (cpu_ptr->cfg.ext_zicbop) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbop-block-size",
                                  cpu_ptr->cfg.cbop_blocksize);
        }
    }

    qemu_fdt_setprop_string(fdt, cpu_name, "compatible", "riscv");
    qemu_fdt_setprop_string(fdt, cpu_name, "status", "okay");
    qemu_fdt_setprop_cell(fdt, cpu_name, "reg",
                          socket_hartid_base + cpu);
    qemu_fdt_setprop_string(fdt, cpu_name, "device_type", "cpu");
    যদি (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, cpu_name, "numa-node-id", socket_id);
    }
    qemu_fdt_setprop_cell(fdt, cpu_name, "phandle", cpu_phandle);

    intc_phandles[cpu] = (*phandle)++;

    intc_name = g_strdup_printf("%s/interrupt-controller", cpu_name);
    qemu_fdt_add_subnode(fdt, intc_name);
    qemu_fdt_setprop_cell(fdt, intc_name, "phandle",
                          intc_phandles[cpu]);
    qemu_fdt_setprop_string(fdt, intc_name, "compatible",
                            "riscv,cpu-intc");
    qemu_fdt_setprop(fdt, intc_name, "interrupt-controller", ফাঁকা, 0);
    qemu_fdt_setprop_cell(fdt, intc_name, "#interrupt-cells", 1);

    core_name = g_strdup_printf("%s/core%d", clust_name, cpu);
    qemu_fdt_add_subnode(fdt, core_name);
    qemu_fdt_setprop_cell(fdt, core_name, "cpu", cpu_phandle);
}

শূন্য riscv_create_fdt_socket_cpus(শূন্য *fdt, RISCVCPU *socket_harts,
                                  পূর্ণসংখ্যা socket_id, পূর্ণসংখ্যা অঙ্ক_harts_socket,
                                  পূর্ণসংখ্যা socket_hartid_base, uint32_t *phandle,
                                  uint32_t *intc_phandles, bool numa_enabled,
                                  bool is_32_bit)
{
    g_autofree অক্ষর *clust_name = ফাঁকা;

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", socket_id);
    qemu_fdt_add_subnode(fdt, clust_name);

    জন্য (পূর্ণসংখ্যা cpu = অঙ্ক_harts_socket - 1; cpu >= 0; cpu--) {
        RISCVCPU *cpu_ptr = &socket_harts[cpu];

        create_fdt_socket_cpu_internal(fdt, clust_name, cpu_ptr, cpu,
                                       socket_id, socket_hartid_base,
                                       phandle, intc_phandles, numa_enabled,
                                       is_32_bit);
    }
}

শূন্য
riscv_create_fdt_socket_cpu_sifive(শূন্য *fdt, অক্ষর *clust_name,
                                   পূর্ণসংখ্যা cpu_id, পূর্ণসংখ্যা socket_id,
                                   পূর্ণসংখ্যা socket_hartid_base, uint32_t *phandle,
                                   uint32_t *intc_phandles)
{
    create_fdt_socket_cpu_internal(fdt, clust_name, ফাঁকা, cpu_id,
                                   socket_id, socket_hartid_base,
                                   phandle, intc_phandles, false, false);
}

শূন্য riscv_create_fdt_plic(শূন্য *fdt, hwaddr addr, uint64_t আকার_2,
                           uint32_t plic_phandle, uint32_t int_cells,
                           uint32_t addr_cells, uint32_t *plic_cells,
                           uint32_t cells_আকার, uint32_t ndev_sources,
                           bool numa_enabled, পূর্ণসংখ্যা socket_id)
{
    g_autofree অক্ষর *nodename = ফাঁকা;
    স্থির ধ্রুবক অক্ষর * ধ্রুবক plic_compat[2] = {
        "sifive,plic-1.0.0", "riscv,plic0"
    };

    nodename = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx, addr);

    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", int_cells);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", addr_cells);
    qemu_fdt_setprop_string_array(fdt, nodename, "compatible",
        (অক্ষর **)&plic_compat, ARRAY_আকার(plic_compat));
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", ফাঁকা, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
                     plic_cells, cells_আকার);
    qemu_fdt_setprop_sized_cells(fdt, nodename, "reg",
                                 2, addr, 2, আকার_2);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", ndev_sources);
    যদি (numa_enabled) {
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
শূন্য riscv_pmu_generate_fdt_গ্রন্থি(শূন্য *fdt, uint32_t cmask, অক্ষর *pmu_name)
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
   fdt_event_ctr_map[0] = cpu_to_be32(0x00000001);
   fdt_event_ctr_map[1] = cpu_to_be32(0x00000001);
   fdt_event_ctr_map[2] = cpu_to_be32(cmask | 1 << 0);

   /* SBI_PMU_HW_INSTRUCTIONS: 0x02 : type(0x00) */
   fdt_event_ctr_map[3] = cpu_to_be32(0x00000002);
   fdt_event_ctr_map[4] = cpu_to_be32(0x00000002);
   fdt_event_ctr_map[5] = cpu_to_be32(cmask | 1 << 2);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[6] = cpu_to_be32(0x00010019);
   fdt_event_ctr_map[7] = cpu_to_be32(0x00010019);
   fdt_event_ctr_map[8] = cpu_to_be32(cmask);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 WRITE : 0x01 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[9] = cpu_to_be32(0x0001001B);
   fdt_event_ctr_map[10] = cpu_to_be32(0x0001001B);
   fdt_event_ctr_map[11] = cpu_to_be32(cmask);

   /* SBI_PMU_HW_CACHE_ITLB : 0x04 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[12] = cpu_to_be32(0x00010021);
   fdt_event_ctr_map[13] = cpu_to_be32(0x00010021);
   fdt_event_ctr_map[14] = cpu_to_be32(cmask);

   /* This a OpenSBI specific DT property documented in OpenSBI docs */
   qemu_fdt_setprop(fdt, pmu_name, "riscv,event-to-mhpmcounters",
                    fdt_event_ctr_map, আকার(fdt_event_ctr_map));
}

শূন্য riscv_create_fdt_flash(শূন্য *fdt, hwaddr flashbase, hwaddr flashsize)
{
    g_autofree অক্ষর *name = g_strdup_printf("/flash@%" PRIx64, flashbase);

    qemu_fdt_add_subnode(fdt, name);
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
শূন্য riscv_create_fdt_syscon(শূন্য *fdt, uint32_t *পরবর্তী_phandle,
                             hwaddr addr, hwaddr আকার_2,
                             uint32_t reboot, uint32_t poweroff,
                             bool sifive_test_compat)
{
    uint32_t syscon_phandle;
    অক্ষর *name;

    name = g_strdup_printf("/soc/syscon@%"HWADDR_PRIx, addr);
    qemu_fdt_add_subnode(fdt, name);

    যদি (sifive_test_compat) {
        স্থির ধ্রুবক অক্ষর * ধ্রুবক compat[3] = {
            "sifive,test1", "sifive,test0", "syscon"
        };

        qemu_fdt_setprop_string_array(fdt, name, "compatible",
                                      (অক্ষর **)&compat, ARRAY_আকার(compat));
    } অন্যথা {
        qemu_fdt_setprop_string(fdt, name, "compatible", "syscon");
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr,
                                 2, আকার_2);

    syscon_phandle = পরবর্তী_phandle ? (*পরবর্তী_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, name, "phandle", syscon_phandle);

    g_free(name);

    name = g_strdup_printf("/reboot");
    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-reboot");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", reboot);
    g_free(name);

    name = g_strdup_printf("/poweroff");
    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-poweroff");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", poweroff);
    g_free(name);
}

uint32_t riscv_create_fdt_riscv_iommu_sys(শূন্য *fdt, hwaddr addr, hwaddr আকার_2,
                                          uint32_t *পরবর্তী_phandle,
                                          uint32_t irq_chip,
                                          uint32_t msi_phandle,
                                          uint32_t iommu_sys_irq)
{
    ধ্রুবক অক্ষর comp[] = "riscv,iommu";
    g_autofree অক্ষর *iommu_গ্রন্থি = ফাঁকা;
    uint32_t iommu_phandle;

    iommu_গ্রন্থি = g_strdup_printf("/soc/iommu@%"HWADDR_PRIx, addr);
    qemu_fdt_add_subnode(fdt, iommu_গ্রন্থি);

    qemu_fdt_setprop(fdt, iommu_গ্রন্থি, "compatible", comp, আকার(comp));
    qemu_fdt_setprop_cell(fdt, iommu_গ্রন্থি, "#iommu-cells", 1);

    iommu_phandle = পরবর্তী_phandle ? (*পরবর্তী_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, iommu_গ্রন্থি, "phandle", iommu_phandle);

    qemu_fdt_setprop_sized_cells(fdt, iommu_গ্রন্থি, "reg", 2, addr, 2, আকার_2);
    qemu_fdt_setprop_cell(fdt, iommu_গ্রন্থি, "interrupt-parent", irq_chip);

    qemu_fdt_setprop_cells(fdt, iommu_গ্রন্থি, "interrupts",
        iommu_sys_irq + RISCV_IOMMU_INTR_CQ, FDT_IRQ_TYPE_EDGE_নিচের,
        iommu_sys_irq + RISCV_IOMMU_INTR_FQ, FDT_IRQ_TYPE_EDGE_নিচের,
        iommu_sys_irq + RISCV_IOMMU_INTR_PM, FDT_IRQ_TYPE_EDGE_নিচের,
        iommu_sys_irq + RISCV_IOMMU_INTR_PQ, FDT_IRQ_TYPE_EDGE_নিচের);

    qemu_fdt_setprop_cell(fdt, iommu_গ্রন্থি, "msi-parent", msi_phandle);

    ফেরত iommu_phandle;
}

স্থির শূন্য create_pcie_irq_map(শূন্য *fdt, অক্ষর *nodename,
                                uint32_t irqchip_phandle,
                                RISCVAIAType aia_type, uint32_t pcie_irq)
{
    পূর্ণসংখ্যা pin, dev;
    uint32_t irq_map_stride = 0;
    uint32_t full_irq_map[PCI_অঙ্ক_PINS * PCI_অঙ্ক_PINS *
                          FDT_MAX_INT_MAP_WIDTH] = {};
    uint32_t *irq_map = full_irq_map;

    /*
     * This code creates a standard swizzle of interrupts such that
     * each device's first interrupt is based on it's PCI_SLOT number.
     * (See pci_swizzle_map_irq_fn())
     *
     * We only need one entry per interrupt in the table (not one per
     * possible slot) seeing the interrupt-map-mask will allow the table
     * to wrap to any number of devices.
     */
    জন্য (dev = 0; dev < PCI_অঙ্ক_PINS; dev++) {
        পূর্ণসংখ্যা devfn = dev * 0x8;

        জন্য (pin = 0; pin < PCI_অঙ্ক_PINS; pin++) {
            পূর্ণসংখ্যা irq_nr = pcie_irq + ((pin + PCI_SLOT(devfn)) % PCI_অঙ্ক_PINS);
            পূর্ণসংখ্যা i = 0;

            /* Fill PCI address cells */
            irq_map[i] = cpu_to_be32(devfn << 8);
            i += FDT_PCI_ADDR_CELLS;

            /* Fill PCI Interrupt cells */
            irq_map[i] = cpu_to_be32(pin + 1);
            i += FDT_PCI_INT_CELLS;

            /* Fill interrupt controller phandle and cells */
            irq_map[i++] = cpu_to_be32(irqchip_phandle);
            irq_map[i++] = cpu_to_be32(irq_nr);

            যদি (aia_type != AIA_TYPE_NONE) {
                irq_map[i++] = cpu_to_be32(0x4);
            }

            যদি (!irq_map_stride) {
                irq_map_stride = i;
            }
            irq_map += irq_map_stride;
        }
    }

    qemu_fdt_setprop(fdt, nodename, "interrupt-map", full_irq_map,
                     PCI_অঙ্ক_PINS * PCI_অঙ্ক_PINS *
                     irq_map_stride * আকার(uint32_t));

    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-map-mask",
                           0x1800, 0, 0, 0x7);
}

/*
 * NOTE: this function uses a "/soc/pci@..." FDT subnode that
 * should be created beforehand.
 */
শূন্য riscv_create_fdt_pcie(শূন্য *fdt, পূর্ণসংখ্যা aia_type, bool has_iommu_sys,
                           ধ্রুবক MemMapEntry *pcie_ecam,
                           ধ্রুবক MemMapEntry *pcie_pio,
                           ধ্রুবক MemMapEntry *pcie_mmio,
                           ধ্রুবক MemMapEntry *উপরের_pcie,
                           uint32_t irq_pcie_phandle,
                           uint32_t msi_pcie_phandle,
                           uint32_t iommu_sys_phandle, uint32_t pcie_irq)
{
    g_autofree অক্ষর *name = ফাঁকা;

    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx, pcie_ecam->base);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", FDT_PCI_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", FDT_PCI_INT_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0x2);
    qemu_fdt_setprop_string(fdt, name, "compatible", "pci-host-ecam-generic");
    qemu_fdt_setprop_string(fdt, name, "device_type", "pci");
    qemu_fdt_setprop_cell(fdt, name, "linux,pci-domain", 0);

    qemu_fdt_setprop_cells(fdt, name, "bus-range", 0,
                           pcie_ecam->আকার_2 / PCIE_MMCFG_আকার_MIN - 1);
    qemu_fdt_setprop(fdt, name, "dma-coherent", ফাঁকা, 0);

    যদি (aia_type == AIA_TYPE_APLIC_IMSIC) {
        qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_pcie_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2,
                                 pcie_ecam->base, 2, pcie_ecam->আকার_2);

    qemu_fdt_setprop_sized_cells(fdt, name, "ranges",
        1, FDT_PCI_RANGE_IOPORT, 2, 0,
        2, pcie_pio->base, 2, pcie_pio->আকার_2,
        1, FDT_PCI_RANGE_MMIO,
        2, pcie_mmio->base,
        2, pcie_mmio->base, 2, pcie_mmio->আকার_2,
        1, FDT_PCI_RANGE_MMIO_64BIT,
        2, উপরের_pcie->base,
        2, উপরের_pcie->base, 2, উপরের_pcie->আকার_2);

    যদি (has_iommu_sys) {
        qemu_fdt_setprop_cells(fdt, name, "iommu-map",
                               0, iommu_sys_phandle, 0, 0x10000);
    }

    create_pcie_irq_map(fdt, name, irq_pcie_phandle, aia_type, pcie_irq);
}

স্থির শূন্য create_fdt_one_imsic(শূন্য *fdt, IMSICFdtProps *props,
                                 hwaddr base_addr,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 bool m_mode, uint32_t imsic_guest_bits)
{
    RISCVHartArrayState *soc = (RISCVHartArrayState *)props->soc;
    g_autofree অক্ষর *imsic_name = ফাঁকা;
    g_autofree uint32_t *imsic_cells = ফাঁকা;
    g_autofree uint32_t *imsic_regs = ফাঁকা;
    uint32_t imsic_max_hart_per_socket = 0;
    স্থির ধ্রুবক অক্ষর * ধ্রুবক imsic_compat[2] = {
        "qemu,imsics", "riscv,imsics"
    };

    imsic_cells = g_new0(uint32_t, props->smp_cpus * 2);
    imsic_regs = g_new0(uint32_t, props->socket_গুনতি * 4);

    জন্য (পূর্ণসংখ্যা cpu = 0; cpu < props->smp_cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_to_be32(m_mode ? IRQ_M_EXT : IRQ_S_EXT);
    }

    জন্য (পূর্ণসংখ্যা socket = 0; socket < props->socket_গুনতি; socket++) {
        hwaddr imsic_addr = base_addr + socket * props->imsic_group_max_আকার;
        uint32_t imsic_আকার = IMSIC_HART_আকার(imsic_guest_bits) *
                              soc[socket].অঙ্ক_harts;

        imsic_regs[socket * 4 + 0] = cpu_to_be32(imsic_addr >> 32);
        imsic_regs[socket * 4 + 1] = cpu_to_be32(imsic_addr);
        imsic_regs[socket * 4 + 2] = 0;
        imsic_regs[socket * 4 + 3] = cpu_to_be32(imsic_আকার);
        যদি (imsic_max_hart_per_socket < soc[socket].অঙ্ক_harts) {
            imsic_max_hart_per_socket = soc[socket].অঙ্ক_harts;
        }
    }

    imsic_name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                                 base_addr);
    qemu_fdt_add_subnode(fdt, imsic_name);

    qemu_fdt_setprop_string_array(fdt, imsic_name, "compatible",
                                  (অক্ষর **)&imsic_compat,
                                  ARRAY_আকার(imsic_compat));

    qemu_fdt_setprop_cell(fdt, imsic_name, "#interrupt-cells",
                          FDT_IMSIC_INT_CELLS);
    qemu_fdt_setprop(fdt, imsic_name, "interrupt-controller", ফাঁকা, 0);
    qemu_fdt_setprop(fdt, imsic_name, "msi-controller", ফাঁকা, 0);
    qemu_fdt_setprop(fdt, imsic_name, "interrupts-extended",
                     imsic_cells, props->smp_cpus * আকার(uint32_t) * 2);
    qemu_fdt_setprop(fdt, imsic_name, "reg", imsic_regs,
                     props->socket_গুনতি * আকার(uint32_t) * 4);
    qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,num-ids",
                          props->irqchip_অঙ্ক_msis);

    যদি (imsic_guest_bits) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,guest-index-bits",
                              imsic_guest_bits);
    }

    যদি (props->socket_গুনতি > 1) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,hart-index-bits",
                              imsic_অঙ্ক_bits(imsic_max_hart_per_socket));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-bits",
                              imsic_অঙ্ক_bits(props->socket_গুনতি));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-shift",
                              IMSIC_MMIO_GROUP_MIN_SHIFT);
    }
    qemu_fdt_setprop_cell(fdt, imsic_name, "phandle", msi_phandle);
}

শূন্য riscv_create_fdt_imsic(শূন্য *fdt, IMSICFdtProps *props,
                            uint32_t *পরবর্তী_phandle, uint32_t *intc_phandles,
                            uint32_t *msi_m_phandle, uint32_t *msi_s_phandle)
{
    যদি (পরবর্তী_phandle) {
        *msi_m_phandle = (*পরবর্তী_phandle)++;
        *msi_s_phandle = (*পরবর্তী_phandle)++;
    } অন্যথা {
        *msi_m_phandle = qemu_fdt_alloc_phandle(fdt);
        *msi_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    যদি (props->imsic_m_base) {
        /* M-level IMSIC node */
        create_fdt_one_imsic(fdt, props, props->imsic_m_base,
                             intc_phandles, *msi_m_phandle, true, 0);
    }

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, props, props->imsic_s_base,
                         intc_phandles, *msi_s_phandle, false,
                         imsic_অঙ্ক_bits(props->aia_guests + 1));

}

/* Caller must free string after use */
স্থির অক্ষর *fdt_get_aplic_nodename(hwaddr aplic_addr)
{
    ফেরত g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                           aplic_addr);
}

স্থির শূন্য create_fdt_one_aplic(শূন্য *fdt, APLICFdtProps *props,
                                 hwaddr aplic_addr, uint32_t aplic_আকার,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_child_phandle,
                                 bool m_mode)
{
    g_autofree অক্ষর *aplic_name = fdt_get_aplic_nodename(aplic_addr);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, props->অঙ্ক_harts * 2);
    স্থির ধ্রুবক অক্ষর * ধ্রুবক aplic_compat[2] = {
        "qemu,aplic", "riscv,aplic"
    };

    জন্য (পূর্ণসংখ্যা cpu = 0; cpu < props->অঙ্ক_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_to_be32(m_mode ? IRQ_M_EXT : IRQ_S_EXT);
    }

    qemu_fdt_add_subnode(fdt, aplic_name);
    qemu_fdt_setprop_string_array(fdt, aplic_name, "compatible",
                                  (অক্ষর **)&aplic_compat,
                                  ARRAY_আকার(aplic_compat));
    qemu_fdt_setprop_cell(fdt, aplic_name, "#address-cells",
                          FDT_APLIC_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, aplic_name,
                          "#interrupt-cells", FDT_APLIC_INT_CELLS);
    qemu_fdt_setprop(fdt, aplic_name, "interrupt-controller", ফাঁকা, 0);

    যদি (props->aia_type == AIA_TYPE_APLIC) {
        qemu_fdt_setprop(fdt, aplic_name, "interrupts-extended",
                         aplic_cells, props->অঙ্ক_harts * আকার(uint32_t) * 2);
    } অন্যথা {
        qemu_fdt_setprop_cell(fdt, aplic_name, "msi-parent", msi_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, aplic_name, "reg",
                                 2, aplic_addr, 2, aplic_আকার);
    qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,num-sources",
                          props->irqchip_অঙ্ক_sources);

    যদি (aplic_child_phandle) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,children",
                              aplic_child_phandle);
        qemu_fdt_setprop_cells(fdt, aplic_name, "riscv,delegation",
                               aplic_child_phandle, 0x1,
                               props->irqchip_অঙ্ক_sources);
    }

    যদি (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "numa-node-id", props->socket);
    }

    qemu_fdt_setprop_cell(fdt, aplic_name, "phandle", aplic_phandle);
}

শূন্য riscv_create_fdt_socket_aplic(শূন্য *fdt, APLICFdtProps *props,
                                   uint32_t msi_m_phandle,
                                   uint32_t msi_s_phandle,
                                   uint32_t *পরবর্তী_phandle,
                                   uint32_t *intc_phandles,
                                   uint32_t *aplic_phandles)
{
    uint32_t aplic_m_phandle, aplic_s_phandle;
    hwaddr aplic_addr;

    যদি (পরবর্তী_phandle) {
        aplic_m_phandle = (*পরবর্তী_phandle)++;
        aplic_s_phandle = (*পরবর্তী_phandle)++;
    } অন্যথা {
        aplic_m_phandle = qemu_fdt_alloc_phandle(fdt);
        aplic_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    যদি (props->aplic_m) {
        /* M-level APLIC node */
        aplic_addr = props->aplic_m->base + (props->aplic_m->আকার_2 * props->socket);
        create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_m->আকার_2,
                             msi_m_phandle, intc_phandles,
                             aplic_m_phandle, aplic_s_phandle,
                             true);
    }

    /* S-level APLIC node */
    aplic_addr = props->aplic_s->base + (props->aplic_s->আকার_2 * props->socket);
    create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_s->আকার_2,
                         msi_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         false);

    যদি (!props->socket && props->platform_bus_irq) {
        g_autofree অক্ষর *aplic_name = fdt_get_aplic_nodename(aplic_addr);
        platform_bus_add_all_fdt_nodes(fdt, aplic_name,
                                       props->platform_bus->base,
                                       props->platform_bus->আকার_2,
                                       props->platform_bus_irq);
    }

    aplic_phandles[props->socket] = aplic_s_phandle;
}

শূন্য riscv_create_fdt_socket_aclint(শূন্য *fdt, ACLINTFdtProps *props,
                                    uint32_t *intc_phandles)
{
    uint32_t aclint_cells_আকার = props->অঙ্ক_harts * আকার(uint32_t) * 2;
    g_autofree uint32_t *aclint_mswi_cells = ফাঁকা;
    g_autofree uint32_t *aclint_sswi_cells = ফাঁকা;
    g_autofree uint32_t *aclint_mtimer_cells = ফাঁকা;
    hwaddr addr, আকার_2;
    অক্ষর *name;
    পূর্ণসংখ্যা cpu;

    aclint_mswi_cells = g_new0(uint32_t, props->অঙ্ক_harts * 2);
    aclint_mtimer_cells = g_new0(uint32_t, props->অঙ্ক_harts * 2);
    aclint_sswi_cells = g_new0(uint32_t, props->অঙ্ক_harts * 2);

    জন্য (cpu = 0; cpu < props->অঙ্ক_harts; cpu++) {
        aclint_mswi_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aclint_mswi_cells[cpu * 2 + 1] = cpu_to_be32(IRQ_M_SOFT);
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_to_be32(IRQ_M_TIMER);
        aclint_sswi_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aclint_sswi_cells[cpu * 2 + 1] = cpu_to_be32(IRQ_S_SOFT);
    }

    যদি (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->base + (props->clint->আকার_2 * props->socket);
        name = g_strdup_printf("/soc/mswi@%"HWADDR_PRIx, addr);

        qemu_fdt_add_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, RISCV_ACLINT_SWI_আকার);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_mswi_cells, aclint_cells_আকার);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ফাঁকা, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        যদি (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }

    যদি (props->aia_type == AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->base +
               (RISCV_ACLINT_DEFAULT_MTIMER_আকার * props->socket);
        আকার_2 = RISCV_ACLINT_DEFAULT_MTIMER_আকার;
    } অন্যথা {
        addr = props->clint->base + RISCV_ACLINT_SWI_আকার +
               (props->clint->আকার_2 * props->socket);
        আকার_2 = props->clint->আকার_2 - RISCV_ACLINT_SWI_আকার;
    }

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIx,
                           addr + RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible",
                            "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
        2, addr + RISCV_ACLINT_DEFAULT_MTIME,
        2, আকার_2 - RISCV_ACLINT_DEFAULT_MTIME,
        2, addr + RISCV_ACLINT_DEFAULT_MTIMECMP,
        2, RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_আকার);

    যদি (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
    }

    g_free(name);

    যদি (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->aclint_sswi->base
               + (props->aclint_sswi->আকার_2 * props->socket);

        name = g_strdup_printf("/soc/sswi@%"HWADDR_PRIx, addr);
        qemu_fdt_add_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible",
            "riscv,aclint-sswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, props->aclint_sswi->আকার_2);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_sswi_cells, aclint_cells_আকার);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ফাঁকা, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        যদি (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }
}

অক্ষর *riscv_fdt_get_uart_nodename(hwaddr addr)
{
    ফেরত g_strdup_printf("/soc/serial@%"HWADDR_PRIx, addr);
}

শূন্য riscv_create_fdt_uart(শূন্য *fdt, ধ্রুবক MemMapEntry *uart_mem,
                           পূর্ণসংখ্যা uart_irq, পূর্ণসংখ্যা aia_type,
                           uint32_t irq_phandle)
{
    g_autofree অক্ষর *name = riscv_fdt_get_uart_nodename(uart_mem->base);

    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, uart_mem->base,
                                 2, uart_mem->আকার_2);

    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", UART_STD_CLOCK_FREQ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);

    যদি (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", uart_irq);
    } অন্যথা {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", uart_irq, 0x4);
    }
}

শূন্য riscv_create_fdt_rtc(শূন্য *fdt, ধ্রুবক MemMapEntry *rtc_mem,
                          পূর্ণসংখ্যা rtc_irq, পূর্ণসংখ্যা aia_type,
                          uint32_t irq_phandle)
{
    g_autofree অক্ষর *name = ফাঁকা;

    name = g_strdup_printf("/soc/rtc@%"HWADDR_PRIx,
                           rtc_mem->base);
    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "google,goldfish-rtc");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, rtc_mem->base,
                                 2, rtc_mem->আকার_2);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);
    যদি (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", rtc_irq);
    } অন্যথা {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", rtc_irq, 0x4);
    }
}
