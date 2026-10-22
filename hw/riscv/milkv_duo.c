/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Milk-V Duo board
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/cv1800b.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu-qom.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "system/device_tree.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ <libfdt.h>

ರಚನೆ MilkVDuoState {
    MachineState ಪೋಷಕ_obj;
    CV1800BSoCState soc;
};

#ವ್ಯಾಖ್ಯಾನಿಸು TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

ಸ್ಥಿರ ಶೂನ್ಯ milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    RISCVBootInfo boot_info;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *firmware_name;
    hwaddr firmware_ತುಂಬು_addr, firmware_ಕೊನೆ_addr;
    hwaddr fdt_ತುಂಬು_addr = 0;
    ಪೂರ್ಣಾಂಕ fdt_ಗಾತ್ರ = 0;
    uint64_t kernel_ನಮೂದು = 0;

    object_initialize_ಮಗು(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), ಖಾಲಿ, &error_fatal);

    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                cv1800b_memmap[CV1800B_DEV_DRAM].ತಳ,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_ತುಂಬು_addr = cv1800b_memmap[CV1800B_DEV_DRAM].ತಳ;
    firmware_ಕೊನೆ_addr = riscv_ಕಂಡುಹಿಡಿ_and_ತುಂಬು_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_ತುಂಬು_addr, ಖಾಲಿ);

    ಆದರೆ (machine->dtb) {
        machine->fdt = ತುಂಬು_device_ವೃಕ್ಷ(machine->dtb, &fdt_ಗಾತ್ರ);
        ಆದರೆ (!machine->fdt) {
            error_report("Failed to load device tree");
            ನಿರ್ಗಮಿಸು(1);
        }

        ಆದರೆ (machine->kernel_cmdline && *machine->kernel_cmdline) {
            ಆದರೆ (fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(machine->fdt, "/chosen") < 0) {
                qemu_fdt_ಕೂಡಿಸು_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    ಆದರೆ (machine->kernel_filename) {
        hwaddr kernel_ಆರಂಭ_addr = riscv_calc_kernel_ಆರಂಭ_addr(&boot_info,
                                                                firmware_ಕೊನೆ_addr);
        riscv_ತುಂಬು_kernel(machine, &boot_info, kernel_ಆರಂಭ_addr, true, ಖಾಲಿ);
        kernel_ನಮೂದು = boot_info.image_ಕೆಳಗಿನ_addr;
    }

    ಆದರೆ (machine->dtb) {
        fdt_ತುಂಬು_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].ತಳ,
                                               machine->ram_ಗಾತ್ರ, machine, &boot_info);
        riscv_ತುಂಬು_fdt(fdt_ತುಂಬು_addr, machine->fdt);
    }

    riscv_ಸಿದ್ಧತೆ_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_ತುಂಬು_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].ತಳ,
                              cv1800b_memmap[CV1800B_DEV_ROM].ಗಾತ್ರ_2,
                              kernel_ನಮೂದು,
                              fdt_ತುಂಬು_addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ milkv_duo_machine_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ valid_cpu_types[] = {
        TYPE_RISCV_CPU_THEAD_C906,
        ಖಾಲಿ
    };

    mc->desc = "Milk-V Duo Board (CV1800B)";
    mc->init = milkv_duo_init;
    mc->max_cpus = 2;
    mc->default_cpu_type = TYPE_RISCV_CPU_THEAD_C906;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_ಗಾತ್ರ = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .ಪೋಷಕ = TYPE_MACHINE,
        .instance_ಗಾತ್ರ = ಗಾತ್ರ(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
