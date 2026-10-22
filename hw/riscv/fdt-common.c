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

#ವ್ಯಾಖ್ಯಾನಿಸು UART_STD_CLOCK_FREQ 3686400 /* 3.6864 MHz */

ಶೂನ್ಯ *riscv_create_board_device_ವೃಕ್ಷ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *model, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compatible,
                                     ಪೂರ್ಣಾಂಕ *fdt_ಗಾತ್ರ)
{
    ಶೂನ್ಯ *fdt = create_device_ವೃಕ್ಷ(fdt_ಗಾತ್ರ);

    ಆದರೆ (!fdt) {
        error_report("create_device_tree() failed");
        ನಿರ್ಗಮಿಸು(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", model);
    qemu_fdt_setprop_string(fdt, "/", "compatible", compatible);
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", ಖಾಲಿ, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    ಹಿಂತಿರುಗಿ fdt;
}

ಶೂನ್ಯ riscv_create_fdt_socket_ಸ್ಮರಣೆ(ಶೂನ್ಯ *fdt, hwaddr addr, uint64_t ಗಾತ್ರ_2,
                                    ಪೂರ್ಣಾಂಕ socket_id, bool numa_enabled)
{
    g_autofree ಅಕ್ಷರ *mem_name = g_strdup_printf("/memory@%"HWADDR_PRIx, addr);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, mem_name);
    qemu_fdt_setprop_sized_cells(fdt, mem_name, "reg", 2, addr, 2, ಗಾತ್ರ_2);
    qemu_fdt_setprop_string(fdt, mem_name, "device_type", "memory");

    ಆದರೆ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, mem_name, "numa-node-id", socket_id);
    }
}

ಶೂನ್ಯ riscv_create_fdt_socket_clint(ಶೂನ್ಯ *fdt, hwaddr addr, uint64_t ಗಾತ್ರ_2,
                                   ಪೂರ್ಣಾಂಕ socket_id, uint32_t *intc_phandles,
                                   ಪೂರ್ಣಾಂಕ ಅಂಕೆ_harts, bool numa_enabled)
{
    g_autofree uint32_t *clint_cells = g_new0(uint32_t, ಅಂಕೆ_harts * 4);
    g_autofree ಅಕ್ಷರ *clint_name = ಖಾಲಿ;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ clint_compat[2] = {
        "sifive,clint0", "riscv,clint0"
    };

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cpu = 0; cpu < ಅಂಕೆ_harts; cpu++) {
        clint_cells[cpu * 4 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 1] = cpu_ಗಮ್ಯ_be32(IRQ_M_SOFT);
        clint_cells[cpu * 4 + 2] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 3] = cpu_ಗಮ್ಯ_be32(IRQ_M_TIMER);
    }

    clint_name = g_strdup_printf("/soc/clint@%"HWADDR_PRIx, addr);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, clint_name);
    qemu_fdt_setprop_string_array(fdt, clint_name, "compatible",
                                  (ಅಕ್ಷರ **)&clint_compat,
                                  ARRAY_ಗಾತ್ರ(clint_compat));
    qemu_fdt_setprop_sized_cells(fdt, clint_name, "reg",
                                 2, addr, 2, ಗಾತ್ರ_2);
    qemu_fdt_setprop(fdt, clint_name, "interrupts-extended",
                     clint_cells, ಅಂಕೆ_harts * ಗಾತ್ರ(uint32_t) * 4);

    ಆದರೆ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, clint_name, "numa-node-id", socket_id);
    }
}

ಶೂನ್ಯ riscv_fdt_create_cpu_socket_subnode(ಶೂನ್ಯ *fdt,
                                         uint64_t timebase_frequency)
{
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency",
                          timebase_frequency);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, "/cpus/cpu-map");
}

