// library
#include "Wire.h"
#include <MPU6050_light.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// pin define
int currentsens_pin = 12;
int voltagesens_pin = 13;
int soil_pin = 15; // 2

// define
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;
MPU6050 mpu(Wire);

// global var
unsigned long lastmillis = 0;
float angleX, angleY, angleZ;
float temperature, pressure, altitude;
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

void setup()
{
    Serial.begin(115200);
    pinMode(currentsens_pin, INPUT);
    pinMode(voltagesens_pin, INPUT);
    pinMode(soil_pin, INPUT);
    setupMPU();
    setupBMP();
}

void getSens()
{
    mpu.update();

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
        Serial.print(", Sudut Y =");
        Serial.print(angleY);
        Serial.print(", Sudut Z =");
        Serial.print(angleZ);

        Serial.print("\n");

        Serial.print("pressure = ");
        Serial.print(pressure);
        Serial.print(", temperature = ");
        Serial.print(temperature);
        Serial.print(", altitude = ");
        Serial.print(altitude);

        Serial.print("\n");

        Serial.print("voltage = ");
        Serial.print(voltage);
        Serial.print(", current = ");
        Serial.print(current);
        Serial.print(", soil = ");
        Serial.print(soil);
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