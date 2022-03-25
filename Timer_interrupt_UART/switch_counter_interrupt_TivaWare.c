#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "switch_counter_interrupt_TivaWare.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"


//*****************************************************************************
//
//!
//! Design a counter. The counter is incremented by 1 when SW1 (PF4) or SW2 (PF0) 
//! is pressed.
//
//*****************************************************************************

// global variable visible in Watch window of debugger
// increments at least once per button press
volatile unsigned long count = 0;

void
PortFunctionInit(void)
{
    //
    // Enable Peripheral Clocks 
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable pin PF4 for GPIOInput
    //
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

    //
    // Enable pin PF0 for GPIOInput
    //

    //
    //First open the lock and select the bits we want to modify in the GPIO commit register.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

    //
    //Now modify the configuration of the pins that we unlocked.
    //
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	
		//Enable pull-up on PF4 and PF0
		GPIO_PORTF_PUR_R |= 0x11; 

}


void
Interrupt_Init(void)
{
  IntEnable(INT_GPIOF);  							// enable interrupt 30 in NVIC (GPIOF)
	IntPrioritySet(INT_GPIOF, 0x00); 		// configure GPIOF interrupt priority as 0
	GPIO_PORTF_IM_R |= 0x11;   		// arm interrupt on PF0 and PF4
	GPIO_PORTF_IS_R &= ~0x11;     // PF0 and PF4 are edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;   	// PF0 and PF4 not both edges trigger 
  GPIO_PORTF_IEV_R &= ~0x11;  	// PF0 and PF4 falling edge event
	
}

void
uart_Init(void) {
	
		//SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);


    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, 
        (UART_CONFIG_WLEN_8| UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}


//interrupt handler
void GPIOPortF_Handler(void)
{
	char str[3];
	int i=0;
	//SW1 is pressed
	if(GPIO_PORTF_RIS_R&0x10)
	{
		// acknowledge flag for PF4
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4); 
		//counter imcremented by 1
		count++;
		sprintf(str, "%d", count);
		/*
		UARTCharPut(UART0_BASE, str[0]); 
		UARTCharPut(UART0_BASE, str[1]); 
		UARTCharPut(UART0_BASE, str[2]); 
		*/
		while(str[i]!='\0')
		{
			UARTCharPut(UART0_BASE, str[i]);
			i++;
		}
		//UARTCharPut(UART0_BASE, '\n'); 
		UARTCharPut(UART0_BASE, '\r'); 
	}
	
	//SW2 is pressed
  if(GPIO_PORTF_RIS_R&0x01)
	{
		// acknowledge flag for PF0
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
		//counter imcremented by 1
		count++;
		sprintf(str, "%d", count);
		/*
		UARTCharPut(UART0_BASE, str[0]); 
		UARTCharPut(UART0_BASE, str[1]); 
		UARTCharPut(UART0_BASE, str[2]); 
		*/
		while(str[i]!='\0')
		{
			UARTCharPut(UART0_BASE, str[i]);
			i++;
		}
		//UARTCharPut(UART0_BASE, '\n'); 
		UARTCharPut(UART0_BASE, '\r');
	}
}

void Timer3A_Init(unsigned long period)
{   
	//
  // Enable Peripheral Clocks 
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
  TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC); 		// configure for 32-bit timer mode
  TimerLoadSet(TIMER3_BASE, TIMER_A, period -1);      //reload value
	IntPrioritySet(INT_TIMER3A, 0x40);  	 // configure Timer0A interrupt priority as 0
  IntEnable(INT_TIMER3A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
  TimerEnable(TIMER3_BASE, TIMER_A);      // enable timer0A
}

//interrupt handler for Timer0A
void Timer3A_Handler(void)
{
		char str[3];
		int i=0;
		// acknowledge flag for Timer0A timeout
		TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
	
		count++;
		sprintf(str, "%d", count);
	
		while(str[i]!='\0')
		{
			UARTCharPut(UART0_BASE, str[i]);
			i++;
		}
		//UARTCharPut(UART0_BASE, '\n'); 
		UARTCharPut(UART0_BASE, '\r'); 
	
		// Toggle the blue LED.
    GPIO_PORTF_DATA_R ^=0x04;

}

int main(void)
{
	
		//char msg[11] = "counter=\n\r";
		char msg[] = {'c','o','u','n','t','e','r','=','\n','\r','\0'}; 

		int i=0;
		
		//unsigned long period = 8000000; 
		unsigned long period; 
		
		SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //400Mhz/2/4= 50MHz
		
		//period = 25000000; //reload value to Timer3A to generate half second delay
		period = SysCtlClockGet()/2; //reload value to Timer3A to generate half second delay
		
		//initialize the GPIO ports	
		PortFunctionInit();
	
		//initialize UART0
		uart_Init();
		
		//configure the GPIOF interrupt
		Interrupt_Init();
		
		//initialize Timer0A and configure the interrupt
		Timer3A_Init(period);
		IntMasterEnable();       		// globally enable interrupt
	
		while(msg[i]!='\0')
		{
			UARTCharPut(UART0_BASE, msg[i]);
			i++;
		}
	/*
	  UARTCharPut(UART0_BASE, 'C');
    UARTCharPut(UART0_BASE, 'o');
    UARTCharPut(UART0_BASE, 'u');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, '=');
    UARTCharPut(UART0_BASE, '\n'); 
		UARTCharPut(UART0_BASE, '\r'); 
	*/
    //
    // Loop forever.
    //
    while(1)
    {

    }
}
