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

#ضمن "qemu/osdep.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/riscv/shakti_c.h"
#ضمن "qapi/error.h"
#ضمن "qemu/error-report.h"
#ضمن "hw/intc/sifive_plic.h"
#ضمن "hw/intc/riscv_aclint.h"
#ضمن "system/system.h"
#ضمن "hw/core/qdev-properties.h"
#ضمن "system/address-spaces.h"
#ضمن "hw/riscv/boot.h"

ساكن ثابت بنية Memmap_داخلا {
    hwaddr بنياد;
    hwaddr سائيز;
} shakti_c_memmap[] = {
    [SHAKTI_C_ROM]   =  {  0x00001000,  0x2000   },
    [SHAKTI_C_RAM]   =  {  0x80000000,  0x0      },
    [SHAKTI_C_UART]  =  {  0x00011300,  0x00040  },
    [SHAKTI_C_GPIO]  =  {  0x020d0000,  0x00100  },
    [SHAKTI_C_PLIC]  =  {  0x0c000000,  0x20000  },
    [SHAKTI_C_CLINT] =  {  0x02000000,  0xc0000  },
    [SHAKTI_C_I2C]   =  {  0x20c00000,  0x00100  },
};

ساكن فراغ shakti_c_machine_state_init(MachineState *mstate)
{
    ShaktiCMachineState *sms = RISCV_SHAKTI_MACHINE(mstate);
    يادگيري_Region *system_يادگيري = get_system_يادگيري();
    RISCVBootInfo boot_info;
    hwaddr firmware_ڀريو_addr = shakti_c_memmap[SHAKTI_C_RAM].بنياد;

    /* Initialize SoC */
    object_initialize_ٻار(OBJECT(mstate), "soc", &sms->soc,
                            TYPE_RISCV_SHAKTI_SOC);
    qdev_realize(DEVICE(&sms->soc), عدم, &error_abort);

    /* register RAM */
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
                                shakti_c_memmap[SHAKTI_C_RAM].بنياد,
                                mstate->ram);

    riscv_boot_info_init(&boot_info, &sms->soc.cpus);

    إذا (mstate->firmware) {
        riscv_ڀريو_firmware(mstate, &boot_info, mstate->firmware,
                            &firmware_ڀريو_addr, عدم);
    }

    /* ROM reset vector */
    riscv_تياري_rom_reset_vec(mstate, &sms->soc.cpus, firmware_ڀريو_addr,
                              shakti_c_memmap[SHAKTI_C_ROM].بنياد,
                              shakti_c_memmap[SHAKTI_C_ROM].سائيز, 0, 0);
}

ساكن فراغ shakti_c_machine_instance_init(Object *obj)
{
}

ساكن فراغ shakti_c_machine_class_init(ObjectClass *klass, ثابت فراغ *معلومات)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    ساكن ثابت حرف * ثابت valid_cpu_types[] = {
        RISCV_CPU_TYPE_NAME("shakti-c"),
        عدم
    };

    mc->desc = "RISC-V Board compatible with Shakti SDK";
    mc->init = shakti_c_machine_state_init;
    mc->default_cpu_type = TYPE_RISCV_CPU_SHAKTI_C;
    mc->valid_cpu_types = valid_cpu_types;
    mc->deprecation_reason = "Currently unmaintained with no known users";
    mc->default_ram_id = "riscv.shakti.c.ram";
}

ساكن ثابت TypeInfo shakti_c_machine_type_info = {
    .name = TYPE_RISCV_SHAKTI_MACHINE,
    .والد = TYPE_MACHINE,
    .class_init = shakti_c_machine_class_init,
    .instance_init = shakti_c_machine_instance_init,
    .instance_سائيز = حجم(ShaktiCMachineState),
};

ساكن فراغ shakti_c_machine_type_info_رجسٽر(فراغ)
{
    type_رجسٽر_static(&shakti_c_machine_type_info);
}
type_init(shakti_c_machine_type_info_رجسٽر)

ساكن فراغ shakti_c_soc_state_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(dev);
    يادگيري_Region *system_يادگيري = get_system_يادگيري();

    sysbus_realize(SYS_BUS_DEVICE(&sss->cpus), &error_abort);

    sss->plic = sifive_plic_create(system_يادگيري,
        shakti_c_memmap[SHAKTI_C_PLIC].بنياد,
        (حرف *)SHAKTI_C_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SHAKTI_C_PLIC_انگ_SOURCES,
        SHAKTI_C_PLIC_انگ_PRIORITIES,
        SHAKTI_C_PLIC_ترجيح_بنياد,
        SHAKTI_C_PLIC_PENDING_بنياد,
        SHAKTI_C_PLIC_ENABLE_بنياد,
        SHAKTI_C_PLIC_ENABLE_STRIDE,
        SHAKTI_C_PLIC_CONTEXT_بنياد,
        SHAKTI_C_PLIC_CONTEXT_STRIDE,
        shakti_c_memmap[SHAKTI_C_PLIC].سائيز);

    riscv_aclint_swi_create(system_يادگيري,
        shakti_c_memmap[SHAKTI_C_CLINT].بنياد,
        0, 1, false);
    riscv_aclint_mtimer_create(system_يادگيري,
        shakti_c_memmap[SHAKTI_C_CLINT].بنياد + RISCV_ACLINT_SWI_سائيز,
        RISCV_ACLINT_DEFAULT_MTIMER_سائيز, 0, 1,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);

    qdev_prop_سيٽ_chr(DEVICE(&(sss->uart)), "chardev", serial_hd(0));
    إذا (!sysbus_realize(SYS_BUS_DEVICE(&sss->uart), errp)) {
        أرجع;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&sss->uart), 0,
                    shakti_c_memmap[SHAKTI_C_UART].بنياد);

    /* ROM */
    يادگيري_region_init_rom(&sss->rom, OBJECT(dev), "riscv.shakti.c.rom",
                           shakti_c_memmap[SHAKTI_C_ROM].سائيز, &error_fatal);
    يادگيري_region_جوڙيو_subregion(system_يادگيري,
        shakti_c_memmap[SHAKTI_C_ROM].بنياد, &sss->rom);
}

ساكن فراغ shakti_c_soc_class_init(ObjectClass *klass, ثابت فراغ *معلومات)
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

ساكن فراغ shakti_c_soc_instance_init(Object *obj)
{
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(obj);

    object_initialize_ٻار(obj, "cpus", &sss->cpus, TYPE_RISCV_HART_ARRAY);
    object_initialize_ٻار(obj, "uart", &sss->uart, TYPE_SHAKTI_UART);

    /*
     * CPU type is fixed and we are not supporting passing from commandline yet.
     * So let it be in instance_init. When supported should use ms->cpu_type
     * instead of TYPE_RISCV_CPU_SHAKTI_C
     */
    object_property_سيٽ_str(OBJECT(&sss->cpus), "cpu-type",
                            TYPE_RISCV_CPU_SHAKTI_C, &error_abort);
    object_property_سيٽ_int(OBJECT(&sss->cpus), "num-harts", 1,
                            &error_abort);
}

ساكن ثابت TypeInfo shakti_c_type_info = {
    .name = TYPE_RISCV_SHAKTI_SOC,
    .والد = TYPE_DEVICE,
    .class_init = shakti_c_soc_class_init,
    .instance_init = shakti_c_soc_instance_init,
    .instance_سائيز = حجم(ShaktiCSoCState),
};

ساكن فراغ shakti_c_type_info_رجسٽر(فراغ)
{
    type_رجسٽر_static(&shakti_c_type_info);
}
type_init(shakti_c_type_info_رجسٽر)
