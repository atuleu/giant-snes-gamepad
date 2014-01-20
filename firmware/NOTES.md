# Notes

This file keep some useful conceptions notes.

## Analog to Digital COnversion

### Prescaling and window size selection

The analog input are multiplexed around a single ADC converter. Therefore one convertion a time could occurs

We should choose a prescaler for the clock. BAsed on the XTAL (16MHz) to be between 50 kHz and 200 kHz. Only possible values is 128, and gave us a ADC clock rate of 125 kHz.

First conversiona after ADEN is set takes 25 clock cycle 13 afterwards.

We would like to know the power of two that gives us the best window size to filter the read data. We would like to have a 100 Hz sensor

We have ( 4 * 13 ) * 128 / 16e6 * 12 = 4.99 ms

And ( 8 * 13 ) * 128 / 16e6 * 12 = 9.98 ms.

We choose the later, and we perform updates every 12 ms to the computer (80 Hz controller )


### notes

* channel change can only occurs safely :
  a. When ADATE or ADEN is cleared.
  b. During conversion, minimum one ADC clock cycle after the trigger event.
  c. After a conversion, before the interrupt flag used as trigger source is cleared.

* From the schematics, we have a cleaning capacitor on AVRef, so the default setting would be the best. ( Internal connection to AVcc with Noise cancellation cap on AVref )

* ADEN should be left to 1. Then only start with ADSC.

* Interrupt vector maybe called ADC_vect

* Beware of order where results are read. refer to doc !
