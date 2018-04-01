bool music = true;

HardwareSerial Serial1(2);

void setup(){
    Serial.begin(115200);
    Serial1.begin(115200);

    setupWiFi();
    setupOTA();

    
    blynkSetup();
    
    LEDsetup();
    
    FFTsetup();
}

void loop(){
    wifiLoop();

    LEDloop();

    if (music)
        FFTstuff();
}
