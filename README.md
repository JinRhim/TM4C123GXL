### Basic Functions 

1. RCGC2 (Run Mode Clock Gating Control Register 2) - enable clock for specific port. 

2. Lock Register - unlock port 

3. CR (Commit REgister) - set 1: receive data 

4. DIR (Data Direction Register) - input(0)/output(1)

5. PUR (Pull Up Register) - (There is also PDR: Pull down register)
    1. IF device is not connected to anything --> neither low nor high 

6. DEN (Digital Enable Register)




### PortFunctionInit() for Port F
```

volatile uint32_t ui32Loop/ 

SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;

ui32Loop = SYSCTL_RCGC2_R;

GPIO_PORTF_LOCK_R = 0x4C4F434B;

GPIO_PORTF_CR_R |= 0x01; 
GPIO_PORTF_CR_R |= 0x10; 

GPIO_PORTF_DIR_R |= 0x02;
GPIO_PORTF_DIR_R |= 0x04;      
	
GPIO_PORTF_DIR_R &= ~0x11;  //AND 1110.1110. Clear PF0 and PF4 to set direction

GPIO_PORTF_DEN_R |= 0x01; 
GPIO_PORTF_DEN_R |= 0x02;
GPIO_PORTF_DEN_R |= 0x04;
GPIO_PORTF_DEN_R |= 0x10;
		

GPIO_PORTF_PUR_R |= 0x01;
GPIO_PORTF_PUR_R |= 0x10;
```


### Other Functions 

1. Delay 
    1. #include "driverlib/sysctl.h"
    2. SysCtlDelay(5333333) 
    3. Default Clock = 16 MHz. SysCtlDelay(1) = 3 cycle.  
    4. Thus, 1 seconds = SysCtlDelay(16 Million/3)


2. Switch Function 
    1. IF SW1 is pressed --> if((GPIO_PORTF_DATA_R&0x10)!=0x00)
    2. IF SW2 is pressed --> if((GPIO_PORTF_DATA_R&0x01)!=0x00)


3. Pseudo-Interrupt mimic code 
```

while (1){
 	for (int i=0;i<1000;i++){
 		if(SW1 == pressed){ }
		else {} 
		if (SW2 == pressed) { }
		
		SysCtlDelay(5333333/1000)
 	}
}

```


PORTF  | Function
------------- | -------------
PF0  | SW2 (negative logic)
PF1  | RED LED
PF2  | BLUE LED
PF3  | GREEN LED
PF4  | SW1 (negative logic)





