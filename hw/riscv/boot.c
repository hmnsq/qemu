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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/bswap.h"
#ಅನ್ತರ್ಭಾವ "qemu/datadir.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "exec/cpu-defs.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot_opensbi.h"
#ಅನ್ತರ್ಭಾವ "elf.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "system/qtest.h"
#ಅನ್ತರ್ಭಾವ "system/kvm.h"
#ಅನ್ತರ್ಭಾವ "system/reset.h"

#ಅನ್ತರ್ಭಾವ <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    ಹಿಂತಿರುಗಿ mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
ಅಕ್ಷರ *riscv_plic_hart_config_string(ಪೂರ್ಣಾಂಕ hart_ಎಣಿಕೆ)
{
    g_autofree ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **vals = g_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, hart_ಎಣಿಕೆ + 1);
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < hart_ಎಣಿಕೆ; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        ಆದರೆ (kvm_enabled()) {
            vals[i] = "S";
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } ಇಲ್ಲದಿದ್ದರೆ {
            vals[i] = "M";
        }
    }
    vals[i] = ಖಾಲಿ;

    /* g_strjoinv() obliges us to cast away const here */
    ಹಿಂತಿರುಗಿ g_strjoinv(",", (ಅಕ್ಷರ **)vals);
}

ಶೂನ್ಯ riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_ಕೆಳಗಿನ_ಆರಂಭ = 0;
    info->ram_ಕೆಳಗಿನ_ಗಾತ್ರ = 0;
    info->kernel_ಗಾತ್ರ = 0;
    info->initrd_ಗಾತ್ರ = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
ಶೂನ್ಯ riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr ಕೆಳಗಿನ_ಆರಂಭ, hwaddr ಕೆಳಗಿನ_ಗಾತ್ರ)
{
    riscv_boot_info_init(info, harts);
    info->ram_ಕೆಳಗಿನ_ಆರಂಭ = ಕೆಳಗಿನ_ಆರಂಭ;
    info->ram_ಕೆಳಗಿನ_ಗಾತ್ರ = ಕೆಳಗಿನ_ಗಾತ್ರ;
}

vaddr riscv_calc_kernel_ಆರಂಭ_addr(ಸ್ಥಿರಾಂಕ RISCVBootInfo *info,
                                   hwaddr firmware_ಕೊನೆ_addr) {
    ಆದರೆ (info->is_32bit) {
        ಹಿಂತಿರುಗಿ QEMU_ALIGN_ಮೇಲೆ(firmware_ಕೊನೆ_addr, 4 * MiB);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ QEMU_ALIGN_ಮೇಲೆ(firmware_ಕೊನೆ_addr, 2 * MiB);
    }
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    ಆದರೆ (riscv_is_32bit(harts)) {
        ಹಿಂತಿರುಗಿ RISCV32_BIOS_BIN;
    }

    ಹಿಂತಿರುಗಿ RISCV64_BIOS_BIN;
}

ಸ್ಥಿರ ಅಕ್ಷರ *riscv_ಕಂಡುಹಿಡಿ_bios(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bios_filename)
{
    ಅಕ್ಷರ *filename;

    filename = qemu_ಕಂಡುಹಿಡಿ_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    ಆದರೆ (filename == ಖಾಲಿ) {
        ಆದರೆ (!qtest_enabled()) {
            /*
             * We only ship OpenSBI binary bios images in the QEMU source.
             * For machines that use images other than the default bios,
             * running QEMU test will complain hence let's suppress the error
             * report for QEMU testing.
             */
            error_report("Unable to find the RISC-V BIOS \"%s\"",
                         bios_filename);
            ನಿರ್ಗಮಿಸು(1);
        }
    }

    ಹಿಂತಿರುಗಿ filename;
}

