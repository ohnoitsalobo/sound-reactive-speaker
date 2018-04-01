#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
const char* ssid = "your_ssid_here";
const char* password = "your_password_here";

// #define BLYNK_PRINT Serial
// #define BLYNK_USE_DIRECT_CONNECT
#ifdef BLYNK_USE_DIRECT_CONNECT
    #include <BlynkSimpleSerialBLE.h>
#else
    #include <BlynkSimpleEsp32.h>
#endif
char auth[] = "your_blynk_auth_token_here";

void setupWiFi(){
    wifiScan();
}

void connectWiFi(){
    Serial1.println("\nStarting Wifi");
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
}

void setupOTA(){
    // Port defaults to 3232
    ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    ArduinoOTA.setHostname("speaker");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
        .onStart([]() {
            pinMode(2, OUTPUT);
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
            else // U_SPIFFS
            type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial1.println("\nStart updating " + type);
            fill_solid (leds, NUM_LEDS, CRGB::Black);
            FastLED.show();
        })
        .onEnd([]() {
            Serial1.println("\nEnd");
            delay(10);
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            digitalWrite(2, HIGH);
            Serial1.printf("Progress: %u%%\r", (progress / (total / 100)));
            digitalWrite(2, LOW);
            // uint32_t temp = progress / (total / 100);
            // Serial1.printf("Progress: %u%%\r", temp);
            // leds[map(temp, 0, 100, 0, NUM_LEDS)] = 0x111111;
            // FastLED.show();
        })
        .onError([](ota_error_t error) {
            fill_solid (leds, NUM_LEDS, CRGB::Red);
            FastLED.show();
            Serial1.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial1.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial1.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial1.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial1.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial1.println("End Failed");
        });

    ArduinoOTA.begin();
}

void wifiLoop(){
    if(WiFi.status() == WL_CONNECTED){
        ArduinoOTA.handle();
        Blynk.run();
    }
    if(WiFi.status() != WL_CONNECTED){
        EVERY_N_SECONDS(30){
            FastLED.setBrightness(music ? 40 : 15 );
            // wifiScan();
            // /* Serial1.print("\nIP Address: "); */
            // /* Serial1.println(WiFi.localIP()); */
        }
    }
}

void wifiScan(){
    Serial1.print(millis());
    Serial1.println("\nscan start");
    int n = WiFi.scanNetworks();
    Serial1.print(millis());
    Serial1.println("\nscan done");
    if (n == 0) {
        // Serial.println("no networks found");
    } else {
        for (int i = 0; i < n; ++i) {
            if(WiFi.SSID(i) == ssid){
                connectWiFi();
            }
        }
        // Serial.print(n);
        // Serial.println(" networks found");
        // for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            // Serial.print(i + 1);
            // Serial.print(": ");
            // Serial.print(WiFi.SSID(i));
            // Serial.print(" (");
            // Serial.print(WiFi.RSSI(i));
            // Serial.print(")");
            // Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            // delay(10);
        // }
    }
    // Serial.println("");
}

///////////////////////////////////

void blynkSetup(){
#ifdef BLYNK_USE_DIRECT_CONNECT
    Blynk.begin(Serial1, auth);
#else
    Blynk.config(auth);
#endif
}

BLYNK_WRITE(0){ // brightness
    int temp = param.asInt();
    FastLED.setBrightness(int((temp*temp)/255.0));
}

BLYNK_WRITE(1){ // music / auto select
    music = param.asInt();
    if(music){
        FastLED.setBrightness(255);
        nextPattern();
    } else
        FastLED.setBrightness(15);
}

BLYNK_WRITE(2){ // next pattern
    if(param.asInt())
        nextPattern();
    autoadv = false;
}

BLYNK_WRITE(3){ // L
    music = false;
    LL = param.asInt();
}

BLYNK_WRITE(4){ // R
    music = false;
    RR = param.asInt();
}

BLYNK_WRITE(5){ // hue
    manualColor.h = param.asInt();
    if(LL)
        fill_solid (LEFT , NUM_LEDS/2, manualColor);
    if(RR)
        fill_solid (RIGHT, NUM_LEDS/2, manualColor);
    FastLED.show();
}

BLYNK_WRITE(6){ // sat
    manualColor.s = param.asInt();
    if(LL)
        fill_solid (LEFT , NUM_LEDS/2, manualColor);
    if(RR)
        fill_solid (RIGHT, NUM_LEDS/2, manualColor);
    FastLED.show();
}

// BLYNK_WRITE(125){
    // MAX = param.asInt();
// }

// BLYNK_WRITE(126){
    // AMPLITUDE = param.asInt();
// }

// BLYNK_WRITE(127){
    // noise = param.asFloat() * 100;
// }

