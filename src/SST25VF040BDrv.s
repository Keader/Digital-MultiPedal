;/**********************************************************************
;* © 2007 Microchip Technology Inc.
;*
;* FileName:        SST25VF040BDrv.s
;* Dependencies:    Header (.h) files if applicable, see below
;* Processor:       dsPIC33Fxxxx
;* Compiler:        MPLAB® C30 v3.00 or higher
;*
;* SOFTWARE LICENSE AGREEMENT:
;* Microchip Technology Incorporated ("Microchip") retains all ownership and 
;* intellectual property rights in the code accompanying this message and in all 
;* derivatives hereto.  You may use this code, and any derivatives created by 
;* any person or entity by or on your behalf, exclusively with Microchip's
;* proprietary products.  Your acceptance and/or use of this code constitutes 
;* agreement to the terms and conditions of this notice.
;*
;* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
;* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
;* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
;* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
;* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
;*
;* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
;* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
;* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
;* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
;* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
;* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
;* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
;* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
;* HAVE THIS CODE DEVELOPED.
;*
;* You agree that you are solely responsible for testing the code and 
;* determining its suitability.  Microchip has no obligation to modify, test, 
;* certify, or support the code.
;************************************************************************/
	
	; SFM - Serial Flash Memory
	; DS - Driver Structure

	; SPI1_PRIMARY_PRESCALE - SPI Primary Prescale value
	; SPI1_SECONDARY_PRESCALE - SPI Secondary Prescale 
	; SST25VF040BDRV_BUFFER_SIZE - Allocated write buffer size. Should
	; match header file.
	
	
	.equ SPI1_PRIMARY_PRESCALE,		0x2
	.equ SPI1_SECONDARY_PRESCALE,	0x6
	.equ SST25VF040BDRV_BUFFER_SIZE, 256
	
	; Define the TRIS, LAT and in this case the
	; ADPCFG bits which control the chip select
	; pin of the SFM. The TBP Period is the 
	; program time required for byte program.
	
 	.equ SFM_CHIP_SELECT_TRIS,	TRISB 
 	.equ SFM_CS, LATB 	
 	.equ SFM_CHIP_SELECT_ANPORT,AD1PCFGL
 	.equ SFM_CS_BIT, 2
 	.equ SFM_TBP_PERIOD,0x200
 	
;**********************************************
; Do not change anything below this line 
;**********************************************
 	
 	.macro enable_sfm
 		bclr SFM_CS,#SFM_CS_BIT
 	.endm
 	.macro disable_sfm
 		bset SFM_CS,#SFM_CS_BIT
 	.endm
 	.macro enable_spi_int
 		bset IEC0, #10
 	.endm
 	.macro disable_spi_int
 		bclr IEC0, #10
 	.endm
 	.macro clear_spi_int
 		bclr IFS0, #10
 	.endm
 	.macro wait_spi_tx
 		btss SPI1STAT,#0 	; Wait till data is sent. Check the 
		bra $-2				; SPI1RBF flag for this.
 	.endm

	; Status Word bit
	
	.equ SFMDRV_WRITING, 0x3
		
	.equ SFM_CHIP_ERASE,		0x1
	.equ SFM_READ_FLASH_MFG_ID, 0x2
	.equ SFM_READ_STATUS, 		0x3
	.equ SFM_WRITE_ENABLE,		0x4
	.equ SFM_WR_STATUS,			0x5
	.equ SFM_BLOCK_ERASE,		0x9
	
	; Offset into the flash memory driver structure (DS)

	.equ BUFFERPTR, 0		; Pointer to the driver write buffer (2 bytes)
	.equ STATUSFLAG, 2		; Tracks the current state of the driver (2 bytes)
	.equ DATACOUNT, 4		; Total words to write or read (2 bytes)
	.equ DATAINDEX, 6		; Index into the user buffer (2 bytes)
	.equ WRITEADDRESSLO,8	; Serial flash memory address (4 bytes)
	.equ WRITEADDRESSHI,10
	
	
	.bss
thisFlashMemory: .space 2


	.text
	
	.global _SST25VF040B_open
	.global _SST25VF040B_write
	.global _SST25VF040B_read
	.global _SST25VF040B_ioctl
	.global _SSTSFMISRoutine
	.global __T4Interrupt
	

