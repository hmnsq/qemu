/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Milk-V Duo board
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#ଅନ୍ତର୍ଭାଵ "qemu/osdep.h"
#ଅନ୍ତର୍ଭାଵ "qemu/units.h"
#ଅନ୍ତର୍ଭାଵ "qapi/error.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/boards.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/cv1800b.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/boot.h"
#ଅନ୍ତର୍ଭାଵ "target/riscv/cpu-qom.h"
#ଅନ୍ତର୍ଭାଵ "system/system.h"
#ଅନ୍ତର୍ଭାଵ "system/device_tree.h"
#ଅନ୍ତର୍ଭାଵ "qemu/error-report.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/loader.h"
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

ସଂରଚନା MilkVDuoState {
    MachineState parent_obj;
    CV1800BSoCState soc;
};

#define TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

ସ୍ଥିର ଶୂନ୍ୟ milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    MemoryRegion *system_memory = get_system_memory();
    RISCVBootInfo boot_info;
    ଅଚଳ ଅକ୍ଷର *firmware_name;
    hwaddr firmware_load_addr, firmware_end_addr;
    hwaddr fdt_load_addr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_size = 0;
    uint64_t kernel_entry = 0;

    object_initialize_child(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), ଖାଲି, &error_fatal);

    memory_region_add_subregion(system_memory,
                                cv1800b_memmap[CV1800B_DEV_DRAM].base,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_load_addr = cv1800b_memmap[CV1800B_DEV_DRAM].base;
    firmware_end_addr = riscv_find_and_load_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_load_addr, ଖାଲି);

    ଯଦି (machine->dtb) {
        machine->fdt = load_device_ବୃକ୍ଷ(machine->dtb, &fdt_size);
        ଯଦି (!machine->fdt) {
            error_report("Failed to load device tree");
            exit(1);
        }

        ଯଦି (machine->kernel_cmdline && *machine->kernel_cmdline) {
            ଯଦି (fdt_path_offset(machine->fdt, "/chosen") < 0) {
                qemu_fdt_add_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    ଯଦି (machine->kernel_filename) {
        hwaddr kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                                firmware_end_addr);
        riscv_load_kernel(machine, &boot_info, kernel_start_addr, true, ଖାଲି);
        kernel_entry = boot_info.image_ତଳ_addr;
    }

    ଯଦି (machine->dtb) {
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

ସ୍ଥିର ଶୂନ୍ୟ milkv_duo_machine_class_init(ObjectClass *oc, ଅଚଳ ଶୂନ୍ୟ *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ valid_cpu_types[] = {
        TYPE_RISCV_CPU_THEAD_C906,
        ଖାଲି
    };

    mc->desc = "Milk-V Duo Board (CV1800B)";
    mc->init = milkv_duo_init;
    mc->max_cpus = 2;
    mc->default_cpu_type = TYPE_RISCV_CPU_THEAD_C906;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_size = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

ସ୍ଥିର ଅଚଳ TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .parent = TYPE_MACHINE,
        .instance_size = ଆକାର(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
