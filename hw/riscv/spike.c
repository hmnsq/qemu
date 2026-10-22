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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "hw/riscv/spike.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/riscv/fdt-common.h"
#అన్తర్భావ "hw/riscv/numa.h"
#అన్తర్భావ "hw/char/riscv_htif.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "chardev/char.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "system/system.h"

#అన్తర్భావ <libfdt.h>

స్థిర స్థిరరాశి Mem_Map_నమోదు spike_memmap[] = {
    [SPIKE_MROM] =     {     0x1000,     0xf000 },
    [SPIKE_HTIF] =     {  0x1000000,     0x1000 },
    [SPIKE_CLINT] =    {  0x2000000,    0x10000 },
    [SPIKE_DRAM] =     { 0x80000000,        0x0 },
};

స్థిర శూన్య create_fdt(SpikeState *s, స్థిరరాశి Mem_Map_నమోదు *memmap,
                       bool is_32_బిట్టు, bool htif_custom_పునాది)
{
    శూన్య *fdt;
    పూర్ణాంకం fdt_కొలత;
    గుర్తులేని దీర్ఘ clint_addr;
    పూర్ణాంకం socket;
    MachineState *ms = MACHINE(s);
    uint32_t phandle = 1;
    bool numa_enabled = riscv_numa_enabled(ms);

    fdt = ms->fdt = riscv_create_board_device_వృక్షం(
        "ucbbar,spike-bare,qemu",
        "ucbbar,spike-bare-dev",
        &fdt_కొలత);

    qemu_fdt_కూడు_subnode(fdt, "/htif");
    qemu_fdt_setprop_string(fdt, "/htif", "compatible", "ucb,htif0");
    ఒకవేళ (htif_custom_పునాది) {
        qemu_fdt_setprop_cells(fdt, "/htif", "reg",
            0x0, memmap[SPIKE_HTIF].పునాది, 0x0, memmap[SPIKE_HTIF].కొలత);
    }

    riscv_fdt_create_cpu_socket_subnode(fdt,
                                        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ);

    కొరకు (socket = (riscv_socket_లెక్క(ms) - 1); socket >= 0; socket--) {
        g_autofree uint32_t *intc_phandles = g_new0(uint32_t,
                                                    s->soc[socket].అంకె_harts);
        hwaddr memaddr = memmap[SPIKE_DRAM].పునాది +
                         riscv_socket_mem_స్థానభ్రంశం(ms, socket);
        uint64_t memsize =  riscv_socket_mem_కొలత(ms, socket);

        riscv_create_fdt_socket_cpus(fdt, (&s->soc[socket])->harts, socket,
                                     s->soc[socket].అంకె_harts,
                                     s->soc[socket].hartid_పునాది,
                                     &phandle, intc_phandles, numa_enabled,
                                     is_32_బిట్టు);

        riscv_create_fdt_socket_జ్ఞాపకం(fdt, memaddr, memsize, socket,
                                       riscv_numa_enabled(ms));

        clint_addr = memmap[SPIKE_CLINT].పునాది +
            (memmap[SPIKE_CLINT].కొలత * socket);
        riscv_create_fdt_socket_clint(fdt, clint_addr,
                                      memmap[SPIKE_CLINT].కొలత, socket,
                                      intc_phandles, s->soc[socket].అంకె_harts,
                                      numa_enabled);
    }

    riscv_socket_fdt_write_దూరం_మాతృక(ms);

    qemu_fdt_కూడు_subnode(fdt, "/chosen");
    qemu_fdt_setprop_string(fdt, "/chosen", "stdout-path", "/htif");
}

స్థిర bool spike_test_elf_image(అక్షరం *filename)
{
    తిరిగి ఎక్కించు_elf_hdr(filename, ఖాళీ, ఖాళీ, ఖాళీ);
}

