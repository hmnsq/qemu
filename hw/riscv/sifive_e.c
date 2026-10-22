/*
 * QEMU RISC-V Board Compatible with SiFive Freedom E SDK
 *
 * Copyright (c) 2017 SiFive, Inc.
 *
 * Provides a board compatible with the SiFive Freedom E SDK:
 *
 * 0) UART
 * 1) CLINT (Core Level Interruptor)
 * 2) PLIC (Platform Level Interrupt Controller)
 * 3) PRCI (Power, Reset, Clock, Interrupt)
 * 4) Registers emulated as RAM: AON, GPIO, QSPI, PWM
 * 5) Flash memory emulated as RAM
 *
 * The Mask ROM reset vector jumps to the flash payload at 0x2040_0000.
 * The OTP ROM and Flash boot code will be emulated in a future version.
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
#అన్తర్భావ "qemu/cutils.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/core/loader.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "hw/misc/unimp.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/riscv/riscv_hart.h"
#అన్తర్భావ "hw/riscv/sifive_e.h"
#అన్తర్భావ "hw/riscv/boot.h"
#అన్తర్భావ "hw/char/sifive_uart.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "hw/intc/sifive_plic.h"
#అన్తర్భావ "hw/misc/sifive_e_prci.h"
#అన్తర్భావ "hw/misc/sifive_e_aon.h"
#అన్తర్భావ "chardev/char.h"
#అన్తర్భావ "system/system.h"

స్థిర స్థిరరాశి Mem_Map_నమోదు sifive_e_memmap[] = {
    [SIFIVE_E_DEV_DEBUG] =    {        0x0,     0x1000 },
    [SIFIVE_E_DEV_MROM] =     {     0x1000,     0x2000 },
    [SIFIVE_E_DEV_OTP] =      {    0x20000,     0x2000 },
    [SIFIVE_E_DEV_CLINT] =    {  0x2000000,    0x10000 },
    [SIFIVE_E_DEV_PLIC] =     {  0xc000000,  0x4000000 },
    [SIFIVE_E_DEV_AON] =      { 0x10000000,     0x8000 },
    [SIFIVE_E_DEV_PRCI] =     { 0x10008000,     0x8000 },
    [SIFIVE_E_DEV_OTP_CTRL] = { 0x10010000,     0x1000 },
    [SIFIVE_E_DEV_GPIO0] =    { 0x10012000,     0x1000 },
    [SIFIVE_E_DEV_UART0] =    { 0x10013000,     0x1000 },
    [SIFIVE_E_DEV_QSPI0] =    { 0x10014000,     0x1000 },
    [SIFIVE_E_DEV_PWM0] =     { 0x10015000,     0x1000 },
    [SIFIVE_E_DEV_UART1] =    { 0x10023000,     0x1000 },
    [SIFIVE_E_DEV_QSPI1] =    { 0x10024000,     0x1000 },
    [SIFIVE_E_DEV_PWM1] =     { 0x10025000,     0x1000 },
    [SIFIVE_E_DEV_QSPI2] =    { 0x10034000,     0x1000 },
    [SIFIVE_E_DEV_PWM2] =     { 0x10035000,     0x1000 },
    [SIFIVE_E_DEV_XIP] =      { 0x20000000, 0x20000000 },
    [SIFIVE_E_DEV_DTIM] =     { 0x80000000,     0x4000 }
};

స్థిర శూన్య sifive_e_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    స్థిరరాశి Mem_Map_నమోదు *memmap = sifive_e_memmap;

    SiFiveEState *s = RISCV_E_MACHINE(machine);
    జ్ఞాపకం_Region *sys_mem = get_system_జ్ఞాపకం();
    పూర్ణాంకం i;
    RISCVBootInfo boot_info;

    ఒకవేళ (machine->ram_కొలత != mc->default_ram_కొలత) {
        అక్షరం *sz = కొలత_గమ్యం_str(mc->default_ram_కొలత);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        నిష్క్రమించు(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_బిడ్డ(OBJECT(machine), "soc", &s->soc, TYPE_RISCV_E_SOC);
    qdev_realize(DEVICE(&s->soc), ఖాళీ, &error_fatal);

    /* Data Tightly Integrated Memory */
    జ్ఞాపకం_region_కూడు_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_DTIM].పునాది, machine->ram);

    /* Mask ROM reset vector */
    uint32_t reset_vec[4];

    ఒకవేళ (s->revb) {
        reset_vec[1] = 0x200102b7;  /* 0x1004: lui     t0,0x20010 */
    } లేకపోతే {
        reset_vec[1] = 0x204002b7;  /* 0x1004: lui     t0,0x20400 */
    }
    reset_vec[2] = 0x00028067;      /* 0x1008: jr      t0 */

    reset_vec[0] = reset_vec[3] = 0;

    /* copy in the reset vector in little_endian byte order */
    కొరకు (i = 0; i < పరిమాణం(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_గమ్యం_le32(reset_vec[i]);
    }
    rom_కూడు_blob_fixed_as("mrom.reset", reset_vec, పరిమాణం(reset_vec),
                          memmap[SIFIVE_E_DEV_MROM].పునాది, &చిరునామా_space_జ్ఞాపకం);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);
    ఒకవేళ (machine->kernel_filename) {
        riscv_ఎక్కించు_kernel(machine, &boot_info,
                          memmap[SIFIVE_E_DEV_DTIM].పునాది,
                          false, ఖాళీ);
    }
}