ಸ್ಥಿರ ಶೂನ್ಯ
create_fdt_socket_cpu_ಆಂತರಿಕ(ಶೂನ್ಯ *fdt, ಅಕ್ಷರ *clust_name, RISCVCPU *cpu_ptr,
                               ಪೂರ್ಣಾಂಕ cpu, ಪೂರ್ಣಾಂಕ socket_id, ಪೂರ್ಣಾಂಕ socket_hartid_ತಳ,
                               uint32_t *phandle, uint32_t *intc_phandles,
                               bool numa_enabled, bool is_32_ಬಿಟ್)
{
    g_autofree ಅಕ್ಷರ *cpu_name = ಖಾಲಿ;
    g_autofree ಅಕ್ಷರ *core_name = ಖಾಲಿ;
    g_autofree ಅಕ್ಷರ *intc_name = ಖಾಲಿ;
    uint32_t cpu_phandle = (*phandle)++;
    bool is_sifive_u = cpu_ptr == ಖಾಲಿ;

    cpu_name = g_strdup_printf("/cpus/cpu@%d", socket_hartid_ತಳ + cpu);

    /*
     * The sifive_u board has an exclusive satp and riscv,isa
     * schema that can't be shared with other boards, so part
     * of the CPU FDT creation (i.e. the /cpus/cpu@N subnode)
     * is still being done by the board.
     */
    ಆದರೆ (!is_sifive_u) {
        int8_t satp_ಬಹುಲಕ_max = cpu_ptr->cfg.max_satp_ಬಹುಲಕ;

        qemu_fdt_ಕೂಡಿಸು_subnode(fdt, cpu_name);

        ಆದರೆ (satp_ಬಹುಲಕ_max != -1) {
            g_autofree ಅಕ್ಷರ *sv_name = ಖಾಲಿ;
            sv_name = g_strdup_printf("riscv,%s",
                                      satp_ಬಹುಲಕ_str(satp_ಬಹುಲಕ_max, is_32_ಬಿಟ್));
            qemu_fdt_setprop_string(fdt, cpu_name, "mmu-type", sv_name);
        }
        riscv_isa_write_fdt(cpu_ptr, fdt, cpu_name);

        ಆದರೆ (cpu_ptr->cfg.ext_zicbom) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbom-block-size",
                                  cpu_ptr->cfg.cbom_blocksize);
        }

        ಆದರೆ (cpu_ptr->cfg.ext_zicboz) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cboz-block-size",
                                  cpu_ptr->cfg.cboz_blocksize);
        }

        ಆದರೆ (cpu_ptr->cfg.ext_zicbop) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbop-block-size",
                                  cpu_ptr->cfg.cbop_blocksize);
        }
    }

    qemu_fdt_setprop_string(fdt, cpu_name, "compatible", "riscv");
    qemu_fdt_setprop_string(fdt, cpu_name, "status", "okay");
    qemu_fdt_setprop_cell(fdt, cpu_name, "reg",
                          socket_hartid_ತಳ + cpu);
    qemu_fdt_setprop_string(fdt, cpu_name, "device_type", "cpu");
    ಆದರೆ (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, cpu_name, "numa-node-id", socket_id);
    }
    qemu_fdt_setprop_cell(fdt, cpu_name, "phandle", cpu_phandle);

    intc_phandles[cpu] = (*phandle)++;

    intc_name = g_strdup_printf("%s/interrupt-controller", cpu_name);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, intc_name);
    qemu_fdt_setprop_cell(fdt, intc_name, "phandle",
                          intc_phandles[cpu]);
    qemu_fdt_setprop_string(fdt, intc_name, "compatible",
                            "riscv,cpu-intc");
    qemu_fdt_setprop(fdt, intc_name, "interrupt-controller", ಖಾಲಿ, 0);
    qemu_fdt_setprop_cell(fdt, intc_name, "#interrupt-cells", 1);

    core_name = g_strdup_printf("%s/core%d", clust_name, cpu);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, core_name);
    qemu_fdt_setprop_cell(fdt, core_name, "cpu", cpu_phandle);
}

ಶೂನ್ಯ riscv_create_fdt_socket_cpus(ಶೂನ್ಯ *fdt, RISCVCPU *socket_harts,
                                  ಪೂರ್ಣಾಂಕ socket_id, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_harts_socket,
                                  ಪೂರ್ಣಾಂಕ socket_hartid_ತಳ, uint32_t *phandle,
                                  uint32_t *intc_phandles, bool numa_enabled,
                                  bool is_32_ಬಿಟ್)
{
    g_autofree ಅಕ್ಷರ *clust_name = ಖಾಲಿ;

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", socket_id);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, clust_name);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cpu = ಅಂಕೆ_harts_socket - 1; cpu >= 0; cpu--) {
        RISCVCPU *cpu_ptr = &socket_harts[cpu];

        create_fdt_socket_cpu_ಆಂತರಿಕ(fdt, clust_name, cpu_ptr, cpu,
                                       socket_id, socket_hartid_ತಳ,
                                       phandle, intc_phandles, numa_enabled,
                                       is_32_ಬಿಟ್);
    }
}

ಶೂನ್ಯ
riscv_create_fdt_socket_cpu_sifive(ಶೂನ್ಯ *fdt, ಅಕ್ಷರ *clust_name,
                                   ಪೂರ್ಣಾಂಕ cpu_id, ಪೂರ್ಣಾಂಕ socket_id,
                                   ಪೂರ್ಣಾಂಕ socket_hartid_ತಳ, uint32_t *phandle,
                                   uint32_t *intc_phandles)
{
    create_fdt_socket_cpu_ಆಂತರಿಕ(fdt, clust_name, ಖಾಲಿ, cpu_id,
                                   socket_id, socket_hartid_ತಳ,
                                   phandle, intc_phandles, false, false);
}

