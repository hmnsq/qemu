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

#અન્તર્ભાવ "qemu/osdep.h"
#અન્તર્ભાવ "qemu/bswap.h"
#અન્તર્ભાવ "qemu/datadir.h"
#અન્તર્ભાવ "qemu/units.h"
#અન્તર્ભાવ "qemu/error-report.h"
#અન્તર્ભાવ "exec/cpu-defs.h"
#અન્તર્ભાવ "hw/core/boards.h"
#અન્તર્ભાવ "hw/core/loader.h"
#અન્તર્ભાવ "hw/riscv/boot.h"
#અન્તર્ભાવ "hw/riscv/boot_opensbi.h"
#અન્તર્ભાવ "elf.h"
#અન્તર્ભાવ "system/device_tree.h"
#અન્તર્ભાવ "system/qtest.h"
#અન્તર્ભાવ "system/kvm.h"
#અન્તર્ભાવ "system/reset.h"

#અન્તર્ભાવ <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    પાછા mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
અક્ષર *riscv_plic_hart_config_string(પૂર્ણાંક hart_ગણતરી)
{
    g_autofree અચલ અક્ષર **vals = g_new(અચલ અક્ષર *, hart_ગણતરી + 1);
    પૂર્ણાંક i;

    માટે (i = 0; i < hart_ગણતરી; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        જો (kvm_enabled()) {
            vals[i] = "S";
        } નહીંતર જો (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } નહીંતર {
            vals[i] = "M";
        }
    }
    vals[i] = ખાલી;

    /* g_strjoinv() obliges us to cast away const here */
    પાછા g_strjoinv(",", (અક્ષર **)vals);
}

શૂન્ય riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_નીચલું_start = 0;
    info->ram_નીચલું_size = 0;
    info->kernel_size = 0;
    info->initrd_size = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
શૂન્ય riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr નીચલું_start, hwaddr નીચલું_size)
{
    riscv_boot_info_init(info, harts);
    info->ram_નીચલું_start = નીચલું_start;
    info->ram_નીચલું_size = નીચલું_size;
}

vaddr riscv_calc_kernel_start_addr(અચલ RISCVBootInfo *info,
                                   hwaddr firmware_end_addr) {
    જો (info->is_32bit) {
        પાછા QEMU_ALIGN_UP(firmware_end_addr, 4 * MiB);
    } નહીંતર {
        પાછા QEMU_ALIGN_UP(firmware_end_addr, 2 * MiB);
    }
}

અચલ અક્ષર *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    જો (riscv_is_32bit(harts)) {
        પાછા RISCV32_BIOS_BIN;
    }

    પાછા RISCV64_BIOS_BIN;
}

