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

void printInvalidInput(void) {
	  UARTCharPut(UART0_BASE, 'I');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 'v');
    UARTCharPut(UART0_BASE, 'a');
    UARTCharPut(UART0_BASE, 'l');
    UARTCharPut(UART0_BASE, 'i');
    UARTCharPut(UART0_BASE, 'd');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'I');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 'p');
    UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 't');
	  UARTCharPut(UART0_BASE, '\n');
	
}



void UARTIntHandler(void)
{
    uint32_t ui32Status;

    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status

    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
		
	  /*
	
	  //Enter loop if characters are available. 
		while(UARTCharsAvail(UART0_BASE)) {
			if (UARTCharGetNonBlocking(UART0_BASE) == 'R') {
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
				UARTCharPut(UART0_BASE, 'R');
			}
			else if (UARTCharGetNonBlocking(UART0_BASE) == 'r') {
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
				UARTCharPut(UART0_BASE, 'r');
			}
		  else if (UARTCharGetNonBlocking(UART0_BASE) == 'B') {
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
				UARTCharPut(UART0_BASE, 'B');
			}
			else if (UARTCharGetNonBlocking(UART0_BASE) == 'b') {
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
				UARTCharPut(UART0_BASE, 'b');
			}
			else if (UARTCharGetNonBlocking(UART0_BASE) == 'G') {
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
				UARTCharPut(UART0_BASE, 'G');
			}
			else if (UARTCharGetNonBlocking(UART0_BASE) == 'g') {
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
				UARTCharPut(UART0_BASE, 'g');
			}
			else {
					printInvalidInput();
			}
		}
		*/
		
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
					printInvalidInput();
				break;
				}
			}
		}

int main(void) {
	
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2
		GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
	
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART0); //enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts

    UARTCharPut(UART0_BASE, 'E');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, 'r');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'T');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, 'x');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, ':');
    UARTCharPut(UART0_BASE, ' ');

    while (1) //let interrupt handler do the UART echo function
    {
				
		}

}






