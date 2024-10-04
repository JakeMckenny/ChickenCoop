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

volatile bool input_ready = false;
volatile char buffer [100] = {};
volatile unsigned int ind = 0; // initial value => buffer is empty
char direction;


int main(){
    stdio_init_all();
    initialize_stepper_motor();
    initialize_uart(); 
    // NOTE: Using this 'initialize_uart()' function sets up the interrupt handler so that it is 
    // automatically called when a character is detected. The interesting thing is that we don't 
    // even need to declare the interrupt handler in the main file at all. It now recognises to 
    // use the interrupt handler function when a character is recieved over UART.

    while (true){
        while (!input_ready){
        }    

        // Code that runs when character is detected over UART:
        if (sscanf((char *) buffer, "%c", &direction) == 1){ // (Could make all this a fucntion. would need to #include "drivers/motor_pins/motor_pins.h" in uart.h though, since we are using those move functions. Would also need to make 'direction' a global variable.)
            switch (direction){
                case 'w': // Move forward.
                move_forward();
                printf("MOVING FORWARD...\r\n");
                break;

                case 'x': // Move backward.
                move_backward();
                printf("MOVING BACKWARD...\r\n");
                break;

                case 'a': // Turn left.
                turn_left();
                printf("TURNING LEFT...\r\n");
                break;

                case 'd': // Turn right.
                turn_right();
                printf("TURNING RIGHT...\r\n");
                break;

                case 's': // Stop.
                stop_motors();
                printf("STOP...\r\n");
                break;

                // If a key is pressed that isn't defined, then display an error.
                default:
                printf("ERROR: Invalid key.\r\n");
            }
        }

        // Empty the buffer (Could make all this a function called 'reset_interrupt_handler')
        for (size_t i = 0; i < 100; i++){
            buffer[i] = '\000';
        }

        input_ready = false;  // This will put the pico back in low power mode. ie: it will go back into the 'while (!input_ready)' loop until another character is recieved over uart.
        ind = 0;              // Set the index for the buffer back to zero.
    }
}