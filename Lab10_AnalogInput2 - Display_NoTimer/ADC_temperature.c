#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include <stdio.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"





#define TO_STRING(val) #val



volatile uint8_t digit_selector_line;

char digit_char[128];  
//character array. digit_A = 1110111; 

char msg_array[8];
//msg Array. Store ASCII table value for D[7:0]
//D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 

char msg_i = 0; 
//this is index of msg. it is always 0 to 7.

uint32_t ui32ADC0Value[1];

volatile uint32_t ui32RawValue;



volatile uint32_t counter = 0;

//ADC0 initializaiton
void ADC0_Init(void)
{
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);	//activate the clock of ADC0
	
		ADCSequenceDisable(ADC0_BASE,3);
		
	  	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	
		ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

		//IntPrioritySet (ui32interrupt, ui8priority) 
		// set priority of an interrupt 
		// ui32interrupt --> specifies interrupt in question
		// ui8priority --> specifies the priority of the interrupt. 
		
		IntPrioritySet(INT_ADC0SS3, 0x00);  	 // configure ADC0 SS1 interrupt priority as 0

		IntEnable(INT_ADC0SS3);    				// enable interrupt 31 in NVIC (ADC0 SS1)
		
		ADCIntEnableEx(ADC0_BASE, ADC_INT_SS3);      // arm interrupt of ADC0 SS3

		ADCSequenceEnable(ADC0_BASE, 3); //enable ADC0
}
		
//interrupt handler
void ADC0_Handler(void)
{
		ADCIntClear(ADC0_BASE, 3);
	  	ADCSequenceDataGet(ADC0_BASE, 3, ui32ADC0Value);
   
		ui32RawValue = (1475 - ((2475 * ui32ADC0Value[0])) / 4096)/10; 
	
		//cannot evaluate warning
}

void Port_Init() {
		volatile uint32_t ui32Loop;

		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;  
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; 
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; 
	
		ui32Loop = SYSCTL_RCGC2_R;
   
		//using PE3 for the sensor reading
		
		//0000.1000
	  	//SET DIR as 0 for input. 
		GPIO_PORTE_DIR_R &= ~0x08;       

		//Disable digital function --> DISABLE DIGITAL
		GPIO_PORTE_DEN_R &= ~0x08;

		//Enable Alternate function for PE3
		GPIO_PORTE_AFSEL_R |= 0x08; 
		
		//Enable analog function 
		GPIO_PORTE_AMSEL_R |= 0x08;

		GPIO_PORTF_LOCK_R = 0x4C4F434B;
		
		//allow changes
		GPIO_PORTA_CR_R |= 0xF0; 
		GPIO_PORTB_CR_R |= 0x7F; 
		GPIO_PORTF_CR_R |= 0x0F; 
		
		//set direction 
		GPIO_PORTA_DIR_R |= 0xF0; //output Digit 7654
		GPIO_PORTB_DIR_R |= 0x7F; //Input segment  0111.1111
		GPIO_PORTF_DIR_R |= 0x0F; //output digit 3210
		
		//allow digital.
		GPIO_PORTA_DEN_R |= 0xF0; 
		GPIO_PORTB_DEN_R |= 0x7F; 
		GPIO_PORTF_DEN_R |= 0x0F; 

}

void IntGlobalEnable(void)
{
    __asm("    cpsie   i\n");
}

void Timer0A_Init(unsigned long period)
{   
	volatile uint32_t ui32Loop; 
	
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0; // activate timer0
  	ui32Loop = SYSCTL_RCGC1_R;				// Do a dummy read to insert a few cycles after enabling the peripheral.
  	TIMER0_CTL_R &= ~0x00000001;     // disable timer0A during setup
  	TIMER0_CFG_R = 0x00000000;       // configure for 32-bit timer mode
  	TIMER0_TAMR_R = 0x00000002;      // configure for periodic mode, default down-count settings
  	TIMER0_TAILR_R = period-1;       // reload value
	NVIC_PRI4_R &= ~0xE0000000; 	 // configure Timer0A interrupt priority as 0
  	NVIC_EN0_R |= 0x00080000;     // enable interrupt 19 in NVIC (Timer0A)
	TIMER0_IMR_R |= 0x00000001;      // arm timeout interrupt
  	TIMER0_CTL_R |= 0x00000001;      // enable timer0A
}

void Timer1A_Init(unsigned long period)
{   
	volatile uint32_t ui32Loop; 
	
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER1; // activate timer0
  	ui32Loop = SYSCTL_RCGC1_R;				// Do a dummy read to insert a few cycles after enabling the peripheral.
  	TIMER1_CTL_R &= ~0x00000001;     // disable timer0A during setup
	//TAEN = 0  means that Timer A is disabled. 
  	TIMER1_CFG_R = 0x00000000;       // configure for 32-bit timer mode
  	//Wrote a value of 0x0000.0000 for one-shot/Periodic Timer mode 
	TIMER1_TAMR_R = 0x00000002;      // configure for periodic mode, defaultdown-count settings
  	//GPTMTAMR Register = 0x02 --> Periodic Timer Mode
	TIMER1_TAILR_R = period-1;       // reload value
	NVIC_PRI5_R &= ~0x00E00000; 	 // configure Timer0A interrupt priority as 0
	
	//NVIC_PRI5_R IRQ = 21 
	//0000.0000.1110.0000.0000.0000.0000.0000 = 0x00E00000;
	
	NVIC_EN0_R |= 0x0020000;     // enable interrupt 21 in NVIC (Timer0A)
	//0000.0000.0010.0000.00000.0000.0000.0000 = 0x00200000;
	
	TIMER1_IMR_R |= 0x00000001;      // arm timeout interrupt
  TIMER1_CTL_R |= 0x00000001;      // enable timer0A
	
}


