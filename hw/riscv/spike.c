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

#ضمن "qemu/osdep.h"
#ضمن "qemu/error-report.h"
#ضمن "qapi/error.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/core/sysbus.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "hw/riscv/spike.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/riscv/fdt-common.h"
#ضمن "hw/riscv/numa.h"
#ضمن "hw/char/riscv_htif.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "chardev/char.h"
#ضمن "system/device_tree.h"
#ضمن "system/system.h"

#ضمن <libfdt.h>

ساكن ثابت Mem_Map_ثبت spike_memmap[] = {
    [SPIKE_MROM] =     {     0x1000,     0xf000 },
    [SPIKE_HTIF] =     {  0x1000000,     0x1000 },
    [SPIKE_CLINT] =    {  0x2000000,    0x10000 },
    [SPIKE_DRAM] =     { 0x80000000,        0x0 },
};

ساكن فراغ create_fdt(SpikeState *s, ثابت Mem_Map_ثبت *memmap,
                       bool is_32_بټ, bool htif_custom_بنسټ)
{
    فراغ *fdt;
    صحيح fdt_اندازه;
    بینښه اوږد clint_addr;
    صحيح socket;
    MachineState *ms = MACHINE(s);
    uint32_t phandle = 1;
    bool numa_enabled = riscv_numa_enabled(ms);

    fdt = ms->fdt = riscv_create_board_device_ونه(
        "ucbbar,spike-bare,qemu",
        "ucbbar,spike-bare-dev",
        &fdt_اندازه);

    qemu_fdt_جمع_کړه_subnode(fdt, "/htif");
    qemu_fdt_setprop_string(fdt, "/htif", "compatible", "ucb,htif0");
    إذا (htif_custom_بنسټ) {
        qemu_fdt_setprop_cells(fdt, "/htif", "reg",
            0x0, memmap[SPIKE_HTIF].بنسټ, 0x0, memmap[SPIKE_HTIF].اندازه);
    }

    riscv_fdt_create_cpu_socket_subnode(fdt,
                                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    لكل (socket = (riscv_socket_شمېر(ms) - 1); socket >= 0; socket--) {
        g_autofree uint32_t *intc_phandles = g_new0(uint32_t,
                                                    s->soc[socket].عدد_harts);
        hwaddr memaddr = memmap[SPIKE_DRAM].بنسټ +
                         riscv_socket_mem_دننه_واټن(ms, socket);
        uint64_t memsize =  riscv_socket_mem_اندازه(ms, socket);

        riscv_create_fdt_socket_cpus(fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].عدد_harts,
                                     s->soc[socket].hartid_بنسټ,
                                     &phandle, intc_phandles, numa_enabled,
                                     is_32_بټ);

        riscv_create_fdt_socket_حافظه(fdt, memaddr, memsize, socket,
                                       riscv_numa_enabled(ms));

        clint_addr = memmap[SPIKE_CLINT].بنسټ +
            (memmap[SPIKE_CLINT].اندازه * socket);
        riscv_create_fdt_socket_clint(fdt, clint_addr,
                                      memmap[SPIKE_CLINT].اندازه, socket,
                                      intc_phandles, s->soc[socket].عدد_harts,
                                      numa_enabled);
    }

    riscv_socket_fdt_write_واټن_ماتریکس(ms);

    qemu_fdt_جمع_کړه_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "/htif");
}

ساكن bool spike_test_elf_image(حرف *filename)
{
    أرجع بارول_elf_hdr(filename, عدم, عدم, عدم);
}

