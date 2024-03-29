# APRS-humiditysensor
Please star and fork it as your gratitude to my work! Thanks

## Version
1. humidty.uno => version 1.0
2. humidity.uno => version 2.0 (sending package through the server with API from header)

## What You Need
1. NodeMCU ESP8266 v3
2. DHT22 (Humidity Sensor Module)
3. GY-65 BMP085 (Digital Atmospheric Pressure Module Altitude Sensor)
4. DS18B20 (Temperature Probe)

## What You Can Do First!
1. Install Arduino
2. Install USB Driver CH340G
3. Install Additional Board Manager ESP8266

## Install Additional Board Manager ESP8266
1. Open Arduino application
2. Click File
3. Click Preferences
4. Insert "http://arduino.esp8266.com/stable/package_esp8266com_index.json" in Additional Boards Manager URLs
5. Click OK
6. Click Tools
7. Pick "NodeMCU 1.0 (ESP-12E Module)" in Board
8. Pick Upload Speed 115200

## Cable Connection
1. DHT22 to ESP8266
```bash
- (+) black to 3V
- (-) grey to G
- (out) white to D2
```
2. GY-65 BMP085 to ESP8266
```bash
- (VCC) to 3V
- (SDA) to D7
- (SCL) to D6
- (GND) to G
```
3. DS18B20 to ESP8266
```bash
- (black) to G
- (red) to 3V
- (yellow) to D4
```

## Configuration
### IP and Port Configuration
```bash
const uint16_t port = 14580; 
const char * host = "103.56.149.95"; 
WiFiClient client; 
delay(5000); // 
```

### APRS Call ID & Passcode Configuration
```bash
client.println("user PETSEL-13 pass 12984 vers WS-001 filter m/1"); 
delay (250);
```
**You can get Passcode in https://apps.magicbug.co.uk/passcode/index.php/passcode**
