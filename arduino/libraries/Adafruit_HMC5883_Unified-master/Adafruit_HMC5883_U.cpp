/***************************************************************************
  This is a library for the HMC5883 magnentometer/compass

  Designed specifically to work with the Adafruit HMC5883 Breakout
  http://www.adafruit.com/products/1746

  These displays use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#ifdef __AVR_ATtiny85__
  #include "TinyWireM.h"
  #define Wire1 TinyWireM
#else
  #include <Wire.h>
#endif

#include <limits.h>

#include "Adafruit_HMC5883_U.h"

static float _hmc5883_Gauss_LSB_XY = 1100.0F;  // Varies with gain
static float _hmc5883_Gauss_LSB_Z  = 980.0F;   // Varies with gain

/***************************************************************************
 MAGNETOMETER
 ***************************************************************************/
/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
void Adafruit_HMC5883_Unified::write8(byte address, byte reg, byte value)
{
  Wire1.beginTransmission(address);
  #if ARDUINO >= 100
    Wire1.write((uint8_t)reg);
    Wire1.write((uint8_t)value);
  #else
    Wire1.send(reg);
    Wire1.send(value);
  #endif
  Wire1.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
byte Adafruit_HMC5883_Unified::read8(byte address, byte reg)
{
  byte value;

  Wire1.beginTransmission(address);
  #if ARDUINO >= 100
    Wire1.write((uint8_t)reg);
  #else
    Wire1.send(reg);
  #endif
  Wire1.endTransmission();
  Wire1.requestFrom(address, (byte)1);
  #if ARDUINO >= 100
    value = Wire1.read();
  #else
    value = Wire1.receive();
  #endif  
  Wire1.endTransmission();

  return value;
}

/**************************************************************************/
/*!
    @brief  Reads the raw data from the sensor
*/
/**************************************************************************/
void Adafruit_HMC5883_Unified::read()
{
  // Read the magnetometer
  Wire1.beginTransmission((byte)HMC5883_ADDRESS_MAG);
  #if ARDUINO >= 100
    Wire1.write(HMC5883_REGISTER_MAG_OUT_X_H_M);
  #else
    Wire1.send(HMC5883_REGISTER_MAG_OUT_X_H_M);
  #endif
  Wire1.endTransmission();
  Wire1.requestFrom((byte)HMC5883_ADDRESS_MAG, (byte)6);
  
  // Wait around until enough data is available
  while (Wire1.available() < 6);

  // Note high before low (different than accel)  
  #if ARDUINO >= 100
    uint8_t xhi = Wire1.read();
    uint8_t xlo = Wire1.read();
    uint8_t zhi = Wire1.read();
    uint8_t zlo = Wire1.read();
    uint8_t yhi = Wire1.read();
    uint8_t ylo = Wire1.read();
  #else
    uint8_t xhi = Wire1.receive();
    uint8_t xlo = Wire1.receive();
    uint8_t zhi = Wire1.receive();
    uint8_t zlo = Wire1.receive();
    uint8_t yhi = Wire1.receive();
    uint8_t ylo = Wire1.receive();
  #endif
  
  // Shift values to create properly formed integer (low byte first)
  _magData.x = (int16_t)(xlo | ((int16_t)xhi << 8));
  _magData.y = (int16_t)(ylo | ((int16_t)yhi << 8));
  _magData.z = (int16_t)(zlo | ((int16_t)zhi << 8));
  
  // ToDo: Calculate orientation
  _magData.orientation = 0.0;
}

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
 
/**************************************************************************/
/*!
    @brief  Instantiates a new Adafruit_HMC5883 class
*/
/**************************************************************************/
Adafruit_HMC5883_Unified::Adafruit_HMC5883_Unified(int32_t sensorID) {
  _sensorID = sensorID;
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/
 
/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
bool Adafruit_HMC5883_Unified::begin()
{
  // Enable I2C
  Wire1.begin();

  // Enable the magnetometer
  write8(HMC5883_ADDRESS_MAG, HMC5883_REGISTER_MAG_MR_REG_M, 0x00);
  
  // Set the gain to a known level
  setMagGain(HMC5883_MAGGAIN_1_3);

  return true;
}

/**************************************************************************/
/*!
    @brief  Sets the magnetometer's gain
*/
/**************************************************************************/
void Adafruit_HMC5883_Unified::setMagGain(hmc5883MagGain gain)
{
  write8(HMC5883_ADDRESS_MAG, HMC5883_REGISTER_MAG_CRB_REG_M, (byte)gain);
  
  _magGain = gain;
 
  switch(gain)
  {
    case HMC5883_MAGGAIN_1_3:
      _hmc5883_Gauss_LSB_XY = 1100;
      _hmc5883_Gauss_LSB_Z  = 980;
      break;
    case HMC5883_MAGGAIN_1_9:
      _hmc5883_Gauss_LSB_XY = 855;
      _hmc5883_Gauss_LSB_Z  = 760;
      break;
    case HMC5883_MAGGAIN_2_5:
      _hmc5883_Gauss_LSB_XY = 670;
      _hmc5883_Gauss_LSB_Z  = 600;
      break;
    case HMC5883_MAGGAIN_4_0:
      _hmc5883_Gauss_LSB_XY = 450;
      _hmc5883_Gauss_LSB_Z  = 400;
      break;
    case HMC5883_MAGGAIN_4_7:
      _hmc5883_Gauss_LSB_XY = 400;
      _hmc5883_Gauss_LSB_Z  = 255;
      break;
    case HMC5883_MAGGAIN_5_6:
      _hmc5883_Gauss_LSB_XY = 330;
      _hmc5883_Gauss_LSB_Z  = 295;
      break;
    case HMC5883_MAGGAIN_8_1:
      _hmc5883_Gauss_LSB_XY = 230;
      _hmc5883_Gauss_LSB_Z  = 205;
      break;
  } 
}

/**************************************************************************/
/*! 
    @brief  Gets the most recent sensor event
*/
/**************************************************************************/
bool Adafruit_HMC5883_Unified::getEvent(sensors_event_t *event) {
  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  /* Read new data */
  read();
  
  event->version   = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type      = SENSOR_TYPE_MAGNETIC_FIELD;
  event->timestamp = 0;
  event->magnetic.x = _magData.x / _hmc5883_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
  event->magnetic.y = _magData.y / _hmc5883_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
  event->magnetic.z = _magData.z / _hmc5883_Gauss_LSB_Z * SENSORS_GAUSS_TO_MICROTESLA;
  
  return true;
}

/**************************************************************************/
/*! 
    @brief  Gets the sensor_t data
*/
/**************************************************************************/
void Adafruit_HMC5883_Unified::getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy (sensor->name, "HMC5883", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  sensor->version     = 1;
  sensor->sensor_id   = _sensorID;
  sensor->type        = SENSOR_TYPE_MAGNETIC_FIELD;
  sensor->min_delay   = 0;
  sensor->max_value   = 800; // 8 gauss == 800 microTesla
  sensor->min_value   = -800; // -8 gauss == -800 microTesla
  sensor->resolution  = 0.2; // 2 milligauss == 0.2 microTesla
}
