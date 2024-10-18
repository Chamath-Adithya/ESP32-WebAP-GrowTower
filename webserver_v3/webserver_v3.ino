#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Redmi";
const char* password = "12345678";

WebServer server(80);

#define sensorPin 35
#define sensorPin2 34
#define sensorPin3 19

// Digital pin connected to the VIN pin
const int vinPin = 27;
const int d2Pin = 2;

// Variable to hold sensor reading
int sensorValue = 0;
int sensorValue2 = 0;
int sensorValue3 = 0;

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

  // Configure VIN pin as output
  pinMode(vinPin, OUTPUT);
  digitalWrite(vinPin, LOW); // Initial state is off
  
  // Configure D2 pin as output
  pinMode(d2Pin, OUTPUT);
  digitalWrite(d2Pin, LOW); // Initial state is off

  // Route for root / web page
  server.on("/", HTTP_GET, handleRoot);

  // Route for turning VIN pin on
  server.on("/on", HTTP_GET, handleOn);

  // Route for turning VIN pin off
  server.on("/off", HTTP_GET, handleOff);

  // Route for turning D2 pin on
  server.on("/d2on", HTTP_GET, d2handleOn);

  // Route for turning D2 pin off
  server.on("/d2off", HTTP_GET, d2handleOff);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient(); // Handle client requests

  // Your sensor reading code goes here
  sensorValue = analogRead(sensorPin);
  Serial.println("sensorValue: ");
  Serial.println(sensorValue);
  
  sensorValue2 = analogRead(sensorPin2);
  Serial.println("sensorValue2: ");
  Serial.println(sensorValue2);
  
  sensorValue3 = analogRead(sensorPin3);
  Serial.println("sensorValue3: ");
  Serial.println(sensorValue3);
  
  //sensorValue = 105;
  // You may need to convert sensor value to an appropriate format
  // before displaying it on the web page

  delay(100); // Optionally add delay between readings
}

void handleRoot() {
  // Send HTTP response with a web page
  String html = "<html><head><title>ESP32 Sensor Control</title>";
  html += "<script>function toggleVin(action) {";
  html += "var xhr = new XMLHttpRequest();";
  html += "xhr.open('GET', '/' + action, true);";
  html += "xhr.onreadystatechange = function() {";
  html += "if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "var response = JSON.parse(xhr.responseText);";
  html += "alert(response.message);";
  html += "}};";
  html += "xhr.send();";
  html += "}</script>";
  html += "<script>function toggleD2(action) {";
  html += "var xhr = new XMLHttpRequest();";
  html += "xhr.open('GET', '/d2' + action, true);";
  html += "xhr.onreadystatechange = function() {";
  html += "if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "var response = JSON.parse(xhr.responseText);";
  html += "alert(response.message);";
  html += "}};";
  html += "xhr.send();";
  html += "}</script>";
  html += "</head><body>";
  html += "<h1>ESP32 Sensor Control</h1>";
  
  html += "<p>Current sensor value: " + String(sensorValue) + "</p>";
  html += "<p>Current sensor value: " + String(sensorValue2) + "</p>";
  html += "<p>Current sensor value: " + String(sensorValue3) + "</p>";
  
  html += "<p><button onclick=\"toggleVin('on');\">Turn On VIN Pin</button>&nbsp;";
  html += "<button onclick=\"toggleVin('off');\">Turn Off VIN Pin</button></p>";

  html += "<p><button onclick=\"toggleD2('on');\">Turn On D2 Pin</button>&nbsp;";
  html += "<button onclick=\"toggleD2('off');\">Turn Off D2 Pin</button></p>";
  
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(vinPin, HIGH); // Turn VIN pin on
  sendJsonResponse("VIN pin turned on");
}

void handleOff() {
  digitalWrite(vinPin, LOW); // Turn VIN pin off
  sendJsonResponse("VIN pin turned off");
}

void d2handleOn() {
  digitalWrite(d2Pin, HIGH); // Turn D2 pin on
  sendJsonResponse("D2 pin turned on");
}

void d2handleOff() {
  digitalWrite(d2Pin, LOW); // Turn D2 pin off
  sendJsonResponse("D2 pin turned off");
}

void sendJsonResponse(const char* message) {
  String json = "{\"message\": \"" + String(message) + "\"}";
  server.send(200, "application/json", json);
}
