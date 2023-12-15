#include <Arduino.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 36, TXPin = 39;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

void setup()
{
    Serial.begin(115200);
    ss.begin(GPSBaud);
}

void loop()
{
    if (millis() > 1000 && gps.charsProcessed() < 10)
        Serial.println(F("No GPS data received: check wiring"));

    while (ss.available() > 0)
    {
        byte gpsData = ss.read();
        // Serial.write(gpsData);

        gps.encode(gpsData);
        if (gps.location.isValid())
        {
            Serial.print("Latitude= ");
            Serial.print(gps.location.lat(), 6);
            Serial.print(" Longitude= ");
            Serial.println(gps.location.lng(), 6);
        }
        else
        {
            Serial.print("waiting gps");
        }
    }
}