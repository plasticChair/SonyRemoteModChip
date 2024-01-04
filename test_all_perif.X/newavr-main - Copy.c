/*
 * File:   newavr-main.c
 * Author: super
 *
 * Created on January 3, 2024, 10:40 PM
 */


#include "main.h"


#define T_output 4
#define IR_out 1
#define T_out2 0

#define R_TP63_INT PCINT0
#define B_TP64_INT PCINT3

volatile uint8_t T0_flag = 0;
volatile uint8_t T1_flag = 0;
volatile uint8_t T2_flag = 0;
volatile uint8_t PC_flag = 0;


extern void sleepModePowerDown();
extern void sleepModeIdle();


int main(void) {
    /* Replace with your application code */
    cli();
    // 4 MHz Clock
    CLKPR = _BV(CLKPCE);
    CLKPR = _BV(CLKPS0);
    
    PRR = ( 1 << PRUSI) | (1 << PRADC);
    ADCSRA &= ~_BV(ADEN); //ADC ON
    
    // Outs
    DDRB = 0;
    DDRB |= _BV(T_output) | _BV(IR_out) | _BV(T_out2);

    //Timer 0
    TCCR0A = 0X00;         //TCCR0A to low for normal port operation and mode 0.
    TCCR0B = 0X00;         //
    TCCR0B |= (1<<CS02) | (0 << CS01) | (1 << CS00);   //101 =   prescaler 
    TCNT0 = 0x00;          //initializing the counter to 0
    
    // Timer1, not enabled yet
    TCNT1 = 0;
    OCR1A = 8;
    OCR1C = 26;
    TIMSK = (1 << OCIE1A) | (1 << TOIE1); //| (1 << OCIE1B)
    
   // MCUCR = ((0 << ISC01) | (1 << ISC00)) ; 
    //Enable PC INTs
    GIMSK |= (1 << PCIE) ; //| (1<< INT0) ; // Enable  PC int
    PCMSK |= (1 << R_TP63_INT) | (1 << B_TP64_INT) ;   
    
    
    //Enable INT for Timer0
    TCNT0 = 0x00;
    TIMSK |=(1<<TOIE0);
    
    // Enable INT for Timer 1
    TIMSK |= (_BV(OCIE1A) | _BV(TOIE1));
    
    //Start Timer 1
    TCNT1 = 0;
    GTCCR |= (0 << PWM1B) | (0 << COM1B0);
    TCCR1  = (0 << CTC1) | (1 << PWM1A) | (0 << COM1A0) | (1 << COM1A1)  ;
    TCCR1 |= (0 << CS13) | (0 << CS12) | (1 << CS11) | (1 << CS10);  // Prescaler
    
    sei();
    
    while (1) {
        
        if (T0_flag == 1)
        {
            T0_flag = 0;
            PORTB ^=  _BV(T_output);  
        }
        
        if (T1_flag == 1)
        {
            T1_flag = 0;
         //   PORTB ^=  _BV(IR_out);  
        }
        
        if (T2_flag == 1)
        {
            T2_flag = 0;
            PORTB ^=  _BV(T_out2);  
        }
        
        sleepModeIdle();
    }
}

ISR(PCINT0_vect) {
    PC_flag = 1;
    sei();
}

ISR (TIMER0_OVF_vect)      //Interrupt vector for Timer0/Counter0
{
    T0_flag = 1;   
    sei();
}

ISR(TIMER1_OVF_vect)
{
    T2_flag = 1;
    sei();
}

ISR(TIMER1_COMPA_vect) {
    
   // T1_flag = 1;
 //   sei();
}

