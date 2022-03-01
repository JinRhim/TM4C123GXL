# TM4C123GXL

### 1. Run Mode Clock Gating Control Register 2 (RCGC2) - enable the clock for that port 

##### SYSCTL->RCG2 - 0x0000.0020.    //enable clock for Port F 

### 2. Lock Register - unlock port. 

### 3. GPIO Commit Register (CR) - 1: receive data 

### 4. GPIO Data Direction Register (DIR) - Tells microcontroller which pin is going to be the input 

##### Input: 0

##### Output: 1

### 5. Pull Up Register (PUR) - Built in Pull-up register in PF0 to PF4. (There is also PDR: Pull down register)

##### What is Pull-Up Resistor? 
###### If device is not connected to anything --> neither Low nor High 
###### To prevent this, pull-up resistor ensure either the pin is high or low. 
 

### 6. Digital Enable Register (DEN) - Enable Digital Input/Output






1)Enable Port F Clock 

SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF 

+ Dummy cycle for clock 

ui32Loop = SYSCTL_RCGC2_R 


2)Unlock GPIO Port F 

GPIO_PORTF_LOCK_R = 0x4C4F434B

3) Change Commit Register

GPIO_PORTF_CR_R |= 0x01             //allow change to PF0 


4) Direction (DIR) Register - Set PF2 to output

GPIO_PORTF_DIR_R |= 0x04.           //0x0000.0100 --> PF2:1  PF1:0 PF0:0 

5) Enable PF2 for digital function 

GPIO_PORTF_DEN_R |= 0x04 

6) Enable Pull-up on PF0 

GPI0_PORTF_PUR_R |= 0x04.          //0x0000.0100 --> enable pull up. 

