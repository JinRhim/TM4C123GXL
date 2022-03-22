1. RCGC2 (Run Mode Clock Gating Control Register 2) - enable clock for specific port. 

2. Lock Register - unlock port 

3. CR (Commit REgister) - set 1: receive data 

4. DIR (Data Direction Register) - input(0)/output(1)

5. PUR (Pull Up Register) - (There is also PDR: Pull down register)
    1. IF device is not connected to anything --> neither low nor high 

6. DEN (Digital Enable Register)




1. Enable Port F Clock 

SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF 

+ Dummy cycle for clock 

ui32Loop = SYSCTL_RCGC2_R 

2. Unlock GPIO Port F 

GPIO_PORTF_LOCK_R = 0x4C4F434B

3. Change Commit Register

GPIO_PORTF_CR_R |= 0x01             //allow change to PF0 

4. Direction (DIR) Register - Set PF2 to output

GPIO_PORTF_DIR_R |= 0x04.           //0x0000.0100 --> PF2:1  PF1:0 PF0:0 

5. Enable PF2 for digital function 

GPIO_PORTF_DEN_R |= 0x04 

6. Enable Pull-up on PF0 

GPI0_PORTF_PUR_R |= 0x04.          //0x0000.0100 --> enable pull up. 





##Delay Function 

#include "driverlib/sysctl.h"
SysCtlDelay(2000000).  --> 200000 Loop. Each loop = 3 CPU cycle. 


PORTF  | Function
------------- | -------------
PF0  | SW2 (negative logic)
PF1  | RED LED
PF2  | BLUE LED
PF3  | GREEN LED
PF4  | SW1 (negative logic)





	 //PF0 = SW2 (negative logic)
	 //PF1 = Red LED 
	 //PF2 = Blue LED 
	 //PF3 = Green LED 
	 //PF4 = SW1 (negative logic)
	 
	 //SysCrlDelay(5333333);   --> Delay for 1 seconds 
	 
	 
	 

// while (1){
// 	for (int i=0;i<1000;i++){
// 		if(Data&0x01== 0x00){ //Do something}
// 		SysCrlDelay(5333);
// 	}
// }

