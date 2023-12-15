#include <Arduino.h>

int currentsens_pin = 12; // 2

float val_current;
void setup()
{
    Serial.begin(115200);
    pinMode(currentsens_pin, INPUT);
}

void loop()
{
    float raw_val_currentsens = analogRead(currentsens_pin);

    val_current = ((raw_val_currentsens - 1241) / 3082) * 5000; // 2.5v 5a min 1v
    if (val_current < 0)
    {
        val_current = 0;
    }

    Serial.print("current raw= ");
    Serial.print(raw_val_currentsens);
    Serial.print(", current= ");
    Serial.println(val_current);

    Serial.println("");

    delay(1000);
}