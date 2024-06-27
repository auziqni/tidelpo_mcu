// library utama
#include <Arduino.h>
// library TinyGPS++ (location sensor)
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// global varilale utama
unsigned long lastmillis = 0;
float latitude, longitude;

// global variable TinyGPS++ (location sensor)
static const int RXPin = 36, TXPin = 39;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
bool gpsError = false;
bool gpsReady = false;

// fungsi dari komponen
//-----------------------------------------------------------------------------------------------
void init_locationSensor()
{
    ss.begin(GPSBaud);
}
void locationSensor()
{
    if (gps.charsProcessed() < 10)
    {
        gpsError = true;
    }

    while (ss.available() > 0)
    {
        gpsError = false;
        byte gpsData = ss.read();
        // Serial.write(gpsData);

        gps.encode(gpsData);
        if (gps.location.isValid())
        {
            gpsReady = true;
            latitude = gps.location.lat();
            longitude = gps.location.lng();
        }
        else
        {
            gpsReady = false;
        }
    }
}
// fungsi pendukung
//-----------------------------------------------------------------------------------------------
void monitorSerial()
{
    if (gpsError)
    {
        Serial.println("check gps wiring");
    }
    else
    {
        if (gpsReady)
        {
            Serial.print("Latitude = ");
            Serial.print(latitude, 6);
            Serial.print(" | Longitude = ");
            Serial.println(longitude, 6);
        }
        else
        {
            Serial.println("device is not ready yet");
        }
    }
}
// fungsi utama
//-----------------------------------------------------------------------------------------------

void setup()
{
    Serial.begin(115200);
    init_locationSensor();
}

void loop()
{
    locationSensor();

    if (millis() - lastmillis > 1000)
    {
        //---------------------
        monitorSerial();
        lastmillis = millis();
    }
}