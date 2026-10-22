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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/acpi/acpi-defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/acpi/acpi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/acpi/aml-build.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/acpi/pci.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/acpi/utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/nvram/fw_cfg_acpi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/pci-host/gpex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/virt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/numa.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/virtio/virtio-acpi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kvm/kvm_riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "migration/vmstate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/kvm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/reset.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "aia.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_BUILD_ਸਾਰਣੀ_ਮਾਪ             0x20000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_BUILD_INTC_ID(socket, ਸੂਚਕਾਂਕ) ((socket << 24) | (ਸੂਚਕਾਂਕ))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ AcpiBuildState {
    /* Copy of table in RAM (for patching) */
    ਸਮ੍ਰਿਤੀ_Region *ਸਾਰਣੀ_mr;
    ਸਮ੍ਰਿਤੀ_Region *rsdp_mr;
    ਸਮ੍ਰਿਤੀ_Region *linker_mr;
    /* Is table patched? */
    bool patched;
} AcpiBuildState;

ਸਥਿਰ ਖਾਲੀ acpi_align_ਮਾਪ(GArray *blob, ਬਿਨਾਨਿਸ਼ਾਨੀ align)
{
    /*
     * Align size to multiple of given size. This reduces the chance
     * we need to change size in the future (breaking cross version migration).
     */
    g_array_ਸੈੱਟ_ਮਾਪ(blob, ਗੇੜਾ_ਉੱਪਰ(acpi_ਅੰਕੜੇ_len(blob), align));
}

ਸਥਿਰ ਖਾਲੀ riscv_acpi_madt_ਜੋੜੋ_rintc(uint32_t uid,
                                      ਅਚਲ CPUArchIdList *arch_ids,
                                      GArray *ਇੰਦਰਾਜ,
                                      RISCVVirtState *s)
{
    uint8_t  guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ = imsic_ਅੰਕ_ਬਿੱਟ(s->aia_guests + 1);
    uint64_t hart_id = arch_ids->cpus[uid].arch_id;
    uint32_t imsic_ਮਾਪ, local_cpu_id, socket_id;
    uint64_t imsic_socket_addr, imsic_addr;
    MachineState *ms = MACHINE(s);

    socket_id = arch_ids->cpus[uid].props.ਗੰਢ_id;
    local_cpu_id = (arch_ids->cpus[uid].arch_id -
                    riscv_socket_ਪਹਿਲਾ_hartid(ms, socket_id)) %
                    riscv_socket_hart_ਗਿਣਤੀ(ms, socket_id);
    imsic_socket_addr = s->memmap[VIRT_IMSIC_S].ਅਧਾਰ +
                        (socket_id * VIRT_IMSIC_ਸਮੂਹ_MAX_ਮਾਪ);
    imsic_ਮਾਪ = IMSIC_HART_ਮਾਪ(guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
    imsic_addr = imsic_socket_addr + local_cpu_id * imsic_ਮਾਪ;
    build_append_int_noprefix(ਇੰਦਰਾਜ, 0x18, 1);       /* Type     */
    build_append_int_noprefix(ਇੰਦਰਾਜ, 36, 1);         /*ਲੰਮਾਈ*/
    build_append_int_noprefix(ਇੰਦਰਾਜ, 1, 1);          /*ਸੰਸਕਰਣ*/
    build_append_int_noprefix(ਇੰਦਰਾਜ, 0, 1);          /* Reserved */
    build_append_int_noprefix(ਇੰਦਰਾਜ, 0x1, 4);        /* Flags    */
    build_append_int_noprefix(ਇੰਦਰਾਜ, hart_id, 8);    /* Hart ID  */
    build_append_int_noprefix(ਇੰਦਰਾਜ, uid, 4);        /* ACPI Processor UID */
    /* External Interrupt Controller ID */
    ਜੇ (s->aia_type == VIRT_AIA_TYPE_APLIC) {
        build_append_int_noprefix(ਇੰਦਰਾਜ,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.ਗੰਢ_id,
                                      local_cpu_id),
                                  4);
    } ਨਹੀਂਤਾਂ ਜੇ (s->aia_type == VIRT_AIA_TYPE_NONE) {
        build_append_int_noprefix(ਇੰਦਰਾਜ,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.ਗੰਢ_id,
                                      kvm_enabled() ?
                                      local_cpu_id :
                                      2 * local_cpu_id + 1),
                                  4);
    } ਨਹੀਂਤਾਂ {
        build_append_int_noprefix(ਇੰਦਰਾਜ, 0, 4);
    }

    ਜੇ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC Base address */
        build_append_int_noprefix(ਇੰਦਰਾਜ, imsic_addr, 8);
        /* IMSIC Size */
        build_append_int_noprefix(ਇੰਦਰਾਜ, imsic_ਮਾਪ, 4);
    } ਨਹੀਂਤਾਂ {
        build_append_int_noprefix(ਇੰਦਰਾਜ, 0, 8);
        build_append_int_noprefix(ਇੰਦਰਾਜ, 0, 4);
    }
}

ਸਥਿਰ ਖਾਲੀ acpi_dsdt_ਜੋੜੋ_cpus(Aml *scope, RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ਅਚਲ CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);

    ਲਈ (ਪੂਰਨ i = 0; i < arch_ids->len; i++) {
            Aml *dev;
            GArray *madt_buf = g_array_new(0, 1, 1);

            dev = aml_device("C%.03X", i);
            aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0007")));
            aml_append(dev, aml_name_decl("_UID",
                       aml_int(arch_ids->cpus[i].arch_id)));

            /* build _MAT object */
            riscv_acpi_madt_ਜੋੜੋ_rintc(i, arch_ids, madt_buf, s);
            aml_append(dev, aml_name_decl("_MAT",
                                          aml_ਬਫਰ(madt_buf->len,
                                          (uint8_t *)madt_buf->ਅੰਕੜੇ)));
            g_array_free(madt_buf, true);

            aml_append(scope, dev);
    }
}

