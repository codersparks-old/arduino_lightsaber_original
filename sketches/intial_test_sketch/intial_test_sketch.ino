
#include <EnableInterrupt.h>
/*
 * WT588d Sound slots
 * 
 * 0 - idle
 * 1 - On
 * 2 - Off
 * 3 to 10 - Swing
 * 11 to 13 - Strike
 * 14 to 17 - Hit
 */

#define WT588D_DATA 11
#define WT588D_RESET 3
#define WT588D_BUSY 2
#define INTERRUPT_POWER_PIN 7
#define BUSY_VAL 0

#define OFF_SOUND 2
#define ON_SOUND 1

#define DEBOUNCE_TIME 200

bool playing = false;

volatile bool power = false;
volatile unsigned long last_interrupt_time;

void setup() {
  pinMode(WT588D_DATA, OUTPUT);
  pinMode(WT588D_RESET, OUTPUT);
  pinMode(INTERRUPT_POWER_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  while (! Serial);

  last_interrupt_time = 0;
  power = false;
  registerPowerOffInterrupt();
  
  Serial.println("Setup Finished");
}

void loop() {

  if(power) {
    waitForNotBusy();  
    playSaberSound(0);
  }

  //Serial.print("Power: ");
  //Serial.println(power);
  
}

void powerOnInterrupt() {
  // Do power off

  unsigned long interrupt_time = millis();

  if(interrupt_time - last_interrupt_time > DEBOUNCE_TIME) {

  Serial.println("Power On Interrupt fired");
  
    playSaberSound(OFF_SOUND);
    power = false;
    registerPowerOffInterrupt();
    waitForNotBusy();  

    last_interrupt_time = interrupt_time;
  }
  
}

void powerOffInterrupt() {

  unsigned long interrupt_time = millis();

  if(interrupt_time - last_interrupt_time > DEBOUNCE_TIME) {

    Serial.println("Power Off Interrupt fired");
    
    // Do power on
    playSaberSound(ON_SOUND);
    power = true;
    registerPowerOnInterrupt();
    waitForNotBusy();  
    
    last_interrupt_time = interrupt_time;
  }
}

void registerPowerOffInterrupt() {
  enableInterrupt(INTERRUPT_POWER_PIN, powerOffInterrupt, CHANGE);
}

void registerPowerOnInterrupt() {
  enableInterrupt(INTERRUPT_POWER_PIN, powerOnInterrupt, CHANGE);
}

/*
 * Function takes a byte which refers to the address of the sound to be played.
 */
void playSaberSound(byte b) {
  digitalWrite(WT588D_DATA, 0); // Pull DATA low to wake up chip
  // Stop any playing sounds
  resetSoundBoard();
  delay(5);
  Serial.print("Playing sound: ");
  Serial.println(b);

  for(int i = 0; i<8; i++) {

    if(bitRead(b, i)) { // Read each bit of the sound to be played

      digitalWrite(WT588D_DATA, 1);
      delayMicroseconds(400);
      digitalWrite(WT588D_DATA, 0); // 2:1 high/low indicates high
      delayMicroseconds(200);
    } else {
      digitalWrite(WT588D_DATA, 1);
      delayMicroseconds(200);
      digitalWrite(WT588D_DATA, 0);
      delayMicroseconds(400);
    }

    digitalWrite(WT588D_DATA, 1);
  }

  delayMicroseconds(100);
}

void resetSoundBoard() {
   digitalWrite(WT588D_RESET, LOW);
   delayMicroseconds(100);
   digitalWrite(WT588D_RESET, HIGH);
   delayMicroseconds(1000);
   
}

void waitForNotBusy() {
  playing = true;
  delay(100); // Trying to stop the check from missing the initial busy signal
  while(playing) {
    
    int val = digitalRead(WT588D_BUSY);

    Serial.print("Busy: ");
    Serial.print(val);

    if(val == BUSY_VAL) {
      playing = true;
    } else {
      playing = false;
    }

    Serial.print(" Playing: ");
    Serial.println(playing);

    //delay(500);
  }
}

