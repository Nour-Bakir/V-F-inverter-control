#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SINE_SAMPLES 100

// Pre-calculated Sine Table (scaled between 0 and 799 for ma = 1.0)
// OCR1A values = 399.5 + 399.5 * sin(2 * PI * i / 100)
const uint16_t sine_table[SINE_SAMPLES] = {
    400, 425, 450, 475, 500, 524, 548, 571, 594, 616,
    637, 657, 676, 694, 711, 726, 740, 752, 763, 772,
    780, 786, 791, 795, 797, 798, 797, 795, 791, 786,
    780, 772, 763, 752, 740, 726, 711, 694, 676, 657,
    637, 616, 594, 571, 548, 524, 500, 475, 450, 425,
    400, 374, 349, 324, 299, 275, 251, 228, 205, 183,
    162, 142, 123, 105,  88,  73,  59,  47,  36,  27,
     19,   13,   8,   4,   2,   1,   2,   4,   8,  13,
     19,   27,  36,  47,  59,  73,  88, 105, 123, 142,
    162, 183, 205, 228, 251, 275, 299, 324, 349, 374
};

volatile uint8_t sample_index = 0;

// Interrupt runs at 20 kHz (every time Timer1 overflows)
ISR(TIMER1_OVF_vect) {
    // Update duty cycle for next PWM period
    OCR1A = sine_table[sample_index];

    sample_index++;
    if (sample_index >= SINE_SAMPLES) {
        sample_index = 0;
    }
}

void timer1_init(void) {
    // Set PD5 (OC1A) as output
    DDRD |= (1 << PD5);

    // Mode 14: Fast PWM with TOP = ICR1
    // Non-inverting PWM on OC1A
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // Prescaler = 1

    // Set TOP for 20 kHz PWM (16 MHz / 20 kHz = 800)
    ICR1 = 799;

    // Initial duty cycle (50%)
    OCR1A = 400;

    // Enable Timer1 Overflow Interrupt
    TIMSK |= (1 << TOIE1);
}

int main(void) {
    timer1_init();
    sei(); // Enable global interrupts

    while (1) {
        // Main loop free for other tasks
    }
}
