/*
 * RISC-V board helpers for FDT generation.
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن "qemu/osdep.h"

#ضمن "qemu/error-report.h"
#ضمن "system/device_tree.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/sysbus-fdt.h"
#ضمن "hw/riscv/fdt-common.h"
#ضمن "target/riscv/cpu_bits.h"
#ضمن "hw/riscv/riscv-iommu-bits.h"
#ضمن "hw/riscv/iommu.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/riscv_imsic.h"
#ضمن "hw/pci/pci.h"
#ضمن "hw/pci/pcie_host.h"
#ضمن "hw/riscv/aia.h"

#وصف_ڪريو UART_STD_CLOCK_FREQ 3686400 /* 3.6864 MHz */

فراغ *riscv_create_board_device_وڻ(ثابت حرف *model, ثابت حرف *compatible,
                                     صحيح *fdt_سائيز)
{
    فراغ *fdt = create_device_وڻ(fdt_سائيز);

    إذا (!fdt) {
        error_report("create_device_tree() failed");
        ٻاهر_نڪرو(1);
    }

    qemu_fdt_setprop_string(fdt, "/", "model", model);
    qemu_fdt_setprop_string(fdt, "/", "compatible", compatible);
    qemu_fdt_setprop_cell(fdt, "/", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/", "#address-cells", 0x2);

    qemu_fdt_جوڙيو_subnode(fdt, "/soc");
    qemu_fdt_setprop(fdt, "/soc", "ranges", عدم, 0);
    qemu_fdt_setprop_string(fdt, "/soc", "compatible", "simple-bus");
    qemu_fdt_setprop_cell(fdt, "/soc", "#size-cells", 0x2);
    qemu_fdt_setprop_cell(fdt, "/soc", "#address-cells", 0x2);

    أرجع fdt;
}

فراغ riscv_create_fdt_socket_يادگيري(فراغ *fdt, hwaddr addr, uint64_t سائيز,
                                    صحيح socket_id, bool numa_enabled)
{
    g_autofree حرف *mem_name = g_strdup_printf("/memory@%"HWADDR_PRIx, addr);

    qemu_fdt_جوڙيو_subnode(fdt, mem_name);
    qemu_fdt_setprop_sized_cells(fdt, mem_name, "reg", 2, addr, 2, سائيز);
    qemu_fdt_setprop_string(fdt, mem_name, "device_type", "memory");

    إذا (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, mem_name, "numa-node-id", socket_id);
    }
}

فراغ riscv_create_fdt_socket_clint(فراغ *fdt, hwaddr addr, uint64_t سائيز,
                                   صحيح socket_id, uint32_t *intc_phandles,
                                   صحيح انگ_harts, bool numa_enabled)
{
    g_autofree uint32_t *clint_cells = g_new0(uint32_t, انگ_harts * 4);
    g_autofree حرف *clint_name = عدم;
    ساكن ثابت حرف * ثابت clint_compat[2] = {
        "sifive,clint0", "riscv,clint0"
    };

    لكل (صحيح cpu = 0; cpu < انگ_harts; cpu++) {
        clint_cells[cpu * 4 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 1] = cpu_منزل_be32(IRQ_M_SOFT);
        clint_cells[cpu * 4 + 2] = cpu_منزل_be32(intc_phandles[cpu]);
        clint_cells[cpu * 4 + 3] = cpu_منزل_be32(IRQ_M_TIMER);
    }

    clint_name = g_strdup_printf("/soc/clint@%"HWADDR_PRIx, addr);
    qemu_fdt_جوڙيو_subnode(fdt, clint_name);
    qemu_fdt_setprop_string_array(fdt, clint_name, "compatible",
                                  (حرف **)&clint_compat,
                                  ARRAY_سائيز(clint_compat));
    qemu_fdt_setprop_sized_cells(fdt, clint_name, "reg",
                                 2, addr, 2, سائيز);
    qemu_fdt_setprop(fdt, clint_name, "interrupts-extended",
                     clint_cells, انگ_harts * حجم(uint32_t) * 4);

    إذا (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, clint_name, "numa-node-id", socket_id);
    }
}

فراغ riscv_fdt_create_cpu_socket_subnode(فراغ *fdt,
                                         uint64_t timebase_frequency)
{
    qemu_fdt_جوڙيو_subnode(fdt, "/cpus");
    qemu_fdt_setprop_cell(fdt, "/cpus", "timebase-frequency",
                          timebase_frequency);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#size-cells", 0x0);
    qemu_fdt_setprop_cell(fdt, "/cpus", "#address-cells", 0x1);
    qemu_fdt_جوڙيو_subnode(fdt, "/cpus/cpu-map");
}

