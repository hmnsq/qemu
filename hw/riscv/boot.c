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

#شامل "qemu/osdep.h"
#شامل "qemu/bswap.h"
#شامل "qemu/datadir.h"
#شامل "qemu/units.h"
#شامل "qemu/error-report.h"
#شامل "exec/cpu-defs.h"
#شامل "hw/core/boards.h"
#شامل "hw/core/loader.h"
#شامل "hw/riscv/boot.h"
#شامل "hw/riscv/boot_opensbi.h"
#شامل "elf.h"
#شامل "system/device_tree.h"
#شامل "system/qtest.h"
#شامل "system/kvm.h"
#شامل "system/reset.h"

#شامل <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    واپس mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
حرف *riscv_plic_hart_config_string(صحیح hart_گنتی)
{
    g_autofree ثابت حرف **vals = g_new(ثابت حرف *, hart_گنتی + 1);
    صحیح i;

    برائے (i = 0; i < hart_گنتی; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        اگر (kvm_enabled()) {
            vals[i] = "S";
        } ورنہ اگر (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } ورنہ {
            vals[i] = "M";
        }
    }
    vals[i] = تہی;

    /* g_strjoinv() obliges us to cast away const here */
    واپس g_strjoinv(",", (حرف **)vals);
}

خالی riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_کم_آغاز = 0;
    info->ram_کم_سائز = 0;
    info->kernel_سائز = 0;
    info->initrd_سائز = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
خالی riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr کم_آغاز, hwaddr کم_سائز)
{
    riscv_boot_info_init(info, harts);
    info->ram_کم_آغاز = کم_آغاز;
    info->ram_کم_سائز = کم_سائز;
}

vaddr riscv_calc_kernel_آغاز_addr(ثابت RISCVBootInfo *info,
                                   hwaddr firmware_انتہا_addr) {
    اگر (info->is_32bit) {
        واپس QEMU_ALIGN_اوپر(firmware_انتہا_addr, 4 * MiB);
    } ورنہ {
        واپس QEMU_ALIGN_اوپر(firmware_انتہا_addr, 2 * MiB);
    }
}

ثابت حرف *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    اگر (riscv_is_32bit(harts)) {
        واپس RISCV32_BIOS_BIN;
    }

    واپس RISCV64_BIOS_BIN;
}

