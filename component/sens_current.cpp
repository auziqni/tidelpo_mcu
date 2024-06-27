//
#include <Arduino.h>

// global varilale utama
unsigned long lastmillis = 0;
float valCurrent, raw_val_currentsens;

// global variable current sensor
int currentsens_pin = 35;

// fungsi dari komponen
//-----------------------------------------------------------------------------------------------
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

// fungsi pendukung
//-----------------------------------------------------------------------------------------------

void monitorSerial()
{
    Serial.print("current raw= ");
    Serial.print(raw_val_currentsens);
    Serial.print(", current= ");
    Serial.println(valCurrent);
}
// fungsi utama
//-----------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    init_currentSensor();
}

void loop()
{
    currentSensor();
    // rtos
    if ((millis() - lastmillis) > 1000)
    {

        //---------------------
        monitorSerial();
        lastmillis = millis();
    }
}