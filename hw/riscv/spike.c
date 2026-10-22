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

ساكن ثابت Mem_Map_داخلا spike_memmap[] = {
    [SPIKE_MROM] =     {     0x1000,     0xf000 },
    [SPIKE_HTIF] =     {  0x1000000,     0x1000 },
    [SPIKE_CLINT] =    {  0x2000000,    0x10000 },
    [SPIKE_DRAM] =     { 0x80000000,        0x0 },
};

ساكن فراغ create_fdt(SpikeState *s, ثابت Mem_Map_داخلا *memmap,
                       bool is_32_ٻٽو_انگ, bool htif_custom_بنياد)
{
    فراغ *fdt;
    صحيح fdt_سائيز;
    بنانشان ڊگهو clint_addr;
    صحيح socket;
    MachineState *ms = MACHINE(s);
    uint32_t phandle = 1;
    bool numa_enabled = riscv_numa_enabled(ms);

    fdt = ms->fdt = riscv_create_board_device_وڻ(
        "ucbbar,spike-bare,qemu",
        "ucbbar,spike-bare-dev",
        &fdt_سائيز);

    qemu_fdt_جوڙيو_subnode(fdt, "/htif");
    qemu_fdt_setprop_string(fdt, "/htif", "compatible", "ucb,htif0");
    إذا (htif_custom_بنياد) {
        qemu_fdt_setprop_cells(fdt, "/htif", "reg",
            0x0, memmap[SPIKE_HTIF].بنياد, 0x0, memmap[SPIKE_HTIF].سائيز);
    }

    riscv_fdt_create_cpu_socket_subnode(fdt,
                                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    لكل (socket = (riscv_socket_ڳڻپ(ms) - 1); socket >= 0; socket--) {
        g_autofree uint32_t *intc_phandles = g_new0(uint32_t,
                                                    s->soc[socket].انگ_harts);
        hwaddr memaddr = memmap[SPIKE_DRAM].بنياد +
                         riscv_socket_mem_وٿي(ms, socket);
        uint64_t memsize =  riscv_socket_mem_سائيز(ms, socket);

        riscv_create_fdt_socket_cpus(fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].انگ_harts,
                                     s->soc[socket].hartid_بنياد,
                                     &phandle, intc_phandles, numa_enabled,
                                     is_32_ٻٽو_انگ);

        riscv_create_fdt_socket_يادگيري(fdt, memaddr, memsize, socket,
                                       riscv_numa_enabled(ms));

        clint_addr = memmap[SPIKE_CLINT].بنياد +
            (memmap[SPIKE_CLINT].سائيز * socket);
        riscv_create_fdt_socket_clint(fdt, clint_addr,
                                      memmap[SPIKE_CLINT].سائيز, socket,
                                      intc_phandles, s->soc[socket].انگ_harts,
                                      numa_enabled);
    }

    riscv_socket_fdt_write_مفاصلو_ميٽرڪس(ms);

    qemu_fdt_جوڙيو_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "/htif");
}

ساكن bool spike_test_elf_image(حرف *filename)
{
    أرجع ڀريو_elf_hdr(filename, عدم, عدم, عدم);
}

