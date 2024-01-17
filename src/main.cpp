// library
#include "Wire.h"
#include <MPU6050_light.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// pin define
int currentsens_pin = 35;
int voltagesens_pin = 32;
int soil_pin = 34;                       // 2
static const int RXPin = 36, TXPin = 39; // pin gps

// define
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;
MPU6050 mpu(Wire);
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

// constant
const char *ssid = "teknisee";
const char *password = "myteknisee";
#define API_KEY "AIzaSyCbOkWKCuAbwnj1vVWE9R2cwVaZJcwFfWw"
#define USER_EMAIL "tidelpo@gmail.com"
#define USER_PASSWORD "1234567890"
#define DATABASE_URL "https://tidelpo-default-rtdb.asia-southeast1.firebasedatabase.app/"

// firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;
String databasePath;
String kelembapanTanahPath;
String kemiringanTiangPath;
String latitudePath;
String longitudePath;
String statusTiangPath;

// global var
unsigned long lastmillis = 0;
String statusTiang = "Normal";
float angleX, angleY, angleZ, tilt;
float temperature, pressure, altitude;
float latitude, longitude;
float current, voltage, soil;

bool updatedata = false, showmonitor = false, senddata = false;
int throwerror = 0;
String errordetil = "list error :";

void Wificon()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void intFirebase()
{
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;

  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);

  Serial.println("Getting User UID");
  while ((auth.token.uid) == "")
  {
    Serial.print(" . ");
    delay(1000);
  }

  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  kelembapanTanahPath = "/Data/kelembapanTanah";
  kemiringanTiangPath = "/Data/kemiringanTiang";
  latitudePath = "/Data/latitude";
  longitudePath = "/Data/longitude";
  statusTiangPath = "/Data/statusTiang";
}

void setupMPU()
{
  Wire.begin();

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);

  if (status != 0)
  {
    // Serial.println(F("MPU6050 connection failed. Please check your connection with `connection_check` example."));
    throwerror = +1;
    errordetil += "\nMPU6050 error ";
  }

  // setting offset manually
  mpu.setGyroOffsets(-8.53, 1.56, -0.80); // gyro only
  mpu.setAccOffsets(0.04, 0.00, 0.14);    // accelero only
}

void setupBMP()
{
  Serial.begin(115200);

  Serial.println(F("Arduino + BMP280"));

  if (!bmp280.begin(BMP280_I2C_ADDRESS))
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1)
      ;
  }
  Serial.println("Found BMP280 sensor!");
}

void setupGPS()
{
  ss.begin(GPSBaud);
  if (millis() > 1000 && gps.charsProcessed() < 10)
  {
    // Serial.println(F("No GPS data received: check wiring"));
    throwerror = +1;
    errordetil += "\nGPS error ";
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(currentsens_pin, INPUT);
  pinMode(voltagesens_pin, INPUT);
  pinMode(soil_pin, INPUT);
  setupGPS();
  setupMPU();
  setupBMP();
  WiFi.begin(ssid, password);
  Wificon();
  intFirebase();
}

void sendFloat(String name, String path, float value)
{
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value))
  {
    Serial.print(name);
    Serial.println(": PASSED");

    // Serial.print("Writing value: ");
    // Serial.print(value);
    // Serial.print(" on the following path: ");
    // Serial.println(path);
    // Serial.println("PASSED");
    // Serial.print("PATH: ");
    // Serial.println(fbdo.dataPath());
    // Serial.print("TYPE: ");
    // Serial.println(fbdo.dataType());
  }
  else
  {
    Serial.print(name);
    Serial.println(": FAILED");
    // Serial.println("FAILED");
    // Serial.print("REASON: ");
    // Serial.println(fbdo.errorReason());
  }
}

void sendString(String name, String path, String value)
{
  if (Firebase.RTDB.setString(&fbdo, path.c_str(), value))
  {
    Serial.print(name);
    Serial.println(": PASSED");

    // Serial.print("Writing value: ");
    // Serial.print(value);
    // Serial.print(" on the following path: ");
    // Serial.println(path);
    // Serial.println("PASSED");
    // Serial.print("PATH: ");
    // Serial.println(fbdo.dataPath());
    // Serial.print("TYPE: ");
    // Serial.println(fbdo.dataType());
  }
  else
  {
    Serial.print(name);
    Serial.println(": FAILED");
    // Serial.println("FAILED");
    // Serial.print("REASON: ");
    // Serial.println(fbdo.errorReason());
  }
}

