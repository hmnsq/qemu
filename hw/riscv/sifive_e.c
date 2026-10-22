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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/cutils.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/core/loader.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/unimp.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/riscv_hart.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/sifive_e.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "hw/char/sifive_uart.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/sifive_plic.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/sifive_e_prci.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/sifive_e_aon.h"
#ಅನ್ತರ್ಭಾವ "chardev/char.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು sifive_e_memmap[] = {
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

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = sifive_e_memmap;

    SiFiveEState *s = RISCV_E_MACHINE(machine);
    ಸ್ಮರಣೆ_Region *sys_mem = get_system_ಸ್ಮರಣೆ();
    ಪೂರ್ಣಾಂಕ i;
    RISCVBootInfo boot_info;

    ಆದರೆ (machine->ram_ಗಾತ್ರ != mc->default_ram_ಗಾತ್ರ) {
        ಅಕ್ಷರ *sz = ಗಾತ್ರ_ಗಮ್ಯ_str(mc->default_ram_ಗಾತ್ರ);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_ಮಗು(OBJECT(machine), "soc", &s->soc, TYPE_RISCV_E_SOC);
    qdev_realize(DEVICE(&s->soc), ಖಾಲಿ, &error_fatal);

    /* Data Tightly Integrated Memory */
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_DTIM].ತಳ, machine->ram);

    /* Mask ROM reset vector */
    uint32_t reset_vec[4];

    ಆದರೆ (s->revb) {
        reset_vec[1] = 0x200102b7;  /* 0x1004: lui     t0,0x20010 */
    } ಇಲ್ಲದಿದ್ದರೆ {
        reset_vec[1] = 0x204002b7;  /* 0x1004: lui     t0,0x20400 */
    }
    reset_vec[2] = 0x00028067;      /* 0x1008: jr      t0 */

    reset_vec[0] = reset_vec[3] = 0;

    /* copy in the reset vector in little_endian byte order */
    ಗಾಗಿ (i = 0; i < ಗಾತ್ರ(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_ಗಮ್ಯ_le32(reset_vec[i]);
    }
    rom_ಕೂಡಿಸು_blob_fixed_as("mrom.reset", reset_vec, ಗಾತ್ರ(reset_vec),
                          memmap[SIFIVE_E_DEV_MROM].ತಳ, &ವಿಳಾಸ_space_ಸ್ಮರಣೆ);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);
    ಆದರೆ (machine->kernel_filename) {
        riscv_ತುಂಬು_kernel(machine, &boot_info,
                          memmap[SIFIVE_E_DEV_DTIM].ತಳ,
                          false, ಖಾಲಿ);
    }
}

ಸ್ಥಿರ bool sifive_e_machine_get_revb(Object *obj, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    ಹಿಂತಿರುಗಿ s->revb;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_machine_ಗಣ_revb(Object *obj, bool ಮೌಲ್ಯ, Error **errp)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = ಮೌಲ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_machine_instance_init(Object *obj)
{
    SiFiveEState *s = RISCV_E_MACHINE(obj);

    s->revb = false;
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_machine_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with SiFive E SDK";
    mc->init = sifive_e_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = SIFIVE_E_CPU;
    mc->default_ram_id = "riscv.sifive.e.ram";
    mc->default_ram_ಗಾತ್ರ = sifive_e_memmap[SIFIVE_E_DEV_DTIM].ಗಾತ್ರ_2;

    object_class_property_ಕೂಡಿಸು_bool(oc, "revb", sifive_e_machine_get_revb,
                                   sifive_e_machine_ಗಣ_revb);
    object_class_property_ಗಣ_description(oc, "revb",
                                          "Set on to tell QEMU that it should model "
                                          "the revB HiFive1 board");
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo sifive_e_machine_typeinfo = {
    .name       = MACHINE_TYPE_NAME("sifive_e"),
    .ಪೋಷಕ     = TYPE_MACHINE,
    .class_init = sifive_e_machine_class_init,
    .instance_init = sifive_e_machine_instance_init,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(SiFiveEState),
};

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_machine_init_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&sifive_e_machine_typeinfo);
}

