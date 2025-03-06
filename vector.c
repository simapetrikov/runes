#include "vector.h"

int initPointList(PointList *list) {
   list->points = (Point *)calloc(list->size, sizeof(Point));
   return list->points ? 0 : 1;
}

int resizePointList(PointList *list, size_t new_size) {
   Point *temp = (Point*)realloc(list->points, new_size * sizeof(Point));
   if (temp) {
      list->points = temp;
      list->size = new_size;
      return 0;
   }
   return 1;
}

int appendPointList(PointList *list, Point new_point) {
   if (resizePointList(list, list->size + 1) == 0) {
      list->points[list->size - 1] = new_point;
      return 0;
   }
   return 1;
}

int replacePoint(PointList *list, size_t index, Point new_point) {
   if (index >= list->size) return 1;
   list->points[index] = new_point;
   return 0;
}

int removePoint(PointList* list, size_t index) {
   if (index >= list->size) return 1;
   for (size_t i = index; i < list->size - 1; ++i) {
      list->points[i] = list->points[i + 1];
   }
   return resizePointList(list, list->size - 1);
}

void printPointList(PointList list) {
   for (size_t i = 0; i < list.size; i++) {
      printf("(%.2f, %.2f) ", list.points[i].x, list.points[i].y);
   }
   printf("\n");
}
