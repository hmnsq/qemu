/*
 * QEMU RISC-V Board Compatible with the Xiangshan Kunminghu
 * FPGA prototype platform
 *
 * Copyright (c) 2025 Beijing Institute of Open Source Chip (BOSC)
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Provides a board compatible with the Xiangshan Kunminghu
 * FPGA prototype platform:
 *
 * 0) UART (16550A)
 * 1) CLINT (Core-Local Interruptor)
 * 2) IMSIC (Incoming MSI Controller)
 * 3) APLIC (Advanced Platform-Level Interrupt Controller)
 *
 * More information can be found in our Github repository:
 * https://github.com/OpenXiangShan/XiangShan
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "system/address-spaces.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/char/serial-mm.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aplic.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_imsic.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/xiangshan_kmh.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು xiangshan_kmh_memmap[] = {
    [XIANGSHAN_KMH_ROM] =          {     0x1000,        0xF000 },
    [XIANGSHAN_KMH_UART0] =        { 0x310B0000,       0x10000 },
    [XIANGSHAN_KMH_CLINT] =        { 0x38000000,       0x10000 },
    [XIANGSHAN_KMH_APLIC_M] =      { 0x31100000,        0x4000 },
    [XIANGSHAN_KMH_APLIC_S] =      { 0x31120000,        0x4000 },
    [XIANGSHAN_KMH_IMSIC_M] =      { 0x3A800000,       0x10000 },
    [XIANGSHAN_KMH_IMSIC_S] =      { 0x3B000000,       0x80000 },
    [XIANGSHAN_KMH_DRAM] =         { 0x80000000,           0x0 },
};

ಸ್ಥಿರ DeviceState *xiangshan_kmh_create_aia(uint32_t ಅಂಕೆ_harts)
{
    ಪೂರ್ಣಾಂಕ i;
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = xiangshan_kmh_memmap;
    hwaddr addr = 0;
    DeviceState *aplic_m = ಖಾಲಿ;

    /* M-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_M].ತಳ;
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_harts; i++) {
        riscv_imsic_create(get_system_ಸ್ಮರಣೆ(),
                           addr + i * IMSIC_HART_ಗಾತ್ರ(0), i, true,
                           1, XIANGSHAN_KMH_IMSIC_ಅಂಕೆ_IDS);
    }

    /* S-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_S].ತಳ;
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_harts; i++) {
        riscv_imsic_create(get_system_ಸ್ಮರಣೆ(), addr +
                           i * IMSIC_HART_ಗಾತ್ರ(XIANGSHAN_KMH_IMSIC_GUEST_ಬಿಟ್ಟುಗಳು),
                           i, false, 1 + XIANGSHAN_KMH_IMSIC_GUEST_ಬಿಟ್ಟುಗಳು,
                           XIANGSHAN_KMH_IMSIC_ಅಂಕೆ_IDS);
    }

    /* M-level APLIC */
    aplic_m = riscv_aplic_create(get_system_ಸ್ಮರಣೆ(),
                                 memmap[XIANGSHAN_KMH_APLIC_M].ತಳ,
                                 memmap[XIANGSHAN_KMH_APLIC_M].ಗಾತ್ರ_2,
                                 0, 0, XIANGSHAN_KMH_APLIC_ಅಂಕೆ_SOURCES,
                                 1, true, true, ಖಾಲಿ);

    /* S-level APLIC */
    riscv_aplic_create(get_system_ಸ್ಮರಣೆ(),
                       memmap[XIANGSHAN_KMH_APLIC_S].ತಳ,
                       memmap[XIANGSHAN_KMH_APLIC_S].ಗಾತ್ರ_2,
                       0, 0, XIANGSHAN_KMH_APLIC_ಅಂಕೆ_SOURCES,
                       1, true, false, aplic_m);

    ಹಿಂತಿರುಗಿ aplic_m;
}

