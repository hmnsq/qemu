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

#ضمن "qemu/osdep.h"
#ضمن "hw/acpi/acpi-defs.h"
#ضمن "hw/acpi/acpi.h"
#ضمن "hw/acpi/aml-build.h"
#ضمن "hw/acpi/pci.h"
#ضمن "hw/acpi/utils.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/nvram/fw_cfg_acpi.h"
#ضمن "hw/pci-host/gpex.h"
#ضمن "hw/riscv/virt.h"
#ضمن "hw/riscv/numa.h"
#ضمن "hw/virtio/virtio-acpi.h"
#ضمن "kvm/kvm_riscv.h"
#ضمن "migration/vmstate.h"
#ضمن "qapi/error.h"
#ضمن "qemu/error-report.h"
#ضمن "system/kvm.h"
#ضمن "system/reset.h"

#ضمن "aia.h"

#تعریف_کن ACPI_BUILD_جدول_اندازه             0x20000
#تعریف_کن ACPI_BUILD_INTC_ID(socket, شاخص) ((socket << 24) | (شاخص))

تعريفنوع بنية AcpiBuildState {
    /* Copy of table in RAM (for patching) */
    حافظه_Region *جدول_mr;
    حافظه_Region *rsdp_mr;
    حافظه_Region *linker_mr;
    /* Is table patched? */
    bool patched;
} AcpiBuildState;

ساكن فراغ acpi_align_اندازه(GArray *blob, بیعلامت align)
{
    /*
     * Align size to multiple of given size. This reduces the chance
     * we need to change size in the future (breaking cross version migration).
     */
    g_array_مجموعه_اندازه(blob, دور_به_بالا(acpi_داده_len(blob), align));
}

ساكن فراغ riscv_acpi_madt_جمع_کن_rintc(uint32_t uid,
                                      ثابت CPUArchIdList *arch_ids,
                                      GArray *مدخل,
                                      RISCVVirtState *s)
{
    uint8_t  guest_شاخص_بیتها = imsic_رقم_بیتها(s->aia_guests + 1);
    uint64_t hart_id = arch_ids->cpus[uid].arch_id;
    uint32_t imsic_اندازه, local_cpu_id, socket_id;
    uint64_t imsic_socket_addr, imsic_addr;
    MachineState *ms = MACHINE(s);

    socket_id = arch_ids->cpus[uid].props.گره_id;
    local_cpu_id = (arch_ids->cpus[uid].arch_id -
                    riscv_socket_اولی_hartid(ms, socket_id)) %
                    riscv_socket_hart_شمار(ms, socket_id);
    imsic_socket_addr = s->memmap[VIRT_IMSIC_S].پایه +
                        (socket_id * VIRT_IMSIC_گروه_MAX_اندازه);
    imsic_اندازه = IMSIC_HART_اندازه(guest_شاخص_بیتها);
    imsic_addr = imsic_socket_addr + local_cpu_id * imsic_اندازه;
    build_append_int_noprefix(مدخل, 0x18, 1);       /* Type     */
    build_append_int_noprefix(مدخل, 36, 1);         /*درازی*/
    build_append_int_noprefix(مدخل, 1, 1);          /*نسخه*/
    build_append_int_noprefix(مدخل, 0, 1);          /* Reserved */
    build_append_int_noprefix(مدخل, 0x1, 4);        /* Flags    */
    build_append_int_noprefix(مدخل, hart_id, 8);    /* Hart ID  */
    build_append_int_noprefix(مدخل, uid, 4);        /* ACPI Processor UID */
    /* External Interrupt Controller ID */
    إذا (s->aia_type == VIRT_AIA_TYPE_APLIC) {
        build_append_int_noprefix(مدخل,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.گره_id,
                                      local_cpu_id),
                                  4);
    } وإلا إذا (s->aia_type == VIRT_AIA_TYPE_NONE) {
        build_append_int_noprefix(مدخل,
                                  ACPI_BUILD_INTC_ID(
                                      arch_ids->cpus[uid].props.گره_id,
                                      kvm_enabled() ?
                                      local_cpu_id :
                                      2 * local_cpu_id + 1),
                                  4);
    } وإلا {
        build_append_int_noprefix(مدخل, 0, 4);
    }

    إذا (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC Base address */
        build_append_int_noprefix(مدخل, imsic_addr, 8);
        /* IMSIC Size */
        build_append_int_noprefix(مدخل, imsic_اندازه, 4);
    } وإلا {
        build_append_int_noprefix(مدخل, 0, 8);
        build_append_int_noprefix(مدخل, 0, 4);
    }
}

ساكن فراغ acpi_dsdt_جمع_کن_cpus(Aml *scope, RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ثابت CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);

    لكل (صحيح i = 0; i < arch_ids->len; i++) {
            Aml *dev;
            GArray *madt_buf = g_array_new(0, 1, 1);

            dev = aml_device("C%.03X", i);
            aml_append(dev, aml_name_decl("_HID", aml_string("ACPI0007")));
            aml_append(dev, aml_name_decl("_UID",
                       aml_int(arch_ids->cpus[i].arch_id)));

            /* build _MAT object */
            riscv_acpi_madt_جمع_کن_rintc(i, arch_ids, madt_buf, s);
            aml_append(dev, aml_name_decl("_MAT",
                                          aml_میانگیر(madt_buf->len,
                                          (uint8_t *)madt_buf->داده)));
            g_array_free(madt_buf, true);

            aml_append(scope, dev);
    }
}

