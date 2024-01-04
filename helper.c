
#include "helper.h"

uint8_t *cmdDataPtr;

// PWM SM
volatile uint8_t IR_Cnt;
volatile uint8_t BURST_CHANGE;
volatile uint8_t timer1_flag;

IR_MODE_LIST IR_MODE;

// D I/O Samples
uint8_t sample_R_TP63 = 0;
uint8_t sample_G_TP12 = 0;
uint8_t sample_B_TP64 = 0;

#define DATA_SIZE 4
uint8_t volUpMsg[DATA_SIZE]   = {0x7a,0xd5,0xaa,0xab};
uint8_t volDownMsg[DATA_SIZE] = {0xF6,0xd5,0xaa,0xab};

uint8_t runningFlag = 0;
uint8_t stopFlag = 1;
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
    IR_Cnt = 0;
    BURST_CHANGE = 0;
    timer1_flag = 0;
    initIRMode();
    
    if (checkVolUp()){
       cmdDataPtr = volUpMsg;
    }
    else{
       cmdDataPtr = volUpMsg;
    }
}

void setModeRunning()
{
    if (IR_MODE == WAIT_IR){
        IR_MODE = ASSIGN;
        stopFlag = 0;
    }
    
}

void setModeStop()
{
    stopFlag = 1;
}
uint8_t getRunningMode()
{
    return runningFlag;
}

void initIRMode()
{
    IR_MODE = WAIT_IR;
}

uint8_t sendingSM()
{
    static uint8_t data_byte_idx = 0;
    static uint8_t data_bit_idx = 0;
    static uint8_t curr_byte = 0;
    
    uint8_t curr_bit = 0;
    uint8_t Low_Length = 0;
    

    
    if (IR_MODE != WAIT_IR)
    {
        IRPinOut(1); 
  
        runningFlag = 1;
        timer1_flag = 0;
        if (stopFlag == 1)
        {
            IRPinOut(0);
            IR_MODE = WAIT_IR;
            runningFlag = 0;
            //setIRLow();
        }
        return;
    }
    
    switch (IR_MODE)
    {
        case WAIT_IR:
            runningFlag = 0;
            return 1;
            break;
            
        case ASSIGN:
            runningFlag = 1;
            startTimer1();
            IR_MODE = NEXT_BYTE;
             setIRLow();
            /*Fall through*/

        case NEXT_BYTE:
           // Check if reached end of data
            curr_byte = *(cmdDataPtr + data_bit_idx);
            if (data_byte_idx > (DATA_SIZE-1)){
                setIRLow();
                data_byte_idx = 0;
                if (stopFlag != 1){
                    IR_MODE  = ASSIGN;
                }
                else
                {
                    IR_MODE  = WAIT_IR;
                }
                break;
            }
            else{
                data_byte_idx++;
            }
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            BURST_CHANGE = 0;
            data_bit_idx = 7;
            IR_MODE      = NEXT_BIT;
            /*Fall through*/

        case NEXT_BIT:
            // Check if all 8 bits are done
            if (data_bit_idx < 0){
                IR_MODE = WAIT_IR;
                //timer1_flag  = 1; // Force Loop again
                return 1;
                break;
            }
            else{
                // Get single bit
                curr_bit =  (curr_byte >> data_bit_idx) & 1;
                data_bit_idx--;

                if (curr_bit == 1){
                    IR_MODE = LOGIC_HIGH;
                }
                else{
                    IR_MODE = LOGIC_LOW;
                }
            }

            // Reset Vals
            BURST_CHANGE = 0;
            IR_Cnt       = 0;
            stopTimer1();
            startTimer1();
            timer1_flag  = 1; // Force Loop again

            break;

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

