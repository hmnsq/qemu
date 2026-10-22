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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/bswap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/datadir.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/units.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "exec/cpu-defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/loader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot_opensbi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "elf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/qtest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/kvm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/reset.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    ਵਾਪਸ mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
ਅੱਖਰ *riscv_plic_hart_config_string(ਪੂਰਨ hart_ਗਿਣਤੀ)
{
    g_autofree ਅਚਲ ਅੱਖਰ **vals = g_new(ਅਚਲ ਅੱਖਰ *, hart_ਗਿਣਤੀ + 1);
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < hart_ਗਿਣਤੀ; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        ਜੇ (kvm_enabled()) {
            vals[i] = "S";
        } ਨਹੀਂਤਾਂ ਜੇ (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } ਨਹੀਂਤਾਂ {
            vals[i] = "M";
        }
    }
    vals[i] = ਨਿਰਾ;

    /* g_strjoinv() obliges us to cast away const here */
    ਵਾਪਸ g_strjoinv(",", (ਅੱਖਰ **)vals);
}

ਖਾਲੀ riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_ਨੀਵਾਂ_ਸ਼ੁਰੂ = 0;
    info->ram_ਨੀਵਾਂ_ਮਾਪ = 0;
    info->kernel_ਮਾਪ = 0;
    info->initrd_ਮਾਪ = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
ਖਾਲੀ riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr ਨੀਵਾਂ_ਸ਼ੁਰੂ, hwaddr ਨੀਵਾਂ_ਮਾਪ)
{
    riscv_boot_info_init(info, harts);
    info->ram_ਨੀਵਾਂ_ਸ਼ੁਰੂ = ਨੀਵਾਂ_ਸ਼ੁਰੂ;
    info->ram_ਨੀਵਾਂ_ਮਾਪ = ਨੀਵਾਂ_ਮਾਪ;
}

vaddr riscv_calc_kernel_ਸ਼ੁਰੂ_addr(ਅਚਲ RISCVBootInfo *info,
                                   hwaddr firmware_ਸਿਰਾ_addr) {
    ਜੇ (info->is_32bit) {
        ਵਾਪਸ QEMU_ALIGN_ਉੱਪਰ(firmware_ਸਿਰਾ_addr, 4 * MiB);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ QEMU_ALIGN_ਉੱਪਰ(firmware_ਸਿਰਾ_addr, 2 * MiB);
    }
}

ਅਚਲ ਅੱਖਰ *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    ਜੇ (riscv_is_32bit(harts)) {
        ਵਾਪਸ RISCV32_BIOS_BIN;
    }

    ਵਾਪਸ RISCV64_BIOS_BIN;
}

