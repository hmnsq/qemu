/*
 * Shakti C-class SoC emulation
 *
 * Copyright (c) 2021 Vijai Kumar K <vijai@behindbytes.com>
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

#അന്തര്ഭാവ "qemu/osdep.h"
#അന്തര്ഭാവ "hw/core/boards.h"
#അന്തര്ഭാവ "hw/riscv/shakti_c.h"
#അന്തര്ഭാവ "qapi/error.h"
#അന്തര്ഭാവ "qemu/error-report.h"
#അന്തര്ഭാവ "hw/intc/sifive_plic.h"
#അന്തര്ഭാവ "hw/intc/riscv_aclint.h"
#അന്തര്ഭാവ "system/system.h"
#അന്തര്ഭാവ "hw/core/qdev-properties.h"
#അന്തര്ഭാവ "system/address-spaces.h"
#അന്തര്ഭാവ "hw/riscv/boot.h"

സ്ഥിതം സ്ഥിരം ഘടന MemmapEntry {
    hwaddr base;
    hwaddr size;
} shakti_c_memmap[] = {
    [SHAKTI_C_ROM]   =  {  0x00001000,  0x2000   },
    [SHAKTI_C_RAM]   =  {  0x80000000,  0x0      },
    [SHAKTI_C_UART]  =  {  0x00011300,  0x00040  },
    [SHAKTI_C_GPIO]  =  {  0x020d0000,  0x00100  },
    [SHAKTI_C_PLIC]  =  {  0x0c000000,  0x20000  },
    [SHAKTI_C_CLINT] =  {  0x02000000,  0xc0000  },
    [SHAKTI_C_I2C]   =  {  0x20c00000,  0x00100  },
};

സ്ഥിതം ശൂന്യം shakti_c_machine_state_init(MachineState *mstate)
{
    ShaktiCMachineState *sms = RISCV_SHAKTI_MACHINE(mstate);
    MemoryRegion *system_memory = get_system_memory();
    RISCVBootInfo boot_info;
    hwaddr firmware_load_addr = shakti_c_memmap[SHAKTI_C_RAM].base;

    /* Initialize SoC */
    object_initialize_child(OBJECT(mstate), "soc", &sms->soc,
                            TYPE_RISCV_SHAKTI_SOC);
    qdev_realize(DEVICE(&sms->soc), ശൂന്യ, &error_abort);

    /* register RAM */
    memory_region_add_subregion(system_memory,
                                shakti_c_memmap[SHAKTI_C_RAM].base,
                                mstate->ram);

    riscv_boot_info_init(&boot_info, &sms->soc.cpus);

    എങ്കിൽ (mstate->firmware) {
        riscv_load_firmware(mstate, &boot_info, mstate->firmware,
                            &firmware_load_addr, ശൂന്യ);
    }

    /* ROM reset vector */
    riscv_setup_rom_reset_vec(mstate, &sms->soc.cpus, firmware_load_addr,
                              shakti_c_memmap[SHAKTI_C_ROM].base,
                              shakti_c_memmap[SHAKTI_C_ROM].size, 0, 0);
}

സ്ഥിതം ശൂന്യം shakti_c_machine_instance_init(Object *obj)
{
}

സ്ഥിതം ശൂന്യം shakti_c_machine_class_init(ObjectClass *klass, സ്ഥിരം ശൂന്യം *data)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം valid_cpu_types[] = {
        RISCV_CPU_TYPE_NAME("shakti-c"),
        ശൂന്യ
    };

    mc->desc = "RISC-V Board compatible with Shakti SDK";
    mc->init = shakti_c_machine_state_init;
    mc->default_cpu_type = TYPE_RISCV_CPU_SHAKTI_C;
    mc->valid_cpu_types = valid_cpu_types;
    mc->deprecation_reason = "Currently unmaintained with no known users";
    mc->default_ram_id = "riscv.shakti.c.ram";
}

സ്ഥിതം സ്ഥിരം TypeInfo shakti_c_machine_type_info = {
    .name = TYPE_RISCV_SHAKTI_MACHINE,
    .parent = TYPE_MACHINE,
    .class_init = shakti_c_machine_class_init,
    .instance_init = shakti_c_machine_instance_init,
    .instance_size = വലുപ്പം(ShaktiCMachineState),
};

