#include "MPU9250.h"



//uint8_t buffer[256];
//float magScaleX, magScaleY, magScaleZ ;


////float magCalibration[3] = {0, 0, 0}, magbias[3] = {0, 0, 0};  // Factory mag calibration and mag bias
////float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};      // Bias corrections for gyro and accelerometer
////float   SelfTest[6];    // holds results of gyro and accelerometer self test



////float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
////float eInt[3] = {0.0f, 0.0f, 0.0f};       // vector to hold integral error for Mahony method

////float magBias[3],magScale[3];



//void MPU9250_initialize() {	
//	
////float M_PI = acos(-1.0);
////float GyroMeasError = M_PI*(40.0f / 180.0f);
////float GyroMeasDrift = M_PI* (0.0f  / 180.0f); 

////float beta = sqrt(3.0f / 4.0f) * GyroMeasError;   // compute beta
////float zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift;   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value

//    MPU9250_setSleepDisabled();
//    MPU9250_setFullScaleGyroRange(MPU9250_GYRO_FULL_SCALE_500DPS);
//    MPU9250_setFullScaleAccelRange(MPU9250_FULL_SCALE_4G);
//    
//    /* Configure Low Pass Filter*/
//    uint8_t* c;
//    I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG2, c);
//    *c = *c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
//    *c = *c | 0x01;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
//    I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG2, *c); // Write new ACCEL_CONFIG2 register value
//    
//    I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_PWR_MGMT_1, 0x00);
//    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
//    I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_INT_PIN_CFG, 0x22); //bypass mode
//    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
//    
//    I2C1WriteByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_CNTL, AK8963_PWR_DOWN);//power down
//    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
//    I2C1WriteByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_CNTL, AK8963_FUSE_ROM);//read factory calibration values
//    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay

//    //I2C1ReadBytes(MPU9250_MAG_ADDRESS, AK8963_ASA, 3, buffer); //Sensitivity Adjustment Page 32: https://download.mikroe.com/documents/datasheets/ak8963c-datasheet.pdf
//		buffer[0]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ASAX, false);
//		buffer[1]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ASAY, false);
//		buffer[2]=I2C1ReadByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_ASAZ, false);
////		magScaleX = ((((float)buffer[0]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
////    magScaleY = ((((float)buffer[1]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla
////    magScaleZ = ((((float)buffer[2]) - 128.0f)/(256.0f) + 1.0f) * 4912.0f / 32760.0f; // micro Tesla 
////			magScaleX = ((((float)buffer[0]) - 128.0f)/(256.0f) + 1.0f) ; // micro Tesla
////			magScaleY = ((((float)buffer[1]) - 128.0f)/(256.0f) + 1.0f) ; // micro Tesla
////			magScaleZ = ((((float)buffer[2]) - 128.0f)/(256.0f) + 1.0f) ; // micro Tesla 


//    I2C1WriteByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_CNTL, AK8963_PWR_DOWN);//power down
//    SysCtlDelay(SysCtlClockGet() / (10*3));// 100 ms delay
//    I2C1WriteByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_CNTL, AK8963_CNT_MEAS2);//continuous measurement mode
//		
//		
//		MPU9250SelfTest(false);
//    
//	//char buf2[32];
////		UART_OutString("x-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , SelfTest[0]); UART_OutString(buf2); UART_OutString("% of factory value\n");
////    UART_OutString("y-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , SelfTest[1]); UART_OutString(buf2); UART_OutString("% of factory value\n");
////    UART_OutString("z-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , SelfTest[2]); UART_OutString(buf2); UART_OutString("% of factory value\n");
////    UART_OutString("x-axis self test: gyration trim within : "); sprintf(buf2, "%f" , SelfTest[3]); UART_OutString(buf2); UART_OutString("% of factory value\n");
////    UART_OutString("y-axis self test: gyration trim within : "); sprintf(buf2, "%f" , SelfTest[4]); UART_OutString(buf2); UART_OutString("% of factory value\n");
////    UART_OutString("z-axis self test: gyration trim within : "); sprintf(buf2, "%f" , SelfTest[5]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//// 
//		
//}





//// Accelerometer and gyroscope self test; check calibration wrt factory settings
//void MPU9250SelfTest(float * destination) // Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
//{
//   uint8_t rawData[6] = {0, 0, 0, 0, 0, 0};
//   uint8_t selfTest[6];
//   int16_t gAvg[3], aAvg[3], aSTAvg[3], gSTAvg[3];
//   float factoryTrim[6];
//   uint8_t FS = 0;
//   
//  I2C1WriteByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);    // Set gyro sample rate to 1 kHz
//  I2C1WriteByte(MPU9250_ADDRESS, CONFIG, 0x02);        // Set gyro sample rate to 1 kHz and DLPF to 92 Hz
//  //I2C1WriteByte(MPU9250_ADDRESS, GYRO_CONFIG, 1<<FS);  // Set full scale range for the gyro to 250 dps
//	I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_CONFIG, MPU9250_GYRO_FULL_SCALE_500DPS);
//  I2C1WriteByte(MPU9250_ADDRESS, ACCEL_CONFIG2, 0x02); // Set accelerometer rate to 1 kHz and bandwidth to 92 Hz
//  //I2C1WriteByte(MPU9250_ADDRESS, ACCEL_CONFIG, 1<<FS); // Set full scale range for the accelerometer to 2 g
//	I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG, MPU9250_FULL_SCALE_4G);

