//
#include <Arduino.h>

// global varilale utama
unsigned long lastmillis = 0;
float valVoltage, raw_val_voltagesens;

// global variable voltage sensor
int voltagesens_pin = 32;

// fungsi dari komponen
//-----------------------------------------------------------------------------------------------
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
    Serial.print("voltage raw= ");
    Serial.print(raw_val_voltagesens);
    Serial.print(", voltage= ");
    Serial.println(valVoltage);
}
// fungsi utama
//-----------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    init_voltageSensor();
}

void loop()
{
    voltageSensor();
    // rtos
    if ((millis() - lastmillis) > 1000)
    {

        //---------------------
        monitorSerial();
        lastmillis = millis();
    }
}