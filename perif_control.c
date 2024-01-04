#include "perif_control.h"
#include <avr/io.h>



void setClock()
{
    // 4 MHz Clock
    CLKPR = _BV(CLKPCE);
    CLKPR = _BV(CLKPS0);
}

void setGPIO()
{
    DDRB = 0;// (0 << PB3) | (0 << PB3) | (0 << PB3);
    DDRB |= (1 << T_output);
}

void setPowerSavings()
{
    //PRR 
    PRR = ( 1 << PRUSI) | (1 << PRADC);
    ADCSRA &= ~_BV(ADEN); //ADC ON
}

void setTimer0()
{
        // Timer 0
    TCCR0A = 0X00;         //TCCR0A to low for normal port operation and mode 0.
    TCCR0B = 0X00;         //WGM02=0
    TCCR0B |= (1<<CS02) | (0 << CS01) | (1 << CS00);   //100 =  256 prescaler (about 62ms)
    TCNT0 = 0x00;          //initializing the counter to 0
    // Timer should overflow after 65ms @ 1MHz
}

void armWakeUpInt()
{
    MCUCR = ((0 << ISC01) | (1 << ISC00)) ; 
    GIMSK |= (1 << PCIE) ; //| (1<< INT0) ; // Enable INT0 int and PC int
    //GIMSK |=  (1<< INT0) ; // Enable INT0 int and PC int
    //PCMSK |= (1 << WAKE_UP_INT) ;
    PCMSK |= (1 << R_TP63_INT) | (1 << B_TP64_INT) ; // | (1 << B_TP64_INT) ;             
}