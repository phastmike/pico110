# Pico 110 

![pico110](images/pico110.png "Pico110 prototype 1")

A Motorola Radius M110 mod / modification

## Description

This mood applies to the 2 channel Motorola Radius M110 radios and consists in 
the replacement of the eeprom by a microcontroller capable of behaving like a i2c
slave simulating a serial i2c 24c01 eeprom. 

Everytime the user switches channel the radio pulls the channel from the eeprom.

As a consequence, if we operate on top of the eeprom data and electronically switch
channels, triggering a new read and committment in the internal organics of the frequency
synthesis, we, e.g., can make a new frontal and with very few changes on the equipment
, preservaing it's origin by swapping the frontal and the eeprom.


## Prototyping

Based on this idea and taking in account some compromises, I proceeded to use a raspberry
pi pico board and a TM1638 board with 8 buttons and 8 leds. The drawback is the use of polling
for the buttons. A TFT display and some interrupts for keys would suit better.

We will be using a TM1638 board with 8 push buttons, 8 leds and 8x BCD 7 segment
modules, able to indicate the frequency but with limited text representation.

## Menus / Features

- VFO Mode
- Memory Mode
- Tune Step (TS) (kHz)
- Encoder (Enc) (Hz + OFF)
- Decode (Dec) (Hz + OFF)
- Timeout Timer (TOT) (s)
- Timeout Rekey (DLY/TPY/Pen) (s)
- Reverse (Rev) (+/-) ***
- Repeater Mode/Duplex (Dup) +/- Shift MHz/kHz ***
- Power (P/PA/PO) Hi/Lo (Watts)
- TxAdmit (Tad = MO/AL/NC/NC.PL)

- Display Brightness **
- Screen On/Off (Or a clear so we can keep Leds) ** 
- Scanning (just memories, wip)

### Missing

- Memories (Name and Frequency)
- How to memorize, Edit, Remove (CRUD)
- Dual VFO with same/distinct steps etc.
- Mem to vfo

### New control head

- Need a way to open Squelch, hw

## Links

### I2C

- https://github.com/vmilea/pico_i2c_slave/

### TM1638

- https://github.com/mcauser/micropython-tm1638
- https://github.com/rjbatista/tm1638-library
- https://github.com/thilaire/rpi-TM1638
- https://github.com/wahlencraft/TM1637-pico
- https://github.com/lpodkalicki/attiny-tm1638-library
- https://github.com/gavinlyonsrepo/TM1638plus


---

## NOTES

...
