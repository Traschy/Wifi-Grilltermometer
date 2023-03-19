#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <ESPAsyncWiFiManager.h>
#include <SH1106.h> 

//  config Display 1.3 zoll
SH1106Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL


// Set A/D wandler
 Adafruit_ADS1115 ads;
 int16_t adc0, adc1, adc2, adc3;
 float temp1;
 float temp2;

// AsyncWebServer object auf Port 80
AsyncWebServer server(80);

DNSServer dns;

//  Kerntemperatur
String getTemperature1() {
  float temperature1 = temp1;
  Serial.println(temperature1);
  return String(temperature1);
}

//  Rost temperatur  
String getTemperature2() {
  float temperature2 = temp2;
  Serial.println(temperature2);
  return String(temperature2);
}

String processor(const String& var){
 
  if (var == "TEMPERATURE1"){
  return getTemperature1();
  }
  else if (var == "TEMPERATURE2"){
    return getTemperature2();
  }
}


void setup(){
 
// Serial port einstellen
  Serial.begin(115200);

// Initialize Display
  display.init();
    display.flipScreenVertically();
    display.setContrast(255);
 
// I2C ADS1115
  ads.begin(); 
    
// Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
    }
// Starte WIFI Manager  
  AsyncWiFiManager wifiManager(&server,&dns);
     wifiManager.autoConnect("Grillthermometer");
      Serial.println("connected...yeey :)");
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Grillthermometer connected\n" + WiFi.localIP().toString());
      display.display();
      
      
  
// Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
// Route to load style.css file
  server.on("/main.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/main.css", "text/css");
  });
  
   server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature1().c_str());
  });
  
  server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature2().c_str());
  });


// Start server
  server.begin();
  Serial.println("HTTP server started");
   
}
 
void loop(){


// ADC adressierung

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  temp1 = 0.0001725768 * pow(map(adc0 ,15000 ,17800 ,1 , 1023) - 358.9383561644, 2) + 20.2409402426;
  temp2 = 0.0001725768 * pow(map(adc1 ,15000 ,17800 ,1 , 1023) - 358.9383561644, 2) + 20.2409402426;
 
  delay(1000);
  
}
