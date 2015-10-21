
		Readme File for Code Example:
		MPLAB Starter Kit for dsPIC DSC
		Record and Play Demo with
            	using the Wolfson WM8510 Codec. 
             ----------------------------------------

This file contains the following sections:
1. Code Example Description
2. Folder Contents
3. Suggested Development Resources
4. Revision History


1. Code Example Description:
----------------------------
This demo will run on the MPLAB Starter Kit for dsPIC DSC. The demo records speech samples
on the serial flash and reproduces them when requested. The application uses the Wolfson WM8510
in 16 bit mode for capture and playback of audio signals.
Refer to the MPLAB Starter Kit for dsPIC DSC User Guide for more details on starter kit. 

Follow these steps prior to running the demo. On the starter kit Board

1. Set up jumper J6 to CODEC.
2. Set up jumper J7 to MIC.
3. Plug in microphone into socket J9 (LINE IN/MIC).
4. Plug in headphone into socket J8 (Speaker Out).
6. Rotate potentiometer R56 (MIC GAIN) fully clockwise.

Open the MPLAB workspace, compile the code, program the dsPIC and excute.

1. A introductory message plays back repeatedly on the headphone. 
2. To Record, press switch S1 on the starter bit board. The RED LED will turn ON
indicating that the serial flash is being erased. After a couple of seconds the 
YELLOW LED will turn on. This means that application is now ready to
record speech. 
3. Press switch S2 to listen to what was recorded. The GREEN LED will turn on. The
recorded speech will be played back in a loop.
4. Press S1 to record again. The previous recording will be erased.

If the recording exceeds the limitation of the memory, then the application will
stop recording and switch to playback. When S1 is pressed again, the previous
recording will be erased and the application will start a fresh recording.

Adjust potentiometer R56 to adjust the microphone gain and recording volume. 

2. Folder Contents:
-------------------
This project folder contains the following sub-folders:

a. h
        This folder contains include files for the code example.


b. src
        This folder contains all the C and Assembler source files (*.c,
        *.s) used in demonstrating the described example. 

3. Required Development Resources:
-----------------------------------
        a. MPLAB Starter Kit for dsPIC DSC.


4. Revision History :
---------------------
        11/20/2008 - Initial Release of the Code Example