;***************************************************************
; Function SST25VF040B_open
; Desctiption: This function initialises the driver 
; Inputs:
; W0 - Pointer to the uninitiliazed driver structure (DS)
; W1 - Pointer to the user allocated buffer which will be used 
;      for buffering write data.
; Uses - W2
; Returns:
; Function will return the handle to the initialized buffer.
;***************************************************************  
_SST25VF040B_open:
	

	mov w0, thisFlashMemory	; Make a local copy of the DS								
	mov w1, [w0+BUFFERPTR]	; Assign the buffer to the DS 
	clr w2
	mov w2, [w0+STATUSFLAG] ; members.
	mov w2, [w0+DATACOUNT] ;
	mov w2, [w0+DATAINDEX] ;
	

	; SPI setup. Use Internal clock
	; Byte wide communication. Input sampled at 
	; end of data time. Data changes from clock
	; high to clock low. Master mode enabled.
	; Refer to SPI FRM for more details. 
	; Framed mode disabled. Operate in idle mode.

	mov	#0x0320, w2			
	ior #(SPI1_SECONDARY_PRESCALE * 4), w2	; Mul by 4 to shift left 2 bits
	ior #SPI1_PRIMARY_PRESCALE, w2
	mov w2, SPI1CON1 
	bclr SPI1CON2, #15
	bclr SPI1STAT, #13	
	
	bclr IFS0, #10 ; Clear SPI1 Interrupt Flag
	bset IEC0, #10 ; Enable the SPI1 Interrupt
	
	bset SPI1STAT, #15	; Enable the SPI1 module.
	
	; Initialize the SFM chip select line
	; Start with chip select high i.e SFM disabled
	
	bset SFM_CHIP_SELECT_ANPORT, #SFM_CS_BIT
	bclr SFM_CHIP_SELECT_TRIS, #SFM_CS_BIT
	disable_sfm
	
	; Timer T4 is used for timing the program duration
	
	clr	T4CON
	clr TMR4
	mov	#SFM_TBP_PERIOD, w2 ;Load period register with count
	mov w2, PR4
	bclr IFS1, #11			; Clear Timer 4 interrupt flag
	bset IEC1, #11  		; Enable Timer 4 interrupt
	
	mov thisFlashMemory,w0	; Return pointer to initialized DS
	
	return

;***************************************************************	
; Function SST25VF040B_write
; Desctiption: This function writes data to the the SFM
; Inputs:
; W0 - Pointer to the DS
; W1 - Pointer to byte array containing data to be written 
; W2:W3 - SFM address     
; W4 - number of bytes to write
; Uses: W5, W6, W7
; Returns:
; Function will return number of bytes written in W0
;***************************************************************

_SST25VF040B_write:
	
	mov	[w0+STATUSFLAG],w6		; Check if the driver is already writing
	btss w6, #SFMDRV_WRITING	; by checking writing bit in status flag
	bra not_wr_busy			
	clr w0						; Set the number of written bytes to zero
	return						; and exit.

not_wr_busy:
	
	cp0 w4					; If bytes to write is zero or negative
	bra GT, wr_gt_zero   	; then exit with return value 0.
	clr w0
	return

wr_gt_zero:
	
	mov	#SST25VF040BDRV_BUFFER_SIZE,W5	; If bytes to write is greater
	cpslt w4,w5							; than size of buffer, then truncate
	mov #SST25VF040BDRV_BUFFER_SIZE,W4  ; to size of buffer.
	
	clr w5
	mov w5, [w0+DATAINDEX]	; Reset the data index
	mov w4, [W0+DATACOUNT]	; Store the count in the DS
;	mov	w4,	w7				; Back up count in W7
	mov [w0+BUFFERPTR],W5	; Get address of SFM DS write buffer
	clr w7					; Use W7 for counting
copy_wr_data:

	mov.b [w1+w7],[w5+w7]	; W4 has number of bytes
	inc	w7,w7				; Increment index
	dec w4, w4				; W5 has the SFM DS buffer address
	cp0	w4					; W1 points to data to write.
	bra gt,copy_wr_data		; 
	
	bset w6, #SFMDRV_WRITING  	; Set the writing bit in status flag (in w6)
	mov	#0xFFF8,w4				; Clear the state counter in the 	
	and w6, w4, w6				; last three bits of the status flag
	mov w6, [w0+STATUSFLAG]   	; update the status flag.
	
	mov	w2,[w0+WRITEADDRESSLO]	; Update the destination address in the
	mov	w3,[w0+WRITEADDRESSHI]  ; SFM DS
	
	; Enable the SFM and output byte program
	; code to the SPI
	enable_sfm
	mov #0x6,w4					; 0x6 is the Write Enable Code
	mov	w4, SPI1BUF
	
	mov w7, w0 ; Return the number of bytes written.
	return

