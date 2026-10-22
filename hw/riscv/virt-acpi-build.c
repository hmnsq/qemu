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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "hw/acpi/acpi-defs.h"
#ಅನ್ತರ್ಭಾವ "hw/acpi/acpi.h"
#ಅನ್ತರ್ಭಾವ "hw/acpi/aml-build.h"
#ಅನ್ತರ್ಭಾವ "hw/acpi/pci.h"
#ಅನ್ತರ್ಭಾವ "hw/acpi/utils.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/nvram/fw_cfg_acpi.h"
#ಅನ್ತರ್ಭಾವ "hw/pci-host/gpex.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/virt.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/numa.h"
#ಅನ್ತರ್ಭಾವ "hw/virtio/virtio-acpi.h"
#ಅನ್ತರ್ಭಾವ "kvm/kvm_riscv.h"
#ಅನ್ತರ್ಭಾವ "migration/vmstate.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "system/kvm.h"
#ಅನ್ತರ್ಭಾವ "system/reset.h"

#ಅನ್ತರ್ಭಾವ "aia.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_BUILD_ಕೋಷ್ಟಕ_ಗಾತ್ರ             0x20000
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_BUILD_INTC_ID(socket, ಸೂಚ್ಯಂಕ) ((socket << 24) | (ಸೂಚ್ಯಂಕ))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ AcpiBuildState {
    /* Copy of table in RAM (for patching) */
    ಸ್ಮರಣೆ_Region *ಕೋಷ್ಟಕ_mr;
    ಸ್ಮರಣೆ_Region *rsdp_mr;
    ಸ್ಮರಣೆ_Region *linker_mr;
    /* Is table patched? */
    bool patched;
} AcpiBuildState;

