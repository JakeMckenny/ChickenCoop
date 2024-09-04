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
        move_backward();
        sleep_ms(2000);
        stop_motors();
        sleep_ms(2000);
        move_forward();
        sleep_ms(2000);
        stop_motors();
        sleep_ms(2000);
        turn_left();
        sleep_ms(2000);
        turn_right();
    }
    return 0;
}