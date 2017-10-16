(all):
	g++ -o test `pkg-config --cflags --libs sdl` sdl.cpp