ಶೂನ್ಯ riscv_create_fdt_plic(ಶೂನ್ಯ *fdt, hwaddr addr, uint64_t ಗಾತ್ರ_2,
                           uint32_t plic_phandle, uint32_t int_cells,
                           uint32_t addr_cells, uint32_t *plic_cells,
                           uint32_t cells_ಗಾತ್ರ, uint32_t ndev_sources,
                           bool numa_enabled, ಪೂರ್ಣಾಂಕ socket_id)
{
    g_autofree ಅಕ್ಷರ *nodename = ಖಾಲಿ;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ plic_compat[2] = {
        "sifive,plic-1.0.0", "riscv,plic0"
    };

    nodename = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx, addr);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", int_cells);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", addr_cells);
    qemu_fdt_setprop_string_array(fdt, nodename, "compatible",
        (ಅಕ್ಷರ **)&plic_compat, ARRAY_ಗಾತ್ರ(plic_compat));
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", ಖಾಲಿ, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
                     plic_cells, cells_ಗಾತ್ರ);
    qemu_fdt_setprop_sized_cells(fdt, nodename, "reg",
                                 2, addr, 2, ಗಾತ್ರ_2);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", ndev_sources);
    ಆದರೆ (numa_enabled) {
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
ಶೂನ್ಯ riscv_pmu_generate_fdt_ಗಂಟು(ಶೂನ್ಯ *fdt, uint32_t cmask, ಅಕ್ಷರ *pmu_name)
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
   fdt_event_ctr_map[0] = cpu_ಗಮ್ಯ_be32(0x00000001);
   fdt_event_ctr_map[1] = cpu_ಗಮ್ಯ_be32(0x00000001);
   fdt_event_ctr_map[2] = cpu_ಗಮ್ಯ_be32(cmask | 1 << 0);

   /* SBI_PMU_HW_INSTRUCTIONS: 0x02 : type(0x00) */
   fdt_event_ctr_map[3] = cpu_ಗಮ್ಯ_be32(0x00000002);
   fdt_event_ctr_map[4] = cpu_ಗಮ್ಯ_be32(0x00000002);
   fdt_event_ctr_map[5] = cpu_ಗಮ್ಯ_be32(cmask | 1 << 2);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[6] = cpu_ಗಮ್ಯ_be32(0x00010019);
   fdt_event_ctr_map[7] = cpu_ಗಮ್ಯ_be32(0x00010019);
   fdt_event_ctr_map[8] = cpu_ಗಮ್ಯ_be32(cmask);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 WRITE : 0x01 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[9] = cpu_ಗಮ್ಯ_be32(0x0001001B);
   fdt_event_ctr_map[10] = cpu_ಗಮ್ಯ_be32(0x0001001B);
   fdt_event_ctr_map[11] = cpu_ಗಮ್ಯ_be32(cmask);

   /* SBI_PMU_HW_CACHE_ITLB : 0x04 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[12] = cpu_ಗಮ್ಯ_be32(0x00010021);
   fdt_event_ctr_map[13] = cpu_ಗಮ್ಯ_be32(0x00010021);
   fdt_event_ctr_map[14] = cpu_ಗಮ್ಯ_be32(cmask);

   /* This a OpenSBI specific DT property documented in OpenSBI docs */
   qemu_fdt_setprop(fdt, pmu_name, "riscv,event-to-mhpmcounters",
                    fdt_event_ctr_map, ಗಾತ್ರ(fdt_event_ctr_map));
}

ಶೂನ್ಯ riscv_create_fdt_flash(ಶೂನ್ಯ *fdt, hwaddr flashbase, hwaddr flashsize)
{
    g_autofree ಅಕ್ಷರ *name = g_strdup_printf("/flash@%" PRIx64, flashbase);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
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
ಶೂನ್ಯ riscv_create_fdt_syscon(ಶೂನ್ಯ *fdt, uint32_t *ಮುಂದಿನ_phandle,
                             hwaddr addr, hwaddr ಗಾತ್ರ_2,
                             uint32_t reboot, uint32_t poweroff,
                             bool sifive_test_compat)
{
    uint32_t syscon_phandle;
    ಅಕ್ಷರ *name;

    name = g_strdup_printf("/soc/syscon@%"HWADDR_PRIx, addr);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);

    ಆದರೆ (sifive_test_compat) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ compat[3] = {
            "sifive,test1", "sifive,test0", "syscon"
        };

        qemu_fdt_setprop_string_array(fdt, name, "compatible",
                                      (ಅಕ್ಷರ **)&compat, ARRAY_ಗಾತ್ರ(compat));
    } ಇಲ್ಲದಿದ್ದರೆ {
        qemu_fdt_setprop_string(fdt, name, "compatible", "syscon");
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr,
                                 2, ಗಾತ್ರ_2);

    syscon_phandle = ಮುಂದಿನ_phandle ? (*ಮುಂದಿನ_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, name, "phandle", syscon_phandle);

    g_free(name);

    name = g_strdup_printf("/reboot");
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-reboot");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", reboot);
    g_free(name);

    name = g_strdup_printf("/poweroff");
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-poweroff");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", poweroff);
    g_free(name);
}

