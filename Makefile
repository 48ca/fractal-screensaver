(all):
	g++ -o test `pkg-config --cflags --libs sdl` \
		-Wall -Wextra -pedantic -Ofast -fopenmp -ggdb3 \
		sdl.cpp
