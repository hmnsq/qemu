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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/core/loader.h"
#अन्तर्भाव "hw/core/sysbus.h"
#अन्तर्भाव "target/riscv/cpu.h"
#अन्तर्भाव "hw/riscv/riscv_hart.h"
#अन्तर्भाव "hw/riscv/spike.h"
#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "hw/riscv/fdt-common.h"
#अन्तर्भाव "hw/riscv/numa.h"
#अन्तर्भाव "hw/char/riscv_htif.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "chardev/char.h"
#अन्तर्भाव "system/device_tree.h"
#अन्तर्भाव "system/system.h"

#अन्तर्भाव <libfdt.h>

स्थिर ध्रुव Mem_Map_प्रविष्टिः spike_memmap[] = {
    [SPIKE_MROM] =     {     0x1000,     0xf000 },
    [SPIKE_HTIF] =     {  0x1000000,     0x1000 },
    [SPIKE_CLINT] =    {  0x2000000,    0x10000 },
    [SPIKE_DRAM] =     { 0x80000000,        0x0 },
};

स्थिर शून्य create_fdt(SpikeState *s, ध्रुव Mem_Map_प्रविष्टिः *memmap,
                       bool is_32_bit, bool htif_custom_आधारः)
{
    शून्य *fdt;
    पूर्णाङ्क fdt_आकार;
    अचिह्नित दीर्घ clint_addr;
    पूर्णाङ्क socket;
    MachineState *ms = MACHINE(s);
    uint32_t phandle = 1;
    bool numa_enabled = riscv_numa_enabled(ms);

    fdt = ms->fdt = riscv_create_board_device_वृक्षः(
        "ucbbar,spike-bare,qemu",
        "ucbbar,spike-bare-dev",
        &fdt_आकार);

    qemu_fdt_सङ्कलय_subnode(fdt, "/htif");
    qemu_fdt_setprop_string(fdt, "/htif", "compatible", "ucb,htif0");
    यदि (htif_custom_आधारः) {
        qemu_fdt_setprop_cells(fdt, "/htif", "reg",
            0x0, memmap[SPIKE_HTIF].आधारः, 0x0, memmap[SPIKE_HTIF].आकार);
    }

    riscv_fdt_create_cpu_socket_subnode(fdt,
                                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    प्रति (socket = (riscv_socket_गणनम्(ms) - 1); socket >= 0; socket--) {
        g_autofree uint32_t *intc_phandles = g_new0(uint32_t,
                                                    s->soc[socket].अङ्कः_harts);
        hwaddr memaddr = memmap[SPIKE_DRAM].आधारः +
                         riscv_socket_mem_विस्थापनम्(ms, socket);
        uint64_t memsize =  riscv_socket_mem_आकार(ms, socket);

        riscv_create_fdt_socket_cpus(fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].अङ्कः_harts,
                                     s->soc[socket].hartid_आधारः,
                                     &phandle, intc_phandles, numa_enabled,
                                     is_32_bit);

        riscv_create_fdt_socket_स्मृतिः(fdt, memaddr, memsize, socket,
                                       riscv_numa_enabled(ms));

        clint_addr = memmap[SPIKE_CLINT].आधारः +
            (memmap[SPIKE_CLINT].आकार * socket);
        riscv_create_fdt_socket_clint(fdt, clint_addr,
                                      memmap[SPIKE_CLINT].आकार, socket,
                                      intc_phandles, s->soc[socket].अङ्कः_harts,
                                      numa_enabled);
    }

    riscv_socket_fdt_write_distance_matrix(ms);

    qemu_fdt_सङ्कलय_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "/htif");
}

स्थिर bool spike_test_elf_image(वर्ण *filename)
{
    प्रत्यागम आरोपय_elf_hdr(filename, रिक्त, रिक्त, रिक्त);
}

