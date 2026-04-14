// ===== Mootori pinide määramine =====
// Kanal A (vasak ratas tavaliselt)
const int dirA   = 12;   // suuna pin (HIGH/LOW määrab edasi/tagasi)
const int brakeA = 9;    // piduri pin (HIGH = pidur peal)
const int pwmA   = 3;    // kiiruse PWM pin (0–255)

// Kanal B (parem ratas tavaliselt)
const int dirB   = 13;
const int brakeB = 8;
const int pwmB   = 11;

// ===== RC vastuvõtja kanalid =====
const int chSteerPin    = 2;   // CH1 – keeramine (vasak/parem)
const int chThrottlePin = 4;   // CH3 – liikumine (edasi/tagasi)

void setup() {
  // Seadistame kõik mootori pinid väljunditeks
  pinMode(dirA, OUTPUT);
  pinMode(brakeA, OUTPUT);
  pinMode(pwmA, OUTPUT);

  pinMode(dirB, OUTPUT);
  pinMode(brakeB, OUTPUT);
  pinMode(pwmB, OUTPUT);

  // Seadistame RC kanalite pinid sisenditeks
  pinMode(chSteerPin, INPUT);
  pinMode(chThrottlePin, INPUT);

  // Serial monitor (debugimiseks)
  Serial.begin(115200);

  // Alguses peatame mootorid
  stopMotors();
}

void loop() {
  // ===== RC signaali lugemine =====
  // pulseIn mõõdab HIGH impulsi pikkust mikrosekundites (~1000–2000)
  unsigned long steerPulse = pulseIn(chSteerPin, HIGH, 30000);
  unsigned long throttlePulse = pulseIn(chThrottlePin, HIGH, 30000);

  // Kuvame väärtused Serial Monitorisse
  Serial.print("CH1=");
  Serial.print(steerPulse);
  Serial.print("  CH3=");
  Serial.println(throttlePulse);

  // ===== Turvakontroll =====
  // Kui signaal puudub või on vigane, peatame robotid
  if (steerPulse < 900 || steerPulse > 2100 || 
      throttlePulse < 900 || throttlePulse > 2100) {
    stopMotors();
    Serial.println("Signaal puudub");
    delay(20);
    return;
  }

  // ===== Signaali teisendamine =====
  // Muudame RC signaali vahemikust 1000–2000 → -255 kuni 255
  int steer = map((int)steerPulse, 1000, 2000, -255, 255);
  int throttle = map((int)throttlePulse, 1000, 2000, -255, 255);

  // ===== Deadzone (vältimaks värinat keskel) =====
  // Kui kang on peaaegu keskel, loeme selle nulliks
  if (abs(steer) < 20) steer = 0;
  if (abs(throttle) < 20) throttle = 0;

  // ===== Diferentsiaaljuhtimine =====
  // Kombineerime liikumise ja keeramise
  int leftMotor  = throttle + steer;   // vasak ratas
  int rightMotor = throttle - steer;   // parem ratas

  // Piirame väärtused lubatud vahemikku
  leftMotor  = constrain(leftMotor, -255, 255);
  rightMotor = constrain(rightMotor, -255, 255);

  // ===== Mootorite juhtimine =====
  setMotorA(leftMotor);
  setMotorB(rightMotor);

  delay(20); // väike paus (~50 Hz nagu RC signaal)
}

// ===== Mootor A juhtimine =====
void setMotorA(int speedValue) {
  if (speedValue == 0) {
    // Kui kiirus 0 → pidur peale
    digitalWrite(brakeA, HIGH);
    analogWrite(pwmA, 0);
    return;
  }

  // Võtame piduri maha
  digitalWrite(brakeA, LOW);

  if (speedValue > 0) {
    // Edasi liikumine
    digitalWrite(dirA, HIGH);
    analogWrite(pwmA, speedValue);
  } else {
    // Tagasi liikumine
    digitalWrite(dirA, LOW);
    analogWrite(pwmA, -speedValue); // negatiivne → positiivseks
  }
}

// ===== Mootor B juhtimine =====
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

// ===== Mootorite täielik peatamine =====
void stopMotors() {
  digitalWrite(brakeA, HIGH);
  digitalWrite(brakeB, HIGH);
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);
}
