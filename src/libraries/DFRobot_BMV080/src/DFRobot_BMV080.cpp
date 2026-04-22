/*!
 * @file DFRobot_BMV080.cpp
 * @brief Driver for BMV080 sensor
 * @n Support IIC and SPI communication interfaces
 * @n Used the official SDK of Bosch
 * @n Can obtain PM1, PM2.5, PM10
 * @copyright	Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [lbx](liubx8023@gmail.com)
 * @version V1.0
 * @date 2025-09-15
 * @url https://github.com/DFRobot/DFRobot_BMV080
 */

#include "DFRobot_BMV080.h"

int8_t DFRobot_BMV080::bmv080Write16BitCb(bmv080_sercom_handle_t sercom_handle, uint16_t header, const uint16_t* payload, uint16_t payload_length)
{
  DFRobot_BMV080 *_pDev = (DFRobot_BMV080 *)sercom_handle;
  if(_pDev->devClass)
    header = header << 1;
  if(_pDev->writeReg(header, payload, payload_length) != 0){
    DBG("writeReg failed");
    return E_BMV080_ERROR_HW_WRITE; 
  }

  return E_BMV080_OK;
}

int8_t DFRobot_BMV080::bmv080Read16BitCb(bmv080_sercom_handle_t sercom_handle, uint16_t header, uint16_t* payload, uint16_t payload_length)
{
  DFRobot_BMV080 *_pDev = (DFRobot_BMV080 *)sercom_handle;
  if(_pDev->devClass)
    header = header << 1;

  return _pDev->readReg(header, payload, payload_length) == payload_length ? E_BMV080_OK : E_BMV080_ERROR_HW_READ;
}

int8_t DFRobot_BMV080::bmv080DelayCb(uint32_t duration_in_ms)
{
  delay(duration_in_ms);

  return E_BMV080_OK;
}

uint32_t DFRobot_BMV080::bmv080DelayCyclingCb(void)
{
  return millis();
}

void DFRobot_BMV080::getBmv080DataCb(bmv080_output_t bmv080_output, void *cb_parameters)
{
  ((DFRobot_BMV080 *)cb_parameters)->get_bmv080Data(bmv080_output);
}

uint16_t DFRobot_BMV080::openBmv080(void)
{
  bmv080_status_code_t bmv080_status = bmv080_open(
        &_bmv080_handle_class, (bmv080_sercom_handle_t)this, (bmv080_callback_read_t)bmv080Read16BitCb,
        (bmv080_callback_write_t)bmv080Write16BitCb, (bmv080_callback_delay_t)bmv080DelayCb);
  //DBG("status is:" + String(bmv080_status));
  if(bmv080_status == E_BMV080_ERROR_PRECONDITION_UNSATISFIED) {
    DBG("bmv080_open failed, status is:" + String(bmv080_status));
  }
  return bmv080_status;
}

bool DFRobot_BMV080::closeBmv080(void)
{
  bmv080_status_code_t bmv080_status = bmv080_close(&_bmv080_handle_class);
  if(bmv080_status != E_BMV080_OK) {
      DBG("bmv080_close failed, status is:" + String(bmv080_status));
      return false;
  }
  return bmv080_status == E_BMV080_OK;  
}

bool DFRobot_BMV080::stopBmv080(void)
{
  bmv080_status_code_t bmv080_status = bmv080_stop_measurement(_bmv080_handle_class);

  return bmv080_status == E_BMV080_OK;
}

bool DFRobot_BMV080::resetBmv080(void)
{
  bmv080_status_code_t bmv080_status = bmv080_reset(_bmv080_handle_class);

  if(bmv080_status != E_BMV080_OK) {
    DBG("bmv080_reset failed, status is:" + String(bmv080_status));
    return false;
  }
  return bmv080_status == E_BMV080_OK;
}

bool DFRobot_BMV080::getBmv080ID(char *id)
{
  bmv080_status_code_t bmv080_status = bmv080_get_sensor_id(_bmv080_handle_class, id);
  
  return (bmv080_status == E_BMV080_OK);
}

bool DFRobot_BMV080::getBmv080DV(uint16_t &major, uint16_t &minor, uint16_t &patch)
{
  char git_hash[12];
  int32_t num_commits_ahead = 0;

  bmv080_status_code_t bmv080_status = bmv080_get_driver_version(&major, &minor, &patch, git_hash, &num_commits_ahead);

  return (bmv080_status == E_BMV080_OK);
}

