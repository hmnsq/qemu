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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "hw/acpi/acpi-defs.h"
#अन्तर्भाव "hw/acpi/acpi.h"
#अन्तर्भाव "hw/acpi/aml-build.h"
#अन्तर्भाव "hw/acpi/pci.h"
#अन्तर्भाव "hw/acpi/utils.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "hw/nvram/fw_cfg_acpi.h"
#अन्तर्भाव "hw/pci-host/gpex.h"
#अन्तर्भाव "hw/riscv/virt.h"
#अन्तर्भाव "hw/riscv/numa.h"
#अन्तर्भाव "hw/virtio/virtio-acpi.h"
#अन्तर्भाव "kvm/kvm_riscv.h"
#अन्तर्भाव "migration/vmstate.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "system/kvm.h"
#अन्तर्भाव "system/reset.h"

#अन्तर्भाव "aia.h"

#परिभाषय ACPI_BUILD_सारणी_आकार             0x20000
#परिभाषय ACPI_BUILD_INTC_ID(socket, सूचकांक) ((socket << 24) | (सूचकांक))

प्रकारनिर्देश संरचना AcpiBuildState {
    /* Copy of table in RAM (for patching) */
    स्मृतिः_Region *सारणी_mr;
    स्मृतिः_Region *rsdp_mr;
    स्मृतिः_Region *linker_mr;
    /* Is table patched? */
    bool patched;
} AcpiBuildState;

स्थिर शून्य acpi_align_आकार(GArray *blob, अचिह्नित align)
{
    /*
     * Align size to multiple of given size. This reduces the chance
     * we need to change size in the future (breaking cross version migration).
     */
    g_array_समुच्चयः_आकार(blob, आवर्तनम्_ऊर्ध्वम्(acpi_दत्त_len(blob), align));
}

स्थिर शून्य riscv_acpi_madt_सङ्कलय_rintc(uint32_t uid,
                                      ध्रुव CPUArchIdList *arch_ids,
                                      GArray *प्रविष्टिः,
                                      RISCVVirtState *s)
{
    uint8_t  guest_सूचकांक_द्वयङ्काः = imsic_अङ्कः_द्वयङ्काः(s->aia_guests + 1);
    uint64_t hart_id = arch_ids->cpus[uid].arch_id;
    uint32_t imsic_आकार, local_cpu_id, socket_id;
    uint64_t imsic_socket_addr, imsic_addr;
    MachineState *ms = MACHINE(s);

    socket_id = arch_ids->cpus[uid].props.ग्रन्थिः_id;
    local_cpu_id = (arch_ids->cpus[uid].arch_id -
                    riscv_socket_first_hartid(ms, socket_id)) %
                    riscv_socket_hart_गणनम्(ms, socket_id);
    imsic_socket_addr = s->memmap[VIRT_IMSIC_S].आधारः +
                        (socket_id * VIRT_IMSIC_गणः_MAX_आकार);
    imsic_आकार = IMSIC_HART_आकार(guest_सूचकांक_द्वयङ्काः);
    imsic_addr = imsic_socket_addr + local_cpu_id * imsic_आकार;
    build_append_int_noprefix(प्रविष्टिः, 0x18, 1);       /* Type     */
    build_append_int_noprefix(प्रविष्टिः, 36, 1);         /*दीर्घता*/
    build_append_int_noprefix(प्रविष्टिः, 1, 1);          /*संस्करणम्*/
    build_append_int_noprefix(प्रविष्टिः, 0, 1);          /* Reserved */
    build_append_int_noprefix(प्रविष्टिः, 0x1, 4);        /* Flags    */
    build_append_int_noprefix(प्रविष्टिः, hart_id, 8);    /* Hart ID  */
    build_append_int_noprefix(प्रविष्टिः, uid, 4);        /* ACPI Processor UID */
    /* External Interrupt Controller ID */
    यदि (s->aia_type == VIRT_AIA_TYPE_APLIC) {
        build_append_int_noprefix(प्रविष्टिः,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.ग्रन्थिः_id,
                                      local_cpu_id),
                                  4);
    } अन्यथा यदि (s->aia_type == VIRT_AIA_TYPE_NONE) {
        build_append_int_noprefix(प्रविष्टिः,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.ग्रन्थिः_id,
                                      kvm_enabled() ?
                                      local_cpu_id :
                                      2 * local_cpu_id + 1),
                                  4);
    } अन्यथा {
        build_append_int_noprefix(प्रविष्टिः, 0, 4);
    }

    यदि (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC Base address */
        build_append_int_noprefix(प्रविष्टिः, imsic_addr, 8);
        /* IMSIC Size */
        build_append_int_noprefix(प्रविष्टिः, imsic_आकार, 4);
    } अन्यथा {
        build_append_int_noprefix(प्रविष्टिः, 0, 8);
        build_append_int_noprefix(प्रविष्टिः, 0, 4);
    }
}

स्थिर शून्य acpi_dsdt_सङ्कलय_cpus(Aml *scope, RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ध्रुव CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);

    प्रति (पूर्णाङ्क i = 0; i < arch_ids->len; i++) {
            Aml *dev;
            GArray *madt_buf = g_array_new(0, 1, 1);

            dev = aml_device("C%.03X", i);
            aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0007")));
            aml_append(dev, aml_name_decl("_UID",
                       aml_int(arch_ids->cpus[i].arch_id)));

            /* build _MAT object */
            riscv_acpi_madt_सङ्कलय_rintc(i, arch_ids, madt_buf, s);
            aml_append(dev, aml_name_decl("_MAT",
                                          aml_बफर(madt_buf->len,
                                          (uint8_t *)madt_buf->दत्त)));
            g_array_free(madt_buf, true);

            aml_append(scope, dev);
    }
}

