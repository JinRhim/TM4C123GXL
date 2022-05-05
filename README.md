# Basic Ports 

### Initial Port Activation 

1. RCGC2 (Run Mode Clock Gating Control Register 2) - enable clock for specific port. 

2. Lock Register - unlock port 

3. CR (Commit REgister) - set 1: receive data 

4. DIR (Data Direction Register) - input(0)/output(1)

5. PUR (Pull Up Register) - (There is also PDR: Pull down register)
    1. IF device is not connected to anything --> neither low nor high 

6. DEN (Digital Enable Register)

#### Other ports 

7. AMSEL (Analog Function Register) - analog: 1 digital: 0
 
9. AFSEL (Alternate Function Register) - alternate function: 1 none: 0

### I/O Register Basic Address 
<img width="629" alt="image" src="https://user-images.githubusercontent.com/93160540/166481030-ae4ec999-5ad7-4e08-bb12-8a881d2af51a.png">


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
GPIO_PORTF_DIR_R |= 0x08;

GPIO_PORTF_DIR_R &= ~0x11;  //AND 1110.1110. Clear PF0 and PF4 to set direction

GPIO_PORTF_DEN_R |= 0x01; 
GPIO_PORTF_DEN_R |= 0x02;
GPIO_PORTF_DEN_R |= 0x04;
GPIO_PORTF_DEN_R |= 0x08;
GPIO_PORTF_DEN_R |= 0x10;
		

GPIO_PORTF_PUR_R |= 0x01;   //Pull-up register is only for PF0 and PF4. 
GPIO_PORTF_PUR_R |= 0x10;
```

### PF0 ~ PF4 Basic Functions

```
if((GPIO_PORTF_DATA_R&0x10)==0x00) //SW1 presssed 
		
if((GPIO_PORTF_DATA_R&0x01)==0x00) //SW2 presssed 
		
GPIO_PORTF_DATA_R |= 0x02; // turn on red PF1
GPIO_PORTF_DATA_R &= ~0x02; // turn off red 

GPIO_PORTF_DATA_R |= 0x04; // turn on Blue PF2
GPIO_PORTF_DATA_R &= ~0x04; // turn off Blue

GPIO_PORTF_DATA_R |= 0x08; // turn on Green PF3
GPIO_PORTF_DATA_R &= ~0x08; // turn off Green 	

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



## Clock Generation / Timer  
1. Clock Generator 
  1. Precision Internal OScillator (PIOSC) - 16 MHz default 
  2. Main OScillator (MOSC) - 5 ~ 25 MHz
  3. Low Frequency Interal OScillator (LFIOSC) - 30KHz Deep Sleep Mode
  4. Hibernate Module Clock Source - 32.768 KHz Hibernate mode
 
2. Header File to include - #include "driverlib/sysctl.h"
 
3. SysCtlDelay()
- generates 3 redundant instruction to create delay. 
- SysCtlDelay(1) will generate 3 instruction cycle. 
- For example, SysCtlDelay(1 million) will generate 3 million redundant instruction.
- If system clock is set to 10 MHz, it will create 0.3 seconds delay. 
- Default clock: 16 MHz. In order to create 1 seconds delay, 16 million/3 = 5333333. 
- SysCtlDelay(5333333) = 1 seonds delay
 
4. Code
 
```
System Clock = always comes from 400 MHz source.   
System Clock = 400 MHz/2/SYSCTL_SYSDIV_(Number)  

SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
//Clock = 400/2/5 --> 40 MHz

uint32_t ui32SysClkFreq; 

SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
//Clock = 400/2/10 --> 20MHz

ui32SysClkFreq = SysCtlClockGet( );

Clock = SysCtlClockGet();     //get the processor clock rate

```

## Bit-specific addressing

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


# Interrupt 

## Basic Interrupt Initialization 

