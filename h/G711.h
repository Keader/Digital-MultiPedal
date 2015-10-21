/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        G711.h
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33FJ256GP506
* Compiler:        MPLAB® C30 v3.00 or higher
*
*SOFTWARE LICENSE AGREEMENT:
*Except as otherwise expressly provided below, Microchip Technology Inc. 
*(Microchip) licenses this software to you solely for use with Microchip products.
*This software is owned by Microchip and/or its licensors, and is protected under 
*applicable copyright laws.  All rights reserved.
*
*This software and any accompanying information is for suggestion only.  
*It shall not be deemed to modify Microchip’s standard warranty for its products.
*It is your responsibility to ensure that this software meets your requirements.
*
*WARRANTY DISCLAIMER AND LIMIT ON LIABILITY:  SOFTWARE IS PROVIDED AS IS.  
*MICROCHIP AND ITS LICENSORS EXPRESSLY DISCLAIM ANY WARRANTY OF ANY KIND, 
*WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
*OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. 
*IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE FOR ANY INCIDENTAL, 
*SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, 
*HARM TO YOUR EQUIPMENT, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY 
*OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY 
*DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR 
*COSTS.  To the fullest extend allowed by law, Microchip and its licensors 
*liability shall not exceed the amount of fees, if any, that you have paid 
*directly to Microchip to use this software.
*
*THIRD PARTY SOFTWARE:  Notwithstanding anything to the contrary, any third 
*party software accompanying this software including but not limited to ITU 
*software is subject to the terms and conditions of the third party’s license
*agreement such as the General Public License.  To the extent required by third
*party licenses covering such third party software, the terms of such license 
*will apply in lieu of the terms provided herein.  To the extent the terms of 
*such third party licenses prohibit any of the restrictions described herein, 
*such restrictions will not apply to such third party software.  THIRD PARTY 
*SOFTWARE  IS SUBJECT TO THE FOREGOING WARRANTY DISCLAIMER AND LIMIT ON LIABILITY 
*PROVIDED IN THE PARAGRAPH ABOVE
*
*MICROCHIP PROVIDES THIS SOFTWARE (INCLUDING ACCOMPANYING THIRD PARTY SOFTWARE)
*CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.
************************************************************************/

#ifndef	__G711_H__
#define	__G711_H_

#define ZEROTRAP    /* turn on the trap as per the MIL-STD */
#define BIAS 0x84   /* define the add-in bias for 16 bit samples */
#define CLIP 32635


extern void G711Lin2Ulaw (int *   source, char * destination, int size);
extern void G711Ulaw2Lin(char* source, int *    destination, int size);
extern void G711Lin2Alaw (int *   source, char * destination, int size);
extern void G711Alaw2Lin(char* source, int *    destination, int size);

#endif
