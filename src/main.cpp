// #include <Arduino.h>
#include "DHT.h"

//D2 on NodeMCU is GPIO 4
#define DHTPIN 4 

#include <ESP8266WiFi.h>

const char* ssid = "Incognito";
const char* password = "biscuits";

//the type of sensor we are using
// define DHTTYPE DHT!!
#define DHTTYPE DHT11 

//Instance of DHT takes two arguments
DHT dht(DHTPIN,DHTTYPE);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  dht.begin();

  //Connecting the board to the wifi
   while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Waiting to connect to WiFi");
  }
  Serial.println("");
  Serial.println("WiFi connected ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  float h = dht.readHumidity(); // returns humility in percentage...

  //in celcius
  float t = dht.readTemperature();

  //in fahreinheit
  float f = dht.readTemperature(true); //adding the true argument means we will get the temperature value in celcius...

  //To check if any reads failed and exit early

  if(isnan(h) || isnan(t) || isnan(f)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Compute heat index in fahrenheit
  float hif = dht.computeHeatIndex(f,h);

  //compute same in celcius
  float hic = dht.computeHeatIndex(t,h,false); //fahrenheit is false...

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F/t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
  
}