ਸਥਿਰ ਖਾਲੀ acpi_dsdt_ਜੋੜੋ_plic_aplic(Aml *scope, uint8_t socket_ਗਿਣਤੀ,
                                     uint16_t ਅੰਕ_sources,
                                     uint64_t mmio_ਅਧਾਰ, uint64_t mmio_ਮਾਪ,
                                     ਅਚਲ ਅੱਖਰ *hid)
{
    uint64_t plic_aplic_addr;
    uint32_t gsi_ਅਧਾਰ;
    uint8_t  socket;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(ਅੰਕ_sources <= 1023);

    ਲਈ (socket = 0; socket < socket_ਗਿਣਤੀ; socket++) {
        plic_aplic_addr = mmio_ਅਧਾਰ + mmio_ਮਾਪ * socket;
        gsi_ਅਧਾਰ = ਅੰਕ_sources * socket;
        Aml *dev = aml_device("IC%.02X", socket);
        aml_append(dev, aml_name_decl("_HID", aml_string("%s", hid)));
        aml_append(dev, aml_name_decl("_UID", aml_int(socket)));
        aml_append(dev, aml_name_decl("_GSB", aml_int(gsi_ਅਧਾਰ)));

        Aml *crs = aml_resource_template();
        aml_append(crs, aml_memory32_fixed(plic_aplic_addr, mmio_ਮਾਪ,
                                           AML_READ_WRITE));
        aml_append(dev, aml_name_decl("_CRS", crs));
        aml_append(scope, dev);
    }
}

