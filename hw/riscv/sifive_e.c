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

#અન્તર્ભાવ "qemu/osdep.h"
#અન્તર્ભાવ "qemu/cutils.h"
#અન્તર્ભાવ "qemu/error-report.h"
#અન્તર્ભાવ "qapi/error.h"
#અન્તર્ભાવ "hw/core/boards.h"
#અન્તર્ભાવ "hw/core/loader.h"
#અન્તર્ભાવ "hw/core/sysbus.h"
#અન્તર્ભાવ "hw/misc/unimp.h"
#અન્તર્ભાવ "target/riscv/cpu.h"
#અન્તર્ભાવ "hw/riscv/riscv_hart.h"
#અન્તર્ભાવ "hw/riscv/sifive_e.h"
#અન્તર્ભાવ "hw/riscv/boot.h"
#અન્તર્ભાવ "hw/char/sifive_uart.h"
#અન્તર્ભાવ "hw/intc/riscv_aclint.h"
#અન્તર્ભાવ "hw/intc/sifive_plic.h"
#અન્તર્ભાવ "hw/misc/sifive_e_prci.h"
#અન્તર્ભાવ "hw/misc/sifive_e_aon.h"
#અન્તર્ભાવ "chardev/char.h"
#અન્તર્ભાવ "system/system.h"

સ્થિર અચલ MemMapEntry sifive_e_memmap[] = {
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

સ્થિર શૂન્ય sifive_e_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    અચલ MemMapEntry *memmap = sifive_e_memmap;

    SiFiveEState *s = RISCV_E_MACHINE(machine);
    MemoryRegion *sys_mem = get_system_memory();
    પૂર્ણાંક i;
    RISCVBootInfo boot_info;

    જો (machine->ram_size != mc->default_ram_size) {
        અક્ષર *sz = size_to_str(mc->default_ram_size);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        exit(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc, TYPE_RISCV_E_SOC);
    qdev_realize(DEVICE(&s->soc), ખાલી, &error_fatal);

    /* Data Tightly Integrated Memory */
    memory_region_add_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_DTIM].base, machine->ram);

    /* Mask ROM reset vector */
    uint32_t reset_vec[4];

    જો (s->revb) {
        reset_vec[1] = 0x200102b7;  /* 0x1004: lui     t0,0x20010 */
    } નહીંતર {
        reset_vec[1] = 0x204002b7;  /* 0x1004: lui     t0,0x20400 */
    }
    reset_vec[2] = 0x00028067;      /* 0x1008: jr      t0 */

    reset_vec[0] = reset_vec[3] = 0;

    /* copy in the reset vector in little_endian byte order */
    માટે (i = 0; i < કદ(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_to_le32(reset_vec[i]);
    }
    rom_add_blob_fixed_as("mrom.reset", reset_vec, કદ(reset_vec),
                          memmap[SIFIVE_E_DEV_MROM].base, &address_space_memory);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);
    જો (machine->kernel_filename) {
        riscv_load_kernel(machine, &boot_info,
                          memmap[SIFIVE_E_DEV_DTIM].base,
                          false, ખાલી);
    }
}

સ્થિર bool sifive_e_machine_get_revb(Object *obj, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    પાછા s->revb;
}

સ્થિર શૂન્ય sifive_e_machine_set_revb(Object *obj, bool કિંમત, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = કિંમત;
}

સ્થિર શૂન્ય sifive_e_machine_instance_init(Object *obj)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = false;
}

સ્થિર શૂન્ય sifive_e_machine_class_init(ObjectClass *oc, અચલ શૂન્ય *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with SiFive E SDK";
    mc->init = sifive_e_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = SIFIVE_E_CPU;
    mc->default_ram_id = "riscv.sifive.e.ram";
    mc->default_ram_size = sifive_e_memmap[SIFIVE_E_DEV_DTIM].size;

    object_class_property_add_bool(oc, "revb", sifive_e_machine_get_revb,
                                   sifive_e_machine_set_revb);
    object_class_property_set_description(oc, "revb",
                                          "Set on to tell QEMU that it should model "
                                          "the revB HiFive1 board");
}

સ્થિર અચલ TypeInfo sifive_e_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("sifive_e"),
    .parent     = TYPE_MACHINE,
    .class_init = sifive_e_machine_class_init,
    .instance_init = sifive_e_machine_instance_init,
    .instance_size = કદ(SiFiveEState),
};

સ્થિર શૂન્ય sifive_e_machine_init_register_types(શૂન્ય)
{
    type_register_static(&sifive_e_machine_typeinfo);
}

type_init(sifive_e_machine_init_register_types)

સ્થિર શૂન્ય sifive_e_soc_init(Object *obj)
{
    SiFiveESoCState *s = RISCV_E_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    object_property_set_int(OBJECT(&s->cpus), "resetvec", 0x1004, &error_abort);
    object_initialize_child(obj, "riscv.sifive.e.gpio0", &s->gpio,
                            TYPE_SIFIVE_GPIO);
    object_initialize_child(obj, "riscv.sifive.e.aon", &s->aon,
                            TYPE_SIFIVE_E_AON);
}

