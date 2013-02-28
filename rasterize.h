#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/param.h>
#include <glib-2.0/glib.h>

#define EMPTY_VAL 0.0f

typedef struct {
    double x;
    double y;
    double z;
} Point3D;

typedef struct PointNode_t {
    Point3D p;
    struct PointNode_t *next;
} PointNode;

typedef struct {
    double lu_long;
    double lu_lat;
    double rl_long;
    double rl_lat;
    double gsd; // in metres
    char *pointsFileName;
} Config;

typedef struct {
    float *pixels;
    int rows;
    int cols;
    Config *config;
} Image;

typedef struct {
    PointNode **nodes;
    Config *config;
    int rows;
    int cols;
} Index;

void printImage(Image *image);

void printIndexBins(Index *index);

void rasterize(Index *index, Image *image);

void addPointToIndex(Index *index, Point3D *point);

void addToBin(PointNode **current, Point3D* point);

void readPointsFromFile(char *filename, Index *index);

void readConfig(char *filename, Config *config);

void calcImageSize(Config *config, int *rows, int *cols);

void point2Index(Config *config, Point3D *point, int *row, int *col);

void pixel2Point(Image *image, int *row, int *col, Point3D *point);

#endif
