/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#شامل "qemu/osdep.h"

#شامل "qemu/error-report.h"
#شامل "system/device_tree.h"
#شامل "hw/core/boards.h"
#شامل "hw/core/sysbus-fdt.h"
#شامل "hw/riscv/fdt-common.h"
#شامل "target/riscv/cpu_bits.h"
#شامل "hw/riscv/riscv-iommu-bits.h"
#شامل "hw/riscv/iommu.h"
#شامل "hw/intc/riscv_aclint.h"
#شامل "hw/intc/riscv_imsic.h"
#شامل "hw/pci/pci.h"
#شامل "hw/pci/pcie_host.h"
#شامل "hw/riscv/aia.h"

#تعریف_کرو UART_STD_CLOCK_FREQ 3686400 /* 3.6864 MHz */

خالی *riscv_create_board_device_درخت(ثابت حرف *model, ثابت حرف *compatible,
                                     صحیح *fdt_سائز)
{
    خالی *fdt = create_device_درخت(fdt_سائز);

    اگر (!fdt) {
        error_report("create_device_tree() failed");
        باہر_نکلو(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", model);
    qemu_fdt_setprop_string(fdt, "/", "compatible", compatible);
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_جوڑو_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", تہی, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    واپس fdt;
}

خالی riscv_create_fdt_socket_حافظہ(خالی *fdt, hwaddr addr, uint64_t سائز,
                                    صحیح socket_id, bool numa_enabled)
{
    g_autofree حرف *mem_name = g_strdup_printf("/memory@%"HWADDR_PRIx, addr);

    qemu_fdt_جوڑو_subnode(fdt, mem_name);
    qemu_fdt_setprop_sized_cells(fdt, mem_name, "reg", 2, addr, 2, سائز);
    qemu_fdt_setprop_string(fdt, mem_name, "device_type", "memory");

    اگر (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, mem_name, "numa-node-id", socket_id);
    }
}

خالی riscv_create_fdt_socket_clint(خالی *fdt, hwaddr addr, uint64_t سائز,
                                   صحیح socket_id, uint32_t *intc_phandles,
                                   صحیح عدد_harts, bool numa_enabled)
{
    g_autofree uint32_t *clint_cells = g_new0(uint32_t, عدد_harts * 4);
    g_autofree حرف *clint_name = تہی;
    ساکن ثابت حرف * ثابت clint_compat[2] = {
        "sifive,clint0", "riscv,clint0"
    };

    برائے (صحیح cpu = 0; cpu < عدد_harts; cpu++) {
        clint_cells[cpu * 4 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 1] = cpu_مقصد_be32(IRQ_M_SOFT);
        clint_cells[cpu * 4 + 2] = cpu_مقصد_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 3] = cpu_مقصد_be32(IRQ_M_TIMER);
    }

    clint_name = g_strdup_printf("/soc/clint@%"HWADDR_PRIx, addr);
    qemu_fdt_جوڑو_subnode(fdt, clint_name);
    qemu_fdt_setprop_string_array(fdt, clint_name, "compatible",
                                  (حرف **)&clint_compat,
                                  ARRAY_سائز(clint_compat));
    qemu_fdt_setprop_sized_cells(fdt, clint_name, "reg",
                                 2, addr, 2, سائز);
    qemu_fdt_setprop(fdt, clint_name, "interrupts-extended",
                     clint_cells, عدد_harts * حجم(uint32_t) * 4);

    اگر (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, clint_name, "numa-node-id", socket_id);
    }
}

خالی riscv_fdt_create_cpu_socket_subnode(خالی *fdt,
                                         uint64_t timebase_frequency)
{
    qemu_fdt_جوڑو_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency",
                          timebase_frequency);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);
    qemu_fdt_جوڑو_subnode(fdt, "/cpus/cpu-map");
}

