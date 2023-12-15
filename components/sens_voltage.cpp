#include <Arduino.h>

int voltagesens_pin = 13; // 2

float val_voltage;
void setup()
{
    Serial.begin(115200);
    pinMode(voltagesens_pin, INPUT);
}

void loop()
{
    float raw_val_voltagesens = analogRead(voltagesens_pin);

    val_voltage = (raw_val_voltagesens / 4095) * 25;

    Serial.print("voltage raw= ");
    Serial.print(raw_val_voltagesens);
    Serial.print(", voltage= ");
    Serial.println(val_voltage);

    Serial.println("");

    delay(1000);
}