સ્થિર અક્ષર *riscv_find_bios(અચલ અક્ષર *bios_filename)
{
    અક્ષર *filename;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    જો (filename == ખાલી) {
        જો (!qtest_enabled()) {
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

    પાછા filename;
}

અક્ષર *riscv_find_firmware(અચલ અક્ષર *firmware_filename,
                          અચલ અક્ષર *default_machine_firmware)
{
    અક્ષર *filename = ખાલી;

    જો ((!firmware_filename) || (!strcmp(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_find_bios(default_machine_firmware);
    } નહીંતર જો (strcmp(firmware_filename, "none")) {
        filename = riscv_find_bios(firmware_filename);
    }

    પાછા filename;
}

hwaddr riscv_find_and_load_firmware(MachineState *machine,
                                    અચલ RISCVBootInfo *info,
                                    અચલ અક્ષર *default_machine_firmware,
                                    hwaddr *firmware_load_addr,
                                    પ્રતીક_fn_t sym_cb)
{
    અક્ષર *firmware_filename;
    hwaddr firmware_end_addr = *firmware_load_addr;

    firmware_filename = riscv_find_firmware(machine->firmware,
                                            default_machine_firmware);

    જો (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_end_addr = riscv_load_firmware(machine, info,
                                                firmware_filename,
                                                firmware_load_addr, sym_cb);
        g_free(firmware_filename);
    }

    પાછા firmware_end_addr;
}

hwaddr riscv_load_firmware(MachineState *machine,
                           અચલ RISCVBootInfo *info,
                           અચલ અક્ષર *firmware_filename,
                           hwaddr *firmware_load_addr,
                           પ્રતીક_fn_t sym_cb)
{
    uint64_t mem_size = info->ram_નીચલું_size ?: machine->ram_size;
    uint64_t firmware_entry, firmware_end;
    ssize_t firmware_size;

    g_assert(firmware_filename != ખાલી);

    firmware_size = load_elf_ram_sym(firmware_filename, ખાલી, ખાલી, ખાલી,
                                     &firmware_entry, ખાલી, &firmware_end,
                                     ખાલી, 0, EM_RISCV, 1, 0, ખાલી, false,
                                     sym_cb);
    જો (firmware_size > 0) {
        *firmware_load_addr = firmware_entry;
        પાછા firmware_end;
    }

    જો (firmware_size != ELF_LOAD_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    load_elf_strerror(firmware_size));
    }

    firmware_size = load_image_targphys_as(firmware_filename,
                                           *firmware_load_addr,
                                           mem_size, ખાલી,
                                           ખાલી);

    જો (firmware_size > 0) {
        પાછા *firmware_load_addr + firmware_size;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 load_elf_strerror(firmware_size));
    exit(1);
}

સ્થિર શૂન્ય riscv_load_initrd(MachineState *machine, RISCVBootInfo *info)
{
    અચલ અક્ષર *filename = machine->initrd_filename;
    uint64_t mem_size = info->ram_નીચલું_size ?: machine->ram_size;
    શૂન્ય *fdt = machine->fdt;
    hwaddr start, end;
    ssize_t size;

    g_assert(filename != ખાલી);

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
    start = info->image_નીચલું_addr + MIN(mem_size / 2, 512 * MiB);

    size = load_ramdisk(filename, start, mem_size - start);
    જો (size == -1) {
        size = load_image_targphys(filename, start, mem_size - start, ખાલી);
        જો (size == -1) {
            error_report("could not load ramdisk '%s'", filename);
            exit(1);
        }
    }

    info->initrd_start = start;
    info->initrd_size = size;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    જો (fdt) {
        end = start + size;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", start);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", end);
    }
}

શૂન્ય riscv_load_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_start_addr,
                       bool load_initrd,
                       પ્રતીક_fn_t sym_cb)
{
    uint64_t mem_size = info->ram_નીચલું_size ?: machine->ram_size;
    અચલ અક્ષર *kernel_filename = machine->kernel_filename;
    ssize_t kernel_size;
    શૂન્ય *fdt = machine->fdt;

    g_assert(kernel_filename != ખાલી);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_size = load_elf_ram_sym(kernel_filename, ખાલી, ખાલી, ખાલી, ખાલી,
                                   &info->image_નીચલું_addr, &info->image_ઉપલું_addr,
                                   ખાલી, ELFDATA2LSB, EM_RISCV,
                                   1, 0, ખાલી, true, sym_cb);
    જો (kernel_size > 0) {
        info->kernel_size = kernel_size;
        જાઓ out;
    }

    kernel_size = load_uimage_as(kernel_filename, &info->image_નીચલું_addr,
                                 ખાલી, ખાલી, ખાલી, ખાલી, ખાલી);
    જો (kernel_size > 0) {
        info->kernel_size = kernel_size;
        info->image_ઉપલું_addr = info->image_નીચલું_addr + kernel_size;
        જાઓ out;
    }

    kernel_size = load_image_targphys_as(kernel_filename, kernel_start_addr,
                                         mem_size, ખાલી, ખાલી);
    જો (kernel_size > 0) {
        info->kernel_size = kernel_size;
        info->image_નીચલું_addr = kernel_start_addr;
        info->image_ઉપલું_addr = info->image_નીચલું_addr + kernel_size;
        જાઓ out;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    exit(1);

out:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    જો (info->is_32bit) {
        info->image_નીચલું_addr = extract64(info->image_નીચલું_addr, 0, 32);
    }

    જો (load_initrd && machine->initrd_filename) {
        riscv_load_initrd(machine, info);
    }

    જો (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_base, hwaddr dram_size,
                                MachineState *ms, અચલ RISCVBootInfo *info)
{
    પૂર્ણાંક ret = fdt_pack(ms->fdt);
    hwaddr dram_end, temp;
    પૂર્ણાંક fdtsize;
    uint64_t dtb_start, dtb_start_limit;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    જો (fdtsize <= 0) {
        error_report("invalid device-tree");
        exit(1);
    }

    જો (info->initrd_size) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_start_limit = info->initrd_start + info->initrd_size;
    } નહીંતર જો (info->kernel_size) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_start_limit = info->image_ઉપલું_addr;
    } નહીંતર {
        /* Otherwise, do not check DTB overlapping */
        dtb_start_limit = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_end = dram_base;
    dram_end += dram_size ? MIN(ms->ram_size, dram_size) : ms->ram_size;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    જો (!info->is_32bit) {
        temp = dram_end;
    } નહીંતર {
        temp = (dram_base < 3072 * MiB) ? MIN(dram_end, 3072 * MiB) : dram_end;
    }

    dtb_start = QEMU_ALIGN_DOWN(temp - fdtsize, 2 * MiB);

    જો (dtb_start_limit && (dtb_start < dtb_start_limit)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        exit(1);
    }

    પાછા dtb_start;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
શૂન્ય riscv_load_fdt(hwaddr fdt_addr, શૂન્ય *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_add_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &address_space_memory);
    qemu_register_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&address_space_memory, fdt_addr, fdtsize));
}