ساکن خالی
create_fdt_socket_cpu_اندرونی(خالی *fdt, حرف *clust_name, RISCVCPU *cpu_ptr,
                               صحیح cpu, صحیح socket_id, صحیح socket_hartid_بنیاد,
                               uint32_t *phandle, uint32_t *intc_phandles,
                               bool numa_enabled, bool is_32_ثنائی_ہندسہ)
{
    g_autofree حرف *cpu_name = تہی;
    g_autofree حرف *core_name = تہی;
    g_autofree حرف *intc_name = تہی;
    uint32_t cpu_phandle = (*phandle)++;
    bool is_sifive_u = cpu_ptr == تہی;

    cpu_name = g_strdup_printf("/cpus/cpu@%d", socket_hartid_بنیاد + cpu);

    /*
     * The sifive_u board has an exclusive satp and riscv,isa
     * schema that can't be shared with other boards, so part
     * of the CPU FDT creation (i.e. the /cpus/cpu@N subnode)
     * is still being done by the board.
     */
    اگر (!is_sifive_u) {
        int8_t satp_منوال_max = cpu_ptr->cfg.max_satp_منوال;

        qemu_fdt_جوڑو_subnode(fdt, cpu_name);

        اگر (satp_منوال_max != -1) {
            g_autofree حرف *sv_name = تہی;
            sv_name = g_strdup_printf("riscv,%s",
                                      satp_منوال_str(satp_منوال_max, is_32_ثنائی_ہندسہ));
            qemu_fdt_setprop_string(fdt, cpu_name, "mmu-type", sv_name);
        }
        riscv_isa_write_fdt(cpu_ptr, fdt, cpu_name);

        اگر (cpu_ptr->cfg.ext_zicbom) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbom-block-size",
                                  cpu_ptr->cfg.cbom_blocksize);
        }

        اگر (cpu_ptr->cfg.ext_zicboz) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cboz-block-size",
                                  cpu_ptr->cfg.cboz_blocksize);
        }

        اگر (cpu_ptr->cfg.ext_zicbop) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbop-block-size",
                                  cpu_ptr->cfg.cbop_blocksize);
        }
    }

    qemu_fdt_setprop_string(fdt, cpu_name, "compatible", "riscv");
    qemu_fdt_setprop_string(fdt, cpu_name, "status", "okay");
    qemu_fdt_setprop_cell(fdt, cpu_name, "reg",
                          socket_hartid_بنیاد + cpu);
    qemu_fdt_setprop_string(fdt, cpu_name, "device_type", "cpu");
    اگر (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, cpu_name, "numa-node-id", socket_id);
    }
    qemu_fdt_setprop_cell(fdt, cpu_name, "phandle", cpu_phandle);

    intc_phandles[cpu] = (*phandle)++;

    intc_name = g_strdup_printf("%s/interrupt-controller", cpu_name);
    qemu_fdt_جوڑو_subnode(fdt, intc_name);
    qemu_fdt_setprop_cell(fdt, intc_name, "phandle",
                          intc_phandles[cpu]);
    qemu_fdt_setprop_string(fdt, intc_name, "compatible",
                            "riscv,cpu-intc");
    qemu_fdt_setprop(fdt, intc_name, "interrupt-controller", تہی, 0);
    qemu_fdt_setprop_cell(fdt, intc_name, "#interrupt-cells", 1);

    core_name = g_strdup_printf("%s/core%d", clust_name, cpu);
    qemu_fdt_جوڑو_subnode(fdt, core_name);
    qemu_fdt_setprop_cell(fdt, core_name, "cpu", cpu_phandle);
}

خالی riscv_create_fdt_socket_cpus(خالی *fdt, RISCVCPU *socket_harts,
                                  صحیح socket_id, صحیح عدد_harts_socket,
                                  صحیح socket_hartid_بنیاد, uint32_t *phandle,
                                  uint32_t *intc_phandles, bool numa_enabled,
                                  bool is_32_ثنائی_ہندسہ)
{
    g_autofree حرف *clust_name = تہی;

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", socket_id);
    qemu_fdt_جوڑو_subnode(fdt, clust_name);

    برائے (صحیح cpu = عدد_harts_socket - 1; cpu >= 0; cpu--) {
        RISCVCPU *cpu_ptr = &socket_harts[cpu];

        create_fdt_socket_cpu_اندرونی(fdt, clust_name, cpu_ptr, cpu,
                                       socket_id, socket_hartid_بنیاد,
                                       phandle, intc_phandles, numa_enabled,
                                       is_32_ثنائی_ہندسہ);
    }
}

