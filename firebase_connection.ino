#include <WiFi.h>

#include <FirebaseESP32.h>

#define FIREBASE_HOST "https://esp32-73a15-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "1hffIReG4utWvgRng68oCyUi3j6WMyNM30W1s7Uw"
#define LED 2
FirebaseData firebaseData;

void setup()
{
  Serial.begin(115200);

  WiFi.begin("hotspot", "@pritam123");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Wifi connected..........");
  
  Firebase.setInt(firebaseData, "/DATA", 1);
  
  Serial.println("Database conneced.......");
  pinMode(LED,OUTPUT);

}


void loop()
{
  int test = Firebase.getInt(firebaseData, "/DATA");
  if (test == 1) {
    digitalWrite(LED,HIGH);
    Serial.println("LED is ON");
    delay(1000);
    Firebase.setInt(firebaseData, "/DATA", 0);
  }
    digitalWrite(LED,LOW);
    Serial.println("LED is OFF");
    delay(1000);
    Firebase.setInt(firebaseData, "/DATA", 1);
//    Serial.println((Firebase.getInt(firebaseData, "/DATA")));
//    Serial.println((Firebase.setString(firebaseData, "/DATA", "test")));

}
