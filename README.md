# CO2NanoDetector
Small project to combine Arduino nano with CO2 detection and a LED matrix.
The matrix shows several states of CO2 quality, so you know when it is time for fresh air.

The final setup was glued into a Minecraft styled scene.
Check thingiverse for some pictures https://www.thingiverse.com/thing:4668278

# What you need
You need (see circuit below):
- Arduino Nano
- MAX7219 8x8 LED Dot Matrix 
- MH-Z19 CO2 sensor
- A button to show the ppm value as digits is optional
- Cables, plugs and so on depending on your needs
- I use an old powerbank Lipo with a 18650 Battery Shield to run it. Be careful, the power consumption is so low that a powerbank maybe goes to sleep mode. Check the internet for possible solutions with an additional resistor. 

# How it works
The code checks every second the current CO2 quality in ppm. 
According to the ppm value it shows a smiley in the matrix.
The default thresholds in the code are:

0 - 800 ppm Good quality -> smiling face

801 - 1200 ppm Average quality -> neutral face

\>1200 ppm Bad quality -> sad face and a window open animation each five seconds
 
You can connect an optional button, which shows the ppm value as digits in the matrix when pressed long.

All images and animations can be changed, please see the link in the code to design your own ones.

# Code and circuit
The code was built with VS Code and Plattform.IO Extension. My plattfomio.ini looks like this (keep an eye on lib_deps you must include):   

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

My circuit looks like this. Unfortunatley I did not find a good image for the LED connectors, so please follow the labels / order on the note. Circuit files are also in subdirectory of repo.

![circuit image](/circuit/circuit_Schaltplan.png?raw=true "Circuit")


