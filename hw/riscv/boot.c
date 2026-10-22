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
حرف *riscv_plic_hart_config_string(صحيح hart_گنتی)
{
    g_autofree ثابت حرف **vals = g_new(ثابت حرف *, hart_گنتی + 1);
    صحيح i;

    لكل (i = 0; i < hart_گنتی; i++) {
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
    info->ram_کم_شروع = 0;
    info->ram_کم_ناپ = 0;
    info->kernel_ناپ = 0;
    info->initrd_ناپ = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
فراغ riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr کم_شروع, hwaddr کم_ناپ)
{
    riscv_boot_info_init(info, harts);
    info->ram_کم_شروع = کم_شروع;
    info->ram_کم_ناپ = کم_ناپ;
}

vaddr riscv_calc_kernel_شروع_addr(ثابت RISCVBootInfo *info,
                                   hwaddr firmware_سرا_addr) {
    إذا (info->is_32bit) {
        أرجع QEMU_ALIGN_اتے(firmware_سرا_addr, 4 * MiB);
    } وإلا {
        أرجع QEMU_ALIGN_اتے(firmware_سرا_addr, 2 * MiB);
    }
}

ثابت حرف *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    إذا (riscv_is_32bit(harts)) {
        أرجع RISCV32_BIOS_BIN;
    }

    أرجع RISCV64_BIOS_BIN;
}

ساكن حرف *riscv_لبھو_bios(ثابت حرف *bios_filename)
{
    حرف *filename;

    filename = qemu_لبھو_file(QEMU_FILE_TYPE_BIOS, bios_filename);
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
            باہر_نکلو(1);
        }
    }

    أرجع filename;
}

حرف *riscv_لبھو_firmware(ثابت حرف *firmware_filename,
                          ثابت حرف *default_machine_firmware)
{
    حرف *filename = عدم;

    إذا ((!firmware_filename) || (!strcmp(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_لبھو_bios(default_machine_firmware);
    } وإلا إذا (strcmp(firmware_filename, "none")) {
        filename = riscv_لبھو_bios(firmware_filename);
    }

    أرجع filename;
}

hwaddr riscv_لبھو_and_لدو_firmware(MachineState *machine,
                                    ثابت RISCVBootInfo *info,
                                    ثابت حرف *default_machine_firmware,
                                    hwaddr *firmware_لدو_addr,
                                    علامت_fn_t sym_cb)
{
    حرف *firmware_filename;
    hwaddr firmware_سرا_addr = *firmware_لدو_addr;

    firmware_filename = riscv_لبھو_firmware(machine->firmware,
                                            default_machine_firmware);

    إذا (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_سرا_addr = riscv_لدو_firmware(machine, info,
                                                firmware_filename,
                                                firmware_لدو_addr, sym_cb);
        g_free(firmware_filename);
    }

    أرجع firmware_سرا_addr;
}

hwaddr riscv_لدو_firmware(MachineState *machine,
                           ثابت RISCVBootInfo *info,
                           ثابت حرف *firmware_filename,
                           hwaddr *firmware_لدو_addr,
                           علامت_fn_t sym_cb)
{
    uint64_t mem_ناپ = info->ram_کم_ناپ ?: machine->ram_ناپ;
    uint64_t firmware_اندراج, firmware_سرا;
    ssize_t firmware_ناپ;

    g_assert(firmware_filename != عدم);

    firmware_ناپ = لدو_elf_ram_sym(firmware_filename, عدم, عدم, عدم,
                                     &firmware_اندراج, عدم, &firmware_سرا,
                                     عدم, 0, EM_RISCV, 1, 0, عدم, false,
                                     sym_cb);
    إذا (firmware_ناپ > 0) {
        *firmware_لدو_addr = firmware_اندراج;
        أرجع firmware_سرا;
    }

    إذا (firmware_ناپ != ELF_لدو_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    لدو_elf_strerror(firmware_ناپ));
    }

    firmware_ناپ = لدو_image_targphys_as(firmware_filename,
                                           *firmware_لدو_addr,
                                           mem_ناپ, عدم,
                                           عدم);

    إذا (firmware_ناپ > 0) {
        أرجع *firmware_لدو_addr + firmware_ناپ;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 لدو_elf_strerror(firmware_ناپ));
    باہر_نکلو(1);
}

ساكن فراغ riscv_لدو_initrd(MachineState *machine, RISCVBootInfo *info)
{
    ثابت حرف *filename = machine->initrd_filename;
    uint64_t mem_ناپ = info->ram_کم_ناپ ?: machine->ram_ناپ;
    فراغ *fdt = machine->fdt;
    hwaddr شروع, سرا;
    ssize_t ناپ;

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
    شروع = info->image_کم_addr + MIN(mem_ناپ / 2, 512 * MiB);

    ناپ = لدو_ramdisk(filename, شروع, mem_ناپ - شروع);
    إذا (ناپ == -1) {
        ناپ = لدو_image_targphys(filename, شروع, mem_ناپ - شروع, عدم);
        إذا (ناپ == -1) {
            error_report("could not load ramdisk '%s'", filename);
            باہر_نکلو(1);
        }
    }

    info->initrd_شروع = شروع;
    info->initrd_ناپ = ناپ;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    إذا (fdt) {
        سرا = شروع + ناپ;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", شروع);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", سرا);
    }
}

