#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "OTA.hpp"

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

const char *SSID PROGMEM = "MR200_2.4GHz";
const char *PASSWORD PROGMEM = "wifi@mr200";
const char *MQTT_BROKER PROGMEM = "linux-vm-southeastasia-1.southeastasia.cloudapp.azure.com";
const int MQTT_PORT PROGMEM = 8883;
const char *MQTT_USER PROGMEM = "esp32";
const char *MQTT_PASSWORD PROGMEM = "esp32gaming";
const char *MQTT_TOPIC PROGMEM = "test";
const char *MQTT_COMMAND_TOPIC PROGMEM = "command";
const char *MQTT_MODEL_TOPIC PROGMEM = "model";

decode_type_t protocol = decode_type_t::UNKNOWN;
decode_results results; // Somewhere to store the results

IRac ac(IR_SEND_PIN); // Create a A/C object using GPIO to sending messages with.
IRrecv irrecv(IR_RECEIVE_PIN, CAPTURE_BUFFER_SIZE, CAPTURE_TIMEOUT, true);

WiFiClientSecure wificlient;
PubSubClient mqttClient(wificlient);

DynamicJsonDocument jsonDocument(1024);

void setup()
{
  Serial.begin(500000);
  delay(1000);
  wificlient.setCACert(ca);
  connectWiFi();
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(callback);
  connectMQTT();
  mqttClient.subscribe(MQTT_MODEL_TOPIC);
  initAc();
  irrecv.setTolerance(TOLERANCE_PERCENTAGE);
  irrecv.enableIRIn();
}

void loop()
{
  if (!isWiFiConnected())
  {
    Serial.println("WiFi connection lost. Attempting to reconnect.");
    WiFi.reconnect();
    connectWiFi();
  }
  else if (!mqttClient.connected())
  {
    Serial.println("MQTT connection lost. Attempting to reconnect.");
    connectMQTT();
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
        jsonDocument["model"] = (int)protocol;
        serializeJson(jsonDocument, output);
        mqttClient.publish(MQTT_MODEL_TOPIC, output.c_str());
      }
    }
    if (protocol != decode_type_t::UNKNOWN)
    {
      ac.next.protocol = protocol;
      ac.next.power = false;
      Serial.println(ac.sendAc());
    }
    mqttClient.loop();
  }
}

void initAc()
{
  ac.next.model = 1;                              // Some A/Cs have different models. Try just the first.
  ac.next.mode = stdAc::opmode_t::kCool;          // Run in cool mode initially.
  ac.next.celsius = true;                         // Use Celsius for temp units. False = Fahrenheit
  ac.next.degrees = 25;                           // 25 degrees.
  ac.next.fanspeed = stdAc::fanspeed_t::kMedium;  // Start the fan at medium.
  ac.next.swingv = stdAc::swingv_t::kUpperMiddle; // Don't swing the fan up or down.
  ac.next.swingh = stdAc::swingh_t::kOff;         // Don't swing the fan left or right.
  ac.next.light = false;                          // Turn off any LED/Lights/Display that we can.
  ac.next.beep = false;                           // Turn off any beep from the A/C if we can.
  ac.next.econo = false;                          // Turn off any economy modes if we can.
  ac.next.filter = false;                         // Turn off any Ion/Mold/Health filters if we can.
  ac.next.turbo = false;                          // Don't use any turbo/powerful/etc modes.
  ac.next.quiet = false;                          // Don't use any quiet/silent/etc modes.
  ac.next.sleep = -1;                             // Don't set any sleep time or modes.
  ac.next.clean = false;                          // Turn off any Cleaning options if we can.
  ac.next.clock = -1;                             // Don't set any current time if we can avoid it.
  ac.next.power = false;
}

bool sendAc(bool power, int model, int mode, int degrees, int fanspeed, int swingv, int swingh, bool econo, bool turbo)
{
  ac.next.power = power;
  ac.next.model = model;                          // Some A/Cs have different models. Try just the first.
  ac.next.mode = (stdAc::opmode_t)mode;           // Run in cool mode initially.
  ac.next.degrees = degrees;                      // 25 degrees.
  ac.next.fanspeed = (stdAc::fanspeed_t)fanspeed; // Start the fan at medium.
  ac.next.swingv = (stdAc::swingv_t)swingv;       // Don't swing the fan up or down.
  ac.next.swingh = (stdAc::swingh_t)swingh;       // Don't swing the fan left or right.
  ac.next.econo = econo;                          // Turn off any economy modes if we can.
  ac.next.turbo = turbo;                          // Don't use any turbo/powerful/etc modes.
  return ac.sendAc();
}

void callback(char *topic, byte *payload, unsigned int length)
{
  if (strcmp(topic, MQTT_COMMAND_TOPIC) == 0)
  {
    DeserializationError error = deserializeJson(jsonDocument, payload);
    if (error)
    {
      Serial.print("Deserialization failed: ");
      Serial.println(error.c_str());
      return;
    }
    int model = jsonDocument["model"];
    int mode = jsonDocument["mode"];
    int degrees = jsonDocument["degrees"];
    int fanspeed = jsonDocument["fanspeed"];
    int swingv = jsonDocument["swingv"];
    int swingh = jsonDocument["swingh"];
    bool econo = jsonDocument["econo"];
    bool turbo = jsonDocument["turbo"];
    bool power = jsonDocument["power"];
    sendAc(power, model, mode, degrees, fanspeed, swingv, swingh, econo, turbo);
    Serial.println("AC sent");
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

void connectMQTT()
{
  while (!mqttClient.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    if (mqttClient.connect(client_id.c_str(), MQTT_USER, MQTT_PASSWORD))
    {
      Serial.println("MQTT broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.println(mqttClient.state());
      delay(1000);
    }
  }
}