خالی
riscv_create_fdt_socket_cpu_sifive(خالی *fdt, حرف *clust_name,
                                   صحیح cpu_id, صحیح socket_id,
                                   صحیح socket_hartid_بنیاد, uint32_t *phandle,
                                   uint32_t *intc_phandles)
{
    create_fdt_socket_cpu_اندرونی(fdt, clust_name, تہی, cpu_id,
                                   socket_id, socket_hartid_بنیاد,
                                   phandle, intc_phandles, false, false);
}

خالی riscv_create_fdt_plic(خالی *fdt, hwaddr addr, uint64_t سائز,
                           uint32_t plic_phandle, uint32_t int_cells,
                           uint32_t addr_cells, uint32_t *plic_cells,
                           uint32_t cells_سائز, uint32_t ndev_sources,
                           bool numa_enabled, صحیح socket_id)
{
    g_autofree حرف *nodename = تہی;
    ساکن ثابت حرف * ثابت plic_compat[2] = {
        "sifive,plic-1.0.0", "riscv,plic0"
    };

    nodename = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx, addr);

    qemu_fdt_جوڑو_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", int_cells);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", addr_cells);
    qemu_fdt_setprop_string_array(fdt, nodename, "compatible",
        (حرف **)&plic_compat, ARRAY_سائز(plic_compat));
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", تہی, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
                     plic_cells, cells_سائز);
    qemu_fdt_setprop_sized_cells(fdt, nodename, "reg",
                                 2, addr, 2, سائز);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", ndev_sources);
    اگر (numa_enabled) {
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
خالی riscv_pmu_generate_fdt_گرہ(خالی *fdt, uint32_t cmask, حرف *pmu_name)
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
   fdt_event_ctr_map[0] = cpu_مقصد_be32(0x00000001);
   fdt_event_ctr_map[1] = cpu_مقصد_be32(0x00000001);
   fdt_event_ctr_map[2] = cpu_مقصد_be32(cmask | 1 << 0);

   /* SBI_PMU_HW_INSTRUCTIONS: 0x02 : type(0x00) */
   fdt_event_ctr_map[3] = cpu_مقصد_be32(0x00000002);
   fdt_event_ctr_map[4] = cpu_مقصد_be32(0x00000002);
   fdt_event_ctr_map[5] = cpu_مقصد_be32(cmask | 1 << 2);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[6] = cpu_مقصد_be32(0x00010019);
   fdt_event_ctr_map[7] = cpu_مقصد_be32(0x00010019);
   fdt_event_ctr_map[8] = cpu_مقصد_be32(cmask);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 WRITE : 0x01 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[9] = cpu_مقصد_be32(0x0001001B);
   fdt_event_ctr_map[10] = cpu_مقصد_be32(0x0001001B);
   fdt_event_ctr_map[11] = cpu_مقصد_be32(cmask);

   /* SBI_PMU_HW_CACHE_ITLB : 0x04 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[12] = cpu_مقصد_be32(0x00010021);
   fdt_event_ctr_map[13] = cpu_مقصد_be32(0x00010021);
   fdt_event_ctr_map[14] = cpu_مقصد_be32(cmask);

   /* This a OpenSBI specific DT property documented in OpenSBI docs */
   qemu_fdt_setprop(fdt, pmu_name, "riscv,event-to-mhpmcounters",
                    fdt_event_ctr_map, حجم(fdt_event_ctr_map));
}

