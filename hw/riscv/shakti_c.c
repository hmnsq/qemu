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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/riscv/shakti_c.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "qemu/error-report.h"
#అన్తర్భావ "hw/intc/sifive_plic.h"
#అన్తర్భావ "hw/intc/riscv_aclint.h"
#అన్తర్భావ "system/system.h"
#అన్తర్భావ "hw/core/qdev-properties.h"
#అన్తర్భావ "system/address-spaces.h"
#అన్తర్భావ "hw/riscv/boot.h"

స్థిర స్థిరరాశి నిర్మాణం Memmap_నమోదు {
    hwaddr పునాది;
    hwaddr కొలత;
} shakti_c_memmap[] = {
    [SHAKTI_C_ROM]   =  {  0x00001000,  0x2000   },
    [SHAKTI_C_RAM]   =  {  0x80000000,  0x0      },
    [SHAKTI_C_UART]  =  {  0x00011300,  0x00040  },
    [SHAKTI_C_GPIO]  =  {  0x020d0000,  0x00100  },
    [SHAKTI_C_PLIC]  =  {  0x0c000000,  0x20000  },
    [SHAKTI_C_CLINT] =  {  0x02000000,  0xc0000  },
    [SHAKTI_C_I2C]   =  {  0x20c00000,  0x00100  },
};

స్థిర శూన్య shakti_c_machine_state_init(MachineState *mstate)
{
    ShaktiCMachineState *sms = RISCV_SHAKTI_MACHINE(mstate);
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();
    RISCVBootInfo boot_info;
    hwaddr firmware_ఎక్కించు_addr = shakti_c_memmap[SHAKTI_C_RAM].పునాది;

    /* Initialize SoC */
    object_initialize_బిడ్డ(OBJECT(mstate), "soc", &sms->soc,
                            TYPE_RISCV_SHAKTI_SOC);
    qdev_realize(DEVICE(&sms->soc), ఖాళీ, &error_abort);

    /* register RAM */
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
                                shakti_c_memmap[SHAKTI_C_RAM].పునాది,
                                mstate->ram);

    riscv_boot_info_init(&boot_info, &sms->soc.cpus);

    ఒకవేళ (mstate->firmware) {
        riscv_ఎక్కించు_firmware(mstate, &boot_info, mstate->firmware,
                            &firmware_ఎక్కించు_addr, ఖాళీ);
    }

    /* ROM reset vector */
    riscv_అమరిక_rom_reset_vec(mstate, &sms->soc.cpus, firmware_ఎక్కించు_addr,
                              shakti_c_memmap[SHAKTI_C_ROM].పునాది,
                              shakti_c_memmap[SHAKTI_C_ROM].కొలత, 0, 0);
}

స్థిర శూన్య shakti_c_machine_instance_init(Object *obj)
{
}

స్థిర శూన్య shakti_c_machine_class_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
{
    MachineClass *mc = MACHINE_CLASS(klass);
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి valid_cpu_types[] = {
        RISCV_CPU_TYPE_NAME("shakti-c"),
        ఖాళీ
    };

    mc->desc = "RISC-V Board compatible with Shakti SDK";
    mc->init = shakti_c_machine_state_init;
    mc->default_cpu_type = TYPE_RISCV_CPU_SHAKTI_C;
    mc->valid_cpu_types = valid_cpu_types;
    mc->deprecation_reason = "Currently unmaintained with no known users";
    mc->default_ram_id = "riscv.shakti.c.ram";
}

స్థిర స్థిరరాశి TypeInfo shakti_c_machine_type_info = {
    .name = TYPE_RISCV_SHAKTI_MACHINE,
    .మాతృ = TYPE_MACHINE,
    .class_init = shakti_c_machine_class_init,
    .instance_init = shakti_c_machine_instance_init,
    .instance_కొలత = పరిమాణం(ShaktiCMachineState),
};

స్థిర శూన్య shakti_c_machine_type_info_రిజిస్టరు(శూన్య)
{
    type_రిజిస్టరు_static(&shakti_c_machine_type_info);
}
type_init(shakti_c_machine_type_info_రిజిస్టరు)

