#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <string.h>
#include "hardware/irq.h"
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/motor_pins/motor_pins.h"

uint16_t STEP_LEVEL = 32750;
uint16_t WRAP_LEVEL = 65500;
uint STOP_LEVEL = 0;

// Define the variables for the PWM slices.
uint slice_num_STEP1;
uint slice_num_STEP2;

// Function to initialize the steppers and PWM.
void initialize_stepper_motor(){
    // Initialize GPIO pins to their respective functionalities. 
    gpio_init(nENBL);
    gpio_set_dir(nENBL, GPIO_OUT);

    gpio_init(MODE0);
    gpio_set_dir(MODE0, GPIO_OUT);

    gpio_init(MODE1);
    gpio_set_dir(MODE1, GPIO_OUT);

    gpio_init(MODE2);
    gpio_set_dir(MODE2, GPIO_OUT);

    gpio_init(nSLEEP);
    gpio_set_dir(nSLEEP, GPIO_OUT);

    gpio_init(nRESET);
    gpio_set_dir(nRESET, GPIO_OUT);

    gpio_init(DIR2);
    gpio_set_dir(DIR2, GPIO_OUT);

    gpio_init(DIR1);
    gpio_set_dir(DIR1, GPIO_OUT);

    // Set the step pins as PWM.
    gpio_set_function(STEP1, GPIO_FUNC_PWM);
    gpio_set_function(STEP2, GPIO_FUNC_PWM);

    // Get slice number for each stepper motor.
    slice_num_STEP1 = pwm_gpio_to_slice_num(STEP1);
    slice_num_STEP2 = pwm_gpio_to_slice_num(STEP2);

    // Set the PWM wrap (period) for the stepper motors.
    pwm_set_wrap(slice_num_STEP1, WRAP_LEVEL);
    pwm_set_wrap(slice_num_STEP2, WRAP_LEVEL);

    // Set PWM channels for steppers to OFF initially.
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);

    // Enable the PWM for the steppers.
    pwm_set_enabled(slice_num_STEP1, true);
    pwm_set_enabled(slice_num_STEP2, true);

    // Set level of reset, sleep, and enable pins.
    gpio_put(nRESET, 1);
    gpio_put(nSLEEP, 1);
    gpio_put(nENBL, 0);

    // Set stepper mode pins for 1/32 microstepping.
    gpio_put(MODE0, 1);
    gpio_put(MODE1, 1);
    gpio_put(MODE2, 1);
}

// Function to move backward.
void move_backward(){
    // Set directions for backwards movement.
    gpio_put(DIR1, 0); // Backward direction for stepper 1.
    gpio_put(DIR2, 1); // Backward direction for stepper 2.

    // Enable PWM.
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
}

// Function to move forward.
void move_forward(){
    // Set directions for forward movement
    gpio_put(DIR1, 1); // Forward direction for stepper 1.
    gpio_put(DIR2, 0); // Forward direction for stepper 2.

    // Enable PWM.
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
}

// Function to turn left.
void turn_left(){
    // Set directions for left turn.
    gpio_put(DIR1, 1); // Stepper 1 moves backward.
    gpio_put(DIR2, 1); // Stepper 2 moves forward.

    // Enable PWM.
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
}

// Function to turn right.
void turn_right(){
    // Set directions for right turn.
    gpio_put(DIR1, 0); // Stepper 1 moves forward.
    gpio_put(DIR2, 0); // Stepper 2 moves backward.

    // Enable PWM.
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
}

// Function to stop movement of motors.
void stop_motors() {
    // Disable PWM by setting the step level to 0.
    pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
    pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
}