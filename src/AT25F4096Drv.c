/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        AT25F4096Drv.c
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

#include "..\h\AT25F4096Drv.h"


AT25F4096Handle * thisFlashMemory;


void AT25F4096Init(AT25F4096Handle* pHandle,  char * buffer)
{
	
	thisFlashMemory = pHandle;
	thisFlashMemory->buffer	= buffer;
	
	
	SPI1CON1bits.DISSCK	= 0;		/* Internal clock enabled			*/
	SPI1CON1bits.DISSDO = 0;		/* SDO controlled by module			*/
	SPI1CON1bits.MODE16	= 0;		/* Byte wide communication			*/
	SPI1CON1bits.SMP	= 1;		/* Input sampled at end of data time	*/
	SPI1CON1bits.CKE	= 1;		/* Data changes from high to low clock*/
	SPI1CON1bits.CKP	= 0;
	SPI1CON1bits.MSTEN	= 1;		/* Master mode enabled			*/
	SPI1CON1bits.SSEN	= 0;		/* SS pin is controlled by port		*/
	
	SPI1CON1bits.SPRE	=  AT25F4096DRV_SPI1_SECONDARY_PRESCALE;
	SPI1CON1bits.PPRE	=  AT25F4096DRV_SPI1_PRIMARY_PRESCALE;
	
	SPI1CON2bits.FRMEN	= 0;		/* Framed SPIx is disabled			*/
	
	AD1PCFGLbits.PCFG2 = 1;			/* SS pin is analog by default. Make it	*/
	AT25F4096_CHIP_SELECT_TRIS 	= 0;/* Make SS1/RB2 output			*/
	AT25F4096_CHIP_SELECT = 1;
	SPI1STATbits.SPISIDL = 0;
	
}

void AT25F4096Start(AT25F4096Handle* pHandle)
{
	_SPI1IE = 1;					/* Enable the interrupts				*/
	SPI1STATbits.SPIEN	= 1;		/* Enable the module				*/
	pHandle->statusFlag	= 0;
	pHandle->statusWord = 0;
	pHandle->dataIndex	= 0;
	pHandle->dataCount 	= AT25F4096DRV_MAX_BUFFER_SIZE;
	AT25F4096_CHIP_SELECT = 1;
}	

int AT25F4096Write(AT25F4096Handle* pHandle, long address,  char* data,   int count)
{	
	unsigned int index;

	unsigned char * dest = (unsigned char *) pHandle->buffer;
	
	dest[0] = AT25F4096_PROGRAM_CODE; 			/* This is the program instruction	*/
	dest[1] = (char)((address & 0x00FF0000) >> 16);	/* Address A23-A16	*/
	dest[2] = (char)((address & 0x0000FF00) >> 8);	 /* Address A15-A8	*/
	dest[3] =  (char)(address & 0x000000FF);			 /* Address A7-A0		*/
	
	if(count >  AT25F4096DRV_MAX_BUFFER_SIZE)
	{

		count =AT25F4096DRV_MAX_BUFFER_SIZE;	/* Size cannot be greater than the buffer size	*/
	}
	
	if( dest[3] != 0)
	{
		/* If the start address is not a 256 byte boundary
		 * and number of bytes to write would make the 
		 * address rollover, then write only that many bytes
		 * that would fit with in the boundary*/
		if(count > (0x100 - dest[3]))
		{ 

			count = (0x100 - dest[3]);
		}
		 
	}
	
	for( index =0;index < count;index ++)			/* Load the data portion of the array	*/
	{
		dest[index + 4] = data[index];
	}
	
	pHandle->statusFlag |= AT25F4096DRV_SET_IS_WRITING;	/* Set the write flag 	*/
	pHandle->dataCount = count + 4;							/* load the count		*/
	AT25F4096_CHIP_SELECT = 0;										/* Enable the chip		*/
	SPI1BUF = dest[0];										/* start the transmission	*/

	return count;
}

void 	AT25F4096Read(AT25F4096Handle* pHandle, long address,  char* data, int count)
{
	char upperAddress,highAddress,lowAddress;

	
	upperAddress = (char)((address & 0x00FF0000) >> 16);	/* Address A23-A16	*/
	highAddress = (char)((address & 0x0000FF00) >> 8);	 /* Address A15-A8	*/
	lowAddress = (char)(address & 0x000000FF);			 /* Address A7-A0		*/
	
	if(count >  AT25F4096DRV_MAX_BUFFER_SIZE)
	{
		pHandle->dataCount = AT25F4096DRV_MAX_BUFFER_SIZE;	/* Size cannot be greater than the buffer size	*/
	}
	else
	{
		pHandle->dataCount = count;
	}
	
	pHandle->readData = data;
	pHandle->dataIndex = 0;
	
	AT25F4096_CHIP_SELECT = 0;
	SPI1BUF = AT25F4096_READ_CODE;
	while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;

	SPI1BUF = upperAddress;
	while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
	SPI1BUF = highAddress;
	while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
	SPI1BUF = lowAddress;
	while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
	SPI1BUF = 0;
	pHandle->statusFlag |= AT25F4096DRV_SET_IS_READING;
	
}	

int		AT25F4096IsBusy	(AT25F4096Handle* pHandle)
{
	if ((pHandle->statusFlag & AT25F4096DRV_GET_IS_WRITING) != 0)
		return(1);
	
	if ((pHandle->statusFlag & AT25F4096DRV_GET_IS_READING) != 0)
		return(1);
	
	AT25F4096_CHIP_SELECT = 0;

	SPI1BUF 	= AT25F4096_READ_STATUS_CODE;		/* Read status register				*/	
	while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;

	SPI1BUF=0x0;					/* Output dummy words for SPI clock	*/
	while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;

	AT25F4096_CHIP_SELECT = 1;
	return((pHandle->statusWord) & 0x01);
}	

