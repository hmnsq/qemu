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

#ଅନ୍ତର୍ଭାଵ "qemu/osdep.h"
#ଅନ୍ତର୍ଭାଵ "qapi/error.h"
#ଅନ୍ତର୍ଭାଵ "system/address-spaces.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/boards.h"
#ଅନ୍ତର୍ଭାଵ "hw/char/serial-mm.h"
#ଅନ୍ତର୍ଭାଵ "hw/intc/riscv_aclint.h"
#ଅନ୍ତର୍ଭାଵ "hw/intc/riscv_aplic.h"
#ଅନ୍ତର୍ଭାଵ "hw/intc/riscv_imsic.h"
#ଅନ୍ତର୍ଭାଵ "hw/core/qdev-properties.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/boot.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/xiangshan_kmh.h"
#ଅନ୍ତର୍ଭାଵ "hw/riscv/riscv_hart.h"
#ଅନ୍ତର୍ଭାଵ "system/system.h"

ସ୍ଥିର ଅଚଳ MemMapEntry xiangshan_kmh_memmap[] = {
    [XIANGSHAN_KMH_ROM] =          {     0x1000,        0xF000 },
    [XIANGSHAN_KMH_UART0] =        { 0x310B0000,       0x10000 },
    [XIANGSHAN_KMH_CLINT] =        { 0x38000000,       0x10000 },
    [XIANGSHAN_KMH_APLIC_M] =      { 0x31100000,        0x4000 },
    [XIANGSHAN_KMH_APLIC_S] =      { 0x31120000,        0x4000 },
    [XIANGSHAN_KMH_IMSIC_M] =      { 0x3A800000,       0x10000 },
    [XIANGSHAN_KMH_IMSIC_S] =      { 0x3B000000,       0x80000 },
    [XIANGSHAN_KMH_DRAM] =         { 0x80000000,           0x0 },
};

ସ୍ଥିର DeviceState *xiangshan_kmh_create_aia(uint32_t ଅଙ୍କ_harts)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ଅଚଳ MemMapEntry *memmap = xiangshan_kmh_memmap;
    hwaddr addr = 0;
    DeviceState *aplic_m = ଖାଲି;

    /* M-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_M].base;
    ପାଇଁ (i = 0; i < ଅଙ୍କ_harts; i++) {
        riscv_imsic_create(get_system_memory(),
                           addr + i * IMSIC_HART_SIZE(0), i, true,
                           1, XIANGSHAN_KMH_IMSIC_ଅଙ୍କ_IDS);
    }

    /* S-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_S].base;
    ପାଇଁ (i = 0; i < ଅଙ୍କ_harts; i++) {
        riscv_imsic_create(get_system_memory(), addr +
                           i * IMSIC_HART_SIZE(XIANGSHAN_KMH_IMSIC_GUEST_BITS),
                           i, false, 1 + XIANGSHAN_KMH_IMSIC_GUEST_BITS,
                           XIANGSHAN_KMH_IMSIC_ଅଙ୍କ_IDS);
    }

    /* M-level APLIC */
    aplic_m = riscv_aplic_create(get_system_memory(),
                                 memmap[XIANGSHAN_KMH_APLIC_M].base,
                                 memmap[XIANGSHAN_KMH_APLIC_M].size,
                                 0, 0, XIANGSHAN_KMH_APLIC_ଅଙ୍କ_SOURCES,
                                 1, true, true, ଖାଲି);

    /* S-level APLIC */
    riscv_aplic_create(get_system_memory(),
                       memmap[XIANGSHAN_KMH_APLIC_S].base,
                       memmap[XIANGSHAN_KMH_APLIC_S].size,
                       0, 0, XIANGSHAN_KMH_APLIC_ଅଙ୍କ_SOURCES,
                       1, true, false, aplic_m);

    ଫେରନ୍ତୁ aplic_m;
}

