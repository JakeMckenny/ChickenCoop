#define TRIG 13
#define ECHO3V3 14
#define DISTANCE_THRESHOLD 16 // Distance threshold in centimeters

extern volatile uint32_t rise_time;
extern volatile uint32_t fall_time;
extern volatile bool trig_pulse_complete;
extern float distance_cm; 

void initialize_ultrasonic();

void measure_distance(uint gpio, uint32_t events);

void measure_trig_pulse_duration();