uint32_t riscv_create_fdt_riscv_iommu_sys(ಶೂನ್ಯ *fdt, hwaddr addr, hwaddr ಗಾತ್ರ_2,
                                          uint32_t *ಮುಂದಿನ_phandle,
                                          uint32_t irq_chip,
                                          uint32_t msi_phandle,
                                          uint32_t iommu_sys_irq)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ comp[] = "riscv,iommu";
    g_autofree ಅಕ್ಷರ *iommu_ಗಂಟು = ಖಾಲಿ;
    uint32_t iommu_phandle;

    iommu_ಗಂಟು = g_strdup_printf("/soc/iommu@%"HWADDR_PRIx, addr);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, iommu_ಗಂಟು);

    qemu_fdt_setprop(fdt, iommu_ಗಂಟು, "compatible", comp, ಗಾತ್ರ(comp));
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "#iommu-cells", 1);

    iommu_phandle = ಮುಂದಿನ_phandle ? (*ಮುಂದಿನ_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "phandle", iommu_phandle);

    qemu_fdt_setprop_sized_cells(fdt, iommu_ಗಂಟು, "reg", 2, addr, 2, ಗಾತ್ರ_2);
    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "interrupt-parent", irq_chip);

    qemu_fdt_setprop_cells(fdt, iommu_ಗಂಟು, "interrupts",
        iommu_sys_irq + RISCV_IOMMU_INTR_CQ, FDT_IRQ_TYPE_ಅಂಚು_ಕೆಳಗಿನ,
        iommu_sys_irq + RISCV_IOMMU_INTR_FQ, FDT_IRQ_TYPE_ಅಂಚು_ಕೆಳಗಿನ,
        iommu_sys_irq + RISCV_IOMMU_INTR_PM, FDT_IRQ_TYPE_ಅಂಚು_ಕೆಳಗಿನ,
        iommu_sys_irq + RISCV_IOMMU_INTR_PQ, FDT_IRQ_TYPE_ಅಂಚು_ಕೆಳಗಿನ);

    qemu_fdt_setprop_cell(fdt, iommu_ಗಂಟು, "msi-parent", msi_phandle);

    ಹಿಂತಿರುಗಿ iommu_phandle;
}

ಸ್ಥಿರ ಶೂನ್ಯ create_pcie_irq_map(ಶೂನ್ಯ *fdt, ಅಕ್ಷರ *nodename,
                                uint32_t irqchip_phandle,
                                RISCVAIAType aia_type, uint32_t pcie_irq)
{
    ಪೂರ್ಣಾಂಕ pin, dev;
    uint32_t irq_map_stride = 0;
    uint32_t ತುಂಬಿದ_irq_map[PCI_ಅಂಕೆ_PINS * PCI_ಅಂಕೆ_PINS *
                          FDT_MAX_INT_MAP_ಅಗಲ] = {};
    uint32_t *irq_map = ತುಂಬಿದ_irq_map;

    /*
     * This code creates a standard swizzle of interrupts such that
     * each device's first interrupt is based on it's PCI_SLOT number.
     * (See pci_swizzle_map_irq_fn())
     *
     * We only need one entry per interrupt in the table (not one per
     * possible slot) seeing the interrupt-map-mask will allow the table
     * to wrap to any number of devices.
     */
    ಗಾಗಿ (dev = 0; dev < PCI_ಅಂಕೆ_PINS; dev++) {
        ಪೂರ್ಣಾಂಕ devfn = dev * 0x8;

        ಗಾಗಿ (pin = 0; pin < PCI_ಅಂಕೆ_PINS; pin++) {
            ಪೂರ್ಣಾಂಕ irq_nr = pcie_irq + ((pin + PCI_ಸ್ಥಾನ(devfn)) % PCI_ಅಂಕೆ_PINS);
            ಪೂರ್ಣಾಂಕ i = 0;

            /* Fill PCI address cells */
            irq_map[i] = cpu_ಗಮ್ಯ_be32(devfn << 8);
            i += FDT_PCI_ADDR_CELLS;

            /* Fill PCI Interrupt cells */
            irq_map[i] = cpu_ಗಮ್ಯ_be32(pin + 1);
            i += FDT_PCI_INT_CELLS;

            /* Fill interrupt controller phandle and cells */
            irq_map[i++] = cpu_ಗಮ್ಯ_be32(irqchip_phandle);
            irq_map[i++] = cpu_ಗಮ್ಯ_be32(irq_nr);

            ಆದರೆ (aia_type != AIA_TYPE_NONE) {
                irq_map[i++] = cpu_ಗಮ್ಯ_be32(0x4);
            }

            ಆದರೆ (!irq_map_stride) {
                irq_map_stride = i;
            }
            irq_map += irq_map_stride;
        }
    }

    qemu_fdt_setprop(fdt, nodename, "interrupt-map", ತುಂಬಿದ_irq_map,
                     PCI_ಅಂಕೆ_PINS * PCI_ಅಂಕೆ_PINS *
                     irq_map_stride * ಗಾತ್ರ(uint32_t));

    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-map-mask",
                           0x1800, 0, 0, 0x7);
}