;*************************************************************
; Function SST25VF040B_read
; Desctiption: This function reads data from the the SFM
; Inputs:
; W0 - Pointer to the DS
; W1 - Pointer to destination buffer
; W2:W3 - SFM address     
; W4 - number of bytes to read
; Uses: W5, W6, W7
; Returns:
; Function will return number of bytes read in W0
;*************************************************************

_SST25VF040B_read:
	
	; The driver cannot read while data is being written
	; If the driver is busy with a write then the function
	; will return with a zero.
	
	mov	[w0+STATUSFLAG],w6		; Check if the driver is already writing
	btss w6, #SFMDRV_WRITING	; by checking writing bit in status flag
	bra not_busy		
	clr w0						; Set the number of written bytes to zero
	return						; and exit.

not_busy:
	
	cp0 w4					; If bytes to read is zero or negative
	bra GT, rd_gt_zero   	; then exit with return value 0.
	clr w0
	return
	
rd_gt_zero:

	; The read operation is blocking and
	; does not use interrupts.
	
	disable_spi_int
	mov	SPI1BUF, w5	; Read SPI1BUF to clear SPI1RBF
	
	enable_sfm			 ; Enable SFM
	
	mov	#0x3, w5		; Load the read command - 0x3
	mov w5, SPI1BUF		; and send to SFM
	wait_spi_tx
	mov	SPI1BUF, w5		; Read SPI1BUF to clear SPI1RBF
	
	mov w3,SPI1BUF		; Send address A23-A16
	wait_spi_tx
	mov	SPI1BUF, w5		; Read SPI1BUF to clear SPI1RBF
	
	swap w2
	mov w2, SPI1BUF		; Send address A15-A8	
	wait_spi_tx
	mov	SPI1BUF, w5		; Read SPI1BUF to clear SPI1RBF
	
	swap w2
	mov w2, SPI1BUF		; Send address A7-A0
	wait_spi_tx
	mov	SPI1BUF, w5		; Read SPI1BUF to clear SPI1RBF
	
	clr w6				; Use w6 to count.
	clr w5				; Use w5 to write 0 to SPI1BUF
read_more:
	mov	w5, SPI1BUF
	wait_spi_tx
	mov SPI1BUF,w7		; Read the data and store in the
	mov.b w7, [w1+w6]	; user specified location.
	inc w6,w6			; W1 points to user array
	cpseq w6, w4		; W4 contains number of bytes to read
	bra read_more		; W6 counts the bytes read
	
	disable_sfm
	clear_spi_int
	enable_spi_int
	
	mov	w4,w0			; Return the number of bytes read.
	return

;**************************************************************	
; Function SST25VF040B_ioctl
; Desctiption: This function performs control operations
; Inputs:
; W0 - Pointer to the DS
; W1 - Command Value
; W2 - void pointer value of which depends on command    
; Uses: W3, W4, W5, W6, W7
; Returns:
; Will return 1 if command executed successfully, 0 otherwise.
;***************************************************************

_SST25VF040B_ioctl:
	
	; The driver cannot perform any other function
	; while a write is being peformed. Check if the
	; driver is performing write and if so exit.
	
	mov	[w0+STATUSFLAG],w6		; Check if the driver is already writing
	btss w6, #SFMDRV_WRITING	; by checking writing bit in status flag
	bra cmd0		
	clr w0						; Return 0 and exit
	return					

cmd0:

	mov	#SFM_READ_FLASH_MFG_ID,w3	; Check the command
	cpseq w3,w1						; If it's not read flash id	
	bra	cmd1 						; then jump to next command check
	
	; Procedure for reading the SFM ID - Start
	; Send the read chip id code
	; Send three zeroes
	; Read four bytes

	disable_spi_int					;Disable SPI Interrupts	
	mov SPI1BUF,w4					; Read to clear the SPIRBF flag
	mov	#0x90,w3					; 0x90 is the Chip ID Read Command
	enable_sfm
	mov	w3, SPI1BUF
	btss SPI1STAT,#0 				; Wait till data is sent. Check the 
	bra $-2							; SPI1RBF flag for this.
	mov	SPI1BUF,w4
	
	clr	w3
	mov #3, w5						; Three 0's need to be sent
