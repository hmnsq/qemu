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

#ضمن "qemu/osdep.h"
#ضمن "qapi/error.h"
#ضمن "system/address-spaces.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/char/serial-mm.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/riscv_aplic.h"
#ضمن "hw/intc/riscv_imsic.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/riscv/xiangshan_kmh.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "system/system.h"

ساكن ثابت Mem_Map_اندراج xiangshan_kmh_memmap[] = {
    [XIANGSHAN_KMH_ROM] =          {     0x1000,        0xF000 },
    [XIANGSHAN_KMH_UART0] =        { 0x310B0000,       0x10000 },
    [XIANGSHAN_KMH_CLINT] =        { 0x38000000,       0x10000 },
    [XIANGSHAN_KMH_APLIC_M] =      { 0x31100000,        0x4000 },
    [XIANGSHAN_KMH_APLIC_S] =      { 0x31120000,        0x4000 },
    [XIANGSHAN_KMH_IMSIC_M] =      { 0x3A800000,       0x10000 },
    [XIANGSHAN_KMH_IMSIC_S] =      { 0x3B000000,       0x80000 },
    [XIANGSHAN_KMH_DRAM] =         { 0x80000000,           0x0 },
};

ساكن DeviceState *xiangshan_kmh_create_aia(uint32_t عدد_harts)
{
    صحيح i;
    ثابت Mem_Map_اندراج *memmap = xiangshan_kmh_memmap;
    hwaddr addr = 0;
    DeviceState *aplic_m = عدم;

    /* M-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_M].ادھار;
    لكل (i = 0; i < عدد_harts; i++) {
        riscv_imsic_create(get_system_حافظہ(),
                           addr + i * IMSIC_HART_ناپ(0), i, true,
                           1, XIANGSHAN_KMH_IMSIC_عدد_IDS);
    }

    /* S-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_S].ادھار;
    لكل (i = 0; i < عدد_harts; i++) {
        riscv_imsic_create(get_system_حافظہ(), addr +
                           i * IMSIC_HART_ناپ(XIANGSHAN_KMH_IMSIC_GUEST_بٹ),
                           i, false, 1 + XIANGSHAN_KMH_IMSIC_GUEST_بٹ,
                           XIANGSHAN_KMH_IMSIC_عدد_IDS);
    }

    /* M-level APLIC */
    aplic_m = riscv_aplic_create(get_system_حافظہ(),
                                 memmap[XIANGSHAN_KMH_APLIC_M].ادھار,
                                 memmap[XIANGSHAN_KMH_APLIC_M].ناپ,
                                 0, 0, XIANGSHAN_KMH_APLIC_عدد_SOURCES,
                                 1, true, true, عدم);

    /* S-level APLIC */
    riscv_aplic_create(get_system_حافظہ(),
                       memmap[XIANGSHAN_KMH_APLIC_S].ادھار,
                       memmap[XIANGSHAN_KMH_APLIC_S].ناپ,
                       0, 0, XIANGSHAN_KMH_APLIC_عدد_SOURCES,
                       1, true, false, aplic_m);

    أرجع aplic_m;
}

ساكن فراغ xiangshan_kmh_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(dev);
    ثابت Mem_Map_اندراج *memmap = xiangshan_kmh_memmap;
    حافظہ_Region *system_حافظہ = get_system_حافظہ();
    uint32_t عدد_harts = ms->smp.cpus;

    qdev_prop_سیٹ_uint32(DEVICE(&s->cpus), "num-harts", عدد_harts);
    qdev_prop_سیٹ_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_سیٹ_string(DEVICE(&s->cpus), "cpu-type",
                         TYPE_RISCV_CPU_XIANGSHAN_KMH);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* AIA */
    s->irqchip = xiangshan_kmh_create_aia(عدد_harts);

    /* UART */
    serial_mm_init(system_حافظہ, memmap[XIANGSHAN_KMH_UART0].ادھار, 2,
                   qdev_get_gpio_وچ(s->irqchip, XIANGSHAN_KMH_UART0_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* CLINT */
    riscv_aclint_swi_create(system_حافظہ,
                            memmap[XIANGSHAN_KMH_CLINT].ادھار,
                            0, عدد_harts, false);
    riscv_aclint_mtimer_create(system_حافظہ,
                               memmap[XIANGSHAN_KMH_CLINT].ادھار +
                               RISCV_ACLINT_SWI_ناپ,
                               RISCV_ACLINT_DEFAULT_MTIMER_ناپ,
                               0, عدد_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               XIANGSHAN_KMH_CLINT_TIMEBASE_FREQ, true);

    /* ROM */
    حافظہ_region_init_rom(&s->rom, OBJECT(dev), "xiangshan.kunminghu.rom",
                           memmap[XIANGSHAN_KMH_ROM].ناپ, &error_fatal);
    حافظہ_region_جوڑو_subregion(system_حافظہ,
                                memmap[XIANGSHAN_KMH_ROM].ادھار, &s->rom);
}

ساكن فراغ xiangshan_kmh_soc_class_init(ObjectClass *klass, ثابت فراغ *انکڑے)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = xiangshan_kmh_soc_realize;
    dc->user_creatable = false;
}

