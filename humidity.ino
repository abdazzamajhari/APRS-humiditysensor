#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdlib_noniso.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
 
#define ONE_WIRE_BUS 2  // DS18B20 pin 2 по nodemcu 0.9 D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
 
#define I2C_SCL 12      // Barometric Pressure Sensor (BMP085)
#define I2C_SDA 13      // Barometric Pressure Sensor (BMP085)
Adafruit_BMP085 bmp;
 
#define DHTPIN 4 
#define DHTTYPE DHT22  
 
#define USE_SERIAL Serial
#define SLEEP_DELAY 3000
 
ESP8266WiFiMulti WiFiMulti;
DHT dht(DHTPIN, DHTTYPE);
 
float dst, bt, bp, ba;
bool bmp085_present = true;
 
char * skipControlChars(char * sLine) {
    char * startFrom = sLine;
  
    while(true) {
        if(nullptr == sLine) {
            break;
        }
        if('#' == (*sLine)) {
            if((sLine - startFrom) > 0) {
                --sLine;
            } else {
                sLine = nullptr;
            }   
            break;
        } else if('\0' == (*sLine)) {
            sLine = nullptr;
        
            break;
        }
        ++sLine;
    }
    return sLine;
}
 
void setup() {
  USE_SERIAL.begin(115200); 
  USE_SERIAL.flush();    
  delay(1000);
  WiFiMulti.addAP("LKTI_24Ghz", "labir2121"); 
 
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(10);
  if (!bmp.begin()) {
    USE_SERIAL.println("No BMP085 sensor detected!");
    bmp085_present = false;
  }
    dht.begin(); 
}
 
void closeConnection(HTTPClient * pClientToClose) {
  pClientToClose -> end();
  delay(SLEEP_DELAY);
}
 
void loop() {
  if((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;

        // APRS Server
        const uint16_t port = 14580; 
        const char * host = "103.56.149.95"; 
        
        WiFiClient client; 
        delay(5000); // 
 
        if (!client.connect(host, port)) {           
           return;
        }

        // CALLSIGN & PASSCODE
        client.println("user PETSEL-13 pass 12984 vers WS-001 filter m/1"); 
        delay (250);
 
        DS18B20.requestTemperatures();
        dst = DS18B20.getTempCByIndex(0);
        USE_SERIAL.print("Temperature: ");
        USE_SERIAL.println(dst);
 
        dst=(dst*1.8)+32;
 
        if(bmp085_present) {
          bt = bmp.readTemperature();//(bmp.readTemperature() * 9/5) + 32;
          USE_SERIAL.print("Temperature bmp: ");
          USE_SERIAL.println(bt);
        }
  
        if(bmp085_present) {
          bp = bmp.readPressure()/10;//133.3224;// / 3386;
          USE_SERIAL.print("Pressure: ");
          USE_SERIAL.println(bp);
        }
   
        if(bmp085_present) {
          USE_SERIAL.print("Real altitude = ");
          USE_SERIAL.print(bmp.readAltitude(101500));
          USE_SERIAL.println(" meters");
          ba=bmp.readAltitude(101500);
        }
 
         float h22 = dht.readHumidity(); 
         float t22 = dht.readTemperature();
 
        if (isnan(t22) || isnan(h22)) 
        { 
          Serial.println("Failed to read from DHT"); 
        } 
        else
        {
          USE_SERIAL.print("Temperature dht: ");
          USE_SERIAL.println(t22);
          USE_SERIAL.print("Humidity dht: ");
          USE_SERIAL.println(h22);
        }
     
        const int & f = dst; 
        const int & h = h22; 
        const int & p = bp; 
         
        USE_SERIAL.println(f); 
        USE_SERIAL.println(h);
        USE_SERIAL.println(p);

        // Configuration API Server
        // === START ===
       

        // Insert to API Server
        // === START ===
        // Your Domain name with URL path or IP address with path
        // Prepare your HTTP POST request data
        String httpRequestData = "https://dev.azhamudev.com/api/log_sensor.php?dst="+String(f)+"&h22="+String(h)+"&bp="+String(p);
        
        http.begin(httpRequestData);
//        httpRequestData.begin();
        
        Serial.print("httpRequestData: ");
        Serial.println(httpRequestData);
        // === END ===
 
        client.print("PETSEL-13>APRSWX,TCPIP*,qAC,WIDE1-1:=0615.10S/10645.38E_"); 
        client.print("000/000g...");
        if (f >= 0)
        {
        client.print("t0"); client.print(f);
        }
        else
        {
        client.print("t"); client.print(f);
        }
        client.print("r...p...P...h"); client.print(h);  
        
        if (p >= 10000)
        {
        client.print("b"); client.print(p); client.println(" ");
        }
        else
        {
        client.print("b0"); client.print(p); client.println(" "); 
        }
        client.println("PETSEL-13>APRSWX,TCPIP*,qAC,WIDE1-1:>"); 
        
        delay(10000);  
    }
}
