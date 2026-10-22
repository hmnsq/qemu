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

#ضمن "qemu/osdep.h"
#ضمن "qemu/bswap.h"
#ضمن "qemu/datadir.h"
#ضمن "qemu/units.h"
#ضمن "qemu/error-report.h"
#ضمن "exec/cpu-defs.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/riscv/boot_opensbi.h"
#ضمن "elf.h"
#ضمن "system/device_tree.h"
#ضمن "system/qtest.h"
#ضمن "system/kvm.h"
#ضمن "system/reset.h"

#ضمن <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    أرجع mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
حرف *riscv_plic_hart_config_string(صحيح hart_شمېر)
{
    g_autofree ثابت حرف **vals = g_new(ثابت حرف *, hart_شمېر + 1);
    صحيح i;

    لكل (i = 0; i < hart_شمېر; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        إذا (kvm_enabled()) {
            vals[i] = "S";
        } وإلا إذا (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } وإلا {
            vals[i] = "M";
        }
    }
    vals[i] = عدم;

    /* g_strjoinv() obliges us to cast away const here */
    أرجع g_strjoinv(",", (حرف **)vals);
}

فراغ riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_ټیټ_پیل = 0;
    info->ram_ټیټ_اندازه = 0;
    info->kernel_اندازه = 0;
    info->initrd_اندازه = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
فراغ riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr ټیټ_پیل, hwaddr ټیټ_اندازه)
{
    riscv_boot_info_init(info, harts);
    info->ram_ټیټ_پیل = ټیټ_پیل;
    info->ram_ټیټ_اندازه = ټیټ_اندازه;
}

vaddr riscv_calc_kernel_پیل_addr(ثابت RISCVBootInfo *info,
                                   hwaddr firmware_پای_ټکی_addr) {
    إذا (info->is_32bit) {
        أرجع QEMU_ALIGN_پورته(firmware_پای_ټکی_addr, 4 * MiB);
    } وإلا {
        أرجع QEMU_ALIGN_پورته(firmware_پای_ټکی_addr, 2 * MiB);
    }
}

ثابت حرف *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    إذا (riscv_is_32bit(harts)) {
        أرجع RISCV32_BIOS_BIN;
    }

    أرجع RISCV64_BIOS_BIN;
}

ساكن حرف *riscv_پیدا_کړه_bios(ثابت حرف *bios_filename)
{
    حرف *filename;

    filename = qemu_پیدا_کړه_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    إذا (filename == عدم) {
        إذا (!qtest_enabled()) {
            /*
             * We only ship OpenSBI binary bios images in the QEMU source.
             * For machines that use images other than the default bios,
             * running QEMU test will complain hence let's suppress the error
             * report for QEMU testing.
             */
            error_report("Unable to find the RISC-V BIOS \"%s\"",
                         bios_filename);
            وتل(1);
        }
    }

    أرجع filename;
}

