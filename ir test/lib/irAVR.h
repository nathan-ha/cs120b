#ifndef IRAVR_h
#define IRAVR_h

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "helper.h"
#include "serialATmega.h"

#define MAX_RAWBUF_LEN 100

// IR codes
#define VOL_UP_IR 16736925
#define VOL_DOWN_IR 16754775
#define LEFT_IR 16720605
#define RIGHT_IR 16761405
#define PAUSE_IR 16712445

#define LTOL (1.0 - 25 / 100.)
#define UTOL (1.0 + 25 / 100.)

#define TICKS_LOW(us) (int)(((us) * LTOL / 50))
#define TICKS_HIGH(us) (int)(((us) * UTOL / 50 + 1))

int MATCH(int measured, int desired) { return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired); }
int MATCH_MARK(int measured_ticks, int desired_us) { return MATCH(measured_ticks, (desired_us + 100)); }
int MATCH_SPACE(int measured_ticks, int desired_us) { return MATCH(measured_ticks, (desired_us - 100)); }

typedef struct {
  uint8_t recvpin;   // Pin for IR data from detector
  uint8_t rcvstate;  // State machine
  volatile uint8_t *ddr;
  volatile uint8_t *pin;
  unsigned int timer;                   // State timer, counts 50uS ticks.
  unsigned int rawbuf[MAX_RAWBUF_LEN];  // Raw data
  uint8_t rawlen;                       // Counter of entries in rawbuf
} irparams_t;

class decode_results {
 public:
  int decode_type;                // NEC
  unsigned long value;            // Decoded value
  int bits;                       // Number of bits in decoded value
  volatile unsigned int *rawbuf;  // Raw intervals in .5 us ticks
  int rawlen;                     // Number of records in rawbuf.
};

enum IRStates { STATE_IDLE, STATE_MARK, STATE_SPACE, STATE_STOP };

volatile irparams_t irparams;

void IRinit(volatile uint8_t *ddr, volatile uint8_t *pin, int recvpin) {
  if (recvpin < 0 || recvpin > 7) {
    // Invalid pin number
    return;
  }

  irparams.recvpin = recvpin;
  irparams.ddr = ddr;
  irparams.pin = pin;

  cli();
  // Setup pulse clock timer interrupt
  // Prescale /8 (16M/8 = 0.5 microseconds per tick)
  // Therefore, the timer interval can range from 0.5 to 128 microseconds
  // depending on the reset value (255 to 0)
  TCCR0A = _BV(WGM01);
  TCCR0B = _BV(CS01);
  OCR0A = (16000000 * 50 / 1000000) / 8;
  TCNT0 = 0;

  // Timer0 Overflow Interrupt Enable
  TIMSK0 = _BV(OCIE0A);

  sei();  // Enable interrupts

  // Initialize state machine variables
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;

  *irparams.ddr &= ~(1 << irparams.recvpin);
}

void IRresume() {
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;
}

long decodeNEC(decode_results *results) {
  long data = 0;
  int offset = 1;  // Skip first space
  // Initial mark
  if (!MATCH_MARK(results->rawbuf[offset], 9000)) {
    return 0;
  }
  offset++;
  // Check for repeat
  if (irparams.rawlen == 4 && MATCH_SPACE(results->rawbuf[offset], 2250) &&
      MATCH_MARK(results->rawbuf[offset + 1], 560)) {
    results->bits = 0;
    results->decode_type = 1;
    return 1;
  }
  if (irparams.rawlen < 2 * 32 + 4) {
    return 0;
  }
  // Initial space
  if (!MATCH_SPACE(results->rawbuf[offset], 4500)) {
    return 0;
  }
  offset++;
  for (int i = 0; i < 32; i++) {
    if (!MATCH_MARK(results->rawbuf[offset], 560)) {
      return 0;
    }
    offset++;
    if (MATCH_SPACE(results->rawbuf[offset], 1600)) {
      data = (data << 1) | 1;
    } else if (MATCH_SPACE(results->rawbuf[offset], 560)) {
      data <<= 1;
    } else {
      return 0;
    }
    offset++;
  }
  // Success
  results->bits = 32;
  results->value = data;
  results->decode_type = 0;
  return 1;
}

int IRdecode(decode_results *results) {
  results->rawbuf = irparams.rawbuf;
  results->rawlen = irparams.rawlen;
  if (irparams.rcvstate != STATE_STOP) {
    return 0;
  }
  if (decodeNEC(results)) {
    return 1;
  }

  // Throw away and start over
  IRresume();
  return 0;
}

ISR(TIMER0_COMPA_vect) {
  uint8_t irdata = (*(irparams.pin) >> irparams.recvpin) & 0x01;

  irparams.timer++;  // One more 50us tick
  if (irparams.rawlen >= MAX_RAWBUF_LEN) {
    // Buffer overflow
    irparams.rcvstate = STATE_STOP;
    return;
  }
  switch (irparams.rcvstate) {
    case STATE_IDLE:  // In the middle of a gap
      if (irdata == 0) {
        if (irparams.timer < 5000 / 50) {
          // Not big enough to be a gap.
          irparams.timer = 0;
        } else {
          // Gap just ended, record duration and start recording transmission
          irparams.rawlen = 0;
          irparams.rawbuf[irparams.rawlen++] = irparams.timer;
          irparams.timer = 0;
          irparams.rcvstate = STATE_MARK;
        }
      }
      break;
    case STATE_MARK:      // Timing MARK
      if (irdata == 1) {  // MARK ended, record time
        irparams.rawbuf[irparams.rawlen++] = irparams.timer;
        irparams.timer = 0;
        irparams.rcvstate = STATE_SPACE;
      }
      break;
    case STATE_SPACE:     // Timing SPACE
      if (irdata == 0) {  // SPACE just ended, record it
        irparams.rawbuf[irparams.rawlen++] = irparams.timer;
        irparams.timer = 0;
        irparams.rcvstate = STATE_MARK;
      } else {  // SPACE
        if (irparams.timer > 5000 / 50) {
          // Big SPACE, indicates gap between codes
          // Mark current code as ready for processing
          // Switch to STOP
          // Don't reset timer; keep counting space width
          irparams.rcvstate = STATE_STOP;
        }
      }
      break;
    case STATE_STOP:      // Waiting, measuring gap
      if (irdata == 0) {  // Reset gap timer
        irparams.timer = 0;
      }
      break;
  }
  decode_results d;
  if (IRdecode(&d)) {
    serial_println(d.value);
    IRresume();
  }
}

#endif
