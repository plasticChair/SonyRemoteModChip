#include "perif_control.h"


void setClock()
{
    // 4 MHz Clock
    CLKPR = _BV(CLKPCE);
    CLKPR = _BV(CLKPS0);
}

void setGPIO()
{
    DDRB = 0;// (0 << PB3) | (0 << PB3) | (0 << PB3);
    DDRB |= _BV(T_output) | _BV(IR_OUT);
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

void setTimer1()
{
    TCNT1 = 0;
    OCR1A = 8;
    OCR1B = 5;
    OCR1C = 26;
            
    TIMSK = (0 << OCIE1A) | (1 << TOIE1); //| (1 << OCIE1B)
}

void armWakeUpInt()
{
   // MCUCR = ((0 << ISC01) | (1 << ISC00)) ; 
    //GIMSK |= (1 << PCIE) ; //| (1<< INT0) ; // Enable INT0 int and PC int
    PCMSK |= (1 << R_TP63_INT) | (1 << B_TP64_INT) ; // | (1 << B_TP64_INT) ;             
}


void enable_PCINT()
{
    cli();
    GIMSK |= (1<< PCIE) ;        // Enable  
    //MCUCR |= ((1 << ISC01) | (0 << ISC00)) ; // Falling
    //PCMSK |= (1 << R_TP63_INT) | (1 << B_TP64_INT) ;
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

void disable_Timer1Int()
{
    cli();
    TIMSK &= ~(_BV(OCIE1A) | _BV(TOIE1));
    sei();
}

void enable_Timer1Int()
{
    cli();
   // TIMSK |= (_BV(OCIE1A) | _BV(TOIE1));
     TIMSK |=  _BV(TOIE1);
    sei();
}

void resetTimer0()
{
    TCNT0 = 0x00;
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


void startTimer1()
{
    TCNT1 = 0;
    GTCCR |= (0 << PWM1B) | (0 << COM1B0);
    TCCR1  = (0 << CTC1) | (1 << PWM1A) | (0 << COM1A0) | (1 << COM1A1)  ;
    TCCR1 |= (0 << CS13) | (0 << CS12) | (1 << CS11) | (1 << CS10);  // Prescaler
           
}
void stopTimer1()
{
    TCCR1 = 0;
}

void sampleGPIO()
{
    sample_R_TP63 = (PINB & (1 << R_TP63)) >> R_TP63; //digitalRead(R_TP63);
    sample_G_TP12 = (PINB & (1 << G_TP12)) >> G_TP12; //digitalRead(G_TP12);
    sample_B_TP64 = (PINB & (1 << B_TP64)) >> B_TP64; //digitalRead(B_TP64);
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

void setIRLow()
{
    DDRB &= ~(1 << PB1); // LOW
    PORTB |= (1 << PB1); // LOW
}

void setIRHigh()
{
    DDRB |= (1 << PB1); // HIGH
}