ساكن فراغ
create_fdt_socket_cpu_اندروني(فراغ *fdt, حرف *clust_name, RISCVCPU *cpu_ptr,
                               صحيح cpu, صحيح socket_id, صحيح socket_hartid_بنياد,
                               uint32_t *phandle, uint32_t *intc_phandles,
                               bool numa_enabled, bool is_32_ٻٽو_انگ)
{
    g_autofree حرف *cpu_name = عدم;
    g_autofree حرف *core_name = عدم;
    g_autofree حرف *intc_name = عدم;
    uint32_t cpu_phandle = (*phandle)++;
    bool is_sifive_u = cpu_ptr == عدم;

    cpu_name = g_strdup_printf("/cpus/cpu@%d", socket_hartid_بنياد + cpu);

    /*
     * The sifive_u board has an exclusive satp and riscv,isa
     * schema that can't be shared with other boards, so part
     * of the CPU FDT creation (i.e. the /cpus/cpu@N subnode)
     * is still being done by the board.
     */
    إذا (!is_sifive_u) {
        int8_t satp_گهڻائي_max = cpu_ptr->cfg.max_satp_گهڻائي;

        qemu_fdt_جوڙيو_subnode(fdt, cpu_name);

        إذا (satp_گهڻائي_max != -1) {
            g_autofree حرف *sv_name = عدم;
            sv_name = g_strdup_printf("riscv,%s",
                                      satp_گهڻائي_str(satp_گهڻائي_max, is_32_ٻٽو_انگ));
            qemu_fdt_setprop_string(fdt, cpu_name, "mmu-type", sv_name);
        }
        riscv_isa_write_fdt(cpu_ptr, fdt, cpu_name);

        إذا (cpu_ptr->cfg.ext_zicbom) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbom-block-size",
                                  cpu_ptr->cfg.cbom_blocksize);
        }

        إذا (cpu_ptr->cfg.ext_zicboz) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cboz-block-size",
                                  cpu_ptr->cfg.cboz_blocksize);
        }

        إذا (cpu_ptr->cfg.ext_zicbop) {
            qemu_fdt_setprop_cell(fdt, cpu_name, "riscv,cbop-block-size",
                                  cpu_ptr->cfg.cbop_blocksize);
        }
    }

    qemu_fdt_setprop_string(fdt, cpu_name, "compatible", "riscv");
    qemu_fdt_setprop_string(fdt, cpu_name, "status", "okay");
    qemu_fdt_setprop_cell(fdt, cpu_name, "reg",
                          socket_hartid_بنياد + cpu);
    qemu_fdt_setprop_string(fdt, cpu_name, "device_type", "cpu");
    إذا (numa_enabled) {
        qemu_fdt_setprop_cell(fdt, cpu_name, "numa-node-id", socket_id);
    }
    qemu_fdt_setprop_cell(fdt, cpu_name, "phandle", cpu_phandle);

    intc_phandles[cpu] = (*phandle)++;

    intc_name = g_strdup_printf("%s/interrupt-controller", cpu_name);
    qemu_fdt_جوڙيو_subnode(fdt, intc_name);
    qemu_fdt_setprop_cell(fdt, intc_name, "phandle",
                          intc_phandles[cpu]);
    qemu_fdt_setprop_string(fdt, intc_name, "compatible",
                            "riscv,cpu-intc");
    qemu_fdt_setprop(fdt, intc_name, "interrupt-controller", عدم, 0);
    qemu_fdt_setprop_cell(fdt, intc_name, "#interrupt-cells", 1);

    core_name = g_strdup_printf("%s/core%d", clust_name, cpu);
    qemu_fdt_جوڙيو_subnode(fdt, core_name);
    qemu_fdt_setprop_cell(fdt, core_name, "cpu", cpu_phandle);
}

فراغ riscv_create_fdt_socket_cpus(فراغ *fdt, RISCVCPU *socket_harts,
                                  صحيح socket_id, صحيح انگ_harts_socket,
                                  صحيح socket_hartid_بنياد, uint32_t *phandle,
                                  uint32_t *intc_phandles, bool numa_enabled,
                                  bool is_32_ٻٽو_انگ)
{
    g_autofree حرف *clust_name = عدم;

    clust_name = g_strdup_printf("/cpus/cpu-map/cluster%d", socket_id);
    qemu_fdt_جوڙيو_subnode(fdt, clust_name);

    لكل (صحيح cpu = انگ_harts_socket - 1; cpu >= 0; cpu--) {
        RISCVCPU *cpu_ptr = &socket_harts[cpu];

        create_fdt_socket_cpu_اندروني(fdt, clust_name, cpu_ptr, cpu,
                                       socket_id, socket_hartid_بنياد,
                                       phandle, intc_phandles, numa_enabled,
                                       is_32_ٻٽو_انگ);
    }
}