स्थिर शून्य spike_board_init(MachineState *machine)
{
    ध्रुव Mem_Map_प्रविष्टिः *memmap = spike_memmap;
    SpikeState *s = SPIKE_MACHINE(machine);
    स्मृतिः_Region *system_स्मृतिः = get_system_स्मृतिः();
    स्मृतिः_Region *आवरणम्_rom = g_new(स्मृतिः_Region, 1);
    hwaddr firmware_अन्तः_addr = memmap[SPIKE_DRAM].आधारः;
    hwaddr firmware_आरोपय_addr = memmap[SPIKE_DRAM].आधारः;
    vaddr kernel_start_addr;
    वर्ण *firmware_name;
    uint64_t fdt_आरोपय_addr;
    uint64_t kernel_प्रविष्टिः;
    वर्ण *soc_name;
    पूर्णाङ्क i, आधारः_hartid, hart_गणनम्;
    bool htif_custom_आधारः = false;
    RISCVBootInfo boot_info;

    /* Check socket count limit */
    यदि (SPIKE_SOCKETS_MAX < riscv_socket_गणनम्(machine)) {
        error_report("number of sockets/nodes should be less than %d",
            SPIKE_SOCKETS_MAX);
        निर्गच्छ(1);
    }

    /* Initialize sockets */
    प्रति (i = 0; i < riscv_socket_गणनम्(machine); i++) {
        यदि (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            निर्गच्छ(1);
        }

        आधारः_hartid = riscv_socket_first_hartid(machine, i);
        यदि (आधारः_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            निर्गच्छ(1);
        }

        hart_गणनम् = riscv_socket_hart_गणनम्(machine, i);
        यदि (hart_गणनम् < 0) {
            error_report("can't find hart count for socket%d", i);
            निर्गच्छ(1);
        }

        soc_name = g_strdup_printf("soc%d", i);
        object_initialize_child(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        g_free(soc_name);
        object_property_समुच्चयः_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_समुच्चयः_int(OBJECT(&s->soc[i]), "hartid-base",
                                आधारः_hartid, &error_abort);
        object_property_समुच्चयः_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_गणनम्, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Core Local Interruptor (timer and IPI) for each socket */
        riscv_aclint_swi_create(system_स्मृतिः,
            memmap[SPIKE_CLINT].आधारः + i * memmap[SPIKE_CLINT].आकार,
            आधारः_hartid, hart_गणनम्, false);
        riscv_aclint_mtimer_create(system_स्मृतिः,
            memmap[SPIKE_CLINT].आधारः + i * memmap[SPIKE_CLINT].आकार +
                RISCV_ACLINT_SWI_आकार,
            RISCV_ACLINT_DEFAULT_MTIMER_आकार, आधारः_hartid, hart_गणनम्,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }

    /* register system main memory (actual RAM) */
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः, memmap[SPIKE_DRAM].आधारः,
        machine->ram);

    /* boot rom */
    स्मृतिः_region_init_rom(आवरणम्_rom, रिक्त, "riscv.spike.mrom",
                           memmap[SPIKE_MROM].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः, memmap[SPIKE_MROM].आधारः,
                                आवरणम्_rom);

    /* Find firmware */
    firmware_name = riscv_find_firmware(machine->firmware,
                        riscv_default_firmware_name(&s->soc[0]));

    /*
     * Test the given firmware or kernel file to see if it is an ELF image.
     * If it is an ELF, we assume it contains the symbols required for
     * the HTIF console, otherwise we fall back to use the custom base
     * passed from device tree for the HTIF console.
     */
    यदि (!firmware_name && !machine->kernel_filename) {
        htif_custom_आधारः = true;
    } अन्यथा {
        यदि (firmware_name) {
            htif_custom_आधारः = !spike_test_elf_image(firmware_name);
        }
        यदि (!htif_custom_आधारः && machine->kernel_filename) {
            htif_custom_आधारः = !spike_test_elf_image(machine->kernel_filename);
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    /* Load firmware */
    यदि (firmware_name) {
        firmware_अन्तः_addr = riscv_आरोपय_firmware(machine, &boot_info,
                                                firmware_name,
                                                &firmware_आरोपय_addr,
                                                htif_चिह्नम्_callback);
        g_free(firmware_name);
    }

    /* Create device tree */
    create_fdt(s, memmap, riscv_is_32bit(&s->soc[0]), htif_custom_आधारः);

    /* Load kernel */
    यदि (machine->kernel_filename) {
        kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                         firmware_अन्तः_addr);

        riscv_आरोपय_kernel(machine, &boot_info, kernel_start_addr,
                          true, htif_चिह्नम्_callback);
        kernel_प्रविष्टिः = boot_info.image_नीचः_addr;
    } अन्यथा {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_प्रविष्टिः = 0;
    }

    fdt_आरोपय_addr = riscv_compute_fdt_addr(memmap[SPIKE_DRAM].आधारः,
                                           memmap[SPIKE_DRAM].आकार,
                                           machine, &boot_info);
    riscv_आरोपय_fdt(fdt_आरोपय_addr, machine->fdt);

    /* load the reset vector */
    riscv_सज्जता_rom_reset_vec(machine, &s->soc[0], firmware_आरोपय_addr,
                              memmap[SPIKE_MROM].आधारः,
                              memmap[SPIKE_MROM].आकार, kernel_प्रविष्टिः,
                              fdt_आरोपय_addr);

    /* initialize HTIF using symbols found in load_kernel */
    htif_mm_init(system_स्मृतिः, serial_hd(0), memmap[SPIKE_HTIF].आधारः,
                 htif_custom_आधारः);
}

स्थिर शून्य spike_समुच्चयः_signature(Object *obj, ध्रुव वर्ण *val, Error **errp)
{
    sig_file = g_strdup(val);
}

स्थिर शून्य spike_machine_instance_init(Object *obj)
{
}

स्थिर शून्य spike_machine_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Spike board";
    mc->init = spike_board_init;
    mc->max_cpus = SPIKE_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_MAX;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_सूचकांक_गन्तव्यम्_instance_props = riscv_numa_cpu_सूचकांक_गन्तव्यम्_props;
    mc->get_default_cpu_ग्रन्थिः_id = riscv_numa_get_default_cpu_ग्रन्थिः_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv.spike.ram";
    object_class_property_सङ्कलय_str(oc, "signature", रिक्त, spike_समुच्चयः_signature);
    object_class_property_समुच्चयः_description(oc, "signature",
                                          "File to write ACT test signature");
    object_class_static_property_सङ्कलय_uint8_ptr(oc, "signature-granularity",
                                               &पङ्क्तिः_आकार,
                                               OBJ_PROP_FLAG_WRITE);
    object_class_property_समुच्चयः_description(oc, "signature-granularity",
                                          "Size of each line in ACT signature "
                                          "file");
}

स्थिर ध्रुव TypeInfo spike_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("spike"),
    .parent     = TYPE_MACHINE,
    .class_init = spike_machine_class_init,
    .instance_init = spike_machine_instance_init,
    .instance_आकार = परिमाण(SpikeState),
};

स्थिर शून्य spike_machine_init_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&spike_machine_typeinfo);
}

type_init(spike_machine_init_पञ्जिकाकोशः_types)
