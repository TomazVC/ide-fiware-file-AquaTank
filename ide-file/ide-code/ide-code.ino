/////////--------IOT--------FIAP------------///////////

#include <WiFi.h>
#include <DHTesp.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <Wire.h>
#include "ccs811.h"
#include <ArduinoJson.h>

CCS811 ccs811(23);
float val1, val2;

// Configurações de WiFi
const char *SSID = "";
const char *PASSWORD = "";

// Configurações de MQTT
const char *BROKER_MQTT = "46.17.108.113";
const int BROKER_PORT = 1883;
const char *ID_MQTT = "fiware_aquatank8";
const char *TOPIC_PUBLISH_CODOIS = "/TEF/aquatank8/attrs/c";
const char *TOPIC_PUBLISH_TVOC = "/TEF/aquatank8/attrs/v";
const char *TOPIC_PUBLISH_TEMP = "/TEF/aquatank8/attrs/t";
const char *TOPIC_PUBLISH_UMID = "/TEF/aquatank8/attrs/h";
const char *TOPIC_PUBLISH_LUMI = "/TEF/aquatank8/attrs/l";
const char *TOPIC_PUBLISH_BOIA = "/TEF/aquatank8/attrs/b";

// Configurações de Hardware
#define PIN_DHT 27
#define PUBLISH_DELAY 2000

#define Sensor1 26
int sensor1 = 1;
int nivelinicial = 0;

// Variáveis globais
WiFiClient espClient;
PubSubClient MQTT(espClient);
DHTesp dht;
unsigned long publishUpdate = 0;
TempAndHumidity sensorValues;
const int TAMANHO = 200;
// eco sensorValues;
// CCSesp ccs;

// Protótipos de funções
void updateSensorValues();
void initWiFi();
void initMQTT();
void reconnectMQTT();
void reconnectWiFi();
void checkWiFIAndMQTT();

// void updateSensorValues() {
//   sensorValues = ccs.geteco();
// }

void updateSensorValues() {
  sensorValues = dht.getTempAndHumidity();
}

void initWiFi() {
  Serial.print("Conectando com a rede: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso: ");
  Serial.println(SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar com o Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
     } else {
      Serial.println("Falha na conexão com MQTT. Tentando novamente em 2 segundos.");
      delay(2000);
    }
  }
}

void checkWiFIAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) reconnectWiFi();
  if (!MQTT.connected()) reconnectMQTT();
}

void reconnectWiFi(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Wifi conectado com sucesso");
  Serial.print(SSID);
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  dht.setup(PIN_DHT, DHTesp::DHT11);
  Serial.println("setup: Starting CCS811 basic demo");
  Serial.print("setup: ccs811 lib  version: ");
  Serial.println(CCS811_VERSION);
  initWiFi();
  initMQTT();

  Wire.begin();

  pinMode(Sensor1, INPUT);

  // Enable CCS811
  // ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
  bool ok = ccs811.begin();
  if ( !ok ) Serial.println("setup: CCS811 begin FAILED");

  // Start measuring
  ok = ccs811.start(CCS811_MODE_1SEC);
  if ( !ok ) Serial.println("setup: CCS811 start FAILED");
}

void loop() {
  checkWiFIAndMQTT();
  MQTT.loop();
  const int potPin = 34;

  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2, &etvoc, &errstat, &raw);

  int sensor1 = digitalRead(Sensor1);

  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();
    updateSensorValues();

    //CO2 E TVOC
    if ( errstat == CCS811_ERRSTAT_OK ){
      val1 = eco2;
      val2 = etvoc;

      char msgBuffer1[5];
      char msgBuffer2[5];
      // int sensorValue1 = sensorValues.eco2;

      Serial.print("CCS811: ");
      Serial.print("eco2=");
      Serial.print(val1);
      Serial.print(" ppm  ");
      dtostrf(val1, 4, 2, msgBuffer1);
      MQTT.publish(TOPIC_PUBLISH_CODOIS,msgBuffer1);
  
      Serial.print("etvoc=");
      Serial.print(val2);
      Serial.print(" ppb  ");
      Serial.println();
      dtostrf(val2, 4, 2, msgBuffer2);
      MQTT.publish(TOPIC_PUBLISH_TVOC,msgBuffer2);
      
      }
      else if ( errstat == CCS811_ERRSTAT_OK_NODATA ){
        Serial.println("CCS811: waiting for (new) data");
      }
      else if ( errstat & CCS811_ERRSTAT_I2CFAIL ){
        Serial.println("CCS811: I2C error");
      }
      else{
        Serial.print("CCS811: errstat=");
        Serial.print(errstat, HEX);
        Serial.print("=");
        Serial.println( ccs811.errstat_str(errstat) );
      }

    //temperatura
    char msgBuffer3[5];
    int sensorValue1 = sensorValues.temperature;  
    Serial.print("Temperature: ");
    Serial.println(sensorValue1);
    dtostrf(sensorValue1, 4, 2, msgBuffer3);
    MQTT.publish(TOPIC_PUBLISH_TEMP,msgBuffer3);

    //umidade
    char msgBuffer4[5];
    int sensorValue2 = sensorValues.humidity;  
    Serial.print("Humidity: ");
    Serial.println(sensorValue2);
    dtostrf(sensorValue2, 4, 2, msgBuffer4);
    MQTT.publish(TOPIC_PUBLISH_UMID,msgBuffer4);

    //luminosidade
    char msgBuffer5[5];
    int sensorValue3 = analogRead(potPin);
    Serial.print("Luminosity: ");
    Serial.println(sensorValue3);
    int luminosity = map(sensorValue3, 0, 4095, 0, 100);
    Serial.println(luminosity);
    dtostrf(luminosity, 4, 2, msgBuffer5);
    MQTT.publish(TOPIC_PUBLISH_LUMI,msgBuffer5);
    }

    //boia
    char msgBuffer6[5];
    dtostrf(sensor1, 4, 2, msgBuffer6);
    MQTT.publish(TOPIC_PUBLISH_BOIA,msgBuffer6);

    if (sensor1 == 1){
      Serial.print("Boia: ");
      Serial.println(sensor1);
    }
    else if (sensor1 == 0){
      Serial.print("Boia: ");
      Serial.println(sensor1);
    }
    
    // Wait
    delay(6000);
}
