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

#ضمن "qemu/osdep.h"
#ضمن "qemu/units.h"
#ضمن "qapi/error.h"
#ضمن "target/riscv/cpu.h"
#ضمن "hw/core/sysbus.h"
#ضمن "system/system.h"
#ضمن "net/net.h"
#ضمن "hw/core/boards.h"
#ضمن "hw/char/serial-mm.h"
#ضمن "system/address-spaces.h"
#ضمن "hw/char/xilinx_uartlite.h"
#ضمن "hw/misc/unimp.h"

#تعریف_کن LMB_BRAM_اندازه (128 * KiB)
#تعریف_کن حافظه_BASEADDR 0x80000000
#تعریف_کن INTC_BASEADDR 0x41200000
#تعریف_کن TIMER_BASEADDR 0x41c00000
#تعریف_کن TIMER_BASEADDR2 0x41c10000
#تعریف_کن UARTLITE_BASEADDR 0x40600000
#تعریف_کن ETHLITE_BASEADDR 0x40e00000
#تعریف_کن UART16550_BASEADDR 0x44a10000
#تعریف_کن AXIENET_BASEADDR 0x40c00000
#تعریف_کن AXIDMA_BASEADDR 0x41e00000
#تعریف_کن GPIO_BASEADDR 0x40000000
#تعریف_کن GPIO_BASEADDR2 0x40010000
#تعریف_کن GPIO_BASEADDR3 0x40020000
#تعریف_کن I2C_BASEADDR 0x40800000
#تعریف_کن QSPI_BASEADDR 0x44a00000

#تعریف_کن TIMER_IRQ           0
#تعریف_کن UARTLITE_IRQ        1
#تعریف_کن UART16550_IRQ       4
#تعریف_کن ETHLITE_IRQ         5
#تعریف_کن TIMER_IRQ2          6
#تعریف_کن AXIENET_IRQ         7
#تعریف_کن AXIDMA_IRQ1         8
#تعریف_کن AXIDMA_IRQ0         9

