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

ساكن ثابت Mem_Map_داخلا xiangshan_kmh_memmap[] = {
    [XIANGSHAN_KMH_ROM] =          {     0x1000,        0xF000 },
    [XIANGSHAN_KMH_UART0] =        { 0x310B0000,       0x10000 },
    [XIANGSHAN_KMH_CLINT] =        { 0x38000000,       0x10000 },
    [XIANGSHAN_KMH_APLIC_M] =      { 0x31100000,        0x4000 },
    [XIANGSHAN_KMH_APLIC_S] =      { 0x31120000,        0x4000 },
    [XIANGSHAN_KMH_IMSIC_M] =      { 0x3A800000,       0x10000 },
    [XIANGSHAN_KMH_IMSIC_S] =      { 0x3B000000,       0x80000 },
    [XIANGSHAN_KMH_DRAM] =         { 0x80000000,           0x0 },
};

ساكن DeviceState *xiangshan_kmh_create_aia(uint32_t انگ_harts)
{
    صحيح i;
    ثابت Mem_Map_داخلا *memmap = xiangshan_kmh_memmap;
    hwaddr addr = 0;
    DeviceState *aplic_m = عدم;

    /* M-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_M].بنياد;
    لكل (i = 0; i < انگ_harts; i++) {
        riscv_imsic_create(get_system_يادگيري(),
                           addr + i * IMSIC_HART_سائيز(0), i, true,
                           1, XIANGSHAN_KMH_IMSIC_انگ_IDS);
    }

    /* S-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_S].بنياد;
    لكل (i = 0; i < انگ_harts; i++) {
        riscv_imsic_create(get_system_يادگيري(), addr +
                           i * IMSIC_HART_سائيز(XIANGSHAN_KMH_IMSIC_GUEST_بٽ),
                           i, false, 1 + XIANGSHAN_KMH_IMSIC_GUEST_بٽ,
                           XIANGSHAN_KMH_IMSIC_انگ_IDS);
    }

    /* M-level APLIC */
    aplic_m = riscv_aplic_create(get_system_يادگيري(),
                                 memmap[XIANGSHAN_KMH_APLIC_M].بنياد,
                                 memmap[XIANGSHAN_KMH_APLIC_M].سائيز,
                                 0, 0, XIANGSHAN_KMH_APLIC_انگ_SOURCES,
                                 1, true, true, عدم);

    /* S-level APLIC */
    riscv_aplic_create(get_system_يادگيري(),
                       memmap[XIANGSHAN_KMH_APLIC_S].بنياد,
                       memmap[XIANGSHAN_KMH_APLIC_S].سائيز,
                       0, 0, XIANGSHAN_KMH_APLIC_انگ_SOURCES,
                       1, true, false, aplic_m);

    أرجع aplic_m;
}

ساكن فراغ xiangshan_kmh_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(dev);
    ثابت Mem_Map_داخلا *memmap = xiangshan_kmh_memmap;
    يادگيري_Region *system_يادگيري = get_system_يادگيري();
    uint32_t انگ_harts = ms->smp.cpus;

    qdev_prop_سيٽ_uint32(DEVICE(&s->cpus), "num-harts", انگ_harts);
    qdev_prop_سيٽ_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_سيٽ_string(DEVICE(&s->cpus), "cpu-type",
                         TYPE_RISCV_CPU_XIANGSHAN_KMH);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* AIA */
    s->irqchip = xiangshan_kmh_create_aia(انگ_harts);

    /* UART */
    serial_mm_init(system_يادگيري, memmap[XIANGSHAN_KMH_UART0].بنياد, 2,
                   qdev_get_gpio_اندريون(s->irqchip, XIANGSHAN_KMH_UART0_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* CLINT */
    riscv_aclint_swi_create(system_يادگيري,
                            memmap[XIANGSHAN_KMH_CLINT].بنياد,
                            0, انگ_harts, false);
    riscv_aclint_mtimer_create(system_يادگيري,
                               memmap[XIANGSHAN_KMH_CLINT].بنياد +
                               RISCV_ACLINT_SWI_سائيز,
                               RISCV_ACLINT_DEFAULT_MTIMER_سائيز,
                               0, انگ_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               XIANGSHAN_KMH_CLINT_TIMEBASE_FREQ, true);

    /* ROM */
    يادگيري_region_init_rom(&s->rom, OBJECT(dev), "xiangshan.kunminghu.rom",
                           memmap[XIANGSHAN_KMH_ROM].سائيز, &error_fatal);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[XIANGSHAN_KMH_ROM].بنياد, &s->rom);
}

ساكن فراغ xiangshan_kmh_soc_class_init(ObjectClass *klass, ثابت فراغ *معلومات)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = xiangshan_kmh_soc_realize;
    dc->user_creatable = false;
}

ساكن فراغ xiangshan_kmh_soc_instance_init(Object *obj)
{
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(obj);

    object_initialize_ٻار(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
}

ساكن ثابت TypeInfo xiangshan_kmh_soc_info = {
    .name = TYPE_XIANGSHAN_KMH_SOC,
    .والد = TYPE_DEVICE,
    .instance_سائيز = حجم(XiangshanKmhSoCState),
    .instance_init = xiangshan_kmh_soc_instance_init,
    .class_init = xiangshan_kmh_soc_class_init,
};

ساكن فراغ xiangshan_kmh_soc_رجسٽر_types(فراغ)
{
    type_رجسٽر_static(&xiangshan_kmh_soc_info);
}
type_init(xiangshan_kmh_soc_رجسٽر_types)

ساكن فراغ xiangshan_kmh_machine_init(MachineState *machine)
{
    XiangshanKmhState *s = XIANGSHAN_KMH_MACHINE(machine);
    ثابت Mem_Map_داخلا *memmap = xiangshan_kmh_memmap;
    يادگيري_Region *system_يادگيري = get_system_يادگيري();
    hwaddr شروعات_addr = memmap[XIANGSHAN_KMH_DRAM].بنياد;
    RISCVBootInfo boot_info;

    /* Initialize SoC */
    object_initialize_ٻار(OBJECT(machine), "soc", &s->soc,
                            TYPE_XIANGSHAN_KMH_SOC);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    /* Register RAM */
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                memmap[XIANGSHAN_KMH_DRAM].بنياد,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    /* ROM reset vector */
    riscv_تياري_rom_reset_vec(machine, &s->soc.cpus,
                              شروعات_addr,
                              memmap[XIANGSHAN_KMH_ROM].بنياد,
                              memmap[XIANGSHAN_KMH_ROM].سائيز, 0, 0);
    إذا (machine->firmware) {
        riscv_ڀريو_firmware(machine, &boot_info, machine->firmware,
                            &شروعات_addr, عدم);
    }

    /* Note: dtb has been integrated into firmware(OpenSBI) when compiling */
}

ساكن فراغ xiangshan_kmh_machine_class_init(ObjectClass *klass, ثابت فراغ *معلومات)
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
    .والد = TYPE_MACHINE,
    .instance_سائيز = حجم(XiangshanKmhState),
    .class_init = xiangshan_kmh_machine_class_init,
};

ساكن فراغ xiangshan_kmh_machine_رجسٽر_types(فراغ)
{
    type_رجسٽر_static(&xiangshan_kmh_machine_info);
}
type_init(xiangshan_kmh_machine_رجسٽر_types)