فراغ
riscv_create_fdt_socket_cpu_sifive(فراغ *fdt, حرف *clust_name,
                                   صحيح cpu_id, صحيح socket_id,
                                   صحيح socket_hartid_بنياد, uint32_t *phandle,
                                   uint32_t *intc_phandles)
{
    create_fdt_socket_cpu_اندروني(fdt, clust_name, عدم, cpu_id,
                                   socket_id, socket_hartid_بنياد,
                                   phandle, intc_phandles, false, false);
}

فراغ riscv_create_fdt_plic(فراغ *fdt, hwaddr addr, uint64_t سائيز,
                           uint32_t plic_phandle, uint32_t int_cells,
                           uint32_t addr_cells, uint32_t *plic_cells,
                           uint32_t cells_سائيز, uint32_t ndev_sources,
                           bool numa_enabled, صحيح socket_id)
{
    g_autofree حرف *nodename = عدم;
    ساكن ثابت حرف * ثابت plic_compat[2] = {
        "sifive,plic-1.0.0", "riscv,plic0"
    };

    nodename = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx, addr);

    qemu_fdt_جوڙيو_subnode(fdt, nodename);
    qemu_fdt_setprop_cell(fdt, nodename, "#interrupt-cells", int_cells);
    qemu_fdt_setprop_cell(fdt, nodename, "#address-cells", addr_cells);
    qemu_fdt_setprop_string_array(fdt, nodename, "compatible",
        (حرف **)&plic_compat, ARRAY_سائيز(plic_compat));
    qemu_fdt_setprop(fdt, nodename, "interrupt-controller", عدم, 0);
    qemu_fdt_setprop(fdt, nodename, "interrupts-extended",
                     plic_cells, cells_سائيز);
    qemu_fdt_setprop_sized_cells(fdt, nodename, "reg",
                                 2, addr, 2, سائيز);
    qemu_fdt_setprop_cell(fdt, nodename, "riscv,ndev", ndev_sources);
    إذا (numa_enabled) {
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
فراغ riscv_pmu_generate_fdt_ڳنڍ(فراغ *fdt, uint32_t cmask, حرف *pmu_name)
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
   fdt_event_ctr_map[0] = cpu_منزل_be32(0x00000001);
   fdt_event_ctr_map[1] = cpu_منزل_be32(0x00000001);
   fdt_event_ctr_map[2] = cpu_منزل_be32(cmask | 1 << 0);

   /* SBI_PMU_HW_INSTRUCTIONS: 0x02 : type(0x00) */
   fdt_event_ctr_map[3] = cpu_منزل_be32(0x00000002);
   fdt_event_ctr_map[4] = cpu_منزل_be32(0x00000002);
   fdt_event_ctr_map[5] = cpu_منزل_be32(cmask | 1 << 2);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[6] = cpu_منزل_be32(0x00010019);
   fdt_event_ctr_map[7] = cpu_منزل_be32(0x00010019);
   fdt_event_ctr_map[8] = cpu_منزل_be32(cmask);

   /* SBI_PMU_HW_CACHE_DTLB : 0x03 WRITE : 0x01 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[9] = cpu_منزل_be32(0x0001001B);
   fdt_event_ctr_map[10] = cpu_منزل_be32(0x0001001B);
   fdt_event_ctr_map[11] = cpu_منزل_be32(cmask);

   /* SBI_PMU_HW_CACHE_ITLB : 0x04 READ : 0x00 MISS : 0x00 type(0x01) */
   fdt_event_ctr_map[12] = cpu_منزل_be32(0x00010021);
   fdt_event_ctr_map[13] = cpu_منزل_be32(0x00010021);
   fdt_event_ctr_map[14] = cpu_منزل_be32(cmask);

   /* This a OpenSBI specific DT property documented in OpenSBI docs */
   qemu_fdt_setprop(fdt, pmu_name, "riscv,event-to-mhpmcounters",
                    fdt_event_ctr_map, حجم(fdt_event_ctr_map));
}

فراغ riscv_create_fdt_flash(فراغ *fdt, hwaddr flashbase, hwaddr flashsize)
{
    g_autofree حرف *name = g_strdup_printf("/flash@%" PRIx64, flashbase);

    qemu_fdt_جوڙيو_subnode(fdt, name);
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
فراغ riscv_create_fdt_syscon(فراغ *fdt, uint32_t *ايندڙ_phandle,
                             hwaddr addr, hwaddr سائيز,
                             uint32_t reboot, uint32_t poweroff,
                             bool sifive_test_compat)
{
    uint32_t syscon_phandle;
    حرف *name;

    name = g_strdup_printf("/soc/syscon@%"HWADDR_PRIx, addr);
    qemu_fdt_جوڙيو_subnode(fdt, name);

    إذا (sifive_test_compat) {
        ساكن ثابت حرف * ثابت compat[3] = {
            "sifive,test1", "sifive,test0", "syscon"
        };

        qemu_fdt_setprop_string_array(fdt, name, "compatible",
                                      (حرف **)&compat, ARRAY_سائيز(compat));
    } وإلا {
        qemu_fdt_setprop_string(fdt, name, "compatible", "syscon");
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, addr,
                                 2, سائيز);

    syscon_phandle = ايندڙ_phandle ? (*ايندڙ_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, name, "phandle", syscon_phandle);

    g_free(name);

    name = g_strdup_printf("/reboot");
    qemu_fdt_جوڙيو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-reboot");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", reboot);
    g_free(name);

    name = g_strdup_printf("/poweroff");
    qemu_fdt_جوڙيو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "syscon-poweroff");
    qemu_fdt_setprop_cell(fdt, name, "regmap", syscon_phandle);
    qemu_fdt_setprop_cell(fdt, name, "offset", 0x0);
    qemu_fdt_setprop_cell(fdt, name, "value", poweroff);
    g_free(name);
}

