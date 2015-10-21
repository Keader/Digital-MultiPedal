/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        AT25F4096Drv.h
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

#ifndef __AT25F4096DRV_H__
#define __AT25F4096DRV_H__

#include "..\h\p33FJ256GP506.h"

#define AT25F4096DRV_SPI1_PRIMARY_PRESCALE			0x2
#define AT25F4096DRV_SPI1_SECONDARY_PRESCALE		0x6
#define AT25F4096DRV_MAX_BUFFER_SIZE				256
#define AT25F4096DRV_BUFFER_SIZE						(AT25F4096DRV_MAX_BUFFER_SIZE + 4)
#define AT25F4096DRV_LAST_ADDRESS					0x7FFFF


typedef struct s_AT25F4096Handle
{
	volatile char * buffer;
	volatile int statusWord;
	volatile int statusFlag;
	volatile int dataCount;
	volatile int dataIndex;
	volatile char* readData;
	
}AT25F4096Handle;	

#define AT25F4096DRV_SET_RW_DONE	0x0001
#define AT25F4096DRV_CLR_RW_DONE 	0xFFFE
#define AT25F4096DRV_GET_RW_DONE 	0x0001

#define AT25F4096DRV_SET_IS_READING	0x0002
#define AT25F4096DRV_CLR_IS_READING	0xFFFD
#define AT25F4096DRV_GET_IS_READING	0x0002

#define AT25F4096DRV_SET_IS_WRITING	0x0004
#define AT25F4096DRV_CLR_IS_WRITING	0xFFFB
#define AT25F4096DRV_GET_IS_WRITING	0x0004

/* These are the commands that can be used with 
 * ioctl functions */
 
#define AT25F4096DRV_CHIP_ERASE			0x1
#define AT25F4096DRV_READ_FLASH_MFG_ID	0x2
#define AT25F4096DRV_READ_STATUS		0x3
#define AT25F4096DRV_WRITE_ENABLE		0x4
#define AT25F4096DRV_WRITE_STATUS		0x5
#define AT25F4096DRV_WRITE_DISABLE		0x6
#define AT25F4096DRV_SECTOR_ERASE		0x7

#define AT25F4096_PROGRAM_CODE			0x2
#define AT25F4096_READ_CODE				0x3
#define AT25F4096_CHIP_ERASE_CODE		0x62
#define AT25F4096_READ_CHIPID_CODE		0x15
#define AT25F4096_READ_STATUS_CODE		0x5
#define AT25F4096_WRITE_ENABLE_CODE		0x6
#define AT25F4096_WRITE_DISABLE_CODE	0x4
#define AT25F4096_SECTOR_ERASE_CODE	0x52
#define AT25F4096_WRITE_STATUS_CODE	0x1


#define AT25F4096_CHIP_SELECT_TRIS		TRISBbits.TRISB2 
#define AT25F4096_CHIP_SELECT			LATBbits.LATB2 	
#define AT25F4096_CHIP_SELECT_ANPORT	AD1PCFGLbits.PCFG2



void 	AT25F4096Init		(AT25F4096Handle* pHandle, char * buffer);
int		AT25F4096Write	(AT25F4096Handle* pHandle, long address,  char* data,   int count);
void		AT25F4096Read	(AT25F4096Handle* pHandle, long address,  char* data,int count);
void 	AT25F4096IoCtl	(AT25F4096Handle* pHandle,int command, void *data);
int		AT25F4096IsBusy	(AT25F4096Handle* pHandle);
void 	AT25F4096Start	(AT25F4096Handle* pHandle);

void ATSFMISRoutine(void);

#endif
