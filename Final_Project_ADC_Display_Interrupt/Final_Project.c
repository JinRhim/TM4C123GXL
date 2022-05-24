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



//=====================================================================
// Variable Initialization 
//=====================================================================

volatile uint8_t digit_selector_line;

char digit_char[128];  
//character array.

char msg_array[8];
//msg Array. Store ASCII table value for D[7:0]
//D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 
char str_buffer[8];


volatile char charValue;

char msg_i = 0; 
//this is index of msg. it is always 0 to 7.

volatile uint32_t debugger = 0;

int msg_count = 0;

volatile int counter = 0;

uint32_t ui32ADC0Value[1];
volatile uint32_t ui32RawValue;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;


volatile uint32_t HR_Buff[100];  // 2^4 = 16 
volatile int HR_Buff_Index = 0; 
volatile int sum = 0; 
volatile uint32_t ten_seconds_avg = 0;
volatile int HeartRate = 0;



unsigned long display_refresh_period = 10000000/2000; //Screen Refresh Rate 
unsigned long ADC_update_period = 10000000/10; //Timer 1A delay for ADC update. 10 Hz.
unsigned long initial_load_message_period = 10000000*1; //Initial message refresh

//=====================================================================
// Port_ABEF, Timer012A, ADC0 Initialization 
//=====================================================================

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
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC); 		// configure for 32-bit timer mode
  TimerLoadSet(TIMER1_BASE, TIMER_A, period -1);      //reload value
	IntPrioritySet(INT_TIMER1A, 0x00);  	 // configure Timer0A interrupt priority as 0
  IntEnable(INT_TIMER1A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
  TimerEnable(TIMER1_BASE, TIMER_A);      // enable timer0A
}

void Timer2A_Init()
{   
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  TimerConfigure(TIMER2_BASE, TIMER_CFG_ONE_SHOT); 		// configure for 32-bit timer mode
	IntPrioritySet(INT_TIMER2A, 0x02);  	 // configure Timer1A interrupt priority as 0
  IntEnable(INT_TIMER2A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
}

//ADC0 initializaiton
void ADC0_Init(void)
{
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);	//activate the clock of ADC0
		SysCtlDelay(2);	//insert a few cycles after enabling the peripheral to allow the clock to be fully activated.
		//ADCSequenceDiable (base address ADC module, Sample Sequence number) 
		//prevent specified sample sequence from being captured when its trigger is detected.
	
		//ADCSequenceDisable(ADC0_BASE, 1); //disable ADC0 before the configuration is complete
		ADCSequenceDisable(ADC0_BASE,3);
		//ADCSequenceConfigure(ADC module base address, sample sequence number, trigger source, priority of sample sequence)
		
		//ADCSequenceConfigure(A, B, C, D);
		//A = adc module address 
		//B = Sequence Num : Sample Sequence Number 
		//C = Trigger Source 
		//D = priority 
	
		//ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0); // will use ADC0, SS1, processor-trigger, priority 0
	  ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	
		//ADC_CTL_TS --> Temperature sensor input.
		//ADC_CTL_CH0 --> external PE3 input 
	
		//ADCSequenceStepConfigure(A, B, C, D) 
		//A = base address of ADC module 
		//B = sample sequence number 
		//C = step to be confugured 
		//D = Configuration of this step. Logical OR ( ADC_CTL_TS, ADC_CTL_IE, ADC_CTL_END, _ADC_CTL_D)
		// (ADC_CTL_CH0 ~ ADC_CTL_CH23)(ADC_CTL_CMP0 ~ ADC_CTL_CMP7);
		ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

		//IntPrioritySet (ui32interrupt, ui8priority) 
		// set priority of an interrupt 
		// ui32interrupt --> specifies interrupt in question
		// ui8priority --> specifies the priority of the interrupt. 
		
		IntPrioritySet(INT_ADC0SS3, 0x00);  	 // configure ADC0 SS1 interrupt priority as 0
		
		//IntEnable --> enable interrupt 
		// Int_ADC0SS1 --> specifies the interrupt to be enabled.
		IntEnable(INT_ADC0SS3);    				// enable interrupt 31 in NVIC (ADC0 SS1)
		
		ADCIntEnableEx(ADC0_BASE, ADC_INT_SS3);      // arm interrupt of ADC0 SS3
	
		//Enables a sample sequence. 
		//ADCSequenceEnable(A, B) 
		// A --> ADC base address 
		// B --> sample sequence number.
		ADCSequenceEnable(ADC0_BASE, 3); //enable ADC0
}
		
//interrupt handler




