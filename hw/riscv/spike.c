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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/loader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/sysbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/riscv_hart.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/spike.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/fdt-common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/numa.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/char/riscv_htif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chardev/char.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

ਸਥਿਰ ਅਚਲ Mem_Map_ਇੰਦਰਾਜ spike_memmap[] = {
    [SPIKE_MROM] =     {     0x1000,     0xf000 },
    [SPIKE_HTIF] =     {  0x1000000,     0x1000 },
    [SPIKE_CLINT] =    {  0x2000000,    0x10000 },
    [SPIKE_DRAM] =     { 0x80000000,        0x0 },
};

ਸਥਿਰ ਖਾਲੀ create_fdt(SpikeState *s, ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap,
                       bool is_32_ਦੋਆਧਾਰੀ_ਅੰਕ, bool htif_custom_ਅਧਾਰ)
{
    ਖਾਲੀ *fdt;
    ਪੂਰਨ fdt_ਮਾਪ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ clint_addr;
    ਪੂਰਨ socket;
    MachineState *ms = MACHINE(s);
    uint32_t phandle = 1;
    bool numa_enabled = riscv_numa_enabled(ms);

    fdt = ms->fdt = riscv_create_board_device_ਰੁੱਖ(
        "ucbbar,spike-bare,qemu",
        "ucbbar,spike-bare-dev",
        &fdt_ਮਾਪ);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, "/htif");
    qemu_fdt_setprop_string(fdt, "/htif", "compatible", "ucb,htif0");
    ਜੇ (htif_custom_ਅਧਾਰ) {
        qemu_fdt_setprop_cells(fdt, "/htif", "reg",
            0x0, memmap[SPIKE_HTIF].ਅਧਾਰ, 0x0, memmap[SPIKE_HTIF].ਮਾਪ);
    }

    riscv_fdt_create_cpu_socket_subnode(fdt,
                                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    ਲਈ (socket = (riscv_socket_ਗਿਣਤੀ(ms) - 1); socket >= 0; socket--) {
        g_autofree uint32_t *intc_phandles = g_new0(uint32_t,
                                                    s->soc[socket].ਅੰਕ_harts);
        hwaddr memaddr = memmap[SPIKE_DRAM].ਅਧਾਰ +
                         riscv_socket_mem_ਵਿੱਥ(ms, socket);
        uint64_t memsize =  riscv_socket_mem_ਮਾਪ(ms, socket);

        riscv_create_fdt_socket_cpus(fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].ਅੰਕ_harts,
                                     s->soc[socket].hartid_ਅਧਾਰ,
                                     &phandle, intc_phandles, numa_enabled,
                                     is_32_ਦੋਆਧਾਰੀ_ਅੰਕ);

        riscv_create_fdt_socket_ਸਮ੍ਰਿਤੀ(fdt, memaddr, memsize, socket,
                                       riscv_numa_enabled(ms));

        clint_addr = memmap[SPIKE_CLINT].ਅਧਾਰ +
            (memmap[SPIKE_CLINT].ਮਾਪ * socket);
        riscv_create_fdt_socket_clint(fdt, clint_addr,
                                      memmap[SPIKE_CLINT].ਮਾਪ, socket,
                                      intc_phandles, s->soc[socket].ਅੰਕ_harts,
                                      numa_enabled);
    }

    riscv_socket_fdt_write_ਦੂਰੀ_ਮੈਟ੍ਰਿਕਸ(ms);

    qemu_fdt_ਜੋੜੋ_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "/htif");
}

