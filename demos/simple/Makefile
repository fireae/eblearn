# This makefile assumes eblearn headers are installed on the system
# (using make install)

includes = -I/usr/include/qt4 \
	   -I/usr/include/qt4/Qt \
	   -I/usr/include/qt4/QtGui \
	   -I/usr/include/qt4/QtCore

links = -lidx -lidxgui

options = -D__IMAGEMAGICK__ # let eblearn know that image magick is present

all:
	g++ simple.cpp ${links} ${includes} ${options}
