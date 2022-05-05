#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/gpio.h"
/*====================================
ENGR 478 Mock Coding Exam. 3/5/2022



Input: positive logic PB5 Switch

Output: negative logic PD1 LED 

If PB5 switch is pressed --> turn on PD1 LED 

If PD1 switch is not pressed --> turn off PD1 LED 

=====================================*/

//PB5 = 0b0010.0000 = 0x20 
//PD1 = 0b0000.0010 = 0x02

void Port_Init(void) {
	
	//Dynamically update the register value 
	volatile uint32_t ui32Loop; 
	
	//activate the clock of port E 
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOB;  
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOD;  
	
	//few dummy loops to initialize loop
	ui32Loop = SYSCTL_RCGC2_R; 
	
	//unlock port password 
	GPIO_PORTB_LOCK_R = 0x4C4F434B; 
	GPIO_PORTD_LOCK_R = 0x4C4F434B; 
	
	//this is commit register. Allow change value 
	GPIO_PORTB_CR_R |= 0x20;  //0x0011.0000
	GPIO_PORTD_CR_R |= 0x02;  //0x0011.0000
	
	//Data Direction Register set to 1 for output or 0 for input
	GPIO_PORTB_DIR_R &= ~0x20; 
	GPIO_PORTD_DIR_R |= 0x02; 
	
	//Enable pull up register for PD1 and PD2. this will allow definite signal 
	GPIO_PORTB_PUR_R |= 0x20; 
	GPIO_PORTD_PUR_R |= 0x02; 
	
	
		//turn off analog signal 
	GPIO_PORTB_AMSEL_R &= ~0x20; 
  GPIO_PORTD_AMSEL_R &= ~0x02; 	
	
		//Disable Alternative Funtion - set to 0. 
	GPIO_PORTB_AFSEL_R &= ~0x20; 
	GPIO_PORTD_AFSEL_R &= ~0x02; 
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
	//NVIC Port B Interrupt - IRQ 1 
	//should use NVIC_EN0_R. 
	//Calculate Constant 
	//PortA = 0. 
	NVIC_EN0_R |= 0x00000001; 
	
	//Set Priority Register - positive logic switch
	//Calculate offset
	//0b0000.0000.0000.0000.1110.0000.0000.0000 = 0x0000E000 
	NVIC_PRI0_R &= ~0x0000E000; 
	
	//Should be rising edge interrupt. 
	
	GPIO_PORTB_IM_R |= 0x20;  //ARM interrupt on PB5.
	
	GPIO_PORTB_IS_R &= ~GPIO_PIN_5; //IS Register as 0.  Edge-triggered
	GPIO_PORTB_IBE_R &= ~0x20; //IBE register as 1. Both edge will trigger interrupt
	GPIO_PORTB_IEV_R |= 0x20; //Falling Edge Event - does not matter? 
	
	IntGlobalEnable(); 
	
}


void GPIOPortD_Handler(void) {
	
	//IF PD1 signal is positive 
	if (GPIO_PORTB_RIS_R&0x20) {
		GPIO_PORTD_ICR_R ^= 0x02;  //Remove Interrupt Flag 
		//Turn on the negative logic LED1 PE4 
		
		GPIO_PORTE_DATA_R &= ~0x10; 
		
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