type_init(sifive_e_machine_init_ರಿಜಿಸ್ಟರ್_types)

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_soc_init(Object *obj)
{
    SiFiveESoCState *s = RISCV_E_SOC(obj);

    object_initialize_ಮಗು(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
    /*
     * Set the `num-harts` property later as the machine is potentially not
     * created yet.
     */
    object_property_ಗಣ_int(OBJECT(&s->cpus), "resetvec", 0x1004, &error_abort);
    object_initialize_ಮಗು(obj, "riscv.sifive.e.gpio0", &s->gpio,
                            TYPE_SIFIVE_GPIO);
    object_initialize_ಮಗು(obj, "riscv.sifive.e.aon", &s->aon,
                            TYPE_SIFIVE_E_AON);
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_soc_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = sifive_e_memmap;
    SiFiveESoCState *s = RISCV_E_SOC(dev);
    ಸ್ಮರಣೆ_Region *sys_mem = get_system_ಸ್ಮರಣೆ();

    object_property_ಗಣ_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Mask ROM */
    ಸ್ಮರಣೆ_region_init_rom(&s->ಮುಖವಾಡ_rom, OBJECT(dev), "riscv.sifive.e.mrom",
                           memmap[SIFIVE_E_DEV_MROM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem,
        memmap[SIFIVE_E_DEV_MROM].ತಳ, &s->ಮುಖವಾಡ_rom);

    /* MMIO */
    s->plic = sifive_plic_create(sys_mem, memmap[SIFIVE_E_DEV_PLIC].ತಳ,
        (ಅಕ್ಷರ *)SIFIVE_E_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SIFIVE_E_PLIC_ಅಂಕೆ_SOURCES,
        SIFIVE_E_PLIC_ಅಂಕೆ_PRIORITIES,
        SIFIVE_E_PLIC_ಆದ್ಯತೆ_ತಳ,
        SIFIVE_E_PLIC_PENDING_ತಳ,
        SIFIVE_E_PLIC_ENABLE_ತಳ,
        SIFIVE_E_PLIC_ENABLE_STRIDE,
        SIFIVE_E_PLIC_CONTEXT_ತಳ,
        SIFIVE_E_PLIC_CONTEXT_STRIDE,
        memmap[SIFIVE_E_DEV_PLIC].ಗಾತ್ರ_2);
    riscv_aclint_swi_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].ತಳ,
        0, ms->smp.cpus, false);
    riscv_aclint_mtimer_create(sys_mem, memmap[SIFIVE_E_DEV_CLINT].ತಳ +
            RISCV_ACLINT_SWI_ಗಾತ್ರ,
        RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ, 0, ms->smp.cpus,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        SIFIVE_E_LFCLK_DEFAULT_FREQ, false);
    sifive_e_prci_create(memmap[SIFIVE_E_DEV_PRCI].ತಳ);

    /* AON */

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->aon), errp)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* Map AON registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->aon), 0, memmap[SIFIVE_E_DEV_AON].ತಳ);

    /* GPIO */

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* Map GPIO registers */
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->gpio), 0, memmap[SIFIVE_E_DEV_GPIO0].ತಳ);

    /* Pass all GPIOs to the SOC layer so they are available to the board */
    qdev_pass_gpios(DEVICE(&s->gpio), dev, ಖಾಲಿ);

    /* Connect GPIO interrupts to the PLIC */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 32; i++) {
        sysbus_connect_irq(SYS_BUS_DEVICE(&s->gpio), i,
                           qdev_get_gpio_ಒಳ(DEVICE(s->plic),
                                            SIFIVE_E_GPIO0_IRQ0 + i));
    }
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->aon), 0,
                       qdev_get_gpio_ಒಳ(DEVICE(s->plic),
                                        SIFIVE_E_AON_WDT_IRQ));

    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART0].ತಳ,
        serial_hd(0), qdev_get_gpio_ಒಳ(DEVICE(s->plic), SIFIVE_E_UART0_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi0",
        memmap[SIFIVE_E_DEV_QSPI0].ತಳ, memmap[SIFIVE_E_DEV_QSPI0].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.sifive.e.pwm0",
        memmap[SIFIVE_E_DEV_PWM0].ತಳ, memmap[SIFIVE_E_DEV_PWM0].ಗಾತ್ರ_2);
    sifive_uart_create(sys_mem, memmap[SIFIVE_E_DEV_UART1].ತಳ,
        serial_hd(1), qdev_get_gpio_ಒಳ(DEVICE(s->plic), SIFIVE_E_UART1_IRQ));
    create_unimplemented_device("riscv.sifive.e.qspi1",
        memmap[SIFIVE_E_DEV_QSPI1].ತಳ, memmap[SIFIVE_E_DEV_QSPI1].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.sifive.e.pwm1",
        memmap[SIFIVE_E_DEV_PWM1].ತಳ, memmap[SIFIVE_E_DEV_PWM1].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.sifive.e.qspi2",
        memmap[SIFIVE_E_DEV_QSPI2].ತಳ, memmap[SIFIVE_E_DEV_QSPI2].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.sifive.e.pwm2",
        memmap[SIFIVE_E_DEV_PWM2].ತಳ, memmap[SIFIVE_E_DEV_PWM2].ಗಾತ್ರ_2);

    /* Flash memory */
    ಸ್ಮರಣೆ_region_init_rom(&s->xip_mem, OBJECT(dev), "riscv.sifive.e.xip",
                           memmap[SIFIVE_E_DEV_XIP].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem, memmap[SIFIVE_E_DEV_XIP].ತಳ,
        &s->xip_mem);
}

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_soc_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = sifive_e_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo sifive_e_soc_type_info = {
    .name = TYPE_RISCV_E_SOC,
    .ಪೋಷಕ = TYPE_DEVICE,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(SiFiveESoCState),
    .instance_init = sifive_e_soc_init,
    .class_init = sifive_e_soc_class_init,
};

ಸ್ಥಿರ ಶೂನ್ಯ sifive_e_soc_ರಿಜಿಸ್ಟರ್_types(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&sifive_e_soc_type_info);
}

type_init(sifive_e_soc_ರಿಜಿಸ್ಟರ್_types)
