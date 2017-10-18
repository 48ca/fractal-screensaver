(all):
	g++ -o fss `pkg-config --cflags --libs sdl` \
		-Wall -Wextra -pedantic -Ofast -fopenmp -ggdb3 \
		main.cpp
