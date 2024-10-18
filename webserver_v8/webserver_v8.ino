#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* ssid = "Redmi";
const char* password = "12345678";

WebServer server(80);

#define tdsPin 35
#define WaterTempPin 34
#define ldrPin 26 // Pin connected to LDR

// Digital pin connected to the VIN pin
const int vinPin = 27;
const int d2Pin = 2;

// Initialize DHT sensor
#define DHTPIN 19      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22  // DHT 22 (AM2302) sensor type
DHT dht(DHTPIN, DHTTYPE);

// Variable to hold sensor reading
int ecVal = 0;
int waTempVal = 0;
int ldrVal = 0; // Variable to hold LDR reading
float temperature = 0;
float humidity = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  
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
  ecVal = analogRead(tdsPin);
  Serial.println("ecVal: ");
  Serial.println(ecVal);
  
  waTempVal = analogRead(WaterTempPin);
  Serial.println("waTempVal: ");
  Serial.println(waTempVal);

  // Read temperature and humidity from DHT sensor
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
  }

  // Read LDR value
  ldrVal = analogRead(ldrPin);
  Serial.println("LDR value: ");
  Serial.println(ldrVal);

  delay(100); // Optionally add delay between readings
}

void handleRoot() {
  // Send HTTP response with a web page
  String html = "<html><head><title>ESP32 Sensor Control</title>";
  html += "<style>";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f7f7f7; margin: 0; padding: 0; background-image: linear-gradient(to right, #3494e6, #ec6ead); }";
  html += "h1 { color: #fff; text-align: center; margin-top: 20px; text-shadow: 2px 2px 4px rgba(0,0,0,0.5); }";
  html += ".container { max-width: 600px; margin: 20px auto; padding: 20px; background-color: rgba(255, 255, 255, 0.9); border-radius: 20px; box-shadow: 0 12px 24px rgba(0,0,0,0.2); border: 1px solid #ddd; }";
  html += ".sensor-info { margin-bottom: 20px; padding: 15px; border-radius: 15px; background-color: #fdcb6e; box-shadow: 0 6px 12px rgba(0,0,0,0.1); }";
  html += ".sensor-info p { margin: 5px 0; }";
  html += ".middle-container { padding: 20px; background-color: #fab1a0; border-radius: 20px; box-shadow: 0 6px 12px rgba(0,0,0,0.1); }";
  html += ".btn-container { text-align: center; }";
  html += ".btn { padding: 14px 28px; background-color: #00b894; color: white; border: none; cursor: pointer; margin: 10px; border-radius: 12px; box-shadow: 0 6px 12px rgba(0,0,0,0.2); transition: background-color 0.3s, transform 0.2s; }";
  html += ".btn:hover { background-color: #00a896; transform: translateY(-2px); }";
  html += ".btn.off { background-color: #d63031; }";
  html += ".btn.off:hover { background-color: #b71540; }";
  html += "</style>";
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
  html += "<h1>Grow Tower Control</h1>";
  html += "<div class='container'>";
  html += "<div class='sensor-info'>";
  html += "<p><strong>EC sensor value:</strong> " + String(ecVal) + "</p><br>";
  html += "<p><strong>Water Templature:</strong> " + String(waTempVal) + "</p><br>";
  html += "<p><strong>LDR value:</strong> " + String(ldrVal) + "</p><br>";
  html += "<p><strong>Air Temperature:</strong> " + String(temperature) + "°C</p><br>";
  html += "<p><strong>Air Humidity:</strong> " + String(humidity) + "%</p>";
  html += "</div>";
  html += "<div class='middle-container'>";
  html += "<div class='btn-container'>";
  html += "<button class='btn' onclick=\"toggleVin('on');\"> Motor On </button>";
  html += "<button class='btn off' onclick=\"toggleVin('off');\"> Motor Off </button><br>";
  html += "<button class='btn' onclick=\"toggleD2('on');\"> Buzzer On </button>";
  html += "<button class='btn off' onclick=\"toggleD2('off');\"> Buzzer Off </button>";
  html += "</div></div>";
  html += "</div></body></html>";
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