ਸਥਿਰ ਖਾਲੀ
acpi_dsdt_ਜੋੜੋ_uart(Aml *scope, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *uart_memmap,
                    uint32_t uart_irq, ਪੂਰਨ uartidx)
{
    Aml *dev = aml_device("COM%d", uartidx);
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0003")));
    aml_append(dev, aml_name_decl("_UID", aml_int(uartidx)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(uart_memmap->ਅਧਾਰ,
                                         uart_memmap->ਮਾਪ, AML_READ_WRITE));
    aml_append(crs,
                aml_ਰੁਕਾਵਟ(AML_ਖਪਤਕਾਰ, AML_LEVEL, AML_ਸਰਗਰਮ_ਉੱਚਾ,
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
ਸਥਿਰ ਖਾਲੀ acpi_dsdt_ਜੋੜੋ_iommu_sys(Aml *scope, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *iommu_memmap,
                                    uint32_t iommu_irq)
{
    uint32_t i;

    Aml *dev = aml_device("IMU0");
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0004")));
    aml_append(dev, aml_name_decl("_UID", aml_int(0)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(iommu_memmap->ਅਧਾਰ,
                                       iommu_memmap->ਮਾਪ, AML_READ_WRITE));
    ਲਈ (i = iommu_irq; i < iommu_irq + 4; i++) {
        aml_append(crs, aml_ਰੁਕਾਵਟ(AML_ਖਪਤਕਾਰ, AML_ਕਿਨਾਰਾ, AML_ਸਰਗਰਮ_ਨੀਵਾਂ,
                                      AML_EXCLUSIVE, &i, 1));
    }

    aml_append(dev, aml_name_decl("_CRS", crs));
    aml_append(scope, dev);
}

/*
 * Serial Port Console Redirection Table (SPCR)
 * Rev: 1.10
 */

ਸਥਿਰ ਖਾਲੀ
spcr_ਤਿਆਰੀ(GArray *ਸਾਰਣੀ_ਅੰਕੜੇ, BIOSLinker *linker, RISCVVirtState *s)
{
    ਅਚਲ ਅੱਖਰ name[] = ".";
    Acpi_Spcr_ਅੰਕੜੇ serial = {
        .interface_type = 0x12,       /* 16550 compatible */
        .ਅਧਾਰ_addr.id = AML_AS_SYSTEM_ਸਮ੍ਰਿਤੀ,
        .ਅਧਾਰ_addr.ਚੌੜਾਈ = 8,
        .ਅਧਾਰ_addr.ਵਿੱਥ = 0,
        .ਅਧਾਰ_addr.ਮਾਪ = 1,
        .ਅਧਾਰ_addr.addr = s->memmap[VIRT_UART0].ਅਧਾਰ,
        .ਰੁਕਾਵਟ_type = (1 << 4),/* Bit[4] RISC-V PLIC/APLIC */
        .pc_ਰੁਕਾਵਟ = 0,
        .ਰੁਕਾਵਟ = UART0_IRQ,
        .baud_ਦਰ = 7,            /* 15200 */
        .parity = 0,
        .stop_ਬਿੱਟ = 1,
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
        .namespace_string_ਲੰਮਾਈ = ਆਕਾਰ(name),
        .namespace_string_ਵਿੱਥ = 88,
    };

    build_spcr(ਸਾਰਣੀ_ਅੰਕੜੇ, linker, &serial, 4, s->oem_id, s->oem_ਸਾਰਣੀ_id,
               name);
}

/* RHCT Node[N] starts at offset 56 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RHCT_ਗੰਢ_ARRAY_ਵਿੱਥ 56

/*
 * ACPI spec, Revision 6.6
 * 5.2.37 RISC-V Hart Capabilities Table (RHCT)
 */
ਸਥਿਰ ਖਾਲੀ build_rhct(GArray *ਸਾਰਣੀ_ਅੰਕੜੇ,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ਅਚਲ CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    ਮਾਪ_t len, aligned_len;
    uint32_t isa_ਵਿੱਥ, ਅੰਕ_rhct_nodes, cmo_ਵਿੱਥ = 0;
    RISCVCPU *cpu = &s->soc[0].harts[0];
    uint32_t mmu_ਵਿੱਥ = 0;
    bool rv32 = riscv_cpu_is_32bit(cpu);
    g_autofree ਅੱਖਰ *isa = ਨਿਰਾ;

    Acpi_ਸਾਰਣੀ ਸਾਰਣੀ = { .sig = "RHCT", .rev = 1, .oem_id = s->oem_id,
                        .oem_ਸਾਰਣੀ_id = s->oem_ਸਾਰਣੀ_id };

    acpi_ਸਾਰਣੀ_begin(&ਸਾਰਣੀ, ਸਾਰਣੀ_ਅੰਕੜੇ);

    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x0, 4);   /* Reserved */

    /* Time Base Frequency */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ,
                              kvm_enabled() ?
                              kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                              RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ,
                              8);

    /* ISA + N hart info */
    ਅੰਕ_rhct_nodes = 1 + ms->smp.cpus;
    ਜੇ (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        ਅੰਕ_rhct_nodes++;
    }

    ਜੇ (!rv32 && cpu->cfg.max_satp_ਬਹੁਲਕ >= VM_1_10_SV39) {
        ਅੰਕ_rhct_nodes++;
    }

    /* Number of RHCT nodes*/
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਅੰਕ_rhct_nodes, 4);

    /* Offset to the RHCT node array */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, RHCT_ਗੰਢ_ARRAY_ਵਿੱਥ, 4);

    /* ISA String Node */
    isa_ਵਿੱਥ = ਸਾਰਣੀ_ਅੰਕੜੇ->len - ਸਾਰਣੀ.ਸਾਰਣੀ_ਵਿੱਥ;
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);   /* Type 0 */

    isa = riscv_isa_string(cpu);
    len = 8 + ਲੜੀ_ਲੰਬਾਈ(isa) + 1;
    aligned_len = (len % 2) ? (len + 1) : len;

    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, aligned_len, 2);   /* Length */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x1, 2);           /* Revision */

    /* ISA string length including NUL */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਲੜੀ_ਲੰਬਾਈ(isa) + 1, 2);
    g_array_append_vals(ਸਾਰਣੀ_ਅੰਕੜੇ, isa, ਲੜੀ_ਲੰਬਾਈ(isa) + 1);   /* ISA string */

    ਜੇ (aligned_len != len) {
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x0, 1);   /* Optional Padding */
    }

    /* CMO node */
    ਜੇ (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        cmo_ਵਿੱਥ = ਸਾਰਣੀ_ਅੰਕੜੇ->len - ਸਾਰਣੀ.ਸਾਰਣੀ_ਵਿੱਥ;
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 2);    /* Type */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 10, 2);   /* Length */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x1, 2);  /* Revision */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 1);    /* Reserved */

        /* CBOM block size */
        ਜੇ (cpu->cfg.cbom_blocksize) {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ,
                                      __builtin_ctz(cpu->cfg.cbom_blocksize),
                                      1);
        } ਨਹੀਂਤਾਂ {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 1);
        }

        /* CBOP block size */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 1);

        /* CBOZ block size */
        ਜੇ (cpu->cfg.cboz_blocksize) {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ,
                                      __builtin_ctz(cpu->cfg.cboz_blocksize),
                                      1);
        } ਨਹੀਂਤਾਂ {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 1);
        }
    }

    /* MMU node structure */
    ਜੇ (!rv32 && cpu->cfg.max_satp_ਬਹੁਲਕ >= VM_1_10_SV39) {
        mmu_ਵਿੱਥ = ਸਾਰਣੀ_ਅੰਕੜੇ->len - ਸਾਰਣੀ.ਸਾਰਣੀ_ਵਿੱਥ;
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 2, 2);    /* Type */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 8, 2);    /* Length */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x1, 2);  /* Revision */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 1);    /* Reserved */
        /* MMU Type */
        ਜੇ (cpu->cfg.max_satp_ਬਹੁਲਕ == VM_1_10_SV57) {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 2, 1);    /* Sv57 */
        } ਨਹੀਂਤਾਂ ਜੇ (cpu->cfg.max_satp_ਬਹੁਲਕ == VM_1_10_SV48) {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 1);    /* Sv48 */
        } ਨਹੀਂਤਾਂ ਜੇ (cpu->cfg.max_satp_ਬਹੁਲਕ == VM_1_10_SV39) {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 1);    /* Sv39 */
        } ਨਹੀਂਤਾਂ {
            g_assert_not_reached();
        }
    }

    /* Hart Info Node */
    ਲਈ (ਪੂਰਨ i = 0; i < arch_ids->len; i++) {
        len = 16;
        ਪੂਰਨ ਅੰਕ_offsets = 1;
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0xFFFF, 2);  /* Type */

        /* Length */
        ਜੇ (cmo_ਵਿੱਥ) {
            len += 4;
            ਅੰਕ_offsets++;
        }

        ਜੇ (mmu_ਵਿੱਥ) {
            len += 4;
            ਅੰਕ_offsets++;
        }

        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, len, 2);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x1, 2); /* Revision */
        /* Number of offsets */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਅੰਕ_offsets, 2);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, i, 4);   /* ACPI Processor UID */
        /* Offsets */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, isa_ਵਿੱਥ, 4);
        ਜੇ (cmo_ਵਿੱਥ) {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, cmo_ਵਿੱਥ, 4);
        }

        ਜੇ (mmu_ਵਿੱਥ) {
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, mmu_ਵਿੱਥ, 4);
        }
    }

    acpi_ਸਾਰਣੀ_ਸਿਰਾ(linker, &ਸਾਰਣੀ);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.9 Fixed ACPI Description Table (MADT)
 */
ਸਥਿਰ ਖਾਲੀ build_fadt_rev6(GArray *ਸਾਰਣੀ_ਅੰਕੜੇ,
                            BIOSLinker *linker,
                            RISCVVirtState *s,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ dsdt_tbl_ਵਿੱਥ)
{
    Acpi_Fadt_ਅੰਕੜੇ fadt = {
        .rev = 6,
        .minor_ver = 6,
        .flags = 1 << ACPI_FADT_F_HW_REDUCED_ACPI,
        .xdsdt_tbl_ਵਿੱਥ = &dsdt_tbl_ਵਿੱਥ,
    };

    build_fadt(ਸਾਰਣੀ_ਅੰਕੜੇ, linker, &fadt, s->oem_id, s->oem_ਸਾਰਣੀ_id);
}