حرف *riscv_پیدا_کړه_firmware(ثابت حرف *firmware_filename,
                          ثابت حرف *default_machine_firmware)
{
    حرف *filename = عدم;

    إذا ((!firmware_filename) || (!strcmp(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_پیدا_کړه_bios(default_machine_firmware);
    } وإلا إذا (strcmp(firmware_filename, "none")) {
        filename = riscv_پیدا_کړه_bios(firmware_filename);
    }

    أرجع filename;
}

hwaddr riscv_پیدا_کړه_and_بارول_firmware(MachineState *machine,
                                    ثابت RISCVBootInfo *info,
                                    ثابت حرف *default_machine_firmware,
                                    hwaddr *firmware_بارول_addr,
                                    نښه_fn_t sym_cb)
{
    حرف *firmware_filename;
    hwaddr firmware_پای_ټکی_addr = *firmware_بارول_addr;

    firmware_filename = riscv_پیدا_کړه_firmware(machine->firmware,
                                            default_machine_firmware);

    إذا (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_پای_ټکی_addr = riscv_بارول_firmware(machine, info,
                                                firmware_filename,
                                                firmware_بارول_addr, sym_cb);
        g_free(firmware_filename);
    }

    أرجع firmware_پای_ټکی_addr;
}

hwaddr riscv_بارول_firmware(MachineState *machine,
                           ثابت RISCVBootInfo *info,
                           ثابت حرف *firmware_filename,
                           hwaddr *firmware_بارول_addr,
                           نښه_fn_t sym_cb)
{
    uint64_t mem_اندازه = info->ram_ټیټ_اندازه ?: machine->ram_اندازه;
    uint64_t firmware_ثبت, firmware_پای_ټکی;
    ssize_t firmware_اندازه;

    g_assert(firmware_filename != عدم);

    firmware_اندازه = بارول_elf_ram_sym(firmware_filename, عدم, عدم, عدم,
                                     &firmware_ثبت, عدم, &firmware_پای_ټکی,
                                     عدم, 0, EM_RISCV, 1, 0, عدم, false,
                                     sym_cb);
    إذا (firmware_اندازه > 0) {
        *firmware_بارول_addr = firmware_ثبت;
        أرجع firmware_پای_ټکی;
    }

    إذا (firmware_اندازه != ELF_بارول_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    بارول_elf_strerror(firmware_اندازه));
    }

    firmware_اندازه = بارول_image_targphys_as(firmware_filename,
                                           *firmware_بارول_addr,
                                           mem_اندازه, عدم,
                                           عدم);

    إذا (firmware_اندازه > 0) {
        أرجع *firmware_بارول_addr + firmware_اندازه;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 بارول_elf_strerror(firmware_اندازه));
    وتل(1);
}

ساكن فراغ riscv_بارول_initrd(MachineState *machine, RISCVBootInfo *info)
{
    ثابت حرف *filename = machine->initrd_filename;
    uint64_t mem_اندازه = info->ram_ټیټ_اندازه ?: machine->ram_اندازه;
    فراغ *fdt = machine->fdt;
    hwaddr پیل, پای_ټکی;
    ssize_t اندازه;

    g_assert(filename != عدم);

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
    پیل = info->image_ټیټ_addr + MIN(mem_اندازه / 2, 512 * MiB);

    اندازه = بارول_ramdisk(filename, پیل, mem_اندازه - پیل);
    إذا (اندازه == -1) {
        اندازه = بارول_image_targphys(filename, پیل, mem_اندازه - پیل, عدم);
        إذا (اندازه == -1) {
            error_report("could not load ramdisk '%s'", filename);
            وتل(1);
        }
    }

    info->initrd_پیل = پیل;
    info->initrd_اندازه = اندازه;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    إذا (fdt) {
        پای_ټکی = پیل + اندازه;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", پیل);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", پای_ټکی);
    }
}

