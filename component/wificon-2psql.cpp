// library utama
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// static global varilale utama
const char *deviceCode = "1xmmtha";
const char *ssid = "teknisee";
const char *password = "myteknisee";

// global varilale utama
unsigned long lastmillis = 0;
float angelx = 0.1, angely = 0.3, angelz = 0.5;
float temperature = 32.52, pressure = 997.29, altitude = 113.8;
float valCurrent = 0.1, raw_val_currentsens = 0;
float latitude = -5.364038, longitude = 105.236122;
float valSoil = 12, raw_val_soil = 0;
float valVoltage = 12.1, raw_val_voltagesens = 0;
//--setellah pengolahan
// int statusTiang = 1;

// fungsi dari komponen
//-----------------------------------------------------------------------------------------------
void init_wifi()
{
    WiFi.begin(ssid, password);

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); 
}

void wifi()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
    }
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

    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.print(" °C");
    Serial.print(" | Pressure = ");
    Serial.print(pressure);
    Serial.print(" hPa");
    Serial.print(" | Altitude = ");
    Serial.print(altitude);
    Serial.println(" m");

    Serial.print("Latitude = ");
    Serial.print(latitude, 6);
    Serial.print(" | Longitude = ");
    Serial.println(longitude, 6);

    Serial.print("soil raw= ");
    Serial.print(raw_val_soil);
    Serial.print(", soil= ");
    Serial.println(valSoil);

    Serial.print("current raw= ");
    Serial.print(raw_val_currentsens);
    Serial.print(", current= ");
    Serial.println(valCurrent);

    Serial.print("voltage raw= ");
    Serial.print(raw_val_voltagesens);
    Serial.print(", voltage= ");
    Serial.println(valVoltage);

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("");
}

float getTilt()
{
    return sqrt(pow(angelx, 2) + pow(angely, 2));
}

String getStatusTiang()
{
    float tilt = getTilt();
    if (tilt > 15)
    {
        return "bahaya";
    }
    else if (tilt > 10)
    {
        return "waspada";
    }
    else
    {
        return "aman";
    }
}

void kirim2psql()
{
    HTTPClient http;
    http.begin("https://www.tidelpo.my.id/api/poststate");
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> doc;
    doc["deviceCode"] = deviceCode;
    doc["lat"] = latitude;
    doc["lng"] = longitude;
    doc["statusTiang"] = getStatusTiang();
    doc["sensSoil"] = valSoil;
    doc["sensPressure"] = pressure;
    doc["sensTilt"] = getTilt();

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        Serial.println("Response: " + response);
    }
    else
    {
        Serial.println("Error on HTTP request");
        Serial.println("HTTP Response code: " + String(httpResponseCode));
    }

    http.end();
}

// fungsi utama
//-----------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    init_wifi();
}
void loop()
{
    wifi();
    // rtos
    if ((millis() - lastmillis) > 10000)
    {
        //---------------------
        // monitorSerial();
        kirim2psql();
        lastmillis = millis();
    }
}