ಸ್ಥಿರ ಶೂನ್ಯ acpi_align_ಗಾತ್ರ(GArray *blob, ಚಿಹ್ನೆರಹಿತ align)
{
    /*
     * Align size to multiple of given size. This reduces the chance
     * we need to change size in the future (breaking cross version migration).
     */
    g_array_ಗಣ_ಗಾತ್ರ(blob, ಸುತ್ತು_ಮೇಲೆ(acpi_ದತ್ತಾಂಶ_len(blob), align));
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_acpi_madt_ಕೂಡಿಸು_rintc(uint32_t uid,
                                      ಸ್ಥಿರಾಂಕ CPUArchIdList *arch_ids,
                                      GArray *ನಮೂದು,
                                      RISCVVirtState *s)
{
    uint8_t  guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(s->aia_guests + 1);
    uint64_t hart_id = arch_ids->cpus[uid].arch_id;
    uint32_t imsic_ಗಾತ್ರ, local_cpu_id, socket_id;
    uint64_t imsic_socket_addr, imsic_addr;
    MachineState *ms = MACHINE(s);

    socket_id = arch_ids->cpus[uid].props.ಗಂಟು_id;
    local_cpu_id = (arch_ids->cpus[uid].arch_id -
                    riscv_socket_ಮೊದಲ_hartid(ms, socket_id)) %
                    riscv_socket_hart_ಎಣಿಕೆ(ms, socket_id);
    imsic_socket_addr = s->memmap[VIRT_IMSIC_S].ತಳ +
                        (socket_id * VIRT_IMSIC_ಗುಂಪು_MAX_ಗಾತ್ರ);
    imsic_ಗಾತ್ರ = IMSIC_HART_ಗಾತ್ರ(guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
    imsic_addr = imsic_socket_addr + local_cpu_id * imsic_ಗಾತ್ರ;
    build_append_int_noprefix(ನಮೂದು, 0x18, 1);       /* Type     */
    build_append_int_noprefix(ನಮೂದು, 36, 1);         /*ಉದ್ದಳತೆ*/
    build_append_int_noprefix(ನಮೂದು, 1, 1);          /*ಆವೃತ್ತಿ*/
    build_append_int_noprefix(ನಮೂದು, 0, 1);          /* Reserved */
    build_append_int_noprefix(ನಮೂದು, 0x1, 4);        /* Flags    */
    build_append_int_noprefix(ನಮೂದು, hart_id, 8);    /* Hart ID  */
    build_append_int_noprefix(ನಮೂದು, uid, 4);        /* ACPI Processor UID */
    /* External Interrupt Controller ID */
    ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_APLIC) {
        build_append_int_noprefix(ನಮೂದು,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.ಗಂಟು_id,
                                      local_cpu_id),
                                  4);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_NONE) {
        build_append_int_noprefix(ನಮೂದು,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.ಗಂಟು_id,
                                      kvm_enabled() ?
                                      local_cpu_id :
                                      2 * local_cpu_id + 1),
                                  4);
    } ಇಲ್ಲದಿದ್ದರೆ {
        build_append_int_noprefix(ನಮೂದು, 0, 4);
    }

    ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC Base address */
        build_append_int_noprefix(ನಮೂದು, imsic_addr, 8);
        /* IMSIC Size */
        build_append_int_noprefix(ನಮೂದು, imsic_ಗಾತ್ರ, 4);
    } ಇಲ್ಲದಿದ್ದರೆ {
        build_append_int_noprefix(ನಮೂದು, 0, 8);
        build_append_int_noprefix(ನಮೂದು, 0, 4);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ acpi_dsdt_ಕೂಡಿಸು_cpus(Aml *scope, RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ಸ್ಥಿರಾಂಕ CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < arch_ids->len; i++) {
            Aml *dev;
            GArray *madt_buf = g_array_new(0, 1, 1);

            dev = aml_device("C%.03X", i);
            aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0007")));
            aml_append(dev, aml_name_decl("_UID",
                       aml_int(arch_ids->cpus[i].arch_id)));

            /* build _MAT object */
            riscv_acpi_madt_ಕೂಡಿಸು_rintc(i, arch_ids, madt_buf, s);
            aml_append(dev, aml_name_decl("_MAT",
                                          aml_ಬಫರ್(madt_buf->len,
                                          (uint8_t *)madt_buf->ದತ್ತಾಂಶ)));
            g_array_free(madt_buf, true);

            aml_append(scope, dev);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ acpi_dsdt_ಕೂಡಿಸು_plic_aplic(Aml *scope, uint8_t socket_ಎಣಿಕೆ,
                                     uint16_t ಅಂಕೆ_sources,
                                     uint64_t mmio_ತಳ, uint64_t mmio_ಗಾತ್ರ,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hid)
{
    uint64_t plic_aplic_addr;
    uint32_t gsi_ತಳ;
    uint8_t  socket;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(ಅಂಕೆ_sources <= 1023);

    ಗಾಗಿ (socket = 0; socket < socket_ಎಣಿಕೆ; socket++) {
        plic_aplic_addr = mmio_ತಳ + mmio_ಗಾತ್ರ * socket;
        gsi_ತಳ = ಅಂಕೆ_sources * socket;
        Aml *dev = aml_device("IC%.02X", socket);
        aml_append(dev, aml_name_decl("_HID", aml_string("%s", hid)));
        aml_append(dev, aml_name_decl("_UID", aml_int(socket)));
        aml_append(dev, aml_name_decl("_GSB", aml_int(gsi_ತಳ)));

        Aml *crs = aml_resource_template();
        aml_append(crs, aml_memory32_fixed(plic_aplic_addr, mmio_ಗಾತ್ರ,
                                           AML_READ_WRITE));
        aml_append(dev, aml_name_decl("_CRS", crs));
        aml_append(scope, dev);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ
acpi_dsdt_ಕೂಡಿಸು_uart(Aml *scope, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *uart_memmap,
                    uint32_t uart_irq, ಪೂರ್ಣಾಂಕ uartidx)
{
    Aml *dev = aml_device("COM%d", uartidx);
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0003")));
    aml_append(dev, aml_name_decl("_UID", aml_int(uartidx)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(uart_memmap->ತಳ,
                                         uart_memmap->ಗಾತ್ರ_2, AML_READ_WRITE));
    aml_append(crs,
                aml_ಅಡಚಣೆ(AML_ಗ್ರಾಹಕ, AML_LEVEL, AML_ಸಕ್ರಿಯ_ಮೇಲಿನ,
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
ಸ್ಥಿರ ಶೂನ್ಯ acpi_dsdt_ಕೂಡಿಸು_iommu_sys(Aml *scope, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *iommu_memmap,
                                    uint32_t iommu_irq)
{
    uint32_t i;

    Aml *dev = aml_device("IMU0");
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0004")));
    aml_append(dev, aml_name_decl("_UID", aml_int(0)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(iommu_memmap->ತಳ,
                                       iommu_memmap->ಗಾತ್ರ_2, AML_READ_WRITE));
    ಗಾಗಿ (i = iommu_irq; i < iommu_irq + 4; i++) {
        aml_append(crs, aml_ಅಡಚಣೆ(AML_ಗ್ರಾಹಕ, AML_ಅಂಚು, AML_ಸಕ್ರಿಯ_ಕೆಳಗಿನ,
                                      AML_EXCLUSIVE, &i, 1));
    }

    aml_append(dev, aml_name_decl("_CRS", crs));
    aml_append(scope, dev);
}

/*
 * Serial Port Console Redirection Table (SPCR)
 * Rev: 1.10
 */

ಸ್ಥಿರ ಶೂನ್ಯ
spcr_ಸಿದ್ಧತೆ(GArray *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, BIOSLinker *linker, RISCVVirtState *s)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ name[] = ".";
    Acpi_Spcr_ದತ್ತಾಂಶ serial = {
        .interface_type = 0x12,       /* 16550 compatible */
        .ತಳ_addr.id = AML_AS_SYSTEM_ಸ್ಮರಣೆ,
        .ತಳ_addr.ಅಗಲ = 8,
        .ತಳ_addr.ಸ್ಥಾನಾಂತರ = 0,
        .ತಳ_addr.ಗಾತ್ರ_2 = 1,
        .ತಳ_addr.addr = s->memmap[VIRT_UART0].ತಳ,
        .ಅಡಚಣೆ_type = (1 << 4),/* Bit[4] RISC-V PLIC/APLIC */
        .pc_ಅಡಚಣೆ = 0,
        .ಅಡಚಣೆ = UART0_IRQ,
        .baud_ದರ = 7,            /* 15200 */
        .parity = 0,
        .stop_ಬಿಟ್ಟುಗಳು = 1,
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
        .namespace_string_ಉದ್ದಳತೆ = ಗಾತ್ರ(name),
        .namespace_string_ಸ್ಥಾನಾಂತರ = 88,
    };

    build_spcr(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, linker, &serial, 4, s->oem_id, s->oem_ಕೋಷ್ಟಕ_id,
               name);
}

/* RHCT Node[N] starts at offset 56 */
#ವ್ಯಾಖ್ಯಾನಿಸು RHCT_ಗಂಟು_ARRAY_ಸ್ಥಾನಾಂತರ 56

/*
 * ACPI spec, Revision 6.6
 * 5.2.37 RISC-V Hart Capabilities Table (RHCT)
 */
ಸ್ಥಿರ ಶೂನ್ಯ build_rhct(GArray *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ಸ್ಥಿರಾಂಕ CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    ಗಾತ್ರ_t len, aligned_len;
    uint32_t isa_ಸ್ಥಾನಾಂತರ, ಅಂಕೆ_rhct_nodes, cmo_ಸ್ಥಾನಾಂತರ = 0;
    RISCVCPU *cpu = &s->soc[0].harts[0];
    uint32_t mmu_ಸ್ಥಾನಾಂತರ = 0;
    bool rv32 = riscv_cpu_is_32bit(cpu);
    g_autofree ಅಕ್ಷರ *isa = ಖಾಲಿ;

    Acpi_ಕೋಷ್ಟಕ ಕೋಷ್ಟಕ = { .sig = "RHCT", .rev = 1, .oem_id = s->oem_id,
                        .oem_ಕೋಷ್ಟಕ_id = s->oem_ಕೋಷ್ಟಕ_id };

    acpi_ಕೋಷ್ಟಕ_begin(&ಕೋಷ್ಟಕ, ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);

    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x0, 4);   /* Reserved */

    /* Time Base Frequency */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                              kvm_enabled() ?
                              kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                              RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ,
                              8);

    /* ISA + N hart info */
    ಅಂಕೆ_rhct_nodes = 1 + ms->smp.cpus;
    ಆದರೆ (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        ಅಂಕೆ_rhct_nodes++;
    }

    ಆದರೆ (!rv32 && cpu->cfg.max_satp_ಬಹುಲಕ >= VM_1_10_SV39) {
        ಅಂಕೆ_rhct_nodes++;
    }

    /* Number of RHCT nodes*/
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಅಂಕೆ_rhct_nodes, 4);

    /* Offset to the RHCT node array */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, RHCT_ಗಂಟು_ARRAY_ಸ್ಥಾನಾಂತರ, 4);

    /* ISA String Node */
    isa_ಸ್ಥಾನಾಂತರ = ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ->len - ಕೋಷ್ಟಕ.ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ;
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);   /* Type 0 */

    isa = riscv_isa_string(cpu);
    len = 8 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(isa) + 1;
    aligned_len = (len % 2) ? (len + 1) : len;

    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, aligned_len, 2);   /* Length */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x1, 2);           /* Revision */

    /* ISA string length including NUL */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(isa) + 1, 2);
    g_array_append_vals(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, isa, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(isa) + 1);   /* ISA string */

    ಆದರೆ (aligned_len != len) {
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x0, 1);   /* Optional Padding */
    }

    /* CMO node */
    ಆದರೆ (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        cmo_ಸ್ಥಾನಾಂತರ = ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ->len - ಕೋಷ್ಟಕ.ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ;
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 2);    /* Type */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 10, 2);   /* Length */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x1, 2);  /* Revision */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 1);    /* Reserved */

        /* CBOM block size */
        ಆದರೆ (cpu->cfg.cbom_blocksize) {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                                      __builtin_ctz(cpu->cfg.cbom_blocksize),
                                      1);
        } ಇಲ್ಲದಿದ್ದರೆ {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 1);
        }

        /* CBOP block size */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 1);

        /* CBOZ block size */
        ಆದರೆ (cpu->cfg.cboz_blocksize) {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                                      __builtin_ctz(cpu->cfg.cboz_blocksize),
                                      1);
        } ಇಲ್ಲದಿದ್ದರೆ {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 1);
        }
    }

    /* MMU node structure */
    ಆದರೆ (!rv32 && cpu->cfg.max_satp_ಬಹುಲಕ >= VM_1_10_SV39) {
        mmu_ಸ್ಥಾನಾಂತರ = ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ->len - ಕೋಷ್ಟಕ.ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ;
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 2, 2);    /* Type */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 8, 2);    /* Length */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x1, 2);  /* Revision */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 1);    /* Reserved */
        /* MMU Type */
        ಆದರೆ (cpu->cfg.max_satp_ಬಹುಲಕ == VM_1_10_SV57) {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 2, 1);    /* Sv57 */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cpu->cfg.max_satp_ಬಹುಲಕ == VM_1_10_SV48) {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 1);    /* Sv48 */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cpu->cfg.max_satp_ಬಹುಲಕ == VM_1_10_SV39) {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 1);    /* Sv39 */
        } ಇಲ್ಲದಿದ್ದರೆ {
            g_assert_not_reached();
        }
    }

    /* Hart Info Node */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < arch_ids->len; i++) {
        len = 16;
        ಪೂರ್ಣಾಂಕ ಅಂಕೆ_offsets = 1;
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0xFFFF, 2);  /* Type */

        /* Length */
        ಆದರೆ (cmo_ಸ್ಥಾನಾಂತರ) {
            len += 4;
            ಅಂಕೆ_offsets++;
        }

        ಆದರೆ (mmu_ಸ್ಥಾನಾಂತರ) {
            len += 4;
            ಅಂಕೆ_offsets++;
        }

        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, len, 2);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x1, 2); /* Revision */
        /* Number of offsets */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಅಂಕೆ_offsets, 2);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, i, 4);   /* ACPI Processor UID */
        /* Offsets */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, isa_ಸ್ಥಾನಾಂತರ, 4);
        ಆದರೆ (cmo_ಸ್ಥಾನಾಂತರ) {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, cmo_ಸ್ಥಾನಾಂತರ, 4);
        }

        ಆದರೆ (mmu_ಸ್ಥಾನಾಂತರ) {
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, mmu_ಸ್ಥಾನಾಂತರ, 4);
        }
    }

    acpi_ಕೋಷ್ಟಕ_ಕೊನೆ(linker, &ಕೋಷ್ಟಕ);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.9 Fixed ACPI Description Table (MADT)
 */
