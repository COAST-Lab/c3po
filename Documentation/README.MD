# Initialization of Atlas Conductivity Sensor with Boron

## Change protocol of Atlas Conductivity Sensor
1. Out of the kit the atlas is in UART mode which we need to change to I2C mode because UART connections (TX/RX) are used for communication to the OpenMV.
2. I manually set the protocol from UART to I2C described [here.](https://www.whiteboxes.ch/docs/tentacle/t2-mkII/#/protocols) I have also copy and pasted the steps and images below. Please note that the figure uses an ArduinoUno while I used a Boron. Every connection remains the same besides the Arduino powering with the +5V pin. I used the VUSB pin on the Boron because the +5V pin does not exist on the Boron. This uses the power directly from the usb port.

This procedure is easiest using a breadboard and a set of jumper wires.

1. Connect (shortcut) these two pins:
    - PGND pin to the TX pin if your circuit is EZO pH, EZO DO, EZO ORP, or EZO EC
    - Only exception is EZO RT

<img src="pathway" width="600">

D: Short the PRB pin to the TX pin instead.

2. Power the EZO Circuit (GND, +5V) (Use VSUB instead of +5V)
    - Wait for the LED to change from green to blue (UART->I2C) or from blue to green (I2C->UART). (The Arduino is used as a power source only. You can connect any other power source (3.3V-5V))
    1. Remove the jumper wire from the PGND (or PRB respectively) pin to the TX pin
    2. Remove power (GND, 5V)

## Wiring Diagram Assembly
[insert photo]
Figure: Made using Canva due to Circuit Diagram issues.

1. Ground both devices
2. Power the Boron from USB input for now (switch to 5V Solar System later on). Power the Ezo using the 3V3 input on the Boron.
    - Power Supply Note: Boron can provide stable 3.3V to carrier board long term.
3. SCL pin on Boron to SCL pin on carrier board
4. SDA pin on Boron to SDA pin on carrier board
5. The probe attaches to the carrier board.

## Firmware for Boron and Atlas
1. Firmware for the Boron and Atlas are found in the `Conductivity Repository/Firmware` folder.
2. I was successfully able to collect results onto the serial monitor in VS code.

## Important Links
[AtlasScientific EZO<sup>TM</sup> Conductivity Sensor](https://atlas-scientific.com/embedded-solutions/ezo-conductivity-circuit/) - contains documentation, sample Arduino code, and more
[TheGeographer/water-quality-array](https://github.com/TheGeographer/water-quality-array/tree/master) - a project that used the Atlas probe for water quality measurements