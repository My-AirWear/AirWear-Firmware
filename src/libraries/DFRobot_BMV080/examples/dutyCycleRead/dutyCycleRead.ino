/*!
 * @file dutyCycRead.ino
 * @brief This routine uses the IIC or SPI interface to periodically read data, with a cycle of 20 seconds.
 * @n The obtained data include the current levels of PM1, PM2.5 and PM10 in the air.
 * @n The demo supports FireBeetle-ESP32-E, FireBeetle-ESP32-S3.
 * @details Experimental phenomenon: The read data will be output in the serial port monitor.
 * 
 * @copyright Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [lbx](liubx8023@gmail.com)
 * @version V1.0
 * @date 2025-09-15
 * @url https://github.com/DFRobot/DFRobot_BMV080
 */

#include "DFRobot_BMV080.h"
#include <string>

SET_LOOP_TASK_STACK_SIZE(60 * 1024); // Set the stack size of the loop task to 60KB

//You can choose to use either the IIC interface or the SPI interface. The default is IIC. 
/*
 * address selection of I2C:
 * --------------------------------------
 * |    CSB    |    MISO    |  Address  |
 * --------------------------------------
 * |     0     |     0      |   0x54    |
 * --------------------------------------
 * |     0     |     1      |   0x55    |
 * --------------------------------------
 * |     1     |     0      |   0x56    |
 * --------------------------------------
 * |     1     |     1      |   0x57    |
 * --------------------------------------
 */
DFRobot_BMV080_I2C sensor(&Wire, 0x57); // Create an instance of the DFRobot_BMV080_I2C class with the I2C address 0x57.

/* If you want to use SPI, simply remove the following comments, and change the SPI_CS_PIN to the corresponding SPI CS pin.*/
//#define SPI_CS_PIN 17
//DFRobot_BMV080_SPI sensor(&SPI,SPI_CS_PIN); // Create an instance of the DFRobot_BMV080_SPI class with the SPI CS pin.

#define DUTY_CYCLE_PERIOD 20 // Duty cycle period in seconds. Must be greater than or equal to 12s.
#define INTEGRATION_TIME 10.0f // Integration Time in seconds. Must be greater than or equal to 1.0s and less than DUTY_CYCLE_PERIOD 2s.

void setup() {
  char id[13]; // Variable to store the chip ID of the BMV080 sensor.
  Serial.begin(115200);
  while(!Serial) delay(100); // Wait for the serial port to be ready.
  // Initialize the sensor.
  Serial.println("Periodic reading routine.");
  while(sensor.begin() != 0){
    Serial.println("Initialization of the sensor failed! Please confirm if the sensor chip connection is correct.");
    delay(1000);
  }
  Serial.println("Initialization of the sensor was successful.");
  // Open the BMV080 sensor.
  while(sensor.openBmv080()){
    Serial.println("open failed");
    delay(1000);
  }
  Serial.println("open successful");
  // Get the chip ID of the BMV080 sensor.
  sensor.getBmv080ID(id);
  Serial.println("Chip ID is:" + String(id));

  // Set the duty cycling period of the BMV080 sensor.
  // DUTY_CYCLE_PERIOD must be greater than 12 seconds.
  if(sensor.setDutyCyclingPeriod(DUTY_CYCLE_PERIOD)){
    Serial.println("The measurement period set error!");
  }
  Serial.print("The measurement period is set to:");
  Serial.println(sensor.getDutyCyclingPeriod());

  // Set the integration time of the BMV080 sensor.
  // INTEGRATION_TIME must be less than DUTY_CYCLE_PERIOD by at least 2 seconds.
  if(sensor.setIntegrationTime(INTEGRATION_TIME)){
    Serial.println("The integration time set error!");
  }
  Serial.print("The integration time is set to:");
  Serial.println(sensor.getIntegrationTime());
  
  // Set the obstruction detection feature of the sensor to true.
  /*!
   * @brief Enable the obstruction detection feature of the BMV080 sensor.
   * @n When this function is enabled, if there is any obstruction above the sensor, a prompt message indicating the obstruction will be displayed.
   * @param obstructed Set to true to enable obstruction detection, false to disable it.
   * @return 1 successful, other error.
   */
  sensor.setObstructionDetection(true); 

  // Set the vibration filtering feature of the sensor to true.
  /*!
   * @brief Enable the vibration filtering feature of the BMV080 sensor.
   * @n When this function is enabled, the sensor will filter out vibrations and provide more stable readings.
   * @param do_vibration_filtering Set to true to enable vibration filtering, false to disable it.
   * @return 1 successful, other error.
   */
  sensor.setDoVibrationFiltering(true); 

  // Set the measurement algorithm parameter of the sensor to HIGH_PRECISION.
  /*!
   * @brief Set the measurement algorithm of the BMV080 sensor.
   * @param measurement_algorithm The measurement algorithm to use.
   *                              FAST_RESPONSE //Fast response,suitable for scenarios requiring quick response
   *                              BALANCED //Balanced, suitable for scenarios where a balance needs to be struck between precision and rapid response
   *                              HIGH_PRECISION //High precision, suitable for scenarios requiring high accuracy
   * @return 0 successful, other error.
   */
  if(0 == sensor.setMeasurementAlgorithm(BALANCED)){
    Serial.println("Set measurement algorithm to BALANCED successfully.");
  } else {
    Serial.println("Failed to set measurement algorithm.");
  }

  // Set the measurement mode to continuous mode.
  // In the cycle mode, it must be set to DUTY_CYCLE_MODE.
  /*!
   * @brief Set the measurement mode of the BMV080 sensor.
   * @param mode The mode to set, either CONTINUOUS_MODE or DUTY_CYCLE_MODE
   *              CONTINUOUS_MODE: Sensor takes measurements continuously
   *              DUTY_CYCLE_MODE: Sensor takes measurements at specified intervals
   */
  if(0 == sensor.setBmv080Mode(DUTY_CYCLE_MODE)){
    Serial.println("Mode setting successful");
  }else{
    Serial.println("Mode setting failed");
  }
}

// define the variables to store the PM data.
float pm1,pm2_5,pm10;
void loop() {
  // Read the PM data from the sensor.
  if(sensor.getBmv080Data(&pm1,&pm2_5,&pm10)){
    Serial.print("pm1:" + String(pm1) + " ug/m3  " + "pm2.5:" + String(pm2_5) + " ug/m3  " + "pm10:" + String(pm10) + " ug/m3");

    if(sensor.ifObstructed()){
      Serial.print("  Obstructed The data may be invalid.");
    }

    Serial.println();
  }
  delay(100);
}
