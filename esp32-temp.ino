#include <DHT.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>

#define DHTPIN 15
#define DHTTYPE DHT11

#define RED_PIN 13
#define GREEN_PIN 12
#define BLUE_PIN 14

DHT dht(DHTPIN, DHTTYPE);
AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);

    // Start
    dht.begin();
    
    // Set the LED pins as output
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    // Start Wi-Fi
    WiFiManager wifiManager;
    // Config SSID
    wifiManager.autoConnect("ESP32_Config");
    // Print the IP
    Serial.print("Done. IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        float temperature = dht.readTemperature(); // Get temperature in Celsius
        float humidity = dht.readHumidity(); // Get humidity

        // Check if the readings failed
        if (isnan(temperature) || isnan(humidity)) {
            request->send(500, "text/plain", "Sensor reading error.");
            return;
        }

        String color = "green";

        if (temperature > 25 || humidity >= 75 || temperature < 18) {
            color = "red";
        } else if (temperature >= 23.5 && temperature <= 25 || humidity > 68) {
            color = "yellow";
        }

        String html = "<html><head>";
        html += "<meta http-equiv='refresh' content='5'>"; // Refresh
        html += "<meta charset='UTF-8'>";
        html += "<style>body { font-family: Arial, sans-serif; text-align: center; padding: 50px; background-color: " + color + "; color:black; }";
        html += "h1 { font-size: 3em; } p { font-size: 1.5em; }</style>";
        html += "</head><body>";
        html += "<h1>Our Room Temperature & Humidity</h1>";
        html += "<p>Temperature: " + String(temperature) + "°C</p>";
        html += "<p>Humidity: " + String(humidity) + "%</p>";
        html += "</body></html>";
        request->send(200, "text/html", html);
    });

    // Start the server
    server.begin();
}

void setColor(int r, int g, int b) {
    digitalWrite(RED_PIN, r);
    digitalWrite(GREEN_PIN, g);
    digitalWrite(BLUE_PIN, b);
}

void loop() {
    // Read sensor
    float temperature = dht.readTemperature(); // Read temperature in Celsius
    float humidity = dht.readHumidity(); // Read humidity
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Sensor reading error.");
        return;
    }
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    if (temperature > 25 || humidity >= 75 || temperature < 18) {
        setColor(HIGH, LOW, LOW); // Red
    } 
    else if (temperature >= 23.5 && temperature <= 25 || humidity > 68) {
        setColor(HIGH, HIGH, LOW); // Yellow
    } 
    else {
        setColor(LOW, HIGH, LOW); // Green
    }

    delay(2000);
}
