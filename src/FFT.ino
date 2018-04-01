#include <arduinoFFT.h>

#define SAMPLES 512              // Must be a power of 2
#define SAMPLING_FREQUENCY 40000 // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
// #define AMPLITUDE 150.0          // Depending on your audio source level, you may need to increase this value
#define Left  true
#define Right false
// #define MAX 255.0

float MAX = 725;
int AMPLITUDE = 25;
int noise = 1000;

unsigned int sampling_period_us;
unsigned long microseconds;
long int Lpeak[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
long int Rpeak[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

double LvReal[SAMPLES];
double LvImag[SAMPLES];

double RvReal[SAMPLES];
double RvImag[SAMPLES];

unsigned long newTime, oldTime;

arduinoFFT LFFT = arduinoFFT(LvReal, LvImag, SAMPLES, SAMPLING_FREQUENCY);
arduinoFFT RFFT = arduinoFFT(RvReal, RvImag, SAMPLES, SAMPLING_FREQUENCY);

void FFTsetup(){
    sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
}

void FFTstuff(){
    for (int i = 0; i < SAMPLES; i++) {
        newTime = micros()-oldTime;
        oldTime = newTime;
        LvReal[i] = analogRead(39);
        LvImag[i] = 0;
        // RvReal[i] = analogRead(36);
        // RvImag[i] = 0;
        while (micros() < (newTime + sampling_period_us)) {  }
    }
    for (int i = 0; i < SAMPLES; i++) {
        newTime = micros()-oldTime;
        oldTime = newTime;
        // LvReal[i] = analogRead(39);
        // LvImag[i] = 0;
        RvReal[i] = analogRead(36);
        RvImag[i] = 0;
        while (micros() < (newTime + sampling_period_us)) {  }
    }
    
    // LFFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    LFFT.Windowing(FFT_WIN_TYP_HANN, FFT_FORWARD);
    LFFT.Compute(FFT_FORWARD);
    LFFT.ComplexToMagnitude();

    // RFFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    RFFT.Windowing(FFT_WIN_TYP_HANN, FFT_FORWARD);
    RFFT.Compute(FFT_FORWARD);
    RFFT.ComplexToMagnitude();
    
    processBands();

}

void processBands(){
    for (byte band = 0; band <= 13; band++) {
        Lpeak[band] = 0;
        Rpeak[band] = 0;
    }
    for (int i = 2; i < (SAMPLES/2); i++){ // Don't use sample 0 and only the first SAMPLES/2 are usable.
        // Each array element represents a frequency and its value, is the amplitude. Note the frequencies are not discrete.
        if (LvReal[i] > noise || RvReal[i] > noise) { // Add a crude noise filter, 10 x amplitude or more
            if (i<=2 )             { displayBand(Left, 0,int(LvReal[i]*1.5));  displayBand(Right, 0,int(RvReal[i]*1.5)); }
            if (i >2   && i<=4 )   { displayBand(Left, 1,int(LvReal[i]  ));  displayBand(Right, 1,int(RvReal[i]  )); }
            if (i >4   && i<=7 )   { displayBand(Left, 2,int(LvReal[i]  ));  displayBand(Right, 2,int(RvReal[i]  )); }
            if (i >7   && i<=15 )  { displayBand(Left, 3,int(LvReal[i]  ));  displayBand(Right, 3,int(RvReal[i]  )); }
            if (i >15  && i<=20 )  { displayBand(Left, 4,int(LvReal[i]  ));  displayBand(Right, 4,int(RvReal[i]  )); }
            if (i >20  && i<=28 )  { displayBand(Left, 5,int(LvReal[i]  ));  displayBand(Right, 5,int(RvReal[i]  )); }
            if (i >28  && i<=40 )  { displayBand(Left, 6,int(LvReal[i]  ));  displayBand(Right, 6,int(RvReal[i]  )); }
            if (i >40  && i<=46 )  { displayBand(Left, 7,int(LvReal[i]  ));  displayBand(Right, 7,int(RvReal[i]  )); }
            if (i >46  && i<=55 )  { displayBand(Left, 8,int(LvReal[i]  ));  displayBand(Right, 8,int(RvReal[i]  )); }
            if (i >55  && i<=70 )  { displayBand(Left, 9,int(LvReal[i]  ));  displayBand(Right, 9,int(RvReal[i]  )); }
            if (i >70  && i<=90 )  { displayBand(Left,10,int(LvReal[i]  ));  displayBand(Right,10,int(RvReal[i]  )); }
            if (i >90  && i<=120 ) { displayBand(Left,11,int(LvReal[i]  ));  displayBand(Right,11,int(RvReal[i]  )); }
            if (i >120 && i<=180 ) { displayBand(Left,12,int(LvReal[i]  ));  displayBand(Right,12,int(RvReal[i]  )); }
            if (i >180 && i<=288 ) { displayBand(Left,13,int(LvReal[i]  ));  displayBand(Right,13,int(RvReal[i]  )); }
            // if (i >288           ) { displayBand(Left, 7,int(LvReal[i]  ));  displayBand(Right, 7,int(RvReal[i]  )); }
        }
    }
    // for (byte band = 0; band <= 13; band++) {
        // Serial1.print(Lpeak[band]);
        // Serial1.print("\t");
    // }
    // Serial1.print("\t");
    // for (byte band = 0; band <= 13; band++) {
        // Serial1.print(Rpeak[band]);
        // Serial1.print("\t");
    // }
    // Serial1.print("\r");
}
    
void displayBand(bool channel, int band, int dsize){
    int dmax = MAX;
    dsize /= AMPLITUDE;
    if (dsize > dmax) dsize = dmax;
    if (channel == Left ) {Lpeak[band] = dsize;}
    if (channel == Right) {Rpeak[band] = dsize;}
    
}

// void processBands(){
    // for (byte band = 0; band <= 7; band++) {
        // Lpeak[band] = 0;
        // Rpeak[band] = 0;
    // }
    // for (int i = 2; i < (SAMPLES/2); i++){ // Don't use sample 0 and only the first SAMPLES/2 are usable.
        // /* Each array element represents a frequency and its value, is the amplitude. Note the frequencies are not discrete. */
        // if (LvReal[i] > noise || RvReal[i] > noise) { // Add a crude noise filter, 10 x amplitude or more
            // if (i<=2 )             { displayBand(Left,0,int(LvReal[i]*2  ));  displayBand(Right,0,int(RvReal[i]*2  )); }
            // if (i >2   && i<=4 )   { displayBand(Left,1,int(LvReal[i]/2  ));  displayBand(Right,1,int(RvReal[i]/2  )); }
            // if (i >4   && i<=7 )   { displayBand(Left,2,int(LvReal[i]/3  ));  displayBand(Right,2,int(RvReal[i]/3  )); }
            // if (i >7   && i<=15 )  { displayBand(Left,3,int(LvReal[i]/8  ));  displayBand(Right,3,int(RvReal[i]/8  )); }
            // if (i >15  && i<=40 )  { displayBand(Left,4,int(LvReal[i]/25 ));  displayBand(Right,4,int(RvReal[i]/25 )); }
            // if (i >40  && i<=70 )  { displayBand(Left,5,int(LvReal[i]/20 ));  displayBand(Right,5,int(RvReal[i]/20 )); }
            // if (i >70  && i<=288 ) { displayBand(Left,6,int(LvReal[i]/40 ));  displayBand(Right,6,int(RvReal[i]/40 )); }
            // if (i >288           ) { displayBand(Left,7,int(LvReal[i]    ));  displayBand(Right,7,int(RvReal[i]    )); }
        // }
    // }
    // for (byte band = 0; band <= 7; band++) {
        // Serial1.print(Lpeak[band]);
        // Serial1.print("\t");
    // }
    // Serial1.print("\t");
    // for (byte band = 0; band <= 7; band++) {
        // Serial1.print(Rpeak[band]);
        // Serial1.print("\t");
    // }
    // Serial1.print("\r");
// }
    
// void displayBand(bool channel, int band, int dsize){
    // int dmax = MAX;
    // dsize /= AMPLITUDE;
    // if (dsize > dmax) dsize = dmax;
    // if (channel == Left ) {Lpeak[band] += dsize;}
    // if (channel == Right) {Rpeak[band] += dsize;}
    
// }
