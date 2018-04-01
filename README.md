# Sound reactive speaker (stereo)

This code runs using the Arduino core on Espressif ESP32. It is based largely on the stereo version of the code developed by @G6EJD here: https://github.com/G6EJD/ESP32-8-Octave-Audio-Spectrum-Display

The code is written in Notepad++ and compiled using Platformio (in my case, command-line). It should work straight out of the box once the libraries (below) and the Arduino Core for ESP32 is installed: https://github.com/espressif/arduino-esp32

Libraries needed:

- ArduinoFFT: https://github.com/kosme/arduinoFFT
- FastLED: https://github.com/samguyer/FastLED (this is a fork better suited for the ESP32)
- Blynk: https://github.com/blynkkk/blynk-library
