// #define F_CPU 1000000UL // keep internal clock at 1 MHz, should be sufficient

// uncomment to enable serial output
#define DEBUG FALSE

// uncomment to initialize RTC after loss of power, etc.
// (need to set time inside setup() first!)
#define SET_RTC_TIME FALSE

// pins
#define SDA    0 /* PB0 or pin 5 */
#define RELAY  1 /* PB1 or pin 6 */
#define SCL    2 /* PB2 or pin 7 */
#define BUTTON 3 /* PB3 or pin 2 */
#define LED    4 /* PB4 or pin 3 */

// abstractions
#define UIV_ON HIGH
#define UIV_OFF LOW
#define INV_ON LOW
#define INV_OFF HIGH

// settings
#define LED_BLINK_FREQ 750 /* milliseconds */

void setup();
void loop();
void cycle_opstate();
void blink_led();
void set_led_state();
void parker_lewis_clock_check();
void set_relay_state();
