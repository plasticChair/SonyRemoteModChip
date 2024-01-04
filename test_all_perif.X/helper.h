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

void sendCMD();
void setModeStop();
void setModeRunning();
void initIRMode();
void setRunningFlag();
void setExecuteFlag();

uint8_t checkRunningMode();
uint8_t getRunningMode();
uint8_t getExecuteFlag();
uint8_t sendingSM();

uint8_t checkVolUp();
uint8_t checkVolDown();
uint8_t checkHighs();

extern void startTimer1();
extern void stopTimer1();
extern void setIRLow();
extern void setIRHigh();

extern void sampleGPIO();
extern void testPinOut(int state);


extern void enable_Timer1Int();
#endif	/* HELPER_H */

