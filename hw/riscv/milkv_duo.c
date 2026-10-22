/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Milk-V Duo board
 *
 * Copyright (c) 2026 Kuan-Wei Chiu <visitorckw@gmail.com>
 */

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/units.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/riscv/cv1800b.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "target/riscv/cpu-qom.h"
#అన్తర్భావ "system/system.h"
#అన్తర్భావ "system/device_tree.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ <libfdt.h>

నిర్మాణం MilkVDuoState {
    MachineState మాతృ_obj;
    CV1800BSoCState soc;
};

#నిర్వచించు TYPE_MILK_V_DUO MACHINE_TYPE_NAME("milkv-duo")
OBJECT_DECLARE_SIMPLE_TYPE(MilkVDuoState, MILK_V_DUO)

స్థిర శూన్య milkv_duo_init(MachineState *machine)
{
    MilkVDuoState *s = MILK_V_DUO(machine);
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    RISCVBootInfo boot_info;
    స్థిరరాశి అక్షరం *firmware_name;
    hwaddr firmware_ఎక్కించు_addr, firmware_చివర_addr;
    hwaddr fdt_ఎక్కించు_addr = 0;
    పూర్ణాంకం fdt_కొలత = 0;
    uint64_t kernel_నమోదు = 0;

    object_initialize_బిడ్డ(OBJECT(machine), "soc", &s->soc, TYPE_CV1800B_SOC);
    qdev_realize(DEVICE(&s->soc), ఖాళీ, &error_fatal);

    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                cv1800b_memmap[CV1800B_DEV_DRAM].పునాది,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    firmware_name = riscv_default_firmware_name(&s->soc.cpus);
    firmware_ఎక్కించు_addr = cv1800b_memmap[CV1800B_DEV_DRAM].పునాది;
    firmware_చివర_addr = riscv_కనుగొను_and_ఎక్కించు_firmware(machine, &boot_info,
                                                     firmware_name,
                                                     &firmware_ఎక్కించు_addr, ఖాళీ);

    ఒకవేళ (machine->dtb) {
        machine->fdt = ఎక్కించు_device_వృక్షం(machine->dtb, &fdt_కొలత);
        ఒకవేళ (!machine->fdt) {
            error_report("Failed to load device tree");
            నిష్క్రమించు(1);
        }

        ఒకవేళ (machine->kernel_cmdline && *machine->kernel_cmdline) {
            ఒకవేళ (fdt_మార్గం_స్థానభ్రంశం(machine->fdt, "/chosen") < 0) {
                qemu_fdt_కూడు_subnode(machine->fdt, "/chosen");
            }
            qemu_fdt_setprop_string(machine->fdt, "/chosen", "bootargs",
                                    machine->kernel_cmdline);
        }
    }

    ఒకవేళ (machine->kernel_filename) {
        hwaddr kernel_ప్రారంభం_addr = riscv_calc_kernel_ప్రారంభం_addr(&boot_info,
                                                                firmware_చివర_addr);
        riscv_ఎక్కించు_kernel(machine, &boot_info, kernel_ప్రారంభం_addr, true, ఖాళీ);
        kernel_నమోదు = boot_info.image_దిగువ_addr;
    }

    ఒకవేళ (machine->dtb) {
        fdt_ఎక్కించు_addr = riscv_compute_fdt_addr(cv1800b_memmap[CV1800B_DEV_DRAM].పునాది,
                                               machine->ram_కొలత, machine, &boot_info);
        riscv_ఎక్కించు_fdt(fdt_ఎక్కించు_addr, machine->fdt);
    }

    riscv_అమరిక_rom_reset_vec(machine, &s->soc.cpus,
                              firmware_ఎక్కించు_addr,
                              cv1800b_memmap[CV1800B_DEV_ROM].పునాది,
                              cv1800b_memmap[CV1800B_DEV_ROM].కొలత,
                              kernel_నమోదు,
                              fdt_ఎక్కించు_addr);
}

స్థిర శూన్య milkv_duo_machine_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి valid_cpu_types[] = {
        TYPE_RISCV_CPU_THEAD_C906,
        ఖాళీ
    };

    mc->desc = "Milk-V Duo Board (CV1800B)";
    mc->init = milkv_duo_init;
    mc->max_cpus = 2;
    mc->default_cpu_type = TYPE_RISCV_CPU_THEAD_C906;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_కొలత = 64 * MiB;
    mc->default_ram_id = "riscv.milkv_duo.ram";
}

స్థిర స్థిరరాశి TypeInfo milkv_duo_machine_type_infos[] = {
    {
        .name = TYPE_MILK_V_DUO,
        .మాతృ = TYPE_MACHINE,
        .instance_కొలత = పరిమాణం(MilkVDuoState),
        .class_init = milkv_duo_machine_class_init,
    },
};

DEFINE_TYPES(milkv_duo_machine_type_infos)