s1:
	mov	w3, SPI1BUF					; Send 0 to read the SFM
	btss SPI1STAT,#0 				; Wait till data is sent. Check the 
	bra $-2							; SPI1RBF flag for this.
	mov	SPI1BUF,w4					; clear SPIRBF flag
	dec w5,w5
	cp0 w5
	bra	gt,s1
	
	mov #4, w5						; Four bytes to read
s2:
	mov	w3, SPI1BUF					; Send 0 to read the SFM
	btss SPI1STAT,#0 				; Wait till data is sent. Check the 
	bra $-2							; SPI1RBF flag for this.
	mov	SPI1BUF,w4					; clear SPIRBF flag
	mov.b	w4, [w2++]				; Store read data at user specified loc.
	dec w5,w5
	cp0 w5
	bra	gt,s2

	disable_sfm
	clear_spi_int
	enable_spi_int
	mov	#1,w0						;return 1 
	return
		
cmd1:
	
	mov	#SFM_CHIP_ERASE,w3	; Check the command
	cpseq w3,w1				; If it's not chip erase
	bra	cmd2 				; then jump to next command check
	
	; Chip erase sequence
	; Send write enable command
	; Send chip erase code
	; read status register to see if
	; chip erase is complete
	
	disable_spi_int ; disable interrupt
	mov SPI1BUF, w4	; clear SPIRBF
	mov	#0x6, w3	; Write enable command
	enable_sfm
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	disable_sfm		; disable sfm
	mov SPI1BUF, w4 ; clear SPIRBF

	mov	#0x60,w3	; 0x60 is the chip erase code
	nop				; nop are needed to ensure
	nop				; minimum chip sel high time.
	enable_sfm
	mov w3,SPI1BUF	; Write to SFM
	wait_spi_tx		; macro
	disable_sfm		; disable sfm for command to work
	mov	SPI1BUF,w4	; clear SPIRBF
	
	enable_sfm		; enable sfm
	mov	#0x5, w3	; read status command code
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait till tx done
	mov SPI1BUF,w4	; clear SPI1RBF
	clr	w3			; write zero to sfm
s3:
	mov	w3, SPI1BUF	; to read data
	wait_spi_tx
	mov	SPI1BUF,w4 	; w4 has the status value
	btsc w4,#0 		; if bit 0 is 1 then write is
	bra s3			; in progress. keep reading 

	disable_sfm		; erase is complete
	clear_spi_int	; clear interrupt flag
	enable_spi_int	; enable interrupt
	mov	#1, w0		; return 1
	return
	
cmd2:

	mov	#SFM_READ_STATUS,w3	; Check the command
	cpseq w3,w1				; If it's not read status	
	bra	cmd3 				; then jump to next command check

	; Procedure to read status
	; output read status command
	; read the response from sfm
	
	disable_spi_int	; disable SPI Interrupts
	enable_sfm		; enable sfm
	mov	SPI1BUF,w4	; clear SPIRBF flag
	mov #0x5, w3	; Read status command code
	mov w3,SPI1BUF	; Write to sfm
	wait_spi_tx		; wait for tx
	mov SPI1BUF,w4	; clear SPIRBF flag
	clr	w3			; output zero to read SFM
	mov w3,SPI1BUF
	wait_spi_tx
	mov	SPI1BUF,w4	; This is the status word
	mov.b w4, [w2]	; Store status word
	disable_sfm		; disable sfm
	clear_spi_int	; clear spi interrupt
	enable_spi_int	; enable spi interrupt
	mov	#1,w0		; return 1
	return
	
	
cmd3:	

	mov	#SFM_WRITE_ENABLE,w3	; Check the command
	cpseq w3,w1				; If it's not write enable
	bra	cmd4 				; then jump to next command check

	; Procedure to write enable device
	; send write enable command
	; 
	
	disable_spi_int	; disable interrupts
	mov SPI1BUF, w4	; clear SPIRBF
	mov	#0x6, w3	; Write enable command
	enable_sfm
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	disable_sfm		; disable sfm
	mov SPI1BUF, w4 ; clear SPIRBF
	clear_spi_int	; clear SPI interrupts flags
	enable_spi_int	; enable SPI interrupts.
	mov	#1,w0
	return
		
