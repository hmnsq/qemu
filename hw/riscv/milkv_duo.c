/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Milk-V Duo board
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/units.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/cv1800b.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "target/riscv/cpu-qom.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/device_tree.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/loader.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

ਬਣਤਰ MilkVDuoState {
    MachineState ਮਾਪਾ_obj;
    CV1800BSoCState soc;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

ਸਥਿਰ ਖਾਲੀ milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    ਸਮ੍ਰਿਤੀ_Region *system_ਸਮ੍ਰਿਤੀ = get_system_ਸਮ੍ਰਿਤੀ();
    RISCVBootInfo boot_info;
    ਅਚਲ ਅੱਖਰ *firmware_name;
    hwaddr firmware_ਲੱਦੋ_addr, firmware_ਸਿਰਾ_addr;
    hwaddr fdt_ਲੱਦੋ_addr = 0;
    ਪੂਰਨ fdt_ਮਾਪ = 0;
    uint64_t kernel_ਇੰਦਰਾਜ = 0;

    object_initialize_ਬੱਚਾ(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), ਨਿਰਾ, &error_fatal);

    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ,
                                cv1800b_memmap[CV1800B_DEV_DRAM].ਅਧਾਰ,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_ਲੱਦੋ_addr = cv1800b_memmap[CV1800B_DEV_DRAM].ਅਧਾਰ;
    firmware_ਸਿਰਾ_addr = riscv_ਲੱਭੋ_and_ਲੱਦੋ_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_ਲੱਦੋ_addr, ਨਿਰਾ);

    ਜੇ (machine->dtb) {
        machine->fdt = ਲੱਦੋ_device_ਰੁੱਖ(machine->dtb, &fdt_ਮਾਪ);
        ਜੇ (!machine->fdt) {
            error_report("Failed to load device tree");
            ਬਾਹਰ_ਨਿਕਲੋ(1);
        }

        ਜੇ (machine->kernel_cmdline && *machine->kernel_cmdline) {
            ਜੇ (fdt_ਰਸਤਾ_ਵਿੱਥ(machine->fdt, "/chosen") < 0) {
                qemu_fdt_ਜੋੜੋ_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    ਜੇ (machine->kernel_filename) {
        hwaddr kernel_ਸ਼ੁਰੂ_addr = riscv_calc_kernel_ਸ਼ੁਰੂ_addr(&boot_info,
                                                                firmware_ਸਿਰਾ_addr);
        riscv_ਲੱਦੋ_kernel(machine, &boot_info, kernel_ਸ਼ੁਰੂ_addr, true, ਨਿਰਾ);
        kernel_ਇੰਦਰਾਜ = boot_info.image_ਨੀਵਾਂ_addr;
    }

    ਜੇ (machine->dtb) {
        fdt_ਲੱਦੋ_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].ਅਧਾਰ,
                                               machine->ram_ਮਾਪ, machine, &boot_info);
        riscv_ਲੱਦੋ_fdt(fdt_ਲੱਦੋ_addr, machine->fdt);
    }

    riscv_ਤਿਆਰੀ_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_ਲੱਦੋ_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].ਅਧਾਰ,
                              cv1800b_memmap[CV1800B_DEV_ROM].ਮਾਪ,
                              kernel_ਇੰਦਰਾਜ,
                              fdt_ਲੱਦੋ_addr);
}

ਸਥਿਰ ਖਾਲੀ milkv_duo_machine_class_init(ObjectClass *oc, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ valid_cpu_types[] = {
        TYPE_RISCV_CPU_THEAD_C906,
        ਨਿਰਾ
    };

    mc->desc = "Milk-V Duo Board (CV1800B)";
    mc->init = milkv_duo_init;
    mc->max_cpus = 2;
    mc->default_cpu_type = TYPE_RISCV_CPU_THEAD_C906;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_ਮਾਪ = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

ਸਥਿਰ ਅਚਲ TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .ਮਾਪਾ = TYPE_MACHINE,
        .instance_ਮਾਪ = ਆਕਾਰ(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
