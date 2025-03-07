#include <GL/freeglut.h>
#include <GL/glut.h>
#include <stdio.h>
#include "vector.h" 
#include <math.h>

#define POINT_COLOR 1.0f, 0.0f, 0.0f
#define LINE_COLOR 0.0f, 0.0f, 1.0f
#define POINT_COLOR_B 0.0f, 1.0f, 0.0f
#define LINE_COLOR_B 0.5f, 0.0f, 0.5f
#define POINT_COLOR_C 0.1f, 0.1f, 0.1f
#define LINE_COLOR_C 1.0f, 1.0f, 1.0
#define MAX_POINT_NUM 8
#define WIDTH 640
#define HEIGHT 480 
#define PATERN_SCALE 20

typedef struct {
   PointList list; 
   int mouseDown;
   Point newList[MAX_POINT_NUM];
   Point editedList[MAX_POINT_NUM];
   int isNewListEmpty;
   Point resolution;
} AppState;

void display(void);
void mouseFunc(int button, int state, int x, int y);
void motionFunc(int x, int y);
void reshape(int w, int h);
void appStateInit(AppState *state);
void resample(AppState *state);
void drawPointList(PointList list);
void drawPointArray(Point array[MAX_POINT_NUM]);
void drawPointArrayFromCenter(Point array[MAX_POINT_NUM], Point center);
float pointsDistance(Point a, Point b);
float pointListPathLength(PointList list);
void resample(AppState *state);
void printPointArray(Point array[MAX_POINT_NUM]);
Point getCenterPoint(Point array[MAX_POINT_NUM]);
Point getCenterPoint(Point array[MAX_POINT_NUM]);
void editNewList(AppState *state);
void printPoint(Point p);

int main(int argc, char **argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(WIDTH, HEIGHT);
   glutCreateWindow("runes");


   AppState *state = (AppState *)calloc(1, sizeof(AppState));
   appStateInit(state);
   glutSetWindowData(state);


   glutDisplayFunc(display);
   glutMouseFunc(mouseFunc);
   glutMotionFunc(motionFunc);
   glutReshapeFunc(reshape);

   glutMainLoop();
   return 0;
}

void mouseFunc(int button, int state, int x, int y) {
   AppState *stateVars = (AppState *)glutGetWindowData();
   if (state == GLUT_DOWN) {
      printf("Mouse button %d pressed at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 1;
      stateVars->isNewListEmpty = 1;
      if(stateVars->list.points != NULL){
	 free(stateVars->list.points);
	 stateVars->list = (PointList){NULL, 0};
      }
   } else if (state == GLUT_UP) {
      printf("Mouse button %d released at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 0;
      printPointList(stateVars->list);
      resample(stateVars);
      printPointArray(stateVars->newList);
      Point center = getCenterPoint(stateVars->newList);
      printPoint(center);

      editNewList(stateVars);
      
      glutPostRedisplay();
   }
}



void editNewList(AppState *state) {
    Point min = state->newList[0];
    Point max = state->newList[0];

    for (size_t i = 1; i < MAX_POINT_NUM; i++) {
        if (state->newList[i].x > max.x)
            max.x = state->newList[i].x;
        if (state->newList[i].y > max.y)
            max.y = state->newList[i].y;
        
        if (state->newList[i].x < min.x)
            min.x = state->newList[i].x;
        if (state->newList[i].y < min.y)
            min.y = state->newList[i].y;
    }
    
    float rangeX = (max.x - min.x);
    float rangeY = (max.y - min.y);
    if (rangeX == 0) rangeX = 1;
    if (rangeY == 0) rangeY = 1;

    float aspectRatio = (max.x - min.x) / (max.y - min.y);
    printf("%f\n",aspectRatio);

    for (size_t i = 0; i < MAX_POINT_NUM; i++) {
        state->editedList[i].x = (2 * (state->newList[i].x - min.x) / rangeX 
	      - 1) * PATERN_SCALE;
        state->editedList[i].y = (2 * (state->newList[i].y - min.y) / rangeY 
	      - 1) * PATERN_SCALE / aspectRatio;
    }
}

void motionFunc(int x, int y) {
   printf("Mouse dragged to (%d, %d)\n", x, y);
   AppState *state = (AppState *)glutGetWindowData();
   if (state->mouseDown) {
      appendPointList(&state->list, (Point){(float)x, (float)y});
   }

   glutPostRedisplay();
}

void drawPointArrayFromCenter(Point array[MAX_POINT_NUM], Point center){
   glColor3f(LINE_COLOR_C);
   glLineWidth(5.f);
   glBegin(GL_LINE_STRIP);
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      glVertex2f(array[i].x + center.x, array[i].y + center.y);
   }
   glEnd();

   glColor3f(POINT_COLOR_C);
   glPointSize(6.f);
   glBegin(GL_POINTS);
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      glVertex2f(array[i].x + center.x, array[i].y + center.y);
   }
   glEnd();
}

