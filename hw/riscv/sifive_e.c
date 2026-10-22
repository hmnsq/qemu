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

#ضمن "qemu/osdep.h"
#ضمن "qemu/cutils.h"
#ضمن "qemu/error-report.h"
#ضمن "qapi/error.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/core/loader.h"
#ضمن "hw/core/sysbus.h"
#ضمن "hw/misc/unimp.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/riscv/riscv_hart.h"
#ضمن "hw/riscv/sifive_e.h"
#ضمن "hw/riscv/boot.h"
#ضمن "hw/char/sifive_uart.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "hw/intc/sifive_plic.h"
#ضمن "hw/misc/sifive_e_prci.h"
#ضمن "hw/misc/sifive_e_aon.h"
#ضمن "chardev/char.h"
#ضمن "system/system.h"

ساكن ثابت Mem_Map_ثبت sifive_e_memmap[] = {
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

ساكن فراغ sifive_e_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    ثابت Mem_Map_ثبت *memmap = sifive_e_memmap;

    SiFiveEState *s = RISCV_E_MACHINE(machine);
    حافظه_Region *sys_mem = get_system_حافظه();
    صحيح i;
    RISCVBootInfo boot_info;

    إذا (machine->ram_اندازه != mc->default_ram_اندازه) {
        حرف *sz = اندازه_منزل_str(mc->default_ram_اندازه);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        وتل(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_ماشوم(OBJECT(machine), "soc", &s->soc, TYPE_RISCV_E_SOC);
    qdev_realize(DEVICE(&s->soc), عدم, &error_fatal);

    /* Data Tightly Integrated Memory */
    حافظه_region_جمع_کړه_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_DTIM].بنسټ, machine->ram);

    /* Mask ROM reset vector */
    uint32_t reset_vec[4];

    إذا (s->revb) {
        reset_vec[1] = 0x200102b7;  /* 0x1004: lui     t0,0x20010 */
    } وإلا {
        reset_vec[1] = 0x204002b7;  /* 0x1004: lui     t0,0x20400 */
    }
    reset_vec[2] = 0x00028067;      /* 0x1008: jr      t0 */

    reset_vec[0] = reset_vec[3] = 0;

    /* copy in the reset vector in little_endian byte order */
    لكل (i = 0; i < حجم(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_منزل_le32(reset_vec[i]);
    }
    rom_جمع_کړه_blob_fixed_as("mrom.reset", reset_vec, حجم(reset_vec),
                          memmap[SIFIVE_E_DEV_MROM].بنسټ, &پته_space_حافظه);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);
    إذا (machine->kernel_filename) {
        riscv_بارول_kernel(machine, &boot_info,
                          memmap[SIFIVE_E_DEV_DTIM].بنسټ,
                          false, عدم);
    }
}

ساكن bool sifive_e_machine_get_revb(Object *obj, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    أرجع s->revb;
}

ساكن فراغ sifive_e_machine_سېټ_revb(Object *obj, bool ارزښت, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = ارزښت;
}

ساكن فراغ sifive_e_machine_instance_init(Object *obj)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = false;
}

ساكن فراغ sifive_e_machine_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with SiFive E SDK";
    mc->init = sifive_e_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = SIFIVE_E_CPU;
    mc->default_ram_id = "riscv.sifive.e.ram";
    mc->default_ram_اندازه = sifive_e_memmap[SIFIVE_E_DEV_DTIM].اندازه;

    object_class_property_جمع_کړه_bool(oc, "revb", sifive_e_machine_get_revb,
                                   sifive_e_machine_سېټ_revb);
    object_class_property_سېټ_description(oc, "revb",
                                          "Set on to tell QEMU that it should model "
                                          "the revB HiFive1 board");
}

ساكن ثابت TypeInfo sifive_e_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("sifive_e"),
    .پلار     = TYPE_MACHINE,
    .class_init = sifive_e_machine_class_init,
    .instance_init = sifive_e_machine_instance_init,
    .instance_اندازه = حجم(SiFiveEState),
};

ساكن فراغ sifive_e_machine_init_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&sifive_e_machine_typeinfo);
}

type_init(sifive_e_machine_init_ثبتوونکی_types)

