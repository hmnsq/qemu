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

#समावेश "qemu/osdep.h"
#समावेश "qemu/error-report.h"
#समावेश "qapi/error.h"
#समावेश "hw/core/boards.h"
#समावेश "hw/core/loader.h"
#समावेश "hw/core/sysbus.h"
#समावेश "target/riscv/cpu.h"
#समावेश "hw/riscv/riscv_hart.h"
#समावेश "hw/riscv/spike.h"
#समावेश "hw/riscv/boot.h"
#समावेश "hw/riscv/fdt-common.h"
#समावेश "hw/riscv/numa.h"
#समावेश "hw/char/riscv_htif.h"
#समावेश "hw/intc/riscv_aclint.h"
#समावेश "chardev/char.h"
#समावेश "system/device_tree.h"
#समावेश "system/system.h"

#समावेश <libfdt.h>

अटल स्थिर MemMapEntry spike_memmap[] = {
    [SPIKE_MROM] =     {     0x1000,     0xf000 },
    [SPIKE_HTIF] =     {  0x1000000,     0x1000 },
    [SPIKE_CLINT] =    {  0x2000000,    0x10000 },
    [SPIKE_DRAM] =     { 0x80000000,        0x0 },
};

अटल व्योम create_fdt(SpikeState *s, स्थिर MemMapEntry *memmap,
                       bool is_32_bit, bool htif_custom_base)
{
    व्योम *fdt;
    पूर्णांक fdt_आकार;
    अचिन्हित दीर्घ clint_addr;
    पूर्णांक socket;
    MachineState *ms = MACHINE(s);
    uint32_t phandle = 1;
    bool numa_enabled = riscv_numa_enabled(ms);

    fdt = ms->fdt = riscv_create_board_device_वृक्ष(
        "ucbbar,spike-bare,qemu",
        "ucbbar,spike-bare-dev",
        &fdt_आकार);

    qemu_fdt_add_subnode(fdt, "/htif");
    qemu_fdt_setprop_string(fdt, "/htif", "compatible", "ucb,htif0");
    अगर (htif_custom_base) {
        qemu_fdt_setprop_cells(fdt, "/htif", "reg",
            0x0, memmap[SPIKE_HTIF].base, 0x0, memmap[SPIKE_HTIF].आकार);
    }

    riscv_fdt_create_cpu_socket_subnode(fdt,
                                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    क्रम (socket = (riscv_socket_गिनती(ms) - 1); socket >= 0; socket--) {
        g_autofree uint32_t *intc_phandles = g_new0(uint32_t,
                                                    s->soc[socket].अंक_harts);
        hwaddr memaddr = memmap[SPIKE_DRAM].base +
                         riscv_socket_mem_offset(ms, socket);
        uint64_t memsize =  riscv_socket_mem_आकार(ms, socket);

        riscv_create_fdt_socket_cpus(fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].अंक_harts,
                                     s->soc[socket].hartid_base,
                                     &phandle, intc_phandles, numa_enabled,
                                     is_32_bit);

        riscv_create_fdt_socket_memory(fdt, memaddr, memsize, socket,
                                       riscv_numa_enabled(ms));

        clint_addr = memmap[SPIKE_CLINT].base +
            (memmap[SPIKE_CLINT].आकार * socket);
        riscv_create_fdt_socket_clint(fdt, clint_addr,
                                      memmap[SPIKE_CLINT].आकार, socket,
                                      intc_phandles, s->soc[socket].अंक_harts,
                                      numa_enabled);
    }

    riscv_socket_fdt_write_distance_matrix(ms);

    qemu_fdt_add_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "/htif");
}

अटल bool spike_test_elf_image(अक्षर *filename)
{
    वापस load_elf_hdr(filename, शून्य, शून्य, शून्य);
}

