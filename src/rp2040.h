// Define the GPIO pins for the motors
#define DIR1 22  // Direction for Y-axis stepper
#define DIR2 20  // Direction for X-axis stepper
#define DIR3 9   // Direction for Z-axis stepper
#define STEP1 21 // Movement for Y-axis stepper
#define STEP2 19 // Movement for X-axis stepper
#define STEP3 11 // Movement for Z-axis stepper
#define MODE0 12 // MODE0, MODE1, and MODE2 control the step size of the stepper motors
#define MODE1 13
#define MODE2 14
#define nHOME 15  // CURRENTLY UNUSED
#define nFAULT 16 // CURRENTLY UNUSED
#define nSLEEP 17 // Enables/disables sleep mode of all 3 stepper motors
#define nRESET 18
#define DECAY 8  // CURRENTLY UNUSED
#define DIR3 9   // Controls direction of stepper 3
#define nENBL 10 // Enables/disables all 3 stepper motors

// This is fo ultrasonic sensor
// #define TRIG_PIN (find pin)   // GPIO pin connected to TRIG of HC-SR04
// #define ECHO_PIN  (find pin)  // GPIO pin connected to ECHO of HC-SR04