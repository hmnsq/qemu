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

#شامل "qemu/osdep.h"
#شامل "hw/acpi/acpi-defs.h"
#شامل "hw/acpi/acpi.h"
#شامل "hw/acpi/aml-build.h"
#شامل "hw/acpi/pci.h"
#شامل "hw/acpi/utils.h"
#شامل "hw/intc/riscv_aclint.h"
#شامل "hw/nvram/fw_cfg_acpi.h"
#شامل "hw/pci-host/gpex.h"
#شامل "hw/riscv/virt.h"
#شامل "hw/riscv/numa.h"
#شامل "hw/virtio/virtio-acpi.h"
#شامل "kvm/kvm_riscv.h"
#شامل "migration/vmstate.h"
#شامل "qapi/error.h"
#شامل "qemu/error-report.h"
#شامل "system/kvm.h"
#شامل "system/reset.h"

#شامل "aia.h"

#تعریف_کرو ACPI_BUILD_جدول_سائز             0x20000
#تعریف_کرو ACPI_BUILD_INTC_ID(socket, اشاریہ) ((socket << 24) | (اشاریہ))

قسمتعریف ساخت AcpiBuildState {
    /* Copy of table in RAM (for patching) */
    حافظہ_Region *جدول_mr;
    حافظہ_Region *rsdp_mr;
    حافظہ_Region *linker_mr;
    /* Is table patched? */
    bool patched;
} AcpiBuildState;

ساکن خالی acpi_align_سائز(GArray *blob, غیرعلامتی align)
{
    /*
     * Align size to multiple of given size. This reduces the chance
     * we need to change size in the future (breaking cross version migration).
     */
    g_array_سیٹ_سائز(blob, باری_اوپر(acpi_ڈیٹا_len(blob), align));
}

ساکن خالی riscv_acpi_madt_جوڑو_rintc(uint32_t uid,
                                      ثابت CPUArchIdList *arch_ids,
                                      GArray *اندراج_2,
                                      RISCVVirtState *s)
{
    uint8_t  guest_اشاریہ_بٹ = imsic_عدد_بٹ(s->aia_guests + 1);
    uint64_t hart_id = arch_ids->cpus[uid].arch_id;
    uint32_t imsic_سائز, local_cpu_id, socket_id;
    uint64_t imsic_socket_addr, imsic_addr;
    MachineState *ms = MACHINE(s);

    socket_id = arch_ids->cpus[uid].props.گرہ_id;
    local_cpu_id = (arch_ids->cpus[uid].arch_id -
                    riscv_socket_پہلا_hartid(ms, socket_id)) %
                    riscv_socket_hart_گنتی(ms, socket_id);
    imsic_socket_addr = s->memmap[VIRT_IMSIC_S].بنیاد +
                        (socket_id * VIRT_IMSIC_گروہ_MAX_سائز);
    imsic_سائز = IMSIC_HART_سائز(guest_اشاریہ_بٹ);
    imsic_addr = imsic_socket_addr + local_cpu_id * imsic_سائز;
    build_append_int_noprefix(اندراج_2, 0x18, 1);       /* Type     */
    build_append_int_noprefix(اندراج_2, 36, 1);         /*لمبائی*/
    build_append_int_noprefix(اندراج_2, 1, 1);          /*نسخہ*/
    build_append_int_noprefix(اندراج_2, 0, 1);          /* Reserved */
    build_append_int_noprefix(اندراج_2, 0x1, 4);        /* Flags    */
    build_append_int_noprefix(اندراج_2, hart_id, 8);    /* Hart ID  */
    build_append_int_noprefix(اندراج_2, uid, 4);        /* ACPI Processor UID */
    /* External Interrupt Controller ID */
    اگر (s->aia_type == VIRT_AIA_TYPE_APLIC) {
        build_append_int_noprefix(اندراج_2,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.گرہ_id,
                                      local_cpu_id),
                                  4);
    } ورنہ اگر (s->aia_type == VIRT_AIA_TYPE_NONE) {
        build_append_int_noprefix(اندراج_2,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.گرہ_id,
                                      kvm_enabled() ?
                                      local_cpu_id :
                                      2 * local_cpu_id + 1),
                                  4);
    } ورنہ {
        build_append_int_noprefix(اندراج_2, 0, 4);
    }

    اگر (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC Base address */
        build_append_int_noprefix(اندراج_2, imsic_addr, 8);
        /* IMSIC Size */
        build_append_int_noprefix(اندراج_2, imsic_سائز, 4);
    } ورنہ {
        build_append_int_noprefix(اندراج_2, 0, 8);
        build_append_int_noprefix(اندراج_2, 0, 4);
    }
}

ساکن خالی acpi_dsdt_جوڑو_cpus(Aml *scope, RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ثابت CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);

    برائے (صحیح i = 0; i < arch_ids->len; i++) {
            Aml *dev;
            GArray *madt_buf = g_array_new(0, 1, 1);

            dev = aml_device("C%.03X", i);
            aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0007")));
            aml_append(dev, aml_name_decl("_UID",
                       aml_int(arch_ids->cpus[i].arch_id)));

            /* build _MAT object */
            riscv_acpi_madt_جوڑو_rintc(i, arch_ids, madt_buf, s);
            aml_append(dev, aml_name_decl("_MAT",
                                          aml_بفر(madt_buf->len,
                                          (uint8_t *)madt_buf->ڈیٹا)));
            g_array_free(madt_buf, true);

            aml_append(scope, dev);
    }
}

