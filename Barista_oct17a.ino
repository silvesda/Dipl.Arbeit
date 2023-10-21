#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Wire.h>
#include <Adafruit_MLX90614.h>

const int relayPin = 2; 
const int relayPin3 = 3; 
unsigned long lastToggleTime = 0;
unsigned long lastCoffeeTriggerTime = 0;
unsigned long lastSteamTriggerTime = 0;

//Ungültige Temp werte ignorieren
const int MIN_VALID_TEMP = 10;  // Min. Temp
const int MAX_VALID_TEMP = 180;  // Max. Temp 


// HC-SR04 Pins
const int trigPin = 6;
const int echoPin = 7;
long duration;
int distance;
unsigned long lastDistanceCheck = 0;
const long distanceInterval = 5000;
const int MAX_DISTANCE = 11;
const int MIN_DISTANCE = 1;

// Einschaltverzögerung für iRTEMP
const int triggerPin = 8;
const unsigned long delayTime = 10000;
bool pinTriggered = false;

bool relayIsActive = false; // Ein internes Flag, das den tatsächlichen Zustand des Relais verfolgt

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {

  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, HIGH);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(relayPin3, OUTPUT);
  digitalWrite(relayPin3, LOW);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  Serial.begin(9600);
  Wire.begin();
  mlx.begin();

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  if (!pinTriggered && millis() >= delayTime) {  
    digitalWrite(triggerPin, LOW);
    pinTriggered = true;
  }


  if (millis() - lastDistanceCheck > distanceInterval) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    Serial.print("Abstand: ");
    Serial.print(distance);
    Serial.println(" cm");

    waterPercentage = (1 - ((float)(distance - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE))) * 100;
    waterPercentage = constrain(waterPercentage, 0, 100);
    lastDistanceCheck = millis();
  }


    // Kaffebezug
    if (relayState && !relayIsActive) { // Wenn der Button gedrückt wird und das Relais nicht aktiv ist
        digitalWrite(relayPin, HIGH);
        lastToggleTime = millis();
        relayIsActive = true;
    } else if (relayState && relayIsActive) { // Wenn der Button erneut gedrückt wird und das Relais aktiv ist
        digitalWrite(relayPin, LOW);
        relayIsActive = false;
    }

    // Prüfe, ob die Relay-Duration erreicht ist
    if (relayIsActive && (millis() - lastToggleTime >= relayDuration * 1000) && digitalRead(relayPin) == HIGH) {
        digitalWrite(relayPin, LOW);
        relayIsActive = false;
    }

    // Reset relayState für das nächste Mal
    relayState = false;



  // Dampfsteuerung
  if (relaystate2) {
    digitalWrite(relayPin3, HIGH);
  } else {
    digitalWrite(relayPin3, LOW);
    
  }

//Ungültige Temp werte ignorieren

int temp = mlx.readObjectTempC();

if (temp >= MIN_VALID_TEMP && temp <= MAX_VALID_TEMP) {
    temperature = temp;  // Wert nur zuweisen, wenn er gültig ist
} else {
    // Ungültiger Temperaturwert:
    Serial.println("schrott I2C Bus");
}
}

void onExternalTemperatureValueChange() {
    // Implementierung (falls erforderlich)
}

void onRelayDurationChange() {
    // Implementierung (falls erforderlich)
}

void onRelayStateChange() {
    // Implementierung (falls erforderlich)
}

void onRelaystate2Change() {
    // Implementierung (falls erforderlich)
}