/* DSDT */
ਸਥਿਰ ਖਾਲੀ build_dsdt(GArray *ਸਾਰਣੀ_ਅੰਕੜੇ,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    Aml *scope, *dsdt;
    MachineState *ms = MACHINE(s);
    uint8_t socket_ਗਿਣਤੀ;
    ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap = s->memmap;
    Acpi_ਸਾਰਣੀ ਸਾਰਣੀ = { .sig = "DSDT", .rev = 2, .oem_id = s->oem_id,
                        .oem_ਸਾਰਣੀ_id = s->oem_ਸਾਰਣੀ_id };


    acpi_ਸਾਰਣੀ_begin(&ਸਾਰਣੀ, ਸਾਰਣੀ_ਅੰਕੜੇ);
    dsdt = init_aml_allocator();

    /*
     * When booting the VM with UEFI, UEFI takes ownership of the RTC hardware.
     * While UEFI can use libfdt to disable the RTC device node in the DTB that
     * it passes to the OS, it cannot modify AML. Therefore, we won't generate
     * the RTC ACPI device at all when using UEFI.
     */
    scope = aml_scope("\\_SB");
    acpi_dsdt_ਜੋੜੋ_cpus(scope, s);

    fw_cfg_acpi_dsdt_ਜੋੜੋ(scope, &memmap[VIRT_FW_CFG]);

    socket_ਗਿਣਤੀ = riscv_socket_ਗਿਣਤੀ(ms);

    ਜੇ (s->aia_type == VIRT_AIA_TYPE_NONE) {
        acpi_dsdt_ਜੋੜੋ_plic_aplic(scope, socket_ਗਿਣਤੀ, s->ਅੰਕ_sources,
                                 memmap[VIRT_PLIC].ਅਧਾਰ,
                                 memmap[VIRT_PLIC].ਮਾਪ,
                                 "RSCV0001");
    } ਨਹੀਂਤਾਂ {
        acpi_dsdt_ਜੋੜੋ_plic_aplic(scope, socket_ਗਿਣਤੀ, s->ਅੰਕ_sources,
                                 memmap[VIRT_APLIC_S].ਅਧਾਰ,
                                 memmap[VIRT_APLIC_S].ਮਾਪ, "RSCV0002");
    }

    acpi_dsdt_ਜੋੜੋ_uart(scope, &memmap[VIRT_UART0], UART0_IRQ, 0);
    ਜੇ (s->uart1_present) {
        acpi_dsdt_ਜੋੜੋ_uart(scope, &memmap[VIRT_UART1], UART1_IRQ, 1);
    }

    ਜੇ (virt_is_iommu_sys_enabled(s)) {
        acpi_dsdt_ਜੋੜੋ_iommu_sys(scope, &memmap[VIRT_IOMMU_SYS], IOMMU_SYS_IRQ);
    }

    ਜੇ (socket_ਗਿਣਤੀ == 1) {
        virtio_acpi_dsdt_ਜੋੜੋ(scope, memmap[VIRT_VIRTIO].ਅਧਾਰ,
                             memmap[VIRT_VIRTIO].ਮਾਪ,
                             VIRTIO_IRQ, 0, VIRTIO_ਗਿਣਤੀ);
        acpi_dsdt_ਜੋੜੋ_gpex_host(scope, PCIE_IRQ);
    } ਨਹੀਂਤਾਂ ਜੇ (socket_ਗਿਣਤੀ == 2) {
        virtio_acpi_dsdt_ਜੋੜੋ(scope, memmap[VIRT_VIRTIO].ਅਧਾਰ,
                             memmap[VIRT_VIRTIO].ਮਾਪ,
                             VIRTIO_IRQ + s->ਅੰਕ_sources, 0,
                             VIRTIO_ਗਿਣਤੀ);
        acpi_dsdt_ਜੋੜੋ_gpex_host(scope, PCIE_IRQ + s->ਅੰਕ_sources);
    } ਨਹੀਂਤਾਂ {
        virtio_acpi_dsdt_ਜੋੜੋ(scope, memmap[VIRT_VIRTIO].ਅਧਾਰ,
                             memmap[VIRT_VIRTIO].ਮਾਪ,
                             VIRTIO_IRQ + s->ਅੰਕ_sources, 0,
                             VIRTIO_ਗਿਣਤੀ);
        acpi_dsdt_ਜੋੜੋ_gpex_host(scope, PCIE_IRQ + s->ਅੰਕ_sources * 2);
    }

    aml_append(dsdt, scope);

    /* copy AML table into ACPI tables blob and patch header there */
    g_array_append_vals(ਸਾਰਣੀ_ਅੰਕੜੇ, dsdt->buf->ਅੰਕੜੇ, dsdt->buf->len);

    acpi_ਸਾਰਣੀ_ਸਿਰਾ(linker, &ਸਾਰਣੀ);
    free_aml_allocator();
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.12 Multiple APIC Description Table (MADT)
 */
ਸਥਿਰ ਖਾਲੀ build_madt(GArray *ਸਾਰਣੀ_ਅੰਕੜੇ,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ਅਚਲ CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    uint8_t  ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ = imsic_ਅੰਕ_ਬਿੱਟ(riscv_socket_ਗਿਣਤੀ(ms));
    uint8_t  guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ = imsic_ਅੰਕ_ਬਿੱਟ(s->aia_guests + 1);
    uint16_t imsic_max_hart_ਪ੍ਰਤੀ_socket = 0;
    uint8_t  hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
    uint64_t aplic_addr;
    uint32_t gsi_ਅਧਾਰ;
    uint8_t  socket;

    ਲਈ (socket = 0; socket < riscv_socket_ਗਿਣਤੀ(ms); socket++) {
        ਜੇ (imsic_max_hart_ਪ੍ਰਤੀ_socket < s->soc[socket].ਅੰਕ_harts) {
            imsic_max_hart_ਪ੍ਰਤੀ_socket = s->soc[socket].ਅੰਕ_harts;
        }
    }

    hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ = imsic_ਅੰਕ_ਬਿੱਟ(imsic_max_hart_ਪ੍ਰਤੀ_socket);

    Acpi_ਸਾਰਣੀ ਸਾਰਣੀ = { .sig = "APIC", .rev = 7, .oem_id = s->oem_id,
                        .oem_ਸਾਰਣੀ_id = s->oem_ਸਾਰਣੀ_id };

    acpi_ਸਾਰਣੀ_begin(&ਸਾਰਣੀ, ਸਾਰਣੀ_ਅੰਕੜੇ);
    /* Local Interrupt Controller Address */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);   /* MADT Flags */

    /* RISC-V Local INTC structures per HART */
    ਲਈ (ਪੂਰਨ i = 0; i < arch_ids->len; i++) {
        riscv_acpi_madt_ਜੋੜੋ_rintc(i, arch_ids, ਸਾਰਣੀ_ਅੰਕੜੇ, s);
    }

    /* IMSIC */
    ਜੇ (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x19, 1);     /* Type */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 16, 1);       /* Length */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 1);        /* Version */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 1);        /* Reserved */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);        /* Flags */
        /* Number of supervisor mode Interrupt Identities */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, VIRT_IRQCHIP_ਅੰਕ_MSIS, 2);
        /* Number of guest mode Interrupt Identities */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, VIRT_IRQCHIP_ਅੰਕ_MSIS, 2);
        /* Guest Index Bits */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, guest_ਸੂਚਕਾਂਕ_ਬਿੱਟ, 1);
        /* Hart Index Bits */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, hart_ਸੂਚਕਾਂਕ_ਬਿੱਟ, 1);
        /* Group Index Bits */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਸਮੂਹ_ਸੂਚਕਾਂਕ_ਬਿੱਟ, 1);
        /* Group Index Shift */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, IMSIC_MMIO_ਸਮੂਹ_MIN_ਖਿਸਕਾਓ, 1);
    }

    ਜੇ (s->aia_type != VIRT_AIA_TYPE_NONE) {
        /* APLICs */
        ਲਈ (socket = 0; socket < riscv_socket_ਗਿਣਤੀ(ms); socket++) {
            aplic_addr = s->memmap[VIRT_APLIC_S].ਅਧਾਰ +
                             s->memmap[VIRT_APLIC_S].ਮਾਪ * socket;
            gsi_ਅਧਾਰ = s->ਅੰਕ_sources * socket;
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x1A, 1);    /* Type */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 36, 1);      /* Length */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 1);       /* Version */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, socket, 1);  /* APLIC ID */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);       /* Flags */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 8);       /* Hardware ID */
            /* Number of IDCs */
            ਜੇ (s->aia_type == VIRT_AIA_TYPE_APLIC) {
                build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ,
                                          s->soc[socket].ਅੰਕ_harts,
                                          2);
            } ਨਹੀਂਤਾਂ {
                build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);
            }
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, VIRT_IRQCHIP_ਅੰਕ_SOURCES, 2);
            /* Global System Interrupt Base */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, gsi_ਅਧਾਰ, 4);
            /* APLIC Address */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, aplic_addr, 8);
            /* APLIC size */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ,
                                      s->memmap[VIRT_APLIC_S].ਮਾਪ, 4);
        }
    } ਨਹੀਂਤਾਂ {
        /* PLICs */
        ਲਈ (socket = 0; socket < riscv_socket_ਗਿਣਤੀ(ms); socket++) {
            aplic_addr = s->memmap[VIRT_PLIC].ਅਧਾਰ +
                         s->memmap[VIRT_PLIC].ਮਾਪ * socket;
            gsi_ਅਧਾਰ = VIRT_IRQCHIP_ਅੰਕ_SOURCES * socket;
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0x1B, 1);   /* Type */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 36, 1);     /* Length */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 1);      /* Version */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, socket, 1); /* PLIC ID */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 8);      /* Hardware ID */
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ,
                                      VIRT_IRQCHIP_ਅੰਕ_SOURCES - 1, 2);
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);     /* Max Priority */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);     /* Flags */
            /* PLIC Size */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, s->memmap[VIRT_PLIC].ਮਾਪ, 4);
            /* PLIC Address */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, aplic_addr, 8);
            /* Global System Interrupt Vector Base */
            build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, gsi_ਅਧਾਰ, 4);
        }
    }

    acpi_ਸਾਰਣੀ_ਸਿਰਾ(linker, &ਸਾਰਣੀ);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ID_MAPPING_ਇੰਦਰਾਜ_ਮਾਪ        20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOMMU_ਇੰਦਰਾਜ_ਮਾਪ             40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RISCV_ਰੁਕਾਵਟ_WIRE_OFFSSET 40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਜੜ੍ਹ_COMPLEX_ਇੰਦਰਾਜ_ਮਾਪ      20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RIMT_ਗੰਢ_ਵਿੱਥ             48