ساکن خالی acpi_dsdt_جوڑو_plic_aplic(Aml *scope, uint8_t socket_گنتی,
                                     uint16_t عدد_sources,
                                     uint64_t mmio_بنیاد, uint64_t mmio_سائز,
                                     ثابت حرف *hid)
{
    uint64_t plic_aplic_addr;
    uint32_t gsi_بنیاد;
    uint8_t  socket;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(عدد_sources <= 1023);

    برائے (socket = 0; socket < socket_گنتی; socket++) {
        plic_aplic_addr = mmio_بنیاد + mmio_سائز * socket;
        gsi_بنیاد = عدد_sources * socket;
        Aml *dev = aml_device("IC%.02X", socket);
        aml_append(dev, aml_name_decl("_HID", aml_string("%s", hid)));
        aml_append(dev, aml_name_decl("_UID", aml_int(socket)));
        aml_append(dev, aml_name_decl("_GSB", aml_int(gsi_بنیاد)));

        Aml *crs = aml_resource_template();
        aml_append(crs, aml_memory32_fixed(plic_aplic_addr, mmio_سائز,
                                           AML_READ_WRITE));
        aml_append(dev, aml_name_decl("_CRS", crs));
        aml_append(scope, dev);
    }
}

ساکن خالی
acpi_dsdt_جوڑو_uart(Aml *scope, ثابت Mem_Map_اندراج *uart_memmap,
                    uint32_t uart_irq, صحیح uartidx)
{
    Aml *dev = aml_device("COM%d", uartidx);
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0003")));
    aml_append(dev, aml_name_decl("_UID", aml_int(uartidx)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(uart_memmap->بنیاد,
                                         uart_memmap->سائز, AML_READ_WRITE));
    aml_append(crs,
                aml_مداخلت(AML_صارف, AML_LEVEL, AML_فعال_بلند,
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
ساکن خالی acpi_dsdt_جوڑو_iommu_sys(Aml *scope, ثابت Mem_Map_اندراج *iommu_memmap,
                                    uint32_t iommu_irq)
{
    uint32_t i;

    Aml *dev = aml_device("IMU0");
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0004")));
    aml_append(dev, aml_name_decl("_UID", aml_int(0)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(iommu_memmap->بنیاد,
                                       iommu_memmap->سائز, AML_READ_WRITE));
    برائے (i = iommu_irq; i < iommu_irq + 4; i++) {
        aml_append(crs, aml_مداخلت(AML_صارف, AML_کنارہ, AML_فعال_کم,
                                      AML_EXCLUSIVE, &i, 1));
    }

    aml_append(dev, aml_name_decl("_CRS", crs));
    aml_append(scope, dev);
}

/*
 * Serial Port Console Redirection Table (SPCR)
 * Rev: 1.10
 */

ساکن خالی
spcr_تیاری(GArray *جدول_ڈیٹا, BIOSLinker *linker, RISCVVirtState *s)
{
    ثابت حرف name[] = ".";
    Acpi_Spcr_ڈیٹا serial = {
        .interface_type = 0x12,       /* 16550 compatible */
        .بنیاد_addr.id = AML_AS_SYSTEM_حافظہ,
        .بنیاد_addr.چوڑائی = 8,
        .بنیاد_addr.انحراف = 0,
        .بنیاد_addr.سائز = 1,
        .بنیاد_addr.addr = s->memmap[VIRT_UART0].بنیاد,
        .مداخلت_type = (1 << 4),/* Bit[4] RISC-V PLIC/APLIC */
        .pc_مداخلت = 0,
        .مداخلت = UART0_IRQ,
        .baud_شرح = 7,            /* 15200 */
        .parity = 0,
        .stop_بٹ = 1,
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
        .namespace_string_لمبائی = حجم(name),
        .namespace_string_انحراف = 88,
    };

    build_spcr(جدول_ڈیٹا, linker, &serial, 4, s->oem_id, s->oem_جدول_id,
               name);
}

/* RHCT Node[N] starts at offset 56 */
#تعریف_کرو RHCT_گرہ_ARRAY_انحراف 56

/*
 * ACPI spec, Revision 6.6
 * 5.2.37 RISC-V Hart Capabilities Table (RHCT)
 */
ساکن خالی build_rhct(GArray *جدول_ڈیٹا,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ثابت CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    سائز_t len, aligned_len;
    uint32_t isa_انحراف, عدد_rhct_nodes, cmo_انحراف = 0;
    RISCVCPU *cpu = &s->soc[0].harts[0];
    uint32_t mmu_انحراف = 0;
    bool rv32 = riscv_cpu_is_32bit(cpu);
    g_autofree حرف *isa = تہی;

    Acpi_جدول جدول = { .sig = "RHCT", .rev = 1, .oem_id = s->oem_id,
                        .oem_جدول_id = s->oem_جدول_id };

    acpi_جدول_begin(&جدول, جدول_ڈیٹا);

    build_append_int_noprefix(جدول_ڈیٹا, 0x0, 4);   /* Reserved */

    /* Time Base Frequency */
    build_append_int_noprefix(جدول_ڈیٹا,
                              kvm_enabled() ?
                              kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                              RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ,
                              8);

    /* ISA + N hart info */
    عدد_rhct_nodes = 1 + ms->smp.cpus;
    اگر (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        عدد_rhct_nodes++;
    }

    اگر (!rv32 && cpu->cfg.max_satp_منوال >= VM_1_10_SV39) {
        عدد_rhct_nodes++;
    }

    /* Number of RHCT nodes*/
    build_append_int_noprefix(جدول_ڈیٹا, عدد_rhct_nodes, 4);

    /* Offset to the RHCT node array */
    build_append_int_noprefix(جدول_ڈیٹا, RHCT_گرہ_ARRAY_انحراف, 4);

    /* ISA String Node */
    isa_انحراف = جدول_ڈیٹا->len - جدول.جدول_انحراف;
    build_append_int_noprefix(جدول_ڈیٹا, 0, 2);   /* Type 0 */

    isa = riscv_isa_string(cpu);
    len = 8 + لڑی_لمبائی(isa) + 1;
    aligned_len = (len % 2) ? (len + 1) : len;

    build_append_int_noprefix(جدول_ڈیٹا, aligned_len, 2);   /* Length */
    build_append_int_noprefix(جدول_ڈیٹا, 0x1, 2);           /* Revision */

    /* ISA string length including NUL */
    build_append_int_noprefix(جدول_ڈیٹا, لڑی_لمبائی(isa) + 1, 2);
    g_array_append_vals(جدول_ڈیٹا, isa, لڑی_لمبائی(isa) + 1);   /* ISA string */

    اگر (aligned_len != len) {
        build_append_int_noprefix(جدول_ڈیٹا, 0x0, 1);   /* Optional Padding */
    }

    /* CMO node */
    اگر (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        cmo_انحراف = جدول_ڈیٹا->len - جدول.جدول_انحراف;
        build_append_int_noprefix(جدول_ڈیٹا, 1, 2);    /* Type */
        build_append_int_noprefix(جدول_ڈیٹا, 10, 2);   /* Length */
        build_append_int_noprefix(جدول_ڈیٹا, 0x1, 2);  /* Revision */
        build_append_int_noprefix(جدول_ڈیٹا, 0, 1);    /* Reserved */

        /* CBOM block size */
        اگر (cpu->cfg.cbom_blocksize) {
            build_append_int_noprefix(جدول_ڈیٹا,
                                      __builtin_ctz(cpu->cfg.cbom_blocksize),
                                      1);
        } ورنہ {
            build_append_int_noprefix(جدول_ڈیٹا, 0, 1);
        }

        /* CBOP block size */
        build_append_int_noprefix(جدول_ڈیٹا, 0, 1);

        /* CBOZ block size */
        اگر (cpu->cfg.cboz_blocksize) {
            build_append_int_noprefix(جدول_ڈیٹا,
                                      __builtin_ctz(cpu->cfg.cboz_blocksize),
                                      1);
        } ورنہ {
            build_append_int_noprefix(جدول_ڈیٹا, 0, 1);
        }
    }

    /* MMU node structure */
    اگر (!rv32 && cpu->cfg.max_satp_منوال >= VM_1_10_SV39) {
        mmu_انحراف = جدول_ڈیٹا->len - جدول.جدول_انحراف;
        build_append_int_noprefix(جدول_ڈیٹا, 2, 2);    /* Type */
        build_append_int_noprefix(جدول_ڈیٹا, 8, 2);    /* Length */
        build_append_int_noprefix(جدول_ڈیٹا, 0x1, 2);  /* Revision */
        build_append_int_noprefix(جدول_ڈیٹا, 0, 1);    /* Reserved */
        /* MMU Type */
        اگر (cpu->cfg.max_satp_منوال == VM_1_10_SV57) {
            build_append_int_noprefix(جدول_ڈیٹا, 2, 1);    /* Sv57 */
        } ورنہ اگر (cpu->cfg.max_satp_منوال == VM_1_10_SV48) {
            build_append_int_noprefix(جدول_ڈیٹا, 1, 1);    /* Sv48 */
        } ورنہ اگر (cpu->cfg.max_satp_منوال == VM_1_10_SV39) {
            build_append_int_noprefix(جدول_ڈیٹا, 0, 1);    /* Sv39 */
        } ورنہ {
            g_assert_not_reached();
        }
    }

    /* Hart Info Node */
    برائے (صحیح i = 0; i < arch_ids->len; i++) {
        len = 16;
        صحیح عدد_offsets = 1;
        build_append_int_noprefix(جدول_ڈیٹا, 0xFFFF, 2);  /* Type */

        /* Length */
        اگر (cmo_انحراف) {
            len += 4;
            عدد_offsets++;
        }

        اگر (mmu_انحراف) {
            len += 4;
            عدد_offsets++;
        }

        build_append_int_noprefix(جدول_ڈیٹا, len, 2);
        build_append_int_noprefix(جدول_ڈیٹا, 0x1, 2); /* Revision */
        /* Number of offsets */
        build_append_int_noprefix(جدول_ڈیٹا, عدد_offsets, 2);
        build_append_int_noprefix(جدول_ڈیٹا, i, 4);   /* ACPI Processor UID */
        /* Offsets */
        build_append_int_noprefix(جدول_ڈیٹا, isa_انحراف, 4);
        اگر (cmo_انحراف) {
            build_append_int_noprefix(جدول_ڈیٹا, cmo_انحراف, 4);
        }

        اگر (mmu_انحراف) {
            build_append_int_noprefix(جدول_ڈیٹا, mmu_انحراف, 4);
        }
    }

    acpi_جدول_انتہا(linker, &جدول);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.9 Fixed ACPI Description Table (MADT)
 */
ساکن خالی build_fadt_rev6(GArray *جدول_ڈیٹا,
                            BIOSLinker *linker,
                            RISCVVirtState *s,
                            غیرعلامتی dsdt_tbl_انحراف)
{
    Acpi_Fadt_ڈیٹا fadt = {
        .rev = 6,
        .minor_ver = 6,
        .flags = 1 << ACPI_FADT_F_HW_REDUCED_ACPI,
        .xdsdt_tbl_انحراف = &dsdt_tbl_انحراف,
    };

    build_fadt(جدول_ڈیٹا, linker, &fadt, s->oem_id, s->oem_جدول_id);
}

/* DSDT */
ساکن خالی build_dsdt(GArray *جدول_ڈیٹا,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    Aml *scope, *dsdt;
    MachineState *ms = MACHINE(s);
    uint8_t socket_گنتی;
    ثابت Mem_Map_اندراج *memmap = s->memmap;
    Acpi_جدول جدول = { .sig = "DSDT", .rev = 2, .oem_id = s->oem_id,
                        .oem_جدول_id = s->oem_جدول_id };


    acpi_جدول_begin(&جدول, جدول_ڈیٹا);
    dsdt = init_aml_allocator();

    /*
     * When booting the VM with UEFI, UEFI takes ownership of the RTC hardware.
     * While UEFI can use libfdt to disable the RTC device node in the DTB that
     * it passes to the OS, it cannot modify AML. Therefore, we won't generate
     * the RTC ACPI device at all when using UEFI.
     */
    scope = aml_scope("\\_SB");
    acpi_dsdt_جوڑو_cpus(scope, s);

    fw_cfg_acpi_dsdt_جوڑو(scope, &memmap[VIRT_FW_CFG]);

    socket_گنتی = riscv_socket_گنتی(ms);

    اگر (s->aia_type == VIRT_AIA_TYPE_NONE) {
        acpi_dsdt_جوڑو_plic_aplic(scope, socket_گنتی, s->عدد_sources,
                                 memmap[VIRT_PLIC].بنیاد,
                                 memmap[VIRT_PLIC].سائز,
                                 "RSCV0001");
    } ورنہ {
        acpi_dsdt_جوڑو_plic_aplic(scope, socket_گنتی, s->عدد_sources,
                                 memmap[VIRT_APLIC_S].بنیاد,
                                 memmap[VIRT_APLIC_S].سائز, "RSCV0002");
    }

    acpi_dsdt_جوڑو_uart(scope, &memmap[VIRT_UART0], UART0_IRQ, 0);
    اگر (s->uart1_present) {
        acpi_dsdt_جوڑو_uart(scope, &memmap[VIRT_UART1], UART1_IRQ, 1);
    }

    اگر (virt_is_iommu_sys_enabled(s)) {
        acpi_dsdt_جوڑو_iommu_sys(scope, &memmap[VIRT_IOMMU_SYS], IOMMU_SYS_IRQ);
    }

    اگر (socket_گنتی == 1) {
        virtio_acpi_dsdt_جوڑو(scope, memmap[VIRT_VIRTIO].بنیاد,
                             memmap[VIRT_VIRTIO].سائز,
                             VIRTIO_IRQ, 0, VIRTIO_گنتی);
        acpi_dsdt_جوڑو_gpex_host(scope, PCIE_IRQ);
    } ورنہ اگر (socket_گنتی == 2) {
        virtio_acpi_dsdt_جوڑو(scope, memmap[VIRT_VIRTIO].بنیاد,
                             memmap[VIRT_VIRTIO].سائز,
                             VIRTIO_IRQ + s->عدد_sources, 0,
                             VIRTIO_گنتی);
        acpi_dsdt_جوڑو_gpex_host(scope, PCIE_IRQ + s->عدد_sources);
    } ورنہ {
        virtio_acpi_dsdt_جوڑو(scope, memmap[VIRT_VIRTIO].بنیاد,
                             memmap[VIRT_VIRTIO].سائز,
                             VIRTIO_IRQ + s->عدد_sources, 0,
                             VIRTIO_گنتی);
        acpi_dsdt_جوڑو_gpex_host(scope, PCIE_IRQ + s->عدد_sources * 2);
    }

    aml_append(dsdt, scope);

    /* copy AML table into ACPI tables blob and patch header there */
    g_array_append_vals(جدول_ڈیٹا, dsdt->buf->ڈیٹا, dsdt->buf->len);

    acpi_جدول_انتہا(linker, &جدول);
    free_aml_allocator();
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.12 Multiple APIC Description Table (MADT)
 */
ساکن خالی build_madt(GArray *جدول_ڈیٹا,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ثابت CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    uint8_t  گروہ_اشاریہ_بٹ = imsic_عدد_بٹ(riscv_socket_گنتی(ms));
    uint8_t  guest_اشاریہ_بٹ = imsic_عدد_بٹ(s->aia_guests + 1);
    uint16_t imsic_max_hart_فی_socket = 0;
    uint8_t  hart_اشاریہ_بٹ;
    uint64_t aplic_addr;
    uint32_t gsi_بنیاد;
    uint8_t  socket;

    برائے (socket = 0; socket < riscv_socket_گنتی(ms); socket++) {
        اگر (imsic_max_hart_فی_socket < s->soc[socket].عدد_harts) {
            imsic_max_hart_فی_socket = s->soc[socket].عدد_harts;
        }
    }

    hart_اشاریہ_بٹ = imsic_عدد_بٹ(imsic_max_hart_فی_socket);

    Acpi_جدول جدول = { .sig = "APIC", .rev = 7, .oem_id = s->oem_id,
                        .oem_جدول_id = s->oem_جدول_id };

    acpi_جدول_begin(&جدول, جدول_ڈیٹا);
    /* Local Interrupt Controller Address */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 4);
    build_append_int_noprefix(جدول_ڈیٹا, 0, 4);   /* MADT Flags */

    /* RISC-V Local INTC structures per HART */
    برائے (صحیح i = 0; i < arch_ids->len; i++) {
        riscv_acpi_madt_جوڑو_rintc(i, arch_ids, جدول_ڈیٹا, s);
    }

    /* IMSIC */
    اگر (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC */
        build_append_int_noprefix(جدول_ڈیٹا, 0x19, 1);     /* Type */
        build_append_int_noprefix(جدول_ڈیٹا, 16, 1);       /* Length */
        build_append_int_noprefix(جدول_ڈیٹا, 1, 1);        /* Version */
        build_append_int_noprefix(جدول_ڈیٹا, 0, 1);        /* Reserved */
        build_append_int_noprefix(جدول_ڈیٹا, 0, 4);        /* Flags */
        /* Number of supervisor mode Interrupt Identities */
        build_append_int_noprefix(جدول_ڈیٹا, VIRT_IRQCHIP_عدد_MSIS, 2);
        /* Number of guest mode Interrupt Identities */
        build_append_int_noprefix(جدول_ڈیٹا, VIRT_IRQCHIP_عدد_MSIS, 2);
        /* Guest Index Bits */
        build_append_int_noprefix(جدول_ڈیٹا, guest_اشاریہ_بٹ, 1);
        /* Hart Index Bits */
        build_append_int_noprefix(جدول_ڈیٹا, hart_اشاریہ_بٹ, 1);
        /* Group Index Bits */
        build_append_int_noprefix(جدول_ڈیٹا, گروہ_اشاریہ_بٹ, 1);
        /* Group Index Shift */
        build_append_int_noprefix(جدول_ڈیٹا, IMSIC_MMIO_گروہ_MIN_سرکاؤ, 1);
    }

    اگر (s->aia_type != VIRT_AIA_TYPE_NONE) {
        /* APLICs */
        برائے (socket = 0; socket < riscv_socket_گنتی(ms); socket++) {
            aplic_addr = s->memmap[VIRT_APLIC_S].بنیاد +
                             s->memmap[VIRT_APLIC_S].سائز * socket;
            gsi_بنیاد = s->عدد_sources * socket;
            build_append_int_noprefix(جدول_ڈیٹا, 0x1A, 1);    /* Type */
            build_append_int_noprefix(جدول_ڈیٹا, 36, 1);      /* Length */
            build_append_int_noprefix(جدول_ڈیٹا, 1, 1);       /* Version */
            build_append_int_noprefix(جدول_ڈیٹا, socket, 1);  /* APLIC ID */
            build_append_int_noprefix(جدول_ڈیٹا, 0, 4);       /* Flags */
            build_append_int_noprefix(جدول_ڈیٹا, 0, 8);       /* Hardware ID */
            /* Number of IDCs */
            اگر (s->aia_type == VIRT_AIA_TYPE_APLIC) {
                build_append_int_noprefix(جدول_ڈیٹا,
                                          s->soc[socket].عدد_harts,
                                          2);
            } ورنہ {
                build_append_int_noprefix(جدول_ڈیٹا, 0, 2);
            }
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(جدول_ڈیٹا, VIRT_IRQCHIP_عدد_SOURCES, 2);
            /* Global System Interrupt Base */
            build_append_int_noprefix(جدول_ڈیٹا, gsi_بنیاد, 4);
            /* APLIC Address */
            build_append_int_noprefix(جدول_ڈیٹا, aplic_addr, 8);
            /* APLIC size */
            build_append_int_noprefix(جدول_ڈیٹا,
                                      s->memmap[VIRT_APLIC_S].سائز, 4);
        }
    } ورنہ {
        /* PLICs */
        برائے (socket = 0; socket < riscv_socket_گنتی(ms); socket++) {
            aplic_addr = s->memmap[VIRT_PLIC].بنیاد +
                         s->memmap[VIRT_PLIC].سائز * socket;
            gsi_بنیاد = VIRT_IRQCHIP_عدد_SOURCES * socket;
            build_append_int_noprefix(جدول_ڈیٹا, 0x1B, 1);   /* Type */
            build_append_int_noprefix(جدول_ڈیٹا, 36, 1);     /* Length */
            build_append_int_noprefix(جدول_ڈیٹا, 1, 1);      /* Version */
            build_append_int_noprefix(جدول_ڈیٹا, socket, 1); /* PLIC ID */
            build_append_int_noprefix(جدول_ڈیٹا, 0, 8);      /* Hardware ID */
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(جدول_ڈیٹا,
                                      VIRT_IRQCHIP_عدد_SOURCES - 1, 2);
            build_append_int_noprefix(جدول_ڈیٹا, 0, 2);     /* Max Priority */
            build_append_int_noprefix(جدول_ڈیٹا, 0, 4);     /* Flags */
            /* PLIC Size */
            build_append_int_noprefix(جدول_ڈیٹا, s->memmap[VIRT_PLIC].سائز, 4);
            /* PLIC Address */
            build_append_int_noprefix(جدول_ڈیٹا, aplic_addr, 8);
            /* Global System Interrupt Vector Base */
            build_append_int_noprefix(جدول_ڈیٹا, gsi_بنیاد, 4);
        }
    }

    acpi_جدول_انتہا(linker, &جدول);
}

#تعریف_کرو ID_MAPPING_اندراج_سائز        20
#تعریف_کرو IOMMU_اندراج_سائز             40
#تعریف_کرو RISCV_مداخلت_WIRE_OFFSSET 40
#تعریف_کرو جڑ_COMPLEX_اندراج_سائز      20
#تعریف_کرو RIMT_گرہ_انحراف             48

/*
 * ID Mapping Structure
 */
ساکن خالی build_rimt_id_mapping(GArray *جدول_ڈیٹا, uint32_t ماخذ_id_بنیاد,
                                  uint32_t عدد_ids, uint32_t dest_id_بنیاد)
{
    /* Source ID Base */
    build_append_int_noprefix(جدول_ڈیٹا, ماخذ_id_بنیاد, 4);
    /* Number of IDs */
    build_append_int_noprefix(جدول_ڈیٹا, عدد_ids, 4);
    /* Destination Device ID Base */
    build_append_int_noprefix(جدول_ڈیٹا, ماخذ_id_بنیاد, 4);
    /* Destination IOMMU Offset */
    build_append_int_noprefix(جدول_ڈیٹا, dest_id_بنیاد, 4);
    /* Flags */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 4);
}

ساخت AcpiRimtIdMapping {
    uint32_t ماخذ_id_بنیاد;
    uint32_t عدد_ids;
};
قسمتعریف ساخت AcpiRimtIdMapping AcpiRimtIdMapping;

/* Build the rimt ID mapping to IOMMU for a given PCI host bridge */
ساکن صحیح rimt_host_bridges(Object *obj, خالی *opaque)
{
    GArray *idmap_blob = opaque;

    اگر (object_dynamic_cast(obj, TYPE_PCI_HOST_BRIDGE)) {
        PCIBus *bus = PCI_HOST_BRIDGE(obj)->bus;

        اگر (bus && !pci_bus_bypass_iommu(bus)) {
            صحیح min_bus, max_bus;

            pci_bus_range(bus, &min_bus, &max_bus);

            AcpiRimtIdMapping idmap = {
                .ماخذ_id_بنیاد = min_bus << 8,
                .عدد_ids = (max_bus - min_bus + 1) << 8,
            };
            g_array_append_val(idmap_blob, idmap);
        }
    }

    واپس 0;
}

ساکن صحیح rimt_idmap_compare(gconstpointer a, gconstpointer b)
{
    AcpiRimtIdMapping *idmap_a = (AcpiRimtIdMapping *)a;
    AcpiRimtIdMapping *idmap_b = (AcpiRimtIdMapping *)b;

    واپس idmap_a->ماخذ_id_بنیاد - idmap_b->ماخذ_id_بنیاد;
}

/*
 * RISC-V IO Mapping Table (RIMT)
 * https://github.com/riscv-non-isa/riscv-acpi-rimt/releases/download/v0.99/rimt-spec.pdf
 */
ساکن خالی build_rimt(GArray *جدول_ڈیٹا, BIOSLinker *linker,
                       RISCVVirtState *s)
{
    صحیح i, nb_nodes, rc_mapping_گنتی;
    سائز_t گرہ_سائز, iommu_انحراف = 0;
    uint32_t id = 0;
    g_autoptr(GArray) iommu_idmaps = g_array_new(false, true,
                                                 حجم(AcpiRimtIdMapping));

    Acpi_جدول جدول = { .sig = "RIMT", .rev = 1, .oem_id = s->oem_id,
                        .oem_جدول_id = s->oem_جدول_id };

    acpi_جدول_begin(&جدول, جدول_ڈیٹا);

    object_بچہ_foreach_recursive(object_get_جڑ(),
                                   rimt_host_bridges, iommu_idmaps);

    /* Sort the ID mapping  by Source ID Base*/
    g_array_ترتیب_دو(iommu_idmaps, rimt_idmap_compare);

    nb_nodes = 2; /* RC, IOMMU */
    rc_mapping_گنتی = iommu_idmaps->len;
    /* Number of RIMT Nodes */
    build_append_int_noprefix(جدول_ڈیٹا, nb_nodes, 4);

    /* Offset to Array of RIMT Nodes */
    build_append_int_noprefix(جدول_ڈیٹا, RIMT_گرہ_انحراف, 4);
    build_append_int_noprefix(جدول_ڈیٹا, 0, 4); /* Reserved */

    iommu_انحراف = جدول_ڈیٹا->len - جدول.جدول_انحراف;
    /*  IOMMU Device Structure */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 1);         /* Type - IOMMU*/
    build_append_int_noprefix(جدول_ڈیٹا, 1, 1);         /* Revision */
    گرہ_سائز =  IOMMU_اندراج_سائز;
    build_append_int_noprefix(جدول_ڈیٹا, گرہ_سائز, 2); /* Length */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 2);         /* Reserved */
    build_append_int_noprefix(جدول_ڈیٹا, id++, 2);      /* ID */
    اگر (virt_is_iommu_sys_enabled(s)) {
        /* Hardware ID */
        build_append_int_noprefix(جدول_ڈیٹا, 'R', 1);
        build_append_int_noprefix(جدول_ڈیٹا, 'S', 1);
        build_append_int_noprefix(جدول_ڈیٹا, 'C', 1);
        build_append_int_noprefix(جدول_ڈیٹا, 'V', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '0', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '0', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '0', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '4', 1);
        /* Base Address */
        build_append_int_noprefix(جدول_ڈیٹا,
                                  s->memmap[VIRT_IOMMU_SYS].بنیاد, 8);
        build_append_int_noprefix(جدول_ڈیٹا, 0, 4);   /* Flags */
    } ورنہ {
        /* Hardware ID */
        build_append_int_noprefix(جدول_ڈیٹا, '0', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '0', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '1', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '0', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '0', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '0', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '1', 1);
        build_append_int_noprefix(جدول_ڈیٹا, '4', 1);

        build_append_int_noprefix(جدول_ڈیٹا, 0, 8);   /* Base Address */
        build_append_int_noprefix(جدول_ڈیٹا, 1, 4);   /* Flags */
    }

    build_append_int_noprefix(جدول_ڈیٹا, 0, 4);       /* Proximity Domain */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 2);       /* PCI Segment number */
    /* PCIe B/D/F */
    اگر (virt_is_iommu_sys_enabled(s)) {
        build_append_int_noprefix(جدول_ڈیٹا, 0, 2);
    } ورنہ {
        build_append_int_noprefix(جدول_ڈیٹا, s->pci_iommu_bdf, 2);
    }
    /* Number of interrupt wires */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 2);
    /* Interrupt wire array offset */
    build_append_int_noprefix(جدول_ڈیٹا, RISCV_مداخلت_WIRE_OFFSSET, 2);

    /*  PCIe Root Complex Node */
    build_append_int_noprefix(جدول_ڈیٹا, 1, 1);           /* Type */
    build_append_int_noprefix(جدول_ڈیٹا, 1, 1);           /* Revision */
    گرہ_سائز =  جڑ_COMPLEX_اندراج_سائز +
                 ID_MAPPING_اندراج_سائز * rc_mapping_گنتی;
    build_append_int_noprefix(جدول_ڈیٹا, گرہ_سائز, 2);   /* Length */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 2);           /* Reserved */
    build_append_int_noprefix(جدول_ڈیٹا, id++, 2);        /* ID */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 4);           /* Flags */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 2);           /* Reserved */
    /* PCI Segment number */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 2);
    /* ID mapping array offset */
    build_append_int_noprefix(جدول_ڈیٹا, جڑ_COMPLEX_اندراج_سائز, 2);
    /* Number of ID mappings */
    build_append_int_noprefix(جدول_ڈیٹا, rc_mapping_گنتی, 2);

    /* Output Reference */
    AcpiRimtIdMapping *range;

    /* ID mapping array */
    برائے (i = 0; i < iommu_idmaps->len; i++) {
        range = &g_array_اشاریہ(iommu_idmaps, AcpiRimtIdMapping, i);
        اگر (virt_is_iommu_sys_enabled(s)) {
            range->ماخذ_id_بنیاد = 0;
            range->عدد_ids = 0x10000;
        } ورنہ {
            range->ماخذ_id_بنیاد = s->pci_iommu_bdf + 1;
            range->عدد_ids = 0xffff - s->pci_iommu_bdf;
        }
        build_rimt_id_mapping(جدول_ڈیٹا, range->ماخذ_id_بنیاد,
                              range->عدد_ids, iommu_انحراف);
    }

    acpi_جدول_انتہا(linker, &جدول);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.16 System Resource Affinity Table (SRAT)
 */
