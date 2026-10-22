/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"

#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus-fdt.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/fdt-common.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu_bits.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv-iommu-bits.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/iommu.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_imsic.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pci.h"
#ಅನ್ತರ್ಭಾವ "hw/pci/pcie_host.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/aia.h"

#define UART_STD_CLOCK_FREQ 3686400 /* 3.6864 MHz */

ಶೂನ್ಯ *riscv_create_board_device_ವೃಕ್ಷ(ಧ್ರುವ ವರ್ಣ *model, ಧ್ರುವ ವರ್ಣ *compatible,
                                     ಪೂರ್ಣಾಙ್ಕ *fdt_size)
{
    ಶೂನ್ಯ *fdt = create_device_ವೃಕ್ಷ(fdt_size);

    ಯದಿ (!fdt) {
        error_report("create_device_tree() failed");
        exit(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", model);
    qemu_fdt_setprop_string(fdt, "/", "compatible", compatible);
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_add_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", ರಿಕ್ತ, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    ಪ್ರತ್ಯಾಗಮ fdt;
}

ಶೂನ್ಯ riscv_create_fdt_socket_memory(ಶೂನ್ಯ *fdt, hwaddr addr, uint64_t size,
                                    ಪೂರ್ಣಾಙ್ಕ socket_id, bool numa_enabled)
{
    g_autofree ವರ್ಣ *mem_name = g_strdup_printf("/memory@%"HWADDR_PRIx, addr);

    qemu_fdt_add_subnode(fdt, mem_name);
    qemu_fdt_setprop_sized_cells(fdt, mem_name, "reg", 2, addr, 2, size);
    qemu_fdt_setprop_string(fdt, mem_name, "device_type", "memory");

    ಯದಿ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, mem_name, "numa-node-id", socket_id);
    }
}

ಶೂನ್ಯ riscv_create_fdt_socket_clint(ಶೂನ್ಯ *fdt, hwaddr addr, uint64_t size,
                                   ಪೂರ್ಣಾಙ್ಕ socket_id, uint32_t *intc_phandles,
                                   ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_harts, bool numa_enabled)
{
    g_autofree uint32_t *clint_cells = g_new0(uint32_t, ಅಂಕೆ_harts * 4);
    g_autofree ವರ್ಣ *clint_name = ರಿಕ್ತ;
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ clint_compat[2] = {
        "sifive,clint0", "riscv,clint0"
    };

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cpu = 0; cpu < ಅಂಕೆ_harts; cpu++) {
        clint_cells[cpu * 4 + 0] = cpu_to_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 1] = cpu_to_be32(IRQ_M_SOFT);
        clint_cells[cpu * 4 + 2] = cpu_to_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 3] = cpu_to_be32(IRQ_M_TIMER);
    }

    clint_name = g_strdup_printf("/soc/clint@%"HWADDR_PRIx, addr);
    qemu_fdt_add_subnode(fdt, clint_name);
    qemu_fdt_setprop_string_array(fdt, clint_name, "compatible",
                                  (ವರ್ಣ **)&clint_compat,
                                  ARRAY_SIZE(clint_compat));
    qemu_fdt_setprop_sized_cells(fdt, clint_name, "reg",
                                 2, addr, 2, size);
    qemu_fdt_setprop(fdt, clint_name, "interrupts-extended",
                     clint_cells, ಅಂಕೆ_harts * ಪರಿಮಾಣ(uint32_t) * 4);

    ಯದಿ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, clint_name, "numa-node-id", socket_id);
    }
}

ಶೂನ್ಯ riscv_fdt_create_cpu_socket_subnode(ಶೂನ್ಯ *fdt,
                                         uint64_t timebase_frequency)
{
    qemu_fdt_add_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency",
                          timebase_frequency);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);
    qemu_fdt_add_subnode(fdt, "/cpus/cpu-map");
}

