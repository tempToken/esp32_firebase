#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "esp_wifi.h"
#include "time.h"

#define FIREBASE_HOST "https://esp32-73a15-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "1hffIReG4utWvgRng68oCyUi3j6WMyNM30W1s7Uw"
#define LED 2
const int ledPin = 2;
FirebaseData firebaseData;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // UTC offset for IST is 5 hours and 30 minutes (19800 seconds)

String bssid;
const int maxStations = 10; // Maximum number of stations to store
String stationMacs[maxStations];
int numStations = 0;
String name = "pritam";
int count = 0;
bool time_calibration = false;



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

// LED animation ---------------------------------------------

void light(String state) {
  if (state == "ON")
    digitalWrite(LED, HIGH);
  else if (state == "OFF")
    digitalWrite(LED, LOW);
}

void fadeInOut() {
  const int fadeInterval = 10; // Interval at which to update the brightness
  int fadeAmount = 5; // How much to fade the LED by

  for (int i = 0; i < 255; i += fadeAmount) {
    analogWrite(ledPin, i);
    delay(fadeInterval);
  }

  for (int i = 255; i > 0; i -= fadeAmount) {
    analogWrite(ledPin, i);
    delay(fadeInterval);
  }
}

void knightRider() {
  const int delayTime = 50; // Delay between LED movements
  int direction = 1; // Direction of LED movement
  int position = 0; // Current LED position

  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, LOW);
    delay(delayTime);

    position += direction;
    if (position == 0 || position == 255) {
      direction = -direction;
    }

    analogWrite(ledPin, position);
    delay(delayTime);
  }
}

void randomBlink() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(ledPin, HIGH);
    delay(random(50, 1000)); // Random delay between 50ms and 1000ms
    digitalWrite(ledPin, LOW);
    delay(random(50, 1000)); // Random delay between 50ms and 1000ms
  }
}


// ------------------------------------------------



void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  WiFi.softAP("ESP32-AP", "password");
  delay(20000);
  PrintStations();
  numStations = WiFi.softAPgetStationNum();
  count = numStations;

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

  //time calibration
  if (time_calibration == false) {
    for (int i = 0; i <= 5; i++) {
      timeClient.update();
      Serial.println("Time calibration: " + timeClient.getFormattedTime());
      delay(2000);
    }
    Serial.println("Time calibration succesful:");
    time_calibration = true;
  }

  timeClient.update();

  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && count != 0) {
    for (int i = 0; i < numStations; i++) {

      String path = "/station/" + stationMacs[i];
      Firebase.setString(firebaseData, (path + "/Name").c_str(), name);
      Serial.println("BSSID upload successful: " + String(bssid));

      Firebase.setString(firebaseData, (path + "/Join_Time").c_str(), timeClient.getFormattedTime());
      Serial.println("Time upload successful: " + timeClient.getFormattedTime());

      Firebase.setInt(firebaseData, (path + "/sl_no").c_str(), i + 1);
      Serial.println("NumStations upload successful: " + String(numStations));

      delay(500);
      light("ON");
      delay(100);
      light("OFF");
      count--;
    }
  }
  else if (count == 0) {
    fadeInOut();
    Serial.println("all user upload succesfully");
  }
  else {
    Serial.println("error");
  }
}