void kirimFirebase()
{
  if (Firebase.ready() && senddata)
  {
    // create random float
    sendFloat("latitude", latitudePath, latitude);
    sendFloat("longitude", longitudePath, longitude);
    sendFloat("soil", kelembapanTanahPath, soil);
    sendFloat("kemiringan", kemiringanTiangPath, tilt);
    sendString("statusTiang", statusTiangPath, statusTiang);

    senddata = false;
  }
}

void getSens()
{
  mpu.update();
  while (ss.available() > 0)
  {
    byte gpsData = ss.read();
    // Serial.write(gpsData);

    gps.encode(gpsData);
    if (gps.location.isValid())
    {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
    }
  }

  if (updatedata)
  {
    // gyro
    angleX = mpu.getAngleX();
    angleY = mpu.getAngleY();
    angleZ = mpu.getAngleZ();

    // bmp
    temperature = bmp280.readTemperature();
    pressure = bmp280.readPressure();
    altitude = bmp280.readAltitude(1013.25);

    // current
    float raw_val_currentsens = analogRead(currentsens_pin);
    current = ((raw_val_currentsens - 1241) / 3082) * 5000; // 2.5v 5a min 1v
    if (current < 0)
    {
      current = 0;
    }

    // voltage
    float raw_val_voltagesens = analogRead(voltagesens_pin);
    voltage = (raw_val_voltagesens / 4095) * 14.3;
    // voltage = raw_val_voltagesens;

    // soil
    float raw_val_soil = analogRead(soil_pin);
    soil = ((4095 - raw_val_soil) / 4095) * 100;

    updatedata = false;
  }
}

void process()
{
  tilt = sqrtf(powf(angleX, 2) + powf(angleY, 2) + powf(angleZ, 2));

  if (tilt > 15 || pressure > 55158.1)
  {
    statusTiang = "Bahaya";
    // Serial.println("Bahaya");
  }
  else if (tilt > 10 || pressure < 55158.1)
  {
    statusTiang = "Waspada";
    // Serial.println("Waspada");
  }
  else if (tilt < 10 || pressure > 55158.1)
  {
    statusTiang = "Siaga";
    // Serial.println("Siaga");
  }
  else
  {
    statusTiang = "Normal";
    // Serial.println("Normal");
  }
}

void monitorSerial()
{
  if (showmonitor)
  {
    Serial.print(tilt);
    Serial.print("WIFI STATUS =");
    Serial.println(WiFi.status());
    Serial.print("\n");

    Serial.print(statusTiang);
    Serial.print("\n");
    Serial.print("Sudut X =");
    Serial.print(angleX);
    Serial.print("'");
    Serial.print(", Sudut Y =");
    Serial.print(angleY);
    Serial.print("'");
    Serial.print(", Sudut Z =");
    Serial.print(angleZ);
    Serial.print("'");

    Serial.print("\n");

    Serial.print("pressure = ");
    Serial.print(pressure);
    Serial.print("Pa");
    Serial.print(", temperature = ");
    Serial.print(temperature);
    Serial.print("C");
    Serial.print(", altitude = ");
    Serial.print(altitude);
    Serial.print("m");

    Serial.print("\n");

    Serial.print("latitude = ");
    Serial.print(latitude, 6);
    Serial.print(", longitude = ");
    Serial.print(longitude, 6);

    Serial.print("\n");

    Serial.print("voltage = ");
    Serial.print(voltage);
    Serial.print("V");
    Serial.print(", current = ");
    Serial.print(current);
    Serial.print("A");
    Serial.print(", soil = ");
    Serial.print(soil);
    Serial.print("%");
    Serial.print("\n");

    showmonitor = false;
  }
}

void loop()
{
  // timer
  if (millis() - lastmillis > 1000)
  {
    lastmillis = millis();
    updatedata = true;
    showmonitor = true;
    senddata = true;
  }
  // getting data
  getSens();

  // process data
  process();

  // show monitor
  monitorSerial();

  // check wifi and push data
  if (WiFi.status() == WL_CONNECTED)
  {
    kirimFirebase();
  }
  else
  {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    Wificon();
  }
}