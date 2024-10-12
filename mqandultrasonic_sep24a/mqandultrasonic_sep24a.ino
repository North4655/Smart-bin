#include <TridentTD_LineNotify.h>
#include <ESP8266WiFi.h>

// Wi-Fi Credentials
const char* ssid = "North";         // Replace with your WiFi SSID
const char* password = "11111111"; // Replace with your WiFi password

// Line Notify Token
#define LINE_TOKEN "H1sluwyN7R4ALZkKvFiRoKzIq7FLn9k4gZkSwFgWDSW"

// MQ-135 Analog Pin
const int mq135AnalogPin = A0;  // Analog input pin for MQ-135
const int ammoniaThreshold = 700;  // Threshold for high ammonia levels

// Ultrasonic Sensor Pins
#define Trig_PIN D3 // Trigger pin for Ultrasonic Sensor
#define Echo_PIN D4 // Echo pin for Ultrasonic Sensor

// Notification Flags
bool notificationSentTrash = false;   // Flag to track whether trash bin notification has been sent
bool notificationSentGas = false;     // Flag to track whether ammonia notification has been sent

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Initialize WiFi connection
  WiFi.begin(ssid, password);
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected!");

  // Initialize Line Notify
  LINE.setToken(LINE_TOKEN);

  // Setup ultrasonic sensor pins
  pinMode(Trig_PIN, OUTPUT);
  pinMode(Echo_PIN, INPUT);

  Serial.println("System Initialized.");
}

void loop() {
  // Check the ultrasonic sensor (for trash level)
  double distance = measureDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance <= 5 && !notificationSentTrash) {
    // Send notification if the trash is full
    LINE.notify("Trash bin is almost full! Distance: " );
    notificationSentTrash = true;
  } else if (distance > 5 && notificationSentTrash) {
    // Reset notification flag if the bin is emptied
    notificationSentTrash = false;
  }

  // Check the MQ-135 sensor (for ammonia levels)
  int analogValue = analogRead(mq135AnalogPin);
  Serial.print("MQ-135 Analog Value: ");
  Serial.println(analogValue);

  if (analogValue > ammoniaThreshold && !notificationSentGas) {
    // Send notification if ammonia levels exceed threshold
    LINE.notify("High Ammonia levels detected! Analog Value: " );
    notificationSentGas = true;
  } else if (analogValue <= ammoniaThreshold && notificationSentGas) {
    // Reset notification flag if ammonia levels return to normal
    notificationSentGas = false;
  }

  // Small delay between sensor readings
  delay(2000);
}

// Function to measure distance using ultrasonic sensor
long measureDistance() {
  long duration, distance;

  // Send a pulse to trigger the sensor
  digitalWrite(Trig_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_PIN, LOW);

  // Measure the pulse duration
  duration = pulseIn(Echo_PIN, HIGH);

  // Convert the pulse duration to distance in centimeters
  distance = duration / 29 / 2;

  return distance;
}