خالی riscv_create_fdt_flash(خالی *fdt, hwaddr flashbase, hwaddr flashsize)
{
    g_autofree حرف *name = g_strdup_printf("/flash@%" PRIx64, flashbase);

    qemu_fdt_جوڑو_subnode(fdt, name);
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
خالی riscv_create_fdt_syscon(خالی *fdt, uint32_t *اگلا_phandle,
                             hwaddr addr, hwaddr سائز,
                             uint32_t reboot, uint32_t poweroff,
                             bool sifive_test_compat)
{
    uint32_t syscon_phandle;
    حرف *name;

    name = g_strdup_printf("/soc/syscon@%"HWADDR_PRIx, addr);
    qemu_fdt_جوڑو_subnode(fdt, name);

    اگر (sifive_test_compat) {
        ساکن ثابت حرف * ثابت compat[3] = {
            "sifive,test1", "sifive,test0", "syscon"
        };

        qemu_fdt_setprop_string_array(fdt, name, "compatible",
                                      (حرف **)&compat, ARRAY_سائز(compat));
    } ورنہ {
        qemu_fdt_setprop_string(fdt, name, "compatible", "syscon");
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr,
                                 2, سائز);

    syscon_phandle = اگلا_phandle ? (*اگلا_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, name, "phandle", syscon_phandle);

    g_free(name);

    name = g_strdup_printf("/reboot");
    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-reboot");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", reboot);
    g_free(name);

    name = g_strdup_printf("/poweroff");
    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-poweroff");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", poweroff);
    g_free(name);
}

uint32_t riscv_create_fdt_riscv_iommu_sys(خالی *fdt, hwaddr addr, hwaddr سائز,
                                          uint32_t *اگلا_phandle,
                                          uint32_t irq_chip,
                                          uint32_t msi_phandle,
                                          uint32_t iommu_sys_irq)
{
    ثابت حرف comp[] = "riscv,iommu";
    g_autofree حرف *iommu_گرہ = تہی;
    uint32_t iommu_phandle;

    iommu_گرہ = g_strdup_printf("/soc/iommu@%"HWADDR_PRIx, addr);
    qemu_fdt_جوڑو_subnode(fdt, iommu_گرہ);

    qemu_fdt_setprop(fdt, iommu_گرہ, "compatible", comp, حجم(comp));
    qemu_fdt_setprop_cell(fdt, iommu_گرہ, "#iommu-cells", 1);

    iommu_phandle = اگلا_phandle ? (*اگلا_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, iommu_گرہ, "phandle", iommu_phandle);

    qemu_fdt_setprop_sized_cells(fdt, iommu_گرہ, "reg", 2, addr, 2, سائز);
    qemu_fdt_setprop_cell(fdt, iommu_گرہ, "interrupt-parent", irq_chip);

    qemu_fdt_setprop_cells(fdt, iommu_گرہ, "interrupts",
        iommu_sys_irq + RISCV_IOMMU_INTR_CQ, FDT_IRQ_TYPE_کنارہ_کم,
        iommu_sys_irq + RISCV_IOMMU_INTR_FQ, FDT_IRQ_TYPE_کنارہ_کم,
        iommu_sys_irq + RISCV_IOMMU_INTR_PM, FDT_IRQ_TYPE_کنارہ_کم,
        iommu_sys_irq + RISCV_IOMMU_INTR_PQ, FDT_IRQ_TYPE_کنارہ_کم);

    qemu_fdt_setprop_cell(fdt, iommu_گرہ, "msi-parent", msi_phandle);

    واپس iommu_phandle;
}

ساکن خالی create_pcie_irq_map(خالی *fdt, حرف *nodename,
                                uint32_t irqchip_phandle,
                                RISCVAIAType aia_type, uint32_t pcie_irq)
{
    صحیح pin, dev;
    uint32_t irq_map_stride = 0;
    uint32_t بھرا_irq_map[PCI_عدد_PINS * PCI_عدد_PINS *
                          FDT_MAX_INT_MAP_چوڑائی] = {};
    uint32_t *irq_map = بھرا_irq_map;

    /*
     * This code creates a standard swizzle of interrupts such that
     * each device's first interrupt is based on it's PCI_SLOT number.
     * (See pci_swizzle_map_irq_fn())
     *
     * We only need one entry per interrupt in the table (not one per
     * possible slot) seeing the interrupt-map-mask will allow the table
     * to wrap to any number of devices.
     */
    برائے (dev = 0; dev < PCI_عدد_PINS; dev++) {
        صحیح devfn = dev * 0x8;

        برائے (pin = 0; pin < PCI_عدد_PINS; pin++) {
            صحیح irq_nr = pcie_irq + ((pin + PCI_خانہ(devfn)) % PCI_عدد_PINS);
            صحیح i = 0;

            /* Fill PCI address cells */
            irq_map[i] = cpu_مقصد_be32(devfn << 8);
            i += FDT_PCI_ADDR_CELLS;

            /* Fill PCI Interrupt cells */
            irq_map[i] = cpu_مقصد_be32(pin + 1);
            i += FDT_PCI_INT_CELLS;

            /* Fill interrupt controller phandle and cells */
            irq_map[i++] = cpu_مقصد_be32(irqchip_phandle);
            irq_map[i++] = cpu_مقصد_be32(irq_nr);

            اگر (aia_type != AIA_TYPE_NONE) {
                irq_map[i++] = cpu_مقصد_be32(0x4);
            }

            اگر (!irq_map_stride) {
                irq_map_stride = i;
            }
            irq_map += irq_map_stride;
        }
    }

    qemu_fdt_setprop(fdt, nodename, "interrupt-map", بھرا_irq_map,
                     PCI_عدد_PINS * PCI_عدد_PINS *
                     irq_map_stride * حجم(uint32_t));

    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-map-mask",
                           0x1800, 0, 0, 0x7);
}

