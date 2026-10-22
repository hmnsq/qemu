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

#ଅନ୍ତର୍ଭାଵ "qemu/osdep.h"
#ଅନ୍ତର୍ଭାଵ "qemu/error-report.h"
#ଅନ୍ତର୍ଭାଵ "qapi/error.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/boards.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/loader.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/sysbus.h"
#ଅନ୍ତର୍ଭାଵ "target/riscv/cpu.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/riscv_hart.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/spike.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/boot.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/fdt-common.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/numa.h"
#ଅନ୍ତର୍ଭାଵ "hw/char/riscv_htif.h"
#ଅନ୍ତର୍ଭାଵ "hw/intc/riscv_aclint.h"
#ଅନ୍ତର୍ଭାଵ "chardev/char.h"
#ଅନ୍ତର୍ଭାଵ "system/device_tree.h"
#ଅନ୍ତର୍ଭାଵ "system/system.h"

#ଅନ୍ତର୍ଭାଵ <libfdt.h>

ସ୍ଥିର ଅଚଳ MemMapEntry spike_memmap[] = {
    [SPIKE_MROM] =     {     0x1000,     0xf000 },
    [SPIKE_HTIF] =     {  0x1000000,     0x1000 },
    [SPIKE_CLINT] =    {  0x2000000,    0x10000 },
    [SPIKE_DRAM] =     { 0x80000000,        0x0 },
};

ସ୍ଥିର ଶୂନ୍ୟ create_fdt(SpikeState *s, ଅଚଳ MemMapEntry *memmap,
                       bool is_32_bit, bool htif_custom_base)
{
    ଶୂନ୍ୟ *fdt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_size;
    unsigned long clint_addr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket;
    MachineState *ms = MACHINE(s);
    uint32_t phandle = 1;
    bool numa_enabled = riscv_numa_enabled(ms);

    fdt = ms->fdt = riscv_create_board_device_ବୃକ୍ଷ(
        "ucbbar,spike-bare,qemu",
        "ucbbar,spike-bare-dev",
        &fdt_size);

    qemu_fdt_add_subnode(fdt, "/htif");
    qemu_fdt_setprop_string(fdt, "/htif", "compatible", "ucb,htif0");
    ଯଦି (htif_custom_base) {
        qemu_fdt_setprop_cells(fdt, "/htif", "reg",
            0x0, memmap[SPIKE_HTIF].base, 0x0, memmap[SPIKE_HTIF].size);
    }

    riscv_fdt_create_cpu_socket_subnode(fdt,
                                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    ପାଇଁ (socket = (riscv_socket_ଗଣନା(ms) - 1); socket >= 0; socket--) {
        g_autofree uint32_t *intc_phandles = g_new0(uint32_t,
                                                    s->soc[socket].ଅଙ୍କ_harts);
        hwaddr memaddr = memmap[SPIKE_DRAM].base +
                         riscv_socket_mem_offset(ms, socket);
        uint64_t memsize =  riscv_socket_mem_size(ms, socket);

        riscv_create_fdt_socket_cpus(fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].ଅଙ୍କ_harts,
                                     s->soc[socket].hartid_base,
                                     &phandle, intc_phandles, numa_enabled,
                                     is_32_bit);

        riscv_create_fdt_socket_memory(fdt, memaddr, memsize, socket,
                                       riscv_numa_enabled(ms));

        clint_addr = memmap[SPIKE_CLINT].base +
            (memmap[SPIKE_CLINT].size * socket);
        riscv_create_fdt_socket_clint(fdt, clint_addr,
                                      memmap[SPIKE_CLINT].size, socket,
                                      intc_phandles, s->soc[socket].ଅଙ୍କ_harts,
                                      numa_enabled);
    }

    riscv_socket_fdt_write_distance_matrix(ms);

    qemu_fdt_add_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "/htif");
}

ସ୍ଥିର bool spike_test_elf_image(ଅକ୍ଷର *filename)
{
    ଫେରନ୍ତୁ load_elf_hdr(filename, ଖାଲି, ଖାଲି, ଖାଲି);
}

