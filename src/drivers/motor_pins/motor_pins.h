// Declare external global variables for PWM slice numbers
extern uint slice_num_STEP1;
extern uint slice_num_STEP2;
extern uint slice_num_STEP3;

void initialize_stepper_motor();

void move_forward();

void move_backward();

void turn_right();

void turn_left();

void stop_motors();