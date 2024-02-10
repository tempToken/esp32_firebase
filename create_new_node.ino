#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "hotspot"
#define WIFI_PASSWORD "@pritam123"
#define FIREBASE_HOST "https://esp32-73a15-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "1hffIReG4utWvgRng68oCyUi3j6WMyNM30W1s7Uw"

FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED) {
    for (int i = 1; i <= 10; i++) {
      // Create a new node with value i
      String path = "/numbers/" + String(i);
      Firebase.setInt(firebaseData, path.c_str(), i);
      if (firebaseData.dataType() == "null") {
        Serial.println("Failed to create node " + String(i));
      } else {
        Serial.println("Node " + String(i) + " created successfully");
      }
      delay(1000); // Optional delay between each node creation
    }
  }

  delay(5000); // Delay before looping again
}
