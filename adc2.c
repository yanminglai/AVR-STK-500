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
