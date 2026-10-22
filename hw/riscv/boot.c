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

#समावेश "qemu/osdep.h"
#समावेश "qemu/bswap.h"
#समावेश "qemu/datadir.h"
#समावेश "qemu/units.h"
#समावेश "qemu/error-report.h"
#समावेश "exec/cpu-defs.h"
#समावेश "hw/core/boards.h"
#समावेश "hw/core/loader.h"
#समावेश "hw/riscv/boot.h"
#समावेश "hw/riscv/boot_opensbi.h"
#समावेश "elf.h"
#समावेश "system/device_tree.h"
#समावेश "system/qtest.h"
#समावेश "system/kvm.h"
#समावेश "system/reset.h"

#समावेश <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    वापस mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
अक्षर *riscv_plic_hart_config_string(पूर्णांक hart_गिनती)
{
    g_autofree स्थिर अक्षर **vals = g_new(स्थिर अक्षर *, hart_गिनती + 1);
    पूर्णांक i;

    क्रम (i = 0; i < hart_गिनती; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        अगर (kvm_enabled()) {
            vals[i] = "S";
        } अन्यथा अगर (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } अन्यथा {
            vals[i] = "M";
        }
    }
    vals[i] = शून्य;

    /* g_strjoinv() obliges us to cast away const here */
    वापस g_strjoinv(",", (अक्षर **)vals);
}

व्योम riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_निचला_start = 0;
    info->ram_निचला_आकार = 0;
    info->kernel_आकार = 0;
    info->initrd_आकार = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
व्योम riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr निचला_start, hwaddr निचला_आकार)
{
    riscv_boot_info_init(info, harts);
    info->ram_निचला_start = निचला_start;
    info->ram_निचला_आकार = निचला_आकार;
}

vaddr riscv_calc_kernel_start_addr(स्थिर RISCVBootInfo *info,
                                   hwaddr firmware_end_addr) {
    अगर (info->is_32bit) {
        वापस QEMU_ALIGN_UP(firmware_end_addr, 4 * MiB);
    } अन्यथा {
        वापस QEMU_ALIGN_UP(firmware_end_addr, 2 * MiB);
    }
}

स्थिर अक्षर *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    अगर (riscv_is_32bit(harts)) {
        वापस RISCV32_BIOS_BIN;
    }

    वापस RISCV64_BIOS_BIN;
}

अटल अक्षर *riscv_find_bios(स्थिर अक्षर *bios_filename)
{
    अक्षर *filename;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    अगर (filename == शून्य) {
        अगर (!qtest_enabled()) {
            /*
             * We only ship OpenSBI binary bios images in the QEMU source.
             * For machines that use images other than the default bios,
             * running QEMU test will complain hence let's suppress the error
             * report for QEMU testing.
             */
            error_report("Unable to find the RISC-V BIOS \"%s\"",
                         bios_filename);
            निकास(1);
        }
    }

    वापस filename;
}

अक्षर *riscv_find_firmware(स्थिर अक्षर *firmware_filename,
                          स्थिर अक्षर *default_machine_firmware)
{
    अक्षर *filename = शून्य;

    अगर ((!firmware_filename) || (!म_भेद(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_find_bios(default_machine_firmware);
    } अन्यथा अगर (म_भेद(firmware_filename, "none")) {
        filename = riscv_find_bios(firmware_filename);
    }

    वापस filename;
}

hwaddr riscv_find_and_load_firmware(MachineState *machine,
                                    स्थिर RISCVBootInfo *info,
                                    स्थिर अक्षर *default_machine_firmware,
                                    hwaddr *firmware_load_addr,
                                    प्रतीक_fn_t sym_cb)
{
    अक्षर *firmware_filename;
    hwaddr firmware_end_addr = *firmware_load_addr;

    firmware_filename = riscv_find_firmware(machine->firmware,
                                            default_machine_firmware);

    अगर (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_end_addr = riscv_load_firmware(machine, info,
                                                firmware_filename,
                                                firmware_load_addr, sym_cb);
        g_free(firmware_filename);
    }

    वापस firmware_end_addr;
}

hwaddr riscv_load_firmware(MachineState *machine,
                           स्थिर RISCVBootInfo *info,
                           स्थिर अक्षर *firmware_filename,
                           hwaddr *firmware_load_addr,
                           प्रतीक_fn_t sym_cb)
{
    uint64_t mem_आकार = info->ram_निचला_आकार ?: machine->ram_आकार;
    uint64_t firmware_entry, firmware_end;
    ssize_t firmware_आकार;

    g_assert(firmware_filename != शून्य);

    firmware_आकार = load_elf_ram_sym(firmware_filename, शून्य, शून्य, शून्य,
                                     &firmware_entry, शून्य, &firmware_end,
                                     शून्य, 0, EM_RISCV, 1, 0, शून्य, false,
                                     sym_cb);
    अगर (firmware_आकार > 0) {
        *firmware_load_addr = firmware_entry;
        वापस firmware_end;
    }

    अगर (firmware_आकार != ELF_LOAD_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    load_elf_strerror(firmware_आकार));
    }

    firmware_आकार = load_image_targphys_as(firmware_filename,
                                           *firmware_load_addr,
                                           mem_आकार, शून्य,
                                           शून्य);

    अगर (firmware_आकार > 0) {
        वापस *firmware_load_addr + firmware_आकार;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 load_elf_strerror(firmware_आकार));
    निकास(1);
}

अटल व्योम riscv_load_initrd(MachineState *machine, RISCVBootInfo *info)
{
    स्थिर अक्षर *filename = machine->initrd_filename;
    uint64_t mem_आकार = info->ram_निचला_आकार ?: machine->ram_आकार;
    व्योम *fdt = machine->fdt;
    hwaddr start, end;
    ssize_t आकार;

    g_assert(filename != शून्य);

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
    start = info->image_निचला_addr + MIN(mem_आकार / 2, 512 * MiB);

    आकार = load_ramdisk(filename, start, mem_आकार - start);
    अगर (आकार == -1) {
        आकार = load_image_targphys(filename, start, mem_आकार - start, शून्य);
        अगर (आकार == -1) {
            error_report("could not load ramdisk '%s'", filename);
            निकास(1);
        }
    }

    info->initrd_start = start;
    info->initrd_आकार = आकार;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    अगर (fdt) {
        end = start + आकार;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", start);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", end);
    }
}

