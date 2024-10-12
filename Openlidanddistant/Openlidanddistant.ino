#include <Servo.h>       // Servo library
#include <TridentTD_LineNotify.h>
#include <ESP8266WiFi.h>

Servo servo;             // Servo object

// Define pins for Ultrasonic Sensor 1 (for controlling servo)
int trigPinServo = D5;
int echoPinServo = D6;

// Define pins for Ultrasonic Sensor 2 (for sending LINE notifications)
int trigPinNotif = D3;
int echoPinNotif = D4;

int servoPin = D7;       // Servo pin
int led = 10;            // LED pin for visual indicator

long durationServo, distServo, averageServo;
long averServo[3];       // Array for averaging distances (Servo control)

long durationNotif, distNotif; // Variables for notification sensor

// WiFi credentials
#define SSID "North"           // WiFi SSID
#define PASSWORD "11111111"    // WiFi password

// LINE Notify token
#define LINE_TOKEN "H1sluwyN7R4ALZkKvFiRoKzIq7FLn9k4gZkSwFgWDSW"
bool notificationSent = false; // Flag to check if notification was sent

void setup() {
    // Serial communication for debugging
    Serial.begin(115200);
    Serial.println();
    Serial.println(LINE.getVersion());

    // Connect to WiFi
    WiFi.begin(SSID, PASSWORD);
    Serial.printf("WiFi connecting to %s\n", SSID);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(400);
    }

    // Setup pins
    servo.attach(servoPin);
    pinMode(trigPinServo, OUTPUT);
    pinMode(echoPinServo, INPUT);
    pinMode(trigPinNotif, OUTPUT);
    pinMode(echoPinNotif, INPUT);
    pinMode(led, OUTPUT);

    servo.write(0);  // Start with the cap closed
    delay(100);
    servo.detach();  // Detach servo for power saving
    
    LINE.setToken(LINE_TOKEN); // Set LINE token
}

// Function to measure distance for Servo (Bin lid control)
void measureServo() {
    digitalWrite(trigPinServo, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPinServo, HIGH);
    delayMicroseconds(15);
    digitalWrite(trigPinServo, LOW);

    pinMode(echoPinServo, INPUT);
    durationServo = pulseIn(echoPinServo, HIGH);
    distServo = (durationServo / 2) / 29.1; // Convert time to distance in cm
}

// Function to measure distance for Notifications (Trash level monitoring)
void measureNotif() {
    digitalWrite(trigPinNotif, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPinNotif, HIGH);
    delayMicroseconds(15);
    digitalWrite(trigPinNotif, LOW);

    pinMode(echoPinNotif, INPUT);
    durationNotif = pulseIn(echoPinNotif, HIGH);
    distNotif = (durationNotif / 2) / 29.1; // Convert time to distance in cm
}

void loop() {
    // --- Servo Control (Ultrasonic Sensor 1) ---
    for (int i = 0; i < 3; i++) {  // Measure distance for the servo and average the values
        measureServo();
        averServo[i] = distServo;
        delay(10);
    }
    distServo = (averServo[0] + averServo[1] + averServo[2]) / 3;  // Average the readings

    Serial.print("Distance to trigger servo: ");
    Serial.print(distServo);
    Serial.println(" cm");

    // Servo control: If distance is less than 25 cm, open the lid
    if (distServo < 25) {
        servo.attach(servoPin);      // Attach servo
        servo.write(0);              // Open the lid
        delay(4000);                 // Keep it open for 4 seconds
        servo.write(180);            // Close the lid
        delay(1000);                 // Wait 1 second
        servo.detach();              // Detach servo
    }

    // --- Notification Control (Ultrasonic Sensor 2) ---
    measureNotif();  // Measure distance for notification

    Serial.print("Distance to trigger notification: ");
    Serial.print(distNotif);
    Serial.println(" cm");

    // Notification logic: Send LINE Notify when distance is less than 5 cm
    if (distNotif <= 5 && !notificationSent) {
        LINE.notify("Trash bin almost full, distance: " + String(distNotif) + " cm");
        notificationSent = true;
        digitalWrite(trigPinNotif, LOW);  // Turn off sensor (optional)
    }

    // Reset notification if distance increases
    if (distNotif > 5 && notificationSent) {
        notificationSent = false;
        digitalWrite(trigPinNotif, HIGH);  // Turn sensor back on
    }

    delay(1000); // Optional delay for stability
}
