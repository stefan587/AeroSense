
#define BLYNK_TEMPLATE_ID "TMPL4jU9IdAu_"
#define BLYNK_TEMPLATE_NAME "AeroSense"
#define BLYNK_AUTH_TOKEN "***"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>


char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "***";
char pass[] = "***";

#define DHTPIN 2        // D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define RELAY_PIN 16    // D0
#define RED_PIN    14   // D5
#define YELLOW_PIN 12   // D6
#define GREEN_PIN  13   // D7

LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

int displayState = 0;
float lastTemp = 0;
float lastHum = 0;
float lastCO2 = 0;
float lastAirQ = 0;

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  dht.begin();
  lcd.init();
  lcd.backlight();

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(5000L, readSensors);
  timer.setInterval(3000L, updateDisplay);
}

void readSensors() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  float co2 = analogRead(A0) * 1000.0 / 1023.0; 
  float airq = analogRead(A0); 

  if (isnan(temp) || isnan(hum)) return;

  lastTemp = temp;
  lastHum = hum;
  lastCO2 = co2;
  lastAirQ = airq;

  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V2, co2);

  if (temp > 25) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" | Hum: "); Serial.print(hum);
  Serial.print(" | CO2: "); Serial.println(co2);
}

void updateDisplay() {
  lcd.clear();

  switch (displayState) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.setCursor(0, 1);
      lcd.print(lastTemp, 1);
      lcd.print((char)223); // символ за °
      lcd.print("C");
      break;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Humidity: ");
      lcd.setCursor(0, 1);
      lcd.print(lastHum, 1);
      lcd.print("%");
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("CO2: ");
      lcd.setCursor(0, 1);
      lcd.print(lastCO2, 0);
      lcd.print(" ppm");
      break;

    case 3:
      lcd.setCursor(0, 0);
      lcd.print("AirQuality: ");
      lcd.setCursor(0, 1);
      lcd.print(lastAirQ, 0);
      break;
  }

  displayState = (displayState + 1) % 4;
}


void loop() {
  Blynk.run();
  timer.run();
}
