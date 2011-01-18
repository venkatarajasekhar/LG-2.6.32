/*
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2008-2009, Code Aurora Forum. All rights reserved.
 * Copyright (c) 2010 LGE. All rights reserved.
 * Author: Brian Swetland <swetland@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#ifdef CONFIG_USB_FUNCTION
#include <linux/usb/mass_storage_function.h>
#endif
#include <linux/i2c.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/flash.h>
#ifdef CONFIG_CACHE_L2X0
#include <asm/hardware/cache-l2x0.h>
#endif

#include <mach/hardware.h>
#include <mach/msm_hsusb.h>
#ifdef CONFIG_USB_ANDROID
#include <linux/usb/android.h>
#endif
#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <mach/msm_serial_hs.h>
#include <mach/memory.h>
#include <mach/msm_battery.h>
#include <mach/gpio.h>
#include <mach/board_lge.h>

#include "devices.h"
#include "socinfo.h"
#include "clock.h"
#include "msm-keypad-devices.h"
#include "board-alohav.h"
#include "pm.h"
#include "lg_fw_diag_communication.h"

/* board-specific pm tuning data definitions */

/* currently, below declaration code is blocked.
 * if power management tuning is required in any board,
 * below "msm7x27_pm_data" array can be redefined and can be unblocked.
 * qualocomm's default setting value is configured in devices_lge.c
 * but that variable is declared in weak attribute
 * so board specific configuration can be redefined like "over riding" in OOP
 */
extern struct msm_pm_platform_data msm7x27_pm_data[MSM_PM_SLEEP_MODE_NR];
#if 0
struct msm_pm_platform_data msm7x27_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].latency = 16000,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].residency = 20000,

	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].supported = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].suspend_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].latency = 12000,
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].residency = 20000,

	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].supported = 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].suspend_enabled
		= 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].idle_enabled = 1,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency = 2000,
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].residency = 0,
};
#endif

/* board-specific usb data definitions */
#ifdef CONFIG_USB_SUPPORT_LGDRIVER
struct usb_mass_storage_platform_data usb_mass_storage_pdata = {
	.nluns          = 0x01,
	.buf_size       = 16384,
	.vendor         = "GOOGLE",
	.product        = "Mass storage",
	.release        = 0xffff,
};
#endif

#ifdef CONFIG_USB_ANDROID
/* dynamic composition */
struct usb_composition usb_func_composition[] = {
	{
		.product_id         = 0x9015,
		/* MSC + ADB */
		.functions	    = 0x12 /* 10010 */
	},
	{
		.product_id         = 0xF000,
		/* MSC */
		.functions	    = 0x02, /* 0010 */
	},
	{
		.product_id         = 0xF005,
		/* MODEM ONLY */
		.functions	    = 0x03,
	},

	{
		.product_id         = 0x8080,
		/* DIAG + MODEM */
		.functions	    = 0x34,
	},
	{
		.product_id         = 0x8082,
		/* DIAG + ADB + MODEM */
		.functions	    = 0x0314,
	},
	{
		.product_id         = 0x8085,
		/* DIAG + ADB + MODEM + NMEA + MSC*/
		.functions	    = 0x25314,
	},
	{
		.product_id         = 0x9016,
		/* DIAG + GENERIC MODEM + GENERIC NMEA*/
		.functions	    = 0x764,
	},
	{
		.product_id         = 0x9017,
		/* DIAG + GENERIC MODEM + GENERIC NMEA + MSC*/
		.functions	    = 0x2764,
	},
	{
		.product_id         = 0x9018,
		/* DIAG + ADB + GENERIC MODEM + GENERIC NMEA + MSC*/
		.functions	    = 0x27614,
	},
	{
		.product_id         = 0xF009,
		/* CDC-ECM*/
		.functions	    = 0x08,
	}
};
struct android_usb_platform_data android_usb_pdata = {
	.vendor_id	= 0x05C6,
	.product_id	= 0x9018,
	.functions	= 0x27614,
	.version	= 0x0100,
	.compositions   = usb_func_composition,
	.num_compositions = ARRAY_SIZE(usb_func_composition),
	.product_name	= "Qualcomm HSUSB Device",
	.manufacturer_name = "Qualcomm Incorporated",
	.nluns = 1,
};
static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id		= -1,
	.dev		= {
		.platform_data = &android_usb_pdata,
	},
};
#endif

