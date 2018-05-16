//digital theramin  Yanming Lai


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>//for printf  formatted i/o
#include <stdlib.h> //for itoa function
#include "sine_table.h"

#define  Xtal       3686400          // system clock frequency
#define  prescaler  1                // timer1 prescaler
#define  N_samples  128              // samples needed
#define  Fck        Xtal/prescaler   // t1 frequency

const unsigned char auc_frequency [8] = {
37, 42, 47, 50, 56, 62, 70, 74};

volatile unsigned char x_SW = 0x00, ch, Av;      //x_SW = step width  channel , voltage ahd freq
unsigned int  i_CurSinVal = 0;           // find freq LUT
unsigned int  i_TmpSinVal;
unsigned char x=0, y = 0;
int n;

unsigned char msg1[] = "\fThe frequency is ";  //using stdio
unsigned char msg2[] = ".\n";// 2 charcters 
unsigned char msg3[] = "The volume is ";// 22 char

// Function prototype Declaration
unsigned char decode(unsigned char x);
unsigned char decodevol(unsigned char x);
int usart_putchar (char c, FILE *stream);
int usart_getchar (FILE *stream);
unsigned char sw_decode(unsigned char x);
void init(void);

FILE usart_str = FDEV_SETUP_STREAM(usart_putchar, usart_getchar, _FDEV_SETUP_RW);


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
		printf("The frequency is %c.\n", x + '0');
		_delay_ms(50);             //fprintf(the freq is ...)
	   //or for (n=0;n<19;n++) usart+transimit(msg1{n},.......

		ch = 0;
		ADMUX = (ADMUX & 0xE0)|ch;
		ADCSRA |= (1<<ADSC);
		Av = decodevol(y);
		printf("The volume is %c.\n", Av + '0');

		   //ch1 volume 
	    //fprintf(the vuloume is ...)  prefer this one
	    
	   //or convert x_SW to ascii -- itoa(x_SW, val, 10)
	   //for (n=0;n<19;n++) usart+transimit(msg1{n},.......
	   
	   //continue infinitely
		_delay_ms(100);
		printf("\f");

	} 
	return 0;
}

ISR(ADC_vect)
{
	if(ch == 1)
		x = ADCH>>4;  // to make the change smooth by shifting 4 bits    for freq
 
	if(ch == 0)
		y = (ADCH>>1)+30;   //need some tweak    30-157
}

// Timer overflow interrupt service routine
ISR(TIMER1_OVF_vect)
{
	unsigned char fout;

	i_CurSinVal += x_SW; 
	i_TmpSinVal = (char)(((i_CurSinVal+2)>>2)&0x007F);
	// Set OCR1A
	
	fout = (unsigned char) pgm_read_byte(&auc_SinParam[i_TmpSinVal]);
	fout -= fout>> Av;
	OCR1A = fout;
	

}

// Initialization
void init (void)
{
	stdout = stdin = &usart_str;
	
	TIMSK  = (1<<TOIE1);               // T1 OVF interrupt enable
	TCCR1A = (1<<COM1A1)|(1<<WGM10);   // non inverting / 8Bit PWM
	TCCR1B = (1<<CS10);                // clk/1

	DDRD   = (1 << PD5);               // PD5 (OC1A) as output
	PORTD = 0xFF;                      // Pull-up inputs

	ADMUX |= (1<<REFS0);   
	ADCSRA |= (1<<ADEN)|(1<<ADIE);
	ADMUX |= (1<<ADLAR);

	UBRRH = 0;          // initialize baud rate to 9600 bps
	UBRRL = 23;

	UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);  // set 8 data bits, no parity, 1 stop bit (8-N-1)
	UCSRB |= (1<<RXEN)|(1<<TXEN);   // enable receiver and transmitter
	UCSRB |= (1<<RXCIE);

	sei();                     	     // Interrupts enabled
}

//usart_putchar
int usart_putchar (char c, FILE *stream)
{
	if (c=='\n') 
		usart_putchar('\r',stream);
	while (!(UCSRA & (1<<UDRE)));
	UDR = c;
	return 0;
}

//usart_getchar
int usart_getchar (FILE *stream)
{
	if (UCSRA & (1<<RXC)) //double check
		return UDR;
	else {
		return 0;
	}
}

unsigned char decode(unsigned char x)
{
	if(x<32) return 1;
	else if(x<64) return 2;
	else if(x<96) return 3;
	else if(x<128) return 4;
	else if(x<160) return 5;
	else if(x<192) return 6;
	else if(x<224) return 7;
	else if(x<256) return 8;
	else return 0;
}
unsigned char decodevol(unsigned char x)
{
	if(x<64) return 0;
	else if(x<128) return 1;
	else if(x<192) return 2;
	else if(x<256) return 3;
	else return 0;

}