bool DFRobot_BMV080::get_bmv080Data(bmv080_output_t bmv080_output)
{
  _bmv080Data = bmv080_output;
  _bmv080DataOK = true;

  return E_BMV080_OK;
}

bool DFRobot_BMV080::getBmv080Data(float *PM1, float *PM2_5, float *PM10, bmv080_output_t *allData)
{
  _bmv080DataOK = false;

  if(_bmv080_handle_class == NULL) {
    DBG("bmv080_handle_class is null 1");
    return false;
  }
  bmv080_status_code_t bmv080_status =
    bmv080_serve_interrupt(_bmv080_handle_class, (bmv080_callback_data_ready_t)getBmv080DataCb, (void *)this);

  if(bmv080_status != E_BMV080_OK) {
    return false;
  }
  if(_bmv080DataOK){
    *PM1 = _bmv080Data.pm1_mass_concentration;
    *PM2_5 = _bmv080Data.pm2_5_mass_concentration;
    *PM10 = _bmv080Data.pm10_mass_concentration;
    if (allData != nullptr) {
      *allData = _bmv080Data;
    }
  }

  return _bmv080DataOK;
}

int DFRobot_BMV080::setBmv080Mode(uint8_t mode)
{
  bmv080_status_code_t bmv080_status = E_BMV080_ERROR_PARAM_INVALID_VALUE;

  if(mode == CONTINUOUS_MODE) {
    bmv080_status = bmv080_start_continuous_measurement(_bmv080_handle_class);
  }else if(mode == DUTY_CYCLE_MODE) {
    bmv080_duty_cycling_mode_t bmv080_duty_cycling_mode = E_BMV080_DUTY_CYCLING_MODE_0;
    bmv080_status = bmv080_start_duty_cycling_measurement(_bmv080_handle_class, (bmv080_callback_tick_t)bmv080DelayCyclingCb, bmv080_duty_cycling_mode);
  } else {
    DBG("setBmv080Mode failed, mode is invalid");
    return -1;
  }
  if(bmv080_status != E_BMV080_OK) {
    if(bmv080_status == E_BMV080_ERROR_PRECONDITION_UNSATISFIED) {
      DBG("setBmv080Mode failed, precondition is unsatisfied");
      return -2;
    }
    DBG("setBmv080Mode failed, status is:" + String(bmv080_status));
    return bmv080_status;
  }

  return 0;
}

int DFRobot_BMV080::setIntegrationTime(float integration_time)
{
  int duty_cycling_period = getDutyCyclingPeriod();
  if(integration_time < 1.0f){
    DBG("setIntegrationTime failed, integration_time is invalid");
    return -1;
  }else if(duty_cycling_period - integration_time < 2){
    DBG("integration_time must less than duty_cycling_period by at least 2 seconds");
    return -2;
  }

  bmv080_status_code_t bmv080_status = bmv080_set_parameter(_bmv080_handle_class, "integration_time", (void *)&integration_time);
  if(bmv080_status != E_BMV080_OK) {
    if(bmv080_status == E_BMV080_ERROR_PRECONDITION_UNSATISFIED) {
      DBG("setIntegrationTime failed, precondition is unsatisfied");
      return -3;
    }
    DBG("setIntegrationTime failed, status is:" + String(bmv080_status));
    return bmv080_status;
  }

  return 0;
}

float DFRobot_BMV080::getIntegrationTime(void)
{
  float integration_time = 0.0f;

  bmv080_status_code_t bmv080_status = bmv080_get_parameter(_bmv080_handle_class, "integration_time", (void *)&integration_time);

  if(bmv080_status != E_BMV080_OK) {
    DBG("setIntegrationTime failed, status is:" + String(bmv080_status));
    return NAN;
  }

  return integration_time;
}