/*
 * NOTE: this function uses a "/soc/pci@..." FDT subnode that
 * should be created beforehand.
 */
ಶೂನ್ಯ riscv_create_fdt_pcie(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ aia_type, bool has_iommu_sys,
                           ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *pcie_ecam,
                           ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *pcie_pio,
                           ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *pcie_mmio,
                           ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *ಮೇಲಿನ_pcie,
                           uint32_t irq_pcie_phandle,
                           uint32_t msi_pcie_phandle,
                           uint32_t iommu_sys_phandle, uint32_t pcie_irq)
{
    g_autofree ಅಕ್ಷರ *name = ಖಾಲಿ;

    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx, pcie_ecam->ತಳ);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", FDT_PCI_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", FDT_PCI_INT_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0x2);
    qemu_fdt_setprop_string(fdt, name, "compatible", "pci-host-ecam-generic");
    qemu_fdt_setprop_string(fdt, name, "device_type", "pci");
    qemu_fdt_setprop_cell(fdt, name, "linux,pci-domain", 0);

    qemu_fdt_setprop_cells(fdt, name, "bus-range", 0,
                           pcie_ecam->ಗಾತ್ರ_2 / PCIE_MMCFG_ಗಾತ್ರ_MIN - 1);
    qemu_fdt_setprop(fdt, name, "dma-coherent", ಖಾಲಿ, 0);

    ಆದರೆ (aia_type == AIA_TYPE_APLIC_IMSIC) {
        qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_pcie_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2,
                                 pcie_ecam->ತಳ, 2, pcie_ecam->ಗಾತ್ರ_2);

    qemu_fdt_setprop_sized_cells(fdt, name, "ranges",
        1, FDT_PCI_RANGE_IOPORT, 2, 0,
        2, pcie_pio->ತಳ, 2, pcie_pio->ಗಾತ್ರ_2,
        1, FDT_PCI_RANGE_MMIO,
        2, pcie_mmio->ತಳ,
        2, pcie_mmio->ತಳ, 2, pcie_mmio->ಗಾತ್ರ_2,
        1, FDT_PCI_RANGE_MMIO_64_ಬಿಟ್,
        2, ಮೇಲಿನ_pcie->ತಳ,
        2, ಮೇಲಿನ_pcie->ತಳ, 2, ಮೇಲಿನ_pcie->ಗಾತ್ರ_2);

    ಆದರೆ (has_iommu_sys) {
        qemu_fdt_setprop_cells(fdt, name, "iommu-map",
                               0, iommu_sys_phandle, 0, 0x10000);
    }

    create_pcie_irq_map(fdt, name, irq_pcie_phandle, aia_type, pcie_irq);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_one_imsic(ಶೂನ್ಯ *fdt, IMSICFdtProps *props,
                                 hwaddr ತಳ_addr,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 bool m_ಬಹುಲಕ, uint32_t imsic_guest_ಬಿಟ್ಟುಗಳು)
{
    RISCVHartArrayState *soc = (RISCVHartArrayState *)props->soc;
    g_autofree ಅಕ್ಷರ *imsic_name = ಖಾಲಿ;
    g_autofree uint32_t *imsic_cells = ಖಾಲಿ;
    g_autofree uint32_t *imsic_regs = ಖಾಲಿ;
    uint32_t imsic_max_hart_ಪ್ರತಿ_socket = 0;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ imsic_compat[2] = {
        "qemu,imsics", "riscv,imsics"
    };

    imsic_cells = g_new0(uint32_t, props->smp_cpus * 2);
    imsic_regs = g_new0(uint32_t, props->socket_ಎಣಿಕೆ * 4);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cpu = 0; cpu < props->smp_cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(m_ಬಹುಲಕ ? IRQ_M_EXT : IRQ_S_EXT);
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ socket = 0; socket < props->socket_ಎಣಿಕೆ; socket++) {
        hwaddr imsic_addr = ತಳ_addr + socket * props->imsic_ಗುಂಪು_max_ಗಾತ್ರ;
        uint32_t imsic_ಗಾತ್ರ = IMSIC_HART_ಗಾತ್ರ(imsic_guest_ಬಿಟ್ಟುಗಳು) *
                              soc[socket].ಅಂಕೆ_harts;

        imsic_regs[socket * 4 + 0] = cpu_ಗಮ್ಯ_be32(imsic_addr >> 32);
        imsic_regs[socket * 4 + 1] = cpu_ಗಮ್ಯ_be32(imsic_addr);
        imsic_regs[socket * 4 + 2] = 0;
        imsic_regs[socket * 4 + 3] = cpu_ಗಮ್ಯ_be32(imsic_ಗಾತ್ರ);
        ಆದರೆ (imsic_max_hart_ಪ್ರತಿ_socket < soc[socket].ಅಂಕೆ_harts) {
            imsic_max_hart_ಪ್ರತಿ_socket = soc[socket].ಅಂಕೆ_harts;
        }
    }

    imsic_name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                                 ತಳ_addr);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, imsic_name);

    qemu_fdt_setprop_string_array(fdt, imsic_name, "compatible",
                                  (ಅಕ್ಷರ **)&imsic_compat,
                                  ARRAY_ಗಾತ್ರ(imsic_compat));

    qemu_fdt_setprop_cell(fdt, imsic_name, "#interrupt-cells",
                          FDT_IMSIC_INT_CELLS);
    qemu_fdt_setprop(fdt, imsic_name, "interrupt-controller", ಖಾಲಿ, 0);
    qemu_fdt_setprop(fdt, imsic_name, "msi-controller", ಖಾಲಿ, 0);
    qemu_fdt_setprop(fdt, imsic_name, "interrupts-extended",
                     imsic_cells, props->smp_cpus * ಗಾತ್ರ(uint32_t) * 2);
    qemu_fdt_setprop(fdt, imsic_name, "reg", imsic_regs,
                     props->socket_ಎಣಿಕೆ * ಗಾತ್ರ(uint32_t) * 4);
    qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,num-ids",
                          props->irqchip_ಅಂಕೆ_msis);

    ಆದರೆ (imsic_guest_ಬಿಟ್ಟುಗಳು) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,guest-index-bits",
                              imsic_guest_ಬಿಟ್ಟುಗಳು);
    }

    ಆದರೆ (props->socket_ಎಣಿಕೆ > 1) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,hart-index-bits",
                              imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(imsic_max_hart_ಪ್ರತಿ_socket));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-bits",
                              imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(props->socket_ಎಣಿಕೆ));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-shift",
                              IMSIC_MMIO_ಗುಂಪು_MIN_ಸರಿಸು);
    }
    qemu_fdt_setprop_cell(fdt, imsic_name, "phandle", msi_phandle);
}