अटल व्योम spike_board_init(MachineState *machine)
{
    स्थिर MemMapEntry *memmap = spike_memmap;
    SpikeState *s = SPIKE_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();
    MemoryRegion *mask_rom = g_new(MemoryRegion, 1);
    hwaddr firmware_end_addr = memmap[SPIKE_DRAM].base;
    hwaddr firmware_load_addr = memmap[SPIKE_DRAM].base;
    vaddr kernel_start_addr;
    अक्षर *firmware_name;
    uint64_t fdt_load_addr;
    uint64_t kernel_entry;
    अक्षर *soc_name;
    पूर्णांक i, base_hartid, hart_गिनती;
    bool htif_custom_base = false;
    RISCVBootInfo boot_info;

    /* Check socket count limit */
    अगर (SPIKE_SOCKETS_MAX < riscv_socket_गिनती(machine)) {
        error_report("number of sockets/nodes should be less than %d",
            SPIKE_SOCKETS_MAX);
        निकास(1);
    }

    /* Initialize sockets */
    क्रम (i = 0; i < riscv_socket_गिनती(machine); i++) {
        अगर (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            निकास(1);
        }

        base_hartid = riscv_socket_first_hartid(machine, i);
        अगर (base_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            निकास(1);
        }

        hart_गिनती = riscv_socket_hart_गिनती(machine, i);
        अगर (hart_गिनती < 0) {
            error_report("can't find hart count for socket%d", i);
            निकास(1);
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
                                hart_गिनती, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Core Local Interruptor (timer and IPI) for each socket */
        riscv_aclint_swi_create(system_memory,
            memmap[SPIKE_CLINT].base + i * memmap[SPIKE_CLINT].आकार,
            base_hartid, hart_गिनती, false);
        riscv_aclint_mtimer_create(system_memory,
            memmap[SPIKE_CLINT].base + i * memmap[SPIKE_CLINT].आकार +
                RISCV_ACLINT_SWI_आकार,
            RISCV_ACLINT_DEFAULT_MTIMER_आकार, base_hartid, hart_गिनती,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }

    /* register system main memory (actual RAM) */
    memory_region_add_subregion(system_memory, memmap[SPIKE_DRAM].base,
        machine->ram);

    /* boot rom */
    memory_region_init_rom(mask_rom, शून्य, "riscv.spike.mrom",
                           memmap[SPIKE_MROM].आकार, &error_fatal);
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
    अगर (!firmware_name && !machine->kernel_filename) {
        htif_custom_base = true;
    } अन्यथा {
        अगर (firmware_name) {
            htif_custom_base = !spike_test_elf_image(firmware_name);
        }
        अगर (!htif_custom_base && machine->kernel_filename) {
            htif_custom_base = !spike_test_elf_image(machine->kernel_filename);
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    /* Load firmware */
    अगर (firmware_name) {
        firmware_end_addr = riscv_load_firmware(machine, &boot_info,
                                                firmware_name,
                                                &firmware_load_addr,
                                                htif_प्रतीक_callback);
        g_free(firmware_name);
    }

    /* Create device tree */
    create_fdt(s, memmap, riscv_is_32bit(&s->soc[0]), htif_custom_base);

    /* Load kernel */
    अगर (machine->kernel_filename) {
        kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                         firmware_end_addr);

        riscv_load_kernel(machine, &boot_info, kernel_start_addr,
                          true, htif_प्रतीक_callback);
        kernel_entry = boot_info.image_निचला_addr;
    } अन्यथा {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_entry = 0;
    }

    fdt_load_addr = riscv_compute_fdt_addr(memmap[SPIKE_DRAM].base,
                                           memmap[SPIKE_DRAM].आकार,
                                           machine, &boot_info);
    riscv_load_fdt(fdt_load_addr, machine->fdt);

    /* load the reset vector */
    riscv_setup_rom_reset_vec(machine, &s->soc[0], firmware_load_addr,
                              memmap[SPIKE_MROM].base,
                              memmap[SPIKE_MROM].आकार, kernel_entry,
                              fdt_load_addr);

    /* initialize HTIF using symbols found in load_kernel */
    htif_mm_init(system_memory, serial_hd(0), memmap[SPIKE_HTIF].base,
                 htif_custom_base);
}

अटल व्योम spike_set_signature(Object *obj, स्थिर अक्षर *val, Error **errp)
{
    sig_file = g_strdup(val);
}

अटल व्योम spike_machine_instance_init(Object *obj)
{
}

अटल व्योम spike_machine_class_init(ObjectClass *oc, स्थिर व्योम *तथ्य)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Spike board";
    mc->init = spike_board_init;
    mc->max_cpus = SPIKE_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_MAX;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_सूचकांक_to_instance_props = riscv_numa_cpu_सूचकांक_to_props;
    mc->get_default_cpu_गाँठ_id = riscv_numa_get_default_cpu_गाँठ_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv.spike.ram";
    object_class_property_add_str(oc, "signature", शून्य, spike_set_signature);
    object_class_property_set_description(oc, "signature",
                                          "File to write ACT test signature");
    object_class_static_property_add_uint8_ptr(oc, "signature-granularity",
                                               &पंक्ति_आकार,
                                               OBJ_PROP_FLAG_WRITE);
    object_class_property_set_description(oc, "signature-granularity",
                                          "Size of each line in ACT signature "
                                          "file");
}

अटल स्थिर TypeInfo spike_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("spike"),
    .parent     = TYPE_MACHINE,
    .class_init = spike_machine_class_init,
    .instance_init = spike_machine_instance_init,
    .instance_आकार = माप(SpikeState),
};

अटल व्योम spike_machine_init_register_types(व्योम)
{
    type_register_static(&spike_machine_typeinfo);
}

type_init(spike_machine_init_register_types)
