#include <DHT.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// Pin Definitions
#define DHTPIN A0
#define DHTTYPE DHT11
#define FLAME_SENSOR 2
#define VOLTAGE_SENSOR A1
#define RELAY_PIN 8
#define BUZZER_PIN 9

// Thresholds for Safety [cite: 170, 171, 181]
const float TEMP_THRESHOLD = 45.0; // Max safe temp in Celsius
const float VOLT_MIN = 10.5;      // Deep discharge limit
const float VOLT_MAX = 12.6;      // Overcharge limit

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(RELAY_PIN, HIGH); // Connect battery initially
  lcd.begin(16, 2);
  lcd.print("BMS Initializing");
  delay(2000);
}

void loop() {
  // 1. Data Acquisition [cite: 125, 126]
  float temperature = dht.readTemperature();
  int flameValue = digitalRead(FLAME_SENSOR);
  int sensorValue = analogRead(VOLTAGE_SENSOR);
  float voltage = sensorValue * (5.0 / 1023.0) * 3; // Adjusted for voltage divider

  // 2. Local Display [cite: 147, 148]
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V: "); lcd.print(voltage); lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("T: "); lcd.print(temperature); lcd.print("C");

  // 3. Safety & Control Logic [cite: 61, 62, 135]
  bool isUnsafe = false;

  if (temperature > TEMP_THRESHOLD || voltage > VOLT_MAX || voltage < VOLT_MIN || flameValue == LOW) {
    isUnsafe = true;
  }

  if (isUnsafe) {
    digitalWrite(RELAY_PIN, LOW);   // Disconnect battery [cite: 12, 62]
    digitalWrite(BUZZER_PIN, HIGH); // Alert sound [cite: 12, 63]
    lcd.clear();
    lcd.print("SAFETY ALERT!");
    Serial.println("ALERT: System Disconnected for Safety");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // 4. Data Transmission (IoT Simulation) [cite: 63, 141]
  Serial.print("Data Log - Temp:");
  Serial.print(temperature);
  Serial.print(" Volt:");
  Serial.println(voltage);

  delay(2000); // Sample every 2 seconds
}
