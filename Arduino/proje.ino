
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <PubSubClient.h>

//needed for library
#include <DNSServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
char mqtt_server[16] = "10.0.1.78";
char mqtt_port[6] = "1883";
char mqtt_role[2] = "T";
char mqtt_ping[10] = "ping";
char mqtt_report[10] = "report";
long lastMsg = 0;
char msg[50];
int value = 0;
bool transmitter = true;
const byte thePin = 13; //D7
const int led = LED_BUILTIN;
long interruptTime = 0;
WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    managerSetup();
    mqttSetup();
    pinConfig();
}
void handleInterrupt(){
  Serial.println("Interrupt Occured.");
  interruptTime = micros();
}
void pinConfig(){
  if(transmitter){
    pinMode(thePin, OUTPUT);
  }else{
    pinMode(thePin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(thePin), handleInterrupt, FALLING);
  }
}
void managerSetup(){
    WiFiManager wifiManager;
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 16);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
    WiFiManagerParameter custom_mqtt_role("role", "mqtt role", mqtt_role, 2);
    WiFiManagerParameter custom_mqtt_ping("ping", "mqtt ping topic", mqtt_ping, 10);
    WiFiManagerParameter custom_mqtt_report("repot", "mqtt report topic", mqtt_report, 10);
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_mqtt_role);
    wifiManager.addParameter(&custom_mqtt_ping);
    wifiManager.addParameter(&custom_mqtt_report);
    wifiManager.resetSettings();
    wifiManager.autoConnect("Wemos", "123321123");
    Serial.println("connected...yeey :)");
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    strcpy(mqtt_role, custom_mqtt_role.getValue());
    strcpy(mqtt_ping, custom_mqtt_ping.getValue());
    strcpy(mqtt_report, custom_mqtt_report.getValue());
    transmitter = mqtt_role[0] == 'T';
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), "test", "test")) {
      Serial.print("Connected to server:");
      Serial.print(mqtt_server);
      Serial.print(" as: ");
      Serial.println(transmitter? "transmitter" : "receiver");
      if(!transmitter){
        client.subscribe(mqtt_ping);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void mqttSetup(){
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);
}
void transmit(){
  long now = millis();
  if(now - lastMsg > 1000){
    digitalWrite(thePin, HIGH);
  }
  if (now - lastMsg > 2000) {
    digitalWrite(thePin, LOW);
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "transmitter:%ld", value);
    Serial.print("Publish message: ");
    Serial.print(msg);
    Serial.print(". To channel: ");
    Serial.println(mqtt_ping);
    client.publish(mqtt_ping, msg);
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  char copyPayload[40];
  strncpy(copyPayload, (char*)payload, length);
  copyPayload[length] = '\0';
  const char *castee = (char*)copyPayload;
  int num = atoi(strchr(castee, ':') + 1);
  Serial.println();
  long timeDiff = micros() - interruptTime;
  //snprintf (msg, 50, "receiver:%ld", num);
  snprintf (msg, 50, "receiver:%ld-%ld", num, timeDiff);
  client.publish(mqtt_report, msg);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //Serial.println(mqtt_role[0]);
  if(transmitter){
    transmit();
  }
}