#ifdef CONFIG_USB_SUPPORT_LGDRIVER
static struct usb_function_map usb_functions_map[] = {
	{"modem", 0},
	{"diag", 1},
	{"nmea", 2},
#ifdef CONFIG_USB_SUPPORT_LGDRIVER_LEGACY
	{"mass_storage", 4},
	{"adb", 3},
#else	
	{"mass_storage", 3},
	{"adb", 4},
#endif
	{"ethernet", 5},
};

/* dynamic composition */
struct usb_composition usb_func_composition[] = {
#ifdef CONFIG_USB_SUPPORT_LGDRIVER_LEGACY
	{
		.product_id         = 0x9018,
		.functions	    = 0x1F, /* 011111 */
	},
	{
		.product_id         = 0x901A,
		.functions          = 0x0F, /* 01111 */
	},
	{
		.product_id 		  = 0x6000,
		.functions			  = 0x03 /* 000011 	Diag, Modem*/
	},
	{
		.product_id 		  = 0x6004,
		.functions			  = 0x08, /* 01000 	ADB only*/
	},
	{
		.product_id 		  = 0x6002,
		.functions			  = 0x0B, /* 01011 	Diag, Modem, ADB*/
	},
	{
		.product_id 		  = 0x6003,
		.functions			  = 0x12, 	/* 00010	Diag only, Mass Storage*/
	},
	{
		.product_id 		  = 0x6001,
		.functions			  = 0x10, /* 10000 	mass storage only*/
	},
	{
		.product_id 		  = 0x6005,
		.functions			  = 0x18, /* 11000 	mass storage, ADB*/
	},	
	{
		.product_id 		  = 0x6171,
		.functions			  = 0x1A, /* 01010 	Diag, ADB, Mass storage*/
	},	
#else
	{
		.product_id         = 0x9012,
		.functions	    = 0x5, /* 0101 */
	},

	{
		.product_id         = 0x9013,
		.functions	    = 0x15, /* 10101 */
	},

	{
		.product_id         = 0x9014,
		.functions	    = 0x30, /* 110000 */
	},

	{
		.product_id         = 0x9016,
		.functions	    = 0xD, /* 01101 */
	},

	{
		.product_id         = 0x9017,
		.functions	    = 0x1D, /* 11101 */
	},

	{
		.product_id         = 0xF000,
		.functions	    = 0x10, /* 10000 */
	},

	{
		.product_id         = 0xF009,
		.functions	    = 0x20, /* 100000 */
	},
	{
		.product_id 		  = 0x6000,
		.functions			  = 0x07 /* 000111 	NMEA, Diag, Modem*/
	},
	{
		.product_id 		  = 0x6001,
		.functions			  = 0x17 /* 010111 	NMEA, Diag, Modem,ADB*/
	},
    {
        .product_id           = 0x6002,
        .functions            = 0x18 /* 011000  Mass, ADB*/
    },
    {
        .product_id           = 0x6003,
        .functions            = 0x0F /* 001111  Modem,diag,NMEA,Mass*/
    },
	{
		.product_id 		  = 0x618E,
		.functions			  = 0x1F /* 011111 	Modem,diag,NMEA,Mass,ADB*/
	},
	{
		.product_id 		  = 0x618F,
        .functions            = 0x08 /* 001000  Mass*/
	},
#endif
};
#endif

#ifdef CONFIG_USB_SUPPORT_LGDRIVER
struct msm_hsusb_platform_data msm_hsusb_pdata = {
	.version	= 0x0100,
	.phy_info	= (USB_PHY_INTEGRATED | USB_PHY_MODEL_65NM),
	.vendor_id          = 0x1004,
	.product_name       = "LG Mobile USB Modem",
	.serial_number		= "LGANDROIDVS740",	
	.manufacturer_name	= "LG Electronics Inc.",
	.compositions	= usb_func_composition,
	.num_compositions = ARRAY_SIZE(usb_func_composition),
	.function_map   = usb_functions_map,
	.num_functions	= ARRAY_SIZE(usb_functions_map),
	.config_gpio    = NULL,
};
#endif

static struct diagcmd_platform_data lg_fw_diagcmd_pdata = {
	.name = "lg_fw_diagcmd",
};

static struct platform_device lg_fw_diagcmd_device = {
	.name = "lg_fw_diagcmd",
	.id = -1,
	.dev    = {
		.platform_data = &lg_fw_diagcmd_pdata
	},
};

static struct platform_device lg_diag_cmd_device = {
	.name = "lg_diag_cmd",
	.id = -1,
	.dev    = {
		.platform_data = 0, //&lg_diag_cmd_pdata
	},
};

