// Serial Port Programming
// Port B: LEDs, Port C: Switches
// Kie Eom
// 1/31/2018
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
   unsigned char x,x_old=0xFF;

   // initialize Port B as output
   DDRB = 0xFF;
   PORTB = 0xFF;  // turn-off LEDs
   // initialize Port A as input
   DDRA = 0x00;
   PORTA = 0xFF;  // pull-up port A

   // initialize baudrate to 9600 bps
   UBRRH = 0;
   UBRRL = 23;

   // set 8 data bits, no parity, 1 stop bit (8-N-1)
   UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);

   // Enable receiver and transmitter
   UCSRB |= (1<<RXEN)|(1<<TXEN);

   while (1)
   {
       // Receive data and display to LED
       if (UCSRA & (1<<RXC))  // test if receive is complete
       {
          x = UDR;  // read receive byte
          PORTB = ~x;   // display to LEDs
           // send the data back to PC
          if (UCSRA &(1<<UDRE))   // if buffer is empty
             UDR = x;   // send it to PC        
       }

      // Read switches, debounce and send ASCII code
      x = PINA; // read switches
      if ((x!=x_old) && (x!=0xFF)) // test if a switch is pressed
      {
         _delay_ms(10);
			x = PINA; // read switches
        if ((x!=x_old) && (x!=0xFF)) // test if a switch is pressed
        {
           while (!(UCSRA&(1<<UDRE)));  // wait until UDR become empty
           UDR = ~x+0x2F;   // convert to ASCII and sent to PC
           PORTB = x;  // turn on LEDs
        }
      }
    x_old = x;
	
    }
   return 0;
}