cmd4:

	mov	#SFM_WR_STATUS,w3	; Check the command
	cpseq w3,w1				; If it's not write status	
	bra	cmd5 				; then jump to next command check	
	
	; Procedure to write to status register
	; First send command to enable write to 
	; status register. Then send the the 
	; send code for write to status register
	; and then write the status word.
	
	disable_spi_int	; disable spi interrupts
	mov	SPI1BUF,w4	; clear SPIRBF
	mov	#0x50, w3	; Write status enable command
	enable_sfm
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	disable_sfm		; disable sfm
	mov	SPI1BUF,w4	; clear SPIRBF
	
	mov	#0x1, w3	; Write status command
	enable_sfm
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	mov SPI1BUF,w4	; clear SPIRBF
	mov [w2],w3		; Send the status word
	mov	w3,SPI1BUF	; to the SFM
	wait_spi_tx
	disable_sfm
	mov	SPI1BUF,w4	; clear SPIRBF
	
	clear_spi_int	; clear interrupt
	enable_spi_int	; enable interrupt
	mov #1,w0		; return 1
	return

cmd5:
	mov	#SFM_BLOCK_ERASE,w3	; Check the command
	cpseq w3,w1				; If it's not block erase	
	bra	cmd6 				; then jump to next command check

	; Procedure to erase a block
	; Send write enable command
	; execute command
	; send block erase command
	; send address A23-A16
	; send address A15-A8
	; send addess  A7-A0
	; execute command
	; read status to see if erase is complete
	
	disable_spi_int ; disable interrupt
	mov SPI1BUF, w4	; clear SPIRBF
	enable_sfm
	mov	#0x6, w3	; Write enable command
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	disable_sfm		; disable sfm
	mov SPI1BUF, w4 ; clear SPIRBF
	
	
	mov	#0xD8, w3	; Block erase command
	nop
	nop
	enable_sfm
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	mov SPI1BUF, w4 ; clear SPIRBF
	
	mov [w2+2],w3	; Address A23-A16
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	mov SPI1BUF, w4 ; clear SPIRBF
	
	mov [w2],w3		; Address A15-A8
	swap w3			
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	mov SPI1BUF, w4 ; clear SPIRBF
	
	swap w3			
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait for spi tx
	mov SPI1BUF, w4 ; clear SPIRBF
	
	disable_sfm		; Execute the erase
	nop				; nop instructions
	nop				; needed between disable
	nop				; and enable sfm instructions
	mov	#0x5, w3	; read status command code
	enable_sfm		; enable sfm
	mov	w3,SPI1BUF	; write to sfm
	wait_spi_tx		; wait till tx done
	mov SPI1BUF,w4	; clear SPI1RBF
	clr	w3			; write zero to sfm
cmd5_s3:
	mov	w3, SPI1BUF	; to read data
	wait_spi_tx
	mov	SPI1BUF,w4 	; w4 has the status value
	btsc w4,#0 		; if bit 0 is 1 then write is
	bra cmd5_s3		; in progress. keep reading 
	
	disable_sfm
	clear_spi_int
	enable_spi_int
	mov #1,w0
	return

cmd6:
	clr w0			; Invalid Command
	return			; return 0

;*********************************************
;SPI1 Interrupt Service Routine
;*********************************************

_SSTSFMISRoutine:
	push w0
	push w1
	push w2
	push w3
	push w4

	clear_spi_int			; clear interrupt flag
	mov	SPI1BUF, w4			; clear SPIRBF
	mov	thisFlashMemory,w0	; get the driver handle
	mov	[w0+STATUSFLAG],w1	; get status flag in w1
	and w1,#7,w2			; get the state counter
	
	cp0	w2			;Check for State 0
	bra gt,state1
	
	;**********************************
	; State 0 - Send the Byte Program Code
	; Increment the state count. Note that
	; before this byte program code has bee
	; sent.
	;**********************************
	disable_sfm					; disable so that WREN command
								; is accepted
	inc w1,w1					; Increment state count		
	mov	w1,[w0+STATUSFLAG]		; Update Status Flag
	mov	#0x2,w3 				; Send Byte Program Code
	enable_sfm
	mov w3,SPI1BUF
	bra isr_exit	

