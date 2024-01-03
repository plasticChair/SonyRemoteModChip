/*
 * File:   newavr-main.c
 * Author: super
 *
 * Created on January 1, 2024, 6:14 PM
 */

#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void startTimer1();
void stopTimer1();

#define DATA_SIZE 3
typedef enum {
            ASSIGN,
            NEXT_BYTE,
            NEXT_BIT,
            DATA_OUT,
}IR_MODE_LIST;

IR_MODE_LIST IR_MODE = ASSIGN;
    
    
volatile uint8_t STATE = 0;
volatile uint8_t STATE2 = 0;
volatile uint8_t IR_Cnt = 0;

volatile uint8_t BURST_CHANGE = 0;
volatile uint8_t timer1_flag = 0;

void testPinOut(int state, int pin);

int main(void) {
    
    cli();
 
    
    // 4 MHz Clock
    CLKPR = _BV(CLKPCE);
    CLKPR = _BV(CLKPS0);
            
            
    // 14.4.3 DDRB ? The Port B Data Direction Register
    DDRB =     (1 << PB0)  | (1 << PB1)   | (1 << PB2)  | (1 << PB4)  ;
    
    TCCR1 = 0;
   // PLLCSR = (0 << LSM) | (0 << PCKE) | (0 << PLLE) ;
     
     
    TCNT1 = 0;
    
    OCR1A = 12;
    OCR1B = 5;
    OCR1C = 25;
            
    TIMSK = (1 << OCIE1A) | (1 << TOIE1); //| (1 << OCIE1B)
    
    sei();

    uint8_t data[DATA_SIZE] = {0x00,0xFF,0x00};
    uint8_t curr_byte = 0;
    uint8_t curr_bit = 0;
    
    uint8_t data_byte_idx = 0;
    uint8_t data_bit_idx = 0;
    uint8_t Low_Length = 0;
    
    _delay_ms(1);
    
    stopTimer1();
    startTimer1();
    IR_MODE =  ASSIGN;
    
    
    while(1)
    {
        if (timer1_flag == 1)
        {
            timer1_flag = 0;
            
            switch (IR_MODE)
            {
                case ASSIGN:
                    IR_MODE = NEXT_BYTE;
                    data_byte_idx = 0;
                    /*Fall through*/
                    
                case NEXT_BYTE:
                   // Check if reached end of data
                     PORTB |=  (1 << PB2);
                     PORTB |=  (1 << PB2);
                    if (data_byte_idx > (DATA_SIZE-1)){
                        data_byte_idx = 0;
                        IR_MODE       = ASSIGN;
                        break;
                    }
                    else{
                        // Grab selected byte
                        curr_byte = data[data_byte_idx];
                        data_byte_idx++;
                    }
                    BURST_CHANGE = 0;
                    data_bit_idx = 0;
                    IR_MODE      = NEXT_BIT;
                    /*Fall through*/
                    
                case NEXT_BIT:
                    // Check if all 8 bits are done
                    PORTB |=  (1 << PB4);
                    PORTB |=  (1 << PB4);
                    if (data_bit_idx >= 8){
                                                IR_MODE = NEXT_BYTE;
                        timer1_flag  = 1; // Force Loop again
                        break;
                    }
                    else{
                        // Get single bit
                        curr_bit = (curr_byte >> data_bit_idx) & 1;
                        data_bit_idx++;
                        
                        if (curr_bit == 1){
                            Low_Length = 4;
                        }
                        else{
                            Low_Length = 1;
                        }
                    }

                    // Reset Vals
                    BURST_CHANGE = 0;
                    IR_Cnt       = 0;
                    stopTimer1();
                    startTimer1();
                    IR_MODE = DATA_OUT;  // AKA Default state
                    /*Fall through*/
                    
               case DATA_OUT:
                   
                   if (BURST_CHANGE == 0){
                        //OUT HIGH
                         DDRB |= (1 << PB1);
                    }
                    else if (BURST_CHANGE <= Low_Length){
                        //OUT LOW
                        DDRB &= ~(1 << PB1);
                    }
                    else{
                         IR_MODE = NEXT_BIT;
                    }
                    
                default:
                    break;
            } // End Switch
        }


    }
}

ISR(TIMER1_COMPA_vect)
{

}

ISR(TIMER1_COMPB_vect)
{
    //STATE = !STATE;
   // testPinOut(STATE,PB2);
    
 //   PORTB ^= (1 << PB0);
}

ISR(TIMER1_OVF_vect)
{
  //  STATE2 = !STATE2;
  //  testPinOut(STATE2,PB0);
   
    
    IR_Cnt++;
    if (IR_Cnt >= 14)
    {
        IR_Cnt = 0;
        BURST_CHANGE++;
        timer1_flag = 1;
        //PORTB ^= (1 << PB2);
       // PORTB ^= (1 << PB2);
    }

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
void testPinOut(int state, int pin)
{
    if (state == 0)
    {
        PORTB &= ~(1 << pin);  // LED OFF
    }
    else if (state == 1)
    {
        PORTB |=  (1 << pin);  
    }
}