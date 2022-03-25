

#ifndef __IMU_H__
#define __IMU_H__


//#include <stdarg.h>
////#include <stdint.h>
////#include <stdbool.h>
//#include <math.h>
////#include "IMU.h"
//#include "inc/hw_types.h"
//#include "inc/hw_memmap.h"
//#include "inc/hw_gpio.h"
//#include "inc/hw_ints.h"
////#include "inc/hw_i2c.h"
//#include "driverlib/sysctl.h"
//#include "driverlib/pin_map.h"
//#include "driverlib/gpio.h"
//#include "driverlib/interrupt.h"
////#include "driverlib/i2c.h"
////#include "driverlib/i2c.c"
//#include "driverlib/uart.h"
//#include "driverlib/timer.h"
//#include <time.h>
//#include "toggle_timer_interrupt_TivaWare.h"

//#include <string.h>
//#include <ctype.h>

#include "MPU9250.h"


//#include "inc/tm4c123gh6pm.h"


extern void PortFunctionInit(void);
extern void    MPU9250_GetGyro(void);
extern void    MPU9250_GetAcc(void);
extern void		MPU9250_GetComp(void);
extern void MPU9250SelfTest(void);
extern  void magcalMPU9250(void) ;
extern void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);

#endif //  __IMU_H__
