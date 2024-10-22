#define BLYNK_TEMPLATE_ID "TMPL3YVU0zF7-"
#define BLYNK_TEMPLATE_NAME "Air monitoring system"
#define BLYNK_AUTH_TOKEN "fQUR1aV2Er6TII97RmykQNpZYbIytUMb"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Define LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define MQ sensors
#define MQ3PIN 35
#define MQ135PIN 34

// WiFi credentials
char ssid[] = "Airtel_Renukanthan";
char pass[] = "Renu@0987";

BlynkTimer timer;

// Function to send sensor data to Blynk and display on LCD
void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int mq3_value = analogRead(MQ3PIN);
  int mq135_value = analogRead(MQ135PIN);

  // Convert analog readings to ppm (simplified approximation)
  float alcohol_ppm = (mq3_value / 4095.0) * 1000;
  float co2_ppm = (mq135_value / 4095.0) * 1000;

  // Print data to Serial (for debugging)
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C");
  Serial.print(" Humidity: ");
  Serial.print(humidity);
  Serial.print(" %");
  Serial.print(" Alcohol: ");
  Serial.print(alcohol_ppm);
  Serial.print(" ppm");
  Serial.print(" CO2: ");
  Serial.print(co2_ppm);
  Serial.println(" ppm");

  // Print data to LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("Alcohol: ");
  lcd.print(alcohol_ppm);
  lcd.print(" ppm");

  lcd.setCursor(0, 3);
  lcd.print("CO2: ");
  lcd.print(co2_ppm);
  lcd.print(" ppm");

  // Send data to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, alcohol_ppm);
  Blynk.virtualWrite(V3, co2_ppm);
}

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD
  lcd.begin();
  lcd.backlight();

  // Connect to WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
