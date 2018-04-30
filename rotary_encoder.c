// Interfacing mechanical rotary encoder
#include <avr/io.h>
#include <avr/interrupt.h>
#define A 0x01   // PA0 for channel A
#define B 0x02   // PA1 for channel B
#define AB 0x03   // mask for optical encoder


volatile unsigned char opt1, opt2;
volatile unsigned char x = 128;


int main(void)
{
   // Initialize LED port
   DDRB = 0xFF;
   PORTB = 0xFF;
   // Initialize port A (rotary encoder)
   DDRA = 0X00;
   PORTA = 0xFF;

   // Timer Initialization  
   TCCR0 |= (1<<CS01)|(1<<CS00);  // prescaler = 64
   TIMSK |= (1<<TOIE0);  // enable Timer 0 overflow interrupt
   sei();   // enable global interrupt

   while (1)
   {	
	PORTB = ~x;
   }
   return 0;
}


ISR(TIMER0_OVF_vect)
{
   opt2 = PINA&AB;
   if (opt2!=opt1) // if there is change
   {
	if ((opt1==0x00) && (opt2==0x01)) x++;
	if ((opt1==0x01) && (opt2==0x00)) x--;
	if ((opt1==0x01) && (opt2==0x03)) x++;
	if ((opt1==0x03) && (opt2==0x01)) x--;
	if ((opt1==0x03) && (opt2==0x02)) x++;
	if ((opt1==0x02) && (opt2==0x03)) x--;
	if ((opt1==0x02) && (opt2==0x00)) x++;
	if ((opt1==0x00) && (opt2==0x02)) x--;        
   }
   opt1 = opt2;
}
