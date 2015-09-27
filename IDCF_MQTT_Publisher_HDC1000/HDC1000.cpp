#include "HDC1000.h"
#include "Wire.h"

HDC1000::HDC1000() {

}

void HDC1000::begin() {
  Wire.beginTransmission(HDC1000_I2C_ADDRESS);
  Wire.write(HDC1000_CONFIG);
  Wire.write(0x10); // both temperature and humidity in 14 bit resolution
  Wire.write(0x00);
  Wire.endTransmission();
}

void HDC1000::readMeasurements() {
  Wire.beginTransmission(HDC1000_I2C_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(15);
  Wire.requestFrom(HDC1000_I2C_ADDRESS, 4);
  if (4 <= Wire.available()) {
    uint16_t rawValue = 0;
    rawValue = (Wire.read() << 8);
    rawValue += Wire.read();
    _temperature = (rawValue / 65536.0) * 165.0 - 40.0;

    rawValue = (Wire.read() << 8);
    rawValue += Wire.read();
    _humidity = (rawValue / 65536.0) * 100.0;
  }
}

double HDC1000::getTemperature() {
  return _temperature;
}

double HDC1000::getHumidity() {
  return _humidity;
}