int DFRobot_BMV080::setDutyCyclingPeriod(uint16_t duty_cycling_period)
{
  float integration_time = getIntegrationTime();
  if(duty_cycling_period < 12){
    DBG("setDutyCyclingPeriod failed, duty_cycling_period is invalid");
    return -1;
  }else if(integration_time > duty_cycling_period - 2){
    DBG("duty_cycling_period must be greater than integration_time by at least 2 seconds");
    return -2;
  }
  bmv080_status_code_t bmv080_status =
      bmv080_set_parameter(_bmv080_handle_class, "duty_cycling_period", (void *)&duty_cycling_period);
  if(bmv080_status != E_BMV080_OK) {
    if(bmv080_status == E_BMV080_ERROR_PRECONDITION_UNSATISFIED) {
      DBG("setDutyCyclingPeriod failed, precondition is unsatisfied");
      return -3;
    }
    DBG("setDutyCyclingPeriod failed, status is:" + String(bmv080_status));
    return bmv080_status;
  }
  return 0;
}

uint16_t DFRobot_BMV080::getDutyCyclingPeriod(void)
{
  uint16_t duty_cycling_period = 0;
  bmv080_status_code_t bmv080_status = bmv080_get_parameter(_bmv080_handle_class, "duty_cycling_period", (void *)&duty_cycling_period);

  return (bmv080_status == E_BMV080_OK ? duty_cycling_period : 0);
}

bool DFRobot_BMV080::setObstructionDetection(bool obstructed)
{
  bmv080_status_code_t bmv080_status = bmv080_set_parameter(_bmv080_handle_class, "do_obstruction_detection", (void *)&obstructed);

  return (bmv080_status == E_BMV080_OK);
}

int DFRobot_BMV080::getObstructionDetection(void)
{
  bool obstructed = 0;
  bmv080_status_code_t bmv080_status = bmv080_get_parameter(_bmv080_handle_class, "do_obstruction_detection", (void *)&obstructed);

  return (bmv080_status == E_BMV080_OK ? obstructed : -1);
}

bool DFRobot_BMV080::ifObstructed(void)
{
  return _bmv080Data.is_obstructed;
}
bool DFRobot_BMV080::setDoVibrationFiltering(bool do_vibration_filtering)
{
  bmv080_status_code_t bmv080_status =
      bmv080_set_parameter(_bmv080_handle_class, "do_vibration_filtering", (void *)&do_vibration_filtering);

  return (bmv080_status == E_BMV080_OK);
}

int DFRobot_BMV080::getDoVibrationFiltering(void)
{
  bool do_vibration_filtering = 0;
  bmv080_status_code_t bmv080_status = bmv080_get_parameter(_bmv080_handle_class, "do_vibration_filtering", (void *)&do_vibration_filtering);

  return (bmv080_status == E_BMV080_OK ? do_vibration_filtering : -1);
}

int DFRobot_BMV080::setMeasurementAlgorithm(uint8_t measurement_algorithm)
{
  if(measurement_algorithm > 3){
    DBG("setMeasurementAlgorithm failed, measurement_algorithm is invalid");
    return -1;
  }
  bmv080_measurement_algorithm_t bmv080_measurement_algorithm = (bmv080_measurement_algorithm_t)measurement_algorithm;
  bmv080_status_code_t bmv080_status = bmv080_set_parameter(_bmv080_handle_class, "measurement_algorithm", (void *)&bmv080_measurement_algorithm);
  if(bmv080_status != E_BMV080_OK) {
    if(bmv080_status == E_BMV080_ERROR_PRECONDITION_UNSATISFIED) {
      DBG("setMeasurementAlgorithm failed, precondition is unsatisfied");
      return -3;
    }
    DBG("setMeasurementAlgorithm failed, status is:" + String(bmv080_status));
    return bmv080_status;
  }
  return 0;
}
uint8_t DFRobot_BMV080::getMeasurementAlgorithm(void)
{
  bmv080_measurement_algorithm_t measurement_algorithm;
  bmv080_status_code_t bmv080_status = bmv080_get_parameter(_bmv080_handle_class, "measurement_algorithm", (void *)&measurement_algorithm);

  return (bmv080_status == E_BMV080_OK ? (uint8_t)measurement_algorithm : 0);
}

DFRobot_BMV080_I2C::DFRobot_BMV080_I2C(TwoWire * Wire, uint8_t deviceAddr)
{
  _pWire = Wire;
  _deviceAddr = deviceAddr;
  devClass = true;
}

int DFRobot_BMV080_I2C::begin(void) 
{
  _pWire->begin();
  _pWire->beginTransmission(_deviceAddr);
  if(_pWire == NULL)
  {
    Serial.println("_pWire == NULL");
    return ERR_DATA_BUS;
  }
  return _pWire->endTransmission() == 0 ? ERR_OK : ERR_DATA_BUS; 
}

