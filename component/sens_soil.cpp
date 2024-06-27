//
#include <Arduino.h>

// global varilale utama
unsigned long lastmillis = 0;
float valSoil, raw_val_soil;

// global variable soil sensor
int soil_pin = 34;

// fungsi dari komponen
//-----------------------------------------------------------------------------------------------
void init_soilSensor()
{
    pinMode(soil_pin, INPUT);
}

void soilSensor()
{
    raw_val_soil = analogRead(soil_pin);

    valSoil = ((4095 - raw_val_soil) / 4095) * 100;
}

// fungsi pendukung
//-----------------------------------------------------------------------------------------------

void monitorSerial()
{
    Serial.print("soil raw= ");
    Serial.print(raw_val_soil);
    Serial.print(", soil= ");
    Serial.println(valSoil);
}
// fungsi utama
//-----------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    init_soilSensor();
}

void loop()
{
    soilSensor();
    // rtos
    if ((millis() - lastmillis) > 1000)
    {

        //---------------------
        monitorSerial();
        lastmillis = millis();
    }
}