સ્થિર શૂન્ય sifive_e_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    અચલ MemMapEntry *memmap = sifive_e_memmap;
    SiFiveESoCState *s = RISCV_E_SOC(dev);
    MemoryRegion *sys_mem = get_system_memory();

    object_property_set_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Mask ROM */
    memory_region_init_rom(&s->mask_rom, OBJECT(dev), "riscv.sifive.e.mrom",
                           memmap[SIFIVE_E_DEV_MROM].size, &error_fatal);
    memory_region_add_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_MROM].base, &s->mask_rom);

    /* MMIO */
    s->plic = sifive_plic_create(sys_mem, memmap[SIFIVE_E_DEV_PLIC].base,
        (અક્ષર *)SIFIVE_E_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SIFIVE_E_PLIC_અંક_SOURCES,
        SIFIVE_E_PLIC_અંક_PRIORITIES,
        SIFIVE_E_PLIC_PRIORITY_BASE,
        SIFIVE_E_PLIC_PENDING_BASE,
        SIFIVE_E_PLIC_ENABLE_BASE,
        SIFIVE_E_PLIC_ENABLE_STRIDE,
        SIFIVE_E_PLIC_CONTEXT_BASE,
        SIFIVE_E_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_E_DEV_PLIC].size);
    riscv_aclint_swi_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].base,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].base +
            RISCV_ACLINT_SWI_SIZE,
        RISCV_ACLINT_DEFAULT_MTIMER_SIZE, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        SIFIVE_E_LFCLK_DEFAULT_FREQ, false);
    sifive_e_prci_create(memmap[SIFIVE_E_DEV_PRCI].base);

    /* AON */

    જો (!sysbus_realize(SYS_BUS_DEVICE(&s->aon), errp)) {
        પાછા;
    }

    /* Map AON registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->aon), 0, memmap[SIFIVE_E_DEV_AON].base);

    /* GPIO */

    જો (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
        પાછા;
    }

    /* Map GPIO registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_E_DEV_GPIO0].base);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, ખાલી);

    /* Connect GPIO interrupts to the PLIC */
    માટે (પૂર્ણાંક i = 0; i < 32; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i,
                           qdev_get_gpio_in(DEVICE(s->plic),
                                            SIFIVE_E_GPIO0_IRQ0 + i));
    }
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->aon), 0,
                       qdev_get_gpio_in(DEVICE(s->plic),
                                        SIFIVE_E_AON_WDT_IRQ));

    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART0].base,
        serial_hd(0), qdev_get_gpio_in(DEVICE(s->plic), SIFIVE_E_UART0_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi0",
        memmap[SIFIVE_E_DEV_QSPI0].base, memmap[SIFIVE_E_DEV_QSPI0].size);
    create_unimplemented_device("riscv.sifive.e.pwm0",
        memmap[SIFIVE_E_DEV_PWM0].base, memmap[SIFIVE_E_DEV_PWM0].size);
    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART1].base,
        serial_hd(1), qdev_get_gpio_in(DEVICE(s->plic), SIFIVE_E_UART1_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi1",
        memmap[SIFIVE_E_DEV_QSPI1].base, memmap[SIFIVE_E_DEV_QSPI1].size);
    create_unimplemented_device("riscv.sifive.e.pwm1",
        memmap[SIFIVE_E_DEV_PWM1].base, memmap[SIFIVE_E_DEV_PWM1].size);
    create_unimplemented_device("riscv.sifive.e.qspi2",
        memmap[SIFIVE_E_DEV_QSPI2].base, memmap[SIFIVE_E_DEV_QSPI2].size);
    create_unimplemented_device("riscv.sifive.e.pwm2",
        memmap[SIFIVE_E_DEV_PWM2].base, memmap[SIFIVE_E_DEV_PWM2].size);

    /* Flash memory */
    memory_region_init_rom(&s->xip_mem, OBJECT(dev), "riscv.sifive.e.xip",
                           memmap[SIFIVE_E_DEV_XIP].size, &error_fatal);
    memory_region_add_subregion(sys_mem, memmap[SIFIVE_E_DEV_XIP].base,
        &s->xip_mem);
}

સ્થિર શૂન્ય sifive_e_soc_class_init(ObjectClass *oc, અચલ શૂન્ય *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = sifive_e_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

સ્થિર અચલ TypeInfo sifive_e_soc_type_info = {
    .name = TYPE_RISCV_E_SOC,
    .parent = TYPE_DEVICE,
    .instance_size = કદ(SiFiveESoCState),
    .instance_init = sifive_e_soc_init,
    .class_init = sifive_e_soc_class_init,
};

સ્થિર શૂન્ય sifive_e_soc_register_types(શૂન્ય)
{
    type_register_static(&sifive_e_soc_type_info);
}

type_init(sifive_e_soc_register_types)
