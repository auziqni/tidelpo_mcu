// library utama
#include <Arduino.h>
// library BMP280 (barometer sensor)
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// global varilale utama
unsigned long lastmillis = 0;
float temperature, pressure, altitude;

// global variable BMP280 (barometer sensor)
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;

// fungsi dari komponen
//-----------------------------------------------------------------------------------------------
void init_barometerSensor()
{
    if (!bmp280.begin(BMP280_I2C_ADDRESS))
    {
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        while (1)
            ;
    }
    Serial.println("Found BMP280 sensor!");
}

void barometerSensor()
{
    temperature = bmp280.readTemperature();
    pressure = bmp280.readPressure() / 100;
    altitude = bmp280.readAltitude(1013.25);
}

// fungsi pendukung
//-----------------------------------------------------------------------------------------------
void monitorSerial()
{
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.print(" °C");

    Serial.print(" | Pressure = ");
    Serial.print(pressure);
    Serial.print(" hPa");

    Serial.print(" | Altitude = ");
    Serial.print(altitude);
    Serial.println(" m");
}

// fungsi utama
//-----------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    init_barometerSensor();
}

void loop()
{
    // rtos
    if ((millis() - lastmillis) > 1000)
    {
        barometerSensor();
        //---------------------
        monitorSerial();
        lastmillis = millis();
    }
}