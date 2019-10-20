/*
 *      ESP 32 Dash Firmware (Initial Test)
 *      Clinton Flowers, 2019
 */

#include <FastLED.h>
#define NUMPIXELS 20
#define ALPHANUMERIC_SEGMENTS 5
#define ALPHANUMERIC_LATCH_PIN 26
#define ALPHANUMERIC_CLOCK_PIN 27
#define ALPHANUMERIC_DATA_PIN 14
#define ALPHANUMERIC_BLANK_PIN 13
#define NEOPIXEL_PIN 17
#define TWELVE_VOLT_ADC_PIN 35
const char characterList[] =    {' ',  '$',  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z'};
const byte characterBottoms[] = {0x00, 0x65, 0xe2, 0x20, 0xc1, 0x61, 0x21, 0x61, 0xe1, 0x20, 0xe1, 0x21, 0xa1, 0x65, 0xc0, 0x64, 0xc1, 0x81, 0xe0, 0xa1, 0x44, 0xe0, 0x89, 0xc0, 0xa0, 0xa8, 0xe0, 0x81, 0xe8, 0x89, 0x60, 0x04, 0xe0, 0x82, 0xaa, 0x0a, 0x05, 0x42};
const byte characterTops[] =    {0x00, 0x6a, 0x56, 0x14, 0x0e, 0x0e, 0x4c, 0x4a, 0x4a, 0x06, 0x4e, 0x4e, 0x4e, 0x2e, 0x42, 0x26, 0x42, 0x42, 0x4a, 0x4c, 0x22, 0x04, 0x50, 0x40, 0x55, 0x45, 0x46, 0x4e, 0x46, 0x4e, 0x0b, 0x22, 0x44, 0x50, 0x44, 0x11, 0x4c, 0x12};

int x = 0;

struct Pixel {
  uint8_t n;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

Pixel displayPixels [NUMPIXELS];
CRGB leds[NUMPIXELS];
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
// the setup routine runs once when you press reset:
void setup() {
  pinMode(ALPHANUMERIC_BLANK_PIN, OUTPUT);
  digitalWrite(ALPHANUMERIC_BLANK_PIN, HIGH);
  pinMode(ALPHANUMERIC_DATA_PIN, OUTPUT);
  pinMode(ALPHANUMERIC_CLOCK_PIN, OUTPUT);
  pinMode(ALPHANUMERIC_LATCH_PIN, OUTPUT);

  pinMode(TWELVE_VOLT_ADC_PIN, INPUT);
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, GRB>(leds, NUMPIXELS);
  pinMode(NEOPIXEL_PIN, OUTPUT);
  showText("");
  digitalWrite(ALPHANUMERIC_BLANK_PIN, LOW);
//
//  pixels.setBrightness(0);
//  pixels.begin();
//  pixels.setBrightness(0);
//  pixels.show(); // Initialize all pixels to 'off'

  for(int i = 0; i < NUMPIXELS; i++){
    displayPixels[i].n = 0;
    displayPixels[i].r = 0;
    displayPixels[i].g = 0;
    displayPixels[i].b = 0;
  }

  Serial.begin(115200);
 
}

// the loop routine runs over and over again forever:
void loop() {
  x++;
  Serial.println(String(x) + ", " + String(analogRead(TWELVE_VOLT_ADC_PIN)));
  for(int i = 0; i < NUMPIXELS; i++){
    for(int j = 0; j < NUMPIXELS; j++){
      if(i == j){
        leds[j] = CRGB::Red;
        if(j-1 >= 0){
          leds[j-1] = CRGB::Green;
        }
        if(j-2 >= 0){
          leds[j-2] = CRGB::Blue;
        }
      }else{
        leds[j] = CRGB::Black;
      }
    }
    FastLED.show();
//    showText(String(x) + " " + String(i));
    delay(100);
    switch(x%5){
      case 0:
        showText("    8.");
        break;
      case 1:
        showText("   8. ");
        break;
      case 2:
        showText("  8.  ");
        break;
      case 3:
        showText(" 8.   ");
        break;
      case 4:
        showText("8.    ");
        break;
      default:
        break;
    }
  }

//  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);               // wait for a second
}

 // A method to simplify showing text on the 3 alphanumerics
void showText (String whatToWrite){
  int decimalCount = 0;
  String whatToActuallyWrite; // Used after decimal points have been accounted for and merged
  String decimals;
  // Left-pad the input
  if(whatToWrite.length() < ALPHANUMERIC_SEGMENTS){
    for(int i = whatToWrite.length(); i < ALPHANUMERIC_SEGMENTS; i++){
      whatToWrite = " " + whatToWrite;
    }
  }else if(whatToWrite.length() > ALPHANUMERIC_SEGMENTS*2){
    // The string definitely needs to be scrolled out since there can only be 1 decimal per alphanumeric character
    scrollText(whatToWrite);
    return;
  }
  // Deal with decimals (e.g., printing "12.4" can fit on 3 alphanumeric displays, so merge the decimal)
  for(int i = 0; i < whatToWrite.length(); i++){
    // Append the character to the portion displayed, dealing with decimals
    decimals += ' ';
    if(whatToWrite[i] == '.'){
      // Any time this loop finds a decimal, go to the last 'valid' char and append one
      // (This implementation discards any leading decimals)
      decimals[whatToActuallyWrite.length() - 1] = '.'; 
    }else{
      whatToActuallyWrite += whatToWrite[i];
    }
  }
  if(whatToActuallyWrite.length() <= ALPHANUMERIC_SEGMENTS){
    show(whatToActuallyWrite, decimals);
    return;
  }else{
    scrollText(whatToWrite);
    return;
  }
} // End showText()

// Default method for scrolled text
void scrollText(String input){
  scrollText(input, 150, ALPHANUMERIC_SEGMENTS);
} // End scrollText(String)


// Scroll the text from left to right across the displays.
// This is done by calling show() on one set of characters,
// shifting left by one character, and then repeating. 
void scrollText(String whatToScroll, int delayTime, int addedPadding){
  int decimalCount = 0;
  String whatToActuallyWrite;
  String decimals;
  // Add padding to the scrolled text based on input request
  for(int i = 0; i < addedPadding; i++){
    whatToScroll = "  " + whatToScroll + "  ";
  }
  for(int i = 0; i < whatToScroll.length(); i++){
    // Append the character to the portion displayed, dealing with decimals
    decimals += ' ';
    if(whatToScroll[i] == '.'){
      // Any time this loop finds a decimal, go to the last valid char and "place" one
      // (Note: This implementation discards any leading or standalone decimals)
      decimals[whatToActuallyWrite.length() - 1] = '.'; 
    }else{
      whatToActuallyWrite += whatToScroll[i];
    }

    // Send the latest group of 3 alphanumerics to be shown, and remove the earliest character.
    if(whatToActuallyWrite.length() >= ALPHANUMERIC_SEGMENTS){
      show(whatToActuallyWrite, decimals);
      decimals.remove(0,1);
      whatToActuallyWrite.remove(0,1);  
      delay(delayTime); // This delay will cause the main loop to stop operating (and thus updating LED's) if text that's too long is being shifted out. Interrupts might be a better choice.
    }
  }
} // End scrollText(String, int, int)


// Time-based method to show text
void show(String whatToActuallyWrite, String decimals){
  // Example Inputs: "123", "1.234", "1.23", "Hello World"
  if(whatToActuallyWrite.length() > ALPHANUMERIC_SEGMENTS){
    // The string still needs to be scrolled out.
    return;
  }
  
  // For this version we shift everything to uppercase (until lowercase values are added to the arrays above)
  whatToActuallyWrite.toUpperCase(); // "As of 1.0, toUpperCase() modifies the string in place rather than returning a new one."
  
  // At this point, the string can (probably) be displayed on n characters, so do that. 
  byte topDatas[ALPHANUMERIC_SEGMENTS];
  byte bottomDatas[ALPHANUMERIC_SEGMENTS];
  // Initialize
  for(int i = 0; i < ALPHANUMERIC_SEGMENTS; i++){
    topDatas[i] = 0;
    bottomDatas[i] = 0;
  }

  // Load the input sequence into the given characters
  // i is the loop for each of the (nominally 3) alphanumeric characters
  for(int i = 0; i < ALPHANUMERIC_SEGMENTS; i++){
    for(int j = 0; j < sizeof(characterList); j++){
      if(whatToActuallyWrite[i] == characterList[j]){
        // The first character is the same as the current character. 
        // Load that into the top and bottom bytes.
        bottomDatas[i] = characterBottoms[j];
        topDatas[i] = characterTops[j];
      }
    }
    if(decimals[i] == '.'){
      bottomDatas[i] |= 0x10;  // Append the decimal to this character
    }
  }

  // Ground latchPin and hold low for as long as you are transmitting
  digitalWrite(ALPHANUMERIC_LATCH_PIN, LOW);
  // Shift the bytes out
    for(int i =  ALPHANUMERIC_SEGMENTS - 1; i >= 0; i--){
      shiftOut(ALPHANUMERIC_DATA_PIN, ALPHANUMERIC_CLOCK_PIN, topDatas[i]);
      shiftOut(ALPHANUMERIC_DATA_PIN, ALPHANUMERIC_CLOCK_PIN, bottomDatas[i]);
    }
  // Return the latch pin high to signal chip that it no longer needs to listen for information
  digitalWrite(ALPHANUMERIC_LATCH_PIN, HIGH);
} // End show()


// The method that actually shifts out the raw data to the shift registers/LED drivers
void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, on the rising edge of the clock, clock idles low
  // Internal function setup
  int i=0;
  bool pinState = LOW;
  // Clear everything out just in case to prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);
  //For each bit in the byte myDataOut notice that we are counting down in our for loop
  //This means that %00000001 or "1" will go through such that it will be pin Q0 that lights. 
  for (i = 7; i >= 0; i--)  {
    digitalWrite(myClockPin, 0);
    // If the value passed to myDataOut and a bitmask result true then... so if we are at i=6 and our value is %11010100 it would the code compares it to %01000000 and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= HIGH;
    }
    else {  
      pinState= LOW;
    }
    digitalWrite(myDataPin, pinState);  //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myClockPin, 1);        //register shifts bits on upstroke of clock pin  
    digitalWrite(myDataPin, 0);         //zero the data pin after shift to prevent bleed through
  }

  // Stop shifting
  digitalWrite(myClockPin, 0);
} // End shiftOut()