```
void
Interrupt_Init(void)
{
        NVIC_EN0_R |= 0x40000000;  		// enable interrupt 30 in NVIC (GPIOF)
	NVIC_PRI7_R &= 0x00E00000; 		// configure GPIOF interrupt priority as 0
	GPIO_PORTF_IM_R |= 0x11;   		// arm interrupt on PF0 and PF4
	GPIO_PORTF_IS_R &= ~0x11;     // PF0 and PF4 are edge-sensitive. set 0
        GPIO_PORTF_IBE_R &= ~0x11;   	// PF0 and PF4 not both edges trigger. set 0
        GPIO_PORTF_IEV_R &= ~0x11;  	// PF0 and PF4 falling edge event. set 0
	IntGlobalEnable();        		// globally enable interrupt
}


void IntGlobalEnable(void)
{
    __asm("    cpsie   i\n");
}

//Globally disable interrupts 
void IntGlobalDisable(void)
{
    __asm("    cpsid   i\n");
}

```
<img width="514" alt="image" src="https://user-images.githubusercontent.com/93160540/162279195-700fb811-f89d-4797-9a15-6281ffd5869e.png">
<img width="527" alt="image" src="https://user-images.githubusercontent.com/93160540/162279251-bc5a3f95-4547-448f-8401-21a2a29d5770.png">


### 4 Conditions of Interrupt Conditions 
- NVIC enable 
- ARM - set Interrupt Mask Bit 
- Global Enable
- Trigger - hardware action sets a source-specific flag

### How to calculate NVIC_EN_Register 	

NVIC Register | IRQ number
------------- | -------------
NVIC_EN0_R  | 0 ~ 31
NVIC_EN1_R | 32 ~ 63
	
1. To enable UART2 Interrupt, find out IRQ number
 	1. UART2 IRQ = 33. 
 	2. 2. Use NVIC_EN1_R --> Enable UART Interrupt bit as 1. 
2. To enable GPIOPortF_Handler, 
	1. GPIOPortF_Handler IRQ = 30 
	2. Use NVIC_EN0_R
3. To enable GPIOPortA_Handler,
	1. GPIOPortA_Handler IRQ = 0
	2. Use NVIC_EN0_R 

### How to calculate offset for NVIC Registers 
<img width="512" alt="image" src="https://user-images.githubusercontent.com/93160540/162282594-fe568e20-26ab-4e02-b5b0-b6afc9ec72f7.png">

1. In order to enable GPIO_PortF, 30th bit should be 1. 
- 0b0100.0000.0000.0000.0000.0000.0000.0000. 
- ⇒ NVIC_EN0_R |= 0x4000.0000. 

2. In order to enable GPIO port A, 0st bit should be 1. 
- 0b0000.0000.0000.0000.0000.0000.0000.0000 
- ⇒ NVIC_EN0_R |= 0x0000.0001. 

3. In order to enable UART2 interrupt, 1st bit should be 1. 
- 0b0000.0000.0000.0000.0000.0000.0000.0010
- ⇒ NVIC_EN1_R |= 0x0000.0010


### How to select PRI Registers 
- Look up table and figure out which PRI_Registers to use.

Port | PRI Registers to use
---------------- | ------------------
Port A  | NVIC_PRI0_R
Port B  | NVIC_PRI0_R
Port C  | NVIC_PRI0_R
Port D  | NVIC_PRI0_R
Port E  | NVIC_PRI1_R
Port F  | NVIC_PRI7_R
Timer 0A  | NVIC_PRI4_R

### How to calculate PRI Register offset (NVIC_PRI7_R &= 0x00E00000)
<img width="880" alt="image" src="https://user-images.githubusercontent.com/93160540/162283684-f4f463c7-78db-4e29-999e-fc4b00ec9f94.png">

### How to set up Edge Trigger Interrupt Initialization - Summary Chart 
1. NVIC_EN0/1_R - set to 1. (Look at chart to figure out NVIC_EN0 or EN1.) 
2. NVIC_PRI_R - set to 0. (This will make priority. Setting it to 0 will make highest priority)(Look at datasheet to calculate offset constant)