ಸ್ಥಿರ ಶೂನ್ಯ
create_fdt_socket_cpu_internal(ಶೂನ್ಯ *fdt, ವರ್ಣ *clust_name, RISCVCPU *cpu_ptr,
                               ಪೂರ್ಣಾಙ್ಕ cpu, ಪೂರ್ಣಾಙ್ಕ socket_id, ಪೂರ್ಣಾಙ್ಕ socket_hartid_base,
                               uint32_t *phandle, uint32_t *intc_phandles,
                               bool numa_enabled, bool is_32_bit)
{
    g_autofree ವರ್ಣ *cpu_name = ರಿಕ್ತ;
    g_autofree ವರ್ಣ *core_name = ರಿಕ್ತ;
    g_autofree ವರ್ಣ *intc_name = ರಿಕ್ತ;
    uint32_t cpu_phandle = (*phandle)++;
    bool is_sifive_u = cpu_ptr == ರಿಕ್ತ;

    cpu_name = g_strdup_printf("/cpus/cpu@%d", socket_hartid_base + cpu);

    /*
     * The sifive_u board has an exclusive satp and riscv,isa
     * schema that can't be shared with other boards, so part
     * of the CPU FDT creation (i.e. the /cpus/cpu@N subnode)
     * is still being done by the board.
     */
    ಯದಿ (!is_sifive_u) {
        int8_t satp_mode_max = cpu_ptr->cfg.max_satp_mode;

        qemu_fdt_add_subnode(fdt, cpu_name);

        ಯದಿ (satp_mode_max != -1) {
            g_autofree ವರ್ಣ *sv_name = ರಿಕ್ತ;
            sv_name = g_strdup_printf("riscv,%s",
                                      satp_mode_str(satp_mode_max, is_32_bit));
            qemu_fdt_setprop_string(fdt, cpu_name, "mmu-type", sv_name);
        }
        riscv_isa_write_fdt(cpu_ptr, fdt, cpu_name);

        ಯದಿ (cpu_ptr->cfg.ext_zicbom) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbom-block-size",
                                  cpu_ptr->cfg.cbom_blocksize);
        }

        ಯದಿ (cpu_ptr->cfg.ext_zicboz) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cboz-block-size",
                                  cpu_ptr->cfg.cboz_blocksize);
        }

        ಯದಿ (cpu_ptr->cfg.ext_zicbop) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbop-block-size",
                                  cpu_ptr->cfg.cbop_blocksize);
        }
    }

    qemu_fdt_setprop_string(fdt, cpu_name, "compatible", "riscv");
    qemu_fdt_setprop_string(fdt, cpu_name, "status", "okay");
    qemu_fdt_setprop_cell(fdt, cpu_name, "reg",
                          socket_hartid_base + cpu);
    qemu_fdt_setprop_string(fdt, cpu_name, "device_type", "cpu");
    ಯದಿ (numa_enabled) {
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
    qemu_fdt_setprop(fdt, intc_name, "interrupt-controller", ರಿಕ್ತ, 0);
    qemu_fdt_setprop_cell(fdt, intc_name, "#interrupt-cells", 1);

    core_name = g_strdup_printf("%s/core%d", clust_name, cpu);
    qemu_fdt_add_subnode(fdt, core_name);
    qemu_fdt_setprop_cell(fdt, core_name, "cpu", cpu_phandle);
}

ಶೂನ್ಯ riscv_create_fdt_socket_cpus(ಶೂನ್ಯ *fdt, RISCVCPU *socket_harts,
                                  ಪೂರ್ಣಾಙ್ಕ socket_id, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_harts_socket,
                                  ಪೂರ್ಣಾಙ್ಕ socket_hartid_base, uint32_t *phandle,
                                  uint32_t *intc_phandles, bool numa_enabled,
                                  bool is_32_bit)
{
    g_autofree ವರ್ಣ *clust_name = ರಿಕ್ತ;

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", socket_id);
    qemu_fdt_add_subnode(fdt, clust_name);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cpu = ಅಂಕೆ_harts_socket - 1; cpu >= 0; cpu--) {
        RISCVCPU *cpu_ptr = &socket_harts[cpu];

        create_fdt_socket_cpu_internal(fdt, clust_name, cpu_ptr, cpu,
                                       socket_id, socket_hartid_base,
                                       phandle, intc_phandles, numa_enabled,
                                       is_32_bit);
    }
}