/*
 * NOTE: this function uses a "/soc/pci@..." FDT subnode that
 * should be created beforehand.
 */
خالی riscv_create_fdt_pcie(خالی *fdt, صحیح aia_type, bool has_iommu_sys,
                           ثابت Mem_Map_اندراج *pcie_ecam,
                           ثابت Mem_Map_اندراج *pcie_pio,
                           ثابت Mem_Map_اندراج *pcie_mmio,
                           ثابت Mem_Map_اندراج *بلند_pcie,
                           uint32_t irq_pcie_phandle,
                           uint32_t msi_pcie_phandle,
                           uint32_t iommu_sys_phandle, uint32_t pcie_irq)
{
    g_autofree حرف *name = تہی;

    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx, pcie_ecam->بنیاد);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", FDT_PCI_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", FDT_PCI_INT_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0x2);
    qemu_fdt_setprop_string(fdt, name, "compatible", "pci-host-ecam-generic");
    qemu_fdt_setprop_string(fdt, name, "device_type", "pci");
    qemu_fdt_setprop_cell(fdt, name, "linux,pci-domain", 0);

    qemu_fdt_setprop_cells(fdt, name, "bus-range", 0,
                           pcie_ecam->سائز / PCIE_MMCFG_سائز_MIN - 1);
    qemu_fdt_setprop(fdt, name, "dma-coherent", تہی, 0);

    اگر (aia_type == AIA_TYPE_APLIC_IMSIC) {
        qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_pcie_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2,
                                 pcie_ecam->بنیاد, 2, pcie_ecam->سائز);

    qemu_fdt_setprop_sized_cells(fdt, name, "ranges",
        1, FDT_PCI_RANGE_IOPORT, 2, 0,
        2, pcie_pio->بنیاد, 2, pcie_pio->سائز,
        1, FDT_PCI_RANGE_MMIO,
        2, pcie_mmio->بنیاد,
        2, pcie_mmio->بنیاد, 2, pcie_mmio->سائز,
        1, FDT_PCI_RANGE_MMIO_64_ثنائی_ہندسہ,
        2, بلند_pcie->بنیاد,
        2, بلند_pcie->بنیاد, 2, بلند_pcie->سائز);

    اگر (has_iommu_sys) {
        qemu_fdt_setprop_cells(fdt, name, "iommu-map",
                               0, iommu_sys_phandle, 0, 0x10000);
    }

    create_pcie_irq_map(fdt, name, irq_pcie_phandle, aia_type, pcie_irq);
}

