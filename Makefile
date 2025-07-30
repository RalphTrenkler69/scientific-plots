# Makefile for scientific OpenGL plot programs, called "plt-programs".
# Written by Ralph Trenkler, February 2022.
CFLAGS = -O3 -Wall -Wextra -march=native -mtune=native
CPPFLAGS = -O3 -Wall -Wextra -march=native -mtune=native
FFLAGS = -O
FONT=\"/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf\"

all: surfplt xyzplt xyplt surftest xyztest xytest

surfplt: Makefile surfplt.o axes.o text3d.o
	gcc $(CFLAGS) -o $@ surfplt.o axes.o text3d.o \
	-lGL -lGLU -lglut -lftgl -lfreetype -lm -lstdc++

surfplt.o: Makefile surfplt.c
	gcc $(CFLAGS) -c surfplt.c

xyzplt: Makefile xyzplt.o axes.o text3d.o fort_record.o
	gcc $(CFLAGS) -o $@ xyzplt.o fort_record.o axes.o text3d.o \
	-lGL -lGLU -lglut -lftgl -lfreetype -lm -lstdc++

xyzplt.o: Makefile xyzplt.c
	gcc $(CFLAGS) -c xyzplt.c

fort_record.o: Makefile fort_record.c
	gcc $(CFLAGS) -c fort_record.c

axes.o: Makefile axes.cpp text3d.h
	g++ $(CPPFLAGS) -DFONT=$(FONT) -c -I/usr/include/freetype2 axes.cpp

axes2d.o: Makefile axes2d.cpp
	g++ $(CPPFLAGS) -DFONT=$(FONT) -c -I/usr/include/freetype2 axes2d.cpp

text3d.o: Makefile text3d.h text3d.cpp
	g++ $(CPPFLAGS) -c -I/usr/include/freetype2 text3d.cpp

xyplt: Makefile xyplt.o axes2d.o text3d.o
	g++ $(CPPFLAGS) -o xyplt xyplt.o axes2d.o text3d.o \
	-lGL -lGLU -lglut -lftgl -lfreetype -lm

xyplt.o: Makefile xyplt.cpp
	g++ $(CPPFLAGS) -c xyplt.cpp

surftest: Makefile surftest.f90
	gfortran $(FFLAGS) -o $@ surftest.f90

xyztest: Makefile xyztest.f03
	gfortran $(FFLAGS) -o $@ xyztest.f03

xytest: Makefile xytest.f03
	gfortran $(FFLAGS) -o $@ xytest.f03

install: surfplt xyzplt surfplt.1 xyzplt.1 sciplot3d.py
	install -o root -g staff -m 0555 surfplt xyzplt xyplt /usr/local/bin
	mkdir -p /usr/local/man/man1
	mkdir -p /usr/local/lib/python3
	install -o root -g staff -m 0444 sciplot3d.py /usr/local/lib/python3
	install -o root -g staff -m 0444 surfplt.1 xyzplt.1 xyplt.1 \
		/usr/local/man/man1
