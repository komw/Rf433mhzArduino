#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <IPAddress.h>
#include <Rf433mhzArduinoReceiver.h>

#define WWW_BUF_SIZE 1460

Rf433mhzArduinoReceiver rfReceiver = Rf433mhzArduinoReceiver();
int value = 0;
//IPAddress server(91, 239, 66, 46);
IPAddress server(192,168,100,104);
WiFiClient client;
String request;
void setup(void)
{

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("wait to connect to wifi");
    delay(200);
  }
  Serial.println(WiFi.localIP());
  Serial.print("Start");
  rfReceiver.enableReceive(0);  // ustaw 0 na esp d3
}

void loop() {
  value = Rf433mhzArduinoReceiver::popLastCode();
  Serial.println(value);
  delay(200);
  if (value > 0) {


    if (client.connect(server, 80)) {
      request = "POST /alarm.php HTTP/1.0\r\nHost: komw.sgladysz.com\r\n\r\ncode=" + ((String)value);
      Serial.println(request);
      client.println(request);
      client.println();
      while (client.available()) {
        client.read();
      }
      client.stop();
      client = WiFiClient();
    }
  }
}