/*
 * ID Mapping Structure
 */
ਸਥਿਰ ਖਾਲੀ build_rimt_id_mapping(GArray *ਸਾਰਣੀ_ਅੰਕੜੇ, uint32_t ਸਰੋਤ_id_ਅਧਾਰ,
                                  uint32_t ਅੰਕ_ids, uint32_t dest_id_ਅਧਾਰ)
{
    /* Source ID Base */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਸਰੋਤ_id_ਅਧਾਰ, 4);
    /* Number of IDs */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਅੰਕ_ids, 4);
    /* Destination Device ID Base */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਸਰੋਤ_id_ਅਧਾਰ, 4);
    /* Destination IOMMU Offset */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, dest_id_ਅਧਾਰ, 4);
    /* Flags */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);
}

ਬਣਤਰ AcpiRimtIdMapping {
    uint32_t ਸਰੋਤ_id_ਅਧਾਰ;
    uint32_t ਅੰਕ_ids;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ AcpiRimtIdMapping AcpiRimtIdMapping;

/* Build the rimt ID mapping to IOMMU for a given PCI host bridge */
ਸਥਿਰ ਪੂਰਨ rimt_host_bridges(Object *obj, ਖਾਲੀ *opaque)
{
    GArray *idmap_blob = opaque;

    ਜੇ (object_dynamic_cast(obj, TYPE_PCI_HOST_BRIDGE)) {
        PCIBus *bus = PCI_HOST_BRIDGE(obj)->bus;

        ਜੇ (bus && !pci_bus_bypass_iommu(bus)) {
            ਪੂਰਨ min_bus, max_bus;

            pci_bus_range(bus, &min_bus, &max_bus);

            AcpiRimtIdMapping idmap = {
                .ਸਰੋਤ_id_ਅਧਾਰ = min_bus << 8,
                .ਅੰਕ_ids = (max_bus - min_bus + 1) << 8,
            };
            g_array_append_val(idmap_blob, idmap);
        }
    }

    ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rimt_idmap_compare(gconstpointer a, gconstpointer b)
{
    AcpiRimtIdMapping *idmap_a = (AcpiRimtIdMapping *)a;
    AcpiRimtIdMapping *idmap_b = (AcpiRimtIdMapping *)b;

    ਵਾਪਸ idmap_a->ਸਰੋਤ_id_ਅਧਾਰ - idmap_b->ਸਰੋਤ_id_ਅਧਾਰ;
}

/*
 * RISC-V IO Mapping Table (RIMT)
 * https://github.com/riscv-non-isa/riscv-acpi-rimt/releases/download/v0.99/rimt-spec.pdf
 */
ਸਥਿਰ ਖਾਲੀ build_rimt(GArray *ਸਾਰਣੀ_ਅੰਕੜੇ, BIOSLinker *linker,
                       RISCVVirtState *s)
{
    ਪੂਰਨ i, nb_nodes, rc_mapping_ਗਿਣਤੀ;
    ਮਾਪ_t ਗੰਢ_ਮਾਪ, iommu_ਵਿੱਥ = 0;
    uint32_t id = 0;
    g_autoptr(GArray) iommu_idmaps = g_array_new(false, true,
                                                 ਆਕਾਰ(AcpiRimtIdMapping));

    Acpi_ਸਾਰਣੀ ਸਾਰਣੀ = { .sig = "RIMT", .rev = 1, .oem_id = s->oem_id,
                        .oem_ਸਾਰਣੀ_id = s->oem_ਸਾਰਣੀ_id };

    acpi_ਸਾਰਣੀ_begin(&ਸਾਰਣੀ, ਸਾਰਣੀ_ਅੰਕੜੇ);

    object_ਬੱਚਾ_foreach_recursive(object_get_ਜੜ੍ਹ(),
                                   rimt_host_bridges, iommu_idmaps);

    /* Sort the ID mapping  by Source ID Base*/
    g_array_ਛਾਂਟੋ(iommu_idmaps, rimt_idmap_compare);

    nb_nodes = 2; /* RC, IOMMU */
    rc_mapping_ਗਿਣਤੀ = iommu_idmaps->len;
    /* Number of RIMT Nodes */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, nb_nodes, 4);

    /* Offset to Array of RIMT Nodes */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, RIMT_ਗੰਢ_ਵਿੱਥ, 4);
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4); /* Reserved */

    iommu_ਵਿੱਥ = ਸਾਰਣੀ_ਅੰਕੜੇ->len - ਸਾਰਣੀ.ਸਾਰਣੀ_ਵਿੱਥ;
    /*  IOMMU Device Structure */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 1);         /* Type - IOMMU*/
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 1);         /* Revision */
    ਗੰਢ_ਮਾਪ =  IOMMU_ਇੰਦਰਾਜ_ਮਾਪ;
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਗੰਢ_ਮਾਪ, 2); /* Length */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);         /* Reserved */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, id++, 2);      /* ID */
    ਜੇ (virt_is_iommu_sys_enabled(s)) {
        /* Hardware ID */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 'R', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 'S', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 'C', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 'V', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '0', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '0', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '0', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '4', 1);
        /* Base Address */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ,
                                  s->memmap[VIRT_IOMMU_SYS].ਅਧਾਰ, 8);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);   /* Flags */
    } ਨਹੀਂਤਾਂ {
        /* Hardware ID */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '0', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '0', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '1', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '0', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '0', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '0', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '1', 1);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, '4', 1);

        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 8);   /* Base Address */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 4);   /* Flags */
    }

    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);       /* Proximity Domain */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);       /* PCI Segment number */
    /* PCIe B/D/F */
    ਜੇ (virt_is_iommu_sys_enabled(s)) {
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);
    } ਨਹੀਂਤਾਂ {
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, s->pci_iommu_bdf, 2);
    }
    /* Number of interrupt wires */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);
    /* Interrupt wire array offset */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, RISCV_ਰੁਕਾਵਟ_WIRE_OFFSSET, 2);

    /*  PCIe Root Complex Node */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 1);           /* Type */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 1);           /* Revision */
    ਗੰਢ_ਮਾਪ =  ਜੜ੍ਹ_COMPLEX_ਇੰਦਰਾਜ_ਮਾਪ +
                 ID_MAPPING_ਇੰਦਰਾਜ_ਮਾਪ * rc_mapping_ਗਿਣਤੀ;
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਗੰਢ_ਮਾਪ, 2);   /* Length */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);           /* Reserved */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, id++, 2);        /* ID */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4);           /* Flags */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);           /* Reserved */
    /* PCI Segment number */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);
    /* ID mapping array offset */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, ਜੜ੍ਹ_COMPLEX_ਇੰਦਰਾਜ_ਮਾਪ, 2);
    /* Number of ID mappings */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, rc_mapping_ਗਿਣਤੀ, 2);

    /* Output Reference */
    AcpiRimtIdMapping *range;

    /* ID mapping array */
    ਲਈ (i = 0; i < iommu_idmaps->len; i++) {
        range = &g_array_ਸੂਚਕਾਂਕ(iommu_idmaps, AcpiRimtIdMapping, i);
        ਜੇ (virt_is_iommu_sys_enabled(s)) {
            range->ਸਰੋਤ_id_ਅਧਾਰ = 0;
            range->ਅੰਕ_ids = 0x10000;
        } ਨਹੀਂਤਾਂ {
            range->ਸਰੋਤ_id_ਅਧਾਰ = s->pci_iommu_bdf + 1;
            range->ਅੰਕ_ids = 0xffff - s->pci_iommu_bdf;
        }
        build_rimt_id_mapping(ਸਾਰਣੀ_ਅੰਕੜੇ, range->ਸਰੋਤ_id_ਅਧਾਰ,
                              range->ਅੰਕ_ids, iommu_ਵਿੱਥ);
    }

    acpi_ਸਾਰਣੀ_ਸਿਰਾ(linker, &ਸਾਰਣੀ);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.16 System Resource Affinity Table (SRAT)
 */