स्थिर शून्य acpi_dsdt_सङ्कलय_plic_aplic(Aml *scope, uint8_t socket_गणनम्,
                                     uint16_t अङ्कः_sources,
                                     uint64_t mmio_आधारः, uint64_t mmio_आकार,
                                     ध्रुव वर्ण *hid)
{
    uint64_t plic_aplic_addr;
    uint32_t gsi_आधारः;
    uint8_t  socket;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(अङ्कः_sources <= 1023);

    प्रति (socket = 0; socket < socket_गणनम्; socket++) {
        plic_aplic_addr = mmio_आधारः + mmio_आकार * socket;
        gsi_आधारः = अङ्कः_sources * socket;
        Aml *dev = aml_device("IC%.02X", socket);
        aml_append(dev, aml_name_decl("_HID", aml_string("%s", hid)));
        aml_append(dev, aml_name_decl("_UID", aml_int(socket)));
        aml_append(dev, aml_name_decl("_GSB", aml_int(gsi_आधारः)));

        Aml *crs = aml_resource_template();
        aml_append(crs, aml_memory32_fixed(plic_aplic_addr, mmio_आकार,
                                           AML_READ_WRITE));
        aml_append(dev, aml_name_decl("_CRS", crs));
        aml_append(scope, dev);
    }
}

