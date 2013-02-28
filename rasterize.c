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

    Config config;

    int rows, //9175,
        cols, //6814,
        i,
        j;

    calcImageSize(&config, &rows, &cols);

    Index index;
    index.nodes = malloc(rows*cols*sizeof(PointNode *));
    index.config = &config;
    index.rows = rows;
    index.cols = cols;

    Image image;
    image.pixels = malloc(rows*cols*sizeof(float));
    image.rows = rows;
    image.cols = cols;
    image.config = &config;

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

int readConfig(char *filename, Config *config) {
    printf("Reading config from file: %s\n\n", filename);
    FILE *file = fopen(filename, "r");
    float rx,ry,rz;
    int n = 0;

    if ( fscanf(file, "lu_long=%d", &(config->lu_long)) != 1) {
        printf("Could not read lu_long from config.");
        return 1;
    }
    if ( fscanf(file, "lu_lat=%d", &(config->lu_lat)) != 1) {
        printf("Could not read lu_lat from config.");
        return 1;
    }
    if ( fscanf(file, "rl_long=%d", &(config->rl_long)) != 1) {
        printf("Could not read rl_long from config.");
        return 1;
    }
    if ( fscanf(file, "rl_lat=%d", &(config->rl_lat)) != 1) {
        printf("Could not read rl_lat from config.");
        return 1;
    }
    if ( fscanf(file, "gsd=%d", &(config->gsd)) != 1) {
        printf("Could not read gsd from config.");
        return 1;
    }

    fclose(file);

    printf("\nDone reading config.\n", n);
    printf(END_SECT);

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

double dist(double x1, double y1, double x2, double y2) {
    return sqrt( pow(x1-x2, 2) + pow(y1-y2, 2) );
}

int blockDist(int x1, int y1, int x2, int y2) {
    return ceil( sqrt( pow(x1-x2, 2) + pow(y1-y2, 2) ) );
}

Point3D* useNearest(Index *index, Point3D *pixel_rep) {
    Point3D *nearest = NULL;
    int i,j,
        d=1,
        row, col;

    point2Index(index->config, pixel_rep, &row, &col);

    int minj = MAX(0, row-d),
        maxj = MIN(row+d, index->rows - 1),
        mini = MAX(0, col-d),
        maxi = MIN(col+d, index->cols - 1);

    double shortestDist = 100000000000;

    // Go through relevant bins
    for (j = minj; j <= maxj; j++) {
        for (i = mini; i <= maxi; i++) {

            // Go through one bin
            PointNode *node = *(index->nodes+j*(index->cols)+i);
            while (node != NULL) {

                Point3D *current = &(node->p);
                double currentDist = dist(pixel_rep->x, pixel_rep->y, current->x, current->y);

                if ( shortestDist > currentDist ) {
                    nearest = current;
                    shortestDist = currentDist;
                }

                node = node->next;
            }
        }
    }
    printf("row=%d col=%d ... mini=%d maxi=%d minj=%d maxj=%d\n", row, col, mini, maxi, minj, maxj);

    return nearest;
}

void rasterize(Index *index, Image *image) {
    printf("Creating raster image from indexed points.\n");

    int i,j;
    for (j=0; j < index->rows; j++) {
        for (i=0; i < index->cols; i++) {
            Point3D pixel_rep;
            pixel2Point(image, &j, &i, &pixel_rep);
            Point3D *nearest = useNearest(index, &pixel_rep);
            if(nearest != NULL) {
                *(image->pixels+(j*index->cols)+i) = nearest->z;
            } else {
                *(image->pixels+(j*index->cols)+i) = EMPTY_VAL;
            }
        }
    }

    printf(END_SECT);
}

void calcImageSize(Config *config, int *rows, int *cols) {
    assert(config->lu_long < config->rl_long && "Longitudes switched.");
    assert(config->lu_lat  > config->rl_lat  && "Latitudes switched.");

    printf("Calculating image size ...\n");
    printf("    Image extents: left  upper corner %6.2f %6.2f\n", config->lu_long, config->lu_lat);
    printf("                   right lower corner %6.2f %6.2f\n", config->rl_long, config->rl_lat);
    printf("    GSD: %6.2f metres.\n\n", config->gsd);

    double hor = config->rl_long - config->lu_long;
    double ver = config->lu_lat  - config->rl_lat;

    *rows = floor(hor / config->gsd);
    *cols = floor(ver / config->gsd);

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
    point2Index(index->config, point, &row, &col);
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

void point2Index(Config *config, Point3D *p, int *row, int *col) {
    *row = floor((p->x - config->lu_long) / config->gsd);
    *col = floor((p->y - config->rl_lat) / config->gsd);
    assert(*row >= 0 && *col >= 0 && "Row number positive.");
    // printf("    Point x=%4.2f y=%4.2f z=%4.2f -> row=%d col=%d.\n", p->x, p->y, p->z, *row, *col);
}

void pixel2Point(Image *image, int *row, int *col, Point3D *point) {
    float col_rel = (float)*col / image->cols;
    float row_rel = (float)*row / image->rows;
    Config *config = image->config;
    point->x = config->lu_long + (config->rl_long - config->lu_long)*col_rel;
    point->y = config->lu_lat  + (config->rl_lat  - config->lu_lat )*row_rel;
    point->z = EMPTY_VAL;
    printf("pixel2Point: col=%d row=%d -> x=%f y=%f\n", *col, *row, point->x, point->y);
}
