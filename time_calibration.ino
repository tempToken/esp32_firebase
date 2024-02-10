#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char *ssid = "hotspot";
const char *password = "@pritam123";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // UTC offset for IST is 5 hours and 30 minutes (19800 seconds)

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  timeClient.begin();
}

void loop(){
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  delay(1000);
}
