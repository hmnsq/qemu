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

#ಅನ್ತರ್ಭಾವ "qemu/osdep.h"
#ಅನ್ತರ್ಭಾವ "qemu/units.h"
#ಅನ್ತರ್ಭಾವ "qapi/error.h"
#ಅನ್ತರ್ಭಾವ "target/riscv/cpu.h"
#ಅನ್ತರ್ಭಾವ "hw/core/sysbus.h"
#ಅನ್ತರ್ಭಾವ "system/system.h"
#ಅನ್ತರ್ಭಾವ "net/net.h"
#ಅನ್ತರ್ಭಾವ "hw/core/boards.h"
#ಅನ್ತರ್ಭಾವ "hw/char/serial-mm.h"
#ಅನ್ತರ್ಭಾವ "system/address-spaces.h"
#ಅನ್ತರ್ಭಾವ "hw/char/xilinx_uartlite.h"
#ಅನ್ತರ್ಭಾವ "hw/misc/unimp.h"

#ವ್ಯಾಖ್ಯಾನಿಸು LMB_BRAM_ಗಾತ್ರ (128 * KiB)
#ವ್ಯಾಖ್ಯಾನಿಸು ಸ್ಮರಣೆ_BASEADDR 0x80000000
#ವ್ಯಾಖ್ಯಾನಿಸು INTC_BASEADDR 0x41200000
#ವ್ಯಾಖ್ಯಾನಿಸು TIMER_BASEADDR 0x41c00000
#ವ್ಯಾಖ್ಯಾನಿಸು TIMER_BASEADDR2 0x41c10000
#ವ್ಯಾಖ್ಯಾನಿಸು UARTLITE_BASEADDR 0x40600000
#ವ್ಯಾಖ್ಯಾನಿಸು ETHLITE_BASEADDR 0x40e00000
#ವ್ಯಾಖ್ಯಾನಿಸು UART16550_BASEADDR 0x44a10000
#ವ್ಯಾಖ್ಯಾನಿಸು AXIENET_BASEADDR 0x40c00000
#ವ್ಯಾಖ್ಯಾನಿಸು AXIDMA_BASEADDR 0x41e00000
#ವ್ಯಾಖ್ಯಾನಿಸು GPIO_BASEADDR 0x40000000
#ವ್ಯಾಖ್ಯಾನಿಸು GPIO_BASEADDR2 0x40010000
#ವ್ಯಾಖ್ಯಾನಿಸು GPIO_BASEADDR3 0x40020000
#ವ್ಯಾಖ್ಯಾನಿಸು I2C_BASEADDR 0x40800000
#ವ್ಯಾಖ್ಯಾನಿಸು QSPI_BASEADDR 0x44a00000

#ವ್ಯಾಖ್ಯಾನಿಸು TIMER_IRQ           0
#ವ್ಯಾಖ್ಯಾನಿಸು UARTLITE_IRQ        1
#ವ್ಯಾಖ್ಯಾನಿಸು UART16550_IRQ       4
#ವ್ಯಾಖ್ಯಾನಿಸು ETHLITE_IRQ         5
#ವ್ಯಾಖ್ಯಾನಿಸು TIMER_IRQ2          6
#ವ್ಯಾಖ್ಯಾನಿಸು AXIENET_IRQ         7
#ವ್ಯಾಖ್ಯಾನಿಸು AXIDMA_IRQ1         8
#ವ್ಯಾಖ್ಯಾನಿಸು AXIDMA_IRQ0         9

