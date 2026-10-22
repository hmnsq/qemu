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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/bswap.h"
#అన్తర్భావ "qemu/datadir.h"
#అన్తర్భావ "qemu/units.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "exec/cpu-defs.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/riscv/boot_opensbi.h"
#అన్తర్భావ "elf.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "system/qtest.h"
#అన్తర్భావ "system/kvm.h"
#అన్తర్భావ "system/reset.h"

#అన్తర్భావ <libfdt.h>

bool riscv_is_32bit(RISCVHartArrayState *harts)
{
    RISCVCPUClass *mcc = RISCV_CPU_GET_CLASS(&harts->harts[0]);
    తిరిగి mcc->def->misa_mxl_max == MXL_RV32;
}

/*
 * Return the per-socket PLIC hart topology configuration string
 * (caller must free with g_free())
 */
అక్షరం *riscv_plic_hart_config_string(పూర్ణాంకం hart_లెక్క)
{
    g_autofree స్థిరరాశి అక్షరం **vals = g_new(స్థిరరాశి అక్షరం *, hart_లెక్క + 1);
    పూర్ణాంకం i;

    కొరకు (i = 0; i < hart_లెక్క; i++) {
        CPUState *cs = qemu_get_cpu(i);
        CPURISCVState *env = &RISCV_CPU(cs)->env;

        ఒకవేళ (kvm_enabled()) {
            vals[i] = "S";
        } లేకపోతే ఒకవేళ (riscv_has_ext(env, RVS)) {
            vals[i] = "MS";
        } లేకపోతే {
            vals[i] = "M";
        }
    }
    vals[i] = ఖాళీ;

    /* g_strjoinv() obliges us to cast away const here */
    తిరిగి g_strjoinv(",", (అక్షరం **)vals);
}

శూన్య riscv_boot_info_init(RISCVBootInfo *info, RISCVHartArrayState *harts)
{
    info->ram_దిగువ_ప్రారంభం = 0;
    info->ram_దిగువ_కొలత = 0;
    info->kernel_కొలత = 0;
    info->initrd_కొలత = 0;
    info->is_32bit = riscv_is_32bit(harts);
}

/*
 * This can be used instead of riscv_boot_info_init() if the machine has
 * discontiguous physical memory. The low memory range specified will be
 * used to place firmware images.
 */
శూన్య riscv_boot_info_init_discontig_mem(RISCVBootInfo *info,
                                        RISCVHartArrayState *harts,
                                        hwaddr దిగువ_ప్రారంభం, hwaddr దిగువ_కొలత)
{
    riscv_boot_info_init(info, harts);
    info->ram_దిగువ_ప్రారంభం = దిగువ_ప్రారంభం;
    info->ram_దిగువ_కొలత = దిగువ_కొలత;
}

vaddr riscv_calc_kernel_ప్రారంభం_addr(స్థిరరాశి RISCVBootInfo *info,
                                   hwaddr firmware_చివర_addr) {
    ఒకవేళ (info->is_32bit) {
        తిరిగి QEMU_ALIGN_పైకి(firmware_చివర_addr, 4 * MiB);
    } లేకపోతే {
        తిరిగి QEMU_ALIGN_పైకి(firmware_చివర_addr, 2 * MiB);
    }
}

స్థిరరాశి అక్షరం *riscv_default_firmware_name(RISCVHartArrayState *harts)
{
    ఒకవేళ (riscv_is_32bit(harts)) {
        తిరిగి RISCV32_BIOS_BIN;
    }

    తిరిగి RISCV64_BIOS_BIN;
}

స్థిర అక్షరం *riscv_కనుగొను_bios(స్థిరరాశి అక్షరం *bios_filename)
{
    అక్షరం *filename;

    filename = qemu_కనుగొను_file(QEMU_FILE_TYPE_BIOS, bios_filename);
    ఒకవేళ (filename == ఖాళీ) {
        ఒకవేళ (!qtest_enabled()) {
            /*
             * We only ship OpenSBI binary bios images in the QEMU source.
             * For machines that use images other than the default bios,
             * running QEMU test will complain hence let's suppress the error
             * report for QEMU testing.
             */
            error_report("Unable to find the RISC-V BIOS \"%s\"",
                         bios_filename);
            నిష్క్రమించు(1);
        }
    }

    తిరిగి filename;
}

