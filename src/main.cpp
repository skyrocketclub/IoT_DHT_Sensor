// #include <Arduino.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
// #include "ThingSpeak.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"


//D2 on NodeMCU is GPIO 4
#define DHTPIN 4 
#define DHTTYPE DHT11 
//Instance of DHT takes two arguments
DHT dht(DHTPIN,DHTTYPE);

// const char* ssid = "Incognito";
// const char* password = "biscuits";
// unsigned long myChannelNumber = 1832709;
// const char * myWriteAPIKey = "VQIG4M1SVDOMM9HH";
// const char* server = "api.thingspeak.com";


//Initialize the client library...
// WiFiClient client;
//the type of sensor we are using
// define DHTTYPE DHT!!




// void updateThingSpeak(float t, float h){
//   // ThingSpeak.setField(1,t);
//   // ThingSpeak.setField(2,h);
//   // ThingSpeak.writeFields(myChannelNumber,myWriteAPIKey);
//   delay(20*1000); //ThingSpeak recommends a delay of 20 seconds
// }

float h;
float t;
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;

#define AWS_IOT_PUBLISH_TOPIC "esp8266/pub"
#define AWS_IOT_SUSCRIBE_TOPIC "esp8266/sub"

WiFiClientSecure net;

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

PubSubClient client(net);

time_t now;
time_t nowish = 1510592825;

void NTPConnect(void){
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while(now < nowish){
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now,&timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void messageReceived(char *topic, byte *payload, unsigned int length){
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i< length; i++){
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void connectAWS(){
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);

  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  NTPConnect();
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  client.setServer(MQTT_HOST,8883);
  client.setCallback(messageReceived);

  Serial.println("Connecting to AWS IOT");

  while(!client.connect(THINGNAME)){
    Serial.print(".");
    delay(1000);
  }

  if(!client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  //subscribe to a topic
  client.subscribe(AWS_IOT_SUSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}

void publishMessage(){
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["humidity"] = h;
  doc["temperature"]=t;
  char jsonBuffer[512];
  serializeJson(doc,jsonBuffer);

  client.publish(AWS_IOT_PUBLISH_TOPIC,jsonBuffer);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(1000);
  dht.begin();
  // ThingSpeak.begin(client);
  // WiFi.begin(ssid,password);
  

  //Connecting the board to the wifi
  //  while(WiFi.status() != WL_CONNECTED){
  //   delay(500);
  //   Serial.println("Waiting to connect to WiFi");
  // }

  // Serial.println("");
  // Serial.println("WiFi connected ");
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP()); 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  h = dht.readHumidity(); // returns humility in percentage...

  //in celcius
  t = dht.readTemperature();

  //in fahreinheit
  // float f = dht.readTemperature(true); //adding the true argument means we will get the temperature value in celcius...

  //To check if any reads failed and exit early

  if(isnan(h) || isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Compute heat index in fahrenheit
  // float hif = dht.computeHeatIndex(f,h);

  //compute same in celcius
  // float hic = dht.computeHeatIndex(t,h,false); //fahrenheit is false...

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(" %t");
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F(" *C "));
  // Serial.print(f);
  // Serial.print(" *F/t");
  // Serial.print("Heat index: ");
  // Serial.print(hic);
  // Serial.print(" *C ");
  // Serial.print(hif);
  // Serial.println(" *F");

  // if(client.connect(server,80)){
  //   updateThingSpeak(t,h);
  // }
  now = time(nullptr);
  if(!client.connected()){
    connectAWS();
  }
  else{
    client.loop();
    if(millis() - lastMillis >5000){
      lastMillis = millis();
      publishMessage();
    }
  }
  
}
