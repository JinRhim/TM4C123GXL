

//Declarations in header file
#include "IMU2.h"

//*****************************************************************************


//variables:
char buf[200];
char buf1[500];

int16_t CAX, CAY, CAZ; //current acceleration values
int16_t CGX, CGY, CGZ; //current gyroscope values
int16_t CMX, CMY, CMZ; //current magnetometer values

float AXoff1, AYoff1, AZoff1; //accelerometer offset values
float GXoff1, GYoff1, GZoff1; //gyroscope offset values
float MXoff1, MYoff1, MZoff1;


float AX, AY, AZ; //acceleration floats
float GX, GY, GZ; //gyroscope floats
float MX, MY, MZ;

float magScaleX, magScaleY, magScaleZ ;
float magCalibrationX = 0.0f,magCalibrationY=0.0f,magCalibrationZ=0.0f;
float mag_bias[3] = {0.0f, 0.0f, 0.0f}, mag_scale[3] = {0.0f, 0.0f, 0.0f};

float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};
float eInt[3] = {0.0f, 0.0f, 0.0f};  

float deltat = 0.0f, sum = 0.0f;        // integration interval for both filter schemes
//					float GyroMeasError = PI * (40.0f / 180.0f);   // gyroscope measurement error in rads/s (start at 40 deg/s)
//					float GyroMeasDrift = PI * (0.0f  / 180.0f);   // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
					float GyroMeasError = 3.1415927f * (40.0f / 180.0f);   // gyroscope measurement error in rads/s (start at 40 deg/s)
					float GyroMeasDrift = 3.1415927f * (0.0f  / 180.0f);   // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
					float beta = 0.0f;   // compute beta
					float zeta= 0.0f;   // compute zeta, the other free parameter in the Madgwick scheme usually set
					
					#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#					define Ki 0.0f


		


void PortFunctionInit(void) {
    //
    // Enable Peripheral Clocks 
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Enable pin PF1,PF2,PF3 for GPIOOutput
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	 GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	 GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
}



void UART_Init(void){
	
	//
	// Enable Peripheral Clocks 
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	//
	// Enable pin PA0 for UART0 U0RX
	//
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0);

	//
	// Enable pin PA1 for UART0 U0TX
	//
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);

	//
	// Enable pin PB0 for UART1 U1RX
	//
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0);

	//
	// Enable pin PB1 for UART1 U1TX
	//
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);

	
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

}

void I2C_Init(void){

	//
	// Enable Peripheral Clocks 
	//
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);


	//reset module
	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
	//SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);


	//
	// Enable pin PB3 for I2C0 I2C0SDA
	//
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	//
	// Enable pin PB2 for I2C0 I2C0SCL
	//
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);


	// Enable and initialize the I2C0 master module.  Use the system clock for
	// the I2C0 module.  The last parameter sets the I2C data transfer rate.
	// If false the data rate is set to 100kbps and if true the data rate will
	// be set to 400kbps.
	I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);
	 
	//clear I2C FIFOs
	HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;

}

void Timer_Init(int period){
	
	//
	// Enable Peripheral Clocks 
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	// Enable pin PB7 for TIMER0 T0CCP1
	GPIOPinConfigure(GPIO_PB7_T0CCP1);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_7);

	// Enable pin PB6 for TIMER0 T0CCP0
	GPIOPinConfigure(GPIO_PB6_T0CCP0);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
	
  
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); 		// configure for 32-bit timer mode
  TimerLoadSet(TIMER0_BASE, TIMER_A, period -1);      //reload value
	IntPrioritySet(INT_TIMER0A, 0x00);  	 // configure Timer0A interrupt priority as 0
  IntEnable(INT_TIMER0A);    				// enable interrupt 19 in NVIC (Timer0A)
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);      // arm timeout interrupt
  TimerEnable(TIMER0_BASE, TIMER_A);      // enable timer0A
				
}


















//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(unsigned char data){
  UARTCharPut(UART0_BASE, data);
	UARTCharPut(UART1_BASE, data);
}


