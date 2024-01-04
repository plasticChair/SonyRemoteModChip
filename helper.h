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
#ifndef HELPER_H
#define	HELPER_H

#include <stdint.h>



#define T_output 4
#define R_TP63 0
#define G_TP12 2
#define B_TP64 3


// PWM List                
typedef enum {
            WAIT_IR,
            ASSIGN,
            NEXT_BYTE,
            NEXT_BIT,
            DATA_OUT,
            LOGIC_LOW,
            LOGIC_HIGH,
}IR_MODE_LIST;










uint8_t checkVolUp();
uint8_t checkVolDown();
uint8_t checkHighs();
extern void sampleGPIO();
extern void testPinOut(int state);

void sendCMD();
uint8_t sendingSM();

void setModeStop();
void setModeRunning();
uint8_t getRunningMode();
void initIRMode();

extern void startTimer1();
extern void stopTimer1();
extern void setIRLow();
extern void setIRHigh();

extern void enable_Timer1Int();
#endif	/* HELPER_H */