ساکن خالی create_fdt_one_imsic(خالی *fdt, IMSICFdtProps *props,
                                 hwaddr بنیاد_addr,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 bool m_منوال, uint32_t imsic_guest_بٹ)
{
    RISCVHartArrayState *soc = (RISCVHartArrayState *)props->soc;
    g_autofree حرف *imsic_name = تہی;
    g_autofree uint32_t *imsic_cells = تہی;
    g_autofree uint32_t *imsic_regs = تہی;
    uint32_t imsic_max_hart_فی_socket = 0;
    ساکن ثابت حرف * ثابت imsic_compat[2] = {
        "qemu,imsics", "riscv,imsics"
    };

    imsic_cells = g_new0(uint32_t, props->smp_cpus * 2);
    imsic_regs = g_new0(uint32_t, props->socket_گنتی * 4);

    برائے (صحیح cpu = 0; cpu < props->smp_cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_مقصد_be32(m_منوال ? IRQ_M_EXT : IRQ_S_EXT);
    }

    برائے (صحیح socket = 0; socket < props->socket_گنتی; socket++) {
        hwaddr imsic_addr = بنیاد_addr + socket * props->imsic_گروہ_max_سائز;
        uint32_t imsic_سائز = IMSIC_HART_سائز(imsic_guest_بٹ) *
                              soc[socket].عدد_harts;

        imsic_regs[socket * 4 + 0] = cpu_مقصد_be32(imsic_addr >> 32);
        imsic_regs[socket * 4 + 1] = cpu_مقصد_be32(imsic_addr);
        imsic_regs[socket * 4 + 2] = 0;
        imsic_regs[socket * 4 + 3] = cpu_مقصد_be32(imsic_سائز);
        اگر (imsic_max_hart_فی_socket < soc[socket].عدد_harts) {
            imsic_max_hart_فی_socket = soc[socket].عدد_harts;
        }
    }

    imsic_name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                                 بنیاد_addr);
    qemu_fdt_جوڑو_subnode(fdt, imsic_name);

    qemu_fdt_setprop_string_array(fdt, imsic_name, "compatible",
                                  (حرف **)&imsic_compat,
                                  ARRAY_سائز(imsic_compat));

    qemu_fdt_setprop_cell(fdt, imsic_name, "#interrupt-cells",
                          FDT_IMSIC_INT_CELLS);
    qemu_fdt_setprop(fdt, imsic_name, "interrupt-controller", تہی, 0);
    qemu_fdt_setprop(fdt, imsic_name, "msi-controller", تہی, 0);
    qemu_fdt_setprop(fdt, imsic_name, "interrupts-extended",
                     imsic_cells, props->smp_cpus * حجم(uint32_t) * 2);
    qemu_fdt_setprop(fdt, imsic_name, "reg", imsic_regs,
                     props->socket_گنتی * حجم(uint32_t) * 4);
    qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,num-ids",
                          props->irqchip_عدد_msis);

    اگر (imsic_guest_بٹ) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,guest-index-bits",
                              imsic_guest_بٹ);
    }

    اگر (props->socket_گنتی > 1) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,hart-index-bits",
                              imsic_عدد_بٹ(imsic_max_hart_فی_socket));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-bits",
                              imsic_عدد_بٹ(props->socket_گنتی));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-shift",
                              IMSIC_MMIO_گروہ_MIN_سرکاؤ);
    }
    qemu_fdt_setprop_cell(fdt, imsic_name, "phandle", msi_phandle);
}

خالی riscv_create_fdt_imsic(خالی *fdt, IMSICFdtProps *props,
                            uint32_t *اگلا_phandle, uint32_t *intc_phandles,
                            uint32_t *msi_m_phandle, uint32_t *msi_s_phandle)
{
    اگر (اگلا_phandle) {
        *msi_m_phandle = (*اگلا_phandle)++;
        *msi_s_phandle = (*اگلا_phandle)++;
    } ورنہ {
        *msi_m_phandle = qemu_fdt_alloc_phandle(fdt);
        *msi_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    اگر (props->imsic_m_بنیاد) {
        /* M-level IMSIC node */
        create_fdt_one_imsic(fdt, props, props->imsic_m_بنیاد,
                             intc_phandles, *msi_m_phandle, true, 0);
    }

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, props, props->imsic_s_بنیاد,
                         intc_phandles, *msi_s_phandle, false,
                         imsic_عدد_بٹ(props->aia_guests + 1));

}

/* Caller must free string after use */
ساکن حرف *fdt_get_aplic_nodename(hwaddr aplic_addr)
{
    واپس g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                           aplic_addr);
}