//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(char *pt){
  while(*pt){
    UART_OutChar(*pt);
    pt++;
  }
}



//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART_OutUDec(unsigned long n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
  if(n >= 10){
    UART_OutUDec(n/10);
    n = n%10;
  }
  UART_OutChar(n+'0'); /* n is between 0 and 9 */
}


	void MPU9250_initialize(){
		
    MPU9250_setSleepDisabled();
    MPU9250_setFullScaleGyroRange(MPU9250_GYRO_FULL_SCALE_500DPS);
    MPU9250_setFullScaleAccelRange(MPU9250_FULL_SCALE_4G);
		MPU9250_ConfigureAcceleromterLowPassFilter();
		
		MPU9250_setSleepDisabled();
    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
		
    I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_INT_PIN_CFG, 0x22); //bypass mode
    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
		
		
		MPU9250SelfTest();
		

    
		MPU9250_setMagPowerDown();
		MPU9250_setFullScaleMagRange();
    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
		MPU9250_setMagFuseROM();
    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay

		
		
		uint8_t buffer1[3];
//    //I2C1ReadBytes(MPU9250_MAG_ADDRESS, AK8963_ASA, 3, buffer); //Sensitivity Adjustment Page 32: https://download.mikroe.com/documents/datasheets/ak8963c-datasheet.pdf
		buffer1[0]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ASAX);
		buffer1[1]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ASAY);
		buffer1[2]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ASAZ);
		
		
			magCalibrationX = ((((float)buffer1[0]) - 128.0f)/256.0f + 1.0f) ; // micro Tesla
			magCalibrationY = ((((float)buffer1[1]) - 128.0f)/256.0f + 1.0f) ; // micro Tesla
			magCalibrationZ = ((((float)buffer1[2]) - 128.0f)/256.0f + 1.0f) ; // micro Tesla 
			
			 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1); // LED on
			//sprintf(buf1,"%f,%f,%f\n",magCalibrationX,magCalibrationY,magCalibrationZ);
 		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0); // LED off
		
		MPU9250_setMagPowerDown();
    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
		MPU9250_setFullScaleMagRange();
    MPU9250_setMagContinousMeasurementMode();
		
		magcalMPU9250() ;
		
	}

	
	
	
	
	
	
