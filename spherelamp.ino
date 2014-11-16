#include <EEPROM.h>
// position 0-220 stored in position 0 
// red 0-255 stored in position 1
// green 0-255 stored in position 2
// blue 0-255 stored in position 3
// brightness 0-255 stored in position 4

// NOTE: 
// If setting the color stops working again,try limit the strings further, perhaps by using constants, or by sending colors /10 (0-25) and then multiplying them here (0-250).
// The issue seems very specific to strings, e.g., if I simply free up memory by removing the motor, it does not solve the problem. 

#include <SoftwareSerial.h>  
#include <SmartThings.h>
#define PIN_THING_RX    0 
#define PIN_THING_TX    1 
SmartThingsCallout_t messageCallout;    // call out function forward decalaration
SmartThings smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);  // constructor
 
#include <Stepper.h>
const int stepsPerRevolution = 200;  
Stepper myStepper(stepsPerRevolution, 12,13);     
 
// give the motor control pins names:
const uint8_t pwmA = 3;
const uint8_t pwmB = 11;
const uint8_t brakeA = 9;
const uint8_t brakeB = 8;
const uint8_t dirA = 12;
const uint8_t dirB = 13;

uint16_t currentPosition = 0;   // 0 - 2200; set by position from smartthings; 0 is closed/furthest from motor 
uint8_t brightness;     
uint8_t red;
uint8_t green;
uint8_t blue;
boolean isOn = true;
uint8_t mode; //0 both, 1 top, 2 bottom

#include <Adafruit_NeoPixel.h>
#define PIN 7
Adafruit_NeoPixel strip = Adafruit_NeoPixel(114, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // read values out of memory
  currentPosition = EEPROM.read(0)*10;
  red = EEPROM.read(1);
  green = EEPROM.read(2);
  blue = EEPROM.read(3);
  brightness = EEPROM.read(4);
  
  mode=0;
  
  // set the PWM and brake pins so that the direction pins  // can be used to control the motor:
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(brakeA, OUTPUT);
  pinMode(brakeB, OUTPUT);
   
  // turn on POWER (turn to low to release the motor and avoid heating)
  digitalWrite(pwmA, HIGH);
  digitalWrite(pwmB, HIGH);
   
  // turn off BRAKES
  digitalWrite(brakeA, LOW);
  digitalWrite(brakeB, LOW);
     
  // 150 is max speed unloaded
  // at 150, there is not enough torque to open the lamp, lowering speed overcomes this.
  myStepper.setSpeed(100); 
  
  // turn off POWER (to release the motor and avoid heating)
  digitalWrite(pwmA, LOW);
  digitalWrite(pwmB, LOW);
  
  smartthing.send("Arduino controller online");
  
  strip.begin();
  strip.setBrightness(brightness);
  strip.show(); // Initialize all pixels to 'off'
  
  updateLamp();
}

void loop() {
  // run smartthing logic
  smartthing.run();
}
 
void messageCallout(String message) {
  String value;
  
  if (message.startsWith("sp:")) {
    value = message.substring(3);
    setPosition(value);
  } else if (message.startsWith("sc:")) { 
    value = message.substring(3);
    uint8_t commaIndex = value.indexOf(',');
    uint8_t secondCommaIndex = value.indexOf(',', commaIndex+1);
  
    red = value.substring(0, commaIndex).toInt();
    green = value.substring(commaIndex+1, secondCommaIndex).toInt();
    blue = value.substring(secondCommaIndex+1).toInt();

    // store values
    EEPROM.write(1, red);
    EEPROM.write(2, green);
    EEPROM.write(3, blue);
 
    updateLamp();
  } else if (message.startsWith("sl:")) {
    value = message.substring(3);
    setLevel(value);
  } else if (message.startsWith("o:")){
    value = message.substring(2);
    if (value.toInt() == 0) {
      smartthing.send("off");     
      isOn = false;
    } else {
      smartthing.send("on");    
      isOn = true;
    }
    updateLamp();
  } else if (message.startsWith("a: r")){
    animate(random(0,5));
  } else if (message.startsWith("a:")){
    value = message.substring(2);
    animate(value.toInt()-1);
  } else if (message.startsWith("m:")){
    value = message.substring(3);
    mode = value.toInt();  
    updateLamp();
  } 
}

void setLevel (String level) {
  // level will be 0-99  
  brightness = level.toInt()/99.0*250; 
  strip.setBrightness(brightness);
  EEPROM.write(4, brightness);
  updateLamp();
}

void setPosition (String targetPosition) {
  uint16_t targetMapped = map(targetPosition.toInt(), 0, 99, 0, 220);
  targetMapped = targetMapped*10; // since we save value 0-220 we want an increment of 10
  uint16_t deltaPosition = targetMapped - currentPosition;
  
  // TODO: add a saftey to be sure current position will be 0-2200;
  currentPosition += deltaPosition;
  
  EEPROM.write(0, currentPosition/10);
  
  digitalWrite(pwmA, HIGH);
  digitalWrite(pwmB, HIGH);
  delay(250);
  myStepper.step(deltaPosition); 
  
  digitalWrite(pwmA, LOW);
  digitalWrite(pwmB, LOW);
}

