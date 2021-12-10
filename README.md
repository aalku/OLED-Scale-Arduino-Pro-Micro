# OLED-Scale-Arduino-Pro-Mini

Compact scale with OLED display - Hardware and code

![image](https://user-images.githubusercontent.com/12587056/145628375-a3acac8c-aebe-4db4-b823-13afd2103d17.png)

## Code

The code is included in the code folder.

Theese are the referenced libraries.

* <https://github.com/bogde/HX711>
* <https://github.com/rocketscream/Low-Power>
* <https://github.com/netguy204/OakOLED>
* <https://github.com/adafruit/Adafruit-GFX-Library>

## Hardware

* Arduino Pro Mini: <https://es.aliexpress.com/item/1983330995.html>
* Pressure sensor + HX711 amplifier: <https://es.aliexpress.com/item/33046037411.html>
  * I think I used 5KG.
* SSD1306 I2C 0.96" 128X64 pixels white OLED display: <https://es.aliexpress.com/item/1005003371135306.html>
* 9v Battery and connector: <https://es.aliexpress.com/item/4001007666600.html>
* MCP1702-3302E Voltage regulator: <https://es.aliexpress.com/item/33026559699.html>
* Standard 6x6x5mm DIP monentary switch: <https://es.aliexpress.com/item/33002039235.html>
* I designed a structure to be 3D printed. It is as a FreeCad file in the structure folder.

## Hardware modifications and details

![image](https://user-images.githubusercontent.com/12587056/145628960-f516cf97-24d5-411e-9fc7-a1609de76eb7.png)

I removed the microcontroller power LED and voltage regulator so that the battery last longer. I used an external MCP1702-3302E voltage regulator. *I need to document this more*

You can use an Arduino Nano microcontroller but then you can't power off the serial-USB interface and it will drain power too. Arduino Pro Mini doesn't have one. You have to use an external Serial adaptor.

The microcontroller enters sleep mode waiting for interrupt to wake up. *I need to document this more*

16 MHz microcontroller maybe should be used with 5v but screen is 3.3v and HX711 supports both. I'm using 3.3v for everything with no trouble.

You should use a transistor to power on the HX711 while awake and power it down while in sleep mode. I'm opening the circuit by the ground side to power down. *I need to document this more*

It seems (I am actually reverse engineering my own circuit) as I open the circuit from the ground side I put a pull-up resistor on the data an clock lines.

![image](https://user-images.githubusercontent.com/12587056/145629880-48ef841f-73d4-40cb-9522-efac9c3eb6be.png)

If you need more detail or want to improve my circuit please open a issue or pull-request.
