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

#అన్తర్భావ "qemu/osdep.h"
#అన్తర్భావ "qemu/units.h"
#అన్తర్భావ "qapi/error.h"
#అన్తర్భావ "target/riscv/cpu.h"
#అన్తర్భావ "hw/core/sysbus.h"
#అన్తర్భావ "system/system.h"
#అన్తర్భావ "net/net.h"
#అన్తర్భావ "hw/core/boards.h"
#అన్తర్భావ "hw/char/serial-mm.h"
#అన్తర్భావ "system/address-spaces.h"
#అన్తర్భావ "hw/char/xilinx_uartlite.h"
#అన్తర్భావ "hw/misc/unimp.h"

#నిర్వచించు LMB_BRAM_కొలత (128 * KiB)
#నిర్వచించు జ్ఞాపకం_BASEADDR 0x80000000
#నిర్వచించు INTC_BASEADDR 0x41200000
#నిర్వచించు TIMER_BASEADDR 0x41c00000
#నిర్వచించు TIMER_BASEADDR2 0x41c10000
#నిర్వచించు UARTLITE_BASEADDR 0x40600000
#నిర్వచించు ETHLITE_BASEADDR 0x40e00000
#నిర్వచించు UART16550_BASEADDR 0x44a10000
#నిర్వచించు AXIENET_BASEADDR 0x40c00000
#నిర్వచించు AXIDMA_BASEADDR 0x41e00000
#నిర్వచించు GPIO_BASEADDR 0x40000000
#నిర్వచించు GPIO_BASEADDR2 0x40010000
#నిర్వచించు GPIO_BASEADDR3 0x40020000
#నిర్వచించు I2C_BASEADDR 0x40800000
#నిర్వచించు QSPI_BASEADDR 0x44a00000

#నిర్వచించు TIMER_IRQ           0
#నిర్వచించు UARTLITE_IRQ        1
#నిర్వచించు UART16550_IRQ       4
#నిర్వచించు ETHLITE_IRQ         5
#నిర్వచించు TIMER_IRQ2          6
#నిర్వచించు AXIENET_IRQ         7
#నిర్వచించు AXIDMA_IRQ1         8
#నిర్వచించు AXIDMA_IRQ0         9

