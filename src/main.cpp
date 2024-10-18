// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/gpio.h"
// #include "hardware/pio.h"
// #include "hardware/pwm.h"
// #include "hardware/adc.h"
// #include <stdio.h>
// #include <string.h>
// #include "hardware/irq.h"
// #include "rp2040.h"
// #include "WS2812.pio.h"
// #include "drivers/logging/logging.h"
// #include "drivers/motor_pins/motor_pins.h"
// #include "drivers/uart/uart.h"

// volatile bool input_ready = false;
// volatile char buffer [100] = {};
// volatile unsigned int ind = 0; // initial value => buffer is empty
// char direction;

// int main(){
//     stdio_init_all();
//     initialize_stepper_motor();
//     initialize_uart();
//     // NOTE: Using this 'initialize_uart()' function sets up the interrupt handler so that it is
//     // automatically called when a character is detected. The interesting thing is that we don't
//     // even need to declare the interrupt handler in the main file at all. It now recognises to
//     // use the interrupt handler function from the driver file when a character is recieved over UART.

//     while (true){
//         while (!input_ready){}
//         movement_command();
//         reset_buffer();
//     }
// }

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <string.h>
#include "hardware/irq.h"
#include "rp2040.h"
#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/motor_pins/motor_pins.h"
#include "drivers/uart/uart.h"
#include <iostream>

using namespace std;
#define TRIG 13
#define ECHO3V3 14
#define DISTANCE_THRESHOLD 16 // Distance threshold in centimeters

volatile uint32_t rise_time = 0;
volatile uint32_t fall_time = 0;
volatile bool trig_pulse_complete = false;
float distance_cm = DISTANCE_THRESHOLD + 1; // Initialise distance_cm so that it is greater than the threshold value.

void gpio_callback(uint gpio, uint32_t events){
    if (gpio == ECHO3V3)
    { // Change to EchoPin for the callback
        if (events & GPIO_IRQ_EDGE_RISE) {
            rise_time = time_us_32();
        }

        if (events & GPIO_IRQ_EDGE_FALL)
        {

            fall_time = time_us_32();

            trig_pulse_complete = true; // Signal that the pulse is complete
        }
    }
}

void measure_trig_pulse_duration()
{
    gpio_init(TRIG);
    gpio_set_dir(TRIG, GPIO_OUT);
    gpio_put(TRIG, 0);
    sleep_ms(2); // Ensure a stable state
    gpio_put(TRIG, 1);
    sleep_us(10); // 10 microseconds pulse
    gpio_put(TRIG, 0);
    // Set up the EchoPin as input
    gpio_init(ECHO3V3);
    gpio_set_dir(ECHO3V3, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO3V3, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    while (!trig_pulse_complete)
    {
        tight_loop_contents(); // Wait for the pulse to complete
    }
    trig_pulse_complete = false; // Reset for the next measurement
}

volatile bool input_ready = false;
volatile char buffer[100] = {};
volatile unsigned int ind = 0; // initial value => buffer is empty
char direction;

int main(){
    stdio_init_all();
    initialize_stepper_motor();
    initialize_uart();
    // NOTE: Using this 'initialize_uart()' function sets up the interrupt handler so that it is
    // automatically called when a character is detected. The interesting thing is that we don't
    // even need to declare the interrupt handler in the main file at all. It now recognises to
    // use the interrupt handler function from the driver file when a character is recieved over UART.

    while (true){
        while (!input_ready){
            measure_trig_pulse_duration();
            sleep_ms(100); // Wait a bit before taking the next measurement
            // Calculate distance after the pulse is complete
            uint32_t time_diff_us = fall_time - rise_time;     // Calculate time difference
            distance_cm = (time_diff_us / 2.0) * 0.0343; // Distance in cm
            // Control the LED based on distance
            if (distance_cm < DISTANCE_THRESHOLD){
                stop_motors();
            }
            else{}
            sleep_ms(500); // Adjust timing as needed
        }
        if (distance_cm >= DISTANCE_THRESHOLD){
            movement_command();
            reset_buffer();
        }
    }
}