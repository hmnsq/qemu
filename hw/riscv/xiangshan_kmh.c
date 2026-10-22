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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "system/address-spaces.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/char/serial-mm.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/riscv_aplic.h"
#అన్తర్భావ "hw/intc/riscv_imsic.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/riscv/xiangshan_kmh.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "system/system.h"

స్థిర స్థిరరాశి Mem_Map_నమోదు xiangshan_kmh_memmap[] = {
    [XIANGSHAN_KMH_ROM] =          {     0x1000,        0xF000 },
    [XIANGSHAN_KMH_UART0] =        { 0x310B0000,       0x10000 },
    [XIANGSHAN_KMH_CLINT] =        { 0x38000000,       0x10000 },
    [XIANGSHAN_KMH_APLIC_M] =      { 0x31100000,        0x4000 },
    [XIANGSHAN_KMH_APLIC_S] =      { 0x31120000,        0x4000 },
    [XIANGSHAN_KMH_IMSIC_M] =      { 0x3A800000,       0x10000 },
    [XIANGSHAN_KMH_IMSIC_S] =      { 0x3B000000,       0x80000 },
    [XIANGSHAN_KMH_DRAM] =         { 0x80000000,           0x0 },
};

స్థిర DeviceState *xiangshan_kmh_create_aia(uint32_t అంకె_harts)
{
    పూర్ణాంకం i;
    స్థిరరాశి Mem_Map_నమోదు *memmap = xiangshan_kmh_memmap;
    hwaddr addr = 0;
    DeviceState *aplic_m = ఖాళీ;

    /* M-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_M].పునాది;
    కొరకు (i = 0; i < అంకె_harts; i++) {
        riscv_imsic_create(get_system_జ్ఞాపకం(),
                           addr + i * IMSIC_HART_కొలత(0), i, true,
                           1, XIANGSHAN_KMH_IMSIC_అంకె_IDS);
    }

    /* S-level IMSICs */
    addr = memmap[XIANGSHAN_KMH_IMSIC_S].పునాది;
    కొరకు (i = 0; i < అంకె_harts; i++) {
        riscv_imsic_create(get_system_జ్ఞాపకం(), addr +
                           i * IMSIC_HART_కొలత(XIANGSHAN_KMH_IMSIC_GUEST_బిట్లు),
                           i, false, 1 + XIANGSHAN_KMH_IMSIC_GUEST_బిట్లు,
                           XIANGSHAN_KMH_IMSIC_అంకె_IDS);
    }

    /* M-level APLIC */
    aplic_m = riscv_aplic_create(get_system_జ్ఞాపకం(),
                                 memmap[XIANGSHAN_KMH_APLIC_M].పునాది,
                                 memmap[XIANGSHAN_KMH_APLIC_M].కొలత,
                                 0, 0, XIANGSHAN_KMH_APLIC_అంకె_SOURCES,
                                 1, true, true, ఖాళీ);

    /* S-level APLIC */
    riscv_aplic_create(get_system_జ్ఞాపకం(),
                       memmap[XIANGSHAN_KMH_APLIC_S].పునాది,
                       memmap[XIANGSHAN_KMH_APLIC_S].కొలత,
                       0, 0, XIANGSHAN_KMH_APLIC_అంకె_SOURCES,
                       1, true, false, aplic_m);

    తిరిగి aplic_m;
}

స్థిర శూన్య xiangshan_kmh_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(dev);
    స్థిరరాశి Mem_Map_నమోదు *memmap = xiangshan_kmh_memmap;
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    uint32_t అంకె_harts = ms->smp.cpus;

    qdev_prop_సమితి_uint32(DEVICE(&s->cpus), "num-harts", అంకె_harts);
    qdev_prop_సమితి_uint32(DEVICE(&s->cpus), "hartid-base", 0);
    qdev_prop_సమితి_string(DEVICE(&s->cpus), "cpu-type",
                         TYPE_RISCV_CPU_XIANGSHAN_KMH);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* AIA */
    s->irqchip = xiangshan_kmh_create_aia(అంకె_harts);

    /* UART */
    serial_mm_init(system_జ్ఞాపకం, memmap[XIANGSHAN_KMH_UART0].పునాది, 2,
                   qdev_get_gpio_లోపల(s->irqchip, XIANGSHAN_KMH_UART0_IRQ),
                   115200, serial_hd(0), DEVICE_LITTLE_ENDIAN);

    /* CLINT */
    riscv_aclint_swi_create(system_జ్ఞాపకం,
                            memmap[XIANGSHAN_KMH_CLINT].పునాది,
                            0, అంకె_harts, false);
    riscv_aclint_mtimer_create(system_జ్ఞాపకం,
                               memmap[XIANGSHAN_KMH_CLINT].పునాది +
                               RISCV_ACLINT_SWI_కొలత,
                               RISCV_ACLINT_DEFAULT_MTIMER_కొలత,
                               0, అంకె_harts, RISCV_ACLINT_DEFAULT_MTIMECMP,
                               RISCV_ACLINT_DEFAULT_MTIME,
                               XIANGSHAN_KMH_CLINT_TIMEBASE_FREQ, true);

    /* ROM */
    జ్ఞాపకం_region_init_rom(&s->rom, OBJECT(dev), "xiangshan.kunminghu.rom",
                           memmap[XIANGSHAN_KMH_ROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[XIANGSHAN_KMH_ROM].పునాది, &s->rom);
}

