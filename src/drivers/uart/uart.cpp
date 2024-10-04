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
#include "drivers/uart/uart.h"

// Values for UART communication:
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// Function for RX interrupt handler.
void on_uart_rx(void) {
    while (uart_is_readable(UART_ID)) {
      char ch = uart_getc(UART_ID);
      if (uart_is_writable(UART_ID)){
            buffer[ind] = ch; // save the character
            // Index can't have more than 99 characters (100th character needs to be the trailing null)
            if (ind <= 98){
                if (ch == 0x7f){         // 0x7f is the backspace character. This first part of the if statement allows the backspace to clear the last character entered.
                    buffer[ind] = '\000';
                    ind--;
                    buffer[ind] = '\000';
                }
                else{
                    ind++; // increment the index
                }
                //uart_putc(UART_ID, ch); // Displays typed character in putty
            }
            else{
                ind = ind - 1; // decrease index back to 98 if index reaches 99
                printf("\r\nWARNING: Buffer character limit reached\r\n");
            }

            buffer[ind] = 0; // add a trailing NULL
            printf ("\r\n"); // goes to a new line
            input_ready = true;
        }
    }
}

// Function to initialize UART communication.
void initialize_uart(){
    // Set up our UART with a basic baud rate.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);
}