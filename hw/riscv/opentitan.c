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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/cutils.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/opentitan.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "qemu/error-report.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/unimp.h"
#ಅನ್ತರ್ಭಾವ "hw/riscv/boot.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "system/address-spaces.h"

/*
 * This version of the OpenTitan machine currently supports
 * OpenTitan RTL version:
 * <lowRISC/opentitan@565e4af39760a123c59a184aa2f5812a961fde47>
 *
 * MMIO mapping as per (specified commit):
 * lowRISC/opentitan: hw/top_earlgrey/sw/autogen/top_earlgrey_memory.h
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು ibex_memmap[] = {
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

ಸ್ಥಿರ ಶೂನ್ಯ opentitan_machine_init(MachineState *machine)
{
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    OpenTitanState *s = OPENTITAN_MACHINE(machine);
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = ibex_memmap;
    ಸ್ಮರಣೆ_Region *sys_mem = get_system_ಸ್ಮರಣೆ();
    RISCVBootInfo boot_info;

    ಆದರೆ (machine->ram_ಗಾತ್ರ != mc->default_ram_ಗಾತ್ರ) {
        ಅಕ್ಷರ *sz = ಗಾತ್ರ_ಗಮ್ಯ_str(mc->default_ram_ಗಾತ್ರ);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
    }

    /* Initialize SoC */
    object_initialize_ಮಗು(OBJECT(machine), "soc", &s->soc,
                            TYPE_RISCV_IBEX_SOC);
    qdev_realize(DEVICE(&s->soc), ಖಾಲಿ, &error_fatal);

    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem,
        memmap[IBEX_DEV_RAM].ತಳ, machine->ram);

    riscv_boot_info_init(&boot_info, &s->soc.cpus);

    ಆದರೆ (machine->firmware) {
        hwaddr firmware_ತುಂಬು_addr = memmap[IBEX_DEV_RAM].ತಳ;
        riscv_ತುಂಬು_firmware(machine, &boot_info, machine->firmware,
                            &firmware_ತುಂಬು_addr, ಖಾಲಿ);
    }

    ಆದರೆ (machine->kernel_filename) {
        riscv_ತುಂಬು_kernel(machine, &boot_info,
                          memmap[IBEX_DEV_RAM].ತಳ,
                          false, ಖಾಲಿ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ opentitan_machine_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "RISC-V Board compatible with OpenTitan";
    mc->init = opentitan_machine_init;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_IBEX;
    mc->default_ram_id = "riscv.lowrisc.ibex.ram";
    mc->default_ram_ಗಾತ್ರ = ibex_memmap[IBEX_DEV_RAM].ಗಾತ್ರ_2;
}

ಸ್ಥಿರ ಶೂನ್ಯ lowrisc_ibex_soc_init(Object *obj)
{
    ಕೆಳಗಿನ_RISCIbex_So_CState *s = RISCV_IBEX_SOC(obj);

    object_initialize_ಮಗು(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);

    object_initialize_ಮಗು(obj, "plic", &s->plic, TYPE_SIFIVE_PLIC);

    object_initialize_ಮಗು(obj, "uart", &s->uart, TYPE_IBEX_UART);

    object_initialize_ಮಗು(obj, "timer", &s->timer, TYPE_IBEX_TIMER);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < OPENTITAN_ಅಂಕೆ_SPI_HOSTS; i++) {
        object_initialize_ಮಗು(obj, "spi_host[*]", &s->spi_host[i],
                                TYPE_IBEX_SPI_HOST);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ lowrisc_ibex_soc_realize(DeviceState *dev_soc, Error **errp)
{
    ಸ್ಥಿರಾಂಕ Mem_Map_ನಮೂದು *memmap = ibex_memmap;
    DeviceState *dev;
    SysBusDevice *busdev;
    MachineState *ms = MACHINE(qdev_get_machine());
    ಕೆಳಗಿನ_RISCIbex_So_CState *s = RISCV_IBEX_SOC(dev_soc);
    ಸ್ಮರಣೆ_Region *sys_mem = get_system_ಸ್ಮರಣೆ();
    ಪೂರ್ಣಾಂಕ i;

    object_property_ಗಣ_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus,
                            &error_abort);
    object_property_ಗಣ_int(OBJECT(&s->cpus), "resetvec", s->resetvec,
                            &error_abort);
    sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_fatal);

    /* Boot ROM */
    ಸ್ಮರಣೆ_region_init_rom(&s->rom, OBJECT(dev_soc), "riscv.lowrisc.ibex.rom",
                           memmap[IBEX_DEV_ROM].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem,
        memmap[IBEX_DEV_ROM].ತಳ, &s->rom);

    /* Flash memory */
    ಸ್ಮರಣೆ_region_init_rom(&s->flash_mem, OBJECT(dev_soc), "riscv.lowrisc.ibex.flash",
                           memmap[IBEX_DEV_FLASH].ಗಾತ್ರ_2, &error_fatal);
    ಸ್ಮರಣೆ_region_init_alias(&s->flash_alias, OBJECT(dev_soc),
                             "riscv.lowrisc.ibex.flash_virtual", &s->flash_mem, 0,
                             memmap[IBEX_DEV_FLASH_VIRTUAL].ಗಾತ್ರ_2);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem, memmap[IBEX_DEV_FLASH].ತಳ,
                                &s->flash_mem);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sys_mem, memmap[IBEX_DEV_FLASH_VIRTUAL].ತಳ,
                                &s->flash_alias);

    /* PLIC */
    qdev_prop_ಗಣ_string(DEVICE(&s->plic), "hart-config", "M");
    qdev_prop_ಗಣ_uint32(DEVICE(&s->plic), "num-sources", 180);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->plic), "num-priorities", 3);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->plic), "pending-base", 0x1000);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->plic), "enable-base", 0x2000);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->plic), "enable-stride", 32);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->plic), "context-base", 0x200000);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->plic), "context-stride", 8);
    qdev_prop_ಗಣ_uint32(DEVICE(&s->plic), "aperture-size", memmap[IBEX_DEV_PLIC].ಗಾತ್ರ_2);

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->plic), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->plic), 0, memmap[IBEX_DEV_PLIC].ತಳ);

    ಗಾಗಿ (i = 0; i < ms->smp.cpus; i++) {
        CPUState *cpu = qemu_get_cpu(i);

        qdev_connect_gpio_ಹೊರಹೋಗು(DEVICE(&s->plic), ms->smp.cpus + i,
                              qdev_get_gpio_ಒಳ(DEVICE(cpu), IRQ_M_EXT));
    }

    /* UART */
    qdev_prop_ಗಣ_chr(DEVICE(&(s->uart)), "chardev", serial_hd(0));
    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->uart), 0, memmap[IBEX_DEV_UART].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       0, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                       IBEX_UART0_TX_WATERMARK_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       1, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                       IBEX_UART0_RX_WATERMARK_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       2, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                       IBEX_UART0_TX_ಖಾಲಿಗಳು_IRQ));
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart),
                       3, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                       IBEX_UART0_RX_ಅತಿಪ್ರವಾಹ_IRQ));

    ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->timer), errp)) {
        ಹಿಂತಿರುಗಿ;
    }
    sysbus_mmio_map(SYS_BUS_DEVICE(&s->timer), 0, memmap[IBEX_DEV_TIMER].ತಳ);
    sysbus_connect_irq(SYS_BUS_DEVICE(&s->timer),
                       0, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                       IBEX_TIMER_TIMEREXPIRED0_0));
    qdev_connect_gpio_ಹೊರಹೋಗು(DEVICE(&s->timer), 0,
                          qdev_get_gpio_ಒಳ(DEVICE(qemu_get_cpu(0)),
                                           IRQ_M_TIMER));

    /* SPI-Hosts */
    ಗಾಗಿ (i = 0; i < OPENTITAN_ಅಂಕೆ_SPI_HOSTS; ++i) {
        dev = DEVICE(&(s->spi_host[i]));
        ಆದರೆ (!sysbus_realize(SYS_BUS_DEVICE(&s->spi_host[i]), errp)) {
            ಹಿಂತಿರುಗಿ;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, memmap[IBEX_DEV_SPI_HOST0 + i].ತಳ);

        ಆಯ್ಕೆ (i) {
        ಸ್ಥಿತಿ OPENTITAN_SPI_HOST0:
            sysbus_connect_irq(busdev, 0, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                                IBEX_SPI_HOST0_ERR_IRQ));
            sysbus_connect_irq(busdev, 1, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                                IBEX_SPI_HOST0_SPI_EVENT_IRQ));
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ OPENTITAN_SPI_HOST1:
            sysbus_connect_irq(busdev, 0, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                                IBEX_SPI_HOST1_ERR_IRQ));
            sysbus_connect_irq(busdev, 1, qdev_get_gpio_ಒಳ(DEVICE(&s->plic),
                                IBEX_SPI_HOST1_SPI_EVENT_IRQ));
            ನಿಲ್ಲಿಸು;
        }
    }

    create_unimplemented_device("riscv.lowrisc.ibex.gpio",
        memmap[IBEX_DEV_GPIO].ತಳ, memmap[IBEX_DEV_GPIO].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.spi_device",
        memmap[IBEX_DEV_SPI_DEVICE].ತಳ, memmap[IBEX_DEV_SPI_DEVICE].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.i2c",
        memmap[IBEX_DEV_I2C].ತಳ, memmap[IBEX_DEV_I2C].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.pattgen",
        memmap[IBEX_DEV_PATTGEN].ತಳ, memmap[IBEX_DEV_PATTGEN].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.sensor_ctrl",
        memmap[IBEX_DEV_SENSOR_CTRL].ತಳ, memmap[IBEX_DEV_SENSOR_CTRL].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.otp_ctrl",
        memmap[IBEX_DEV_OTP_CTRL].ತಳ, memmap[IBEX_DEV_OTP_CTRL].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.lc_ctrl",
        memmap[IBEX_DEV_LC_CTRL].ತಳ, memmap[IBEX_DEV_LC_CTRL].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.pwrmgr",
        memmap[IBEX_DEV_PWRMGR].ತಳ, memmap[IBEX_DEV_PWRMGR].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.rstmgr",
        memmap[IBEX_DEV_RSTMGR].ತಳ, memmap[IBEX_DEV_RSTMGR].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.clkmgr",
        memmap[IBEX_DEV_CLKMGR].ತಳ, memmap[IBEX_DEV_CLKMGR].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.pinmux",
        memmap[IBEX_DEV_PINMUX].ತಳ, memmap[IBEX_DEV_PINMUX].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.aon_timer",
        memmap[IBEX_DEV_AON_TIMER].ತಳ, memmap[IBEX_DEV_AON_TIMER].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.usbdev",
        memmap[IBEX_DEV_USBDEV].ತಳ, memmap[IBEX_DEV_USBDEV].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.flash_ctrl",
        memmap[IBEX_DEV_FLASH_CTRL].ತಳ, memmap[IBEX_DEV_FLASH_CTRL].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.aes",
        memmap[IBEX_DEV_AES].ತಳ, memmap[IBEX_DEV_AES].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.hmac",
        memmap[IBEX_DEV_HMAC].ತಳ, memmap[IBEX_DEV_HMAC].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.kmac",
        memmap[IBEX_DEV_KMAC].ತಳ, memmap[IBEX_DEV_KMAC].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.keymgr",
        memmap[IBEX_DEV_KEYMGR].ತಳ, memmap[IBEX_DEV_KEYMGR].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.csrng",
        memmap[IBEX_DEV_CSRNG].ತಳ, memmap[IBEX_DEV_CSRNG].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.entropy",
        memmap[IBEX_DEV_ENTROPY].ತಳ, memmap[IBEX_DEV_ENTROPY].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.edn0",
        memmap[IBEX_DEV_EDNO].ತಳ, memmap[IBEX_DEV_EDNO].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.edn1",
        memmap[IBEX_DEV_EDN1].ತಳ, memmap[IBEX_DEV_EDN1].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.alert_handler",
        memmap[IBEX_DEV_ALERT_HANDLER].ತಳ, memmap[IBEX_DEV_ALERT_HANDLER].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.sram_ctrl",
        memmap[IBEX_DEV_SRAM_CTRL].ತಳ, memmap[IBEX_DEV_SRAM_CTRL].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.otbn",
        memmap[IBEX_DEV_OTBN].ತಳ, memmap[IBEX_DEV_OTBN].ಗಾತ್ರ_2);
    create_unimplemented_device("riscv.lowrisc.ibex.ibex_cfg",
        memmap[IBEX_DEV_IBEX_CFG].ತಳ, memmap[IBEX_DEV_IBEX_CFG].ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Property lowrisc_ibex_soc_props[] = {
    DEFINE_PROP_UINT32("resetvec", ಕೆಳಗಿನ_RISCIbex_So_CState, resetvec, 0x20000400),
};

ಸ್ಥಿರ ಶೂನ್ಯ lowrisc_ibex_soc_class_init(ObjectClass *oc, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_ಗಣ_props(dc, lowrisc_ibex_soc_props);
    dc->realize = lowrisc_ibex_soc_realize;
    /* Reason: Uses serial_hds in realize function, thus can't be used twice */
    dc->user_creatable = false;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ TypeInfo open_titan_types[] = {
    {
        .name           = TYPE_RISCV_IBEX_SOC,
        .ಪೋಷಕ         = TYPE_DEVICE,
        .instance_ಗಾತ್ರ  = ಗಾತ್ರ(ಕೆಳಗಿನ_RISCIbex_So_CState),
        .instance_init  = lowrisc_ibex_soc_init,
        .class_init     = lowrisc_ibex_soc_class_init,
    }, {
        .name           = TYPE_OPENTITAN_MACHINE,
        .ಪೋಷಕ         = TYPE_MACHINE,
        .instance_ಗಾತ್ರ  = ಗಾತ್ರ(OpenTitanState),
        .class_init     = opentitan_machine_class_init,
    }
};

DEFINE_TYPES(open_titan_types)
