/*
 * QEMU RISC-V Boot Helper
 *
 * Copyright (c) 2017 SiFive, Inc.
 * Copyright (c) 2019 Alistair Francis <alistair.francis@wdc.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qemu/bswap.h"
#अन्तर्भाव "qemu/datadir.h"
#अन्तर्भाव "qemu/units.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "exec/cpu-defs.h"
#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/core/loader.h"
#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "hw/riscv/boot_opensbi.h"
#अन्तर्भाव "elf.h"
#अन्तर्भाव "system/device_tree.h"
#अन्तर्भाव "system/qtest.h"
#अन्तर्भाव "system/kvm.h"
#अन्तर्भाव "system/reset.h"

#अन्तर्भाव <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    प्रत्यागम mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
वर्ण *riscv_plic_hart_config_string(पूर्णाङ्क hart_गणनम्)
{
    g_autofree ध्रुव वर्ण **vals = g_new(ध्रुव वर्ण *, hart_गणनम् + 1);
    पूर्णाङ्क i;

    प्रति (i = 0; i < hart_गणनम्; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        यदि (kvm_enabled()) {
            vals[i] = "S";
        } अन्यथा यदि (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } अन्यथा {
            vals[i] = "M";
        }
    }
    vals[i] = रिक्त;

    /* g_strjoinv() obliges us to cast away const here */
    प्रत्यागम g_strjoinv(",", (वर्ण **)vals);
}

शून्य riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_नीचः_start = 0;
    info->ram_नीचः_आकार = 0;
    info->kernel_आकार = 0;
    info->initrd_आकार = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
शून्य riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr नीचः_start, hwaddr नीचः_आकार)
{
    riscv_boot_info_init(info, harts);
    info->ram_नीचः_start = नीचः_start;
    info->ram_नीचः_आकार = नीचः_आकार;
}

vaddr riscv_calc_kernel_start_addr(ध्रुव RISCVBootInfo *info,
                                   hwaddr firmware_अन्तः_addr) {
    यदि (info->is_32bit) {
        प्रत्यागम QEMU_ALIGN_ऊर्ध्वम्(firmware_अन्तः_addr, 4 * MiB);
    } अन्यथा {
        प्रत्यागम QEMU_ALIGN_ऊर्ध्वम्(firmware_अन्तः_addr, 2 * MiB);
    }
}

ध्रुव वर्ण *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    यदि (riscv_is_32bit(harts)) {
        प्रत्यागम RISCV32_BIOS_BIN;
    }

    प्रत्यागम RISCV64_BIOS_BIN;
}

स्थिर वर्ण *riscv_find_bios(ध्रुव वर्ण *bios_filename)
{
    वर्ण *filename;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    यदि (filename == रिक्त) {
        यदि (!qtest_enabled()) {
            /*
             * We only ship OpenSBI binary bios images in the QEMU source.
             * For machines that use images other than the default bios,
             * running QEMU test will complain hence let's suppress the error
             * report for QEMU testing.
             */
            error_report("Unable to find the RISC-V BIOS \"%s\"",
                         bios_filename);
            निर्गच्छ(1);
        }
    }

    प्रत्यागम filename;
}

