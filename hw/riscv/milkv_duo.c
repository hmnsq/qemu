/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Milk-V Duo board
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#ضمن "qemu/osdep.h"
#ضمن "qemu/units.h"
#ضمن "qapi/error.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/riscv/cv1800b.h"
#ضمن "hw/riscv/boot.h"
#ضمن "target/riscv/cpu-qom.h"
#ضمن "system/system.h"
#ضمن "system/device_tree.h"
#ضمن "qemu/error-report.h"
#ضمن "hw/core/loader.h"
#ضمن <libfdt.h>

بنية MilkVDuoState {
    MachineState والد_obj;
    CV1800BSoCState soc;
};

#وصف_ڪريو TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

ساكن فراغ milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    يادگيري_Region *system_يادگيري = get_system_يادگيري();
    RISCVBootInfo boot_info;
    ثابت حرف *firmware_name;
    hwaddr firmware_ڀريو_addr, firmware_پڇاڙي_addr;
    hwaddr fdt_ڀريو_addr = 0;
    صحيح fdt_سائيز = 0;
    uint64_t kernel_داخلا = 0;

    object_initialize_ٻار(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                cv1800b_memmap[CV1800B_DEV_DRAM].بنياد,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_ڀريو_addr = cv1800b_memmap[CV1800B_DEV_DRAM].بنياد;
    firmware_پڇاڙي_addr = riscv_ڳوليو_and_ڀريو_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_ڀريو_addr, عدم);

    إذا (machine->dtb) {
        machine->fdt = ڀريو_device_وڻ(machine->dtb, &fdt_سائيز);
        إذا (!machine->fdt) {
            error_report("Failed to load device tree");
            ٻاهر_نڪرو(1);
        }

        إذا (machine->kernel_cmdline && *machine->kernel_cmdline) {
            إذا (fdt_رستو_وٿي(machine->fdt, "/chosen") < 0) {
                qemu_fdt_جوڙيو_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    إذا (machine->kernel_filename) {
        hwaddr kernel_شروعات_addr = riscv_calc_kernel_شروعات_addr(&boot_info,
                                                                firmware_پڇاڙي_addr);
        riscv_ڀريو_kernel(machine, &boot_info, kernel_شروعات_addr, true, عدم);
        kernel_داخلا = boot_info.image_هيٺيون_addr;
    }

    إذا (machine->dtb) {
        fdt_ڀريو_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].بنياد,
                                               machine->ram_سائيز, machine, &boot_info);
        riscv_ڀريو_fdt(fdt_ڀريو_addr, machine->fdt);
    }

    riscv_تياري_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_ڀريو_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].بنياد,
                              cv1800b_memmap[CV1800B_DEV_ROM].سائيز,
                              kernel_داخلا,
                              fdt_ڀريو_addr);
}

ساكن فراغ milkv_duo_machine_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    ساكن ثابت حرف *ثابت valid_cpu_types[] = {
        TYPE_RISCV_CPU_THEAD_C906,
        عدم
    };

    mc->desc = "Milk-V Duo Board (CV1800B)";
    mc->init = milkv_duo_init;
    mc->max_cpus = 2;
    mc->default_cpu_type = TYPE_RISCV_CPU_THEAD_C906;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_سائيز = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

ساكن ثابت TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .والد = TYPE_MACHINE,
        .instance_سائيز = حجم(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
