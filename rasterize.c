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
 *        |    |a     |        ======>             |
 *        |    |     b|                            |
 *        |    +-----rl                            |
 *        |                                        |
 *    ----+----------------->                0,h-1 |
 *    0,0 |             long                                           
 *                                              lu -> 0,0
 *        ^-- greenwich                         rl -> 
 *                                      
 */

#include "rasterize.h"

#define END_SECT "\n\n"

int main(int argc, char **argv) {
    printf("\n");
    printf("Welcome to Rasterizer 3000.\n");
    printf("===========================\n\n");

    TaskInfo info = { 0.0, 5.0, 5.0, 0.0, 1.0 };

    int rows, //9175,
        cols, //6814,
        i,
        j;

    calcImageSize(info, &rows, &cols);

    Index index;
    index.nodes = malloc(rows*cols*sizeof(PointNode *));
    index.info = info;
    index.rows = rows;
    index.cols = cols;

    Image image;
    image.pixels = malloc(rows*cols*sizeof(float));
    image.rows = rows;
    image.cols = cols;

    // Initialize arrays
    for (j=0; j<rows; j++) {
        for (i=0; i<cols; i++) {
            *(index.nodes+(j*cols)+i) = 0;
            *(image.pixels+(j*cols)+i) = EMPTY_VAL;
        }
    }

    // data
    readPointsFromFile("sample.txt", &index);

    printIndexBins(&index);

    rasterize(&index, &image);

    printImage(&image);

    free(image.pixels);
    free(index.nodes);
    return 0;
}

// float valueUsingKNN(Index *index, int *row, int *col) {
//     int k = 5; // k-max
//     int k_used = 0;
//     float nearest[5];
// 
//     int i;
//     float ksum = 0f;
//     for(i=0; i<k_used; i++) {
//         ksum += nearest[i];
//     }
//     return ksum/(float)k_used;
// }

float useNearest(Index *index, int *row, int *col) {
    printf("Not implemented!\n");
    return EMPTY_VAL;    
}

void rasterize(Index *index, Image *image) {
    printf("Creating raster image from indexed points.\n");

    int i,j;
    for (j=0; j < index->rows; j++) {
        for (i=0; i < index->cols; i++) {
            *(image->pixels+(j*index->cols)+i) = useNearest(index, &j, &i);
        }
    }

    printf(END_SECT);
}

void calcImageSize(TaskInfo info, int *rows, int *cols) {
    assert(info.lu_long < info.rl_long && "Longitudes switched.");
    assert(info.lu_lat  > info.rl_lat  && "Latitudes switched.");

    printf("Calculating image size ...\n");
    printf("    Image extents: left  upper corner %6.2f %6.2f\n", info.lu_long, info.lu_lat);
    printf("                   right lower corner %6.2f %6.2f\n", info.rl_long, info.rl_lat);
    printf("    GSD: %6.2f metres.\n\n", info.gsd);

    double hor = info.rl_long - info.lu_long;
    double ver = info.lu_lat  - info.rl_lat;

    *rows = floor(hor / info.gsd);
    *cols = floor(ver / info.gsd);

    printf("    Horizontal extent: %6.2f metres => %d cols.\n", hor, *cols);
    printf("    Vertical   extent: %6.2f metres => %d rows.\n", ver, *rows);
    printf(END_SECT);
}

int binDepth(int d, PointNode *node) {
    if(node == 0) {
        return d;
    } else {
        return binDepth(d+1, node->next);
    }
}

// Prints out Index struct's bin usages.
void printIndexBins(Index *index) {
    printf("Printing out index bin depths (number of links per bin):\n\n");

    int i,j,d;
    for (j=0; j<index->rows; j++) {
        for (i=0; i<index->cols; i++) {
            d = binDepth(0, *(index->nodes + index->cols*j + i));
            printf("%d  ", d);
        }
        printf("\n");
    }

    printf(END_SECT);
}

void printImage(Image *image) {
    printf("Printing rasterized image:\n\n");

    int i,j;
    for (j=0; j<image->rows; j++) {
        for (i=0; i<image->cols; i++) {
            printf("%04.2f  ", *(image->pixels+(j*image->cols)+i));
        }
        printf("\n");
    }

    printf(END_SECT);
}

void readPointsFromFile(char *filename, Index *index) {
    printf("Reading points from file: %s\n\n", filename);
    FILE *file = fopen(filename, "r");
    float rx,ry,rz;
    int n = 0;

    int valuesToRead = 3;
    while (fscanf(file, "%f %f %f", &rx, &ry, &rz) == valuesToRead) {
        Point3D p;
        p.x = rx;
        p.y = ry;
        p.z = rz;
        addPointToIndex(index, &p);

        if(n <= 10) {
            printf("x=%04.2f y=%04.2f z=%04.2f\n", rx, ry, rz);
        } else if (n == 11) {
            printf("...\n");
        } else {
            // Gradually increase shown amounts.
            if (n <= 100 && n % 10 == 0) { printf("%d.. ", n); }
            else if (n > 100    && n <= 1000    && n % 100    == 0) { printf("%d.. ", n); }
            else if (n > 1000   && n <= 10000   && n % 1000   == 0) { printf("%d.. ", n); }
            else if (n > 10000  && n <= 100000  && n % 10000  == 0) { printf("%d.. ", n); }
            else if (n > 100000 && n <= 1000000 && n % 100000 == 0) { printf("%d.. ", n); }
        }
        n++;
    }

    fclose(file);

    printf("\nDone reading %d points to index.\n", n);

    printf(END_SECT);
}



void addPointToIndex(Index *index, Point3D *point) {
    int col,row;
    transformPoint(index->info, point, &row, &col);
    assert(row < (index->rows) && col < (index->cols) && "col,row < cols,rows");
    PointNode **binstart = (index->nodes)+(row*index->cols)+col;
    addToBin(binstart, point);
}

void addToBin(PointNode **current, Point3D* point) {
    PointNode *new = malloc(sizeof(PointNode));
    (new->p).x = point->x;
    (new->p).y = point->y;
    (new->p).z = point->z;
    new->next = NULL;

    if( *current == NULL ) {
        *current = new;
    } else {
        PointNode *i = *current;
        while(1) {
            if (i->next == NULL) break;
            i = i->next;
        }
        i->next = new;
    }
}

void transformPoint(TaskInfo info, Point3D *p, int *row, int *col) {
    *row = floor((p->x - info.lu_long) / info.gsd);
    *col = floor((p->y - info.rl_lat) / info.gsd);
    assert(*row >= 0 && *col >= 0 && "Row number positive.");
    // printf("    Point x=%4.2f y=%4.2f z=%4.2f -> row=%d col=%d.\n", p->x, p->y, p->z, *row, *col);
}