वर्ण *riscv_find_firmware(ध्रुव वर्ण *firmware_filename,
                          ध्रुव वर्ण *default_machine_firmware)
{
    वर्ण *filename = रिक्त;

    यदि ((!firmware_filename) || (!वाक्यं_तुलय(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_find_bios(default_machine_firmware);
    } अन्यथा यदि (वाक्यं_तुलय(firmware_filename, "none")) {
        filename = riscv_find_bios(firmware_filename);
    }

    प्रत्यागम filename;
}

hwaddr riscv_find_and_आरोपय_firmware(MachineState *machine,
                                    ध्रुव RISCVBootInfo *info,
                                    ध्रुव वर्ण *default_machine_firmware,
                                    hwaddr *firmware_आरोपय_addr,
                                    चिह्नम्_fn_t sym_cb)
{
    वर्ण *firmware_filename;
    hwaddr firmware_अन्तः_addr = *firmware_आरोपय_addr;

    firmware_filename = riscv_find_firmware(machine->firmware,
                                            default_machine_firmware);

    यदि (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_अन्तः_addr = riscv_आरोपय_firmware(machine, info,
                                                firmware_filename,
                                                firmware_आरोपय_addr, sym_cb);
        g_free(firmware_filename);
    }

    प्रत्यागम firmware_अन्तः_addr;
}

hwaddr riscv_आरोपय_firmware(MachineState *machine,
                           ध्रुव RISCVBootInfo *info,
                           ध्रुव वर्ण *firmware_filename,
                           hwaddr *firmware_आरोपय_addr,
                           चिह्नम्_fn_t sym_cb)
{
    uint64_t mem_आकार = info->ram_नीचः_आकार ?: machine->ram_आकार;
    uint64_t firmware_प्रविष्टिः, firmware_अन्तः;
    ssize_t firmware_आकार;

    g_assert(firmware_filename != रिक्त);

    firmware_आकार = आरोपय_elf_ram_sym(firmware_filename, रिक्त, रिक्त, रिक्त,
                                     &firmware_प्रविष्टिः, रिक्त, &firmware_अन्तः,
                                     रिक्त, 0, EM_RISCV, 1, 0, रिक्त, false,
                                     sym_cb);
    यदि (firmware_आकार > 0) {
        *firmware_आरोपय_addr = firmware_प्रविष्टिः;
        प्रत्यागम firmware_अन्तः;
    }

    यदि (firmware_आकार != ELF_आरोपय_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    आरोपय_elf_strerror(firmware_आकार));
    }

    firmware_आकार = आरोपय_image_targphys_as(firmware_filename,
                                           *firmware_आरोपय_addr,
                                           mem_आकार, रिक्त,
                                           रिक्त);

    यदि (firmware_आकार > 0) {
        प्रत्यागम *firmware_आरोपय_addr + firmware_आकार;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 आरोपय_elf_strerror(firmware_आकार));
    निर्गच्छ(1);
}

स्थिर शून्य riscv_आरोपय_initrd(MachineState *machine, RISCVBootInfo *info)
{
    ध्रुव वर्ण *filename = machine->initrd_filename;
    uint64_t mem_आकार = info->ram_नीचः_आकार ?: machine->ram_आकार;
    शून्य *fdt = machine->fdt;
    hwaddr start, अन्तः;
    ssize_t आकार;

    g_assert(filename != रिक्त);

    /*
     * We want to put the initrd far enough into RAM that when the
     * kernel is uncompressed it will not clobber the initrd. However
     * on boards without much RAM we must ensure that we still leave
     * enough room for a decent sized initrd, and on boards with large
     * amounts of RAM, we put the initrd at 512MB to allow large kernels
     * to boot.
     * So for boards with less than 1GB of RAM we put the initrd
     * halfway into RAM, and for boards with 1GB of RAM or more we put
     * the initrd at 512MB.
     */
    start = info->image_नीचः_addr + MIN(mem_आकार / 2, 512 * MiB);

    आकार = आरोपय_ramdisk(filename, start, mem_आकार - start);
    यदि (आकार == -1) {
        आकार = आरोपय_image_targphys(filename, start, mem_आकार - start, रिक्त);
        यदि (आकार == -1) {
            error_report("could not load ramdisk '%s'", filename);
            निर्गच्छ(1);
        }
    }

    info->initrd_start = start;
    info->initrd_आकार = आकार;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    यदि (fdt) {
        अन्तः = start + आकार;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", start);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", अन्तः);
    }
}