ಸ್ಥಿರ ಶೂನ್ಯ build_fadt_rev6(GArray *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                            BIOSLinker *linker,
                            RISCVVirtState *s,
                            ಚಿಹ್ನೆರಹಿತ dsdt_tbl_ಸ್ಥಾನಾಂತರ)
{
    Acpi_Fadt_ದತ್ತಾಂಶ fadt = {
        .rev = 6,
        .minor_ver = 6,
        .flags = 1 << ACPI_FADT_F_HW_REDUCED_ACPI,
        .xdsdt_tbl_ಸ್ಥಾನಾಂತರ = &dsdt_tbl_ಸ್ಥಾನಾಂತರ,
    };

    build_fadt(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, linker, &fadt, s->oem_id, s->oem_ಕೋಷ್ಟಕ_id);
}

/* DSDT */
ಸ್ಥಿರ ಶೂನ್ಯ build_dsdt(GArray *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    Aml *scope, *dsdt;
    MachineState *ms = MACHINE(s);
    uint8_t socket_ಎಣಿಕೆ;
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = s->memmap;
    Acpi_ಕೋಷ್ಟಕ ಕೋಷ್ಟಕ = { .sig = "DSDT", .rev = 2, .oem_id = s->oem_id,
                        .oem_ಕೋಷ್ಟಕ_id = s->oem_ಕೋಷ್ಟಕ_id };


    acpi_ಕೋಷ್ಟಕ_begin(&ಕೋಷ್ಟಕ, ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);
    dsdt = init_aml_allocator();

    /*
     * When booting the VM with UEFI, UEFI takes ownership of the RTC hardware.
     * While UEFI can use libfdt to disable the RTC device node in the DTB that
     * it passes to the OS, it cannot modify AML. Therefore, we won't generate
     * the RTC ACPI device at all when using UEFI.
     */
    scope = aml_scope("\\_SB");
    acpi_dsdt_ಕೂಡಿಸು_cpus(scope, s);

    fw_cfg_acpi_dsdt_ಕೂಡಿಸು(scope, &memmap[VIRT_FW_CFG]);

    socket_ಎಣಿಕೆ = riscv_socket_ಎಣಿಕೆ(ms);

    ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_NONE) {
        acpi_dsdt_ಕೂಡಿಸು_plic_aplic(scope, socket_ಎಣಿಕೆ, s->ಅಂಕೆ_sources,
                                 memmap[VIRT_PLIC].ತಳ,
                                 memmap[VIRT_PLIC].ಗಾತ್ರ_2,
                                 "RSCV0001");
    } ಇಲ್ಲದಿದ್ದರೆ {
        acpi_dsdt_ಕೂಡಿಸು_plic_aplic(scope, socket_ಎಣಿಕೆ, s->ಅಂಕೆ_sources,
                                 memmap[VIRT_APLIC_S].ತಳ,
                                 memmap[VIRT_APLIC_S].ಗಾತ್ರ_2, "RSCV0002");
    }

    acpi_dsdt_ಕೂಡಿಸು_uart(scope, &memmap[VIRT_UART0], UART0_IRQ, 0);
    ಆದರೆ (s->uart1_present) {
        acpi_dsdt_ಕೂಡಿಸು_uart(scope, &memmap[VIRT_UART1], UART1_IRQ, 1);
    }

    ಆದರೆ (virt_is_iommu_sys_enabled(s)) {
        acpi_dsdt_ಕೂಡಿಸು_iommu_sys(scope, &memmap[VIRT_IOMMU_SYS], IOMMU_SYS_IRQ);
    }

    ಆದರೆ (socket_ಎಣಿಕೆ == 1) {
        virtio_acpi_dsdt_ಕೂಡಿಸು(scope, memmap[VIRT_VIRTIO].ತಳ,
                             memmap[VIRT_VIRTIO].ಗಾತ್ರ_2,
                             VIRTIO_IRQ, 0, VIRTIO_ಎಣಿಕೆ);
        acpi_dsdt_ಕೂಡಿಸು_gpex_host(scope, PCIE_IRQ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (socket_ಎಣಿಕೆ == 2) {
        virtio_acpi_dsdt_ಕೂಡಿಸು(scope, memmap[VIRT_VIRTIO].ತಳ,
                             memmap[VIRT_VIRTIO].ಗಾತ್ರ_2,
                             VIRTIO_IRQ + s->ಅಂಕೆ_sources, 0,
                             VIRTIO_ಎಣಿಕೆ);
        acpi_dsdt_ಕೂಡಿಸು_gpex_host(scope, PCIE_IRQ + s->ಅಂಕೆ_sources);
    } ಇಲ್ಲದಿದ್ದರೆ {
        virtio_acpi_dsdt_ಕೂಡಿಸು(scope, memmap[VIRT_VIRTIO].ತಳ,
                             memmap[VIRT_VIRTIO].ಗಾತ್ರ_2,
                             VIRTIO_IRQ + s->ಅಂಕೆ_sources, 0,
                             VIRTIO_ಎಣಿಕೆ);
        acpi_dsdt_ಕೂಡಿಸು_gpex_host(scope, PCIE_IRQ + s->ಅಂಕೆ_sources * 2);
    }

    aml_append(dsdt, scope);

    /* copy AML table into ACPI tables blob and patch header there */
    g_array_append_vals(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, dsdt->buf->ದತ್ತಾಂಶ, dsdt->buf->len);

    acpi_ಕೋಷ್ಟಕ_ಕೊನೆ(linker, &ಕೋಷ್ಟಕ);
    free_aml_allocator();
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.12 Multiple APIC Description Table (MADT)
 */
ಸ್ಥಿರ ಶೂನ್ಯ build_madt(GArray *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ಸ್ಥಿರಾಂಕ CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    uint8_t  ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(riscv_socket_ಎಣಿಕೆ(ms));
    uint8_t  guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(s->aia_guests + 1);
    uint16_t imsic_max_hart_ಪ್ರತಿ_socket = 0;
    uint8_t  hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
    uint64_t aplic_addr;
    uint32_t gsi_ತಳ;
    uint8_t  socket;

    ಗಾಗಿ (socket = 0; socket < riscv_socket_ಎಣಿಕೆ(ms); socket++) {
        ಆದರೆ (imsic_max_hart_ಪ್ರತಿ_socket < s->soc[socket].ಅಂಕೆ_harts) {
            imsic_max_hart_ಪ್ರತಿ_socket = s->soc[socket].ಅಂಕೆ_harts;
        }
    }

    hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು = imsic_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(imsic_max_hart_ಪ್ರತಿ_socket);

    Acpi_ಕೋಷ್ಟಕ ಕೋಷ್ಟಕ = { .sig = "APIC", .rev = 7, .oem_id = s->oem_id,
                        .oem_ಕೋಷ್ಟಕ_id = s->oem_ಕೋಷ್ಟಕ_id };

    acpi_ಕೋಷ್ಟಕ_begin(&ಕೋಷ್ಟಕ, ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);
    /* Local Interrupt Controller Address */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);   /* MADT Flags */

    /* RISC-V Local INTC structures per HART */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < arch_ids->len; i++) {
        riscv_acpi_madt_ಕೂಡಿಸು_rintc(i, arch_ids, ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, s);
    }

    /* IMSIC */
    ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x19, 1);     /* Type */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 16, 1);       /* Length */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 1);        /* Version */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 1);        /* Reserved */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);        /* Flags */
        /* Number of supervisor mode Interrupt Identities */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, VIRT_IRQCHIP_ಅಂಕೆ_MSIS, 2);
        /* Number of guest mode Interrupt Identities */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, VIRT_IRQCHIP_ಅಂಕೆ_MSIS, 2);
        /* Guest Index Bits */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, guest_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು, 1);
        /* Hart Index Bits */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, hart_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು, 1);
        /* Group Index Bits */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಗುಂಪು_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು, 1);
        /* Group Index Shift */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, IMSIC_MMIO_ಗುಂಪು_MIN_ಸರಿಸು, 1);
    }

    ಆದರೆ (s->aia_type != VIRT_AIA_TYPE_NONE) {
        /* APLICs */
        ಗಾಗಿ (socket = 0; socket < riscv_socket_ಎಣಿಕೆ(ms); socket++) {
            aplic_addr = s->memmap[VIRT_APLIC_S].ತಳ +
                             s->memmap[VIRT_APLIC_S].ಗಾತ್ರ_2 * socket;
            gsi_ತಳ = s->ಅಂಕೆ_sources * socket;
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x1A, 1);    /* Type */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 36, 1);      /* Length */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 1);       /* Version */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, socket, 1);  /* APLIC ID */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);       /* Flags */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 8);       /* Hardware ID */
            /* Number of IDCs */
            ಆದರೆ (s->aia_type == VIRT_AIA_TYPE_APLIC) {
                build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                                          s->soc[socket].ಅಂಕೆ_harts,
                                          2);
            } ಇಲ್ಲದಿದ್ದರೆ {
                build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);
            }
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, VIRT_IRQCHIP_ಅಂಕೆ_SOURCES, 2);
            /* Global System Interrupt Base */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, gsi_ತಳ, 4);
            /* APLIC Address */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, aplic_addr, 8);
            /* APLIC size */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                                      s->memmap[VIRT_APLIC_S].ಗಾತ್ರ_2, 4);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* PLICs */
        ಗಾಗಿ (socket = 0; socket < riscv_socket_ಎಣಿಕೆ(ms); socket++) {
            aplic_addr = s->memmap[VIRT_PLIC].ತಳ +
                         s->memmap[VIRT_PLIC].ಗಾತ್ರ_2 * socket;
            gsi_ತಳ = VIRT_IRQCHIP_ಅಂಕೆ_SOURCES * socket;
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0x1B, 1);   /* Type */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 36, 1);     /* Length */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 1);      /* Version */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, socket, 1); /* PLIC ID */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 8);      /* Hardware ID */
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                                      VIRT_IRQCHIP_ಅಂಕೆ_SOURCES - 1, 2);
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);     /* Max Priority */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);     /* Flags */
            /* PLIC Size */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, s->memmap[VIRT_PLIC].ಗಾತ್ರ_2, 4);
            /* PLIC Address */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, aplic_addr, 8);
            /* Global System Interrupt Vector Base */
            build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, gsi_ತಳ, 4);
        }
    }

    acpi_ಕೋಷ್ಟಕ_ಕೊನೆ(linker, &ಕೋಷ್ಟಕ);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ID_MAPPING_ನಮೂದು_ಗಾತ್ರ        20
