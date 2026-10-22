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
    MachineState پلار_obj;
    CV1800BSoCState soc;
};

#تعریف_کړه TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

ساكن فراغ milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    حافظه_Region *system_حافظه = get_system_حافظه();
    RISCVBootInfo boot_info;
    ثابت حرف *firmware_name;
    hwaddr firmware_بارول_addr, firmware_پای_ټکی_addr;
    hwaddr fdt_بارول_addr = 0;
    صحيح fdt_اندازه = 0;
    uint64_t kernel_ثبت = 0;

    object_initialize_ماشوم(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    حافظه_region_جمع_کړه_subregion(system_حافظه,
                                cv1800b_memmap[CV1800B_DEV_DRAM].بنسټ,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_بارول_addr = cv1800b_memmap[CV1800B_DEV_DRAM].بنسټ;
    firmware_پای_ټکی_addr = riscv_پیدا_کړه_and_بارول_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_بارول_addr, عدم);

    إذا (machine->dtb) {
        machine->fdt = بارول_device_ونه(machine->dtb, &fdt_اندازه);
        إذا (!machine->fdt) {
            error_report("Failed to load device tree");
            وتل(1);
        }

        إذا (machine->kernel_cmdline && *machine->kernel_cmdline) {
            إذا (fdt_لار_دننه_واټن(machine->fdt, "/chosen") < 0) {
                qemu_fdt_جمع_کړه_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    إذا (machine->kernel_filename) {
        hwaddr kernel_پیل_addr = riscv_calc_kernel_پیل_addr(&boot_info,
                                                                firmware_پای_ټکی_addr);
        riscv_بارول_kernel(machine, &boot_info, kernel_پیل_addr, true, عدم);
        kernel_ثبت = boot_info.image_ټیټ_addr;
    }

    إذا (machine->dtb) {
        fdt_بارول_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].بنسټ,
                                               machine->ram_اندازه, machine, &boot_info);
        riscv_بارول_fdt(fdt_بارول_addr, machine->fdt);
    }

    riscv_تیاری_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_بارول_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].بنسټ,
                              cv1800b_memmap[CV1800B_DEV_ROM].اندازه,
                              kernel_ثبت,
                              fdt_بارول_addr);
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
    mc->default_ram_اندازه = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

ساكن ثابت TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .پلار = TYPE_MACHINE,
        .instance_اندازه = حجم(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