व्योम riscv_load_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_start_addr,
                       bool load_initrd,
                       प्रतीक_fn_t sym_cb)
{
    uint64_t mem_आकार = info->ram_निचला_आकार ?: machine->ram_आकार;
    स्थिर अक्षर *kernel_filename = machine->kernel_filename;
    ssize_t kernel_आकार;
    व्योम *fdt = machine->fdt;

    g_assert(kernel_filename != शून्य);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_आकार = load_elf_ram_sym(kernel_filename, शून्य, शून्य, शून्य, शून्य,
                                   &info->image_निचला_addr, &info->image_ऊपरी_addr,
                                   शून्य, ELFDATA2LSB, EM_RISCV,
                                   1, 0, शून्य, true, sym_cb);
    अगर (kernel_आकार > 0) {
        info->kernel_आकार = kernel_आकार;
        जाओ out;
    }

    kernel_आकार = load_uimage_as(kernel_filename, &info->image_निचला_addr,
                                 शून्य, शून्य, शून्य, शून्य, शून्य);
    अगर (kernel_आकार > 0) {
        info->kernel_आकार = kernel_आकार;
        info->image_ऊपरी_addr = info->image_निचला_addr + kernel_आकार;
        जाओ out;
    }

    kernel_आकार = load_image_targphys_as(kernel_filename, kernel_start_addr,
                                         mem_आकार, शून्य, शून्य);
    अगर (kernel_आकार > 0) {
        info->kernel_आकार = kernel_आकार;
        info->image_निचला_addr = kernel_start_addr;
        info->image_ऊपरी_addr = info->image_निचला_addr + kernel_आकार;
        जाओ out;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    निकास(1);

out:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    अगर (info->is_32bit) {
        info->image_निचला_addr = extract64(info->image_निचला_addr, 0, 32);
    }

    अगर (load_initrd && machine->initrd_filename) {
        riscv_load_initrd(machine, info);
    }

    अगर (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_base, hwaddr dram_आकार,
                                MachineState *ms, स्थिर RISCVBootInfo *info)
{
    पूर्णांक ret = fdt_pack(ms->fdt);
    hwaddr dram_end, अस्थायी;
    पूर्णांक fdtsize;
    uint64_t dtb_start, dtb_start_limit;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    अगर (fdtsize <= 0) {
        error_report("invalid device-tree");
        निकास(1);
    }

    अगर (info->initrd_आकार) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_start_limit = info->initrd_start + info->initrd_आकार;
    } अन्यथा अगर (info->kernel_आकार) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_start_limit = info->image_ऊपरी_addr;
    } अन्यथा {
        /* Otherwise, do not check DTB overlapping */
        dtb_start_limit = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_end = dram_base;
    dram_end += dram_आकार ? MIN(ms->ram_आकार, dram_आकार) : ms->ram_आकार;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    अगर (!info->is_32bit) {
        अस्थायी = dram_end;
    } अन्यथा {
        अस्थायी = (dram_base < 3072 * MiB) ? MIN(dram_end, 3072 * MiB) : dram_end;
    }

    dtb_start = QEMU_ALIGN_DOWN(अस्थायी - fdtsize, 2 * MiB);

    अगर (dtb_start_limit && (dtb_start < dtb_start_limit)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        निकास(1);
    }

    वापस dtb_start;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
व्योम riscv_load_fdt(hwaddr fdt_addr, व्योम *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_add_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &address_space_memory);
    qemu_register_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&address_space_memory, fdt_addr, fdtsize));
}

