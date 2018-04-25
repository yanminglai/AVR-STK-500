// Serial Port Programming
// Port B: LEDs, Port A: Switches
// Yanming Lai  
// 2/28/2018
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define BAUD_VAL 23
int usart_putchar (char c, FILE *stream);
int usart_getchar (FILE *stream);
unsigned char sw_decode(unsigned char x);
FILE usart_str = FDEV_SETUP_STREAM(usart_putchar, usart_getchar, _FDEV_SETUP_RW);




int main(void)
{
   unsigned char x,x1=0xFF,y;

   stdout = stdin = &usart_str;

   // initialize Port B as output
   DDRB = 0xFF;
   PORTB = 0xFF;  // turn-off LEDs
   // initialize Port C as input
   DDRA = 0x00;
   PORTA = 0xFF;  // pull-up port A

   // initialize baudrate to 9600 bps
   UBRRH = 0;
   UBRRL = BAUD_VAL;

   // set 8 data bits, no parity, 1 stop bit (8-N-1)
   UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);

   // Enable receiver and transmitter
   UCSRB = (1<<RXEN)|(1<<TXEN);
   
   // Enable RXC Interrupt
   UCSRB |=(1<<RXCIE);
   
   // Enable global interrupt
   sei();

   while (1)
   {
     x = PINA;
     if((x!=0xFF) && (x!=x1)){
         _delay_ms(10);
         x = PINA;
         if((x!=0xFF) && (x!=x1)){
             
             y = sw_decode(x);
             printf("Switch '%c' is pressed!\n",y + '0');
             
             
             PORTB = ~x;
         }
         x1 = x;
     }
    }
   return 0;
}

ISR (USART_RXC_vect){
    unsigned char x;
    x = UDR;
    PORTB = ~x; // Display to LEDs
    printf("Key '%c' is pressed!\n", x);
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
   if ((UCSRA & (1<<RXC)))
      return UDR;
   else {
      return 0;
   }
}


// Switch Decode
unsigned char sw_decode(unsigned char x)
{
	unsigned char y;
	switch (x)
	{
		case 0xFE:
			y=1;
			break;
		case 0xFD:
			y=2;
			break;
		case 0xFB:
			y=3;
			break;
		case 0xF7:
			y=4;
			break;
		case 0xEF:
			y=5;
			break;
		case 0xDF:
			y=6;
			break;
		case 0xBF:
			y=7;
			break;
		case 0x7F:
			y=8;
			break;	
		default:
			y = 0;
	}
	return y;
}
