(all):
	g++ -o main `pkg-config --cflags --libs sdl` \
		-Wall -Wextra -pedantic -Ofast -fopenmp -ggdb3 \
		main.cpp
