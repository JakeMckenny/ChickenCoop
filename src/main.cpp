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

// Values for UART communication:
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

static int chars_rxed = 0;
volatile bool input_ready = false;

volatile char buffer [100] = {};
volatile unsigned int ind = 0; // initial value => buffer is empty

char direction;
char sleep_status[10];

// RX interrupt handler
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








int main(){
    stdio_init_all();
    initialize_stepper_motor();



    // MORE CODE FOR UART:
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

    // OK, all set up.







    while (true){
        while (!input_ready){
            // move_forward(); 
            // sleep_ms(1000);  
            // stop_motors();   
            // sleep_ms(1000); 
        }    


        // Code TO RUN WHEN NOT IN LOW POWER MODE:
        if (sscanf((char *) buffer, "%c", &direction) == 1){
        //   // Set steppers to sleep:
        //   if (direction == 'p' && sleepflag == 1){
        //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
        //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
        //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
        //     pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, STOP_LEVEL);
        //     gpio_put(nSLEEP, 0);
        //     strcpy (sleep_status, "ASLEEP");
        //     sleepflag = 0;
        //   }
        //   // Wake steppers up:
        //   else if (direction == 'p' && sleepflag == 0){
        //     gpio_put(nSLEEP, 1);
        //     strcpy (sleep_status, "AWAKE");
        //     stepperflagreset();
        //     sleepflag = 1;
        //   }

            
            switch (direction){
                // // Move up.
                // case 'o':
                // homeflag = 0;
                // centreflag = 0;
                // if (oflag == 0 && zminflag == 0){                                      // 9. If zminflag is 0, then count is greater than 0 and pressing 'o' will move stepper upwards.
                //     gpio_put(DIR3, 1);
                //     stepperflagreset();
                //     oflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
                //     zmaxflag = 0;                                                        // 5. If the motor moves back up, z isnt at max anymore, so re-enable down by setting max flag to 0.
                //     printf("MOVING UP...\r\n");
                //     printf("zmaxflag OFF\r\n");
                // }
                // else if (oflag == 1 && zminflag == 0){
                //     oflag = 0;
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                // }              
                // break;
        
                // // Move Down.
                // case 'l':
                // homeflag = 0;
                // centreflag = 0;
                // if (lflag == 0 && zmaxflag == 0){                                      // 4. If zmaxflag is 0, then count is less than 21000 and pressing 'l' will move stepper downwards.
                //     gpio_put(DIR3, 0);
                //     stepperflagreset();
                //     lflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
                //     zminflag = 0;                                                        // 10. If the motor moves back down, z isnt at min anymore, so re-enable up by setting min flag to 0.
                //     printf("MOVING DOWN...\r\n");
                //     printf("zminflag OFF\r\n");
                // }
                // else if (lflag == 1 && zmaxflag == 0){
                //     lflag = 0;
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                // }                      
                // break;

                // // Move back.
                // case 'x':
                // homeflag = 0;
                // centreflag = 0;
                // if (xflag == 0 && yminflag == 0){                                      // 9. If yminflag is 0, then count is greater than 0 and pressing 'x' will move stepper downwards.
                //     gpio_put(DIR1, 1);
                //     stepperflagreset();
                //     xflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                //     ymaxflag = 0;                                                        // 5. If the motor moves back down, y isnt at max anymore, so re-enable forwards by setting max flag to 0.
                //     printf("MOVING BACK...\r\n");
                //     printf("ymaxflag OFF\r\n");
                // }
                // else if (xflag == 1 && yminflag == 0){
                //     xflag = 0;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                // }
                // break;
            
                // Move forward.
                case 'w':
                move_forward();
                printf("MOVING FORWARD...\r\n");
                break;

                // Stop.
                case 's':
                stop_motors();
                printf("STOP...\r\n");
                break;

                // // Move left.
                // case 'a':
                // homeflag = 0;
                // centreflag = 0;
                // if (aflag == 0 && xminflag == 0){                                      // 9. If xminflag is 0, then count is greater than 0 and pressing 'a' will move stepper leftwards.
                //     gpio_put(DIR2, 1);
                //     stepperflagreset();
                //     aflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                //     xmaxflag = 0;                                                        // 5. If the motor moves back left, x isnt at max anymore, so re-enable right by setting max flag to 0.
                //     printf("MOVING LEFT...\r\n");
                //     printf("xmaxflag OFF\r\n");
                // }
                // else if (aflag == 1 && xminflag == 0){
                //     aflag = 0;
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                // }        
                // break;

                // // Move right.
                // case 'd':
                // homeflag = 0;
                // centreflag = 0;
                // if (dflag == 0 && xmaxflag == 0){                                      // 4. If xmaxflag is 0, then count is less than 5000 and pressing 'd' will move stepper rightwards.
                //     gpio_put(DIR2, 0);
                //     stepperflagreset();
                //     dflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                //     xminflag = 0;                                                        // 10. If the motor moves back right, x isnt at min anymore, so re-enable left by setting min flag to 0.
                //     printf("MOVING RIGHT...\r\n");
                //     printf("xminflag OFF\r\n");
                // }
                // else if (dflag == 1 && xmaxflag == 0){
                //     dflag = 0;
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                // }                
                // break;

                // // Move diagonally back and left.
                // case 'z':
                // homeflag = 0;
                // centreflag = 0;
                // if (zflag == 0 && yminflag == 0 && xminflag == 0){                     // 4. If yminflag is 0 and xminflag is 0, then ycount is greater than 0 and xcount is greater than 0 and pressing 'z' will move stepper backwards and leftwards.
                //     gpio_put(DIR1, 1);
                //     gpio_put(DIR2, 1);
                //     stepperflagreset();
                //     zflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                //     ymaxflag = 0;                                                        // 5. If the motor moves backwards and left, x isnt at max anymore and y isn't at max anymore, so re-enable right and forwards by setting max ymaxflag and xmaxflag to 0.
                //     xmaxflag = 0;        
                //     printf("MOVING LEFT AND BACK...\r\n");
                //     printf("ymaxflag OFF\r\n"); 
                //     printf("xmaxflag OFF\r\n");                                                                                                         
                // }
                // else if (zflag == 1 && yminflag == 0 && xminflag == 0){
                //     zflag = 0;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                // }                  
                // break;

                // // Move diagonally down and right.
                // case 'c':
                // homeflag = 0;
                // centreflag = 0;
                // if (cflag == 0 && yminflag == 0 && xmaxflag == 0){                     // 4. If yminflag is 0 and xmaxflag is 0, then ycount is greater than 0 and xcount is less than 5000 and pressing 'c' will move stepper backwards and rightwards.
                //     gpio_put(DIR1, 1);
                //     gpio_put(DIR2, 0);
                //     stepperflagreset();
                //     cflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                //     ymaxflag = 0;                                                        // 5. If the motor moves backwards and right, x isnt at min anymore and y isn't at max anymore, so re-enable left and forwards by setting max ymaxflag and xminflag to 0.
                //     xminflag = 0;
                //     printf("MOVING RIGHT AND BACK...\r\n");
                //     printf("ymaxflag OFF\r\n"); 
                //     printf("xminflag OFF\r\n");
                // }
                // else if (cflag == 1 && yminflag == 0 && xmaxflag == 0){
                //     cflag = 0;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                // }                    
                // break;

                // // Move diagonally up and left.
                // case 'q':
                // homeflag = 0;
                // centreflag = 0;
                // if (qflag == 0 && ymaxflag == 0 && xminflag == 0){                     // 4. If ymaxflag is 0 and xminflag is 0, then ycount is less than 5000 and xcount is greater than 0 and pressing 'q' will move stepper forwards and leftwards.
                //     gpio_put(DIR1, 0);
                //     gpio_put(DIR2, 1);
                //     stepperflagreset();
                //     qflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                //     yminflag = 0;                                                        // 5. If the motor moves forwards and left, x isnt at max anymore and y isn't at min anymore, so re-enable right and backwards by setting yminflag and xmaxflag to 0.
                //     xmaxflag = 0;
                //     printf("MOVING LEFT AND FORWARDS...\r\n");
                //     printf("yminflag OFF\r\n"); 
                //     printf("xmaxflag OFF\r\n");
                // }
                // else if (qflag == 1 && ymaxflag == 0 && xminflag == 0){
                //     qflag = 0;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                // }              
                // break;

                // // Move diagonally up and right.
                // case 'e':
                // homeflag = 0;
                // centreflag = 0;
                // if (eflag == 0 && ymaxflag == 0 && xmaxflag == 0){                     // 4. If ymaxflag is 0 and xmaxflag is 0, then ycount is less than 5000 and xcount is less than 5000 and pressing 'e' will move stepper forwards and rightwards.
                //     gpio_put(DIR1, 0);
                //     gpio_put(DIR2, 0);
                //     stepperflagreset();
                //     eflag = 1;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                //     yminflag = 0;                                                        // 5. If the motor moves forwards and right, x isnt at min anymore and y isn't at min anymore, so re-enable left and backwards by setting yminflag and xminflag to 0.
                //     xminflag = 0;
                //     printf("MOVING RIGHT AND FORWARDS...\r\n");
                //     printf("yminflag OFF\r\n"); 
                //     printf("xminflag OFF\r\n");
                // }
                // else if (eflag == 1 && ymaxflag == 0 && xmaxflag == 0){
                //     eflag = 0;
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                // }        
                // break;

                // // SPINDLE SPEED CONTROL:
                // case 'r':
                // pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, 0);
                // printf("SPINDLE OFF\r\n"); 
                // break;
                // case 't':
                // pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, SPINDLE_25);
                // printf("SPINDLE 25%%\r\n");
                // break;
                // case 'y':
                // pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, SPINDLE_50);
                // printf("SPINDLE 50%%\r\n");
                // break;
                // case 'u':
                // pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, SPINDLE_75);
                // printf("SPINDLE 75%%\r\n");
                // break;
                // case 'i':
                // pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, WRAP_LEVEL);
                // printf("SPINDLE 100%%\r\n");
                // break;

                // // EMERGENCY STOP (Turns all steppers and spindle off):
                // case 32:
                // pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STOP_LEVEL);
                // pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STOP_LEVEL);
                // pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STOP_LEVEL);
                // pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, STOP_LEVEL);
                // printf("EMERGENCY STOP\r\n");
                // stepperflagreset();
                // homeflag = 0;
                // centreflag = 0;
                // break;

                // // Home Button:
                // case 'h':
                // centreflag = 0;
                // printf("GOING HOME...\r\n");
                // pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, 0);
                // xmaxflag = 0;
                // ymaxflag = 0;
                // zmaxflag = 0;
                // if (homeflag == 0 && yminflag == 0 && xminflag == 0 && zminflag == 0){      // Home code settings for when all stepper counts are nonzero.
                //     homeflag = 1;
                //     stepperflagreset();
                //     gpio_put(DIR1, 1);
                //     gpio_put(DIR2, 1);
                //     gpio_put(DIR3, 1);
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
                // }
                // else if(homeflag == 0 && yminflag == 0 && xminflag == 0 && zminflag == 1){  // Home code settings for when x & y stepper counts are nonzero and z count is zero.
                //     homeflag = 1;
                //     stepperflagreset();
                //     gpio_put(DIR1, 1);
                //     gpio_put(DIR2, 1);
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                // }
                // else if (homeflag == 0 && xminflag == 0 && zminflag == 0 && yminflag == 1){ // Home code settings for when x & z stepper counts are nonzero and y count is zero.
                //     homeflag = 1;
                //     stepperflagreset();
                //     gpio_put(DIR2, 1);
                //     gpio_put(DIR3, 1);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
                // }
                // else if (homeflag == 0 && xminflag == 0 && yminflag == 1 && zminflag == 1){ // Home code settings for when x stepper count is nonzero and y & z counts are zero.
                //     homeflag = 1;
                //     stepperflagreset();
                //     gpio_put(DIR2, 1);
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                // }
                // else if (homeflag == 0 && yminflag == 0 && zminflag == 0 && xminflag == 1){ // Home code settings for when y & z stepper counts are nonzero and x count is zero.
                //     homeflag = 1;
                //     stepperflagreset();
                //     gpio_put(DIR1, 1);
                //     gpio_put(DIR3, 1);
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
                // }
                // else if (homeflag == 0 && yminflag == 0 && xminflag == 1 && zminflag == 1){ // Home code settings for when y stepper count is nonzero and x & z counts are zero.
                //     homeflag = 1;
                //     stepperflagreset();
                //     gpio_put(DIR1, 1);
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL);
                // }
                // else if (homeflag == 0 && zminflag == 0 && yminflag == 1 && xminflag == 1){ // Home code settings for when z stepper count is nonzero and x & y counts are zero.
                //     homeflag = 1;
                //     stepperflagreset();
                //     gpio_put(DIR3, 1);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
                // }
                // else{
                //     break;
                // }
                // break;

                // // Centre Button:
                // case 'g':
                // homeflag = 0;
                // if (centreflag == 0){ // Only run code if centre flag is 0.
                //     centreflag = 1; // Turn centre flag on to run appropriate code in low power mode to get to home.
                //     printf("GOING TO CENTRE...\r\n");
                //     pwm_set_chan_level(slice_num_SPINDLE, PWM_CHAN_A, 0); // Turn spindle off.
                //     xmaxflag = 0; // Turn all X and Y edge flags off as we are moving to centre.
                //     ymaxflag = 0;
                //     xminflag = 0;
                //     yminflag = 0;

                //     if (zminflag == 0){ // If Z stepper isn't already at the top, turn it on so it heads to the top.
                //     gpio_put(DIR3, 1);
                //     pwm_set_chan_level(slice_num_STEP3, PWM_CHAN_B, STEP_LEVEL);
                //     zmaxflag = 0;
                //     }
                //     // TOP RIGHT QUADRANT:
                //     if (xcount > XCENTRE && ycount > YCENTRE){
                //     gpio_put(DIR2, 1); // Set X stepper left.
                //     gpio_put(DIR1, 1); // Set Y stepper back.
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL); // Turn X and Y steppers on.
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);
                //     }
                //     // TOP LEFT QUADRANT:
                //     else if (xcount < XCENTRE && ycount > YCENTRE){
                //     gpio_put(DIR2, 0); // Set X stepper right.
                //     gpio_put(DIR1, 1); // Set Y stepper back.    
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL); // Turn X and Y steppers on.
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);        
                //     }
                //     // BOTTOM LEFT QUADRANT:
                //     else if (xcount < XCENTRE && ycount < YCENTRE){
                //     gpio_put(DIR2, 0); // Set X stepper right.
                //     gpio_put(DIR1, 0); // Set Y stepper forward.
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL); // Turn X and Y steppers on.
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);            
                //     }
                //     // BOTTOM RIGHT QUADRANT:
                //     else if (xcount > XCENTRE && ycount < YCENTRE){
                //     gpio_put(DIR2, 1); // Set X stepper left.
                //     gpio_put(DIR1, 0); // Set Y stepper forward.   
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL); // Turn X and Y steppers on.
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL);         
                //     }
                //     // X CENTERED, Y BOTTOM HALF:
                //     else if (xcount == XCENTRE && ycount < YCENTRE){
                //     gpio_put(DIR1, 0); // Set Y stepper forward.   
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL); // Turn Y stepper on only.       
                //     }
                //     // X CENTERED, Y TOP HALF:
                //     else if (xcount == XCENTRE && ycount > YCENTRE){
                //     gpio_put(DIR1, 1); // Set Y stepper back.   
                //     pwm_set_chan_level(slice_num_STEP1, PWM_CHAN_B, STEP_LEVEL); // Turn Y stepper on only.       
                //     }
                //     // Y CENTERED, X LEFT HALF:
                //     else if (xcount < XCENTRE && ycount == YCENTRE){
                //     gpio_put(DIR2, 0); // Set X stepper rightward.   
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL); // Turn X stepper on only.       
                //     }
                //     // Y CENTERED, X RIGHT HALF:
                //     else if (xcount > XCENTRE && ycount == YCENTRE){
                //     gpio_put(DIR2, 1); // Set X stepper leftward.   
                //     pwm_set_chan_level(slice_num_STEP2, PWM_CHAN_B, STEP_LEVEL); // Turn X stepper on only.       
                //     }
                // }
                // break;

                // If a key is pressed that isn't defined, then display an error.
                default:
                printf("ERROR: Invalid key.\r\n");
            }
            
        }
        // Empty the buffer
        for (size_t i = 0; i < 100; i++){
        buffer[i] = '\000';
        }
        //GUI();                // Update rgb values in GUI
        input_ready = false;  // This will put the pico back in low power mode
        ind = 0;              // Set the index back to zero









    }
}