ساكن فراغ sifive_e_soc_init(Object *obj)
{
    SiFiveESoCState *s = RISCV_E_SOC(obj);

    object_initialize_ماشوم(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    object_property_سېټ_int(OBJECT(&s->cpus), "resetvec", 0x1004, &error_abort);
    object_initialize_ماشوم(obj, "riscv.sifive.e.gpio0", &s->gpio,
                            TYPE_SIFIVE_GPIO);
    object_initialize_ماشوم(obj, "riscv.sifive.e.aon", &s->aon,
                            TYPE_SIFIVE_E_AON);
}

ساكن فراغ sifive_e_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ثابت Mem_Map_ثبت *memmap = sifive_e_memmap;
    SiFiveESoCState *s = RISCV_E_SOC(dev);
    حافظه_Region *sys_mem = get_system_حافظه();

    object_property_سېټ_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_سېټ_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Mask ROM */
    حافظه_region_init_rom(&s->ماسک_rom, OBJECT(dev), "riscv.sifive.e.mrom",
                           memmap[SIFIVE_E_DEV_MROM].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_MROM].بنسټ, &s->ماسک_rom);

    /* MMIO */
    s->plic = sifive_plic_create(sys_mem, memmap[SIFIVE_E_DEV_PLIC].بنسټ,
        (حرف *)SIFIVE_E_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SIFIVE_E_PLIC_عدد_SOURCES,
        SIFIVE_E_PLIC_عدد_PRIORITIES,
        SIFIVE_E_PLIC_لومړیتوب_بنسټ,
        SIFIVE_E_PLIC_PENDING_بنسټ,
        SIFIVE_E_PLIC_ENABLE_بنسټ,
        SIFIVE_E_PLIC_ENABLE_STRIDE,
        SIFIVE_E_PLIC_CONTEXT_بنسټ,
        SIFIVE_E_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_E_DEV_PLIC].اندازه);
    riscv_aclint_swi_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].بنسټ,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].بنسټ +
            RISCV_ACLINT_SWI_اندازه,
        RISCV_ACLINT_DEFAULT_MTIMER_اندازه, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        SIFIVE_E_LFCLK_DEFAULT_FREQ, false);
    sifive_e_prci_create(memmap[SIFIVE_E_DEV_PRCI].بنسټ);

    /* AON */

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->aon), errp)) {
        أرجع;
    }

    /* Map AON registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->aon), 0, memmap[SIFIVE_E_DEV_AON].بنسټ);

    /* GPIO */

    إذا (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
        أرجع;
    }

    /* Map GPIO registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_E_DEV_GPIO0].بنسټ);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, عدم);

    /* Connect GPIO interrupts to the PLIC */
    لكل (صحيح i = 0; i < 32; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i,
                           qdev_get_gpio_دننه(DEVICE(s->plic),
                                            SIFIVE_E_GPIO0_IRQ0 + i));
    }
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->aon), 0,
                       qdev_get_gpio_دننه(DEVICE(s->plic),
                                        SIFIVE_E_AON_WDT_IRQ));

    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART0].بنسټ,
        serial_hd(0), qdev_get_gpio_دننه(DEVICE(s->plic), SIFIVE_E_UART0_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi0",
        memmap[SIFIVE_E_DEV_QSPI0].بنسټ, memmap[SIFIVE_E_DEV_QSPI0].اندازه);
    create_unimplemented_device("riscv.sifive.e.pwm0",
        memmap[SIFIVE_E_DEV_PWM0].بنسټ, memmap[SIFIVE_E_DEV_PWM0].اندازه);
    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART1].بنسټ,
        serial_hd(1), qdev_get_gpio_دننه(DEVICE(s->plic), SIFIVE_E_UART1_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi1",
        memmap[SIFIVE_E_DEV_QSPI1].بنسټ, memmap[SIFIVE_E_DEV_QSPI1].اندازه);
    create_unimplemented_device("riscv.sifive.e.pwm1",
        memmap[SIFIVE_E_DEV_PWM1].بنسټ, memmap[SIFIVE_E_DEV_PWM1].اندازه);
    create_unimplemented_device("riscv.sifive.e.qspi2",
        memmap[SIFIVE_E_DEV_QSPI2].بنسټ, memmap[SIFIVE_E_DEV_QSPI2].اندازه);
    create_unimplemented_device("riscv.sifive.e.pwm2",
        memmap[SIFIVE_E_DEV_PWM2].بنسټ, memmap[SIFIVE_E_DEV_PWM2].اندازه);

    /* Flash memory */
    حافظه_region_init_rom(&s->xip_mem, OBJECT(dev), "riscv.sifive.e.xip",
                           memmap[SIFIVE_E_DEV_XIP].اندازه, &error_fatal);
    حافظه_region_جمع_کړه_subregion(sys_mem, memmap[SIFIVE_E_DEV_XIP].بنسټ,
        &s->xip_mem);
}

ساكن فراغ sifive_e_soc_class_init(ObjectClass *oc, ثابت فراغ *معلومات)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = sifive_e_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

ساكن ثابت TypeInfo sifive_e_soc_type_info = {
    .name = TYPE_RISCV_E_SOC,
    .پلار = TYPE_DEVICE,
    .instance_اندازه = حجم(SiFiveESoCState),
    .instance_init = sifive_e_soc_init,
    .class_init = sifive_e_soc_class_init,
};

ساكن فراغ sifive_e_soc_ثبتوونکی_types(فراغ)
{
    type_ثبتوونکی_static(&sifive_e_soc_type_info);
}

type_init(sifive_e_soc_ثبتوونکی_types)
