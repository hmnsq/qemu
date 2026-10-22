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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "hw/acpi/acpi-defs.h"
#అన్తర్భావ "hw/acpi/acpi.h"
#అన్తర్భావ "hw/acpi/aml-build.h"
#అన్తర్భావ "hw/acpi/pci.h"
#అన్తర్భావ "hw/acpi/utils.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/nvram/fw_cfg_acpi.h"
#అన్తర్భావ "hw/pci-host/gpex.h"
#అన్తర్భావ "hw/riscv/virt.h"
#అన్తర్భావ "hw/riscv/numa.h"
#అన్తర్భావ "hw/virtio/virtio-acpi.h"
#అన్తర్భావ "kvm/kvm_riscv.h"
#అన్తర్భావ "migration/vmstate.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "system/kvm.h"
#అన్తర్భావ "system/reset.h"

#అన్తర్భావ "aia.h"

#నిర్వచించు ACPI_BUILD_పట్టిక_కొలత             0x20000
#నిర్వచించు ACPI_BUILD_INTC_ID(socket, సూచీ) ((socket << 24) | (సూచీ))

రకనిర్వచనం నిర్మాణం AcpiBuildState {
    /* Copy of table in RAM (for patching) */
    జ్ఞాపకం_Region *పట్టిక_mr;
    జ్ఞాపకం_Region *rsdp_mr;
    జ్ఞాపకం_Region *linker_mr;
    /* Is table patched? */
    bool patched;
} AcpiBuildState;

స్థిర శూన్య acpi_align_కొలత(GArray *blob, గుర్తులేని align)
{
    /*
     * Align size to multiple of given size. This reduces the chance
     * we need to change size in the future (breaking cross version migration).
     */
    g_array_సమితి_కొలత(blob, విడత_పైకి(acpi_దత్తాంశం_len(blob), align));
}

స్థిర శూన్య riscv_acpi_madt_కూడు_rintc(uint32_t uid,
                                      స్థిరరాశి CPUArchIdList *arch_ids,
                                      GArray *నమోదు_2,
                                      RISCVVirtState *s)
{
    uint8_t  guest_సూచీ_బిట్లు = imsic_అంకె_బిట్లు(s->aia_guests + 1);
    uint64_t hart_id = arch_ids->cpus[uid].arch_id;
    uint32_t imsic_కొలత, local_cpu_id, socket_id;
    uint64_t imsic_socket_addr, imsic_addr;
    MachineState *ms = MACHINE(s);

    socket_id = arch_ids->cpus[uid].props.కణుపు_id;
    local_cpu_id = (arch_ids->cpus[uid].arch_id -
                    riscv_socket_మొదటి_hartid(ms, socket_id)) %
                    riscv_socket_hart_లెక్క(ms, socket_id);
    imsic_socket_addr = s->memmap[VIRT_IMSIC_S].పునాది +
                        (socket_id * VIRT_IMSIC_సమూహం_MAX_కొలత);
    imsic_కొలత = IMSIC_HART_కొలత(guest_సూచీ_బిట్లు);
    imsic_addr = imsic_socket_addr + local_cpu_id * imsic_కొలత;
    build_append_int_noprefix(నమోదు_2, 0x18, 1);       /* Type     */
    build_append_int_noprefix(నమోదు_2, 36, 1);         /*నిడివి*/
    build_append_int_noprefix(నమోదు_2, 1, 1);          /*సంస్కరణ*/
    build_append_int_noprefix(నమోదు_2, 0, 1);          /* Reserved */
    build_append_int_noprefix(నమోదు_2, 0x1, 4);        /* Flags    */
    build_append_int_noprefix(నమోదు_2, hart_id, 8);    /* Hart ID  */
    build_append_int_noprefix(నమోదు_2, uid, 4);        /* ACPI Processor UID */
    /* External Interrupt Controller ID */
    ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_APLIC) {
        build_append_int_noprefix(నమోదు_2,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.కణుపు_id,
                                      local_cpu_id),
                                  4);
    } లేకపోతే ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_NONE) {
        build_append_int_noprefix(నమోదు_2,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.కణుపు_id,
                                      kvm_enabled() ?
                                      local_cpu_id :
                                      2 * local_cpu_id + 1),
                                  4);
    } లేకపోతే {
        build_append_int_noprefix(నమోదు_2, 0, 4);
    }

    ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC Base address */
        build_append_int_noprefix(నమోదు_2, imsic_addr, 8);
        /* IMSIC Size */
        build_append_int_noprefix(నమోదు_2, imsic_కొలత, 4);
    } లేకపోతే {
        build_append_int_noprefix(నమోదు_2, 0, 8);
        build_append_int_noprefix(నమోదు_2, 0, 4);
    }
}

స్థిర శూన్య acpi_dsdt_కూడు_cpus(Aml *scope, RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    స్థిరరాశి CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);

    కొరకు (పూర్ణాంకం i = 0; i < arch_ids->len; i++) {
            Aml *dev;
            GArray *madt_buf = g_array_new(0, 1, 1);

            dev = aml_device("C%.03X", i);
            aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0007")));
            aml_append(dev, aml_name_decl("_UID",
                       aml_int(arch_ids->cpus[i].arch_id)));

            /* build _MAT object */
            riscv_acpi_madt_కూడు_rintc(i, arch_ids, madt_buf, s);
            aml_append(dev, aml_name_decl("_MAT",
                                          aml_బఫరు(madt_buf->len,
                                          (uint8_t *)madt_buf->దత్తాంశం)));
            g_array_free(madt_buf, true);

            aml_append(scope, dev);
    }
}