state1:		
	cp	w2,#1			;Check for State 1
	bra gt,state2
	
	;**********************************
	;State 1 - Send the address A23-A16 
	;Increment the state count
	;**********************************

	inc w1,w1					; Increment state count		
	mov	w1,[w0+STATUSFLAG]		; Update Status Flag
	mov	[w0+WRITEADDRESSHI],w3 	; Get address A23-16
	
	mov w3,SPI1BUF
	bra isr_exit	
	
state2:
	cp w2,#2					; Is it state 1
	bra gt,state3	
	
	;**********************************
	;State 2 - Send the address A15-A8 
	;Increment the state count
	;**********************************
	
	inc w1,w1					; Increment state count		
	mov	w1,[w0+STATUSFLAG]		; Update Status Flag
	mov	[w0+WRITEADDRESSLO],w3 	; Get address A15-A8
	swap w3
	mov w3,SPI1BUF
	bra isr_exit

state3:
	cp	w2,#3					; Is it state 3
	bra gt,state4	

	;**********************************
	;State 3 - Send the address A7-A0 
	;Increment the state count
	;**********************************
	
	inc w1,w1					; Increment state count		
	mov	w1,[w0+STATUSFLAG]		; Update Status Flag
	mov	[w0+WRITEADDRESSLO],w3 	; Get address A7-A0
	mov w3,SPI1BUF				; Send to SFM
	bra isr_exit

	
state4:
	cp	w2,#4				; Is it state 4
	bra gt,state5
	
	;**********************************
	;State 4 - Send the Data 
	;Increment the state count
	;**********************************
		
	inc w1,w1				; Increment state count		
	mov	w1,[w0+STATUSFLAG]	; Update Status Flag
	mov	[w0+BUFFERPTR],w3 	; Get pointer to buffer
	mov [w0+DATAINDEX],w4	; Get the current index
	mov.b [w3+w4],w3		; Get one byte
	mov [w0+DATACOUNT],w4	; Decrement count
	dec w4,w4
	mov	w4,[w0+DATACOUNT]
	mov w3,SPI1BUF			; Send data to SFM
	bra isr_exit

state5:

	;**************************************
	; State 5 - Last state. Disable SFM
	; Enable program timer
	;**************************************
	
	disable_SFM
	bset T4CON,#15

isr_exit:
	
	pop w4	
	pop w3
	pop w2
	pop w1
	pop w0
	return
	
	;*************************************
	; Timer 4 - Timer 4 Interrupt Service Routine
	;*************************************
	
__T4Interrupt:
	push w0
	push w1
	push w2
	push w3

	bclr IFS1,#11	; Clear interrupt flag
	bclr T4CON,#15	; Disable the timer
	clr TMR4		; Clear the timer
	mov	thisFlashMemory,w0
	mov [w0+DATACOUNT], w1
	cp0 w1
	bra le,t4_1
	
	; More data to be sent
	; Increment the address
	; Increment data index
	; reset state counter
	
	mov	[W0+DATAINDEX], w1		; Increment the data Index
	inc w1,w1
	mov w1, [W0+DATAINDEX]
	
	mov	[w0+WRITEADDRESSHI],w3	; Increment Address
	mov [w0+WRITEADDRESSLO],w2
	add w2,#1,w2				; Increment address A15-A0
	addc w3,#0,w3				; Add carry to Address A23-A16
	mov	#0x8,w1					; Is A23 greater than 0x08
	cpslt w3,w1				
	clr w3						; Resey address to all zeroes
	mov	w3,	[W0+WRITEADDRESSHI]	; Increment Address
	mov w2, [W0+WRITEADDRESSLO]
	mov [w0+STATUSFLAG], w1		; Reset the state Counter
	mov	#0xFFF8,w2
	and w2,w1,w1
	mov	w1,[W0+STATUSFLAG]		; Update status flag
	enable_sfm
	mov	#0x6, w3				; write enable for the next byte
	mov	w3,SPI1BUF
	bra t4_exit

t4_1: 
	mov [W0+STATUSFLAG], w1		; Clear the writing flag	
	bclr w1,#SFMDRV_WRITING		; and update the status flag
	mov	w1,[W0+STATUSFLAG]		;
t4_exit:	
	pop w3
	pop w2
	pop w1
	pop w0
	retfie
		
	.end
	
