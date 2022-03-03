#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include <string.h>

/*
PB5 : Alphabet A
PB0 : Alphabet B
PB1 : Alphabet C 
PE4 : Alphabet D
PE5 : Alphabet E
PB4 : Alphabet F 
PA5 : Alphabet G 
*/


void PB5(bool a);   //diaplay A
void PB0(bool a);   //display B 
void PB1(bool a);		//diaplay C
void PE4(bool a);		//diaplay D
void PE5(bool a);		//diaplay E
void PB4(bool a);		//diaplay F
void PA5(bool a);		//diaplay G


void
PortFunctionInit(void)
{ 
	/*
	This is test program for 7-digit display 
	It will display the number 
	  A
	 ----
F |    | B
	 -G--
E |    | C
	 ----
	  D
	
	
	4511 4-bit input will be: 
	0: 0000
	1: 0001
	2: 0010
	3: 0011
	4: 0100
	5: 0101
	6: 0110
	7: 0111
	8: 1000
	9: 1001

	PD0:     
	PD1:
	PD2:
	PD3:
	
	*/
    volatile uint32_t ui32Loop;
	
    //uint32_t = 2^32 - 1.
    // Enable the clock of the GPIO port that is used for the on-board LED and switch.
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOD;
	 
    // Do a dummy read to insert a few cycles after enabling the peripheral.
    ui32Loop = SYSCTL_RCGC2_R;

     // Unlock GPIO Port D
		GPIO_PORTD_LOCK_R = 0x4C4F434B; 
	
    // allow changes to Port D 
    GPIO_PORTD_CR_R |= 0x0F;      

		//Direction of output Port D = 1.  
		// 0000.1111 for PD0123
		GPIO_PORTD_DIR_R |= 0x0F;    //0000.1111
	
    //Enable Port D for digital function
    GPIO_PORTD_DEN_R |= 0x0F;   //0000.1111
	
     //Enable pull-up on Port D 
    GPIO_PORTD_PUR_R |= 0x0F; 
}

int main(void)
{
     //initialize the GPIO ports	 
     PortFunctionInit();
	
    while(1)
    {
		// it will loop from 0 --> 1 ---> 2 ---> 0 	
		
		GPIO_PORTD_DATA_R &= 0x00; 
		SysCtlDelay(100000); 
		GPIO_PORTD_DATA_R |= 0x01;
		SysCtlDelay(100000);
		GPIO_PORTD_DATA_R &= 0x00; 
		GPIO_PORTD_DATA_R |= 0x02;
		SysCtlDelay(100000);

    }
}

void setDisplayNum(char c, int num) {
	
	if (c == 'A' || c== 'a') { 
		GPIO_PORTA_DATA_R &= 0x00; 
		GPIO_PORTA_DATA_R |= num; 
		SysCtlDelay(100000);
	}
	else if (c == 'B' || c== 'b') {
		SysCtlDelay(100000); 
		GPIO_PORTB_DATA_R &= 0x00; 
		GPIO_PORTB_DATA_R |= num; 
		SysCtlDelay(100000);
	}
	else if (c == 'C' || c== 'c') {
		SysCtlDelay(100000); 
		GPIO_PORTC_DATA_R &= 0x00; 
		GPIO_PORTC_DATA_R |= num; 
		SysCtlDelay(100000);
	}
	else if (c == 'D' || c== 'd') {
		SysCtlDelay(100000); 
		GPIO_PORTD_DATA_R &= 0x00; 
		GPIO_PORTD_DATA_R |= num; 
		SysCtlDelay(100000);
	}
	else if (c == 'E' || c== 'e') {
		SysCtlDelay(100000); 
		GPIO_PORTE_DATA_R &= 0x00; 
		GPIO_PORTE_DATA_R |= num; 
		SysCtlDelay(100000);
	}
	else if (c == 'F' || c== 'f') {
		SysCtlDelay(100000); 
		GPIO_PORTE_DATA_R &= 0x00; 
		GPIO_PORTE_DATA_R |= num; 
		SysCtlDelay(100000);
	}
	else { 
}
}


/*

	  A
	 ----
F |    | B
	 -G--
E |    | C
	 ----
	  D
	
If the port is connected in Array form in J1 row 
PB5 : Alphabet A
PB0 : Alphabet B
PB1 : Alphabet C 
PE4 : Alphabet D
PE5 : Alphabet E
PB4 : Alphabet F 
PA5 : Alphabet G 

need to unlock port B, E, A 
*/