ਸਥਿਰ ਖਾਲੀ
build_srat(GArray *ਸਾਰਣੀ_ਅੰਕੜੇ, BIOSLinker *linker, RISCVVirtState *vms)
{
    ਪੂਰਨ i;
    uint64_t mem_ਅਧਾਰ;
    MachineClass *mc = MACHINE_GET_CLASS(vms);
    MachineState *ms = MACHINE(vms);
    ਅਚਲ CPUArchIdList *cpu_list = mc->possible_cpu_arch_ids(ms);
    Acpi_ਸਾਰਣੀ ਸਾਰਣੀ = { .sig = "SRAT", .rev = 3, .oem_id = vms->oem_id,
                        .oem_ਸਾਰਣੀ_id = vms->oem_ਸਾਰਣੀ_id };

    acpi_ਸਾਰਣੀ_begin(&ਸਾਰਣੀ, ਸਾਰਣੀ_ਅੰਕੜੇ);
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1, 4); /* Reserved */
    build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 8); /* Reserved */

    ਲਈ (i = 0; i < cpu_list->len; ++i) {
        uint32_t nodeid = cpu_list->cpus[i].props.ਗੰਢ_id;
        /*
         * 5.2.16.8 RINTC Affinity Structure
         */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 7, 1);      /* Type */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 20, 1);     /* Length */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 2);        /* Reserved */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, nodeid, 4); /* Proximity Domain */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, i, 4); /* ACPI Processor UID */
        /* Flags, Table 5-70 */
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 1 /* Flags: Enabled */, 4);
        build_append_int_noprefix(ਸਾਰਣੀ_ਅੰਕੜੇ, 0, 4); /* Clock Domain */
    }

    mem_ਅਧਾਰ = vms->memmap[VIRT_DRAM].ਅਧਾਰ;
    ਲਈ (i = 0; i < ms->numa_state->ਅੰਕ_nodes; ++i) {
        ਜੇ (ms->numa_state->nodes[i].ਗੰਢ_mem > 0) {
            build_srat_ਸਮ੍ਰਿਤੀ(ਸਾਰਣੀ_ਅੰਕੜੇ, mem_ਅਧਾਰ,
                              ms->numa_state->nodes[i].ਗੰਢ_mem, i,
                              MEM_AFFINITY_ENABLED);
            mem_ਅਧਾਰ += ms->numa_state->nodes[i].ਗੰਢ_mem;
        }
    }

    acpi_ਸਾਰਣੀ_ਸਿਰਾ(linker, &ਸਾਰਣੀ);
}

