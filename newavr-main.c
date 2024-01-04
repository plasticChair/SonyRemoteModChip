/*
 * File:   newavr-main.c
 * Author: super
 *
 * Created on December 30, 2023, 9:47 PM
 */
#define F_CPU 4000000

#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "perif_control.h"

typedef enum  { ARM,
            WAIT,
            TRIGGERED_WAKE,
            TIMER_WAKE,
            WAIT_COND,
             WAIT_COND2,
            RESET
            } MODE_LIST;


// D I/O Samples
uint8_t sample_R_TP63 = 0;
uint8_t sample_G_TP12 = 0;
uint8_t sample_B_TP64 = 0;

// Time Vars
volatile uint8_t vol_up_isr = 0;
volatile uint8_t vol_down_isr = 0;
volatile uint32_t currTime = 0;
volatile uint32_t prevTime = 0;
volatile uint32_t diffTime = 0;

volatile uint8_t gpio_wake = 0;
volatile uint8_t timer0_wake = 0;
volatile uint8_t wakeup_activity = 0;
volatile uint8_t OUTPUT_STATE = 0;
volatile uint8_t timerWakeCnt = 0;
volatile MODE_LIST PREV_MODE = RESET;
volatile MODE_LIST MODE = ARM;
volatile uint8_t wake_flag = 0;
// Prototypes
void sleepModeIdle();
void sleepModePowerDown();
uint16_t getDiffMillis();
uint8_t check_vol_up_state();
uint8_t check_vol_down_state();
void enable_INT0();
void disable_INT0();
void enable_PCINT();
void disable_PCINT();
void startTimeout();
void resetTimer0();
void disable_Timer0Int();
void testPinOut(int state);
void startToggle();

uint8_t checkVolUp();
uint8_t checkVolDown();
uint8_t checkHighs();
void sampleGPIO();


int main(void) {
    
    //Define Ins/outs
    cli();
    
    setClock();
    setGPIO();
    setPowerSavings();

    setTimer0();

    
    startToggle();
    
    timer0_wake = 0;
    wakeup_activity = 0;
  
    MODE = ARM;
    
    while (1) 
    {

        switch (MODE)
        {
            case ARM:
                    // Interrupt Setup 
                    armWakeUpInt();
                    enable_PCINT();
                    sei();
                    MODE = WAIT;
                     // Fall through
                
            case WAIT:
                    sleepModePowerDown();
                    if (MODE != TIMER_WAKE)
                        MODE = TRIGGERED_WAKE;
                    else
                        break;
                    // Fall through

            case TRIGGERED_WAKE:

                    sampleGPIO();
                    if (checkVolUp() || checkVolDown())
                    {
  //--------------------
                        testPinOut(1); testPinOut(0);
                        startTimeout();
                        enable_PCINT();
                        MODE = WAIT_COND;
                    }
                    else
                    {
                        MODE = RESET;
                    }
                    break;
                    
            case TIMER_WAKE:
                timerWakeCnt++;
                testPinOut(1); testPinOut(0);
                testPinOut(1); testPinOut(0);
                                testPinOut(1); testPinOut(0);
                testPinOut(1); testPinOut(0);
                                testPinOut(1); testPinOut(0);
                testPinOut(1); testPinOut(0);
                
                if (timerWakeCnt > 0)
                {

                    timerWakeCnt = 0;
                    MODE = RESET;
                    //set_skip_sleep()
                }
                else
                {
                    MODE = PREV_MODE;
                }

                break;
            
            case WAIT_COND:
                
                sampleGPIO();   
                if (checkHighs())
                {
                    //Good, go back to sleep
                    enable_PCINT();
                    testPinOut(1); _delay_ms(1);   testPinOut(0);
                    testPinOut(1);  testPinOut(0);
                    MODE = WAIT_COND;
                     break;
                }
                else
                {
                   if (checkVolUp() | checkVolDown())
                   {
                       if (TCNT0 < 255)
                       {
                           resetTimer0();
                           //Send CMD
     //------------------------  
                           timerWakeCnt = 0;
                           testPinOut(1); _delay_ms(3);   testPinOut(0);

                       }
                        enable_PCINT();
                       //sei();
                        MODE = WAIT_COND;
                         break;
                   }
                   else
                   {
                        MODE = RESET;
                   }
                }
                

            case RESET:
                
                enable_INT0();
                disable_PCINT();
                disable_Timer0Int();
                resetTimer0();
                MODE = ARM;
//--//-//-//--- 
                testPinOut(1); _delay_ms(6);   testPinOut(0);
                 _delay_ms(6);   testPinOut(1);
                 _delay_ms(6);   testPinOut(0);
                break;
                 
            default:
                break;         
            
        }
        

        if (MODE == WAIT_COND)
        {
 //--//---  testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            sleepModeIdle();
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
            testPinOut(1); testPinOut(0);
        }
        
        if (wake_flag == 1)
        {
            wake_flag =0;

        }

        
        /*

        if(timer0_wake == 1)
        {
            timer0_wake = 0;
            testPinOut(1); _delay_ms(25);              
            testPinOut(0);
            disable_Timer0Int();
        }
         */
       
    }
}

