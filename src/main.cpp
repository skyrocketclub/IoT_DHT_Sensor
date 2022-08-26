// #include <Arduino.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
// #include "ThingSpeak.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
//Contains all the credentials and Information related to AWS
#include "secrets.h"
//Neededfor implementing EEPROM
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>


//D2 on NodeMCU is GPIO 4
#define DHTPIN 4 
#define DHTTYPE DHT11 
//Instance of DHT takes two arguments
DHT dht(DHTPIN,DHTTYPE);

//Variables
int i = 0;
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;
ESP8266WebServer server(80); //port 80


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

//For the AWS Certificates 
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

PubSubClient client(net);

time_t now;
time_t nowish = 1510592825;

//Function declarations
bool testWiFi(void);
void launchWeb(void);
void setupAP(void);
void createWebServer();

//Used to set the time
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

//Called when the brokar publishes data to a topic our project suscribes to
void messageReceived(char *topic, byte *payload, unsigned int length){
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i< length; i++){
    //The payload is the message that the broker sends
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//This function helps to connect to AWS IoT
void connectAWS(){
  delay(3000);

//The default WiFi COnnection
  /*
  WiFi.mode(WIFI_STA); // wifi is in station mode - The Default
  //it firsts tries to connect to you WiFi
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);

  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
*/
//Implementing EEPROM
Serial.println("Disconnecting previously connected WiFi");
WiFi.disconnect();
EEPROM.begin(512);
delay(10);
pinMode(LED_BUILTIN,OUTPUT);
Serial.println();
Serial.println();
Serial.println("Startup");

//Reading EEPROM Memory FOR SSID & PASS
Serial.print("Reading EEPROM SSID");
String esid;
for(int i =0; i<32; i++){
  esid += char(EEPROM.read(i));
}
Serial.println();
Serial.print("SSID: ");
Serial.print(esid);
Serial.println("reading EEPROM pass");

String epass = "";
for(int i = 32; i<96; i++){
  epass += char(EEPROM.read(i));
}
Serial.print("PASS: ");
Serial.println(epass);

//You try to connect to WiFi with the details you got from EEPROM Memory
WiFi.begin(esid.c_str(),epass.c_str());
if(testWiFi()){
  //WiFi in Station Mode
  Serial.println("Succesfully COnnected!!!");
  return;
}
else{
//TIme to turn the ESP8266 to an AP - Access Point
  Serial.println("Turning the Hotspot On");
  launchWeb();
  setupAP(); 
}

Serial.println();
Serial.println("Waiting...");

while((WiFi.status() != WL_CONNECTED)){
  Serial.print(".");
  delay(100);
  server.handleClient();
}

  //When the wifi is connected, then the time is set
  NTPConnect();
  //The certifications are consumed in SSL format defined above...
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  client.setServer(MQTT_HOST,8883);
  client.setCallback(messageReceived);

  Serial.println("Connecting to AWS IOT");

  //Make sure Thing name is same as that in AWS
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

//Here your message is published, the pattern of using JSON is important
void publishMessage(){
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["humidity"] = h;
  doc["temperature"]=t;
  char jsonBuffer[512];
  serializeJson(doc,jsonBuffer);
// the topic . the jsonBuffer passed as parameters
  client.publish(AWS_IOT_PUBLISH_TOPIC,jsonBuffer);
}

//This is what tells us that it's time to switch to AP
bool testWiFi(void){
  int c =0;
  Serial.println("Waiting for WiFi to connect");
  while(c<20){
    if(WiFi.status() == WL_CONNECTED){
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("COnnection timed out, opening AP");
  return false;
}

void launchWeb(void){
  Serial.println("");
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected");
  }
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  //start the server
}

void setupAP(void){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if(n == 0){
    Serial.println("no networks found");
  }
  else{
    Serial.print(n);
    Serial.println(" networks found:");
    for(int i = 0; i< n; i++){
      //Print SSID and RSSI for each network found
      Serial.print(i+1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(") ");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
    }
  }
  Serial.println("");
  st = "<ol>";
  for(int i = 0; i<n; i++){
    //Print the SSID and RSSI for each network found
    st+= "<li>";
    st+= WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st +=") ";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("how2electronics", "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}

void createWebServer()
{
 {
    server.on("/", []() {
 
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
 
      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });
 
    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
 
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();
 
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
 
    });
  } 
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(1000);
  dht.begin();
  server.begin();
  connectAWS();
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