ساکن خالی
build_srat(GArray *جدول_ڈیٹا, BIOSLinker *linker, RISCVVirtState *vms)
{
    صحیح i;
    uint64_t mem_بنیاد;
    MachineClass *mc = MACHINE_GET_CLASS(vms);
    MachineState *ms = MACHINE(vms);
    ثابت CPUArchIdList *cpu_list = mc->possible_cpu_arch_ids(ms);
    Acpi_جدول جدول = { .sig = "SRAT", .rev = 3, .oem_id = vms->oem_id,
                        .oem_جدول_id = vms->oem_جدول_id };

    acpi_جدول_begin(&جدول, جدول_ڈیٹا);
    build_append_int_noprefix(جدول_ڈیٹا, 1, 4); /* Reserved */
    build_append_int_noprefix(جدول_ڈیٹا, 0, 8); /* Reserved */

    برائے (i = 0; i < cpu_list->len; ++i) {
        uint32_t nodeid = cpu_list->cpus[i].props.گرہ_id;
        /*
         * 5.2.16.8 RINTC Affinity Structure
         */
        build_append_int_noprefix(جدول_ڈیٹا, 7, 1);      /* Type */
        build_append_int_noprefix(جدول_ڈیٹا, 20, 1);     /* Length */
        build_append_int_noprefix(جدول_ڈیٹا, 0, 2);        /* Reserved */
        build_append_int_noprefix(جدول_ڈیٹا, nodeid, 4); /* Proximity Domain */
        build_append_int_noprefix(جدول_ڈیٹا, i, 4); /* ACPI Processor UID */
        /* Flags, Table 5-70 */
        build_append_int_noprefix(جدول_ڈیٹا, 1 /* Flags: Enabled */, 4);
        build_append_int_noprefix(جدول_ڈیٹا, 0, 4); /* Clock Domain */
    }

    mem_بنیاد = vms->memmap[VIRT_DRAM].بنیاد;
    برائے (i = 0; i < ms->numa_state->عدد_nodes; ++i) {
        اگر (ms->numa_state->nodes[i].گرہ_mem > 0) {
            build_srat_حافظہ(جدول_ڈیٹا, mem_بنیاد,
                              ms->numa_state->nodes[i].گرہ_mem, i,
                              MEM_AFFINITY_ENABLED);
            mem_بنیاد += ms->numa_state->nodes[i].گرہ_mem;
        }
    }

    acpi_جدول_انتہا(linker, &جدول);
}