ಅಕ್ಷರ *riscv_ಕಂಡುಹಿಡಿ_firmware(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *firmware_filename,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_machine_firmware)
{
    ಅಕ್ಷರ *filename = ಖಾಲಿ;

    ಆದರೆ ((!firmware_filename) || (!strcmp(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_ಕಂಡುಹಿಡಿ_bios(default_machine_firmware);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(firmware_filename, "none")) {
        filename = riscv_ಕಂಡುಹಿಡಿ_bios(firmware_filename);
    }

    ಹಿಂತಿರುಗಿ filename;
}

hwaddr riscv_ಕಂಡುಹಿಡಿ_and_ತುಂಬು_firmware(MachineState *machine,
                                    ಸ್ಥಿರಾಂಕ RISCVBootInfo *info,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *default_machine_firmware,
                                    hwaddr *firmware_ತುಂಬು_addr,
                                    ಚಿಹ್ನೆ_fn_t sym_cb)
{
    ಅಕ್ಷರ *firmware_filename;
    hwaddr firmware_ಕೊನೆ_addr = *firmware_ತುಂಬು_addr;

    firmware_filename = riscv_ಕಂಡುಹಿಡಿ_firmware(machine->firmware,
                                            default_machine_firmware);

    ಆದರೆ (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_ಕೊನೆ_addr = riscv_ತುಂಬು_firmware(machine, info,
                                                firmware_filename,
                                                firmware_ತುಂಬು_addr, sym_cb);
        g_free(firmware_filename);
    }

    ಹಿಂತಿರುಗಿ firmware_ಕೊನೆ_addr;
}

hwaddr riscv_ತುಂಬು_firmware(MachineState *machine,
                           ಸ್ಥಿರಾಂಕ RISCVBootInfo *info,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *firmware_filename,
                           hwaddr *firmware_ತುಂಬು_addr,
                           ಚಿಹ್ನೆ_fn_t sym_cb)
{
    uint64_t mem_ಗಾತ್ರ = info->ram_ಕೆಳಗಿನ_ಗಾತ್ರ ?: machine->ram_ಗಾತ್ರ;
    uint64_t firmware_ನಮೂದು, firmware_ಕೊನೆ;
    ssize_t firmware_ಗಾತ್ರ;

    g_assert(firmware_filename != ಖಾಲಿ);

    firmware_ಗಾತ್ರ = ತುಂಬು_elf_ram_sym(firmware_filename, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                                     &firmware_ನಮೂದು, ಖಾಲಿ, &firmware_ಕೊನೆ,
                                     ಖಾಲಿ, 0, EM_RISCV, 1, 0, ಖಾಲಿ, false,
                                     sym_cb);
    ಆದರೆ (firmware_ಗಾತ್ರ > 0) {
        *firmware_ತುಂಬು_addr = firmware_ನಮೂದು;
        ಹಿಂತಿರುಗಿ firmware_ಕೊನೆ;
    }

    ಆದರೆ (firmware_ಗಾತ್ರ != ELF_ತುಂಬು_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    ತುಂಬು_elf_strerror(firmware_ಗಾತ್ರ));
    }

    firmware_ಗಾತ್ರ = ತುಂಬು_image_targphys_as(firmware_filename,
                                           *firmware_ತುಂಬು_addr,
                                           mem_ಗಾತ್ರ, ಖಾಲಿ,
                                           ಖಾಲಿ);

    ಆದರೆ (firmware_ಗಾತ್ರ > 0) {
        ಹಿಂತಿರುಗಿ *firmware_ತುಂಬು_addr + firmware_ಗಾತ್ರ;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 ತುಂಬು_elf_strerror(firmware_ಗಾತ್ರ));
    ನಿರ್ಗಮಿಸು(1);
}

ಸ್ಥಿರ ಶೂನ್ಯ riscv_ತುಂಬು_initrd(MachineState *machine, RISCVBootInfo *info)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename = machine->initrd_filename;
    uint64_t mem_ಗಾತ್ರ = info->ram_ಕೆಳಗಿನ_ಗಾತ್ರ ?: machine->ram_ಗಾತ್ರ;
    ಶೂನ್ಯ *fdt = machine->fdt;
    hwaddr ಆರಂಭ, ಕೊನೆ;
    ssize_t ಗಾತ್ರ_2;

    g_assert(filename != ಖಾಲಿ);

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
    ಆರಂಭ = info->image_ಕೆಳಗಿನ_addr + MIN(mem_ಗಾತ್ರ / 2, 512 * MiB);

    ಗಾತ್ರ_2 = ತುಂಬು_ramdisk(filename, ಆರಂಭ, mem_ಗಾತ್ರ - ಆರಂಭ);
    ಆದರೆ (ಗಾತ್ರ_2 == -1) {
        ಗಾತ್ರ_2 = ತುಂಬು_image_targphys(filename, ಆರಂಭ, mem_ಗಾತ್ರ - ಆರಂಭ, ಖಾಲಿ);
        ಆದರೆ (ಗಾತ್ರ_2 == -1) {
            error_report("could not load ramdisk '%s'", filename);
            ನಿರ್ಗಮಿಸು(1);
        }
    }

    info->initrd_ಆರಂಭ = ಆರಂಭ;
    info->initrd_ಗಾತ್ರ = ಗಾತ್ರ_2;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    ಆದರೆ (fdt) {
        ಕೊನೆ = ಆರಂಭ + ಗಾತ್ರ_2;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", ಆರಂಭ);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", ಕೊನೆ);
    }
}

