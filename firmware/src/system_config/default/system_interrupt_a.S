/*******************************************************************************
 System Interrupt Source File

  File Name:
    sys_interrupt_a.S

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the 
    interrupt sub-system.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END
/*
*********************************************************************************************************
*                                           INCLUDES
*********************************************************************************************************
*/
#include <xc.h>



#include "ISR_Support.h"

/* Change Notice Interrupt */
   .extern  IntHandlerChangeNotification

   .section	.vector_34,code, keep
   .equ     __vector_dispatch_34, IntVectorChangeNotification
   .global  __vector_dispatch_34
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorChangeNotification

IntVectorChangeNotification:
    la    $26,  _IntHandlerChangeNotification         
    jr    $26
    nop
	.end	IntVectorChangeNotification

   .section	.ChangeNotification_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerChangeNotification

_IntHandlerChangeNotification:
    portSAVE_CONTEXT
    la    s6,  IntHandlerChangeNotification
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerChangeNotification


 

/* TMR Instance 0 Interrupt */
   .extern  IntHandlerDrvTmrInstance0

   .section	.vector_12,code, keep
   .equ     __vector_dispatch_12, IntVectorDrvTmrInstance0
   .global  __vector_dispatch_12
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvTmrInstance0

IntVectorDrvTmrInstance0:
    la    $26,  _IntHandlerDrvTmrInstance0         
    jr    $26
    nop
	.end	IntVectorDrvTmrInstance0

   .section	.DrvTmrInstance0_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvTmrInstance0

_IntHandlerDrvTmrInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvTmrInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvTmrInstance0




/* USART Instance 0 Interrupt */

   .extern  IntHandlerDrvUsartInstance0

   .section	.vector_32,code, keep
   .equ     __vector_dispatch_32, IntVectorDrvUsartInstance0
   .global  __vector_dispatch_32
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvUsartInstance0

IntVectorDrvUsartInstance0:
    la    $26,  _IntHandlerDrvUsartInstance0         
    jr    $26
    nop
	.end	IntVectorDrvUsartInstance0

   .section	.DrvUsartInstance0_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvUsartInstance0

_IntHandlerDrvUsartInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvUsartInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvUsartInstance0



/* USART Instance 1 Interrupt */

   .extern  IntHandlerDrvUsartInstance1

   .section	.vector_37,code, keep
   .equ     __vector_dispatch_37, IntVectorDrvUsartInstance1
   .global  __vector_dispatch_37
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorDrvUsartInstance1

IntVectorDrvUsartInstance1:
    la    $26,  _IntHandlerDrvUsartInstance1         
    jr    $26
    nop
	.end	IntVectorDrvUsartInstance1

   .section	.DrvUsartInstance1_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerDrvUsartInstance1

_IntHandlerDrvUsartInstance1:
    portSAVE_CONTEXT
    la    s6,  IntHandlerDrvUsartInstance1
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerDrvUsartInstance1




/* SPI Instance 0 Interrupt */

   .extern  IntHandlerSPIInstance0

   .section	.vector_31,code, keep
   .equ     __vector_dispatch_31, IntVectorSPIInstance0
   .global  __vector_dispatch_31
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  IntVectorSPIInstance0

IntVectorSPIInstance0:
    la    $26,  _IntHandlerSPIInstance0         
    jr    $26
    nop
	.end	IntVectorSPIInstance0

   .section	.SPIInstance0_vector_text, code, keep
   .set     nomicromips
   .set     noreorder
   .set     nomips16
   .set     noat
   .ent  _IntHandlerSPIInstance0

_IntHandlerSPIInstance0:
    portSAVE_CONTEXT
    la    s6,  IntHandlerSPIInstance0
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end	_IntHandlerSPIInstance0


/*******************************************************************************
 End of File
 */

