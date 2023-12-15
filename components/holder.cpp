// library
#include "Wire.h"
#include <MPU6050_light.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// pin define
int currentsens_pin = 12;
int voltagesens_pin = 13;
int soil_pin = 15; // 2
static const int RXPin = 36, TXPin = 39;

// define
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;
MPU6050 mpu(Wire);
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

// global var
unsigned long lastmillis = 0;
float angleX, angleY, angleZ;
float temperature, pressure, altitude;
float latitude, longitude;
float current, voltage, soil;

bool updatedata = false, showmonitor = false;
int throwerror = 0;
String errordetil = "list error :";

void setupMPU()
{
    Wire.begin();

    byte status = mpu.begin();
    Serial.print(F("MPU6050 status: "));
    Serial.println(status);

    if (status != 0)
    {
        // Serial.println(F("MPU6050 connection failed. Please check your connection with `connection_check` example."));
        throwerror = +1;
        errordetil += "\nMPU6050 error ";
    }

    // setting offset manually
    mpu.setGyroOffsets(-8.53, 1.56, -0.80); // gyro only
    mpu.setAccOffsets(0.04, 0.00, 0.14);    // accelero only
}

void setupBMP()
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

void setupGPS()
{
    ss.begin(GPSBaud);
    if (millis() > 1000 && gps.charsProcessed() < 10)
    {
        // Serial.println(F("No GPS data received: check wiring"));
        throwerror = +1;
        errordetil += "\nGPS error ";
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(currentsens_pin, INPUT);
    pinMode(voltagesens_pin, INPUT);
    pinMode(soil_pin, INPUT);
    setupGPS();
    setupMPU();
    setupBMP();
}

void getSens()
{
    mpu.update();
    while (ss.available() > 0)
    {
        byte gpsData = ss.read();
        // Serial.write(gpsData);

        gps.encode(gpsData);
        if (gps.location.isValid())
        {
            latitude = gps.location.lat();
            longitude = gps.location.lng();
        }
    }

    if (updatedata)
    {
        // gyro
        angleX = mpu.getAngleX();
        angleY = mpu.getAngleY();
        angleZ = mpu.getAngleZ();

        // bmp
        temperature = bmp280.readTemperature();
        pressure = bmp280.readPressure();
        altitude = bmp280.readAltitude(1013.25);

        // current
        float raw_val_currentsens = analogRead(currentsens_pin);
        current = ((raw_val_currentsens - 1241) / 3082) * 5000; // 2.5v 5a min 1v
        if (current < 0)
        {
            current = 0;
        }

        // voltage
        float raw_val_voltagesens = analogRead(voltagesens_pin);
        voltage = (raw_val_voltagesens / 4095) * 25;

        // soil
        float raw_val_soil = analogRead(soil_pin);
        soil = ((4095 - raw_val_soil) / 4095) * 100;

        updatedata = false;
    }
}

void monitorSerial()
{
    if (showmonitor)
    {
        Serial.print("Sudut X =");
        Serial.print(angleX);
        Serial.print("'");
        Serial.print(", Sudut Y =");
        Serial.print(angleY);
        Serial.print("'");
        Serial.print(", Sudut Z =");
        Serial.print(angleZ);
        Serial.print("'");

        Serial.print("\n");

        Serial.print("pressure = ");
        Serial.print(pressure);
        Serial.print("Pa");
        Serial.print(", temperature = ");
        Serial.print(temperature);
        Serial.print("C");
        Serial.print(", altitude = ");
        Serial.print(altitude);
        Serial.print("m");

        Serial.print("\n");

        Serial.print("latitude = ");
        Serial.print(latitude, 6);
        Serial.print(", longitude = ");
        Serial.print(longitude, 6);

        Serial.print("\n");

        Serial.print("voltage = ");
        Serial.print(voltage);
        Serial.print("V");
        Serial.print(", current = ");
        Serial.print(current);
        Serial.print("A");
        Serial.print(", soil = ");
        Serial.print(soil);
        Serial.print("%");
        Serial.print("\n");

        Serial.println("\n");

        showmonitor = false;
    }
}

void loop()
{
    if (millis() - lastmillis > 1000)
    {
        lastmillis = millis();
        updatedata = true;
        showmonitor = true;
    }

    getSens();
    monitorSerial();
}