#include <Arduino.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <WebSocketsClient.h>
#include "AC.hpp"
#include "Type.cpp"

const char *ca PROGMEM = R"(-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----)";

const uint16_t IR_RECEIVE_PIN PROGMEM = 34; // The ESP GPIO pin to use that controls the IR LED.
const uint16_t IR_SEND_PIN PROGMEM = 32;    // The ESP GPIO pin to use that controls the IR LED.
const uint16_t CAPTURE_BUFFER_SIZE PROGMEM = 1024;
const uint8_t CAPTURE_TIMEOUT PROGMEM = 50;              // This is the gap timeout
const uint8_t TOLERANCE_PERCENTAGE PROGMEM = kTolerance; // kTolerance is normally 25%

const char *SSID PROGMEM = "XDDD";
const char *PASSWORD PROGMEM = "XDDD";
const char *WEBSOCKET_SERVER PROGMEM = "linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com";
const int WEBSOCKET_PORT PROGMEM = 8080;
const int MQTT_PORT PROGMEM = 8883;

decode_type_t protocol = decode_type_t::UNKNOWN;
decode_results results;

AC ac(IR_SEND_PIN);
IRrecv irrecv(IR_RECEIVE_PIN, CAPTURE_BUFFER_SIZE, CAPTURE_TIMEOUT, true);

WiFiClientSecure wificlient;
WebSocketsClient webSocket;

void handlePing();
void handleCommand(DynamicJsonDocument);
void handleIncomingText(uint8_t*);

void setup()
{
  Serial.begin(921600);
  delay(1000);
  wificlient.setCACert(ca);
  connectWiFi();
  irrecv.setTolerance(TOLERANCE_PERCENTAGE);
  irrecv.enableIRIn();
  webSocket.beginSSL(WEBSOCKET_SERVER, WEBSOCKET_PORT, "/model");
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  if (!isWiFiConnected())
  {
    Serial.println("WiFi connection lost. Attempting to reconnect.");
    WiFi.reconnect();
  }
  else
  {
    if (irrecv.decode(&results))
    {
      Serial.println(typeToString(results.decode_type));
      if (results.decode_type != decode_type_t::UNKNOWN)
      {
        String output;
        protocol = results.decode_type;
        StaticJsonDocument<256> jsonDocument;
        jsonDocument["type"] = TYPE_PROTOCAL;
        jsonDocument["value"]["model"] = (int)protocol;
        serializeJson(jsonDocument, output);
        webSocket.sendTXT(output);
      }
    }
    webSocket.loop();
  }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
  {
    Serial.printf("[WSc] Connected to url: %s\n", payload);
    webSocket.sendTXT("{ \"type\": \"status\", \"value\": { \"status\": 1 } }");
  }
  break;
  case WStype_TEXT:
    Serial.printf("[WSc] get text: %s\n", payload);
    handleIncomingText(payload);
    break;
  case WStype_BIN:
    Serial.printf("[WSc] get binary length: %u\n", length);
    break;
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

void connectWiFi()
{
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(SSID);
}

bool isWiFiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

void handlePing()
{
  webSocket.sendTXT("{\"type\":\"pong\"}");
}

void handleCommand(DynamicJsonDocument jsonDocument)
{
  int protocol = jsonDocument["protocol"];
  bool power = jsonDocument["power"];
  int mode = (int)stdAc::opmode_t::kCool;
  int degrees = jsonDocument["degrees"];
  int fanspeed = jsonDocument["fanspeed"];
  int swingv = jsonDocument["swingv"];
  int swingh = jsonDocument["swingh"];
  bool econo = jsonDocument["econo"];
  bool turbo = jsonDocument["turbo"];
  ac.sendAc(power, protocol, mode, degrees, fanspeed, swingv, swingh, econo, turbo);
}

void handleIncomingText(uint8_t* payload) {
   DynamicJsonDocument jsonDocument(1024);
    deserializeJson(jsonDocument, payload);
    if (jsonDocument["type"] = TYPE_PING)
    {
      handlePing();
    }
    else if (jsonDocument["type"] = TYPE_COMMAND)
    {
      handleCommand(jsonDocument["type"]);
    }
}
