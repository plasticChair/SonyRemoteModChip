#include "helper.h"

#define F_CPU 4000000
#include <util/delay.h>

uint8_t *cmdDataPtr;

// PWM SM

extern volatile uint8_t BURST_CHANGE;

IR_MODE_LIST IR_MODE;

// D I/O Samples
uint8_t sample_R_TP63 = 0;
uint8_t sample_G_TP12 = 0;
uint8_t sample_B_TP64 = 0;

#define DATA_SIZE 4
uint8_t volUpMsg[DATA_SIZE]   = {0xF5,0xAB,0x55,0x56};
uint8_t volDownMsg[DATA_SIZE] = {0xED,0xAB,0x55,0x56};


uint8_t runningFlag = 0;
volatile uint8_t executeFlag = 0;
uint8_t stopFlag = 0;
//--------------------------------//
// DIO Sampling
uint8_t checkVolUp()
{
    return (   (sample_R_TP63 == 0)
            && (sample_G_TP12 == 0)
            && (sample_B_TP64 == 1));
}

uint8_t checkVolDown()
{
    return (    (sample_R_TP63 == 1)
             && (sample_G_TP12 == 0)
             && (sample_B_TP64 == 0));
}

uint8_t checkHighs()
{
    return (    (sample_B_TP64 == 1)
             && (sample_G_TP12 == 1)
             && (sample_R_TP63 == 1));
}


void sendCMD()
{
    setTimer1();
    enable_Timer1Int();
    setIRCnt( 0);
    BURST_CHANGE = 0;
    initIRMode();
    
    if (checkVolUp()){
       cmdDataPtr = volUpMsg;
    }
    else{
       cmdDataPtr = volDownMsg;
    }
}

void setModeRunning()
{

    if (IR_MODE == WAIT_IR){
        IR_MODE = ASSIGN;
        stopFlag = 0;
        setExecuteFlag();
    }
    
}

void setModeStop()
{
    stopFlag = 1;
    IR_MODE = WAIT_IR;
}
uint8_t getRunningMode()
{
    return runningFlag;
}

uint8_t checkRunningMode()
{
    return (IR_MODE != WAIT_IR);
}

void setRunningFlag()
{
    runningFlag = 1;
}

void setExecuteFlag()
{
    executeFlag = 1;
}

void clearExecuteFlag()
{
    executeFlag = 0;
}


uint8_t getExecuteFlag()
{
    return executeFlag;
}


void initIRMode()
{
    IR_MODE = WAIT_IR;
}

uint8_t sendingSM()
{
    static uint8_t data_byte_idx = 0;
    static int8_t data_bit_idx = 8;
    static uint8_t curr_byte = 0;
    
    uint8_t curr_bit = 0;
    uint8_t Low_Length = 0;
    

  /*
    if (IR_MODE != WAIT_IR)
    {
        IRPinOut(1); IRPinOut(0); 
  
        if (runningFlag == 0)
        {
            //startTimer1();
            setIRHigh();
            runningFlag = 1;
        }
        
        executeFlag = 0;

        
        if (stopFlag == 1)
        {
            IRPinOut(0);
            IR_MODE = WAIT_IR;
            runningFlag = 0;
            //stopTimer1();
            setIRLow();
        }
        
    }
    return;
    */
    clearExecuteFlag();
    switch (IR_MODE)
    {
        case WAIT_IR:
            runningFlag = 0;
            return 1;
            break;
            
        case ASSIGN:
            runningFlag = 1;
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
                
            startTimer1();
            BURST_CHANGE = 0;
            setIRCnt(0);
            IR_MODE = NEXT_BYTE;
            data_byte_idx = 0;
            /*Fall through*/

        case NEXT_BYTE:
           // Check if reached end of data

            if (data_byte_idx >= (DATA_SIZE)){
                setIRLow();
                data_byte_idx = 0;
                IR_MODE  = WAIT_IR;
                stopTimer1();
                setExecuteFlag();
                return 1;

                break;
            }
            else{
                curr_byte = *(cmdDataPtr + data_byte_idx);
                data_byte_idx++;
            }

            BURST_CHANGE = 0;
            setIRCnt(0) ;
            data_bit_idx = 8;
            IR_MODE      = NEXT_BIT;
            /*Fall through*/

        case NEXT_BIT:
            // Check if all 8 bits are done
            data_bit_idx--;
            if (data_bit_idx < 0){
                IR_MODE = NEXT_BYTE;
                setExecuteFlag(); // Force Loop again

                //return 1;
                break;
            }
            else{
                // Get single bit
                curr_bit =  (curr_byte >> data_bit_idx) & 1;
            }
            

            // Reset Vals
            BURST_CHANGE = 0;
            setIRCnt(0);
            //stopTimer1();
            //startTimer1();
            
            
            if (curr_bit == 1){
                IR_MODE = LOGIC_HIGH;
                /*Fall through*/
            }
            else{
                IR_MODE = LOGIC_LOW;
                setExecuteFlag(); // Force Loop again
                break;
            }

        case LOGIC_HIGH:
            //OUT HIGH
            setIRHigh();
             IR_MODE = NEXT_BIT;
            break;

        case LOGIC_LOW:

            //OUT LOW
             setIRLow();

             IR_MODE = NEXT_BIT;
            break;

        default:
            break;
    } // End Switch
    return 0;
}

