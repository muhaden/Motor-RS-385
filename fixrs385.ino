#define IN1 13
#define IN2 12
#define ENA 27
#define ENCODER_PIN 14

#define PWM_CHANNEL 0
#define PWM_FREQ 1000
#define PWM_RESOLUTION 8  // 8-bit: 0 - 255

volatile unsigned long pulseCount = 0;
unsigned long lastRPMTime = 0;
float rpm = 0;
const int PULSES_PER_REV = 20;

int currentPWM = 200; // Menyimpan nilai PWM terakhir

void IRAM_ATTR countPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENCODER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), countPulse, RISING);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, PWM_CHANNEL);

  ledcWrite(PWM_CHANNEL, currentPWM);
  lastRPMTime = millis();

  Serial.println("Ketik nilai PWM (0-255) di Serial Monitor:");
}

void loop() {
  unsigned long now = millis();

  if (now - lastRPMTime >= 1000) {
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    rpm = (pulses * 60.0) / PULSES_PER_REV;
    Serial.print("RPM: ");
    Serial.println(rpm);

    lastRPMTime = now;
  }

  // Input Serial dengan perlindungan agar tidak reset ke 0
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Hilangkan spasi dan newline

    if (input.length() > 0 && input.toInt() >= 0 && input.toInt() <= 255) {
      currentPWM = input.toInt();
      ledcWrite(PWM_CHANNEL, currentPWM);
      Serial.print("Set PWM to: ");
      Serial.println(currentPWM);
    } else {
      Serial.println("Input tidak valid. Ketik nilai antara 0 - 255.");
    }
  }
}