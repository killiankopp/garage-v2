#ifndef CONFIG_H
#define CONFIG_H

// Hardware pins configuration
#define RELAY_1_PIN 16
#define SENSOR_CLOSED_PIN 18
#define SENSOR_OPEN_PIN 19

// Timing configuration (in milliseconds)
const unsigned long OPENING_TIMEOUT = 15000;   // 15 seconds to open
const unsigned long CLOSING_TIMEOUT = 20000;   // 20 seconds to close
const unsigned long AUTO_CLOSE_DELAY = 180000; // 3 minutes auto-close delay

// Serial communication
const unsigned long SERIAL_BAUD_RATE = 115200;

// Server configuration
const int SERVER_PORT = 80;

// Main loop delay
const unsigned long MAIN_LOOP_DELAY = 100;

#endif // CONFIG_H