void unlockDisplayPort1() {
	
	volatile uint32_t ui32Loop;
	//Enable clock 
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA;
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOB; 
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOE; 
	
	//Dummy Loop
	ui32Loop = SYSCTL_RCGC2_R; 
	
	//Unlock port 
	GPIO_PORTA_LOCK_R = 0x4C4F434B;
	GPIO_PORTB_LOCK_R = 0x4C4F434B;
	GPIO_PORTE_LOCK_R = 0x4C4F434B;	
	
	//allow changes to port 
	GPIO_PORTA_CR_R |= 0x20;      //unlock PA5   0010.0000
	GPIO_PORTB_CR_R |= 0x23;			//unlock PB015.0010.0011
	GPIO_PORTE_CR_R |= 0x30;      //unlock PE45. 0011.0000
	
	//Direction to output 
	GPIO_PORTA_DIR_R |= 0x20;
	GPIO_PORTB_DIR_R |= 0x23;
	GPIO_PORTE_DIR_R |= 0x10;
	
	//Enable digital function 
	GPIO_PORTA_DEN_R |= 0x20;
	GPIO_PORTB_DEN_R |= 0x23;
	GPIO_PORTE_DEN_R |= 0x10;
	
	//Enable PULL-UP 
	GPIO_PORTF_PUR_R |= 0x20; 
	GPIO_PORTB_DEN_R |= 0x23; 
	GPIO_PORTE_DEN_R |= 0x10; 
}


/*

	  A
	 ----
F |    | B
	 -G--
E |    | C
	 ----
	  D
	
If the port is connected in Array form in J1 row 
PB5 : Alphabet A
PB0 : Alphabet B
PB1 : Alphabet C 
PE4 : Alphabet D
PE5 : Alphabet E
PB4 : Alphabet F 
PA5 : Alphabet G 

Possible Alphabet: 
A b C d E F H I J L O P S U Y 

LOAd 
HELLO  
bYE

--> A b d E L O Y H 
*/

void charDisplay1(char displayStr[]) {
	int length = strlen(displayStr); 
	for (int i = 0; i < length; i++) {
		if(displayStr[i] == 'A' || displayStr[i]== 'a') {
			PB5(1);
			PB0(1);
			PB1(1);
			PE4(0);
			PE5(1);
			PB4(1);
			PA5(1);		
		}
	 else if(displayStr[i] == 'b'||displayStr[i]== 'B') {
		 	PB5(0);
			PB0(0);
			PB1(1);
			PE4(1);
			PE5(1);
			PB4(1);
			PA5(1);	
		}
	 else if(displayStr[i] == 'd' || displayStr[i] == 'D') {
		 	PB5(0);
			PB0(1);
			PB1(1);
			PE4(1);
			PE5(1);
			PB4(0);
			PA5(1);	
		}
	 else if(displayStr[i] == 'E' || displayStr[i] == 'e') {
			PB5(1);
			PB0(0);
			PB1(0);
			PE4(1);
			PE5(1);
			PB4(1);
			PA5(1);	
		}
	 SysCtlDelay(100000);
	}
}

void PB5(bool a) {
	if(a) {
		GPIO_PORTB_DATA_R |= 0x20;    //0010.0000
	}
	else {
		GPIO_PORTB_DATA_R &= ~0x20;    //1101.1111
	}
	
}

void PB0(bool a) {
	if(a) {
		GPIO_PORTB_DATA_R |= 0x01;    //0000.0001
	}
	else {
		GPIO_PORTB_DATA_R &= ~0x01;    //1111.1110
	}
	
}


void PB1(bool a) {
	if(a) {
		GPIO_PORTB_DATA_R |= 0x02;    //0000.0010
	}
	else {
		GPIO_PORTB_DATA_R &= ~0x02;
	}
}


void PE4(bool a) {
	if(a) {
		GPIO_PORTE_DATA_R |= 0x10;    //0001.0000
	}
	else {
		GPIO_PORTE_DATA_R &= ~0x10;
	}
}

void PE5(bool a) {
	if(a) {
		GPIO_PORTE_DATA_R |= 0x20;    //0010.0000
	}
	else {
		GPIO_PORTE_DATA_R &= ~0x20;
	}
}


void PB4(bool a) {
	if(a) {
		GPIO_PORTB_DATA_R |= 0x10;    //0001.0000
	}
	else {
		GPIO_PORTB_DATA_R &= ~0x00;
	}
}

void PA5(bool a) {
	if(a) {
		GPIO_PORTB_DATA_R |= 0x20;    //0010.0000
	}
	else {
		GPIO_PORTB_DATA_R &= ~0x20;
	}
}







