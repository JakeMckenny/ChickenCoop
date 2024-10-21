#define Accel_SCL_SCLK 2
#define Accel_SDA_MOSI 3
#define Accel_MISO 4
#define Accel_CS 5
#define Accel_Int1 6
#define Accel_Int2 7
#define UART1_TX 8
#define UART1_RX 9
#define UART1_CTS 10
#define UART1_RTS 11
#define RESET 12
#define TRIG 13
#define ECHO3V3 14
#define STEP2 15 // Movement for stepper 2.
#define DIR2 18  // Direction for stepper 2.
#define nENBL 19 // Enables/disables stepper motors.
#define DIR1 20  // Direction for stepper 1.
#define DECAY 21  // CURRENTLY UNUSED.
#define nSLEEP 22 // Enables/disables sleep mode of stepper motors.
#define nRESET 23
#define MODE2 24
#define MODE1 25
#define MODE0 26 // MODE0, MODE1, and MODE2 control the step size of the stepper motors.
#define STEP1 27 // Movement for stepper 1.

// Declare external variables for PWM slice numbers.
extern uint slice_num_STEP1;
extern uint slice_num_STEP2;

void initialize_stepper_motor();

void move_forward();

void move_backward();

void turn_right();

void turn_left();

void stop_motors();