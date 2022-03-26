### Basic Ports 

Ports | Function
------------- | -------------
PA2 ~ PA7  | PA6: I2C1SCL / PA7: I2C1SDA 
PB0 ~ PB7  | PB2: I2C0SCL / PB3: I2C0SDA
PC4 ~ PC6  | 
PD0 ~ PD7  | PD0: I2C3SCL / PD1: I2C3SDA
PE0 ~ PE1  | PE4: I2C2SCL / PE5: I2CSDA
PF0 ~ PF4  | PF0: SW2 / PF4: SW1




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

Negative Logic  | Positive Logic
------------- | -------------
Pressed: 0V F | Pressed: 3.3v T 
Not Pressed: 3.3v T | Not Pressed: 0v F



### Clock Generation 
1. Clock Generator 
  1. Precision Internal OScillator (PIOSC) - 16 MHz default 
  2. Main OScillator (MOSC) - 5 ~ 25 MHz
  3. Low Frequency Interal OScillator (LFIOSC) - 30KHz Deep Sleep Mode
  4. Hibernate Module Clock Source - 32.768 KHz Hibernate mode
 
2. System Clock = always comes from 400 MHz source.   
System Clock = 400 MHz/2/SYSCTL_SYSDIV_(Number)  
 
3. Code
 
```

SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
//Clock = 400/2/5 --> 40 MHz

uint32_t ui32SysClkFreq; 

SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
//Clock = 400/2/10 --> 20MHz

ui32SysClkFreq = SysCtlClockGet( );

Clock = SysCtlClockGet();     //get the processor clock rate

```

### Bit-specific addressing

Address | Name
------------- | -------------
$400F.E108  | SYSCTL_RCGC2_R
$4000.4000. | PORTA base address
$4000.5000. | PORTB base address
$4000.6000. | PORTC base address
$4000.7000. | PORTD base address
$4002.4000. | PORTE base address
$4002.5000. | PORTF base address
base+$3FC | GPIO_PORTx_DATA_R 
base+$400 | GPIO_PORTx_DIR_R 
base+$420 | GPIO_PORTx_AFSEL_R 
base+$510 | GPIO_PORTx_PUR_R 
base+$51C | GPIO_PORTx_DEN_R 
base+$524 | GPIO_PORTx_CR_R 
base+$528 | GPIO_PORTx_AMSEL_R 

1. Read/Write 32bit value directly 

```
#define GPIO_PORTF_DATA_R (*((volatile unsigned long *) 0x400253FC))

unsigned long PortF_Input(void){
	return (GPIO_PORTF_DATA_R); // read Port F
}

void PortF_Output(unsigned long data){ // write Port F 
	GPIO_PORTF_DATA_R = data;
}

```



