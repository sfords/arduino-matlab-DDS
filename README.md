# arduino-matlab-DDS

This is a protoype I wrote and made in the summer of 2015 as a research assistant in Dr. Min Ouyang's nano-optics group at the University of Maryland. The goal of this project was to demonstrate that we could use a computer interface to have live control over hardware interfacing with an Arduino. 

It consists of the .m and .ino files contained in this repository as well as two pieces of hardware: An Arduino Mega2560 and an Analog AD9850 DDS board that is essentially a sine wave generator. I won't discuss the hardware here other than to say that the Arduino and DDS are connected Serially, and the Arduino has a USB Serial connection to a PC.

The DDScom.m file is a set of control loops. The program waits for the user to input a command into the Matlab console. The user can change DDS sweep parameters, run a frequency sweep or close the program. The user inputs a number if required, and then the program sends data out the Serial port as a string formatted as 'X12345"... where X is the mode, and the numbers are a given frequency.

In the wavegen_matlab.ino file, processByte() processes the input from Matlab via the Serial bus, and updates the mode the Arduino is in. PassVal() sets the specificed variable to the given value. sendFrequency() communicates with the AD9850 byte by byte. Setup() and loop() are native functions necessary to run an Arduino program.