ಶೂನ್ಯ riscv_create_fdt_imsic(ಶೂನ್ಯ *fdt, IMSICFdtProps *props,
                            uint32_t *ಮುಂದಿನ_phandle, uint32_t *intc_phandles,
                            uint32_t *msi_m_phandle, uint32_t *msi_s_phandle)
{
    ಆದರೆ (ಮುಂದಿನ_phandle) {
        *msi_m_phandle = (*ಮುಂದಿನ_phandle)++;
        *msi_s_phandle = (*ಮುಂದಿನ_phandle)++;
    } ಇಲ್ಲದಿದ್ದರೆ {
        *msi_m_phandle = qemu_fdt_alloc_phandle(fdt);
        *msi_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    ಆದರೆ (props->imsic_m_ತಳ) {
        /* M-level IMSIC node */
        create_fdt_one_imsic(fdt, props, props->imsic_m_ತಳ,
                             intc_phandles, *msi_m_phandle, true, 0);
    }

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, props, props->imsic_s_ತಳ,
                         intc_phandles, *msi_s_phandle, false,
                         imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(props->aia_guests + 1));

}

/* Caller must free string after use */
ಸ್ಥಿರ ಅಕ್ಷರ *fdt_get_aplic_nodename(hwaddr aplic_addr)
{
    ಹಿಂತಿರುಗಿ g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                           aplic_addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt_one_aplic(ಶೂನ್ಯ *fdt, APLICFdtProps *props,
                                 hwaddr aplic_addr, uint32_t aplic_ಗಾತ್ರ,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_ಮಗು_phandle,
                                 bool m_ಬಹುಲಕ)
{
    g_autofree ಅಕ್ಷರ *aplic_name = fdt_get_aplic_nodename(aplic_addr);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, props->ಅಂಕೆ_harts * 2);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ aplic_compat[2] = {
        "qemu,aplic", "riscv,aplic"
    };

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cpu = 0; cpu < props->ಅಂಕೆ_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(m_ಬಹುಲಕ ? IRQ_M_EXT : IRQ_S_EXT);
    }

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, aplic_name);
    qemu_fdt_setprop_string_array(fdt, aplic_name, "compatible",
                                  (ಅಕ್ಷರ **)&aplic_compat,
                                  ARRAY_ಗಾತ್ರ(aplic_compat));
    qemu_fdt_setprop_cell(fdt, aplic_name, "#address-cells",
                          FDT_APLIC_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, aplic_name,
                          "#interrupt-cells", FDT_APLIC_INT_CELLS);
    qemu_fdt_setprop(fdt, aplic_name, "interrupt-controller", ಖಾಲಿ, 0);

    ಆದರೆ (props->aia_type == AIA_TYPE_APLIC) {
        qemu_fdt_setprop(fdt, aplic_name, "interrupts-extended",
                         aplic_cells, props->ಅಂಕೆ_harts * ಗಾತ್ರ(uint32_t) * 2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        qemu_fdt_setprop_cell(fdt, aplic_name, "msi-parent", msi_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, aplic_name, "reg",
                                 2, aplic_addr, 2, aplic_ಗಾತ್ರ);
    qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,num-sources",
                          props->irqchip_ಅಂಕೆ_sources);

    ಆದರೆ (aplic_ಮಗು_phandle) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,children",
                              aplic_ಮಗು_phandle);
        qemu_fdt_setprop_cells(fdt, aplic_name, "riscv,delegation",
                               aplic_ಮಗು_phandle, 0x1,
                               props->irqchip_ಅಂಕೆ_sources);
    }

    ಆದರೆ (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "numa-node-id", props->socket);
    }

    qemu_fdt_setprop_cell(fdt, aplic_name, "phandle", aplic_phandle);
}