ಶೂನ್ಯ
riscv_create_fdt_socket_cpu_sifive(ಶೂನ್ಯ *fdt, ವರ್ಣ *clust_name,
                                   ಪೂರ್ಣಾಙ್ಕ cpu_id, ಪೂರ್ಣಾಙ್ಕ socket_id,
                                   ಪೂರ್ಣಾಙ್ಕ socket_hartid_base, uint32_t *phandle,
                                   uint32_t *intc_phandles)
{
    create_fdt_socket_cpu_internal(fdt, clust_name, ರಿಕ್ತ, cpu_id,
                                   socket_id, socket_hartid_base,
                                   phandle, intc_phandles, false, false);
}

ಶೂನ್ಯ riscv_create_fdt_plic(ಶೂನ್ಯ *fdt, hwaddr addr, uint64_t size,
                           uint32_t plic_phandle, uint32_t int_cells,
                           uint32_t addr_cells, uint32_t *plic_cells,
                           uint32_t cells_size, uint32_t ndev_sources,
                           bool numa_enabled, ಪೂರ್ಣಾಙ್ಕ socket_id)
{
    g_autofree ವರ್ಣ *nodename = ರಿಕ್ತ;
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ plic_compat[2] = {
        "sifive,plic-1.0.0", "riscv,plic0"
    };

    nodename = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx, addr);

    qemu_fdt_add_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", int_cells);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", addr_cells);
    qemu_fdt_setprop_string_array(fdt, nodename, "compatible",
        (ವರ್ಣ **)&plic_compat, ARRAY_SIZE(plic_compat));
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", ರಿಕ್ತ, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
                     plic_cells, cells_size);
    qemu_fdt_setprop_sized_cells(fdt, nodename, "reg",
                                 2, addr, 2, size);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", ndev_sources);
    ಯದಿ (numa_enabled) {
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
ಶೂನ್ಯ riscv_pmu_generate_fdt_ಗಂಟು(ಶೂನ್ಯ *fdt, uint32_t cmask, ವರ್ಣ *pmu_name)
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
                    fdt_event_ctr_map, ಪರಿಮಾಣ(fdt_event_ctr_map));
}

ಶೂನ್ಯ riscv_create_fdt_flash(ಶೂನ್ಯ *fdt, hwaddr flashbase, hwaddr flashsize)
{
    g_autofree ವರ್ಣ *name = g_strdup_printf("/flash@%" PRIx64, flashbase);

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
ಶೂನ್ಯ riscv_create_fdt_syscon(ಶೂನ್ಯ *fdt, uint32_t *next_phandle,
                             hwaddr addr, hwaddr size,
                             uint32_t reboot, uint32_t poweroff,
                             bool sifive_test_compat)
{
    uint32_t syscon_phandle;
    ವರ್ಣ *name;

    name = g_strdup_printf("/soc/syscon@%"HWADDR_PRIx, addr);
    qemu_fdt_add_subnode(fdt, name);

    ಯದಿ (sifive_test_compat) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ compat[3] = {
            "sifive,test1", "sifive,test0", "syscon"
        };

        qemu_fdt_setprop_string_array(fdt, name, "compatible",
                                      (ವರ್ಣ **)&compat, ARRAY_SIZE(compat));
    } ಅನ್ಯಥಾ {
        qemu_fdt_setprop_string(fdt, name, "compatible", "syscon");
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr,
                                 2, size);

    syscon_phandle = next_phandle ? (*next_phandle)++ : qemu_fdt_alloc_phandle(fdt);
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

uint32_t riscv_create_fdt_riscv_iommu_sys(ಶೂನ್ಯ *fdt, hwaddr addr, hwaddr size,
                                          uint32_t *next_phandle,
                                          uint32_t irq_chip,
                                          uint32_t msi_phandle,
                                          uint32_t iommu_sys_irq)
{
    ಧ್ರುವ ವರ್ಣ comp[] = "riscv,iommu";
    g_autofree ವರ್ಣ *iommu_ಗಂಟು = ರಿಕ್ತ;
    uint32_t iommu_phandle;

    iommu_ಗಂಟು = g_strdup_printf("/soc/iommu@%"HWADDR_PRIx, addr);
    qemu_fdt_add_subnode(fdt, iommu_ಗಂಟು);

    qemu_fdt_setprop(fdt, iommu_ಗಂಟು, "compatible", comp, ಪರಿಮಾಣ(comp));
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "#iommu-cells", 1);

    iommu_phandle = next_phandle ? (*next_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "phandle", iommu_phandle);

    qemu_fdt_setprop_sized_cells(fdt, iommu_ಗಂಟು, "reg", 2, addr, 2, size);
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "interrupt-parent", irq_chip);

    qemu_fdt_setprop_cells(fdt, iommu_ಗಂಟು, "interrupts",
        iommu_sys_irq + RISCV_IOMMU_INTR_CQ, FDT_IRQ_TYPE_EDGE_ಕೆಳಗಿನ,
        iommu_sys_irq + RISCV_IOMMU_INTR_FQ, FDT_IRQ_TYPE_EDGE_ಕೆಳಗಿನ,
        iommu_sys_irq + RISCV_IOMMU_INTR_PM, FDT_IRQ_TYPE_EDGE_ಕೆಳಗಿನ,
        iommu_sys_irq + RISCV_IOMMU_INTR_PQ, FDT_IRQ_TYPE_EDGE_ಕೆಳಗಿನ);

    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "msi-parent", msi_phandle);

    ಪ್ರತ್ಯಾಗಮ iommu_phandle;
}

