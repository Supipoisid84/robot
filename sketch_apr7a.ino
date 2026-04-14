const int dirA   = 12;
const int brakeA = 9;
const int pwmA   = 3;

const int dirB   = 13;
const int brakeB = 8;
const int pwmB   = 11;

const int chSteerPin    = 2;   // CH1
const int chThrottlePin = 4;   // CH3

void setup() {
  pinMode(dirA, OUTPUT);
  pinMode(brakeA, OUTPUT);
  pinMode(pwmA, OUTPUT);

  pinMode(dirB, OUTPUT);
  pinMode(brakeB, OUTPUT);
  pinMode(pwmB, OUTPUT);

  pinMode(chSteerPin, INPUT);
  pinMode(chThrottlePin, INPUT);

  Serial.begin(115200);
  stopMotors();
}

void loop() {
  unsigned long steerPulse = pulseIn(chSteerPin, HIGH, 30000);
  unsigned long throttlePulse = pulseIn(chThrottlePin, HIGH, 30000);

  Serial.print("CH1=");
  Serial.print(steerPulse);
  Serial.print("  CH3=");
  Serial.println(throttlePulse);

  if (steerPulse < 900 || steerPulse > 2100 || throttlePulse < 900 || throttlePulse > 2100) {
    stopMotors();
    Serial.println("Signaal puudub");
    delay(20);
    return;
  }

  int steer = map((int)steerPulse, 1000, 2000, -255, 255);
  int throttle = map((int)throttlePulse, 1000, 2000, -255, 255);

  if (abs(steer) < 20) steer = 0;
  if (abs(throttle) < 20) throttle = 0;

  int leftMotor  = throttle + steer;
  int rightMotor = throttle - steer;

  leftMotor  = constrain(leftMotor, -255, 255);
  rightMotor = constrain(rightMotor, -255, 255);

  setMotorA(leftMotor);
  setMotorB(rightMotor);

  delay(20);
}

void setMotorA(int speedValue) {
  if (speedValue == 0) {
    digitalWrite(brakeA, HIGH);
    analogWrite(pwmA, 0);
    return;
  }

  digitalWrite(brakeA, LOW);

  if (speedValue > 0) {
    digitalWrite(dirA, HIGH);
    analogWrite(pwmA, speedValue);
  } else {
    digitalWrite(dirA, LOW);
    analogWrite(pwmA, -speedValue);
  }
}

void setMotorB(int speedValue) {
  if (speedValue == 0) {
    digitalWrite(brakeB, HIGH);
    analogWrite(pwmB, 0);
    return;
  }

  digitalWrite(brakeB, LOW);

  if (speedValue > 0) {
    digitalWrite(dirB, HIGH);
    analogWrite(pwmB, speedValue);
  } else {
    digitalWrite(dirB, LOW);
    analogWrite(pwmB, -speedValue);
  }
}

void stopMotors() {
  digitalWrite(brakeA, HIGH);
  digitalWrite(brakeB, HIGH);
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);
}
