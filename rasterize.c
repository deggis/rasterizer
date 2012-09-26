
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
    double x;
    double y;
    double z;
} Point;

typedef struct
{
    Point p;
    struct PointNode *next;
} PointNode;

typedef struct
{
    double lu_long;
    double lu_lat;
    double rl_long;
    double rl_lat;
    double gsd; // in metres
} TaskInfo;


// void append(Point** node, Point* new) {
//     if( *node == 0 ) {
//         *node = malloc(sizeof(PointNode));
//         *node->p = *new;
//         *node->next = 0;
//     } else {
//         append(*node->
//     }
// }
// 
// void addPoint(PointNode* nodes, int rows, int cols, Point* p) {
//     int i = floor(p->x);
//     int j = floor(p->y);
//     append(&nodes[j][i], p);
// }
// 




/*
 * Long/lat's are projected to pixel centers.
 *
 * h = rows-1, w = cols-1
 * 
 * 
 *
 *
 *            LONG / LAT                 
 *        ^                                    0,0 |             w-1,0
 *    lat |    lu-----+                      ------+------------------
 *        |    |      |        ======>             |
 *        |    +-----rl                            |
 *        |                                        |
 *    ----+----------------->                0,h-1 |
 *    0,0 |             long                                           
 *                                              lu -> 0,0
 *        ^-- greenwich                         rl -> 
 *                                      
 */

void transform(TaskInfo *info, int *px, int *py) {
    *px = 
}

int main(int argc, char **argv) {
    printf("Jotain\n");

    TaskInfo info = { 24.0, 65.0, 25.0, 66.0, 0.05 };

    int rows=5, //9175,
        cols=5, //6814,
        i,
        j;

    PointNode* nodes[rows][cols]; // = malloc(rows*cols*sizeof(PointNode *));

    // nollaus
    for (j=0; j<rows; j++) {
        for (i=0; i<cols; i++) {
            nodes[j][i] = 0;
        }
    }

    // data
    Point points[5] = { 1.7, 2.7, 8 } 
            Point b = { 1.2, 2.2, 6 }
            Point c = { 1.4, 3.2, 5 }
            Point d = { 1.4, 3.2, 5 }
            Point e = { 2.5, 4.5, 3 }
    printf("Size: %d\n", sizeof(PointNode));
//
//    addPoint(nodes, rows, cols, &a);
//    addPoint(nodes, rows, cols, &b);
//    addPoint(nodes, rows, cols, &c);
//    addPoint(nodes, rows, cols, &d);
//    addPoint(nodes, rows, cols, &e);
//
    // FIXME: muistin vapautus

    return 0;
}