static int alohav_reboot_key_detect(void)
{
	if (gpio_get_value(GPIO_PP2106_IRQ) == 0)
		return REBOOT_KEY_PRESS;
	else
		return REBOOT_KEY_NOT_PRESS;
}

struct lge_panic_handler_platform_data panic_handler_data = {
	.reboot_key_detect = alohav_reboot_key_detect,
};

static struct platform_device *devices[] __initdata = {
	&msm_device_smd,
	&msm_device_dmov,
	&msm_device_nand,
	&msm_device_i2c,
	&msm_device_uart_dm2,
	&msm_device_snd,
	&msm_device_adspdec,
	&lg_fw_diagcmd_device,
	&lg_diag_cmd_device,
};

extern struct sys_timer msm_timer;

static void __init msm7x2x_init_irq(void)
{
	msm_init_irq();
}

static struct msm_acpu_clock_platform_data msm7x2x_clock_data = {
	.acpu_switch_time_us = 50,
	.max_speed_delta_khz = 256000,
	.vdd_switch_time_us = 62,
	.max_axi_khz = 160000,
};

void msm_serial_debug_init(unsigned int base, int irq,
			   struct device *clk_device, int signal_irq);

unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;

static void __init msm7x2x_init(void)
{
	if (socinfo_init() < 0)
		BUG();

	msm_clock_init(msm_clocks_7x27, msm_num_clocks_7x27);

#if defined(CONFIG_MSM_SERIAL_DEBUGGER)
	msm_serial_debug_init(MSM_UART1_PHYS, INT_UART1,
			&msm_device_uart1.dev, 1);
#endif

	if (cpu_is_msm7x27())
		msm7x2x_clock_data.max_axi_khz = 200000;

	msm_acpu_clock_init(&msm7x2x_clock_data);

	msm_add_pmem_devices();
	msm_add_fb_device();
#if !defined(CONFIG_MSM_SERIAL_DEBUGGER)
	if (lge_get_uart_mode())
		platform_device_register(&msm_device_uart1);
#endif
	platform_add_devices(devices, ARRAY_SIZE(devices));
#ifdef CONFIG_ARCH_MSM7X27
	msm_add_kgsl_device();
#endif
	msm_add_usb_devices();

#ifdef CONFIG_MSM_CAMERA
	config_camera_off_gpios(); /* might not be necessary */
#endif
	msm_device_i2c_init();
	i2c_register_board_info(0, i2c_devices, ARRAY_SIZE(i2c_devices));

	msm_pm_set_platform_data(msm7x27_pm_data, ARRAY_SIZE(msm7x27_pm_data));

	lge_add_ramconsole_devices();
	lge_add_ers_devices();
#ifdef LG_BLOCK_KERNEL_MISC
	// block if the TARGET_BUILD_VARIANT is user
#else
	lge_add_panic_handler_devices();
#endif
	lge_add_camera_devices();
	lge_add_lcd_devices();
	lge_add_btpower_devices();
	lge_add_mmc_devices();
	lge_add_input_devices();
	lge_add_misc_devices();
	
	/* gpio i2c devices should be registered at latest point */
	lge_add_gpio_i2c_devices();
}

static void __init msm7x2x_map_io(void)
{
	msm_map_common_io();
	/* Technically dependent on the SoC but using machine_is
	 * macros since socinfo is not available this early and there
	 * are plans to restructure the code which will eliminate the
	 * need for socinfo.
	 */
	//msm_clock_init(msm_clocks_7x27, msm_num_clocks_7x27);

	msm_msm7x2x_allocate_memory_regions();

#ifdef CONFIG_CACHE_L2X0
	/* 7x27 has 256KB L2 cache:
		64Kb/Way and 4-Way Associativity;
		R/W latency: 3 cycles;
		evmon/parity/share disabled. */
	l2x0_init(MSM_L2CC_BASE, 0x00068012, 0xfe000000);
#endif
}

MACHINE_START(MSM7X27_ALOHAV, "ALOHA Verizone board (LGE VS740)")
#ifdef CONFIG_MSM_DEBUG_UART
	.phys_io        = MSM_DEBUG_UART_PHYS,
	.io_pg_offst    = ((MSM_DEBUG_UART_BASE) >> 18) & 0xfffc,
#endif
	.boot_params	= 0x00200100,
	.map_io			= msm7x2x_map_io,
	.init_irq		= msm7x2x_init_irq,
	.init_machine	= msm7x2x_init,
	.timer			= &msm_timer,
MACHINE_END
