#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

const char *ssid = "teknisee";
const char *password = "myteknisee";

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define API_KEY "AIzaSyCbOkWKCuAbwnj1vVWE9R2cwVaZJcwFfWw"
#define USER_EMAIL "tidelpo@gmail.com"
#define USER_PASSWORD "1234567890"
#define DATABASE_URL "https://tidelpo-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;
String databasePath;
String getaranTanahPath;
String kelembapanTanahPath;
String kemiringanTiangPath;
String pergeseranTanahPath;
String latitudePath;
String longitudePath;

unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 5000;

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

  getaranTanahPath = "/Data/getaranTanah";
  kelembapanTanahPath = "/Data/kelembapanTanah";
  pergeseranTanahPath = "/Data/pergeseranTanah";
  kemiringanTiangPath = "/Data/kemiringanTiang";
  latitudePath = "/Data/latitude";
  longitudePath = "/Data/longitude";
}

void sendFloat(String path, float value)
{
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value))
  {
    Serial.print("Writing value: ");
    Serial.print(value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.print("PATH: ");
    Serial.println(fbdo.dataPath());
    Serial.print("TYPE: ");
    Serial.println(fbdo.dataType());
  }
  else
  {
    Serial.println("FAILED");
    Serial.print("REASON: ");
    Serial.println(fbdo.errorReason());
  }
}

void kirimFirebase()
{
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // create random float
    float val_soil = random(0, 100);
    sendFloat(latitudePath, val_soil / 3);
    sendFloat(longitudePath, val_soil / 2);
    sendFloat(kelembapanTanahPath, val_soil);
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    Wificon();
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  intFirebase();
}
void loop()
{
  // reconect
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    Wificon();
  }

  kirimFirebase();
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);
}