ସ୍ଥିର ଶୂନ୍ୟ xiangshan_kmh_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(dev);
    ଅଚଳ MemMapEntry *memmap = xiangshan_kmh_memmap;
    MemoryRegion *system_memory = get_system_memory();
    uint32_t ଅଙ୍କ_harts = ms->smp.cpus;

    qdev_prop_set_uint32(DEVICE(&s->cpus), "num-harts", ଅଙ୍କ_harts);
    qdev_prop_set_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_set_string(DEVICE(&s->cpus), "cpu-type",
                         TYPE_RISCV_CPU_XIANGSHAN_KMH);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* AIA */
    s->irqchip = xiangshan_kmh_create_aia(ଅଙ୍କ_harts);

    /* UART */
    serial_mm_init(system_memory, memmap[XIANGSHAN_KMH_UART0].base, 2,
                   qdev_get_gpio_in(s->irqchip, XIANGSHAN_KMH_UART0_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* CLINT */
    riscv_aclint_swi_create(system_memory,
                            memmap[XIANGSHAN_KMH_CLINT].base,
                            0, ଅଙ୍କ_harts, false);
    riscv_aclint_mtimer_create(system_memory,
                               memmap[XIANGSHAN_KMH_CLINT].base +
                               RISCV_ACLINT_SWI_SIZE,
                               RISCV_ACLINT_DEFAULT_MTIMER_SIZE,
                               0, ଅଙ୍କ_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               XIANGSHAN_KMH_CLINT_TIMEBASE_FREQ, true);

    /* ROM */
    memory_region_init_rom(&s->rom, OBJECT(dev), "xiangshan.kunminghu.rom",
                           memmap[XIANGSHAN_KMH_ROM].size, &error_fatal);
    memory_region_add_subregion(system_memory,
                                memmap[XIANGSHAN_KMH_ROM].base, &s->rom);
}

ସ୍ଥିର ଶୂନ୍ୟ xiangshan_kmh_soc_class_init(ObjectClass *klass, ଅଚଳ ଶୂନ୍ୟ *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = xiangshan_kmh_soc_realize;
    dc->user_creatable = false;
}

ସ୍ଥିର ଶୂନ୍ୟ xiangshan_kmh_soc_instance_init(Object *obj)
{
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
}

ସ୍ଥିର ଅଚଳ TypeInfo xiangshan_kmh_soc_info = {
    .name = TYPE_XIANGSHAN_KMH_SOC,
    .parent = TYPE_DEVICE,
    .instance_size = ଆକାର(XiangshanKmhSoCState),
    .instance_init = xiangshan_kmh_soc_instance_init,
    .class_init = xiangshan_kmh_soc_class_init,
};

ସ୍ଥିର ଶୂନ୍ୟ xiangshan_kmh_soc_register_types(ଶୂନ୍ୟ)
{
    type_register_static(&xiangshan_kmh_soc_info);
}
type_init(xiangshan_kmh_soc_register_types)

ସ୍ଥିର ଶୂନ୍ୟ xiangshan_kmh_machine_init(MachineState *machine)
{
    XiangshanKmhState *s = XIANGSHAN_KMH_MACHINE(machine);
    ଅଚଳ MemMapEntry *memmap = xiangshan_kmh_memmap;
    MemoryRegion *system_memory = get_system_memory();
    hwaddr start_addr = memmap[XIANGSHAN_KMH_DRAM].base;
    RISCVBootInfo boot_info;

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            TYPE_XIANGSHAN_KMH_SOC);
    qdev_realize(DEVICE(&s->soc), ଖାଲି, &error_fatal);

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
    ଯଦି (machine->firmware) {
        riscv_load_firmware(machine, &boot_info, machine->firmware,
                            &start_addr, ଖାଲି);
    }

    /* Note: dtb has been integrated into firmware(OpenSBI) when compiling */
}

ସ୍ଥିର ଶୂନ୍ୟ xiangshan_kmh_machine_class_init(ObjectClass *klass, ଅଚଳ ଶୂନ୍ୟ *data)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ valid_cpu_types[] = {
        TYPE_RISCV_CPU_XIANGSHAN_KMH,
        ଖାଲି
    };

    mc->desc = "RISC-V Board compatible with the Xiangshan " \
               "Kunminghu FPGA prototype platform";
    mc->init = xiangshan_kmh_machine_init;
    mc->max_cpus = XIANGSHAN_KMH_MAX_CPUS;
    mc->default_cpu_type = TYPE_RISCV_CPU_XIANGSHAN_KMH;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_id = "xiangshan.kunminghu.ram";
}

ସ୍ଥିର ଅଚଳ TypeInfo xiangshan_kmh_machine_info = {
    .name = TYPE_XIANGSHAN_KMH_MACHINE,
    .parent = TYPE_MACHINE,
    .instance_size = ଆକାର(XiangshanKmhState),
    .class_init = xiangshan_kmh_machine_class_init,
};

ସ୍ଥିର ଶୂନ୍ୟ xiangshan_kmh_machine_register_types(ଶୂନ୍ୟ)
{
    type_register_static(&xiangshan_kmh_machine_info);
}
type_init(xiangshan_kmh_machine_register_types)
