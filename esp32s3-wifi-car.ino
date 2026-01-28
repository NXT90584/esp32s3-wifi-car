#include <WiFi.h>
#include <WebServer.h>

// WiFi AP credentials
const char* ssid = "ESP32-AP";
const char* password = "12345678";

// Create web server on port 80
WebServer server(80);

// L298N motor driver pins
const int ENA = 26;  // Left motor PWM (speed)
const int IN1 = 27;  // Left motor direction 1
const int IN2 = 25;  // Left motor direction 2
const int ENB = 14;  // Right motor PWM (speed)
const int IN3 = 12;  // Right motor direction 1
const int IN4 = 13;  // Right motor direction 2

// Variables to store command and speed
String currentCommand = "";
int currentSpeed = 0;
const int defaultSpeed = 100; // Default speed when no value is provided

void setup() {
  // Start Serial
  Serial.begin(115200);
  delay(1000);
  
  // Set up motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Set up Access Point
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // Define server routes
  server.onNotFound(handleCommand);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleCommand() {
  String request = server.uri();
  if (request.startsWith("/")) {
    request = request.substring(1);
  }
  
  // Parse command and speed (if provided)
  if (request.indexOf(":") != -1) {
    int colonIndex = request.indexOf(":");
    currentCommand = request.substring(0, colonIndex);
    String speedStr = request.substring(colonIndex + 1);
    currentSpeed = speedStr.toInt();
  } else {
    currentCommand = request;
    // Only assign default speed to movement commands (F, B, L, R)
    // S, X, Y, Z should have speed = 0
    if (currentCommand == "F" || currentCommand == "B" || 
        currentCommand == "L" || currentCommand == "R") {
      currentSpeed = defaultSpeed;
    } else {
      currentSpeed = 0; // For S, X, Y, Z and any other special commands
    }
  }
  
  // Process command and control robot
  controlRobot();
  displayCommand();
  
  // Send response back to client
  server.send(200, "text/plain", "Command received: " + request);
}

void controlRobot() {
  // Stop motors by default
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  
  if (currentCommand == "F") { // Forward
    digitalWrite(IN1, HIGH);  // Left motor forward
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);  // Right motor forward
    digitalWrite(IN4, LOW);
    analogWrite(ENA, currentSpeed);
    analogWrite(ENB, currentSpeed);
  } 
  else if (currentCommand == "B") { // Backward
    digitalWrite(IN1, LOW);   // Left motor backward
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);   // Right motor backward
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, currentSpeed);
    analogWrite(ENB, currentSpeed);
  } 
  else if (currentCommand == "L") { // Left
    digitalWrite(IN1, LOW);   // Left motor backward
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);  // Right motor forward
    digitalWrite(IN4, LOW);
    analogWrite(ENA, currentSpeed);
    analogWrite(ENB, currentSpeed);
  } 
  else if (currentCommand == "R") { // Right
    digitalWrite(IN1, HIGH);  // Left motor forward
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);   // Right motor backward
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, currentSpeed);
    analogWrite(ENB, currentSpeed);
  } 
  else if (currentCommand == "S") { // Stop
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
  }
  else if (currentCommand == "X") { // Custom action X
    // Add your custom action for X here
    Serial.println("Custom Action X triggered");
  }
  else if (currentCommand == "Y") { // Custom action Y
    // Add your custom action for Y here
    Serial.println("Custom Action Y triggered");
  }
  else if (currentCommand == "Z") { // Custom action Z
    // Add your custom action for Z here
    Serial.println("Custom Action Z triggered");
  }
}

void displayCommand() {
  Serial.println("-------------------");
  Serial.print("Received Command: ");
  Serial.println(currentCommand);
  
  // Only display speed for commands that use it
  if (currentSpeed > 0 && (currentCommand == "F" || currentCommand == "B" || 
                           currentCommand == "L" || currentCommand == "R")) {
    Serial.print("Speed Value: ");
    Serial.println(currentSpeed);
  }
  
  if (currentCommand == "F") {
    Serial.println("Action: Moving Forward");
  } else if (currentCommand == "B") {
    Serial.println("Action: Moving Backward");
  } else if (currentCommand == "L") {
    Serial.println("Action: Turning Left");
  } else if (currentCommand == "R") {
    Serial.println("Action: Turning Right");
  } else if (currentCommand == "S") {
    Serial.println("Action: Stop");
  } else if (currentCommand == "X") {
    Serial.println("Action: Custom X");
  } else if (currentCommand == "Y") {
    Serial.println("Action: Custom Y");
  } else if (currentCommand == "Z") {
    Serial.println("Action: Custom Z");
  }
  Serial.println("-------------------");
}
