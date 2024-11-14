#include <WiFi.h>

#include <Wire.h>

#include <LiquidCrystal_I2C.h>

#include <DHT.h>



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

const char* ssid = "wifiname";

const char* password = "wifipassword";



// HTML content for the web page

String htmlPage;



// Function to generate the HTML dashboard

void generateHTML(float temperature, float humidity, float alcohol_ppm, float co2_ppm) {

  htmlPage = "<!DOCTYPE html><html lang='en'>";

  htmlPage += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";

  htmlPage += "<title>Air Monitoring System</title>";

  htmlPage += "<style>";

  htmlPage += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f0f8ff; color: #333; }";

  htmlPage += ".header { background-color: #4CAF50; padding: 20px; text-align: center; color: white; font-size: 2em; }";

  htmlPage += ".container { padding: 20px; display: flex; flex-direction: column; align-items: center; }";

  htmlPage += ".card { background-color: white; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); padding: 20px; margin: 10px; border-radius: 10px; width: 80%; max-width: 600px; text-align: center; }";

  htmlPage += "h2 { color: #4CAF50; }";

  htmlPage += ".chart { margin-top: 20px; }";

  htmlPage += "canvas { max-width: 100%; }";

  htmlPage += "</style>";

  htmlPage += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";

  htmlPage += "</head><body>";

  htmlPage += "<div class='header'>Air Monitoring System</div>";

  htmlPage += "<div class='container'>";

  htmlPage += "<div class='card'><h2>Temperature</h2><p>" + String(temperature) + " °C</p></div>";

  htmlPage += "<div class='card'><h2>Humidity</h2><p>" + String(humidity) + " %</p></div>";

  htmlPage += "<div class='card'><h2>Alcohol</h2><p>" + String(alcohol_ppm) + " ppm</p></div>";

  htmlPage += "<div class='card'><h2>CO2</h2><p>" + String(co2_ppm) + " ppm</p></div>";

  htmlPage += "<div class='chart'><canvas id='sensorChart'></canvas></div>";

  htmlPage += "</div>";

  htmlPage += "<script>";

  htmlPage += "const ctx = document.getElementById('sensorChart').getContext('2d');";

  htmlPage += "const chart = new Chart(ctx, { type: 'bar', data: { labels: ['Temperature', 'Humidity', 'Alcohol', 'CO2'], datasets: [{ label: 'Sensor Data', data: [" + String(temperature) + ", " + String(humidity) + ", " + String(alcohol_ppm) + ", " + String(co2_ppm) + "], backgroundColor: ['#ff9999', '#99ccff', '#ccff99', '#ffcc99'], borderColor: ['#ff4d4d', '#4d94ff', '#4dff4d', '#ff944d'], borderWidth: 1 }] }, options: { scales: { y: { beginAtZero: true } } } });";

  htmlPage += "</script>";

  htmlPage += "</body></html>";

}



// Create a server on port 80

WiFiServer server(80);



void setup() {

  // Start serial communication

  Serial.begin(9600);



  // Initialize DHT sensor

  dht.begin();



  // Initialize LCD

  lcd.begin();

  lcd.backlight();



  // Connect to WiFi

  Serial.print("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);

    Serial.print(".");

  }

  Serial.println("\nConnected to WiFi");

  Serial.print("IP Address: ");

  Serial.println(WiFi.localIP());



  // Start the server

  server.begin();

}



void loop() {

  // Read sensor data

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

  Serial.print(" °C Humidity: ");

  Serial.print(humidity);

  Serial.print(" % Alcohol: ");

  Serial.print(alcohol_ppm);

  Serial.print(" ppm CO2: ");

  Serial.println(co2_ppm);



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



  // Generate the HTML dashboard

  generateHTML(temperature, humidity, alcohol_ppm, co2_ppm);



  // Check for incoming client requests

  WiFiClient client = server.available();

  if (client) {

    Serial.println("New Client Connected");

    String request = client.readStringUntil('\r');

    Serial.println(request);

    client.flush();



    // Send the HTML response

    client.print("HTTP/1.1 200 OK\r\n");

    client.print("Content-Type: text/html\r\n");

    client.print("Connection: close\r\n\r\n");

    client.print(htmlPage);

    client.stop();

    Serial.println("Client Disconnected");

  }



  delay(1000);

}

