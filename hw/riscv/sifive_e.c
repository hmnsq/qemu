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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "qemu/cutils.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/core/loader.h"
#अन्तर्भाव "hw/core/sysbus.h"
#अन्तर्भाव "hw/misc/unimp.h"
#अन्तर्भाव "target/riscv/cpu.h"
#अन्तर्भाव "hw/riscv/riscv_hart.h"
#अन्तर्भाव "hw/riscv/sifive_e.h"
#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "hw/char/sifive_uart.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "hw/intc/sifive_plic.h"
#अन्तर्भाव "hw/misc/sifive_e_prci.h"
#अन्तर्भाव "hw/misc/sifive_e_aon.h"
#अन्तर्भाव "chardev/char.h"
#अन्तर्भाव "system/system.h"

स्थिर ध्रुव Mem_Map_प्रविष्टिः sifive_e_memmap[] = {
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

स्थिर शून्य sifive_e_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    ध्रुव Mem_Map_प्रविष्टिः *memmap = sifive_e_memmap;

    SiFiveEState *s = RISCV_E_MACHINE(machine);
    स्मृतिः_Region *sys_mem = get_system_स्मृतिः();
    पूर्णाङ्क i;
    RISCVBootInfo boot_info;

    यदि (machine->ram_आकार != mc->default_ram_आकार) {
        वर्ण *sz = आकार_गन्तव्यम्_str(mc->default_ram_आकार);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        निर्गच्छ(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc, TYPE_RISCV_E_SOC);
    qdev_realize(DEVICE(&s->soc), रिक्त, &error_fatal);

    /* Data Tightly Integrated Memory */
    स्मृतिः_region_सङ्कलय_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_DTIM].आधारः, machine->ram);

    /* Mask ROM reset vector */
    uint32_t reset_vec[4];

    यदि (s->revb) {
        reset_vec[1] = 0x200102b7;  /* 0x1004: lui     t0,0x20010 */
    } अन्यथा {
        reset_vec[1] = 0x204002b7;  /* 0x1004: lui     t0,0x20400 */
    }
    reset_vec[2] = 0x00028067;      /* 0x1008: jr      t0 */

    reset_vec[0] = reset_vec[3] = 0;

    /* copy in the reset vector in little_endian byte order */
    प्रति (i = 0; i < परिमाण(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_गन्तव्यम्_le32(reset_vec[i]);
    }
    rom_सङ्कलय_blob_fixed_as("mrom.reset", reset_vec, परिमाण(reset_vec),
                          memmap[SIFIVE_E_DEV_MROM].आधारः, &स्थानाङ्कः_space_स्मृतिः);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);
    यदि (machine->kernel_filename) {
        riscv_आरोपय_kernel(machine, &boot_info,
                          memmap[SIFIVE_E_DEV_DTIM].आधारः,
                          false, रिक्त);
    }
}

स्थिर bool sifive_e_machine_get_revb(Object *obj, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    प्रत्यागम s->revb;
}

स्थिर शून्य sifive_e_machine_समुच्चयः_revb(Object *obj, bool मानम्, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = मानम्;
}

स्थिर शून्य sifive_e_machine_instance_init(Object *obj)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = false;
}

स्थिर शून्य sifive_e_machine_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with SiFive E SDK";
    mc->init = sifive_e_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = SIFIVE_E_CPU;
    mc->default_ram_id = "riscv.sifive.e.ram";
    mc->default_ram_आकार = sifive_e_memmap[SIFIVE_E_DEV_DTIM].आकार;

    object_class_property_सङ्कलय_bool(oc, "revb", sifive_e_machine_get_revb,
                                   sifive_e_machine_समुच्चयः_revb);
    object_class_property_समुच्चयः_description(oc, "revb",
                                          "Set on to tell QEMU that it should model "
                                          "the revB HiFive1 board");
}

स्थिर ध्रुव TypeInfo sifive_e_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("sifive_e"),
    .parent     = TYPE_MACHINE,
    .class_init = sifive_e_machine_class_init,
    .instance_init = sifive_e_machine_instance_init,
    .instance_आकार = परिमाण(SiFiveEState),
};

स्थिर शून्य sifive_e_machine_init_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&sifive_e_machine_typeinfo);
}

type_init(sifive_e_machine_init_पञ्जिकाकोशः_types)

स्थिर शून्य sifive_e_soc_init(Object *obj)
{
    SiFiveESoCState *s = RISCV_E_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    object_property_समुच्चयः_int(OBJECT(&s->cpus), "resetvec", 0x1004, &error_abort);
    object_initialize_child(obj, "riscv.sifive.e.gpio0", &s->gpio,
                            TYPE_SIFIVE_GPIO);
    object_initialize_child(obj, "riscv.sifive.e.aon", &s->aon,
                            TYPE_SIFIVE_E_AON);
}