فراغ riscv_بارول_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_پیل_addr,
                       bool بارول_initrd,
                       نښه_fn_t sym_cb)
{
    uint64_t mem_اندازه = info->ram_ټیټ_اندازه ?: machine->ram_اندازه;
    ثابت حرف *kernel_filename = machine->kernel_filename;
    ssize_t kernel_اندازه;
    فراغ *fdt = machine->fdt;

    g_assert(kernel_filename != عدم);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_اندازه = بارول_elf_ram_sym(kernel_filename, عدم, عدم, عدم, عدم,
                                   &info->image_ټیټ_addr, &info->image_لوړ_addr,
                                   عدم, ELFDATA2LSB, EM_RISCV,
                                   1, 0, عدم, true, sym_cb);
    إذا (kernel_اندازه > 0) {
        info->kernel_اندازه = kernel_اندازه;
        اذهب بهر;
    }

    kernel_اندازه = بارول_uimage_as(kernel_filename, &info->image_ټیټ_addr,
                                 عدم, عدم, عدم, عدم, عدم);
    إذا (kernel_اندازه > 0) {
        info->kernel_اندازه = kernel_اندازه;
        info->image_لوړ_addr = info->image_ټیټ_addr + kernel_اندازه;
        اذهب بهر;
    }

    kernel_اندازه = بارول_image_targphys_as(kernel_filename, kernel_پیل_addr,
                                         mem_اندازه, عدم, عدم);
    إذا (kernel_اندازه > 0) {
        info->kernel_اندازه = kernel_اندازه;
        info->image_ټیټ_addr = kernel_پیل_addr;
        info->image_لوړ_addr = info->image_ټیټ_addr + kernel_اندازه;
        اذهب بهر;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    وتل(1);

بهر:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    إذا (info->is_32bit) {
        info->image_ټیټ_addr = extract64(info->image_ټیټ_addr, 0, 32);
    }

    إذا (بارول_initrd && machine->initrd_filename) {
        riscv_بارول_initrd(machine, info);
    }

    إذا (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_بنسټ, hwaddr dram_اندازه,
                                MachineState *ms, ثابت RISCVBootInfo *info)
{
    صحيح ret = fdt_pack(ms->fdt);
    hwaddr dram_پای_ټکی, لنډمهاله;
    صحيح fdtsize;
    uint64_t dtb_پیل, dtb_پیل_حد;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    إذا (fdtsize <= 0) {
        error_report("invalid device-tree");
        وتل(1);
    }

    إذا (info->initrd_اندازه) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_پیل_حد = info->initrd_پیل + info->initrd_اندازه;
    } وإلا إذا (info->kernel_اندازه) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_پیل_حد = info->image_لوړ_addr;
    } وإلا {
        /* Otherwise, do not check DTB overlapping */
        dtb_پیل_حد = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_پای_ټکی = dram_بنسټ;
    dram_پای_ټکی += dram_اندازه ? MIN(ms->ram_اندازه, dram_اندازه) : ms->ram_اندازه;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    إذا (!info->is_32bit) {
        لنډمهاله = dram_پای_ټکی;
    } وإلا {
        لنډمهاله = (dram_بنسټ < 3072 * MiB) ? MIN(dram_پای_ټکی, 3072 * MiB) : dram_پای_ټکی;
    }

    dtb_پیل = QEMU_ALIGN_ښکته(لنډمهاله - fdtsize, 2 * MiB);

    إذا (dtb_پیل_حد && (dtb_پیل < dtb_پیل_حد)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        وتل(1);
    }

    أرجع dtb_پیل;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
فراغ riscv_بارول_fdt(hwaddr fdt_addr, فراغ *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_جمع_کړه_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &پته_space_حافظه);
    qemu_ثبتوونکی_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&پته_space_حافظه, fdt_addr, fdtsize));
}

