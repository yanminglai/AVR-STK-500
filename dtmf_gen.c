// DTMF generator 
// Frequency is selected by PB switches at PA.0-3
#define F_CPU 3686400
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
//Table of x_SW (excess 4): x_SW = ROUND(4*N_samples*f*510/Fck) = f/14.12
//**************************************************************************

//high frequency (coloun)
//1209hz  ---> x_SW = 86
//1336hz  ---> x_SW = 95
//1477hz  ---> x_SW = 105
//1633hz  ---> x_SW = 116

const unsigned char auc_frequencyH [4] = {
86,95,105,116};

//low frequency (row)
//697hz  ---> x_SW = 49
//770hz  ---> x_SW = 55
//852hz  ---> x_SW = 60
//941hz  ---> x_SW = 67

const unsigned char auc_frequencyL [4] = {
49,55,60,67};



//**************************  global variables  ****************************
volatile unsigned char x_SWa = 0x00;               // step width of high frequency
volatile unsigned char x_SWb = 0x00;               // step width of low frequency
unsigned int  i_CurSinValA = 0;           // position freq. A in LUT (extended format)
unsigned int  i_CurSinValB = 0;           // position freq. B in LUT (extended format)
unsigned int  i_TmpSinValA;               // position freq. A in LUT (actual position)
unsigned int  i_TmpSinValB;               // position freq. B in LUT (actual position)

unsigned char decode(unsigned char x);

//**************************************************************************
// Timer overflow interrupt service routine
//**************************************************************************
ISR(TIMER1_OVF_vect)
{ 
  // move Pointer about step width aheaed
  i_CurSinValA += x_SWa;       
  i_CurSinValB += x_SWb;
  // normalize Temp-Pointer
  i_TmpSinValA  =  (char)(((i_CurSinValA+2) >> 2)&(0x007F)); 
  i_TmpSinValB  =  (char)(((i_CurSinValB+2) >> 2)&(0x007F));
  // calculate PWM value: high frequency value + 3/4 low frequency value
  OCR1A = pgm_read_byte(&auc_SinParam[i_TmpSinValA]) + pgm_read_byte(&auc_SinParam[i_TmpSinValB])
         - (pgm_read_byte(&auc_SinParam[i_TmpSinValB])>>2);
}

//**************************************************************************
// Initialization
//**************************************************************************
void init (void)
{
  TIMSK  = (1<<TOIE1);               // Int T1 Overflow enabled
  TCCR1A = (1<<COM1A1)|(1<<WGM10);   // non inverting / 8Bit PWM
  TCCR1B = (1<<CS10);                // CLK/1
  DDRD   = (1 << PD5);               // PD5 (OC1A) as output
  PORTD = 0xFF;                      // Pull-up inputs
  sei();                     	     // Interrupts enabled
}


//**************************************************************************
// MAIN
//**************************************************************************

int main (void)
{
  unsigned char x;
  init();
  while (1) 
  { 
//	 x = ~PINA;
	 x = decode(PINA);
	 if (x>0) {
		 x_SWb = auc_frequencyL[x&0x03];
		 x_SWa = auc_frequencyH[(x>>2)&0x03];
		 TCCR1A |= (1<<COM1A1);
	 }
	 else TCCR1A &= ~(1<<COM1A1);
	 _delay_ms(100);
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