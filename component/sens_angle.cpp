// library utama
#include <Arduino.h>
// library MPU6050_light (angel sensor)
#include "Wire.h"
#include <MPU6050_light.h>

// global varilale utama
unsigned long lastmillis = 0;
float angelx, angely, angelz;

// global variable MPU6050_light (angel sensor)
MPU6050 mpu(Wire);

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

// fungsi pendukung
//-----------------------------------------------------------------------------------------------
void monitorSerial()
{
    Serial.print("X: ");
    Serial.print(angelx);
    Serial.print(" | Y: ");
    Serial.print(angely);
    Serial.print(" | Z: ");
    Serial.println(angelz);
}

// fungsi utama
//-----------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    init_angelSensor();
}

void loop()
{
    // rtos
    if ((millis() - lastmillis) > 1000)
    {
        angelSensor();
        //---------------------
        monitorSerial();
        lastmillis = millis();
    }
}