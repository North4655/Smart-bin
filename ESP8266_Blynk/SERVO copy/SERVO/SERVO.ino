// Pin Definitions
const int mq135Pin = A0;  // Analog input pin for MQ-135

void setup() {
  // Initialize serial communication at 115200 baud rate for debugging
  Serial.begin(115200);
  
  // Setup the MQ-135 pin as input
  pinMode(mq135Pin, INPUT);
}

void loop() {
  // Read the analog value from MQ-135
  int mq135Value = analogRead(mq135Pin);
  
  // Print the value to the serial monitor
  Serial.print("MQ-135 Sensor Value: ");
  Serial.println(mq135Value);
  
  // Simple delay for readability (2 seconds)
  delay(2000);
}
