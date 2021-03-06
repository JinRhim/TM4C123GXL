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
// Use ADC0, SS3 to measure external PE3 input for 1000Hz. 
// Use Timer!
//*****************************************************************************

uint32_t ui32ADC0Value[1];  //this is the array for saving external sensor value 
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;

//ADC0 initializaiton
void ADC0_Init(void)
{
		SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); 
	  // configure the system clock to be 40MHz
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);	//activate the clock of ADC0
		ADCSequenceDisable(ADC0_BASE,3);
	    
	
	
	  ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
		// 3= SS3 
		// ADC_TRIGGER_PROCESSOR = trigger source that starts SS
		// 0 = priority 0. 
	
		// ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS);
		//Internal temperature can be selected by ADC_CTL_TS = 1. 
		//ADC_CTL_IE ==> set interrupt when step is complete. 
		//ADC_CTL_END ==> last in the sequence. 
		//ADC_CTL_CH0 will get the reading value from external PE3. 
		ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0| ADC_CTL_IE | ADC_CTL_END);
	
		IntPrioritySet(INT_ADC0SS3, 0x00);  	 
	  // configure ADC0 SS1 interrupt priority as 0
		
		IntEnable(INT_ADC0SS3);    				
		// enable interrupt 31 in NVIC (ADC0 SS1)
		
		ADCIntEnableEx(ADC0_BASE, ADC_INT_SS3);      
		// arm interrupt of ADC0 SS3
	
		ADCSequenceEnable(ADC0_BASE, 3); //enable ADC0
}	 
		
//interrupt handler
void ADC0_Handler(void)
{
		ADCIntClear(ADC0_BASE, 3);
		//Clear interrupt flag.

	  ADCSequenceDataGet(ADC0_BASE, 3, ui32ADC0Value);
	// ui32ADC0Value = buffer where data is stored. 
    
		ui32TempValueC = (1475 - ((2475 * ui32ADC0Value[0])) / 4096)/10; 
	//celsius reading
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
	//Farenheit reading
}

void PE_Init() {
	
		volatile uint32_t ui32Loop;

		SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOE;

		ui32Loop = SYSCTL_RCGC2_R;
   
		 
		//PE3 = 0000.1000 = 0x08.
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
	
		//Every 1/1000 seconds, the timer will trigger ADC SS3. 
    ADCProcessorTrigger(ADC0_BASE, 3);

}

//End of timer ========================================================

int main(void)
{
		unsigned long period = 5333333*1/1000; //This will count for 1000Hz
		
		Timer0A_Init(period); //Every 1/1000 seconds, the ADC0 will be triggered.
		PE_Init();  //enable PE3
		ADC0_Init(); //Initialize ADC0. 
		IntMasterEnable();       		// globally enable interrupt
	
		while(1)
		{
			
		}
}