ساکن خالی virt_acpi_build(RISCVVirtState *s, AcpiBuildTables *tables)
{
    GArray *جدول_offsets;
    غیرعلامتی dsdt, xsdt;
    GArray *tables_blob = tables->جدول_ڈیٹا;
    MachineState *ms = MACHINE(s);

    جدول_offsets = g_array_new(false, true,
                                حجم(uint32_t));

    bios_linker_loader_alloc(tables->linker,
                             ACPI_BUILD_جدول_FILE, tables_blob,
                             64, false);

    /* DSDT is pointed to by FADT */
    dsdt = tables_blob->len;
    build_dsdt(tables_blob, tables->linker, s);

    /* FADT and others pointed to by XSDT */
    acpi_جوڑو_جدول(جدول_offsets, tables_blob);
    build_fadt_rev6(tables_blob, tables->linker, s, dsdt);

    acpi_جوڑو_جدول(جدول_offsets, tables_blob);
    build_madt(tables_blob, tables->linker, s);

    acpi_جوڑو_جدول(جدول_offsets, tables_blob);
    build_rhct(tables_blob, tables->linker, s);

    اگر (virt_is_iommu_sys_enabled(s) || s->pci_iommu_bdf) {
        acpi_جوڑو_جدول(جدول_offsets, tables_blob);
        build_rimt(tables_blob, tables->linker, s);
    }

    acpi_جوڑو_جدول(جدول_offsets, tables_blob);

    اگر (ms->acpi_spcr_enabled) {
        spcr_تیاری(tables_blob, tables->linker, s);
    }

    acpi_جوڑو_جدول(جدول_offsets, tables_blob);
    {
        AcpiMcfgInfo mcfg = {
           .بنیاد = s->memmap[VIRT_PCIE_ECAM].بنیاد,
           .سائز = s->memmap[VIRT_PCIE_ECAM].سائز,
        };
        build_mcfg(tables_blob, tables->linker, &mcfg, s->oem_id,
                   s->oem_جدول_id);
    }

    اگر (ms->numa_state->عدد_nodes > 0) {
        acpi_جوڑو_جدول(جدول_offsets, tables_blob);
        build_srat(tables_blob, tables->linker, s);
        اگر (ms->numa_state->have_numa_فاصلہ) {
            acpi_جوڑو_جدول(جدول_offsets, tables_blob);
            build_slit(tables_blob, tables->linker, ms, s->oem_id,
                       s->oem_جدول_id);
        }
    }

    /* XSDT is pointed to by RSDP */
    xsdt = tables_blob->len;
    build_xsdt(tables_blob, tables->linker, جدول_offsets, s->oem_id,
                s->oem_جدول_id);

    /* RSDP is in FSEG memory, so allocate it separately */
    {
        Acpi_Rsdp_ڈیٹا rsdp_ڈیٹا = {
            .revision = 2,
            .oem_id = s->oem_id,
            .xsdt_tbl_انحراف = &xsdt,
            .rsdt_tbl_انحراف = تہی,
        };
        build_rsdp(tables->rsdp, tables->linker, &rsdp_ڈیٹا);
    }

    /*
     * The align size is 128, warn if 64k is not enough therefore
     * the align size could be resized.
     */
    اگر (tables_blob->len > ACPI_BUILD_جدول_سائز / 2) {
        warn_report("ACPI table size %u exceeds %d bytes,"
                    " migration may not work",
                    tables_blob->len, ACPI_BUILD_جدول_سائز / 2);
        error_printf("Try removing some objects.");
    }

    acpi_align_سائز(tables_blob, ACPI_BUILD_جدول_سائز);

    /* Clean up memory that's no longer used */
    g_array_free(جدول_offsets, true);
}

