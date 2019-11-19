#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

#define SECRET_SSID "MARINEFORDJSBB"    // replace MySSID with your WiFi network name
#define SECRET_PASS "bbbbbbbb"         // replace MyPassword with your WiFi password
#define SECRET_CH_ID 909562            // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "TCEU8YST9NXX987C"   // replace XYZ with your channel write API Key

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

const size_t LengthData = 5;
float data[LengthData];
int ESP_Status, IC = 0;

void setup() {
  Serial.begin(9600);  // Initialize serial
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  delay(1000);
}

void loop() {
  if (Serial.available() >= LengthData * sizeof(data[0])) {
    Serial.readBytes((byte*)data, LengthData * sizeof(data[0]));

    if (WiFi.status() != WL_CONNECTED) {
      for (int i = 0; i <= 1; i++) {
        WiFi.begin(ssid, pass);
        delay(3000);
        if (WiFi.status() == WL_CONNECTED) break;
      }
    }

    //    while (WiFi.status() != WL_CONNECTED) {
    //      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
    //      delay(3000);
    //      if (IC == 2) break;
    //      IC++;
    //    }

    ThingSpeak.setField(1, data[0]);
    ThingSpeak.setField(2, data[1]);
    ThingSpeak.setField(3, data[2]);
    ThingSpeak.setField(4, data[3]);
    ThingSpeak.setField(5, data[4]);
    ESP_Status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    Serial.print(ESP_Status);
  }
  delay(100);
}
