/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
* @file     xuartps_low_echo_example.c
*
* This file contains a design example using the hardware interface.
*
* First, certain character sequence is output onto the terminal. Then any
* characters typed in are echoed back, for letters, cases are switched.
* An 'ESC' character terminates the execution of the example.
*
* This example requires an external SchmartModule to be connected to the
* appropriate pins for the device through a daughter board. The test uses
* the default settings of the device:
*	. baud rate of 9600
*	. 8 bits data
* 	. 1 stop bit
* 	. no parity
*
* @note
* The test hangs if communication channel from the user terminal to the device
* is broken.
*
* MODIFICATION HISTORY:
* <pre>
* Ver   Who    Date     Changes
* ----- ------ -------- -----------------------------------------------------
* 1.00a drg/jz 01/13/10 First release
* 3.4   ms     01/23/17 Added xil_printf statement in main function to
*                       ensure that "Successfully ran" and "Failed" strings
*                       are available in all examples. This is a fix for
*                       CR-965028.
* </pre>
****************************************************************************/

/***************************** Include Files *******************************/

#include "xparameters.h"
#include "xstatus.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xuartps_hw.h"
#include "xil_printf.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "xuartps.h"

/************************** Constant Definitions ***************************/
#define UART_DEVICE_ID              XPAR_XUARTPS_0_DEVICE_ID



#define CHAR_ESC		0x1b	/* 'ESC' character is used as terminator */

#define A_Row 64
#define A_Col 8
#define B_Row 8
#define B_Col 1

int A[A_Row][A_Col];
int B[B_Row][B_Col];
int RES[A_Row][B_Col];

/**************************** Type Definitions *****************************/

typedef enum{
	IDLE,
	READA,
	READB,
	COMPUTE,
	WRITEBACK,
}states;



/************************** Function Prototypes ****************************/

int UART_init(u16 DeviceId);
void def_read(u8 Rowin,u8 Colin,u8 M);
void def_compute();
void def_write();

/************************** Variable Definitions ***************************/

XUartPs Uart_PS;		/* Instance of the UART Device */


/***************************************************************************/
/**
*
* Main function to call the Uart Echo example.
*
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful
*
* @note		None
*
****************************************************************************/
int main(void)
{

	states NextState = IDLE;

	switch(NextState){

		case(IDLE):{
			UART_init(UART_DEVICE_ID);
			NextState = READA;
		}

		case(READA):{
			def_read(A_Row,A_Col, 1);
			NextState = READB;
		}
		case(READB):{
			def_read(B_Row,B_Col,0);
			NextState = COMPUTE;
		}
		case(COMPUTE):{
			def_compute();
			NextState = WRITEBACK;
		}
		case(WRITEBACK):{
			//sleep(2000);
			def_write();
		}
	}
	}
	/*
	 * Run the Uart Echo example , specify the Base Address that is
	 * generated in xparameters.h
	 */


/**************************************************************************/
/**
*
* This function does a minimal test on the UART device using the hardware
* interface.
*
* @param	UartBaseAddress is the base address of the device
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful
*
* @note		None.
*
**************************************************************************/
int UART_init(u16 DeviceId){
	int Status;
	XUartPs_Config *Config;


	Config = XUartPs_LookupConfig(DeviceId);

	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Check hardware build. */
	Status = XUartPs_SelfTest(&Uart_PS);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XUartPs_SetOperMode(&Uart_PS, XUARTPS_OPER_MODE_NORMAL);
}
void def_read(u8 Rowin,u8 Colin,u8 M)
{
	u32 Running;
	u8 RecvChar[1];
	u8 Row = 0;
	u8 Col = 0;
	char currentChars[10]; //
	int currentCharIndex = 0;
	unsigned int ReceivedCount;

	Running = TRUE;
	while (Running &(Row<Rowin)) {
		 /* Wait until there is data */
		ReceivedCount = 0;
			while (ReceivedCount < 1) {
				ReceivedCount +=
					XUartPs_Recv(&Uart_PS, &RecvChar[ReceivedCount],
						      (1 - ReceivedCount));
			}

		/* Change the capitalization */
		if (RecvChar[0] != ',' && RecvChar[0] != '\n') {

		      if (currentCharIndex < sizeof(currentChars) - 1) {
		          currentChars[currentCharIndex++] = RecvChar[0];
		        }
		      } else {
		        	currentChars[currentCharIndex++] = '\0';

		            if (currentCharIndex > 0) {
		            	if (M){
		            		A[Row][Col] = atoi(currentChars);
		            	}
		            	else{
		            		B[Row][Col] = atoi(currentChars);
		            	}
		                currentCharIndex = 0;
		                Col++;
		                if (Col >= Colin) {
		                    Col = 0;
		                    Row++;
		                }
		            }
		            if (CHAR_ESC == RecvChar[0]) {
		                Running = FALSE;
		            }
		        }

	}
}

void def_compute(){


	for (int Colout = 0; Colout < B_Col;Colout ++){


		for (int Row = 0; Row < A_Row;Row++){
			u16 RES16 = 0;

			for (int Col = 0; Col < A_Col;Col++){

			RES16 += A[Row][Col] * B[Col][Colout];
			}
		RES[Row][Colout] = (u8)(RES16 >> 8);

		}

	}
}

void def_write(void){

	int Index;

	char asciiStr[10];
	  /* Send the entire transmit buffer. */
	    for (Index = 0; Index < A_Row; Index++) {

		     sprintf(asciiStr, "%d,", RES[Index][0]);
		     XUartPs_Send(&Uart_PS, (u8 *)asciiStr, strlen(asciiStr));
			XUartPs_Send(&Uart_PS, (u8 *)"\n", 1);
	        }
		
	  }