స్థిర శూన్య xiangshan_kmh_soc_class_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = xiangshan_kmh_soc_realize;
    dc->user_creatable = false;
}

స్థిర శూన్య xiangshan_kmh_soc_instance_init(Object *obj)
{
    XiangshanKmhSoCState *s = XIANGSHAN_KMH_SOC(obj);

    object_initialize_బిడ్డ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
}

స్థిర స్థిరరాశి TypeInfo xiangshan_kmh_soc_info = {
    .name = TYPE_XIANGSHAN_KMH_SOC,
    .మాతృ = TYPE_DEVICE,
    .instance_కొలత = పరిమాణం(XiangshanKmhSoCState),
    .instance_init = xiangshan_kmh_soc_instance_init,
    .class_init = xiangshan_kmh_soc_class_init,
};

స్థిర శూన్య xiangshan_kmh_soc_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&xiangshan_kmh_soc_info);
}
type_init(xiangshan_kmh_soc_రిజిస్టరు_types)

స్థిర శూన్య xiangshan_kmh_machine_init(MachineState *machine)
{
    XiangshanKmhState *s = XIANGSHAN_KMH_MACHINE(machine);
    స్థిరరాశి Mem_Map_నమోదు *memmap = xiangshan_kmh_memmap;
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    hwaddr ప్రారంభం_addr = memmap[XIANGSHAN_KMH_DRAM].పునాది;
    RISCVBootInfo boot_info;

    /* Initialize SoC */
    object_initialize_బిడ్డ(OBJECT(machine), "soc", &s->soc,
                            TYPE_XIANGSHAN_KMH_SOC);
    qdev_realize(DEVICE(&s->soc), ఖాళీ, &error_fatal);

    /* Register RAM */
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                memmap[XIANGSHAN_KMH_DRAM].పునాది,
                                machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    /* ROM reset vector */
    riscv_అమరిక_rom_reset_vec(machine, &s->soc.cpus,
                              ప్రారంభం_addr,
                              memmap[XIANGSHAN_KMH_ROM].పునాది,
                              memmap[XIANGSHAN_KMH_ROM].కొలత, 0, 0);
    ఒకవేళ (machine->firmware) {
        riscv_ఎక్కించు_firmware(machine, &boot_info, machine->firmware,
                            &ప్రారంభం_addr, ఖాళీ);
    }

    /* Note: dtb has been integrated into firmware(OpenSBI) when compiling */
}

స్థిర శూన్య xiangshan_kmh_machine_class_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి valid_cpu_types[] = {
        TYPE_RISCV_CPU_XIANGSHAN_KMH,
        ఖాళీ
    };

    mc->desc = "RISC-V Board compatible with the Xiangshan " \
               "Kunminghu FPGA prototype platform";
    mc->init = xiangshan_kmh_machine_init;
    mc->max_cpus = XIANGSHAN_KMH_MAX_CPUS;
    mc->default_cpu_type = TYPE_RISCV_CPU_XIANGSHAN_KMH;
    mc->valid_cpu_types = valid_cpu_types;
    mc->default_ram_id = "xiangshan.kunminghu.ram";
}

స్థిర స్థిరరాశి TypeInfo xiangshan_kmh_machine_info = {
    .name = TYPE_XIANGSHAN_KMH_MACHINE,
    .మాతృ = TYPE_MACHINE,
    .instance_కొలత = పరిమాణం(XiangshanKmhState),
    .class_init = xiangshan_kmh_machine_class_init,
};

స్థిర శూన్య xiangshan_kmh_machine_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&xiangshan_kmh_machine_info);
}
type_init(xiangshan_kmh_machine_రిజిస్టరు_types)
