#include "vector.h"


int resizePointList(PointList *list, size_t new_size) {
   int error = 0;
      Point *temp = (Point*)realloc(list->points, new_size * sizeof(Point));
   if (!temp) {
      error = 1;
   }
   else {
      list->points = temp;
      list->size = new_size;
   }
   return error;
}

int appendPointList(PointList *list, Point new_point) {
   if (list == NULL)
      return 1;

   int error = 0;

   if (resizePointList(list, list->size + 1) == 0)
      list->points[list->size - 1] = new_point;
   else
      error = 1;

   return error;
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