സ്ഥിതം ശൂന്യം shakti_c_machine_type_info_register(ശൂന്യം)
{
    type_register_static(&shakti_c_machine_type_info);
}
type_init(shakti_c_machine_type_info_register)

സ്ഥിതം ശൂന്യം shakti_c_soc_state_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(dev);
    MemoryRegion *system_memory = get_system_memory();

    sysbus_realize(SYS_BUS_DEVICE(&sss->cpus), &error_abort);

    sss->plic = sifive_plic_create(system_memory,
        shakti_c_memmap[SHAKTI_C_PLIC].base,
        (അക്ഷരം *)SHAKTI_C_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SHAKTI_C_PLIC_അക്കം_SOURCES,
        SHAKTI_C_PLIC_അക്കം_PRIORITIES,
        SHAKTI_C_PLIC_PRIORITY_BASE,
        SHAKTI_C_PLIC_PENDING_BASE,
        SHAKTI_C_PLIC_ENABLE_BASE,
        SHAKTI_C_PLIC_ENABLE_STRIDE,
        SHAKTI_C_PLIC_CONTEXT_BASE,
        SHAKTI_C_PLIC_CONTEXT_STRIDE,
        shakti_c_memmap[SHAKTI_C_PLIC].size);

    riscv_aclint_swi_create(system_memory,
        shakti_c_memmap[SHAKTI_C_CLINT].base,
        0, 1, false);
    riscv_aclint_mtimer_create(system_memory,
        shakti_c_memmap[SHAKTI_C_CLINT].base + RISCV_ACLINT_SWI_SIZE,
        RISCV_ACLINT_DEFAULT_MTIMER_SIZE, 0, 1,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);

    qdev_prop_set_chr(DEVICE(&(sss->uart)), "chardev", serial_hd(0));
    എങ്കിൽ (!sysbus_realize(SYS_BUS_DEVICE(&sss->uart), errp)) {
        തിരികെ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&sss->uart), 0,
                    shakti_c_memmap[SHAKTI_C_UART].base);

    /* ROM */
    memory_region_init_rom(&sss->rom, OBJECT(dev), "riscv.shakti.c.rom",
                           shakti_c_memmap[SHAKTI_C_ROM].size, &error_fatal);
    memory_region_add_subregion(system_memory,
        shakti_c_memmap[SHAKTI_C_ROM].base, &sss->rom);
}

സ്ഥിതം ശൂന്യം shakti_c_soc_class_init(ObjectClass *klass, സ്ഥിരം ശൂന്യം *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->realize = shakti_c_soc_state_realize;
    /*
     * Reasons:
     *     - Creates CPUS in riscv_hart_realize(), and can create unintended
     *       CPUs
     *     - Uses serial_hds in realize function, thus can't be used twice
     */
    dc->user_creatable = false;
}

സ്ഥിതം ശൂന്യം shakti_c_soc_instance_init(Object *obj)
{
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(obj);

    object_initialize_child(obj, "cpus", &sss->cpus, TYPE_RISCV_HART_ARRAY);
    object_initialize_child(obj, "uart", &sss->uart, TYPE_SHAKTI_UART);

    /*
     * CPU type is fixed and we are not supporting passing from commandline yet.
     * So let it be in instance_init. When supported should use ms->cpu_type
     * instead of TYPE_RISCV_CPU_SHAKTI_C
     */
    object_property_set_str(OBJECT(&sss->cpus), "cpu-type",
                            TYPE_RISCV_CPU_SHAKTI_C, &error_abort);
    object_property_set_int(OBJECT(&sss->cpus), "num-harts", 1,
                            &error_abort);
}

സ്ഥിതം സ്ഥിരം TypeInfo shakti_c_type_info = {
    .name = TYPE_RISCV_SHAKTI_SOC,
    .parent = TYPE_DEVICE,
    .class_init = shakti_c_soc_class_init,
    .instance_init = shakti_c_soc_instance_init,
    .instance_size = വലുപ്പം(ShaktiCSoCState),
};

സ്ഥിതം ശൂന്യം shakti_c_type_info_register(ശൂന്യം)
{
    type_register_static(&shakti_c_type_info);
}
type_init(shakti_c_type_info_register)
