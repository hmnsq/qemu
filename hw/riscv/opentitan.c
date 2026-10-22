/*
 * QEMU RISC-V Board Compatible with OpenTitan FPGA platform
 *
 * Copyright (c) 2020 Western Digital
 *
 * Provides a board compatible with the OpenTitan FPGA platform:
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
#अन्तर्भाव "hw/riscv/opentitan.h"
#अन्तर्भाव "qapi/error.h"
#अन्तर्भाव "qemu/error-report.h"
#अन्तर्भाव "hw/core/boards.h"
#अन्तर्भाव "hw/misc/unimp.h"
#अन्तर्भाव "hw/riscv/boot.h"
#अन्तर्भाव "qemu/units.h"
#अन्तर्भाव "system/system.h"
#अन्तर्भाव "system/address-spaces.h"

/*
 * This version of the OpenTitan machine currently supports
 * OpenTitan RTL version:
 * <lowRISC/opentitan@565e4af39760a123c59a184aa2f5812a961fde47>
 *
 * MMIO mapping as per (specified commit):
 * lowRISC/opentitan: hw/top_earlgrey/sw/autogen/top_earlgrey_memory.h
 */
स्थिर ध्रुव MemMapEntry ibex_memmap[] = {
    [IBEX_DEV_ROM] =            {  0x00008000,  0x8000      },
    [IBEX_DEV_RAM] =            {  0x10000000,  0x20000     },
    [IBEX_DEV_FLASH] =          {  0x20000000,  0x100000    },
    [IBEX_DEV_UART] =           {  0x40000000,  0x40        },
    [IBEX_DEV_GPIO] =           {  0x40040000,  0x40        },
    [IBEX_DEV_SPI_DEVICE] =     {  0x40050000,  0x2000      },
    [IBEX_DEV_I2C] =            {  0x40080000,  0x80        },
    [IBEX_DEV_PATTGEN] =        {  0x400e0000,  0x40        },
    [IBEX_DEV_TIMER] =          {  0x40100000,  0x200       },
    [IBEX_DEV_OTP_CTRL] =       {  0x40130000,  0x2000      },
    [IBEX_DEV_LC_CTRL] =        {  0x40140000,  0x100       },
    [IBEX_DEV_ALERT_HANDLER] =  {  0x40150000,  0x800       },
    [IBEX_DEV_SPI_HOST0] =      {  0x40300000,  0x40        },
    [IBEX_DEV_SPI_HOST1] =      {  0x40310000,  0x40        },
    [IBEX_DEV_USBDEV] =         {  0x40320000,  0x1000      },
    [IBEX_DEV_PWRMGR] =         {  0x40400000,  0x80        },
    [IBEX_DEV_RSTMGR] =         {  0x40410000,  0x80        },
    [IBEX_DEV_CLKMGR] =         {  0x40420000,  0x80        },
    [IBEX_DEV_PINMUX] =         {  0x40460000,  0x1000      },
    [IBEX_DEV_AON_TIMER] =      {  0x40470000,  0x40        },
    [IBEX_DEV_SENSOR_CTRL] =    {  0x40490000,  0x40        },
    [IBEX_DEV_FLASH_CTRL] =     {  0x41000000,  0x200       },
    [IBEX_DEV_AES] =            {  0x41100000,  0x100       },
    [IBEX_DEV_HMAC] =           {  0x41110000,  0x1000      },
    [IBEX_DEV_KMAC] =           {  0x41120000,  0x1000      },
    [IBEX_DEV_OTBN] =           {  0x41130000,  0x10000     },
    [IBEX_DEV_KEYMGR] =         {  0x41140000,  0x100       },
    [IBEX_DEV_CSRNG] =          {  0x41150000,  0x80        },
    [IBEX_DEV_ENTROPY] =        {  0x41160000,  0x100       },
    [IBEX_DEV_EDNO] =           {  0x41170000,  0x80        },
    [IBEX_DEV_EDN1] =           {  0x41180000,  0x80        },
    [IBEX_DEV_SRAM_CTRL] =      {  0x411c0000,  0x20        },
    [IBEX_DEV_IBEX_CFG] =       {  0x411f0000,  0x100       },
    [IBEX_DEV_PLIC] =           {  0x48000000,  0x8000000   },
    [IBEX_DEV_FLASH_VIRTUAL] =  {  0x80000000,  0x80000     },
};

