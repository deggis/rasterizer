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

// void append(Point** node, Point* new) {
//     if( *node == 0 ) {
//         *node = malloc(sizeof(PointNode));
//         *node->p = *new;
//         *node->next = 0;
//     } else {
//         append(*node->
//     }
// }
#include "rasterize.h"

int main(int argc, char **argv) {
    printf("Jotain\n");

    TaskInfo info = { 24.0, 65.0, 25.0, 66.0, 0.05 };

    int rows=5, //9175,
        cols=5, //6814,
        i,
        j;

    Index index;
    index.nodes = malloc(rows*cols*sizeof(PointNode *));
    index.info = info;

    Image image;
    image.pixels = malloc(rows*cols*sizeof(float));
    image.rows = rows;
    image.cols = cols;

    // nollaus
    for (j=0; j<rows; j++) {
        for (i=0; i<cols; i++) {
            *(index.nodes+(j*cols)+i) = 0;
            *(image.pixels+(j*cols)+i) = EMPTY_VAL;
        }
    }

    // data
    Point3D a = { 1.7, 2.7, 8 }; 
    Point3D b = { 1.2, 2.2, 6 };
    Point3D c = { 1.4, 3.2, 5 };
    Point3D d = { 1.4, 3.2, 5 };
    Point3D e = { 2.5, 4.5, 3 };

//    addPoint(nodes, rows, cols, &a);
//    addPoint(nodes, rows, cols, &b);
//    addPoint(nodes, rows, cols, &c);
//    addPoint(nodes, rows, cols, &d);
//    addPoint(nodes, rows, cols, &e);

    // FIXME: muistin vapautus

    printImage(image);

    free(image.pixels);
    free(index.nodes);
    printf("Float %d", sizeof(float));
    return 0;
}

void printImage(Image im) {
    int i,j;
    for (j=0; j<im.rows; j++) {
        for (i=0; i<im.cols; i++) {
            printf("%04.2f  ", *(im.pixels+(j*im.cols)+im.rows));
        }
        printf("\n");
    }
}

void addPoint(Index index, Point3D *point) {
//    int i = floor(p->x);
//    int j = floor(p->y);
//    append(&nodes[j][i], p);
}

// void transform(TaskInfo info, Point3D *p, int *row, int *col) {
//     printf("asda");
// }
