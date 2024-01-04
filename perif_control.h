/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PERIF_CONTROL_H
#define	PERIF_CONTROL_H

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/io.h>

#define T_output 4
#define R_TP63 0
#define G_TP12 2
#define B_TP64 3
#define IR_OUT 1



#define WAKE_UP_INT PCINT2
#define R_TP63_INT PCINT0
#define B_TP64_INT PCINT3
//#define G_TP12 PCINT2


void setClock();
void setGPIO();
void setPowerSavings();
void setTimer0();
void setTimer1();
void armWakeUpInt();

void enable_PCINT();
void disable_PCINT();

void startTimeout();
void resetTimer0();
void disable_Timer0Int();
void disable_Timer1Int();
void enable_Timer1Int();

// D I/O Samples
extern uint8_t sample_R_TP63;
extern uint8_t sample_G_TP12;
extern uint8_t sample_B_TP64;

void setIRLow();
void setIRHigh();

#endif	/* PERIF_CONTROL_H */

