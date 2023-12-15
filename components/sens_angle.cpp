/* Get tilt angles on X and Y, and rotation angle on Z
 * Angles are given in degrees
 *
 * License: MIT
 */

#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;

void setup()
{
    Serial.begin(115200);
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
    mpu.calcOffsets(); // gyro and accelero

    // mpu.setGyroOffsets(-8.53, 1.56, -0.80); // gyro only
    // mpu.setAccOffsets(0.04, 0.00, 0.14);     // accelero only
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

void loop()
{
    mpu.update();

    if ((millis() - timer) > 1000)
    { // print data every 10ms
        Serial.print("X : ");
        Serial.print(mpu.getAngleX());
        Serial.print("\tY : ");
        Serial.print(mpu.getAngleY());
        Serial.print("\tZ : ");
        Serial.println(mpu.getAngleZ());
        timer = millis();
    }
}