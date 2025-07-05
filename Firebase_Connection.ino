/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp8266-nodemcu-firebase-realtime-database/
*********/
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// Network and Firebase credentials
#define WIFI_SSID "Your Mom"
#define WIFI_PASSWORD "ppsdragon"

#define Web_API_KEY "AIzaSyCOv6u8b1LSeC6wrgvT-50fIbYDLaLwdiw"
#define DATABASE_URL "https://iot-test-53512-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "kavinduv69@gmail.com"
#define USER_PASS "kvcraft4real"


// User function
void processData(AsyncResult &aResult);

// Authentication
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

// Firebase components
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;


unsigned long lastSendTime = 0;
const unsigned long sendInterval = 100; // 

#include <SoftwareSerial.h>
SoftwareSerial nodemcuSerial(D8, D7); // RX, TX

void setup(){

  Serial.begin(9600);
  nodemcuSerial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)    {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  // Configure SSL client
  ssl_client.setInsecure();
  ssl_client.setTimeout(1000); // Set connection timeout
  ssl_client.setBufferSizes(4096, 1024); // Set buffer sizes

  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
}

int water;

void loop(){
  // Maintain authentication and async tasks
  app.loop();

  // Check if authentication is ready
  if (app.ready()){
    // Periodic data sending every 10 seconds
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      // Update the last send time
      lastSendTime = currentTime;

      WaterLevel();

      Database.set<int>(aClient, "/Sensors/waterLevel",water, processData, "water");  }
}
}

void processData(AsyncResult &aResult){
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}

void WaterLevel(){
  if (nodemcuSerial.available()) {
    water = nodemcuSerial.read();
  }
}