//  for( int ii = 0; ii < 200; ii++) {  // get average current values of gyro and acclerometer
//  
//	rawData[0] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_XOUT_H  , false);
//	rawData[1] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_XOUT_L  , false);
//	rawData[2] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_YOUT_H  , false);
//	rawData[3] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_YOUT_L  , false);
//	rawData[4] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_ZOUT_H  , false);
//	rawData[5] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_ZOUT_L  , false);
//  //I2C1ReadBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);        // Read the six raw data registers into data array
//  aAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
//  aAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;  
//  aAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ; 
//  
//		
//	rawData[0] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_XOUT_H  , false);
//	rawData[1] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_XOUT_L  , false);
//	rawData[2] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_YOUT_H  , false);
//	rawData[3] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_YOUT_L  , false);
//	rawData[4] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_ZOUT_H  , false);
//	rawData[5] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_ZOUT_L  , false);
//  //I2C1ReadBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);       // Read the six raw data registers sequentially into data array
//  gAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
//  gAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;  
//  gAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ; 
//  }
//  
//  for (int ii =0; ii < 3; ii++) {  // Get average of 200 values and store as average current readings
//  aAvg[ii] /= 200;
//  gAvg[ii] /= 200;
//  }
//	
//	// Configure the accelerometer for self-test
//   I2C1WriteByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0xE0 | MPU9250_FULL_SCALE_4G); // Enable self test on all three axes and set accelerometer range to +/- 4 g
//   I2C1WriteByte(MPU9250_ADDRESS, GYRO_CONFIG,  0xE0 | MPU9250_GYRO_FULL_SCALE_500DPS); // Enable self test on all three axes and set gyro range to +/- 500 degrees/s
//   //delay(25);  // Delay a while to let the device stabilize
//	 SysCtlDelay(SysCtlClockGet()/200/3);

//  for( int ii = 0; ii < 200; ii++) {  // get average self-test values of gyro and acclerometer
//		
//	rawData[0] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_XOUT_H  , false);
//	rawData[1] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_XOUT_L  , false);
//	rawData[2] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_YOUT_H  , false);
//	rawData[3] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_YOUT_L  , false);
//	rawData[4] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_ZOUT_H  , false);
//	rawData[5] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, ACCEL_ZOUT_L  , false);
//  //readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
//  aSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
//  aSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;  
//  aSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ; 
//  
//	rawData[0] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_XOUT_H  , false);
//	rawData[1] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_XOUT_L  , false);
//	rawData[2] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_YOUT_H  , false);
//	rawData[3] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_YOUT_L  , false);
//	rawData[4] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_ZOUT_H  , false);
//	rawData[5] = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, GYRO_ZOUT_L  , false);
//    //readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
//  gSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
//  gSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;  
//  gSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ; 
//  }
//  
//  for (int ii =0; ii < 3; ii++) {  // Get average of 200 values and store as average self-test readings
//  aSTAvg[ii] /= 200;
//  gSTAvg[ii] /= 200;
//  }   
//  
// // Configure the gyro and accelerometer for normal operation
//   I2C1WriteByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00);  
//   I2C1WriteByte(MPU9250_ADDRESS, GYRO_CONFIG,  0x00);  
//   //delay(25);  // Delay a while to let the device stabilize
//		SysCtlDelay(SysCtlClockGet()/200/3);
//   
//   // Retrieve accelerometer and gyro factory Self-Test Code from USR_Reg
//   selfTest[0] = I2C1ReadByte(MPU9250_ADDRESS, SELF_TEST_X_ACCEL, false); // X-axis accel self-test results
//   selfTest[1] = I2C1ReadByte(MPU9250_ADDRESS, SELF_TEST_Y_ACCEL, false); // Y-axis accel self-test results
//   selfTest[2] = I2C1ReadByte(MPU9250_ADDRESS, SELF_TEST_Z_ACCEL, false); // Z-axis accel self-test results
//   selfTest[3] = I2C1ReadByte(MPU9250_ADDRESS, SELF_TEST_X_GYRO, false);  // X-axis gyro self-test results
//   selfTest[4] = I2C1ReadByte(MPU9250_ADDRESS, SELF_TEST_Y_GYRO, false);  // Y-axis gyro self-test results
//   selfTest[5] = I2C1ReadByte(MPU9250_ADDRESS, SELF_TEST_Z_GYRO, false);  // Z-axis gyro self-test results

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
//   for (int i = 0; i < 3; i++) {
//     destination[i]   = 100.0*((float)(aSTAvg[i] - aAvg[i]))/factoryTrim[i];   // Report percent differences
//     destination[i+3] = 100.0*((float)(gSTAvg[i] - gAvg[i]))/factoryTrim[i+3]; // Report percent differences
//   }
//   
//	 	char buf2[64];
//		UART_OutString("x-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , destination[0]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("y-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , destination[1]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("z-axis self test: acceleration trim within : "); sprintf(buf2, "%f" , destination[2]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("x-axis self test: gyration trim within : "); sprintf(buf2, "%f" , destination[3]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("y-axis self test: gyration trim within : "); sprintf(buf2, "%f" , destination[4]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//    UART_OutString("z-axis self test: gyration trim within : "); sprintf(buf2, "%f" , destination[5]); UART_OutString(buf2); UART_OutString("% of factory value\n");
//	
//}