ಶೂನ್ಯ riscv_create_fdt_socket_aplic(ಶೂನ್ಯ *fdt, APLICFdtProps *props,
                                   uint32_t msi_m_phandle,
                                   uint32_t msi_s_phandle,
                                   uint32_t *ಮುಂದಿನ_phandle,
                                   uint32_t *intc_phandles,
                                   uint32_t *aplic_phandles)
{
    uint32_t aplic_m_phandle, aplic_s_phandle;
    hwaddr aplic_addr;

    ಆದರೆ (ಮುಂದಿನ_phandle) {
        aplic_m_phandle = (*ಮುಂದಿನ_phandle)++;
        aplic_s_phandle = (*ಮುಂದಿನ_phandle)++;
    } ಇಲ್ಲದಿದ್ದರೆ {
        aplic_m_phandle = qemu_fdt_alloc_phandle(fdt);
        aplic_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    ಆದರೆ (props->aplic_m) {
        /* M-level APLIC node */
        aplic_addr = props->aplic_m->ತಳ + (props->aplic_m->ಗಾತ್ರ_2 * props->socket);
        create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_m->ಗಾತ್ರ_2,
                             msi_m_phandle, intc_phandles,
                             aplic_m_phandle, aplic_s_phandle,
                             true);
    }

    /* S-level APLIC node */
    aplic_addr = props->aplic_s->ತಳ + (props->aplic_s->ಗಾತ್ರ_2 * props->socket);
    create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_s->ಗಾತ್ರ_2,
                         msi_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         false);

    ಆದರೆ (!props->socket && props->platform_bus_irq) {
        g_autofree ಅಕ್ಷರ *aplic_name = fdt_get_aplic_nodename(aplic_addr);
        platform_bus_ಕೂಡಿಸು_all_fdt_nodes(fdt, aplic_name,
                                       props->platform_bus->ತಳ,
                                       props->platform_bus->ಗಾತ್ರ_2,
                                       props->platform_bus_irq);
    }

    aplic_phandles[props->socket] = aplic_s_phandle;
}