ساكن فراغ xiangshan_kmh_soc_instance_init(Object *obj)
{
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(obj);

    object_initialize_بچہ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
}

ساكن ثابت TypeInfo xiangshan_kmh_soc_info = {
    .name = TYPE_XIANGSHAN_KMH_SOC,
    .ماپا = TYPE_DEVICE,
    .instance_ناپ = حجم(XiangshanKmhSoCState),
    .instance_init = xiangshan_kmh_soc_instance_init,
    .class_init = xiangshan_kmh_soc_class_init,
};

ساكن فراغ xiangshan_kmh_soc_رجسٹر_types(فراغ)
{
    type_رجسٹر_static(&xiangshan_kmh_soc_info);
}
type_init(xiangshan_kmh_soc_رجسٹر_types)

ساكن فراغ xiangshan_kmh_machine_init(MachineState *machine)
{
    XiangshanKmhState *s = XIANGSHAN_KMH_MACHINE(machine);
    ثابت Mem_Map_اندراج *memmap = xiangshan_kmh_memmap;
    حافظہ_Region *system_حافظہ = get_system_حافظہ();
    hwaddr شروع_addr = memmap[XIANGSHAN_KMH_DRAM].ادھار;
    RISCVBootInfo boot_info;

    /* Initialize SoC */
    object_initialize_بچہ(OBJECT(machine), "soc", &s->soc,
                            TYPE_XIANGSHAN_KMH_SOC);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    /* Register RAM */
    حافظہ_region_جوڑو_subregion(system_حافظہ,
                                memmap[XIANGSHAN_KMH_DRAM].ادھار,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    /* ROM reset vector */
    riscv_تیاری_rom_reset_vec(machine, &s->soc.cpus,
                              شروع_addr,
                              memmap[XIANGSHAN_KMH_ROM].ادھار,
                              memmap[XIANGSHAN_KMH_ROM].ناپ, 0, 0);
    إذا (machine->firmware) {
        riscv_لدو_firmware(machine, &boot_info, machine->firmware,
                            &شروع_addr, عدم);
    }

    /* Note: dtb has been integrated into firmware(OpenSBI) when compiling */
}

ساكن فراغ xiangshan_kmh_machine_class_init(ObjectClass *klass, ثابت فراغ *انکڑے)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    ساكن ثابت حرف *ثابت valid_cpu_types[] = {
        TYPE_RISCV_CPU_XIANGSHAN_KMH,
        عدم
    };

    mc->desc = "RISC-V Board compatible with the Xiangshan " \
               "Kunminghu FPGA prototype platform";
    mc->init = xiangshan_kmh_machine_init;
    mc->max_cpus = XIANGSHAN_KMH_MAX_CPUS;
    mc->default_cpu_type = TYPE_RISCV_CPU_XIANGSHAN_KMH;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_id = "xiangshan.kunminghu.ram";
}

ساكن ثابت TypeInfo xiangshan_kmh_machine_info = {
    .name = TYPE_XIANGSHAN_KMH_MACHINE,
    .ماپا = TYPE_MACHINE,
    .instance_ناپ = حجم(XiangshanKmhState),
    .class_init = xiangshan_kmh_machine_class_init,
};

ساكن فراغ xiangshan_kmh_machine_رجسٹر_types(فراغ)
{
    type_رجسٹر_static(&xiangshan_kmh_machine_info);
}
type_init(xiangshan_kmh_machine_رجسٹر_types)