uint16_t DFRobot_swap16(uint16_t i)
{
  #if defined(__clang__) || defined(__GNUC__)
      return __builtin_bswap16(i);
  #else
      return (i << 8) | (i >> 8);
  #endif
}

uint8_t DFRobot_BMV080_I2C::writeReg(uint16_t reg, const uint16_t* pBuf, size_t size)
{ 
  uint16_t data16[size];

  for (size_t i = 0; i < size; i++)
      data16[i] = DFRobot_swap16(pBuf[i]);
  reg = DFRobot_swap16(reg);

  if(pBuf == NULL){
      DBG("pBuf ERROR!! : null pointer");
      return 1;
  }

  _pWire->beginTransmission(_deviceAddr);
  _pWire->write((uint8_t*)&reg, sizeof(reg));
  _pWire->write((const uint8_t*)data16, size * sizeof(uint16_t));
  uint8_t status = _pWire->endTransmission();

  return status == 0 ? 0 : 1;
}

uint8_t DFRobot_BMV080_I2C::readReg(uint16_t reg, uint16_t* pBuf, size_t size)
{
  if (_pWire == NULL || pBuf == NULL) {
    DBG("Invalid pointer: _pWire or pBuf is NULL");
    return 0;
  }

  reg = DFRobot_swap16(reg);
  _pWire->beginTransmission(_deviceAddr);
  _pWire->write((uint8_t*)&reg, sizeof(reg));
  if (_pWire->endTransmission() != 0) {
    DBG("Failed to send register address");
    return 0;
  }

  size_t totalBytes = size * sizeof(uint16_t);
  uint8_t buffer[32];
  uint16_t* pWord = pBuf;

  while (totalBytes > 0) {
    uint8_t chunkSize = (totalBytes > sizeof(buffer)) ? sizeof(buffer) : totalBytes;

    _pWire->requestFrom(_deviceAddr, chunkSize);

    for (uint8_t i = 0; i < chunkSize; i++) {
      buffer[i] = _pWire->read();
    }

    for (uint8_t i = 0; i < chunkSize; i += sizeof(uint16_t)) {
      *pWord = (buffer[i] << 8) | buffer[i + 1];
      pWord++;
    }
    totalBytes -= chunkSize;
  }

  return size; 
}


DFRobot_BMV080_SPI::DFRobot_BMV080_SPI(SPIClass *spi, uint8_t csPin)
{
  _pSpi = spi;
  _csPin = csPin;
  devClass = false;
}

SPISettings spiSettings;

int DFRobot_BMV080_SPI::begin(void) 
{
  if (_pSpi == NULL) {
    DBG("_pSpi ERROR!! : null pointer");
    return ERR_DATA_BUS;
  }
  pinMode(_csPin, OUTPUT);
  _pSpi->begin();
  spiSettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);
  return ERR_OK; 
}

uint8_t DFRobot_BMV080_SPI::writeReg(uint16_t reg, const uint16_t* pBuf, size_t size)
{
  if (_pSpi == NULL) {
    DBG("pBuf ERROR!! : null pointer");
    return 1;
  }
  
  _pSpi->beginTransaction(spiSettings);
  digitalWrite(_csPin, LOW);
  _pSpi->transfer16(reg);
  
  while(size--) {
    _pSpi->transfer16(*pBuf++);
  }
  
  digitalWrite(_csPin, HIGH);
  _pSpi->endTransaction();

  return 0;
}

uint8_t DFRobot_BMV080_SPI::readReg(uint16_t reg, uint16_t* pBuf, size_t size) 
{
  if (_pSpi == NULL) {
    DBG("_pSpi ERROR!! : null pointer");
    return 0;
  }

  if (pBuf == NULL) {
    DBG("pBuf ERROR!! : null pointer");
    return 0;
  }
  uint16_t * _pBuf = (uint16_t *)pBuf;
  size_t count = 0;

  _pSpi->beginTransaction(spiSettings);
  digitalWrite(_csPin, LOW);

  _pSpi->transfer16(reg);

  while(size--) {
    *_pBuf = _pSpi->transfer16(0x00);
    _pBuf++;
    count++;
  }

   digitalWrite(_csPin, HIGH);
  _pSpi->endTransaction();
 
  return count;
}