స్థిర bool sifive_e_machine_get_revb(Object *obj, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    తిరిగి s->revb;
}

స్థిర శూన్య sifive_e_machine_సమితి_revb(Object *obj, bool విలువ, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = విలువ;
}

స్థిర శూన్య sifive_e_machine_instance_init(Object *obj)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = false;
}

స్థిర శూన్య sifive_e_machine_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with SiFive E SDK";
    mc->init = sifive_e_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = SIFIVE_E_CPU;
    mc->default_ram_id = "riscv.sifive.e.ram";
    mc->default_ram_కొలత = sifive_e_memmap[SIFIVE_E_DEV_DTIM].కొలత;

    object_class_property_కూడు_bool(oc, "revb", sifive_e_machine_get_revb,
                                   sifive_e_machine_సమితి_revb);
    object_class_property_సమితి_description(oc, "revb",
                                          "Set on to tell QEMU that it should model "
                                          "the revB HiFive1 board");
}

స్థిర స్థిరరాశి TypeInfo sifive_e_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("sifive_e"),
    .మాతృ     = TYPE_MACHINE,
    .class_init = sifive_e_machine_class_init,
    .instance_init = sifive_e_machine_instance_init,
    .instance_కొలత = పరిమాణం(SiFiveEState),
};

స్థిర శూన్య sifive_e_machine_init_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&sifive_e_machine_typeinfo);
}

type_init(sifive_e_machine_init_రిజిస్టరు_types)

స్థిర శూన్య sifive_e_soc_init(Object *obj)
{
    SiFiveESoCState *s = RISCV_E_SOC(obj);

    object_initialize_బిడ్డ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    object_property_సమితి_int(OBJECT(&s->cpus), "resetvec", 0x1004, &error_abort);
    object_initialize_బిడ్డ(obj, "riscv.sifive.e.gpio0", &s->gpio,
                            TYPE_SIFIVE_GPIO);
    object_initialize_బిడ్డ(obj, "riscv.sifive.e.aon", &s->aon,
                            TYPE_SIFIVE_E_AON);
}

