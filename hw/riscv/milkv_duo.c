/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Milk-V Duo board
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#شامل "qemu/osdep.h"
#شامل "qemu/units.h"
#شامل "qapi/error.h"
#شامل "hw/core/boards.h"
#شامل "hw/riscv/cv1800b.h"
#شامل "hw/riscv/boot.h"
#شامل "target/riscv/cpu-qom.h"
#شامل "system/system.h"
#شامل "system/device_tree.h"
#شامل "qemu/error-report.h"
#شامل "hw/core/loader.h"
#شامل <libfdt.h>

ساخت MilkVDuoState {
    MachineState والد_obj;
    CV1800BSoCState soc;
};

#تعریف_کرو TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

ساکن خالی milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    حافظہ_Region *system_حافظہ = get_system_حافظہ();
    RISCVBootInfo boot_info;
    ثابت حرف *firmware_name;
    hwaddr firmware_بھرو_addr, firmware_انتہا_addr;
    hwaddr fdt_بھرو_addr = 0;
    صحیح fdt_سائز = 0;
    uint64_t kernel_اندراج = 0;

    object_initialize_بچہ(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), تہی, &error_fatal);

    حافظہ_region_جوڑو_subregion(system_حافظہ,
                                cv1800b_memmap[CV1800B_DEV_DRAM].بنیاد,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_بھرو_addr = cv1800b_memmap[CV1800B_DEV_DRAM].بنیاد;
    firmware_انتہا_addr = riscv_ڈھونڈو_and_بھرو_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_بھرو_addr, تہی);

    اگر (machine->dtb) {
        machine->fdt = بھرو_device_درخت(machine->dtb, &fdt_سائز);
        اگر (!machine->fdt) {
            error_report("Failed to load device tree");
            باہر_نکلو(1);
        }

        اگر (machine->kernel_cmdline && *machine->kernel_cmdline) {
            اگر (fdt_راستہ_انحراف(machine->fdt, "/chosen") < 0) {
                qemu_fdt_جوڑو_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    اگر (machine->kernel_filename) {
        hwaddr kernel_آغاز_addr = riscv_calc_kernel_آغاز_addr(&boot_info,
                                                                firmware_انتہا_addr);
        riscv_بھرو_kernel(machine, &boot_info, kernel_آغاز_addr, true, تہی);
        kernel_اندراج = boot_info.image_کم_addr;
    }

    اگر (machine->dtb) {
        fdt_بھرو_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].بنیاد,
                                               machine->ram_سائز, machine, &boot_info);
        riscv_بھرو_fdt(fdt_بھرو_addr, machine->fdt);
    }

    riscv_تیاری_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_بھرو_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].بنیاد,
                              cv1800b_memmap[CV1800B_DEV_ROM].سائز,
                              kernel_اندراج,
                              fdt_بھرو_addr);
}

ساکن خالی milkv_duo_machine_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    ساکن ثابت حرف *ثابت valid_cpu_types[] = {
        TYPE_RISCV_CPU_THEAD_C906,
        تہی
    };

    mc->desc = "Milk-V Duo Board (CV1800B)";
    mc->init = milkv_duo_init;
    mc->max_cpus = 2;
    mc->default_cpu_type = TYPE_RISCV_CPU_THEAD_C906;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_سائز = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

ساکن ثابت TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .والد = TYPE_MACHINE,
        .instance_سائز = حجم(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