ಸ್ಥಿರ ಶೂನ್ಯ create_pcie_irq_map(ಶೂನ್ಯ *fdt, ವರ್ಣ *nodename,
                                uint32_t irqchip_phandle,
                                RISCVAIAType aia_type, uint32_t pcie_irq)
{
    ಪೂರ್ಣಾಙ್ಕ pin, dev;
    uint32_t irq_map_stride = 0;
    uint32_t full_irq_map[PCI_ಅಂಕೆ_PINS * PCI_ಅಂಕೆ_PINS *
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
    ಪ್ರತಿ (dev = 0; dev < PCI_ಅಂಕೆ_PINS; dev++) {
        ಪೂರ್ಣಾಙ್ಕ devfn = dev * 0x8;

        ಪ್ರತಿ (pin = 0; pin < PCI_ಅಂಕೆ_PINS; pin++) {
            ಪೂರ್ಣಾಙ್ಕ irq_nr = pcie_irq + ((pin + PCI_SLOT(devfn)) % PCI_ಅಂಕೆ_PINS);
            ಪೂರ್ಣಾಙ್ಕ i = 0;

            /* Fill PCI address cells */
            irq_map[i] = cpu_to_be32(devfn << 8);
            i += FDT_PCI_ADDR_CELLS;

            /* Fill PCI Interrupt cells */
            irq_map[i] = cpu_to_be32(pin + 1);
            i += FDT_PCI_INT_CELLS;

            /* Fill interrupt controller phandle and cells */
            irq_map[i++] = cpu_to_be32(irqchip_phandle);
            irq_map[i++] = cpu_to_be32(irq_nr);

            ಯದಿ (aia_type != AIA_TYPE_NONE) {
                irq_map[i++] = cpu_to_be32(0x4);
            }

            ಯದಿ (!irq_map_stride) {
                irq_map_stride = i;
            }
            irq_map += irq_map_stride;
        }
    }

    qemu_fdt_setprop(fdt, nodename, "interrupt-map", full_irq_map,
                     PCI_ಅಂಕೆ_PINS * PCI_ಅಂಕೆ_PINS *
                     irq_map_stride * ಪರಿಮಾಣ(uint32_t));

    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-map-mask",
                           0x1800, 0, 0, 0x7);
}

/*
 * NOTE: this function uses a "/soc/pci@..." FDT subnode that
 * should be created beforehand.
 */
