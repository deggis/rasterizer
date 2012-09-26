#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

typedef struct {
    Point3D p;
    struct PointNode *next;
} PointNode;

typedef struct {
    double lu_long;
    double lu_lat;
    double rl_long;
    double rl_lat;
    double gsd; // in metres
} TaskInfo;

typedef struct {
    float *pixels;
    int rows;
    int cols;
} Image;

typedef struct {
    PointNode **nodes;
    TaskInfo info;
    int rows;
    int cols;
} Index;

void printImage(Image image);

void addPoint(Index index, Point3D *point);

// void transform(TaskInfo info, Point3D *p, int *row, int *col);

#endif