స్థిర శూన్య shakti_c_soc_state_realize(DeviceState *dev, Error **errp)
{
    MachineState *ms = MACHINE(qdev_get_machine());
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(dev);
    జ్ఞాపకం_Region *system_జ్ఞాపకం = get_system_జ్ఞాపకం();

    sysbus_realize(SYS_BUS_DEVICE(&sss->cpus), &error_abort);

    sss->plic = sifive_plic_create(system_జ్ఞాపకం,
        shakti_c_memmap[SHAKTI_C_PLIC].పునాది,
        (అక్షరం *)SHAKTI_C_PLIC_HART_CONFIG, ms->smp.cpus, 0,
        SHAKTI_C_PLIC_అంకె_SOURCES,
        SHAKTI_C_PLIC_అంకె_PRIORITIES,
        SHAKTI_C_PLIC_ప్రాధాన్యత_పునాది,
        SHAKTI_C_PLIC_PENDING_పునాది,
        SHAKTI_C_PLIC_ENABLE_పునాది,
        SHAKTI_C_PLIC_ENABLE_STRIDE,
        SHAKTI_C_PLIC_CONTEXT_పునాది,
        SHAKTI_C_PLIC_CONTEXT_STRIDE,
        shakti_c_memmap[SHAKTI_C_PLIC].కొలత);

    riscv_aclint_swi_create(system_జ్ఞాపకం,
        shakti_c_memmap[SHAKTI_C_CLINT].పునాది,
        0, 1, false);
    riscv_aclint_mtimer_create(system_జ్ఞాపకం,
        shakti_c_memmap[SHAKTI_C_CLINT].పునాది + RISCV_ACLINT_SWI_కొలత,
        RISCV_ACLINT_DEFAULT_MTIMER_కొలత, 0, 1,
        RISCV_ACLINT_DEFAULT_MTIMECMP, RISCV_ACLINT_DEFAULT_MTIME,
        RISCV_ACLINT_DEFAULT_TIMEBASE_FREQ, false);

    qdev_prop_సమితి_chr(DEVICE(&(sss->uart)), "chardev", serial_hd(0));
    ఒకవేళ (!sysbus_realize(SYS_BUS_DEVICE(&sss->uart), errp)) {
        తిరిగి;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&sss->uart), 0,
                    shakti_c_memmap[SHAKTI_C_UART].పునాది);

    /* ROM */
    జ్ఞాపకం_region_init_rom(&sss->rom, OBJECT(dev), "riscv.shakti.c.rom",
                           shakti_c_memmap[SHAKTI_C_ROM].కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(system_జ్ఞాపకం,
        shakti_c_memmap[SHAKTI_C_ROM].పునాది, &sss->rom);
}

స్థిర శూన్య shakti_c_soc_class_init(ObjectClass *klass, స్థిరరాశి శూన్య *దత్తాంశం)
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

స్థిర శూన్య shakti_c_soc_instance_init(Object *obj)
{
    ShaktiCSoCState *sss = RISCV_SHAKTI_SOC(obj);

    object_initialize_బిడ్డ(obj, "cpus", &sss->cpus, TYPE_RISCV_HART_ARRAY);
    object_initialize_బిడ్డ(obj, "uart", &sss->uart, TYPE_SHAKTI_UART);

    /*
     * CPU type is fixed and we are not supporting passing from commandline yet.
     * So let it be in instance_init. When supported should use ms->cpu_type
     * instead of TYPE_RISCV_CPU_SHAKTI_C
     */
    object_property_సమితి_str(OBJECT(&sss->cpus), "cpu-type",
                            TYPE_RISCV_CPU_SHAKTI_C, &error_abort);
    object_property_సమితి_int(OBJECT(&sss->cpus), "num-harts", 1,
                            &error_abort);
}

స్థిర స్థిరరాశి TypeInfo shakti_c_type_info = {
    .name = TYPE_RISCV_SHAKTI_SOC,
    .మాతృ = TYPE_DEVICE,
    .class_init = shakti_c_soc_class_init,
    .instance_init = shakti_c_soc_instance_init,
    .instance_కొలత = పరిమాణం(ShaktiCSoCState),
};

స్థిర శూన్య shakti_c_type_info_రిజిస్టరు(శూన్య)
{
    type_రిజిస్టరు_static(&shakti_c_type_info);
}
type_init(shakti_c_type_info_రిజిస్టరు)