ಸ್ಥಿರ ಶೂನ್ಯ mb_v_generic_init(MachineState *machine)
{
    ram_addr_t ram_ಗಾತ್ರ = machine->ram_ಗಾತ್ರ;
    DeviceState *dev, *dma, *eth0;
    Object *ds, *cs;
    ಪೂರ್ಣಾಂಕ i;
    RISCVCPU *cpu;
    hwaddr ddr_ತಳ = ಸ್ಮರಣೆ_BASEADDR;
    ಸ್ಮರಣೆ_Region *phys_lmb_bram = g_new(ಸ್ಮರಣೆ_Region, 1);
    ಸ್ಮರಣೆ_Region *phys_ram = g_new(ಸ್ಮರಣೆ_Region, 1);
    qemu_irq irq[32];
    ಸ್ಮರಣೆ_Region *sysmem = get_system_ಸ್ಮರಣೆ();

    cpu = RISCV_CPU(object_new(machine->cpu_type));
    object_property_ಗಣ_bool(OBJECT(cpu), "h", false, ಖಾಲಿ);
    object_property_ಗಣ_bool(OBJECT(cpu), "d", false, ಖಾಲಿ);
    qdev_realize(DEVICE(cpu), ಖಾಲಿ, &error_abort);
    /* Attach emulated BRAM through the LMB.  */
    ಸ್ಮರಣೆ_region_init_ram(phys_lmb_bram, ಖಾಲಿ,
                           "mb_v.lmb_bram", LMB_BRAM_ಗಾತ್ರ,
                           &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sysmem, 0x00000000, phys_lmb_bram);

    ಸ್ಮರಣೆ_region_init_ram(phys_ram, ಖಾಲಿ, "mb_v.ram",
                           ram_ಗಾತ್ರ, &error_fatal);
    ಸ್ಮರಣೆ_region_ಕೂಡಿಸು_subregion(sysmem, ddr_ತಳ, phys_ram);

    dev = qdev_new("xlnx.xps-intc");
    qdev_prop_ಗಣ_enum(dev, "endianness", ENDIAN_ಬಹುಲಕ_LITTLE);
    qdev_prop_ಗಣ_uint32(dev, "kind-of-intr",
                         1 << UARTLITE_IRQ);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, INTC_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0,
                       qdev_get_gpio_ಒಳ(DEVICE(cpu), 11));
    ಗಾಗಿ (i = 0; i < 32; i++) {
        irq[i] = qdev_get_gpio_ಒಳ(dev, i);
    }

    /* Uartlite */
    dev = qdev_new(TYPE_XILINX_UARTLITE);
    qdev_prop_ಗಣ_enum(dev, "endianness", ENDIAN_ಬಹುಲಕ_LITTLE);
    qdev_prop_ಗಣ_chr(dev, "chardev", serial_hd(0));
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, UARTLITE_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[UARTLITE_IRQ]);

    /* Full uart */
    serial_mm_init(sysmem, UART16550_BASEADDR + 0x1000, 2,
                   irq[UART16550_IRQ], 115200, serial_hd(1),
                   DEVICE_LITTLE_ENDIAN);

    /* 2 timers at irq 0 @ 100 Mhz.  */
    dev = qdev_new("xlnx.xps-timer");
    qdev_prop_ಗಣ_enum(dev, "endianness", ENDIAN_ಬಹುಲಕ_LITTLE);
    qdev_prop_ಗಣ_uint32(dev, "one-timer-only", 0);
    qdev_prop_ಗಣ_uint32(dev, "clock-frequency", 100000000);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_IRQ]);

    /* 2 timers at irq 3 @ 100 Mhz.  */
    dev = qdev_new("xlnx.xps-timer");
    qdev_prop_ಗಣ_enum(dev, "endianness", ENDIAN_ಬಹುಲಕ_LITTLE);
    qdev_prop_ಗಣ_uint32(dev, "one-timer-only", 0);
    qdev_prop_ಗಣ_uint32(dev, "clock-frequency", 100000000);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, TIMER_BASEADDR2);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[TIMER_IRQ2]);

    /* Emaclite */
    dev = qdev_new("xlnx.xps-ethernetlite");
    qdev_prop_ಗಣ_enum(dev, "endianness", ENDIAN_ಬಹುಲಕ_LITTLE);
    qemu_configure_nic_device(dev, true, ಖಾಲಿ);
    qdev_prop_ಗಣ_uint32(dev, "tx-ping-pong", 0);
    qdev_prop_ಗಣ_uint32(dev, "rx-ping-pong", 0);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, ETHLITE_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, irq[ETHLITE_IRQ]);

    /* axi ethernet and dma initialization. */
    eth0 = qdev_new("xlnx.axi-ethernet");
    dma = qdev_new("xlnx.axi-dma");

    /* FIXME: attach to the sysbus instead */
    object_property_ಕೂಡಿಸು_ಮಗು(qdev_get_machine(), "xilinx-eth", OBJECT(eth0));
    object_property_ಕೂಡಿಸು_ಮಗು(qdev_get_machine(), "xilinx-dma", OBJECT(dma));

    ds = object_property_get_link(OBJECT(dma),
                                  "axistream-connected-target", ಖಾಲಿ);
    cs = object_property_get_link(OBJECT(dma),
                                  "axistream-control-connected-target", ಖಾಲಿ);
    qemu_configure_nic_device(eth0, true, ಖಾಲಿ);
    qdev_prop_ಗಣ_uint32(eth0, "rxmem", 0x1000);
    qdev_prop_ಗಣ_uint32(eth0, "txmem", 0x1000);
    object_property_ಗಣ_link(OBJECT(eth0), "axistream-connected", ds,
                             &error_abort);
    object_property_ಗಣ_link(OBJECT(eth0), "axistream-control-connected", cs,
                             &error_abort);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(eth0), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(eth0), 0, AXIENET_BASEADDR);
    sysbus_connect_irq(SYS_BUS_DEVICE(eth0), 0, irq[AXIENET_IRQ]);

    ds = object_property_get_link(OBJECT(eth0),
                                  "axistream-connected-target", ಖಾಲಿ);
    cs = object_property_get_link(OBJECT(eth0),
                                  "axistream-control-connected-target", ಖಾಲಿ);
    qdev_prop_ಗಣ_uint32(dma, "freqhz", 100000000);
    object_property_ಗಣ_link(OBJECT(dma), "axistream-connected", ds,
                             &error_abort);
    object_property_ಗಣ_link(OBJECT(dma), "axistream-control-connected", cs,
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

ಸ್ಥಿರ ಶೂನ್ಯ mb_v_generic_machine_init(MachineClass *mc)
{
    mc->desc = "AMD Microblaze-V generic platform";
    mc->init = mb_v_generic_init;
    mc->min_cpus = 1;
    mc->max_cpus = 1;
    mc->default_cpu_type = TYPE_RISCV_CPU_ತಳ;
    mc->default_cpus = 1;
}

DEFINE_MACHINE("amd-microblaze-v-generic", mb_v_generic_machine_init)
