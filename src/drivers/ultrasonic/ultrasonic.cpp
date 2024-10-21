#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include <string.h>
#include "hardware/irq.h"
#include "WS2812.pio.h" 
#include "drivers/logging/logging.h"
#include "drivers/motor_pins/motor_pins.h"
#include "drivers/uart/uart.h"
#include "drivers/ultrasonic/ultrasonic.h"
#include <pico/time.h>

// Function to measure the distance from the ultrasonic sensor.
void measure_distance(uint gpio, uint32_t events){
    if (gpio == ECHO3V3){ // Change to EchoPin for the callback
        if (events & GPIO_IRQ_EDGE_RISE){
            rise_time = time_us_32();
        }

        if (events & GPIO_IRQ_EDGE_FALL){
            fall_time = time_us_32();
            trig_pulse_complete = true; // Signal that the pulse is complete

            uint32_t time_diff_us = fall_time - rise_time; // Calculate time difference.
            distance_cm = (time_diff_us / 2.0) * 0.0343; // Distance in cm.
            
            // Print the distance to the terminal.
            printf ("Distance: %f\r\n", distance_cm);
            // Send the distance over UART.
            char distance_buffer[50];  // Create a buffer to store the distance value as a string.
            sprintf(distance_buffer, "Distance: %f\r\n", distance_cm);  // Store the distance measurement into the buffer.
            uart_puts(UART_ID, distance_buffer);  // Send the string over UART.
        }
    }
}

// Function to initialize the ultrasonic sensor.
void initialize_ultrasonic(){
    gpio_init(TRIG);
    gpio_set_dir(TRIG, GPIO_OUT);
    gpio_init(ECHO3V3);
    gpio_set_dir(ECHO3V3, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO3V3, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &measure_distance);
}

// Function to send a trigger pulse to the ultrasonic sensor.
void measure_trig_pulse_duration(){
    gpio_put(TRIG, 0);
    sleep_ms(2); // Ensure a stable state
    gpio_put(TRIG, 1);
    sleep_us(10); // 10 microseconds pulse
    gpio_put(TRIG, 0);

    while (!trig_pulse_complete){
        tight_loop_contents(); // Wait for the pulse to complete
    }
    trig_pulse_complete = false; // Reset for the next measurement
}