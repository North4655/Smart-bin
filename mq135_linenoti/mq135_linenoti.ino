#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>

// Your Wi-Fi credentials
const char* ssid = "North";         // Replace with your WiFi SSID
const char* password = "11111111"; // Replace with your WiFi password

// Line Notify Token (replace with your generated token)
#define LINE_TOKEN "H1sluwyN7R4ALZkKvFiRoKzIq7FLn9k4gZkSwFgWDSW"  // Line Notify token

// Pin Definitions
const int mq135AnalogPin = A0;  // Analog input pin for MQ-135

// Define a threshold value for Ammonia (You may need to calibrate this)
const int ammoniaThreshold = 700;  // Adjust this based on calibration

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize Wi-Fi connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi!");

  // Initialize Line Notify with your token
  LINE.setToken(LINE_TOKEN);

  Serial.println("MQ-135 Analog Test Initialized");
}

void loop() {
  // Read the analog signal from MQ-135
  int analogValue = analogRead(mq135AnalogPin);

  // Display the analog value for debugging
  Serial.print("Analog Value: ");
  Serial.println(analogValue);

  // Check if the analog value exceeds the threshold
  if (analogValue > ammoniaThreshold) {
    Serial.println("High Ammonia levels detected!");

    // Send notification via Line Notify
    LINE.notify("Warning: High levels of Ammonia detected! : ");

    // Small delay to avoid sending too many notifications
    delay(1000);  // Wait 1 minute before checking again
  } else {
    Serial.println("Ammonia levels are normal.");
  }

  // Simple delay for readability (2 seconds)
  delay(1000);
}