શૂન્ય riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_base, hwaddr rom_size,
                                  uint32_t reset_vec_size,
                                  uint64_t kernel_entry)
{
    સંરચના fw_dynamic_info32 dinfo32;
    સંરચના fw_dynamic_info64 dinfo64;
    શૂન્ય *dinfo_ptr = ખાલી;
    size_t dinfo_len;
    અચલ bool rv32 = riscv_is_32bit(harts);
    અચલ bool be = harts->harts[0].cfg.big_endian;

    જો (rv32) {
        dinfo32.magic = be ? cpu_to_be32(FW_DYNAMIC_INFO_MAGIC_કિંમત)
                           : cpu_to_le32(FW_DYNAMIC_INFO_MAGIC_કિંમત);
        dinfo32.version = be ? cpu_to_be32(FW_DYNAMIC_INFO_VERSION)
                             : cpu_to_le32(FW_DYNAMIC_INFO_VERSION);
        dinfo32.next_mode = be ? cpu_to_be32(FW_DYNAMIC_INFO_NEXT_MODE_S)
                               : cpu_to_le32(FW_DYNAMIC_INFO_NEXT_MODE_S);
        dinfo32.next_addr = be ? cpu_to_be32(kernel_entry)
                               : cpu_to_le32(kernel_entry);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = કદ(dinfo32);
    } નહીંતર {
        dinfo64.magic = be ? cpu_to_be64(FW_DYNAMIC_INFO_MAGIC_કિંમત)
                           : cpu_to_le64(FW_DYNAMIC_INFO_MAGIC_કિંમત);
        dinfo64.version = be ? cpu_to_be64(FW_DYNAMIC_INFO_VERSION)
                             : cpu_to_le64(FW_DYNAMIC_INFO_VERSION);
        dinfo64.next_mode = be ? cpu_to_be64(FW_DYNAMIC_INFO_NEXT_MODE_S)
                               : cpu_to_le64(FW_DYNAMIC_INFO_NEXT_MODE_S);
        dinfo64.next_addr = be ? cpu_to_be64(kernel_entry)
                               : cpu_to_le64(kernel_entry);
        dinfo64.options = 0;
        dinfo64.boot_hart = 0;
        dinfo_ptr = &dinfo64;
        dinfo_len = કદ(dinfo64);
    }

    /**
     * copy the dynamic firmware info. This information is specific to
     * OpenSBI but doesn't break any other firmware as long as they don't
     * expect any certain value in "a2" register.
     */
    જો (dinfo_len > (rom_size - reset_vec_size)) {
        error_report("not enough space to store dynamic firmware info");
        exit(1);
    }

    rom_add_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_base + reset_vec_size,
                           &address_space_memory);
}

#define CODE_શબ્દો 6
#define DATA_શબ્દો 4

શૂન્ય riscv_setup_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr start_addr,
                               hwaddr rom_base, hwaddr rom_size,
                               uint64_t kernel_entry,
                               uint64_t fdt_load_addr)
{
    અચલ bool rv32 = riscv_is_32bit(harts);
    અચલ bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[CODE_શબ્દો + DATA_શબ્દો];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    જો (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } નહીંતર {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    જો (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } નહીંતર {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    જો (big_endian) {
        stq_be_p(&reset_vec[6], start_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_load_addr); /* fdt_laddr:   .dword */
    } નહીંતર {
        stq_le_p(&reset_vec[6], start_addr);
        stq_le_p(&reset_vec[8], fdt_load_addr);
    }

    rom_add_blob_fixed_as("mrom.reset", reset_vec, કદ(reset_vec),
                          rom_base, &address_space_memory);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_base, rom_size,
                                 કદ(reset_vec),
                                 kernel_entry);
}

શૂન્ય riscv_setup_direct_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

શૂન્ય riscv_setup_firmware_boot(MachineState *machine)
{
    જો (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_find();

        પ્રતિજ્ઞા(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        load_image_to_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_SIZE, FW_CFG_KERNEL_DATA,
                             machine->kernel_filename,
                             true);
        load_image_to_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_SIZE, FW_CFG_INITRD_DATA,
                             machine->initrd_filename, false);

        જો (machine->kernel_cmdline) {
            fw_cfg_add_i32(fw_cfg, FW_CFG_CMDLINE_SIZE,
                           strlen(machine->kernel_cmdline) + 1);
            fw_cfg_add_string(fw_cfg, FW_CFG_CMDLINE_DATA,
                              machine->kernel_cmdline);
        }
    }
}
