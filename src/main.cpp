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
int main()
{
    stdio_init_all();
    initialize_stepper_motor();
    for (;;)
    {
            for (int i = 0; i < 5; i++) { 
      // First loop (right loop)
        move_forward();  // Move forward
        sleep_ms(10000);   // Adjust timing for forward movement
        turn_right();    // Make a pronounced right turn
        sleep_ms(3000);   // Increase sleep time to make a sharp turn
            }
}
    return 0;
}