

#ifndef _SFMDRV_H_
#define _SFMDRV_H_

#include "..\h\AT25F4096Drv.h"
#include "..\h\SST25VF040BDrv.h"

#define ATSFM 0
#define SSTSFM 1
#define SFM_LAST_ADDRESS 0x7FFFF

#if(AT25F4096DRV_BUFFER_SIZE >  SST25VF040BDRV_BUFFER_SIZE)
	#define SFMDRV_BUFFER_SIZE AT25F4096DRV_BUFFER_SIZE
#else
	#define SFMDRV_BUFFER_SIZE SST25VF040BDRV_BUFFER_SIZE
#endif

/***********************************************
 * Function - SFMInit
 * Arguments: char type array to be used by the 
 * driver for internal buffer.
 * Description: This function will intialize the 
 * SFM driver structure and the SPI module. It will
 * detect the SFM being used and will internally 
 * register the SFM type being used on the board
 * Return Value - None
 * *********************************************/

void SFMInit(char * flashMemoryBuffer);


/***********************************************
 * Function - SFMChipErase
 * Arguments: None
 * Description: This function will erase the SFM.
 * The function is blocking and will return when 
 * chip erase is complete
 * Return Value - None
 * *********************************************/

void SFMChipErase(void);


/***********************************************
 * Function - SFMBlockErase
 * Arguments: 
 * address - address of block to delete
 * Description: This function will delete a 64K 
 * SFM block.
 * Return Value - none.
 * *********************************************/

void SFMBlockErase(long address);


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

int SFMWrite(long address, char * data, int count);


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

int SFMRead(long address, char * data, int count);

#endif