ساكن فراغ acpi_dsdt_جمع_کن_plic_aplic(Aml *scope, uint8_t socket_شمار,
                                     uint16_t رقم_sources,
                                     uint64_t mmio_پایه, uint64_t mmio_اندازه,
                                     ثابت حرف *hid)
{
    uint64_t plic_aplic_addr;
    uint32_t gsi_پایه;
    uint8_t  socket;

    /* The RISC-V Advanced Interrupt Architecture, Chapter 1.2. Limits */
    g_assert(رقم_sources <= 1023);

    لكل (socket = 0; socket < socket_شمار; socket++) {
        plic_aplic_addr = mmio_پایه + mmio_اندازه * socket;
        gsi_پایه = رقم_sources * socket;
        Aml *dev = aml_device("IC%.02X", socket);
        aml_append(dev, aml_name_decl("_HID", aml_string("%s", hid)));
        aml_append(dev, aml_name_decl("_UID", aml_int(socket)));
        aml_append(dev, aml_name_decl("_GSB", aml_int(gsi_پایه)));

        Aml *crs = aml_resource_template();
        aml_append(crs, aml_memory32_fixed(plic_aplic_addr, mmio_اندازه,
                                           AML_READ_WRITE));
        aml_append(dev, aml_name_decl("_CRS", crs));
        aml_append(scope, dev);
    }
}