స్థిర శూన్య acpi_dsdt_కూడు_plic_aplic(Aml *scope, uint8_t socket_లెక్క,
                                     uint16_t అంకె_sources,
                                     uint64_t mmio_పునాది, uint64_t mmio_కొలత,
                                     స్థిరరాశి అక్షరం *hid)
{
    uint64_t plic_aplic_addr;
    uint32_t gsi_పునాది;
    uint8_t  socket;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(అంకె_sources <= 1023);

    కొరకు (socket = 0; socket < socket_లెక్క; socket++) {
        plic_aplic_addr = mmio_పునాది + mmio_కొలత * socket;
        gsi_పునాది = అంకె_sources * socket;
        Aml *dev = aml_device("IC%.02X", socket);
        aml_append(dev, aml_name_decl("_HID", aml_string("%s", hid)));
        aml_append(dev, aml_name_decl("_UID", aml_int(socket)));
        aml_append(dev, aml_name_decl("_GSB", aml_int(gsi_పునాది)));

        Aml *crs = aml_resource_template();
        aml_append(crs, aml_memory32_fixed(plic_aplic_addr, mmio_కొలత,
                                           AML_READ_WRITE));
        aml_append(dev, aml_name_decl("_CRS", crs));
        aml_append(scope, dev);
    }
}

