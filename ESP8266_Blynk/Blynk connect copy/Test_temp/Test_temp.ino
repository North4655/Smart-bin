#define BLYNK_TEMPLATE_ID "TMPL6xSvMRz97"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "duiVCPLR_UUnxUvQj-X0Pkw88XG47KNY"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
Servo servo;



char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "North";
char pass[] = "11111111";




void setup() {
  Serial.begin(9600);
 
  Serial.println("Starting");

  // connect to the WiFi
  Blynk.begin(auth, ssid, pass);
  servo.attach(D8);
 
  }



void loop() 
   // start blynk
{
  Blynk.run();
}

BLYNK_WRITE(V0)

{
   servo.write(param.asInt());
}
BLYNK_WRITE(V1)

{
   servo.write(0); 
}
BLYNK_WRITE(V2)

{
   servo.write(180);
}