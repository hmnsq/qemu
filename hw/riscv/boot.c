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

#অন্তর্ভাব "qemu/osdep.h"
#অন্তর্ভাব "qemu/bswap.h"
#অন্তর্ভাব "qemu/datadir.h"
#অন্তর্ভাব "qemu/units.h"
#অন্তর্ভাব "qemu/error-report.h"
#অন্তর্ভাব "exec/cpu-defs.h"
#অন্তর্ভাব "hw/core/boards.h"
#অন্তর্ভাব "hw/core/loader.h"
#অন্তর্ভাব "hw/riscv/boot.h"
#অন্তর্ভাব "hw/riscv/boot_opensbi.h"
#অন্তর্ভাব "elf.h"
#অন্তর্ভাব "system/device_tree.h"
#অন্তর্ভাব "system/qtest.h"
#অন্তর্ভাব "system/kvm.h"
#অন্তর্ভাব "system/reset.h"

#অন্তর্ভাব <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    ফেরত mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
অক্ষর *riscv_plic_hart_config_string(পূর্ণসংখ্যা hart_গুনতি)
{
    g_autofree ধ্রুবক অক্ষর **vals = g_new(ধ্রুবক অক্ষর *, hart_গুনতি + 1);
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < hart_গুনতি; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        যদি (kvm_enabled()) {
            vals[i] = "S";
        } অন্যথা যদি (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } অন্যথা {
            vals[i] = "M";
        }
    }
    vals[i] = ফাঁকা;

    /* g_strjoinv() obliges us to cast away const here */
    ফেরত g_strjoinv(",", (অক্ষর **)vals);
}

শূন্য riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_নিচের_start = 0;
    info->ram_নিচের_আকার = 0;
    info->kernel_আকার = 0;
    info->initrd_আকার = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
শূন্য riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr নিচের_start, hwaddr নিচের_আকার)
{
    riscv_boot_info_init(info, harts);
    info->ram_নিচের_start = নিচের_start;
    info->ram_নিচের_আকার = নিচের_আকার;
}

vaddr riscv_calc_kernel_start_addr(ধ্রুবক RISCVBootInfo *info,
                                   hwaddr firmware_end_addr) {
    যদি (info->is_32bit) {
        ফেরত QEMU_ALIGN_UP(firmware_end_addr, 4 * MiB);
    } অন্যথা {
        ফেরত QEMU_ALIGN_UP(firmware_end_addr, 2 * MiB);
    }
}

ধ্রুবক অক্ষর *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    যদি (riscv_is_32bit(harts)) {
        ফেরত RISCV32_BIOS_BIN;
    }

    ফেরত RISCV64_BIOS_BIN;
}

স্থির অক্ষর *riscv_find_bios(ধ্রুবক অক্ষর *bios_filename)
{
    অক্ষর *filename;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    যদি (filename == ফাঁকা) {
        যদি (!qtest_enabled()) {
            /*
             * We only ship OpenSBI binary bios images in the QEMU source.
             * For machines that use images other than the default bios,
             * running QEMU test will complain hence let's suppress the error
             * report for QEMU testing.
             */
            error_report("Unable to find the RISC-V BIOS \"%s\"",
                         bios_filename);
            exit(1);
        }
    }

    ফেরত filename;
}

