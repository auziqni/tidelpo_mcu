// library utama
#include <Arduino.h>

// library MPU6050_light (angel sensor)
#include "Wire.h"
#include <MPU6050_light.h>

// library BMP280 (barometer sensor)
// -----#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// library TinyGPS++ (location sensor)
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// global varilale utama
unsigned long lastmillis = 0;
float angelx, angely, angelz;
float temperature, pressure, altitude;
float valCurrent, raw_val_currentsens;
float latitude, longitude;
float valSoil, raw_val_soil;
float valVoltage, raw_val_voltagesens;

// global variable MPU6050_light (angel sensor)
MPU6050 mpu(Wire);

// global variable BMP280 (barometer sensor)
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;

// global variable current sensor
int currentsens_pin = 12;

// global variable TinyGPS++ (location sensor)
static const int RXPin = 36, TXPin = 39;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
bool gpsError = false;
bool gpsReady = false;

// global variable soil sensor
int soil_pin = 15;

// global variable voltage sensor
int voltagesens_pin = 13;

// fungsi dari komponen
//-----------------------------------------------------------------------------------------------
void init_angelSensor()
{
    Wire.begin();

    byte status = mpu.begin();
    Serial.print(F("MPU6050 status: "));
    Serial.println(status);
    while (status != 0)
    {
    } // stop everything if could not connect to MPU6050

    Serial.println(F("Calculating offsets, do not move MPU6050"));
    delay(1000);
    // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
    // mpu.calcOffsets(); // gyro and accelero

    mpu.setGyroOffsets(-8.53, 1.56, -0.80); // gyro only
    mpu.setAccOffsets(0.04, 0.00, 0.14);    // accelero only
    Serial.println("Done!\n");

    Serial.print("gyro x offsets are: ");
    Serial.println(mpu.getGyroXoffset());
    Serial.print("gyro y offsets are: ");
    Serial.println(mpu.getGyroYoffset());
    Serial.print("gyro z offsets are: ");
    Serial.println(mpu.getGyroZoffset());
    Serial.println("");

    Serial.print("acc x offsets are: ");
    Serial.println(mpu.getAccXoffset());
    Serial.print("acc y offsets are: ");
    Serial.println(mpu.getAccYoffset());
    Serial.print("acc z offsets are: ");
    Serial.println(mpu.getAccZoffset());
    Serial.println("");
    delay(5000);
}

void angelSensor()
{
    mpu.update();
    angelx = mpu.getAngleX();
    angely = mpu.getAngleY();
    angelz = mpu.getAngleZ();
}

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

void init_currentSensor()
{
    pinMode(currentsens_pin, INPUT);
}

void currentSensor()
{
    raw_val_currentsens = analogRead(currentsens_pin);

    valCurrent = ((raw_val_currentsens - 1241) / 3082) * 5000; // 2.5v 5a min 1v
    if (valCurrent < 0)
    {
        valCurrent = 0;
    }
}

void init_locationSensor()
{
    ss.begin(GPSBaud);
}

void locationSensor()
{
    if (gps.charsProcessed() < 10)
    {
        gpsError = true;
    }

    while (ss.available() > 0)
    {
        gpsError = false;
        byte gpsData = ss.read();
        // Serial.write(gpsData);

        gps.encode(gpsData);
        if (gps.location.isValid())
        {
            gpsReady = true;
            latitude = gps.location.lat();
            longitude = gps.location.lng();
        }
        else
        {
            gpsReady = false;
        }
    }
}

void init_soilSensor()
{
    pinMode(soil_pin, INPUT);
}

void soilSensor()
{
    raw_val_soil = analogRead(soil_pin);

    valSoil = ((4095 - raw_val_soil) / 4095) * 100;
}

void init_voltageSensor()
{
    pinMode(voltagesens_pin, INPUT);
}

void voltageSensor()
{
    raw_val_voltagesens = analogRead(voltagesens_pin);

    valVoltage = (raw_val_voltagesens / 4095) * 25;
}

// fungsi pendukung
//-----------------------------------------------------------------------------------------------
void monitorSerial()
{
    if (gpsError)
    {
        Serial.println("check gps wiring");
    }
    else
    {
        if (gpsReady)
        {
            Serial.print("X: ");
            Serial.print(angelx);
            Serial.print(" | Y: ");
            Serial.print(angely);
            Serial.print(" | Z: ");
            Serial.println(angelz);

            Serial.print("Temperature = ");
            Serial.print(temperature);
            Serial.print(" °C");
            Serial.print(" | Pressure = ");
            Serial.print(pressure);
            Serial.print(" hPa");
            Serial.print(" | Altitude = ");
            Serial.print(altitude);
            Serial.println(" m");

            Serial.print("Latitude = ");
            Serial.print(latitude, 6);
            Serial.print(" | Longitude = ");
            Serial.println(longitude, 6);

            Serial.print("soil raw= ");
            Serial.print(raw_val_soil);
            Serial.print(", soil= ");
            Serial.println(valSoil);

            Serial.print("current raw= ");
            Serial.print(raw_val_currentsens);
            Serial.print(", current= ");
            Serial.println(valCurrent);

            Serial.print("voltage raw= ");
            Serial.print(raw_val_voltagesens);
            Serial.print(", voltage= ");
            Serial.println(valVoltage);

            Serial.println("");
        }
        else
        {
            Serial.println("device is not ready yet");
        }
    }
}

// fungsi utama
//-----------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    init_angelSensor();
    init_barometerSensor();
    init_currentSensor();
    init_locationSensor();
    init_soilSensor();
    init_voltageSensor();
}

void loop()
{
    angelSensor();
    barometerSensor();
    currentSensor();
    locationSensor();
    soilSensor();
    voltageSensor();

    // rtos
    if ((millis() - lastmillis) > 1000)
    {
        //---------------------
        monitorSerial();
        lastmillis = millis();
    }
}