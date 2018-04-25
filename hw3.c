//part1
//ADC with single channel ADC0 input


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>




int main(void){
    // initialization 
    DDRB = 0xFF; //set LED port as output
    PORTB = 0xFF; //turn off LEDS
    
    //ADC initialization
    ADMUX |= (1<<REFS1)|(1<<REFS0);   //internal reference
    
    //set left adjust
    ADMUX |= (1<<ADLAR);
    
    //prescaler /2, enable ADC
    ADCSRA |= (1<<ADEN);
    
    //enable ADC interrupt
    ADCSRA |= (1<<ADIE);
    
    //enable global interrupt
    sei();
    
    while(1){
    //enable ADC interrupt
    ADCSRA |= (1<<ADIE);
    
    //enable global interrupt
    sei();
    
    //start conversion
    ADCSRA |= (1<<ADSC);
    
    }
    return 0;
}


//interrput service routine

ISR(ADC_vect){
    PORTB = ~ADCH;
    
}






//part2
//blink between ADC0 and ADC1


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void adc_init(void);
void adc_start_conversion(unsigned char);

//current channel
unsigned char ch;

 
//*****************************************************************************
//
//  ADC module initialization  
//
//*****************************************************************************
void adc_init(void)
{
   //select reference voltage
   //AVCC with external capacitor at AREF pin
   ADMUX|=(0<<REFS1)|(1<<REFS0);
   // Left adjust
   ADMUX |= (1<<ADLAR);
   //set prescaller and enable ADC
   ADCSRA|=(1<<ADEN)|(1<<ADIE);//enable ADC with dummy conversion
   //set sleep mode for ADC noise reduction conversion
   //set_sleep_mode(SLEEP_MODE_ADC);
}

//*****************************************************************************
//
//  ADC single conversion routine  
//
//*****************************************************************************
void adc_start_conversion(unsigned char channel)
{
   //remember current ADC channel;
   ch=channel;
   //set ADC channel
   ADMUX=(ADMUX&0xE0)|channel;
   //Start conversion with Interrupt after conversion
   //enable global interrupts
   ADCSRA |= (1<<ADIE);
   sei();
   ADCSRA |= (1<<ADSC);
}


//*****************************************************************************
//
//  ADC conversion complete service routine  
//
//*****************************************************************************
ISR(ADC_vect)
{
	// display result
	PORTB = ~ADCH;
}

 
//*****************************************************************************
//
//  run analog digital converter, timer.  
//
//*****************************************************************************
int main(void)
{
    DDRB = 0xFF;

	adc_init();
	while(1)
	{
	   //read ADC0
	   adc_start_conversion(0);
	   _delay_ms(500);
	   //read ADC1
	   adc_start_conversion(1);
	   _delay_ms(500);
	   //continue infinitely
	}
	return 0;
}

//part3
//first 4 bit for ADC0, last 4 bits for ADC1


// 2 channel ADC
// upper and lower nibble display
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void adc_init(void);

//current channel
volatile unsigned char ch;

 
//*****************************************************************************
//
//  ADC module initialization  
//
//*****************************************************************************
void adc_init(void)
{
   //select reference voltage
   //AVCC with external capacitor at AREF pin
   ADMUX|=(0<<REFS1)|(1<<REFS0);
   // Left adjust
   ADMUX |= (1<<ADLAR);
   //set prescaller and enable ADC
   ADCSRA|=(1<<ADEN)|(1<<ADIE);//enable ADC with dummy conversion
   sei();
}


//*****************************************************************************
//
//  ADC conversion complete service routine  
//
//*****************************************************************************
ISR(ADC_vect)
{
	// display result
	if (ch==0)   
	  PORTB = (PORTB&0x0F)|((~ADCH)&0xF0);
	if (ch==1)
	  PORTB = (PORTB&0xF0)|((~ADCH)>>4);

}

 
//*****************************************************************************
//
//  run analog digital converter, timer.  
//
//*****************************************************************************
int main(void)
{
    DDRB = 0xFF;

	adc_init();
	while(1)//loop demos
	{
	   ch = 0;
	   ADMUX=(ADMUX&0xE0)|ch;
       ADCSRA |= (1<<ADSC)|(1<<ADIE);	   
	   //_delay_ms(50);
	   ch = 1;
	   ADMUX=(ADMUX&0xE0)|ch;
       ADCSRA |= (1<<ADSC)|(1<<ADIE);
	   //_delay_ms(50);
	   //continue infinitely
	}
	return 0;
}