शून्य riscv_आरोपय_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_start_addr,
                       bool आरोपय_initrd,
                       चिह्नम्_fn_t sym_cb)
{
    uint64_t mem_आकार = info->ram_नीचः_आकार ?: machine->ram_आकार;
    ध्रुव वर्ण *kernel_filename = machine->kernel_filename;
    ssize_t kernel_आकार;
    शून्य *fdt = machine->fdt;

    g_assert(kernel_filename != रिक्त);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_आकार = आरोपय_elf_ram_sym(kernel_filename, रिक्त, रिक्त, रिक्त, रिक्त,
                                   &info->image_नीचः_addr, &info->image_उच्चः_addr,
                                   रिक्त, ELFDATA2LSB, EM_RISCV,
                                   1, 0, रिक्त, true, sym_cb);
    यदि (kernel_आकार > 0) {
        info->kernel_आकार = kernel_आकार;
        गच्छ बहिर्गमनम्;
    }

    kernel_आकार = आरोपय_uimage_as(kernel_filename, &info->image_नीचः_addr,
                                 रिक्त, रिक्त, रिक्त, रिक्त, रिक्त);
    यदि (kernel_आकार > 0) {
        info->kernel_आकार = kernel_आकार;
        info->image_उच्चः_addr = info->image_नीचः_addr + kernel_आकार;
        गच्छ बहिर्गमनम्;
    }

    kernel_आकार = आरोपय_image_targphys_as(kernel_filename, kernel_start_addr,
                                         mem_आकार, रिक्त, रिक्त);
    यदि (kernel_आकार > 0) {
        info->kernel_आकार = kernel_आकार;
        info->image_नीचः_addr = kernel_start_addr;
        info->image_उच्चः_addr = info->image_नीचः_addr + kernel_आकार;
        गच्छ बहिर्गमनम्;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    निर्गच्छ(1);

बहिर्गमनम्:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    यदि (info->is_32bit) {
        info->image_नीचः_addr = extract64(info->image_नीचः_addr, 0, 32);
    }

    यदि (आरोपय_initrd && machine->initrd_filename) {
        riscv_आरोपय_initrd(machine, info);
    }

    यदि (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
        qemu_fdt_setprop_string(fdt, "/chosen", "bootargs",
                                machine->kernel_cmdline);
    }
}

/*
 * This function makes an assumption that the DRAM interval
 * 'dram_base' + 'dram_size' is contiguous.
 *
 * Considering that 'dram_end' is the lowest value between
 * the end of the DRAM block and MachineState->ram_size, the
 * FDT location will vary according to 'dram_base':
 *
 * - if 'dram_base' is less that 3072 MiB, the FDT will be
 * put at the lowest value between 3072 MiB and 'dram_end';
 *
 * - if 'dram_base' is higher than 3072 MiB, the FDT will be
 * put at 'dram_end'.
 *
 * The FDT is fdt_packed() during the calculation.
 */
uint64_t riscv_compute_fdt_addr(hwaddr dram_आधारः, hwaddr dram_आकार,
                                MachineState *ms, ध्रुव RISCVBootInfo *info)
{
    पूर्णाङ्क ret = fdt_pack(ms->fdt);
    hwaddr dram_अन्तः, अस्थायी;
    पूर्णाङ्क fdtsize;
    uint64_t dtb_start, dtb_start_सीमा;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    यदि (fdtsize <= 0) {
        error_report("invalid device-tree");
        निर्गच्छ(1);
    }

    यदि (info->initrd_आकार) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_start_सीमा = info->initrd_start + info->initrd_आकार;
    } अन्यथा यदि (info->kernel_आकार) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_start_सीमा = info->image_उच्चः_addr;
    } अन्यथा {
        /* Otherwise, do not check DTB overlapping */
        dtb_start_सीमा = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_अन्तः = dram_आधारः;
    dram_अन्तः += dram_आकार ? MIN(ms->ram_आकार, dram_आकार) : ms->ram_आकार;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    यदि (!info->is_32bit) {
        अस्थायी = dram_अन्तः;
    } अन्यथा {
        अस्थायी = (dram_आधारः < 3072 * MiB) ? MIN(dram_अन्तः, 3072 * MiB) : dram_अन्तः;
    }

    dtb_start = QEMU_ALIGN_अधस्तात्(अस्थायी - fdtsize, 2 * MiB);

    यदि (dtb_start_सीमा && (dtb_start < dtb_start_सीमा)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        निर्गच्छ(1);
    }

    प्रत्यागम dtb_start;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
शून्य riscv_आरोपय_fdt(hwaddr fdt_addr, शून्य *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_सङ्कलय_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &स्थानाङ्कः_space_स्मृतिः);
    qemu_पञ्जिकाकोशः_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&स्थानाङ्कः_space_स्मृतिः, fdt_addr, fdtsize));
}

