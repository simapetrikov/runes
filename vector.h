#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
   float x, y;
} Point;

typedef struct {
   Point *points;
   size_t size;
} PointList;

int initPointList(PointList *list);
int resizePointList(PointList *list, size_t new_size);
int appendPointList(PointList *list, Point new_point);
int replacePoint(PointList *list, size_t index, Point new_point);
int removePoint(PointList* list, size_t index);
void printPointList(PointList list);

#endif
