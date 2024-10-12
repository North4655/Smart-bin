#include <TridentTD_LineNotify.h>
#include <ESP8266WiFi.h>

#define Trig_PIN D3 // Define pin D5 in NodeMCU ESP8266 for Trigger
#define Echo_PIN D4 // Define pin D6 in NodeMCU ESP8266 for Echo

// Set WiFi SSID and password for internet connection
#define SSID "North"    // WiFi name to connect to (use "Registers" for the instructor's case)
#define PASSWORD "11111111" // WiFi password to connect to (use "12345678" for the instructor's case)

#define LINE_TOKEN "H1sluwyN7R4ALZkKvFiRoKzIq7FLn9k4gZkSwFgWDSW" // Token obtained from previous steps

bool notificationSent = false; // Flag to track whether notification has been sent

void setup() {
  Serial.begin(115200); // Start serial communication
  Serial.println();
  Serial.println(LINE.getVersion());

  // Connect to WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }

  // Initialize pins
  pinMode(Echo_PIN, INPUT);
  pinMode(Trig_PIN, OUTPUT);

  // Set LINE token
  LINE.setToken(LINE_TOKEN);
}

void loop() {
  double distance = US();  // Measure distance using ultrasonic sensor
  Serial.print(distance);
  Serial.println(" : centimeters");
  delay(100);

  // Check condition: If distance is less than or equal to 5 cm and notification hasn't been sent, send notification and turn off sensor
  if (distance <= 5 && !notificationSent) {
    LINE.notify("Distance = " + String(distance)); // Send notification
    LINE.notify("Trash bin almost full");
    notificationSent = true; // Set flag to true indicating notification has been sent
    digitalWrite(Trig_PIN, LOW); // Turn off sensor
    delay(1000); // Optional delay for stability
  }

  // If distance is greater than 5 cm and notification has been sent, reset flag and turn on sensor
  if (distance > 5 && notificationSent) {
    notificationSent = false; // Reset flag
    digitalWrite(Trig_PIN, HIGH); // Turn on sensor
    delay(1000); // Optional delay for stability
  }
}

// Function to measure distance using ultrasonic sensor
long US() {
  long duration, cm;

  // Trigger ultrasonic sensor
  pinMode(Trig_PIN, OUTPUT);
  digitalWrite(Trig_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(Trig_PIN, LOW);

  // Receive echo and calculate duration
  pinMode(Echo_PIN, INPUT);
  duration = pulseIn(Echo_PIN, HIGH);

  // Calculate distance in centimeters
  cm = duration / 29 / 2;
  return cm;

  delay(1000);
}
