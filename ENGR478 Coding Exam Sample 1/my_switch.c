#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

/*====================================
ENGR 478 Coding Exam Sample 1. 

Input: PD1, PD2 Positive Logic Switch

Output: Negative Logic LED PE4, PE5 

Edge-Triggered Interrupt. 
When Switch is Pressed --> Edge triggered interrupt 
When switch is released --> No interrupt. 

Assume no switch bouncing. 

=====================================*/


		//We are using input Port D 1, Port D 2 
		//We are using output PE4, PE5  
		//PD1 = 0000.00x0 --> 0x02  
		//PD2 = 0000.0x00 --> 0x04
		//PE4 = 000x.0000 --> 0x10 
		//PE5 = 00x0.0000 --> 0x20
	
//0x400253FC + 0x0008

void Port_Init(void) {
	
	//Dynamically update the register value 
	volatile uint32_t ui32Loop; 
	
	//activate the clock of port E 
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOE;  
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOD;  
	
	
	//few dummy loops to initialize loop
	ui32Loop = SYSCTL_RCGC2_R; 
	
	//unlock port password - no need
	GPIO_PORTD_LOCK_R = 0x4C4F434B; 
	GPIO_PORTE_LOCK_R = 0x4C4F434B; 
	
	//this is commit register. Allow change value 
	GPIO_PORTE_CR_R |= 0x30;  //0x0011.0000
	GPIO_PORTD_CR_R |= 0x06;  //0x0011.0000
	
	//Data Direction Register set to 1 for output
	GPIO_PORTE_DIR_R |= 0x30; 
	GPIO_PORTD_DIR_R &= ~0x06; 
	
	
	//Enable pull up register for PD1 and PD2. this will allow definite signal 
	//This is the input switch PD1 and PD2 
	GPIO_PORTE_PUR_R |= 0x30; 
	//This is the output LED.
	
	GPIO_PORTD_PUR_R |= 0x06; 
	
		//turn off analog signal - usually off 
	GPIO_PORTE_AMSEL_R &= ~0x30; 
  GPIO_PORTD_AMSEL_R &= ~0x06; 	
	
		//Disable Alternative Funtion - set to 0. 
	GPIO_PORTE_AFSEL_R &= ~0x30; 
	GPIO_PORTD_AFSEL_R &= ~0x06; 
}

//This is part for Interrupt =====================================

void IntGlobalEnable(void)
{
    __asm("    cpsie   i\n");
}

//Globally disable interrupts 
void IntGlobalDisable(void)
{
    __asm("    cpsid   i\n");
}

//This will allow interrupt from PD1 and PD2. 


void Interrupt_Init(void) {
	//NVIC Port D interrupt - PD1 and PD2 
	//Port D IRQ = 3. 
	//Use NVIC_EN0_R Register. 
	//PortD = 3rd bit. 
	//0b0000.0000.0000.0000.0000.0000.0000.0000.1000 --> 0x0000.0008 
	NVIC_EN0_R |= 0x00000008; 
	
	//Set Priority Register - positive logic switch
	NVIC_PRI0_R &= ~0xE0000000; 
	
	GPIO_PORTD_IM_R |= 0x06;  //ARM interrupt on PD1 and PD2. 
	
	GPIO_PORTD_IS_R &= ~0x06; //0: input. 
	GPIO_PORTD_IBE_R &= ~0x06; //0: one edge trigger 1: both edge trigger
  GPIO_PORTD_IEV_R |= 0x06; //0: falling edge trigger 1: rising edge trigger
	
	IntGlobalEnable(); 
}


void GPIOPortD_Handler(void) {
	
	//IF PD1 signal is positive 
	if (GPIO_PORTD_RIS_R&0x02) {
		GPIO_PORTD_ICR_R |= 0x02;  //Remove Interrupt Flag 
		//Turn on the negative logic LED1 PE4 
		
		GPIO_PORTE_DATA_R &= ~0x10; 
		
	}
	//If PD2 signal is positive
	if (GPIO_PORTD_RIS_R&0x04) {
		GPIO_PORTD_ICR_R |= 0x04; 
		
		//Turn on the negative logic LD2 PE5
		
		GPIO_PORTE_DATA_R &= ~0x20; 
	}
	
	
}



int main(void) {
	Port_Init(); 
	
	Interrupt_Init(); 
	
	
	while(1) 
	{ 
		//PD1 = positive logic sw1 
		//PD2 = positive logic sw2 
		
		//PE4 = negative logic LED 
		//PE5 = negative logic LED 
	}
	
	
	
}