అక్షరం *riscv_కనుగొను_firmware(స్థిరరాశి అక్షరం *firmware_filename,
                          స్థిరరాశి అక్షరం *default_machine_firmware)
{
    అక్షరం *filename = ఖాళీ;

    ఒకవేళ ((!firmware_filename) || (!strcmp(firmware_filename, "default"))) {
        /*
         * The user didn't specify -bios, or has specified "-bios default".
         * That means we are going to load the OpenSBI binary included in
         * the QEMU source.
         */
        filename = riscv_కనుగొను_bios(default_machine_firmware);
    } లేకపోతే ఒకవేళ (strcmp(firmware_filename, "none")) {
        filename = riscv_కనుగొను_bios(firmware_filename);
    }

    తిరిగి filename;
}

hwaddr riscv_కనుగొను_and_ఎక్కించు_firmware(MachineState *machine,
                                    స్థిరరాశి RISCVBootInfo *info,
                                    స్థిరరాశి అక్షరం *default_machine_firmware,
                                    hwaddr *firmware_ఎక్కించు_addr,
                                    చిహ్నం_fn_t sym_cb)
{
    అక్షరం *firmware_filename;
    hwaddr firmware_చివర_addr = *firmware_ఎక్కించు_addr;

    firmware_filename = riscv_కనుగొను_firmware(machine->firmware,
                                            default_machine_firmware);

    ఒకవేళ (firmware_filename) {
        /* If not "none" load the firmware */
        firmware_చివర_addr = riscv_ఎక్కించు_firmware(machine, info,
                                                firmware_filename,
                                                firmware_ఎక్కించు_addr, sym_cb);
        g_free(firmware_filename);
    }

    తిరిగి firmware_చివర_addr;
}

hwaddr riscv_ఎక్కించు_firmware(MachineState *machine,
                           స్థిరరాశి RISCVBootInfo *info,
                           స్థిరరాశి అక్షరం *firmware_filename,
                           hwaddr *firmware_ఎక్కించు_addr,
                           చిహ్నం_fn_t sym_cb)
{
    uint64_t mem_కొలత = info->ram_దిగువ_కొలత ?: machine->ram_కొలత;
    uint64_t firmware_నమోదు, firmware_చివర;
    ssize_t firmware_కొలత;

    g_assert(firmware_filename != ఖాళీ);

    firmware_కొలత = ఎక్కించు_elf_ram_sym(firmware_filename, ఖాళీ, ఖాళీ, ఖాళీ,
                                     &firmware_నమోదు, ఖాళీ, &firmware_చివర,
                                     ఖాళీ, 0, EM_RISCV, 1, 0, ఖాళీ, false,
                                     sym_cb);
    ఒకవేళ (firmware_కొలత > 0) {
        *firmware_ఎక్కించు_addr = firmware_నమోదు;
        తిరిగి firmware_చివర;
    }

    ఒకవేళ (firmware_కొలత != ELF_ఎక్కించు_NOT_ELF) {
        /*
         * If the user specified an ELF format firmware that could not be
         * loaded as an ELF, it's possible that loading it as a binary is
         * not what was intended.
         */
        warn_report("could not load ELF format firmware '%s' (%s). "
                    "Attempting to load as binary.",
                    firmware_filename,
                    ఎక్కించు_elf_strerror(firmware_కొలత));
    }

    firmware_కొలత = ఎక్కించు_image_targphys_as(firmware_filename,
                                           *firmware_ఎక్కించు_addr,
                                           mem_కొలత, ఖాళీ,
                                           ఖాళీ);

    ఒకవేళ (firmware_కొలత > 0) {
        తిరిగి *firmware_ఎక్కించు_addr + firmware_కొలత;
    }

    error_report("could not load firmware '%s': %s", firmware_filename,
                 ఎక్కించు_elf_strerror(firmware_కొలత));
    నిష్క్రమించు(1);
}

