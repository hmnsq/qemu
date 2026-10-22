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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/shakti_c.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/sifive_plic.h"
#ಅನ್ತರ್ಭಾವ "hw/intc/riscv_aclint.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "hw/core/qdev-properties.h"
#ಅನ್ತರ್ಭಾವ "system/address-spaces.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Memmap_ನಮೂದು {
    hwaddr ತಳ;
    hwaddr ಗಾತ್ರ_2;
} shakti_c_memmap[] = {
    [SHAKTI_C_ROM]   =  {  0x00001000,  0x2000   },
    [SHAKTI_C_RAM]   =  {  0x80000000,  0x0      },
    [SHAKTI_C_UART]  =  {  0x00011300,  0x00040  },
    [SHAKTI_C_GPIO]  =  {  0x020d0000,  0x00100  },
    [SHAKTI_C_PLIC]  =  {  0x0c000000,  0x20000  },
    [SHAKTI_C_CLINT] =  {  0x02000000,  0xc0000  },
    [SHAKTI_C_I2C]   =  {  0x20c00000,  0x00100  },
};

ಸ್ಥಿರ ಶೂನ್ಯ shakti_c_machine_state_init(MachineState *mstate)
{
    ShaktiCMachineState *sms = RISCV_SHAKTI_MACHINE(mstate);
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();
    RISCVBootInfo boot_info;
    hwaddr firmware_ತುಂಬು_addr = shakti_c_memmap[SHAKTI_C_RAM].ತಳ;

    /* Initialize SoC */
    object_initialize_ಮಗು(OBJECT(mstate), "soc", &sms->soc,
                            TYPE_RISCV_SHAKTI_SOC);
    qdev_realize(DEVICE(&sms->soc), ಖಾಲಿ, &error_abort);

    /* register RAM */
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
                                shakti_c_memmap[SHAKTI_C_RAM].ತಳ,
                                mstate->ram);

    riscv_boot_info_init(&boot_info, &sms->soc.cpus);

    ಆದರೆ (mstate->firmware) {
        riscv_ತುಂಬು_firmware(mstate, &boot_info, mstate->firmware,
                            &firmware_ತುಂಬು_addr, ಖಾಲಿ);
    }

    /* ROM reset vector */
    riscv_ಸಿದ್ಧತೆ_rom_reset_vec(mstate, &sms->soc.cpus, firmware_ತುಂಬು_addr,
                              shakti_c_memmap[SHAKTI_C_ROM].ತಳ,
                              shakti_c_memmap[SHAKTI_C_ROM].ಗಾತ್ರ_2, 0, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ shakti_c_machine_instance_init(Object *obj)
{
}

ಸ್ಥಿರ ಶೂನ್ಯ shakti_c_machine_class_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ valid_cpu_types[] = {
        RISCV_CPU_TYPE_NAME("shakti-c"),
        ಖಾಲಿ
    };

    mc->desc = "RISC-V Board compatible with Shakti SDK";
    mc->init = shakti_c_machine_state_init;
    mc->default_cpu_type = TYPE_RISCV_CPU_SHAKTI_C;
    mc->valid_cpu_types = valid_cpu_types;
    mc->deprecation_reason = "Currently unmaintained with no known users";
    mc->default_ram_id = "riscv.shakti.c.ram";
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo shakti_c_machine_type_info = {
    .name = TYPE_RISCV_SHAKTI_MACHINE,
    .ಪೋಷಕ = TYPE_MACHINE,
    .class_init = shakti_c_machine_class_init,
    .instance_init = shakti_c_machine_instance_init,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(ShaktiCMachineState),
};

ಸ್ಥಿರ ಶೂನ್ಯ shakti_c_machine_type_info_ರಿಜಿಸ್ಟರ್(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&shakti_c_machine_type_info);
}
type_init(shakti_c_machine_type_info_ರಿಜಿಸ್ಟರ್)