ಶೂನ್ಯ riscv_create_fdt_pcie(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ aia_type, bool has_iommu_sys,
                           ಧ್ರುವ MemMapEntry *pcie_ecam,
                           ಧ್ರುವ MemMapEntry *pcie_pio,
                           ಧ್ರುವ MemMapEntry *pcie_mmio,
                           ಧ್ರುವ MemMapEntry *ಮೇಲಿನ_pcie,
                           uint32_t irq_pcie_phandle,
                           uint32_t msi_pcie_phandle,
                           uint32_t iommu_sys_phandle, uint32_t pcie_irq)
{
    g_autofree ವರ್ಣ *name = ರಿಕ್ತ;

    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx, pcie_ecam->base);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", FDT_PCI_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", FDT_PCI_INT_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0x2);
    qemu_fdt_setprop_string(fdt, name, "compatible", "pci-host-ecam-generic");
    qemu_fdt_setprop_string(fdt, name, "device_type", "pci");
    qemu_fdt_setprop_cell(fdt, name, "linux,pci-domain", 0);

    qemu_fdt_setprop_cells(fdt, name, "bus-range", 0,
                           pcie_ecam->size / PCIE_MMCFG_SIZE_MIN - 1);
    qemu_fdt_setprop(fdt, name, "dma-coherent", ರಿಕ್ತ, 0);

    ಯದಿ (aia_type == AIA_TYPE_APLIC_IMSIC) {
        qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_pcie_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2,
                                 pcie_ecam->base, 2, pcie_ecam->size);

    qemu_fdt_setprop_sized_cells(fdt, name, "ranges",
        1, FDT_PCI_RANGE_IOPORT, 2, 0,
        2, pcie_pio->base, 2, pcie_pio->size,
        1, FDT_PCI_RANGE_MMIO,
        2, pcie_mmio->base,
        2, pcie_mmio->base, 2, pcie_mmio->size,
        1, FDT_PCI_RANGE_MMIO_64BIT,
        2, ಮೇಲಿನ_pcie->base,
        2, ಮೇಲಿನ_pcie->base, 2, ಮೇಲಿನ_pcie->size);

    ಯದಿ (has_iommu_sys) {
        qemu_fdt_setprop_cells(fdt, name, "iommu-map",
                               0, iommu_sys_phandle, 0, 0x10000);
    }

    create_pcie_irq_map(fdt, name, irq_pcie_phandle, aia_type, pcie_irq);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_one_imsic(ಶೂನ್ಯ *fdt, IMSICFdtProps *props,
                                 hwaddr base_addr,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 bool m_mode, uint32_t imsic_guest_bits)
{
    RISCVHartArrayState *soc = (RISCVHartArrayState *)props->soc;
    g_autofree ವರ್ಣ *imsic_name = ರಿಕ್ತ;
    g_autofree uint32_t *imsic_cells = ರಿಕ್ತ;
    g_autofree uint32_t *imsic_regs = ರಿಕ್ತ;
    uint32_t imsic_max_hart_per_socket = 0;
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ imsic_compat[2] = {
        "qemu,imsics", "riscv,imsics"
    };

    imsic_cells = g_new0(uint32_t, props->smp_cpus * 2);
    imsic_regs = g_new0(uint32_t, props->socket_ಎಣಿಕೆ * 4);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cpu = 0; cpu < props->smp_cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_to_be32(m_mode ? IRQ_M_EXT : IRQ_S_EXT);
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ socket = 0; socket < props->socket_ಎಣಿಕೆ; socket++) {
        hwaddr imsic_addr = base_addr + socket * props->imsic_group_max_size;
        uint32_t imsic_size = IMSIC_HART_SIZE(imsic_guest_bits) *
                              soc[socket].ಅಂಕೆ_harts;

        imsic_regs[socket * 4 + 0] = cpu_to_be32(imsic_addr >> 32);
        imsic_regs[socket * 4 + 1] = cpu_to_be32(imsic_addr);
        imsic_regs[socket * 4 + 2] = 0;
        imsic_regs[socket * 4 + 3] = cpu_to_be32(imsic_size);
        ಯದಿ (imsic_max_hart_per_socket < soc[socket].ಅಂಕೆ_harts) {
            imsic_max_hart_per_socket = soc[socket].ಅಂಕೆ_harts;
        }
    }

    imsic_name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                                 base_addr);
    qemu_fdt_add_subnode(fdt, imsic_name);

    qemu_fdt_setprop_string_array(fdt, imsic_name, "compatible",
                                  (ವರ್ಣ **)&imsic_compat,
                                  ARRAY_SIZE(imsic_compat));

    qemu_fdt_setprop_cell(fdt, imsic_name, "#interrupt-cells",
                          FDT_IMSIC_INT_CELLS);
    qemu_fdt_setprop(fdt, imsic_name, "interrupt-controller", ರಿಕ್ತ, 0);
    qemu_fdt_setprop(fdt, imsic_name, "msi-controller", ರಿಕ್ತ, 0);
    qemu_fdt_setprop(fdt, imsic_name, "interrupts-extended",
                     imsic_cells, props->smp_cpus * ಪರಿಮಾಣ(uint32_t) * 2);
    qemu_fdt_setprop(fdt, imsic_name, "reg", imsic_regs,
                     props->socket_ಎಣಿಕೆ * ಪರಿಮಾಣ(uint32_t) * 4);
    qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,num-ids",
                          props->irqchip_ಅಂಕೆ_msis);

    ಯದಿ (imsic_guest_bits) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,guest-index-bits",
                              imsic_guest_bits);
    }

    ಯದಿ (props->socket_ಎಣಿಕೆ > 1) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,hart-index-bits",
                              imsic_ಅಂಕೆ_bits(imsic_max_hart_per_socket));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-bits",
                              imsic_ಅಂಕೆ_bits(props->socket_ಎಣಿಕೆ));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-shift",
                              IMSIC_MMIO_GROUP_MIN_SHIFT);
    }
    qemu_fdt_setprop_cell(fdt, imsic_name, "phandle", msi_phandle);
}