// _delay_ms(50);              // 50ms delay

ISR(PCINT0_vect) {
    MODE = WAIT_COND;
    disable_PCINT();
}

ISR(INT0_vect) {
    disable_INT0();
    MODE = TRIGGERED_WAKE;
  //  GIFR = (1 << INTF0);

}

ISR (TIMER0_OVF_vect)      //Interrupt vector for Timer0/Counter0
{
   // wake_flag = 1;
    PREV_MODE = MODE;
    MODE = TIMER_WAKE;    
}

ISR(TIMER1_COMPA_vect) {
}

void sleepModePowerDown() {
    // Turn off ADC
    ADCSRA &= (~(1 << ADEN));
    // Set sleep mode to Power Down
    //SLEEP_MODE_IDLE
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sei(); 
    // Enable sleep
    sleep_enable();
    // Sleep until an interrupt occurs
    sleep_cpu();
    // Disable sleep after waking up
    sleep_disable();
}


void sleepModeIdle() {
    // Turn off ADC
    ADCSRA &= (~(1 << ADEN));
    // Set sleep mode to Power Down
    //SLEEP_MODE_IDLE
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei(); 
    // Enable sleep
    sleep_enable();
    // Sleep until an interrupt occurs
    sleep_cpu();
    // Disable sleep after waking up
    sleep_disable();
}

void enable_INT0()
{
    cli();
    GIMSK |= (1<< INT0) ; // Enable INT0 
    MCUCR &= ~((1 << ISC01) | (0 << ISC00)) ; // Low level int
  //  PCMSK |= (1<< WAKE_UP_INT) ; // Enable  
    sei();
}

void disable_INT0()
{
    cli();
    GIMSK &= ~(1<< INT0) ; // Enable INT0 
   // PCMSK &= ~(1<< WAKE_UP_INT) ; // Enable  
    sei();
}


void enable_PCINT()
{
    cli();
    GIMSK |= (1<< PCIE) ;        // Enable  
    MCUCR |= ((1 << ISC01) | (0 << ISC00)) ; // Falling
    PCMSK |= (1 << R_TP63_INT) | (1 << B_TP64_INT) ;
    sei();

}

void disable_PCINT()
{
    cli();
    GIMSK &= ~(1<< PCIE) ;        // Enable  
    //PCMSK |= (1 << R_TP63_INT) | (1 << B_TP64_INT) ;    
    sei();
}

void startTimeout()
{
    cli();
    TCNT0 = 0x00;
    TIMSK |=(1<<TOIE0);
    sei();
}

void disable_Timer0Int()
{
    cli();
    TIMSK &= ~(1<<TOIE0);
    sei();
}

void resetTimer0()
{
    TCNT0 = 0x00;
}

void testPinOut(int state)
{
    if (state == 0)
    {
        PORTB &= ~(1 << T_output);  // LED OFF
    }
    else if (state == 1)
    {
        PORTB |=  (1 << T_output);  
    }
}


uint8_t checkVolUp()
{
    return (   (sample_R_TP63 == 0)
            && (sample_G_TP12 == 0)
            && (sample_B_TP64 == 1));
}

uint8_t checkVolDown()
{
    return (   (sample_R_TP63 == 1)
             && (sample_G_TP12 == 0)
            && (sample_B_TP64 == 0));
}

uint8_t checkHighs()
{
    return (   (sample_B_TP64 == 1)
             && (sample_G_TP12 == 1)
            && (sample_R_TP63 == 1));
}
void sampleGPIO()
{
    sample_R_TP63 = (PINB & (1 << R_TP63)) >> R_TP63; //digitalRead(R_TP63);
    sample_G_TP12 = (PINB & (1 << G_TP12)) >> G_TP12; //digitalRead(G_TP12);
    sample_B_TP64 = (PINB & (1 << B_TP64)) >> B_TP64; //digitalRead(B_TP64);
}
///////////////////

void startToggle()
{
        testPinOut(0);
     _delay_ms(15);
    testPinOut(1);
     _delay_ms(200);              
    testPinOut(0); _delay_ms(15);              
    testPinOut(1); _delay_ms(15);             
     testPinOut(0); 

}