অক্ষর *riscv_find_firmware(ধ্রুবক অক্ষর *firmware_filename,
                          ধ্রুবক অক্ষর *default_machine_firmware)
{
    অক্ষর *filename = ফাঁকা;

    যদি ((!firmware_filename) || (!strcmp(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_find_bios(default_machine_firmware);
    } অন্যথা যদি (strcmp(firmware_filename, "none")) {
        filename = riscv_find_bios(firmware_filename);
    }

    ফেরত filename;
}

hwaddr riscv_find_and_load_firmware(MachineState *machine,
                                    ধ্রুবক RISCVBootInfo *info,
                                    ধ্রুবক অক্ষর *default_machine_firmware,
                                    hwaddr *firmware_load_addr,
                                    প্রতীক_fn_t sym_cb)
{
    অক্ষর *firmware_filename;
    hwaddr firmware_end_addr = *firmware_load_addr;

    firmware_filename = riscv_find_firmware(machine->firmware,
                                            default_machine_firmware);

    যদি (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_end_addr = riscv_load_firmware(machine, info,
                                                firmware_filename,
                                                firmware_load_addr, sym_cb);
        g_free(firmware_filename);
    }

    ফেরত firmware_end_addr;
}

hwaddr riscv_load_firmware(MachineState *machine,
                           ধ্রুবক RISCVBootInfo *info,
                           ধ্রুবক অক্ষর *firmware_filename,
                           hwaddr *firmware_load_addr,
                           প্রতীক_fn_t sym_cb)
{
    uint64_t mem_আকার = info->ram_নিচের_আকার ?: machine->ram_আকার;
    uint64_t firmware_entry, firmware_end;
    ssize_t firmware_আকার;

    g_assert(firmware_filename != ফাঁকা);

    firmware_আকার = load_elf_ram_sym(firmware_filename, ফাঁকা, ফাঁকা, ফাঁকা,
                                     &firmware_entry, ফাঁকা, &firmware_end,
                                     ফাঁকা, 0, EM_RISCV, 1, 0, ফাঁকা, false,
                                     sym_cb);
    যদি (firmware_আকার > 0) {
        *firmware_load_addr = firmware_entry;
        ফেরত firmware_end;
    }

    যদি (firmware_আকার != ELF_LOAD_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    load_elf_strerror(firmware_আকার));
    }

    firmware_আকার = load_image_targphys_as(firmware_filename,
                                           *firmware_load_addr,
                                           mem_আকার, ফাঁকা,
                                           ফাঁকা);

    যদি (firmware_আকার > 0) {
        ফেরত *firmware_load_addr + firmware_আকার;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 load_elf_strerror(firmware_আকার));
    exit(1);
}

স্থির শূন্য riscv_load_initrd(MachineState *machine, RISCVBootInfo *info)
{
    ধ্রুবক অক্ষর *filename = machine->initrd_filename;
    uint64_t mem_আকার = info->ram_নিচের_আকার ?: machine->ram_আকার;
    শূন্য *fdt = machine->fdt;
    hwaddr start, end;
    ssize_t আকার_2;

    g_assert(filename != ফাঁকা);

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
    start = info->image_নিচের_addr + MIN(mem_আকার / 2, 512 * MiB);

    আকার_2 = load_ramdisk(filename, start, mem_আকার - start);
    যদি (আকার_2 == -1) {
        আকার_2 = load_image_targphys(filename, start, mem_আকার - start, ফাঁকা);
        যদি (আকার_2 == -1) {
            error_report("could not load ramdisk '%s'", filename);
            exit(1);
        }
    }

    info->initrd_start = start;
    info->initrd_আকার = আকার_2;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    যদি (fdt) {
        end = start + আকার_2;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", start);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", end);
    }
}

শূন্য riscv_load_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_start_addr,
                       bool load_initrd,
                       প্রতীক_fn_t sym_cb)
{
    uint64_t mem_আকার = info->ram_নিচের_আকার ?: machine->ram_আকার;
    ধ্রুবক অক্ষর *kernel_filename = machine->kernel_filename;
    ssize_t kernel_আকার;
    শূন্য *fdt = machine->fdt;

    g_assert(kernel_filename != ফাঁকা);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_আকার = load_elf_ram_sym(kernel_filename, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা,
                                   &info->image_নিচের_addr, &info->image_উপরের_addr,
                                   ফাঁকা, ELFDATA2LSB, EM_RISCV,
                                   1, 0, ফাঁকা, true, sym_cb);
    যদি (kernel_আকার > 0) {
        info->kernel_আকার = kernel_আকার;
        যাও out;
    }

    kernel_আকার = load_uimage_as(kernel_filename, &info->image_নিচের_addr,
                                 ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);
    যদি (kernel_আকার > 0) {
        info->kernel_আকার = kernel_আকার;
        info->image_উপরের_addr = info->image_নিচের_addr + kernel_আকার;
        যাও out;
    }

    kernel_আকার = load_image_targphys_as(kernel_filename, kernel_start_addr,
                                         mem_আকার, ফাঁকা, ফাঁকা);
    যদি (kernel_আকার > 0) {
        info->kernel_আকার = kernel_আকার;
        info->image_নিচের_addr = kernel_start_addr;
        info->image_উপরের_addr = info->image_নিচের_addr + kernel_আকার;
        যাও out;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    exit(1);

out:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    যদি (info->is_32bit) {
        info->image_নিচের_addr = extract64(info->image_নিচের_addr, 0, 32);
    }

    যদি (load_initrd && machine->initrd_filename) {
        riscv_load_initrd(machine, info);
    }

    যদি (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_base, hwaddr dram_আকার,
                                MachineState *ms, ধ্রুবক RISCVBootInfo *info)
{
    পূর্ণসংখ্যা ret = fdt_pack(ms->fdt);
    hwaddr dram_end, অস্থায়ী;
    পূর্ণসংখ্যা fdtsize;
    uint64_t dtb_start, dtb_start_limit;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    যদি (fdtsize <= 0) {
        error_report("invalid device-tree");
        exit(1);
    }

    যদি (info->initrd_আকার) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_start_limit = info->initrd_start + info->initrd_আকার;
    } অন্যথা যদি (info->kernel_আকার) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_start_limit = info->image_উপরের_addr;
    } অন্যথা {
        /* Otherwise, do not check DTB overlapping */
        dtb_start_limit = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_end = dram_base;
    dram_end += dram_আকার ? MIN(ms->ram_আকার, dram_আকার) : ms->ram_আকার;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    যদি (!info->is_32bit) {
        অস্থায়ী = dram_end;
    } অন্যথা {
        অস্থায়ী = (dram_base < 3072 * MiB) ? MIN(dram_end, 3072 * MiB) : dram_end;
    }

    dtb_start = QEMU_ALIGN_DOWN(অস্থায়ী - fdtsize, 2 * MiB);

    যদি (dtb_start_limit && (dtb_start < dtb_start_limit)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        exit(1);
    }

    ফেরত dtb_start;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
শূন্য riscv_load_fdt(hwaddr fdt_addr, শূন্য *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_add_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &address_space_memory);
    qemu_register_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&address_space_memory, fdt_addr, fdtsize));
}

