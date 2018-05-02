//digital theramin  Yanming Lai


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "sine_table.h" 
#include <stdio.h>  //for printf  formatted i/o
#include <stdlib.h> //for itoa function
//global variables


volatile unsigned char ch, Av, x_SW;   //channel , voltage ahd freq
unsigned int  i_CurSinVal = 0;           // position freq. in LUT (extended format)
unsigned int  i_TmpSinVal;

unsigned char msg1[] = "\fThe freq is ";  // clear screen (\f) first, then msg
unsigned char msg2[] = ".\n";  // 2 charcters
unsigned char msg3[] = "The volume is "; // 22 char



//function prototype
void initialize(void);
int usart_putchar (char c, FILE *stream);

int usart_getchar (FILE *stream);
FILE usart_str = FDEV_SETUP_STREAM(usart_putchar, usart_getchar, _FDEV_SETUP_RW);





int main(void)
{
    

	initialize();
	while(1)//loop demos
	{
	   ch = 0;
	   ADMUX=(ADMUX&0xE0)|ch;
       ADCSRA |= (1<<ADSC)|(1<<ADIE);	   
	   _delay_ms(1);
	   
	   
	   fprintf("The frequence is %d\n", x_SW);
	   //fprintf(the freq is ...)
	   //or for (n=0;n<19;n++) usart+transimit(msg1{n},.......
	   ch = 1;
	   ADMUX=(ADMUX&0xE0)|ch;
       ADCSRA |= (1<<ADSC)|(1<<ADIE);
	   _delay_ms(1);
	   
	   
	   fprintf("The Volume is %d\n", Av);
	   //ch1 volume 
	    //fprintf(the vuloume is ...)  prefer this one
	    
	   //or convert x_SW to ascii -- itoa(x_SW, val, 10)
	   //for (n=0;n<19;n++) usart+transimit(msg1{n},.......
	   
	   //continue infinitely
	}
	return 0;
}


void initialize(){     //missing this part
    // initialize ports
	DDRB = 0xFF; 						// LEDs
	PORTB = 0xFF;
	DDRD |= (1<<PD5); 					// set OC1A(PD5) as output

	// initialize ADC and interrupts
	// select reference voltage and ADC0
	ADMUX &= 0xE0; 						// set lower 5 bits to 0
	ADMUX |= (1<<REFS1)|(1<<REFS0); 	// internal 2.56 V reference
	ADMUX |= (1<<ADLAR); 				// set left-adjust

	// enable ADC and ADC interupt
	ADCSRA |= (1<<ADEN)|(1<<ADIE);

	// Timer 1 Initialization
	TIMSK |= (1<<TOIE1); 				// T1 OVF interrupt enable
	TCCR1A |= (1<<COM1A1)|(1<<WGM10); 	// non-inverting, 8-bit
	TCCR1B |= (1<<CS10); 				// clk/1

	// initialize baud rate to 9600 bps
	UBRRH = 0;
	UBRRL = 23;

	// set 8 data bits, no parity, 1 stop bit (8-N-1)
	UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);

	// enable receiver and transmitter
	UCSRB |= (1<<RXEN)|(1<<TXEN);

	// enable global interrupt
	sei();
    
}




//ISR for ADC
ISR(ADC_vect){
    if (ch == 1)
        Av = ADCH>>4; // to make the change smooth by shifting 4 bits    for freq
    if (ch == 0)   // for value
        x_SW = (ADCH>>1) + 30;  //need some tweak    30-157
    
}




//**************************************************************************
// Timer overflow interrupt service routine
//**************************************************************************
ISR(TIMER1_OVF_vect)
{ 
    
    unsigned char fout;
    
    
    
    
  // move Pointer about step width aheaed
  i_CurSinVal += x_SW; 
  i_TmpSinVal = (char)(((i_CurSinVal+2)>>2)&0x007F);
  
  fout = (unsigned char)(pgm_read_byte(&auc_SinParam[i_TmpSinVal])); //freq
  
  
  fout *= (unsigned int)Av;
  
  
  
  // Set OCR1A
  OCR1A = (unsigned char)(fout>>8);
}





int usart_putchar (char c, FILE *stream)
{
   if (c=='\n')
      usart_putchar('\r',stream);
   while (!(UCSRA & (1<<UDRE)));
   UDR = c;
   return 0;
}

int usart_getchar (FILE *stream)
{
	if (UCSRA & (1<<RXC))
		return UDR;
	else {
		return 0;
	}
}