ساكن فراغ
acpi_dsdt_جمع_کن_uart(Aml *scope, ثابت Mem_Map_مدخل *uart_memmap,
                    uint32_t uart_irq, صحيح uartidx)
{
    Aml *dev = aml_device("COM%d", uartidx);
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0003")));
    aml_append(dev, aml_name_decl("_UID", aml_int(uartidx)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(uart_memmap->پایه,
                                         uart_memmap->اندازه, AML_READ_WRITE));
    aml_append(crs,
                aml_وقفه(AML_مصرفکننده, AML_LEVEL, AML_فعال_بالا,
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
ساكن فراغ acpi_dsdt_جمع_کن_iommu_sys(Aml *scope, ثابت Mem_Map_مدخل *iommu_memmap,
                                    uint32_t iommu_irq)
{
    uint32_t i;

    Aml *dev = aml_device("IMU0");
    aml_append(dev, aml_name_decl("_HID", aml_string("RSCV0004")));
    aml_append(dev, aml_name_decl("_UID", aml_int(0)));

    Aml *crs = aml_resource_template();
    aml_append(crs, aml_memory32_fixed(iommu_memmap->پایه,
                                       iommu_memmap->اندازه, AML_READ_WRITE));
    لكل (i = iommu_irq; i < iommu_irq + 4; i++) {
        aml_append(crs, aml_وقفه(AML_مصرفکننده, AML_یال, AML_فعال_کم,
                                      AML_EXCLUSIVE, &i, 1));
    }

    aml_append(dev, aml_name_decl("_CRS", crs));
    aml_append(scope, dev);
}

/*
 * Serial Port Console Redirection Table (SPCR)
 * Rev: 1.10
 */

ساكن فراغ
spcr_راهاندازی(GArray *جدول_داده, BIOSLinker *linker, RISCVVirtState *s)
{
    ثابت حرف name[] = ".";
    Acpi_Spcr_داده serial = {
        .interface_type = 0x12,       /* 16550 compatible */
        .پایه_addr.id = AML_AS_SYSTEM_حافظه,
        .پایه_addr.عرض = 8,
        .پایه_addr.جابه_جایی = 0,
        .پایه_addr.اندازه = 1,
        .پایه_addr.addr = s->memmap[VIRT_UART0].پایه,
        .وقفه_type = (1 << 4),/* Bit[4] RISC-V PLIC/APLIC */
        .pc_وقفه = 0,
        .وقفه = UART0_IRQ,
        .baud_نرخ = 7,            /* 15200 */
        .parity = 0,
        .stop_بیتها = 1,
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
        .namespace_string_درازی = حجم(name),
        .namespace_string_جابه_جایی = 88,
    };

    build_spcr(جدول_داده, linker, &serial, 4, s->oem_id, s->oem_جدول_id,
               name);
}

/* RHCT Node[N] starts at offset 56 */
#تعریف_کن RHCT_گره_ARRAY_جابه_جایی 56

/*
 * ACPI spec, Revision 6.6
 * 5.2.37 RISC-V Hart Capabilities Table (RHCT)
 */
ساكن فراغ build_rhct(GArray *جدول_داده,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ثابت CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    اندازه_t len, aligned_len;
    uint32_t isa_جابه_جایی, رقم_rhct_nodes, cmo_جابه_جایی = 0;
    RISCVCPU *cpu = &s->soc[0].harts[0];
    uint32_t mmu_جابه_جایی = 0;
    bool rv32 = riscv_cpu_is_32bit(cpu);
    g_autofree حرف *isa = عدم;

    Acpi_جدول جدول = { .sig = "RHCT", .rev = 1, .oem_id = s->oem_id,
                        .oem_جدول_id = s->oem_جدول_id };

    acpi_جدول_begin(&جدول, جدول_داده);

    build_append_int_noprefix(جدول_داده, 0x0, 4);   /* Reserved */

    /* Time Base Frequency */
    build_append_int_noprefix(جدول_داده,
                              kvm_enabled() ?
                              kvm_riscv_get_timebase_frequency(&s->soc->harts[0]) :
                              RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ,
                              8);

    /* ISA + N hart info */
    رقم_rhct_nodes = 1 + ms->smp.cpus;
    إذا (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        رقم_rhct_nodes++;
    }

    إذا (!rv32 && cpu->cfg.max_satp_نما >= VM_1_10_SV39) {
        رقم_rhct_nodes++;
    }

    /* Number of RHCT nodes*/
    build_append_int_noprefix(جدول_داده, رقم_rhct_nodes, 4);

    /* Offset to the RHCT node array */
    build_append_int_noprefix(جدول_داده, RHCT_گره_ARRAY_جابه_جایی, 4);

    /* ISA String Node */
    isa_جابه_جایی = جدول_داده->len - جدول.جدول_جابه_جایی;
    build_append_int_noprefix(جدول_داده, 0, 2);   /* Type 0 */

    isa = riscv_isa_string(cpu);
    len = 8 + طول_رشته(isa) + 1;
    aligned_len = (len % 2) ? (len + 1) : len;

    build_append_int_noprefix(جدول_داده, aligned_len, 2);   /* Length */
    build_append_int_noprefix(جدول_داده, 0x1, 2);           /* Revision */

    /* ISA string length including NUL */
    build_append_int_noprefix(جدول_داده, طول_رشته(isa) + 1, 2);
    g_array_append_vals(جدول_داده, isa, طول_رشته(isa) + 1);   /* ISA string */

    إذا (aligned_len != len) {
        build_append_int_noprefix(جدول_داده, 0x0, 1);   /* Optional Padding */
    }

    /* CMO node */
    إذا (cpu->cfg.ext_zicbom || cpu->cfg.ext_zicboz) {
        cmo_جابه_جایی = جدول_داده->len - جدول.جدول_جابه_جایی;
        build_append_int_noprefix(جدول_داده, 1, 2);    /* Type */
        build_append_int_noprefix(جدول_داده, 10, 2);   /* Length */
        build_append_int_noprefix(جدول_داده, 0x1, 2);  /* Revision */
        build_append_int_noprefix(جدول_داده, 0, 1);    /* Reserved */

        /* CBOM block size */
        إذا (cpu->cfg.cbom_blocksize) {
            build_append_int_noprefix(جدول_داده,
                                      __builtin_ctz(cpu->cfg.cbom_blocksize),
                                      1);
        } وإلا {
            build_append_int_noprefix(جدول_داده, 0, 1);
        }

        /* CBOP block size */
        build_append_int_noprefix(جدول_داده, 0, 1);

        /* CBOZ block size */
        إذا (cpu->cfg.cboz_blocksize) {
            build_append_int_noprefix(جدول_داده,
                                      __builtin_ctz(cpu->cfg.cboz_blocksize),
                                      1);
        } وإلا {
            build_append_int_noprefix(جدول_داده, 0, 1);
        }
    }

    /* MMU node structure */
    إذا (!rv32 && cpu->cfg.max_satp_نما >= VM_1_10_SV39) {
        mmu_جابه_جایی = جدول_داده->len - جدول.جدول_جابه_جایی;
        build_append_int_noprefix(جدول_داده, 2, 2);    /* Type */
        build_append_int_noprefix(جدول_داده, 8, 2);    /* Length */
        build_append_int_noprefix(جدول_داده, 0x1, 2);  /* Revision */
        build_append_int_noprefix(جدول_داده, 0, 1);    /* Reserved */
        /* MMU Type */
        إذا (cpu->cfg.max_satp_نما == VM_1_10_SV57) {
            build_append_int_noprefix(جدول_داده, 2, 1);    /* Sv57 */
        } وإلا إذا (cpu->cfg.max_satp_نما == VM_1_10_SV48) {
            build_append_int_noprefix(جدول_داده, 1, 1);    /* Sv48 */
        } وإلا إذا (cpu->cfg.max_satp_نما == VM_1_10_SV39) {
            build_append_int_noprefix(جدول_داده, 0, 1);    /* Sv39 */
        } وإلا {
            g_assert_not_reached();
        }
    }

    /* Hart Info Node */
    لكل (صحيح i = 0; i < arch_ids->len; i++) {
        len = 16;
        صحيح رقم_offsets = 1;
        build_append_int_noprefix(جدول_داده, 0xFFFF, 2);  /* Type */

        /* Length */
        إذا (cmo_جابه_جایی) {
            len += 4;
            رقم_offsets++;
        }

        إذا (mmu_جابه_جایی) {
            len += 4;
            رقم_offsets++;
        }

        build_append_int_noprefix(جدول_داده, len, 2);
        build_append_int_noprefix(جدول_داده, 0x1, 2); /* Revision */
        /* Number of offsets */
        build_append_int_noprefix(جدول_داده, رقم_offsets, 2);
        build_append_int_noprefix(جدول_داده, i, 4);   /* ACPI Processor UID */
        /* Offsets */
        build_append_int_noprefix(جدول_داده, isa_جابه_جایی, 4);
        إذا (cmo_جابه_جایی) {
            build_append_int_noprefix(جدول_داده, cmo_جابه_جایی, 4);
        }

        إذا (mmu_جابه_جایی) {
            build_append_int_noprefix(جدول_داده, mmu_جابه_جایی, 4);
        }
    }

    acpi_جدول_انتها(linker, &جدول);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.9 Fixed ACPI Description Table (MADT)
 */
ساكن فراغ build_fadt_rev6(GArray *جدول_داده,
                            BIOSLinker *linker,
                            RISCVVirtState *s,
                            بیعلامت dsdt_tbl_جابه_جایی)
{
    Acpi_Fadt_داده fadt = {
        .rev = 6,
        .minor_ver = 6,
        .flags = 1 << ACPI_FADT_F_HW_REDUCED_ACPI,
        .xdsdt_tbl_جابه_جایی = &dsdt_tbl_جابه_جایی,
    };

    build_fadt(جدول_داده, linker, &fadt, s->oem_id, s->oem_جدول_id);
}

/* DSDT */
ساكن فراغ build_dsdt(GArray *جدول_داده,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    Aml *scope, *dsdt;
    MachineState *ms = MACHINE(s);
    uint8_t socket_شمار;
    ثابت Mem_Map_مدخل *memmap = s->memmap;
    Acpi_جدول جدول = { .sig = "DSDT", .rev = 2, .oem_id = s->oem_id,
                        .oem_جدول_id = s->oem_جدول_id };


    acpi_جدول_begin(&جدول, جدول_داده);
    dsdt = init_aml_allocator();

    /*
     * When booting the VM with UEFI, UEFI takes ownership of the RTC hardware.
     * While UEFI can use libfdt to disable the RTC device node in the DTB that
     * it passes to the OS, it cannot modify AML. Therefore, we won't generate
     * the RTC ACPI device at all when using UEFI.
     */
    scope = aml_scope("\\_SB");
    acpi_dsdt_جمع_کن_cpus(scope, s);

    fw_cfg_acpi_dsdt_جمع_کن(scope, &memmap[VIRT_FW_CFG]);

    socket_شمار = riscv_socket_شمار(ms);

    إذا (s->aia_type == VIRT_AIA_TYPE_NONE) {
        acpi_dsdt_جمع_کن_plic_aplic(scope, socket_شمار, s->رقم_sources,
                                 memmap[VIRT_PLIC].پایه,
                                 memmap[VIRT_PLIC].اندازه,
                                 "RSCV0001");
    } وإلا {
        acpi_dsdt_جمع_کن_plic_aplic(scope, socket_شمار, s->رقم_sources,
                                 memmap[VIRT_APLIC_S].پایه,
                                 memmap[VIRT_APLIC_S].اندازه, "RSCV0002");
    }

    acpi_dsdt_جمع_کن_uart(scope, &memmap[VIRT_UART0], UART0_IRQ, 0);
    إذا (s->uart1_present) {
        acpi_dsdt_جمع_کن_uart(scope, &memmap[VIRT_UART1], UART1_IRQ, 1);
    }

    إذا (virt_is_iommu_sys_enabled(s)) {
        acpi_dsdt_جمع_کن_iommu_sys(scope, &memmap[VIRT_IOMMU_SYS], IOMMU_SYS_IRQ);
    }

    إذا (socket_شمار == 1) {
        virtio_acpi_dsdt_جمع_کن(scope, memmap[VIRT_VIRTIO].پایه,
                             memmap[VIRT_VIRTIO].اندازه,
                             VIRTIO_IRQ, 0, VIRTIO_شمار);
        acpi_dsdt_جمع_کن_gpex_host(scope, PCIE_IRQ);
    } وإلا إذا (socket_شمار == 2) {
        virtio_acpi_dsdt_جمع_کن(scope, memmap[VIRT_VIRTIO].پایه,
                             memmap[VIRT_VIRTIO].اندازه,
                             VIRTIO_IRQ + s->رقم_sources, 0,
                             VIRTIO_شمار);
        acpi_dsdt_جمع_کن_gpex_host(scope, PCIE_IRQ + s->رقم_sources);
    } وإلا {
        virtio_acpi_dsdt_جمع_کن(scope, memmap[VIRT_VIRTIO].پایه,
                             memmap[VIRT_VIRTIO].اندازه,
                             VIRTIO_IRQ + s->رقم_sources, 0,
                             VIRTIO_شمار);
        acpi_dsdt_جمع_کن_gpex_host(scope, PCIE_IRQ + s->رقم_sources * 2);
    }

    aml_append(dsdt, scope);

    /* copy AML table into ACPI tables blob and patch header there */
    g_array_append_vals(جدول_داده, dsdt->buf->داده, dsdt->buf->len);

    acpi_جدول_انتها(linker, &جدول);
    free_aml_allocator();
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.12 Multiple APIC Description Table (MADT)
 */
ساكن فراغ build_madt(GArray *جدول_داده,
                       BIOSLinker *linker,
                       RISCVVirtState *s)
{
    MachineClass *mc = MACHINE_GET_CLASS(s);
    MachineState *ms = MACHINE(s);
    ثابت CPUArchIdList *arch_ids = mc->possible_cpu_arch_ids(ms);
    uint8_t  گروه_شاخص_بیتها = imsic_رقم_بیتها(riscv_socket_شمار(ms));
    uint8_t  guest_شاخص_بیتها = imsic_رقم_بیتها(s->aia_guests + 1);
    uint16_t imsic_max_hart_در_هر_socket = 0;
    uint8_t  hart_شاخص_بیتها;
    uint64_t aplic_addr;
    uint32_t gsi_پایه;
    uint8_t  socket;

    لكل (socket = 0; socket < riscv_socket_شمار(ms); socket++) {
        إذا (imsic_max_hart_در_هر_socket < s->soc[socket].رقم_harts) {
            imsic_max_hart_در_هر_socket = s->soc[socket].رقم_harts;
        }
    }

    hart_شاخص_بیتها = imsic_رقم_بیتها(imsic_max_hart_در_هر_socket);

    Acpi_جدول جدول = { .sig = "APIC", .rev = 7, .oem_id = s->oem_id,
                        .oem_جدول_id = s->oem_جدول_id };

    acpi_جدول_begin(&جدول, جدول_داده);
    /* Local Interrupt Controller Address */
    build_append_int_noprefix(جدول_داده, 0, 4);
    build_append_int_noprefix(جدول_داده, 0, 4);   /* MADT Flags */

    /* RISC-V Local INTC structures per HART */
    لكل (صحيح i = 0; i < arch_ids->len; i++) {
        riscv_acpi_madt_جمع_کن_rintc(i, arch_ids, جدول_داده, s);
    }

    /* IMSIC */
    إذا (s->aia_type == VIRT_AIA_TYPE_APLIC_IMSIC) {
        /* IMSIC */
        build_append_int_noprefix(جدول_داده, 0x19, 1);     /* Type */
        build_append_int_noprefix(جدول_داده, 16, 1);       /* Length */
        build_append_int_noprefix(جدول_داده, 1, 1);        /* Version */
        build_append_int_noprefix(جدول_داده, 0, 1);        /* Reserved */
        build_append_int_noprefix(جدول_داده, 0, 4);        /* Flags */
        /* Number of supervisor mode Interrupt Identities */
        build_append_int_noprefix(جدول_داده, VIRT_IRQCHIP_رقم_MSIS, 2);
        /* Number of guest mode Interrupt Identities */
        build_append_int_noprefix(جدول_داده, VIRT_IRQCHIP_رقم_MSIS, 2);
        /* Guest Index Bits */
        build_append_int_noprefix(جدول_داده, guest_شاخص_بیتها, 1);
        /* Hart Index Bits */
        build_append_int_noprefix(جدول_داده, hart_شاخص_بیتها, 1);
        /* Group Index Bits */
        build_append_int_noprefix(جدول_داده, گروه_شاخص_بیتها, 1);
        /* Group Index Shift */
        build_append_int_noprefix(جدول_داده, IMSIC_MMIO_گروه_MIN_لغزش, 1);
    }

    إذا (s->aia_type != VIRT_AIA_TYPE_NONE) {
        /* APLICs */
        لكل (socket = 0; socket < riscv_socket_شمار(ms); socket++) {
            aplic_addr = s->memmap[VIRT_APLIC_S].پایه +
                             s->memmap[VIRT_APLIC_S].اندازه * socket;
            gsi_پایه = s->رقم_sources * socket;
            build_append_int_noprefix(جدول_داده, 0x1A, 1);    /* Type */
            build_append_int_noprefix(جدول_داده, 36, 1);      /* Length */
            build_append_int_noprefix(جدول_داده, 1, 1);       /* Version */
            build_append_int_noprefix(جدول_داده, socket, 1);  /* APLIC ID */
            build_append_int_noprefix(جدول_داده, 0, 4);       /* Flags */
            build_append_int_noprefix(جدول_داده, 0, 8);       /* Hardware ID */
            /* Number of IDCs */
            إذا (s->aia_type == VIRT_AIA_TYPE_APLIC) {
                build_append_int_noprefix(جدول_داده,
                                          s->soc[socket].رقم_harts,
                                          2);
            } وإلا {
                build_append_int_noprefix(جدول_داده, 0, 2);
            }
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(جدول_داده, VIRT_IRQCHIP_رقم_SOURCES, 2);
            /* Global System Interrupt Base */
            build_append_int_noprefix(جدول_داده, gsi_پایه, 4);
            /* APLIC Address */
            build_append_int_noprefix(جدول_داده, aplic_addr, 8);
            /* APLIC size */
            build_append_int_noprefix(جدول_داده,
                                      s->memmap[VIRT_APLIC_S].اندازه, 4);
        }
    } وإلا {
        /* PLICs */
        لكل (socket = 0; socket < riscv_socket_شمار(ms); socket++) {
            aplic_addr = s->memmap[VIRT_PLIC].پایه +
                         s->memmap[VIRT_PLIC].اندازه * socket;
            gsi_پایه = VIRT_IRQCHIP_رقم_SOURCES * socket;
            build_append_int_noprefix(جدول_داده, 0x1B, 1);   /* Type */
            build_append_int_noprefix(جدول_داده, 36, 1);     /* Length */
            build_append_int_noprefix(جدول_داده, 1, 1);      /* Version */
            build_append_int_noprefix(جدول_داده, socket, 1); /* PLIC ID */
            build_append_int_noprefix(جدول_داده, 0, 8);      /* Hardware ID */
            /* Total External Interrupt Sources Supported */
            build_append_int_noprefix(جدول_داده,
                                      VIRT_IRQCHIP_رقم_SOURCES - 1, 2);
            build_append_int_noprefix(جدول_داده, 0, 2);     /* Max Priority */
            build_append_int_noprefix(جدول_داده, 0, 4);     /* Flags */
            /* PLIC Size */
            build_append_int_noprefix(جدول_داده, s->memmap[VIRT_PLIC].اندازه, 4);
            /* PLIC Address */
            build_append_int_noprefix(جدول_داده, aplic_addr, 8);
            /* Global System Interrupt Vector Base */
            build_append_int_noprefix(جدول_داده, gsi_پایه, 4);
        }
    }

    acpi_جدول_انتها(linker, &جدول);
}

#تعریف_کن ID_MAPPING_مدخل_اندازه        20
#تعریف_کن IOMMU_مدخل_اندازه             40
#تعریف_کن RISCV_وقفه_WIRE_OFFSSET 40
#تعریف_کن ریشه_COMPLEX_مدخل_اندازه      20
#تعریف_کن RIMT_گره_جابه_جایی             48

/*
 * ID Mapping Structure
 */
ساكن فراغ build_rimt_id_mapping(GArray *جدول_داده, uint32_t منبع_id_پایه,
                                  uint32_t رقم_ids, uint32_t dest_id_پایه)
{
    /* Source ID Base */
    build_append_int_noprefix(جدول_داده, منبع_id_پایه, 4);
    /* Number of IDs */
    build_append_int_noprefix(جدول_داده, رقم_ids, 4);
    /* Destination Device ID Base */
    build_append_int_noprefix(جدول_داده, منبع_id_پایه, 4);
    /* Destination IOMMU Offset */
    build_append_int_noprefix(جدول_داده, dest_id_پایه, 4);
    /* Flags */
    build_append_int_noprefix(جدول_داده, 0, 4);
}

بنية AcpiRimtIdMapping {
    uint32_t منبع_id_پایه;
    uint32_t رقم_ids;
};
تعريفنوع بنية AcpiRimtIdMapping AcpiRimtIdMapping;

/* Build the rimt ID mapping to IOMMU for a given PCI host bridge */
ساكن صحيح rimt_host_bridges(Object *obj, فراغ *opaque)
{
    GArray *idmap_blob = opaque;

    إذا (object_dynamic_cast(obj, TYPE_PCI_HOST_BRIDGE)) {
        PCIBus *bus = PCI_HOST_BRIDGE(obj)->bus;

        إذا (bus && !pci_bus_bypass_iommu(bus)) {
            صحيح min_bus, max_bus;

            pci_bus_range(bus, &min_bus, &max_bus);

            AcpiRimtIdMapping idmap = {
                .منبع_id_پایه = min_bus << 8,
                .رقم_ids = (max_bus - min_bus + 1) << 8,
            };
            g_array_append_val(idmap_blob, idmap);
        }
    }

    أرجع 0;
}

ساكن صحيح rimt_idmap_compare(gconstpointer a, gconstpointer b)
{
    AcpiRimtIdMapping *idmap_a = (AcpiRimtIdMapping *)a;
    AcpiRimtIdMapping *idmap_b = (AcpiRimtIdMapping *)b;

    أرجع idmap_a->منبع_id_پایه - idmap_b->منبع_id_پایه;
}

/*
 * RISC-V IO Mapping Table (RIMT)
 * https://github.com/riscv-non-isa/riscv-acpi-rimt/releases/download/v0.99/rimt-spec.pdf
 */
ساكن فراغ build_rimt(GArray *جدول_داده, BIOSLinker *linker,
                       RISCVVirtState *s)
{
    صحيح i, nb_nodes, rc_mapping_شمار;
    اندازه_t گره_اندازه, iommu_جابه_جایی = 0;
    uint32_t id = 0;
    g_autoptr(GArray) iommu_idmaps = g_array_new(false, true,
                                                 حجم(AcpiRimtIdMapping));

    Acpi_جدول جدول = { .sig = "RIMT", .rev = 1, .oem_id = s->oem_id,
                        .oem_جدول_id = s->oem_جدول_id };

    acpi_جدول_begin(&جدول, جدول_داده);

    object_فرزند_foreach_recursive(object_get_ریشه(),
                                   rimt_host_bridges, iommu_idmaps);

    /* Sort the ID mapping  by Source ID Base*/
    g_array_مرتب_سازی(iommu_idmaps, rimt_idmap_compare);

    nb_nodes = 2; /* RC, IOMMU */
    rc_mapping_شمار = iommu_idmaps->len;
    /* Number of RIMT Nodes */
    build_append_int_noprefix(جدول_داده, nb_nodes, 4);

    /* Offset to Array of RIMT Nodes */
    build_append_int_noprefix(جدول_داده, RIMT_گره_جابه_جایی, 4);
    build_append_int_noprefix(جدول_داده, 0, 4); /* Reserved */

    iommu_جابه_جایی = جدول_داده->len - جدول.جدول_جابه_جایی;
    /*  IOMMU Device Structure */
    build_append_int_noprefix(جدول_داده, 0, 1);         /* Type - IOMMU*/
    build_append_int_noprefix(جدول_داده, 1, 1);         /* Revision */
    گره_اندازه =  IOMMU_مدخل_اندازه;
    build_append_int_noprefix(جدول_داده, گره_اندازه, 2); /* Length */
    build_append_int_noprefix(جدول_داده, 0, 2);         /* Reserved */
    build_append_int_noprefix(جدول_داده, id++, 2);      /* ID */
    إذا (virt_is_iommu_sys_enabled(s)) {
        /* Hardware ID */
        build_append_int_noprefix(جدول_داده, 'R', 1);
        build_append_int_noprefix(جدول_داده, 'S', 1);
        build_append_int_noprefix(جدول_داده, 'C', 1);
        build_append_int_noprefix(جدول_داده, 'V', 1);
        build_append_int_noprefix(جدول_داده, '0', 1);
        build_append_int_noprefix(جدول_داده, '0', 1);
        build_append_int_noprefix(جدول_داده, '0', 1);
        build_append_int_noprefix(جدول_داده, '4', 1);
        /* Base Address */
        build_append_int_noprefix(جدول_داده,
                                  s->memmap[VIRT_IOMMU_SYS].پایه, 8);
        build_append_int_noprefix(جدول_داده, 0, 4);   /* Flags */
    } وإلا {
        /* Hardware ID */
        build_append_int_noprefix(جدول_داده, '0', 1);
        build_append_int_noprefix(جدول_داده, '0', 1);
        build_append_int_noprefix(جدول_داده, '1', 1);
        build_append_int_noprefix(جدول_داده, '0', 1);
        build_append_int_noprefix(جدول_داده, '0', 1);
        build_append_int_noprefix(جدول_داده, '0', 1);
        build_append_int_noprefix(جدول_داده, '1', 1);
        build_append_int_noprefix(جدول_داده, '4', 1);

        build_append_int_noprefix(جدول_داده, 0, 8);   /* Base Address */
        build_append_int_noprefix(جدول_داده, 1, 4);   /* Flags */
    }

    build_append_int_noprefix(جدول_داده, 0, 4);       /* Proximity Domain */
    build_append_int_noprefix(جدول_داده, 0, 2);       /* PCI Segment number */
    /* PCIe B/D/F */
    إذا (virt_is_iommu_sys_enabled(s)) {
        build_append_int_noprefix(جدول_داده, 0, 2);
    } وإلا {
        build_append_int_noprefix(جدول_داده, s->pci_iommu_bdf, 2);
    }
    /* Number of interrupt wires */
    build_append_int_noprefix(جدول_داده, 0, 2);
    /* Interrupt wire array offset */
    build_append_int_noprefix(جدول_داده, RISCV_وقفه_WIRE_OFFSSET, 2);

    /*  PCIe Root Complex Node */
    build_append_int_noprefix(جدول_داده, 1, 1);           /* Type */
    build_append_int_noprefix(جدول_داده, 1, 1);           /* Revision */
    گره_اندازه =  ریشه_COMPLEX_مدخل_اندازه +
                 ID_MAPPING_مدخل_اندازه * rc_mapping_شمار;
    build_append_int_noprefix(جدول_داده, گره_اندازه, 2);   /* Length */
    build_append_int_noprefix(جدول_داده, 0, 2);           /* Reserved */
    build_append_int_noprefix(جدول_داده, id++, 2);        /* ID */
    build_append_int_noprefix(جدول_داده, 0, 4);           /* Flags */
    build_append_int_noprefix(جدول_داده, 0, 2);           /* Reserved */
    /* PCI Segment number */
    build_append_int_noprefix(جدول_داده, 0, 2);
    /* ID mapping array offset */
    build_append_int_noprefix(جدول_داده, ریشه_COMPLEX_مدخل_اندازه, 2);
    /* Number of ID mappings */
    build_append_int_noprefix(جدول_داده, rc_mapping_شمار, 2);

    /* Output Reference */
    AcpiRimtIdMapping *range;

    /* ID mapping array */
    لكل (i = 0; i < iommu_idmaps->len; i++) {
        range = &g_array_شاخص(iommu_idmaps, AcpiRimtIdMapping, i);
        إذا (virt_is_iommu_sys_enabled(s)) {
            range->منبع_id_پایه = 0;
            range->رقم_ids = 0x10000;
        } وإلا {
            range->منبع_id_پایه = s->pci_iommu_bdf + 1;
            range->رقم_ids = 0xffff - s->pci_iommu_bdf;
        }
        build_rimt_id_mapping(جدول_داده, range->منبع_id_پایه,
                              range->رقم_ids, iommu_جابه_جایی);
    }

    acpi_جدول_انتها(linker, &جدول);
}

/*
 * ACPI spec, Revision 6.6
 * 5.2.16 System Resource Affinity Table (SRAT)
 */
ساكن فراغ
build_srat(GArray *جدول_داده, BIOSLinker *linker, RISCVVirtState *vms)
{
    صحيح i;
    uint64_t mem_پایه;
    MachineClass *mc = MACHINE_GET_CLASS(vms);
    MachineState *ms = MACHINE(vms);
    ثابت CPUArchIdList *cpu_list = mc->possible_cpu_arch_ids(ms);
    Acpi_جدول جدول = { .sig = "SRAT", .rev = 3, .oem_id = vms->oem_id,
                        .oem_جدول_id = vms->oem_جدول_id };

    acpi_جدول_begin(&جدول, جدول_داده);
    build_append_int_noprefix(جدول_داده, 1, 4); /* Reserved */
    build_append_int_noprefix(جدول_داده, 0, 8); /* Reserved */

    لكل (i = 0; i < cpu_list->len; ++i) {
        uint32_t nodeid = cpu_list->cpus[i].props.گره_id;
        /*
         * 5.2.16.8 RINTC Affinity Structure
         */
        build_append_int_noprefix(جدول_داده, 7, 1);      /* Type */
        build_append_int_noprefix(جدول_داده, 20, 1);     /* Length */
        build_append_int_noprefix(جدول_داده, 0, 2);        /* Reserved */
        build_append_int_noprefix(جدول_داده, nodeid, 4); /* Proximity Domain */
        build_append_int_noprefix(جدول_داده, i, 4); /* ACPI Processor UID */
        /* Flags, Table 5-70 */
        build_append_int_noprefix(جدول_داده, 1 /* Flags: Enabled */, 4);
        build_append_int_noprefix(جدول_داده, 0, 4); /* Clock Domain */
    }

    mem_پایه = vms->memmap[VIRT_DRAM].پایه;
    لكل (i = 0; i < ms->numa_state->رقم_nodes; ++i) {
        إذا (ms->numa_state->nodes[i].گره_mem > 0) {
            build_srat_حافظه(جدول_داده, mem_پایه,
                              ms->numa_state->nodes[i].گره_mem, i,
                              MEM_AFFINITY_ENABLED);
            mem_پایه += ms->numa_state->nodes[i].گره_mem;
        }
    }

    acpi_جدول_انتها(linker, &جدول);
}

ساكن فراغ virt_acpi_build(RISCVVirtState *s, AcpiBuildTables *tables)
{
    GArray *جدول_offsets;
    بیعلامت dsdt, xsdt;
    GArray *tables_blob = tables->جدول_داده;
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
    acpi_جمع_کن_جدول(جدول_offsets, tables_blob);
    build_fadt_rev6(tables_blob, tables->linker, s, dsdt);

    acpi_جمع_کن_جدول(جدول_offsets, tables_blob);
    build_madt(tables_blob, tables->linker, s);

    acpi_جمع_کن_جدول(جدول_offsets, tables_blob);
    build_rhct(tables_blob, tables->linker, s);

    إذا (virt_is_iommu_sys_enabled(s) || s->pci_iommu_bdf) {
        acpi_جمع_کن_جدول(جدول_offsets, tables_blob);
        build_rimt(tables_blob, tables->linker, s);
    }

    acpi_جمع_کن_جدول(جدول_offsets, tables_blob);

    إذا (ms->acpi_spcr_enabled) {
        spcr_راهاندازی(tables_blob, tables->linker, s);
    }

    acpi_جمع_کن_جدول(جدول_offsets, tables_blob);
    {
        AcpiMcfgInfo mcfg = {
           .پایه = s->memmap[VIRT_PCIE_ECAM].پایه,
           .اندازه = s->memmap[VIRT_PCIE_ECAM].اندازه,
        };
        build_mcfg(tables_blob, tables->linker, &mcfg, s->oem_id,
                   s->oem_جدول_id);
    }

    إذا (ms->numa_state->رقم_nodes > 0) {
        acpi_جمع_کن_جدول(جدول_offsets, tables_blob);
        build_srat(tables_blob, tables->linker, s);
        إذا (ms->numa_state->have_numa_فاصله) {
            acpi_جمع_کن_جدول(جدول_offsets, tables_blob);
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
        Acpi_Rsdp_داده rsdp_داده = {
            .revision = 2,
            .oem_id = s->oem_id,
            .xsdt_tbl_جابه_جایی = &xsdt,
            .rsdt_tbl_جابه_جایی = عدم,
        };
        build_rsdp(tables->rsdp, tables->linker, &rsdp_داده);
    }

    /*
     * The align size is 128, warn if 64k is not enough therefore
     * the align size could be resized.
     */
    إذا (tables_blob->len > ACPI_BUILD_جدول_اندازه / 2) {
        warn_report("ACPI table size %u exceeds %d bytes,"
                    " migration may not work",
                    tables_blob->len, ACPI_BUILD_جدول_اندازه / 2);
        error_printf("Try removing some objects.");
    }

    acpi_align_اندازه(tables_blob, ACPI_BUILD_جدول_اندازه);

    /* Clean up memory that's no longer used */
    g_array_free(جدول_offsets, true);
}

ساكن فراغ acpi_ram_update(حافظه_Region *mr, GArray *داده)
{
    uint32_t اندازه = acpi_داده_len(داده);

    /*
     * Make sure RAM size is correct - in case it got changed
     * e.g. by migration
     */
    حافظه_region_ram_resize(mr, اندازه, &error_abort);

    کپی_حافظه(حافظه_region_get_ram_ptr(mr), داده->داده, اندازه);
    حافظه_region_مجموعه_dirty(mr, 0, اندازه);
}

ساكن فراغ virt_acpi_build_update(فراغ *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    AcpiBuildTables tables;

    /* No state to update or already patched? Nothing to do. */
    إذا (!build_state || build_state->patched) {
        أرجع;
    }

    build_state->patched = true;

    acpi_build_tables_init(&tables);

    virt_acpi_build(RISCV_VIRT_MACHINE(qdev_get_machine()), &tables);

    acpi_ram_update(build_state->جدول_mr, tables.جدول_داده);
    acpi_ram_update(build_state->rsdp_mr, tables.rsdp);
    acpi_ram_update(build_state->linker_mr, tables.linker->cmd_blob);

    acpi_build_tables_cleanup(&tables, true);
}

ساكن فراغ virt_acpi_build_reset(فراغ *build_opaque)
{
    AcpiBuildState *build_state = build_opaque;
    build_state->patched = false;
}

ساكن ثابت VMStateDescription vmstate_virt_acpi_build = {
    .name = "virt_acpi_build",
    .نسخه_id = 1,
    .minimum_نسخه_id = 1,
    .fields = (ثابت VMState_فیلد[]) {
        VMSTATE_BOOL(patched, AcpiBuildState),
        VMSTATE_انتها_از_LIST()
    },
};

فراغ virt_acpi_راهاندازی(RISCVVirtState *s)
{
    AcpiBuildTables tables;
    AcpiBuildState *build_state;

    build_state = g_malloc0(حجم *build_state);

    acpi_build_tables_init(&tables);
    virt_acpi_build(s, &tables);

    /* Now expose it all to Guest */
    build_state->جدول_mr = acpi_جمع_کن_rom_blob(virt_acpi_build_update,
                                              build_state, tables.جدول_داده,
                                              ACPI_BUILD_جدول_FILE);
    أكد(build_state->جدول_mr != عدم);

    build_state->linker_mr = acpi_جمع_کن_rom_blob(virt_acpi_build_update,
                                               build_state,
                                               tables.linker->cmd_blob,
                                               ACPI_BUILD_LOADER_FILE);

    build_state->rsdp_mr = acpi_جمع_کن_rom_blob(virt_acpi_build_update,
                                             build_state, tables.rsdp,
                                             ACPI_BUILD_RSDP_FILE);

    qemu_ثبات_reset(virt_acpi_build_reset, build_state);
    virt_acpi_build_reset(build_state);
    vmstate_ثبات(عدم, 0, &vmstate_virt_acpi_build, build_state);

    /*
     * Clean up tables but don't free the memory: we track it
     * in build_state.
     */
    acpi_build_tables_cleanup(&tables, false);
}