స్థిర శూన్య spike_board_init(MachineState *machine)
{
    స్థిరరాశి Mem_Map_నమోదు *memmap = spike_memmap;
    SpikeState *s = SPIKE_MACHINE(machine);
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    జ్ఞాపకం_Region *ముసుగు_rom = g_new(జ్ఞాపకం_Region, 1);
    hwaddr firmware_చివర_addr = memmap[SPIKE_DRAM].పునాది;
    hwaddr firmware_ఎక్కించు_addr = memmap[SPIKE_DRAM].పునాది;
    vaddr kernel_ప్రారంభం_addr;
    అక్షరం *firmware_name;
    uint64_t fdt_ఎక్కించు_addr;
    uint64_t kernel_నమోదు;
    అక్షరం *soc_name;
    పూర్ణాంకం i, పునాది_hartid, hart_లెక్క;
    bool htif_custom_పునాది = false;
    RISCVBootInfo boot_info;

    /* Check socket count limit */
    ఒకవేళ (SPIKE_SOCKETS_MAX < riscv_socket_లెక్క(machine)) {
        error_report("number of sockets/nodes should be less than %d",
            SPIKE_SOCKETS_MAX);
        నిష్క్రమించు(1);
    }

    /* Initialize sockets */
    కొరకు (i = 0; i < riscv_socket_లెక్క(machine); i++) {
        ఒకవేళ (!riscv_socket_check_hartids(machine, i)) {
            error_report("discontinuous hartids in socket%d", i);
            నిష్క్రమించు(1);
        }

        పునాది_hartid = riscv_socket_మొదటి_hartid(machine, i);
        ఒకవేళ (పునాది_hartid < 0) {
            error_report("can't find hartid base for socket%d", i);
            నిష్క్రమించు(1);
        }

        hart_లెక్క = riscv_socket_hart_లెక్క(machine, i);
        ఒకవేళ (hart_లెక్క < 0) {
            error_report("can't find hart count for socket%d", i);
            నిష్క్రమించు(1);
        }

        soc_name = g_strdup_printf("soc%d", i);
        object_initialize_బిడ్డ(OBJECT(machine), soc_name, &s->soc[i],
                                TYPE_RISCV_HART_ARRAY);
        g_free(soc_name);
        object_property_సమితి_str(OBJECT(&s->soc[i]), "cpu-type",
                                machine->cpu_type, &error_abort);
        object_property_సమితి_int(OBJECT(&s->soc[i]), "hartid-base",
                                పునాది_hartid, &error_abort);
        object_property_సమితి_int(OBJECT(&s->soc[i]), "num-harts",
                                hart_లెక్క, &error_abort);
        sysbus_realize(SYS_BUS_DEVICE(&s->soc[i]), &error_fatal);

        /* Core Local Interruptor (timer and IPI) for each socket */
        riscv_aclint_swi_create(system_జ్ఞాపకం,
            memmap[SPIKE_CLINT].పునాది + i * memmap[SPIKE_CLINT].కొలత,
            పునాది_hartid, hart_లెక్క, false);
        riscv_aclint_mtimer_create(system_జ్ఞాపకం,
            memmap[SPIKE_CLINT].పునాది + i * memmap[SPIKE_CLINT].కొలత +
                RISCV_ACLINT_SWI_కొలత,
            RISCV_ACLINT_DEFAULT_MTIMER_కొలత, పునాది_hartid, hart_లెక్క,
            RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
            RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);
    }

    /* register system main memory (actual RAM) */
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, memmap[SPIKE_DRAM].పునాది,
        machine->ram);

    /* boot rom */
    జ్ఞాపకం_region_init_rom(ముసుగు_rom, ఖాళీ, "riscv.spike.mrom",
                           memmap[SPIKE_MROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం, memmap[SPIKE_MROM].పునాది,
                                ముసుగు_rom);

    /* Find firmware */
    firmware_name = riscv_కనుగొను_firmware(machine->firmware,
                        riscv_default_firmware_name(&s->soc[0]));

    /*
     * Test the given firmware or kernel file to see if it is an ELF image.
     * If it is an ELF, we assume it contains the symbols required for
     * the HTIF console, otherwise we fall back to use the custom base
     * passed from device tree for the HTIF console.
     */
    ఒకవేళ (!firmware_name && !machine->kernel_filename) {
        htif_custom_పునాది = true;
    } లేకపోతే {
        ఒకవేళ (firmware_name) {
            htif_custom_పునాది = !spike_test_elf_image(firmware_name);
        }
        ఒకవేళ (!htif_custom_పునాది && machine->kernel_filename) {
            htif_custom_పునాది = !spike_test_elf_image(machine->kernel_filename);
        }
    }

    riscv_boot_info_init(&boot_info, &s->soc[0]);

    /* Load firmware */
    ఒకవేళ (firmware_name) {
        firmware_చివర_addr = riscv_ఎక్కించు_firmware(machine, &boot_info,
                                                firmware_name,
                                                &firmware_ఎక్కించు_addr,
                                                htif_చిహ్నం_callback);
        g_free(firmware_name);
    }

    /* Create device tree */
    create_fdt(s, memmap, riscv_is_32bit(&s->soc[0]), htif_custom_పునాది);

    /* Load kernel */
    ఒకవేళ (machine->kernel_filename) {
        kernel_ప్రారంభం_addr = riscv_calc_kernel_ప్రారంభం_addr(&boot_info,
                                                         firmware_చివర_addr);

        riscv_ఎక్కించు_kernel(machine, &boot_info, kernel_ప్రారంభం_addr,
                          true, htif_చిహ్నం_callback);
        kernel_నమోదు = boot_info.image_దిగువ_addr;
    } లేకపోతే {
       /*
        * If dynamic firmware is used, it doesn't know where is the next mode
        * if kernel argument is not set.
        */
        kernel_నమోదు = 0;
    }

    fdt_ఎక్కించు_addr = riscv_compute_fdt_addr(memmap[SPIKE_DRAM].పునాది,
                                           memmap[SPIKE_DRAM].కొలత,
                                           machine, &boot_info);
    riscv_ఎక్కించు_fdt(fdt_ఎక్కించు_addr, machine->fdt);

    /* load the reset vector */
    riscv_అమరిక_rom_reset_vec(machine, &s->soc[0], firmware_ఎక్కించు_addr,
                              memmap[SPIKE_MROM].పునాది,
                              memmap[SPIKE_MROM].కొలత, kernel_నమోదు,
                              fdt_ఎక్కించు_addr);

    /* initialize HTIF using symbols found in load_kernel */
    htif_mm_init(system_జ్ఞాపకం, serial_hd(0), memmap[SPIKE_HTIF].పునాది,
                 htif_custom_పునాది);
}