ਸਥਿਰ bool spike_test_elf_image(ਅੱਖਰ *filename)
{
    ਵਾਪਸ ਲੱਦੋ_elf_hdr(filename, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
}

ਸਥਿਰ ਖਾਲੀ spike_board_init(MachineState *machine)
{
    ਅਚਲ Mem_Map_ਇੰਦਰਾਜ *memmap = spike_memmap;
    SpikeState *s = SPIKE_MACHINE(machine);
    ਸਮ੍ਰਿਤੀ_Region *system_ਸਮ੍ਰਿਤੀ = get_system_ਸਮ੍ਰਿਤੀ();
    ਸਮ੍ਰਿਤੀ_Region *ਨਕਾਬ_rom = g_new(ਸਮ੍ਰਿਤੀ_Region, 1);
    hwaddr firmware_ਸਿਰਾ_addr = memmap[SPIKE_DRAM].ਅਧਾਰ;
    hwaddr firmware_ਲੱਦੋ_addr = memmap[SPIKE_DRAM].ਅਧਾਰ;
    vaddr kernel_ਸ਼ੁਰੂ_addr;
    ਅੱਖਰ *firmware_name;
    uint64_t fdt_ਲੱਦੋ_addr;
    uint64_t kernel_ਇੰਦਰਾਜ;
    ਅੱਖਰ *soc_name;
    ਪੂਰਨ i, ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ;
    bool htif_custom_ਅਧਾਰ = false;
    RISCVBootInfo boot_info;

    /* Check socket count limit */
    ਜੇ (SPIKE_SOCKETS_MAX < riscv_socket_ਗਿਣਤੀ(machine)) {
        error_report("number of sockets/nodes should be less than %d",
            SPIKE_SOCKETS_MAX);
        ਬਾਹਰ_ਨਿਕਲੋ(1);
    }

    /* Initialize sockets */
    ਲਈ (i = 0; i < riscv_socket_ਗਿਣਤੀ(machine); i++) {
        ਜੇ (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }

        ਅਧਾਰ_hartid = riscv_socket_ਪਹਿਲਾ_hartid(machine, i);
        ਜੇ (ਅਧਾਰ_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }

        hart_ਗਿਣਤੀ = riscv_socket_hart_ਗਿਣਤੀ(machine, i);
        ਜੇ (hart_ਗਿਣਤੀ < 0) {
            error_report("can't find hart count for socket%d", i);
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }

        soc_name = g_strdup_printf("soc%d", i);
        object_initialize_ਬੱਚਾ(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        g_free(soc_name);
        object_property_ਸੈੱਟ_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_ਸੈੱਟ_int(OBJECT(&s->soc[i]), "hartid-base",
                                ਅਧਾਰ_hartid, &error_abort);
        object_property_ਸੈੱਟ_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_ਗਿਣਤੀ, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Core Local Interruptor (timer and IPI) for each socket */
        riscv_aclint_swi_create(system_ਸਮ੍ਰਿਤੀ,
            memmap[SPIKE_CLINT].ਅਧਾਰ + i * memmap[SPIKE_CLINT].ਮਾਪ,
            ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ, false);
        riscv_aclint_mtimer_create(system_ਸਮ੍ਰਿਤੀ,
            memmap[SPIKE_CLINT].ਅਧਾਰ + i * memmap[SPIKE_CLINT].ਮਾਪ +
                RISCV_ACLINT_SWI_ਮਾਪ,
            RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ, ਅਧਾਰ_hartid, hart_ਗਿਣਤੀ,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }

    /* register system main memory (actual RAM) */
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ, memmap[SPIKE_DRAM].ਅਧਾਰ,
        machine->ram);

    /* boot rom */
    ਸਮ੍ਰਿਤੀ_region_init_rom(ਨਕਾਬ_rom, ਨਿਰਾ, "riscv.spike.mrom",
                           memmap[SPIKE_MROM].ਮਾਪ, &error_fatal);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ, memmap[SPIKE_MROM].ਅਧਾਰ,
                                ਨਕਾਬ_rom);

    /* Find firmware */
    firmware_name = riscv_ਲੱਭੋ_firmware(machine->firmware,
                        riscv_default_firmware_name(&s->soc[0]));

    /*
     * Test the given firmware or kernel file to see if it is an ELF image.
     * If it is an ELF, we assume it contains the symbols required for
     * the HTIF console, otherwise we fall back to use the custom base
     * passed from device tree for the HTIF console.
     */
    ਜੇ (!firmware_name && !machine->kernel_filename) {
        htif_custom_ਅਧਾਰ = true;
    } ਨਹੀਂਤਾਂ {
        ਜੇ (firmware_name) {
            htif_custom_ਅਧਾਰ = !spike_test_elf_image(firmware_name);
        }
        ਜੇ (!htif_custom_ਅਧਾਰ && machine->kernel_filename) {
            htif_custom_ਅਧਾਰ = !spike_test_elf_image(machine->kernel_filename);
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    /* Load firmware */
    ਜੇ (firmware_name) {
        firmware_ਸਿਰਾ_addr = riscv_ਲੱਦੋ_firmware(machine, &boot_info,
                                                firmware_name,
                                                &firmware_ਲੱਦੋ_addr,
                                                htif_ਚਿੰਨ੍ਹ_callback);
        g_free(firmware_name);
    }

    /* Create device tree */
    create_fdt(s, memmap, riscv_is_32bit(&s->soc[0]), htif_custom_ਅਧਾਰ);

    /* Load kernel */
    ਜੇ (machine->kernel_filename) {
        kernel_ਸ਼ੁਰੂ_addr = riscv_calc_kernel_ਸ਼ੁਰੂ_addr(&boot_info,
                                                         firmware_ਸਿਰਾ_addr);

        riscv_ਲੱਦੋ_kernel(machine, &boot_info, kernel_ਸ਼ੁਰੂ_addr,
                          true, htif_ਚਿੰਨ੍ਹ_callback);
        kernel_ਇੰਦਰਾਜ = boot_info.image_ਨੀਵਾਂ_addr;
    } ਨਹੀਂਤਾਂ {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_ਇੰਦਰਾਜ = 0;
    }

    fdt_ਲੱਦੋ_addr = riscv_compute_fdt_addr(memmap[SPIKE_DRAM].ਅਧਾਰ,
                                           memmap[SPIKE_DRAM].ਮਾਪ,
                                           machine, &boot_info);
    riscv_ਲੱਦੋ_fdt(fdt_ਲੱਦੋ_addr, machine->fdt);

    /* load the reset vector */
    riscv_ਤਿਆਰੀ_rom_reset_vec(machine, &s->soc[0], firmware_ਲੱਦੋ_addr,
                              memmap[SPIKE_MROM].ਅਧਾਰ,
                              memmap[SPIKE_MROM].ਮਾਪ, kernel_ਇੰਦਰਾਜ,
                              fdt_ਲੱਦੋ_addr);

    /* initialize HTIF using symbols found in load_kernel */
    htif_mm_init(system_ਸਮ੍ਰਿਤੀ, serial_hd(0), memmap[SPIKE_HTIF].ਅਧਾਰ,
                 htif_custom_ਅਧਾਰ);
}

ਸਥਿਰ ਖਾਲੀ spike_ਸੈੱਟ_signature(Object *obj, ਅਚਲ ਅੱਖਰ *val, Error **errp)
{
    sig_file = g_strdup(val);
}

ਸਥਿਰ ਖਾਲੀ spike_machine_instance_init(Object *obj)
{
}

ਸਥਿਰ ਖਾਲੀ spike_machine_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Spike board";
    mc->init = spike_board_init;
    mc->max_cpus = SPIKE_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_MAX;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_instance_props = riscv_numa_cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_props;
    mc->get_default_cpu_ਗੰਢ_id = riscv_numa_get_default_cpu_ਗੰਢ_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv.spike.ram";
    object_class_property_ਜੋੜੋ_str(oc, "signature", ਨਿਰਾ, spike_ਸੈੱਟ_signature);
    object_class_property_ਸੈੱਟ_description(oc, "signature",
                                          "File to write ACT test signature");
    object_class_static_property_ਜੋੜੋ_uint8_ptr(oc, "signature-granularity",
                                               &ਸਤਰ_ਮਾਪ,
                                               OBJ_PROP_FLAG_WRITE);
    object_class_property_ਸੈੱਟ_description(oc, "signature-granularity",
                                          "Size of each line in ACT signature "
                                          "file");
}

ਸਥਿਰ ਅਚਲ TypeInfo spike_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("spike"),
    .ਮਾਪਾ     = TYPE_MACHINE,
    .class_init = spike_machine_class_init,
    .instance_init = spike_machine_instance_init,
    .instance_ਮਾਪ = ਆਕਾਰ(SpikeState),
};

ਸਥਿਰ ਖਾਲੀ spike_machine_init_ਰਜਿਸਟਰ_types(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&spike_machine_typeinfo);
}

type_init(spike_machine_init_ਰਜਿਸਟਰ_types)