void updateLamp(){
  if (isOn){
    if (mode == 0){
      colorWipe(red, green, blue, 10);
    } else if (mode == 1){
      lightTop(red, green, blue, 10);
    } else if (mode == 2){
      lightBottom(red, green, blue, 10);
    }
    //smartthing.shieldSetLED(red, green, blue);
  } else {
    colorWipe(0, 0, 0, 10);
  }
}

// Fill the dots one after the other with a color
void setAllColor(uint8_t r, uint8_t g, uint8_t b) {
  for(uint8_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  delay(150);
  for(uint8_t i=0; i<45; i++) {
      strip.setPixelColor(i/1.875, r, g, b);
      strip.setPixelColor(24+i, r, g, b);
      strip.setPixelColor(114-i, r, g, b);
      strip.show();
      delay(wait);
  }
}

void animate(uint8_t pick) {
  setAllColor(0,0,0);  
  
  if (pick == 0){
    wave(red, green, blue);
    delay(250);
    wave(0, 0, 0);
  } else if (pick == 1) {
    lightTop(red, green, blue, 0);
    delay(1000);
    lightTop(0,0,0, 0);
    lightBottom(red, green, blue, 0);
    delay(1000);
    lightBottom(0,0,0, 0);
    lightTop(red, green, blue, 0);
    delay(1000);
    lightTop(0,0,0, 0);
    lightBottom(red, green, blue, 0);
    delay(1000);
    lightBottom(0,0,0, 0);
    lightTop(red, green, blue, 0);
    delay(1000);
    lightTop(0,0,0, 0);
    lightBottom(red, green, blue, 0);
    delay(1000);
    lightBottom(0,0,0, 0);
    lightTop(red, green, blue, 0);
    delay(1000);
    lightTop(0,0,0, 0);
    lightBottom(red, green, blue, 0);
    delay(1000);
    lightBottom(0,0,0, 0);
  } else if (pick == 2) {
    cylon(5, 10, 0);
  } else if (pick == 3) {
    LEDFlash(40, 50);
  } else if (pick == 4) {
    cylon(5, 10, 1);
  } 
  delay(1000);
  updateLamp();
}

void cylon (uint8_t number, uint8_t wait, boolean showTwo) {
  for(uint8_t n=0; n<number; n++) {
    for(uint8_t i=0; i<45; i++) {
        uint8_t j = i + 22;
        if (j > 44) j = j - 45;
        setThreePixels(i/1.875, red, green, blue);
        setThreePixels(24+i, red, green, blue);
        setThreePixels(114-i, red, green, blue);  
        if (showTwo){   
          setThreePixels(j/1.875, red, green, blue);
          setThreePixels(24+j, red, green, blue);
          setThreePixels(114-j, red, green, blue);   
        }
        strip.show();
        delay(wait);
        setThreePixels(i/1.875, 0, 0, 0);
        setThreePixels(24+i, 0, 0, 0);
        setThreePixels(114-i, 0, 0, 0);  
        if (showTwo){      
          setThreePixels(j/1.875, 0, 0, 0);
          setThreePixels(24+j, 0, 0, 0);
          setThreePixels(114-j, 0, 0, 0); 
        }     
        strip.show();
    }
  }
}

void wave(int r,int g,int b){
  for(uint8_t i=0; i<99; i++) { 
    setThreePixels(i, r, g, b);
    strip.show();
    delay(10);
  }
  for(uint8_t i=0; i<45; i++){
    setThreePixels(strip.numPixels()-i, r, g, b);
    strip.show();
    delay(10);
  }
}

void lightTop(int r,int g,int b, uint8_t wait){   
  for(uint8_t i=0; i<45; i++) { 
    strip.setPixelColor(i/1.875, r, g, b);
    strip.setPixelColor(24+i, r, g, b);  
    strip.setPixelColor(114-i, 0, 0, 0); 
    strip.show();
    delay(wait);  
  }
}

void lightBottom(int r,int g,int b, uint8_t wait){    
  for(uint8_t i=0; i<45; i++) {
    strip.setPixelColor(i/1.875, 0, 0, 0);
    strip.setPixelColor(24+i, 0, 0, 0);  
    strip.setPixelColor(114-i, r, g, b); 
    strip.show();  
    delay(wait);  
  }
}

void setThreePixels(int LED,int r,int g,int b) {
   strip.setPixelColor(LED, r, g, b);
   if (LED>0) strip.setPixelColor(LED-1, r, g, b);
   if (LED<strip.numPixels()-1)strip.setPixelColor(LED+1, r, g, b);
}

void LEDFlash(uint8_t number, uint8_t wait) {
  for(uint8_t i=0; i<number; i++) {
   uint16_t LED = random(0,strip.numPixels());
   setThreePixels(LED, red, green, blue);
   strip.show();
   delay(wait);
   setThreePixels(LED, 0, 0, 0);
   strip.show();
  }
}