//interrupt handler for Timer0A
void Timer0A_Handler(void)
{
	 //unsigned long period = 10000000/400; //Screen Refresh Rate 
		// acknowledge flag for Timer0A
		TIMER0_ICR_R |= 0x00000001; 
	 
		counter++;
	
		// Port D, PortA - digit selector 
    digit_selector_line = 0x80 >> msg_i;  //if index = 1 --> 10 
		//digit = 0b1000.0000  ==> 0x80
		//1000.0000
	  //0100.0000
	  //0010.0000
	  //0001.0000
	  //0000.1000
	  //0000.0100 
	  //0000.0010 
	  //0000.0001
	
		// [PA7 PA6 PA5 PA4] [PF3 PF2 PF1 PF0]
    GPIO_PORTF_DATA_R = digit_selector_line & 0x0F;
		GPIO_PORTF_DIR_R = 0x0F;

		GPIO_PORTA_DATA_R = digit_selector_line & 0xF0;
		GPIO_PORTA_DIR_R = 0xF0;

		//0abc.defg --> 0 will be ignored. as digit_char[] = 7 digit binary val.
		GPIO_PORTB_DATA_R = ~digit_char[msg_array[msg_i]];
		msg_i++; 
		msg_i %=8; //the value is always between 0 to 7.
}

void Timer1A_Handler(void) {
	// strcpy(msg_array, HeartRate); - does not work for pointer
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	
	
	//ADC Value not updating
	ADCProcessorTrigger(ADC0_BASE, 3); 
	
	int arr[] = {62, 80, 3, 6}; // ASCII char array for bP__. 
	
	msg_array[0] = arr[0]; //b
	msg_array[1] = arr[1]; //P
	msg_array[2] = arr[2]; //_
	msg_array[3] = arr[2]; //_

	volatile char HR_digit[4] = {0};   //initial value is null. 
	
	sprintf(HR_digit, "%d", HeartRate); //sprintf() --> change string to int 
	
	msg_array[4] = HR_digit[0];   	//first digit of heart rate
	msg_array[5] = HR_digit[1];  		//second digit of heart rate
	msg_array[6] = HR_digit[2];     //null 
	msg_array[7] = HR_digit[3]; 		//null
	}
	
void Timer2A_Handler(void)
{
		//Remove Flag
		TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	  
		//msg_array will copy 8 digit char. Display will display
		//ASCII values copied to the msg_array[7:0]
	 if (msg_count == 0) {
		 strcpy(msg_array, "HELL0   ");
	 }
	 else if (msg_count == 1) {
		 strcpy(msg_array, "  UU0r1d");
	 }
	 else if (msg_count == 2) {
		 strcpy(msg_array, "L0AdPr0J");
	 } 
	 else if (msg_count == 3) {
		 strcpy(msg_array, "EnGr478 ");
	 }
	 else if (msg_count == 4) {
		 strcpy(msg_array, "HrSEnS0r");
	 }
	 else {
	 } 
	 msg_count++;
	 
	 if (msg_count < 6) {
		 TimerLoadSet(TIMER2_BASE, TIMER_A, initial_load_message_period); //Message Refresh Rate
		 TimerEnable(TIMER2_BASE, TIMER_A); //automatic counter will start
	 }
	 else {
		 Timer1A_Init(ADC_update_period);
	 }
}
void ADC0_Handler(void)
{
		ADCIntClear(ADC0_BASE, 3);

	  ADCSequenceDataGet(ADC0_BASE, 3, ui32ADC0Value);
  
		
		ui32RawValue = (10 * *ui32ADC0Value);
	

		HR_Buff[HR_Buff_Index] = ui32RawValue; 
		sum += ui32RawValue; 
		HR_Buff_Index = (HR_Buff_Index + 1) % 100;  
		//The value stays between 0 to 100
		
	
		ten_seconds_avg = sum / 100; 
			if (HR_Buff_Index == 99) {
				HeartRate = 0;
				for (int i = 1; i < 99; i++) {
					if (HR_Buff[i] > ten_seconds_avg && HR_Buff[i-1] < HR_Buff[i] && HR_Buff[i] > HR_Buff[(i+1)]) {
						HeartRate++;
				}
			}			
		}	
	
	   //cannot evaluate warning
}






int main(void)
{
	//Timer 0A - update Display 
	//Timer 1A - ADC refresh rate 10Hz - 10 measurement / sec.
	//Timer 2A - Initial Display String update rate 
	
	
	SysCtlClockSet(SYSCTL_SYSDIV_20 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    //Clock = 400/2/20 --> 10 MHz

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
		digit_char[102] = 0x47; //0b0100111; //F 
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
		digit_char[0] = 0x40; //0b1000000; // seg a 
		digit_char[1] = 0x20; //0b0100000; // seg b 
		digit_char[2] = 0x10; //0b0010000; // seg c 
		digit_char[3] = 0x08; // 0b0001000; // seg d
		digit_char[4] = 0x04; //0b0000100; // seg e 
		digit_char[5] = 0x02; //0b0000010; // seg f
		digit_char[6] = 0x01; //0b0000001; // seg g
		
		Port_Init();
		ADC0_Init();      		

		Timer0A_Init(display_refresh_period); 
		//Timer0A: display refresh rate 
		//Timer1A: ADC refresh rate 
		Timer2A_Init(); //Timer2A: Initial Message Refresh Rate + Increase K
		ADC0_Init(); 
		IntGlobalEnable();
		
		TimerLoadSet(TIMER2_BASE, TIMER_A, initial_load_message_period); //Hello Message Refresh Rate
		TimerEnable(TIMER2_BASE, TIMER_A); //automatic counter will start
	
		//Beginning of Program 
	
		while(1)
		{ 
			
		}
}