స్థిర శూన్య
acpi_dsdt_కూడు_uart(Aml *scope, స్థిరరాశి Mem_Map_నమోదు *uart_memmap,
                    uint32_t uart_irq, పూర్ణాంకం uartidx)
{
    Aml *dev = aml_device("COM%d", uartidx);
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0003")));
    aml_append(dev, aml_name_decl("_UID", aml_int(uartidx)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(uart_memmap->పునాది,
                                         uart_memmap->కొలత, AML_READ_WRITE));
    aml_append(crs,
                aml_అంతరాయం(AML_వినియోగదారు, AML_LEVEL, AML_చురుకైన_ఎగువ,
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
స్థిర శూన్య acpi_dsdt_కూడు_iommu_sys(Aml *scope, స్థిరరాశి Mem_Map_నమోదు *iommu_memmap,
                                    uint32_t iommu_irq)
{
    uint32_t i;

    Aml *dev = aml_device("IMU0");
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0004")));
    aml_append(dev, aml_name_decl("_UID", aml_int(0)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(iommu_memmap->పునాది,
                                       iommu_memmap->కొలత, AML_READ_WRITE));
    కొరకు (i = iommu_irq; i < iommu_irq + 4; i++) {
        aml_append(crs, aml_అంతరాయం(AML_వినియోగదారు, AML_అంచు, AML_చురుకైన_దిగువ,
                                      AML_EXCLUSIVE, &i, 1));
    }

    aml_append(dev, aml_name_decl("_CRS", crs));
    aml_append(scope, dev);
}

/*
 * Serial Port Console Redirection Table (SPCR)
 * Rev: 1.10
 */

స్థిర శూన్య
spcr_అమరిక(GArray *పట్టిక_దత్తాంశం, BIOSLinker *linker, RISCVVirtState *s)
{
    స్థిరరాశి అక్షరం name[] = ".";
    Acpi_Spcr_దత్తాంశం serial = {
        .interface_type = 0x12,       /* 16550 compatible */
        .పునాది_addr.id = AML_AS_SYSTEM_జ్ఞాపకం,
        .పునాది_addr.వెడల్పు = 8,
        .పునాది_addr.స్థానభ్రంశం = 0,
        .పునాది_addr.కొలత = 1,
        .పునాది_addr.addr = s->memmap[VIRT_UART0].పునాది,
        .అంతరాయం_type = (1 << 4),/* Bit[4] RISC-V PLIC/APLIC */
        .pc_అంతరాయం = 0,
        .అంతరాయం = UART0_IRQ,
        .baud_రేటు = 7,            /* 15200 */
        .parity = 0,
        .stop_బిట్లు = 1,
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
        .namespace_string_నిడివి = పరిమాణం(name),
        .namespace_string_స్థానభ్రంశం = 88,
    };

    build_spcr(పట్టిక_దత్తాంశం, linker, &serial, 4, s->oem_id, s->oem_పట్టిక_id,
               name);
}

/* RHCT Node[N] starts at offset 56 */
#నిర్వచించు RHCT_కణుపు_ARRAY_స్థానభ్రంశం 56

/*
 * ACPI spec, Revision 6.6
 * 5.2.37 RISC-V Hart Capabilities Table (RHCT)
 */
స్థిర శూన్య build_rhct(GArray *పట్టిక_దత్తాంశం,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    స్థిరరాశి CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    కొలత_t len, aligned_len;
    uint32_t isa_స్థానభ్రంశం, అంకె_rhct_nodes, cmo_స్థానభ్రంశం = 0;
    RISCVCPU *cpu = &s->soc[0].harts[0];
    uint32_t mmu_స్థానభ్రంశం = 0;
    bool rv32 = riscv_cpu_is_32bit(cpu);
    g_autofree అక్షరం *isa = ఖాళీ;

    Acpi_పట్టిక పట్టిక = { .sig = "RHCT", .rev = 1, .oem_id = s->oem_id,
                        .oem_పట్టిక_id = s->oem_పట్టిక_id };

    acpi_పట్టిక_begin(&పట్టిక, పట్టిక_దత్తాంశం);

    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x0, 4);   /* Reserved */

    /* Time Base Frequency */
    build_append_int_noprefix(పట్టిక_దత్తాంశం,
                              kvm_enabled() ?
                              kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                              RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ,
                              8);

    /* ISA + N hart info */
    అంకె_rhct_nodes = 1 + ms->smp.cpus;
    ఒకవేళ (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        అంకె_rhct_nodes++;
    }

    ఒకవేళ (!rv32 && cpu->cfg.max_satp_బహుళకం >= VM_1_10_SV39) {
        అంకె_rhct_nodes++;
    }

    /* Number of RHCT nodes*/
    build_append_int_noprefix(పట్టిక_దత్తాంశం, అంకె_rhct_nodes, 4);

    /* Offset to the RHCT node array */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, RHCT_కణుపు_ARRAY_స్థానభ్రంశం, 4);

    /* ISA String Node */
    isa_స్థానభ్రంశం = పట్టిక_దత్తాంశం->len - పట్టిక.పట్టిక_స్థానభ్రంశం;
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);   /* Type 0 */

    isa = riscv_isa_string(cpu);
    len = 8 + అక్షరశ్రేణి_పొడవు(isa) + 1;
    aligned_len = (len % 2) ? (len + 1) : len;

    build_append_int_noprefix(పట్టిక_దత్తాంశం, aligned_len, 2);   /* Length */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x1, 2);           /* Revision */

    /* ISA string length including NUL */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, అక్షరశ్రేణి_పొడవు(isa) + 1, 2);
    g_array_append_vals(పట్టిక_దత్తాంశం, isa, అక్షరశ్రేణి_పొడవు(isa) + 1);   /* ISA string */

    ఒకవేళ (aligned_len != len) {
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x0, 1);   /* Optional Padding */
    }

    /* CMO node */
    ఒకవేళ (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        cmo_స్థానభ్రంశం = పట్టిక_దత్తాంశం->len - పట్టిక.పట్టిక_స్థానభ్రంశం;
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 2);    /* Type */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 10, 2);   /* Length */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x1, 2);  /* Revision */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 1);    /* Reserved */

        /* CBOM block size */
        ఒకవేళ (cpu->cfg.cbom_blocksize) {
            build_append_int_noprefix(పట్టిక_దత్తాంశం,
                                      __builtin_ctz(cpu->cfg.cbom_blocksize),
                                      1);
        } లేకపోతే {
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 1);
        }

        /* CBOP block size */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 1);

        /* CBOZ block size */
        ఒకవేళ (cpu->cfg.cboz_blocksize) {
            build_append_int_noprefix(పట్టిక_దత్తాంశం,
                                      __builtin_ctz(cpu->cfg.cboz_blocksize),
                                      1);
        } లేకపోతే {
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 1);
        }
    }

    /* MMU node structure */
    ఒకవేళ (!rv32 && cpu->cfg.max_satp_బహుళకం >= VM_1_10_SV39) {
        mmu_స్థానభ్రంశం = పట్టిక_దత్తాంశం->len - పట్టిక.పట్టిక_స్థానభ్రంశం;
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 2, 2);    /* Type */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 8, 2);    /* Length */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x1, 2);  /* Revision */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 1);    /* Reserved */
        /* MMU Type */
        ఒకవేళ (cpu->cfg.max_satp_బహుళకం == VM_1_10_SV57) {
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 2, 1);    /* Sv57 */
        } లేకపోతే ఒకవేళ (cpu->cfg.max_satp_బహుళకం == VM_1_10_SV48) {
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 1);    /* Sv48 */
        } లేకపోతే ఒకవేళ (cpu->cfg.max_satp_బహుళకం == VM_1_10_SV39) {
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 1);    /* Sv39 */
        } లేకపోతే {
            g_assert_not_reached();
        }
    }

    /* Hart Info Node */
    కొరకు (పూర్ణాంకం i = 0; i < arch_ids->len; i++) {
        len = 16;
        పూర్ణాంకం అంకె_offsets = 1;
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0xFFFF, 2);  /* Type */

        /* Length */
        ఒకవేళ (cmo_స్థానభ్రంశం) {
            len += 4;
            అంకె_offsets++;
        }

        ఒకవేళ (mmu_స్థానభ్రంశం) {
            len += 4;
            అంకె_offsets++;
        }

        build_append_int_noprefix(పట్టిక_దత్తాంశం, len, 2);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x1, 2); /* Revision */
        /* Number of offsets */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, అంకె_offsets, 2);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, i, 4);   /* ACPI Processor UID */
        /* Offsets */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, isa_స్థానభ్రంశం, 4);
        ఒకవేళ (cmo_స్థానభ్రంశం) {
            build_append_int_noprefix(పట్టిక_దత్తాంశం, cmo_స్థానభ్రంశం, 4);
        }

        ఒకవేళ (mmu_స్థానభ్రంశం) {
            build_append_int_noprefix(పట్టిక_దత్తాంశం, mmu_స్థానభ్రంశం, 4);
        }
    }

    acpi_పట్టిక_చివర(linker, &పట్టిక);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.9 Fixed ACPI Description Table (MADT)
 */