فراغ riscv_لدو_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_شروع_addr,
                       bool لدو_initrd,
                       علامت_fn_t sym_cb)
{
    uint64_t mem_ناپ = info->ram_کم_ناپ ?: machine->ram_ناپ;
    ثابت حرف *kernel_filename = machine->kernel_filename;
    ssize_t kernel_ناپ;
    فراغ *fdt = machine->fdt;

    g_assert(kernel_filename != عدم);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_ناپ = لدو_elf_ram_sym(kernel_filename, عدم, عدم, عدم, عدم,
                                   &info->image_کم_addr, &info->image_بلند_addr,
                                   عدم, ELFDATA2LSB, EM_RISCV,
                                   1, 0, عدم, true, sym_cb);
    إذا (kernel_ناپ > 0) {
        info->kernel_ناپ = kernel_ناپ;
        اذهب باہرلا;
    }

    kernel_ناپ = لدو_uimage_as(kernel_filename, &info->image_کم_addr,
                                 عدم, عدم, عدم, عدم, عدم);
    إذا (kernel_ناپ > 0) {
        info->kernel_ناپ = kernel_ناپ;
        info->image_بلند_addr = info->image_کم_addr + kernel_ناپ;
        اذهب باہرلا;
    }

    kernel_ناپ = لدو_image_targphys_as(kernel_filename, kernel_شروع_addr,
                                         mem_ناپ, عدم, عدم);
    إذا (kernel_ناپ > 0) {
        info->kernel_ناپ = kernel_ناپ;
        info->image_کم_addr = kernel_شروع_addr;
        info->image_بلند_addr = info->image_کم_addr + kernel_ناپ;
        اذهب باہرلا;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    باہر_نکلو(1);

باہرلا:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    إذا (info->is_32bit) {
        info->image_کم_addr = extract64(info->image_کم_addr, 0, 32);
    }

    إذا (لدو_initrd && machine->initrd_filename) {
        riscv_لدو_initrd(machine, info);
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_ادھار, hwaddr dram_ناپ,
                                MachineState *ms, ثابت RISCVBootInfo *info)
{
    صحيح ret = fdt_pack(ms->fdt);
    hwaddr dram_سرا, عارضی;
    صحيح fdtsize;
    uint64_t dtb_شروع, dtb_شروع_حد_بندی;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    إذا (fdtsize <= 0) {
        error_report("invalid device-tree");
        باہر_نکلو(1);
    }

    إذا (info->initrd_ناپ) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_شروع_حد_بندی = info->initrd_شروع + info->initrd_ناپ;
    } وإلا إذا (info->kernel_ناپ) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_شروع_حد_بندی = info->image_بلند_addr;
    } وإلا {
        /* Otherwise, do not check DTB overlapping */
        dtb_شروع_حد_بندی = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_سرا = dram_ادھار;
    dram_سرا += dram_ناپ ? MIN(ms->ram_ناپ, dram_ناپ) : ms->ram_ناپ;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    إذا (!info->is_32bit) {
        عارضی = dram_سرا;
    } وإلا {
        عارضی = (dram_ادھار < 3072 * MiB) ? MIN(dram_سرا, 3072 * MiB) : dram_سرا;
    }

    dtb_شروع = QEMU_ALIGN_تھلے(عارضی - fdtsize, 2 * MiB);

    إذا (dtb_شروع_حد_بندی && (dtb_شروع < dtb_شروع_حد_بندی)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        باہر_نکلو(1);
    }

    أرجع dtb_شروع;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
فراغ riscv_لدو_fdt(hwaddr fdt_addr, فراغ *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_جوڑو_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &پتہ_space_حافظہ);
    qemu_رجسٹر_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&پتہ_space_حافظہ, fdt_addr, fdtsize));
}