ಶೂನ್ಯ riscv_create_fdt_socket_aclint(ಶೂನ್ಯ *fdt, ACLINTFdtProps *props,
                                    uint32_t *intc_phandles)
{
    uint32_t aclint_cells_ಗಾತ್ರ = props->ಅಂಕೆ_harts * ಗಾತ್ರ(uint32_t) * 2;
    g_autofree uint32_t *aclint_mswi_cells = ಖಾಲಿ;
    g_autofree uint32_t *aclint_sswi_cells = ಖಾಲಿ;
    g_autofree uint32_t *aclint_mtimer_cells = ಖಾಲಿ;
    hwaddr addr, ಗಾತ್ರ_2;
    ಅಕ್ಷರ *name;
    ಪೂರ್ಣಾಂಕ cpu;

    aclint_mswi_cells = g_new0(uint32_t, props->ಅಂಕೆ_harts * 2);
    aclint_mtimer_cells = g_new0(uint32_t, props->ಅಂಕೆ_harts * 2);
    aclint_sswi_cells = g_new0(uint32_t, props->ಅಂಕೆ_harts * 2);

    ಗಾಗಿ (cpu = 0; cpu < props->ಅಂಕೆ_harts; cpu++) {
        aclint_mswi_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        aclint_mswi_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(IRQ_M_SOFT);
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(IRQ_M_TIMER);
        aclint_sswi_cells[cpu * 2 + 0] = cpu_ಗಮ್ಯ_be32(intc_phandles[cpu]);
        aclint_sswi_cells[cpu * 2 + 1] = cpu_ಗಮ್ಯ_be32(IRQ_S_SOFT);
    }

    ಆದರೆ (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->ತಳ + (props->clint->ಗಾತ್ರ_2 * props->socket);
        name = g_strdup_printf("/soc/mswi@%"HWADDR_PRIx, addr);

        qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, RISCV_ACLINT_SWI_ಗಾತ್ರ);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_mswi_cells, aclint_cells_ಗಾತ್ರ);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ಖಾಲಿ, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        ಆದರೆ (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }

    ಆದರೆ (props->aia_type == AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->ತಳ +
               (RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ * props->socket);
        ಗಾತ್ರ_2 = RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        addr = props->clint->ತಳ + RISCV_ACLINT_SWI_ಗಾತ್ರ +
               (props->clint->ಗಾತ್ರ_2 * props->socket);
        ಗಾತ್ರ_2 = props->clint->ಗಾತ್ರ_2 - RISCV_ACLINT_SWI_ಗಾತ್ರ;
    }

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIx,
                           addr + RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible",
                            "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
        2, addr + RISCV_ACLINT_DEFAULT_MTIME,
        2, ಗಾತ್ರ_2 - RISCV_ACLINT_DEFAULT_MTIME,
        2, addr + RISCV_ACLINT_DEFAULT_MTIMECMP,
        2, RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_ಗಾತ್ರ);

    ಆದರೆ (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
    }

    g_free(name);

    ಆದರೆ (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->aclint_sswi->ತಳ
               + (props->aclint_sswi->ಗಾತ್ರ_2 * props->socket);

        name = g_strdup_printf("/soc/sswi@%"HWADDR_PRIx, addr);
        qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible",
            "riscv,aclint-sswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, props->aclint_sswi->ಗಾತ್ರ_2);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_sswi_cells, aclint_cells_ಗಾತ್ರ);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", ಖಾಲಿ, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        ಆದರೆ (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }
}

ಅಕ್ಷರ *riscv_fdt_get_uart_nodename(hwaddr addr)
{
    ಹಿಂತಿರುಗಿ g_strdup_printf("/soc/serial@%"HWADDR_PRIx, addr);
}

ಶೂನ್ಯ riscv_create_fdt_uart(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *uart_mem,
                           ಪೂರ್ಣಾಂಕ uart_irq, ಪೂರ್ಣಾಂಕ aia_type,
                           uint32_t irq_phandle)
{
    g_autofree ಅಕ್ಷರ *name = riscv_fdt_get_uart_nodename(uart_mem->ತಳ);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, uart_mem->ತಳ,
                                 2, uart_mem->ಗಾತ್ರ_2);

    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", UART_STD_CLOCK_FREQ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);

    ಆದರೆ (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", uart_irq);
    } ಇಲ್ಲದಿದ್ದರೆ {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", uart_irq, 0x4);
    }
}

ಶೂನ್ಯ riscv_create_fdt_rtc(ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *rtc_mem,
                          ಪೂರ್ಣಾಂಕ rtc_irq, ಪೂರ್ಣಾಂಕ aia_type,
                          uint32_t irq_phandle)
{
    g_autofree ಅಕ್ಷರ *name = ಖಾಲಿ;

    name = g_strdup_printf("/soc/rtc@%"HWADDR_PRIx,
                           rtc_mem->ತಳ);
    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "google,goldfish-rtc");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, rtc_mem->ತಳ,
                                 2, rtc_mem->ಗಾತ್ರ_2);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);
    ಆದರೆ (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", rtc_irq);
    } ಇಲ್ಲದಿದ್ದರೆ {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", rtc_irq, 0x4);
    }
}
