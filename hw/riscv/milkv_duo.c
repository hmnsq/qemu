/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Milk-V Duo board
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#অন্তর্ভাৱ "qemu/osdep.h"
#অন্তর্ভাৱ "qemu/units.h"
#অন্তর্ভাৱ "qapi/error.h"
#অন্তর্ভাৱ "hw/core/boards.h"
#অন্তর্ভাৱ "hw/riscv/cv1800b.h"
#অন্তর্ভাৱ "hw/riscv/boot.h"
#অন্তর্ভাৱ "target/riscv/cpu-qom.h"
#অন্তর্ভাৱ "system/system.h"
#অন্তর্ভাৱ "system/device_tree.h"
#অন্তর্ভাৱ "qemu/error-report.h"
#অন্তর্ভাৱ "hw/core/loader.h"
#অন্তর্ভাৱ <libfdt.h>

সংরচনা MilkVDuoState {
    MachineState parent_obj;
    CV1800BSoCState soc;
};

#define TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

স্থির শূন্য milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    MemoryRegion *system_memory = get_system_memory();
    RISCVBootInfo boot_info;
    ধ্রুৱ ৱর্ণ *firmware_name;
    hwaddr firmware_load_addr, firmware_end_addr;
    hwaddr fdt_load_addr = 0;
    পূর্ণাঙ্ক fdt_size = 0;
    uint64_t kernel_entry = 0;

    object_initialize_child(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), রিক্ত, &error_fatal);

    memory_region_add_subregion(system_memory,
                                cv1800b_memmap[CV1800B_DEV_DRAM].base,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_load_addr = cv1800b_memmap[CV1800B_DEV_DRAM].base;
    firmware_end_addr = riscv_find_and_load_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_load_addr, রিক্ত);

    যদি (machine->dtb) {
        machine->fdt = load_device_বৃক্ষ(machine->dtb, &fdt_size);
        যদি (!machine->fdt) {
            error_report("Failed to load device tree");
            exit(1);
        }

        যদি (machine->kernel_cmdline && *machine->kernel_cmdline) {
            যদি (fdt_path_offset(machine->fdt, "/chosen") < 0) {
                qemu_fdt_add_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    যদি (machine->kernel_filename) {
        hwaddr kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                                firmware_end_addr);
        riscv_load_kernel(machine, &boot_info, kernel_start_addr, true, রিক্ত);
        kernel_entry = boot_info.image_নিচেৰ_addr;
    }

    যদি (machine->dtb) {
        fdt_load_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].base,
                                               machine->ram_size, machine, &boot_info);
        riscv_load_fdt(fdt_load_addr, machine->fdt);
    }

    riscv_setup_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_load_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].base,
                              cv1800b_memmap[CV1800B_DEV_ROM].size,
                              kernel_entry,
                              fdt_load_addr);
}

স্থির শূন্য milkv_duo_machine_class_init(ObjectClass *oc, ধ্রুৱ শূন্য *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    স্থির ধ্রুৱ ৱর্ণ *ধ্রুৱ valid_cpu_types[] = {
        TYPE_RISCV_CPU_THEAD_C906,
        রিক্ত
    };

    mc->desc = "Milk-V Duo Board (CV1800B)";
    mc->init = milkv_duo_init;
    mc->max_cpus = 2;
    mc->default_cpu_type = TYPE_RISCV_CPU_THEAD_C906;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_size = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

স্থির ধ্রুৱ TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .parent = TYPE_MACHINE,
        .instance_size = পরিমাণ(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
