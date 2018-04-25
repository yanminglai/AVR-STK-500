// Sinewave generator 
// Frequency is selected by PB switches (PortA)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define  Xtal       3686400          // system clock frequency
#define  prescaler  1                // timer1 prescaler
#define  N_samples  128              // Number of samples in lookup table
#define  Fck        Xtal/prescaler   // Timer1 working frequency

//************************** SIN TABLE *************************************
// Samples table : one period sampled on 128 samples and
// quantized on 7 bit
//**************************************************************************
const unsigned char auc_SinParam [128] PROGMEM = {
64,67,
70,73,
76,79,
82,85,
88,91,
94,96,
99,102,
104,106,
109,111,
113,115,
117,118,
120,121,
123,124,
125,126,
126,127,
127,127,
127,127,
127,127,
126,126,
125,124,
123,121,
120,118,
117,115,
113,111,
109,106,
104,102,
99,96,
94,91,
88,85,
82,79,
76,73,
70,67,
64,60,
57,54,
51,48,
45,42,
39,36,
33,31,
28,25,
23,21,
18,16,
14,12,
10,9,
7,6,
4,3,
2,1,
1,0,
0,0,
0,0,
0,0,
1,1,
2,3,
4,6,
7,9,
10,12,
14,16,
18,21,
23,25,
28,31,
33,36,
39,42,
45,48,
51,54,
57,60};

//***************************  x_SW  ***************************************
//Table of x_SW (excess 4): x_SW = ROUND(4*N_samples*f*510/Fck)
//**************************************************************************

// Frequency and X_SW values
// X_SW = f/14.12 for 3.6864 MHz Fclk
// C5 = 523		37
// D5 = 587		42
// E5 = 659		47
// F5 = 698		50
// G5 = 784		56
// A5 = 880		62
// B5 = 988		70
// C6 = 1047	74

const unsigned char auc_frequency [8] = {
37, 42, 47, 50, 56, 62, 70, 74};


//**************************  global variables  ****************************
volatile unsigned char x_SW = 0x00;               // step width of frequency
unsigned int  i_CurSinVal = 0;           // position freq. in LUT (extended format)
unsigned int  i_TmpSinVal;

// Function prototype
unsigned char decode(unsigned char x);

//**************************************************************************
// Timer overflow interrupt service routine
//**************************************************************************
ISR(TIMER1_OVF_vect)
{ 
  // move Pointer about step width aheaed
  i_CurSinVal += x_SW; 
  i_TmpSinVal = (char)(((i_CurSinVal+2)>>2)&0x007F);
  // Set OCR1A
  OCR1A = pgm_read_byte(&auc_SinParam[i_TmpSinVal]);
}

//**************************************************************************
// Initialization
//**************************************************************************
void init (void)
{
  TIMSK  = (1<<TOIE1);               // Int T1 Overflow enabled
  TCCR1A = (1<<COM1A1)|(1<<WGM10);   // non inverting / 8Bit PWM
  TCCR1B = (1<<CS10);                // CLK/1
//  DDRD = 0xFF;
  DDRD   = (1 << PD5);               // PD5 (OC1A) as output
  PORTD = 0xFF;                      // Pull-up inputs
  DDRA = 0x00;  
  PORTA = 0xFF;                      // Pull-up inputs 
  sei();                     	     // Interrupts enabled
}


//**************************************************************************
// MAIN
//**************************************************************************

int main (void)
{
  unsigned char x=0;
	
  init();
  while (1) 
  { 
	  x = decode(PINA);
	  if (x>0) { 
		  TCCR1A |= (1<<COM1A1);
		  x_SW = auc_frequency[x-1];
	  }
	  else TCCR1A &= ~(1<<COM1A1);
//	  _delay_ms(100);
  } 
  return 0;
}

unsigned char decode(unsigned char x)
{
	unsigned char y;
	switch (x) {
		case 0xFE:
			y = 1;
			break;
		case 0xFD:
			y = 2;
			break;
		case 0xFB:
			y = 3;
			break;
		case 0xF7:
			y = 4;
			break;
		case 0xEF:
			y = 5;
			break;
		case 0xDF:
			y = 6;
			break;
		case 0xBF:
			y = 7;
			break;
		case 0x7F:
			y = 8;
			break;
		default:
			y = 0;
		}
	return y;
}