ਸਥਿਰ ਖਾਲੀ virt_acpi_build(RISCVVirtState *s, AcpiBuildTables *tables)
{
    GArray *ਸਾਰਣੀ_offsets;
    ਬਿਨਾਨਿਸ਼ਾਨੀ dsdt, xsdt;
    GArray *tables_blob = tables->ਸਾਰਣੀ_ਅੰਕੜੇ;
    MachineState *ms = MACHINE(s);

    ਸਾਰਣੀ_offsets = g_array_new(false, true,
                                ਆਕਾਰ(uint32_t));

    bios_linker_loader_alloc(tables->linker,
                             ACPI_BUILD_ਸਾਰਣੀ_FILE, tables_blob,
                             64, false);

    /* DSDT is pointed to by FADT */
    dsdt = tables_blob->len;
    build_dsdt(tables_blob, tables->linker, s);

    /* FADT and others pointed to by XSDT */
    acpi_ਜੋੜੋ_ਸਾਰਣੀ(ਸਾਰਣੀ_offsets, tables_blob);
    build_fadt_rev6(tables_blob, tables->linker, s, dsdt);

    acpi_ਜੋੜੋ_ਸਾਰਣੀ(ਸਾਰਣੀ_offsets, tables_blob);
    build_madt(tables_blob, tables->linker, s);

    acpi_ਜੋੜੋ_ਸਾਰਣੀ(ਸਾਰਣੀ_offsets, tables_blob);
    build_rhct(tables_blob, tables->linker, s);

    ਜੇ (virt_is_iommu_sys_enabled(s) || s->pci_iommu_bdf) {
        acpi_ਜੋੜੋ_ਸਾਰਣੀ(ਸਾਰਣੀ_offsets, tables_blob);
        build_rimt(tables_blob, tables->linker, s);
    }

    acpi_ਜੋੜੋ_ਸਾਰਣੀ(ਸਾਰਣੀ_offsets, tables_blob);

    ਜੇ (ms->acpi_spcr_enabled) {
        spcr_ਤਿਆਰੀ(tables_blob, tables->linker, s);
    }

    acpi_ਜੋੜੋ_ਸਾਰਣੀ(ਸਾਰਣੀ_offsets, tables_blob);
    {
        AcpiMcfgInfo mcfg = {
           .ਅਧਾਰ = s->memmap[VIRT_PCIE_ECAM].ਅਧਾਰ,
           .ਮਾਪ = s->memmap[VIRT_PCIE_ECAM].ਮਾਪ,
        };
        build_mcfg(tables_blob, tables->linker, &mcfg, s->oem_id,
                   s->oem_ਸਾਰਣੀ_id);
    }

    ਜੇ (ms->numa_state->ਅੰਕ_nodes > 0) {
        acpi_ਜੋੜੋ_ਸਾਰਣੀ(ਸਾਰਣੀ_offsets, tables_blob);
        build_srat(tables_blob, tables->linker, s);
        ਜੇ (ms->numa_state->have_numa_ਦੂਰੀ) {
            acpi_ਜੋੜੋ_ਸਾਰਣੀ(ਸਾਰਣੀ_offsets, tables_blob);
            build_slit(tables_blob, tables->linker, ms, s->oem_id,
                       s->oem_ਸਾਰਣੀ_id);
        }
    }

    /* XSDT is pointed to by RSDP */
    xsdt = tables_blob->len;
    build_xsdt(tables_blob, tables->linker, ਸਾਰਣੀ_offsets, s->oem_id,
                s->oem_ਸਾਰਣੀ_id);

    /* RSDP is in FSEG memory, so allocate it separately */
    {
        Acpi_Rsdp_ਅੰਕੜੇ rsdp_ਅੰਕੜੇ = {
            .revision = 2,
            .oem_id = s->oem_id,
            .xsdt_tbl_ਵਿੱਥ = &xsdt,
            .rsdt_tbl_ਵਿੱਥ = ਨਿਰਾ,
        };
        build_rsdp(tables->rsdp, tables->linker, &rsdp_ਅੰਕੜੇ);
    }

    /*
     * The align size is 128, warn if 64k is not enough therefore
     * the align size could be resized.
     */
    ਜੇ (tables_blob->len > ACPI_BUILD_ਸਾਰਣੀ_ਮਾਪ / 2) {
        warn_report("ACPI table size %u exceeds %d bytes,"
                    " migration may not work",
                    tables_blob->len, ACPI_BUILD_ਸਾਰਣੀ_ਮਾਪ / 2);
        error_printf("Try removing some objects.");
    }

    acpi_align_ਮਾਪ(tables_blob, ACPI_BUILD_ਸਾਰਣੀ_ਮਾਪ);

    /* Clean up memory that's no longer used */
    g_array_free(ਸਾਰਣੀ_offsets, true);
}