uint32_t riscv_create_fdt_riscv_iommu_sys(فراغ *fdt, hwaddr addr, hwaddr سائيز,
                                          uint32_t *ايندڙ_phandle,
                                          uint32_t irq_chip,
                                          uint32_t msi_phandle,
                                          uint32_t iommu_sys_irq)
{
    ثابت حرف comp[] = "riscv,iommu";
    g_autofree حرف *iommu_ڳنڍ = عدم;
    uint32_t iommu_phandle;

    iommu_ڳنڍ = g_strdup_printf("/soc/iommu@%"HWADDR_PRIx, addr);
    qemu_fdt_جوڙيو_subnode(fdt, iommu_ڳنڍ);

    qemu_fdt_setprop(fdt, iommu_ڳنڍ, "compatible", comp, حجم(comp));
    qemu_fdt_setprop_cell(fdt, iommu_ڳنڍ, "#iommu-cells", 1);

    iommu_phandle = ايندڙ_phandle ? (*ايندڙ_phandle)++ : qemu_fdt_alloc_phandle(fdt);
    qemu_fdt_setprop_cell(fdt, iommu_ڳنڍ, "phandle", iommu_phandle);

    qemu_fdt_setprop_sized_cells(fdt, iommu_ڳنڍ, "reg", 2, addr, 2, سائيز);
    qemu_fdt_setprop_cell(fdt, iommu_ڳنڍ, "interrupt-parent", irq_chip);

    qemu_fdt_setprop_cells(fdt, iommu_ڳنڍ, "interrupts",
        iommu_sys_irq + RISCV_IOMMU_INTR_CQ, FDT_IRQ_TYPE_ڪنارو_هيٺيون,
        iommu_sys_irq + RISCV_IOMMU_INTR_FQ, FDT_IRQ_TYPE_ڪنارو_هيٺيون,
        iommu_sys_irq + RISCV_IOMMU_INTR_PM, FDT_IRQ_TYPE_ڪنارو_هيٺيون,
        iommu_sys_irq + RISCV_IOMMU_INTR_PQ, FDT_IRQ_TYPE_ڪنارو_هيٺيون);

    qemu_fdt_setprop_cell(fdt, iommu_ڳنڍ, "msi-parent", msi_phandle);

    أرجع iommu_phandle;
}

ساكن فراغ create_pcie_irq_map(فراغ *fdt, حرف *nodename,
                                uint32_t irqchip_phandle,
                                RISCVAIAType aia_type, uint32_t pcie_irq)
{
    صحيح pin, dev;
    uint32_t irq_map_stride = 0;
    uint32_t ڀريل_irq_map[PCI_انگ_PINS * PCI_انگ_PINS *
                          FDT_MAX_INT_MAP_ويڪر] = {};
    uint32_t *irq_map = ڀريل_irq_map;

    /*
     * This code creates a standard swizzle of interrupts such that
     * each device's first interrupt is based on it's PCI_SLOT number.
     * (See pci_swizzle_map_irq_fn())
     *
     * We only need one entry per interrupt in the table (not one per
     * possible slot) seeing the interrupt-map-mask will allow the table
     * to wrap to any number of devices.
     */
    لكل (dev = 0; dev < PCI_انگ_PINS; dev++) {
        صحيح devfn = dev * 0x8;

        لكل (pin = 0; pin < PCI_انگ_PINS; pin++) {
            صحيح irq_nr = pcie_irq + ((pin + PCI_جڳهه(devfn)) % PCI_انگ_PINS);
            صحيح i = 0;

            /* Fill PCI address cells */
            irq_map[i] = cpu_منزل_be32(devfn << 8);
            i += FDT_PCI_ADDR_CELLS;

            /* Fill PCI Interrupt cells */
            irq_map[i] = cpu_منزل_be32(pin + 1);
            i += FDT_PCI_INT_CELLS;

            /* Fill interrupt controller phandle and cells */
            irq_map[i++] = cpu_منزل_be32(irqchip_phandle);
            irq_map[i++] = cpu_منزل_be32(irq_nr);

            إذا (aia_type != AIA_TYPE_NONE) {
                irq_map[i++] = cpu_منزل_be32(0x4);
            }

            إذا (!irq_map_stride) {
                irq_map_stride = i;
            }
            irq_map += irq_map_stride;
        }
    }

    qemu_fdt_setprop(fdt, nodename, "interrupt-map", ڀريل_irq_map,
                     PCI_انگ_PINS * PCI_انگ_PINS *
                     irq_map_stride * حجم(uint32_t));

    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-map-mask",
                           0x1800, 0, 0, 0x7);
}

