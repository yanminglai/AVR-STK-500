// Sinewave generator 
// Frequency is selected by PB switches (PortA)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "sine_wave.h"

#define  Xtal       3686400          // system clock frequency
#define  prescaler  1                // timer1 prescaler
#define  N_samples  128              // Number of samples in lookup table
#define  Fck        Xtal/prescaler   // Timer1 working frequency

const unsigned char auc_frequency [8] = {
37, 42, 47, 50, 56, 62, 70, 74};

volatile unsigned char x_SW = 0x00, ch, Av;               // step width of frequency
unsigned int  i_CurSinVal = 0;           // position freq. in LUT (extended format)
unsigned int  i_TmpSinVal;
unsigned char x=0, y = 0;

// Function prototype
unsigned char decode(unsigned char x);
unsigned char decodevol(unsigned char x);

ISR(ADC_vect)
{
	if(ch == 1)
		x = ADCH;
 
	if(ch == 0)
		y = ADCH;
}

// Timer overflow interrupt service routine
ISR(TIMER1_OVF_vect)
{
	unsigned char fout;
	// move Pointer about step width aheaed
	i_CurSinVal += x_SW; 
	i_TmpSinVal = (char)(((i_CurSinVal+2)>>2)&0x007F);
	// Set OCR1A
	
	fout = (unsigned char) pgm_read_byte(&auc_SinParam[i_TmpSinVal]);
	fout -= fout>> Av;
	OCR1A = fout;
	
	//OCR1A = pgm_read_byte(&auc_SinParam[i_TmpSinVal]);
}

// Initialization
void init (void)
{
  TIMSK  = (1<<TOIE1);               // Int T1 Overflow enabled
  TCCR1A = (1<<COM1A1)|(1<<WGM10);   // non inverting / 8Bit PWM
  TCCR1B = (1<<CS10);                // CLK/1
//  DDRD = 0xFF;
  DDRD   = (1 << PD5);               // PD5 (OC1A) as output
  PORTD = 0xFF;                      // Pull-up inputs

  ADMUX |= (1<<REFS0);
  ADCSRA |= (1<<ADEN)|(1<<ADIE);
  ADMUX |= (1<<ADLAR);

  sei();                     	     // Interrupts enabled
}


int main (void)
{
	init();
	while (1)
	{
		ch = 1;
		ADMUX = (ADMUX & 0xE0)|ch;
		ADCSRA |= (1<<ADSC);
		x = decode(x);
		if (x>0) { 
			x_SW = auc_frequency[x-1];
		}
		_delay_ms(1);

		ch = 0;
		ADMUX = (ADMUX & 0xE0)|ch;
		ADCSRA |= (1<<ADSC);
		Av = decodevol(y);
		_delay_ms(1);

	} 
	return 0;
}

unsigned char decode(unsigned char x)
{
	if(x==0x00)
	{
		return 1;
	}
	else if(x==0x01)
	{
		return 2;
	}
	else if(x<0x04 && x>=0x02)
	{
		return 3;
	}
	else if(x<0x08 && x>=0x04)
	{
		return 4;
	}
	else if(x<0x10 && x>=0x08)
	{
		return 5;
	}
	else if(x<0x20 && x>=0x10)
	{
		return 6;
	}
	else if(x<0x40 && x>=0x20)
	{
		return 7;
	}
	else if(x<0x80 && x>=0x40)
	{
		return 8;
	}
	else
	{
		return 0;
	}
}
unsigned char decodevol(unsigned char x)
{
	if(x<50) return 0;
	else if(x<100) return 3;
	else if(x<150) return 2;
	else if(x<200) return 1;
	else return 0;

}
