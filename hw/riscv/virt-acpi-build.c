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

#समावेश "qemu/osdep.h"
#समावेश "hw/acpi/acpi-defs.h"
#समावेश "hw/acpi/acpi.h"
#समावेश "hw/acpi/aml-build.h"
#समावेश "hw/acpi/pci.h"
#समावेश "hw/acpi/utils.h"
#समावेश "hw/intc/riscv_aclint.h"
#समावेश "hw/nvram/fw_cfg_acpi.h"
#समावेश "hw/pci-host/gpex.h"
#समावेश "hw/riscv/virt.h"
#समावेश "hw/riscv/numa.h"
#समावेश "hw/virtio/virtio-acpi.h"
#समावेश "kvm/kvm_riscv.h"
#समावेश "migration/vmstate.h"
#समावेश "qapi/error.h"
#समावेश "qemu/error-report.h"
#समावेश "system/kvm.h"
#समावेश "system/reset.h"

#समावेश "aia.h"

#घोषणा ACPI_BUILD_TABLE_आकार             0x20000
#घोषणा ACPI_BUILD_INTC_ID(socket, सूचकांक) ((socket << 24) | (सूचकांक))

प्रकार काष्ठा AcpiBuildState {
    /* Copy of table in RAM (for patching) */
    MemoryRegion *table_mr;
    MemoryRegion *rsdp_mr;
    MemoryRegion *linker_mr;
    /* Is table patched? */
    bool patched;
} AcpiBuildState;

अटल व्योम acpi_align_आकार(GArray *blob, अचिन्हित align)
{
    /*
     * Align size to multiple of given size. This reduces the chance
     * we need to change size in the future (breaking cross version migration).
     */
    g_array_set_आकार(blob, ROUND_UP(acpi_तथ्य_len(blob), align));
}

अटल व्योम riscv_acpi_madt_add_rintc(uint32_t uid,
                                      स्थिर CPUArchIdList *arch_ids,
                                      GArray *entry,
                                      RISCVVirtState *s)
{
    uint8_t  guest_सूचकांक_bits = imsic_अंक_bits(s->aia_guests + 1);
    uint64_t hart_id = arch_ids->cpus[uid].arch_id;
    uint32_t imsic_आकार, local_cpu_id, socket_id;
    uint64_t imsic_socket_addr, imsic_addr;
    MachineState *ms = MACHINE(s);

    socket_id = arch_ids->cpus[uid].props.गाँठ_id;
    local_cpu_id = (arch_ids->cpus[uid].arch_id -
                    riscv_socket_first_hartid(ms, socket_id)) %
                    riscv_socket_hart_गिनती(ms, socket_id);
    imsic_socket_addr = s->memmap[VIRT_IMSIC_S].base +
                        (socket_id * VIRT_IMSIC_GROUP_MAX_आकार);
    imsic_आकार = IMSIC_HART_आकार(guest_सूचकांक_bits);
    imsic_addr = imsic_socket_addr + local_cpu_id * imsic_आकार;
    build_append_int_noprefix(entry, 0x18, 1);       /* Type     */
    build_append_int_noprefix(entry, 36, 1);         /*लंबाई*/
    build_append_int_noprefix(entry, 1, 1);          /* Version  */
    build_append_int_noprefix(entry, 0, 1);          /* Reserved */
    build_append_int_noprefix(entry, 0x1, 4);        /* Flags    */
    build_append_int_noprefix(entry, hart_id, 8);    /* Hart ID  */
    build_append_int_noprefix(entry, uid, 4);        /* ACPI Processor UID */
    /* External Interrupt Controller ID */
    अगर (s->aia_type == VIRT_AIA_TYPE_APLIC) {
        build_append_int_noprefix(entry,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.गाँठ_id,
                                      local_cpu_id),
                                  4);
    } अन्यथा अगर (s->aia_type == VIRT_AIA_TYPE_NONE) {
        build_append_int_noprefix(entry,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.गाँठ_id,
                                      kvm_enabled() ?
                                      local_cpu_id :
                                      2 * local_cpu_id + 1),
                                  4);
    } अन्यथा {
        build_append_int_noprefix(entry, 0, 4);
    }

    अगर (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC Base address */
        build_append_int_noprefix(entry, imsic_addr, 8);
        /* IMSIC Size */
        build_append_int_noprefix(entry, imsic_आकार, 4);
    } अन्यथा {
        build_append_int_noprefix(entry, 0, 8);
        build_append_int_noprefix(entry, 0, 4);
    }
}

अटल व्योम acpi_dsdt_add_cpus(Aml *scope, RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    स्थिर CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);

    क्रम (पूर्णांक i = 0; i < arch_ids->len; i++) {
            Aml *dev;
            GArray *madt_buf = g_array_new(0, 1, 1);

            dev = aml_device("C%.03X", i);
            aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0007")));
            aml_append(dev, aml_name_decl("_UID",
                       aml_int(arch_ids->cpus[i].arch_id)));

            /* build _MAT object */
            riscv_acpi_madt_add_rintc(i, arch_ids, madt_buf, s);
            aml_append(dev, aml_name_decl("_MAT",
                                          aml_बफर(madt_buf->len,
                                          (uint8_t *)madt_buf->तथ्य)));
            g_array_free(madt_buf, true);

            aml_append(scope, dev);
    }
}