Edge Trigger Register Set | Number Set
---------------- | ------------------
GPIO_PORT_IM_R  | 0: no interrupt 1: interrupt
GPIO_PORT_IS_R  | 0: Input. (Always set to 0)
GPIO_PORT_IBE_R | 0: one edge trigger 1: both edge trigger
GPIO_PORT_IEV_R | 0: falling edge trigger 1: rising edge trigger

<img width="596" alt="image" src="https://user-images.githubusercontent.com/93160540/162283920-9220ee44-eac9-45c4-9c85-7a4c58be9656.png">


##### Example Codes for Rising Edge Trigger of PD1 and PD2

```
void Interrupt_Init(void) {
	//NVIC Port D interrupt - PD1 and PD2 
	//Port D IRQ = 3. 
	//Use NVIC_EN0_R Register. 
	//PortD = 3rd bit. 
	//0b0000.0000.0000.0000.0000.0000.0000.0000.1000 --> 0x0000.0008 
	NVIC_EN0_R |= 0x00000008; 
	
	//Set Priority Register - positive logic switch
	NVIC_PRI0_R &= ~0xE0000000; 
	
	GPIO_PORTD_IM_R |= 0x06;  //ARM interrupt on PD1 and PD2. 
	
	GPIO_PORTD_IS_R &= ~0x06; //0: input. 
	GPIO_PORTD_IBE_R &= ~0x06; //0: one edge trigger 1: both edge trigger
        GPIO_PORTD_IEV_R |= 0x06; //0: falling edge trigger 1: rising edge trigger
	
	IntGlobalEnable(); 
}
```




# I2C Communication 

<img width="379" alt="image" src="https://user-images.githubusercontent.com/93160540/160469132-3dc94af1-d5a4-4bde-a460-f1612df81139.png">

To generate a single transmit cycle,

1. I2CMSA (I2C Master Slave Address) --> written with desireded address. 
2. R/S bit is cleared. 
3. Control Register is written with ACK = X (0 or 1) --> to perform operation and stop 
     1. STOP = 1 
     2. START = 1 
     3. RUN = 1
  
After the operation is complete 
1. Interrupt pin = active.  
2. I2CMDR (I2C Master Data) records data. (Data can be read from here)


Data Format with 7-bit address
1. After START condition --> slave address is transmitted.  
2. Address is 
     1. 7-bit long
     2. 8th bit --> data direction bit (R/S bit in the I2CMSA register)
- R/S bit: 0 --> SEND 
3. Master: send STOP condition. --> Data transfer termination
<img width="559" alt="image" src="https://user-images.githubusercontent.com/93160540/160470244-b6eb2c6f-f8c8-4443-bf1f-45f289a3ca32.png">

4. Steops of MAster Single Transmit

<img width="376" alt="image" src="https://user-images.githubusercontent.com/93160540/160470705-c40370cc-6efd-49b7-a992-8c8ecc323b99.png">
















<img width="362" alt="image" src="https://user-images.githubusercontent.com/93160540/160477391-8490b5c4-f017-4d09-a2fe-bb42e1eb60e6.png">

<img width="497" alt="image" src="https://user-images.githubusercontent.com/93160540/160478187-0a56d9eb-6599-4758-ba6b-e0cfe6c3bb5d.png">


1602 LCD Pins | Purpose 
-----------|-----------
GND | Ground 
VCC | Power Supply 
V0 | LCD Contrast: use potentiometer to set contrast 
RS | Register Select 


RS Pin 
Low | High 
----|------
send command to LCD | sending Data/Characters to the LCD 
cursor to specific location, clear display ,scroll the display, etc... | sending character

E (Enable) 
Low | High 
----|------
ignore R/W, RS, bus | process R/W, RS, bus data 

D0-D7 
- To display "A": 01000001 to LCD (ASCII table.) x41 = [A]

A-C (Anode, Cathode) 
- used to control backlight of the LCD

