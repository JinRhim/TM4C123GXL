#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib./interrupt.h"
#include "inc/hw_memmap.h"

/*
			000  state 0: no light      
			001  state 1: red 
			010  state 2: blue 
			011  state 3: red + blue 
			100  state 4: green
			101  state 5: green + red 
			110  state 6: green + blue 
			111  state 7: green + blue + red 

			Green | Blue | Red = 2 1 0 bit counter 
 
 - counter: incremented by 1 every seconds 
 - SW1: increment counter 
 - SW2: decrement counter
*/



//volatile keyword will force Microcontroller to always update the count status. 

volatile unsigned count = 0;

void LEDLogic(){
	
	/*
			000  state 0: no light      
			001  state 1: red 
			010  state 2: blue 
			011  state 3: red + blue 
			100  state 4: green
			101  state 5: green + red 
			110  state 6: green + blue 
			111  state 7: green + blue + red 

			Green | Blue | Red = 2 1 0 bit counter 
	    
			Below loop will do Bitwise operation to check the status
	*/
			count = count % 8;

			if ((count & 0x01) == 0x01) {        //if 00X is 1 then
					GPIO_PORTF_DATA_R |= 0x02;   //turn on red
			}
			else {
					GPIO_PORTF_DATA_R &= ~0x02; // turn off red 
			}
		
			if ((count & 0x02) == 0x02 ) {     //if 0X0 is 1 then
					GPIO_PORTF_DATA_R |= 0x04;  //turn on blue 
			}
			else {
					GPIO_PORTF_DATA_R &= ~0x04; //turn off blue
			}
			
			if ((count & 0x04) == 0x04 ) {    //if X00 is 1 then
					GPIO_PORTF_DATA_R |= 0x08; //turn on green
			}
			else {
					GPIO_PORTF_DATA_R &= ~0x08; //turn off green
			}

		
}


void
PortFunctionInit(void)
{

		volatile uint32_t ui32Loop;   
		// Enable the clock of the GPIO port that is used for the on-board LED and switch.
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
    // Do a dummy read to insert a few cycles after enabling the peripheral.
    ui32Loop = SYSCTL_RCGC2_R;

		// Unlock GPIO Port F
		GPIO_PORTF_LOCK_R = 0x4C4F434B;   
		GPIO_PORTF_CR_R |= 0x01;           // allow changes to PF0

		// Set the direction of PF4 (SW1) and PF0 (SW2) as input by clearing the bit
    GPIO_PORTF_DIR_R &= ~0x11; 
	
		GPIO_PORTF_DIR_R |= 0x02;
		GPIO_PORTF_DIR_R |= 0x04;
		GPIO_PORTF_DIR_R |= 0x08;
	
    // Enable PF4, and PF0 for digital function.
    GPIO_PORTF_DEN_R |= 0x01; 
		GPIO_PORTF_DEN_R |= 0x02;
		GPIO_PORTF_DEN_R |= 0x04;
		GPIO_PORTF_DEN_R |= 0x08;
		GPIO_PORTF_DEN_R |= 0x10;
	
		//Enable pull-up on PF4 and PF0
		GPIO_PORTF_PUR_R |= 0x11; 
}


//Globally enable interrupts 
void IntGlobalEnable(void)
{
    __asm("    cpsie   i\n");
}

void Timer0A_Init(unsigned long period)
{   
	//
  // Enable Peripheral Clocks 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); 		// configure for 32-bit timer mode
  TimerLoadSet(TIMER0_BASE, TIMER_A, period -1);      //reload value
	IntPrioritySet(INT_TIMER0A, 0x00);  	 // configure Timer0A interrupt priority as 0
  IntEnable(INT_TIMER0A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
  TimerEnable(TIMER0_BASE, TIMER_A);      // enable timer0A
}


void Timer1A_Init()
{   
	//
  // Enable Peripheral Clocks 
	//
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT); 		// configure for 32-bit timer mode
  // TimerLoadSet(TIMER1_BASE, TIMER_A, period -1);      //reload value
	IntPrioritySet(INT_TIMER1A, 0x01);  	 // configure Timer1A interrupt priority as 0
  IntEnable(INT_TIMER1A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
  // TimerEnable(TIMER1_BASE, TIMER_A);      // enable timer0A
}


void Switch_Interrupt_Init(void)
{
  NVIC_EN0_R |= 0x40000000;  		// enable interrupt 30 in NVIC (GPIOF)
	NVIC_PRI7_R &= 0x00E00000; 		// configure GPIOF interrupt priority as 0
	GPIO_PORTF_IM_R |= 0x11;   		// arm interrupt on PF0 and PF4
	GPIO_PORTF_IS_R &= ~0x11;     // PF0 and PF4 are edge-sensitive. set 0
  GPIO_PORTF_IBE_R &= ~0x11;   	// PF0 and PF4 not both edges trigger. set 0
  GPIO_PORTF_IEV_R &= ~0x11;  	// PF0 and PF4 falling edge event. set 0
	IntGlobalEnable();        		// globally enable interrupt
}



//interrupt handler for Timer0A
void Timer0A_Handler(void)
{
		// acknowledge flag for Timer0A
		TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
		//Add 1 to count 
    count ++;

		LEDLogic();
}


void Timer1A_Handler(void)
{
		//Remove Flag
		TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    NVIC_EN0_R |= 0x40000000; 
		LEDLogic();
}


//interrupt handler
void GPIOPortF_Handler(void)
{
	
	 NVIC_EN0_R &= ~0x40000000;
	
	//SW1 is pressed
	if(GPIO_PORTF_RIS_R&0x10)
	{

		// acknowledge flag for PF4
		GPIO_PORTF_ICR_R |= 0x10; 
		//counter imcremented by 1 
		
		if((GPIO_PORTF_DATA_R&0x10)==0x00) {
			count++;
		}
	}
	
	//SW2 is pressed
  if(GPIO_PORTF_RIS_R&0x01)
	{
		// acknowledge flag for PF0
		GPIO_PORTF_ICR_R |= 0x01; 
		//counter decremented by 1
		if((GPIO_PORTF_DATA_R&0x01)==0x00) {

			if (count > 0) { 
				count--;
			}
			else {
				count = 7;
			}

	}
	
	}
	LEDLogic();  //This will update the LED
	TimerLoadSet(TIMER1_BASE, TIMER_A, 80000); //Count for 1 seconds
	TimerEnable(TIMER1_BASE, TIMER_A); //automatic counter will start
}


int main(void)
{	
		unsigned long period = 16000000; //reload value to Timer0A to generate half second delay
	
		//initialize the GPIO ports	
		PortFunctionInit();
	
	  Switch_Interrupt_Init();
    // Turn on the LED.
    GPIO_PORTF_DATA_R |= 0x04;

    //initialize Timer0A and configure the interrupt
		Timer0A_Init(period);
		Timer1A_Init();
		
		IntGlobalEnable();        		// globally enable interrupt
	
    // Loop forever.
    //
    while(1)
    {
		
    }
}



