bool music = true;          // use to change light patterns if music is playing or not

// define hardware serial object in order to use additional serial port for debugging
// ESP32 has three UARTs - the built-in '0' for programming, '1' often used for SPI flash, '2' is unused
// hardware serials can be reassigned to any unused GPIO
// if not specified in setup(), UART RX/TX pins default to: 0 = 3/1 ; 1 = 9/10 ; 2 = 16/17 
HardwareSerial Serial1(2);
// here, I am calling it Serial1 in code, but it uses UART 2
// https://github.com/G6EJD/ESP32-Using-Hardware-Serial-Ports
// https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/HardwareSerial.cpp

void setup(){
    // initialize serial ports
    Serial.begin(115200);
    // Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
    Serial1.begin(115200);
    
    // setup wifi
    setupWiFi();
    // setup OTA reprogram
    setupOTA();

    // setup Blynk for wireless control
    blynkSetup();
    // setup LEDs
    LEDsetup();
    // setup FFT
    FFTsetup();
}

void loop(){
    // maintain wifi / Blynk connectivity
    wifiLoop();
    // drive LEDs
    LEDloop();
    // compute FFTs if in music mode
    if (music)
        FFTstuff();
}