స్థిర శూన్య build_fadt_rev6(GArray *పట్టిక_దత్తాంశం,
                            BIOSLinker *linker,
                            RISCVVirtState *s,
                            గుర్తులేని dsdt_tbl_స్థానభ్రంశం)
{
    Acpi_Fadt_దత్తాంశం fadt = {
        .rev = 6,
        .minor_ver = 6,
        .flags = 1 << ACPI_FADT_F_HW_REDUCED_ACPI,
        .xdsdt_tbl_స్థానభ్రంశం = &dsdt_tbl_స్థానభ్రంశం,
    };

    build_fadt(పట్టిక_దత్తాంశం, linker, &fadt, s->oem_id, s->oem_పట్టిక_id);
}

/* DSDT */
స్థిర శూన్య build_dsdt(GArray *పట్టిక_దత్తాంశం,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    Aml *scope, *dsdt;
    MachineState *ms = MACHINE(s);
    uint8_t socket_లెక్క;
    స్థిరరాశి Mem_Map_నమోదు *memmap = s->memmap;
    Acpi_పట్టిక పట్టిక = { .sig = "DSDT", .rev = 2, .oem_id = s->oem_id,
                        .oem_పట్టిక_id = s->oem_పట్టిక_id };


    acpi_పట్టిక_begin(&పట్టిక, పట్టిక_దత్తాంశం);
    dsdt = init_aml_allocator();

    /*
     * When booting the VM with UEFI, UEFI takes ownership of the RTC hardware.
     * While UEFI can use libfdt to disable the RTC device node in the DTB that
     * it passes to the OS, it cannot modify AML. Therefore, we won't generate
     * the RTC ACPI device at all when using UEFI.
     */
    scope = aml_scope("\\_SB");
    acpi_dsdt_కూడు_cpus(scope, s);

    fw_cfg_acpi_dsdt_కూడు(scope, &memmap[VIRT_FW_CFG]);

    socket_లెక్క = riscv_socket_లెక్క(ms);

    ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_NONE) {
        acpi_dsdt_కూడు_plic_aplic(scope, socket_లెక్క, s->అంకె_sources,
                                 memmap[VIRT_PLIC].పునాది,
                                 memmap[VIRT_PLIC].కొలత,
                                 "RSCV0001");
    } లేకపోతే {
        acpi_dsdt_కూడు_plic_aplic(scope, socket_లెక్క, s->అంకె_sources,
                                 memmap[VIRT_APLIC_S].పునాది,
                                 memmap[VIRT_APLIC_S].కొలత, "RSCV0002");
    }

    acpi_dsdt_కూడు_uart(scope, &memmap[VIRT_UART0], UART0_IRQ, 0);
    ఒకవేళ (s->uart1_present) {
        acpi_dsdt_కూడు_uart(scope, &memmap[VIRT_UART1], UART1_IRQ, 1);
    }

    ఒకవేళ (virt_is_iommu_sys_enabled(s)) {
        acpi_dsdt_కూడు_iommu_sys(scope, &memmap[VIRT_IOMMU_SYS], IOMMU_SYS_IRQ);
    }

    ఒకవేళ (socket_లెక్క == 1) {
        virtio_acpi_dsdt_కూడు(scope, memmap[VIRT_VIRTIO].పునాది,
                             memmap[VIRT_VIRTIO].కొలత,
                             VIRTIO_IRQ, 0, VIRTIO_లెక్క);
        acpi_dsdt_కూడు_gpex_host(scope, PCIE_IRQ);
    } లేకపోతే ఒకవేళ (socket_లెక్క == 2) {
        virtio_acpi_dsdt_కూడు(scope, memmap[VIRT_VIRTIO].పునాది,
                             memmap[VIRT_VIRTIO].కొలత,
                             VIRTIO_IRQ + s->అంకె_sources, 0,
                             VIRTIO_లెక్క);
        acpi_dsdt_కూడు_gpex_host(scope, PCIE_IRQ + s->అంకె_sources);
    } లేకపోతే {
        virtio_acpi_dsdt_కూడు(scope, memmap[VIRT_VIRTIO].పునాది,
                             memmap[VIRT_VIRTIO].కొలత,
                             VIRTIO_IRQ + s->అంకె_sources, 0,
                             VIRTIO_లెక్క);
        acpi_dsdt_కూడు_gpex_host(scope, PCIE_IRQ + s->అంకె_sources * 2);
    }

    aml_append(dsdt, scope);

    /* copy AML table into ACPI tables blob and patch header there */
    g_array_append_vals(పట్టిక_దత్తాంశం, dsdt->buf->దత్తాంశం, dsdt->buf->len);

    acpi_పట్టిక_చివర(linker, &పట్టిక);
    free_aml_allocator();
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.12 Multiple APIC Description Table (MADT)
 */