स्थिर शून्य opentitan_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    OpenTitanState *s = OPENTITAN_MACHINE(machine);
    ध्रुव MemMapEntry *memmap = ibex_memmap;
    MemoryRegion *sys_mem = get_system_memory();
    RISCVBootInfo boot_info;

    सचे (machine->ram_size != mc->default_ram_size) {
        वर्ण *sz = size_to_str(mc->default_ram_size);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        exit(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_child(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_IBEX_SOC);
    qdev_realize(DEVICE(&s->soc), रिक्त, &error_fatal);

    memory_region_add_subregion(sys_mem,
        memmap[IBEX_DEV_RAM].base, machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    सचे (machine->firmware) {
        hwaddr firmware_load_addr = memmap[IBEX_DEV_RAM].base;
        riscv_load_firmware(machine, &boot_info, machine->firmware,
                            &firmware_load_addr, रिक्त);
    }

    सचे (machine->kernel_filename) {
        riscv_load_kernel(machine, &boot_info,
                          memmap[IBEX_DEV_RAM].base,
                          false, रिक्त);
    }
}

स्थिर शून्य opentitan_machine_class_init(ObjectClass *oc, ध्रुव शून्य *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with OpenTitan";
    mc->init = opentitan_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_IBEX;
    mc->default_ram_id = "riscv.lowrisc.ibex.ram";
    mc->default_ram_size = ibex_memmap[IBEX_DEV_RAM].size;
}

स्थिर शून्य lowrisc_ibex_soc_init(Object *obj)
{
    नीचः_RISCIbex_So_CState *s = RISCV_IBEX_SOC(obj);

    object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);

    object_initialize_child(obj, "plic", &s->plic, TYPE_SIFIVE_PLIC);

    object_initialize_child(obj, "uart", &s->uart, TYPE_IBEX_UART);

    object_initialize_child(obj, "timer", &s->timer, TYPE_IBEX_TIMER);

    अत्थाय (पूर्णाङ्क i = 0; i < OPENTITAN_अङ्कः_SPI_HOSTS; i++) {
        object_initialize_child(obj, "spi_host[*]", &s->spi_host[i],
                                TYPE_IBEX_SPI_HOST);
    }
}

स्थिर शून्य lowrisc_ibex_soc_realize(DeviceState *dev_soc, Error **errp)
{
    ध्रुव MemMapEntry *memmap = ibex_memmap;
    DeviceState *dev;
    SysBusDevice *busdev;
    MachineState *ms = MACHINE(qdev_get_machine());
    नीचः_RISCIbex_So_CState *s = RISCV_IBEX_SOC(dev_soc);
    MemoryRegion *sys_mem = get_system_memory();
    पूर्णाङ्क i;

    object_property_set_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    object_property_set_int(OBJECT(&s->cpus), "resetvec", s->resetvec,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Boot ROM */
    memory_region_init_rom(&s->rom, OBJECT(dev_soc), "riscv.lowrisc.ibex.rom",
                           memmap[IBEX_DEV_ROM].size, &error_fatal);
    memory_region_add_subregion(sys_mem,
        memmap[IBEX_DEV_ROM].base, &s->rom);

    /* Flash memory */
    memory_region_init_rom(&s->flash_mem, OBJECT(dev_soc), "riscv.lowrisc.ibex.flash",
                           memmap[IBEX_DEV_FLASH].size, &error_fatal);
    memory_region_init_alias(&s->flash_alias, OBJECT(dev_soc),
                             "riscv.lowrisc.ibex.flash_virtual", &s->flash_mem, 0,
                             memmap[IBEX_DEV_FLASH_VIRTUAL].size);
    memory_region_add_subregion(sys_mem, memmap[IBEX_DEV_FLASH].base,
                                &s->flash_mem);
    memory_region_add_subregion(sys_mem, memmap[IBEX_DEV_FLASH_VIRTUAL].base,
                                &s->flash_alias);

    /* PLIC */
    qdev_prop_set_string(DEVICE(&s->plic), "hart-config", "M");
    qdev_prop_set_uint32(DEVICE(&s->plic), "num-sources", 180);
    qdev_prop_set_uint32(DEVICE(&s->plic), "num-priorities", 3);
    qdev_prop_set_uint32(DEVICE(&s->plic), "pending-base", 0x1000);
    qdev_prop_set_uint32(DEVICE(&s->plic), "enable-base", 0x2000);
    qdev_prop_set_uint32(DEVICE(&s->plic), "enable-stride", 32);
    qdev_prop_set_uint32(DEVICE(&s->plic), "context-base", 0x200000);
    qdev_prop_set_uint32(DEVICE(&s->plic), "context-stride", 8);
    qdev_prop_set_uint32(DEVICE(&s->plic), "aperture-size", memmap[IBEX_DEV_PLIC].size);

    सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->plic), errp)) {
        पटिनिवत्त;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->plic), 0, memmap[IBEX_DEV_PLIC].base);

    अत्थाय (i = 0; i < ms->smp.cpus; i++) {
        CPUState *cpu = qemu_get_cpu(i);

        qdev_connect_gpio_out(DEVICE(&s->plic), ms->smp.cpus + i,
                              qdev_get_gpio_in(DEVICE(cpu), IRQ_M_EXT));
    }

    /* UART */
    qdev_prop_set_chr(DEVICE(&(s->uart)), "chardev", serial_hd(0));
    सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        पटिनिवत्त;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, memmap[IBEX_DEV_UART].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       0, qdev_get_gpio_in(DEVICE(&s->plic),
                       IBEX_UART0_TX_WATERMARK_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       1, qdev_get_gpio_in(DEVICE(&s->plic),
                       IBEX_UART0_RX_WATERMARK_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       2, qdev_get_gpio_in(DEVICE(&s->plic),
                       IBEX_UART0_TX_EMPTY_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       3, qdev_get_gpio_in(DEVICE(&s->plic),
                       IBEX_UART0_RX_OVERFLOW_IRQ));

    सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->timer), errp)) {
        पटिनिवत्त;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->timer), 0, memmap[IBEX_DEV_TIMER].base);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->timer),
                       0, qdev_get_gpio_in(DEVICE(&s->plic),
                       IBEX_TIMER_TIMEREXPIRED0_0));
    qdev_connect_gpio_out(DEVICE(&s->timer), 0,
                          qdev_get_gpio_in(DEVICE(qemu_get_cpu(0)),
                                           IRQ_M_TIMER));

    /* SPI-Hosts */
    अत्थाय (i = 0; i < OPENTITAN_अङ्कः_SPI_HOSTS; ++i) {
        dev = DEVICE(&(s->spi_host[i]));
        सचे (!sysbus_realize(SYS_BUS_DEVICE(&s->spi_host[i]), errp)) {
            पटिनिवत्त;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, memmap[IBEX_DEV_SPI_HOST0 + i].base);

        विकल्प (i) {
        पक्ष OPENTITAN_SPI_HOST0:
            sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(DEVICE(&s->plic),
                                IBEX_SPI_HOST0_ERR_IRQ));
            sysbus_connect_irq(busdev, 1, qdev_get_gpio_in(DEVICE(&s->plic),
                                IBEX_SPI_HOST0_SPI_EVENT_IRQ));
            भङ्ग;
        पक्ष OPENTITAN_SPI_HOST1:
            sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(DEVICE(&s->plic),
                                IBEX_SPI_HOST1_ERR_IRQ));
            sysbus_connect_irq(busdev, 1, qdev_get_gpio_in(DEVICE(&s->plic),
                                IBEX_SPI_HOST1_SPI_EVENT_IRQ));
            भङ्ग;
        }
    }

    create_unimplemented_device("riscv.lowrisc.ibex.gpio",
        memmap[IBEX_DEV_GPIO].base, memmap[IBEX_DEV_GPIO].size);
    create_unimplemented_device("riscv.lowrisc.ibex.spi_device",
        memmap[IBEX_DEV_SPI_DEVICE].base, memmap[IBEX_DEV_SPI_DEVICE].size);
    create_unimplemented_device("riscv.lowrisc.ibex.i2c",
        memmap[IBEX_DEV_I2C].base, memmap[IBEX_DEV_I2C].size);
    create_unimplemented_device("riscv.lowrisc.ibex.pattgen",
        memmap[IBEX_DEV_PATTGEN].base, memmap[IBEX_DEV_PATTGEN].size);
    create_unimplemented_device("riscv.lowrisc.ibex.sensor_ctrl",
        memmap[IBEX_DEV_SENSOR_CTRL].base, memmap[IBEX_DEV_SENSOR_CTRL].size);
    create_unimplemented_device("riscv.lowrisc.ibex.otp_ctrl",
        memmap[IBEX_DEV_OTP_CTRL].base, memmap[IBEX_DEV_OTP_CTRL].size);
    create_unimplemented_device("riscv.lowrisc.ibex.lc_ctrl",
        memmap[IBEX_DEV_LC_CTRL].base, memmap[IBEX_DEV_LC_CTRL].size);
    create_unimplemented_device("riscv.lowrisc.ibex.pwrmgr",
        memmap[IBEX_DEV_PWRMGR].base, memmap[IBEX_DEV_PWRMGR].size);
    create_unimplemented_device("riscv.lowrisc.ibex.rstmgr",
        memmap[IBEX_DEV_RSTMGR].base, memmap[IBEX_DEV_RSTMGR].size);
    create_unimplemented_device("riscv.lowrisc.ibex.clkmgr",
        memmap[IBEX_DEV_CLKMGR].base, memmap[IBEX_DEV_CLKMGR].size);
    create_unimplemented_device("riscv.lowrisc.ibex.pinmux",
        memmap[IBEX_DEV_PINMUX].base, memmap[IBEX_DEV_PINMUX].size);
    create_unimplemented_device("riscv.lowrisc.ibex.aon_timer",
        memmap[IBEX_DEV_AON_TIMER].base, memmap[IBEX_DEV_AON_TIMER].size);
    create_unimplemented_device("riscv.lowrisc.ibex.usbdev",
        memmap[IBEX_DEV_USBDEV].base, memmap[IBEX_DEV_USBDEV].size);
    create_unimplemented_device("riscv.lowrisc.ibex.flash_ctrl",
        memmap[IBEX_DEV_FLASH_CTRL].base, memmap[IBEX_DEV_FLASH_CTRL].size);
    create_unimplemented_device("riscv.lowrisc.ibex.aes",
        memmap[IBEX_DEV_AES].base, memmap[IBEX_DEV_AES].size);
    create_unimplemented_device("riscv.lowrisc.ibex.hmac",
        memmap[IBEX_DEV_HMAC].base, memmap[IBEX_DEV_HMAC].size);
    create_unimplemented_device("riscv.lowrisc.ibex.kmac",
        memmap[IBEX_DEV_KMAC].base, memmap[IBEX_DEV_KMAC].size);
    create_unimplemented_device("riscv.lowrisc.ibex.keymgr",
        memmap[IBEX_DEV_KEYMGR].base, memmap[IBEX_DEV_KEYMGR].size);
    create_unimplemented_device("riscv.lowrisc.ibex.csrng",
        memmap[IBEX_DEV_CSRNG].base, memmap[IBEX_DEV_CSRNG].size);
    create_unimplemented_device("riscv.lowrisc.ibex.entropy",
        memmap[IBEX_DEV_ENTROPY].base, memmap[IBEX_DEV_ENTROPY].size);
    create_unimplemented_device("riscv.lowrisc.ibex.edn0",
        memmap[IBEX_DEV_EDNO].base, memmap[IBEX_DEV_EDNO].size);
    create_unimplemented_device("riscv.lowrisc.ibex.edn1",
        memmap[IBEX_DEV_EDN1].base, memmap[IBEX_DEV_EDN1].size);
    create_unimplemented_device("riscv.lowrisc.ibex.alert_handler",
        memmap[IBEX_DEV_ALERT_HANDLER].base, memmap[IBEX_DEV_ALERT_HANDLER].size);
    create_unimplemented_device("riscv.lowrisc.ibex.sram_ctrl",
        memmap[IBEX_DEV_SRAM_CTRL].base, memmap[IBEX_DEV_SRAM_CTRL].size);
    create_unimplemented_device("riscv.lowrisc.ibex.otbn",
        memmap[IBEX_DEV_OTBN].base, memmap[IBEX_DEV_OTBN].size);
    create_unimplemented_device("riscv.lowrisc.ibex.ibex_cfg",
        memmap[IBEX_DEV_IBEX_CFG].base, memmap[IBEX_DEV_IBEX_CFG].size);
}

स्थिर ध्रुव Property lowrisc_ibex_soc_props[] = {
    DEFINE_PROP_UINT32("resetvec", नीचः_RISCIbex_So_CState, resetvec, 0x20000400),
};

स्थिर शून्य lowrisc_ibex_soc_class_init(ObjectClass *oc, ध्रुव शून्य *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_set_props(dc, lowrisc_ibex_soc_props);
    dc->realize = lowrisc_ibex_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

स्थिर ध्रुव TypeInfo open_titan_types[] = {
    {
        .name           = TYPE_RISCV_IBEX_SOC,
        .parent         = TYPE_DEVICE,
        .instance_size  = परिमाण(नीचः_RISCIbex_So_CState),
        .instance_init  = lowrisc_ibex_soc_init,
        .class_init     = lowrisc_ibex_soc_class_init,
    }, {
        .name           = TYPE_OPENTITAN_MACHINE,
        .parent         = TYPE_MACHINE,
        .instance_size  = परिमाण(OpenTitanState),
        .class_init     = opentitan_machine_class_init,
    }
};

DEFINE_TYPES(open_titan_types)
