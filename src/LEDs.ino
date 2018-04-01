// #define FASTLED_ALLOW_INTERRUPTS 0
// #define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define LED_PINS    13
#define NUM_LEDS    66
#define BRIGHTNESS  255
#define FRAMES_PER_SECOND  120

CRGBArray<NUM_LEDS> leds;
CRGBSet LEFT  (leds (0,            NUM_LEDS/2-1)   );
CRGBSet RIGHT (leds (NUM_LEDS/2,   NUM_LEDS)       );
CRGBSet L1    (leds (0,            NUM_LEDS/4-1)   );
CRGBSet L2    (leds (NUM_LEDS/4,   NUM_LEDS/2-1)   );
CRGBSet R1    (leds (NUM_LEDS/2,   3*NUM_LEDS/4-1) );
CRGBSet R2    (leds (3*NUM_LEDS/4, NUM_LEDS)       );
CHSV manualColor(0, 255, 255);

CRGBPalette16 currentPalette, randomPalette1;
CRGBPalette16 targetPalette, randomPalette2;
TBlendType    currentBlending;
uint8_t maxChanges = 24;        // Value for blending between palettes.

static int16_t dist1, dist2;            // A moving location for our noise generator.
uint16_t xscale = 30;           // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
uint16_t yscale = 30;           // Wouldn't recommend changing this on the fly, or the animation will be really blocky.

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { simplePattern1/* , simplePattern1, simplePattern2, simplePattern3 */ };
SimplePatternList gPatterns1 = { rainbow, rainbowWithGlitter, beatwave, confetti, sinelon, inoise8_mover/* , juggle, dot_beat, bpm, blendwave */ };
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0, gHue1 = 0, gHue2 = 0; // rotating "base color" used by many of the patterns
bool autoadv = false, LL = false, RR = false;

void LEDsetup(){
    FastLED.addLeds< LED_TYPE, LED_PINS, COLOR_ORDER >( leds, NUM_LEDS ).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);

    currentPalette = RainbowColors_p;
    targetPalette  = PartyColors_p;
    currentBlending = LINEARBLEND;

    dist1 = random16(12345);                                     // A semi-random number for our noise generator
    dist2 = random16(12345);                                     // A semi-random number for our noise generator
}

void LEDloop(){
    EVERY_N_MILLISECONDS( 20 ) {
        nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
        if(!autoadv){
            gHue1++; gHue2--;
        }
    }     // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS(5){
        targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
        randomPalette1 = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
        randomPalette2 = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
    }
    EVERY_N_SECONDS( 60 ){
        if(autoadv)
            nextPattern();
    }

    if(music)
        gPatterns[gCurrentPatternNumber]();
    else{
        if(!LL & !RR){
            EVERY_N_MILLISECONDS( 1000 / FRAMES_PER_SECOND ){
                gPatterns1[gCurrentPatternNumber]();         // Call the current pattern function once, updating the 'leds' array
            }
        }
    }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
    // add one to the current pattern number, and wrap around at the end
    if(music)
        gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns );
    else
        gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns1);

    // if(gCurrentPatternNumber == 3 || gCurrentPatternNumber == 4 || gCurrentPatternNumber == 5)
        // FastLED.setBrightness(60);
    // else
        // FastLED.setBrightness(25);
    
    // Serial1.print("\ngCurrentPatternNumber = ");
    // Serial1.println(gCurrentPatternNumber);
}


void simplePattern0(){ // frequencies in sequence one LED at a time
    for(int i = 0; i < NUM_LEDS/2; i++){
        LEFT[i]  = CHSV( 224*i/(NUM_LEDS/2.0), 255, (Lpeak[i/3] / MAX * 255));
        RIGHT[i] = CHSV( 224*i/(NUM_LEDS/2.0), 255, (Rpeak[i/3] / MAX * 255));
    }
    FastLED.show();
}

