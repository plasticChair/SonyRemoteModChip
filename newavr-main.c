/*
 * File:   newavr-main.c
 * Author: super
 *
 * Created on December 30, 2023, 9:47 PM
 */
#define F_CPU 4000000
#include <util/delay.h>

#include "main.h"

void IRPinOut(int state);
volatile uint8_t SM_flag = 0;
// PWM SM
uint8_t IR_Cnt = 0;
uint8_t BURST_CHANGE = 0;
uint8_t stat = 0;

int main(void) {
    
    cli();
    setClock();
    setPowerSavings();
    setGPIO();
    
    setTimer0();
    setTimer1(); //Not Enabled yet
    armWakeUpInt();
    setIRLow();
     
    
    MODE = ARM;
    uint8_t wait_cnt = 0;
    
    SM_flag = 1;

            
    while (1) 
    {
        
        if (SM_flag == 1)
        {
            SM_flag = 0;
            switch (MODE)
            {
                case ARM:
                        // Interrupt Setup 
                        enable_PCINT();  //sei()
                        MODE = WAIT;
                         /* Fall through */

                case WAIT:
                        // Power down until button press
                           testPinOut(1); _delay_us(130);testPinOut(0);

        
                        sleepModePowerDown();
                         testPinOut(1);  _delay_us(160);testPinOut(0);
   
        

                        if (MODE != TIMER_WAKE){
                            MODE = WAIT_COND;
                            /* Fall through */
                        }
                        else{
                            SM_flag = 1;
                            break;  // Only if timer wakes us up. Timer sets new STATE
                        }
                         
                case TRIGGERED_WAKE:

                        sampleGPIO();

                        
                        if (checkVolUp() || checkVolDown())
                        {
                            startTimeout();
                            MODE = WAIT_COND;
                        }
                        else
                        {
                            MODE = RESET;
                        }
                        break;

                case WAIT_COND:

                    sampleGPIO();   
                    if (checkHighs()){  // PCINT wakes up on rising edge :(.  Need to ignore it
                        MODE = WAIT_COND;
                        startTimeout();
                        break;
                    }
                    else
                    { // Check if buttons were pressed
                       if (checkVolUp() | checkVolDown()){
                     
                            startTimeout();
                            //Send CMD
                            if (getRunningMode() == 0)
                            {
                                sendCMD();
                                MODE = SENDING;
                                timerWakeCnt = 0;
                                SM_flag      = 1;
                            }
                            break;
                       }
                       else{
                           MODE = RESET;
                       } /* Fall Through */
                    }

                case RESET:
                    stopTimer1();
                    disable_Timer1Int();
                    
                    setModeStop();
                    setExecuteFlag();
                    
                    disable_Timer0Int();
                    resetTimer0();
                    MODE = ARM;
  
                    SM_flag = 1;
                    
                    testPinOut(1); _delay_ms(6);testPinOut(0);
  
                    break;

                case TIMER_WAKE:

                    timerWakeCnt++;
                    if (timerWakeCnt > 0){
                        //  Timeout occurred!
                        timerWakeCnt = 0;
                        SM_flag      = 1;
                        MODE = RESET;
                    }
                    else{
                        MODE = PREV_MODE;
                    }
                    break;

                case SENDING:
                        setModeRunning();
                        MODE = WAIT_COND;
                    break;

                default:
                    break;         
            }
        }
        
        enable_PCINT();
		
		if (TMR1_Flag == 1)
		{
            TMR1_Flag = 0;
			IR_Cnt++;

			 if (IR_Cnt >= 21)
			 {
				 IR_Cnt = 0;
				 BURST_CHANGE++;
				 setExecuteFlag();
			 }
		}
        
        if (getExecuteFlag()){ // True if IR_MODE != WAIT_IR
            stat = sendingSM();
        }

        if (  (MODE == WAIT_COND)
            && (getRunningMode() == 0)   )
        {
            testPinOut(1); _delay_us(95);testPinOut(0);
            sleepModeIdle();
            testPinOut(1); _delay_us(115);testPinOut(0);
        }
    }
}


ISR(PCINT0_vect) {
    MODE = WAIT_COND;
    //disable_PCINT();
    SM_flag = 1;

}

ISR (TIMER0_OVF_vect)      //Interrupt vector for Timer0/Counter0
{
    PREV_MODE = MODE;
    MODE = TIMER_WAKE;  
    testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
    SM_flag = 1;
}

ISR(TIMER1_OVF_vect)
{
	TMR1_Flag = 1;

}

//FYI,  This goes off behind the scenes
ISR(TIMER1_COMPA_vect)
{

}


void IRPinOut(int state)
{
    if (state == 0)
    {
        PORTB &= ~(1 << PB1);  // LED OFF
    }
    else if (state == 1)
    {
        PORTB |=  (1 << PB1);  
    }
}

void setIRCnt(uint8_t val) 
{
    IR_Cnt = val;
}