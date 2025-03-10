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


float pointsDistance(Point a, Point b){
   return sqrt(pow(a.x - b.x, 2) +
	 pow(a.y - b.y, 2));
}

float pointListPathLength(PointList list){
   float sum = 0;
   if(list.size > 1){
      for (size_t i = 1; i < list.size; i++) {
	 sum += pointsDistance(list.points[i-1], list.points[i]);
      }
   }
   return sum;
}


Point getCenterPoint(Point array[MAX_POINT_NUM]){
   Point center = (Point){0, 0};
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      center.x += array[i].x;
      center.y += array[i].y;
   }
   center.x /= MAX_POINT_NUM;
   center.y /= MAX_POINT_NUM;

   return center;
}

Point getMaxPoint(Point array[MAX_POINT_NUM]){
   Point max = array[0];

   for (size_t i = 1; i < MAX_POINT_NUM; i++) {
      if (array[i].x > max.x)
	 max.x = array[i].x;
      if (array[i].y > max.y)
	 max.y = array[i].y;
   }
   return max;
}

Point getMinPoint(Point array[MAX_POINT_NUM]){
   Point min = array[0];

   for (size_t i = 1; i < MAX_POINT_NUM; i++) {
      if (array[i].x < min.x)
	 min.x = array[i].x;
      if (array[i].y < min.y)
	 min.y = array[i].y;
   }
   return min;
}

Point getRange(Point min, Point max) {
   Point range;
   range.x = (max.x - min.x) != 0 ? (max.x - min.x) : 1;
   range.y = (max.y - min.y) != 0 ? (max.y - min.y) : 1;
   return range;
}


void printPoint(Point p){
      printf("(%.2f, %.2f)\n", p.x, p.y);
}

void printPointArray(Point array[MAX_POINT_NUM]){
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      printf("(%.2f, %.2f) ", array[i].x, array[i].y);
   }
   printf("\n");
}

void printPointList(PointList list) {
   for (size_t i = 0; i < list.size; i++) {
      printf("(%.2f, %.2f) ", list.points[i].x, list.points[i].y);
   }
   printf("\n");
}

void copyArray(Point a[MAX_POINT_NUM], Point b[MAX_POINT_NUM]){
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      a[i].x = b[i].x;
      a[i].y = b[i].y;
   }
}

void resample(PointList list, Point array[MAX_POINT_NUM]) {
   if (list.size == 0) return;

   float pathLength = pointListPathLength(list);
   float stepLength = pathLength / (MAX_POINT_NUM - 1);

   float D = 0.0f;
   int count = 0;

   array[count++] = list.points[0];
   Point prev = list.points[0];

   for (size_t i = 1; i < list.size && count < MAX_POINT_NUM; i++) {
      Point curr = list.points[i];
      float d = pointsDistance(prev, curr);

      while ((D + d) >= stepLength && count < MAX_POINT_NUM) {
	 float t = (stepLength - D) / d;

	 Point q;
	 q.x = prev.x + t * (curr.x - prev.x);
	 q.y = prev.y + t * (curr.y - prev.y);

	 array[count++] = q;
	 prev = q;

	 d = pointsDistance(prev, curr);
	 D = 0.0f;
      }
      D += d;
      prev = curr;
   }
   array[MAX_POINT_NUM - 1] = list.points[list.size - 1];
}

float recognize(Point gesture[MAX_POINT_NUM], Point patern[MAX_POINT_NUM]){
   float diff = 0;
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      diff += pointsDistance(gesture[i], patern[i]);
   }

   float avr_diff = diff / MAX_POINT_NUM;

   Point min = getMinPoint(gesture);
   Point max = getMaxPoint(gesture);

   Point range = getRange(min, max);

   float square = sqrt(range.x * range.x + range.y * range.y);
   float score = 1 - (2 * avr_diff/square);

   return score;
}
