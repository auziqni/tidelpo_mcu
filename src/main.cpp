#include <Arduino.h>

int soil_pin = 15; // 2

int val_soil;
void setup()
{
  Serial.begin(115200);
  pinMode(soil_pin, INPUT);
}

void loop()
{
  float raw_val_soil = analogRead(soil_pin);

  val_soil = ((4095 - raw_val_soil) / 4095) * 100;

  Serial.print("soil raw= ");
  Serial.print(raw_val_soil);
  Serial.print(", soil= ");
  Serial.println(val_soil);

  Serial.println("");

  delay(1000);
}