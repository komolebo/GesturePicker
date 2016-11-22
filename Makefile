

#PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}
#export PKG_CONFIG_PATH


build:
		g++ -std=c++11 -o opencv main.cpp handGesture.cpp Starter.cpp data.cpp Functions.cpp `pkg-config --cflags --libs opencv`


all: build
		./opencv