స్థిర శూన్య build_madt(GArray *పట్టిక_దత్తాంశం,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    స్థిరరాశి CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    uint8_t  సమూహం_సూచీ_బిట్లు = imsic_అంకె_బిట్లు(riscv_socket_లెక్క(ms));
    uint8_t  guest_సూచీ_బిట్లు = imsic_అంకె_బిట్లు(s->aia_guests + 1);
    uint16_t imsic_max_hart_చొప్పున_socket = 0;
    uint8_t  hart_సూచీ_బిట్లు;
    uint64_t aplic_addr;
    uint32_t gsi_పునాది;
    uint8_t  socket;

    కొరకు (socket = 0; socket < riscv_socket_లెక్క(ms); socket++) {
        ఒకవేళ (imsic_max_hart_చొప్పున_socket < s->soc[socket].అంకె_harts) {
            imsic_max_hart_చొప్పున_socket = s->soc[socket].అంకె_harts;
        }
    }

    hart_సూచీ_బిట్లు = imsic_అంకె_బిట్లు(imsic_max_hart_చొప్పున_socket);

    Acpi_పట్టిక పట్టిక = { .sig = "APIC", .rev = 7, .oem_id = s->oem_id,
                        .oem_పట్టిక_id = s->oem_పట్టిక_id };

    acpi_పట్టిక_begin(&పట్టిక, పట్టిక_దత్తాంశం);
    /* Local Interrupt Controller Address */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);   /* MADT Flags */

    /* RISC-V Local INTC structures per HART */
    కొరకు (పూర్ణాంకం i = 0; i < arch_ids->len; i++) {
        riscv_acpi_madt_కూడు_rintc(i, arch_ids, పట్టిక_దత్తాంశం, s);
    }

    /* IMSIC */
    ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x19, 1);     /* Type */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 16, 1);       /* Length */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 1);        /* Version */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 1);        /* Reserved */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);        /* Flags */
        /* Number of supervisor mode Interrupt Identities */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, VIRT_IRQCHIP_అంకె_MSIS, 2);
        /* Number of guest mode Interrupt Identities */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, VIRT_IRQCHIP_అంకె_MSIS, 2);
        /* Guest Index Bits */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, guest_సూచీ_బిట్లు, 1);
        /* Hart Index Bits */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, hart_సూచీ_బిట్లు, 1);
        /* Group Index Bits */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, సమూహం_సూచీ_బిట్లు, 1);
        /* Group Index Shift */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, IMSIC_MMIO_సమూహం_MIN_జరుపు, 1);
    }

    ఒకవేళ (s->aia_type != VIRT_AIA_TYPE_NONE) {
        /* APLICs */
        కొరకు (socket = 0; socket < riscv_socket_లెక్క(ms); socket++) {
            aplic_addr = s->memmap[VIRT_APLIC_S].పునాది +
                             s->memmap[VIRT_APLIC_S].కొలత * socket;
            gsi_పునాది = s->అంకె_sources * socket;
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x1A, 1);    /* Type */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 36, 1);      /* Length */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 1);       /* Version */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, socket, 1);  /* APLIC ID */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);       /* Flags */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 8);       /* Hardware ID */
            /* Number of IDCs */
            ఒకవేళ (s->aia_type == VIRT_AIA_TYPE_APLIC) {
                build_append_int_noprefix(పట్టిక_దత్తాంశం,
                                          s->soc[socket].అంకె_harts,
                                          2);
            } లేకపోతే {
                build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);
            }
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, VIRT_IRQCHIP_అంకె_SOURCES, 2);
            /* Global System Interrupt Base */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, gsi_పునాది, 4);
            /* APLIC Address */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, aplic_addr, 8);
            /* APLIC size */
            build_append_int_noprefix(పట్టిక_దత్తాంశం,
                                      s->memmap[VIRT_APLIC_S].కొలత, 4);
        }
    } లేకపోతే {
        /* PLICs */
        కొరకు (socket = 0; socket < riscv_socket_లెక్క(ms); socket++) {
            aplic_addr = s->memmap[VIRT_PLIC].పునాది +
                         s->memmap[VIRT_PLIC].కొలత * socket;
            gsi_పునాది = VIRT_IRQCHIP_అంకె_SOURCES * socket;
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0x1B, 1);   /* Type */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 36, 1);     /* Length */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 1);      /* Version */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, socket, 1); /* PLIC ID */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 8);      /* Hardware ID */
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(పట్టిక_దత్తాంశం,
                                      VIRT_IRQCHIP_అంకె_SOURCES - 1, 2);
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);     /* Max Priority */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);     /* Flags */
            /* PLIC Size */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, s->memmap[VIRT_PLIC].కొలత, 4);
            /* PLIC Address */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, aplic_addr, 8);
            /* Global System Interrupt Vector Base */
            build_append_int_noprefix(పట్టిక_దత్తాంశం, gsi_పునాది, 4);
        }
    }

    acpi_పట్టిక_చివర(linker, &పట్టిక);
}

#నిర్వచించు ID_MAPPING_నమోదు_కొలత        20
#నిర్వచించు IOMMU_నమోదు_కొలత             40
#నిర్వచించు RISCV_అంతరాయం_WIRE_OFFSSET 40
#నిర్వచించు మూలం_COMPLEX_నమోదు_కొలత      20
#నిర్వచించు RIMT_కణుపు_స్థానభ్రంశం             48

/*
 * ID Mapping Structure
 */
స్థిర శూన్య build_rimt_id_mapping(GArray *పట్టిక_దత్తాంశం, uint32_t ఆధారం_id_పునాది,
                                  uint32_t అంకె_ids, uint32_t dest_id_పునాది)
{
    /* Source ID Base */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, ఆధారం_id_పునాది, 4);
    /* Number of IDs */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, అంకె_ids, 4);
    /* Destination Device ID Base */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, ఆధారం_id_పునాది, 4);
    /* Destination IOMMU Offset */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, dest_id_పునాది, 4);
    /* Flags */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);
}

