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

#अन्तर्भाव "qemu/osdep.h"
#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/riscv/shakti_c.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "hw/intc/sifive_plic.h"
#अन्तर्भाव "hw/intc/riscv_aclint.h"
#अन्तर्भाव "system/system.h"
#अन्तर्भाव "hw/core/qdev-properties.h"
#अन्तर्भाव "system/address-spaces.h"
#अन्तर्भाव "hw/riscv/boot.h"

स्थिर ध्रुव संरचना Memmap_प्रविष्टिः {
    hwaddr आधारः;
    hwaddr आकार;
} shakti_c_memmap[] = {
    [SHAKTI_C_ROM]   =  {  0x00001000,  0x2000   },
    [SHAKTI_C_RAM]   =  {  0x80000000,  0x0      },
    [SHAKTI_C_UART]  =  {  0x00011300,  0x00040  },
    [SHAKTI_C_GPIO]  =  {  0x020d0000,  0x00100  },
    [SHAKTI_C_PLIC]  =  {  0x0c000000,  0x20000  },
    [SHAKTI_C_CLINT] =  {  0x02000000,  0xc0000  },
    [SHAKTI_C_I2C]   =  {  0x20c00000,  0x00100  },
};

स्थिर शून्य shakti_c_machine_state_init(MachineState *mstate)
{
    ShaktiCMachineState *sms = RISCV_SHAKTI_MACHINE(mstate);
    स्मृतिः_Region *system_स्मृतिः = get_system_स्मृतिः();
    RISCVBootInfo boot_info;
    hwaddr firmware_आरोपय_addr = shakti_c_memmap[SHAKTI_C_RAM].आधारः;

    /* Initialize SoC */
    object_initialize_child(OBJECT(mstate), "soc", &sms->soc,
                            TYPE_RISCV_SHAKTI_SOC);
    qdev_realize(DEVICE(&sms->soc), रिक्त, &error_abort);

    /* register RAM */
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
                                shakti_c_memmap[SHAKTI_C_RAM].आधारः,
                                mstate->ram);

    riscv_boot_info_init(&boot_info, &sms->soc.cpus);

    यदि (mstate->firmware) {
        riscv_आरोपय_firmware(mstate, &boot_info, mstate->firmware,
                            &firmware_आरोपय_addr, रिक्त);
    }

    /* ROM reset vector */
    riscv_सज्जता_rom_reset_vec(mstate, &sms->soc.cpus, firmware_आरोपय_addr,
                              shakti_c_memmap[SHAKTI_C_ROM].आधारः,
                              shakti_c_memmap[SHAKTI_C_ROM].आकार, 0, 0);
}

स्थिर शून्य shakti_c_machine_instance_init(Object *obj)
{
}

स्थिर शून्य shakti_c_machine_class_init(ObjectClass *klass, ध्रुव शून्य *दत्त)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    स्थिर ध्रुव वर्ण * ध्रुव valid_cpu_types[] = {
        RISCV_CPU_TYPE_NAME("shakti-c"),
        रिक्त
    };

    mc->desc = "RISC-V Board compatible with Shakti SDK";
    mc->init = shakti_c_machine_state_init;
    mc->default_cpu_type = TYPE_RISCV_CPU_SHAKTI_C;
    mc->valid_cpu_types = valid_cpu_types;
    mc->deprecation_reason = "Currently unmaintained with no known users";
    mc->default_ram_id = "riscv.shakti.c.ram";
}

स्थिर ध्रुव TypeInfo shakti_c_machine_type_info = {
    .name = TYPE_RISCV_SHAKTI_MACHINE,
    .parent = TYPE_MACHINE,
    .class_init = shakti_c_machine_class_init,
    .instance_init = shakti_c_machine_instance_init,
    .instance_आकार = परिमाण(ShaktiCMachineState),
};

स्थिर शून्य shakti_c_machine_type_info_पञ्जिकाकोशः(शून्य)
{
    type_पञ्जिकाकोशः_static(&shakti_c_machine_type_info);
}
type_init(shakti_c_machine_type_info_पञ्जिकाकोशः)