// Accelerometer and gyroscope self test; check calibration wrt factory settings
void MPU9250SelfTest() // Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
{
   uint8_t selfTest[6];
   int16_t gAvg[3], aAvg[3], aSTAvg[3], gSTAvg[3];
   float factoryTrim[6];
   uint8_t FS = 0;
   
  //I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, SMPLRT_DIV, 0x00);    // Set gyro sample rate to 1 kHz
  //I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, CONFIG, 0x02);        // Set gyro sample rate to 1 kHz and DLPF to 92 Hz
	 
	 	MPU9250_setFullScaleGyroRange(MPU9250_GYRO_FULL_SCALE_500DPS);
		    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
    MPU9250_setFullScaleAccelRange(MPU9250_FULL_SCALE_4G);
		    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
		MPU9250_clearGyroSelfTest();
		    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
		MPU9250_clearAccelSelfTest();
		    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay


  for( int ii = 0; ii < 200; ii++) {  // get average current values of gyro and acclerometer
  
	MPU9250_GetAcc();
	aAvg[0] += CAX;
  aAvg[1] += CAY;
  aAvg[2] += CAZ;
  
		
	MPU9250_GetGyro();
	gAvg[0] += CMX;
  gAvg[1] += CMY;
  gAvg[2] += CMZ;
  }
  
  for (int ii =0; ii < 3; ii++) {  // Get average of 200 values and store as average current readings
  aAvg[ii] /= 200;
  gAvg[ii] /= 200;
  }
	
	// Configure the accelerometer for self-test
	MPU9250_setGyroSelfTest();
	MPU9250_setAccelSelfTest();
	MPU9250_setFullScaleGyroRange(MPU9250_GYRO_FULL_SCALE_500DPS);
	SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
  MPU9250_setFullScaleAccelRange(MPU9250_FULL_SCALE_4G);
	SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay


 for( int ii = 0; ii < 200; ii++) {  // get average current values of gyro and acclerometer
  
	MPU9250_GetAcc();
	aSTAvg[0] += CAX;
  aSTAvg[1] += CAY;
  aSTAvg[2] += CAZ;
  
		
	MPU9250_GetGyro();
	gSTAvg[0] += CMX;
  gSTAvg[1] += CMY;
  gSTAvg[2] += CMZ;
  }
  
  for (int ii =0; ii < 3; ii++) {  // Get average of 200 values and store as average current readings
  aSTAvg[ii] /= 200;
  gSTAvg[ii] /= 200;
  } 
  
 // Configure the gyro and accelerometer for normal operation
		MPU9250_setFullScaleGyroRange(MPU9250_GYRO_FULL_SCALE_500DPS);
		    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
    MPU9250_setFullScaleAccelRange(MPU9250_FULL_SCALE_4G);
		    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
		MPU9250_clearGyroSelfTest();
		    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
		MPU9250_clearAccelSelfTest();
		    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay

//   
//   // Retrieve accelerometer and gyro factory Self-Test Code from USR_Reg
//   selfTest[0] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_SELF_TEST_Z_ACCEL); // X-axis accel self-test results
//   selfTest[1] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_SELF_TEST_Z_ACCEL); // Y-axis accel self-test results
//   selfTest[2] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_SELF_TEST_Z_ACCEL); // Z-axis accel self-test results
//   selfTest[3] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_SELF_TEST_X_GYRO);  // X-axis gyro self-test results
//   selfTest[4] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_SELF_TEST_Y_GYRO);  // Y-axis gyro self-test results
//   selfTest[5] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_SELF_TEST_Z_GYRO);  // Z-axis gyro self-test results

//  // Retrieve factory self-test value from self-test code reads
//   factoryTrim[0] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[0] - 1.0) )); // FT[Xa] factory trim calculation
//   factoryTrim[1] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[1] - 1.0) )); // FT[Ya] factory trim calculation
//   factoryTrim[2] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[2] - 1.0) )); // FT[Za] factory trim calculation
//   factoryTrim[3] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[3] - 1.0) )); // FT[Xg] factory trim calculation
//   factoryTrim[4] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[4] - 1.0) )); // FT[Yg] factory trim calculation
//   factoryTrim[5] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[5] - 1.0) )); // FT[Zg] factory trim calculation
// 
// // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
// // To get percent, must multiply by 100
// 
// float destination[6];
//   for (int i = 0; i < 3; i++) {
//     destination[i]   = 100.0*((float)(aSTAvg[i] - aAvg[i]))/factoryTrim[i];   // Report percent differences
//     destination[i+3] = 100.0*((float)(gSTAvg[i] - gAvg[i]))/factoryTrim[i+3]; // Report percent differences
//   }
//   
//	 	char buf2[500];
//		UART_OutString("x-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , destination[0]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("y-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , destination[1]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("z-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , destination[2]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("x-axis self test: gyration trim within : "); sprintf(buf2, "%f" , destination[3]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("y-axis self test: gyration trim within : "); sprintf(buf2, "%f" , destination[4]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("z-axis self test: gyration trim within : "); sprintf(buf2, "%f" , destination[5]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//	
}
	