/*
 * NOTE: this function uses a "/soc/pci@..." FDT subnode that
 * should be created beforehand.
 */
فراغ riscv_create_fdt_pcie(فراغ *fdt, صحيح aia_type, bool has_iommu_sys,
                           ثابت Mem_Map_داخلا *pcie_ecam,
                           ثابت Mem_Map_داخلا *pcie_pio,
                           ثابت Mem_Map_داخلا *pcie_mmio,
                           ثابت Mem_Map_داخلا *مٿاهون_pcie,
                           uint32_t irq_pcie_phandle,
                           uint32_t msi_pcie_phandle,
                           uint32_t iommu_sys_phandle, uint32_t pcie_irq)
{
    g_autofree حرف *name = عدم;

    name = g_strdup_printf("/soc/pci@%"HWADDR_PRIx, pcie_ecam->بنياد);
    qemu_fdt_setprop_cell(fdt, name, "#address-cells", FDT_PCI_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", FDT_PCI_INT_CELLS);
    qemu_fdt_setprop_cell(fdt, name, "#size-cells", 0x2);
    qemu_fdt_setprop_string(fdt, name, "compatible", "pci-host-ecam-generic");
    qemu_fdt_setprop_string(fdt, name, "device_type", "pci");
    qemu_fdt_setprop_cell(fdt, name, "linux,pci-domain", 0);

    qemu_fdt_setprop_cells(fdt, name, "bus-range", 0,
                           pcie_ecam->سائيز / PCIE_MMCFG_سائيز_MIN - 1);
    qemu_fdt_setprop(fdt, name, "dma-coherent", عدم, 0);

    إذا (aia_type == AIA_TYPE_APLIC_IMSIC) {
        qemu_fdt_setprop_cell(fdt, name, "msi-parent", msi_pcie_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, name, "reg", 2,
                                 pcie_ecam->بنياد, 2, pcie_ecam->سائيز);

    qemu_fdt_setprop_sized_cells(fdt, name, "ranges",
        1, FDT_PCI_RANGE_IOPORT, 2, 0,
        2, pcie_pio->بنياد, 2, pcie_pio->سائيز,
        1, FDT_PCI_RANGE_MMIO,
        2, pcie_mmio->بنياد,
        2, pcie_mmio->بنياد, 2, pcie_mmio->سائيز,
        1, FDT_PCI_RANGE_MMIO_64_ٻٽو_انگ,
        2, مٿاهون_pcie->بنياد,
        2, مٿاهون_pcie->بنياد, 2, مٿاهون_pcie->سائيز);

    إذا (has_iommu_sys) {
        qemu_fdt_setprop_cells(fdt, name, "iommu-map",
                               0, iommu_sys_phandle, 0, 0x10000);
    }

    create_pcie_irq_map(fdt, name, irq_pcie_phandle, aia_type, pcie_irq);
}

ساكن فراغ create_fdt_one_imsic(فراغ *fdt, IMSICFdtProps *props,
                                 hwaddr بنياد_addr,
                                 uint32_t *intc_phandles, uint32_t msi_phandle,
                                 bool m_گهڻائي, uint32_t imsic_guest_بٽ)
{
    RISCVHartArrayState *soc = (RISCVHartArrayState *)props->soc;
    g_autofree حرف *imsic_name = عدم;
    g_autofree uint32_t *imsic_cells = عدم;
    g_autofree uint32_t *imsic_regs = عدم;
    uint32_t imsic_max_hart_في_socket = 0;
    ساكن ثابت حرف * ثابت imsic_compat[2] = {
        "qemu,imsics", "riscv,imsics"
    };

    imsic_cells = g_new0(uint32_t, props->smp_cpus * 2);
    imsic_regs = g_new0(uint32_t, props->socket_ڳڻپ * 4);

    لكل (صحيح cpu = 0; cpu < props->smp_cpus; cpu++) {
        imsic_cells[cpu * 2 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        imsic_cells[cpu * 2 + 1] = cpu_منزل_be32(m_گهڻائي ? IRQ_M_EXT : IRQ_S_EXT);
    }

    لكل (صحيح socket = 0; socket < props->socket_ڳڻپ; socket++) {
        hwaddr imsic_addr = بنياد_addr + socket * props->imsic_گروهه_max_سائيز;
        uint32_t imsic_سائيز = IMSIC_HART_سائيز(imsic_guest_بٽ) *
                              soc[socket].انگ_harts;

        imsic_regs[socket * 4 + 0] = cpu_منزل_be32(imsic_addr >> 32);
        imsic_regs[socket * 4 + 1] = cpu_منزل_be32(imsic_addr);
        imsic_regs[socket * 4 + 2] = 0;
        imsic_regs[socket * 4 + 3] = cpu_منزل_be32(imsic_سائيز);
        إذا (imsic_max_hart_في_socket < soc[socket].انگ_harts) {
            imsic_max_hart_في_socket = soc[socket].انگ_harts;
        }
    }

    imsic_name = g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                                 بنياد_addr);
    qemu_fdt_جوڙيو_subnode(fdt, imsic_name);

    qemu_fdt_setprop_string_array(fdt, imsic_name, "compatible",
                                  (حرف **)&imsic_compat,
                                  ARRAY_سائيز(imsic_compat));

    qemu_fdt_setprop_cell(fdt, imsic_name, "#interrupt-cells",
                          FDT_IMSIC_INT_CELLS);
    qemu_fdt_setprop(fdt, imsic_name, "interrupt-controller", عدم, 0);
    qemu_fdt_setprop(fdt, imsic_name, "msi-controller", عدم, 0);
    qemu_fdt_setprop(fdt, imsic_name, "interrupts-extended",
                     imsic_cells, props->smp_cpus * حجم(uint32_t) * 2);
    qemu_fdt_setprop(fdt, imsic_name, "reg", imsic_regs,
                     props->socket_ڳڻپ * حجم(uint32_t) * 4);
    qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,num-ids",
                          props->irqchip_انگ_msis);

    إذا (imsic_guest_بٽ) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,guest-index-bits",
                              imsic_guest_بٽ);
    }

    إذا (props->socket_ڳڻپ > 1) {
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,hart-index-bits",
                              imsic_انگ_بٽ(imsic_max_hart_في_socket));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-bits",
                              imsic_انگ_بٽ(props->socket_ڳڻپ));
        qemu_fdt_setprop_cell(fdt, imsic_name, "riscv,group-index-shift",
                              IMSIC_MMIO_گروهه_MIN_سرڪايو);
    }
    qemu_fdt_setprop_cell(fdt, imsic_name, "phandle", msi_phandle);
}

