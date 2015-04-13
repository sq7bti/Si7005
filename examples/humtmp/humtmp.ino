#include <Energia.h>
#include <Wire.h>
#include "Si7005.h"

TwoWire i2c(0);

//                     device max_age fast?
Si7005 TempHumidSensor(&i2c, 5000, false);

void setup()
{
  i2c.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);
  delay(1000);
  Serial.print("humtmp: setup done");
}

unsigned long t = millis();
unsigned long cnt;
void loop()
{
  TempHumidSensor.process();
  ++cnt;
  if(((millis() - t) > 500) && TempHumidSensor.getTempValid() && TempHumidSensor.getHumValid()) {
    Serial.print("L:");
    Serial.print(cnt); cnt = 0;
    Serial.print(" S:");
    Serial.print(TempHumidSensor.getState());
    Serial.print("; ");
    Serial.print(TempHumidSensor.getTempAge()/1000);
    Serial.print("s; ");
    Serial.print(TempHumidSensor.getTempValid()?"T:":"t:");
    Serial.print(TempHumidSensor.getTemperature(), 3);
    Serial.print("C; ");
    Serial.print(TempHumidSensor.getHumAge()/1000);
    Serial.print("s; ");
    Serial.print(TempHumidSensor.getHumValid()?"H:":"h:");
    Serial.print(TempHumidSensor.getHumidity(), 3);
    Serial.println("%");
    t = millis();
  }
}