فراغ riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_بنسټ, hwaddr rom_اندازه,
                                  uint32_t reset_vec_اندازه,
                                  uint64_t kernel_ثبت)
{
    بنية fw_dynamic_info32 dinfo32;
    بنية fw_dynamic_info64 dinfo64;
    فراغ *dinfo_ptr = عدم;
    اندازه_t dinfo_len;
    ثابت bool rv32 = riscv_is_32bit(harts);
    ثابت bool be = harts->harts[0].cfg.big_endian;

    إذا (rv32) {
        dinfo32.magic = be ? cpu_منزل_be32(FW_DYNAMIC_INFO_MAGIC_ارزښت)
                           : cpu_منزل_le32(FW_DYNAMIC_INFO_MAGIC_ارزښت);
        dinfo32.نسخه = be ? cpu_منزل_be32(FW_DYNAMIC_INFO_نسخه)
                             : cpu_منزل_le32(FW_DYNAMIC_INFO_نسخه);
        dinfo32.بل_ډېرپېښ = be ? cpu_منزل_be32(FW_DYNAMIC_INFO_بل_ډېرپېښ_S)
                               : cpu_منزل_le32(FW_DYNAMIC_INFO_بل_ډېرپېښ_S);
        dinfo32.بل_addr = be ? cpu_منزل_be32(kernel_ثبت)
                               : cpu_منزل_le32(kernel_ثبت);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = حجم(dinfo32);
    } وإلا {
        dinfo64.magic = be ? cpu_منزل_be64(FW_DYNAMIC_INFO_MAGIC_ارزښت)
                           : cpu_منزل_le64(FW_DYNAMIC_INFO_MAGIC_ارزښت);
        dinfo64.نسخه = be ? cpu_منزل_be64(FW_DYNAMIC_INFO_نسخه)
                             : cpu_منزل_le64(FW_DYNAMIC_INFO_نسخه);
        dinfo64.بل_ډېرپېښ = be ? cpu_منزل_be64(FW_DYNAMIC_INFO_بل_ډېرپېښ_S)
                               : cpu_منزل_le64(FW_DYNAMIC_INFO_بل_ډېرپېښ_S);
        dinfo64.بل_addr = be ? cpu_منزل_be64(kernel_ثبت)
                               : cpu_منزل_le64(kernel_ثبت);
        dinfo64.options = 0;
        dinfo64.boot_hart = 0;
        dinfo_ptr = &dinfo64;
        dinfo_len = حجم(dinfo64);
    }

    /**
     * copy the dynamic firmware info. This information is specific to
     * OpenSBI but doesn't break any other firmware as long as they don't
     * expect any certain value in "a2" register.
     */
    إذا (dinfo_len > (rom_اندازه - reset_vec_اندازه)) {
        error_report("not enough space to store dynamic firmware info");
        وتل(1);
    }

    rom_جمع_کړه_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_بنسټ + reset_vec_اندازه,
                           &پته_space_حافظه);
}

#تعریف_کړه کوډ_ټکي 6
#تعریف_کړه معلومات_ټکي 4

فراغ riscv_تیاری_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr پیل_addr,
                               hwaddr rom_بنسټ, hwaddr rom_اندازه,
                               uint64_t kernel_ثبت,
                               uint64_t fdt_بارول_addr)
{
    ثابت bool rv32 = riscv_is_32bit(harts);
    ثابت bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[کوډ_ټکي + معلومات_ټکي];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    إذا (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } وإلا {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    إذا (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } وإلا {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    إذا (big_endian) {
        stq_be_p(&reset_vec[6], پیل_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_بارول_addr); /* fdt_laddr:   .dword */
    } وإلا {
        stq_le_p(&reset_vec[6], پیل_addr);
        stq_le_p(&reset_vec[8], fdt_بارول_addr);
    }

    rom_جمع_کړه_blob_fixed_as("mrom.reset", reset_vec, حجم(reset_vec),
                          rom_بنسټ, &پته_space_حافظه);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_بنسټ, rom_اندازه,
                                 حجم(reset_vec),
                                 kernel_ثبت);
}

فراغ riscv_تیاری_مستقیم_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

فراغ riscv_تیاری_firmware_boot(MachineState *machine)
{
    إذا (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_پیدا_کړه();

        أكد(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        بارول_image_منزل_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_اندازه, FW_CFG_KERNEL_معلومات,
                             machine->kernel_filename,
                             true);
        بارول_image_منزل_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_اندازه, FW_CFG_INITRD_معلومات,
                             machine->initrd_filename, false);

        إذا (machine->kernel_cmdline) {
            fw_cfg_جمع_کړه_i32(fw_cfg, FW_CFG_CMDLINE_اندازه,
                           مزي_اوږدوالی(machine->kernel_cmdline) + 1);
            fw_cfg_جمع_کړه_string(fw_cfg, FW_CFG_CMDLINE_معلومات,
                              machine->kernel_cmdline);
        }
    }
}
