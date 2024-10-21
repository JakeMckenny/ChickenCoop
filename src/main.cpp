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
#include "drivers/uart/uart.h"
#include "drivers/ultrasonic/ultrasonic.h"
#include <iostream>
#include <pico/time.h>
#include "drivers/LIS3DH/LIS3DH.h"

// Define namespace.
using namespace std;

// Define variables.
volatile uint32_t rise_time = 0;
volatile uint32_t fall_time = 0;
volatile bool trig_pulse_complete = false;
float distance_cm = DISTANCE_THRESHOLD + 1; // Initialise distance_cm so that it is greater than the threshold value.
volatile bool input_ready = false;
volatile char buffer[100] = {};
volatile unsigned int ind = 0; // initial value => buffer is empty
char direction;
bool obstacle_flag = false;
absolute_time_t start_time, end_time;
char prev_command;
int16_t x;
int16_t y;
int16_t z;

int main(){
    stdio_init_all();
    initialize_stepper_motor();
    initialize_uart();
    initialize_bluetooth();
    initialize_ultrasonic();
    LIS3DH_init();

    // Define initial starting time.
    start_time = get_absolute_time();

    while (true){
        while (!input_ready){
            // Send data over UART if half a second has passed.
            end_time = get_absolute_time(); // Get current time.
            int64_t time_diff_ms = (absolute_time_diff_us(start_time, end_time)) * 0.001; // Determine time difference.
            if (time_diff_ms >= 500){
                measure_trig_pulse_duration(); // Send the ultrasonic sensor data over UART.
                send_accel_readings(); // Also send the accelerometer readings over UART.
                start_time = get_absolute_time(); // Reset timer.
            }
           
           // Stop movement of the coop if an object detected by the ultrasonic sensor is within the distance threshold.
           if (distance_cm < DISTANCE_THRESHOLD){
                stop_motors();
                if (obstacle_flag == false){
                    printf("OBSTACLE DETECTED!\r\n");
                }
                obstacle_flag = true;
            }
           
           // Once obstacle is cleared, continue with previous movement command.
           if (obstacle_flag == true && distance_cm >= DISTANCE_THRESHOLD){
                prev_movement_command();
                obstacle_flag = false;
            }
        }

        // Only interpret movement commands if there are no obstacles within the distance threshold of the ultrasonic sensor.
        if (distance_cm >= DISTANCE_THRESHOLD){
            movement_command();
        }

        // Reset buffer for UART communication.
        reset_buffer();
    }
}