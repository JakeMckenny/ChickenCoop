// Declare external global variables for use in the interrupt handler (this needs to be done since we update these variables in the main code).
extern volatile bool input_ready;      
extern volatile unsigned int ind;      
extern volatile char buffer[100];

void on_uart_rx(void);

void initialize_uart();