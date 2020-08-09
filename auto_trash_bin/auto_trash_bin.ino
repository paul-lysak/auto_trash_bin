#include <Servo.h>

#define DEBUG_LOGS

Servo lidServo; 

const int DISTANCE_SENSOR_PIN = 0;
const int SERVO_PIN = 9;

const int CLOSE_ANGLE = 0;
const int OPEN_ANGLE  = 60;

const int HISTORY_LENGTH = 20;
const int DISTANCE_THRESHOLD = 50;
const int CYCLE_DURATION = 200;
const int CYCLES_BEFORE_CLOSE=30;


int distHistory[HISTORY_LENGTH];
byte historyItem = 0;
bool historyInitialized = false;

bool lidOpened = false;

byte closeCyclesCountdown = 0;

void setup() {
#ifdef DEBUG_LOGS  
  Serial.begin(9600);
#endif  
  
  lidServo.attach(SERVO_PIN);

  lidClose();

  for(byte i=0; i<HISTORY_LENGTH; i++) {
    distHistory[i]=0;
  }
}



void rememberHistory(int dist) {
  distHistory[historyItem] = dist;
  historyItem++;
  if(!historyInitialized && historyItem == HISTORY_LENGTH) {
    historyInitialized = true;
  }
  historyItem = historyItem % HISTORY_LENGTH;  
}

int avgDist() {
  int sum = 0;  
  for(byte i=0; i<HISTORY_LENGTH; i++) {
    sum += distHistory[i];
  }
  return sum/HISTORY_LENGTH;
}

void lidOpen() {
#ifdef DEBUG_LOGS  
 Serial.println("open the lid");
#endif
 lidServo.write(OPEN_ANGLE);  
 lidOpened = true;
}

void lidClose() {
#ifdef DEBUG_LOGS  
  Serial.println("close the lid");
#endif
  lidServo.write(CLOSE_ANGLE);
  lidOpened = false;
}


bool distChanges() {
  if(!historyInitialized) {
    return false;
  } else {
    int avg = avgDist();
    int distN = distHistory[historyItem];
    int distN_1 = distHistory[(historyItem - 1 + HISTORY_LENGTH) % HISTORY_LENGTH];
    int distN_2 = distHistory[(historyItem - 2 + HISTORY_LENGTH) % HISTORY_LENGTH];
    bool changed = abs(avg - distN) > DISTANCE_THRESHOLD &&
    abs(avg - distN_1) > DISTANCE_THRESHOLD &&
    abs(avg - distN_2) > DISTANCE_THRESHOLD;
#ifdef DEBUG_LOGS
    if(changed) {
      Serial.print("Changed: avg=");
      Serial.print(avg);
      Serial.print(", distN=");
      Serial.print(distN);
      Serial.print(", distN-1=");
      Serial.print(distN_1);
      Serial.print(", distN-2=");
      Serial.println(distN_2);
    }
#endif    
    return changed;
  }
}
  
void loop() {
  int dist = analogRead(DISTANCE_SENSOR_PIN);
  rememberHistory(dist);

  if(distChanges()) {
#ifdef DEBUG_LOGS    
    Serial.print("Changes ");
    Serial.println(dist);
    Serial.println();
#endif    
    closeCyclesCountdown = CYCLES_BEFORE_CLOSE;
    if(!lidOpened) {
      lidOpen();
    }
  }

  if(closeCyclesCountdown > 0) {
#ifdef DEBUG_LOGS    
    Serial.print("countdown=");
    Serial.println(closeCyclesCountdown);
#endif    
    closeCyclesCountdown--;
  }
  if(closeCyclesCountdown == 0 && lidOpened) {    
    lidClose();
  }
  

  delay(CYCLE_DURATION);  
}
