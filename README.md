# Basic Ports 

![image](https://user-images.githubusercontent.com/93160540/160483828-0df5835c-5573-4f8c-b2bb-241aa3b16ba6.png)


Ports | Function
------------- | -------------
PA2 ~ PA7  | PA6: I2C1SCL / PA7: I2C1SDA 
PB0 ~ PB7  | PB2: I2C0SCL / PB3: I2C0SDA
PC4 ~ PC6  | 
PD0 ~ PD7  | PD0: I2C3SCL / PD1: I2C3SDA
PE0 ~ PE1  | PE4: I2C2SCL / PE5: I2CSDA
PF0 ~ PF4  | PF0: SW2 / PF4: SW1




### Initial Port Activation 

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
GPIO_PORTF_DIR_R |= 0x08;

GPIO_PORTF_DIR_R &= ~0x11;  //AND 1110.1110. Clear PF0 and PF4 to set direction

GPIO_PORTF_DEN_R |= 0x01; 
GPIO_PORTF_DEN_R |= 0x02;
GPIO_PORTF_DEN_R |= 0x04;
GPIO_PORTF_DEN_R |= 0x08;
GPIO_PORTF_DEN_R |= 0x10;
		

GPIO_PORTF_PUR_R |= 0x01;
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



### Clock Generation 
1. Clock Generator 
  1. Precision Internal OScillator (PIOSC) - 16 MHz default 
  2. Main OScillator (MOSC) - 5 ~ 25 MHz
  3. Low Frequency Interal OScillator (LFIOSC) - 30KHz Deep Sleep Mode
  4. Hibernate Module Clock Source - 32.768 KHz Hibernate mode
 
2. System Clock = always comes from 400 MHz source.   
System Clock = 400 MHz/2/SYSCTL_SYSDIV_(Number)  
 
3. SysCtlDelay(5333333) -  #include "driverlib/sysctl.h"
 
 
4. Code
 
```

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

1. Interrupt Conditions: All 4 of these must be true
	1. NVIC enable
		1. allow interrupt for specific source in NVIC 
		2. NVIC_ENx_R  
		3. NVIC_PRIx_R 
	2. ARM  
		1. Set interrupt mask bit
		2.  configure IMR register 
	3. Global Enable 
		1. Interrupt globally enabled (I=0 in PRIMASK)
	4. Trigger 
		1. Hardware action --> set source specific flag


* Cortex-M4 Exception Types 
<img width="874" alt="image" src="https://user-images.githubusercontent.com/93160540/160904941-abc77708-797a-4705-ab86-3ad65b78e868.png">

* Cortex-M4 Interrupts 
	1. Vector Address - Pointer: ISR beginning address
	2. Vector Number - stored in IPSR during context switch 
	3. Interrupt Number (IRQ Number) - Bit in NVIC registers
<img width="714" alt="image" src="https://user-images.githubusercontent.com/93160540/160905038-f18bcc68-a2ff-4a6a-b5c7-bea63d7cf759.png">

* Program Status Register
<img width="865" alt="image" src="https://user-images.githubusercontent.com/93160540/160907824-f574eda7-1744-4b5b-b1a2-3af40a726fee.png">



7. **BASEPRI** - set priority 0 ~ 7.
	1. IF BASEPRI = 3, 
	2. 0 ~ 2 can occur 
	3. 3- 7 are masked. 
	4. Default: BASEPRI = 0. All exceptions are unmasked. 

8. NVIC Interrupt Enable Register 

Enable Registers | Controls...
-------------|------------
NVIC_EN0_R | IRQ 0 ~ 31 
NVIC_EN1_R | IRQ 32 ~ 6
NVIC_EN2_R | ?
NVIC_EN3_R | ?
NVIC_EN4_R | ?

* Cortex-M4 Interrupts 
<img width="693" alt="image" src="https://user-images.githubusercontent.com/93160540/160931034-905081b5-1bd3-41ef-93f0-6283ae4a16e3.png">


9. Example of Enabling NVIC Interrupt Enable Registes 

```
//enable interrupt 30 in NVIC (GPIOF) 
NVIC_EN0_R |= 0x4000.0000;   
NVIC_PRI7_R &= ~0x00E0.0000;    //configure GPIOF interrupt priority as 0

//0b0000.0000.[010]0.0000.0000.0000.0000.0000. --> NVIC_PRI_4_R

```

10. Globally Enable Interrupt

```
void IntGlobalEnable(void)
{
__asm("cpsie i\n"); // Inline assembly 
}

```

11. How to set Trigger 
Register | Port | Function
--------|-------|---------
IS (Interrupt Sense) | GPIO_PORTx_IS_R | 0: edge triggering. 1: level triggering 
IBE (Interrupt Both Edges) | GPIO_PORTx_IBE_R | 1 --> both edge triggered 
IEV (Interrupt Event) | GPIO_PORTx_IEV_R | 1: Rising edge triggering. 0: Falling edge triggering
IME (Interrupt Mask Enable) | GPIO_PORTx_IM_R | 1: ARM interrupt 0: disarm Interrupt 

<img width="821" alt="image" src="https://user-images.githubusercontent.com/93160540/160934070-3f104427-7e5c-4f49-bbc4-7611a686480a.png">

12. Sample Code for Edge-triggered Modes 
```
GPIO_PORTF_IM_R |= 0x01;  //arm interrupt on PF0 
GPIO_PORTF_IS_R &= ~0x01; //PF0 is edge-sensitive 
GPIO_PORTF_IBE_R |= 0x01; //PF0 both edge trigger
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



## I2C API 

Starting Functions

Functions | Operation
------------- | -------------
I2CMasterInitExpClk()  | set bus speed / enable master module 
I2CMasterSlaveAddrSet() | define whether send/receive.
I2CMasterBusBusy() | If the Bus have multiple master --> unlikely to use this function 
I2CMasterDataPut() | trying to send data...
I2CMasterControl() | transaction initiation with following commands... 

- I2C_Master_CMD_SINGLE_SEND 
- I2C_MASTER_CMD_SINGLE_RECEIVE
- I2C_Master_CMD_BURST_SEND_START 
- I@C_MASTER_CMD_BURST_RECEIVE_START


I2CMasterBusy() | indicate whether I2C master is busy or not. 
I2CMasterErr() | check data error 
I2CMasterDataGet() | If no error --> Data has been sent or ready to be read.

Functions 

- uint32_t I2CFIFODataGet (uint32_t ui32Base)
- uint32_t I2CFIFODataGetNonBlocking (uint32_t ui32Base, uint8_t ∗pui8Data)
- void I2CFIFODataPut (uint32_t ui32Base, uint8_t ui8Data)
- uint32_t I2CFIFODataPutNonBlocking (uint32_t ui32Base, uint8_t ui8Data)
- uint32_t I2CFIFOStatus (uint32_t ui32Base)
- void I2CIntRegister (uint32_t ui32Base, void (∗pfnHandler)(void))
- void I2CIntUnregister (uint32_t ui32Base)
- uint32_t I2CMasterBurstCountGet (uint32_t ui32Base)
- void I2CMasterBurstLengthSet (uint32_t ui32Base, uint8_t ui8Length)
- bool I2CMasterBusBusy (uint32_t ui32Base)
- bool I2CMasterBusy (uint32_t ui32Base)
- void I2CMasterControl (uint32_t ui32Base, uint32_t ui32Cmd)
- uint32_t I2CMasterDataGet (uint32_t ui32Base)
- void I2CMasterDataPut (uint32_t ui32Base, uint8_t ui8Data)
- void I2CMasterDisable (uint32_t ui32Base)
- void I2CMasterEnable (uint32_t ui32Base)
- uint32_t I2CMasterErr (uint32_t ui32Base)
- void I2CMasterGlitchFilterConfigSet (uint32_t ui32Base, uint32_t ui32Config)
- void I2CMasterInitExpClk (uint32_t ui32Base, uint32_t ui32I2CClk, bool bFast)
- void I2CMasterIntClear (uint32_t ui32Base)
- void I2CMasterIntClearEx (uint32_t ui32Base, uint32_t ui32IntFlags)
- void I2CMasterIntDisable (uint32_t ui32Base)
- void I2CMasterIntDisableEx (uint32_t ui32Base, uint32_t ui32IntFlags)
- void I2CMasterIntEnable (uint32_t ui32Base)
- void I2CMasterIntEnableEx (uint32_t ui32Base, uint32_t ui32IntFlags)
- bool I2CMasterIntStatus (uint32_t ui32Base, bool bMasked)
- uint32_t I2CMasterIntStatusEx (uint32_t ui32Base, bool bMasked)
- uint32_t I2CMasterLineStateGet (uint32_t ui32Base)
- void I2CMasterSlaveAddrSet (uint32_t ui32Base, uint8_t ui8SlaveAddr, bool bReceive)
- void I2CMasterTimeoutSet (uint32_t ui32Base, uint32_t ui32Value)
- void I2CRxFIFOConfigSet (uint32_t ui32Base, uint32_t ui32Config)
- void I2CRxFIFOFlush (uint32_t ui32Base)
- void I2CSlaveACKOverride (uint32_t ui32Base, bool bEnable)
- void I2CSlaveACKValueSet (uint32_t ui32Base, bool bACK)
- void I2CSlaveAddressSet (uint32_t ui32Base, uint8_t ui8AddrNum, uint8_t ui8SlaveAddr)
- uint32_t I2CSlaveDataGet (uint32_t ui32Base)
- void I2CSlaveDataPut (uint32_t ui32Base, uint8_t ui8Data)
- void I2CSlaveDisable (uint32_t ui32Base)
- void I2CSlaveEnable (uint32_t ui32Base)
- void I2CSlaveFIFODisable (uint32_t ui32Base)
- void I2CSlaveFIFOEnable (uint32_t ui32Base, uint32_t ui32Config)
- void I2CSlaveInit (uint32_t ui32Base, uint8_t ui8SlaveAddr)
- void I2CSlaveIntClear (uint32_t ui32Base)
- void I2CSlaveIntClearEx (uint32_t ui32Base, uint32_t ui32IntFlags)
- void I2CSlaveIntDisable (uint32_t ui32Base)
- void I2CSlaveIntDisableEx (uint32_t ui32Base, uint32_t ui32IntFlags)
- void I2CSlaveIntEnable (uint32_t ui32Base)
- void I2CSlaveIntEnableEx (uint32_t ui32Base, uint32_t ui32IntFlags)
- bool I2CSlaveIntStatus (uint32_t ui32Base, bool bMasked)
- uint32_t I2CSlaveIntStatusEx (uint32_t ui32Base, bool bMasked)
- uint32_t I2CSlaveStatus (uint32_t ui32Base)
- void I2CTxFIFOConfigSet (uint32_t ui32Base, uint32_t ui32Config)
- void I2CTxFIFOFlush (uint32_t ui32Base)


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

