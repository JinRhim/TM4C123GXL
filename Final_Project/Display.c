#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include <string.h>

void PB5(bool a);   //diaplay A
void PB0(bool a);   //display B 
void PB1(bool a);		//diaplay C
void PE4(bool a);		//diaplay D
void PE5(bool a);		//diaplay E
void PB4(bool a);		//diaplay F
void PA5(bool a);		//diaplay G

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