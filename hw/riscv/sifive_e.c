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

#شامل "qemu/osdep.h"
#شامل "qemu/cutils.h"
#شامل "qemu/error-report.h"
#شامل "qapi/error.h"
#شامل "hw/core/boards.h"
#شامل "hw/core/loader.h"
#شامل "hw/core/sysbus.h"
#شامل "hw/misc/unimp.h"
#شامل "target/riscv/cpu.h"
#شامل "hw/riscv/riscv_hart.h"
#شامل "hw/riscv/sifive_e.h"
#شامل "hw/riscv/boot.h"
#شامل "hw/char/sifive_uart.h"
#شامل "hw/intc/riscv_aclint.h"
#شامل "hw/intc/sifive_plic.h"
#شامل "hw/misc/sifive_e_prci.h"
#شامل "hw/misc/sifive_e_aon.h"
#شامل "chardev/char.h"
#شامل "system/system.h"

ساکن ثابت Mem_Map_اندراج sifive_e_memmap[] = {
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

ساکن خالی sifive_e_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    ثابت Mem_Map_اندراج *memmap = sifive_e_memmap;

    SiFiveEState *s = RISCV_E_MACHINE(machine);
    حافظہ_Region *sys_mem = get_system_حافظہ();
    صحیح i;
    RISCVBootInfo boot_info;

    اگر (machine->ram_سائز != mc->default_ram_سائز) {
        حرف *sz = سائز_مقصد_str(mc->default_ram_سائز);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        باہر_نکلو(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_بچہ(OBJECT(machine), "soc", &s->soc, TYPE_RISCV_E_SOC);
    qdev_realize(DEVICE(&s->soc), تہی, &error_fatal);

    /* Data Tightly Integrated Memory */
    حافظہ_region_جوڑو_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_DTIM].بنیاد, machine->ram);

    /* Mask ROM reset vector */
    uint32_t reset_vec[4];

    اگر (s->revb) {
        reset_vec[1] = 0x200102b7;  /* 0x1004: lui     t0,0x20010 */
    } ورنہ {
        reset_vec[1] = 0x204002b7;  /* 0x1004: lui     t0,0x20400 */
    }
    reset_vec[2] = 0x00028067;      /* 0x1008: jr      t0 */

    reset_vec[0] = reset_vec[3] = 0;

    /* copy in the reset vector in little_endian byte order */
    برائے (i = 0; i < حجم(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_مقصد_le32(reset_vec[i]);
    }
    rom_جوڑو_blob_fixed_as("mrom.reset", reset_vec, حجم(reset_vec),
                          memmap[SIFIVE_E_DEV_MROM].بنیاد, &پتہ_space_حافظہ);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);
    اگر (machine->kernel_filename) {
        riscv_بھرو_kernel(machine, &boot_info,
                          memmap[SIFIVE_E_DEV_DTIM].بنیاد,
                          false, تہی);
    }
}

ساکن bool sifive_e_machine_get_revb(Object *obj, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    واپس s->revb;
}

ساکن خالی sifive_e_machine_سیٹ_revb(Object *obj, bool قدر, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = قدر;
}

ساکن خالی sifive_e_machine_instance_init(Object *obj)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = false;
}

ساکن خالی sifive_e_machine_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with SiFive E SDK";
    mc->init = sifive_e_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = SIFIVE_E_CPU;
    mc->default_ram_id = "riscv.sifive.e.ram";
    mc->default_ram_سائز = sifive_e_memmap[SIFIVE_E_DEV_DTIM].سائز;

    object_class_property_جوڑو_bool(oc, "revb", sifive_e_machine_get_revb,
                                   sifive_e_machine_سیٹ_revb);
    object_class_property_سیٹ_description(oc, "revb",
                                          "Set on to tell QEMU that it should model "
                                          "the revB HiFive1 board");
}

ساکن ثابت TypeInfo sifive_e_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("sifive_e"),
    .والد     = TYPE_MACHINE,
    .class_init = sifive_e_machine_class_init,
    .instance_init = sifive_e_machine_instance_init,
    .instance_سائز = حجم(SiFiveEState),
};

ساکن خالی sifive_e_machine_init_رجسٹر_types(خالی)
{
    type_رجسٹر_static(&sifive_e_machine_typeinfo);
}

type_init(sifive_e_machine_init_رجسٹر_types)

ساکن خالی sifive_e_soc_init(Object *obj)
{
    SiFiveESoCState *s = RISCV_E_SOC(obj);

    object_initialize_بچہ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    object_property_سیٹ_int(OBJECT(&s->cpus), "resetvec", 0x1004, &error_abort);
    object_initialize_بچہ(obj, "riscv.sifive.e.gpio0", &s->gpio,
                            TYPE_SIFIVE_GPIO);
    object_initialize_بچہ(obj, "riscv.sifive.e.aon", &s->aon,
                            TYPE_SIFIVE_E_AON);
}

