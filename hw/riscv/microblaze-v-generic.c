/*
 * QEMU model of Microblaze V generic board.
 *
 * based on hw/microblaze/petalogix_ml605_mmu.c
 *
 * Copyright (c) 2011 Michal Simek <monstr@monstr.eu>
 * Copyright (c) 2011 PetaLogix
 * Copyright (c) 2009 Edgar E. Iglesias.
 * Copyright (C) 2024, Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Written by Sai Pavan Boddu <sai.pavan.boddu@amd.com
 *     and by Michal Simek <michal.simek@amd.com>.
 */

#شامل "qemu/osdep.h"
#شامل "qemu/units.h"
#شامل "qapi/error.h"
#شامل "target/riscv/cpu.h"
#شامل "hw/core/sysbus.h"
#شامل "system/system.h"
#شامل "net/net.h"
#شامل "hw/core/boards.h"
#شامل "hw/char/serial-mm.h"
#شامل "system/address-spaces.h"
#شامل "hw/char/xilinx_uartlite.h"
#شامل "hw/misc/unimp.h"

#تعریف_کرو LMB_BRAM_سائز (128 * KiB)
#تعریف_کرو حافظہ_BASEADDR 0x80000000
#تعریف_کرو INTC_BASEADDR 0x41200000
#تعریف_کرو TIMER_BASEADDR 0x41c00000
#تعریف_کرو TIMER_BASEADDR2 0x41c10000
#تعریف_کرو UARTLITE_BASEADDR 0x40600000
#تعریف_کرو ETHLITE_BASEADDR 0x40e00000
#تعریف_کرو UART16550_BASEADDR 0x44a10000
#تعریف_کرو AXIENET_BASEADDR 0x40c00000
#تعریف_کرو AXIDMA_BASEADDR 0x41e00000
#تعریف_کرو GPIO_BASEADDR 0x40000000
#تعریف_کرو GPIO_BASEADDR2 0x40010000
#تعریف_کرو GPIO_BASEADDR3 0x40020000
#تعریف_کرو I2C_BASEADDR 0x40800000
#تعریف_کرو QSPI_BASEADDR 0x44a00000

#تعریف_کرو TIMER_IRQ           0
#تعریف_کرو UARTLITE_IRQ        1
#تعریف_کرو UART16550_IRQ       4
#تعریف_کرو ETHLITE_IRQ         5
#تعریف_کرو TIMER_IRQ2          6
#تعریف_کرو AXIENET_IRQ         7
#تعریف_کرو AXIDMA_IRQ1         8
#تعریف_کرو AXIDMA_IRQ0         9