ಶೂನ್ಯ riscv_create_fdt_imsic(ಶೂನ್ಯ *fdt, IMSICFdtProps *props,
                            uint32_t *next_phandle, uint32_t *intc_phandles,
                            uint32_t *msi_m_phandle, uint32_t *msi_s_phandle)
{
    ಯದಿ (next_phandle) {
        *msi_m_phandle = (*next_phandle)++;
        *msi_s_phandle = (*next_phandle)++;
    } ಅನ್ಯಥಾ {
        *msi_m_phandle = qemu_fdt_alloc_phandle(fdt);
        *msi_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    ಯದಿ (props->imsic_m_base) {
        /* M-level IMSIC node */
        create_fdt_one_imsic(fdt, props, props->imsic_m_base,
                             intc_phandles, *msi_m_phandle, true, 0);
    }

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, props, props->imsic_s_base,
                         intc_phandles, *msi_s_phandle, false,
                         imsic_ಅಂಕೆ_bits(props->aia_guests + 1));

}

/* Caller must free string after use */
ಸ್ಥಿರ ವರ್ಣ *fdt_get_aplic_nodename(hwaddr aplic_addr)
{
    ಪ್ರತ್ಯಾಗಮ g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                           aplic_addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_one_aplic(ಶೂನ್ಯ *fdt, APLICFdtProps *props,
                                 hwaddr aplic_addr, uint32_t aplic_size,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_child_phandle,
                                 bool m_mode)
{
    g_autofree ವರ್ಣ *aplic_name = fdt_get_aplic_nodename(aplic_addr);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, props->ಅಂಕೆ_harts * 2);
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ aplic_compat[2] = {
        "qemu,aplic", "riscv,aplic"
    };

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ cpu = 0; cpu < props->ಅಂಕೆ_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_to_be32(m_mode ? IRQ_M_EXT : IRQ_S_EXT);
    }

    qemu_fdt_add_subnode(fdt, aplic_name);
    qemu_fdt_setprop_string_array(fdt, aplic_name, "compatible",
                                  (ವರ್ಣ **)&aplic_compat,
                                  ARRAY_SIZE(aplic_compat));
    qemu_fdt_setprop_cell(fdt, aplic_name, "#address-cells",
                          FDT_APLIC_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, aplic_name,
                          "#interrupt-cells", FDT_APLIC_INT_CELLS);
    qemu_fdt_setprop(fdt, aplic_name, "interrupt-controller", ರಿಕ್ತ, 0);

    ಯದಿ (props->aia_type == AIA_TYPE_APLIC) {
        qemu_fdt_setprop(fdt, aplic_name, "interrupts-extended",
                         aplic_cells, props->ಅಂಕೆ_harts * ಪರಿಮಾಣ(uint32_t) * 2);
    } ಅನ್ಯಥಾ {
        qemu_fdt_setprop_cell(fdt, aplic_name, "msi-parent", msi_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, aplic_name, "reg",
                                 2, aplic_addr, 2, aplic_size);
    qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,num-sources",
                          props->irqchip_ಅಂಕೆ_sources);

    ಯದಿ (aplic_child_phandle) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,children",
                              aplic_child_phandle);
        qemu_fdt_setprop_cells(fdt, aplic_name, "riscv,delegation",
                               aplic_child_phandle, 0x1,
                               props->irqchip_ಅಂಕೆ_sources);
    }

    ಯದಿ (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "numa-node-id", props->socket);
    }

    qemu_fdt_setprop_cell(fdt, aplic_name, "phandle", aplic_phandle);
}

