
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP280_I2C_ADDRESS 0x76

Adafruit_BMP280 bmp280;

void setup()
{
  Serial.begin(115200);

  Serial.println(F("Arduino + BMP280"));

  if (!bmp280.begin(BMP280_I2C_ADDRESS))
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1)
      ;
  }
  Serial.println("Found BMP280 sensor!");
}

void loop()
{

  float temperature = bmp280.readTemperature();
  float pressure = bmp280.readPressure();
  float altitude_ = bmp280.readAltitude(1013.25);

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure    = ");
  Serial.print(pressure / 100);
  Serial.println(" hPa");

  Serial.print("Approx Altitude = ");
  Serial.print(altitude_);
  Serial.println(" m");

  Serial.println();
  delay(2000);
}