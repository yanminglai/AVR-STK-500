// Serial Port Programming
// Port B: LEDs, Port A: Switches
// Kie Eom
// 1/31/2018
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>


unsigned char msg1[] = "The key pressed is ";  // 19 characters
unsigned char msg2[] = ".\r\n";  // 3 charcters
unsigned char msg3[] = "The switch pressed is "; // 22 characters


int main(void)
{
   unsigned char n,x,x_old=0xFF;

   // initialize Port B as output
   DDRB = 0xFF;
   PORTB = 0xFF;  // turn-off LEDs
   // initialize Port C as input
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
           for (n=0; n<19; n++)
           {
             while (!(UCSRA&(1<<UDRE)));  // wait until UDR become empty
             UDR = msg1[n];
           }

             while (!(UCSRA&(1<<UDRE)));  // wait until UDR become empty
             UDR = x;   // send it to PC   
     
           for (n=0; n<3; n++)
           {
             while (!(UCSRA&(1<<UDRE)));  // wait until UDR become empty
             UDR = msg2[n];
           }           // send the data back to PC

       }

      // Read switches, debounce and send ASCII code
      x = PINA; // read switches
      if ((x!=x_old) && (x!=0xFF)) // test if a switch is pressed
      {
         _delay_ms(10);
	x = PINA; // read switches
        if ((x!=x_old) && (x!=0xFF)) // test if a switch is pressed
        {
          for (n=0; n<22; n++)
           {
             while (!(UCSRA&(1<<UDRE)));  // wait until UDR become empty
             UDR = msg3[n];
           }

           while (!(UCSRA&(1<<UDRE)));  // wait until UDR become empty
           UDR = ~x+0x2F;   // convert to ASCII and sent to PC
           PORTB = x;  // turn on LEDs

		    for (n=0; n<3; n++)
           {
             while (!(UCSRA&(1<<UDRE)));  // wait until UDR become empty
             UDR = msg2[n];
           }           // send the data back to PC
        }
      }
    x_old = x;
    }
   return 0;
}