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

#تعریف_کن TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

ساكن فراغ milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    حافظه_Region *system_حافظه = get_system_حافظه();
    RISCVBootInfo boot_info;
    ثابت حرف *firmware_name;
    hwaddr firmware_بارگیری_addr, firmware_انتها_addr;
    hwaddr fdt_بارگیری_addr = 0;
    صحيح fdt_اندازه = 0;
    uint64_t kernel_مدخل = 0;

    object_initialize_فرزند(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    حافظه_region_جمع_کن_subregion(system_حافظه,
                                cv1800b_memmap[CV1800B_DEV_DRAM].پایه,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_بارگیری_addr = cv1800b_memmap[CV1800B_DEV_DRAM].پایه;
    firmware_انتها_addr = riscv_پیدا_کن_and_بارگیری_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_بارگیری_addr, عدم);

    إذا (machine->dtb) {
        machine->fdt = بارگیری_device_درخت(machine->dtb, &fdt_اندازه);
        إذا (!machine->fdt) {
            error_report("Failed to load device tree");
            خروج(1);
        }

        إذا (machine->kernel_cmdline && *machine->kernel_cmdline) {
            إذا (fdt_مسیر_جابه_جایی(machine->fdt, "/chosen") < 0) {
                qemu_fdt_جمع_کن_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    إذا (machine->kernel_filename) {
        hwaddr kernel_شروع_addr = riscv_calc_kernel_شروع_addr(&boot_info,
                                                                firmware_انتها_addr);
        riscv_بارگیری_kernel(machine, &boot_info, kernel_شروع_addr, true, عدم);
        kernel_مدخل = boot_info.image_کم_addr;
    }

    إذا (machine->dtb) {
        fdt_بارگیری_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].پایه,
                                               machine->ram_اندازه, machine, &boot_info);
        riscv_بارگیری_fdt(fdt_بارگیری_addr, machine->fdt);
    }

    riscv_راهاندازی_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_بارگیری_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].پایه,
                              cv1800b_memmap[CV1800B_DEV_ROM].اندازه,
                              kernel_مدخل,
                              fdt_بارگیری_addr);
}

ساكن فراغ milkv_duo_machine_class_init(ObjectClass *oc, ثابت فراغ *داده)
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
        .والد = TYPE_MACHINE,
        .instance_اندازه = حجم(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
