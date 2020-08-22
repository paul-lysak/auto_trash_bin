#include <Servo.h>

//#define DEBUG_LOGS

Servo lidServo; 

const int DISTANCE_SENSOR_PIN = 0;
const int SERVO_PIN = 9;

const int CLOSE_ANGLE = 0;
const int OPEN_ANGLE  = 70;

const int OPEN_STEP = 2;
const int CLOSE_STEP = 1;

const int HISTORY_LENGTH = 50;
const int DISTANCE_THRESHOLD = 50;
const int CYCLE_DURATION = 10;
const int SECONDS_BEFORE_CLOSE = 10;
const int CYCLES_BEFORE_CLOSE= SECONDS_BEFORE_CLOSE * 1000 / CYCLE_DURATION;


int distHistory[HISTORY_LENGTH];
byte historyItem = 0;
bool historyInitialized = false;

byte closeCyclesCountdown = 0;
int lidTargetAngle = CLOSE_ANGLE;
int lidWrittenAngle = CLOSE_ANGLE;

void setup() {
#ifdef DEBUG_LOGS  
  Serial.begin(9600);
#endif  
  
  lidServo.attach(SERVO_PIN);

  lidServo.write(CLOSE_ANGLE);
  lidWrittenAngle = CLOSE_ANGLE;
#ifdef DEBUG_LOGS
  Serial.print("Init the lid at ");
  Serial.println(lidWrittenAngle);
#endif

  for(byte i=0; i<HISTORY_LENGTH; i++) {
    distHistory[i]=0;
  }
}

void rememberHistory(int dist) {
  distHistory[historyItem] = dist;
  historyItem++;
  if(!historyInitialized && historyItem == HISTORY_LENGTH) {
    historyInitialized = true;
#ifdef DEBUG_LOGS
  Serial.println("History initialized");
#endif    
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

void lidTriggerOpen() {
  if(lidTargetAngle != OPEN_ANGLE) {
#ifdef DEBUG_LOGS  
   Serial.println("open the lid");
#endif 
     lidTargetAngle = OPEN_ANGLE;
  }
}

void lidTriggerClose() {
  if(lidTargetAngle != CLOSE_ANGLE) {
#ifdef DEBUG_LOGS  
    Serial.println("close the lid");
#endif
    lidTargetAngle = CLOSE_ANGLE;
  }
}

void moveLid() {
  int nextAngle;
  if(lidTargetAngle > lidWrittenAngle) {
    nextAngle = lidWrittenAngle + OPEN_STEP;
    if(nextAngle > lidTargetAngle)
        nextAngle = lidTargetAngle;
#ifdef DEBUG_LOGS
    Serial.print("Opening to ");
    Serial.println(nextAngle);
#endif    
    lidServo.write(nextAngle);
    lidWrittenAngle = nextAngle;
  } else if(lidTargetAngle < lidWrittenAngle) {
    nextAngle = lidWrittenAngle - CLOSE_STEP;
    if(nextAngle < lidTargetAngle)
       nextAngle = lidTargetAngle;
#ifdef DEBUG_LOGS        
    Serial.print("Closing to ");
    Serial.println(nextAngle);
#endif    
    lidServo.write(nextAngle);
    lidWrittenAngle = nextAngle;
  }
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
    lidTriggerOpen();
  }

  if(closeCyclesCountdown > 0) {
#ifdef DEBUG_LOGS    
    Serial.print("countdown=");
    Serial.println(closeCyclesCountdown);
#endif    
    closeCyclesCountdown--;
  }
  if(closeCyclesCountdown == 0) {    
    lidTriggerClose();
  }

  moveLid();

  delay(CYCLE_DURATION);  
}