ಶೂನ್ಯ riscv_create_fdt_socket_aplic(ಶೂನ್ಯ *fdt, APLICFdtProps *props,
                                   uint32_t msi_m_phandle,
                                   uint32_t msi_s_phandle,
                                   uint32_t *next_phandle,
                                   uint32_t *intc_phandles,
                                   uint32_t *aplic_phandles)
{
    uint32_t aplic_m_phandle, aplic_s_phandle;
    hwaddr aplic_addr;

    ಯದಿ (next_phandle) {
        aplic_m_phandle = (*next_phandle)++;
        aplic_s_phandle = (*next_phandle)++;
    } ಅನ್ಯಥಾ {
        aplic_m_phandle = qemu_fdt_alloc_phandle(fdt);
        aplic_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    ಯದಿ (props->aplic_m) {
        /* M-level APLIC node */
        aplic_addr = props->aplic_m->base + (props->aplic_m->size * props->socket);
        create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_m->size,
                             msi_m_phandle, intc_phandles,
                             aplic_m_phandle, aplic_s_phandle,
                             true);
    }

    /* S-level APLIC node */
    aplic_addr = props->aplic_s->base + (props->aplic_s->size * props->socket);
    create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_s->size,
                         msi_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         false);

    ಯದಿ (!props->socket && props->platform_bus_irq) {
        g_autofree ವರ್ಣ *aplic_name = fdt_get_aplic_nodename(aplic_addr);
        platform_bus_add_all_fdt_nodes(fdt, aplic_name,
                                       props->platform_bus->base,
                                       props->platform_bus->size,
                                       props->platform_bus_irq);
    }

    aplic_phandles[props->socket] = aplic_s_phandle;
}