स्थिर शून्य
acpi_dsdt_सङ्कलय_uart(Aml *scope, ध्रुव Mem_Map_प्रविष्टिः *uart_memmap,
                    uint32_t uart_irq, पूर्णाङ्क uartidx)
{
    Aml *dev = aml_device("COM%d", uartidx);
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0003")));
    aml_append(dev, aml_name_decl("_UID", aml_int(uartidx)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(uart_memmap->आधारः,
                                         uart_memmap->आकार, AML_READ_WRITE));
    aml_append(crs,
                aml_व्यवधानम्(AML_उपभोक्ता, AML_LEVEL, AML_सक्रिय_उच्चः,
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
स्थिर शून्य acpi_dsdt_सङ्कलय_iommu_sys(Aml *scope, ध्रुव Mem_Map_प्रविष्टिः *iommu_memmap,
                                    uint32_t iommu_irq)
{
    uint32_t i;

    Aml *dev = aml_device("IMU0");
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0004")));
    aml_append(dev, aml_name_decl("_UID", aml_int(0)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(iommu_memmap->आधारः,
                                       iommu_memmap->आकार, AML_READ_WRITE));
    प्रति (i = iommu_irq; i < iommu_irq + 4; i++) {
        aml_append(crs, aml_व्यवधानम्(AML_उपभोक्ता, AML_EDGE, AML_सक्रिय_नीचः,
                                      AML_EXCLUSIVE, &i, 1));
    }

    aml_append(dev, aml_name_decl("_CRS", crs));
    aml_append(scope, dev);
}

/*
 * Serial Port Console Redirection Table (SPCR)
 * Rev: 1.10
 */

स्थिर शून्य
spcr_सज्जता(GArray *सारणी_दत्त, BIOSLinker *linker, RISCVVirtState *s)
{
    ध्रुव वर्ण name[] = ".";
    Acpi_Spcr_दत्त serial = {
        .interface_type = 0x12,       /* 16550 compatible */
        .आधारः_addr.id = AML_AS_SYSTEM_स्मृतिः,
        .आधारः_addr.विस्तारः = 8,
        .आधारः_addr.विस्थापनम् = 0,
        .आधारः_addr.आकार = 1,
        .आधारः_addr.addr = s->memmap[VIRT_UART0].आधारः,
        .व्यवधानम्_type = (1 << 4),/* Bit[4] RISC-V PLIC/APLIC */
        .pc_व्यवधानम् = 0,
        .व्यवधानम् = UART0_IRQ,
        .baud_दरः = 7,            /* 15200 */
        .parity = 0,
        .stop_द्वयङ्काः = 1,
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
        .namespace_string_दीर्घता = परिमाण(name),
        .namespace_string_विस्थापनम् = 88,
    };

    build_spcr(सारणी_दत्त, linker, &serial, 4, s->oem_id, s->oem_सारणी_id,
               name);
}

/* RHCT Node[N] starts at offset 56 */
#परिभाषय RHCT_ग्रन्थिः_ARRAY_विस्थापनम् 56

/*
 * ACPI spec, Revision 6.6
 * 5.2.37 RISC-V Hart Capabilities Table (RHCT)
 */
स्थिर शून्य build_rhct(GArray *सारणी_दत्त,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ध्रुव CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    आकार_t len, aligned_len;
    uint32_t isa_विस्थापनम्, अङ्कः_rhct_nodes, cmo_विस्थापनम् = 0;
    RISCVCPU *cpu = &s->soc[0].harts[0];
    uint32_t mmu_विस्थापनम् = 0;
    bool rv32 = riscv_cpu_is_32bit(cpu);
    g_autofree वर्ण *isa = रिक्त;

    Acpi_सारणी सारणी = { .sig = "RHCT", .rev = 1, .oem_id = s->oem_id,
                        .oem_सारणी_id = s->oem_सारणी_id };

    acpi_सारणी_begin(&सारणी, सारणी_दत्त);

    build_append_int_noprefix(सारणी_दत्त, 0x0, 4);   /* Reserved */

    /* Time Base Frequency */
    build_append_int_noprefix(सारणी_दत्त,
                              kvm_enabled() ?
                              kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                              RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ,
                              8);

    /* ISA + N hart info */
    अङ्कः_rhct_nodes = 1 + ms->smp.cpus;
    यदि (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        अङ्कः_rhct_nodes++;
    }

    यदि (!rv32 && cpu->cfg.max_satp_बहुलकम् >= VM_1_10_SV39) {
        अङ्कः_rhct_nodes++;
    }

    /* Number of RHCT nodes*/
    build_append_int_noprefix(सारणी_दत्त, अङ्कः_rhct_nodes, 4);

    /* Offset to the RHCT node array */
    build_append_int_noprefix(सारणी_दत्त, RHCT_ग्रन्थिः_ARRAY_विस्थापनम्, 4);

    /* ISA String Node */
    isa_विस्थापनम् = सारणी_दत्त->len - सारणी.सारणी_विस्थापनम्;
    build_append_int_noprefix(सारणी_दत्त, 0, 2);   /* Type 0 */

    isa = riscv_isa_string(cpu);
    len = 8 + वाक्यदैर्घ्यम्(isa) + 1;
    aligned_len = (len % 2) ? (len + 1) : len;

    build_append_int_noprefix(सारणी_दत्त, aligned_len, 2);   /* Length */
    build_append_int_noprefix(सारणी_दत्त, 0x1, 2);           /* Revision */

    /* ISA string length including NUL */
    build_append_int_noprefix(सारणी_दत्त, वाक्यदैर्घ्यम्(isa) + 1, 2);
    g_array_append_vals(सारणी_दत्त, isa, वाक्यदैर्घ्यम्(isa) + 1);   /* ISA string */

    यदि (aligned_len != len) {
        build_append_int_noprefix(सारणी_दत्त, 0x0, 1);   /* Optional Padding */
    }

    /* CMO node */
    यदि (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        cmo_विस्थापनम् = सारणी_दत्त->len - सारणी.सारणी_विस्थापनम्;
        build_append_int_noprefix(सारणी_दत्त, 1, 2);    /* Type */
        build_append_int_noprefix(सारणी_दत्त, 10, 2);   /* Length */
        build_append_int_noprefix(सारणी_दत्त, 0x1, 2);  /* Revision */
        build_append_int_noprefix(सारणी_दत्त, 0, 1);    /* Reserved */

        /* CBOM block size */
        यदि (cpu->cfg.cbom_blocksize) {
            build_append_int_noprefix(सारणी_दत्त,
                                      __builtin_ctz(cpu->cfg.cbom_blocksize),
                                      1);
        } अन्यथा {
            build_append_int_noprefix(सारणी_दत्त, 0, 1);
        }

        /* CBOP block size */
        build_append_int_noprefix(सारणी_दत्त, 0, 1);

        /* CBOZ block size */
        यदि (cpu->cfg.cboz_blocksize) {
            build_append_int_noprefix(सारणी_दत्त,
                                      __builtin_ctz(cpu->cfg.cboz_blocksize),
                                      1);
        } अन्यथा {
            build_append_int_noprefix(सारणी_दत्त, 0, 1);
        }
    }

    /* MMU node structure */
    यदि (!rv32 && cpu->cfg.max_satp_बहुलकम् >= VM_1_10_SV39) {
        mmu_विस्थापनम् = सारणी_दत्त->len - सारणी.सारणी_विस्थापनम्;
        build_append_int_noprefix(सारणी_दत्त, 2, 2);    /* Type */
        build_append_int_noprefix(सारणी_दत्त, 8, 2);    /* Length */
        build_append_int_noprefix(सारणी_दत्त, 0x1, 2);  /* Revision */
        build_append_int_noprefix(सारणी_दत्त, 0, 1);    /* Reserved */
        /* MMU Type */
        यदि (cpu->cfg.max_satp_बहुलकम् == VM_1_10_SV57) {
            build_append_int_noprefix(सारणी_दत्त, 2, 1);    /* Sv57 */
        } अन्यथा यदि (cpu->cfg.max_satp_बहुलकम् == VM_1_10_SV48) {
            build_append_int_noprefix(सारणी_दत्त, 1, 1);    /* Sv48 */
        } अन्यथा यदि (cpu->cfg.max_satp_बहुलकम् == VM_1_10_SV39) {
            build_append_int_noprefix(सारणी_दत्त, 0, 1);    /* Sv39 */
        } अन्यथा {
            g_assert_not_reached();
        }
    }

    /* Hart Info Node */
    प्रति (पूर्णाङ्क i = 0; i < arch_ids->len; i++) {
        len = 16;
        पूर्णाङ्क अङ्कः_offsets = 1;
        build_append_int_noprefix(सारणी_दत्त, 0xFFFF, 2);  /* Type */

        /* Length */
        यदि (cmo_विस्थापनम्) {
            len += 4;
            अङ्कः_offsets++;
        }

        यदि (mmu_विस्थापनम्) {
            len += 4;
            अङ्कः_offsets++;
        }

        build_append_int_noprefix(सारणी_दत्त, len, 2);
        build_append_int_noprefix(सारणी_दत्त, 0x1, 2); /* Revision */
        /* Number of offsets */
        build_append_int_noprefix(सारणी_दत्त, अङ्कः_offsets, 2);
        build_append_int_noprefix(सारणी_दत्त, i, 4);   /* ACPI Processor UID */
        /* Offsets */
        build_append_int_noprefix(सारणी_दत्त, isa_विस्थापनम्, 4);
        यदि (cmo_विस्थापनम्) {
            build_append_int_noprefix(सारणी_दत्त, cmo_विस्थापनम्, 4);
        }

        यदि (mmu_विस्थापनम्) {
            build_append_int_noprefix(सारणी_दत्त, mmu_विस्थापनम्, 4);
        }
    }

    acpi_सारणी_अन्तः(linker, &सारणी);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.9 Fixed ACPI Description Table (MADT)
 */
स्थिर शून्य build_fadt_rev6(GArray *सारणी_दत्त,
                            BIOSLinker *linker,
                            RISCVVirtState *s,
                            अचिह्नित dsdt_tbl_विस्थापनम्)
{
    Acpi_Fadt_दत्त fadt = {
        .rev = 6,
        .minor_ver = 6,
        .flags = 1 << ACPI_FADT_F_HW_REDUCED_ACPI,
        .xdsdt_tbl_विस्थापनम् = &dsdt_tbl_विस्थापनम्,
    };

    build_fadt(सारणी_दत्त, linker, &fadt, s->oem_id, s->oem_सारणी_id);
}

/* DSDT */
स्थिर शून्य build_dsdt(GArray *सारणी_दत्त,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    Aml *scope, *dsdt;
    MachineState *ms = MACHINE(s);
    uint8_t socket_गणनम्;
    ध्रुव Mem_Map_प्रविष्टिः *memmap = s->memmap;
    Acpi_सारणी सारणी = { .sig = "DSDT", .rev = 2, .oem_id = s->oem_id,
                        .oem_सारणी_id = s->oem_सारणी_id };


    acpi_सारणी_begin(&सारणी, सारणी_दत्त);
    dsdt = init_aml_allocator();

    /*
     * When booting the VM with UEFI, UEFI takes ownership of the RTC hardware.
     * While UEFI can use libfdt to disable the RTC device node in the DTB that
     * it passes to the OS, it cannot modify AML. Therefore, we won't generate
     * the RTC ACPI device at all when using UEFI.
     */
    scope = aml_scope("\\_SB");
    acpi_dsdt_सङ्कलय_cpus(scope, s);

    fw_cfg_acpi_dsdt_सङ्कलय(scope, &memmap[VIRT_FW_CFG]);

    socket_गणनम् = riscv_socket_गणनम्(ms);

    यदि (s->aia_type == VIRT_AIA_TYPE_NONE) {
        acpi_dsdt_सङ्कलय_plic_aplic(scope, socket_गणनम्, s->अङ्कः_sources,
                                 memmap[VIRT_PLIC].आधारः,
                                 memmap[VIRT_PLIC].आकार,
                                 "RSCV0001");
    } अन्यथा {
        acpi_dsdt_सङ्कलय_plic_aplic(scope, socket_गणनम्, s->अङ्कः_sources,
                                 memmap[VIRT_APLIC_S].आधारः,
                                 memmap[VIRT_APLIC_S].आकार, "RSCV0002");
    }

    acpi_dsdt_सङ्कलय_uart(scope, &memmap[VIRT_UART0], UART0_IRQ, 0);
    यदि (s->uart1_present) {
        acpi_dsdt_सङ्कलय_uart(scope, &memmap[VIRT_UART1], UART1_IRQ, 1);
    }

    यदि (virt_is_iommu_sys_enabled(s)) {
        acpi_dsdt_सङ्कलय_iommu_sys(scope, &memmap[VIRT_IOMMU_SYS], IOMMU_SYS_IRQ);
    }

    यदि (socket_गणनम् == 1) {
        virtio_acpi_dsdt_सङ्कलय(scope, memmap[VIRT_VIRTIO].आधारः,
                             memmap[VIRT_VIRTIO].आकार,
                             VIRTIO_IRQ, 0, VIRTIO_गणनम्);
        acpi_dsdt_सङ्कलय_gpex_host(scope, PCIE_IRQ);
    } अन्यथा यदि (socket_गणनम् == 2) {
        virtio_acpi_dsdt_सङ्कलय(scope, memmap[VIRT_VIRTIO].आधारः,
                             memmap[VIRT_VIRTIO].आकार,
                             VIRTIO_IRQ + s->अङ्कः_sources, 0,
                             VIRTIO_गणनम्);
        acpi_dsdt_सङ्कलय_gpex_host(scope, PCIE_IRQ + s->अङ्कः_sources);
    } अन्यथा {
        virtio_acpi_dsdt_सङ्कलय(scope, memmap[VIRT_VIRTIO].आधारः,
                             memmap[VIRT_VIRTIO].आकार,
                             VIRTIO_IRQ + s->अङ्कः_sources, 0,
                             VIRTIO_गणनम्);
        acpi_dsdt_सङ्कलय_gpex_host(scope, PCIE_IRQ + s->अङ्कः_sources * 2);
    }

    aml_append(dsdt, scope);

    /* copy AML table into ACPI tables blob and patch header there */
    g_array_append_vals(सारणी_दत्त, dsdt->buf->दत्त, dsdt->buf->len);

    acpi_सारणी_अन्तः(linker, &सारणी);
    free_aml_allocator();
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.12 Multiple APIC Description Table (MADT)
 */
स्थिर शून्य build_madt(GArray *सारणी_दत्त,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ध्रुव CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    uint8_t  गणः_सूचकांक_द्वयङ्काः = imsic_अङ्कः_द्वयङ्काः(riscv_socket_गणनम्(ms));
    uint8_t  guest_सूचकांक_द्वयङ्काः = imsic_अङ्कः_द्वयङ्काः(s->aia_guests + 1);
    uint16_t imsic_max_hart_प्रत्येकम्_socket = 0;
    uint8_t  hart_सूचकांक_द्वयङ्काः;
    uint64_t aplic_addr;
    uint32_t gsi_आधारः;
    uint8_t  socket;

    प्रति (socket = 0; socket < riscv_socket_गणनम्(ms); socket++) {
        यदि (imsic_max_hart_प्रत्येकम्_socket < s->soc[socket].अङ्कः_harts) {
            imsic_max_hart_प्रत्येकम्_socket = s->soc[socket].अङ्कः_harts;
        }
    }

    hart_सूचकांक_द्वयङ्काः = imsic_अङ्कः_द्वयङ्काः(imsic_max_hart_प्रत्येकम्_socket);

    Acpi_सारणी सारणी = { .sig = "APIC", .rev = 7, .oem_id = s->oem_id,
                        .oem_सारणी_id = s->oem_सारणी_id };

    acpi_सारणी_begin(&सारणी, सारणी_दत्त);
    /* Local Interrupt Controller Address */
    build_append_int_noprefix(सारणी_दत्त, 0, 4);
    build_append_int_noprefix(सारणी_दत्त, 0, 4);   /* MADT Flags */

    /* RISC-V Local INTC structures per HART */
    प्रति (पूर्णाङ्क i = 0; i < arch_ids->len; i++) {
        riscv_acpi_madt_सङ्कलय_rintc(i, arch_ids, सारणी_दत्त, s);
    }

    /* IMSIC */
    यदि (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC */
        build_append_int_noprefix(सारणी_दत्त, 0x19, 1);     /* Type */
        build_append_int_noprefix(सारणी_दत्त, 16, 1);       /* Length */
        build_append_int_noprefix(सारणी_दत्त, 1, 1);        /* Version */
        build_append_int_noprefix(सारणी_दत्त, 0, 1);        /* Reserved */
        build_append_int_noprefix(सारणी_दत्त, 0, 4);        /* Flags */
        /* Number of supervisor mode Interrupt Identities */
        build_append_int_noprefix(सारणी_दत्त, VIRT_IRQCHIP_अङ्कः_MSIS, 2);
        /* Number of guest mode Interrupt Identities */
        build_append_int_noprefix(सारणी_दत्त, VIRT_IRQCHIP_अङ्कः_MSIS, 2);
        /* Guest Index Bits */
        build_append_int_noprefix(सारणी_दत्त, guest_सूचकांक_द्वयङ्काः, 1);
        /* Hart Index Bits */
        build_append_int_noprefix(सारणी_दत्त, hart_सूचकांक_द्वयङ्काः, 1);
        /* Group Index Bits */
        build_append_int_noprefix(सारणी_दत्त, गणः_सूचकांक_द्वयङ्काः, 1);
        /* Group Index Shift */
        build_append_int_noprefix(सारणी_दत्त, IMSIC_MMIO_गणः_MIN_सरणम्, 1);
    }

    यदि (s->aia_type != VIRT_AIA_TYPE_NONE) {
        /* APLICs */
        प्रति (socket = 0; socket < riscv_socket_गणनम्(ms); socket++) {
            aplic_addr = s->memmap[VIRT_APLIC_S].आधारः +
                             s->memmap[VIRT_APLIC_S].आकार * socket;
            gsi_आधारः = s->अङ्कः_sources * socket;
            build_append_int_noprefix(सारणी_दत्त, 0x1A, 1);    /* Type */
            build_append_int_noprefix(सारणी_दत्त, 36, 1);      /* Length */
            build_append_int_noprefix(सारणी_दत्त, 1, 1);       /* Version */
            build_append_int_noprefix(सारणी_दत्त, socket, 1);  /* APLIC ID */
            build_append_int_noprefix(सारणी_दत्त, 0, 4);       /* Flags */
            build_append_int_noprefix(सारणी_दत्त, 0, 8);       /* Hardware ID */
            /* Number of IDCs */
            यदि (s->aia_type == VIRT_AIA_TYPE_APLIC) {
                build_append_int_noprefix(सारणी_दत्त,
                                          s->soc[socket].अङ्कः_harts,
                                          2);
            } अन्यथा {
                build_append_int_noprefix(सारणी_दत्त, 0, 2);
            }
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(सारणी_दत्त, VIRT_IRQCHIP_अङ्कः_SOURCES, 2);
            /* Global System Interrupt Base */
            build_append_int_noprefix(सारणी_दत्त, gsi_आधारः, 4);
            /* APLIC Address */
            build_append_int_noprefix(सारणी_दत्त, aplic_addr, 8);
            /* APLIC size */
            build_append_int_noprefix(सारणी_दत्त,
                                      s->memmap[VIRT_APLIC_S].आकार, 4);
        }
    } अन्यथा {
        /* PLICs */
        प्रति (socket = 0; socket < riscv_socket_गणनम्(ms); socket++) {
            aplic_addr = s->memmap[VIRT_PLIC].आधारः +
                         s->memmap[VIRT_PLIC].आकार * socket;
            gsi_आधारः = VIRT_IRQCHIP_अङ्कः_SOURCES * socket;
            build_append_int_noprefix(सारणी_दत्त, 0x1B, 1);   /* Type */
            build_append_int_noprefix(सारणी_दत्त, 36, 1);     /* Length */
            build_append_int_noprefix(सारणी_दत्त, 1, 1);      /* Version */
            build_append_int_noprefix(सारणी_दत्त, socket, 1); /* PLIC ID */
            build_append_int_noprefix(सारणी_दत्त, 0, 8);      /* Hardware ID */
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(सारणी_दत्त,
                                      VIRT_IRQCHIP_अङ्कः_SOURCES - 1, 2);
            build_append_int_noprefix(सारणी_दत्त, 0, 2);     /* Max Priority */
            build_append_int_noprefix(सारणी_दत्त, 0, 4);     /* Flags */
            /* PLIC Size */
            build_append_int_noprefix(सारणी_दत्त, s->memmap[VIRT_PLIC].आकार, 4);
            /* PLIC Address */
            build_append_int_noprefix(सारणी_दत्त, aplic_addr, 8);
            /* Global System Interrupt Vector Base */
            build_append_int_noprefix(सारणी_दत्त, gsi_आधारः, 4);
        }
    }

    acpi_सारणी_अन्तः(linker, &सारणी);
}

#परिभाषय ID_MAPPING_प्रविष्टिः_आकार        20
#परिभाषय IOMMU_प्रविष्टिः_आकार             40
#परिभाषय RISCV_व्यवधानम्_WIRE_OFFSSET 40
#परिभाषय मूलम्_COMPLEX_प्रविष्टिः_आकार      20
#परिभाषय RIMT_ग्रन्थिः_विस्थापनम्             48

/*
 * ID Mapping Structure
 */
स्थिर शून्य build_rimt_id_mapping(GArray *सारणी_दत्त, uint32_t source_id_आधारः,
                                  uint32_t अङ्कः_ids, uint32_t dest_id_आधारः)
{
    /* Source ID Base */
    build_append_int_noprefix(सारणी_दत्त, source_id_आधारः, 4);
    /* Number of IDs */
    build_append_int_noprefix(सारणी_दत्त, अङ्कः_ids, 4);
    /* Destination Device ID Base */
    build_append_int_noprefix(सारणी_दत्त, source_id_आधारः, 4);
    /* Destination IOMMU Offset */
    build_append_int_noprefix(सारणी_दत्त, dest_id_आधारः, 4);
    /* Flags */
    build_append_int_noprefix(सारणी_दत्त, 0, 4);
}

संरचना AcpiRimtIdMapping {
    uint32_t source_id_आधारः;
    uint32_t अङ्कः_ids;
};
प्रकारनिर्देश संरचना AcpiRimtIdMapping AcpiRimtIdMapping;

/* Build the rimt ID mapping to IOMMU for a given PCI host bridge */
स्थिर पूर्णाङ्क rimt_host_bridges(Object *obj, शून्य *opaque)
{
    GArray *idmap_blob = opaque;

    यदि (object_dynamic_cast(obj, TYPE_PCI_HOST_BRIDGE)) {
        PCIBus *bus = PCI_HOST_BRIDGE(obj)->bus;

        यदि (bus && !pci_bus_bypass_iommu(bus)) {
            पूर्णाङ्क min_bus, max_bus;

            pci_bus_range(bus, &min_bus, &max_bus);

            AcpiRimtIdMapping idmap = {
                .source_id_आधारः = min_bus << 8,
                .अङ्कः_ids = (max_bus - min_bus + 1) << 8,
            };
            g_array_append_val(idmap_blob, idmap);
        }
    }

    प्रत्यागम 0;
}

स्थिर पूर्णाङ्क rimt_idmap_compare(gconstpointer a, gconstpointer b)
{
    AcpiRimtIdMapping *idmap_a = (AcpiRimtIdMapping *)a;
    AcpiRimtIdMapping *idmap_b = (AcpiRimtIdMapping *)b;

    प्रत्यागम idmap_a->source_id_आधारः - idmap_b->source_id_आधारः;
}

/*
 * RISC-V IO Mapping Table (RIMT)
 * https://github.com/riscv-non-isa/riscv-acpi-rimt/releases/download/v0.99/rimt-spec.pdf
 */
स्थिर शून्य build_rimt(GArray *सारणी_दत्त, BIOSLinker *linker,
                       RISCVVirtState *s)
{
    पूर्णाङ्क i, nb_nodes, rc_mapping_गणनम्;
    आकार_t ग्रन्थिः_आकार, iommu_विस्थापनम् = 0;
    uint32_t id = 0;
    g_autoptr(GArray) iommu_idmaps = g_array_new(false, true,
                                                 परिमाण(AcpiRimtIdMapping));

    Acpi_सारणी सारणी = { .sig = "RIMT", .rev = 1, .oem_id = s->oem_id,
                        .oem_सारणी_id = s->oem_सारणी_id };

    acpi_सारणी_begin(&सारणी, सारणी_दत्त);

    object_child_foreach_recursive(object_get_मूलम्(),
                                   rimt_host_bridges, iommu_idmaps);

    /* Sort the ID mapping  by Source ID Base*/
    g_array_क्रमय(iommu_idmaps, rimt_idmap_compare);

    nb_nodes = 2; /* RC, IOMMU */
    rc_mapping_गणनम् = iommu_idmaps->len;
    /* Number of RIMT Nodes */
    build_append_int_noprefix(सारणी_दत्त, nb_nodes, 4);

    /* Offset to Array of RIMT Nodes */
    build_append_int_noprefix(सारणी_दत्त, RIMT_ग्रन्थिः_विस्थापनम्, 4);
    build_append_int_noprefix(सारणी_दत्त, 0, 4); /* Reserved */

    iommu_विस्थापनम् = सारणी_दत्त->len - सारणी.सारणी_विस्थापनम्;
    /*  IOMMU Device Structure */
    build_append_int_noprefix(सारणी_दत्त, 0, 1);         /* Type - IOMMU*/
    build_append_int_noprefix(सारणी_दत्त, 1, 1);         /* Revision */
    ग्रन्थिः_आकार =  IOMMU_प्रविष्टिः_आकार;
    build_append_int_noprefix(सारणी_दत्त, ग्रन्थिः_आकार, 2); /* Length */
    build_append_int_noprefix(सारणी_दत्त, 0, 2);         /* Reserved */
    build_append_int_noprefix(सारणी_दत्त, id++, 2);      /* ID */
    यदि (virt_is_iommu_sys_enabled(s)) {
        /* Hardware ID */
        build_append_int_noprefix(सारणी_दत्त, 'R', 1);
        build_append_int_noprefix(सारणी_दत्त, 'S', 1);
        build_append_int_noprefix(सारणी_दत्त, 'C', 1);
        build_append_int_noprefix(सारणी_दत्त, 'V', 1);
        build_append_int_noprefix(सारणी_दत्त, '0', 1);
        build_append_int_noprefix(सारणी_दत्त, '0', 1);
        build_append_int_noprefix(सारणी_दत्त, '0', 1);
        build_append_int_noprefix(सारणी_दत्त, '4', 1);
        /* Base Address */
        build_append_int_noprefix(सारणी_दत्त,
                                  s->memmap[VIRT_IOMMU_SYS].आधारः, 8);
        build_append_int_noprefix(सारणी_दत्त, 0, 4);   /* Flags */
    } अन्यथा {
        /* Hardware ID */
        build_append_int_noprefix(सारणी_दत्त, '0', 1);
        build_append_int_noprefix(सारणी_दत्त, '0', 1);
        build_append_int_noprefix(सारणी_दत्त, '1', 1);
        build_append_int_noprefix(सारणी_दत्त, '0', 1);
        build_append_int_noprefix(सारणी_दत्त, '0', 1);
        build_append_int_noprefix(सारणी_दत्त, '0', 1);
        build_append_int_noprefix(सारणी_दत्त, '1', 1);
        build_append_int_noprefix(सारणी_दत्त, '4', 1);

        build_append_int_noprefix(सारणी_दत्त, 0, 8);   /* Base Address */
        build_append_int_noprefix(सारणी_दत्त, 1, 4);   /* Flags */
    }

    build_append_int_noprefix(सारणी_दत्त, 0, 4);       /* Proximity Domain */
    build_append_int_noprefix(सारणी_दत्त, 0, 2);       /* PCI Segment number */
    /* PCIe B/D/F */
    यदि (virt_is_iommu_sys_enabled(s)) {
        build_append_int_noprefix(सारणी_दत्त, 0, 2);
    } अन्यथा {
        build_append_int_noprefix(सारणी_दत्त, s->pci_iommu_bdf, 2);
    }
    /* Number of interrupt wires */
    build_append_int_noprefix(सारणी_दत्त, 0, 2);
    /* Interrupt wire array offset */
    build_append_int_noprefix(सारणी_दत्त, RISCV_व्यवधानम्_WIRE_OFFSSET, 2);

    /*  PCIe Root Complex Node */
    build_append_int_noprefix(सारणी_दत्त, 1, 1);           /* Type */
    build_append_int_noprefix(सारणी_दत्त, 1, 1);           /* Revision */
    ग्रन्थिः_आकार =  मूलम्_COMPLEX_प्रविष्टिः_आकार +
                 ID_MAPPING_प्रविष्टिः_आकार * rc_mapping_गणनम्;
    build_append_int_noprefix(सारणी_दत्त, ग्रन्थिः_आकार, 2);   /* Length */
    build_append_int_noprefix(सारणी_दत्त, 0, 2);           /* Reserved */
    build_append_int_noprefix(सारणी_दत्त, id++, 2);        /* ID */
    build_append_int_noprefix(सारणी_दत्त, 0, 4);           /* Flags */
    build_append_int_noprefix(सारणी_दत्त, 0, 2);           /* Reserved */
    /* PCI Segment number */
    build_append_int_noprefix(सारणी_दत्त, 0, 2);
    /* ID mapping array offset */
    build_append_int_noprefix(सारणी_दत्त, मूलम्_COMPLEX_प्रविष्टिः_आकार, 2);
    /* Number of ID mappings */
    build_append_int_noprefix(सारणी_दत्त, rc_mapping_गणनम्, 2);

    /* Output Reference */
    AcpiRimtIdMapping *range;

    /* ID mapping array */
    प्रति (i = 0; i < iommu_idmaps->len; i++) {
        range = &g_array_सूचकांक(iommu_idmaps, AcpiRimtIdMapping, i);
        यदि (virt_is_iommu_sys_enabled(s)) {
            range->source_id_आधारः = 0;
            range->अङ्कः_ids = 0x10000;
        } अन्यथा {
            range->source_id_आधारः = s->pci_iommu_bdf + 1;
            range->अङ्कः_ids = 0xffff - s->pci_iommu_bdf;
        }
        build_rimt_id_mapping(सारणी_दत्त, range->source_id_आधारः,
                              range->अङ्कः_ids, iommu_विस्थापनम्);
    }

    acpi_सारणी_अन्तः(linker, &सारणी);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.16 System Resource Affinity Table (SRAT)
 */
स्थिर शून्य
build_srat(GArray *सारणी_दत्त, BIOSLinker *linker, RISCVVirtState *vms)
{
    पूर्णाङ्क i;
    uint64_t mem_आधारः;
    MachineClass *mc = MACHINE_GET_CLASS(vms);
    MachineState *ms = MACHINE(vms);
    ध्रुव CPUArchIdList *cpu_list = mc->possible_cpu_arch_ids(ms);
    Acpi_सारणी सारणी = { .sig = "SRAT", .rev = 3, .oem_id = vms->oem_id,
                        .oem_सारणी_id = vms->oem_सारणी_id };

    acpi_सारणी_begin(&सारणी, सारणी_दत्त);
    build_append_int_noprefix(सारणी_दत्त, 1, 4); /* Reserved */
    build_append_int_noprefix(सारणी_दत्त, 0, 8); /* Reserved */

    प्रति (i = 0; i < cpu_list->len; ++i) {
        uint32_t nodeid = cpu_list->cpus[i].props.ग्रन्थिः_id;
        /*
         * 5.2.16.8 RINTC Affinity Structure
         */
        build_append_int_noprefix(सारणी_दत्त, 7, 1);      /* Type */
        build_append_int_noprefix(सारणी_दत्त, 20, 1);     /* Length */
        build_append_int_noprefix(सारणी_दत्त, 0, 2);        /* Reserved */
        build_append_int_noprefix(सारणी_दत्त, nodeid, 4); /* Proximity Domain */
        build_append_int_noprefix(सारणी_दत्त, i, 4); /* ACPI Processor UID */
        /* Flags, Table 5-70 */
        build_append_int_noprefix(सारणी_दत्त, 1 /* Flags: Enabled */, 4);
        build_append_int_noprefix(सारणी_दत्त, 0, 4); /* Clock Domain */
    }

    mem_आधारः = vms->memmap[VIRT_DRAM].आधारः;
    प्रति (i = 0; i < ms->numa_state->अङ्कः_nodes; ++i) {
        यदि (ms->numa_state->nodes[i].ग्रन्थिः_mem > 0) {
            build_srat_स्मृतिः(सारणी_दत्त, mem_आधारः,
                              ms->numa_state->nodes[i].ग्रन्थिः_mem, i,
                              MEM_AFFINITY_ENABLED);
            mem_आधारः += ms->numa_state->nodes[i].ग्रन्थिः_mem;
        }
    }

    acpi_सारणी_अन्तः(linker, &सारणी);
}

स्थिर शून्य virt_acpi_build(RISCVVirtState *s, AcpiBuildTables *tables)
{
    GArray *सारणी_offsets;
    अचिह्नित dsdt, xsdt;
    GArray *tables_blob = tables->सारणी_दत्त;
    MachineState *ms = MACHINE(s);

    सारणी_offsets = g_array_new(false, true,
                                परिमाण(uint32_t));

    bios_linker_loader_alloc(tables->linker,
                             ACPI_BUILD_सारणी_FILE, tables_blob,
                             64, false);

    /* DSDT is pointed to by FADT */
    dsdt = tables_blob->len;
    build_dsdt(tables_blob, tables->linker, s);

    /* FADT and others pointed to by XSDT */
    acpi_सङ्कलय_सारणी(सारणी_offsets, tables_blob);
    build_fadt_rev6(tables_blob, tables->linker, s, dsdt);

    acpi_सङ्कलय_सारणी(सारणी_offsets, tables_blob);
    build_madt(tables_blob, tables->linker, s);

    acpi_सङ्कलय_सारणी(सारणी_offsets, tables_blob);
    build_rhct(tables_blob, tables->linker, s);

    यदि (virt_is_iommu_sys_enabled(s) || s->pci_iommu_bdf) {
        acpi_सङ्कलय_सारणी(सारणी_offsets, tables_blob);
        build_rimt(tables_blob, tables->linker, s);
    }

    acpi_सङ्कलय_सारणी(सारणी_offsets, tables_blob);

    यदि (ms->acpi_spcr_enabled) {
        spcr_सज्जता(tables_blob, tables->linker, s);
    }

    acpi_सङ्कलय_सारणी(सारणी_offsets, tables_blob);
    {
        AcpiMcfgInfo mcfg = {
           .आधारः = s->memmap[VIRT_PCIE_ECAM].आधारः,
           .आकार = s->memmap[VIRT_PCIE_ECAM].आकार,
        };
        build_mcfg(tables_blob, tables->linker, &mcfg, s->oem_id,
                   s->oem_सारणी_id);
    }

    यदि (ms->numa_state->अङ्कः_nodes > 0) {
        acpi_सङ्कलय_सारणी(सारणी_offsets, tables_blob);
        build_srat(tables_blob, tables->linker, s);
        यदि (ms->numa_state->have_numa_distance) {
            acpi_सङ्कलय_सारणी(सारणी_offsets, tables_blob);
            build_slit(tables_blob, tables->linker, ms, s->oem_id,
                       s->oem_सारणी_id);
        }
    }

    /* XSDT is pointed to by RSDP */
    xsdt = tables_blob->len;
    build_xsdt(tables_blob, tables->linker, सारणी_offsets, s->oem_id,
                s->oem_सारणी_id);

    /* RSDP is in FSEG memory, so allocate it separately */
    {
        Acpi_Rsdp_दत्त rsdp_दत्त = {
            .revision = 2,
            .oem_id = s->oem_id,
            .xsdt_tbl_विस्थापनम् = &xsdt,
            .rsdt_tbl_विस्थापनम् = रिक्त,
        };
        build_rsdp(tables->rsdp, tables->linker, &rsdp_दत्त);
    }

    /*
     * The align size is 128, warn if 64k is not enough therefore
     * the align size could be resized.
     */
    यदि (tables_blob->len > ACPI_BUILD_सारणी_आकार / 2) {
        warn_report("ACPI table size %u exceeds %d bytes,"
                    " migration may not work",
                    tables_blob->len, ACPI_BUILD_सारणी_आकार / 2);
        error_printf("Try removing some objects.");
    }

    acpi_align_आकार(tables_blob, ACPI_BUILD_सारणी_आकार);

    /* Clean up memory that's no longer used */
    g_array_free(सारणी_offsets, true);
}

स्थिर शून्य acpi_ram_update(स्मृतिः_Region *mr, GArray *दत्त)
{
    uint32_t आकार = acpi_दत्त_len(दत्त);

    /*
     * Make sure RAM size is correct - in case it got changed
     * e.g. by migration
     */
    स्मृतिः_region_ram_resize(mr, आकार, &error_abort);

    स्मृतिं_प्रतिलिख(स्मृतिः_region_get_ram_ptr(mr), दत्त->दत्त, आकार);
    स्मृतिः_region_समुच्चयः_dirty(mr, 0, आकार);
}

स्थिर शून्य virt_acpi_build_update(शून्य *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    AcpiBuildTables tables;

    /* No state to update or already patched? Nothing to do. */
    यदि (!build_state || build_state->patched) {
        प्रत्यागम;
    }

    build_state->patched = true;

    acpi_build_tables_init(&tables);

    virt_acpi_build(RISCV_VIRT_MACHINE(qdev_get_machine()), &tables);

    acpi_ram_update(build_state->सारणी_mr, tables.सारणी_दत्त);
    acpi_ram_update(build_state->rsdp_mr, tables.rsdp);
    acpi_ram_update(build_state->linker_mr, tables.linker->cmd_blob);

    acpi_build_tables_cleanup(&tables, true);
}

स्थिर शून्य virt_acpi_build_reset(शून्य *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    build_state->patched = false;
}

स्थिर ध्रुव VMStateDescription vmstate_virt_acpi_build = {
    .name = "virt_acpi_build",
    .संस्करणम्_id = 1,
    .minimum_संस्करणम्_id = 1,
    .fields = (ध्रुव VMState_क्षेत्रम्[]) {
        VMSTATE_BOOL(patched, AcpiBuildState),
        VMSTATE_अन्तः_सम्बन्धि_LIST()
    },
};

शून्य virt_acpi_सज्जता(RISCVVirtState *s)
{
    AcpiBuildTables tables;
    AcpiBuildState *build_state;

    build_state = g_malloc0(परिमाण *build_state);

    acpi_build_tables_init(&tables);
    virt_acpi_build(s, &tables);

    /* Now expose it all to Guest */
    build_state->सारणी_mr = acpi_सङ्कलय_rom_blob(virt_acpi_build_update,
                                              build_state, tables.सारणी_दत्त,
                                              ACPI_BUILD_सारणी_FILE);
    प्रतिज्ञा(build_state->सारणी_mr != रिक्त);

    build_state->linker_mr = acpi_सङ्कलय_rom_blob(virt_acpi_build_update,
                                               build_state,
                                               tables.linker->cmd_blob,
                                               ACPI_BUILD_LOADER_FILE);

    build_state->rsdp_mr = acpi_सङ्कलय_rom_blob(virt_acpi_build_update,
                                             build_state, tables.rsdp,
                                             ACPI_BUILD_RSDP_FILE);

    qemu_पञ्जिकाकोशः_reset(virt_acpi_build_reset, build_state);
    virt_acpi_build_reset(build_state);
    vmstate_पञ्जिकाकोशः(रिक्त, 0, &vmstate_virt_acpi_build, build_state);

    /*
     * Clean up tables but don't free the memory: we track it
     * in build_state.
     */
    acpi_build_tables_cleanup(&tables, false);
}