ਸਥਿਰ ਖਾਲੀ acpi_ram_update(ਸਮ੍ਰਿਤੀ_Region *mr, GArray *ਅੰਕੜੇ)
{
    uint32_t ਮਾਪ = acpi_ਅੰਕੜੇ_len(ਅੰਕੜੇ);

    /*
     * Make sure RAM size is correct - in case it got changed
     * e.g. by migration
     */
    ਸਮ੍ਰਿਤੀ_region_ram_resize(mr, ਮਾਪ, &error_abort);

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਸਮ੍ਰਿਤੀ_region_get_ram_ptr(mr), ਅੰਕੜੇ->ਅੰਕੜੇ, ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_region_ਸੈੱਟ_dirty(mr, 0, ਮਾਪ);
}

ਸਥਿਰ ਖਾਲੀ virt_acpi_build_update(ਖਾਲੀ *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    AcpiBuildTables tables;

    /* No state to update or already patched? Nothing to do. */
    ਜੇ (!build_state || build_state->patched) {
        ਵਾਪਸ;
    }

    build_state->patched = true;

    acpi_build_tables_init(&tables);

    virt_acpi_build(RISCV_VIRT_MACHINE(qdev_get_machine()), &tables);

    acpi_ram_update(build_state->ਸਾਰਣੀ_mr, tables.ਸਾਰਣੀ_ਅੰਕੜੇ);
    acpi_ram_update(build_state->rsdp_mr, tables.rsdp);
    acpi_ram_update(build_state->linker_mr, tables.linker->cmd_blob);

    acpi_build_tables_cleanup(&tables, true);
}

ਸਥਿਰ ਖਾਲੀ virt_acpi_build_reset(ਖਾਲੀ *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    build_state->patched = false;
}

ਸਥਿਰ ਅਚਲ VMStateDescription vmstate_virt_acpi_build = {
    .name = "virt_acpi_build",
    .ਸੰਸਕਰਣ_id = 1,
    .minimum_ਸੰਸਕਰਣ_id = 1,
    .fields = (ਅਚਲ VMState_ਖੇਤਰ[]) {
        VMSTATE_BOOL(patched, AcpiBuildState),
        VMSTATE_ਸਿਰਾ_ਦਾ_LIST()
    },
};

ਖਾਲੀ virt_acpi_ਤਿਆਰੀ(RISCVVirtState *s)
{
    AcpiBuildTables tables;
    AcpiBuildState *build_state;

    build_state = g_malloc0(ਆਕਾਰ *build_state);

    acpi_build_tables_init(&tables);
    virt_acpi_build(s, &tables);

    /* Now expose it all to Guest */
    build_state->ਸਾਰਣੀ_mr = acpi_ਜੋੜੋ_rom_blob(virt_acpi_build_update,
                                              build_state, tables.ਸਾਰਣੀ_ਅੰਕੜੇ,
                                              ACPI_BUILD_ਸਾਰਣੀ_FILE);
    ਪ੍ਰਤਿਜ੍ਞਾ(build_state->ਸਾਰਣੀ_mr != ਨਿਰਾ);

    build_state->linker_mr = acpi_ਜੋੜੋ_rom_blob(virt_acpi_build_update,
                                               build_state,
                                               tables.linker->cmd_blob,
                                               ACPI_BUILD_LOADER_FILE);

    build_state->rsdp_mr = acpi_ਜੋੜੋ_rom_blob(virt_acpi_build_update,
                                             build_state, tables.rsdp,
                                             ACPI_BUILD_RSDP_FILE);

    qemu_ਰਜਿਸਟਰ_reset(virt_acpi_build_reset, build_state);
    virt_acpi_build_reset(build_state);
    vmstate_ਰਜਿਸਟਰ(ਨਿਰਾ, 0, &vmstate_virt_acpi_build, build_state);

    /*
     * Clean up tables but don't free the memory: we track it
     * in build_state.
     */
    acpi_build_tables_cleanup(&tables, false);
}
