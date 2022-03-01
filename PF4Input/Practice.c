#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
/*
This program will turn off RED LED when pressed. Otherwise, LED will stay turned on
*/

void
PortFunctionInit(void)
{
		//PF0 = SW2 (negative logic)
	  //PF1 = Red LED 
	  //PF2 = Blue LED 
	  //PF3 = Green LED 
	  //PF4 = SW1 (negative logic)

    volatile uint32_t ui32Loop;
	
	  //uint32_t = 2^32 - 1.
	
    // Enable the clock of the GPIO port that is used for the on-board LED and switch.
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
	
	 

    // Do a dummy read to insert a few cycles after enabling the peripheral.
    ui32Loop = SYSCTL_RCGC2_R;

     // Unlock GPIO Port F
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
	
    // allow changes to PF0
    GPIO_PORTF_CR_R |= 0x01;      

    // Set the direction of PF123 as output
    GPIO_PORTF_DIR_R |= 0x02;   //0010
		GPIO_PORTF_DIR_R |= 0x01;   //0001
		GPIO_PORTF_DIR_R |= 0x04;   //0100
		GPIO_PORTF_DIR_R |= 0x08;   //1000
	
    // Set the direction of PF0 (SW2) as input by clearing the bit
    GPIO_PORTF_DIR_R &= ~0x01;
	
    // Enable both PF123 and PF0 for digital function.
    //GPIO_PORTF_DEN_R |= 0x03;   0000.0011
		GPIO_PORTF_DEN_R |= 0x0F;   //0000.1111
	
     //Enable pull-up on PF0
    GPIO_PORTF_PUR_R |= 0x01; 
}

int main(void)
{
     //initialize the GPIO ports	 
     PortFunctionInit();

	
	
    // Loop forever.
    while(1)
    {
	//if (GPIO PORTF = XXXX.XXX1)
        if((GPIO_PORTF_DATA_R&0x01)!=0x00) //SW2 is pressed
				{
						// Turn off the LED.
						GPIO_PORTF_DATA_R |= 0x0E;   //0000.1110 14 = E 
					//original val: 0x02
						//XXXX.XX1X
				}
				//else (GPIO PORTF = XXXX.XXX0)
				else 
				{
						// Turn on the LED.
						GPIO_PORTF_DATA_R &= ~0x0E;
						//XXXX.XX0X
				}
    }
}
