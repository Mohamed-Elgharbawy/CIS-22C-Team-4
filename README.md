# Online-Storefront
Our CIS 22C final group project.

Topic: Tablet convertible Laptops.
  
	Members: 
		Mohamed: Team Coordinator & Main
		Jasper: GUI
		Andrew: Priority Queue
		Tin: Hash Tables
		Vu: BST Algorithms
		Alex: File I/O 

### To compile and run the program on Mac:

First, install home-brew from [brew.sh](https://brew.sh) if you don't have it already.

Then, install two packages:
>brew install pkg-config

>brew install gtk


pkg-config manages the includes and libraries for the project

gtk is the graphics library the project uses

Make sure to install gtk2 and NOT gtk3


Then, use this command to compile the program (make sure you cd into the correct directory first):
>g++ \*.cpp -o program \`pkg-config --cflags gtk+-2.0\` \`pkg-config --libs gtk+-2.0\`

and then run it:
>./program

### For Windows:
Make sure you have GTK installed. Tested with GTK3.
Use a makefile, or copy the following into a .bat file:
~~~~
@echo off
  
set gtk_ver=gtk+-3.0
pkg-config %gtk_ver% --cflags --libs >tmp.txt
set /p pkg-info= <tmp.txt
del tmp.txt

rem echo %pkg-info%
g++ *.cpp -o program.exe -Wall %pkg-info%
pause
~~~~
and put it in the directory with your code.
