#include <Arduino.h>
#include <ESP_FlexyStepper.h>
#include <BleMouse.h>
#include <ESPAsyncWebServer.h>
#include "WifiManager.hpp"

// Constants
const float SLOW_SPEED = 1.0;
const float FAST_SPEED = 5.0;
const float SENSITIVITY = 0.5;
const int MOTOR_1_STEP_PIN = 23;
const int MOTOR_1_DIR_PIN = 22;
const int MOTOR_2_STEP_PIN = 19;
const int MOTOR_2_DIR_PIN = 18;
const float MAX_ANGLE = 270.0;

// Global variables
ESP_FlexyStepper motor1;
ESP_FlexyStepper motor2;
BleMouse bleMouse;
AsyncWebServer server(80);
float motor1Angle = 0;
float motor2Angle = 0;
bool fastMode = false;

// Wifi Manager
WiFiManager wifiManager;

// Initialize motors
void setupMotors() {
    motor1.connectToPins(MOTOR_1_STEP_PIN, MOTOR_1_DIR_PIN);
    motor2.connectToPins(MOTOR_2_STEP_PIN, MOTOR_2_DIR_PIN);
    motor1.setSpeedInRevolutionsPerSecond(SLOW_SPEED);
    motor2.setSpeedInRevolutionsPerSecond(SLOW_SPEED);
    motor1.setStepsPerRevolution(200);
    motor2.setStepsPerRevolution(200);
}

// Test motors
void testMotors() {
    motor1.moveRelativeInRevolutions(0.25);
    delay(1000);
    motor1.moveRelativeInRevolutions(-0.5);
    delay(1000);
    motor1.moveRelativeInRevolutions(0.25);
    delay(1000);

    motor2.moveRelativeInRevolutions(0.25);
    delay(1000);
    motor2.moveRelativeInRevolutions(-0.5);
    delay(1000);
    motor2.moveRelativeInRevolutions(0.25);
    delay(1000);
}

// Handle mouse input
void handleMouseInput() {
    if (bleMouse.isPressed(BUTTON_LEFT) && bleMouse.isMoved()) {
        float deltaX = bleMouse.getX() * SENSITIVITY;
        float deltaY = bleMouse.getY() * SENSITIVITY;

        motor1Angle += deltaX;
        motor2Angle += deltaY;

        motor1Angle = constrain(motor1Angle, 0, MAX_ANGLE);
        motor2Angle = constrain(motor2Angle, 0, MAX_ANGLE);

        motor1.moveToPositionInRevolutions(motor1Angle / MAX_ANGLE);
        motor2.moveToPositionInRevolutions(motor2Angle / MAX_ANGLE);
    }

    if (bleMouse.isPressed(BUTTON_RIGHT)) {
        motor1.moveToPositionInRevolutions(0);
        motor2.moveToPositionInRevolutions(0);
        motor1Angle = 0;
        motor2Angle = 0;
    }

    if (bleMouse.isPressed(BUTTON_LEFT) && bleMouse.isPressed(BUTTON_RIGHT)) {
        fastMode = !fastMode;
        float speed = fastMode ? FAST_SPEED : SLOW_SPEED;
        motor1.setSpeedInRevolutionsPerSecond(speed);
        motor2.setSpeedInRevolutionsPerSecond(speed);
    }
}

// Setup web server
void setupWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = "<html><body>";
        html += "<h1>Motor Angles</h1>";
        html += "<p>Motor 1 Angle: " + String(motor1Angle) + "</p>";
        html += "<p>Motor 2 Angle: " + String(motor2Angle) + "</p>";
        html += "</body></html>";
        request->send(200, "text/html", html);
    });

    wifiManager.connect("your_SSID", "your_PASSWORD", 5000);
    server.begin();
}

void setup() {
    setupMotors();
    testMotors();
    bleMouse.begin();
    setupWebServer();
}

void loop() {
    handleMouseInput();
    motor1.processMovement();
    motor2.processMovement();
}
