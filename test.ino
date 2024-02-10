#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "esp_wifi.h"
#include "time.h"

#define FIREBASE_HOST "https://esp32-73a15-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "1hffIReG4utWvgRng68oCyUi3j6WMyNM30W1s7Uw"
#define LED 2
FirebaseData firebaseData;
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // UTC offset for IST is 5 hours and 30 minutes (19800 seconds)

String bssid;
const int maxStations = 10; // Maximum number of stations to store
String stationMacs[maxStations];
int numStations = 0;
String name = "pritam";

void PrintStations() {
  wifi_sta_list_t stationList;
  esp_wifi_ap_get_sta_list(&stationList);

  Serial.print("Number of connected stations: ");
  Serial.println(stationList.num);

  numStations = min(stationList.num, maxStations); // Limit the number of stations to store

  for (int i = 0; i < numStations; i++) {
    wifi_sta_info_t station = stationList.sta[i];
    String macAddress;
    for (int j = 0; j < 6; j++) {
      char str[3];
      sprintf(str, "%02X", (int)station.mac[j]);
      macAddress += str;
      if (j < 5) {
        macAddress += ":";
      }
    }
    stationMacs[i] = macAddress;
  }

  Serial.println("Connected station MAC addresses:");
  for (int i = 0; i < numStations; i++) {
    Serial.println(stationMacs[i]);
    bssid = stationMacs[i];
  }
  Serial.println("-----------------");
}


void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  WiFi.softAP("ESP32-AP", "password");
  delay(20000);
  PrintStations();
  numStations = WiFi.softAPgetStationNum();
  delay(1000);
  WiFi.softAPdisconnect(true);

  delay(5000);

  WiFi.begin("hotspot", "@pritam123");
  Serial.println("Wifi connected..........");

  timeClient.begin();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Database conneced.......");
  Serial.println(numStations);
  Serial.println(bssid);
}

void loop() {
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED) {
    for (int i = 0; i < numStations; i++) { // Corrected the loop condition
      String path = "/station/" + stationMacs[i];
      Firebase.setString(firebaseData, (path + "/BSSID").c_str(), name); // Corrected path concatenation
      Serial.println("BSSID update successful: " + String(bssid)); // Moved BSSID update message inside the loop

      timeClient.update();
      Firebase.setString(firebaseData, (path + "/Time").c_str(), timeClient.getFormattedTime()); // Corrected path concatenation
      Serial.println("Time update successful: " + timeClient.getFormattedTime()); // Moved time update message inside the loop

      Firebase.setInt(firebaseData, (path + "/NumStations").c_str(), numStations); // Corrected path concatenation
      Serial.println("NumStations update successful: " + String(numStations)); // Moved user update message inside the loop

      delay(500); // Optional delay between updates
      digitalWrite(LED,HIGH);
      delay(100);
      digitalWrite(LED,LOW);
    }
  }
}
