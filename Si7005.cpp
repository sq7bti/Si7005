/*******************************************************

SI7005 I2C address == 0x40

*******************************************************/
#include "Si7005.h"

float Si7005::getTemperature() const {
  return temperature;
};

float Si7005::getHumidity() const {
  return humidity;
};

boolean Si7005::getTempValid() const {
  return temp_valid;
};

unsigned long Si7005::getTempAge() const {
  return millis() - last_temp_conv;
};

boolean Si7005::getHumValid() const {
  return humid_valid;
};

unsigned long Si7005::getHumAge() const {
  return millis() - last_humid_conv;
};

Si7005::Si7005(TwoWire *_dev, unsigned long _max_age, boolean _fast)
  : dev(_dev), state(0), temp_valid(false), humid_valid(false), data(0), th(true),
    temperature(0.0), humidity(0.0), max_age(max(70,_max_age)), fast(false)
{
  chip_found = false;
};

void Si7005::begin() {
  dev->requestFrom(0x40, 1, true);
  unsigned long timeout = millis() + 5;
  while(dev->available() && (millis() < timeout)) {
    dev->read();
    chip_found = true;
  };
  dev->endTransmission();

  last_temp_conv = millis();
  last_humid_conv = millis();
};

void Si7005::process() {
  if(!chip_found)
    return;
  switch(state) {
    case 0:
      if((millis() - last_humid_conv) > max_age) {
        th = false;
        ++state;
      } else {
        if((millis() - last_temp_conv) > max_age) {
          th = true;
          ++state;
        }
      }
    break;
    case 1:
      dev->beginTransmission(0x40);
      ++state;
    break;
    case 2:
      dev->write(0x03); // set pointer to register number 3, CONFIG
      ++state;
    break;
    case 3:
      dev->write(((th)?0x11:0x01) | ((fast)?0x20:0x00)); // write data to register, set start and temp
      ++state;
    break;
    case 4:
      dev->endTransmission();
      time_out = millis() + ((fast)?18:35);
      ++state;
    break;
    case 5:
      // wait for end of conversion 35ms
      if(millis() > time_out)
        ++state;
    break;
    case 6:
      dev->beginTransmission(0x40);
      ++state;
    break;
    case 7:
      dev->write(0x00); // set pointer to register number 0, STATUS
      ++state;
    break;
    case 8:
      dev->requestFrom(0x40, 3);
      time_out = millis() + ((fast)?18:35);
      ++state;
    break;
    case 9:
      if(dev->available()){
        data = dev->read();
        data &= 0x01;
        if(th) {
          temp_valid = (data == 0x00);
        } else {
          humid_valid = (data == 0x00);
        }
        time_out = millis() + ((fast)?18:35);
        ++state;
      }
      if(millis() > time_out) {
        dev->endTransmission();
        state = 0;
      }
    break;
    case 10:
      if(dev->available()){
        data = dev->read();
        data <<= 8;
        time_out = millis() + ((fast)?18:35);
        ++state;
      }
      if(millis() > time_out) {
        dev->endTransmission();
        state = 0;
      }
    break;
    case 11:
      if(dev->available()){
        data += dev->read();
        dev->endTransmission();
        time_out = millis() + ((fast)?18:35);
        ++state;
      }
      if(millis() > time_out) {
        dev->endTransmission();
        state = 0;
      }
    break;
    case 12:
      if(th) {
        if(temp_valid) {
          data >>= 2;
          temperature = (data/32.0) - 50.0;
        };
        last_temp_conv = millis();
      } else {
        if(humid_valid) {
          data >>= 4;
          humidity = (data/16.0) - 24.0;
          // RHlin = RHv - (A2 * RHv^2 + A1 * RHv + A0)
          // A0 = -4.7844
          // A1 =  0.4008
          // A2 = -0.00393
  //          humidity -= -4.7844 + humidity * (0.4008 + humidity * (-0.00393));
        };
        last_humid_conv = millis();
      }
      time_out = millis() + fast?18:35;
      ++state;
    break;
    case 13:
      // wait for end of conversion 35ms
      if(millis() > time_out)
        state = 0;
    break;
    default:
      state = 0;
    break;
  };
};

