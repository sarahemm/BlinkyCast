// EL Channels
// A - Top front
// B - Left turn signal
// C - Bottom rear
// D - Bottom front
// E - Top rear
// F - unused
// G - unused
// H - Right turn signal

#define EL_FRONT_TOP    2
#define EL_LEFT_TURN    3
#define EL_REAR_BOTTOM  4
#define EL_FRONT_BOTTOM 5
#define EL_REAR_TOP     6
#define EL_RIGHT_TURN   9

#include <Wire.h>
#include <L3G.h>

L3G gyro;

int offset_x, offset_y, offset_z;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if(!gyro.init()) {
    Serial.println("Failed to initialize gyro.");
    while (1);
  }

  gyro.enableDefault();
  delay(100); // let the gyro initialize, otherwise we get junk back
  
  // cancel out any offset in the gyro's measurements
  gyro.read();
  offset_y = gyro.g.y;

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
}

// 0 is latest, 1 is one cycle ago, 2 is two cycles ago
byte last_motions[3];
// whether we are overall moving
byte moving;

int last_y;
long time_on;
void loop() {
  delay(50);
  if(time_on != 0 && millis() - time_on > 350) {
    digitalWrite(EL_LEFT_TURN, LOW);
    digitalWrite(EL_RIGHT_TURN, LOW);
    time_on = 0;
  } else if(time_on != 0 && millis() - time_on > 250) {
    digitalWrite(EL_FRONT_BOTTOM, LOW);
    digitalWrite(EL_REAR_BOTTOM, LOW);
    digitalWrite(EL_LEFT_TURN, HIGH);
    digitalWrite(EL_RIGHT_TURN, HIGH);
  } else if(time_on != 0 && millis() - time_on > 150) {
    digitalWrite(EL_FRONT_BOTTOM, HIGH);
    digitalWrite(EL_REAR_BOTTOM, HIGH);
    digitalWrite(EL_FRONT_TOP, LOW);
    digitalWrite(EL_REAR_TOP, LOW);
  } else if(time_on != 0 && millis() - time_on > 0) {
    digitalWrite(EL_FRONT_TOP, HIGH);
    digitalWrite(EL_REAR_TOP, HIGH);
  }
  
  gyro.read();
  
  int y_reading = gyro.g.y - offset_y;
  int delta_y = y_reading - last_y;

  last_motions[2] = last_motions[1];
  last_motions[1] = last_motions[0];
  
  if(abs(delta_y) < 1000) {
    // not in motion right now
    last_motions[0] = 0;
  } else {
    // in motion right now
    last_motions[0] = 1;
  }
  
  if(last_motions[0] == last_motions[1]) {
    if(last_motions[0] && !moving) {
      moving = 1;
      //Serial.println("Foot UP");
    } else if(!last_motions[0] && moving) {
      moving = 0;
      //Serial.println("Foot DOWN");
      // only start a new light cycle if the last one is done
      if(time_on == 0) time_on = millis();
    }
  }
  
  //Serial.print(" Y: ");
  //Serial.print(y_reading);
  //Serial.print("\t");
  //Serial.println(delta_y);

  last_y = y_reading;
}
