#include "Arduino.h"

#include "main.h"

#include <TinyWireM.h>          // required by TinyDS1307
#include <USI_TWI_Master.h>     // using I2C via the chip's USI
#include <TinyDS1307.h>         // using the RTC - ATTN: the header needed changing to set the RTCs
                                // memory size to 0 because setting it to its actual value (56 bytes)
                                // grows the sketch beyond bounds!
#include <PinChangeInterrupt.h> // using PCINTs instead of INTs
#include <avr/io.h>             // direct port manipulation
// #include <util/delay.h>      // to have a substitute for delay() - not needed on attiny85

// list of operating states
enum opstates {
  OFF, /* to coincide with 0/LOW/FALSE */
  ON,  /* to coincide with 1/HIGH/TRUE */
  AUTO /* the only one that can not be treated as boolean */
};

// globals (ew!)
unsigned long led_blink_moment = 0 /* milliseconds */;
unsigned char led_state = UIV_OFF;
unsigned char relay_state = INV_OFF;
volatile enum opstates op_state = AUTO;

void setup()
{
  pinMode(RELAY, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  // DDRB = 0b00010010; // PB1 and PB4, pins 6 and 3
  // (all zeroes are inputs by default)

  // the button doesn't sit on pin 7, which is the only one supporting INTn 
  // style interrupts. pin 2 is PCINTn style, so can't use attachInterrupt().
  attachPCINT(digitalPinToPinChangeInterrupt(BUTTON), cycle_opstate, RISING);

  TinyWireM.begin();
  tinyrtc.begin();
#ifdef SET_RTC_TIME
  // if (!tinyrtc.isrunning()) // outcommented since if the #define is set, we always want to adjust the time
  tinyrtc.adjust(2018,1,31, 0,3,6);
#endif
}

void loop()
{
  set_led_state();
  set_relay_state();
}

void cycle_opstate()
{  
  // go from AUTO to ON to OFF to AUTO, and so forth...
  if (op_state < AUTO) {
    op_state = op_state + 1; // can't opstate++ - it's an enum! this here is fishy, too, but works with -fpermissive.
  } else {
    op_state = OFF;
  }

  // ensure a bit of a nicer transition
  led_state = UIV_OFF;
  led_blink_moment = millis();
}

void blink_led()
{
  unsigned long current_time = millis();

  if (current_time - led_blink_moment >= LED_BLINK_FREQ) {
    led_blink_moment = current_time;
    led_state = !led_state;
  }
}

void set_led_state()
{
  if (op_state == AUTO) {
    blink_led();
  } else {
    led_state = op_state; // yup, this is crazy, because i'm crazy.
                          // works because usually HIGH==1 and LOW==0,
                          // also see enum declaration comments.
  }

  if (digitalRead(LED) != led_state) digitalWrite(LED, led_state);
  // if (led_state != (PINB & 1 << PB4)) PORTB = (led_state << PB4);
}

void parker_lewis_clock_check()
{
  tinyrtc.read();

#ifdef DEBUG
  unsigned char the_second = tinyrtc.second();

  if (the_second % 5 == 0) {
    relay_state = !relay_state;
  }
#else
  unsigned char the_hour = tinyrtc.hour(); // char is smaller than int and sufficient for a 0-23 value

  switch (the_hour) {
    case  8 ... 13 : relay_state = INV_ON  ; break; //  8 - 14 o'clock
    case 14 ... 17 : relay_state = INV_OFF ; break; // 14 - 18 o'clock
    case 18 ... 23 : relay_state = INV_ON  ; break; // 18 - 24 o'clock
    case  0 ...  7 : relay_state = INV_OFF ; break; //  0 -  8 o'clock

    default:
    relay_state = INV_OFF ; break;
  }
#endif
}

void set_relay_state()
{
  if (op_state == AUTO) {
    parker_lewis_clock_check();
  } else {
    relay_state = !op_state; // inverted because the relay goes ON on LOW!
                             // possible because the first two enum states
                             // correspond to HIGH (1) and LOW (0).
  }

  if (digitalRead(RELAY) != relay_state) {
  // if (relay_state != (PINB & 1 << PB1)) {
    digitalWrite(RELAY, relay_state);
    // PORTB = (relay_state << PB1);
  
    // no more space left to include delay(),
    // but need some buffering for the button
    // _delay_ms(350 /* milliseconds */);
    delay(500); // on an attiny85, there's enough room alright
  }
}