शून्य riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_आधारः, hwaddr rom_आकार,
                                  uint32_t reset_vec_आकार,
                                  uint64_t kernel_प्रविष्टिः)
{
    संरचना fw_dynamic_info32 dinfo32;
    संरचना fw_dynamic_info64 dinfo64;
    शून्य *dinfo_ptr = रिक्त;
    आकार_t dinfo_len;
    ध्रुव bool rv32 = riscv_is_32bit(harts);
    ध्रुव bool be = harts->harts[0].cfg.big_endian;

    यदि (rv32) {
        dinfo32.magic = be ? cpu_गन्तव्यम्_be32(FW_DYNAMIC_INFO_MAGIC_मानम्)
                           : cpu_गन्तव्यम्_le32(FW_DYNAMIC_INFO_MAGIC_मानम्);
        dinfo32.संस्करणम् = be ? cpu_गन्तव्यम्_be32(FW_DYNAMIC_INFO_संस्करणम्)
                             : cpu_गन्तव्यम्_le32(FW_DYNAMIC_INFO_संस्करणम्);
        dinfo32.अग्र_बहुलकम् = be ? cpu_गन्तव्यम्_be32(FW_DYNAMIC_INFO_अग्र_बहुलकम्_S)
                               : cpu_गन्तव्यम्_le32(FW_DYNAMIC_INFO_अग्र_बहुलकम्_S);
        dinfo32.अग्र_addr = be ? cpu_गन्तव्यम्_be32(kernel_प्रविष्टिः)
                               : cpu_गन्तव्यम्_le32(kernel_प्रविष्टिः);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = परिमाण(dinfo32);
    } अन्यथा {
        dinfo64.magic = be ? cpu_गन्तव्यम्_be64(FW_DYNAMIC_INFO_MAGIC_मानम्)
                           : cpu_गन्तव्यम्_le64(FW_DYNAMIC_INFO_MAGIC_मानम्);
        dinfo64.संस्करणम् = be ? cpu_गन्तव्यम्_be64(FW_DYNAMIC_INFO_संस्करणम्)
                             : cpu_गन्तव्यम्_le64(FW_DYNAMIC_INFO_संस्करणम्);
        dinfo64.अग्र_बहुलकम् = be ? cpu_गन्तव्यम्_be64(FW_DYNAMIC_INFO_अग्र_बहुलकम्_S)
                               : cpu_गन्तव्यम्_le64(FW_DYNAMIC_INFO_अग्र_बहुलकम्_S);
        dinfo64.अग्र_addr = be ? cpu_गन्तव्यम्_be64(kernel_प्रविष्टिः)
                               : cpu_गन्तव्यम्_le64(kernel_प्रविष्टिः);
        dinfo64.options = 0;
        dinfo64.boot_hart = 0;
        dinfo_ptr = &dinfo64;
        dinfo_len = परिमाण(dinfo64);
    }

    /**
     * copy the dynamic firmware info. This information is specific to
     * OpenSBI but doesn't break any other firmware as long as they don't
     * expect any certain value in "a2" register.
     */
    यदि (dinfo_len > (rom_आकार - reset_vec_आकार)) {
        error_report("not enough space to store dynamic firmware info");
        निर्गच्छ(1);
    }

    rom_सङ्कलय_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_आधारः + reset_vec_आकार,
                           &स्थानाङ्कः_space_स्मृतिः);
}

#परिभाषय संहिता_शब्दाः 6
#परिभाषय दत्त_शब्दाः 4

शून्य riscv_सज्जता_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr start_addr,
                               hwaddr rom_आधारः, hwaddr rom_आकार,
                               uint64_t kernel_प्रविष्टिः,
                               uint64_t fdt_आरोपय_addr)
{
    ध्रुव bool rv32 = riscv_is_32bit(harts);
    ध्रुव bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[संहिता_शब्दाः + दत्त_शब्दाः];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    यदि (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } अन्यथा {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    यदि (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } अन्यथा {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    यदि (big_endian) {
        stq_be_p(&reset_vec[6], start_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_आरोपय_addr); /* fdt_laddr:   .dword */
    } अन्यथा {
        stq_le_p(&reset_vec[6], start_addr);
        stq_le_p(&reset_vec[8], fdt_आरोपय_addr);
    }

    rom_सङ्कलय_blob_fixed_as("mrom.reset", reset_vec, परिमाण(reset_vec),
                          rom_आधारः, &स्थानाङ्कः_space_स्मृतिः);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_आधारः, rom_आकार,
                                 परिमाण(reset_vec),
                                 kernel_प्रविष्टिः);
}

शून्य riscv_सज्जता_प्रत्यक्षम्_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

शून्य riscv_सज्जता_firmware_boot(MachineState *machine)
{
    यदि (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_find();

        प्रतिज्ञा(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        आरोपय_image_गन्तव्यम्_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_आकार, FW_CFG_KERNEL_दत्त,
                             machine->kernel_filename,
                             true);
        आरोपय_image_गन्तव्यम्_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_आकार, FW_CFG_INITRD_दत्त,
                             machine->initrd_filename, false);

        यदि (machine->kernel_cmdline) {
            fw_cfg_सङ्कलय_i32(fw_cfg, FW_CFG_CMDLINE_आकार,
                           वाक्यदैर्घ्यम्(machine->kernel_cmdline) + 1);
            fw_cfg_सङ्कलय_string(fw_cfg, FW_CFG_CMDLINE_दत्त,
                              machine->kernel_cmdline);
        }
    }
}