void simplePattern1(){ // frequencies in sequence one LED at a time
    for(int i = 0; i < NUM_LEDS/4; i++){
        // int pos = i/2.35; 
        int pos = i/1.4; 
        float left  = Lpeak[pos] / MAX; // left =  left *2 - left *left /255;
        float right = Rpeak[pos] / MAX; // right = right*2 - right*right/255;
        // L1[i]              = CHSV( 224*i/(NUM_LEDS/4.0), 255, (Lpeak[pos] / MAX * 255));
        // R1[i]              = CHSV( 224*i/(NUM_LEDS/4.0), 255, (Rpeak[pos] / MAX * 255));
        // L2[NUM_LEDS/4-i]   = CHSV( 224*i/(NUM_LEDS/4.0), 255, (Lpeak[pos] / MAX * 255));
        // R2[NUM_LEDS/4-i]   = CHSV( 224*i/(NUM_LEDS/4.0), 255, (Rpeak[pos] / MAX * 255));
        L1[i]              = CHSV( 224*i/(NUM_LEDS/4.0), 255, left  * 255.0);
        R1[i]              = CHSV( 224*i/(NUM_LEDS/4.0), 255, right * 255.0);
        L2[NUM_LEDS/4-i]   = CHSV( 224*i/(NUM_LEDS/4.0), 255, left  * 255.0);
        R2[NUM_LEDS/4-i]   = CHSV( 224*i/(NUM_LEDS/4.0), 255, right * 255.0);
    }
    leds[16] = leds[15];  leds[49] = leds[50];
    FastLED.show();
}

void simplePattern2(){ // frequencies in sequence one LED at a time
    for(int i = 0; i < NUM_LEDS/2; i++){
        leds[i]            = CHSV( i/33.0*224, 255, (Lpeak[i/3] / MAX * 255));
        leds[i+NUM_LEDS/2] = CHSV( i/33.0*224, 255, (Rpeak[i/3] / MAX * 255));
    }
    FastLED.show();
}

void simplePattern3(){ // frequencies in sequence one LED at a time
    for(int i = 0; i < NUM_LEDS/2; i++){
        leds[i]            = CHSV( i/33.0*224, 255, (Lpeak[i/3] / MAX * 255));
        leds[i+NUM_LEDS/2] = CHSV( i/33.0*224, 255, (Rpeak[i/3] / MAX * 255));
    }
    FastLED.show();
}

////////////////////////////////

void rainbow() {
    // FastLED's built-in rainbow generator
    fill_rainbow( RIGHT, NUM_LEDS/2, gHue1, 7);
    fill_rainbow( LEFT , NUM_LEDS/2, gHue2, 7);
    FastLED.show();
}

void rainbowWithGlitter() {
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    // addGlitter(80);
    addGlitter();
    FastLED.show();
}

void addGlitter() {
    if( random8() < 80) {
        leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV( random8(255), 255, 255);
    FastLED.show();
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos1 = beatsin16(11 ,0,NUM_LEDS/2-1);
    int pos2 = beatsin16(13,0,NUM_LEDS/2-1);
    int pos3 = beatsin16(9 ,0,NUM_LEDS/2-1);
    int pos4 = beatsin16(15,0,NUM_LEDS/2-1);
    LEFT [pos1] = ColorFromPalette(randomPalette1, pos1, 255, LINEARBLEND);   // Use that value for both the location as well as the palette index colour for the pixel.
    RIGHT[pos2] = ColorFromPalette(randomPalette2, pos2, 255, LINEARBLEND);   // Use that value for both the location as well as the palette index colour for the pixel.
    LEFT [pos3] += CHSV( gHue2, 255, 255);
    RIGHT[pos4] += CHSV( gHue1, 255, 255);
    FastLED.show();
}

void dot_beat() {
    uint8_t fadeval = 190;                                        // Trail behind the LED's. Lower => faster fade.
    uint8_t BPM = 30;


    uint8_t inner = beatsin8(BPM, NUM_LEDS/4, NUM_LEDS/4*3);    // Move 1/4 to 3/4
    uint8_t outer = beatsin8(BPM, 0, NUM_LEDS-1);               // Move entire length
    uint8_t middle = beatsin8(BPM, NUM_LEDS/3, NUM_LEDS/3*2);   // Move 1/3 to 2/3

    leds[outer]  = CHSV( gHue1    , 200, 255); // CRGB::Aqua;
    leds[middle] = CHSV( gHue1+96 , 200, 255); // CRGB::Purple;
    leds[inner]  = CHSV( gHue1+160, 200, 255); // CRGB::Blue;

    nscale8(leds,NUM_LEDS,fadeval);                             // Fade the entire array. Or for just a few LED's, use  nscale8(&leds[2], 5, fadeval);
    FastLED.show();
} // dot_beat()

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy( leds, NUM_LEDS, 30);
    byte dothue1 = 0, dothue2 = 0;
    for( int i = 0; i < 8; i++) {
        RIGHT[beatsin16(i+7,0,NUM_LEDS/2)] |= CHSV(dothue1, 200, 255);
        LEFT [beatsin16(i+5,0,NUM_LEDS/2)] |= CHSV(dothue2, 200, 255);
        dothue1 += 32;
        dothue2 -= 32;
        yield();
    }
    FastLED.show();
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for( int i = 0; i < NUM_LEDS; i++) { //9948
        leds[i] = ColorFromPalette(palette, gHue1+(i*2), beat-gHue1+(i*10));
        yield();
    }
    FastLED.show();
}

