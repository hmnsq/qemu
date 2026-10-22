/*
 * Support for generating ACPI tables and passing them to Guests
 *
 * RISC-V virt ACPI generation
 *
 * Copyright (C) 2008-2010  Kevin O'Connor <kevin@koconnor.net>
 * Copyright (C) 2006 Fabrice Bellard
 * Copyright (C) 2013 Red Hat Inc
 * Copyright (c) 2015 HUAWEI TECHNOLOGIES CO.,LTD.
 * Copyright (C) 2021-2023 Ventana Micro Systems Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#অন্তর্ভাব "qemu/osdep.h"
#অন্তর্ভাব "hw/acpi/acpi-defs.h"
#অন্তর্ভাব "hw/acpi/acpi.h"
#অন্তর্ভাব "hw/acpi/aml-build.h"
#অন্তর্ভাব "hw/acpi/pci.h"
#অন্তর্ভাব "hw/acpi/utils.h"
#অন্তর্ভাব "hw/intc/riscv_aclint.h"
#অন্তর্ভাব "hw/nvram/fw_cfg_acpi.h"
#অন্তর্ভাব "hw/pci-host/gpex.h"
#অন্তর্ভাব "hw/riscv/virt.h"
#অন্তর্ভাব "hw/riscv/numa.h"
#অন্তর্ভাব "hw/virtio/virtio-acpi.h"
#অন্তর্ভাব "kvm/kvm_riscv.h"
#অন্তর্ভাব "migration/vmstate.h"
#অন্তর্ভাব "qapi/error.h"
#অন্তর্ভাব "qemu/error-report.h"
#অন্তর্ভাব "system/kvm.h"
#অন্তর্ভাব "system/reset.h"

#অন্তর্ভাব "aia.h"

#define ACPI_BUILD_TABLE_আকার             0x20000
#define ACPI_BUILD_INTC_ID(socket, সূচক) ((socket << 24) | (সূচক))

প্রকারলেখ কাঠামো AcpiBuildState {
    /* Copy of table in RAM (for patching) */
    MemoryRegion *table_mr;
    MemoryRegion *rsdp_mr;
    MemoryRegion *linker_mr;
    /* Is table patched? */
    bool patched;
} AcpiBuildState;

স্থির শূন্য acpi_align_আকার(GArray *blob, unsigned align)
{
    /*
     * Align size to multiple of given size. This reduces the chance
     * we need to change size in the future (breaking cross version migration).
     */
    g_array_set_আকার(blob, ROUND_UP(acpi_তথ্য_len(blob), align));
}

স্থির শূন্য riscv_acpi_madt_add_rintc(uint32_t uid,
                                      ধ্রুবক CPUArchIdList *arch_ids,
                                      GArray *entry,
                                      RISCVVirtState *s)
{
    uint8_t  guest_সূচক_bits = imsic_অঙ্ক_bits(s->aia_guests + 1);
    uint64_t hart_id = arch_ids->cpus[uid].arch_id;
    uint32_t imsic_আকার, local_cpu_id, socket_id;
    uint64_t imsic_socket_addr, imsic_addr;
    MachineState *ms = MACHINE(s);

    socket_id = arch_ids->cpus[uid].props.গ্রন্থি_id;
    local_cpu_id = (arch_ids->cpus[uid].arch_id -
                    riscv_socket_first_hartid(ms, socket_id)) %
                    riscv_socket_hart_গুনতি(ms, socket_id);
    imsic_socket_addr = s->memmap[VIRT_IMSIC_S].base +
                        (socket_id * VIRT_IMSIC_GROUP_MAX_আকার);
    imsic_আকার = IMSIC_HART_আকার(guest_সূচক_bits);
    imsic_addr = imsic_socket_addr + local_cpu_id * imsic_আকার;
    build_append_int_noprefix(entry, 0x18, 1);       /* Type     */
    build_append_int_noprefix(entry, 36, 1);         /*দৈর্ঘ্য*/
    build_append_int_noprefix(entry, 1, 1);          /* Version  */
    build_append_int_noprefix(entry, 0, 1);          /* Reserved */
    build_append_int_noprefix(entry, 0x1, 4);        /* Flags    */
    build_append_int_noprefix(entry, hart_id, 8);    /* Hart ID  */
    build_append_int_noprefix(entry, uid, 4);        /* ACPI Processor UID */
    /* External Interrupt Controller ID */
    যদি (s->aia_type == VIRT_AIA_TYPE_APLIC) {
        build_append_int_noprefix(entry,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.গ্রন্থি_id,
                                      local_cpu_id),
                                  4);
    } অন্যথা যদি (s->aia_type == VIRT_AIA_TYPE_NONE) {
        build_append_int_noprefix(entry,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.গ্রন্থি_id,
                                      kvm_enabled() ?
                                      local_cpu_id :
                                      2 * local_cpu_id + 1),
                                  4);
    } অন্যথা {
        build_append_int_noprefix(entry, 0, 4);
    }

    যদি (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC Base address */
        build_append_int_noprefix(entry, imsic_addr, 8);
        /* IMSIC Size */
        build_append_int_noprefix(entry, imsic_আকার, 4);
    } অন্যথা {
        build_append_int_noprefix(entry, 0, 8);
        build_append_int_noprefix(entry, 0, 4);
    }
}

স্থির শূন্য acpi_dsdt_add_cpus(Aml *scope, RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ধ্রুবক CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);

    জন্য (পূর্ণসংখ্যা i = 0; i < arch_ids->len; i++) {
            Aml *dev;
            GArray *madt_buf = g_array_new(0, 1, 1);

            dev = aml_device("C%.03X", i);
            aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0007")));
            aml_append(dev, aml_name_decl("_UID",
                       aml_int(arch_ids->cpus[i].arch_id)));

            /* build _MAT object */
            riscv_acpi_madt_add_rintc(i, arch_ids, madt_buf, s);
            aml_append(dev, aml_name_decl("_MAT",
                                          aml_বাফার(madt_buf->len,
                                          (uint8_t *)madt_buf->তথ্য)));
            g_array_free(madt_buf, true);

            aml_append(scope, dev);
    }
}

