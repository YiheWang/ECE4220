/* ece4220lab3.h
 * ECE4220/7220
 * Author: Luis Alberto Rivera
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 For lab 3, you will need to copy three files to your Raspberry Pi SD card:
 - The header file "ece4220lab3.h" (this file)
 - The kernel module "ece4220lab3.ko"
 - The static library "libece4220lab3.a" (C programs) or
   "libece4220lab3_cpp.a" (C++ programs).
 
 For convenience, copy the files to your lab 3 project directory (or wherever
 you prefer).
 
 To be able to use the check_button and clear_button functions in your
 programs, you need to include this header file.
 
 When compiling your program, you need to link the static library. 
 To do that, compile like this (C):
	gcc myprogram.c -o myprogram libece4220lab3.a
		or
	gcc myprogram.c -o myprogram -L/path/to/library-directory -lece4220lab3
	
 Or like this (C++):
	g++ myprogram.cpp -o myprogram libece4220lab3_cpp.a
		or
	g++ myprogram.cpp -o myprogram -L/path/to/library-directory -lece4220lab3_cpp
	
 Both for C and C++: Include -lwiringPi as well, if using the wiringPi utilities.
 
 If using Eclipse, make sure to include the libraries in the settings.
 
 Before running "myprogram", the kernel module needs to be installed.
 myprogram needs to be run as super user (sudo).
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

int check_button(void);
void clear_button(void);
