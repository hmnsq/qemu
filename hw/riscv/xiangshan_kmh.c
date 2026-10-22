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

#অন্তর্ভাৱ "qemu/osdep.h"
#অন্তর্ভাৱ "qapi/error.h"
#অন্তর্ভাৱ "system/address-spaces.h"
#অন্তর্ভাৱ "hw/core/boards.h"
#অন্তর্ভাৱ "hw/char/serial-mm.h"
#অন্তর্ভাৱ "hw/intc/riscv_aclint.h"
#অন্তর্ভাৱ "hw/intc/riscv_aplic.h"
#অন্তর্ভাৱ "hw/intc/riscv_imsic.h"
#অন্তর্ভাৱ "hw/core/qdev-properties.h"
#অন্তর্ভাৱ "hw/riscv/boot.h"
#অন্তর্ভাৱ "hw/riscv/xiangshan_kmh.h"
#অন্তর্ভাৱ "hw/riscv/riscv_hart.h"
#অন্তর্ভাৱ "system/system.h"

স্থির ধ্রুৱ MemMapEntry xiangshan_kmh_memmap[] = {
    [XIANGSHAN_KMH_ROM] =          {     0x1000,        0xF000 },
    [XIANGSHAN_KMH_UART0] =        { 0x310B0000,       0x10000 },
    [XIANGSHAN_KMH_CLINT] =        { 0x38000000,       0x10000 },
    [XIANGSHAN_KMH_APLIC_M] =      { 0x31100000,        0x4000 },
    [XIANGSHAN_KMH_APLIC_S] =      { 0x31120000,        0x4000 },
    [XIANGSHAN_KMH_IMSIC_M] =      { 0x3A800000,       0x10000 },
    [XIANGSHAN_KMH_IMSIC_S] =      { 0x3B000000,       0x80000 },
    [XIANGSHAN_KMH_DRAM] =         { 0x80000000,           0x0 },
};

স্থির DeviceState *xiangshan_kmh_create_aia(uint32_t অঙ্ক_harts)
{
    পূর্ণাঙ্ক i;
    ধ্রুৱ MemMapEntry *memmap = xiangshan_kmh_memmap;
    hwaddr addr = 0;
    DeviceState *aplic_m = রিক্ত;

    /* M-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_M].base;
    বাবে (i = 0; i < অঙ্ক_harts; i++) {
        riscv_imsic_create(get_system_memory(),
                           addr + i * IMSIC_HART_SIZE(0), i, true,
                           1, XIANGSHAN_KMH_IMSIC_অঙ্ক_IDS);
    }

    /* S-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_S].base;
    বাবে (i = 0; i < অঙ্ক_harts; i++) {
        riscv_imsic_create(get_system_memory(), addr +
                           i * IMSIC_HART_SIZE(XIANGSHAN_KMH_IMSIC_GUEST_BITS),
                           i, false, 1 + XIANGSHAN_KMH_IMSIC_GUEST_BITS,
                           XIANGSHAN_KMH_IMSIC_অঙ্ক_IDS);
    }

    /* M-level APLIC */
    aplic_m = riscv_aplic_create(get_system_memory(),
                                 memmap[XIANGSHAN_KMH_APLIC_M].base,
                                 memmap[XIANGSHAN_KMH_APLIC_M].size,
                                 0, 0, XIANGSHAN_KMH_APLIC_অঙ্ক_SOURCES,
                                 1, true, true, রিক্ত);

    /* S-level APLIC */
    riscv_aplic_create(get_system_memory(),
                       memmap[XIANGSHAN_KMH_APLIC_S].base,
                       memmap[XIANGSHAN_KMH_APLIC_S].size,
                       0, 0, XIANGSHAN_KMH_APLIC_অঙ্ক_SOURCES,
                       1, true, false, aplic_m);

    ঘূৰাই aplic_m;
}