अटल व्योम acpi_dsdt_add_plic_aplic(Aml *scope, uint8_t socket_गिनती,
                                     uint16_t अंक_sources,
                                     uint64_t mmio_base, uint64_t mmio_आकार,
                                     स्थिर अक्षर *hid)
{
    uint64_t plic_aplic_addr;
    uint32_t gsi_base;
    uint8_t  socket;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(अंक_sources <= 1023);

    क्रम (socket = 0; socket < socket_गिनती; socket++) {
        plic_aplic_addr = mmio_base + mmio_आकार * socket;
        gsi_base = अंक_sources * socket;
        Aml *dev = aml_device("IC%.02X", socket);
        aml_append(dev, aml_name_decl("_HID", aml_string("%s", hid)));
        aml_append(dev, aml_name_decl("_UID", aml_int(socket)));
        aml_append(dev, aml_name_decl("_GSB", aml_int(gsi_base)));

        Aml *crs = aml_resource_template();
        aml_append(crs, aml_memory32_fixed(plic_aplic_addr, mmio_आकार,
                                           AML_READ_WRITE));
        aml_append(dev, aml_name_decl("_CRS", crs));
        aml_append(scope, dev);
    }
}

अटल व्योम
acpi_dsdt_add_uart(Aml *scope, स्थिर MemMapEntry *uart_memmap,
                    uint32_t uart_irq, पूर्णांक uartidx)
{
    Aml *dev = aml_device("COM%d", uartidx);
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0003")));
    aml_append(dev, aml_name_decl("_UID", aml_int(uartidx)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(uart_memmap->base,
                                         uart_memmap->आकार, AML_READ_WRITE));
    aml_append(crs,
                aml_interrupt(AML_CONSUMER, AML_LEVEL, AML_ACTIVE_ऊपरी,
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
अटल व्योम acpi_dsdt_add_iommu_sys(Aml *scope, स्थिर MemMapEntry *iommu_memmap,
                                    uint32_t iommu_irq)
{
    uint32_t i;

    Aml *dev = aml_device("IMU0");
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0004")));
    aml_append(dev, aml_name_decl("_UID", aml_int(0)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(iommu_memmap->base,
                                       iommu_memmap->आकार, AML_READ_WRITE));
    क्रम (i = iommu_irq; i < iommu_irq + 4; i++) {
        aml_append(crs, aml_interrupt(AML_CONSUMER, AML_EDGE, AML_ACTIVE_निचला,
                                      AML_EXCLUSIVE, &i, 1));
    }

    aml_append(dev, aml_name_decl("_CRS", crs));
    aml_append(scope, dev);
}

/*
 * Serial Port Console Redirection Table (SPCR)
 * Rev: 1.10
 */

अटल व्योम
spcr_setup(GArray *table_तथ्य, BIOSLinker *linker, RISCVVirtState *s)
{
    स्थिर अक्षर name[] = ".";
    Acpi_Spcr_तथ्य serial = {
        .interface_type = 0x12,       /* 16550 compatible */
        .base_addr.id = AML_AS_SYSTEM_MEMORY,
        .base_addr.width = 8,
        .base_addr.offset = 0,
        .base_addr.आकार = 1,
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
        .namespace_string_लंबाई = माप(name),
        .namespace_string_offset = 88,
    };

    build_spcr(table_तथ्य, linker, &serial, 4, s->oem_id, s->oem_table_id,
               name);
}

/* RHCT Node[N] starts at offset 56 */
#घोषणा RHCT_गाँठ_ARRAY_OFFSET 56

/*
 * ACPI spec, Revision 6.6
 * 5.2.37 RISC-V Hart Capabilities Table (RHCT)
 */
अटल व्योम build_rhct(GArray *table_तथ्य,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    स्थिर CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    माप_प्रकार len, aligned_len;
    uint32_t isa_offset, अंक_rhct_nodes, cmo_offset = 0;
    RISCVCPU *cpu = &s->soc[0].harts[0];
    uint32_t mmu_offset = 0;
    bool rv32 = riscv_cpu_is_32bit(cpu);
    g_autofree अक्षर *isa = शून्य;

    AcpiTable table = { .sig = "RHCT", .rev = 1, .oem_id = s->oem_id,
                        .oem_table_id = s->oem_table_id };

    acpi_table_begin(&table, table_तथ्य);

    build_append_int_noprefix(table_तथ्य, 0x0, 4);   /* Reserved */

    /* Time Base Frequency */
    build_append_int_noprefix(table_तथ्य,
                              kvm_enabled() ?
                              kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                              RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ,
                              8);

    /* ISA + N hart info */
    अंक_rhct_nodes = 1 + ms->smp.cpus;
    अगर (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        अंक_rhct_nodes++;
    }

    अगर (!rv32 && cpu->cfg.max_satp_mode >= VM_1_10_SV39) {
        अंक_rhct_nodes++;
    }

    /* Number of RHCT nodes*/
    build_append_int_noprefix(table_तथ्य, अंक_rhct_nodes, 4);

    /* Offset to the RHCT node array */
    build_append_int_noprefix(table_तथ्य, RHCT_गाँठ_ARRAY_OFFSET, 4);

    /* ISA String Node */
    isa_offset = table_तथ्य->len - table.table_offset;
    build_append_int_noprefix(table_तथ्य, 0, 2);   /* Type 0 */

    isa = riscv_isa_string(cpu);
    len = 8 + म_माप(isa) + 1;
    aligned_len = (len % 2) ? (len + 1) : len;

    build_append_int_noprefix(table_तथ्य, aligned_len, 2);   /* Length */
    build_append_int_noprefix(table_तथ्य, 0x1, 2);           /* Revision */

    /* ISA string length including NUL */
    build_append_int_noprefix(table_तथ्य, म_माप(isa) + 1, 2);
    g_array_append_vals(table_तथ्य, isa, म_माप(isa) + 1);   /* ISA string */

    अगर (aligned_len != len) {
        build_append_int_noprefix(table_तथ्य, 0x0, 1);   /* Optional Padding */
    }

    /* CMO node */
    अगर (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        cmo_offset = table_तथ्य->len - table.table_offset;
        build_append_int_noprefix(table_तथ्य, 1, 2);    /* Type */
        build_append_int_noprefix(table_तथ्य, 10, 2);   /* Length */
        build_append_int_noprefix(table_तथ्य, 0x1, 2);  /* Revision */
        build_append_int_noprefix(table_तथ्य, 0, 1);    /* Reserved */

        /* CBOM block size */
        अगर (cpu->cfg.cbom_blocksize) {
            build_append_int_noprefix(table_तथ्य,
                                      __builtin_ctz(cpu->cfg.cbom_blocksize),
                                      1);
        } अन्यथा {
            build_append_int_noprefix(table_तथ्य, 0, 1);
        }

        /* CBOP block size */
        build_append_int_noprefix(table_तथ्य, 0, 1);

        /* CBOZ block size */
        अगर (cpu->cfg.cboz_blocksize) {
            build_append_int_noprefix(table_तथ्य,
                                      __builtin_ctz(cpu->cfg.cboz_blocksize),
                                      1);
        } अन्यथा {
            build_append_int_noprefix(table_तथ्य, 0, 1);
        }
    }

    /* MMU node structure */
    अगर (!rv32 && cpu->cfg.max_satp_mode >= VM_1_10_SV39) {
        mmu_offset = table_तथ्य->len - table.table_offset;
        build_append_int_noprefix(table_तथ्य, 2, 2);    /* Type */
        build_append_int_noprefix(table_तथ्य, 8, 2);    /* Length */
        build_append_int_noprefix(table_तथ्य, 0x1, 2);  /* Revision */
        build_append_int_noprefix(table_तथ्य, 0, 1);    /* Reserved */
        /* MMU Type */
        अगर (cpu->cfg.max_satp_mode == VM_1_10_SV57) {
            build_append_int_noprefix(table_तथ्य, 2, 1);    /* Sv57 */
        } अन्यथा अगर (cpu->cfg.max_satp_mode == VM_1_10_SV48) {
            build_append_int_noprefix(table_तथ्य, 1, 1);    /* Sv48 */
        } अन्यथा अगर (cpu->cfg.max_satp_mode == VM_1_10_SV39) {
            build_append_int_noprefix(table_तथ्य, 0, 1);    /* Sv39 */
        } अन्यथा {
            g_assert_not_reached();
        }
    }

    /* Hart Info Node */
    क्रम (पूर्णांक i = 0; i < arch_ids->len; i++) {
        len = 16;
        पूर्णांक अंक_offsets = 1;
        build_append_int_noprefix(table_तथ्य, 0xFFFF, 2);  /* Type */

        /* Length */
        अगर (cmo_offset) {
            len += 4;
            अंक_offsets++;
        }

        अगर (mmu_offset) {
            len += 4;
            अंक_offsets++;
        }

        build_append_int_noprefix(table_तथ्य, len, 2);
        build_append_int_noprefix(table_तथ्य, 0x1, 2); /* Revision */
        /* Number of offsets */
        build_append_int_noprefix(table_तथ्य, अंक_offsets, 2);
        build_append_int_noprefix(table_तथ्य, i, 4);   /* ACPI Processor UID */
        /* Offsets */
        build_append_int_noprefix(table_तथ्य, isa_offset, 4);
        अगर (cmo_offset) {
            build_append_int_noprefix(table_तथ्य, cmo_offset, 4);
        }

        अगर (mmu_offset) {
            build_append_int_noprefix(table_तथ्य, mmu_offset, 4);
        }
    }

    acpi_table_end(linker, &table);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.9 Fixed ACPI Description Table (MADT)
 */
अटल व्योम build_fadt_rev6(GArray *table_तथ्य,
                            BIOSLinker *linker,
                            RISCVVirtState *s,
                            अचिन्हित dsdt_tbl_offset)
{
    Acpi_Fadt_तथ्य fadt = {
        .rev = 6,
        .minor_ver = 6,
        .flags = 1 << ACPI_FADT_F_HW_REDUCED_ACPI,
        .xdsdt_tbl_offset = &dsdt_tbl_offset,
    };

    build_fadt(table_तथ्य, linker, &fadt, s->oem_id, s->oem_table_id);
}

/* DSDT */
अटल व्योम build_dsdt(GArray *table_तथ्य,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    Aml *scope, *dsdt;
    MachineState *ms = MACHINE(s);
    uint8_t socket_गिनती;
    स्थिर MemMapEntry *memmap = s->memmap;
    AcpiTable table = { .sig = "DSDT", .rev = 2, .oem_id = s->oem_id,
                        .oem_table_id = s->oem_table_id };


    acpi_table_begin(&table, table_तथ्य);
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

    socket_गिनती = riscv_socket_गिनती(ms);

    अगर (s->aia_type == VIRT_AIA_TYPE_NONE) {
        acpi_dsdt_add_plic_aplic(scope, socket_गिनती, s->अंक_sources,
                                 memmap[VIRT_PLIC].base,
                                 memmap[VIRT_PLIC].आकार,
                                 "RSCV0001");
    } अन्यथा {
        acpi_dsdt_add_plic_aplic(scope, socket_गिनती, s->अंक_sources,
                                 memmap[VIRT_APLIC_S].base,
                                 memmap[VIRT_APLIC_S].आकार, "RSCV0002");
    }

    acpi_dsdt_add_uart(scope, &memmap[VIRT_UART0], UART0_IRQ, 0);
    अगर (s->uart1_present) {
        acpi_dsdt_add_uart(scope, &memmap[VIRT_UART1], UART1_IRQ, 1);
    }

    अगर (virt_is_iommu_sys_enabled(s)) {
        acpi_dsdt_add_iommu_sys(scope, &memmap[VIRT_IOMMU_SYS], IOMMU_SYS_IRQ);
    }

    अगर (socket_गिनती == 1) {
        virtio_acpi_dsdt_add(scope, memmap[VIRT_VIRTIO].base,
                             memmap[VIRT_VIRTIO].आकार,
                             VIRTIO_IRQ, 0, VIRTIO_गिनती);
        acpi_dsdt_add_gpex_host(scope, PCIE_IRQ);
    } अन्यथा अगर (socket_गिनती == 2) {
        virtio_acpi_dsdt_add(scope, memmap[VIRT_VIRTIO].base,
                             memmap[VIRT_VIRTIO].आकार,
                             VIRTIO_IRQ + s->अंक_sources, 0,
                             VIRTIO_गिनती);
        acpi_dsdt_add_gpex_host(scope, PCIE_IRQ + s->अंक_sources);
    } अन्यथा {
        virtio_acpi_dsdt_add(scope, memmap[VIRT_VIRTIO].base,
                             memmap[VIRT_VIRTIO].आकार,
                             VIRTIO_IRQ + s->अंक_sources, 0,
                             VIRTIO_गिनती);
        acpi_dsdt_add_gpex_host(scope, PCIE_IRQ + s->अंक_sources * 2);
    }

    aml_append(dsdt, scope);

    /* copy AML table into ACPI tables blob and patch header there */
    g_array_append_vals(table_तथ्य, dsdt->buf->तथ्य, dsdt->buf->len);

    acpi_table_end(linker, &table);
    free_aml_allocator();
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.12 Multiple APIC Description Table (MADT)
 */
अटल व्योम build_madt(GArray *table_तथ्य,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    स्थिर CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    uint8_t  group_सूचकांक_bits = imsic_अंक_bits(riscv_socket_गिनती(ms));
    uint8_t  guest_सूचकांक_bits = imsic_अंक_bits(s->aia_guests + 1);
    uint16_t imsic_max_hart_per_socket = 0;
    uint8_t  hart_सूचकांक_bits;
    uint64_t aplic_addr;
    uint32_t gsi_base;
    uint8_t  socket;

    क्रम (socket = 0; socket < riscv_socket_गिनती(ms); socket++) {
        अगर (imsic_max_hart_per_socket < s->soc[socket].अंक_harts) {
            imsic_max_hart_per_socket = s->soc[socket].अंक_harts;
        }
    }

    hart_सूचकांक_bits = imsic_अंक_bits(imsic_max_hart_per_socket);

    AcpiTable table = { .sig = "APIC", .rev = 7, .oem_id = s->oem_id,
                        .oem_table_id = s->oem_table_id };

    acpi_table_begin(&table, table_तथ्य);
    /* Local Interrupt Controller Address */
    build_append_int_noprefix(table_तथ्य, 0, 4);
    build_append_int_noprefix(table_तथ्य, 0, 4);   /* MADT Flags */

    /* RISC-V Local INTC structures per HART */
    क्रम (पूर्णांक i = 0; i < arch_ids->len; i++) {
        riscv_acpi_madt_add_rintc(i, arch_ids, table_तथ्य, s);
    }

    /* IMSIC */
    अगर (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC */
        build_append_int_noprefix(table_तथ्य, 0x19, 1);     /* Type */
        build_append_int_noprefix(table_तथ्य, 16, 1);       /* Length */
        build_append_int_noprefix(table_तथ्य, 1, 1);        /* Version */
        build_append_int_noprefix(table_तथ्य, 0, 1);        /* Reserved */
        build_append_int_noprefix(table_तथ्य, 0, 4);        /* Flags */
        /* Number of supervisor mode Interrupt Identities */
        build_append_int_noprefix(table_तथ्य, VIRT_IRQCHIP_अंक_MSIS, 2);
        /* Number of guest mode Interrupt Identities */
        build_append_int_noprefix(table_तथ्य, VIRT_IRQCHIP_अंक_MSIS, 2);
        /* Guest Index Bits */
        build_append_int_noprefix(table_तथ्य, guest_सूचकांक_bits, 1);
        /* Hart Index Bits */
        build_append_int_noprefix(table_तथ्य, hart_सूचकांक_bits, 1);
        /* Group Index Bits */
        build_append_int_noprefix(table_तथ्य, group_सूचकांक_bits, 1);
        /* Group Index Shift */
        build_append_int_noprefix(table_तथ्य, IMSIC_MMIO_GROUP_MIN_SHIFT, 1);
    }

    अगर (s->aia_type != VIRT_AIA_TYPE_NONE) {
        /* APLICs */
        क्रम (socket = 0; socket < riscv_socket_गिनती(ms); socket++) {
            aplic_addr = s->memmap[VIRT_APLIC_S].base +
                             s->memmap[VIRT_APLIC_S].आकार * socket;
            gsi_base = s->अंक_sources * socket;
            build_append_int_noprefix(table_तथ्य, 0x1A, 1);    /* Type */
            build_append_int_noprefix(table_तथ्य, 36, 1);      /* Length */
            build_append_int_noprefix(table_तथ्य, 1, 1);       /* Version */
            build_append_int_noprefix(table_तथ्य, socket, 1);  /* APLIC ID */
            build_append_int_noprefix(table_तथ्य, 0, 4);       /* Flags */
            build_append_int_noprefix(table_तथ्य, 0, 8);       /* Hardware ID */
            /* Number of IDCs */
            अगर (s->aia_type == VIRT_AIA_TYPE_APLIC) {
                build_append_int_noprefix(table_तथ्य,
                                          s->soc[socket].अंक_harts,
                                          2);
            } अन्यथा {
                build_append_int_noprefix(table_तथ्य, 0, 2);
            }
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(table_तथ्य, VIRT_IRQCHIP_अंक_SOURCES, 2);
            /* Global System Interrupt Base */
            build_append_int_noprefix(table_तथ्य, gsi_base, 4);
            /* APLIC Address */
            build_append_int_noprefix(table_तथ्य, aplic_addr, 8);
            /* APLIC size */
            build_append_int_noprefix(table_तथ्य,
                                      s->memmap[VIRT_APLIC_S].आकार, 4);
        }
    } अन्यथा {
        /* PLICs */
        क्रम (socket = 0; socket < riscv_socket_गिनती(ms); socket++) {
            aplic_addr = s->memmap[VIRT_PLIC].base +
                         s->memmap[VIRT_PLIC].आकार * socket;
            gsi_base = VIRT_IRQCHIP_अंक_SOURCES * socket;
            build_append_int_noprefix(table_तथ्य, 0x1B, 1);   /* Type */
            build_append_int_noprefix(table_तथ्य, 36, 1);     /* Length */
            build_append_int_noprefix(table_तथ्य, 1, 1);      /* Version */
            build_append_int_noprefix(table_तथ्य, socket, 1); /* PLIC ID */
            build_append_int_noprefix(table_तथ्य, 0, 8);      /* Hardware ID */
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(table_तथ्य,
                                      VIRT_IRQCHIP_अंक_SOURCES - 1, 2);
            build_append_int_noprefix(table_तथ्य, 0, 2);     /* Max Priority */
            build_append_int_noprefix(table_तथ्य, 0, 4);     /* Flags */
            /* PLIC Size */
            build_append_int_noprefix(table_तथ्य, s->memmap[VIRT_PLIC].आकार, 4);
            /* PLIC Address */
            build_append_int_noprefix(table_तथ्य, aplic_addr, 8);
            /* Global System Interrupt Vector Base */
            build_append_int_noprefix(table_तथ्य, gsi_base, 4);
        }
    }

    acpi_table_end(linker, &table);
}

#घोषणा ID_MAPPING_ENTRY_आकार        20
#घोषणा IOMMU_ENTRY_आकार             40
#घोषणा RISCV_INTERRUPT_WIRE_OFFSSET 40
#घोषणा मूल_COMPLEX_ENTRY_आकार      20
#घोषणा RIMT_गाँठ_OFFSET             48

/*
 * ID Mapping Structure
 */
अटल व्योम build_rimt_id_mapping(GArray *table_तथ्य, uint32_t source_id_base,
                                  uint32_t अंक_ids, uint32_t dest_id_base)
{
    /* Source ID Base */
    build_append_int_noprefix(table_तथ्य, source_id_base, 4);
    /* Number of IDs */
    build_append_int_noprefix(table_तथ्य, अंक_ids, 4);
    /* Destination Device ID Base */
    build_append_int_noprefix(table_तथ्य, source_id_base, 4);
    /* Destination IOMMU Offset */
    build_append_int_noprefix(table_तथ्य, dest_id_base, 4);
    /* Flags */
    build_append_int_noprefix(table_तथ्य, 0, 4);
}

काष्ठा AcpiRimtIdMapping {
    uint32_t source_id_base;
    uint32_t अंक_ids;
};
प्रकार काष्ठा AcpiRimtIdMapping AcpiRimtIdMapping;

/* Build the rimt ID mapping to IOMMU for a given PCI host bridge */
अटल पूर्णांक rimt_host_bridges(Object *obj, व्योम *opaque)
{
    GArray *idmap_blob = opaque;

    अगर (object_dynamic_cast(obj, TYPE_PCI_HOST_BRIDGE)) {
        PCIBus *bus = PCI_HOST_BRIDGE(obj)->bus;

        अगर (bus && !pci_bus_bypass_iommu(bus)) {
            पूर्णांक min_bus, max_bus;

            pci_bus_range(bus, &min_bus, &max_bus);

            AcpiRimtIdMapping idmap = {
                .source_id_base = min_bus << 8,
                .अंक_ids = (max_bus - min_bus + 1) << 8,
            };
            g_array_append_val(idmap_blob, idmap);
        }
    }

    वापस 0;
}

अटल पूर्णांक rimt_idmap_compare(gconstpointer a, gconstpointer b)
{
    AcpiRimtIdMapping *idmap_a = (AcpiRimtIdMapping *)a;
    AcpiRimtIdMapping *idmap_b = (AcpiRimtIdMapping *)b;

    वापस idmap_a->source_id_base - idmap_b->source_id_base;
}

/*
 * RISC-V IO Mapping Table (RIMT)
 * https://github.com/riscv-non-isa/riscv-acpi-rimt/releases/download/v0.99/rimt-spec.pdf
 */
अटल व्योम build_rimt(GArray *table_तथ्य, BIOSLinker *linker,
                       RISCVVirtState *s)
{
    पूर्णांक i, nb_nodes, rc_mapping_गिनती;
    माप_प्रकार गाँठ_आकार, iommu_offset = 0;
    uint32_t id = 0;
    g_autoptr(GArray) iommu_idmaps = g_array_new(false, true,
                                                 माप(AcpiRimtIdMapping));

    AcpiTable table = { .sig = "RIMT", .rev = 1, .oem_id = s->oem_id,
                        .oem_table_id = s->oem_table_id };

    acpi_table_begin(&table, table_तथ्य);

    object_child_foreach_recursive(object_get_मूल(),
                                   rimt_host_bridges, iommu_idmaps);

    /* Sort the ID mapping  by Source ID Base*/
    g_array_sort(iommu_idmaps, rimt_idmap_compare);

    nb_nodes = 2; /* RC, IOMMU */
    rc_mapping_गिनती = iommu_idmaps->len;
    /* Number of RIMT Nodes */
    build_append_int_noprefix(table_तथ्य, nb_nodes, 4);

    /* Offset to Array of RIMT Nodes */
    build_append_int_noprefix(table_तथ्य, RIMT_गाँठ_OFFSET, 4);
    build_append_int_noprefix(table_तथ्य, 0, 4); /* Reserved */

    iommu_offset = table_तथ्य->len - table.table_offset;
    /*  IOMMU Device Structure */
    build_append_int_noprefix(table_तथ्य, 0, 1);         /* Type - IOMMU*/
    build_append_int_noprefix(table_तथ्य, 1, 1);         /* Revision */
    गाँठ_आकार =  IOMMU_ENTRY_आकार;
    build_append_int_noprefix(table_तथ्य, गाँठ_आकार, 2); /* Length */
    build_append_int_noprefix(table_तथ्य, 0, 2);         /* Reserved */
    build_append_int_noprefix(table_तथ्य, id++, 2);      /* ID */
    अगर (virt_is_iommu_sys_enabled(s)) {
        /* Hardware ID */
        build_append_int_noprefix(table_तथ्य, 'R', 1);
        build_append_int_noprefix(table_तथ्य, 'S', 1);
        build_append_int_noprefix(table_तथ्य, 'C', 1);
        build_append_int_noprefix(table_तथ्य, 'V', 1);
        build_append_int_noprefix(table_तथ्य, '0', 1);
        build_append_int_noprefix(table_तथ्य, '0', 1);
        build_append_int_noprefix(table_तथ्य, '0', 1);
        build_append_int_noprefix(table_तथ्य, '4', 1);
        /* Base Address */
        build_append_int_noprefix(table_तथ्य,
                                  s->memmap[VIRT_IOMMU_SYS].base, 8);
        build_append_int_noprefix(table_तथ्य, 0, 4);   /* Flags */
    } अन्यथा {
        /* Hardware ID */
        build_append_int_noprefix(table_तथ्य, '0', 1);
        build_append_int_noprefix(table_तथ्य, '0', 1);
        build_append_int_noprefix(table_तथ्य, '1', 1);
        build_append_int_noprefix(table_तथ्य, '0', 1);
        build_append_int_noprefix(table_तथ्य, '0', 1);
        build_append_int_noprefix(table_तथ्य, '0', 1);
        build_append_int_noprefix(table_तथ्य, '1', 1);
        build_append_int_noprefix(table_तथ्य, '4', 1);

        build_append_int_noprefix(table_तथ्य, 0, 8);   /* Base Address */
        build_append_int_noprefix(table_तथ्य, 1, 4);   /* Flags */
    }

    build_append_int_noprefix(table_तथ्य, 0, 4);       /* Proximity Domain */
    build_append_int_noprefix(table_तथ्य, 0, 2);       /* PCI Segment number */
    /* PCIe B/D/F */
    अगर (virt_is_iommu_sys_enabled(s)) {
        build_append_int_noprefix(table_तथ्य, 0, 2);
    } अन्यथा {
        build_append_int_noprefix(table_तथ्य, s->pci_iommu_bdf, 2);
    }
    /* Number of interrupt wires */
    build_append_int_noprefix(table_तथ्य, 0, 2);
    /* Interrupt wire array offset */
    build_append_int_noprefix(table_तथ्य, RISCV_INTERRUPT_WIRE_OFFSSET, 2);

    /*  PCIe Root Complex Node */
    build_append_int_noprefix(table_तथ्य, 1, 1);           /* Type */
    build_append_int_noprefix(table_तथ्य, 1, 1);           /* Revision */
    गाँठ_आकार =  मूल_COMPLEX_ENTRY_आकार +
                 ID_MAPPING_ENTRY_आकार * rc_mapping_गिनती;
    build_append_int_noprefix(table_तथ्य, गाँठ_आकार, 2);   /* Length */
    build_append_int_noprefix(table_तथ्य, 0, 2);           /* Reserved */
    build_append_int_noprefix(table_तथ्य, id++, 2);        /* ID */
    build_append_int_noprefix(table_तथ्य, 0, 4);           /* Flags */
    build_append_int_noprefix(table_तथ्य, 0, 2);           /* Reserved */
    /* PCI Segment number */
    build_append_int_noprefix(table_तथ्य, 0, 2);
    /* ID mapping array offset */
    build_append_int_noprefix(table_तथ्य, मूल_COMPLEX_ENTRY_आकार, 2);
    /* Number of ID mappings */
    build_append_int_noprefix(table_तथ्य, rc_mapping_गिनती, 2);

    /* Output Reference */
    AcpiRimtIdMapping *range;

    /* ID mapping array */
    क्रम (i = 0; i < iommu_idmaps->len; i++) {
        range = &g_array_सूचकांक(iommu_idmaps, AcpiRimtIdMapping, i);
        अगर (virt_is_iommu_sys_enabled(s)) {
            range->source_id_base = 0;
            range->अंक_ids = 0x10000;
        } अन्यथा {
            range->source_id_base = s->pci_iommu_bdf + 1;
            range->अंक_ids = 0xffff - s->pci_iommu_bdf;
        }
        build_rimt_id_mapping(table_तथ्य, range->source_id_base,
                              range->अंक_ids, iommu_offset);
    }

    acpi_table_end(linker, &table);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.16 System Resource Affinity Table (SRAT)
 */
अटल व्योम
build_srat(GArray *table_तथ्य, BIOSLinker *linker, RISCVVirtState *vms)
{
    पूर्णांक i;
    uint64_t mem_base;
    MachineClass *mc = MACHINE_GET_CLASS(vms);
    MachineState *ms = MACHINE(vms);
    स्थिर CPUArchIdList *cpu_list = mc->possible_cpu_arch_ids(ms);
    AcpiTable table = { .sig = "SRAT", .rev = 3, .oem_id = vms->oem_id,
                        .oem_table_id = vms->oem_table_id };

    acpi_table_begin(&table, table_तथ्य);
    build_append_int_noprefix(table_तथ्य, 1, 4); /* Reserved */
    build_append_int_noprefix(table_तथ्य, 0, 8); /* Reserved */

    क्रम (i = 0; i < cpu_list->len; ++i) {
        uint32_t nodeid = cpu_list->cpus[i].props.गाँठ_id;
        /*
         * 5.2.16.8 RINTC Affinity Structure
         */
        build_append_int_noprefix(table_तथ्य, 7, 1);      /* Type */
        build_append_int_noprefix(table_तथ्य, 20, 1);     /* Length */
        build_append_int_noprefix(table_तथ्य, 0, 2);        /* Reserved */
        build_append_int_noprefix(table_तथ्य, nodeid, 4); /* Proximity Domain */
        build_append_int_noprefix(table_तथ्य, i, 4); /* ACPI Processor UID */
        /* Flags, Table 5-70 */
        build_append_int_noprefix(table_तथ्य, 1 /* Flags: Enabled */, 4);
        build_append_int_noprefix(table_तथ्य, 0, 4); /* Clock Domain */
    }

    mem_base = vms->memmap[VIRT_DRAM].base;
    क्रम (i = 0; i < ms->numa_state->अंक_nodes; ++i) {
        अगर (ms->numa_state->nodes[i].गाँठ_mem > 0) {
            build_srat_memory(table_तथ्य, mem_base,
                              ms->numa_state->nodes[i].गाँठ_mem, i,
                              MEM_AFFINITY_ENABLED);
            mem_base += ms->numa_state->nodes[i].गाँठ_mem;
        }
    }

    acpi_table_end(linker, &table);
}

अटल व्योम virt_acpi_build(RISCVVirtState *s, AcpiBuildTables *tables)
{
    GArray *table_offsets;
    अचिन्हित dsdt, xsdt;
    GArray *tables_blob = tables->table_तथ्य;
    MachineState *ms = MACHINE(s);

    table_offsets = g_array_new(false, true,
                                माप(uint32_t));

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

    अगर (virt_is_iommu_sys_enabled(s) || s->pci_iommu_bdf) {
        acpi_add_table(table_offsets, tables_blob);
        build_rimt(tables_blob, tables->linker, s);
    }

    acpi_add_table(table_offsets, tables_blob);

    अगर (ms->acpi_spcr_enabled) {
        spcr_setup(tables_blob, tables->linker, s);
    }

    acpi_add_table(table_offsets, tables_blob);
    {
        AcpiMcfgInfo mcfg = {
           .base = s->memmap[VIRT_PCIE_ECAM].base,
           .आकार = s->memmap[VIRT_PCIE_ECAM].आकार,
        };
        build_mcfg(tables_blob, tables->linker, &mcfg, s->oem_id,
                   s->oem_table_id);
    }

    अगर (ms->numa_state->अंक_nodes > 0) {
        acpi_add_table(table_offsets, tables_blob);
        build_srat(tables_blob, tables->linker, s);
        अगर (ms->numa_state->have_numa_distance) {
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
        Acpi_Rsdp_तथ्य rsdp_तथ्य = {
            .revision = 2,
            .oem_id = s->oem_id,
            .xsdt_tbl_offset = &xsdt,
            .rsdt_tbl_offset = शून्य,
        };
        build_rsdp(tables->rsdp, tables->linker, &rsdp_तथ्य);
    }

    /*
     * The align size is 128, warn if 64k is not enough therefore
     * the align size could be resized.
     */
    अगर (tables_blob->len > ACPI_BUILD_TABLE_आकार / 2) {
        warn_report("ACPI table size %u exceeds %d bytes,"
                    " migration may not work",
                    tables_blob->len, ACPI_BUILD_TABLE_आकार / 2);
        error_printf("Try removing some objects.");
    }

    acpi_align_आकार(tables_blob, ACPI_BUILD_TABLE_आकार);

    /* Clean up memory that's no longer used */
    g_array_free(table_offsets, true);
}

अटल व्योम acpi_ram_update(MemoryRegion *mr, GArray *तथ्य)
{
    uint32_t आकार = acpi_तथ्य_len(तथ्य);

    /*
     * Make sure RAM size is correct - in case it got changed
     * e.g. by migration
     */
    memory_region_ram_resize(mr, आकार, &error_abort);

    स_नकल(memory_region_get_ram_ptr(mr), तथ्य->तथ्य, आकार);
    memory_region_set_dirty(mr, 0, आकार);
}

अटल व्योम virt_acpi_build_update(व्योम *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    AcpiBuildTables tables;

    /* No state to update or already patched? Nothing to do. */
    अगर (!build_state || build_state->patched) {
        वापस;
    }

    build_state->patched = true;

    acpi_build_tables_init(&tables);

    virt_acpi_build(RISCV_VIRT_MACHINE(qdev_get_machine()), &tables);

    acpi_ram_update(build_state->table_mr, tables.table_तथ्य);
    acpi_ram_update(build_state->rsdp_mr, tables.rsdp);
    acpi_ram_update(build_state->linker_mr, tables.linker->cmd_blob);

    acpi_build_tables_cleanup(&tables, true);
}

अटल व्योम virt_acpi_build_reset(व्योम *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    build_state->patched = false;
}

अटल स्थिर VMStateDescription vmstate_virt_acpi_build = {
    .name = "virt_acpi_build",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (स्थिर VMStateField[]) {
        VMSTATE_BOOL(patched, AcpiBuildState),
        VMSTATE_END_OF_LIST()
    },
};

व्योम virt_acpi_setup(RISCVVirtState *s)
{
    AcpiBuildTables tables;
    AcpiBuildState *build_state;

    build_state = g_malloc0(माप *build_state);

    acpi_build_tables_init(&tables);
    virt_acpi_build(s, &tables);

    /* Now expose it all to Guest */
    build_state->table_mr = acpi_add_rom_blob(virt_acpi_build_update,
                                              build_state, tables.table_तथ्य,
                                              ACPI_BUILD_TABLE_FILE);
    निश्चित(build_state->table_mr != शून्य);

    build_state->linker_mr = acpi_add_rom_blob(virt_acpi_build_update,
                                               build_state,
                                               tables.linker->cmd_blob,
                                               ACPI_BUILD_LOADER_FILE);

    build_state->rsdp_mr = acpi_add_rom_blob(virt_acpi_build_update,
                                             build_state, tables.rsdp,
                                             ACPI_BUILD_RSDP_FILE);

    qemu_register_reset(virt_acpi_build_reset, build_state);
    virt_acpi_build_reset(build_state);
    vmstate_register(शून्य, 0, &vmstate_virt_acpi_build, build_state);

    /*
     * Clean up tables but don't free the memory: we track it
     * in build_state.
     */
    acpi_build_tables_cleanup(&tables, false);
}