//interrupt handler for Timer0A
void Timer0A_Handler(void)
{
		// acknowledge flag for Timer0A
		TIMER0_ICR_R |= 0x00000001; 
	 
		counter++;
	
		// Port D, PortA - digit selector 
    		digit_selector_line = 0x80 >> msg_i;  //if index = 1 --> 10 
		
		//digit = 0b1000.0000  ==> 0x80
	  
	
		// PA7654.0000
		// PD0000.3210
		

		// [PA7 PA6 PA5 PA4] [PF3 PF2 PF1 PF0]
	
    		GPIO_PORTF_DATA_R = digit_selector_line & 0x0F;
		GPIO_PORTF_DIR_R = 0x0F;

	
		GPIO_PORTA_DATA_R = digit_selector_line & 0xF0;

		GPIO_PORTA_DIR_R = 0xF0;


	
		//0abc.defg --> 0 will be ignored. as digit_char[] = 7 digit binary val.
		GPIO_PORTB_DATA_R = ~digit_char[msg_array[msg_i]];
		msg_i++; 
		msg_i %=8; //the value is always between 0 to 7.
		if (counter == 9999) {
			ADCProcessorTrigger(ADC0_BASE, 3);
			strcpy(msg_array, TO_STRING(ui32RawValue));
		}
		counter %= 10000;
		
}

void Timer1A_Handler(void) {
	TIMER1_ICR_R |= 0x00000001; 
	strcpy(msg_array, TO_STRING(ui32RawValue));
}


int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_20 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    //Clock = 400/2/20 --> 10 MHz
		
		unsigned long period = 10000000/1000; //Display Refresh Rate
	
		unsigned long period2 = 10000000; //Tiemr 1A: update ADC value to display.
	
	
	/* 
		 a //0abc.defg 
	  -----
	f|  g  | b
	  -----
	e|     | c 
	  -----
	  d
	
	*/ 
		digit_char[32] = 0x00; //this is space
		digit_char[48] = 0x7E; //0
		digit_char[49] = 0x30; //1
		digit_char[50] = 0x6D; //2
		digit_char[51] = 0x79; //3
		digit_char[52] = 0x33; //4
		digit_char[53] = 0x5B; //5
		digit_char[54] = 0x5F; //6
		digit_char[55] = 0x70; // 0b1110000; //7
		digit_char[56] = 0x7F; // 0b1111111; //8
		digit_char[57] = 0x7B; // 0b1110011; //9
		digit_char[65] = 0x77; //0b1110111; //A 
		digit_char[97] = 0x7D; //0b1111101; //a
		digit_char[62] = 0x1F; // 0b0011111; //b
		digit_char[67] = 0x5E; // 0b1001110; //C 
		digit_char[99] = 0x0D; //0b0001101; //c 
		digit_char[100] = 0x3D; //0b0111101; //d 
		digit_char[69] = 0x4F; //0b1001111; //E
		digit_char[102] = 0x47; //0b1000111; //F 
		digit_char[71] = 0x5E; //0b1011110; //G 
		digit_char[72] = 0x37; //0b0110111; //H 
		digit_char[105] = 0x06; //0b0000110; //i 
		digit_char[74] = 0x3C; //0b0111100; //J
		digit_char[76] = 0x0E; //0b0001110; //L 
		digit_char[110] = 0x15; //0b0010101; //n 
		digit_char[111] = 0x1D; //0b0011101; //o 
		digit_char[80] = 0x67; //0b1100111; //P
		digit_char[113] = 0x73; //0b1110011; //q 
		digit_char[114] = 0x05; // 0b0000101; //r 
		digit_char[83] = 0x5B; //0b1011011; //S 
		digit_char[85] = 0x3E; //0b0111110; //U
		digit_char[89] = 0x3B; //0b0111011; //Y

		digit_char[1] = 0x40; //0b1000000; // seg a 
		digit_char[2] = 0x20; //0b0100000; // seg b 
		digit_char[3] = 0x10; //0b0010000; // seg c 
		digit_char[4] = 0x08; // 0b0001000; // seg d
		digit_char[5] = 0x04; //0b0000100; // seg e 
		digit_char[6] = 0x02; //0b0000010; // seg f
		digit_char[7] = 0x01; //0b0000001; // seg g
		
		
	
		Port_Init();
		ADC0_Init();      		


		Timer0A_Init(period); //100Hz display refresh rate 
		//Timer1A_Init(period2);
		IntGlobalEnable();
		
		//strcpy(msg_array, "01234567");
		//strcpy(msg_array, "HELL0   ");
		
		//strcpy(msg_array, "L0Ad1234");
		
	
		while(1)
		{
			
		}
}