ساکن خالی sifive_e_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ثابت Mem_Map_اندراج *memmap = sifive_e_memmap;
    SiFiveESoCState *s = RISCV_E_SOC(dev);
    حافظہ_Region *sys_mem = get_system_حافظہ();

    object_property_سیٹ_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Mask ROM */
    حافظہ_region_init_rom(&s->نقاب_rom, OBJECT(dev), "riscv.sifive.e.mrom",
                           memmap[SIFIVE_E_DEV_MROM].سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_MROM].بنیاد, &s->نقاب_rom);

    /* MMIO */
    s->plic = sifive_plic_create(sys_mem, memmap[SIFIVE_E_DEV_PLIC].بنیاد,
        (حرف *)SIFIVE_E_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SIFIVE_E_PLIC_عدد_SOURCES,
        SIFIVE_E_PLIC_عدد_PRIORITIES,
        SIFIVE_E_PLIC_ترجیح_بنیاد,
        SIFIVE_E_PLIC_PENDING_بنیاد,
        SIFIVE_E_PLIC_ENABLE_بنیاد,
        SIFIVE_E_PLIC_ENABLE_STRIDE,
        SIFIVE_E_PLIC_CONTEXT_بنیاد,
        SIFIVE_E_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_E_DEV_PLIC].سائز);
    riscv_aclint_swi_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].بنیاد,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].بنیاد +
            RISCV_ACLINT_SWI_سائز,
        RISCV_ACLINT_DEFAULT_MTIMER_سائز, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        SIFIVE_E_LFCLK_DEFAULT_FREQ, false);
    sifive_e_prci_create(memmap[SIFIVE_E_DEV_PRCI].بنیاد);

    /* AON */

    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->aon), errp)) {
        واپس;
    }

    /* Map AON registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->aon), 0, memmap[SIFIVE_E_DEV_AON].بنیاد);

    /* GPIO */

    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
        واپس;
    }

    /* Map GPIO registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_E_DEV_GPIO0].بنیاد);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, تہی);

    /* Connect GPIO interrupts to the PLIC */
    برائے (صحیح i = 0; i < 32; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i,
                           qdev_get_gpio_اندر(DEVICE(s->plic),
                                            SIFIVE_E_GPIO0_IRQ0 + i));
    }
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->aon), 0,
                       qdev_get_gpio_اندر(DEVICE(s->plic),
                                        SIFIVE_E_AON_WDT_IRQ));

    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART0].بنیاد,
        serial_hd(0), qdev_get_gpio_اندر(DEVICE(s->plic), SIFIVE_E_UART0_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi0",
        memmap[SIFIVE_E_DEV_QSPI0].بنیاد, memmap[SIFIVE_E_DEV_QSPI0].سائز);
    create_unimplemented_device("riscv.sifive.e.pwm0",
        memmap[SIFIVE_E_DEV_PWM0].بنیاد, memmap[SIFIVE_E_DEV_PWM0].سائز);
    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART1].بنیاد,
        serial_hd(1), qdev_get_gpio_اندر(DEVICE(s->plic), SIFIVE_E_UART1_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi1",
        memmap[SIFIVE_E_DEV_QSPI1].بنیاد, memmap[SIFIVE_E_DEV_QSPI1].سائز);
    create_unimplemented_device("riscv.sifive.e.pwm1",
        memmap[SIFIVE_E_DEV_PWM1].بنیاد, memmap[SIFIVE_E_DEV_PWM1].سائز);
    create_unimplemented_device("riscv.sifive.e.qspi2",
        memmap[SIFIVE_E_DEV_QSPI2].بنیاد, memmap[SIFIVE_E_DEV_QSPI2].سائز);
    create_unimplemented_device("riscv.sifive.e.pwm2",
        memmap[SIFIVE_E_DEV_PWM2].بنیاد, memmap[SIFIVE_E_DEV_PWM2].سائز);

    /* Flash memory */
    حافظہ_region_init_rom(&s->xip_mem, OBJECT(dev), "riscv.sifive.e.xip",
                           memmap[SIFIVE_E_DEV_XIP].سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion(sys_mem, memmap[SIFIVE_E_DEV_XIP].بنیاد,
        &s->xip_mem);
}

ساکن خالی sifive_e_soc_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = sifive_e_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

ساکن ثابت TypeInfo sifive_e_soc_type_info = {
    .name = TYPE_RISCV_E_SOC,
    .والد = TYPE_DEVICE,
    .instance_سائز = حجم(SiFiveESoCState),
    .instance_init = sifive_e_soc_init,
    .class_init = sifive_e_soc_class_init,
};

ساکن خالی sifive_e_soc_رجسٹر_types(خالی)
{
    type_رجسٹر_static(&sifive_e_soc_type_info);
}

type_init(sifive_e_soc_رجسٹر_types)
