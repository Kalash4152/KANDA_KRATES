#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 19
#define DHTTYPE DHT22

#define MQ135_PIN 33
#define MQ137_PIN 35
#define MQ136_PIN 32

const char* ssid = "VDS-S6+";
const char* password = "Vds@1234";

const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);

void connectWiFi() {

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
}

void connectMQTT() {

  while (!client.connected()) {

    Serial.print("Connecting to MQTT...");

    if (client.connect("ESP32_KANDA")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed. Retrying...");
      delay(2000);
    }

  }

}

void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(MQ135_PIN, INPUT);
  pinMode(MQ137_PIN, INPUT);
  pinMode(MQ136_PIN, INPUT);

  connectWiFi();

  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  int mq135 = analogRead(MQ135_PIN);
  int mq137 = analogRead(MQ137_PIN);
  int mq136 = analogRead(MQ136_PIN);

  String payload = "{";
  payload += "\"temperature\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity) + ",";
  payload += "\"mq135\":" + String(mq135) + ",";
  payload += "\"mq137\":" + String(mq137) + ",";
  payload += "\"mq136\":" + String(mq136);
  payload += "}";

  client.publish("kanda/crate1/batch1", payload.c_str());

  Serial.println("Published sensor data:");
  Serial.println(payload);

  delay(5000);
}