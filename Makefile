# MPLAB IDE generated this makefile for use with GNU make.
# Project: SASK WM8510 Record Play Demo with Intro.mcp
# Date: Fri Feb 27 23:24:36 2015

AS = pic30-as.exe
CC = pic30-gcc.exe
LD = pic30-ld.exe
AR = pic30-ar.exe
HX = pic30-bin2hex.exe
RM = rm

SASK\ WM8510\ Record\ Play\ Demo\ with\ Intro.hex : SASK\ WM8510\ Record\ Play\ Demo\ with\ Intro.cof
	$(HX) "SASK WM8510 Record Play Demo with Intro.cof"

SASK\ WM8510\ Record\ Play\ Demo\ with\ Intro.cof : main.o sask.o G711.o WM8510CodecDrv.o SST25VF040BDrv.o SFMDrv.o AT25F4096Drv.o
	$(CC) -mcpu=33FJ256GP506 "main.o" "sask.o" "G711.o" "WM8510CodecDrv.o" "SST25VF040BDrv.o" "SFMDrv.o" "AT25F4096Drv.o" -o"SASK WM8510 Record Play Demo with Intro.cof" -Wl,-Tp33FJ256GP506.gld,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_SKDE=1,--defsym=__ICD2RAM=1,-Map="SASK WM8510 Record Play Demo with Intro.map",--report-mem

main.o : ../../mplab\ c30/include/math.h ../../mplab\ c30/include/stdlib.h ../../mplab\ c30/include/yvals.h ../../mplab\ c30/include/stdio.h h/G711.h h/p33FJ256GP506.h h/SST25VF040BDrv.h h/p33FJ256GP506.h h/AT25F4096Drv.h h/SFMDrv.h h/p33FJ256GP506.h h/sask.h h/p33FJ256GP506.h h/WM8510CodecDrv.h h/p33FJ256GP506.h src/main.c
	$(CC) -mcpu=33FJ256GP506 -x c -c "src\main.c" -o"main.o" -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 -g -Wall

sask.o : h/p33FJ256GP506.h h/sask.h src/sask.c
	$(CC) -mcpu=33FJ256GP506 -x c -c "src\sask.c" -o"sask.o" -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 -g -Wall

G711.o : src/G711.s
	$(CC) -mcpu=33FJ256GP506 -c -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 "src\G711.s" -o"G711.o" -Wa,-I"..\h",--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SKDE=1,-g

WM8510CodecDrv.o : h/p33FJ256GP506.h h/WM8510CodecDrv.h src/WM8510CodecDrv.c
	$(CC) -mcpu=33FJ256GP506 -x c -c "src\WM8510CodecDrv.c" -o"WM8510CodecDrv.o" -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 -g -Wall

SST25VF040BDrv.o : src/SST25VF040BDrv.s
	$(CC) -mcpu=33FJ256GP506 -c -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 "src\SST25VF040BDrv.s" -o"SST25VF040BDrv.o" -Wa,-I"..\h",--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SKDE=1,-g

SFMDrv.o : h/p33FJ256GP506.h h/SST25VF040BDrv.h h/p33FJ256GP506.h h/AT25F4096Drv.h h/SFMDrv.h src/SFMDrv.c
	$(CC) -mcpu=33FJ256GP506 -x c -c "src\SFMDrv.c" -o"SFMDrv.o" -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 -g -Wall

AT25F4096Drv.o : h/p33FJ256GP506.h h/AT25F4096Drv.h src/AT25F4096Drv.c
	$(CC) -mcpu=33FJ256GP506 -x c -c "src\AT25F4096Drv.c" -o"AT25F4096Drv.o" -D__DEBUG -D__MPLAB_DEBUGGER_SKDE=1 -g -Wall

clean : 
	$(RM) "main.o" "sask.o" "G711.o" "WM8510CodecDrv.o" "SST25VF040BDrv.o" "SFMDrv.o" "AT25F4096Drv.o" "SASK WM8510 Record Play Demo with Intro.cof" "SASK WM8510 Record Play Demo with Intro.hex"

