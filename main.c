#include <GL/freeglut.h>
#include <GL/glut.h>
#include <stdio.h>
#include "vector.h" 

typedef struct {
   PointList list;
   int mouseDown;
} AppState;

void display(void);
void mouseFunc(int button, int state, int x, int y);
void motionFunc(int x, int y);

int main(int argc, char **argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(640, 480);
   glutCreateWindow("GLUT Input Example");


   AppState *state = (AppState *)calloc(1, sizeof(AppState));
   state->list = (PointList){NULL, 0};
   state->mouseDown = 0;

   glutSetWindowData(state);


   glutDisplayFunc(display);
   glutMouseFunc(mouseFunc);
   glutMotionFunc(motionFunc);

   glutMainLoop();
   return 0;
}



void display(void) {
   glClear(GL_COLOR_BUFFER_BIT);
   glutSwapBuffers();
}

void mouseFunc(int button, int state, int x, int y) {
   AppState *stateVars = (AppState *)glutGetWindowData();
   if (state == GLUT_DOWN) {
      printf("Mouse button %d pressed at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 1;
   } else if (state == GLUT_UP) {
      printf("Mouse button %d released at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 0;
      printPointList(stateVars->list);
      free(stateVars->list.points);
      stateVars->list = (PointList){NULL, 0};
   }
}

void motionFunc(int x, int y) {
   printf("Mouse dragged to (%d, %d)\n", x, y);
   AppState *state = (AppState *)glutGetWindowData();
   if (state->mouseDown) {
      appendPointList(&state->list, (Point){(float)x, (float)y});
   }
}
