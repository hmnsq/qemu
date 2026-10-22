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

#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/osdep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/boards.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/shakti_c.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qapi/error.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "qemu/error-report.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/sifive_plic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/intc/riscv_aclint.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/core/qdev-properties.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "system/address-spaces.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw/riscv/boot.h"

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Memmap_ਇੰਦਰਾਜ {
    hwaddr ਅਧਾਰ;
    hwaddr ਮਾਪ;
} shakti_c_memmap[] = {
    [SHAKTI_C_ROM]   =  {  0x00001000,  0x2000   },
    [SHAKTI_C_RAM]   =  {  0x80000000,  0x0      },
    [SHAKTI_C_UART]  =  {  0x00011300,  0x00040  },
    [SHAKTI_C_GPIO]  =  {  0x020d0000,  0x00100  },
    [SHAKTI_C_PLIC]  =  {  0x0c000000,  0x20000  },
    [SHAKTI_C_CLINT] =  {  0x02000000,  0xc0000  },
    [SHAKTI_C_I2C]   =  {  0x20c00000,  0x00100  },
};

ਸਥਿਰ ਖਾਲੀ shakti_c_machine_state_init(MachineState *mstate)
{
    ShaktiCMachineState *sms = RISCV_SHAKTI_MACHINE(mstate);
    ਸਮ੍ਰਿਤੀ_Region *system_ਸਮ੍ਰਿਤੀ = get_system_ਸਮ੍ਰਿਤੀ();
    RISCVBootInfo boot_info;
    hwaddr firmware_ਲੱਦੋ_addr = shakti_c_memmap[SHAKTI_C_RAM].ਅਧਾਰ;

    /* Initialize SoC */
    object_initialize_ਬੱਚਾ(OBJECT(mstate), "soc", &sms->soc,
                            TYPE_RISCV_SHAKTI_SOC);
    qdev_realize(DEVICE(&sms->soc), ਨਿਰਾ, &error_abort);

    /* register RAM */
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ,
                                shakti_c_memmap[SHAKTI_C_RAM].ਅਧਾਰ,
                                mstate->ram);

    riscv_boot_info_init(&boot_info, &sms->soc.cpus);

    ਜੇ (mstate->firmware) {
        riscv_ਲੱਦੋ_firmware(mstate, &boot_info, mstate->firmware,
                            &firmware_ਲੱਦੋ_addr, ਨਿਰਾ);
    }

    /* ROM reset vector */
    riscv_ਤਿਆਰੀ_rom_reset_vec(mstate, &sms->soc.cpus, firmware_ਲੱਦੋ_addr,
                              shakti_c_memmap[SHAKTI_C_ROM].ਅਧਾਰ,
                              shakti_c_memmap[SHAKTI_C_ROM].ਮਾਪ, 0, 0);
}

ਸਥਿਰ ਖਾਲੀ shakti_c_machine_instance_init(Object *obj)
{
}

ਸਥਿਰ ਖਾਲੀ shakti_c_machine_class_init(ObjectClass *klass, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ valid_cpu_types[] = {
        RISCV_CPU_TYPE_NAME("shakti-c"),
        ਨਿਰਾ
    };

    mc->desc = "RISC-V Board compatible with Shakti SDK";
    mc->init = shakti_c_machine_state_init;
    mc->default_cpu_type = TYPE_RISCV_CPU_SHAKTI_C;
    mc->valid_cpu_types = valid_cpu_types;
    mc->deprecation_reason = "Currently unmaintained with no known users";
    mc->default_ram_id = "riscv.shakti.c.ram";
}

ਸਥਿਰ ਅਚਲ TypeInfo shakti_c_machine_type_info = {
    .name = TYPE_RISCV_SHAKTI_MACHINE,
    .ਮਾਪਾ = TYPE_MACHINE,
    .class_init = shakti_c_machine_class_init,
    .instance_init = shakti_c_machine_instance_init,
    .instance_ਮਾਪ = ਆਕਾਰ(ShaktiCMachineState),
};

ਸਥਿਰ ਖਾਲੀ shakti_c_machine_type_info_ਰਜਿਸਟਰ(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&shakti_c_machine_type_info);
}
type_init(shakti_c_machine_type_info_ਰਜਿਸਟਰ)

