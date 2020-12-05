// SmallMeteo parser to APRS v0.1
// http://infotex58.ru
// Огромное спасибо Михаилу aka TurangaLeela за парсер данных! Many thanks TurangaLeela!
 
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
//#define DHTTYPE DHT11
 
#define USE_SERIAL Serial
#define SLEEP_DELAY 3000
 
ESP8266WiFiMulti WiFiMulti;
DHT dht(DHTPIN, DHTTYPE);
 
float dst,bt,bp,ba;
bool bmp085_present=true;
 
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
 
  USE_SERIAL.begin(115200); // Скорость порта 
  USE_SERIAL.flush();    
  delay(1000);
  
  WiFiMulti.addAP("Azzam", "azzam53a"); // SSID и пароль точки доступа WIFI
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(10);
  if (!bmp.begin()) {
    USE_SERIAL.println("No BMP085 sensor detected!");
    bmp085_present=false;
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
    
 
        const uint16_t port = 14580; // Указываем порт сервера
        const char * host = "rotate.aprs2.net"; // Указываем имя хоста или его IP
        WiFiClient client; // Включаем режим WI-FI клиента
        delay(5000); // курим 5 сек
 
        if (!client.connect(host, port)) {
           // Если приконектились к серверу то едим дальше иначе ждём ? сек
            
           return;
        }
 
        client.println("user YD0AVF-13 pass 19617 vers WS-001 filter m/1");  // Логинемся на сервер user UR5TLZ-13 pass 24610 от aprsdroid
        delay (250);
 
 
        DS18B20.requestTemperatures();
        dst = DS18B20.getTempCByIndex(0);
        USE_SERIAL.print("Temperature: ");
        USE_SERIAL.println(dst);
 
        dst=(dst*1.8)+32;
 
        if(bmp085_present) {
          bt = (bmp.readTemperature() * 9/5) + 32;//bmp.readTemperature();
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
         float t22 = dht.readTemperature(false);
      
 
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
     
        const int & f = bt; // Температура
        const int & h = h22; // Влажность
        const int & p = bp; // Давление
         
        USE_SERIAL.println(f); // смотрим в порту что получаем
        USE_SERIAL.println(h);
        USE_SERIAL.println(p);
 
        client.print("YD0AVF-13>APRSWX,TCPIP*,qAC,WIDE1-1:=0614.68S/10645.29E_");     // Поехали кидать инфу на сервак (Указываем позывной и координаты)
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
        client.print("b"); client.print(p);client.println(" "); // Добиваем коммент
        }
        else
        {
        client.print("b0"); client.print(p); client.println(" SERVER APRS"); //Добиваем коммент
        }
        client.println("YD0AVF-13>APRSWX,TCPIP*,qAC,WIDE1-1:> }h05EEWEWWEW{"); // Статус сообщение
        //client.println("CW0002-13>APRSWX,TCPIP*,qAC,WIDE1-1:>"); // Статус сообщение
        delay(1000); // Ждём 10 мин перед следующей отправкой пакета
      
    }
}
