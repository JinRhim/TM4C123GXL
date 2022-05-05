#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"


/*====================================
ENGR 478 Coding Exam Sample 2. 

Output: PA3 negative logic LED 

Periodic Interrupt - Timer2A every half seconds. 

System Freaquency - 10 MHz. 

=====================================*/


void Port_Init(void) {
	
	//Dynamically update the register value 
	volatile uint32_t ui32Loop; 
	
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA; 
	
	GPIO_PORTA_LOCK_R = 0x4C4F434B; 
	
	GPIO_PORTA_CR_R |= 0x08; 
	GPIO_PORTA_DIR_R |= 0x08; 
	GPIO_PORTA_PUR_R |= 0x08; 
	GPIO_PORTA_AMSEL_R &= ~0x08; //no alaog 
	GPIO_PORTA_AFSEL_R &= ~0x08; //no alternate func
	
}

//This is part for Timer Periodic Interrupt =====================================


//Globally enable interrupts 
void IntGlobalEnable(void)
{
    __asm("    cpsie   i\n");
}

void Timer2A_Init(unsigned long period)
{   
	volatile uint32_t ui32Loop; 
	
	//Questions.
	//RCGC1 vs RCGC2 
	//What is CTL_R 
	//What is CFG_R 
	
	
	//RCGC = Run Mode Clock Control.
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER2; // activate timer0
  ui32Loop = SYSCTL_RCGC1_R;				// Do a dummy read to insert a few cycles after enabling the peripheral.
  TIMER2_CTL_R &= ~0x00000001;     // disable timer0A during setup
  TIMER2_CFG_R = 0x00000000;       // configure for 32-bit timer mode
  TIMER2_TAMR_R = 0x00000002;      // configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;       // reload value
	NVIC_PRI5_R &= ~0xE0000000; 	 // configure Timer0A interrupt priority as 0
  NVIC_EN0_R |= 0x00080000;     // enable interrupt 19 in NVIC (Timer0A)
	TIMER2_IMR_R |= 0x00000001;      // arm timeout interrupt
  TIMER2_CTL_R |= 0x00000001;      // enable timer0A
  //0000.0000.1000.0000.0000.0000.0000.0000


}

//interrupt handler for Timer0A
void Timer2A_Handler(void)
{
		// acknowledge flag for Timer0A
		TIMER2_ICR_R |= 0x00000001; 
		
		// Toggle the blue LED.
    
		GPIO_PORTA_DATA_R ^= 0x08; 

}


int main(void) {
	
	
	//default clock = 10 mhz. 
	unsigned long period = 10000000/2; 
	
	
	SysCtlClockSet(SYSCTL_SYSDIV_20 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	
	Port_Init(); 
	
	GPIO_PORTA_DATA_R &= ~0x08; //Turn on LED when system restarts; negative logic
	
	Timer2A_Init(period); 
	
	IntGlobalEnable();
	
	//Clock = 400/2/20 = 10 MHz
	
	while(1) 
	{ 

		
	}
	
	
	
}



