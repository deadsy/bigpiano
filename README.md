
This firmware is used to control a 17 foot dance on piano.

The piano is being used for a community theatre production of "Big the Musical".
This is based on the 1988 movie "Big" which had Tom Hanks dancing on a big piano.

The firmware runs on an Atmel atmega328p microcontroller.
Many people call this an Arduino - although this code does not use the Arduino IDE or libraries. 

Specifically:

There are 28 white keys (4 octaves) being scanned.

When a key is pressed:
A light on the front of the key (an rgb led module) is turned on with a specified color.
A midi note on message (with the appropriate note number) is generated on the serial port.

When a key is released:
The light on the front of the key is turned off.
A midi note off message (with the appropriate note number) is generated on the serial port.

In essence it's 17'x4' midi controller with lights on the keys.
