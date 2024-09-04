#include "pico/stdlib.h"
#include <stdio.h>
#include "rp2040.h"

// #define TRIG_PIN 2   // GPIO pin connected to TRIG of HC-SR04
// #define ECHO_PIN 3   // GPIO pin connected to ECHO of HC-SR04

void init_ultrasonic_sensor() {
    // Initialize TRIG as output
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    
    // Initialize ECHO as input
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

float get_distance_cm() {
    // Ensure TRIG is low initially
    gpio_put(TRIG_PIN, 0);
    sleep_ms(2);

    // Trigger the pulse (10us HIGH)
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    // Wait for the ECHO to go HIGH and measure how long it's HIGH
    while (gpio_get(ECHO_PIN) == 0) {
        // Wait for ECHO to go HIGH
    }
    uint64_t start_time = time_us_64();  // Record start time

    while (gpio_get(ECHO_PIN) == 1) {
        // Wait for ECHO to go LOW
    }
    uint64_t end_time = time_us_64();  // Record end time

    // Calculate duration of the pulse
    uint64_t duration = end_time - start_time;

    // Calculate distance in cm (using the formula)
    float distance = (float)duration / 58.0;

    return distance;
}


//This goes in the main
// int main() {
//     stdio_init_all();
//     init_ultrasonic_sensor();

//     while (1) {
//         float distance = get_distance_cm();
//         printf("Distance: %.2f cm\n", distance);
//         sleep_ms(1000);  // Wait 1 second before the next reading
//     }

//     return 0;
// }
