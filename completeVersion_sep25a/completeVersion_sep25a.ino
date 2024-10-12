// Blynk Credentials
#define BLYNK_TEMPLATE_ID "TMPL6xSvMRz97"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "duiVCPLR_UUnxUvQj-X0Pkw88XG47KNY"
#define BLYNK_PRINT Serial

#include <TridentTD_LineNotify.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

// Wi-Fi Credentials
const char* ssid = "North";         // Replace with your WiFi SSID
const char* password = "11111111"; // Replace with your WiFi password

// Line Notify Token
#define LINE_TOKEN "H1sluwyN7R4ALZkKvFiRoKzIq7FLn9k4gZkSwFgWDSW"

// Pin Definitions
const int mq135AnalogPin = A0;      // Analog input pin for MQ-135
const int ammoniaThreshold = 700;   // Threshold for high ammonia levels
#define Trig_PIN D3                 // Trigger pin for trash ultrasonic sensor
#define Echo_PIN D4                 // Echo pin for trash ultrasonic sensor
#define trigPinPeople D5            // Trigger pin for people detection ultrasonic sensor
#define echoPinPeople D6            // Echo pin for people detection ultrasonic sensor
#define servoPin D7                 // Servo connected to D7

// Notification Flags
bool notificationSentTrash = false;   // Flag to track whether trash bin notification has been sent
bool notificationSentGas = false;     // Flag to track whether ammonia notification has been sent

unsigned long previousMillis = 0;     // For servo control timing
const long interval = 4000;           // 4 seconds open lid
bool isLidOpen = false;

Servo servo; // Servo object for controlling the motor

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

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

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Setup ultrasonic sensor pins
  pinMode(Trig_PIN, OUTPUT);
  pinMode(Echo_PIN, INPUT);
  pinMode(trigPinPeople, OUTPUT);
  pinMode(echoPinPeople, INPUT);

  // Attach servo to D7 and set initial position to closed (180 degrees)
  servo.attach(servoPin);
  servo.write(180); // Start with the servo closed

  Serial.println("System Initialized.");
}

void loop() {
  // Run Blynk
  Blynk.run();

  // Check trash level ultrasonic sensor
  double trashDistance = measureDistance(Trig_PIN, Echo_PIN);
  Serial.print("Trash Distance: ");
  Serial.println(trashDistance);

  if (trashDistance <= 5 && !notificationSentTrash) {
    // Send notification if the trash is full
    LINE.notify("Smart bin is almost full! Distance !!!" );
    notificationSentTrash = true;
  } else if (trashDistance > 5 && notificationSentTrash) {
    // Reset notification flag if the bin is emptied
    notificationSentTrash = false;
  }

  // Check MQ-135 sensor (for ammonia levels)
  int analogValue = analogRead(mq135AnalogPin);
  Serial.print("MQ-135 Analog Value: ");
  Serial.println(analogValue);

  if (analogValue > ammoniaThreshold && !notificationSentGas) {
    // Send notification if ammonia levels exceed threshold
    LINE.notify("High Ammonia levels detected !!! ");
    notificationSentGas = true;
  } else if (analogValue <= ammoniaThreshold && notificationSentGas) {
    // Reset notification flag if ammonia levels return to normal
    notificationSentGas = false;
  }

  // Check people detection ultrasonic sensor
  double peopleDistance = measureDistance(trigPinPeople, echoPinPeople);
  Serial.print("People Detection Distance: ");
  Serial.println(peopleDistance);

  // If a person is detected within 25 cm, open the lid
  if (peopleDistance < 25 && !isLidOpen) {
    servo.write(0);  // Open the lid (0 degrees)
    previousMillis = millis();  // Record the time when the lid was opened
    isLidOpen = true;
  }

  // Close the lid after 4 seconds
  if (isLidOpen && (millis() - previousMillis >= interval)) {
    servo.write(180);  // Close the lid (180 degrees)
    isLidOpen = false;
  }

  // Small delay between sensor readings
  delay(2000);
}

// Function to measure distance using ultrasonic sensors
long measureDistance(int trigPin, int echoPin) {
  long duration, distance;

  // Send a pulse to trigger the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the pulse duration
  duration = pulseIn(echoPin, HIGH);

  // Convert the pulse duration to distance in centimeters
  distance = duration / 29 / 2;

  return distance;
}

// Blynk virtual pin handlers for manual servo control
BLYNK_WRITE(V0) {
  servo.write(param.asInt()); // Control the servo position directly via Blynk
}

BLYNK_WRITE(V1) {
  servo.write(0);  // Move to 0 degrees (open) when V1 is triggered
}

BLYNK_WRITE(V2) {
  servo.write(180); // Move to 180 degrees (closed) when V2 is triggered
}
