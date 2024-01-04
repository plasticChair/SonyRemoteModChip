/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MAIN_H
#define	MAIN_H
#include "helper.h"
#include "perif_control.h"

//Main State List
typedef enum  { ARM,
                WAIT,
                TIMER_WAKE,
                TRIGGERED_WAKE,
                WAIT_COND,
                SENDING,
                RESET
                } MODE_LIST;

// ------ Volatiles ------
//  State Machine
volatile uint8_t timerWakeCnt = 0;
volatile MODE_LIST PREV_MODE = RESET;
volatile MODE_LIST MODE = ARM;
volatile uint8_t wake_flag = 0;

volatile uint8_t TMR1_Flag = 0;
// Prototypes
extern void sleepModeIdle();
extern void sleepModePowerDown();
void setIRCnt(uint8_t val);


extern void setModeStop();
extern void setModeRunning();
extern void initIRMode();
extern void setRunningFlag();
extern void setExecuteFlag();
extern void clearExecuteFlag();

extern uint8_t getRunningMode();
extern uint8_t getExecuteFlag();
extern uint8_t checkRunningMode();
#endif	/* MAIN_H */