ਸਥਿਰ ਖਾਲੀ shakti_c_soc_state_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(dev);
    ਸਮ੍ਰਿਤੀ_Region *system_ਸਮ੍ਰਿਤੀ = get_system_ਸਮ੍ਰਿਤੀ();

    sysbus_realize(SYS_BUS_DEVICE(&sss->cpus), &error_abort);

    sss->plic = sifive_plic_create(system_ਸਮ੍ਰਿਤੀ,
        shakti_c_memmap[SHAKTI_C_PLIC].ਅਧਾਰ,
        (ਅੱਖਰ *)SHAKTI_C_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SHAKTI_C_PLIC_ਅੰਕ_SOURCES,
        SHAKTI_C_PLIC_ਅੰਕ_PRIORITIES,
        SHAKTI_C_PLIC_ਤਰਜੀਹ_ਅਧਾਰ,
        SHAKTI_C_PLIC_PENDING_ਅਧਾਰ,
        SHAKTI_C_PLIC_ENABLE_ਅਧਾਰ,
        SHAKTI_C_PLIC_ENABLE_STRIDE,
        SHAKTI_C_PLIC_CONTEXT_ਅਧਾਰ,
        SHAKTI_C_PLIC_CONTEXT_STRIDE,
        shakti_c_memmap[SHAKTI_C_PLIC].ਮਾਪ);

    riscv_aclint_swi_create(system_ਸਮ੍ਰਿਤੀ,
        shakti_c_memmap[SHAKTI_C_CLINT].ਅਧਾਰ,
        0, 1, false);
    riscv_aclint_mtimer_create(system_ਸਮ੍ਰਿਤੀ,
        shakti_c_memmap[SHAKTI_C_CLINT].ਅਧਾਰ + RISCV_ACLINT_SWI_ਮਾਪ,
        RISCV_ACLINT_DEFAULT_MTIMER_ਮਾਪ, 0, 1,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);

    qdev_prop_ਸੈੱਟ_chr(DEVICE(&(sss->uart)), "chardev", serial_hd(0));
    ਜੇ (!sysbus_realize(SYS_BUS_DEVICE(&sss->uart), errp)) {
        ਵਾਪਸ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&sss->uart), 0,
                    shakti_c_memmap[SHAKTI_C_UART].ਅਧਾਰ);

    /* ROM */
    ਸਮ੍ਰਿਤੀ_region_init_rom(&sss->rom, OBJECT(dev), "riscv.shakti.c.rom",
                           shakti_c_memmap[SHAKTI_C_ROM].ਮਾਪ, &error_fatal);
    ਸਮ੍ਰਿਤੀ_region_ਜੋੜੋ_subregion(system_ਸਮ੍ਰਿਤੀ,
        shakti_c_memmap[SHAKTI_C_ROM].ਅਧਾਰ, &sss->rom);
}

ਸਥਿਰ ਖਾਲੀ shakti_c_soc_class_init(ObjectClass *klass, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ)
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

ਸਥਿਰ ਖਾਲੀ shakti_c_soc_instance_init(Object *obj)
{
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(obj);

    object_initialize_ਬੱਚਾ(obj, "cpus", &sss->cpus, TYPE_RISCV_HART_ARRAY);
    object_initialize_ਬੱਚਾ(obj, "uart", &sss->uart, TYPE_SHAKTI_UART);

    /*
     * CPU type is fixed and we are not supporting passing from commandline yet.
     * So let it be in instance_init. When supported should use ms->cpu_type
     * instead of TYPE_RISCV_CPU_SHAKTI_C
     */
    object_property_ਸੈੱਟ_str(OBJECT(&sss->cpus), "cpu-type",
                            TYPE_RISCV_CPU_SHAKTI_C, &error_abort);
    object_property_ਸੈੱਟ_int(OBJECT(&sss->cpus), "num-harts", 1,
                            &error_abort);
}

ਸਥਿਰ ਅਚਲ TypeInfo shakti_c_type_info = {
    .name = TYPE_RISCV_SHAKTI_SOC,
    .ਮਾਪਾ = TYPE_DEVICE,
    .class_init = shakti_c_soc_class_init,
    .instance_init = shakti_c_soc_instance_init,
    .instance_ਮਾਪ = ਆਕਾਰ(ShaktiCSoCState),
};

ਸਥਿਰ ਖਾਲੀ shakti_c_type_info_ਰਜਿਸਟਰ(ਖਾਲੀ)
{
    type_ਰਜਿਸਟਰ_static(&shakti_c_type_info);
}
type_init(shakti_c_type_info_ਰਜਿਸਟਰ)
