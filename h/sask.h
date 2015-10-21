/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        sask.h
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33FJ256GP506
* Compiler:        MPLAB® C30 v3.00 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
************************************************************************/
#ifndef __SASK_BOARD_H__
#define __SASK_BOARD_H__
#include "..\h\p33FJ256GP506.h"

/* The LEDS and their ports	*/
#define YELLOW_LED_TRIS		_TRISC13
#define GREEN_LED_TRIS		_TRISC14
#define RED_LED_TRIS		_TRISC15

#define YELLOW_LED		_LATC13
#define GREEN_LED			_LATC14
#define RED_LED		_LATC15

/* The Switches and their ports */

#define SWITCH_S1_TRIS	_TRISD8
#define SWITCH_S2_TRIS	_TRISD9

#define SWITCH_S1	_RD8
#define SWITCH_S2	_RD9

/* Volume control pins	*/

#define VOLUME_UPDN_TRIS	_TRISG0
#define VOLUME_CLK_TRIS		_TRISG1

#define VOLUME_UPDN	_LATG0
#define VOLUME_CLK	_LATG1

#define SWITCH_DEBOUNCE			12	/* Wait for this many times before switch press is valid		
									 * For this application, each increment is 
									 * occurs every at sampling interval *
									 * frames size */
#define SASK_LED_ON 	0
#define SASK_LED_OFF	1

#define REGULATOR_CONTROL_TRIS		TRISBbits.TRISB5
#define REGULATOR_CONTROL_ANPCFG	AD1PCFGLbits.PCFG5
#define REGULATOR_CONTROL_LAT		LATBbits.LATB5
#define REGULATOR_WAKE_UP_DELAY	0x800

void SASKInit(void);
int CheckSwitchS1(void);
int CheckSwitchS2(void);

#endif
