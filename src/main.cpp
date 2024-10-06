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
    // use the interrupt handler function from the driver file when a character is recieved over UART.

    while (true){
        while (!input_ready){}    
        movement_command();
        reset_buffer();
    }
}