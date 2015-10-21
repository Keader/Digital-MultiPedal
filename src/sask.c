/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        sask.c
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

#include "..\h\sask.h"

int debounceS1;						/* Debounce counter for switch S1	*/
int debounceS2;						/* Debounce counter for switch S2	*/

void SASKInit(void)
{
	/* Intialize the board LED and swicth ports	
	 * and turn all LEDS off. Also switches on the 
	 * VDD regulator on the serial flash memory 
	 * chip 	*/
	 
	long vddRegWakeUpDelay;
	
	YELLOW_LED_TRIS	= 0;
	RED_LED_TRIS = 0;		
 	GREEN_LED_TRIS = 0;	

 	YELLOW_LED = SASK_LED_OFF;	
	RED_LED	= SASK_LED_OFF;		
 	GREEN_LED = SASK_LED_OFF;		

 	SWITCH_S1_TRIS = 1;	
 	SWITCH_S2_TRIS	= 1;
 	
	VOLUME_UPDN_TRIS = 0;	
	VOLUME_CLK_TRIS	= 0;
	
	VOLUME_UPDN = 0;
	VOLUME_CLK = 0;	
	
	debounceS1 		= 0;
	debounceS2 		= 0;

	REGULATOR_CONTROL_ANPCFG = 1;
	REGULATOR_CONTROL_TRIS	= 0;
	REGULATOR_CONTROL_LAT = 1;
	for(vddRegWakeUpDelay = 0;
	vddRegWakeUpDelay < REGULATOR_WAKE_UP_DELAY;
	vddRegWakeUpDelay++)
	Nop();


}

int CheckSwitchS1(void)
{
	/* This function returns a 1 if a valid key press was detected on SW1 */
	int isActive;
	if(SWITCH_S1 == 0){
		
		debounceS1++;
		if (debounceS1 == SWITCH_DEBOUNCE){
			
			/* This means that the Switch S1 was pressed long enough
			 * for a valid key press	*/
			 isActive = 1;
		}
	}
	else	{
		debounceS1 = 0;
		isActive = 0;
	}
	
	return(isActive);
}	


int CheckSwitchS2(void)
{
	/* This function returns a 1 if a valid key press was detected on SW2 */
	int isActive;
	if(SWITCH_S2 == 0){
		/* If SW2 press was valid then toggle the record function	*/
		debounceS2++;
		if (debounceS2 == SWITCH_DEBOUNCE){
			
			/* This means that the Switch S1 was pressed long enough
			 * for a valid key press	*/
			isActive = 1;
		}
	}
	else	{
		debounceS2 = 0;
		isActive = 0;
	}
	return(isActive);
}