स्थिर शून्य sifive_e_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ध्रुव Mem_Map_प्रविष्टिः *memmap = sifive_e_memmap;
    SiFiveESoCState *s = RISCV_E_SOC(dev);
    स्मृतिः_Region *sys_mem = get_system_स्मृतिः();

    object_property_समुच्चयः_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_समुच्चयः_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Mask ROM */
    स्मृतिः_region_init_rom(&s->आवरणम्_rom, OBJECT(dev), "riscv.sifive.e.mrom",
                           memmap[SIFIVE_E_DEV_MROM].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_MROM].आधारः, &s->आवरणम्_rom);

    /* MMIO */
    s->plic = sifive_plic_create(sys_mem, memmap[SIFIVE_E_DEV_PLIC].आधारः,
        (वर्ण *)SIFIVE_E_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SIFIVE_E_PLIC_अङ्कः_SOURCES,
        SIFIVE_E_PLIC_अङ्कः_PRIORITIES,
        SIFIVE_E_PLIC_प्राथमिकता_आधारः,
        SIFIVE_E_PLIC_PENDING_आधारः,
        SIFIVE_E_PLIC_ENABLE_आधारः,
        SIFIVE_E_PLIC_ENABLE_STRIDE,
        SIFIVE_E_PLIC_CONTEXT_आधारः,
        SIFIVE_E_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_E_DEV_PLIC].आकार);
    riscv_aclint_swi_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].आधारः,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].आधारः +
            RISCV_ACLINT_SWI_आकार,
        RISCV_ACLINT_DEFAULT_MTIMER_आकार, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        SIFIVE_E_LFCLK_DEFAULT_FREQ, false);
    sifive_e_prci_create(memmap[SIFIVE_E_DEV_PRCI].आधारः);

    /* AON */

    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->aon), errp)) {
        प्रत्यागम;
    }

    /* Map AON registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->aon), 0, memmap[SIFIVE_E_DEV_AON].आधारः);

    /* GPIO */

    यदि (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
        प्रत्यागम;
    }

    /* Map GPIO registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_E_DEV_GPIO0].आधारः);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, रिक्त);

    /* Connect GPIO interrupts to the PLIC */
    प्रति (पूर्णाङ्क i = 0; i < 32; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i,
                           qdev_get_gpio_अन्तर्(DEVICE(s->plic),
                                            SIFIVE_E_GPIO0_IRQ0 + i));
    }
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->aon), 0,
                       qdev_get_gpio_अन्तर्(DEVICE(s->plic),
                                        SIFIVE_E_AON_WDT_IRQ));

    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART0].आधारः,
        serial_hd(0), qdev_get_gpio_अन्तर्(DEVICE(s->plic), SIFIVE_E_UART0_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi0",
        memmap[SIFIVE_E_DEV_QSPI0].आधारः, memmap[SIFIVE_E_DEV_QSPI0].आकार);
    create_unimplemented_device("riscv.sifive.e.pwm0",
        memmap[SIFIVE_E_DEV_PWM0].आधारः, memmap[SIFIVE_E_DEV_PWM0].आकार);
    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART1].आधारः,
        serial_hd(1), qdev_get_gpio_अन्तर्(DEVICE(s->plic), SIFIVE_E_UART1_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi1",
        memmap[SIFIVE_E_DEV_QSPI1].आधारः, memmap[SIFIVE_E_DEV_QSPI1].आकार);
    create_unimplemented_device("riscv.sifive.e.pwm1",
        memmap[SIFIVE_E_DEV_PWM1].आधारः, memmap[SIFIVE_E_DEV_PWM1].आकार);
    create_unimplemented_device("riscv.sifive.e.qspi2",
        memmap[SIFIVE_E_DEV_QSPI2].आधारः, memmap[SIFIVE_E_DEV_QSPI2].आकार);
    create_unimplemented_device("riscv.sifive.e.pwm2",
        memmap[SIFIVE_E_DEV_PWM2].आधारः, memmap[SIFIVE_E_DEV_PWM2].आकार);

    /* Flash memory */
    स्मृतिः_region_init_rom(&s->xip_mem, OBJECT(dev), "riscv.sifive.e.xip",
                           memmap[SIFIVE_E_DEV_XIP].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(sys_mem, memmap[SIFIVE_E_DEV_XIP].आधारः,
        &s->xip_mem);
}

स्थिर शून्य sifive_e_soc_class_init(ObjectClass *oc, ध्रुव शून्य *दत्त)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = sifive_e_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

स्थिर ध्रुव TypeInfo sifive_e_soc_type_info = {
    .name = TYPE_RISCV_E_SOC,
    .parent = TYPE_DEVICE,
    .instance_आकार = परिमाण(SiFiveESoCState),
    .instance_init = sifive_e_soc_init,
    .class_init = sifive_e_soc_class_init,
};

स्थिर शून्य sifive_e_soc_पञ्जिकाकोशः_types(शून्य)
{
    type_पञ्जिकाकोशः_static(&sifive_e_soc_type_info);
}

type_init(sifive_e_soc_पञ्जिकाकोशः_types)