ساکن خالی create_fdt_one_aplic(خالی *fdt, APLICFdtProps *props,
                                 hwaddr aplic_addr, uint32_t aplic_سائز,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_بچہ_phandle,
                                 bool m_منوال)
{
    g_autofree حرف *aplic_name = fdt_get_aplic_nodename(aplic_addr);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, props->عدد_harts * 2);
    ساکن ثابت حرف * ثابت aplic_compat[2] = {
        "qemu,aplic", "riscv,aplic"
    };

    برائے (صحیح cpu = 0; cpu < props->عدد_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_مقصد_be32(m_منوال ? IRQ_M_EXT : IRQ_S_EXT);
    }

    qemu_fdt_جوڑو_subnode(fdt, aplic_name);
    qemu_fdt_setprop_string_array(fdt, aplic_name, "compatible",
                                  (حرف **)&aplic_compat,
                                  ARRAY_سائز(aplic_compat));
    qemu_fdt_setprop_cell(fdt, aplic_name, "#address-cells",
                          FDT_APLIC_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, aplic_name,
                          "#interrupt-cells", FDT_APLIC_INT_CELLS);
    qemu_fdt_setprop(fdt, aplic_name, "interrupt-controller", تہی, 0);

    اگر (props->aia_type == AIA_TYPE_APLIC) {
        qemu_fdt_setprop(fdt, aplic_name, "interrupts-extended",
                         aplic_cells, props->عدد_harts * حجم(uint32_t) * 2);
    } ورنہ {
        qemu_fdt_setprop_cell(fdt, aplic_name, "msi-parent", msi_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, aplic_name, "reg",
                                 2, aplic_addr, 2, aplic_سائز);
    qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,num-sources",
                          props->irqchip_عدد_sources);

    اگر (aplic_بچہ_phandle) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,children",
                              aplic_بچہ_phandle);
        qemu_fdt_setprop_cells(fdt, aplic_name, "riscv,delegation",
                               aplic_بچہ_phandle, 0x1,
                               props->irqchip_عدد_sources);
    }

    اگر (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "numa-node-id", props->socket);
    }

    qemu_fdt_setprop_cell(fdt, aplic_name, "phandle", aplic_phandle);
}

خالی riscv_create_fdt_socket_aplic(خالی *fdt, APLICFdtProps *props,
                                   uint32_t msi_m_phandle,
                                   uint32_t msi_s_phandle,
                                   uint32_t *اگلا_phandle,
                                   uint32_t *intc_phandles,
                                   uint32_t *aplic_phandles)
{
    uint32_t aplic_m_phandle, aplic_s_phandle;
    hwaddr aplic_addr;

    اگر (اگلا_phandle) {
        aplic_m_phandle = (*اگلا_phandle)++;
        aplic_s_phandle = (*اگلا_phandle)++;
    } ورنہ {
        aplic_m_phandle = qemu_fdt_alloc_phandle(fdt);
        aplic_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    اگر (props->aplic_m) {
        /* M-level APLIC node */
        aplic_addr = props->aplic_m->بنیاد + (props->aplic_m->سائز * props->socket);
        create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_m->سائز,
                             msi_m_phandle, intc_phandles,
                             aplic_m_phandle, aplic_s_phandle,
                             true);
    }

    /* S-level APLIC node */
    aplic_addr = props->aplic_s->بنیاد + (props->aplic_s->سائز * props->socket);
    create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_s->سائز,
                         msi_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         false);

    اگر (!props->socket && props->platform_bus_irq) {
        g_autofree حرف *aplic_name = fdt_get_aplic_nodename(aplic_addr);
        platform_bus_جوڑو_all_fdt_nodes(fdt, aplic_name,
                                       props->platform_bus->بنیاد,
                                       props->platform_bus->سائز,
                                       props->platform_bus_irq);
    }

    aplic_phandles[props->socket] = aplic_s_phandle;
}