స్థిర శూన్య mb_v_generic_init(MachineState *machine)
{
    ram_addr_t ram_కొలత = machine->ram_కొలత;
    DeviceState *dev, *dma, *eth0;
    Object *ds, *cs;
    పూర్ణాంకం i;
    RISCVCPU *cpu;
    hwaddr ddr_పునాది = జ్ఞాపకం_BASEADDR;
    జ్ఞాపకం_Region *phys_lmb_bram = g_new(జ్ఞాపకం_Region, 1);
    జ్ఞాపకం_Region *phys_ram = g_new(జ్ఞాపకం_Region, 1);
    qemu_irq irq[32];
    జ్ఞాపకం_Region *sysmem = get_system_జ్ఞాపకం();

    cpu = RISCV_CPU(object_new(machine->cpu_type));
    object_property_సమితి_bool(OBJECT(cpu), "h", false, ఖాళీ);
    object_property_సమితి_bool(OBJECT(cpu), "d", false, ఖాళీ);
    qdev_realize(DEVICE(cpu), ఖాళీ, &error_abort);
    /* Attach emulated BRAM through the LMB.  */
    జ్ఞాపకం_region_init_ram(phys_lmb_bram, ఖాళీ,
                           "mb_v.lmb_bram", LMB_BRAM_కొలత,
                           &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(sysmem, 0x00000000, phys_lmb_bram);

    జ్ఞాపకం_region_init_ram(phys_ram, ఖాళీ, "mb_v.ram",
                           ram_కొలత, &error_fatal);
    జ్ఞాపకం_region_కూడు_subregion(sysmem, ddr_పునాది, phys_ram);

    dev = qdev_new("xlnx.xps-intc");
    qdev_prop_సమితి_enum(dev, "endianness", ENDIAN_బహుళకం_LITTLE);
    qdev_prop_సమితి_uint32(dev, "kind-of-intr",
                         1 << UARTLITE_IRQ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, INTC_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0,
                       qdev_get_gpio_లోపల(DEVICE(cpu), 11));
    కొరకు (i = 0; i < 32; i++) {
        irq[i] = qdev_get_gpio_లోపల(dev, i);
    }

    /* Uartlite */
    dev = qdev_new(TYPE_XILINX_UARTLITE);
    qdev_prop_సమితి_enum(dev, "endianness", ENDIAN_బహుళకం_LITTLE);
    qdev_prop_సమితి_chr(dev, "chardev", serial_hd(0));
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, UARTLITE_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[UARTLITE_IRQ]);

    /* Full uart */
    serial_mm_init(sysmem, UART16550_BASEADDR + 0x1000, 2,
                   irq[UART16550_IRQ], 115200, serial_hd(1),
                   DEVICE_LITTLE_ENDIAN);

    /* 2 timers at irq 0 @ 100 Mhz.  */
    dev = qdev_new("xlnx.xps-timer");
    qdev_prop_సమితి_enum(dev, "endianness", ENDIAN_బహుళకం_LITTLE);
    qdev_prop_సమితి_uint32(dev, "one-timer-only", 0);
    qdev_prop_సమితి_uint32(dev, "clock-frequency", 100000000);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_IRQ]);

    /* 2 timers at irq 3 @ 100 Mhz.  */
    dev = qdev_new("xlnx.xps-timer");
    qdev_prop_సమితి_enum(dev, "endianness", ENDIAN_బహుళకం_LITTLE);
    qdev_prop_సమితి_uint32(dev, "one-timer-only", 0);
    qdev_prop_సమితి_uint32(dev, "clock-frequency", 100000000);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_BASEADDR2);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_IRQ2]);

    /* Emaclite */
    dev = qdev_new("xlnx.xps-ethernetlite");
    qdev_prop_సమితి_enum(dev, "endianness", ENDIAN_బహుళకం_LITTLE);
    qemu_configure_nic_device(dev, true, ఖాళీ);
    qdev_prop_సమితి_uint32(dev, "tx-ping-pong", 0);
    qdev_prop_సమితి_uint32(dev, "rx-ping-pong", 0);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, ETHLITE_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[ETHLITE_IRQ]);

    /* axi ethernet and dma initialization. */
    eth0 = qdev_new("xlnx.axi-ethernet");
    dma = qdev_new("xlnx.axi-dma");

    /* FIXME: attach to the sysbus instead */
    object_property_కూడు_బిడ్డ(qdev_get_machine(), "xilinx-eth", OBJECT(eth0));
    object_property_కూడు_బిడ్డ(qdev_get_machine(), "xilinx-dma", OBJECT(dma));

    ds = object_property_get_link(OBJECT(dma),
                                  "axistream-connected-target", ఖాళీ);
    cs = object_property_get_link(OBJECT(dma),
                                  "axistream-control-connected-target", ఖాళీ);
    qemu_configure_nic_device(eth0, true, ఖాళీ);
    qdev_prop_సమితి_uint32(eth0, "rxmem", 0x1000);
    qdev_prop_సమితి_uint32(eth0, "txmem", 0x1000);
    object_property_సమితి_link(OBJECT(eth0), "axistream-connected", ds,
                             &error_abort);
    object_property_సమితి_link(OBJECT(eth0), "axistream-control-connected", cs,
                             &error_abort);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(eth0), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(eth0), 0, AXIENET_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(eth0), 0, irq[AXIENET_IRQ]);

    ds = object_property_get_link(OBJECT(eth0),
                                  "axistream-connected-target", ఖాళీ);
    cs = object_property_get_link(OBJECT(eth0),
                                  "axistream-control-connected-target", ఖాళీ);
    qdev_prop_సమితి_uint32(dma, "freqhz", 100000000);
    object_property_సమితి_link(OBJECT(dma), "axistream-connected", ds,
                             &error_abort);
    object_property_సమితి_link(OBJECT(dma), "axistream-control-connected", cs,
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

స్థిర శూన్య mb_v_generic_machine_init(MachineClass *mc)
{
    mc->desc = "AMD Microblaze-V generic platform";
    mc->init = mb_v_generic_init;
    mc->min_cpus = 1;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_పునాది;
    mc->default_cpus = 1;
}

DEFINE_MACHINE("amd-microblaze-v-generic", mb_v_generic_machine_init)
