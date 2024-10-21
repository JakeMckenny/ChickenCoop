#include <string>
#include <stdint.h>  // For uint8_t, int16_t types

extern int16_t x;
extern int16_t y;
extern int16_t z;

int LIS3DH_writeRegister(uint8_t reg, uint8_t value);  // Writes to a register

int LIS3DH_readRegister(uint8_t reg, uint8_t *value);  // Reads from a register

void LIS3DH_init();

int LIS3DH_readAccelValues();  // Reads raw acceleration values

std::string LIS3DH_getCurrentReadings();  // Returns formatted string of current readings

void send_accel_readings();