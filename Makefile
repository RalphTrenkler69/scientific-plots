# Makefile for scientific OpenGL plot programs, called "plt-programs".
# Written by Ralph Trenkler, February 2022.
CFLAGS = -Wall -O
FFLAGS = -Wall -O

all: surfplt xyzplt xyplt

surfplt: Makefile surfplt.o axes.o text3d.o
	gcc $(CFLAGS) -o $@ surfplt.o axes.o text3d.o \
	-lGL -lGLU -lglut -lftgl -lfreetype -lm -lstdc++

surfplt.o: Makefile surfplt.c
	gcc $(CFLAGS) -c surfplt.c

xyzplt: Makefile xyzplt.o axes.o text3d.o
	gcc $(CFLAGS) -o $@ xyzplt.o axes.o text3d.o \
	-lGL -lGLU -lglut -lftgl -lfreetype -lm -lstdc++

xyzplt.o: Makefile xyzplt.c
	gcc $(CFLAGS) -c xyzplt.c

axes.o: Makefile axes.cpp text3d.h
	g++ $(CFLAGS) -c -I/usr/include/freetype2 axes.cpp

axes2d.o: Makefile axes2d.cpp
	g++ $(CFLAGS) -c -I/usr/include/freetype2 axes2d.cpp

text3d.o: Makefile text3d.h text3d.cpp
	g++ $(CFLAGS) -c -I/usr/include/freetype2 text3d.cpp

xyplt: Makefile xyplt.o axes2d.o text3d.o
	g++ $(CFLAGS) -o xyplt xyplt.o axes2d.o text3d.o \
	-lGL -lGLU -lglut -lftgl -lfreetype -lm

xyplt.o: Makefile xyplt.cpp
	g++ $(CFLAGS) -c xyplt.cpp

install: surfplt xyzplt surfplt.1 xyzplt.1
	install -o root -g staff -m 0555 surfplt xyzplt xyplt /usr/local/bin
	if [ ! -d /usr/local/man ]; then mkdir /usr/local/man; fi
	if [ ! -d /usr/local/man/man1 ]; then mkdir /usr/local/man/man1; fi
	install -o root -g staff -m 0444 surfplt.1 xyzplt.1 xyplt.1 \
		/usr/local/man/man1