ਸਥਿਰ ਅੱਖਰ *riscv_ਲੱਭੋ_bios(ਅਚਲ ਅੱਖਰ *bios_filename)
{
    ਅੱਖਰ *filename;

    filename = qemu_ਲੱਭੋ_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    ਜੇ (filename == ਨਿਰਾ) {
        ਜੇ (!qtest_enabled()) {
            /*
             * We only ship OpenSBI binary bios images in the QEMU source.
             * For machines that use images other than the default bios,
             * running QEMU test will complain hence let's suppress the error
             * report for QEMU testing.
             */
            error_report("Unable to find the RISC-V BIOS \"%s\"",
                         bios_filename);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
    }

    ਵਾਪਸ filename;
}

ਅੱਖਰ *riscv_ਲੱਭੋ_firmware(ਅਚਲ ਅੱਖਰ *firmware_filename,
                          ਅਚਲ ਅੱਖਰ *default_machine_firmware)
{
    ਅੱਖਰ *filename = ਨਿਰਾ;

    ਜੇ ((!firmware_filename) || (!strcmp(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_ਲੱਭੋ_bios(default_machine_firmware);
    } ਨਹੀਂਤਾਂ ਜੇ (strcmp(firmware_filename, "none")) {
        filename = riscv_ਲੱਭੋ_bios(firmware_filename);
    }

    ਵਾਪਸ filename;
}

hwaddr riscv_ਲੱਭੋ_and_ਲੱਦੋ_firmware(MachineState *machine,
                                    ਅਚਲ RISCVBootInfo *info,
                                    ਅਚਲ ਅੱਖਰ *default_machine_firmware,
                                    hwaddr *firmware_ਲੱਦੋ_addr,
                                    ਚਿੰਨ੍ਹ_fn_t sym_cb)
{
    ਅੱਖਰ *firmware_filename;
    hwaddr firmware_ਸਿਰਾ_addr = *firmware_ਲੱਦੋ_addr;

    firmware_filename = riscv_ਲੱਭੋ_firmware(machine->firmware,
                                            default_machine_firmware);

    ਜੇ (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_ਸਿਰਾ_addr = riscv_ਲੱਦੋ_firmware(machine, info,
                                                firmware_filename,
                                                firmware_ਲੱਦੋ_addr, sym_cb);
        g_free(firmware_filename);
    }

    ਵਾਪਸ firmware_ਸਿਰਾ_addr;
}

hwaddr riscv_ਲੱਦੋ_firmware(MachineState *machine,
                           ਅਚਲ RISCVBootInfo *info,
                           ਅਚਲ ਅੱਖਰ *firmware_filename,
                           hwaddr *firmware_ਲੱਦੋ_addr,
                           ਚਿੰਨ੍ਹ_fn_t sym_cb)
{
    uint64_t mem_ਮਾਪ = info->ram_ਨੀਵਾਂ_ਮਾਪ ?: machine->ram_ਮਾਪ;
    uint64_t firmware_ਇੰਦਰਾਜ, firmware_ਸਿਰਾ;
    ssize_t firmware_ਮਾਪ;

    g_assert(firmware_filename != ਨਿਰਾ);

    firmware_ਮਾਪ = ਲੱਦੋ_elf_ram_sym(firmware_filename, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
                                     &firmware_ਇੰਦਰਾਜ, ਨਿਰਾ, &firmware_ਸਿਰਾ,
                                     ਨਿਰਾ, 0, EM_RISCV, 1, 0, ਨਿਰਾ, false,
                                     sym_cb);
    ਜੇ (firmware_ਮਾਪ > 0) {
        *firmware_ਲੱਦੋ_addr = firmware_ਇੰਦਰਾਜ;
        ਵਾਪਸ firmware_ਸਿਰਾ;
    }

    ਜੇ (firmware_ਮਾਪ != ELF_ਲੱਦੋ_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    ਲੱਦੋ_elf_strerror(firmware_ਮਾਪ));
    }

    firmware_ਮਾਪ = ਲੱਦੋ_image_targphys_as(firmware_filename,
                                           *firmware_ਲੱਦੋ_addr,
                                           mem_ਮਾਪ, ਨਿਰਾ,
                                           ਨਿਰਾ);

    ਜੇ (firmware_ਮਾਪ > 0) {
        ਵਾਪਸ *firmware_ਲੱਦੋ_addr + firmware_ਮਾਪ;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 ਲੱਦੋ_elf_strerror(firmware_ਮਾਪ));
    ਬਾਹਰ_ਨਿਕਲੋ(1);
}

ਸਥਿਰ ਖਾਲੀ riscv_ਲੱਦੋ_initrd(MachineState *machine, RISCVBootInfo *info)
{
    ਅਚਲ ਅੱਖਰ *filename = machine->initrd_filename;
    uint64_t mem_ਮਾਪ = info->ram_ਨੀਵਾਂ_ਮਾਪ ?: machine->ram_ਮਾਪ;
    ਖਾਲੀ *fdt = machine->fdt;
    hwaddr ਸ਼ੁਰੂ, ਸਿਰਾ;
    ssize_t ਮਾਪ;

    g_assert(filename != ਨਿਰਾ);

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
    ਸ਼ੁਰੂ = info->image_ਨੀਵਾਂ_addr + MIN(mem_ਮਾਪ / 2, 512 * MiB);

    ਮਾਪ = ਲੱਦੋ_ramdisk(filename, ਸ਼ੁਰੂ, mem_ਮਾਪ - ਸ਼ੁਰੂ);
    ਜੇ (ਮਾਪ == -1) {
        ਮਾਪ = ਲੱਦੋ_image_targphys(filename, ਸ਼ੁਰੂ, mem_ਮਾਪ - ਸ਼ੁਰੂ, ਨਿਰਾ);
        ਜੇ (ਮਾਪ == -1) {
            error_report("could not load ramdisk '%s'", filename);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }
    }

    info->initrd_ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    info->initrd_ਮਾਪ = ਮਾਪ;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    ਜੇ (fdt) {
        ਸਿਰਾ = ਸ਼ੁਰੂ + ਮਾਪ;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", ਸ਼ੁਰੂ);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", ਸਿਰਾ);
    }
}

ਖਾਲੀ riscv_ਲੱਦੋ_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_ਸ਼ੁਰੂ_addr,
                       bool ਲੱਦੋ_initrd,
                       ਚਿੰਨ੍ਹ_fn_t sym_cb)
{
    uint64_t mem_ਮਾਪ = info->ram_ਨੀਵਾਂ_ਮਾਪ ?: machine->ram_ਮਾਪ;
    ਅਚਲ ਅੱਖਰ *kernel_filename = machine->kernel_filename;
    ssize_t kernel_ਮਾਪ;
    ਖਾਲੀ *fdt = machine->fdt;

    g_assert(kernel_filename != ਨਿਰਾ);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_ਮਾਪ = ਲੱਦੋ_elf_ram_sym(kernel_filename, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
                                   &info->image_ਨੀਵਾਂ_addr, &info->image_ਉੱਚਾ_addr,
                                   ਨਿਰਾ, ELFDATA2LSB, EM_RISCV,
                                   1, 0, ਨਿਰਾ, true, sym_cb);
    ਜੇ (kernel_ਮਾਪ > 0) {
        info->kernel_ਮਾਪ = kernel_ਮਾਪ;
        ਜਾਓ ਬਾਹਰਲਾ;
    }

    kernel_ਮਾਪ = ਲੱਦੋ_uimage_as(kernel_filename, &info->image_ਨੀਵਾਂ_addr,
                                 ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (kernel_ਮਾਪ > 0) {
        info->kernel_ਮਾਪ = kernel_ਮਾਪ;
        info->image_ਉੱਚਾ_addr = info->image_ਨੀਵਾਂ_addr + kernel_ਮਾਪ;
        ਜਾਓ ਬਾਹਰਲਾ;
    }

    kernel_ਮਾਪ = ਲੱਦੋ_image_targphys_as(kernel_filename, kernel_ਸ਼ੁਰੂ_addr,
                                         mem_ਮਾਪ, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (kernel_ਮਾਪ > 0) {
        info->kernel_ਮਾਪ = kernel_ਮਾਪ;
        info->image_ਨੀਵਾਂ_addr = kernel_ਸ਼ੁਰੂ_addr;
        info->image_ਉੱਚਾ_addr = info->image_ਨੀਵਾਂ_addr + kernel_ਮਾਪ;
        ਜਾਓ ਬਾਹਰਲਾ;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    ਬਾਹਰ_ਨਿਕਲੋ(1);

ਬਾਹਰਲਾ:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    ਜੇ (info->is_32bit) {
        info->image_ਨੀਵਾਂ_addr = extract64(info->image_ਨੀਵਾਂ_addr, 0, 32);
    }

    ਜੇ (ਲੱਦੋ_initrd && machine->initrd_filename) {
        riscv_ਲੱਦੋ_initrd(machine, info);
    }

    ਜੇ (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_ਅਧਾਰ, hwaddr dram_ਮਾਪ,
                                MachineState *ms, ਅਚਲ RISCVBootInfo *info)
{
    ਪੂਰਨ ret = fdt_pack(ms->fdt);
    hwaddr dram_ਸਿਰਾ, ਆਰਜ਼ੀ;
    ਪੂਰਨ fdtsize;
    uint64_t dtb_ਸ਼ੁਰੂ, dtb_ਸ਼ੁਰੂ_ਹੱਦਬੰਦੀ;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    ਜੇ (fdtsize <= 0) {
        error_report("invalid device-tree");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਜੇ (info->initrd_ਮਾਪ) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_ਸ਼ੁਰੂ_ਹੱਦਬੰਦੀ = info->initrd_ਸ਼ੁਰੂ + info->initrd_ਮਾਪ;
    } ਨਹੀਂਤਾਂ ਜੇ (info->kernel_ਮਾਪ) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_ਸ਼ੁਰੂ_ਹੱਦਬੰਦੀ = info->image_ਉੱਚਾ_addr;
    } ਨਹੀਂਤਾਂ {
        /* Otherwise, do not check DTB overlapping */
        dtb_ਸ਼ੁਰੂ_ਹੱਦਬੰਦੀ = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_ਸਿਰਾ = dram_ਅਧਾਰ;
    dram_ਸਿਰਾ += dram_ਮਾਪ ? MIN(ms->ram_ਮਾਪ, dram_ਮਾਪ) : ms->ram_ਮਾਪ;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    ਜੇ (!info->is_32bit) {
        ਆਰਜ਼ੀ = dram_ਸਿਰਾ;
    } ਨਹੀਂਤਾਂ {
        ਆਰਜ਼ੀ = (dram_ਅਧਾਰ < 3072 * MiB) ? MIN(dram_ਸਿਰਾ, 3072 * MiB) : dram_ਸਿਰਾ;
    }

    dtb_ਸ਼ੁਰੂ = QEMU_ALIGN_ਥੱਲੇ(ਆਰਜ਼ੀ - fdtsize, 2 * MiB);

    ਜੇ (dtb_ਸ਼ੁਰੂ_ਹੱਦਬੰਦੀ && (dtb_ਸ਼ੁਰੂ < dtb_ਸ਼ੁਰੂ_ਹੱਦਬੰਦੀ)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    ਵਾਪਸ dtb_ਸ਼ੁਰੂ;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
ਖਾਲੀ riscv_ਲੱਦੋ_fdt(hwaddr fdt_addr, ਖਾਲੀ *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_ਜੋੜੋ_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &ਪਤਾ_space_ਸਮ੍ਰਿਤੀ);
    qemu_ਰਜਿਸਟਰ_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&ਪਤਾ_space_ਸਮ੍ਰਿਤੀ, fdt_addr, fdtsize));
}

ਖਾਲੀ riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_ਅਧਾਰ, hwaddr rom_ਮਾਪ,
                                  uint32_t reset_vec_ਮਾਪ,
                                  uint64_t kernel_ਇੰਦਰਾਜ)
{
    ਬਣਤਰ fw_dynamic_info32 dinfo32;
    ਬਣਤਰ fw_dynamic_info64 dinfo64;
    ਖਾਲੀ *dinfo_ptr = ਨਿਰਾ;
    ਮਾਪ_t dinfo_len;
    ਅਚਲ bool rv32 = riscv_is_32bit(harts);
    ਅਚਲ bool be = harts->harts[0].cfg.big_endian;

    ਜੇ (rv32) {
        dinfo32.magic = be ? cpu_ਮੰਜ਼ਿਲ_be32(FW_DYNAMIC_INFO_MAGIC_ਮੁੱਲ)
                           : cpu_ਮੰਜ਼ਿਲ_le32(FW_DYNAMIC_INFO_MAGIC_ਮੁੱਲ);
        dinfo32.ਸੰਸਕਰਣ = be ? cpu_ਮੰਜ਼ਿਲ_be32(FW_DYNAMIC_INFO_ਸੰਸਕਰਣ)
                             : cpu_ਮੰਜ਼ਿਲ_le32(FW_DYNAMIC_INFO_ਸੰਸਕਰਣ);
        dinfo32.ਅਗਲਾ_ਬਹੁਲਕ = be ? cpu_ਮੰਜ਼ਿਲ_be32(FW_DYNAMIC_INFO_ਅਗਲਾ_ਬਹੁਲਕ_S)
                               : cpu_ਮੰਜ਼ਿਲ_le32(FW_DYNAMIC_INFO_ਅਗਲਾ_ਬਹੁਲਕ_S);
        dinfo32.ਅਗਲਾ_addr = be ? cpu_ਮੰਜ਼ਿਲ_be32(kernel_ਇੰਦਰਾਜ)
                               : cpu_ਮੰਜ਼ਿਲ_le32(kernel_ਇੰਦਰਾਜ);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = ਆਕਾਰ(dinfo32);
    } ਨਹੀਂਤਾਂ {
        dinfo64.magic = be ? cpu_ਮੰਜ਼ਿਲ_be64(FW_DYNAMIC_INFO_MAGIC_ਮੁੱਲ)
                           : cpu_ਮੰਜ਼ਿਲ_le64(FW_DYNAMIC_INFO_MAGIC_ਮੁੱਲ);
        dinfo64.ਸੰਸਕਰਣ = be ? cpu_ਮੰਜ਼ਿਲ_be64(FW_DYNAMIC_INFO_ਸੰਸਕਰਣ)
                             : cpu_ਮੰਜ਼ਿਲ_le64(FW_DYNAMIC_INFO_ਸੰਸਕਰਣ);
        dinfo64.ਅਗਲਾ_ਬਹੁਲਕ = be ? cpu_ਮੰਜ਼ਿਲ_be64(FW_DYNAMIC_INFO_ਅਗਲਾ_ਬਹੁਲਕ_S)
                               : cpu_ਮੰਜ਼ਿਲ_le64(FW_DYNAMIC_INFO_ਅਗਲਾ_ਬਹੁਲਕ_S);
        dinfo64.ਅਗਲਾ_addr = be ? cpu_ਮੰਜ਼ਿਲ_be64(kernel_ਇੰਦਰਾਜ)
                               : cpu_ਮੰਜ਼ਿਲ_le64(kernel_ਇੰਦਰਾਜ);
        dinfo64.options = 0;
        dinfo64.boot_hart = 0;
        dinfo_ptr = &dinfo64;
        dinfo_len = ਆਕਾਰ(dinfo64);
    }

    /**
     * copy the dynamic firmware info. This information is specific to
     * OpenSBI but doesn't break any other firmware as long as they don't
     * expect any certain value in "a2" register.
     */
    ਜੇ (dinfo_len > (rom_ਮਾਪ - reset_vec_ਮਾਪ)) {
        error_report("not enough space to store dynamic firmware info");
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    rom_ਜੋੜੋ_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_ਅਧਾਰ + reset_vec_ਮਾਪ,
                           &ਪਤਾ_space_ਸਮ੍ਰਿਤੀ);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੋਡ_ਸ਼ਬਦ 6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕੜੇ_ਸ਼ਬਦ 4

ਖਾਲੀ riscv_ਤਿਆਰੀ_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr ਸ਼ੁਰੂ_addr,
                               hwaddr rom_ਅਧਾਰ, hwaddr rom_ਮਾਪ,
                               uint64_t kernel_ਇੰਦਰਾਜ,
                               uint64_t fdt_ਲੱਦੋ_addr)
{
    ਅਚਲ bool rv32 = riscv_is_32bit(harts);
    ਅਚਲ bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[ਕੋਡ_ਸ਼ਬਦ + ਅੰਕੜੇ_ਸ਼ਬਦ];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    ਜੇ (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } ਨਹੀਂਤਾਂ {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    ਜੇ (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } ਨਹੀਂਤਾਂ {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    ਜੇ (big_endian) {
        stq_be_p(&reset_vec[6], ਸ਼ੁਰੂ_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_ਲੱਦੋ_addr); /* fdt_laddr:   .dword */
    } ਨਹੀਂਤਾਂ {
        stq_le_p(&reset_vec[6], ਸ਼ੁਰੂ_addr);
        stq_le_p(&reset_vec[8], fdt_ਲੱਦੋ_addr);
    }

    rom_ਜੋੜੋ_blob_fixed_as("mrom.reset", reset_vec, ਆਕਾਰ(reset_vec),
                          rom_ਅਧਾਰ, &ਪਤਾ_space_ਸਮ੍ਰਿਤੀ);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_ਅਧਾਰ, rom_ਮਾਪ,
                                 ਆਕਾਰ(reset_vec),
                                 kernel_ਇੰਦਰਾਜ);
}

ਖਾਲੀ riscv_ਤਿਆਰੀ_ਸਿੱਧਾ_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

ਖਾਲੀ riscv_ਤਿਆਰੀ_firmware_boot(MachineState *machine)
{
    ਜੇ (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_ਲੱਭੋ();

        ਪ੍ਰਤਿਜ੍ਞਾ(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        ਲੱਦੋ_image_ਮੰਜ਼ਿਲ_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_ਮਾਪ, FW_CFG_KERNEL_ਅੰਕੜੇ,
                             machine->kernel_filename,
                             true);
        ਲੱਦੋ_image_ਮੰਜ਼ਿਲ_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_ਮਾਪ, FW_CFG_INITRD_ਅੰਕੜੇ,
                             machine->initrd_filename, false);

        ਜੇ (machine->kernel_cmdline) {
            fw_cfg_ਜੋੜੋ_i32(fw_cfg, FW_CFG_CMDLINE_ਮਾਪ,
                           ਲੜੀ_ਲੰਬਾਈ(machine->kernel_cmdline) + 1);
            fw_cfg_ਜੋੜੋ_string(fw_cfg, FW_CFG_CMDLINE_ਅੰਕੜੇ,
                              machine->kernel_cmdline);
        }
    }
}
