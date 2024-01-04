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
volatile uint8_t SM_flag = 1;

int main(void) {
    
    cli();
    setClock();
    setGPIO();
    setPowerSavings();
    setTimer0();
  
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
                        armWakeUpInt();
                        enable_PCINT();  //sei()
                        MODE = WAIT;
                        testPinOut(1);  testPinOut(0);
                        testPinOut(1);  testPinOut(0);
                        testPinOut(1);  testPinOut(0);
                        testPinOut(1);  testPinOut(0);
                         /* Fall through */

                case WAIT:
                        // Power down until button press
                         testPinOut(1); 
                     
                        sleepModePowerDown();
                         testPinOut(0);testPinOut(1); testPinOut(0);

                          
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
                        testPinOut(1); testPinOut(0);
                        testPinOut(1); testPinOut(0);
                        
                        if (checkVolUp() || checkVolDown())
                        {
                           // startTimeout();
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

                        testPinOut(1); testPinOut(0);
                        testPinOut(1); testPinOut(0);
                        testPinOut(1); testPinOut(0);
                        break;
                    }
                    else
                    { // Check if buttons were pressed
                       if (checkVolUp() | checkVolDown()){
                           testPinOut(1); _delay_us(250);testPinOut(0);
                            startTimeout();
                            //Send CMD
      //------------------------  
                            if (getRunningMode() == 0)
                            {
                                testPinOut(1); _delay_ms(1);testPinOut(0);
                                sendCMD();
                                timer1_flag = 1;
                                
                                MODE = SENDING;

                                timerWakeCnt = 0;
                                SM_flag = 1;
                                testPinOut(1); _delay_ms(1);testPinOut(0);
                            }
                           // MODE = WAIT_COND;
                            break;
                       }
                       else{
                           MODE = RESET;
                       } /* Fall Through */
                    }

                case RESET:
                    stopTimer1();
                    disable_Timer1Int();
                    timer1_flag = 1;
                    setModeStop();
                    
                    disable_PCINT();
                    disable_Timer0Int();
                    resetTimer0();
                    MODE = ARM;
  
                    SM_flag = 1;
                    testPinOut(1); _delay_ms(6);   testPinOut(0);
                    break;

                case TIMER_WAKE:

                    timerWakeCnt++;
                    if (timerWakeCnt > 0){
                        //  Timeout occurred!
                        timerWakeCnt = 0;
                        testPinOut(1); _delay_us(150);testPinOut(0);
                        SM_flag = 1;
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
        
        if (timer1_flag == 1)
            sendingSM();
        
        

        if (   (MODE == WAIT_COND) 
            && (timer1_flag == 0))
        {
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            
            sleepModeIdle();
            
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
        }
    }
}


ISR(PCINT0_vect) {
    MODE = WAIT_COND;
    disable_PCINT();
    SM_flag = 1;
}

ISR (TIMER0_OVF_vect)      //Interrupt vector for Timer0/Counter0
{
    PREV_MODE = MODE;
    MODE = TIMER_WAKE;    
    SM_flag = 1;
}

ISR(TIMER1_OVF_vect)
{
    IR_Cnt++;
    if (IR_Cnt >= 21)
    {
        IR_Cnt = 0;
        BURST_CHANGE++;
        timer1_flag = 1;
      
    }
}

/* FYI,  This goes off being the scenes
ISR(TIMER1_COMPA_vect)
{
}
 */

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