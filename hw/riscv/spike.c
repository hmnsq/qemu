/*
 * QEMU RISC-V Spike Board
 *
 * Copyright (c) 2016-2017 Sagar Karandikar, sagark@eecs.berkeley.edu
 * Copyright (c) 2017-2018 SiFive, Inc.
 *
 * This provides a RISC-V Board with the following devices:
 *
 * 0) HTIF Console and Poweroff
 * 1) CLINT (Timer and IPI)
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
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/spike.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/fdt-common.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/numa.h"
#ಅನ್ತರ್ಭಾವ "hw/char/riscv_htif.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "chardev/char.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"

#ಅನ್ತರ್ಭಾವ <libfdt.h>

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು spike_memmap[] = {
    [SPIKE_MROM] =     {     0x1000,     0xf000 },
    [SPIKE_HTIF] =     {  0x1000000,     0x1000 },
    [SPIKE_CLINT] =    {  0x2000000,    0x10000 },
    [SPIKE_DRAM] =     { 0x80000000,        0x0 },
};

ಸ್ಥಿರ ಶೂನ್ಯ create_fdt(SpikeState *s, ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap,
                       bool is_32_ಬಿಟ್, bool htif_custom_ತಳ)
{
    ಶೂನ್ಯ *fdt;
    ಪೂರ್ಣಾಂಕ fdt_ಗಾತ್ರ;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ clint_addr;
    ಪೂರ್ಣಾಂಕ socket;
    MachineState *ms = MACHINE(s);
    uint32_t phandle = 1;
    bool numa_enabled = riscv_numa_enabled(ms);

    fdt = ms->fdt = riscv_create_board_device_ವೃಕ್ಷ(
        "ucbbar,spike-bare,qemu",
        "ucbbar,spike-bare-dev",
        &fdt_ಗಾತ್ರ);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, "/htif");
    qemu_fdt_setprop_string(fdt, "/htif", "compatible", "ucb,htif0");
    ಆದರೆ (htif_custom_ತಳ) {
        qemu_fdt_setprop_cells(fdt, "/htif", "reg",
            0x0, memmap[SPIKE_HTIF].ತಳ, 0x0, memmap[SPIKE_HTIF].ಗಾತ್ರ_2);
    }

    riscv_fdt_create_cpu_socket_subnode(fdt,
                                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    ಗಾಗಿ (socket = (riscv_socket_ಎಣಿಕೆ(ms) - 1); socket >= 0; socket--) {
        g_autofree uint32_t *intc_phandles = g_new0(uint32_t,
                                                    s->soc[socket].ಅಂಕೆ_harts);
        hwaddr memaddr = memmap[SPIKE_DRAM].ತಳ +
                         riscv_socket_mem_ಸ್ಥಾನಾಂತರ(ms, socket);
        uint64_t memsize =  riscv_socket_mem_ಗಾತ್ರ(ms, socket);

        riscv_create_fdt_socket_cpus(fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].ಅಂಕೆ_harts,
                                     s->soc[socket].hartid_ತಳ,
                                     &phandle, intc_phandles, numa_enabled,
                                     is_32_ಬಿಟ್);

        riscv_create_fdt_socket_ಸ್ಮರಣೆ(fdt, memaddr, memsize, socket,
                                       riscv_numa_enabled(ms));

        clint_addr = memmap[SPIKE_CLINT].ತಳ +
            (memmap[SPIKE_CLINT].ಗಾತ್ರ_2 * socket);
        riscv_create_fdt_socket_clint(fdt, clint_addr,
                                      memmap[SPIKE_CLINT].ಗಾತ್ರ_2, socket,
                                      intc_phandles, s->soc[socket].ಅಂಕೆ_harts,
                                      numa_enabled);
    }

    riscv_socket_fdt_write_ದೂರ_ಮಾತೃಕೆ(ms);

    qemu_fdt_ಕೂಡಿಸು_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "/htif");
}

ಸ್ಥಿರ bool spike_test_elf_image(ಅಕ್ಷರ *filename)
{
    ಹಿಂತಿರುಗಿ ತುಂಬು_elf_hdr(filename, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
}

ಸ್ಥಿರ ಶೂನ್ಯ spike_board_init(MachineState *machine)
{
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = spike_memmap;
    SpikeState *s = SPIKE_MACHINE(machine);
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    ಸ್ಮರಣೆ_Region *ಮುಖವಾಡ_rom = g_new(ಸ್ಮರಣೆ_Region, 1);
    hwaddr firmware_ಕೊನೆ_addr = memmap[SPIKE_DRAM].ತಳ;
    hwaddr firmware_ತುಂಬು_addr = memmap[SPIKE_DRAM].ತಳ;
    vaddr kernel_ಆರಂಭ_addr;
    ಅಕ್ಷರ *firmware_name;
    uint64_t fdt_ತುಂಬು_addr;
    uint64_t kernel_ನಮೂದು;
    ಅಕ್ಷರ *soc_name;
    ಪೂರ್ಣಾಂಕ i, ತಳ_hartid, hart_ಎಣಿಕೆ;
    bool htif_custom_ತಳ = false;
    RISCVBootInfo boot_info;

    /* Check socket count limit */
    ಆದರೆ (SPIKE_SOCKETS_MAX < riscv_socket_ಎಣಿಕೆ(machine)) {
        error_report("number of sockets/nodes should be less than %d",
            SPIKE_SOCKETS_MAX);
        ನಿರ್ಗಮಿಸು(1);
    }

    /* Initialize sockets */
    ಗಾಗಿ (i = 0; i < riscv_socket_ಎಣಿಕೆ(machine); i++) {
        ಆದರೆ (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            ನಿರ್ಗಮಿಸು(1);
        }

        ತಳ_hartid = riscv_socket_ಮೊದಲ_hartid(machine, i);
        ಆದರೆ (ತಳ_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            ನಿರ್ಗಮಿಸು(1);
        }

        hart_ಎಣಿಕೆ = riscv_socket_hart_ಎಣಿಕೆ(machine, i);
        ಆದರೆ (hart_ಎಣಿಕೆ < 0) {
            error_report("can't find hart count for socket%d", i);
            ನಿರ್ಗಮಿಸು(1);
        }

        soc_name = g_strdup_printf("soc%d", i);
        object_initialize_ಮಗು(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        g_free(soc_name);
        object_property_ಗಣ_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_ಗಣ_int(OBJECT(&s->soc[i]), "hartid-base",
                                ತಳ_hartid, &error_abort);
        object_property_ಗಣ_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_ಎಣಿಕೆ, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Core Local Interruptor (timer and IPI) for each socket */
        riscv_aclint_swi_create(system_ಸ್ಮರಣೆ,
            memmap[SPIKE_CLINT].ತಳ + i * memmap[SPIKE_CLINT].ಗಾತ್ರ_2,
            ತಳ_hartid, hart_ಎಣಿಕೆ, false);
        riscv_aclint_mtimer_create(system_ಸ್ಮರಣೆ,
            memmap[SPIKE_CLINT].ತಳ + i * memmap[SPIKE_CLINT].ಗಾತ್ರ_2 +
                RISCV_ACLINT_SWI_ಗಾತ್ರ,
            RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ, ತಳ_hartid, hart_ಎಣಿಕೆ,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }

    /* register system main memory (actual RAM) */
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ, memmap[SPIKE_DRAM].ತಳ,
        machine->ram);

    /* boot rom */
    ಸ್ಮರಣೆ_region_init_rom(ಮುಖವಾಡ_rom, ಖಾಲಿ, "riscv.spike.mrom",
                           memmap[SPIKE_MROM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ, memmap[SPIKE_MROM].ತಳ,
                                ಮುಖವಾಡ_rom);

    /* Find firmware */
    firmware_name = riscv_ಕಂಡುಹಿಡಿ_firmware(machine->firmware,
                        riscv_default_firmware_name(&s->soc[0]));

    /*
     * Test the given firmware or kernel file to see if it is an ELF image.
     * If it is an ELF, we assume it contains the symbols required for
     * the HTIF console, otherwise we fall back to use the custom base
     * passed from device tree for the HTIF console.
     */
    ಆದರೆ (!firmware_name && !machine->kernel_filename) {
        htif_custom_ತಳ = true;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (firmware_name) {
            htif_custom_ತಳ = !spike_test_elf_image(firmware_name);
        }
        ಆದರೆ (!htif_custom_ತಳ && machine->kernel_filename) {
            htif_custom_ತಳ = !spike_test_elf_image(machine->kernel_filename);
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    /* Load firmware */
    ಆದರೆ (firmware_name) {
        firmware_ಕೊನೆ_addr = riscv_ತುಂಬು_firmware(machine, &boot_info,
                                                firmware_name,
                                                &firmware_ತುಂಬು_addr,
                                                htif_ಚಿಹ್ನೆ_callback);
        g_free(firmware_name);
    }

    /* Create device tree */
    create_fdt(s, memmap, riscv_is_32bit(&s->soc[0]), htif_custom_ತಳ);

    /* Load kernel */
    ಆದರೆ (machine->kernel_filename) {
        kernel_ಆರಂಭ_addr = riscv_calc_kernel_ಆರಂಭ_addr(&boot_info,
                                                         firmware_ಕೊನೆ_addr);

        riscv_ತುಂಬು_kernel(machine, &boot_info, kernel_ಆರಂಭ_addr,
                          true, htif_ಚಿಹ್ನೆ_callback);
        kernel_ನಮೂದು = boot_info.image_ಕೆಳಗಿನ_addr;
    } ಇಲ್ಲದಿದ್ದರೆ {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_ನಮೂದು = 0;
    }

    fdt_ತುಂಬು_addr = riscv_compute_fdt_addr(memmap[SPIKE_DRAM].ತಳ,
                                           memmap[SPIKE_DRAM].ಗಾತ್ರ_2,
                                           machine, &boot_info);
    riscv_ತುಂಬು_fdt(fdt_ತುಂಬು_addr, machine->fdt);

    /* load the reset vector */
    riscv_ಸಿದ್ಧತೆ_rom_reset_vec(machine, &s->soc[0], firmware_ತುಂಬು_addr,
                              memmap[SPIKE_MROM].ತಳ,
                              memmap[SPIKE_MROM].ಗಾತ್ರ_2, kernel_ನಮೂದು,
                              fdt_ತುಂಬು_addr);

    /* initialize HTIF using symbols found in load_kernel */
    htif_mm_init(system_ಸ್ಮರಣೆ, serial_hd(0), memmap[SPIKE_HTIF].ತಳ,
                 htif_custom_ತಳ);
}

ಸ್ಥಿರ ಶೂನ್ಯ spike_ಗಣ_signature(Object *obj, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, Error **errp)
{
    sig_file = g_strdup(val);
}

ಸ್ಥಿರ ಶೂನ್ಯ spike_machine_instance_init(Object *obj)
{
}

ಸ್ಥಿರ ಶೂನ್ಯ spike_machine_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Spike board";
    mc->init = spike_board_init;
    mc->max_cpus = SPIKE_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_MAX;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_instance_props = riscv_numa_cpu_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_props;
    mc->get_default_cpu_ಗಂಟು_id = riscv_numa_get_default_cpu_ಗಂಟು_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv.spike.ram";
    object_class_property_ಕೂಡಿಸು_str(oc, "signature", ಖಾಲಿ, spike_ಗಣ_signature);
    object_class_property_ಗಣ_description(oc, "signature",
                                          "File to write ACT test signature");
    object_class_static_property_ಕೂಡಿಸು_uint8_ptr(oc, "signature-granularity",
                                               &ಸಾಲು_ಗಾತ್ರ,
                                               OBJ_PROP_FLAG_WRITE);
    object_class_property_ಗಣ_description(oc, "signature-granularity",
                                          "Size of each line in ACT signature "
                                          "file");
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo spike_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("spike"),
    .ಪೋಷಕ     = TYPE_MACHINE,
    .class_init = spike_machine_class_init,
    .instance_init = spike_machine_instance_init,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(SpikeState),
};

ಸ್ಥಿರ ಶೂನ್ಯ spike_machine_init_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&spike_machine_typeinfo);
}

type_init(spike_machine_init_ರಿಜಿಸ್ಟರ್_types)