#ವ್ಯಾಖ್ಯಾನಿಸು IOMMU_ನಮೂದು_ಗಾತ್ರ             40
#ವ್ಯಾಖ್ಯಾನಿಸು RISCV_ಅಡಚಣೆ_WIRE_OFFSSET 40
#ವ್ಯಾಖ್ಯಾನಿಸು ಬೇರು_COMPLEX_ನಮೂದು_ಗಾತ್ರ      20
#ವ್ಯಾಖ್ಯಾನಿಸು RIMT_ಗಂಟು_ಸ್ಥಾನಾಂತರ             48

/*
 * ID Mapping Structure
 */
ಸ್ಥಿರ ಶೂನ್ಯ build_rimt_id_mapping(GArray *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, uint32_t ಮೂಲ_id_ತಳ,
                                  uint32_t ಅಂಕೆ_ids, uint32_t dest_id_ತಳ)
{
    /* Source ID Base */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಮೂಲ_id_ತಳ, 4);
    /* Number of IDs */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಅಂಕೆ_ids, 4);
    /* Destination Device ID Base */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಮೂಲ_id_ತಳ, 4);
    /* Destination IOMMU Offset */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, dest_id_ತಳ, 4);
    /* Flags */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);
}

ರಚನೆ AcpiRimtIdMapping {
    uint32_t ಮೂಲ_id_ತಳ;
    uint32_t ಅಂಕೆ_ids;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ AcpiRimtIdMapping AcpiRimtIdMapping;

/* Build the rimt ID mapping to IOMMU for a given PCI host bridge */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rimt_host_bridges(Object *obj, ಶೂನ್ಯ *opaque)
{
    GArray *idmap_blob = opaque;

    ಆದರೆ (object_dynamic_cast(obj, TYPE_PCI_HOST_BRIDGE)) {
        PCIBus *bus = PCI_HOST_BRIDGE(obj)->bus;

        ಆದರೆ (bus && !pci_bus_bypass_iommu(bus)) {
            ಪೂರ್ಣಾಂಕ min_bus, max_bus;

            pci_bus_range(bus, &min_bus, &max_bus);

            AcpiRimtIdMapping idmap = {
                .ಮೂಲ_id_ತಳ = min_bus << 8,
                .ಅಂಕೆ_ids = (max_bus - min_bus + 1) << 8,
            };
            g_array_append_val(idmap_blob, idmap);
        }
    }

    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rimt_idmap_compare(gconstpointer a, gconstpointer b)
{
    AcpiRimtIdMapping *idmap_a = (AcpiRimtIdMapping *)a;
    AcpiRimtIdMapping *idmap_b = (AcpiRimtIdMapping *)b;

    ಹಿಂತಿರುಗಿ idmap_a->ಮೂಲ_id_ತಳ - idmap_b->ಮೂಲ_id_ತಳ;
}

/*
 * RISC-V IO Mapping Table (RIMT)
 * https://github.com/riscv-non-isa/riscv-acpi-rimt/releases/download/v0.99/rimt-spec.pdf
 */
ಸ್ಥಿರ ಶೂನ್ಯ build_rimt(GArray *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, BIOSLinker *linker,
                       RISCVVirtState *s)
{
    ಪೂರ್ಣಾಂಕ i, nb_nodes, rc_mapping_ಎಣಿಕೆ;
    ಗಾತ್ರ_t ಗಂಟು_ಗಾತ್ರ, iommu_ಸ್ಥಾನಾಂತರ = 0;
    uint32_t id = 0;
    g_autoptr(GArray) iommu_idmaps = g_array_new(false, true,
                                                 ಗಾತ್ರ(AcpiRimtIdMapping));

    Acpi_ಕೋಷ್ಟಕ ಕೋಷ್ಟಕ = { .sig = "RIMT", .rev = 1, .oem_id = s->oem_id,
                        .oem_ಕೋಷ್ಟಕ_id = s->oem_ಕೋಷ್ಟಕ_id };

    acpi_ಕೋಷ್ಟಕ_begin(&ಕೋಷ್ಟಕ, ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);

    object_ಮಗು_foreach_recursive(object_get_ಬೇರು(),
                                   rimt_host_bridges, iommu_idmaps);

    /* Sort the ID mapping  by Source ID Base*/
    g_array_ವಿಂಗಡಿಸು(iommu_idmaps, rimt_idmap_compare);

    nb_nodes = 2; /* RC, IOMMU */
    rc_mapping_ಎಣಿಕೆ = iommu_idmaps->len;
    /* Number of RIMT Nodes */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, nb_nodes, 4);

    /* Offset to Array of RIMT Nodes */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, RIMT_ಗಂಟು_ಸ್ಥಾನಾಂತರ, 4);
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4); /* Reserved */

    iommu_ಸ್ಥಾನಾಂತರ = ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ->len - ಕೋಷ್ಟಕ.ಕೋಷ್ಟಕ_ಸ್ಥಾನಾಂತರ;
    /*  IOMMU Device Structure */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 1);         /* Type - IOMMU*/
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 1);         /* Revision */
    ಗಂಟು_ಗಾತ್ರ =  IOMMU_ನಮೂದು_ಗಾತ್ರ;
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಗಂಟು_ಗಾತ್ರ, 2); /* Length */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);         /* Reserved */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, id++, 2);      /* ID */
    ಆದರೆ (virt_is_iommu_sys_enabled(s)) {
        /* Hardware ID */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 'R', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 'S', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 'C', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 'V', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '0', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '0', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '0', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '4', 1);
        /* Base Address */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                                  s->memmap[VIRT_IOMMU_SYS].ತಳ, 8);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);   /* Flags */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Hardware ID */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '0', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '0', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '1', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '0', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '0', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '0', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '1', 1);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, '4', 1);

        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 8);   /* Base Address */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 4);   /* Flags */
    }

    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);       /* Proximity Domain */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);       /* PCI Segment number */
    /* PCIe B/D/F */
    ಆದರೆ (virt_is_iommu_sys_enabled(s)) {
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, s->pci_iommu_bdf, 2);
    }
    /* Number of interrupt wires */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);
    /* Interrupt wire array offset */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, RISCV_ಅಡಚಣೆ_WIRE_OFFSSET, 2);

    /*  PCIe Root Complex Node */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 1);           /* Type */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 1);           /* Revision */
    ಗಂಟು_ಗಾತ್ರ =  ಬೇರು_COMPLEX_ನಮೂದು_ಗಾತ್ರ +
                 ID_MAPPING_ನಮೂದು_ಗಾತ್ರ * rc_mapping_ಎಣಿಕೆ;
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಗಂಟು_ಗಾತ್ರ, 2);   /* Length */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);           /* Reserved */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, id++, 2);        /* ID */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4);           /* Flags */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);           /* Reserved */
    /* PCI Segment number */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);
    /* ID mapping array offset */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, ಬೇರು_COMPLEX_ನಮೂದು_ಗಾತ್ರ, 2);
    /* Number of ID mappings */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, rc_mapping_ಎಣಿಕೆ, 2);

    /* Output Reference */
    AcpiRimtIdMapping *range;

    /* ID mapping array */
    ಗಾಗಿ (i = 0; i < iommu_idmaps->len; i++) {
        range = &g_array_ಸೂಚ್ಯಂಕ(iommu_idmaps, AcpiRimtIdMapping, i);
        ಆದರೆ (virt_is_iommu_sys_enabled(s)) {
            range->ಮೂಲ_id_ತಳ = 0;
            range->ಅಂಕೆ_ids = 0x10000;
        } ಇಲ್ಲದಿದ್ದರೆ {
            range->ಮೂಲ_id_ತಳ = s->pci_iommu_bdf + 1;
            range->ಅಂಕೆ_ids = 0xffff - s->pci_iommu_bdf;
        }
        build_rimt_id_mapping(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, range->ಮೂಲ_id_ತಳ,
                              range->ಅಂಕೆ_ids, iommu_ಸ್ಥಾನಾಂತರ);
    }

    acpi_ಕೋಷ್ಟಕ_ಕೊನೆ(linker, &ಕೋಷ್ಟಕ);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.16 System Resource Affinity Table (SRAT)
 */