ಸ್ಥಿರ ಶೂನ್ಯ xiangshan_kmh_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(dev);
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = xiangshan_kmh_memmap;
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    uint32_t ಅಂಕೆ_harts = ms->smp.cpus;

    qdev_prop_ಗಣ_uint32(DEVICE(&s->cpus), "num-harts", ಅಂಕೆ_harts);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_ಗಣ_string(DEVICE(&s->cpus), "cpu-type",
                         TYPE_RISCV_CPU_XIANGSHAN_KMH);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* AIA */
    s->irqchip = xiangshan_kmh_create_aia(ಅಂಕೆ_harts);

    /* UART */
    serial_mm_init(system_ಸ್ಮರಣೆ, memmap[XIANGSHAN_KMH_UART0].ತಳ, 2,
                   qdev_get_gpio_ಒಳ(s->irqchip, XIANGSHAN_KMH_UART0_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* CLINT */
    riscv_aclint_swi_create(system_ಸ್ಮರಣೆ,
                            memmap[XIANGSHAN_KMH_CLINT].ತಳ,
                            0, ಅಂಕೆ_harts, false);
    riscv_aclint_mtimer_create(system_ಸ್ಮರಣೆ,
                               memmap[XIANGSHAN_KMH_CLINT].ತಳ +
                               RISCV_ACLINT_SWI_ಗಾತ್ರ,
                               RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ,
                               0, ಅಂಕೆ_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               XIANGSHAN_KMH_CLINT_TIMEBASE_FREQ, true);

    /* ROM */
    ಸ್ಮರಣೆ_region_init_rom(&s->rom, OBJECT(dev), "xiangshan.kunminghu.rom",
                           memmap[XIANGSHAN_KMH_ROM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[XIANGSHAN_KMH_ROM].ತಳ, &s->rom);
}

ಸ್ಥಿರ ಶೂನ್ಯ xiangshan_kmh_soc_class_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = xiangshan_kmh_soc_realize;
    dc->user_creatable = false;
}

ಸ್ಥಿರ ಶೂನ್ಯ xiangshan_kmh_soc_instance_init(Object *obj)
{
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(obj);

    object_initialize_ಮಗು(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo xiangshan_kmh_soc_info = {
    .name = TYPE_XIANGSHAN_KMH_SOC,
    .ಪೋಷಕ = TYPE_DEVICE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(XiangshanKmhSoCState),
    .instance_init = xiangshan_kmh_soc_instance_init,
    .class_init = xiangshan_kmh_soc_class_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ xiangshan_kmh_soc_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&xiangshan_kmh_soc_info);
}
type_init(xiangshan_kmh_soc_ರಿಜಿಸ್ಟರ್_types)

ಸ್ಥಿರ ಶೂನ್ಯ xiangshan_kmh_machine_init(MachineState *machine)
{
    XiangshanKmhState *s = XIANGSHAN_KMH_MACHINE(machine);
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = xiangshan_kmh_memmap;
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    hwaddr ಆರಂಭ_addr = memmap[XIANGSHAN_KMH_DRAM].ತಳ;
    RISCVBootInfo boot_info;

    /* Initialize SoC */
    object_initialize_ಮಗು(OBJECT(machine), "soc", &s->soc,
                            TYPE_XIANGSHAN_KMH_SOC);
    qdev_realize(DEVICE(&s->soc), ಖಾಲಿ, &error_fatal);

    /* Register RAM */
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                memmap[XIANGSHAN_KMH_DRAM].ತಳ,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    /* ROM reset vector */
    riscv_ಸಿದ್ಧತೆ_rom_reset_vec(machine, &s->soc.cpus,
                              ಆರಂಭ_addr,
                              memmap[XIANGSHAN_KMH_ROM].ತಳ,
                              memmap[XIANGSHAN_KMH_ROM].ಗಾತ್ರ_2, 0, 0);
    ಆದರೆ (machine->firmware) {
        riscv_ತುಂಬು_firmware(machine, &boot_info, machine->firmware,
                            &ಆರಂಭ_addr, ಖಾಲಿ);
    }

    /* Note: dtb has been integrated into firmware(OpenSBI) when compiling */
}

ಸ್ಥಿರ ಶೂನ್ಯ xiangshan_kmh_machine_class_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ valid_cpu_types[] = {
        TYPE_RISCV_CPU_XIANGSHAN_KMH,
        ಖಾಲಿ
    };

    mc->desc = "RISC-V Board compatible with the Xiangshan " \
               "Kunminghu FPGA prototype platform";
    mc->init = xiangshan_kmh_machine_init;
    mc->max_cpus = XIANGSHAN_KMH_MAX_CPUS;
    mc->default_cpu_type = TYPE_RISCV_CPU_XIANGSHAN_KMH;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_id = "xiangshan.kunminghu.ram";
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo xiangshan_kmh_machine_info = {
    .name = TYPE_XIANGSHAN_KMH_MACHINE,
    .ಪೋಷಕ = TYPE_MACHINE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(XiangshanKmhState),
    .class_init = xiangshan_kmh_machine_class_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ xiangshan_kmh_machine_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&xiangshan_kmh_machine_info);
}
type_init(xiangshan_kmh_machine_ರಿಜಿಸ್ಟರ್_types)