void AT25F4096IoCtl(AT25F4096Handle* pHandle,int command, void * data)
{

	
	if(command == AT25F4096DRV_CHIP_ERASE)
	{
		/* 3 steps - Write Enable -> Chip Erase -> Read Status	*/
		/* Check the SPIRBF flag to check if data has been shifted	*/
		/* out completely.									*/
	
		AT25F4096_CHIP_SELECT = 0;
	
		SPI1BUF	= AT25F4096_CHIP_ERASE_CODE;		/* The code for erasing the flash		*/
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
		
		AT25F4096_CHIP_SELECT= 1;
	}
	else if(command == AT25F4096DRV_READ_FLASH_MFG_ID)
	{	
		int wordCount;
		char * target = (char*)data;
		
		AT25F4096_CHIP_SELECT = 0;
	
		SPI1BUF 	= AT25F4096_READ_CHIPID_CODE;		/* Read device id code				*/	
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
		for(wordCount = 0;wordCount <=3; wordCount++)
		{
			SPI1BUF=0x00;					/* Output dummy words for SPI clock	*/
			while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
			pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
			target[wordCount] = pHandle->statusWord;
		}
	
		AT25F4096_CHIP_SELECT =  1;
	}
	
	else if(command == AT25F4096DRV_READ_STATUS)
	{	
		char * target = (char*)data;
		
		AT25F4096_CHIP_SELECT = 0;
	
		SPI1BUF = AT25F4096_READ_STATUS_CODE;		/* Read status register				*/	
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
		SPI1BUF=0x0;					/* Output dummy words for SPI clock	*/
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
		*target = pHandle->statusWord;
	
		AT25F4096_CHIP_SELECT = 1;
	}
	else 	if(command == AT25F4096DRV_WRITE_ENABLE)
	{	
		AT25F4096_CHIP_SELECT = 0;
	
		SPI1BUF	= AT25F4096_WRITE_ENABLE_CODE;		/* write enable command			*/	
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
		AT25F4096_CHIP_SELECT = 1;
	}
	
	else if(command == AT25F4096DRV_WRITE_DISABLE)
	{	
		AT25F4096_CHIP_SELECT = 0;
	
		SPI1BUF 	= AT25F4096_WRITE_DISABLE_CODE;		/* write enable command			*/	
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
		AT25F4096_CHIP_SELECT = 1;
	}
	
	else if(command == AT25F4096DRV_SECTOR_ERASE)
	{
		char *address = (char*)data;
			
		AT25F4096_CHIP_SELECT = 0;
	
		SPI1BUF 			= AT25F4096_SECTOR_ERASE_CODE;		/* write enable command			*/	
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
		
		SPI1BUF = address[2];
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
		SPI1BUF =address[1];
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
		
		SPI1BUF = address[0];
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
		
		AT25F4096_CHIP_SELECT= 1;
	}
	
	
	else if(command == AT25F4096DRV_WRITE_STATUS)
	{
		AT25F4096_CHIP_SELECT = 0;
	
		SPI1BUF = AT25F4096_WRITE_STATUS_CODE;
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
	
		SPI1BUF = *((char*)data);
		while((pHandle->statusFlag & AT25F4096DRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= AT25F4096DRV_CLR_RW_DONE;
		 
		AT25F4096_CHIP_SELECT = 1;
	}
	
}	


void ATSFMISRoutine(void)
{
	_SPI1IF = 0;

	if((thisFlashMemory->statusFlag & AT25F4096DRV_GET_IS_WRITING) != 0)
	{
		/* The driver is performing a write operation	
		 * Perform a dummy read to prevent a read overflow
		 * condition */
		
		thisFlashMemory->statusWord = SPI1BUF;
		thisFlashMemory->dataIndex ++;
		if(thisFlashMemory->dataIndex < thisFlashMemory->dataCount)
		{
			SPI1BUF = (thisFlashMemory->buffer)[thisFlashMemory->dataIndex];
		}else{
		
			thisFlashMemory->statusFlag &= AT25F4096DRV_CLR_IS_WRITING;
			thisFlashMemory->dataCount = 0;
			thisFlashMemory->dataIndex = 0;
			AT25F4096_CHIP_SELECT = 1;
		}
	}
		
	else if((thisFlashMemory->statusFlag & AT25F4096DRV_GET_IS_READING) != 0)
	{
		/* The driver is performing a read operation	
		 * Perform a dummy read to prevent a read overflow
		 * condition */
		
		(thisFlashMemory->readData)[thisFlashMemory->dataIndex] = SPI1BUF;
		thisFlashMemory->dataIndex ++;
		if(thisFlashMemory->dataIndex < thisFlashMemory->dataCount)
		{
			SPI1BUF = 0;
		}else{
		
			thisFlashMemory->statusFlag &= AT25F4096DRV_CLR_IS_READING;
			thisFlashMemory->dataCount = 0;
			thisFlashMemory->dataIndex = 0;
			AT25F4096_CHIP_SELECT = 1;
		}
	}	
	else
	{
		thisFlashMemory->statusFlag |=AT25F4096DRV_SET_RW_DONE;
		thisFlashMemory->statusWord = SPI1BUF;
	}
	
	
}	