void MPU9250_setGyroSelfTest() {
	
	uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS,  MPU9250_GYRO_CONFIG );
		a = a | 0xE0; //b1110 0000
	I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_CONFIG, a);
}

void MPU9250_setAccelSelfTest(){
	
	uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS,  MPU9250_ACCEL_CONFIG );
		a = a | 0xE0; //b1110 0000
    I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG, a);
}


void MPU9250_clearGyroSelfTest() {
	
	uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS,  MPU9250_GYRO_CONFIG );
		a = a & ~ 0xE0; //b1110 0000
	I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_CONFIG, a);
}

void MPU9250_clearAccelSelfTest(){
	
	uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS,  MPU9250_ACCEL_CONFIG );
		a = a & ~ 0xE0; //b1110 0000
    I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG, a);
}




void MPU9250_setFullScaleGyroRange(uint8_t range) {
	
	uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS,  MPU9250_GYRO_CONFIG );
	
	if(range == MPU9250_GYRO_FULL_SCALE_250DPS){
		a = a & ~ 0x18; //b0001 1000
	} else if(range ==  MPU9250_GYRO_FULL_SCALE_500DPS){
		a = a & ~ 0x10; //b0001 0000
		a = a | 0x08; //b0000 1000
	} else if(range == MPU9250_GYRO_FULL_SCALE_1000DPS){
		a = a & ~ 0x08; //b0000 1000
		a = a | 0x10; //b0001 0000
	} else if(range == MPU9250_GYRO_FULL_SCALE_2000DPS){
		a = a | 0x18; //b0001 1000
	}
	
	I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_CONFIG, a);
}

void MPU9250_setFullScaleAccelRange(uint8_t range){
	
	uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS,  MPU9250_ACCEL_CONFIG );
	
	if(range == MPU9250_FULL_SCALE_2G ){
		a = a & ~ 0x18; //b0001 1000
	} else if(range == MPU9250_FULL_SCALE_4G){
		a = a & ~ 0x10; //b0001 0000
		a = a | 0x08; //b0000 1000
	} else if(range == MPU9250_FULL_SCALE_8G){
		a = a & ~ 0x08; //b0000 1000
		a = a | 0x10; //b0001 0000
	} else if(range == MPU9250_FULL_SCALE_16G){
		a = a | 0x18; //b0001 1000
	}
	
    I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG, a);
}

uint8_t MPU9250_getFullScaleAccelRange(){
	//return I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, 0x1A, false);
	//return I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_WHO_AM_I , false);
	return I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG);
	//return I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_GYRO_CONFIG  , false);
}


void MPU9250_setFullScaleMagRange(){
		uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_MAG_CNTL);
		a = a | 0x10; //b0001 0000 // 16 bit range
    I2C1WriteByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_CNTL, a);
}

void MPU9250_setMagContinousMeasurementMode(){
		uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_MAG_CNTL);
	  a = a & ~ 0x09; //b0000 1001
		a = a | 0x06; //b0000 0110
    I2C1WriteByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_CNTL, a);
}


void MPU9250_setMagPowerDown(){
		uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_MAG_CNTL);
	  a = a & ~ 0x0F; //b0000 1111
    I2C1WriteByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_CNTL, a);
}

void MPU9250_setMagFuseROM(){
		uint8_t a = I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_MAG_CNTL);
	  a = a | 0x0F; //b0000 1111
    I2C1WriteByte(MPU9250_MAG_ADDRESS, MPU9250_MAG_CNTL, a);
}


void MPU9250_setSleepDisabled() {
    I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_PWR_MGMT_1, 0x00);
}

void MPU9250_ConfigureAcceleromterLowPassFilter(){
	    /* Configure Low Pass Filter*/
			uint8_t c;
			I2C1ReadByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG2);
			c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
			c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
			I2C1WriteByte(MPU9250_DEFAULT_ADDRESS, MPU9250_ACCEL_CONFIG2, c); // Write new ACCEL_CONFIG2 register value
}


