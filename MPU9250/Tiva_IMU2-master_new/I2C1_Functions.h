
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"
#include "driverlib/sysctl.h"
#include <math.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include <time.h>

#include <string.h>
#include <ctype.h>




#ifndef __I2C1Functions_H__
#define __I2C1Functions_H__


#define I2C1_ACK_DATA 0x01u;
#define I2C1_NAK_DATA 0x00u;


uint8_t I2C1ReadByte(uint32_t devAddr, uint8_t regAddr);

void I2C1WriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t value);



#endif // __I2C1 Functions_H__
