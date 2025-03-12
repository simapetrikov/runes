#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_POINT_NUM 16

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
int removePoint(PointList *list, size_t index);

float pointsDistance(Point a, Point b);
float pointListPathLength(PointList list);

Point getCenterPoint(Point array[MAX_POINT_NUM]);
Point getMaxPoint(Point array[MAX_POINT_NUM]);
Point getMinPoint(Point array[MAX_POINT_NUM]);
Point getRange(Point min, Point max);

void printPoint(Point p);
void printPointArray(Point array[MAX_POINT_NUM]);
void printPointList(PointList list);

void copyArray(Point a[MAX_POINT_NUM], Point b[MAX_POINT_NUM]);

void resample(PointList list, Point array[MAX_POINT_NUM]);
float recognize(Point a[MAX_POINT_NUM], Point b[MAX_POINT_NUM]);

#endif
