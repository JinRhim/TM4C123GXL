#include <stdint.h>
#include <stdbool.h>
#include "toggle_SysTick_interrupt_TivaWare.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"


#define 	BLUE_MASK 		0x04
//*****************************************************************************
//
//!
//! A very simple example that uses a general purpose timer generated periodic 
//! interrupt to toggle the on-board LED.
//
//*****************************************************************************


void
PortFunctionInit(void)
{
    //
    // Enable Peripheral Clocks 
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable pin PF2 for GPIOOutput
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
}



void SysTick_Init(unsigned long period)
{   
	SysTickDisable(); //disable SysTick timer when setup
	SysTickPeriodSet(period); //set reload value to the counter
	SysTickIntEnable(); //enable SysTick interrupt
	SysTickEnable(); //enable the SysTick timer 
}

//interrupt handler for SysTick timer
void SysTick_Handler(void)
{
	
		// Toggle the blue LED.
    GPIO_PORTF_DATA_R ^=BLUE_MASK;

}

int main(void)
{	
		unsigned long period = 8000000; //reload value to Timer0A to generate half second delay
	
		//initialize the GPIO ports	
		PortFunctionInit();
	
    // Turn on the LED.
    GPIO_PORTF_DATA_R |= 0x04;

    //initialize Timer0A and configure the interrupt
		SysTick_Init(period);
	
		IntMasterEnable();        		// globally enable interrupt
	
    //
    // Loop forever.
    //
    while(1)
    {

    }
}