ساکن حرف *riscv_ڈھونڈو_bios(ثابت حرف *bios_filename)
{
    حرف *filename;

    filename = qemu_ڈھونڈو_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    اگر (filename == تہی) {
        اگر (!qtest_enabled()) {
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

    واپس filename;
}

حرف *riscv_ڈھونڈو_firmware(ثابت حرف *firmware_filename,
                          ثابت حرف *default_machine_firmware)
{
    حرف *filename = تہی;

    اگر ((!firmware_filename) || (!strcmp(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_ڈھونڈو_bios(default_machine_firmware);
    } ورنہ اگر (strcmp(firmware_filename, "none")) {
        filename = riscv_ڈھونڈو_bios(firmware_filename);
    }

    واپس filename;
}

hwaddr riscv_ڈھونڈو_and_بھرو_firmware(MachineState *machine,
                                    ثابت RISCVBootInfo *info,
                                    ثابت حرف *default_machine_firmware,
                                    hwaddr *firmware_بھرو_addr,
                                    علامت_fn_t sym_cb)
{
    حرف *firmware_filename;
    hwaddr firmware_انتہا_addr = *firmware_بھرو_addr;

    firmware_filename = riscv_ڈھونڈو_firmware(machine->firmware,
                                            default_machine_firmware);

    اگر (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_انتہا_addr = riscv_بھرو_firmware(machine, info,
                                                firmware_filename,
                                                firmware_بھرو_addr, sym_cb);
        g_free(firmware_filename);
    }

    واپس firmware_انتہا_addr;
}

hwaddr riscv_بھرو_firmware(MachineState *machine,
                           ثابت RISCVBootInfo *info,
                           ثابت حرف *firmware_filename,
                           hwaddr *firmware_بھرو_addr,
                           علامت_fn_t sym_cb)
{
    uint64_t mem_سائز = info->ram_کم_سائز ?: machine->ram_سائز;
    uint64_t firmware_اندراج, firmware_انتہا;
    ssize_t firmware_سائز;

    g_assert(firmware_filename != تہی);

    firmware_سائز = بھرو_elf_ram_sym(firmware_filename, تہی, تہی, تہی,
                                     &firmware_اندراج, تہی, &firmware_انتہا,
                                     تہی, 0, EM_RISCV, 1, 0, تہی, false,
                                     sym_cb);
    اگر (firmware_سائز > 0) {
        *firmware_بھرو_addr = firmware_اندراج;
        واپس firmware_انتہا;
    }

    اگر (firmware_سائز != ELF_بھرو_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    بھرو_elf_strerror(firmware_سائز));
    }

    firmware_سائز = بھرو_image_targphys_as(firmware_filename,
                                           *firmware_بھرو_addr,
                                           mem_سائز, تہی,
                                           تہی);

    اگر (firmware_سائز > 0) {
        واپس *firmware_بھرو_addr + firmware_سائز;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 بھرو_elf_strerror(firmware_سائز));
    باہر_نکلو(1);
}

ساکن خالی riscv_بھرو_initrd(MachineState *machine, RISCVBootInfo *info)
{
    ثابت حرف *filename = machine->initrd_filename;
    uint64_t mem_سائز = info->ram_کم_سائز ?: machine->ram_سائز;
    خالی *fdt = machine->fdt;
    hwaddr آغاز, انتہا;
    ssize_t سائز;

    g_assert(filename != تہی);

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
    آغاز = info->image_کم_addr + MIN(mem_سائز / 2, 512 * MiB);

    سائز = بھرو_ramdisk(filename, آغاز, mem_سائز - آغاز);
    اگر (سائز == -1) {
        سائز = بھرو_image_targphys(filename, آغاز, mem_سائز - آغاز, تہی);
        اگر (سائز == -1) {
            error_report("could not load ramdisk '%s'", filename);
            باہر_نکلو(1);
        }
    }

    info->initrd_آغاز = آغاز;
    info->initrd_سائز = سائز;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    اگر (fdt) {
        انتہا = آغاز + سائز;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", آغاز);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", انتہا);
    }
}

خالی riscv_بھرو_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_آغاز_addr,
                       bool بھرو_initrd,
                       علامت_fn_t sym_cb)
{
    uint64_t mem_سائز = info->ram_کم_سائز ?: machine->ram_سائز;
    ثابت حرف *kernel_filename = machine->kernel_filename;
    ssize_t kernel_سائز;
    خالی *fdt = machine->fdt;

    g_assert(kernel_filename != تہی);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_سائز = بھرو_elf_ram_sym(kernel_filename, تہی, تہی, تہی, تہی,
                                   &info->image_کم_addr, &info->image_بلند_addr,
                                   تہی, ELFDATA2LSB, EM_RISCV,
                                   1, 0, تہی, true, sym_cb);
    اگر (kernel_سائز > 0) {
        info->kernel_سائز = kernel_سائز;
        جاؤ برآمد;
    }

    kernel_سائز = بھرو_uimage_as(kernel_filename, &info->image_کم_addr,
                                 تہی, تہی, تہی, تہی, تہی);
    اگر (kernel_سائز > 0) {
        info->kernel_سائز = kernel_سائز;
        info->image_بلند_addr = info->image_کم_addr + kernel_سائز;
        جاؤ برآمد;
    }

    kernel_سائز = بھرو_image_targphys_as(kernel_filename, kernel_آغاز_addr,
                                         mem_سائز, تہی, تہی);
    اگر (kernel_سائز > 0) {
        info->kernel_سائز = kernel_سائز;
        info->image_کم_addr = kernel_آغاز_addr;
        info->image_بلند_addr = info->image_کم_addr + kernel_سائز;
        جاؤ برآمد;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    باہر_نکلو(1);

برآمد:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    اگر (info->is_32bit) {
        info->image_کم_addr = extract64(info->image_کم_addr, 0, 32);
    }

    اگر (بھرو_initrd && machine->initrd_filename) {
        riscv_بھرو_initrd(machine, info);
    }

    اگر (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_بنیاد, hwaddr dram_سائز,
                                MachineState *ms, ثابت RISCVBootInfo *info)
{
    صحیح ret = fdt_pack(ms->fdt);
    hwaddr dram_انتہا, عارضی;
    صحیح fdtsize;
    uint64_t dtb_آغاز, dtb_آغاز_حد_بندی;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    اگر (fdtsize <= 0) {
        error_report("invalid device-tree");
        باہر_نکلو(1);
    }

    اگر (info->initrd_سائز) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_آغاز_حد_بندی = info->initrd_آغاز + info->initrd_سائز;
    } ورنہ اگر (info->kernel_سائز) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_آغاز_حد_بندی = info->image_بلند_addr;
    } ورنہ {
        /* Otherwise, do not check DTB overlapping */
        dtb_آغاز_حد_بندی = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_انتہا = dram_بنیاد;
    dram_انتہا += dram_سائز ? MIN(ms->ram_سائز, dram_سائز) : ms->ram_سائز;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    اگر (!info->is_32bit) {
        عارضی = dram_انتہا;
    } ورنہ {
        عارضی = (dram_بنیاد < 3072 * MiB) ? MIN(dram_انتہا, 3072 * MiB) : dram_انتہا;
    }

    dtb_آغاز = QEMU_ALIGN_نیچے_کو(عارضی - fdtsize, 2 * MiB);

    اگر (dtb_آغاز_حد_بندی && (dtb_آغاز < dtb_آغاز_حد_بندی)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        باہر_نکلو(1);
    }

    واپس dtb_آغاز;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
خالی riscv_بھرو_fdt(hwaddr fdt_addr, خالی *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_جوڑو_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &پتہ_space_حافظہ);
    qemu_رجسٹر_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&پتہ_space_حافظہ, fdt_addr, fdtsize));
}