void display(void) {
   glClear(GL_COLOR_BUFFER_BIT);

   AppState *state = (AppState *)glutGetWindowData();


   if (state->list.points != NULL && state->list.size > 0)
      drawPointList(state->list);

   if (!state->isNewListEmpty)
      drawPointArray(state->newList);

   Point center = (Point){(int)state->resolution.x/2, 
      (int)state->resolution.y/2};

   if (!state->isNewListEmpty)
      drawPointArrayFromCenter(state->editedList, center);


   glutSwapBuffers();
}


void reshape(int w, int h) {
   AppState *state = (AppState *)glutGetWindowData();
   state->resolution = (Point){w, h};

   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, w, h, 0, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void appStateInit(AppState *state){
   state->list = (PointList){NULL, 0};
   state->mouseDown = 0;
   state->resolution = (Point){WIDTH, HEIGHT};
   
   //for (size_t i = 0; i < MAX_POINT_NUM; i++) {
   //   state->newList[i] = NULL;
   //}
   state->isNewListEmpty = 1;
}

void drawPointList(PointList list){
   glColor3f(LINE_COLOR);
   glLineWidth(2.f);
   glBegin(GL_LINE_STRIP);
   for (size_t i = 0; i < list.size; i++) {
      glVertex2f(list.points[i].x, list.points[i].y);
   }
   glEnd();

   glColor3f(POINT_COLOR);
   glPointSize(5.f);
   glBegin(GL_POINTS);
   for (size_t i = 0; i < list.size; i++) {
      glVertex2f(list.points[i].x, list.points[i].y);
   }
   glEnd();
}

void drawPointArray(Point array[MAX_POINT_NUM]){
   glColor3f(LINE_COLOR_B);
   glLineWidth(5.f);
   glBegin(GL_LINE_STRIP);
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      glVertex2f(array[i].x, array[i].y);
   }
   glEnd();

   glColor3f(POINT_COLOR_B);
   glPointSize(6.f);
   glBegin(GL_POINTS);
   for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      glVertex2f(array[i].x, array[i].y);
   }
   glEnd();
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

void resample(AppState *state) {
   PointList *list = &state->list;
   if (list->size == 0) return;

   float pathLength = pointListPathLength(*list);
   float stepLength = pathLength / (MAX_POINT_NUM - 1);

   float D = 0.0f;
   int count = 0;

   state->newList[count++] = list->points[0];
   Point prev = list->points[0];

   for (size_t i = 1; i < list->size && count < MAX_POINT_NUM; i++) {
      Point curr = list->points[i];
      float d = pointsDistance(prev, curr);

      while ((D + d) >= stepLength && count < MAX_POINT_NUM) {
	 float t = (stepLength - D) / d;

	 Point q;
	 q.x = prev.x + t * (curr.x - prev.x);
	 q.y = prev.y + t * (curr.y - prev.y);

	 state->newList[count++] = q;
	 prev = q;

	 d = pointsDistance(prev, curr);
	 D = 0.0f;
      }
      D += d;
      prev = curr;
   }
   state->newList[MAX_POINT_NUM - 1] = list->points[list->size - 1];
   state->isNewListEmpty = 0;
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
