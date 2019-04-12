


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


int LED = PC13;
int leftButton = PA0;
int leftButtonState;
int rightButton = PA1;
int rightButtonState;
int track1 = PA2;
int track2 = PB0;
int gate = PC14;

volatile int track1State;
volatile int track2State;
volatile int gateState;
volatile int raceState;
volatile int TOTtripped;
static unsigned long TOTmax = 30000; // If you cant make it down the track in 30 seconds, something is broke.
volatile unsigned long TOT; // time out timer
volatile unsigned long startTime;
volatile unsigned long track1Time;
volatile unsigned long track2Time;

void setup() {

  Serial.begin(115200); // init the serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Setup");
  
  // set out outputs
  pinMode(LED, OUTPUT); // set the LED pin to an output
  // set our inputs
  pinMode(leftButton,INPUT_PULLUP);
  pinMode(rightButton,INPUT_PULLUP);
  pinMode(track1,INPUT_PULLUP);
  attachInterrupt(track1, track1Event, FALLING);
  pinMode(track2,INPUT_PULLUP);
  attachInterrupt(track2, track2Event, FALLING);
  pinMode(gate,INPUT_PULLUP);
  attachInterrupt(gate, gateDrop, FALLING);
  

}
void gateDrop(){
  Serial.println("gatedrop");
  startTime = millis();
  track1Time = 0;
  track2Time = 0;
  track1State = 0;
  track2State = 0;
  gateState = 0; 
  raceState = 1;
  TOTtripped = 0; 
  
}

void track1Event() {
 //Serial.println("track1Event");
 track1Time = millis()-startTime;
 track1State = 1;
}

void track2Event() {
 //Serial.println("track2Event");
 track2Time = millis()-startTime;
 track2State = 1;
}

void loop() {
  while (track1Time == 0 && track2Time == 0) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Gate Dropped");
    delay(100);
    digitalWrite(LED, !digitalRead(LED));
    Serial.print(digitalRead(track1));Serial.print(","); Serial.print(digitalRead(track2));Serial.print(","); Serial.println(digitalRead(gate));
    //Serial.print(track1Time);Serial.print(","); Serial.print(track2Time);Serial.print(","); Serial.println(startTime);
  }
  while ((!track1Time == 0 || !track2Time == 0) && (track1State != track2State) && !TOTtripped) {
    TOT = millis() - startTime;
    if (track1Time == 0 && TOT <= TOTmax){
      Serial.print("Track 2 Wins!: ");
      Serial.println(track2Time);
      Serial.print(digitalRead(track1));Serial.print(","); Serial.print(digitalRead(track2));Serial.print(","); Serial.println(digitalRead(gate));
      delay(500);
    }
    if (track2Time == 0 && TOT <= TOTmax){
      TOT = millis() - startTime;
      Serial.print("Track 1 Wins!: ");
      Serial.println(track1Time);
      Serial.print(digitalRead(track1));Serial.print(","); Serial.print(digitalRead(track2));Serial.print(","); Serial.println(digitalRead(gate));
      TOT++;
      delay(500);
    }
    while (TOT >= TOTmax){
      if (!TOTtripped){
        Serial.println("Timeout!");
        TOTtripped = 1 ;
      }
    }
  } 
  while (raceState == 1){
    Serial.print("Final Results:    Track1: ");Serial.print(track1Time);Serial.print("    Track2: "); Serial.print(track2Time);Serial.print("    Start Time: "); Serial.println(startTime);
    raceState = 0;
  }
    
}