#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <EmonLib.h>
#include <EEPROM.h>
#define BLYNK_TEMPLATE_ID1 "TMPL37ozmL-tH"
#define BLYNK_TEMPLATE_NAME1 "SMART ENERGY METER 2"
#define BLYNK_AUTH_TOKEN1 "rM5TtG8ZZJvPERIKqfxUsQpSX5ktIgLF"
#define BLYNK_TEMPLATE_ID2 "TMPL3isD1NECk"
#define BLYNK_TEMPLATE_NAME2 "Smart Energy Monitoring"
#define BLYNK_AUTH_TOKEN2 "oLBathjnTyuoxp3M6_Ke_2NRomJAIObz"
#include <BlynkSimpleEsp32.h>

EnergyMonitor emon;
#define vCalibration 83.3
#define currCalibration 0.50

BlynkTimer timer1;
BlynkTimer timer2;
char ssid[] = "Shyam_Oneplus";
    char pass[] = "hellohello";
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define RELAY1_PIN 13
#define RELAY2_PIN 12
#define RELAY3_PIN 14
#define RELAY4_PIN 27

bool relay1;
bool relay2;
bool relay3;
bool relay4;

float kWh = 0;
float price = 0;
float monthly_price = 0;
unsigned long lastmillis = millis();



// #define BLYNK_TEMPLATE_ID1 "TMPL37ozmL-tH"
// #define BLYNK_TEMPLATE_NAME1 "SMART ENERGY METER 2"
// #define BLYNK_AUTH_TOKEN1 "rM5TtG8ZZJvPERIKqfxUsQpSX5ktIgLF"

// #define BLYNK_TEMPLATE_ID2 "TMPL3isD1NECk"
// #define BLYNK_TEMPLATE_NAME2 "Smart Energy Monitoring"
// #define BLYNK_AUTH_TOKEN2 "oLBathjnTyuoxp3M6_Ke_2NRomJAIObz"

void initProperties() {
  relay1 = false;
  relay2 = false;
  relay3 = false;
  relay4 = false;
}

void onRelay1Change() {
  Serial.println("On-Change Event Relay1");
  digitalWrite(RELAY1_PIN, relay1 ? HIGH : LOW);
}

void onRelay2Change() {
  Serial.println("On-Change Event Relay2");
  digitalWrite(RELAY2_PIN, relay2 ? HIGH : LOW);
}

void onRelay3Change() {
  Serial.println("On-Change Event Relay3");
  digitalWrite(RELAY3_PIN, relay3 ? HIGH : LOW);
}

void onRelay4Change() {
  Serial.println("On-Change Event Relay4");
  digitalWrite(RELAY4_PIN, relay4 ? HIGH : LOW);
}

void myTimerEvent1() {
  emon.calcVI(20, 2000);
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  price = kWh * 100;
  monthly_price = price * 61;
  yield();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vrms:");
  lcd.print(emon.Vrms, 2);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("Irms:");
  lcd.print(emon.Irms, 4);
  lcd.print("A");
  lcd.setCursor(0, 2);
  lcd.print("Power:");
  lcd.print(emon.apparentPower, 4);
  lcd.print("W");
  lcd.setCursor(0, 3);
  lcd.print("kWh:");
  lcd.print(kWh, 4);
  lcd.print("W");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(BLYNK_TEMPLATE_NAME1);
  lcd.setCursor(0, 1);
  lcd.print("NetCharge:");
  lcd.print("Rs.");
  lcd.print(monthly_price, 4);

  lastmillis = millis();

  Blynk.virtualWrite(V0, emon.Vrms);
  Blynk.virtualWrite(V1, emon.Irms);
  Blynk.virtualWrite(V2, emon.apparentPower);
  Blynk.virtualWrite(V3, kWh);
  Blynk.virtualWrite(V4, monthly_price);
}

void myTimerEvent2() {
  int bulb1State = digitalRead(V5);
  int bulb2State = digitalRead(V6);
  int bulb3State = digitalRead(V7);
  int bulb4State = digitalRead(V8);

  digitalWrite(RELAY1_PIN, bulb1State == HIGH ? HIGH : LOW);
  digitalWrite(RELAY2_PIN, bulb2State == HIGH ? HIGH : LOW);
  digitalWrite(RELAY3_PIN, bulb3State == HIGH ? HIGH : LOW);
  digitalWrite(RELAY4_PIN, bulb4State == HIGH ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN1, ssid, pass, BLYNK_TEMPLATE_ID1, BLYNK_TEMPLATE_NAME1); // Initialize Blynk for the first template
  Blynk.begin(BLYNK_AUTH_TOKEN2, ssid, pass, BLYNK_TEMPLATE_ID2, BLYNK_TEMPLATE_NAME2); // Initialize Blynk for the second template
  lcd.init();
  lcd.backlight();
  emon.voltage(35, vCalibration, 1.7);
  emon.current(34, currCalibration);

  timer1.setInterval(5000L, myTimerEvent1); // Set timer interval for the first template
  timer2.setInterval(5000L, myTimerEvent2); // Set timer interval for the second template
  lcd.setCursor(3, 0);
  lcd.print(BLYNK_TEMPLATE_NAME1);
  lcd.setCursor(5, 1);
  lcd.print("METER");
  delay(3000);
  lcd.clear();

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
}

void loop() {
  Blynk.run(); // Run Blynk for both templates
  timer1.run();
  timer2.run();
}