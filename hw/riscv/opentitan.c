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

#شامل "qemu/osdep.h"
#شامل "qemu/cutils.h"
#شامل "hw/riscv/opentitan.h"
#شامل "qapi/error.h"
#شامل "qemu/error-report.h"
#شامل "hw/core/boards.h"
#شامل "hw/misc/unimp.h"
#شامل "hw/riscv/boot.h"
#شامل "qemu/units.h"
#شامل "system/system.h"
#شامل "system/address-spaces.h"

/*
 * This version of the OpenTitan machine currently supports
 * OpenTitan RTL version:
 * <lowRISC/opentitan@565e4af39760a123c59a184aa2f5812a961fde47>
 *
 * MMIO mapping as per (specified commit):
 * lowRISC/opentitan: hw/top_earlgrey/sw/autogen/top_earlgrey_memory.h
 */
ساکن ثابت Mem_Map_اندراج ibex_memmap[] = {
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

ساکن خالی opentitan_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    OpenTitanState *s = OPENTITAN_MACHINE(machine);
    ثابت Mem_Map_اندراج *memmap = ibex_memmap;
    حافظہ_Region *sys_mem = get_system_حافظہ();
    RISCVBootInfo boot_info;

    اگر (machine->ram_سائز != mc->default_ram_سائز) {
        حرف *sz = سائز_مقصد_str(mc->default_ram_سائز);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        باہر_نکلو(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_بچہ(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_IBEX_SOC);
    qdev_realize(DEVICE(&s->soc), تہی, &error_fatal);

    حافظہ_region_جوڑو_subregion(sys_mem,
        memmap[IBEX_DEV_RAM].بنیاد, machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    اگر (machine->firmware) {
        hwaddr firmware_بھرو_addr = memmap[IBEX_DEV_RAM].بنیاد;
        riscv_بھرو_firmware(machine, &boot_info, machine->firmware,
                            &firmware_بھرو_addr, تہی);
    }

    اگر (machine->kernel_filename) {
        riscv_بھرو_kernel(machine, &boot_info,
                          memmap[IBEX_DEV_RAM].بنیاد,
                          false, تہی);
    }
}

ساکن خالی opentitan_machine_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with OpenTitan";
    mc->init = opentitan_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_IBEX;
    mc->default_ram_id = "riscv.lowrisc.ibex.ram";
    mc->default_ram_سائز = ibex_memmap[IBEX_DEV_RAM].سائز;
}

ساکن خالی lowrisc_ibex_soc_init(Object *obj)
{
    کم_RISCIbex_So_CState *s = RISCV_IBEX_SOC(obj);

    object_initialize_بچہ(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);

    object_initialize_بچہ(obj, "plic", &s->plic, TYPE_SIFIVE_PLIC);

    object_initialize_بچہ(obj, "uart", &s->uart, TYPE_IBEX_UART);

    object_initialize_بچہ(obj, "timer", &s->timer, TYPE_IBEX_TIMER);

    برائے (صحیح i = 0; i < OPENTITAN_عدد_SPI_HOSTS; i++) {
        object_initialize_بچہ(obj, "spi_host[*]", &s->spi_host[i],
                                TYPE_IBEX_SPI_HOST);
    }
}

ساکن خالی lowrisc_ibex_soc_realize(DeviceState *dev_soc, Error **errp)
{
    ثابت Mem_Map_اندراج *memmap = ibex_memmap;
    DeviceState *dev;
    SysBusDevice *busdev;
    MachineState *ms = MACHINE(qdev_get_machine());
    کم_RISCIbex_So_CState *s = RISCV_IBEX_SOC(dev_soc);
    حافظہ_Region *sys_mem = get_system_حافظہ();
    صحیح i;

    object_property_سیٹ_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    object_property_سیٹ_int(OBJECT(&s->cpus), "resetvec", s->resetvec,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Boot ROM */
    حافظہ_region_init_rom(&s->rom, OBJECT(dev_soc), "riscv.lowrisc.ibex.rom",
                           memmap[IBEX_DEV_ROM].سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion(sys_mem,
        memmap[IBEX_DEV_ROM].بنیاد, &s->rom);

    /* Flash memory */
    حافظہ_region_init_rom(&s->flash_mem, OBJECT(dev_soc), "riscv.lowrisc.ibex.flash",
                           memmap[IBEX_DEV_FLASH].سائز, &error_fatal);
    حافظہ_region_init_alias(&s->flash_alias, OBJECT(dev_soc),
                             "riscv.lowrisc.ibex.flash_virtual", &s->flash_mem, 0,
                             memmap[IBEX_DEV_FLASH_VIRTUAL].سائز);
    حافظہ_region_جوڑو_subregion(sys_mem, memmap[IBEX_DEV_FLASH].بنیاد,
                                &s->flash_mem);
    حافظہ_region_جوڑو_subregion(sys_mem, memmap[IBEX_DEV_FLASH_VIRTUAL].بنیاد,
                                &s->flash_alias);

    /* PLIC */
    qdev_prop_سیٹ_string(DEVICE(&s->plic), "hart-config", "M");
    qdev_prop_سیٹ_uint32(DEVICE(&s->plic), "num-sources", 180);
    qdev_prop_سیٹ_uint32(DEVICE(&s->plic), "num-priorities", 3);
    qdev_prop_سیٹ_uint32(DEVICE(&s->plic), "pending-base", 0x1000);
    qdev_prop_سیٹ_uint32(DEVICE(&s->plic), "enable-base", 0x2000);
    qdev_prop_سیٹ_uint32(DEVICE(&s->plic), "enable-stride", 32);
    qdev_prop_سیٹ_uint32(DEVICE(&s->plic), "context-base", 0x200000);
    qdev_prop_سیٹ_uint32(DEVICE(&s->plic), "context-stride", 8);
    qdev_prop_سیٹ_uint32(DEVICE(&s->plic), "aperture-size", memmap[IBEX_DEV_PLIC].سائز);

    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->plic), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->plic), 0, memmap[IBEX_DEV_PLIC].بنیاد);

    برائے (i = 0; i < ms->smp.cpus; i++) {
        CPUState *cpu = qemu_get_cpu(i);

        qdev_connect_gpio_برآمد(DEVICE(&s->plic), ms->smp.cpus + i,
                              qdev_get_gpio_اندر(DEVICE(cpu), IRQ_M_EXT));
    }

    /* UART */
    qdev_prop_سیٹ_chr(DEVICE(&(s->uart)), "chardev", serial_hd(0));
    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, memmap[IBEX_DEV_UART].بنیاد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       0, qdev_get_gpio_اندر(DEVICE(&s->plic),
                       IBEX_UART0_TX_WATERMARK_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       1, qdev_get_gpio_اندر(DEVICE(&s->plic),
                       IBEX_UART0_RX_WATERMARK_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       2, qdev_get_gpio_اندر(DEVICE(&s->plic),
                       IBEX_UART0_TX_خالی_خانے_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       3, qdev_get_gpio_اندر(DEVICE(&s->plic),
                       IBEX_UART0_RX_حد_سے_تجاوز_IRQ));

    اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->timer), errp)) {
        واپس;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->timer), 0, memmap[IBEX_DEV_TIMER].بنیاد);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->timer),
                       0, qdev_get_gpio_اندر(DEVICE(&s->plic),
                       IBEX_TIMER_TIMEREXPIRED0_0));
    qdev_connect_gpio_برآمد(DEVICE(&s->timer), 0,
                          qdev_get_gpio_اندر(DEVICE(qemu_get_cpu(0)),
                                           IRQ_M_TIMER));

    /* SPI-Hosts */
    برائے (i = 0; i < OPENTITAN_عدد_SPI_HOSTS; ++i) {
        dev = DEVICE(&(s->spi_host[i]));
        اگر (!sysbus_realize(SYS_BUS_DEVICE(&s->spi_host[i]), errp)) {
            واپس;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, memmap[IBEX_DEV_SPI_HOST0 + i].بنیاد);

        انتخاب (i) {
        حالت OPENTITAN_SPI_HOST0:
            sysbus_connect_irq(busdev, 0, qdev_get_gpio_اندر(DEVICE(&s->plic),
                                IBEX_SPI_HOST0_ERR_IRQ));
            sysbus_connect_irq(busdev, 1, qdev_get_gpio_اندر(DEVICE(&s->plic),
                                IBEX_SPI_HOST0_SPI_EVENT_IRQ));
            توڑو;
        حالت OPENTITAN_SPI_HOST1:
            sysbus_connect_irq(busdev, 0, qdev_get_gpio_اندر(DEVICE(&s->plic),
                                IBEX_SPI_HOST1_ERR_IRQ));
            sysbus_connect_irq(busdev, 1, qdev_get_gpio_اندر(DEVICE(&s->plic),
                                IBEX_SPI_HOST1_SPI_EVENT_IRQ));
            توڑو;
        }
    }

    create_unimplemented_device("riscv.lowrisc.ibex.gpio",
        memmap[IBEX_DEV_GPIO].بنیاد, memmap[IBEX_DEV_GPIO].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.spi_device",
        memmap[IBEX_DEV_SPI_DEVICE].بنیاد, memmap[IBEX_DEV_SPI_DEVICE].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.i2c",
        memmap[IBEX_DEV_I2C].بنیاد, memmap[IBEX_DEV_I2C].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.pattgen",
        memmap[IBEX_DEV_PATTGEN].بنیاد, memmap[IBEX_DEV_PATTGEN].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.sensor_ctrl",
        memmap[IBEX_DEV_SENSOR_CTRL].بنیاد, memmap[IBEX_DEV_SENSOR_CTRL].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.otp_ctrl",
        memmap[IBEX_DEV_OTP_CTRL].بنیاد, memmap[IBEX_DEV_OTP_CTRL].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.lc_ctrl",
        memmap[IBEX_DEV_LC_CTRL].بنیاد, memmap[IBEX_DEV_LC_CTRL].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.pwrmgr",
        memmap[IBEX_DEV_PWRMGR].بنیاد, memmap[IBEX_DEV_PWRMGR].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.rstmgr",
        memmap[IBEX_DEV_RSTMGR].بنیاد, memmap[IBEX_DEV_RSTMGR].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.clkmgr",
        memmap[IBEX_DEV_CLKMGR].بنیاد, memmap[IBEX_DEV_CLKMGR].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.pinmux",
        memmap[IBEX_DEV_PINMUX].بنیاد, memmap[IBEX_DEV_PINMUX].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.aon_timer",
        memmap[IBEX_DEV_AON_TIMER].بنیاد, memmap[IBEX_DEV_AON_TIMER].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.usbdev",
        memmap[IBEX_DEV_USBDEV].بنیاد, memmap[IBEX_DEV_USBDEV].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.flash_ctrl",
        memmap[IBEX_DEV_FLASH_CTRL].بنیاد, memmap[IBEX_DEV_FLASH_CTRL].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.aes",
        memmap[IBEX_DEV_AES].بنیاد, memmap[IBEX_DEV_AES].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.hmac",
        memmap[IBEX_DEV_HMAC].بنیاد, memmap[IBEX_DEV_HMAC].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.kmac",
        memmap[IBEX_DEV_KMAC].بنیاد, memmap[IBEX_DEV_KMAC].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.keymgr",
        memmap[IBEX_DEV_KEYMGR].بنیاد, memmap[IBEX_DEV_KEYMGR].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.csrng",
        memmap[IBEX_DEV_CSRNG].بنیاد, memmap[IBEX_DEV_CSRNG].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.entropy",
        memmap[IBEX_DEV_ENTROPY].بنیاد, memmap[IBEX_DEV_ENTROPY].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.edn0",
        memmap[IBEX_DEV_EDNO].بنیاد, memmap[IBEX_DEV_EDNO].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.edn1",
        memmap[IBEX_DEV_EDN1].بنیاد, memmap[IBEX_DEV_EDN1].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.alert_handler",
        memmap[IBEX_DEV_ALERT_HANDLER].بنیاد, memmap[IBEX_DEV_ALERT_HANDLER].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.sram_ctrl",
        memmap[IBEX_DEV_SRAM_CTRL].بنیاد, memmap[IBEX_DEV_SRAM_CTRL].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.otbn",
        memmap[IBEX_DEV_OTBN].بنیاد, memmap[IBEX_DEV_OTBN].سائز);
    create_unimplemented_device("riscv.lowrisc.ibex.ibex_cfg",
        memmap[IBEX_DEV_IBEX_CFG].بنیاد, memmap[IBEX_DEV_IBEX_CFG].سائز);
}

ساکن ثابت Property lowrisc_ibex_soc_props[] = {
    DEFINE_PROP_UINT32("resetvec", کم_RISCIbex_So_CState, resetvec, 0x20000400),
};

ساکن خالی lowrisc_ibex_soc_class_init(ObjectClass *oc, ثابت خالی *ڈیٹا)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_سیٹ_props(dc, lowrisc_ibex_soc_props);
    dc->realize = lowrisc_ibex_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

ساکن ثابت TypeInfo open_titan_types[] = {
    {
        .name           = TYPE_RISCV_IBEX_SOC,
        .والد         = TYPE_DEVICE,
        .instance_سائز  = حجم(کم_RISCIbex_So_CState),
        .instance_init  = lowrisc_ibex_soc_init,
        .class_init     = lowrisc_ibex_soc_class_init,
    }, {
        .name           = TYPE_OPENTITAN_MACHINE,
        .والد         = TYPE_MACHINE,
        .instance_سائز  = حجم(OpenTitanState),
        .class_init     = opentitan_machine_class_init,
    }
};

DEFINE_TYPES(open_titan_types)
