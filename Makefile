(all):
	g++ -o test `pkg-config --cflags --libs sdl` \
		-Wall -Wextra -pedantic -Ofast -fopenmp \
		sdl.cpp