ساكن فراغ mb_v_generic_init(MachineState *machine)
{
    ram_addr_t ram_اندازه = machine->ram_اندازه;
    DeviceState *dev, *dma, *eth0;
    Object *ds, *cs;
    صحيح i;
    RISCVCPU *cpu;
    hwaddr ddr_پایه = حافظه_BASEADDR;
    حافظه_Region *phys_lmb_bram = g_new(حافظه_Region, 1);
    حافظه_Region *phys_ram = g_new(حافظه_Region, 1);
    qemu_irq irq[32];
    حافظه_Region *sysmem = get_system_حافظه();

    cpu = RISCV_CPU(object_new(machine->cpu_type));
    object_property_مجموعه_bool(OBJECT(cpu), "h", false, عدم);
    object_property_مجموعه_bool(OBJECT(cpu), "d", false, عدم);
    qdev_realize(DEVICE(cpu), عدم, &error_abort);
    /* Attach emulated BRAM through the LMB.  */
    حافظه_region_init_ram(phys_lmb_bram, عدم,
                           "mb_v.lmb_bram", LMB_BRAM_اندازه,
                           &error_fatal);
    حافظه_region_جمع_کن_subregion(sysmem, 0x00000000, phys_lmb_bram);

    حافظه_region_init_ram(phys_ram, عدم, "mb_v.ram",
                           ram_اندازه, &error_fatal);
    حافظه_region_جمع_کن_subregion(sysmem, ddr_پایه, phys_ram);

    dev = qdev_new("xlnx.xps-intc");
    qdev_prop_مجموعه_enum(dev, "endianness", ENDIAN_نما_LITTLE);
    qdev_prop_مجموعه_uint32(dev, "kind-of-intr",
                         1 << UARTLITE_IRQ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, INTC_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0,
                       qdev_get_gpio_درون(DEVICE(cpu), 11));
    لكل (i = 0; i < 32; i++) {
        irq[i] = qdev_get_gpio_درون(dev, i);
    }

    /* Uartlite */
    dev = qdev_new(TYPE_XILINX_UARTLITE);
    qdev_prop_مجموعه_enum(dev, "endianness", ENDIAN_نما_LITTLE);
    qdev_prop_مجموعه_chr(dev, "chardev", serial_hd(0));
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, UARTLITE_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[UARTLITE_IRQ]);

    /* Full uart */
    serial_mm_init(sysmem, UART16550_BASEADDR + 0x1000, 2,
                   irq[UART16550_IRQ], 115200, serial_hd(1),
                   DEVICE_LITTLE_ENDIAN);

    /* 2 timers at irq 0 @ 100 Mhz.  */
    dev = qdev_new("xlnx.xps-timer");
    qdev_prop_مجموعه_enum(dev, "endianness", ENDIAN_نما_LITTLE);
    qdev_prop_مجموعه_uint32(dev, "one-timer-only", 0);
    qdev_prop_مجموعه_uint32(dev, "clock-frequency", 100000000);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_IRQ]);

    /* 2 timers at irq 3 @ 100 Mhz.  */
    dev = qdev_new("xlnx.xps-timer");
    qdev_prop_مجموعه_enum(dev, "endianness", ENDIAN_نما_LITTLE);
    qdev_prop_مجموعه_uint32(dev, "one-timer-only", 0);
    qdev_prop_مجموعه_uint32(dev, "clock-frequency", 100000000);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_BASEADDR2);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_IRQ2]);

    /* Emaclite */
    dev = qdev_new("xlnx.xps-ethernetlite");
    qdev_prop_مجموعه_enum(dev, "endianness", ENDIAN_نما_LITTLE);
    qemu_configure_nic_device(dev, true, عدم);
    qdev_prop_مجموعه_uint32(dev, "tx-ping-pong", 0);
    qdev_prop_مجموعه_uint32(dev, "rx-ping-pong", 0);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, ETHLITE_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[ETHLITE_IRQ]);

    /* axi ethernet and dma initialization. */
    eth0 = qdev_new("xlnx.axi-ethernet");
    dma = qdev_new("xlnx.axi-dma");

    /* FIXME: attach to the sysbus instead */
    object_property_جمع_کن_فرزند(qdev_get_machine(), "xilinx-eth", OBJECT(eth0));
    object_property_جمع_کن_فرزند(qdev_get_machine(), "xilinx-dma", OBJECT(dma));

    ds = object_property_get_link(OBJECT(dma),
                                  "axistream-connected-target", عدم);
    cs = object_property_get_link(OBJECT(dma),
                                  "axistream-control-connected-target", عدم);
    qemu_configure_nic_device(eth0, true, عدم);
    qdev_prop_مجموعه_uint32(eth0, "rxmem", 0x1000);
    qdev_prop_مجموعه_uint32(eth0, "txmem", 0x1000);
    object_property_مجموعه_link(OBJECT(eth0), "axistream-connected", ds,
                             &error_abort);
    object_property_مجموعه_link(OBJECT(eth0), "axistream-control-connected", cs,
                             &error_abort);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(eth0), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(eth0), 0, AXIENET_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(eth0), 0, irq[AXIENET_IRQ]);

    ds = object_property_get_link(OBJECT(eth0),
                                  "axistream-connected-target", عدم);
    cs = object_property_get_link(OBJECT(eth0),
                                  "axistream-control-connected-target", عدم);
    qdev_prop_مجموعه_uint32(dma, "freqhz", 100000000);
    object_property_مجموعه_link(OBJECT(dma), "axistream-connected", ds,
                             &error_abort);
    object_property_مجموعه_link(OBJECT(dma), "axistream-control-connected", cs,
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

ساكن فراغ mb_v_generic_machine_init(MachineClass *mc)
{
    mc->desc = "AMD Microblaze-V generic platform";
    mc->init = mb_v_generic_init;
    mc->min_cpus = 1;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_پایه;
    mc->default_cpus = 1;
}

DEFINE_MACHINE("amd-microblaze-v-generic", mb_v_generic_machine_init)
