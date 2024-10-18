#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Redmi";
const char* password = "12345678";

WebServer server(80);

// Analog pin connected to the sensor
const int sensorPin = 26;

// Variable to hold sensor reading
int sensorValue = 0;

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi with DHCP
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print ESP32 IP address to serial monitor
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, handleRoot);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient(); // Handle client requests

  // Your sensor reading code goes here
  // sensorValue = analogRead(sensorPin);
  sensorValue = 105;
  // You may need to convert sensor value to an appropriate format
  // before displaying it on the web page

  delay(1000); // Optionally add delay between readings
}

void handleRoot() {
  // Send HTTP response with a web page
  server.send(200, "text/html", "<html><head><title>ESP32 Sensor Readings</title></head><body><h1>Sensor Readings</h1><p>Value: " + String(sensorValue) + "</p></body></html>");
}