خالی riscv_create_fdt_socket_aclint(خالی *fdt, ACLINTFdtProps *props,
                                    uint32_t *intc_phandles)
{
    uint32_t aclint_cells_سائز = props->عدد_harts * حجم(uint32_t) * 2;
    g_autofree uint32_t *aclint_mswi_cells = تہی;
    g_autofree uint32_t *aclint_sswi_cells = تہی;
    g_autofree uint32_t *aclint_mtimer_cells = تہی;
    hwaddr addr, سائز;
    حرف *name;
    صحیح cpu;

    aclint_mswi_cells = g_new0(uint32_t, props->عدد_harts * 2);
    aclint_mtimer_cells = g_new0(uint32_t, props->عدد_harts * 2);
    aclint_sswi_cells = g_new0(uint32_t, props->عدد_harts * 2);

    برائے (cpu = 0; cpu < props->عدد_harts; cpu++) {
        aclint_mswi_cells[cpu * 2 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
        aclint_mswi_cells[cpu * 2 + 1] = cpu_مقصد_be32(IRQ_M_SOFT);
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_مقصد_be32(IRQ_M_TIMER);
        aclint_sswi_cells[cpu * 2 + 0] = cpu_مقصد_be32(intc_phandles[cpu]);
        aclint_sswi_cells[cpu * 2 + 1] = cpu_مقصد_be32(IRQ_S_SOFT);
    }

    اگر (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->بنیاد + (props->clint->سائز * props->socket);
        name = g_strdup_printf("/soc/mswi@%"HWADDR_PRIx, addr);

        qemu_fdt_جوڑو_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, RISCV_ACLINT_SWI_سائز);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_mswi_cells, aclint_cells_سائز);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", تہی, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        اگر (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }

    اگر (props->aia_type == AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->بنیاد +
               (RISCV_ACLINT_DEFAULT_MTIMER_سائز * props->socket);
        سائز = RISCV_ACLINT_DEFAULT_MTIMER_سائز;
    } ورنہ {
        addr = props->clint->بنیاد + RISCV_ACLINT_SWI_سائز +
               (props->clint->سائز * props->socket);
        سائز = props->clint->سائز - RISCV_ACLINT_SWI_سائز;
    }

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIx,
                           addr + RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible",
                            "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
        2, addr + RISCV_ACLINT_DEFAULT_MTIME,
        2, سائز - RISCV_ACLINT_DEFAULT_MTIME,
        2, addr + RISCV_ACLINT_DEFAULT_MTIMECMP,
        2, RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_سائز);

    اگر (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
    }

    g_free(name);

    اگر (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->aclint_sswi->بنیاد
               + (props->aclint_sswi->سائز * props->socket);

        name = g_strdup_printf("/soc/sswi@%"HWADDR_PRIx, addr);
        qemu_fdt_جوڑو_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible",
            "riscv,aclint-sswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, props->aclint_sswi->سائز);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_sswi_cells, aclint_cells_سائز);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", تہی, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        اگر (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }
}

حرف *riscv_fdt_get_uart_nodename(hwaddr addr)
{
    واپس g_strdup_printf("/soc/serial@%"HWADDR_PRIx, addr);
}

خالی riscv_create_fdt_uart(خالی *fdt, ثابت Mem_Map_اندراج *uart_mem,
                           صحیح uart_irq, صحیح aia_type,
                           uint32_t irq_phandle)
{
    g_autofree حرف *name = riscv_fdt_get_uart_nodename(uart_mem->بنیاد);

    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, uart_mem->بنیاد,
                                 2, uart_mem->سائز);

    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", UART_STD_CLOCK_FREQ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);

    اگر (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", uart_irq);
    } ورنہ {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", uart_irq, 0x4);
    }
}

خالی riscv_create_fdt_rtc(خالی *fdt, ثابت Mem_Map_اندراج *rtc_mem,
                          صحیح rtc_irq, صحیح aia_type,
                          uint32_t irq_phandle)
{
    g_autofree حرف *name = تہی;

    name = g_strdup_printf("/soc/rtc@%"HWADDR_PRIx,
                           rtc_mem->بنیاد);
    qemu_fdt_جوڑو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "google,goldfish-rtc");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, rtc_mem->بنیاد,
                                 2, rtc_mem->سائز);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);
    اگر (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", rtc_irq);
    } ورنہ {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", rtc_irq, 0x4);
    }
}