స్థిర శూన్య riscv_ఎక్కించు_initrd(MachineState *machine, RISCVBootInfo *info)
{
    స్థిరరాశి అక్షరం *filename = machine->initrd_filename;
    uint64_t mem_కొలత = info->ram_దిగువ_కొలత ?: machine->ram_కొలత;
    శూన్య *fdt = machine->fdt;
    hwaddr ప్రారంభం, చివర;
    ssize_t కొలత;

    g_assert(filename != ఖాళీ);

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
    ప్రారంభం = info->image_దిగువ_addr + MIN(mem_కొలత / 2, 512 * MiB);

    కొలత = ఎక్కించు_ramdisk(filename, ప్రారంభం, mem_కొలత - ప్రారంభం);
    ఒకవేళ (కొలత == -1) {
        కొలత = ఎక్కించు_image_targphys(filename, ప్రారంభం, mem_కొలత - ప్రారంభం, ఖాళీ);
        ఒకవేళ (కొలత == -1) {
            error_report("could not load ramdisk '%s'", filename);
            నిష్క్రమించు(1);
        }
    }

    info->initrd_ప్రారంభం = ప్రారంభం;
    info->initrd_కొలత = కొలత;

    /* Some RISC-V machines (e.g. opentitan) don't have a fdt. */
    ఒకవేళ (fdt) {
        చివర = ప్రారంభం + కొలత;
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-start", ప్రారంభం);
        qemu_fdt_setprop_u64(fdt, "/chosen", "linux,initrd-end", చివర);
    }
}