స్థిర శూన్య sifive_e_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    స్థిరరాశి Mem_Map_నమోదు *memmap = sifive_e_memmap;
    SiFiveESoCState *s = RISCV_E_SOC(dev);
    జ్ఞాపకం_Region *sys_mem = get_system_జ్ఞాపకం();

    object_property_సమితి_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_సమితి_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Mask ROM */
    జ్ఞాపకం_region_init_rom(&s->ముసుగు_rom, OBJECT(dev), "riscv.sifive.e.mrom",
                           memmap[SIFIVE_E_DEV_MROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_MROM].పునాది, &s->ముసుగు_rom);

    /* MMIO */
    s->plic = sifive_plic_create(sys_mem, memmap[SIFIVE_E_DEV_PLIC].పునాది,
        (అక్షరం *)SIFIVE_E_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SIFIVE_E_PLIC_అంకె_SOURCES,
        SIFIVE_E_PLIC_అంకె_PRIORITIES,
        SIFIVE_E_PLIC_ప్రాధాన్యత_పునాది,
        SIFIVE_E_PLIC_PENDING_పునాది,
        SIFIVE_E_PLIC_ENABLE_పునాది,
        SIFIVE_E_PLIC_ENABLE_STRIDE,
        SIFIVE_E_PLIC_CONTEXT_పునాది,
        SIFIVE_E_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_E_DEV_PLIC].కొలత);
    riscv_aclint_swi_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].పునాది,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].పునాది +
            RISCV_ACLINT_SWI_కొలత,
        RISCV_ACLINT_DEFAULT_MTIMER_కొలత, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        SIFIVE_E_LFCLK_DEFAULT_FREQ, false);
    sifive_e_prci_create(memmap[SIFIVE_E_DEV_PRCI].పునాది);

    /* AON */

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->aon), errp)) {
        తిరిగి;
    }

    /* Map AON registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->aon), 0, memmap[SIFIVE_E_DEV_AON].పునాది);

    /* GPIO */

    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
        తిరిగి;
    }

    /* Map GPIO registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_E_DEV_GPIO0].పునాది);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, ఖాళీ);

    /* Connect GPIO interrupts to the PLIC */
    కొరకు (పూర్ణాంకం i = 0; i < 32; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i,
                           qdev_get_gpio_లోపల(DEVICE(s->plic),
                                            SIFIVE_E_GPIO0_IRQ0 + i));
    }
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->aon), 0,
                       qdev_get_gpio_లోపల(DEVICE(s->plic),
                                        SIFIVE_E_AON_WDT_IRQ));

    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART0].పునాది,
        serial_hd(0), qdev_get_gpio_లోపల(DEVICE(s->plic), SIFIVE_E_UART0_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi0",
        memmap[SIFIVE_E_DEV_QSPI0].పునాది, memmap[SIFIVE_E_DEV_QSPI0].కొలత);
    create_unimplemented_device("riscv.sifive.e.pwm0",
        memmap[SIFIVE_E_DEV_PWM0].పునాది, memmap[SIFIVE_E_DEV_PWM0].కొలత);
    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART1].పునాది,
        serial_hd(1), qdev_get_gpio_లోపల(DEVICE(s->plic), SIFIVE_E_UART1_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi1",
        memmap[SIFIVE_E_DEV_QSPI1].పునాది, memmap[SIFIVE_E_DEV_QSPI1].కొలత);
    create_unimplemented_device("riscv.sifive.e.pwm1",
        memmap[SIFIVE_E_DEV_PWM1].పునాది, memmap[SIFIVE_E_DEV_PWM1].కొలత);
    create_unimplemented_device("riscv.sifive.e.qspi2",
        memmap[SIFIVE_E_DEV_QSPI2].పునాది, memmap[SIFIVE_E_DEV_QSPI2].కొలత);
    create_unimplemented_device("riscv.sifive.e.pwm2",
        memmap[SIFIVE_E_DEV_PWM2].పునాది, memmap[SIFIVE_E_DEV_PWM2].కొలత);

    /* Flash memory */
    జ్ఞాపకం_region_init_rom(&s->xip_mem, OBJECT(dev), "riscv.sifive.e.xip",
                           memmap[SIFIVE_E_DEV_XIP].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(sys_mem, memmap[SIFIVE_E_DEV_XIP].పునాది,
        &s->xip_mem);
}

స్థిర శూన్య sifive_e_soc_class_init(ObjectClass *oc, స్థిరరాశి శూన్య *దత్తాంశం)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = sifive_e_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

స్థిర స్థిరరాశి TypeInfo sifive_e_soc_type_info = {
    .name = TYPE_RISCV_E_SOC,
    .మాతృ = TYPE_DEVICE,
    .instance_కొలత = పరిమాణం(SiFiveESoCState),
    .instance_init = sifive_e_soc_init,
    .class_init = sifive_e_soc_class_init,
};

స్థిర శూన్య sifive_e_soc_రిజిస్టరు_types(శూన్య)
{
    type_రిజిస్టరు_static(&sifive_e_soc_type_info);
}

type_init(sifive_e_soc_రిజిస్టరు_types)
