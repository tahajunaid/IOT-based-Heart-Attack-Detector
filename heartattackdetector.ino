#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.  

const int PulseWire = A0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 1
const int LED13 = 1;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 520;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value.
                               
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"


// 'heart', 32x32px
const unsigned char heart [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x0f, 0xf0, 0x3f, 0xf8, 0x3f, 0xfc,
  0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe,
  0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc,
  0x1f, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0, 0x07, 0xff, 0xff, 0xe0,
  0x03, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0x80, 0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xff, 0x00,
  0x00, 0x3f, 0xfe, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x07, 0xf0, 0x00,
  0x00, 0x03, 0xc0, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int Signal;
bool NoSignal = false;
int cycle = 0;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Configure the PulseSensor object, by assigning our variables to it.
  pulseSensor.analogInput(PulseWire);  
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);  

  // Double-check the "pulseSensor" object was created and "began" seeing a signal.
   if (!pulseSensor.begin()) {
    Serial.println("We failed to start the pulseSensor Object !");
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
 
  display.display();
  delay(3000);
}

void loop() {
  int bpm = pulseSensor.getBeatsPerMinute();
  if (pulseSensor.sawStartOfBeat()) {
    cycle++;
    if(cycle > 5) {
      NoSignal = false;
      drawBPM(bpm);
      cycle = 5;
    }
  } else {
    if (!NoSignal) {
      cycle = 0;
      NoSignal = true;
      drawBPM(bpm);     //i have added this
      //drawNoInput();
    }
  }
  delay(20);
}


void drawBPM(int bpm) {
  display.clearDisplay();

  display.drawBitmap(10, 0, heart, 32, 32, 1);
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(32,0);
  display.print(' ');
  //bpm=bpm-120;
  display.println(bpm);

  display.display();
  delay(1000);
}

void drawNoInput(void) {
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("NO SIGNAL"));
  display.display();
}