ಸ್ಥಿರ ಶೂನ್ಯ
build_srat(GArray *ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, BIOSLinker *linker, RISCVVirtState *vms)
{
    ಪೂರ್ಣಾಂಕ i;
    uint64_t mem_ತಳ;
    MachineClass *mc = MACHINE_GET_CLASS(vms);
    MachineState *ms = MACHINE(vms);
    ಸ್ಥಿರಾಂಕ CPUArchIdList *cpu_list = mc->possible_cpu_arch_ids(ms);
    Acpi_ಕೋಷ್ಟಕ ಕೋಷ್ಟಕ = { .sig = "SRAT", .rev = 3, .oem_id = vms->oem_id,
                        .oem_ಕೋಷ್ಟಕ_id = vms->oem_ಕೋಷ್ಟಕ_id };

    acpi_ಕೋಷ್ಟಕ_begin(&ಕೋಷ್ಟಕ, ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1, 4); /* Reserved */
    build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 8); /* Reserved */

    ಗಾಗಿ (i = 0; i < cpu_list->len; ++i) {
        uint32_t nodeid = cpu_list->cpus[i].props.ಗಂಟು_id;
        /*
         * 5.2.16.8 RINTC Affinity Structure
         */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 7, 1);      /* Type */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 20, 1);     /* Length */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 2);        /* Reserved */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, nodeid, 4); /* Proximity Domain */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, i, 4); /* ACPI Processor UID */
        /* Flags, Table 5-70 */
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 1 /* Flags: Enabled */, 4);
        build_append_int_noprefix(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, 0, 4); /* Clock Domain */
    }

    mem_ತಳ = vms->memmap[VIRT_DRAM].ತಳ;
    ಗಾಗಿ (i = 0; i < ms->numa_state->ಅಂಕೆ_nodes; ++i) {
        ಆದರೆ (ms->numa_state->nodes[i].ಗಂಟು_mem > 0) {
            build_srat_ಸ್ಮರಣೆ(ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ, mem_ತಳ,
                              ms->numa_state->nodes[i].ಗಂಟು_mem, i,
                              MEM_AFFINITY_ENABLED);
            mem_ತಳ += ms->numa_state->nodes[i].ಗಂಟು_mem;
        }
    }

    acpi_ಕೋಷ್ಟಕ_ಕೊನೆ(linker, &ಕೋಷ್ಟಕ);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_acpi_build(RISCVVirtState *s, AcpiBuildTables *tables)
{
    GArray *ಕೋಷ್ಟಕ_offsets;
    ಚಿಹ್ನೆರಹಿತ dsdt, xsdt;
    GArray *tables_blob = tables->ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ;
    MachineState *ms = MACHINE(s);

    ಕೋಷ್ಟಕ_offsets = g_array_new(false, true,
                                ಗಾತ್ರ(uint32_t));

    bios_linker_loader_alloc(tables->linker,
                             ACPI_BUILD_ಕೋಷ್ಟಕ_FILE, tables_blob,
                             64, false);

    /* DSDT is pointed to by FADT */
    dsdt = tables_blob->len;
    build_dsdt(tables_blob, tables->linker, s);

    /* FADT and others pointed to by XSDT */
    acpi_ಕೂಡಿಸು_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ_offsets, tables_blob);
    build_fadt_rev6(tables_blob, tables->linker, s, dsdt);

    acpi_ಕೂಡಿಸು_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ_offsets, tables_blob);
    build_madt(tables_blob, tables->linker, s);

    acpi_ಕೂಡಿಸು_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ_offsets, tables_blob);
    build_rhct(tables_blob, tables->linker, s);

    ಆದರೆ (virt_is_iommu_sys_enabled(s) || s->pci_iommu_bdf) {
        acpi_ಕೂಡಿಸು_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ_offsets, tables_blob);
        build_rimt(tables_blob, tables->linker, s);
    }

    acpi_ಕೂಡಿಸು_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ_offsets, tables_blob);

    ಆದರೆ (ms->acpi_spcr_enabled) {
        spcr_ಸಿದ್ಧತೆ(tables_blob, tables->linker, s);
    }

    acpi_ಕೂಡಿಸು_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ_offsets, tables_blob);
    {
        AcpiMcfgInfo mcfg = {
           .ತಳ = s->memmap[VIRT_PCIE_ECAM].ತಳ,
           .ಗಾತ್ರ_2 = s->memmap[VIRT_PCIE_ECAM].ಗಾತ್ರ_2,
        };
        build_mcfg(tables_blob, tables->linker, &mcfg, s->oem_id,
                   s->oem_ಕೋಷ್ಟಕ_id);
    }

    ಆದರೆ (ms->numa_state->ಅಂಕೆ_nodes > 0) {
        acpi_ಕೂಡಿಸು_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ_offsets, tables_blob);
        build_srat(tables_blob, tables->linker, s);
        ಆದರೆ (ms->numa_state->have_numa_ದೂರ) {
            acpi_ಕೂಡಿಸು_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ_offsets, tables_blob);
            build_slit(tables_blob, tables->linker, ms, s->oem_id,
                       s->oem_ಕೋಷ್ಟಕ_id);
        }
    }

    /* XSDT is pointed to by RSDP */
    xsdt = tables_blob->len;
    build_xsdt(tables_blob, tables->linker, ಕೋಷ್ಟಕ_offsets, s->oem_id,
                s->oem_ಕೋಷ್ಟಕ_id);

    /* RSDP is in FSEG memory, so allocate it separately */
    {
        Acpi_Rsdp_ದತ್ತಾಂಶ rsdp_ದತ್ತಾಂಶ = {
            .revision = 2,
            .oem_id = s->oem_id,
            .xsdt_tbl_ಸ್ಥಾನಾಂತರ = &xsdt,
            .rsdt_tbl_ಸ್ಥಾನಾಂತರ = ಖಾಲಿ,
        };
        build_rsdp(tables->rsdp, tables->linker, &rsdp_ದತ್ತಾಂಶ);
    }

    /*
     * The align size is 128, warn if 64k is not enough therefore
     * the align size could be resized.
     */
    ಆದರೆ (tables_blob->len > ACPI_BUILD_ಕೋಷ್ಟಕ_ಗಾತ್ರ / 2) {
        warn_report("ACPI table size %u exceeds %d bytes,"
                    " migration may not work",
                    tables_blob->len, ACPI_BUILD_ಕೋಷ್ಟಕ_ಗಾತ್ರ / 2);
        error_printf("Try removing some objects.");
    }

    acpi_align_ಗಾತ್ರ(tables_blob, ACPI_BUILD_ಕೋಷ್ಟಕ_ಗಾತ್ರ);

    /* Clean up memory that's no longer used */
    g_array_free(ಕೋಷ್ಟಕ_offsets, true);
}