স্থির শূন্য xiangshan_kmh_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(dev);
    ধ্রুৱ MemMapEntry *memmap = xiangshan_kmh_memmap;
    MemoryRegion *system_memory = get_system_memory();
    uint32_t অঙ্ক_harts = ms->smp.cpus;

    qdev_prop_set_uint32(DEVICE(&s->cpus), "num-harts", অঙ্ক_harts);
    qdev_prop_set_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_set_string(DEVICE(&s->cpus), "cpu-type",
                         TYPE_RISCV_CPU_XIANGSHAN_KMH);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* AIA */
    s->irqchip = xiangshan_kmh_create_aia(অঙ্ক_harts);

    /* UART */
    serial_mm_init(system_memory, memmap[XIANGSHAN_KMH_UART0].base, 2,
                   qdev_get_gpio_in(s->irqchip, XIANGSHAN_KMH_UART0_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* CLINT */
    riscv_aclint_swi_create(system_memory,
                            memmap[XIANGSHAN_KMH_CLINT].base,
                            0, অঙ্ক_harts, false);
    riscv_aclint_mtimer_create(system_memory,
                               memmap[XIANGSHAN_KMH_CLINT].base +
                               RISCV_ACLINT_SWI_SIZE,
                               RISCV_ACLINT_DEFAULT_MTIMER_SIZE,
                               0, অঙ্ক_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               XIANGSHAN_KMH_CLINT_TIMEBASE_FREQ, true);

    /* ROM */
    memory_region_init_rom(&s->rom, OBJECT(dev), "xiangshan.kunminghu.rom",
                           memmap[XIANGSHAN_KMH_ROM].size, &error_fatal);
    memory_region_add_subregion(system_memory,
                                memmap[XIANGSHAN_KMH_ROM].base, &s->rom);
}

স্থির শূন্য xiangshan_kmh_soc_class_init(ObjectClass *klass, ধ্রুৱ শূন্য *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = xiangshan_kmh_soc_realize;
    dc->user_creatable = false;
}

স্থির শূন্য xiangshan_kmh_soc_instance_init(Object *obj)
{
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
}

স্থির ধ্রুৱ TypeInfo xiangshan_kmh_soc_info = {
    .name = TYPE_XIANGSHAN_KMH_SOC,
    .parent = TYPE_DEVICE,
    .instance_size = পরিমাণ(XiangshanKmhSoCState),
    .instance_init = xiangshan_kmh_soc_instance_init,
    .class_init = xiangshan_kmh_soc_class_init,
};

স্থির শূন্য xiangshan_kmh_soc_register_types(শূন্য)
{
    type_register_static(&xiangshan_kmh_soc_info);
}
type_init(xiangshan_kmh_soc_register_types)

স্থির শূন্য xiangshan_kmh_machine_init(MachineState *machine)
{
    XiangshanKmhState *s = XIANGSHAN_KMH_MACHINE(machine);
    ধ্রুৱ MemMapEntry *memmap = xiangshan_kmh_memmap;
    MemoryRegion *system_memory = get_system_memory();
    hwaddr start_addr = memmap[XIANGSHAN_KMH_DRAM].base;
    RISCVBootInfo boot_info;

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            TYPE_XIANGSHAN_KMH_SOC);
    qdev_realize(DEVICE(&s->soc), রিক্ত, &error_fatal);

    /* Register RAM */
    memory_region_add_subregion(system_memory,
                                memmap[XIANGSHAN_KMH_DRAM].base,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    /* ROM reset vector */
    riscv_setup_rom_reset_vec(machine, &s->soc.cpus,
                              start_addr,
                              memmap[XIANGSHAN_KMH_ROM].base,
                              memmap[XIANGSHAN_KMH_ROM].size, 0, 0);
    যদি (machine->firmware) {
        riscv_load_firmware(machine, &boot_info, machine->firmware,
                            &start_addr, রিক্ত);
    }

    /* Note: dtb has been integrated into firmware(OpenSBI) when compiling */
}

স্থির শূন্য xiangshan_kmh_machine_class_init(ObjectClass *klass, ধ্রুৱ শূন্য *data)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    স্থির ধ্রুৱ ৱর্ণ *ধ্রুৱ valid_cpu_types[] = {
        TYPE_RISCV_CPU_XIANGSHAN_KMH,
        রিক্ত
    };

    mc->desc = "RISC-V Board compatible with the Xiangshan " \
               "Kunminghu FPGA prototype platform";
    mc->init = xiangshan_kmh_machine_init;
    mc->max_cpus = XIANGSHAN_KMH_MAX_CPUS;
    mc->default_cpu_type = TYPE_RISCV_CPU_XIANGSHAN_KMH;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_id = "xiangshan.kunminghu.ram";
}

স্থির ধ্রুৱ TypeInfo xiangshan_kmh_machine_info = {
    .name = TYPE_XIANGSHAN_KMH_MACHINE,
    .parent = TYPE_MACHINE,
    .instance_size = পরিমাণ(XiangshanKmhState),
    .class_init = xiangshan_kmh_machine_class_init,
};

স্থির শূন্য xiangshan_kmh_machine_register_types(শূন্য)
{
    type_register_static(&xiangshan_kmh_machine_info);
}
type_init(xiangshan_kmh_machine_register_types)