স্থির শূন্য acpi_dsdt_add_plic_aplic(Aml *scope, uint8_t socket_গুনতি,
                                     uint16_t অঙ্ক_sources,
                                     uint64_t mmio_base, uint64_t mmio_আকার,
                                     ধ্রুবক অক্ষর *hid)
{
    uint64_t plic_aplic_addr;
    uint32_t gsi_base;
    uint8_t  socket;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(অঙ্ক_sources <= 1023);

    জন্য (socket = 0; socket < socket_গুনতি; socket++) {
        plic_aplic_addr = mmio_base + mmio_আকার * socket;
        gsi_base = অঙ্ক_sources * socket;
        Aml *dev = aml_device("IC%.02X", socket);
        aml_append(dev, aml_name_decl("_HID", aml_string("%s", hid)));
        aml_append(dev, aml_name_decl("_UID", aml_int(socket)));
        aml_append(dev, aml_name_decl("_GSB", aml_int(gsi_base)));

        Aml *crs = aml_resource_template();
        aml_append(crs, aml_memory32_fixed(plic_aplic_addr, mmio_আকার,
                                           AML_READ_WRITE));
        aml_append(dev, aml_name_decl("_CRS", crs));
        aml_append(scope, dev);
    }
}

স্থির শূন্য
acpi_dsdt_add_uart(Aml *scope, ধ্রুবক MemMapEntry *uart_memmap,
                    uint32_t uart_irq, পূর্ণসংখ্যা uartidx)
{
    Aml *dev = aml_device("COM%d", uartidx);
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0003")));
    aml_append(dev, aml_name_decl("_UID", aml_int(uartidx)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(uart_memmap->base,
                                         uart_memmap->আকার_2, AML_READ_WRITE));
    aml_append(crs,
                aml_interrupt(AML_CONSUMER, AML_LEVEL, AML_ACTIVE_উপরের,
                               AML_EXCLUSIVE, &uart_irq, 1));
    aml_append(dev, aml_name_decl("_CRS", crs));

    Aml *pkg = aml_package(2);
    aml_append(pkg, aml_string("clock-frequency"));
    aml_append(pkg, aml_int(3686400));

    Aml *UUID = aml_touuid("DAFFD814-6EBA-4D8C-8A91-BC9BBF4AA301");

    Aml *pkg1 = aml_package(1);
    aml_append(pkg1, pkg);

    Aml *package = aml_package(2);
    aml_append(package, UUID);
    aml_append(package, pkg1);

    aml_append(dev, aml_name_decl("_DSD", package));
    aml_append(scope, dev);
}

/*
 * Add DSDT entry for the IOMMU platform device.
 * ACPI ID for IOMMU is defined in the section 6.2 of RISC-V BRS spec.
 * https://github.com/riscv-non-isa/riscv-brs/releases/download/v0.8/riscv-brs-spec.pdf
 */
স্থির শূন্য acpi_dsdt_add_iommu_sys(Aml *scope, ধ্রুবক MemMapEntry *iommu_memmap,
                                    uint32_t iommu_irq)
{
    uint32_t i;

    Aml *dev = aml_device("IMU0");
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0004")));
    aml_append(dev, aml_name_decl("_UID", aml_int(0)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(iommu_memmap->base,
                                       iommu_memmap->আকার_2, AML_READ_WRITE));
    জন্য (i = iommu_irq; i < iommu_irq + 4; i++) {
        aml_append(crs, aml_interrupt(AML_CONSUMER, AML_EDGE, AML_ACTIVE_নিচের,
                                      AML_EXCLUSIVE, &i, 1));
    }

    aml_append(dev, aml_name_decl("_CRS", crs));
    aml_append(scope, dev);
}

/*
 * Serial Port Console Redirection Table (SPCR)
 * Rev: 1.10
 */

স্থির শূন্য
spcr_setup(GArray *table_তথ্য, BIOSLinker *linker, RISCVVirtState *s)
{
    ধ্রুবক অক্ষর name[] = ".";
    Acpi_Spcr_তথ্য serial = {
        .interface_type = 0x12,       /* 16550 compatible */
        .base_addr.id = AML_AS_SYSTEM_MEMORY,
        .base_addr.width = 8,
        .base_addr.offset = 0,
        .base_addr.আকার_2 = 1,
        .base_addr.addr = s->memmap[VIRT_UART0].base,
        .interrupt_type = (1 << 4),/* Bit[4] RISC-V PLIC/APLIC */
        .pc_interrupt = 0,
        .interrupt = UART0_IRQ,
        .baud_rate = 7,            /* 15200 */
        .parity = 0,
        .stop_bits = 1,
        .flow_control = 0,
        .terminal_type = 3,        /* ANSI */
        .language = 0,             /* Language */
        .pci_device_id = 0xffff,   /* not a PCI device*/
        .pci_vendor_id = 0xffff,   /* not a PCI device*/
        .pci_bus = 0,
        .pci_device = 0,
        .pci_function = 0,
        .pci_flags = 0,
        .pci_segment = 0,
        .uart_clk_freq = 0,
        .precise_baudrate = 0,
        .namespace_string_দৈর্ঘ্য = আকার(name),
        .namespace_string_offset = 88,
    };

    build_spcr(table_তথ্য, linker, &serial, 4, s->oem_id, s->oem_table_id,
               name);
}

/* RHCT Node[N] starts at offset 56 */
#define RHCT_গ্রন্থি_ARRAY_OFFSET 56

/*
 * ACPI spec, Revision 6.6
 * 5.2.37 RISC-V Hart Capabilities Table (RHCT)
 */
স্থির শূন্য build_rhct(GArray *table_তথ্য,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ধ্রুবক CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    আকার_t len, aligned_len;
    uint32_t isa_offset, অঙ্ক_rhct_nodes, cmo_offset = 0;
    RISCVCPU *cpu = &s->soc[0].harts[0];
    uint32_t mmu_offset = 0;
    bool rv32 = riscv_cpu_is_32bit(cpu);
    g_autofree অক্ষর *isa = ফাঁকা;

    AcpiTable table = { .sig = "RHCT", .rev = 1, .oem_id = s->oem_id,
                        .oem_table_id = s->oem_table_id };

    acpi_table_begin(&table, table_তথ্য);

    build_append_int_noprefix(table_তথ্য, 0x0, 4);   /* Reserved */

    /* Time Base Frequency */
    build_append_int_noprefix(table_তথ্য,
                              kvm_enabled() ?
                              kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                              RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ,
                              8);

    /* ISA + N hart info */
    অঙ্ক_rhct_nodes = 1 + ms->smp.cpus;
    যদি (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        অঙ্ক_rhct_nodes++;
    }

    যদি (!rv32 && cpu->cfg.max_satp_mode >= VM_1_10_SV39) {
        অঙ্ক_rhct_nodes++;
    }

    /* Number of RHCT nodes*/
    build_append_int_noprefix(table_তথ্য, অঙ্ক_rhct_nodes, 4);

    /* Offset to the RHCT node array */
    build_append_int_noprefix(table_তথ্য, RHCT_গ্রন্থি_ARRAY_OFFSET, 4);

    /* ISA String Node */
    isa_offset = table_তথ্য->len - table.table_offset;
    build_append_int_noprefix(table_তথ্য, 0, 2);   /* Type 0 */

    isa = riscv_isa_string(cpu);
    len = 8 + strlen(isa) + 1;
    aligned_len = (len % 2) ? (len + 1) : len;

    build_append_int_noprefix(table_তথ্য, aligned_len, 2);   /* Length */
    build_append_int_noprefix(table_তথ্য, 0x1, 2);           /* Revision */

    /* ISA string length including NUL */
    build_append_int_noprefix(table_তথ্য, strlen(isa) + 1, 2);
    g_array_append_vals(table_তথ্য, isa, strlen(isa) + 1);   /* ISA string */

    যদি (aligned_len != len) {
        build_append_int_noprefix(table_তথ্য, 0x0, 1);   /* Optional Padding */
    }

    /* CMO node */
    যদি (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        cmo_offset = table_তথ্য->len - table.table_offset;
        build_append_int_noprefix(table_তথ্য, 1, 2);    /* Type */
        build_append_int_noprefix(table_তথ্য, 10, 2);   /* Length */
        build_append_int_noprefix(table_তথ্য, 0x1, 2);  /* Revision */
        build_append_int_noprefix(table_তথ্য, 0, 1);    /* Reserved */

        /* CBOM block size */
        যদি (cpu->cfg.cbom_blocksize) {
            build_append_int_noprefix(table_তথ্য,
                                      __builtin_ctz(cpu->cfg.cbom_blocksize),
                                      1);
        } অন্যথা {
            build_append_int_noprefix(table_তথ্য, 0, 1);
        }

        /* CBOP block size */
        build_append_int_noprefix(table_তথ্য, 0, 1);

        /* CBOZ block size */
        যদি (cpu->cfg.cboz_blocksize) {
            build_append_int_noprefix(table_তথ্য,
                                      __builtin_ctz(cpu->cfg.cboz_blocksize),
                                      1);
        } অন্যথা {
            build_append_int_noprefix(table_তথ্য, 0, 1);
        }
    }

    /* MMU node structure */
    যদি (!rv32 && cpu->cfg.max_satp_mode >= VM_1_10_SV39) {
        mmu_offset = table_তথ্য->len - table.table_offset;
        build_append_int_noprefix(table_তথ্য, 2, 2);    /* Type */
        build_append_int_noprefix(table_তথ্য, 8, 2);    /* Length */
        build_append_int_noprefix(table_তথ্য, 0x1, 2);  /* Revision */
        build_append_int_noprefix(table_তথ্য, 0, 1);    /* Reserved */
        /* MMU Type */
        যদি (cpu->cfg.max_satp_mode == VM_1_10_SV57) {
            build_append_int_noprefix(table_তথ্য, 2, 1);    /* Sv57 */
        } অন্যথা যদি (cpu->cfg.max_satp_mode == VM_1_10_SV48) {
            build_append_int_noprefix(table_তথ্য, 1, 1);    /* Sv48 */
        } অন্যথা যদি (cpu->cfg.max_satp_mode == VM_1_10_SV39) {
            build_append_int_noprefix(table_তথ্য, 0, 1);    /* Sv39 */
        } অন্যথা {
            g_assert_not_reached();
        }
    }

    /* Hart Info Node */
    জন্য (পূর্ণসংখ্যা i = 0; i < arch_ids->len; i++) {
        len = 16;
        পূর্ণসংখ্যা অঙ্ক_offsets = 1;
        build_append_int_noprefix(table_তথ্য, 0xFFFF, 2);  /* Type */

        /* Length */
        যদি (cmo_offset) {
            len += 4;
            অঙ্ক_offsets++;
        }

        যদি (mmu_offset) {
            len += 4;
            অঙ্ক_offsets++;
        }

        build_append_int_noprefix(table_তথ্য, len, 2);
        build_append_int_noprefix(table_তথ্য, 0x1, 2); /* Revision */
        /* Number of offsets */
        build_append_int_noprefix(table_তথ্য, অঙ্ক_offsets, 2);
        build_append_int_noprefix(table_তথ্য, i, 4);   /* ACPI Processor UID */
        /* Offsets */
        build_append_int_noprefix(table_তথ্য, isa_offset, 4);
        যদি (cmo_offset) {
            build_append_int_noprefix(table_তথ্য, cmo_offset, 4);
        }

        যদি (mmu_offset) {
            build_append_int_noprefix(table_তথ্য, mmu_offset, 4);
        }
    }

    acpi_table_end(linker, &table);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.9 Fixed ACPI Description Table (MADT)
 */
স্থির শূন্য build_fadt_rev6(GArray *table_তথ্য,
                            BIOSLinker *linker,
                            RISCVVirtState *s,
                            unsigned dsdt_tbl_offset)
{
    Acpi_Fadt_তথ্য fadt = {
        .rev = 6,
        .minor_ver = 6,
        .flags = 1 << ACPI_FADT_F_HW_REDUCED_ACPI,
        .xdsdt_tbl_offset = &dsdt_tbl_offset,
    };

    build_fadt(table_তথ্য, linker, &fadt, s->oem_id, s->oem_table_id);
}

/* DSDT */
স্থির শূন্য build_dsdt(GArray *table_তথ্য,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    Aml *scope, *dsdt;
    MachineState *ms = MACHINE(s);
    uint8_t socket_গুনতি;
    ধ্রুবক MemMapEntry *memmap = s->memmap;
    AcpiTable table = { .sig = "DSDT", .rev = 2, .oem_id = s->oem_id,
                        .oem_table_id = s->oem_table_id };


    acpi_table_begin(&table, table_তথ্য);
    dsdt = init_aml_allocator();

    /*
     * When booting the VM with UEFI, UEFI takes ownership of the RTC hardware.
     * While UEFI can use libfdt to disable the RTC device node in the DTB that
     * it passes to the OS, it cannot modify AML. Therefore, we won't generate
     * the RTC ACPI device at all when using UEFI.
     */
    scope = aml_scope("\\_SB");
    acpi_dsdt_add_cpus(scope, s);

    fw_cfg_acpi_dsdt_add(scope, &memmap[VIRT_FW_CFG]);

    socket_গুনতি = riscv_socket_গুনতি(ms);

    যদি (s->aia_type == VIRT_AIA_TYPE_NONE) {
        acpi_dsdt_add_plic_aplic(scope, socket_গুনতি, s->অঙ্ক_sources,
                                 memmap[VIRT_PLIC].base,
                                 memmap[VIRT_PLIC].আকার_2,
                                 "RSCV0001");
    } অন্যথা {
        acpi_dsdt_add_plic_aplic(scope, socket_গুনতি, s->অঙ্ক_sources,
                                 memmap[VIRT_APLIC_S].base,
                                 memmap[VIRT_APLIC_S].আকার_2, "RSCV0002");
    }

    acpi_dsdt_add_uart(scope, &memmap[VIRT_UART0], UART0_IRQ, 0);
    যদি (s->uart1_present) {
        acpi_dsdt_add_uart(scope, &memmap[VIRT_UART1], UART1_IRQ, 1);
    }

    যদি (virt_is_iommu_sys_enabled(s)) {
        acpi_dsdt_add_iommu_sys(scope, &memmap[VIRT_IOMMU_SYS], IOMMU_SYS_IRQ);
    }

    যদি (socket_গুনতি == 1) {
        virtio_acpi_dsdt_add(scope, memmap[VIRT_VIRTIO].base,
                             memmap[VIRT_VIRTIO].আকার_2,
                             VIRTIO_IRQ, 0, VIRTIO_গুনতি);
        acpi_dsdt_add_gpex_host(scope, PCIE_IRQ);
    } অন্যথা যদি (socket_গুনতি == 2) {
        virtio_acpi_dsdt_add(scope, memmap[VIRT_VIRTIO].base,
                             memmap[VIRT_VIRTIO].আকার_2,
                             VIRTIO_IRQ + s->অঙ্ক_sources, 0,
                             VIRTIO_গুনতি);
        acpi_dsdt_add_gpex_host(scope, PCIE_IRQ + s->অঙ্ক_sources);
    } অন্যথা {
        virtio_acpi_dsdt_add(scope, memmap[VIRT_VIRTIO].base,
                             memmap[VIRT_VIRTIO].আকার_2,
                             VIRTIO_IRQ + s->অঙ্ক_sources, 0,
                             VIRTIO_গুনতি);
        acpi_dsdt_add_gpex_host(scope, PCIE_IRQ + s->অঙ্ক_sources * 2);
    }

    aml_append(dsdt, scope);

    /* copy AML table into ACPI tables blob and patch header there */
    g_array_append_vals(table_তথ্য, dsdt->buf->তথ্য, dsdt->buf->len);

    acpi_table_end(linker, &table);
    free_aml_allocator();
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.12 Multiple APIC Description Table (MADT)
 */
স্থির শূন্য build_madt(GArray *table_তথ্য,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ধ্রুবক CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    uint8_t  group_সূচক_bits = imsic_অঙ্ক_bits(riscv_socket_গুনতি(ms));
    uint8_t  guest_সূচক_bits = imsic_অঙ্ক_bits(s->aia_guests + 1);
    uint16_t imsic_max_hart_per_socket = 0;
    uint8_t  hart_সূচক_bits;
    uint64_t aplic_addr;
    uint32_t gsi_base;
    uint8_t  socket;

    জন্য (socket = 0; socket < riscv_socket_গুনতি(ms); socket++) {
        যদি (imsic_max_hart_per_socket < s->soc[socket].অঙ্ক_harts) {
            imsic_max_hart_per_socket = s->soc[socket].অঙ্ক_harts;
        }
    }

    hart_সূচক_bits = imsic_অঙ্ক_bits(imsic_max_hart_per_socket);

    AcpiTable table = { .sig = "APIC", .rev = 7, .oem_id = s->oem_id,
                        .oem_table_id = s->oem_table_id };

    acpi_table_begin(&table, table_তথ্য);
    /* Local Interrupt Controller Address */
    build_append_int_noprefix(table_তথ্য, 0, 4);
    build_append_int_noprefix(table_তথ্য, 0, 4);   /* MADT Flags */

    /* RISC-V Local INTC structures per HART */
    জন্য (পূর্ণসংখ্যা i = 0; i < arch_ids->len; i++) {
        riscv_acpi_madt_add_rintc(i, arch_ids, table_তথ্য, s);
    }

    /* IMSIC */
    যদি (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC */
        build_append_int_noprefix(table_তথ্য, 0x19, 1);     /* Type */
        build_append_int_noprefix(table_তথ্য, 16, 1);       /* Length */
        build_append_int_noprefix(table_তথ্য, 1, 1);        /* Version */
        build_append_int_noprefix(table_তথ্য, 0, 1);        /* Reserved */
        build_append_int_noprefix(table_তথ্য, 0, 4);        /* Flags */
        /* Number of supervisor mode Interrupt Identities */
        build_append_int_noprefix(table_তথ্য, VIRT_IRQCHIP_অঙ্ক_MSIS, 2);
        /* Number of guest mode Interrupt Identities */
        build_append_int_noprefix(table_তথ্য, VIRT_IRQCHIP_অঙ্ক_MSIS, 2);
        /* Guest Index Bits */
        build_append_int_noprefix(table_তথ্য, guest_সূচক_bits, 1);
        /* Hart Index Bits */
        build_append_int_noprefix(table_তথ্য, hart_সূচক_bits, 1);
        /* Group Index Bits */
        build_append_int_noprefix(table_তথ্য, group_সূচক_bits, 1);
        /* Group Index Shift */
        build_append_int_noprefix(table_তথ্য, IMSIC_MMIO_GROUP_MIN_SHIFT, 1);
    }

    যদি (s->aia_type != VIRT_AIA_TYPE_NONE) {
        /* APLICs */
        জন্য (socket = 0; socket < riscv_socket_গুনতি(ms); socket++) {
            aplic_addr = s->memmap[VIRT_APLIC_S].base +
                             s->memmap[VIRT_APLIC_S].আকার_2 * socket;
            gsi_base = s->অঙ্ক_sources * socket;
            build_append_int_noprefix(table_তথ্য, 0x1A, 1);    /* Type */
            build_append_int_noprefix(table_তথ্য, 36, 1);      /* Length */
            build_append_int_noprefix(table_তথ্য, 1, 1);       /* Version */
            build_append_int_noprefix(table_তথ্য, socket, 1);  /* APLIC ID */
            build_append_int_noprefix(table_তথ্য, 0, 4);       /* Flags */
            build_append_int_noprefix(table_তথ্য, 0, 8);       /* Hardware ID */
            /* Number of IDCs */
            যদি (s->aia_type == VIRT_AIA_TYPE_APLIC) {
                build_append_int_noprefix(table_তথ্য,
                                          s->soc[socket].অঙ্ক_harts,
                                          2);
            } অন্যথা {
                build_append_int_noprefix(table_তথ্য, 0, 2);
            }
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(table_তথ্য, VIRT_IRQCHIP_অঙ্ক_SOURCES, 2);
            /* Global System Interrupt Base */
            build_append_int_noprefix(table_তথ্য, gsi_base, 4);
            /* APLIC Address */
            build_append_int_noprefix(table_তথ্য, aplic_addr, 8);
            /* APLIC size */
            build_append_int_noprefix(table_তথ্য,
                                      s->memmap[VIRT_APLIC_S].আকার_2, 4);
        }
    } অন্যথা {
        /* PLICs */
        জন্য (socket = 0; socket < riscv_socket_গুনতি(ms); socket++) {
            aplic_addr = s->memmap[VIRT_PLIC].base +
                         s->memmap[VIRT_PLIC].আকার_2 * socket;
            gsi_base = VIRT_IRQCHIP_অঙ্ক_SOURCES * socket;
            build_append_int_noprefix(table_তথ্য, 0x1B, 1);   /* Type */
            build_append_int_noprefix(table_তথ্য, 36, 1);     /* Length */
            build_append_int_noprefix(table_তথ্য, 1, 1);      /* Version */
            build_append_int_noprefix(table_তথ্য, socket, 1); /* PLIC ID */
            build_append_int_noprefix(table_তথ্য, 0, 8);      /* Hardware ID */
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(table_তথ্য,
                                      VIRT_IRQCHIP_অঙ্ক_SOURCES - 1, 2);
            build_append_int_noprefix(table_তথ্য, 0, 2);     /* Max Priority */
            build_append_int_noprefix(table_তথ্য, 0, 4);     /* Flags */
            /* PLIC Size */
            build_append_int_noprefix(table_তথ্য, s->memmap[VIRT_PLIC].আকার_2, 4);
            /* PLIC Address */
            build_append_int_noprefix(table_তথ্য, aplic_addr, 8);
            /* Global System Interrupt Vector Base */
            build_append_int_noprefix(table_তথ্য, gsi_base, 4);
        }
    }

    acpi_table_end(linker, &table);
}

#define ID_MAPPING_ENTRY_আকার        20
#define IOMMU_ENTRY_আকার             40
#define RISCV_INTERRUPT_WIRE_OFFSSET 40
#define মূল_COMPLEX_ENTRY_আকার      20
#define RIMT_গ্রন্থি_OFFSET             48

/*
 * ID Mapping Structure
 */
স্থির শূন্য build_rimt_id_mapping(GArray *table_তথ্য, uint32_t source_id_base,
                                  uint32_t অঙ্ক_ids, uint32_t dest_id_base)
{
    /* Source ID Base */
    build_append_int_noprefix(table_তথ্য, source_id_base, 4);
    /* Number of IDs */
    build_append_int_noprefix(table_তথ্য, অঙ্ক_ids, 4);
    /* Destination Device ID Base */
    build_append_int_noprefix(table_তথ্য, source_id_base, 4);
    /* Destination IOMMU Offset */
    build_append_int_noprefix(table_তথ্য, dest_id_base, 4);
    /* Flags */
    build_append_int_noprefix(table_তথ্য, 0, 4);
}

কাঠামো AcpiRimtIdMapping {
    uint32_t source_id_base;
    uint32_t অঙ্ক_ids;
};
প্রকারলেখ কাঠামো AcpiRimtIdMapping AcpiRimtIdMapping;

/* Build the rimt ID mapping to IOMMU for a given PCI host bridge */
স্থির পূর্ণসংখ্যা rimt_host_bridges(Object *obj, শূন্য *opaque)
{
    GArray *idmap_blob = opaque;

    যদি (object_dynamic_cast(obj, TYPE_PCI_HOST_BRIDGE)) {
        PCIBus *bus = PCI_HOST_BRIDGE(obj)->bus;

        যদি (bus && !pci_bus_bypass_iommu(bus)) {
            পূর্ণসংখ্যা min_bus, max_bus;

            pci_bus_range(bus, &min_bus, &max_bus);

            AcpiRimtIdMapping idmap = {
                .source_id_base = min_bus << 8,
                .অঙ্ক_ids = (max_bus - min_bus + 1) << 8,
            };
            g_array_append_val(idmap_blob, idmap);
        }
    }

    ফেরত 0;
}

স্থির পূর্ণসংখ্যা rimt_idmap_compare(gconstpointer a, gconstpointer b)
{
    AcpiRimtIdMapping *idmap_a = (AcpiRimtIdMapping *)a;
    AcpiRimtIdMapping *idmap_b = (AcpiRimtIdMapping *)b;

    ফেরত idmap_a->source_id_base - idmap_b->source_id_base;
}

/*
 * RISC-V IO Mapping Table (RIMT)
 * https://github.com/riscv-non-isa/riscv-acpi-rimt/releases/download/v0.99/rimt-spec.pdf
 */
স্থির শূন্য build_rimt(GArray *table_তথ্য, BIOSLinker *linker,
                       RISCVVirtState *s)
{
    পূর্ণসংখ্যা i, nb_nodes, rc_mapping_গুনতি;
    আকার_t গ্রন্থি_আকার, iommu_offset = 0;
    uint32_t id = 0;
    g_autoptr(GArray) iommu_idmaps = g_array_new(false, true,
                                                 আকার(AcpiRimtIdMapping));

    AcpiTable table = { .sig = "RIMT", .rev = 1, .oem_id = s->oem_id,
                        .oem_table_id = s->oem_table_id };

    acpi_table_begin(&table, table_তথ্য);

    object_child_foreach_recursive(object_get_মূল(),
                                   rimt_host_bridges, iommu_idmaps);

    /* Sort the ID mapping  by Source ID Base*/
    g_array_sort(iommu_idmaps, rimt_idmap_compare);

    nb_nodes = 2; /* RC, IOMMU */
    rc_mapping_গুনতি = iommu_idmaps->len;
    /* Number of RIMT Nodes */
    build_append_int_noprefix(table_তথ্য, nb_nodes, 4);

    /* Offset to Array of RIMT Nodes */
    build_append_int_noprefix(table_তথ্য, RIMT_গ্রন্থি_OFFSET, 4);
    build_append_int_noprefix(table_তথ্য, 0, 4); /* Reserved */

    iommu_offset = table_তথ্য->len - table.table_offset;
    /*  IOMMU Device Structure */
    build_append_int_noprefix(table_তথ্য, 0, 1);         /* Type - IOMMU*/
    build_append_int_noprefix(table_তথ্য, 1, 1);         /* Revision */
    গ্রন্থি_আকার =  IOMMU_ENTRY_আকার;
    build_append_int_noprefix(table_তথ্য, গ্রন্থি_আকার, 2); /* Length */
    build_append_int_noprefix(table_তথ্য, 0, 2);         /* Reserved */
    build_append_int_noprefix(table_তথ্য, id++, 2);      /* ID */
    যদি (virt_is_iommu_sys_enabled(s)) {
        /* Hardware ID */
        build_append_int_noprefix(table_তথ্য, 'R', 1);
        build_append_int_noprefix(table_তথ্য, 'S', 1);
        build_append_int_noprefix(table_তথ্য, 'C', 1);
        build_append_int_noprefix(table_তথ্য, 'V', 1);
        build_append_int_noprefix(table_তথ্য, '0', 1);
        build_append_int_noprefix(table_তথ্য, '0', 1);
        build_append_int_noprefix(table_তথ্য, '0', 1);
        build_append_int_noprefix(table_তথ্য, '4', 1);
        /* Base Address */
        build_append_int_noprefix(table_তথ্য,
                                  s->memmap[VIRT_IOMMU_SYS].base, 8);
        build_append_int_noprefix(table_তথ্য, 0, 4);   /* Flags */
    } অন্যথা {
        /* Hardware ID */
        build_append_int_noprefix(table_তথ্য, '0', 1);
        build_append_int_noprefix(table_তথ্য, '0', 1);
        build_append_int_noprefix(table_তথ্য, '1', 1);
        build_append_int_noprefix(table_তথ্য, '0', 1);
        build_append_int_noprefix(table_তথ্য, '0', 1);
        build_append_int_noprefix(table_তথ্য, '0', 1);
        build_append_int_noprefix(table_তথ্য, '1', 1);
        build_append_int_noprefix(table_তথ্য, '4', 1);

        build_append_int_noprefix(table_তথ্য, 0, 8);   /* Base Address */
        build_append_int_noprefix(table_তথ্য, 1, 4);   /* Flags */
    }

    build_append_int_noprefix(table_তথ্য, 0, 4);       /* Proximity Domain */
    build_append_int_noprefix(table_তথ্য, 0, 2);       /* PCI Segment number */
    /* PCIe B/D/F */
    যদি (virt_is_iommu_sys_enabled(s)) {
        build_append_int_noprefix(table_তথ্য, 0, 2);
    } অন্যথা {
        build_append_int_noprefix(table_তথ্য, s->pci_iommu_bdf, 2);
    }
    /* Number of interrupt wires */
    build_append_int_noprefix(table_তথ্য, 0, 2);
    /* Interrupt wire array offset */
    build_append_int_noprefix(table_তথ্য, RISCV_INTERRUPT_WIRE_OFFSSET, 2);

    /*  PCIe Root Complex Node */
    build_append_int_noprefix(table_তথ্য, 1, 1);           /* Type */
    build_append_int_noprefix(table_তথ্য, 1, 1);           /* Revision */
    গ্রন্থি_আকার =  মূল_COMPLEX_ENTRY_আকার +
                 ID_MAPPING_ENTRY_আকার * rc_mapping_গুনতি;
    build_append_int_noprefix(table_তথ্য, গ্রন্থি_আকার, 2);   /* Length */
    build_append_int_noprefix(table_তথ্য, 0, 2);           /* Reserved */
    build_append_int_noprefix(table_তথ্য, id++, 2);        /* ID */
    build_append_int_noprefix(table_তথ্য, 0, 4);           /* Flags */
    build_append_int_noprefix(table_তথ্য, 0, 2);           /* Reserved */
    /* PCI Segment number */
    build_append_int_noprefix(table_তথ্য, 0, 2);
    /* ID mapping array offset */
    build_append_int_noprefix(table_তথ্য, মূল_COMPLEX_ENTRY_আকার, 2);
    /* Number of ID mappings */
    build_append_int_noprefix(table_তথ্য, rc_mapping_গুনতি, 2);

    /* Output Reference */
    AcpiRimtIdMapping *range;

    /* ID mapping array */
    জন্য (i = 0; i < iommu_idmaps->len; i++) {
        range = &g_array_সূচক(iommu_idmaps, AcpiRimtIdMapping, i);
        যদি (virt_is_iommu_sys_enabled(s)) {
            range->source_id_base = 0;
            range->অঙ্ক_ids = 0x10000;
        } অন্যথা {
            range->source_id_base = s->pci_iommu_bdf + 1;
            range->অঙ্ক_ids = 0xffff - s->pci_iommu_bdf;
        }
        build_rimt_id_mapping(table_তথ্য, range->source_id_base,
                              range->অঙ্ক_ids, iommu_offset);
    }

    acpi_table_end(linker, &table);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.16 System Resource Affinity Table (SRAT)
 */
স্থির শূন্য
build_srat(GArray *table_তথ্য, BIOSLinker *linker, RISCVVirtState *vms)
{
    পূর্ণসংখ্যা i;
    uint64_t mem_base;
    MachineClass *mc = MACHINE_GET_CLASS(vms);
    MachineState *ms = MACHINE(vms);
    ধ্রুবক CPUArchIdList *cpu_list = mc->possible_cpu_arch_ids(ms);
    AcpiTable table = { .sig = "SRAT", .rev = 3, .oem_id = vms->oem_id,
                        .oem_table_id = vms->oem_table_id };

    acpi_table_begin(&table, table_তথ্য);
    build_append_int_noprefix(table_তথ্য, 1, 4); /* Reserved */
    build_append_int_noprefix(table_তথ্য, 0, 8); /* Reserved */

    জন্য (i = 0; i < cpu_list->len; ++i) {
        uint32_t nodeid = cpu_list->cpus[i].props.গ্রন্থি_id;
        /*
         * 5.2.16.8 RINTC Affinity Structure
         */
        build_append_int_noprefix(table_তথ্য, 7, 1);      /* Type */
        build_append_int_noprefix(table_তথ্য, 20, 1);     /* Length */
        build_append_int_noprefix(table_তথ্য, 0, 2);        /* Reserved */
        build_append_int_noprefix(table_তথ্য, nodeid, 4); /* Proximity Domain */
        build_append_int_noprefix(table_তথ্য, i, 4); /* ACPI Processor UID */
        /* Flags, Table 5-70 */
        build_append_int_noprefix(table_তথ্য, 1 /* Flags: Enabled */, 4);
        build_append_int_noprefix(table_তথ্য, 0, 4); /* Clock Domain */
    }

    mem_base = vms->memmap[VIRT_DRAM].base;
    জন্য (i = 0; i < ms->numa_state->অঙ্ক_nodes; ++i) {
        যদি (ms->numa_state->nodes[i].গ্রন্থি_mem > 0) {
            build_srat_memory(table_তথ্য, mem_base,
                              ms->numa_state->nodes[i].গ্রন্থি_mem, i,
                              MEM_AFFINITY_ENABLED);
            mem_base += ms->numa_state->nodes[i].গ্রন্থি_mem;
        }
    }

    acpi_table_end(linker, &table);
}

স্থির শূন্য virt_acpi_build(RISCVVirtState *s, AcpiBuildTables *tables)
{
    GArray *table_offsets;
    unsigned dsdt, xsdt;
    GArray *tables_blob = tables->table_তথ্য;
    MachineState *ms = MACHINE(s);

    table_offsets = g_array_new(false, true,
                                আকার(uint32_t));

    bios_linker_loader_alloc(tables->linker,
                             ACPI_BUILD_TABLE_FILE, tables_blob,
                             64, false);

    /* DSDT is pointed to by FADT */
    dsdt = tables_blob->len;
    build_dsdt(tables_blob, tables->linker, s);

    /* FADT and others pointed to by XSDT */
    acpi_add_table(table_offsets, tables_blob);
    build_fadt_rev6(tables_blob, tables->linker, s, dsdt);

    acpi_add_table(table_offsets, tables_blob);
    build_madt(tables_blob, tables->linker, s);

    acpi_add_table(table_offsets, tables_blob);
    build_rhct(tables_blob, tables->linker, s);

    যদি (virt_is_iommu_sys_enabled(s) || s->pci_iommu_bdf) {
        acpi_add_table(table_offsets, tables_blob);
        build_rimt(tables_blob, tables->linker, s);
    }

    acpi_add_table(table_offsets, tables_blob);

    যদি (ms->acpi_spcr_enabled) {
        spcr_setup(tables_blob, tables->linker, s);
    }

    acpi_add_table(table_offsets, tables_blob);
    {
        AcpiMcfgInfo mcfg = {
           .base = s->memmap[VIRT_PCIE_ECAM].base,
           .আকার_2 = s->memmap[VIRT_PCIE_ECAM].আকার_2,
        };
        build_mcfg(tables_blob, tables->linker, &mcfg, s->oem_id,
                   s->oem_table_id);
    }

    যদি (ms->numa_state->অঙ্ক_nodes > 0) {
        acpi_add_table(table_offsets, tables_blob);
        build_srat(tables_blob, tables->linker, s);
        যদি (ms->numa_state->have_numa_distance) {
            acpi_add_table(table_offsets, tables_blob);
            build_slit(tables_blob, tables->linker, ms, s->oem_id,
                       s->oem_table_id);
        }
    }

    /* XSDT is pointed to by RSDP */
    xsdt = tables_blob->len;
    build_xsdt(tables_blob, tables->linker, table_offsets, s->oem_id,
                s->oem_table_id);

    /* RSDP is in FSEG memory, so allocate it separately */
    {
        Acpi_Rsdp_তথ্য rsdp_তথ্য = {
            .revision = 2,
            .oem_id = s->oem_id,
            .xsdt_tbl_offset = &xsdt,
            .rsdt_tbl_offset = ফাঁকা,
        };
        build_rsdp(tables->rsdp, tables->linker, &rsdp_তথ্য);
    }

    /*
     * The align size is 128, warn if 64k is not enough therefore
     * the align size could be resized.
     */
    যদি (tables_blob->len > ACPI_BUILD_TABLE_আকার / 2) {
        warn_report("ACPI table size %u exceeds %d bytes,"
                    " migration may not work",
                    tables_blob->len, ACPI_BUILD_TABLE_আকার / 2);
        error_printf("Try removing some objects.");
    }

    acpi_align_আকার(tables_blob, ACPI_BUILD_TABLE_আকার);

    /* Clean up memory that's no longer used */
    g_array_free(table_offsets, true);
}

স্থির শূন্য acpi_ram_update(MemoryRegion *mr, GArray *তথ্য)
{
    uint32_t আকার_2 = acpi_তথ্য_len(তথ্য);

    /*
     * Make sure RAM size is correct - in case it got changed
     * e.g. by migration
     */
    memory_region_ram_resize(mr, আকার_2, &error_abort);

    memcpy(memory_region_get_ram_ptr(mr), তথ্য->তথ্য, আকার_2);
    memory_region_set_dirty(mr, 0, আকার_2);
}

স্থির শূন্য virt_acpi_build_update(শূন্য *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    AcpiBuildTables tables;

    /* No state to update or already patched? Nothing to do. */
    যদি (!build_state || build_state->patched) {
        ফেরত;
    }

    build_state->patched = true;

    acpi_build_tables_init(&tables);

    virt_acpi_build(RISCV_VIRT_MACHINE(qdev_get_machine()), &tables);

    acpi_ram_update(build_state->table_mr, tables.table_তথ্য);
    acpi_ram_update(build_state->rsdp_mr, tables.rsdp);
    acpi_ram_update(build_state->linker_mr, tables.linker->cmd_blob);

    acpi_build_tables_cleanup(&tables, true);
}

স্থির শূন্য virt_acpi_build_reset(শূন্য *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    build_state->patched = false;
}

স্থির ধ্রুবক VMStateDescription vmstate_virt_acpi_build = {
    .name = "virt_acpi_build",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (ধ্রুবক VMStateField[]) {
        VMSTATE_BOOL(patched, AcpiBuildState),
        VMSTATE_END_OF_LIST()
    },
};

শূন্য virt_acpi_setup(RISCVVirtState *s)
{
    AcpiBuildTables tables;
    AcpiBuildState *build_state;

    build_state = g_malloc0(আকার *build_state);

    acpi_build_tables_init(&tables);
    virt_acpi_build(s, &tables);

    /* Now expose it all to Guest */
    build_state->table_mr = acpi_add_rom_blob(virt_acpi_build_update,
                                              build_state, tables.table_তথ্য,
                                              ACPI_BUILD_TABLE_FILE);
    প্রতিজ্ঞা(build_state->table_mr != ফাঁকা);

    build_state->linker_mr = acpi_add_rom_blob(virt_acpi_build_update,
                                               build_state,
                                               tables.linker->cmd_blob,
                                               ACPI_BUILD_LOADER_FILE);

    build_state->rsdp_mr = acpi_add_rom_blob(virt_acpi_build_update,
                                             build_state, tables.rsdp,
                                             ACPI_BUILD_RSDP_FILE);

    qemu_register_reset(virt_acpi_build_reset, build_state);
    virt_acpi_build_reset(build_state);
    vmstate_register(ফাঁকা, 0, &vmstate_virt_acpi_build, build_state);

    /*
     * Clean up tables but don't free the memory: we track it
     * in build_state.
     */
    acpi_build_tables_cleanup(&tables, false);
}
