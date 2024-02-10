#include <WiFi.h>
#include "esp_wifi.h"

const char *ssid = "ESP32-AP";
const char *password = "12345678";
String bssid;
const int maxStations = 10; // Maximum number of stations to store
String stationMacs[maxStations];
int numStations = 0;

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

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  //  bssid = WiFi.softAPmacAddress(); // Store the SoftAP BSSID
  //  Serial.println();
  //  Serial.print("SoftAP IP address: ");
  //  Serial.println(WiFi.softAPIP());
  //  Serial.print("SoftAP BSSID: ");
  //  Serial.println(bssid);
  delay(5000);
  PrintStations();
}

void loop() {
  Serial.println(bssid);
  delay(1000);
}
