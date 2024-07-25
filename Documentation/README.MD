# Initialization of Atlas Conductivity Sensor with Boron

## 1: Change protocol of Atlas Conductivity Sensor
1. Out of the kit, the Atlas is in UART mode. This needs to be changed to I2C mode because UART connections (TX/RX) are used for communication to the OpenMV.
2. Manually set the protocol from UART to I2C as described [here.](https://www.whiteboxes.ch/docs/tentacle/t2-mkII/#/protocols)
    - The linked steps and images are also pasted below.
    - Note: Figure uses an ArduinoUno, while instructions refer to a Boron. Every connection remains the same except the Arduino powering with the +5V pin; use the VUSB pin on the Boron, as the Boron doesn't have a +5V pin. The VUSB connection uses power directly from the USB port.

## Manually set Atlas protocol from UART to I2C
This procedure is easiest using a breadboard and a set of jumper wires.

1. Connect (shortcut):
    - PGND pin to the TX pin if your circuit is EZO pH, EZO DO, EZO ORP, or EZO EC
    - PRB pint to the TX pin if your circuit is EZO RT

<img src="Photos/Protocol_Step1.jpeg" width="600">

2. Power the EZO Circuit (GND, +5V) (Boron: Use VSUB instead of +5V).
Wait for the LED to change from green to blue (UART->I2C) or from blue to green (I2C->UART).
Note: The Arduino is used as a power source only. You can connect any other power source (3.3V-5V)

3. Remove the jumper wire from the PGND (or PRB respectively) pin from the TX pin
4. Remove power (GND, 5V)

## 2: Wiring Diagram Assembly
<img src="Photos/Circuit-Diagram.jpeg" width="600">
Figure: Made using Canva due to Circuit Diagram issues.

1. Stack the EC EZO on the Atlas Carrier Board so that the EZO's TX, RX, and GND pins are on the side nearest to the Atlas' pins.
2. Ground both devices (GND to GND)
3. Power the Boron from USB input for now by connecting 3V3 on Boron to VCC on Atlas Ezo
    - Switch to 5V Solar System later on
    - Power supply note: Boron can provide a stable 3.3V to carrier board long-term
4. Connect SCL pin on Boron to SCL pin on carrier board
5. Connect SDA pin on Boron to SDA pin on carrier board
6. Screw the probe's cable onto the carrier board

## 3: Firmware for Boron and Atlas
1. Firmware for the Boron and Atlas are found in the `Conductivity Repository/Firmware` folder. Navigate to Firmware > AtlasConductivity_20240724 > src > AtlasConductivity_20240724.cpp **(found here (insert link once pull request is approved))** and copy all code in the file.
2. Create a new project in Particle Workbench / VS Code and paste the code you just copied into the generated .cpp file under src (should have the same name as the project you created).
3. Compile and flash code to the Boron; once successful, open the serial monitor to see data.

## Important Links
- [AtlasScientific EZO<sup>TM</sup> Conductivity Sensor](https://atlas-scientific.com/embedded-solutions/ezo-conductivity-circuit/) - contains documentation, sample Arduino code, and more
- [TheGeographer/water-quality-array](https://github.com/TheGeographer/water-quality-array/tree/master) - a project that used the Atlas probe for water quality measurements