నిర్మాణం AcpiRimtIdMapping {
    uint32_t ఆధారం_id_పునాది;
    uint32_t అంకె_ids;
};
రకనిర్వచనం నిర్మాణం AcpiRimtIdMapping AcpiRimtIdMapping;

/* Build the rimt ID mapping to IOMMU for a given PCI host bridge */
స్థిర పూర్ణాంకం rimt_host_bridges(Object *obj, శూన్య *opaque)
{
    GArray *idmap_blob = opaque;

    ఒకవేళ (object_dynamic_cast(obj, TYPE_PCI_HOST_BRIDGE)) {
        PCIBus *bus = PCI_HOST_BRIDGE(obj)->bus;

        ఒకవేళ (bus && !pci_bus_bypass_iommu(bus)) {
            పూర్ణాంకం min_bus, max_bus;

            pci_bus_range(bus, &min_bus, &max_bus);

            AcpiRimtIdMapping idmap = {
                .ఆధారం_id_పునాది = min_bus << 8,
                .అంకె_ids = (max_bus - min_bus + 1) << 8,
            };
            g_array_append_val(idmap_blob, idmap);
        }
    }

    తిరిగి 0;
}

స్థిర పూర్ణాంకం rimt_idmap_compare(gconstpointer a, gconstpointer b)
{
    AcpiRimtIdMapping *idmap_a = (AcpiRimtIdMapping *)a;
    AcpiRimtIdMapping *idmap_b = (AcpiRimtIdMapping *)b;

    తిరిగి idmap_a->ఆధారం_id_పునాది - idmap_b->ఆధారం_id_పునాది;
}

/*
 * RISC-V IO Mapping Table (RIMT)
 * https://github.com/riscv-non-isa/riscv-acpi-rimt/releases/download/v0.99/rimt-spec.pdf
 */
స్థిర శూన్య build_rimt(GArray *పట్టిక_దత్తాంశం, BIOSLinker *linker,
                       RISCVVirtState *s)
{
    పూర్ణాంకం i, nb_nodes, rc_mapping_లెక్క;
    కొలత_t కణుపు_కొలత, iommu_స్థానభ్రంశం = 0;
    uint32_t id = 0;
    g_autoptr(GArray) iommu_idmaps = g_array_new(false, true,
                                                 పరిమాణం(AcpiRimtIdMapping));

    Acpi_పట్టిక పట్టిక = { .sig = "RIMT", .rev = 1, .oem_id = s->oem_id,
                        .oem_పట్టిక_id = s->oem_పట్టిక_id };

    acpi_పట్టిక_begin(&పట్టిక, పట్టిక_దత్తాంశం);

    object_బిడ్డ_foreach_recursive(object_get_మూలం(),
                                   rimt_host_bridges, iommu_idmaps);

    /* Sort the ID mapping  by Source ID Base*/
    g_array_క్రమీకరించు(iommu_idmaps, rimt_idmap_compare);

    nb_nodes = 2; /* RC, IOMMU */
    rc_mapping_లెక్క = iommu_idmaps->len;
    /* Number of RIMT Nodes */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, nb_nodes, 4);

    /* Offset to Array of RIMT Nodes */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, RIMT_కణుపు_స్థానభ్రంశం, 4);
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4); /* Reserved */

    iommu_స్థానభ్రంశం = పట్టిక_దత్తాంశం->len - పట్టిక.పట్టిక_స్థానభ్రంశం;
    /*  IOMMU Device Structure */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 1);         /* Type - IOMMU*/
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 1);         /* Revision */
    కణుపు_కొలత =  IOMMU_నమోదు_కొలత;
    build_append_int_noprefix(పట్టిక_దత్తాంశం, కణుపు_కొలత, 2); /* Length */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);         /* Reserved */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, id++, 2);      /* ID */
    ఒకవేళ (virt_is_iommu_sys_enabled(s)) {
        /* Hardware ID */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 'R', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 'S', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 'C', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 'V', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '0', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '0', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '0', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '4', 1);
        /* Base Address */
        build_append_int_noprefix(పట్టిక_దత్తాంశం,
                                  s->memmap[VIRT_IOMMU_SYS].పునాది, 8);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);   /* Flags */
    } లేకపోతే {
        /* Hardware ID */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '0', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '0', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '1', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '0', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '0', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '0', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '1', 1);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, '4', 1);

        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 8);   /* Base Address */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 4);   /* Flags */
    }

    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);       /* Proximity Domain */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);       /* PCI Segment number */
    /* PCIe B/D/F */
    ఒకవేళ (virt_is_iommu_sys_enabled(s)) {
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);
    } లేకపోతే {
        build_append_int_noprefix(పట్టిక_దత్తాంశం, s->pci_iommu_bdf, 2);
    }
    /* Number of interrupt wires */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);
    /* Interrupt wire array offset */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, RISCV_అంతరాయం_WIRE_OFFSSET, 2);

    /*  PCIe Root Complex Node */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 1);           /* Type */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 1);           /* Revision */
    కణుపు_కొలత =  మూలం_COMPLEX_నమోదు_కొలత +
                 ID_MAPPING_నమోదు_కొలత * rc_mapping_లెక్క;
    build_append_int_noprefix(పట్టిక_దత్తాంశం, కణుపు_కొలత, 2);   /* Length */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);           /* Reserved */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, id++, 2);        /* ID */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4);           /* Flags */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);           /* Reserved */
    /* PCI Segment number */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);
    /* ID mapping array offset */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, మూలం_COMPLEX_నమోదు_కొలత, 2);
    /* Number of ID mappings */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, rc_mapping_లెక్క, 2);

    /* Output Reference */
    AcpiRimtIdMapping *range;

    /* ID mapping array */
    కొరకు (i = 0; i < iommu_idmaps->len; i++) {
        range = &g_array_సూచీ(iommu_idmaps, AcpiRimtIdMapping, i);
        ఒకవేళ (virt_is_iommu_sys_enabled(s)) {
            range->ఆధారం_id_పునాది = 0;
            range->అంకె_ids = 0x10000;
        } లేకపోతే {
            range->ఆధారం_id_పునాది = s->pci_iommu_bdf + 1;
            range->అంకె_ids = 0xffff - s->pci_iommu_bdf;
        }
        build_rimt_id_mapping(పట్టిక_దత్తాంశం, range->ఆధారం_id_పునాది,
                              range->అంకె_ids, iommu_స్థానభ్రంశం);
    }

    acpi_పట్టిక_చివర(linker, &పట్టిక);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.16 System Resource Affinity Table (SRAT)
 */