स्थिर शून्य shakti_c_soc_state_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(dev);
    स्मृतिः_Region *system_स्मृतिः = get_system_स्मृतिः();

    sysbus_realize(SYS_BUS_DEVICE(&sss->cpus), &error_abort);

    sss->plic = sifive_plic_create(system_स्मृतिः,
        shakti_c_memmap[SHAKTI_C_PLIC].आधारः,
        (वर्ण *)SHAKTI_C_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SHAKTI_C_PLIC_अङ्कः_SOURCES,
        SHAKTI_C_PLIC_अङ्कः_PRIORITIES,
        SHAKTI_C_PLIC_प्राथमिकता_आधारः,
        SHAKTI_C_PLIC_PENDING_आधारः,
        SHAKTI_C_PLIC_ENABLE_आधारः,
        SHAKTI_C_PLIC_ENABLE_STRIDE,
        SHAKTI_C_PLIC_CONTEXT_आधारः,
        SHAKTI_C_PLIC_CONTEXT_STRIDE,
        shakti_c_memmap[SHAKTI_C_PLIC].आकार);

    riscv_aclint_swi_create(system_स्मृतिः,
        shakti_c_memmap[SHAKTI_C_CLINT].आधारः,
        0, 1, false);
    riscv_aclint_mtimer_create(system_स्मृतिः,
        shakti_c_memmap[SHAKTI_C_CLINT].आधारः + RISCV_ACLINT_SWI_आकार,
        RISCV_ACLINT_DEFAULT_MTIMER_आकार, 0, 1,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);

    qdev_prop_समुच्चयः_chr(DEVICE(&(sss->uart)), "chardev", serial_hd(0));
    यदि (!sysbus_realize(SYS_BUS_DEVICE(&sss->uart), errp)) {
        प्रत्यागम;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&sss->uart), 0,
                    shakti_c_memmap[SHAKTI_C_UART].आधारः);

    /* ROM */
    स्मृतिः_region_init_rom(&sss->rom, OBJECT(dev), "riscv.shakti.c.rom",
                           shakti_c_memmap[SHAKTI_C_ROM].आकार, &error_fatal);
    स्मृतिः_region_सङ्कलय_subregion(system_स्मृतिः,
        shakti_c_memmap[SHAKTI_C_ROM].आधारः, &sss->rom);
}

स्थिर शून्य shakti_c_soc_class_init(ObjectClass *klass, ध्रुव शून्य *दत्त)
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

स्थिर शून्य shakti_c_soc_instance_init(Object *obj)
{
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(obj);

    object_initialize_child(obj, "cpus", &sss->cpus, TYPE_RISCV_HART_ARRAY);
    object_initialize_child(obj, "uart", &sss->uart, TYPE_SHAKTI_UART);

    /*
     * CPU type is fixed and we are not supporting passing from commandline yet.
     * So let it be in instance_init. When supported should use ms->cpu_type
     * instead of TYPE_RISCV_CPU_SHAKTI_C
     */
    object_property_समुच्चयः_str(OBJECT(&sss->cpus), "cpu-type",
                            TYPE_RISCV_CPU_SHAKTI_C, &error_abort);
    object_property_समुच्चयः_int(OBJECT(&sss->cpus), "num-harts", 1,
                            &error_abort);
}

स्थिर ध्रुव TypeInfo shakti_c_type_info = {
    .name = TYPE_RISCV_SHAKTI_SOC,
    .parent = TYPE_DEVICE,
    .class_init = shakti_c_soc_class_init,
    .instance_init = shakti_c_soc_instance_init,
    .instance_आकार = परिमाण(ShaktiCSoCState),
};

स्थिर शून्य shakti_c_type_info_पञ्जिकाकोशः(शून्य)
{
    type_पञ्जिकाकोशः_static(&shakti_c_type_info);
}
type_init(shakti_c_type_info_पञ्जिकाकोशः)