void magcalMPU9250() 
 {
	  
 		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3); // LED on
 uint16_t ii = 0, sample_count = 0;
	 int32_t mag_bias1[3] = {0, 0, 0}, mag_scale1[3] = {0, 0, 0};
 //int16_t mag_max[3] = {0x8000, 0x8000, 0x8000}, mag_min[3] = {0x7FFF, 0x7FFF, 0x7FFF},  mag_temp[3] = {0, 0, 0};
	  int16_t mag_max[3] = {0, 0, 0}, mag_min[3] = {0, 0, 0},  mag_temp[3] = {0, 0, 0};

 UART_OutString("Mag Calibration: Wave device in a figure eight until done!");

 sample_count = 128;
 for(ii = 0; ii < sample_count; ii++) {
	// Read the mag data   
	 
	 MPU9250_GetComp();
	 mag_temp[1] = CMX;
	 mag_temp[1] = CMY;
	 mag_temp[1] = CMZ;
	 for (int jj = 0; jj < 3; jj++) {
		if(mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
		if(mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
	 }
	 SysCtlDelay(SysCtlClockGet() / (8*3));// 125 ms delay
 }

// Get hard iron correction
 mag_bias1[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
 mag_bias1[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
 mag_bias1[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts

 //dest1[0] = (float) mag_bias[0]*mRes*magCalibration[0];  // save mag biases in G for main program
 //dest1[1] = (float) mag_bias[1]*mRes*magCalibration[1];   
 //dest1[2] = (float) mag_bias[2]*mRes*magCalibration[2];  
 
		mag_bias[0] = ((float)mag_bias1[0])* magCalibrationX *1.46484375f;//4800.0f*2.0f*10.0f/65536.0f
		mag_bias[1] = ((float)mag_bias1[1])* magCalibrationY *1.46484375f;//4800.0f*2.0f*10.0f/65536.0f
		mag_bias[2] = ((float)mag_bias1[2])* magCalibrationZ *1.46484375f;//4800.0f*2.0f*10.0f/65536.0f
 

// Get soft iron correction estimate
 mag_scale1[0]  = (mag_max[0] - mag_min[0])/2;  // get average x axis max chord length in counts
 mag_scale1[1]  = (mag_max[1] - mag_min[1])/2;  // get average y axis max chord length in counts
 mag_scale1[2]  = (mag_max[2] - mag_min[2])/2;  // get average z axis max chord length in counts

 float avg_rad = mag_scale1[0] + mag_scale1[1] + mag_scale1[2];
 avg_rad /= 3.0f;

 mag_scale[0] = avg_rad/((float)mag_scale1[0]);
 mag_scale[1] = avg_rad/((float)mag_scale1[1]);
 mag_scale[2] = avg_rad/((float)mag_scale1[2]);

 UART_OutString("Mag Calibration done!");
 
 //sprintf(buf,"%f,%f,%f,%f,%f,%f\n",mag_scale[0],mag_scale[1],mag_scale[2], mag_bias[0],mag_bias[1],mag_bias[2]);
 //UART_OutString(buf);
 
 		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0); // LED off
 }
 
 
 
 
 
 
 


	
	
	
	
	



uint8_t buffer[6];

void    MPU9250_GetGyro(){
		buffer[0]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_XOUT_H);
		buffer[1]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_XOUT_L);
		buffer[2]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_YOUT_H);
		buffer[3]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_YOUT_L);
		buffer[4]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_ZOUT_H);
		buffer[5]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_ZOUT_L);
	
    CGX = (int16_t) ((((int16_t)buffer[0]) << 8) | buffer[1]);
    CGY = (int16_t) ((((int16_t)buffer[2]) << 8) | buffer[3]);
    CGZ = (int16_t) ((((int16_t)buffer[4]) << 8) | buffer[5]);
	
}


void    MPU9250_GetAcc(){
		buffer[0]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_XOUT_H);
		buffer[1]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_XOUT_L);	
		buffer[2]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_YOUT_H);
		buffer[3]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_YOUT_L);	
		buffer[4]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_ZOUT_H);
		buffer[5]=I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_ZOUT_L);
	
    CAX = (int16_t) ( (((int16_t)buffer[0]) << 8) | buffer[1]);
    CAY = (int16_t) ( (((int16_t)buffer[2]) << 8) | buffer[3]);
    CAZ = (int16_t) ( (((int16_t)buffer[4]) << 8) | buffer[5]);
}
void		MPU9250_GetComp(){
	
		buffer[0]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_XOUT_H);
		buffer[1]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_XOUT_L);	
		buffer[2]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_YOUT_H);
		buffer[3]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_YOUT_L);	
		buffer[4]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ZOUT_H);
		buffer[5]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ZOUT_L);
		I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ST2);//Read b/c Datasheet
		
	
    CMX = (int16_t) ( (((int16_t)buffer[2]) << 8) | buffer[3]);
    CMY = (int16_t) ( (((int16_t)buffer[0]) << 8) | buffer[1]);
    CMZ = -((int16_t) ( (((int16_t)buffer[4]) << 8) | buffer[5]));
}



