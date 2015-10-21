/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        SST25VF040BDrv.h
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx
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

#ifndef __SST25VF040BDRV_H__
#define __SST25VF040BDRV_H__

#include "..\h\p33FJ256GP506.h"

/* While changing SST25VF040BDRV_BUFFER_SIZE, the corresponding
 * value in SST25VF040BDrv.s must also be changed. Both should 
 * match.
 */

#define SST25VF040BDRV_BUFFER_SIZE					256
#define SST25VF040BDRV_LAST_ADDRESS					0x7FFFF
		/* Timer period for 20usec	*/

typedef struct s_SST25VF040BHandle
{
	volatile char * buffer;
	volatile int statusFlag;
	volatile int dataCount;
	volatile int dataIndex;
	unsigned long writeAddress;
	
}SST25VF040BHandle;	


/* These are the commands that can be used with 
 * ioctl functions */
 
#define SST25VF040BDRV_CHIP_ERASE			0x1
#define SST25VF040BDRV_READ_FLASH_MFG_ID	0x2
#define SST25VF040BDRV_READ_STATUS			0x3
#define SST25VF040BDRV_WRITE_ENABLE			0x4
#define SST25VF040BDRV_WRITE_STATUS			0x5
#define SST25VF040BDRV_BLOCK_ERASE			0x9

/********************************************
 * Function: SST25VF040B_open 
 * Input:
 * pHandle - Pointer to the uninitialized SFM DS
 * buffer - char type buffer which is used by the 
 *			driver internally
 * Description:
 * This function intialized the SFM DS. It will 
 * initialize the SPI module, initialise the 
 * ports and the program timer T4.
 * Return:
 * SST25VF040BHandle type pointer to the
 * intialized SFM DS
 **********************************************/ 

 SST25VF040BHandle* 	SST25VF040B_open	(SST25VF040BHandle* pHandle, char * buffer);

/********************************************
 * Function: SST25VF040B_write 
 * Input:
 * pHandle - Pointer to the initialized SFM DS
 * address - 24 bit destination SFM address 
 * data - pointer to data to write.
 * count - number of bytes to writes
 * Description:
 * This function will write data to the SFM at
 * address specified by address. Function is non-
 * blocking. If the function call is attempted 
 * while a previous write is being completed
 * the function will not have any effect and will
 * return 0.
 * Return:
 * Number of bytes written.
 **********************************************/ 

int 	SST25VF040B_write	(SST25VF040BHandle* pHandle, long address,  char* data,   int count);

/********************************************
 * Function: SST25VF040B_read
 * Input:
 * pHandle - Pointer to the initialized SFM DS
 * address - 24 bit source SFM address 
 * data - pointer to destination array.
 * count - number of bytes to read
 * Description:
 * This function will read data from the SFM from
 * address specified by address. Function is 
 * blocking. If the function call is attempted 
 * while a previous write is being completed
 * the function will not have any effect and will
 * return 0.
 * Return:
 * Number of bytes read.
 **********************************************/ 
int 	SST25VF040B_read	(SST25VF040BHandle* pHandle, long address,  char* data,int count);

/********************************************
 * Function: SST25VF040B_ioctl
 * Input:
 * pHandle - Pointer to the initialized SFM DS
 * command - command to be executed
 * data - void * to data structure
 * Description:
 * This function performs specific commands on 
 * on the SFM. The command is specified by command
 * and argument is provided by data.
 *
 * Command								data (pointer to)
 * SST25VF040BDRV_CHIP_ERASE			null
 * SST25VF040BDRV_READ_FLASH_MFG_ID		4 byte array
 * SST25VF040BDRV_READ_STATUS			1 byte
 * SST25VF040BDRV_WRITE_ENABLE			null
 * SST25VF040BDRV_WRITE_STATUS			1 byte command
 * SST25VF040BDRV_BLOCK_ERASE			long address
 *
 * Return:
 * Will return a zero if command is successful
 **********************************************/

int 	SST25VF040B_ioctl	(SST25VF040BHandle* pHandle,int command, void *data);



/* **********************************************
 * Function: SSTSFMISRoutine
 * Input: None
 * Description: This routine is called from the
 * SFMDriver SPI Interrupt service routine
 * Return:
 * None
 * ***********************************************/
 
void SSTSFMISRoutine(void);

#endif