خالی riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_بنیاد, hwaddr rom_سائز,
                                  uint32_t reset_vec_سائز,
                                  uint64_t kernel_اندراج)
{
    ساخت fw_dynamic_info32 dinfo32;
    ساخت fw_dynamic_info64 dinfo64;
    خالی *dinfo_ptr = تہی;
    سائز_t dinfo_len;
    ثابت bool rv32 = riscv_is_32bit(harts);
    ثابت bool be = harts->harts[0].cfg.big_endian;

    اگر (rv32) {
        dinfo32.magic = be ? cpu_مقصد_be32(FW_DYNAMIC_INFO_MAGIC_قدر)
                           : cpu_مقصد_le32(FW_DYNAMIC_INFO_MAGIC_قدر);
        dinfo32.نسخہ = be ? cpu_مقصد_be32(FW_DYNAMIC_INFO_نسخہ)
                             : cpu_مقصد_le32(FW_DYNAMIC_INFO_نسخہ);
        dinfo32.اگلا_منوال = be ? cpu_مقصد_be32(FW_DYNAMIC_INFO_اگلا_منوال_S)
                               : cpu_مقصد_le32(FW_DYNAMIC_INFO_اگلا_منوال_S);
        dinfo32.اگلا_addr = be ? cpu_مقصد_be32(kernel_اندراج)
                               : cpu_مقصد_le32(kernel_اندراج);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = حجم(dinfo32);
    } ورنہ {
        dinfo64.magic = be ? cpu_مقصد_be64(FW_DYNAMIC_INFO_MAGIC_قدر)
                           : cpu_مقصد_le64(FW_DYNAMIC_INFO_MAGIC_قدر);
        dinfo64.نسخہ = be ? cpu_مقصد_be64(FW_DYNAMIC_INFO_نسخہ)
                             : cpu_مقصد_le64(FW_DYNAMIC_INFO_نسخہ);
        dinfo64.اگلا_منوال = be ? cpu_مقصد_be64(FW_DYNAMIC_INFO_اگلا_منوال_S)
                               : cpu_مقصد_le64(FW_DYNAMIC_INFO_اگلا_منوال_S);
        dinfo64.اگلا_addr = be ? cpu_مقصد_be64(kernel_اندراج)
                               : cpu_مقصد_le64(kernel_اندراج);
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
    اگر (dinfo_len > (rom_سائز - reset_vec_سائز)) {
        error_report("not enough space to store dynamic firmware info");
        باہر_نکلو(1);
    }

    rom_جوڑو_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_بنیاد + reset_vec_سائز,
                           &پتہ_space_حافظہ);
}

#تعریف_کرو رمز_الفاظ 6
#تعریف_کرو ڈیٹا_الفاظ 4

خالی riscv_تیاری_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr آغاز_addr,
                               hwaddr rom_بنیاد, hwaddr rom_سائز,
                               uint64_t kernel_اندراج,
                               uint64_t fdt_بھرو_addr)
{
    ثابت bool rv32 = riscv_is_32bit(harts);
    ثابت bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[رمز_الفاظ + ڈیٹا_الفاظ];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    اگر (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } ورنہ {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    اگر (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } ورنہ {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    اگر (big_endian) {
        stq_be_p(&reset_vec[6], آغاز_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_بھرو_addr); /* fdt_laddr:   .dword */
    } ورنہ {
        stq_le_p(&reset_vec[6], آغاز_addr);
        stq_le_p(&reset_vec[8], fdt_بھرو_addr);
    }

    rom_جوڑو_blob_fixed_as("mrom.reset", reset_vec, حجم(reset_vec),
                          rom_بنیاد, &پتہ_space_حافظہ);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_بنیاد, rom_سائز,
                                 حجم(reset_vec),
                                 kernel_اندراج);
}

خالی riscv_تیاری_براہ_راست_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

خالی riscv_تیاری_firmware_boot(MachineState *machine)
{
    اگر (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_ڈھونڈو();

        أكد(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        بھرو_image_مقصد_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_سائز, FW_CFG_KERNEL_ڈیٹا,
                             machine->kernel_filename,
                             true);
        بھرو_image_مقصد_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_سائز, FW_CFG_INITRD_ڈیٹا,
                             machine->initrd_filename, false);

        اگر (machine->kernel_cmdline) {
            fw_cfg_جوڑو_i32(fw_cfg, FW_CFG_CMDLINE_سائز,
                           لڑی_لمبائی(machine->kernel_cmdline) + 1);
            fw_cfg_جوڑو_string(fw_cfg, FW_CFG_CMDLINE_ڈیٹا,
                              machine->kernel_cmdline);
        }
    }
}
