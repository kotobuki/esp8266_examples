#ifndef HDC1000_H
#define HDC1000_H

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define HDC1000_I2C_ADDRESS 0x40
#define HDC1000_CONFIG 0x02

class HDC1000 {
  public:
    HDC1000();
    void begin();
    void readMeasurements();
    double getTemperature();
    double getHumidity();

  private:
    double _temperature;
    double _humidity;
};

#endif