ساکن خالی acpi_ram_update(حافظہ_Region *mr, GArray *ڈیٹا)
{
    uint32_t سائز = acpi_ڈیٹا_len(ڈیٹا);

    /*
     * Make sure RAM size is correct - in case it got changed
     * e.g. by migration
     */
    حافظہ_region_ram_resize(mr, سائز, &error_abort);

    حافظہ_نقل(حافظہ_region_get_ram_ptr(mr), ڈیٹا->ڈیٹا, سائز);
    حافظہ_region_سیٹ_dirty(mr, 0, سائز);
}

ساکن خالی virt_acpi_build_update(خالی *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    AcpiBuildTables tables;

    /* No state to update or already patched? Nothing to do. */
    اگر (!build_state || build_state->patched) {
        واپس;
    }

    build_state->patched = true;

    acpi_build_tables_init(&tables);

    virt_acpi_build(RISCV_VIRT_MACHINE(qdev_get_machine()), &tables);

    acpi_ram_update(build_state->جدول_mr, tables.جدول_ڈیٹا);
    acpi_ram_update(build_state->rsdp_mr, tables.rsdp);
    acpi_ram_update(build_state->linker_mr, tables.linker->cmd_blob);

    acpi_build_tables_cleanup(&tables, true);
}

ساکن خالی virt_acpi_build_reset(خالی *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    build_state->patched = false;
}