ಸ್ಥಿರ ಶೂನ್ಯ acpi_ram_update(ಸ್ಮರಣೆ_Region *mr, GArray *ದತ್ತಾಂಶ)
{
    uint32_t ಗಾತ್ರ_2 = acpi_ದತ್ತಾಂಶ_len(ದತ್ತಾಂಶ);

    /*
     * Make sure RAM size is correct - in case it got changed
     * e.g. by migration
     */
    ಸ್ಮರಣೆ_region_ram_resize(mr, ಗಾತ್ರ_2, &error_abort);

    ಸ್ಮರಣೆ_ನಕಲು(ಸ್ಮರಣೆ_region_get_ram_ptr(mr), ದತ್ತಾಂಶ->ದತ್ತಾಂಶ, ಗಾತ್ರ_2);
    ಸ್ಮರಣೆ_region_ಗಣ_dirty(mr, 0, ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_acpi_build_update(ಶೂನ್ಯ *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    AcpiBuildTables tables;

    /* No state to update or already patched? Nothing to do. */
    ಆದರೆ (!build_state || build_state->patched) {
        ಹಿಂತಿರುಗಿ;
    }

    build_state->patched = true;

    acpi_build_tables_init(&tables);

    virt_acpi_build(RISCV_VIRT_MACHINE(qdev_get_machine()), &tables);

    acpi_ram_update(build_state->ಕೋಷ್ಟಕ_mr, tables.ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);
    acpi_ram_update(build_state->rsdp_mr, tables.rsdp);
    acpi_ram_update(build_state->linker_mr, tables.linker->cmd_blob);

    acpi_build_tables_cleanup(&tables, true);
}

ಸ್ಥಿರ ಶೂನ್ಯ virt_acpi_build_reset(ಶೂನ್ಯ *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    build_state->patched = false;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ VMStateDescription vmstate_virt_acpi_build = {
    .name = "virt_acpi_build",
    .ಆವೃತ್ತಿ_id = 1,
    .minimum_ಆವೃತ್ತಿ_id = 1,
    .fields = (ಸ್ಥಿರಾಂಕ VMState_ಕ್ಷೇತ್ರ[]) {
        VMSTATE_BOOL(patched, AcpiBuildState),
        VMSTATE_ಕೊನೆ_ದ_LIST()
    },
};

ಶೂನ್ಯ virt_acpi_ಸಿದ್ಧತೆ(RISCVVirtState *s)
{
    AcpiBuildTables tables;
    AcpiBuildState *build_state;

    build_state = g_malloc0(ಗಾತ್ರ *build_state);

    acpi_build_tables_init(&tables);
    virt_acpi_build(s, &tables);

    /* Now expose it all to Guest */
    build_state->ಕೋಷ್ಟಕ_mr = acpi_ಕೂಡಿಸು_rom_blob(virt_acpi_build_update,
                                              build_state, tables.ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ,
                                              ACPI_BUILD_ಕೋಷ್ಟಕ_FILE);
    ಪ್ರತಿಜ್ಞಾ(build_state->ಕೋಷ್ಟಕ_mr != ಖಾಲಿ);

    build_state->linker_mr = acpi_ಕೂಡಿಸು_rom_blob(virt_acpi_build_update,
                                               build_state,
                                               tables.linker->cmd_blob,
                                               ACPI_BUILD_LOADER_FILE);

    build_state->rsdp_mr = acpi_ಕೂಡಿಸು_rom_blob(virt_acpi_build_update,
                                             build_state, tables.rsdp,
                                             ACPI_BUILD_RSDP_FILE);

    qemu_ರಿಜಿಸ್ಟರ್_reset(virt_acpi_build_reset, build_state);
    virt_acpi_build_reset(build_state);
    vmstate_ರಿಜಿಸ್ಟರ್(ಖಾಲಿ, 0, &vmstate_virt_acpi_build, build_state);

    /*
     * Clean up tables but don't free the memory: we track it
     * in build_state.
     */
    acpi_build_tables_cleanup(&tables, false);
}