व्योम riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_base, hwaddr rom_आकार,
                                  uint32_t reset_vec_आकार,
                                  uint64_t kernel_entry)
{
    काष्ठा fw_dynamic_info32 dinfo32;
    काष्ठा fw_dynamic_info64 dinfo64;
    व्योम *dinfo_ptr = शून्य;
    माप_प्रकार dinfo_len;
    स्थिर bool rv32 = riscv_is_32bit(harts);
    स्थिर bool be = harts->harts[0].cfg.big_endian;

    अगर (rv32) {
        dinfo32.magic = be ? cpu_to_be32(FW_DYNAMIC_INFO_MAGIC_मूल्य)
                           : cpu_to_le32(FW_DYNAMIC_INFO_MAGIC_मूल्य);
        dinfo32.version = be ? cpu_to_be32(FW_DYNAMIC_INFO_VERSION)
                             : cpu_to_le32(FW_DYNAMIC_INFO_VERSION);
        dinfo32.अग्र_mode = be ? cpu_to_be32(FW_DYNAMIC_INFO_अग्र_MODE_S)
                               : cpu_to_le32(FW_DYNAMIC_INFO_अग्र_MODE_S);
        dinfo32.अग्र_addr = be ? cpu_to_be32(kernel_entry)
                               : cpu_to_le32(kernel_entry);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = माप(dinfo32);
    } अन्यथा {
        dinfo64.magic = be ? cpu_to_be64(FW_DYNAMIC_INFO_MAGIC_मूल्य)
                           : cpu_to_le64(FW_DYNAMIC_INFO_MAGIC_मूल्य);
        dinfo64.version = be ? cpu_to_be64(FW_DYNAMIC_INFO_VERSION)
                             : cpu_to_le64(FW_DYNAMIC_INFO_VERSION);
        dinfo64.अग्र_mode = be ? cpu_to_be64(FW_DYNAMIC_INFO_अग्र_MODE_S)
                               : cpu_to_le64(FW_DYNAMIC_INFO_अग्र_MODE_S);
        dinfo64.अग्र_addr = be ? cpu_to_be64(kernel_entry)
                               : cpu_to_le64(kernel_entry);
        dinfo64.options = 0;
        dinfo64.boot_hart = 0;
        dinfo_ptr = &dinfo64;
        dinfo_len = माप(dinfo64);
    }

    /**
     * copy the dynamic firmware info. This information is specific to
     * OpenSBI but doesn't break any other firmware as long as they don't
     * expect any certain value in "a2" register.
     */
    अगर (dinfo_len > (rom_आकार - reset_vec_आकार)) {
        error_report("not enough space to store dynamic firmware info");
        निकास(1);
    }

    rom_add_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_base + reset_vec_आकार,
                           &address_space_memory);
}

#घोषणा CODE_शब्दगण 6
#घोषणा तथ्य_शब्दगण 4

व्योम riscv_setup_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr start_addr,
                               hwaddr rom_base, hwaddr rom_आकार,
                               uint64_t kernel_entry,
                               uint64_t fdt_load_addr)
{
    स्थिर bool rv32 = riscv_is_32bit(harts);
    स्थिर bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[CODE_शब्दगण + तथ्य_शब्दगण];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    अगर (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } अन्यथा {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    अगर (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } अन्यथा {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    अगर (big_endian) {
        stq_be_p(&reset_vec[6], start_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_load_addr); /* fdt_laddr:   .dword */
    } अन्यथा {
        stq_le_p(&reset_vec[6], start_addr);
        stq_le_p(&reset_vec[8], fdt_load_addr);
    }

    rom_add_blob_fixed_as("mrom.reset", reset_vec, माप(reset_vec),
                          rom_base, &address_space_memory);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_base, rom_आकार,
                                 माप(reset_vec),
                                 kernel_entry);
}

व्योम riscv_setup_direct_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

व्योम riscv_setup_firmware_boot(MachineState *machine)
{
    अगर (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_find();

        निश्चित(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        load_image_to_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_आकार, FW_CFG_KERNEL_तथ्य,
                             machine->kernel_filename,
                             true);
        load_image_to_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_आकार, FW_CFG_INITRD_तथ्य,
                             machine->initrd_filename, false);

        अगर (machine->kernel_cmdline) {
            fw_cfg_add_i32(fw_cfg, FW_CFG_CMDLINE_आकार,
                           म_माप(machine->kernel_cmdline) + 1);
            fw_cfg_add_string(fw_cfg, FW_CFG_CMDLINE_तथ्य,
                              machine->kernel_cmdline);
        }
    }
}
