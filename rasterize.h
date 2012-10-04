#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/param.h>

#define EMPTY_VAL 0.0f

typedef struct {
    double x;
    double y;
    double z;
} Point3D;

// typedef struct
// {
//     double x;
//     double y;
// } Point2D;

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
} Extents;

typedef struct {
    float *pixels;
    int rows;
    int cols;
    Extents *extents;
} Image;

typedef struct {
    PointNode **nodes;
    Extents *extents;
    int rows;
    int cols;
} Index;

void printImage(Image *image);

void printIndexBins(Index *index);

void rasterize(Index *index, Image *image);

void addPointToIndex(Index *index, Point3D *point);

void addToBin(PointNode **current, Point3D* point);

void readPointsFromFile(char *filename, Index *index);

void calcImageSize(Extents *extents, int *rows, int *cols);

void point2Index(Extents *extents, Point3D *point, int *row, int *col);

void pixel2Point(Image *image, int *row, int *col, Point3D *point);

#endif