స్థిర శూన్య
build_srat(GArray *పట్టిక_దత్తాంశం, BIOSLinker *linker, RISCVVirtState *vms)
{
    పూర్ణాంకం i;
    uint64_t mem_పునాది;
    MachineClass *mc = MACHINE_GET_CLASS(vms);
    MachineState *ms = MACHINE(vms);
    స్థిరరాశి CPUArchIdList *cpu_list = mc->possible_cpu_arch_ids(ms);
    Acpi_పట్టిక పట్టిక = { .sig = "SRAT", .rev = 3, .oem_id = vms->oem_id,
                        .oem_పట్టిక_id = vms->oem_పట్టిక_id };

    acpi_పట్టిక_begin(&పట్టిక, పట్టిక_దత్తాంశం);
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 1, 4); /* Reserved */
    build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 8); /* Reserved */

    కొరకు (i = 0; i < cpu_list->len; ++i) {
        uint32_t nodeid = cpu_list->cpus[i].props.కణుపు_id;
        /*
         * 5.2.16.8 RINTC Affinity Structure
         */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 7, 1);      /* Type */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 20, 1);     /* Length */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 2);        /* Reserved */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, nodeid, 4); /* Proximity Domain */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, i, 4); /* ACPI Processor UID */
        /* Flags, Table 5-70 */
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 1 /* Flags: Enabled */, 4);
        build_append_int_noprefix(పట్టిక_దత్తాంశం, 0, 4); /* Clock Domain */
    }

    mem_పునాది = vms->memmap[VIRT_DRAM].పునాది;
    కొరకు (i = 0; i < ms->numa_state->అంకె_nodes; ++i) {
        ఒకవేళ (ms->numa_state->nodes[i].కణుపు_mem > 0) {
            build_srat_జ్ఞాపకం(పట్టిక_దత్తాంశం, mem_పునాది,
                              ms->numa_state->nodes[i].కణుపు_mem, i,
                              MEM_AFFINITY_ENABLED);
            mem_పునాది += ms->numa_state->nodes[i].కణుపు_mem;
        }
    }

    acpi_పట్టిక_చివర(linker, &పట్టిక);
}

స్థిర శూన్య virt_acpi_build(RISCVVirtState *s, AcpiBuildTables *tables)
{
    GArray *పట్టిక_offsets;
    గుర్తులేని dsdt, xsdt;
    GArray *tables_blob = tables->పట్టిక_దత్తాంశం;
    MachineState *ms = MACHINE(s);

    పట్టిక_offsets = g_array_new(false, true,
                                పరిమాణం(uint32_t));

    bios_linker_loader_alloc(tables->linker,
                             ACPI_BUILD_పట్టిక_FILE, tables_blob,
                             64, false);

    /* DSDT is pointed to by FADT */
    dsdt = tables_blob->len;
    build_dsdt(tables_blob, tables->linker, s);

    /* FADT and others pointed to by XSDT */
    acpi_కూడు_పట్టిక(పట్టిక_offsets, tables_blob);
    build_fadt_rev6(tables_blob, tables->linker, s, dsdt);

    acpi_కూడు_పట్టిక(పట్టిక_offsets, tables_blob);
    build_madt(tables_blob, tables->linker, s);

    acpi_కూడు_పట్టిక(పట్టిక_offsets, tables_blob);
    build_rhct(tables_blob, tables->linker, s);

    ఒకవేళ (virt_is_iommu_sys_enabled(s) || s->pci_iommu_bdf) {
        acpi_కూడు_పట్టిక(పట్టిక_offsets, tables_blob);
        build_rimt(tables_blob, tables->linker, s);
    }

    acpi_కూడు_పట్టిక(పట్టిక_offsets, tables_blob);

    ఒకవేళ (ms->acpi_spcr_enabled) {
        spcr_అమరిక(tables_blob, tables->linker, s);
    }

    acpi_కూడు_పట్టిక(పట్టిక_offsets, tables_blob);
    {
        AcpiMcfgInfo mcfg = {
           .పునాది = s->memmap[VIRT_PCIE_ECAM].పునాది,
           .కొలత = s->memmap[VIRT_PCIE_ECAM].కొలత,
        };
        build_mcfg(tables_blob, tables->linker, &mcfg, s->oem_id,
                   s->oem_పట్టిక_id);
    }

    ఒకవేళ (ms->numa_state->అంకె_nodes > 0) {
        acpi_కూడు_పట్టిక(పట్టిక_offsets, tables_blob);
        build_srat(tables_blob, tables->linker, s);
        ఒకవేళ (ms->numa_state->have_numa_దూరం) {
            acpi_కూడు_పట్టిక(పట్టిక_offsets, tables_blob);
            build_slit(tables_blob, tables->linker, ms, s->oem_id,
                       s->oem_పట్టిక_id);
        }
    }

    /* XSDT is pointed to by RSDP */
    xsdt = tables_blob->len;
    build_xsdt(tables_blob, tables->linker, పట్టిక_offsets, s->oem_id,
                s->oem_పట్టిక_id);

    /* RSDP is in FSEG memory, so allocate it separately */
    {
        Acpi_Rsdp_దత్తాంశం rsdp_దత్తాంశం = {
            .revision = 2,
            .oem_id = s->oem_id,
            .xsdt_tbl_స్థానభ్రంశం = &xsdt,
            .rsdt_tbl_స్థానభ్రంశం = ఖాళీ,
        };
        build_rsdp(tables->rsdp, tables->linker, &rsdp_దత్తాంశం);
    }

    /*
     * The align size is 128, warn if 64k is not enough therefore
     * the align size could be resized.
     */
    ఒకవేళ (tables_blob->len > ACPI_BUILD_పట్టిక_కొలత / 2) {
        warn_report("ACPI table size %u exceeds %d bytes,"
                    " migration may not work",
                    tables_blob->len, ACPI_BUILD_పట్టిక_కొలత / 2);
        error_printf("Try removing some objects.");
    }

    acpi_align_కొలత(tables_blob, ACPI_BUILD_పట్టిక_కొలత);

    /* Clean up memory that's no longer used */
    g_array_free(పట్టిక_offsets, true);
}

