/*
 * File:   newavr-main.c
 * Author: super
 *
 * Created on December 30, 2023, 9:47 PM
 */


# define F_CPU 8000000UL

#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



#define T_output 4
#define R_TP63 1
#define G_TP12 2
#define B_TP64 3

#define R_TP63_INT PCINT1
#define B_TP64_INT PCINT3

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
volatile uint8_t OUTPUT_STATE = 0;

// Prototypes
void sleepModeIdleDown();
uint16_t getDiffMillis();
uint8_t check_vol_up_state();
uint8_t check_vol_down_state();


int main(void) {
    
    //Define Ins/outs
    DDRB = 0;// (0 << PB3) | (0 << PB3) | (0 << PB3);
    DDRB |= (1 << T_output);
  
    // Timer 0
    TCCR0A = 0X00;               //TCCR0A to low for normal port operation and mode 0.
    TCCR0B = 0X00;                //WGM02=0
    TCCR0B |= (1<<CS02) | (1<<CS00);    //101 =  1024 prescaler
    TCNT0 = 0x00;       //initializing the counter to 0
    
     // 100.16025641025641 Hz (1000000/((155+1)*64))
    OCR1C = 155;
    // interrupt COMPA
    OCR1A = OCR1C;
    // CTC
    TCCR1 |= (1 << CTC1);
    TCCR1 |= (1 << CS12) | (1 << CS11) | (1 << CS10);
   // TCCR1 |= (1 << CS13) | (1 << CS12) | (0 << CS10); // 2048 prescaler
    TCNT1 = 0x00;
    
    // Output Compare Match A Interrupt Enable
    TIMSK |= (1 << OCIE1A);
    TIMSK|=(1<<TOIE0);// | (1<<TOIE1); //enabling timer0 interrupt
    sei();

    sample_R_TP63 = (PINB & (1 << R_TP63)) >> R_TP63; //digitalRead(R_TP63);
    sample_G_TP12 = (PINB & (1 << G_TP12)) >> G_TP12; //digitalRead(G_TP12);
    sample_B_TP64 = (PINB & (1 << B_TP64)) >> B_TP64; //digitalRead(B_TP64);
  
    /* Replace with your application code */
    while (1) {
        
        if (gpio_wake == 1)
        {
            gpio_wake = 0;
            
            if (OUTPUT_STATE){
                PORTB &= ~(1 << T_output);       // LED off
            }
            else{
                PORTB |=  (1 << T_output);       // LED on
            }            
        }
       // _delay_ms(50);              // 50ms delay
    }
}

ISR (TIMER0_OVF_vect)      //Interrupt vector for Timer0/Counter0
{
     gpio_wake = 1;
 OUTPUT_STATE = !OUTPUT_STATE;
    
}

ISR(TIMER1_COMPA_vect) {


}