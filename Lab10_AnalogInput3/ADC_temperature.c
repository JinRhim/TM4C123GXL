#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/timer.h"


//*****************************************************************************
//
//!
//! In this project we use ADC0, SS1 to measure the data from the on-chip 
//! temperature sensor. The ADC sampling is triggered by software whenever 
//! four samples have been collected. Both the Celsius and the Fahreheit 
//! temperatures are calcuated.
//  
//
// Task1 --> Modify ADC0, SS1 to ADC0, SS3 
//*****************************************************************************

uint32_t ui32ADC0Value[1];
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;

//ADC0 initializaiton
void ADC0_Init(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // configure the system clock to be 40MHz
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
	
	  //ADC0 SS1 Step 0, sample from internal temperature sensor
		ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS); 
		
		//ADC0 SS1 Step 0, sample from internal temperature sensor, completion of this step will set RIS, last sample of the sequence
		
		//???????????????????
		//What does this code do??
		
		//ADCSequenceStepConfigure(A, B, C, D) 
		//A = base address of ADC module 
		//B = sample sequence number 
		//C = step to be confugured 
		//D = Configuration of this step. Logical OR ( ADC_CTL_TS, ADC_CTL_IE, ADC_CTL_END, _ADC_CTL_D)
		// (ADC_CTL_CH0 ~ ADC_CTL_CH23)(ADC_CTL_CMP0 ~ ADC_CTL_CMP7);
		ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0| ADC_CTL_IE |
	                             ADC_CTL_END);

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
void ADC0_Handler(void)
{
		ADCIntClear(ADC0_BASE, 3);
		ADCProcessorTrigger(ADC0_BASE, 3);   //What does it do --> start the reading.
		
	  ADCSequenceDataGet(ADC0_BASE, 3, ui32ADC0Value);
    
		//ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		
	  //SS1 = can store 4 samples 
		//SS3 = can store 1 samples
		
		ui32TempAvg = *ui32ADC0Value;
	
		ui32TempValueC = (1475 - ((2475 * *ui32ADC0Value)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
		//cannot evaluate warning
	
}

void PE_Init() {
	
		volatile uint32_t ui32Loop;

		SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOE;

		ui32Loop = SYSCTL_RCGC2_R;

		GPIO_PORTE_LOCK_R = 0x4C4F434B;
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
}



//This is for Timer ==================================================


//Globally enable interrupts 
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

//interrupt handler for Timer0A
void Timer0A_Handler(void)
{
		// acknowledge flag for Timer0A
		TIMER0_ICR_R |= 0x00000001; 
	
		// Toggle the blue LED.
    ADCProcessorTrigger(ADC0_BASE, 3);

}

//End of timer ========================================================

int main(void)
{
		unsigned long period = 5333333*1/1000; //This will count for 1000Hz
		
		Timer0A_Init(period);
	
		PE_Init();
		ADC0_Init();
		IntMasterEnable();       		// globally enable interrupt
	
	  //ADCProecssorTrigger(A,B) 
    // A --> base address of ADC module 
	  // B --> sample sequence number
		//ADCProcessorTrigger(ADC0_BASE, 3);
	
		while(1)
		{
			
		}
}