ಶೂನ್ಯ riscv_create_fdt_socket_aclint(ಶೂನ್ಯ *fdt, ACLINTFdtProps *props,
                                    uint32_t *intc_phandles)
{
    uint32_t aclint_cells_size = props->ಅಂಕೆ_harts * ಪರಿಮಾಣ(uint32_t) * 2;
    g_autofree uint32_t *aclint_mswi_cells = ರಿಕ್ತ;
    g_autofree uint32_t *aclint_sswi_cells = ರಿಕ್ತ;
    g_autofree uint32_t *aclint_mtimer_cells = ರಿಕ್ತ;
    hwaddr addr, size;
    ವರ್ಣ *name;
    ಪೂರ್ಣಾಙ್ಕ cpu;

    aclint_mswi_cells = g_new0(uint32_t, props->ಅಂಕೆ_harts * 2);
    aclint_mtimer_cells = g_new0(uint32_t, props->ಅಂಕೆ_harts * 2);
    aclint_sswi_cells = g_new0(uint32_t, props->ಅಂಕೆ_harts * 2);

    ಪ್ರತಿ (cpu = 0; cpu < props->ಅಂಕೆ_harts; cpu++) {
        aclint_mswi_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aclint_mswi_cells[cpu * 2 + 1] = cpu_to_be32(IRQ_M_SOFT);
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_to_be32(IRQ_M_TIMER);
        aclint_sswi_cells[cpu * 2 + 0] = cpu_to_be32(intc_phandles[cpu]);
        aclint_sswi_cells[cpu * 2 + 1] = cpu_to_be32(IRQ_S_SOFT);
    }

    ಯದಿ (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->base + (props->clint->size * props->socket);
        name = g_strdup_printf("/soc/mswi@%"HWADDR_PRIx, addr);

        qemu_fdt_add_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, RISCV_ACLINT_SWI_SIZE);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_mswi_cells, aclint_cells_size);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ರಿಕ್ತ, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        ಯದಿ (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }

    ಯದಿ (props->aia_type == AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->base +
               (RISCV_ACLINT_DEFAULT_MTIMER_SIZE * props->socket);
        size = RISCV_ACLINT_DEFAULT_MTIMER_SIZE;
    } ಅನ್ಯಥಾ {
        addr = props->clint->base + RISCV_ACLINT_SWI_SIZE +
               (props->clint->size * props->socket);
        size = props->clint->size - RISCV_ACLINT_SWI_SIZE;
    }

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIx,
                           addr + RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible",
                            "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
        2, addr + RISCV_ACLINT_DEFAULT_MTIME,
        2, size - RISCV_ACLINT_DEFAULT_MTIME,
        2, addr + RISCV_ACLINT_DEFAULT_MTIMECMP,
        2, RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_size);

    ಯದಿ (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
    }

    g_free(name);

    ಯದಿ (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->aclint_sswi->base
               + (props->aclint_sswi->size * props->socket);

        name = g_strdup_printf("/soc/sswi@%"HWADDR_PRIx, addr);
        qemu_fdt_add_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible",
            "riscv,aclint-sswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, props->aclint_sswi->size);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_sswi_cells, aclint_cells_size);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ರಿಕ್ತ, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        ಯದಿ (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }
}

ವರ್ಣ *riscv_fdt_get_uart_nodename(hwaddr addr)
{
    ಪ್ರತ್ಯಾಗಮ g_strdup_printf("/soc/serial@%"HWADDR_PRIx, addr);
}

ಶೂನ್ಯ riscv_create_fdt_uart(ಶೂನ್ಯ *fdt, ಧ್ರುವ MemMapEntry *uart_mem,
                           ಪೂರ್ಣಾಙ್ಕ uart_irq, ಪೂರ್ಣಾಙ್ಕ aia_type,
                           uint32_t irq_phandle)
{
    g_autofree ವರ್ಣ *name = riscv_fdt_get_uart_nodename(uart_mem->base);

    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, uart_mem->base,
                                 2, uart_mem->size);

    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", UART_STD_CLOCK_FREQ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);

    ಯದಿ (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", uart_irq);
    } ಅನ್ಯಥಾ {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", uart_irq, 0x4);
    }
}

ಶೂನ್ಯ riscv_create_fdt_rtc(ಶೂನ್ಯ *fdt, ಧ್ರುವ MemMapEntry *rtc_mem,
                          ಪೂರ್ಣಾಙ್ಕ rtc_irq, ಪೂರ್ಣಾಙ್ಕ aia_type,
                          uint32_t irq_phandle)
{
    g_autofree ವರ್ಣ *name = ರಿಕ್ತ;

    name = g_strdup_printf("/soc/rtc@%"HWADDR_PRIx,
                           rtc_mem->base);
    qemu_fdt_add_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "google,goldfish-rtc");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, rtc_mem->base,
                                 2, rtc_mem->size);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);
    ಯದಿ (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", rtc_irq);
    } ಅನ್ಯಥಾ {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", rtc_irq, 0x4);
    }
}
