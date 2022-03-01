# TM4C123GXL

###Run Mode Clock Gating Control Register 2 (RCGC2) - enable the clock for that port 

###Lock Register - unlock port. 

###GPIO Commit Register (CR) - 1: receive data 

###GPIO Data Direction Register (DIR) - Tells microcontroller which pin is going to be the input 

#####Input: 0

#####Output: 1

###Pull Up Register (PUR) - Built in Pull-up register in PF0 to PF4. (There is also PDR: Pull down register)

#####What is Pull-Up Resistor? 
######If device is not connected to anything --> neither Low nor High 
######To prevent this, pull-up resistor ensure either the pin is high or low. 

####SYSCTL->RCG2 - 0x0000.0020.    //enable clock for Port F 

###Digital Enable Register (DEN) - Enable Digital Input/Output
