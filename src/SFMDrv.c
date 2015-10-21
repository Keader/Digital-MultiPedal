#include "..\h\SFMDrv.h"

/* Declare the SFM Driver Structures
 * globally. The SFM type will be detected
 * at run time and only one of these will 
 * be used.
 * */

static AT25F4096Handle atSFM;
static SST25VF040BHandle sstSFM;
static AT25F4096Handle * atSFMHandle = &atSFM;
static SST25VF040BHandle * sstSFMHandle;
static int sfmType;

/***********************************************
 * Function - SFMInit
 * Arguments: char type array to be used by the 
 * driver for internal buffering.
 * Description: This function will intialize the 
 * SFM driver structure and the SPI module. It will
 * detect the SFM being used and will internally 
 * register the SFM type being used on the board
 * Return Value - None
 * *********************************************/

void SFMInit(char * flashMemoryBuffer)
{
	/* Set up the SPI1CON with values 
	 * which will guarantee communication
	 * with either SFM devices.
	 * */
	
	int readWord;
	int chipId[2];
	int unprotect = 0;

	SPI1CON1 = 0x33A;

	/* Set up the SFM chip select port
	 * on the starter kit board.
	 * */
	TRISBbits.TRISB2 = 0; 
	AD1PCFGLbits.PCFG2 = 1;
	LATBbits.LATB2 = 1;

	/* Enable the SPI module */
	SPI1STATbits.SPIEN = 1;

	/* Try for the SST SFM. The SST chip code
	 * read command is 0x90. Output command
	 * send three 0's and then
	 * read the next two words. Enable and 
	 * disable chip select at the start and
	 * end of the function. Clear interrupts
	 * at the end.
	 * */

	LATBbits.LATB2 = 0;
	
	SPI1BUF = 0x90;
	while(SPI1STATbits.SPIRBF == 0);
	readWord = SPI1BUF;

	SPI1BUF = 0x00;
	while(SPI1STATbits.SPIRBF == 0);
	readWord = SPI1BUF;

	SPI1BUF = 0x00;
	while(SPI1STATbits.SPIRBF == 0);
	readWord = SPI1BUF;
		
	SPI1BUF = 0x00;
	while(SPI1STATbits.SPIRBF == 0);
	readWord = SPI1BUF;

	/* Now read the chip id */

	SPI1BUF = 0x00;
	while(SPI1STATbits.SPIRBF == 0);
	chipId[0] = SPI1BUF;

	SPI1BUF = 0x00;
	while(SPI1STATbits.SPIRBF == 0);
	chipId[1] = SPI1BUF;

	LATBbits.LATB2 = 1;
	_SPI1IF = 0;
	
	/* If the chip is 0xBF, then
	 * the SFM chip is SST type
	 * else it is the AT type.
	 * */

	if (chipId[0] == 0xBF)
	{
		/* If the SFM type is SST, the set type
		 * to SST. Open the SST Driver. Unprotect
		 * the device so that application can write
		 * to it. 
		 * */
		
		sfmType = SSTSFM;
		sstSFMHandle = SST25VF040B_open(&sstSFM,flashMemoryBuffer);
		SST25VF040B_ioctl(sstSFMHandle,
				SST25VF040BDRV_WRITE_STATUS,(void *)(&unprotect));


	}else
	{
		/* If the SFM Type is AT, then set type
		 * to AT. Intialize the AT SFM Driver and
		 * then start the driver.
		 * */

		sfmType = ATSFM;
		AT25F4096Init (atSFMHandle,flashMemoryBuffer);	
		AT25F4096Start	(atSFMHandle);

	}

}

/***********************************************
 * Function - SFMChipErase
 * Arguments: None
 * Description: This function will erase the SFM.
 * The function is blocking and will return when 
 * chip erase is complete
 * Return Value - None
 * *********************************************/

void SFMChipErase(void)
{
	if (sfmType == SSTSFM)
	{
		while((SST25VF040B_ioctl(sstSFMHandle, 
						SST25VF040BDRV_CHIP_ERASE, 0)) == 0);
	}
	else
	{
		AT25F4096IoCtl(atSFMHandle,AT25F4096DRV_WRITE_ENABLE,0);	
		AT25F4096IoCtl(atSFMHandle,AT25F4096DRV_CHIP_ERASE,(void *)0);	
		while(AT25F4096IsBusy(atSFMHandle));	
	}
}

/***********************************************
 * Function - SFMBlockErase
 * Arguments: 
 * address - address of block to delete
 * Description: This function will delete a 64K 
 * SFM block.
 * Return Value - none.
 * *********************************************/

void SFMBlockErase(long address)
{
	if(sfmType == SSTSFM)
	{
		SST25VF040B_ioctl(sstSFMHandle,
				SST25VF040BDRV_BLOCK_ERASE,(void*)(&address));
	}
	else
	{
		/* Erase each sector. Each sector is 0xFFFF long	*/
		AT25F4096IoCtl(atSFMHandle,AT25F4096DRV_WRITE_ENABLE,0);	
		AT25F4096IoCtl(atSFMHandle,AT25F4096DRV_SECTOR_ERASE,(void *)&address);	
		while(AT25F4096IsBusy(atSFMHandle));

	}
}

/***********************************************
 * Function - SFMWrite
 * Arguments:
 * address - SFM destination address 
 * data - byte pointer to source data
 * count - number of bytes to write
 * Description: This function will write count
 * bytes from the data array to the SFM destination
 * address. Function is blocking.
 * Return Value - Number of bytes written
 * *********************************************/

int SFMWrite(long address, char * data, int count)
{
	int written = 0;
	if(sfmType == SSTSFM)
	{
		while(written < count)
		{
			written += SST25VF040B_write(sstSFMHandle,
						(address + written),(char *)(data + written),
						(count - written));
		}

	}
	else
	{
		while(written < count)
		{
			while(AT25F4096IsBusy(atSFMHandle));
			AT25F4096IoCtl(atSFMHandle,AT25F4096DRV_WRITE_ENABLE,0);	
			written += AT25F4096Write(atSFMHandle,
				(address + written) ,(char *)(data + written),
				(count - written));
		}

	
	}
	return(count);
}

/***********************************************
 * Function - SFMRead
 * Arguments:
 * address - SFM source addres
 * data - byte pointer to destination buffer
 * count - number of bytes to read
 * Description: This function will read count
 * bytes of data from the SFM address and store
 * it in data buffer
 * Return Value - number of bytes read
 * *********************************************/

int SFMRead(long address, char * data, int count)
{
	int read = 0;

	if(sfmType == SSTSFM)
	{
		while(read < count)
		{
			read += SST25VF040B_read(sstSFMHandle,
			(address + read), (char *)(data + read),(count - read));
		}
	}
	else
	{
		while(AT25F4096IsBusy(atSFMHandle));
		AT25F4096Read(atSFMHandle,address,data,count);
		while(AT25F4096IsBusy(atSFMHandle));

	}

	return(count);
}

void __attribute__((__interrupt__,no_auto_psv)) _SPI1Interrupt(void)
{
	if(sfmType == SSTSFM)
	{
		SSTSFMISRoutine();
	}
	else
	{
		ATSFMISRoutine();
	}
}