ସ୍ଥିର ଶୂନ୍ୟ spike_board_init(MachineState *machine)
{
    ଅଚଳ MemMapEntry *memmap = spike_memmap;
    SpikeState *s = SPIKE_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *mask_rom = g_new(MemoryRegion, 1);
    hwaddr firmware_end_addr = memmap[SPIKE_DRAM].base;
    hwaddr firmware_load_addr = memmap[SPIKE_DRAM].base;
    vaddr kernel_start_addr;
    ଅକ୍ଷର *firmware_name;
    uint64_t fdt_load_addr;
    uint64_t kernel_entry;
    ଅକ୍ଷର *soc_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, base_hartid, hart_ଗଣନା;
    bool htif_custom_base = false;
    RISCVBootInfo boot_info;

    /* Check socket count limit */
    ଯଦି (SPIKE_SOCKETS_MAX < riscv_socket_ଗଣନା(machine)) {
        error_report("number of sockets/nodes should be less than %d",
            SPIKE_SOCKETS_MAX);
        exit(1);
    }

    /* Initialize sockets */
    ପାଇଁ (i = 0; i < riscv_socket_ଗଣନା(machine); i++) {
        ଯଦି (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            exit(1);
        }

        base_hartid = riscv_socket_first_hartid(machine, i);
        ଯଦି (base_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            exit(1);
        }

        hart_ଗଣନା = riscv_socket_hart_ଗଣନା(machine, i);
        ଯଦି (hart_ଗଣନା < 0) {
            error_report("can't find hart count for socket%d", i);
            exit(1);
        }

        soc_name = g_strdup_printf("soc%d", i);
        object_initialize_child(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        g_free(soc_name);
        object_property_set_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_set_int(OBJECT(&s->soc[i]), "hartid-base",
                                base_hartid, &error_abort);
        object_property_set_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_ଗଣନା, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Core Local Interruptor (timer and IPI) for each socket */
        riscv_aclint_swi_create(system_memory,
            memmap[SPIKE_CLINT].base + i * memmap[SPIKE_CLINT].size,
            base_hartid, hart_ଗଣନା, false);
        riscv_aclint_mtimer_create(system_memory,
            memmap[SPIKE_CLINT].base + i * memmap[SPIKE_CLINT].size +
                RISCV_ACLINT_SWI_SIZE,
            RISCV_ACLINT_DEFAULT_MTIMER_SIZE, base_hartid, hart_ଗଣନା,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }

    /* register system main memory (actual RAM) */
    memory_region_add_subregion(system_memory, memmap[SPIKE_DRAM].base,
        machine->ram);

    /* boot rom */
    memory_region_init_rom(mask_rom, ଖାଲି, "riscv.spike.mrom",
                           memmap[SPIKE_MROM].size, &error_fatal);
    memory_region_add_subregion(system_memory, memmap[SPIKE_MROM].base,
                                mask_rom);

    /* Find firmware */
    firmware_name = riscv_find_firmware(machine->firmware,
                        riscv_default_firmware_name(&s->soc[0]));

    /*
     * Test the given firmware or kernel file to see if it is an ELF image.
     * If it is an ELF, we assume it contains the symbols required for
     * the HTIF console, otherwise we fall back to use the custom base
     * passed from device tree for the HTIF console.
     */
    ଯଦି (!firmware_name && !machine->kernel_filename) {
        htif_custom_base = true;
    } ଅନ୍ୟଥା {
        ଯଦି (firmware_name) {
            htif_custom_base = !spike_test_elf_image(firmware_name);
        }
        ଯଦି (!htif_custom_base && machine->kernel_filename) {
            htif_custom_base = !spike_test_elf_image(machine->kernel_filename);
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    /* Load firmware */
    ଯଦି (firmware_name) {
        firmware_end_addr = riscv_load_firmware(machine, &boot_info,
                                                firmware_name,
                                                &firmware_load_addr,
                                                htif_ପ୍ରତୀକ_callback);
        g_free(firmware_name);
    }

    /* Create device tree */
    create_fdt(s, memmap, riscv_is_32bit(&s->soc[0]), htif_custom_base);

    /* Load kernel */
    ଯଦି (machine->kernel_filename) {
        kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                         firmware_end_addr);

        riscv_load_kernel(machine, &boot_info, kernel_start_addr,
                          true, htif_ପ୍ରତୀକ_callback);
        kernel_entry = boot_info.image_ତଳ_addr;
    } ଅନ୍ୟଥା {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_entry = 0;
    }

    fdt_load_addr = riscv_compute_fdt_addr(memmap[SPIKE_DRAM].base,
                                           memmap[SPIKE_DRAM].size,
                                           machine, &boot_info);
    riscv_load_fdt(fdt_load_addr, machine->fdt);

    /* load the reset vector */
    riscv_setup_rom_reset_vec(machine, &s->soc[0], firmware_load_addr,
                              memmap[SPIKE_MROM].base,
                              memmap[SPIKE_MROM].size, kernel_entry,
                              fdt_load_addr);

    /* initialize HTIF using symbols found in load_kernel */
    htif_mm_init(system_memory, serial_hd(0), memmap[SPIKE_HTIF].base,
                 htif_custom_base);
}

ସ୍ଥିର ଶୂନ୍ୟ spike_set_signature(Object *obj, ଅଚଳ ଅକ୍ଷର *val, Error **errp)
{
    sig_file = g_strdup(val);
}

ସ୍ଥିର ଶୂନ୍ୟ spike_machine_instance_init(Object *obj)
{
}

ସ୍ଥିର ଶୂନ୍ୟ spike_machine_class_init(ObjectClass *oc, ଅଚଳ ଶୂନ୍ୟ *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Spike board";
    mc->init = spike_board_init;
    mc->max_cpus = SPIKE_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_MAX;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_index_to_instance_props = riscv_numa_cpu_index_to_props;
    mc->get_default_cpu_ଗଣ୍ଠି_id = riscv_numa_get_default_cpu_ଗଣ୍ଠି_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv.spike.ram";
    object_class_property_add_str(oc, "signature", ଖାଲି, spike_set_signature);
    object_class_property_set_description(oc, "signature",
                                          "File to write ACT test signature");
    object_class_static_property_add_uint8_ptr(oc, "signature-granularity",
                                               &ଧାଡ଼ି_size,
                                               OBJ_PROP_FLAG_WRITE);
    object_class_property_set_description(oc, "signature-granularity",
                                          "Size of each line in ACT signature "
                                          "file");
}

ସ୍ଥିର ଅଚଳ TypeInfo spike_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("spike"),
    .parent     = TYPE_MACHINE,
    .class_init = spike_machine_class_init,
    .instance_init = spike_machine_instance_init,
    .instance_size = ଆକାର(SpikeState),
};

ସ୍ଥିର ଶୂନ୍ୟ spike_machine_init_register_types(ଶୂନ୍ୟ)
{
    type_register_static(&spike_machine_typeinfo);
}

type_init(spike_machine_init_register_types)
