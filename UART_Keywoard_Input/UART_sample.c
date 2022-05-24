#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "uartstdio.h"
#include <string.h>



//This function will print invalid input of str[].
void printInvalidInput(void) {
	char str[] = "\n\rInvalid Input";
	for (int i = 0; i < sizeof(str); i++) {
	  UARTCharPut(UART0_BASE, str[i]);
	}
}

void UARTIntHandler(void)
{
		//volatile to update
    uint32_t ui32Status; 

    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status as true

		//clear interrupt flag
    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
		
		while(UARTCharsAvail(UART0_BASE)) { //enter loop if characters are available{
      switch(UARTCharGetNonBlocking(UART0_BASE)) 
				{
        case 'R':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);// RED LED ON
            UARTCharPut(UART0_BASE, 'R'); //Display R char
        break;
        case 'r':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);// RED LED OFF
            UARTCharPut(UART0_BASE, 'r'); //Display r char
        break;
				
				
        case 'B':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //BLUE LED ON
            UARTCharPut(UART0_BASE, 'B');  //Display B char
        break;
        case 'b':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);//BLUE LED OFF
            UARTCharPut(UART0_BASE, 'b'); //Display b char	
				break;
				
				
				case 'G':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3); //GREEN LED ON
            UARTCharPut(UART0_BASE, 'G');  //Display G char
        break;
        case 'g':
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);//GREEN LED OFF
            UARTCharPut(UART0_BASE, 'g'); //Display b char	
				break;
				
				default: 
					printInvalidInput(); //this will cal function to print erorr string
				break;
				}
			}
		}

int main(void) {
	
	//Set the system clock to 400/2/4
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//Enable peripheral clock for UART and GPIOA 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	//Enable UART pin PA0 and PA1
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
	//Enable PA0 and PA1 pin
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//Set UART communication speed to 115200. 
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	
    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART0); //enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); 
		//only enable RX and TX interrupts



		//String for "Enter Text"
		//char startStr[] = {'E', 'n', 't', 'e', 'r', ' ', 't', 'e', 'x', 't', ':'};
		//Above string can be more simplified as below.
		char startStr[] = "Enter text: ";
		
		//print string 
		for (int i = 0; i < sizeof(startStr); i++) {
			UARTCharPut(UART0_BASE, startStr[i]);
	}
	
    while (1) //let interrupt handler do the UART echo function
    {
				
		}

}