void blendwave() {
    // CRGB clr1, clr2;
    // uint8_t speed, loc1; // loc2, ran1, ran2;

    // speed = beatsin8(6,0,255);
    
    // if(manual && _cycle){
        // clr1 = blend(gradient1, gradient2, speed);
        // clr2 = blend(gradient2, gradient1, speed);
    // }else{
        // clr1 = blend(CHSV(beatsin8(3,0,255),255,255), CHSV(beatsin8(4,0,255),255,255), speed);
        // clr2 = blend(CHSV(beatsin8(4,0,255),255,255), CHSV(beatsin8(3,0,255),255,255), speed);
    // }
    // loc1 = beatsin8(10,0,NUM_LEDS-1);

    // fill_gradient_RGB(leds, 0, clr2, loc1, clr1);
    // fill_gradient_RGB(leds, loc1, clr2, NUM_LEDS-1, clr1);
    // FastLED.show();
} // blendwave()


void beatwave() {
  
    uint8_t wave1 = beatsin8(9, 0, 255), wave5 = beatsin8(8, 0, 255);                        // That's the same as beatsin8(9);
    uint8_t wave2 = beatsin8(8, 0, 255), wave6 = beatsin8(7, 0, 255);
    uint8_t wave3 = beatsin8(7, 0, 255), wave7 = beatsin8(6, 0, 255);
    uint8_t wave4 = beatsin8(6, 0, 255), wave8 = beatsin8(5, 0, 255);

    for (int i=0; i<NUM_LEDS/2; i++) {
        LEFT [i] = ColorFromPalette( randomPalette1, i+wave5+wave6+wave7+wave8, 255, currentBlending); 
        RIGHT[i] = ColorFromPalette( targetPalette , i+wave1+wave2+wave3+wave4, 255, currentBlending); 
        yield();
    }
    FastLED.show();
} // beatwave()

void inoise8_mover() {

    uint8_t locn = inoise8(xscale, dist1+yscale) % 255;          // Get a new pixel location from moving noise.
    uint8_t pixlen = map(locn,0,255,0,NUM_LEDS/2);                // Map that to the length of the strand.
    LEFT[pixlen] = ColorFromPalette(currentPalette, pixlen, 255, LINEARBLEND);   // Use that value for both the location as well as the palette index colour for the pixel.

    locn = inoise8(xscale, dist2+yscale) % 255;          // Get a new pixel location from moving noise.
    pixlen = map(locn,0,255,0,NUM_LEDS/2);                // Map that to the length of the strand.
    RIGHT[pixlen] = ColorFromPalette(randomPalette2, pixlen, 255, LINEARBLEND);   // Use that value for both the location as well as the palette index colour for the pixel.

    dist1 += beatsin8(10,1,4);                                                // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.                                             
    dist2 += beatsin8(13,1,4);                                                // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.                                             
    FastLED.show();
    fadeToBlackBy(leds, NUM_LEDS, 10);     
} // inoise8_mover()
