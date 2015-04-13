#ifndef __SI7005__
#define __SI7005__

#include <Energia.h>
#include <Wire.h>

class Si7005 {
  public:
    Si7005(TwoWire *_dev, unsigned long _max_age, boolean _fast = false);
    void begin();

    // call it as often as possible
    void process();

    float getTemperature() const;
    float getHumidity() const;
    boolean getTempValid() const;
    unsigned long getTempAge() const;
    boolean getHumValid() const;
    unsigned long getHumAge() const;
    unsigned char getState() { return state; };
    boolean chipFound() { return chip_found; };
  protected:
  private:
    volatile unsigned char state;
    bool temp_valid, humid_valid, fast, chip_found;
    unsigned short data, th;
    float temperature;
    float humidity;
    volatile unsigned long time_out, last_temp_conv, last_humid_conv, max_age;
    TwoWire *dev;
};

#endif //ndef __SI7005__
