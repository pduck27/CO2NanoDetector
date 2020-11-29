# CO2NanoDetector
Small project to combine Arduino nano with CO2 detection and a LED matrix.
The matrix shows several states of CO2 quality, so you know when it is time for fresh air.

The final setup was glued into a Minecraft styled scene.
Check thingiverse for some pictures https://www.thingiverse.com/thing:4668278

You need:
- Arduino Nano
- MAX7219 LED Matrix
- MH-Z19 CO2 sensor
- A button is optional
- Cables, plugs and so on depending on your needs

The code checks every second the current CO2 quality in ppm. 
According to the ppm value it shows a smiley in the matrix. 
If the quality is too bad also a window animation is shown.

You can connect an optional button, which shows the ppm value as digits in the matrix when pressed long.

The code was built with VS Code and Plattform.IO Extension. 

My plattfomio.ini looks like this (keep an eye on lib_deps you must include):
[env:nanoatmega328]
platform = atmelavr
board = nanoatmega328
framework = arduino
monitor_speed = 115200
upload_port = com8
lib_deps = 
	wayoda/LedControl@^1.0.6


Check and maybe adjust the pin setting from source file:
- 2 and 3 (TX & RX) to the CO2 detector
- 10,11 and 13 for the matrix control
- 4 to the button (crossed with a 100kOhm resistor to 3,3 voltage output)