శూన్య riscv_ఎక్కించు_kernel(MachineState *machine,
                       RISCVBootInfo *info,
                       vaddr kernel_ప్రారంభం_addr,
                       bool ఎక్కించు_initrd,
                       చిహ్నం_fn_t sym_cb)
{
    uint64_t mem_కొలత = info->ram_దిగువ_కొలత ?: machine->ram_కొలత;
    స్థిరరాశి అక్షరం *kernel_filename = machine->kernel_filename;
    ssize_t kernel_కొలత;
    శూన్య *fdt = machine->fdt;

    g_assert(kernel_filename != ఖాళీ);

    /*
     * NB: Use low address not ELF entry point to ensure that the fw_dynamic
     * behaviour when loading an ELF matches the fw_payload, fw_jump and BBL
     * behaviour, as well as fw_dynamic with a raw binary, all of which jump to
     * the (expected) load address load address. This allows kernels to have
     * separate SBI and ELF entry points (used by FreeBSD, for example).
     */
    kernel_కొలత = ఎక్కించు_elf_ram_sym(kernel_filename, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ,
                                   &info->image_దిగువ_addr, &info->image_ఎగువ_addr,
                                   ఖాళీ, ELFDATA2LSB, EM_RISCV,
                                   1, 0, ఖాళీ, true, sym_cb);
    ఒకవేళ (kernel_కొలత > 0) {
        info->kernel_కొలత = kernel_కొలత;
        వెళ్ళు బహిర్గతం;
    }

    kernel_కొలత = ఎక్కించు_uimage_as(kernel_filename, &info->image_దిగువ_addr,
                                 ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);
    ఒకవేళ (kernel_కొలత > 0) {
        info->kernel_కొలత = kernel_కొలత;
        info->image_ఎగువ_addr = info->image_దిగువ_addr + kernel_కొలత;
        వెళ్ళు బహిర్గతం;
    }

    kernel_కొలత = ఎక్కించు_image_targphys_as(kernel_filename, kernel_ప్రారంభం_addr,
                                         mem_కొలత, ఖాళీ, ఖాళీ);
    ఒకవేళ (kernel_కొలత > 0) {
        info->kernel_కొలత = kernel_కొలత;
        info->image_దిగువ_addr = kernel_ప్రారంభం_addr;
        info->image_ఎగువ_addr = info->image_దిగువ_addr + kernel_కొలత;
        వెళ్ళు బహిర్గతం;
    }

    error_report("could not load kernel '%s'", kernel_filename);
    నిష్క్రమించు(1);

బహిర్గతం:
    /*
     * For 32 bit CPUs 'image_low_addr' can be sign-extended by
     * load_elf_ram_sym().
     */
    ఒకవేళ (info->is_32bit) {
        info->image_దిగువ_addr = extract64(info->image_దిగువ_addr, 0, 32);
    }

    ఒకవేళ (ఎక్కించు_initrd && machine->initrd_filename) {
        riscv_ఎక్కించు_initrd(machine, info);
    }

    ఒకవేళ (fdt && machine->kernel_cmdline && *machine->kernel_cmdline) {
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
uint64_t riscv_compute_fdt_addr(hwaddr dram_పునాది, hwaddr dram_కొలత,
                                MachineState *ms, స్థిరరాశి RISCVBootInfo *info)
{
    పూర్ణాంకం ret = fdt_pack(ms->fdt);
    hwaddr dram_చివర, తాత్కాలిక;
    పూర్ణాంకం fdtsize;
    uint64_t dtb_ప్రారంభం, dtb_ప్రారంభం_హద్దు;

    /* Should only fail if we've built a corrupted tree */
    g_assert(ret == 0);

    fdtsize = fdt_totalsize(ms->fdt);
    ఒకవేళ (fdtsize <= 0) {
        error_report("invalid device-tree");
        నిష్క్రమించు(1);
    }

    ఒకవేళ (info->initrd_కొలత) {
        /* If initrd is successfully loaded, place DTB after it. */
        dtb_ప్రారంభం_హద్దు = info->initrd_ప్రారంభం + info->initrd_కొలత;
    } లేకపోతే ఒకవేళ (info->kernel_కొలత) {
        /* If only kernel is successfully loaded, place DTB after it. */
        dtb_ప్రారంభం_హద్దు = info->image_ఎగువ_addr;
    } లేకపోతే {
        /* Otherwise, do not check DTB overlapping */
        dtb_ప్రారంభం_హద్దు = 0;
    }

    /*
     * A dram_size == 0, usually from a MemMapEntry[].size element,
     * means that the DRAM block goes all the way to ms->ram_size.
     */
    dram_చివర = dram_పునాది;
    dram_చివర += dram_కొలత ? MIN(ms->ram_కొలత, dram_కొలత) : ms->ram_కొలత;

    /*
     * We should put fdt as far as possible to avoid kernel/initrd overwriting
     * its content. But it should be addressable by 32 bit system as well in RV32.
     * Thus, put it near to the end of dram in RV64, and put it near to the end
     * of dram or 3GB whichever is lesser in RV32.
     */
    ఒకవేళ (!info->is_32bit) {
        తాత్కాలిక = dram_చివర;
    } లేకపోతే {
        తాత్కాలిక = (dram_పునాది < 3072 * MiB) ? MIN(dram_చివర, 3072 * MiB) : dram_చివర;
    }

    dtb_ప్రారంభం = QEMU_ALIGN_కిందికి(తాత్కాలిక - fdtsize, 2 * MiB);

    ఒకవేళ (dtb_ప్రారంభం_హద్దు && (dtb_ప్రారంభం < dtb_ప్రారంభం_హద్దు)) {
        error_report("Not enough memory to place DTB after kernel/initrd");
        నిష్క్రమించు(1);
    }

    తిరిగి dtb_ప్రారంభం;
}

/*
 * 'fdt_addr' is received as hwaddr because boards might put
 * the FDT beyond 32-bit addressing boundary.
 */
శూన్య riscv_ఎక్కించు_fdt(hwaddr fdt_addr, శూన్య *fdt)
{
    uint32_t fdtsize = fdt_totalsize(fdt);

    /* copy in the device tree */
    rom_కూడు_blob_fixed_as("fdt", fdt, fdtsize, fdt_addr,
                          &చిరునామా_space_జ్ఞాపకం);
    qemu_రిజిస్టరు_reset_nosnapshotload(qemu_fdt_randomize_seeds,
                        rom_ptr_for_as(&చిరునామా_space_జ్ఞాపకం, fdt_addr, fdtsize));
}

శూన్య riscv_rom_copy_firmware_info(MachineState *machine,
                                  RISCVHartArrayState *harts,
                                  hwaddr rom_పునాది, hwaddr rom_కొలత,
                                  uint32_t reset_vec_కొలత,
                                  uint64_t kernel_నమోదు)
{
    నిర్మాణం fw_dynamic_info32 dinfo32;
    నిర్మాణం fw_dynamic_info64 dinfo64;
    శూన్య *dinfo_ptr = ఖాళీ;
    కొలత_t dinfo_len;
    స్థిరరాశి bool rv32 = riscv_is_32bit(harts);
    స్థిరరాశి bool be = harts->harts[0].cfg.big_endian;

    ఒకవేళ (rv32) {
        dinfo32.magic = be ? cpu_గమ్యం_be32(FW_DYNAMIC_INFO_MAGIC_విలువ)
                           : cpu_గమ్యం_le32(FW_DYNAMIC_INFO_MAGIC_విలువ);
        dinfo32.సంస్కరణ = be ? cpu_గమ్యం_be32(FW_DYNAMIC_INFO_సంస్కరణ)
                             : cpu_గమ్యం_le32(FW_DYNAMIC_INFO_సంస్కరణ);
        dinfo32.తదుపరి_బహుళకం = be ? cpu_గమ్యం_be32(FW_DYNAMIC_INFO_తదుపరి_బహుళకం_S)
                               : cpu_గమ్యం_le32(FW_DYNAMIC_INFO_తదుపరి_బహుళకం_S);
        dinfo32.తదుపరి_addr = be ? cpu_గమ్యం_be32(kernel_నమోదు)
                               : cpu_గమ్యం_le32(kernel_నమోదు);
        dinfo32.options = 0;
        dinfo32.boot_hart = 0;
        dinfo_ptr = &dinfo32;
        dinfo_len = పరిమాణం(dinfo32);
    } లేకపోతే {
        dinfo64.magic = be ? cpu_గమ్యం_be64(FW_DYNAMIC_INFO_MAGIC_విలువ)
                           : cpu_గమ్యం_le64(FW_DYNAMIC_INFO_MAGIC_విలువ);
        dinfo64.సంస్కరణ = be ? cpu_గమ్యం_be64(FW_DYNAMIC_INFO_సంస్కరణ)
                             : cpu_గమ్యం_le64(FW_DYNAMIC_INFO_సంస్కరణ);
        dinfo64.తదుపరి_బహుళకం = be ? cpu_గమ్యం_be64(FW_DYNAMIC_INFO_తదుపరి_బహుళకం_S)
                               : cpu_గమ్యం_le64(FW_DYNAMIC_INFO_తదుపరి_బహుళకం_S);
        dinfo64.తదుపరి_addr = be ? cpu_గమ్యం_be64(kernel_నమోదు)
                               : cpu_గమ్యం_le64(kernel_నమోదు);
        dinfo64.options = 0;
        dinfo64.boot_hart = 0;
        dinfo_ptr = &dinfo64;
        dinfo_len = పరిమాణం(dinfo64);
    }

    /**
     * copy the dynamic firmware info. This information is specific to
     * OpenSBI but doesn't break any other firmware as long as they don't
     * expect any certain value in "a2" register.
     */
    ఒకవేళ (dinfo_len > (rom_కొలత - reset_vec_కొలత)) {
        error_report("not enough space to store dynamic firmware info");
        నిష్క్రమించు(1);
    }

    rom_కూడు_blob_fixed_as("mrom.finfo",
                           dinfo_ptr,
                           dinfo_len,
                           rom_పునాది + reset_vec_కొలత,
                           &చిరునామా_space_జ్ఞాపకం);
}

#నిర్వచించు సంకేతం_పదాలు 6
#నిర్వచించు దత్తాంశం_పదాలు 4

శూన్య riscv_అమరిక_rom_reset_vec(MachineState *machine, RISCVHartArrayState *harts,
                               hwaddr ప్రారంభం_addr,
                               hwaddr rom_పునాది, hwaddr rom_కొలత,
                               uint64_t kernel_నమోదు,
                               uint64_t fdt_ఎక్కించు_addr)
{
    స్థిరరాశి bool rv32 = riscv_is_32bit(harts);
    స్థిరరాశి bool big_endian = harts->harts[0].cfg.big_endian;
    uint32_t reset_vec[సంకేతం_పదాలు + దత్తాంశం_పదాలు];

    /* .text (RISC-V instructions are always little-endian) */
    reset_vec[0] = const_le32(0x00000297);      /* 1:  auipc  t0, %pcrel_hi(fw_dyn) */
    reset_vec[1] = const_le32(0x02828613);      /*     addi   a2, t0, %pcrel_lo(1b) */
    reset_vec[2] = const_le32(0xf1402573);      /*     csrr   a0, mhartid  */
    ఒకవేళ (harts->harts[0].cfg.ext_zicsr) {
        reset_vec[2] = const_le32(0xf1402573);  /*     csrr   a0, mhartid  */
    } లేకపోతే {
        /*
         * The Zicsr extension has been disabled, so let's ensure we don't
         * run the CSR instruction. Let's fill the address with a non
         * compressed nop.
         */
        reset_vec[2] = const_le32(0x00000013);  /*     addi   x0, x0, 0 */
    }
    ఒకవేళ (rv32) {
        reset_vec[3] = const_le32(0x0202a583);  /*     lw     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182a283);  /*     lw     t0, 24(t0) */
    } లేకపోతే {
        reset_vec[3] = const_le32(0x0202b583);  /*     ld     a1, 32(t0) */
        reset_vec[4] = const_le32(0x0182b283);  /*     ld     t0, 24(t0) */
    }
    reset_vec[5] = const_le32(0x00028067);      /*     jr     t0 */

    /* .data (must match the firmware's data endianness) */
    ఒకవేళ (big_endian) {
        stq_be_p(&reset_vec[6], ప్రారంభం_addr);    /* start:       .dword */
        stq_be_p(&reset_vec[8], fdt_ఎక్కించు_addr); /* fdt_laddr:   .dword */
    } లేకపోతే {
        stq_le_p(&reset_vec[6], ప్రారంభం_addr);
        stq_le_p(&reset_vec[8], fdt_ఎక్కించు_addr);
    }

    rom_కూడు_blob_fixed_as("mrom.reset", reset_vec, పరిమాణం(reset_vec),
                          rom_పునాది, &చిరునామా_space_జ్ఞాపకం);
    riscv_rom_copy_firmware_info(machine, harts,
                                 rom_పునాది, rom_కొలత,
                                 పరిమాణం(reset_vec),
                                 kernel_నమోదు);
}

శూన్య riscv_అమరిక_ప్రత్యక్ష_kernel(hwaddr kernel_addr, hwaddr fdt_addr)
{
    CPUState *cs;

    CPU_FOREACH(cs) {
        CPURISCVState *env = cpu_env(cs);

        env->kernel_addr = kernel_addr;
        env->fdt_addr = fdt_addr;
    }
}

శూన్య riscv_అమరిక_firmware_boot(MachineState *machine)
{
    ఒకవేళ (machine->kernel_filename) {
        FWCfgState *fw_cfg;
        fw_cfg = fw_cfg_కనుగొను();

        ప్రతిజ్ఞా(fw_cfg);
        /*
         * Expose the kernel, the command line, and the initrd in fw_cfg.
         * We don't process them here at all, it's all left to the
         * firmware.
         */
        ఎక్కించు_image_గమ్యం_fw_cfg(fw_cfg,
                             FW_CFG_KERNEL_కొలత, FW_CFG_KERNEL_దత్తాంశం,
                             machine->kernel_filename,
                             true);
        ఎక్కించు_image_గమ్యం_fw_cfg(fw_cfg,
                             FW_CFG_INITRD_కొలత, FW_CFG_INITRD_దత్తాంశం,
                             machine->initrd_filename, false);

        ఒకవేళ (machine->kernel_cmdline) {
            fw_cfg_కూడు_i32(fw_cfg, FW_CFG_CMDLINE_కొలత,
                           అక్షరశ్రేణి_పొడవు(machine->kernel_cmdline) + 1);
            fw_cfg_కూడు_string(fw_cfg, FW_CFG_CMDLINE_దత్తాంశం,
                              machine->kernel_cmdline);
        }
    }
}