فراغ riscv_create_fdt_imsic(فراغ *fdt, IMSICFdtProps *props,
                            uint32_t *ايندڙ_phandle, uint32_t *intc_phandles,
                            uint32_t *msi_m_phandle, uint32_t *msi_s_phandle)
{
    إذا (ايندڙ_phandle) {
        *msi_m_phandle = (*ايندڙ_phandle)++;
        *msi_s_phandle = (*ايندڙ_phandle)++;
    } وإلا {
        *msi_m_phandle = qemu_fdt_alloc_phandle(fdt);
        *msi_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    إذا (props->imsic_m_بنياد) {
        /* M-level IMSIC node */
        create_fdt_one_imsic(fdt, props, props->imsic_m_بنياد,
                             intc_phandles, *msi_m_phandle, true, 0);
    }

    /* S-level IMSIC node */
    create_fdt_one_imsic(fdt, props, props->imsic_s_بنياد,
                         intc_phandles, *msi_s_phandle, false,
                         imsic_انگ_بٽ(props->aia_guests + 1));

}

/* Caller must free string after use */
ساكن حرف *fdt_get_aplic_nodename(hwaddr aplic_addr)
{
    أرجع g_strdup_printf("/soc/interrupt-controller@%"HWADDR_PRIx,
                           aplic_addr);
}

ساكن فراغ create_fdt_one_aplic(فراغ *fdt, APLICFdtProps *props,
                                 hwaddr aplic_addr, uint32_t aplic_سائيز,
                                 uint32_t msi_phandle,
                                 uint32_t *intc_phandles,
                                 uint32_t aplic_phandle,
                                 uint32_t aplic_ٻار_phandle,
                                 bool m_گهڻائي)
{
    g_autofree حرف *aplic_name = fdt_get_aplic_nodename(aplic_addr);
    g_autofree uint32_t *aplic_cells = g_new0(uint32_t, props->انگ_harts * 2);
    ساكن ثابت حرف * ثابت aplic_compat[2] = {
        "qemu,aplic", "riscv,aplic"
    };

    لكل (صحيح cpu = 0; cpu < props->انگ_harts; cpu++) {
        aplic_cells[cpu * 2 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        aplic_cells[cpu * 2 + 1] = cpu_منزل_be32(m_گهڻائي ? IRQ_M_EXT : IRQ_S_EXT);
    }

    qemu_fdt_جوڙيو_subnode(fdt, aplic_name);
    qemu_fdt_setprop_string_array(fdt, aplic_name, "compatible",
                                  (حرف **)&aplic_compat,
                                  ARRAY_سائيز(aplic_compat));
    qemu_fdt_setprop_cell(fdt, aplic_name, "#address-cells",
                          FDT_APLIC_ADDR_CELLS);
    qemu_fdt_setprop_cell(fdt, aplic_name,
                          "#interrupt-cells", FDT_APLIC_INT_CELLS);
    qemu_fdt_setprop(fdt, aplic_name, "interrupt-controller", عدم, 0);

    إذا (props->aia_type == AIA_TYPE_APLIC) {
        qemu_fdt_setprop(fdt, aplic_name, "interrupts-extended",
                         aplic_cells, props->انگ_harts * حجم(uint32_t) * 2);
    } وإلا {
        qemu_fdt_setprop_cell(fdt, aplic_name, "msi-parent", msi_phandle);
    }

    qemu_fdt_setprop_sized_cells(fdt, aplic_name, "reg",
                                 2, aplic_addr, 2, aplic_سائيز);
    qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,num-sources",
                          props->irqchip_انگ_sources);

    إذا (aplic_ٻار_phandle) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "riscv,children",
                              aplic_ٻار_phandle);
        qemu_fdt_setprop_cells(fdt, aplic_name, "riscv,delegation",
                               aplic_ٻار_phandle, 0x1,
                               props->irqchip_انگ_sources);
    }

    إذا (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, aplic_name, "numa-node-id", props->socket);
    }

    qemu_fdt_setprop_cell(fdt, aplic_name, "phandle", aplic_phandle);
}