শূন্য riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_base, hwaddr rom_আকার,
                                  uint32_t reset_vec_আকার,
                                  uint64_t kernel_entry)
{
    কাঠামো fw_dynamic_info32 dinfo32;
    কাঠামো fw_dynamic_info64 dinfo64;
    শূন্য *dinfo_ptr = ফাঁকা;
    আকার_t dinfo_len;
    ধ্রুবক bool rv32 = riscv_is_32bit(harts);
    ধ্রুবক bool be = harts->harts[0].cfg.big_endian;

    যদি (rv32) {
        dinfo32.magic = be ? cpu_to_be32(FW_DYNAMIC_INFO_MAGIC_মান)
                           : cpu_to_le32(FW_DYNAMIC_INFO_MAGIC_মান);
        dinfo32.version = be ? cpu_to_be32(FW_DYNAMIC_INFO_VERSION)
                             : cpu_to_le32(FW_DYNAMIC_INFO_VERSION);
        dinfo32.পরবর্তী_mode = be ? cpu_to_be32(FW_DYNAMIC_INFO_পরবর্তী_MODE_S)
                               : cpu_to_le32(FW_DYNAMIC_INFO_পরবর্তী_MODE_S);
        dinfo32.পরবর্তী_addr = be ? cpu_to_be32(kernel_entry)
                               : cpu_to_le32(kernel_entry);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = আকার(dinfo32);
    } অন্যথা {
        dinfo64.magic = be ? cpu_to_be64(FW_DYNAMIC_INFO_MAGIC_মান)
                           : cpu_to_le64(FW_DYNAMIC_INFO_MAGIC_মান);
        dinfo64.version = be ? cpu_to_be64(FW_DYNAMIC_INFO_VERSION)
                             : cpu_to_le64(FW_DYNAMIC_INFO_VERSION);
        dinfo64.পরবর্তী_mode = be ? cpu_to_be64(FW_DYNAMIC_INFO_পরবর্তী_MODE_S)
                               : cpu_to_le64(FW_DYNAMIC_INFO_পরবর্তী_MODE_S);
        dinfo64.পরবর্তী_addr = be ? cpu_to_be64(kernel_entry)
                               : cpu_to_le64(kernel_entry);
        dinfo64.options = 0;
        dinfo64.boot_hart = 0;
        dinfo_ptr = &dinfo64;
        dinfo_len = আকার(dinfo64);
    }

    /**
     * copy the dynamic firmware info. This information is specific to
     * OpenSBI but doesn't break any other firmware as long as they don't
     * expect any certain value in "a2" register.
     */
    যদি (dinfo_len > (rom_আকার - reset_vec_আকার)) {
        error_report("not enough space to store dynamic firmware info");
        exit(1);
    }

    rom_add_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_base + reset_vec_আকার,
                           &address_space_memory);
}

#define CODE_শব্দগুলি 6
#define তথ্য_শব্দগুলি 4

শূন্য riscv_setup_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr start_addr,
                               hwaddr rom_base, hwaddr rom_আকার,
                               uint64_t kernel_entry,
                               uint64_t fdt_load_addr)
{
    ধ্রুবক bool rv32 = riscv_is_32bit(harts);
    ধ্রুবক bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[CODE_শব্দগুলি + তথ্য_শব্দগুলি];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    যদি (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } অন্যথা {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    যদি (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } অন্যথা {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    যদি (big_endian) {
        stq_be_p(&reset_vec[6], start_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_load_addr); /* fdt_laddr:   .dword */
    } অন্যথা {
        stq_le_p(&reset_vec[6], start_addr);
        stq_le_p(&reset_vec[8], fdt_load_addr);
    }

    rom_add_blob_fixed_as("mrom.reset", reset_vec, আকার(reset_vec),
                          rom_base, &address_space_memory);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_base, rom_আকার,
                                 আকার(reset_vec),
                                 kernel_entry);
}

শূন্য riscv_setup_direct_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

শূন্য riscv_setup_firmware_boot(MachineState *machine)
{
    যদি (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_find();

        প্রতিজ্ঞা(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        load_image_to_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_আকার, FW_CFG_KERNEL_তথ্য,
                             machine->kernel_filename,
                             true);
        load_image_to_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_আকার, FW_CFG_INITRD_তথ্য,
                             machine->initrd_filename, false);

        যদি (machine->kernel_cmdline) {
            fw_cfg_add_i32(fw_cfg, FW_CFG_CMDLINE_আকার,
                           strlen(machine->kernel_cmdline) + 1);
            fw_cfg_add_string(fw_cfg, FW_CFG_CMDLINE_তথ্য,
                              machine->kernel_cmdline);
        }
    }
}
