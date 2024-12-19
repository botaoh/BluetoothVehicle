// Include necessary libraries
#include "mbed.h"
#include "Servo.h"
#include "Motor.h"
#include "uLCD_4DGL.h"

// Set up communication with Raspberry Pi using USB
RawSerial  pi(USBTX, USBRX);

// Initialize the uLCD display
uLCD_4DGL uLCD(p28, p27, p29); // serial tx, serial rx, reset pin

// Ultrasonic sensor pins
DigitalOut trigger(p6); // Trigger pin for sonar
DigitalIn echo(p7);     // Echo pin for sonar

// Variable to store sonar distance
int RadarReturnDistance = 0;

// Initialize left and right motors (PWM, forward, reverse pins)
Motor DLMotor(p25, p13, p12); // Left motor
Motor DRMotor(p26, p10, p9);  // Right motor

// LED indicators for motor movement
DigitalOut bit0(LED1); // Forward indicator
DigitalOut bit1(LED2); // Reverse indicator
DigitalOut bit2(LED3); // Left turn indicator
DigitalOut bit3(LED4); // Right turn indicator

// Red LED for obstacle warning
DigitalOut red(p20); 

// Colors (not used in this code, but defined)
int color[3] = {RED, GREEN, BLUE};

// Variable to store control mode
int mode = 0;

// Function to handle data received from Raspberry Pi
void dev_recv() {
    char temp = 0;
    while (pi.readable()) { // Check if data is available
        temp = pi.getc();  // Read data
        pi.putc(temp);     // Echo data back
        // Set mode based on received data
        if (temp == 'w') {
            mode = 1; // Move forward
        } else if (temp == 'a') {
            mode = 2; // Turn left
        } else if (temp == 's') {
            mode = 3; // Move backward
        } else if (temp == 'd') {
            mode = 4; // Turn right
        } else if (temp == 'x') {
            mode = 6; // (Unused mode)
        } else if (temp == 'y') {
            mode = 7; // (Unused mode)
        } else if (temp == 'b') {
            mode = 8; // (Unused mode)
        } else {
            mode = 0; // Stop
        }
    }
}

// Control variables
double unit = (3.0 - 0.0) / 16; 
float constSpeed = 0.8;         // Motor speed

// Function to turn off all LED indicators
void ledClear() {
    bit0 = 0;
    bit1 = 0;
    bit2 = 0;
    bit3 = 0;
}

// Delay function (100ms)
void sleep_unit() {
    wait_us(100000);
}

// Function to move forward
void moveForward() {
    ledClear();
    bit0 = 1; // Turn on forward indicator
    DLMotor.speed(constSpeed);  
    DRMotor.speed(constSpeed);   
}

// Function to move backward
void moveInverse() {
    ledClear();
    bit1 = 1; // Turn on reverse indicator
    DLMotor.speed(-constSpeed);  
    DRMotor.speed(-constSpeed);  
}

// Function to turn left
void moveLeft() {
    ledClear();
    bit2 = 1; // Turn on left indicator
    DLMotor.speed(-constSpeed); 
    DRMotor.speed(constSpeed);   
}

// Function to turn right
void moveRight() {
    ledClear();
    bit3 = 1; // Turn on right indicator
    DLMotor.speed(constSpeed);   
    DRMotor.speed(-constSpeed); 
}

// Function to stop movement
void stopMove() {
    ledClear();
    DLMotor.speed(0); 
    DRMotor.speed(0); 
}

// Function to display radar distance on uLCD
void LCD_Show() {
    uLCD.cls(); // Clear the display
    int len = snprintf(NULL, 0, "%d", RadarReturnDistance); // Get length of number string
    char *result = (char*)malloc(len + 1); // Allocate memory for string
    snprintf(result, len + 1, "%d", RadarReturnDistance); // Convert number to string
    uLCD.puts(result); // Display string
}

// Variables and timer for ultrasonic sensor
int objectDistance = 0;
int correction = 0;
Timer sonar;

// Function to initialize ultrasonic sensor
void sonor_init() {
    sonar.reset(); // Reset the timer
    sonar.start(); // Start the timer
    while (echo == 2) {}; // Wait for echo pin to stabilize
    sonar.stop(); // Stop the timer
    correction = sonar.read_us(); // Measure software overhead
    printf("Approximate software overhead timer delay is %d uS\n\r", correction);
}

// Function to measure distance using ultrasonic sensor
void sonor_thread() {
    trigger = 1;  // Send trigger signal
    sonar.reset();
    wait_us(10.0);
    trigger = 0;  // Stop trigger signal
    while (echo == 0) {}; // Wait for echo signal to go high
    sonar.start(); // Start the timer
    while (echo == 1) {}; // Wait for echo signal to go low
    sonar.stop(); // Stop the timer
    // Calculate distance in inches
    objectDistance = (sonar.read_us() - correction) / 148.0;
    printf(" %d inch \n\r", objectDistance);
    wait_ms(200); // Wait before next measurement
    RadarReturnDistance = objectDistance; // Update global distance variable

    // Turn on red LED if object is too close
    if (objectDistance < 3) {
        red = 1;
    } else {
        red = 0;
    }
}

// Main function
int main() {  
    pi.baud(9600); // Set baud rate for Raspberry Pi
    pi.attach(&dev_recv, Serial::RxIrq); // Attach interrupt for receiving data
    sonor_init(); // Initialize ultrasonic sensor
    sleep();

    // Main loop
    while (true) { 
        // Perform actions based on mode
        switch (mode) {
            case 1: // Move forward
                if (objectDistance < 3) { // Stop if obstacle is too close
                    stopMove();
                } else {
                    moveForward();
                }
                break;
            case 2: // Turn left
                moveLeft();
                break;
            case 3: // Move backward
                moveInverse();
                break;
            case 4: // Turn right
                moveRight();
                break;
            case 0: // Stop
                stopMove();
                break;
        }
        LCD_Show();    // Update LCD display
        sonor_thread(); // Update sonar distance
    }
}