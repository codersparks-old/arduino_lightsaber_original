

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
#define WT588D_BUSY 2
#define BUSY_VAL 0

bool playing = false;

void setup() {
  pinMode(WT588D_DATA, OUTPUT);
  Serial.begin(9600);
  while (! Serial);
  Serial.println("Setup Finished");
}

void loop() {
  // put your main code here, to run repeatedly:

  playSaberSound(0);
  waitForNotBusy();  

  playSaberSound(1);
  waitForNotBusy();  

  playSaberSound(2);
  waitForNotBusy();

  delay(1000);

}

/*
 * Function takes a byte which refers to the address of the sound to be played.
 */
void playSaberSound(byte b) {
  digitalWrite(WT588D_DATA, 0); // Pull DATA low to wake up chip
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

    delay(500);
  }
}