ساکن ثابت VMStateDescription vmstate_virt_acpi_build = {
    .name = "virt_acpi_build",
    .نسخہ_id = 1,
    .minimum_نسخہ_id = 1,
    .fields = (ثابت VMState_میدان[]) {
        VMSTATE_BOOL(patched, AcpiBuildState),
        VMSTATE_انتہا_کا_LIST()
    },
};

خالی virt_acpi_تیاری(RISCVVirtState *s)
{
    AcpiBuildTables tables;
    AcpiBuildState *build_state;

    build_state = g_malloc0(حجم *build_state);

    acpi_build_tables_init(&tables);
    virt_acpi_build(s, &tables);

    /* Now expose it all to Guest */
    build_state->جدول_mr = acpi_جوڑو_rom_blob(virt_acpi_build_update,
                                              build_state, tables.جدول_ڈیٹا,
                                              ACPI_BUILD_جدول_FILE);
    أكد(build_state->جدول_mr != تہی);

    build_state->linker_mr = acpi_جوڑو_rom_blob(virt_acpi_build_update,
                                               build_state,
                                               tables.linker->cmd_blob,
                                               ACPI_BUILD_LOADER_FILE);

    build_state->rsdp_mr = acpi_جوڑو_rom_blob(virt_acpi_build_update,
                                             build_state, tables.rsdp,
                                             ACPI_BUILD_RSDP_FILE);

    qemu_رجسٹر_reset(virt_acpi_build_reset, build_state);
    virt_acpi_build_reset(build_state);
    vmstate_رجسٹر(تہی, 0, &vmstate_virt_acpi_build, build_state);

    /*
     * Clean up tables but don't free the memory: we track it
     * in build_state.
     */
    acpi_build_tables_cleanup(&tables, false);
}
