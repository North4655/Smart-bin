#define BLYNK_TEMPLATE_ID "TMPL6xSvMRz97"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "duiVCPLR_UUnxUvQj-X0Pkw88XG47KNY"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

Servo servo; // Servo object for controlling the motor

int trigPin = D5;    // Ultrasonic sensor TRIG pin
int echoPin = D6;    // Ultrasonic sensor ECHO pin
int servoPin = D7;   // Servo connected to D7

long duration, dist, average;   
long aver[3];   // Array for averaging distance measurements

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "North";      // Replace with your WiFi SSID
char pass[] = "11111111";   // Replace with your WiFi password

unsigned long previousMillis = 0; 
const long interval = 4000; // 4 seconds open lid
bool isLidOpen = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");

  // Connect to WiFi and Blynk server
  Blynk.begin(auth, ssid, pass);

  // Set up ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);  

  // Attach servo to D7 and set initial position to closed (180 degrees)
  servo.attach(servoPin);  
  servo.write(180); // Start with the servo closed
}

void measure() {  
  // Trigger the ultrasonic sensor to measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the time for the echo to return
  duration = pulseIn(echoPin, HIGH);
  dist = (duration / 2) / 29.1; // Calculate distance in cm
}

void measureDistance() {
  // Measure distance and average over 3 readings
  for (int i = 0; i <= 2; i++) {
    measure();               
    aver[i] = dist;            
    delay(10);  // Small delay between measurements
  }
  dist = (aver[0] + aver[1] + aver[2]) / 3;  // Average the three readings
}

void loop() {
  // Run Blynk-related tasks
  Blynk.run();

  // Measure distance from the ultrasonic sensor
  measureDistance();

  // If an object is closer than 25 cm, control the servo
  if (dist < 25 && !isLidOpen) { 
    servo.write(0); // Open the lid (0 degrees)
    previousMillis = millis();  // Record the time when the lid was opened
    isLidOpen = true;  // Set flag to indicate lid is open
  }

  // Close the lid after 4 seconds
  if (isLidOpen && (millis() - previousMillis >= interval)) {
    servo.write(180); // Close the lid (180 degrees)
    isLidOpen = false; // Reset the flag
  }

  // Output the distance for debugging
  Serial.print("Distance: ");
  Serial.println(dist);
}

// Blynk virtual pin handlers
BLYNK_WRITE(V0) {
  servo.write(param.asInt()); // Control the servo position directly via Blynk
}

BLYNK_WRITE(V1) {
  servo.write(0); // Move to 0 degrees (open) when V1 is triggered
}

BLYNK_WRITE(V2) {
  servo.write(180); // Move to 180 degrees (closed) when V2 is triggered
}