స్థిర శూన్య acpi_ram_update(జ్ఞాపకం_Region *mr, GArray *దత్తాంశం)
{
    uint32_t కొలత = acpi_దత్తాంశం_len(దత్తాంశం);

    /*
     * Make sure RAM size is correct - in case it got changed
     * e.g. by migration
     */
    జ్ఞాపకం_region_ram_resize(mr, కొలత, &error_abort);

    జ్ఞాపకం_నకలు(జ్ఞాపకం_region_get_ram_ptr(mr), దత్తాంశం->దత్తాంశం, కొలత);
    జ్ఞాపకం_region_సమితి_dirty(mr, 0, కొలత);
}

స్థిర శూన్య virt_acpi_build_update(శూన్య *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    AcpiBuildTables tables;

    /* No state to update or already patched? Nothing to do. */
    ఒకవేళ (!build_state || build_state->patched) {
        తిరిగి;
    }

    build_state->patched = true;

    acpi_build_tables_init(&tables);

    virt_acpi_build(RISCV_VIRT_MACHINE(qdev_get_machine()), &tables);

    acpi_ram_update(build_state->పట్టిక_mr, tables.పట్టిక_దత్తాంశం);
    acpi_ram_update(build_state->rsdp_mr, tables.rsdp);
    acpi_ram_update(build_state->linker_mr, tables.linker->cmd_blob);

    acpi_build_tables_cleanup(&tables, true);
}

స్థిర శూన్య virt_acpi_build_reset(శూన్య *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    build_state->patched = false;
}

స్థిర స్థిరరాశి VMStateDescription vmstate_virt_acpi_build = {
    .name = "virt_acpi_build",
    .సంస్కరణ_id = 1,
    .minimum_సంస్కరణ_id = 1,
    .fields = (స్థిరరాశి VMState_క్షేత్రం[]) {
        VMSTATE_BOOL(patched, AcpiBuildState),
        VMSTATE_చివర_యొక్క_LIST()
    },
};

శూన్య virt_acpi_అమరిక(RISCVVirtState *s)
{
    AcpiBuildTables tables;
    AcpiBuildState *build_state;

    build_state = g_malloc0(పరిమాణం *build_state);

    acpi_build_tables_init(&tables);
    virt_acpi_build(s, &tables);

    /* Now expose it all to Guest */
    build_state->పట్టిక_mr = acpi_కూడు_rom_blob(virt_acpi_build_update,
                                              build_state, tables.పట్టిక_దత్తాంశం,
                                              ACPI_BUILD_పట్టిక_FILE);
    ప్రతిజ్ఞా(build_state->పట్టిక_mr != ఖాళీ);

    build_state->linker_mr = acpi_కూడు_rom_blob(virt_acpi_build_update,
                                               build_state,
                                               tables.linker->cmd_blob,
                                               ACPI_BUILD_LOADER_FILE);

    build_state->rsdp_mr = acpi_కూడు_rom_blob(virt_acpi_build_update,
                                             build_state, tables.rsdp,
                                             ACPI_BUILD_RSDP_FILE);

    qemu_రిజిస్టరు_reset(virt_acpi_build_reset, build_state);
    virt_acpi_build_reset(build_state);
    vmstate_రిజిస్టరు(ఖాళీ, 0, &vmstate_virt_acpi_build, build_state);

    /*
     * Clean up tables but don't free the memory: we track it
     * in build_state.
     */
    acpi_build_tables_cleanup(&tables, false);
}