void Calibrate(int trials){


	 /* Calibrate Offset Values */
    UART_OutString("Starting to calibrate values from sensor..\n\r");
    
    //Count and average the first n values, defined by numberOfTests above.. 
    for(int i=0; i<trials; i++)
    {
        sprintf(buf, "CALIBRATING... %d \n\r", i);
        UART_OutString(buf);	     
			
				MPU9250_GetGyro();
				MPU9250_GetAcc();
				//MPU9250_GetComp();        
        
        AXoff1 += CAX;
        AYoff1 += CAY;
        AZoff1 += CAZ;
        GXoff1 += CGX;
        GYoff1 += CGY;
        GZoff1 += CGZ;
        
        SysCtlDelay(SysCtlClockGet()/200/3);
    }

    AXoff1 = AXoff1/trials;
    AYoff1 = AYoff1/trials;
    AZoff1 = AZoff1/trials;
    GXoff1 = GXoff1/trials;
    GYoff1 = GYoff1/trials;
    GZoff1 = GZoff1/trials;
		
    UART_OutString("\n\nTest finished, offset values are shown below\n\r");
		
    sprintf(buf, "MPU9250: AXoff:%f, AYoff:%f, AZoff:%f || GXoff:%f, GYoff:%f, GZoff:%f\n\r", AXoff1,AYoff1,AZoff1,GXoff1,GYoff1,GZoff1);
    UART_OutString(buf);
    SysCtlDelay(SysCtlClockGet()/3);

}


int main(void){
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	PortFunctionInit();
	I2C_Init();
	UART_Init();
	
	MPU9250_initialize();
	//Calibrate(10);
		
	//Timer_Init(SysCtlClockGet()/100);
	//Timer_Init(SysCtlClockGet());
	
	UART_OutString("Ready\n\r");
	
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3); //blink LED
	SysCtlDelay(SysCtlClockGet() / (10 * 3)); //delay ~100 msec
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0); //turn off LED
	
	//IntMasterEnable(); //enable processor interrupts
	
	
	while (1) {

			MPU9250_GetAcc();
			MPU9250_GetGyro();
			MPU9250_GetComp();
		

		
			//Coefficient in g's for -4G to + 4G range.	
			AX = ((float)CAX)*1.1975f/1000;
			AY = ((float)CAY)*1.1975f/1000;
			AZ = ((float)CAZ)*1.1975f/1000;

			//sprintf(buf1, "%f,%f,%f,",AX,AY,AZ);
			//UART_OutString(buf1);

			
			
			
			GX = ((float)CGX)*.0152590219f;
			GY = ((float)CGY)*.0152590219f;
			GZ = ((float)CGZ)*.0152590219f;	

			//sprintf(buf1, "%f,%f,%f,",GX,GY,GZ);
			//UART_OutString(buf1);

			
			MX = ((float)CMX)* magCalibrationX *4800.0f*2.0f*10.0f/65536.0f - mag_bias[0]; //   +/-4800uT range and 16 bits and 10mG per 1uT is the Res for ADC
			MY = ((float)CMY)* magCalibrationY *4800.0f*2.0f*10.0f/65536.0f - mag_bias[1];
			MZ = ((float)CMZ)* magCalibrationZ *4800.0f*2.0f*10.0f/65536.0f - mag_bias[2];

			
			//sprintf(buf1, "%f,%f,%f\n",MX,MY,MZ);
			//sprintf(buf1, "%f,%f,%f\r",MX,MY,MZ);
			//UART_OutString(buf1);
		
			SysCtlDelay(SysCtlClockGet()/3);
   }
	
}