فراغ riscv_create_fdt_socket_aplic(فراغ *fdt, APLICFdtProps *props,
                                   uint32_t msi_m_phandle,
                                   uint32_t msi_s_phandle,
                                   uint32_t *ايندڙ_phandle,
                                   uint32_t *intc_phandles,
                                   uint32_t *aplic_phandles)
{
    uint32_t aplic_m_phandle, aplic_s_phandle;
    hwaddr aplic_addr;

    إذا (ايندڙ_phandle) {
        aplic_m_phandle = (*ايندڙ_phandle)++;
        aplic_s_phandle = (*ايندڙ_phandle)++;
    } وإلا {
        aplic_m_phandle = qemu_fdt_alloc_phandle(fdt);
        aplic_s_phandle = qemu_fdt_alloc_phandle(fdt);
    }

    إذا (props->aplic_m) {
        /* M-level APLIC node */
        aplic_addr = props->aplic_m->بنياد + (props->aplic_m->سائيز * props->socket);
        create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_m->سائيز,
                             msi_m_phandle, intc_phandles,
                             aplic_m_phandle, aplic_s_phandle,
                             true);
    }

    /* S-level APLIC node */
    aplic_addr = props->aplic_s->بنياد + (props->aplic_s->سائيز * props->socket);
    create_fdt_one_aplic(fdt, props, aplic_addr, props->aplic_s->سائيز,
                         msi_s_phandle, intc_phandles,
                         aplic_s_phandle, 0,
                         false);

    إذا (!props->socket && props->platform_bus_irq) {
        g_autofree حرف *aplic_name = fdt_get_aplic_nodename(aplic_addr);
        platform_bus_جوڙيو_all_fdt_nodes(fdt, aplic_name,
                                       props->platform_bus->بنياد,
                                       props->platform_bus->سائيز,
                                       props->platform_bus_irq);
    }

    aplic_phandles[props->socket] = aplic_s_phandle;
}