ಸ್ಥಿರ ಶೂನ್ಯ shakti_c_soc_state_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(dev);
    ಸ್ಮರಣೆ_Region *system_ಸ್ಮರಣೆ = get_system_ಸ್ಮರಣೆ();

    sysbus_realize(SYS_BUS_DEVICE(&sss->cpus), &error_abort);

    sss->plic = sifive_plic_create(system_ಸ್ಮರಣೆ,
        shakti_c_memmap[SHAKTI_C_PLIC].ತಳ,
        (ಅಕ್ಷರ *)SHAKTI_C_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SHAKTI_C_PLIC_ಅಂಕೆ_SOURCES,
        SHAKTI_C_PLIC_ಅಂಕೆ_PRIORITIES,
        SHAKTI_C_PLIC_ಆದ್ಯತೆ_ತಳ,
        SHAKTI_C_PLIC_PENDING_ತಳ,
        SHAKTI_C_PLIC_ENABLE_ತಳ,
        SHAKTI_C_PLIC_ENABLE_STRIDE,
        SHAKTI_C_PLIC_CONTEXT_ತಳ,
        SHAKTI_C_PLIC_CONTEXT_STRIDE,
        shakti_c_memmap[SHAKTI_C_PLIC].ಗಾತ್ರ_2);

    riscv_aclint_swi_create(system_ಸ್ಮರಣೆ,
        shakti_c_memmap[SHAKTI_C_CLINT].ತಳ,
        0, 1, false);
    riscv_aclint_mtimer_create(system_ಸ್ಮರಣೆ,
        shakti_c_memmap[SHAKTI_C_CLINT].ತಳ + RISCV_ACLINT_SWI_ಗಾತ್ರ,
        RISCV_ACLINT_DEFAULT_MTIMER_ಗಾತ್ರ, 0, 1,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);

    qdev_prop_ಗಣ_chr(DEVICE(&(sss->uart)), "chardev", serial_hd(0));
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&sss->uart), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&sss->uart), 0,
                    shakti_c_memmap[SHAKTI_C_UART].ತಳ);

    /* ROM */
    ಸ್ಮರಣೆ_region_init_rom(&sss->rom, OBJECT(dev), "riscv.shakti.c.rom",
                           shakti_c_memmap[SHAKTI_C_ROM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(system_ಸ್ಮರಣೆ,
        shakti_c_memmap[SHAKTI_C_ROM].ತಳ, &sss->rom);
}

ಸ್ಥಿರ ಶೂನ್ಯ shakti_c_soc_class_init(ObjectClass *klass, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
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

ಸ್ಥಿರ ಶೂನ್ಯ shakti_c_soc_instance_init(Object *obj)
{
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(obj);

    object_initialize_ಮಗು(obj, "cpus", &sss->cpus, TYPE_RISCV_HART_ARRAY);
    object_initialize_ಮಗು(obj, "uart", &sss->uart, TYPE_SHAKTI_UART);

    /*
     * CPU type is fixed and we are not supporting passing from commandline yet.
     * So let it be in instance_init. When supported should use ms->cpu_type
     * instead of TYPE_RISCV_CPU_SHAKTI_C
     */
    object_property_ಗಣ_str(OBJECT(&sss->cpus), "cpu-type",
                            TYPE_RISCV_CPU_SHAKTI_C, &error_abort);
    object_property_ಗಣ_int(OBJECT(&sss->cpus), "num-harts", 1,
                            &error_abort);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo shakti_c_type_info = {
    .name = TYPE_RISCV_SHAKTI_SOC,
    .ಪೋಷಕ = TYPE_DEVICE,
    .class_init = shakti_c_soc_class_init,
    .instance_init = shakti_c_soc_instance_init,
    .instance_ಗಾತ್ರ = ಗಾತ್ರ(ShaktiCSoCState),
};

ಸ್ಥಿರ ಶೂನ್ಯ shakti_c_type_info_ರಿಜಿಸ್ಟರ್(ಶೂನ್ಯ)
{
    type_ರಿಜಿಸ್ಟರ್_static(&shakti_c_type_info);
}
type_init(shakti_c_type_info_ರಿಜಿಸ್ಟರ್)