స్థిర శూన్య spike_సమితి_signature(Object *obj, స్థిరరాశి అక్షరం *val, Error **errp)
{
    sig_file = g_strdup(val);
}

స్థిర శూన్య spike_machine_instance_init(Object *obj)
{
}

స్థిర శూన్య spike_machine_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Spike board";
    mc->init = spike_board_init;
    mc->max_cpus = SPIKE_CPUS_MAX;
    mc->default_cpu_type = TYPE_RISCV_CPU_MAX;
    mc->possible_cpu_arch_ids = riscv_numa_possible_cpu_arch_ids;
    mc->cpu_సూచీ_గమ్యం_instance_props = riscv_numa_cpu_సూచీ_గమ్యం_props;
    mc->get_default_cpu_కణుపు_id = riscv_numa_get_default_cpu_కణుపు_id;
    mc->numa_mem_supported = true;
    /* platform instead of architectural choice */
    mc->cpu_cluster_has_numa_boundary = true;
    mc->default_ram_id = "riscv.spike.ram";
    object_class_property_కూడు_str(oc, "signature", ఖాళీ, spike_సమితి_signature);
    object_class_property_సమితి_description(oc, "signature",
                                          "File to write ACT test signature");
    object_class_static_property_కూడు_uint8_ptr(oc, "signature-granularity",
                                               &పంక్తి_కొలత,
                                               OBJ_PROP_FLAG_WRITE);
    object_class_property_సమితి_description(oc, "signature-granularity",
                                          "Size of each line in ACT signature "
                                          "file");
}

స్థిర స్థిరరాశి TypeInfo spike_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("spike"),
    .మాతృ     = TYPE_MACHINE,
    .class_init = spike_machine_class_init,
    .instance_init = spike_machine_instance_init,
    .instance_కొలత = పరిమాణం(SpikeState),
};

స్థిర శూన్య spike_machine_init_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&spike_machine_typeinfo);
}

type_init(spike_machine_init_రిజిస్టరు_types)
