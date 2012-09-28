all: rasterize

rasterize: rasterize.c rasterize.h
	gcc rasterize.c -o rasterize -lm -Wall
