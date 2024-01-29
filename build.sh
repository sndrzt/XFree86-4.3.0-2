#!/usr/bin/env bash

if [[ $(uname -r) != *"2.4.20"* ]]; then echo "XFree86 should be build on Red Hat Linux 9.0 with 2.4.20 kernel."; exit; fi

sudo cp /usr/include/libpng10/png* /usr/include/
sudo cp /usr/lib/libpng10.a /usr/lib/libpng.a
cp config/cf/xf86site.def host.def

pushd config/util
make -f Makefile.ini lndir
sudo cp lndir /usr/bin/
popd

make World

sudo cp /usr/lib/libexpat.so.0 /usr/lib/libexpat.so.1

sudo cp programs/Xserver/XFree86 /usr/X11R6/bin/XFree86
sudo cp programs/xterm/xterm /usr/X11R6/bin/xterm

sudo cp lib/X11/libX11.so.6.2 /usr/X11R6/lib/libX11.so.6.2
sudo cp lib/X11/libXaw.so.6.1 /usr/X11R6/lib/libXaw.so.6.1
sudo cp lib/font/libXfont.so.1.4 /usr/X11R6/lib/libXfont.so.1.4
sudo cp lib/Xft1/libXft.so.1.1 /usr/X11R6/lib/libXft.so.1.1
sudo cp lib/Xft/libXft.so.2.1 /usr/X11R6/lib/libXft.so.2.1
sudo cp lib/freetype2/libfreetype.so.6.3 /usr/lib/libfreetype.so.6.3.2

