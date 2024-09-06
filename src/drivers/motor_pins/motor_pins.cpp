#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <string.h>
#include "hardware/irq.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/motor_pins/motor_pins.h"
#include "rp2040.h"

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

 uint16_t STEP_LEVEL = 32750;
 uint16_t WRAP_LEVEL = 65500;
 uint STOP_LEVEL = 0;

// Define the global variables for the PWM slices here
uint slice_num_STEP1;  // Define these variables
uint slice_num_STEP2;
uint slice_num_STEP3;

// Function to initialize the steppers and PWM
void initialize_stepper_motor()
{
    // Initialize GPIO for directions
    gpio_init(DIR3);
    gpio_set_dir(DIR3, GPIO_OUT);

    gpio_init(nENBL);
    gpio_set_dir(nENBL, GPIO_OUT);

    gpio_init(MODE0);
    gpio_set_dir(MODE0, GPIO_OUT);

    gpio_init(MODE1);
    gpio_set_dir(MODE1, GPIO_OUT);

    gpio_init(MODE2);
    gpio_set_dir(MODE2, GPIO_OUT);

    gpio_init(nHOME);
    gpio_set_dir(nHOME, GPIO_IN);
    gpio_pull_up(nHOME);

    gpio_init(nFAULT);
    gpio_set_dir(nFAULT, GPIO_IN);
    gpio_pull_up(nFAULT);

    gpio_init(nSLEEP);
    gpio_set_dir(nSLEEP, GPIO_OUT);

    gpio_init(nRESET);
    gpio_set_dir(nRESET, GPIO_OUT);

    gpio_init(DIR2);
    gpio_set_dir(DIR2, GPIO_OUT);

    gpio_init(DIR1);
    gpio_set_dir(DIR1, GPIO_OUT);

    // Set the step pins as PWM
    gpio_set_function(STEP1, GPIO_FUNC_PWM);
    gpio_set_function(STEP2, GPIO_FUNC_PWM);
    gpio_set_function(STEP3, GPIO_FUNC_PWM);

    // Get slice number for each stepper motor
    slice_num_STEP1 = pwm_gpio_to_slice_num(STEP1);
    slice_num_STEP2 = pwm_gpio_to_slice_num(STEP2);
    slice_num_STEP3 = pwm_gpio_to_slice_num(STEP3);

    // Set the PWM wrap (period) for the stepper motors
    pwm_set_wrap(slice_num_STEP1, WRAP_LEVEL);
    pwm_set_wrap(slice_num_STEP2, WRAP_LEVEL);
    pwm_set_wrap(slice_num_STEP3, WRAP_LEVEL);

    // Set PWM channels for steppers to OFF initially
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
    pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);

    // Enable the PWM for the steppers
    pwm_set_enabled(slice_num_STEP1, true);
    pwm_set_enabled(slice_num_STEP2, true);
    pwm_set_enabled(slice_num_STEP3, true);
    // Set level of reset, sleep, and enable pins:
    gpio_put(nRESET, 1);
    gpio_put(nSLEEP, 1);
    gpio_put(nENBL, 0);

    // Set stepper mode pins for 1/32 microstepping:
    gpio_put(MODE0, 1);
    gpio_put(MODE1, 1);
    gpio_put(MODE2, 1);
}

void move_backward()
{
    // Set directions for backwards movement
    gpio_put(DIR1, 1); // Forward direction for Y-axis stepper
    gpio_put(DIR2, 0); // Forward direction for X-axis stepper

    // Set the step level for each stepper motor to move forward
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
}

// Function to move forward (no inputs needed)
void move_forward()
{
    // Set directions for forward movement
    gpio_put(DIR1, 0); // Forward direction for Y-axis stepper
    gpio_put(DIR2, 1); // Forward direction for X-axis stepper

    // Set the step level for each stepper motor to move forward
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
}

void turn_left() {
    // Set directions for left turn
    gpio_put(DIR1, 1); // Y-axis stepper (DIR1) moves backward
    gpio_put(DIR2, 1); // X-axis stepper (DIR2) moves forward

    // Set the step level for each stepper motor to turn left
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
}

void turn_right() {
    // Set directions for right turn
    gpio_put(DIR1, 0); // Y-axis stepper (DIR1) moves forward
    gpio_put(DIR2, 0); // X-axis stepper (DIR2) moves backward

    // Set the step level for each stepper motor to turn right
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
}

void stop_motors() {
    // Stop all stepper motors by setting the step level to 0
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
    pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
}