ಶೂನ್ಯ riscv_ತುಂಬು_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_ಆರಂಭ_addr,
                       bool ತುಂಬು_initrd,
                       ಚಿಹ್ನೆ_fn_t sym_cb)
{
    uint64_t mem_ಗಾತ್ರ = info->ram_ಕೆಳಗಿನ_ಗಾತ್ರ ?: machine->ram_ಗಾತ್ರ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *kernel_filename = machine->kernel_filename;
    ssize_t kernel_ಗಾತ್ರ;
    ಶೂನ್ಯ *fdt = machine->fdt;

    g_assert(kernel_filename != ಖಾಲಿ);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_ಗಾತ್ರ = ತುಂಬು_elf_ram_sym(kernel_filename, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                                   &info->image_ಕೆಳಗಿನ_addr, &info->image_ಮೇಲಿನ_addr,
                                   ಖಾಲಿ, ELFDATA2LSB, EM_RISCV,
                                   1, 0, ಖಾಲಿ, true, sym_cb);
    ಆದರೆ (kernel_ಗಾತ್ರ > 0) {
        info->kernel_ಗಾತ್ರ = kernel_ಗಾತ್ರ;
        ಹೋಗು ಹೊರಹೋಗು;
    }

    kernel_ಗಾತ್ರ = ತುಂಬು_uimage_as(kernel_filename, &info->image_ಕೆಳಗಿನ_addr,
                                 ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (kernel_ಗಾತ್ರ > 0) {
        info->kernel_ಗಾತ್ರ = kernel_ಗಾತ್ರ;
        info->image_ಮೇಲಿನ_addr = info->image_ಕೆಳಗಿನ_addr + kernel_ಗಾತ್ರ;
        ಹೋಗು ಹೊರಹೋಗು;
    }

    kernel_ಗಾತ್ರ = ತುಂಬು_image_targphys_as(kernel_filename, kernel_ಆರಂಭ_addr,
                                         mem_ಗಾತ್ರ, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (kernel_ಗಾತ್ರ > 0) {
        info->kernel_ಗಾತ್ರ = kernel_ಗಾತ್ರ;
        info->image_ಕೆಳಗಿನ_addr = kernel_ಆರಂಭ_addr;
        info->image_ಮೇಲಿನ_addr = info->image_ಕೆಳಗಿನ_addr + kernel_ಗಾತ್ರ;
        ಹೋಗು ಹೊರಹೋಗು;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    ನಿರ್ಗಮಿಸು(1);

ಹೊರಹೋಗು:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    ಆದರೆ (info->is_32bit) {
        info->image_ಕೆಳಗಿನ_addr = extract64(info->image_ಕೆಳಗಿನ_addr, 0, 32);
    }

    ಆದರೆ (ತುಂಬು_initrd && machine->initrd_filename) {
        riscv_ತುಂಬು_initrd(machine, info);
    }

    ಆದರೆ (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_ತಳ, hwaddr dram_ಗಾತ್ರ,
                                MachineState *ms, ಸ್ಥಿರಾಂಕ RISCVBootInfo *info)
{
    ಪೂರ್ಣಾಂಕ ret = fdt_pack(ms->fdt);
    hwaddr dram_ಕೊನೆ, ತಾತ್ಕಾಲಿಕ;
    ಪೂರ್ಣಾಂಕ fdtsize;
    uint64_t dtb_ಆರಂಭ, dtb_ಆರಂಭ_ಎಲ್ಲೆ;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    ಆದರೆ (fdtsize <= 0) {
        error_report("invalid device-tree");
        ನಿರ್ಗಮಿಸು(1);
    }

    ಆದರೆ (info->initrd_ಗಾತ್ರ) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_ಆರಂಭ_ಎಲ್ಲೆ = info->initrd_ಆರಂಭ + info->initrd_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (info->kernel_ಗಾತ್ರ) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_ಆರಂಭ_ಎಲ್ಲೆ = info->image_ಮೇಲಿನ_addr;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Otherwise, do not check DTB overlapping */
        dtb_ಆರಂಭ_ಎಲ್ಲೆ = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_ಕೊನೆ = dram_ತಳ;
    dram_ಕೊನೆ += dram_ಗಾತ್ರ ? MIN(ms->ram_ಗಾತ್ರ, dram_ಗಾತ್ರ) : ms->ram_ಗಾತ್ರ;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    ಆದರೆ (!info->is_32bit) {
        ತಾತ್ಕಾಲಿಕ = dram_ಕೊನೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ತಾತ್ಕಾಲಿಕ = (dram_ತಳ < 3072 * MiB) ? MIN(dram_ಕೊನೆ, 3072 * MiB) : dram_ಕೊನೆ;
    }

    dtb_ಆರಂಭ = QEMU_ALIGN_ಕೆಳಕ್ಕೆ(ತಾತ್ಕಾಲಿಕ - fdtsize, 2 * MiB);

    ಆದರೆ (dtb_ಆರಂಭ_ಎಲ್ಲೆ && (dtb_ಆರಂಭ < dtb_ಆರಂಭ_ಎಲ್ಲೆ)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        ನಿರ್ಗಮಿಸು(1);
    }

    ಹಿಂತಿರುಗಿ dtb_ಆರಂಭ;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
ಶೂನ್ಯ riscv_ತುಂಬು_fdt(hwaddr fdt_addr, ಶೂನ್ಯ *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_ಕೂಡಿಸು_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &ವಿಳಾಸ_space_ಸ್ಮರಣೆ);
    qemu_ರಿಜಿಸ್ಟರ್_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&ವಿಳಾಸ_space_ಸ್ಮರಣೆ, fdt_addr, fdtsize));
}

ಶೂನ್ಯ riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_ತಳ, hwaddr rom_ಗಾತ್ರ,
                                  uint32_t reset_vec_ಗಾತ್ರ,
                                  uint64_t kernel_ನಮೂದು)
{
    ರಚನೆ fw_dynamic_info32 dinfo32;
    ರಚನೆ fw_dynamic_info64 dinfo64;
    ಶೂನ್ಯ *dinfo_ptr = ಖಾಲಿ;
    ಗಾತ್ರ_t dinfo_len;
    ಸ್ಥಿರಾಂಕ bool rv32 = riscv_is_32bit(harts);
    ಸ್ಥಿರಾಂಕ bool be = harts->harts[0].cfg.big_endian;

    ಆದರೆ (rv32) {
        dinfo32.magic = be ? cpu_ಗಮ್ಯ_be32(FW_DYNAMIC_INFO_MAGIC_ಮೌಲ್ಯ)
                           : cpu_ಗಮ್ಯ_le32(FW_DYNAMIC_INFO_MAGIC_ಮೌಲ್ಯ);
        dinfo32.ಆವೃತ್ತಿ = be ? cpu_ಗಮ್ಯ_be32(FW_DYNAMIC_INFO_ಆವೃತ್ತಿ)
                             : cpu_ಗಮ್ಯ_le32(FW_DYNAMIC_INFO_ಆವೃತ್ತಿ);
        dinfo32.ಮುಂದಿನ_ಬಹುಲಕ = be ? cpu_ಗಮ್ಯ_be32(FW_DYNAMIC_INFO_ಮುಂದಿನ_ಬಹುಲಕ_S)
                               : cpu_ಗಮ್ಯ_le32(FW_DYNAMIC_INFO_ಮುಂದಿನ_ಬಹುಲಕ_S);
        dinfo32.ಮುಂದಿನ_addr = be ? cpu_ಗಮ್ಯ_be32(kernel_ನಮೂದು)
                               : cpu_ಗಮ್ಯ_le32(kernel_ನಮೂದು);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = ಗಾತ್ರ(dinfo32);
    } ಇಲ್ಲದಿದ್ದರೆ {
        dinfo64.magic = be ? cpu_ಗಮ್ಯ_be64(FW_DYNAMIC_INFO_MAGIC_ಮೌಲ್ಯ)
                           : cpu_ಗಮ್ಯ_le64(FW_DYNAMIC_INFO_MAGIC_ಮೌಲ್ಯ);
        dinfo64.ಆವೃತ್ತಿ = be ? cpu_ಗಮ್ಯ_be64(FW_DYNAMIC_INFO_ಆವೃತ್ತಿ)
                             : cpu_ಗಮ್ಯ_le64(FW_DYNAMIC_INFO_ಆವೃತ್ತಿ);
        dinfo64.ಮುಂದಿನ_ಬಹುಲಕ = be ? cpu_ಗಮ್ಯ_be64(FW_DYNAMIC_INFO_ಮುಂದಿನ_ಬಹುಲಕ_S)
                               : cpu_ಗಮ್ಯ_le64(FW_DYNAMIC_INFO_ಮುಂದಿನ_ಬಹುಲಕ_S);
        dinfo64.ಮುಂದಿನ_addr = be ? cpu_ಗಮ್ಯ_be64(kernel_ನಮೂದು)
                               : cpu_ಗಮ್ಯ_le64(kernel_ನಮೂದು);
        dinfo64.options = 0;
        dinfo64.boot_hart = 0;
        dinfo_ptr = &dinfo64;
        dinfo_len = ಗಾತ್ರ(dinfo64);
    }

    /**
     * copy the dynamic firmware info. This information is specific to
     * OpenSBI but doesn't break any other firmware as long as they don't
     * expect any certain value in "a2" register.
     */
    ಆದರೆ (dinfo_len > (rom_ಗಾತ್ರ - reset_vec_ಗಾತ್ರ)) {
        error_report("not enough space to store dynamic firmware info");
        ನಿರ್ಗಮಿಸು(1);
    }

    rom_ಕೂಡಿಸು_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_ತಳ + reset_vec_ಗಾತ್ರ,
                           &ವಿಳಾಸ_space_ಸ್ಮರಣೆ);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಂಕೇತ_ಪದಗಳು 6
#ವ್ಯಾಖ್ಯಾನಿಸು ದತ್ತಾಂಶ_ಪದಗಳು 4

ಶೂನ್ಯ riscv_ಸಿದ್ಧತೆ_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr ಆರಂಭ_addr,
                               hwaddr rom_ತಳ, hwaddr rom_ಗಾತ್ರ,
                               uint64_t kernel_ನಮೂದು,
                               uint64_t fdt_ತುಂಬು_addr)
{
    ಸ್ಥಿರಾಂಕ bool rv32 = riscv_is_32bit(harts);
    ಸ್ಥಿರಾಂಕ bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[ಸಂಕೇತ_ಪದಗಳು + ದತ್ತಾಂಶ_ಪದಗಳು];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    ಆದರೆ (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    ಆದರೆ (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } ಇಲ್ಲದಿದ್ದರೆ {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    ಆದರೆ (big_endian) {
        stq_be_p(&reset_vec[6], ಆರಂಭ_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_ತುಂಬು_addr); /* fdt_laddr:   .dword */
    } ಇಲ್ಲದಿದ್ದರೆ {
        stq_le_p(&reset_vec[6], ಆರಂಭ_addr);
        stq_le_p(&reset_vec[8], fdt_ತುಂಬು_addr);
    }

    rom_ಕೂಡಿಸು_blob_fixed_as("mrom.reset", reset_vec, ಗಾತ್ರ(reset_vec),
                          rom_ತಳ, &ವಿಳಾಸ_space_ಸ್ಮರಣೆ);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_ತಳ, rom_ಗಾತ್ರ,
                                 ಗಾತ್ರ(reset_vec),
                                 kernel_ನಮೂದು);
}

ಶೂನ್ಯ riscv_ಸಿದ್ಧತೆ_ನೇರ_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

ಶೂನ್ಯ riscv_ಸಿದ್ಧತೆ_firmware_boot(MachineState *machine)
{
    ಆದರೆ (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_ಕಂಡುಹಿಡಿ();

        ಪ್ರತಿಜ್ಞಾ(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        ತುಂಬು_image_ಗಮ್ಯ_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_ಗಾತ್ರ, FW_CFG_KERNEL_ದತ್ತಾಂಶ,
                             machine->kernel_filename,
                             true);
        ತುಂಬು_image_ಗಮ್ಯ_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_ಗಾತ್ರ, FW_CFG_INITRD_ದತ್ತಾಂಶ,
                             machine->initrd_filename, false);

        ಆದರೆ (machine->kernel_cmdline) {
            fw_cfg_ಕೂಡಿಸು_i32(fw_cfg, FW_CFG_CMDLINE_ಗಾತ್ರ,
                           ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(machine->kernel_cmdline) + 1);
            fw_cfg_ಕೂಡಿಸು_string(fw_cfg, FW_CFG_CMDLINE_ದತ್ತಾಂಶ,
                              machine->kernel_cmdline);
        }
    }
}