ساكن فراغ spike_board_init(MachineState *machine)
{
    ثابت Mem_Map_داخلا *memmap = spike_memmap;
    SpikeState *s = SPIKE_MACHINE(machine);
    يادگيري_Region *system_يادگيري = get_system_يادگيري();
    يادگيري_Region *نقاب_rom = g_new(يادگيري_Region, 1);
    hwaddr firmware_پڇاڙي_addr = memmap[SPIKE_DRAM].بنياد;
    hwaddr firmware_ڀريو_addr = memmap[SPIKE_DRAM].بنياد;
    vaddr kernel_شروعات_addr;
    حرف *firmware_name;
    uint64_t fdt_ڀريو_addr;
    uint64_t kernel_داخلا;
    حرف *soc_name;
    صحيح i, بنياد_hartid, hart_ڳڻپ;
    bool htif_custom_بنياد = false;
    RISCVBootInfo boot_info;

    /* Check socket count limit */
    إذا (SPIKE_SOCKETS_MAX < riscv_socket_ڳڻپ(machine)) {
        error_report("number of sockets/nodes should be less than %d",
            SPIKE_SOCKETS_MAX);
        ٻاهر_نڪرو(1);
    }

    /* Initialize sockets */
    لكل (i = 0; i < riscv_socket_ڳڻپ(machine); i++) {
        إذا (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            ٻاهر_نڪرو(1);
        }

        بنياد_hartid = riscv_socket_پهريون_hartid(machine, i);
        إذا (بنياد_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            ٻاهر_نڪرو(1);
        }

        hart_ڳڻپ = riscv_socket_hart_ڳڻپ(machine, i);
        إذا (hart_ڳڻپ < 0) {
            error_report("can't find hart count for socket%d", i);
            ٻاهر_نڪرو(1);
        }

        soc_name = g_strdup_printf("soc%d", i);
        object_initialize_ٻار(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        g_free(soc_name);
        object_property_سيٽ_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_سيٽ_int(OBJECT(&s->soc[i]), "hartid-base",
                                بنياد_hartid, &error_abort);
        object_property_سيٽ_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_ڳڻپ, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Core Local Interruptor (timer and IPI) for each socket */
        riscv_aclint_swi_create(system_يادگيري,
            memmap[SPIKE_CLINT].بنياد + i * memmap[SPIKE_CLINT].سائيز,
            بنياد_hartid, hart_ڳڻپ, false);
        riscv_aclint_mtimer_create(system_يادگيري,
            memmap[SPIKE_CLINT].بنياد + i * memmap[SPIKE_CLINT].سائيز +
                RISCV_ACLINT_SWI_سائيز,
            RISCV_ACLINT_DEFAULT_MTIMER_سائيز, بنياد_hartid, hart_ڳڻپ,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }

    /* register system main memory (actual RAM) */
    يادگيري_region_جوڙيو_subregion(system_يادگيري, memmap[SPIKE_DRAM].بنياد,
        machine->ram);

    /* boot rom */
    يادگيري_region_init_rom(نقاب_rom, عدم, "riscv.spike.mrom",
                           memmap[SPIKE_MROM].سائيز, &error_fatal);
    يادگيري_region_جوڙيو_subregion(system_يادگيري, memmap[SPIKE_MROM].بنياد,
                                نقاب_rom);

    /* Find firmware */
    firmware_name = riscv_ڳوليو_firmware(machine->firmware,
                        riscv_default_firmware_name(&s->soc[0]));

    /*
     * Test the given firmware or kernel file to see if it is an ELF image.
     * If it is an ELF, we assume it contains the symbols required for
     * the HTIF console, otherwise we fall back to use the custom base
     * passed from device tree for the HTIF console.
     */
    إذا (!firmware_name && !machine->kernel_filename) {
        htif_custom_بنياد = true;
    } وإلا {
        إذا (firmware_name) {
            htif_custom_بنياد = !spike_test_elf_image(firmware_name);
        }
        إذا (!htif_custom_بنياد && machine->kernel_filename) {
            htif_custom_بنياد = !spike_test_elf_image(machine->kernel_filename);
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    /* Load firmware */
    إذا (firmware_name) {
        firmware_پڇاڙي_addr = riscv_ڀريو_firmware(machine, &boot_info,
                                                firmware_name,
                                                &firmware_ڀريو_addr,
                                                htif_نشان_callback);
        g_free(firmware_name);
    }

    /* Create device tree */
    create_fdt(s, memmap, riscv_is_32bit(&s->soc[0]), htif_custom_بنياد);

    /* Load kernel */
    إذا (machine->kernel_filename) {
        kernel_شروعات_addr = riscv_calc_kernel_شروعات_addr(&boot_info,
                                                         firmware_پڇاڙي_addr);

        riscv_ڀريو_kernel(machine, &boot_info, kernel_شروعات_addr,
                          true, htif_نشان_callback);
        kernel_داخلا = boot_info.image_هيٺيون_addr;
    } وإلا {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_داخلا = 0;
    }

    fdt_ڀريو_addr = riscv_compute_fdt_addr(memmap[SPIKE_DRAM].بنياد,
                                           memmap[SPIKE_DRAM].سائيز,
                                           machine, &boot_info);
    riscv_ڀريو_fdt(fdt_ڀريو_addr, machine->fdt);

    /* load the reset vector */
    riscv_تياري_rom_reset_vec(machine, &s->soc[0], firmware_ڀريو_addr,
                              memmap[SPIKE_MROM].بنياد,
                              memmap[SPIKE_MROM].سائيز, kernel_داخلا,
                              fdt_ڀريو_addr);

    /* initialize HTIF using symbols found in load_kernel */
    htif_mm_init(system_يادگيري, serial_hd(0), memmap[SPIKE_HTIF].بنياد,
                 htif_custom_بنياد);
}

ساكن فراغ spike_سيٽ_signature(Object *obj, ثابت حرف *val, Error **errp)
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
    mc->cpu_اشاريو_منزل_instance_props = riscv_numa_cpu_اشاريو_منزل_props;
    mc->get_default_cpu_ڳنڍ_id = riscv_numa_get_default_cpu_ڳنڍ_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv.spike.ram";
    object_class_property_جوڙيو_str(oc, "signature", عدم, spike_سيٽ_signature);
    object_class_property_سيٽ_description(oc, "signature",
                                          "File to write ACT test signature");
    object_class_static_property_جوڙيو_uint8_ptr(oc, "signature-granularity",
                                               &سٽ_سائيز,
                                               OBJ_PROP_FLAG_WRITE);
    object_class_property_سيٽ_description(oc, "signature-granularity",
                                          "Size of each line in ACT signature "
                                          "file");
}

ساكن ثابت TypeInfo spike_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("spike"),
    .والد     = TYPE_MACHINE,
    .class_init = spike_machine_class_init,
    .instance_init = spike_machine_instance_init,
    .instance_سائيز = حجم(SpikeState),
};

ساكن فراغ spike_machine_init_رجسٽر_types(فراغ)
{
    type_رجسٽر_static(&spike_machine_typeinfo);
}

type_init(spike_machine_init_رجسٽر_types)