فراغ riscv_create_fdt_socket_aclint(فراغ *fdt, ACLINTFdtProps *props,
                                    uint32_t *intc_phandles)
{
    uint32_t aclint_cells_سائيز = props->انگ_harts * حجم(uint32_t) * 2;
    g_autofree uint32_t *aclint_mswi_cells = عدم;
    g_autofree uint32_t *aclint_sswi_cells = عدم;
    g_autofree uint32_t *aclint_mtimer_cells = عدم;
    hwaddr addr, سائيز;
    حرف *name;
    صحيح cpu;

    aclint_mswi_cells = g_new0(uint32_t, props->انگ_harts * 2);
    aclint_mtimer_cells = g_new0(uint32_t, props->انگ_harts * 2);
    aclint_sswi_cells = g_new0(uint32_t, props->انگ_harts * 2);

    لكل (cpu = 0; cpu < props->انگ_harts; cpu++) {
        aclint_mswi_cells[cpu * 2 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        aclint_mswi_cells[cpu * 2 + 1] = cpu_منزل_be32(IRQ_M_SOFT);
        aclint_mtimer_cells[cpu * 2 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        aclint_mtimer_cells[cpu * 2 + 1] = cpu_منزل_be32(IRQ_M_TIMER);
        aclint_sswi_cells[cpu * 2 + 0] = cpu_منزل_be32(intc_phandles[cpu]);
        aclint_sswi_cells[cpu * 2 + 1] = cpu_منزل_be32(IRQ_S_SOFT);
    }

    إذا (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->بنياد + (props->clint->سائيز * props->socket);
        name = g_strdup_printf("/soc/mswi@%"HWADDR_PRIx, addr);

        qemu_fdt_جوڙيو_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible", "riscv,aclint-mswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, RISCV_ACLINT_SWI_سائيز);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_mswi_cells, aclint_cells_سائيز);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", عدم, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        إذا (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }

    إذا (props->aia_type == AIA_TYPE_APLIC_IMSIC) {
        addr = props->clint->بنياد +
               (RISCV_ACLINT_DEFAULT_MTIMER_سائيز * props->socket);
        سائيز = RISCV_ACLINT_DEFAULT_MTIMER_سائيز;
    } وإلا {
        addr = props->clint->بنياد + RISCV_ACLINT_SWI_سائيز +
               (props->clint->سائيز * props->socket);
        سائيز = props->clint->سائيز - RISCV_ACLINT_SWI_سائيز;
    }

    name = g_strdup_printf("/soc/mtimer@%"HWADDR_PRIx,
                           addr + RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_جوڙيو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible",
                            "riscv,aclint-mtimer");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
        2, addr + RISCV_ACLINT_DEFAULT_MTIME,
        2, سائيز - RISCV_ACLINT_DEFAULT_MTIME,
        2, addr + RISCV_ACLINT_DEFAULT_MTIMECMP,
        2, RISCV_ACLINT_DEFAULT_MTIME);
    qemu_fdt_setprop(fdt, name, "interrupts-extended",
                     aclint_mtimer_cells, aclint_cells_سائيز);

    إذا (props->numa_enabled) {
        qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
    }

    g_free(name);

    إذا (props->aia_type != AIA_TYPE_APLIC_IMSIC) {
        addr = props->aclint_sswi->بنياد
               + (props->aclint_sswi->سائيز * props->socket);

        name = g_strdup_printf("/soc/sswi@%"HWADDR_PRIx, addr);
        qemu_fdt_جوڙيو_subnode(fdt, name);
        qemu_fdt_setprop_string(fdt, name, "compatible",
            "riscv,aclint-sswi");
        qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                     2, addr, 2, props->aclint_sswi->سائيز);
        qemu_fdt_setprop(fdt, name, "interrupts-extended",
                         aclint_sswi_cells, aclint_cells_سائيز);
        qemu_fdt_setprop(fdt, name, "interrupt-controller", عدم, 0);
        qemu_fdt_setprop_cell(fdt, name, "#interrupt-cells", 0);

        إذا (props->numa_enabled) {
            qemu_fdt_setprop_cell(fdt, name, "numa-node-id", props->socket);
        }

        g_free(name);
    }
}

حرف *riscv_fdt_get_uart_nodename(hwaddr addr)
{
    أرجع g_strdup_printf("/soc/serial@%"HWADDR_PRIx, addr);
}

فراغ riscv_create_fdt_uart(فراغ *fdt, ثابت Mem_Map_داخلا *uart_mem,
                           صحيح uart_irq, صحيح aia_type,
                           uint32_t irq_phandle)
{
    g_autofree حرف *name = riscv_fdt_get_uart_nodename(uart_mem->بنياد);

    qemu_fdt_جوڙيو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "ns16550a");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, uart_mem->بنياد,
                                 2, uart_mem->سائيز);

    qemu_fdt_setprop_cell(fdt, name, "clock-frequency", UART_STD_CLOCK_FREQ);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);

    إذا (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", uart_irq);
    } وإلا {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", uart_irq, 0x4);
    }
}

فراغ riscv_create_fdt_rtc(فراغ *fdt, ثابت Mem_Map_داخلا *rtc_mem,
                          صحيح rtc_irq, صحيح aia_type,
                          uint32_t irq_phandle)
{
    g_autofree حرف *name = عدم;

    name = g_strdup_printf("/soc/rtc@%"HWADDR_PRIx,
                           rtc_mem->بنياد);
    qemu_fdt_جوڙيو_subnode(fdt, name);
    qemu_fdt_setprop_string(fdt, name, "compatible", "google,goldfish-rtc");
    qemu_fdt_setprop_sized_cells(fdt, name, "reg",
                                 2, rtc_mem->بنياد,
                                 2, rtc_mem->سائيز);
    qemu_fdt_setprop_cell(fdt, name, "interrupt-parent", irq_phandle);
    إذا (aia_type == AIA_TYPE_NONE) {
        qemu_fdt_setprop_cell(fdt, name, "interrupts", rtc_irq);
    } وإلا {
        qemu_fdt_setprop_cells(fdt, name, "interrupts", rtc_irq, 0x4);
    }
}