فراغ riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_ادھار, hwaddr rom_ناپ,
                                  uint32_t reset_vec_ناپ,
                                  uint64_t kernel_اندراج)
{
    بنية fw_dynamic_info32 dinfo32;
    بنية fw_dynamic_info64 dinfo64;
    فراغ *dinfo_ptr = عدم;
    ناپ_t dinfo_len;
    ثابت bool rv32 = riscv_is_32bit(harts);
    ثابت bool be = harts->harts[0].cfg.big_endian;

    إذا (rv32) {
        dinfo32.magic = be ? cpu_منزل_be32(FW_DYNAMIC_INFO_MAGIC_قدر)
                           : cpu_منزل_le32(FW_DYNAMIC_INFO_MAGIC_قدر);
        dinfo32.نسخہ = be ? cpu_منزل_be32(FW_DYNAMIC_INFO_نسخہ)
                             : cpu_منزل_le32(FW_DYNAMIC_INFO_نسخہ);
        dinfo32.اگلا_منوال = be ? cpu_منزل_be32(FW_DYNAMIC_INFO_اگلا_منوال_S)
                               : cpu_منزل_le32(FW_DYNAMIC_INFO_اگلا_منوال_S);
        dinfo32.اگلا_addr = be ? cpu_منزل_be32(kernel_اندراج)
                               : cpu_منزل_le32(kernel_اندراج);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = حجم(dinfo32);
    } وإلا {
        dinfo64.magic = be ? cpu_منزل_be64(FW_DYNAMIC_INFO_MAGIC_قدر)
                           : cpu_منزل_le64(FW_DYNAMIC_INFO_MAGIC_قدر);
        dinfo64.نسخہ = be ? cpu_منزل_be64(FW_DYNAMIC_INFO_نسخہ)
                             : cpu_منزل_le64(FW_DYNAMIC_INFO_نسخہ);
        dinfo64.اگلا_منوال = be ? cpu_منزل_be64(FW_DYNAMIC_INFO_اگلا_منوال_S)
                               : cpu_منزل_le64(FW_DYNAMIC_INFO_اگلا_منوال_S);
        dinfo64.اگلا_addr = be ? cpu_منزل_be64(kernel_اندراج)
                               : cpu_منزل_le64(kernel_اندراج);
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
    إذا (dinfo_len > (rom_ناپ - reset_vec_ناپ)) {
        error_report("not enough space to store dynamic firmware info");
        باہر_نکلو(1);
    }

    rom_جوڑو_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_ادھار + reset_vec_ناپ,
                           &پتہ_space_حافظہ);
}

#تعریف_کرو کوڈ_الفاظ 6
#تعریف_کرو انکڑے_الفاظ 4

فراغ riscv_تیاری_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr شروع_addr,
                               hwaddr rom_ادھار, hwaddr rom_ناپ,
                               uint64_t kernel_اندراج,
                               uint64_t fdt_لدو_addr)
{
    ثابت bool rv32 = riscv_is_32bit(harts);
    ثابت bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[کوڈ_الفاظ + انکڑے_الفاظ];

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
        stq_be_p(&reset_vec[6], شروع_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_لدو_addr); /* fdt_laddr:   .dword */
    } وإلا {
        stq_le_p(&reset_vec[6], شروع_addr);
        stq_le_p(&reset_vec[8], fdt_لدو_addr);
    }

    rom_جوڑو_blob_fixed_as("mrom.reset", reset_vec, حجم(reset_vec),
                          rom_ادھار, &پتہ_space_حافظہ);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_ادھار, rom_ناپ,
                                 حجم(reset_vec),
                                 kernel_اندراج);
}

فراغ riscv_تیاری_سدھا_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
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
        fw_cfg = fw_cfg_لبھو();

        أكد(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        لدو_image_منزل_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_ناپ, FW_CFG_KERNEL_انکڑے,
                             machine->kernel_filename,
                             true);
        لدو_image_منزل_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_ناپ, FW_CFG_INITRD_انکڑے,
                             machine->initrd_filename, false);

        إذا (machine->kernel_cmdline) {
            fw_cfg_جوڑو_i32(fw_cfg, FW_CFG_CMDLINE_ناپ,
                           لڑی_لمبائی(machine->kernel_cmdline) + 1);
            fw_cfg_جوڑو_string(fw_cfg, FW_CFG_CMDLINE_انکڑے,
                              machine->kernel_cmdline);
        }
    }
}
