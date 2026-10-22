/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Milk-V Duo board
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qemu/units.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/riscv/cv1800b.h"
#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "target/riscv/cpu-qom.h"
#अन्तर्भाव "system/system.h"
#अन्तर्भाव "system/device_tree.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "hw/core/loader.h"
#अन्तर्भाव <libfdt.h>

संरचना MilkVDuoState {
    MachineState parent_obj;
    CV1800BSoCState soc;
};

#परिभाषय TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

स्थिर शून्य milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    स्मृतिः_Region *system_स्मृतिः = get_system_स्मृतिः();
    RISCVBootInfo boot_info;
    ध्रुव वर्ण *firmware_name;
    hwaddr firmware_आरोपय_addr, firmware_अन्तः_addr;
    hwaddr fdt_आरोपय_addr = 0;
    पूर्णाङ्क fdt_आकार = 0;
    uint64_t kernel_प्रविष्टिः = 0;

    object_initialize_child(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), रिक्त, &error_fatal);

    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                cv1800b_memmap[CV1800B_DEV_DRAM].आधारः,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_आरोपय_addr = cv1800b_memmap[CV1800B_DEV_DRAM].आधारः;
    firmware_अन्तः_addr = riscv_find_and_आरोपय_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_आरोपय_addr, रिक्त);

    यदि (machine->dtb) {
        machine->fdt = आरोपय_device_वृक्षः(machine->dtb, &fdt_आकार);
        यदि (!machine->fdt) {
            error_report("Failed to load device tree");
            निर्गच्छ(1);
        }

        यदि (machine->kernel_cmdline && *machine->kernel_cmdline) {
            यदि (fdt_मार्गः_विस्थापनम्(machine->fdt, "/chosen") < 0) {
                qemu_fdt_सङ्कलय_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    यदि (machine->kernel_filename) {
        hwaddr kernel_start_addr = riscv_calc_kernel_start_addr(&boot_info,
                                                                firmware_अन्तः_addr);
        riscv_आरोपय_kernel(machine, &boot_info, kernel_start_addr, true, रिक्त);
        kernel_प्रविष्टिः = boot_info.image_नीचः_addr;
    }

    यदि (machine->dtb) {
        fdt_आरोपय_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].आधारः,
                                               machine->ram_आकार, machine, &boot_info);
        riscv_आरोपय_fdt(fdt_आरोपय_addr, machine->fdt);
    }

    riscv_सज्जता_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_आरोपय_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].आधारः,
                              cv1800b_memmap[CV1800B_DEV_ROM].आकार,
                              kernel_प्रविष्टिः,
                              fdt_आरोपय_addr);
}

स्थिर शून्य milkv_duo_machine_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    स्थिर ध्रुव वर्ण *ध्रुव valid_cpu_types[] = {
        TYPE_RISCV_CPU_THEAD_C906,
        रिक्त
    };

    mc->desc = "Milk-V Duo Board (CV1800B)";
    mc->init = milkv_duo_init;
    mc->max_cpus = 2;
    mc->default_cpu_type = TYPE_RISCV_CPU_THEAD_C906;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_आकार = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

स्थिर ध्रुव TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .parent = TYPE_MACHINE,
        .instance_आकार = परिमाण(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
