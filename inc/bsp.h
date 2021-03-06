/*! @file bsp.h
 * @brief This file contains application specific definitions and includes.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef BSP_H
#define BSP_H

/*------------------------------------------------------------------------*/
/*            Application specific global definitions                     */
/*------------------------------------------------------------------------*/
/*! Platform definition */
/* Note: Plaform is defined in Silabs IDE project file as
 * a command line flag for the compiler. */
//#define SILABS_PLATFORM_WMB930

/*! Extended driver support 
 * Known issues: Some of the example projects 
 * might not build with some extended drivers 
 * due to data memory overflow */
#undef  RADIO_DRIVER_EXTENDED_SUPPORT
#undef  RADIO_DRIVER_FULL_SUPPORT
#undef  SPI_DRIVER_EXTENDED_SUPPORT
#undef  HMI_DRIVER_EXTENDED_SUPPORT
#define  TIMER_DRIVER_EXTENDED_SUPPORT
#define  UART_DRIVER_EXTENDED_SUPPORT

/*------------------------------------------------------------------------*/
/*            Application specific includes                               */
/*------------------------------------------------------------------------*/

#include "compiler_defs.h"
#include "platform_defs.h"
#include "hardware_defs.h"

#include "..\drivers\drivers.h"

#include "..\net\net.h"

/*
#ifdef SILABS_RADIO_SI446X
#include "drivers\radio\Si446x\si446x_api_lib.h"
#include "drivers\radio\Si446x\si446x_defs.h"
#include "drivers\radio\Si446x\si446x_nirq.h"
//#include "drivers\radio\Si446x\si446x_patch.h"
#endif
*/

#ifdef SILABS_RADIO_SI4455
#include "drivers\radio\Si4455\si4455_api_lib.h"
#include "drivers\radio\Si4455\si4455_defs.h"
#include "drivers\radio\Si4455\si4455_nirq.h"
#endif

#endif //BSP_H