ساكن فراغ spike_board_init(MachineState *machine)
{
    ثابت Mem_Map_ثبت *memmap = spike_memmap;
    SpikeState *s = SPIKE_MACHINE(machine);
    حافظه_Region *system_حافظه = get_system_حافظه();
    حافظه_Region *ماسک_rom = g_new(حافظه_Region, 1);
    hwaddr firmware_پای_ټکی_addr = memmap[SPIKE_DRAM].بنسټ;
    hwaddr firmware_بارول_addr = memmap[SPIKE_DRAM].بنسټ;
    vaddr kernel_پیل_addr;
    حرف *firmware_name;
    uint64_t fdt_بارول_addr;
    uint64_t kernel_ثبت;
    حرف *soc_name;
    صحيح i, بنسټ_hartid, hart_شمېر;
    bool htif_custom_بنسټ = false;
    RISCVBootInfo boot_info;

    /* Check socket count limit */
    إذا (SPIKE_SOCKETS_MAX < riscv_socket_شمېر(machine)) {
        error_report("number of sockets/nodes should be less than %d",
            SPIKE_SOCKETS_MAX);
        وتل(1);
    }

    /* Initialize sockets */
    لكل (i = 0; i < riscv_socket_شمېر(machine); i++) {
        إذا (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            وتل(1);
        }

        بنسټ_hartid = riscv_socket_لومړی_hartid(machine, i);
        إذا (بنسټ_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            وتل(1);
        }

        hart_شمېر = riscv_socket_hart_شمېر(machine, i);
        إذا (hart_شمېر < 0) {
            error_report("can't find hart count for socket%d", i);
            وتل(1);
        }

        soc_name = g_strdup_printf("soc%d", i);
        object_initialize_ماشوم(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        g_free(soc_name);
        object_property_سېټ_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_سېټ_int(OBJECT(&s->soc[i]), "hartid-base",
                                بنسټ_hartid, &error_abort);
        object_property_سېټ_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_شمېر, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Core Local Interruptor (timer and IPI) for each socket */
        riscv_aclint_swi_create(system_حافظه,
            memmap[SPIKE_CLINT].بنسټ + i * memmap[SPIKE_CLINT].اندازه,
            بنسټ_hartid, hart_شمېر, false);
        riscv_aclint_mtimer_create(system_حافظه,
            memmap[SPIKE_CLINT].بنسټ + i * memmap[SPIKE_CLINT].اندازه +
                RISCV_ACLINT_SWI_اندازه,
            RISCV_ACLINT_DEFAULT_MTIMER_اندازه, بنسټ_hartid, hart_شمېر,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }

    /* register system main memory (actual RAM) */
    حافظه_region_جمع_کړه_subregion(system_حافظه, memmap[SPIKE_DRAM].بنسټ,
        machine->ram);

    /* boot rom */
    حافظه_region_init_rom(ماسک_rom, عدم, "riscv.spike.mrom",
                           memmap[SPIKE_MROM].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(system_حافظه, memmap[SPIKE_MROM].بنسټ,
                                ماسک_rom);

    /* Find firmware */
    firmware_name = riscv_پیدا_کړه_firmware(machine->firmware,
                        riscv_default_firmware_name(&s->soc[0]));

    /*
     * Test the given firmware or kernel file to see if it is an ELF image.
     * If it is an ELF, we assume it contains the symbols required for
     * the HTIF console, otherwise we fall back to use the custom base
     * passed from device tree for the HTIF console.
     */
    إذا (!firmware_name && !machine->kernel_filename) {
        htif_custom_بنسټ = true;
    } وإلا {
        إذا (firmware_name) {
            htif_custom_بنسټ = !spike_test_elf_image(firmware_name);
        }
        إذا (!htif_custom_بنسټ && machine->kernel_filename) {
            htif_custom_بنسټ = !spike_test_elf_image(machine->kernel_filename);
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    /* Load firmware */
    إذا (firmware_name) {
        firmware_پای_ټکی_addr = riscv_بارول_firmware(machine, &boot_info,
                                                firmware_name,
                                                &firmware_بارول_addr,
                                                htif_نښه_callback);
        g_free(firmware_name);
    }

    /* Create device tree */
    create_fdt(s, memmap, riscv_is_32bit(&s->soc[0]), htif_custom_بنسټ);

    /* Load kernel */
    إذا (machine->kernel_filename) {
        kernel_پیل_addr = riscv_calc_kernel_پیل_addr(&boot_info,
                                                         firmware_پای_ټکی_addr);

        riscv_بارول_kernel(machine, &boot_info, kernel_پیل_addr,
                          true, htif_نښه_callback);
        kernel_ثبت = boot_info.image_ټیټ_addr;
    } وإلا {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_ثبت = 0;
    }

    fdt_بارول_addr = riscv_compute_fdt_addr(memmap[SPIKE_DRAM].بنسټ,
                                           memmap[SPIKE_DRAM].اندازه,
                                           machine, &boot_info);
    riscv_بارول_fdt(fdt_بارول_addr, machine->fdt);

    /* load the reset vector */
    riscv_تیاری_rom_reset_vec(machine, &s->soc[0], firmware_بارول_addr,
                              memmap[SPIKE_MROM].بنسټ,
                              memmap[SPIKE_MROM].اندازه, kernel_ثبت,
                              fdt_بارول_addr);

    /* initialize HTIF using symbols found in load_kernel */
    htif_mm_init(system_حافظه, serial_hd(0), memmap[SPIKE_HTIF].بنسټ,
                 htif_custom_بنسټ);
}

ساكن فراغ spike_سېټ_signature(Object *obj, ثابت حرف *val, Error **errp)
{
    sig_file = g_strdup(val);
}

ساكن فراغ spike_machine_instance_init(Object *obj)
{
}

ساكن فراغ spike_machine_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Spike board";
    mc->init = spike_board_init;
    mc->max_cpus = SPIKE_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_MAX;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_شاخص_منزل_instance_props = riscv_numa_cpu_شاخص_منزل_props;
    mc->get_default_cpu_غوټه_id = riscv_numa_get_default_cpu_غوټه_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv.spike.ram";
    object_class_property_جمع_کړه_str(oc, "signature", عدم, spike_سېټ_signature);
    object_class_property_سېټ_description(oc, "signature",
                                          "File to write ACT test signature");
    object_class_static_property_جمع_کړه_uint8_ptr(oc, "signature-granularity",
                                               &کرښه_اندازه,
                                               OBJ_PROP_FLAG_WRITE);
    object_class_property_سېټ_description(oc, "signature-granularity",
                                          "Size of each line in ACT signature "
                                          "file");
}

ساكن ثابت TypeInfo spike_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("spike"),
    .پلار     = TYPE_MACHINE,
    .class_init = spike_machine_class_init,
    .instance_init = spike_machine_instance_init,
    .instance_اندازه = حجم(SpikeState),
};

ساكن فراغ spike_machine_init_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&spike_machine_typeinfo);
}

type_init(spike_machine_init_ثبتوونکی_types)
