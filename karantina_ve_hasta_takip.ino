#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ESP8266WiFi.h>
#include "Wire.h"
#include "FirebaseArduino.h"
#define REPORTING_PERIOD_MS 1000
#define WIFI_SSID "RG HOLDİNG"
#define WIFI_PASSWORD "27ATATURK27"
#define FIREBASE_HOST "bileklik-1f1fe-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "8ZPGwSf2LHgCaPCwU5ogMQ4jFe1V5fPaqChnwuDy"
int sicaklikSensor = A0;
float sicaklikdegeri;   //Analog değeri dönüştüreceğimiz sıcaklık değeri
float olculendeger;   //Ölçeceğimiz analog değer
PulseOximeter pox;
float BPM, SpO2;
uint32_t tsLastReport = 0;
 
void onBeatDetected()
{
    Serial.println("Beat Detected!");
}
void setup()
{
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(16, OUTPUT);
  
  Serial.print("Initializing Pulse Oximeter..");
  wifiConnect();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (!pox.begin())
    {
      Serial.println("FAILED");
         for(;;);
    }
    else
    {
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }
}
void loop()
{
  olculendeger = analogRead(sicaklikSensor); //A0'den değeri alacak
  olculendeger = (olculendeger / 1024) * 3300;//değeri mV'a dönüştürecek
  sicaklikdegeri = olculendeger / 10, 0; // mV'u sicakliğa dönüştürecek
  Serial.println(sicaklikdegeri);
  Firebase.setInt("sicaklik", sicaklikdegeri);
  Firebase.setInt("nabiz", BPM);
  Firebase.setInt("oksijen", SpO2);
  delay(1000);
   pox.update();
 if (Firebase.failed()) {
    Serial.print("setting /number failed:");
    Serial.println(Firebase.error());
    return;
  }
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");
        delay(1000);
        tsLastReport = millis();
    }
}
void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++teller); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}
