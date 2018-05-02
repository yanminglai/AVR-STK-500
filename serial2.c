// Serial Port Programming
// Port B: LEDs, Port A: Switches
// Kie Eom
// 1/31/2018
#include <avr/io.h>
unsigned char msg1[] = "The key pressed is ";  // 19 characters
unsigned char msg2[] = ".\n";  // 2 charcters
unsigned char msg3[] = "The switch pressed is "; // 22 characters

// Function prototypes
int usart_receive (void);
int usart_transmit (char c);
unsigned char sw_decode(unsigned char x);


int main(void)
{
   unsigned char n,c,x,x_old=0xFF;

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
       x = usart_receive();  // read receive byte
       if (x!=0) 
       {
          PORTB = ~x;   // display to LEDs
           for (n=0; n<19; n++)
           {
             usart_transmit (msg1[n]);
           }

             usart_transmit(x);   // send it to PC   
     
           for (n=0; n<2; n++)
           {
             usart_transmit (msg2[n]);  // send the data back to PC
           }           

       }

      // Read switches, debounce and send ASCII code
      x = PINA; // read switches
      if ((x!=x_old) && (x!=0xFF)) // test if a switch is pressed
      {
         _delay_ms(10);
		x = PINA; // read switches
        if ((x!=x_old) && (x!=0xFF)) // test if a switch is pressed
        {
           c = sw_decode(x);  // decode switch 
           for (n=0; n<22; n++)
           {
             usart_transmit (msg3[n]);
           }

           usart_transmit (c+0x2F);   // convert to ASCII and sent to PC
           PORTB = x;  // turn on LEDs
           for (n=0; n<2; n++)
           {
             usart_transmit (msg2[n]);  // send the data back to PC
           }           

        }
      }
    x_old = x;
    }
   return 0;
}

// USART Receive
int usart_receive (void)
{
	if (UCSRA & (1<<RXC))
		return UDR;
	else {
		return 0;
	}
}


// USART transmit
int usart_transmit (char c)
{
	if (c=='\n')
		usart_transmit('\r');
	while (!(UCSRA & (1<<UDRE)));
	UDR = c;
	return 0;
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