ساکن خالی mb_v_generic_init(MachineState *machine)
{
    ram_addr_t ram_سائز = machine->ram_سائز;
    DeviceState *dev, *dma, *eth0;
    Object *ds, *cs;
    صحیح i;
    RISCVCPU *cpu;
    hwaddr ddr_بنیاد = حافظہ_BASEADDR;
    حافظہ_Region *phys_lmb_bram = g_new(حافظہ_Region, 1);
    حافظہ_Region *phys_ram = g_new(حافظہ_Region, 1);
    qemu_irq irq[32];
    حافظہ_Region *sysmem = get_system_حافظہ();

    cpu = RISCV_CPU(object_new(machine->cpu_type));
    object_property_سیٹ_bool(OBJECT(cpu), "h", false, تہی);
    object_property_سیٹ_bool(OBJECT(cpu), "d", false, تہی);
    qdev_realize(DEVICE(cpu), تہی, &error_abort);
    /* Attach emulated BRAM through the LMB.  */
    حافظہ_region_init_ram(phys_lmb_bram, تہی,
                           "mb_v.lmb_bram", LMB_BRAM_سائز,
                           &error_fatal);
    حافظہ_region_جوڑو_subregion(sysmem, 0x00000000, phys_lmb_bram);

    حافظہ_region_init_ram(phys_ram, تہی, "mb_v.ram",
                           ram_سائز, &error_fatal);
    حافظہ_region_جوڑو_subregion(sysmem, ddr_بنیاد, phys_ram);

    dev = qdev_new("xlnx.xps-intc");
    qdev_prop_سیٹ_enum(dev, "endianness", ENDIAN_منوال_LITTLE);
    qdev_prop_سیٹ_uint32(dev, "kind-of-intr",
                         1 << UARTLITE_IRQ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, INTC_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0,
                       qdev_get_gpio_اندر(DEVICE(cpu), 11));
    برائے (i = 0; i < 32; i++) {
        irq[i] = qdev_get_gpio_اندر(dev, i);
    }

    /* Uartlite */
    dev = qdev_new(TYPE_XILINX_UARTLITE);
    qdev_prop_سیٹ_enum(dev, "endianness", ENDIAN_منوال_LITTLE);
    qdev_prop_سیٹ_chr(dev, "chardev", serial_hd(0));
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, UARTLITE_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[UARTLITE_IRQ]);

    /* Full uart */
    serial_mm_init(sysmem, UART16550_BASEADDR + 0x1000, 2,
                   irq[UART16550_IRQ], 115200, serial_hd(1),
                   DEVICE_LITTLE_ENDIAN);

    /* 2 timers at irq 0 @ 100 Mhz.  */
    dev = qdev_new("xlnx.xps-timer");
    qdev_prop_سیٹ_enum(dev, "endianness", ENDIAN_منوال_LITTLE);
    qdev_prop_سیٹ_uint32(dev, "one-timer-only", 0);
    qdev_prop_سیٹ_uint32(dev, "clock-frequency", 100000000);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_IRQ]);

    /* 2 timers at irq 3 @ 100 Mhz.  */
    dev = qdev_new("xlnx.xps-timer");
    qdev_prop_سیٹ_enum(dev, "endianness", ENDIAN_منوال_LITTLE);
    qdev_prop_سیٹ_uint32(dev, "one-timer-only", 0);
    qdev_prop_سیٹ_uint32(dev, "clock-frequency", 100000000);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_BASEADDR2);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_IRQ2]);

    /* Emaclite */
    dev = qdev_new("xlnx.xps-ethernetlite");
    qdev_prop_سیٹ_enum(dev, "endianness", ENDIAN_منوال_LITTLE);
    qemu_configure_nic_device(dev, true, تہی);
    qdev_prop_سیٹ_uint32(dev, "tx-ping-pong", 0);
    qdev_prop_سیٹ_uint32(dev, "rx-ping-pong", 0);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, ETHLITE_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[ETHLITE_IRQ]);

    /* axi ethernet and dma initialization. */
    eth0 = qdev_new("xlnx.axi-ethernet");
    dma = qdev_new("xlnx.axi-dma");

    /* FIXME: attach to the sysbus instead */
    object_property_جوڑو_بچہ(qdev_get_machine(), "xilinx-eth", OBJECT(eth0));
    object_property_جوڑو_بچہ(qdev_get_machine(), "xilinx-dma", OBJECT(dma));

    ds = object_property_get_link(OBJECT(dma),
                                  "axistream-connected-target", تہی);
    cs = object_property_get_link(OBJECT(dma),
                                  "axistream-control-connected-target", تہی);
    qemu_configure_nic_device(eth0, true, تہی);
    qdev_prop_سیٹ_uint32(eth0, "rxmem", 0x1000);
    qdev_prop_سیٹ_uint32(eth0, "txmem", 0x1000);
    object_property_سیٹ_link(OBJECT(eth0), "axistream-connected", ds,
                             &error_abort);
    object_property_سیٹ_link(OBJECT(eth0), "axistream-control-connected", cs,
                             &error_abort);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(eth0), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(eth0), 0, AXIENET_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(eth0), 0, irq[AXIENET_IRQ]);

    ds = object_property_get_link(OBJECT(eth0),
                                  "axistream-connected-target", تہی);
    cs = object_property_get_link(OBJECT(eth0),
                                  "axistream-control-connected-target", تہی);
    qdev_prop_سیٹ_uint32(dma, "freqhz", 100000000);
    object_property_سیٹ_link(OBJECT(dma), "axistream-connected", ds,
                             &error_abort);
    object_property_سیٹ_link(OBJECT(dma), "axistream-control-connected", cs,
                             &error_abort);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dma), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dma), 0, AXIDMA_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dma), 0, irq[AXIDMA_IRQ0]);
    sysbus_connect_irq(SYS_BUS_DEVICE(dma), 1, irq[AXIDMA_IRQ1]);

    /* unimplemented devices */
    create_unimplemented_device("gpio", GPIO_BASEADDR, 0x10000);
    create_unimplemented_device("gpio2", GPIO_BASEADDR2, 0x10000);
    create_unimplemented_device("gpio3", GPIO_BASEADDR3, 0x10000);
    create_unimplemented_device("i2c", I2C_BASEADDR, 0x10000);
    create_unimplemented_device("qspi", QSPI_BASEADDR, 0x10000);
}

ساکن خالی mb_v_generic_machine_init(MachineClass *mc)
{
    mc->desc = "AMD Microblaze-V generic platform";
    mc->init = mb_v_generic_init;
    mc->min_cpus = 1;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_بنیاد;
    mc->default_cpus = 1;
}

DEFINE_MACHINE("amd-microblaze-v-generic", mb_v_generic_machine_init)
