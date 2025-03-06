#include <GL/freeglut.h>
#include <GL/glut.h>
#include <stdio.h>
#include "vector.h" 

#define POINT_COLOR 1.0f, 0.0f, 0.0f
#define LINE_COLOR 0.0f, 0.0f, 1.0f

typedef struct {
   PointList list;
   int mouseDown;
} AppState;

void display(void);
void mouseFunc(int button, int state, int x, int y);
void motionFunc(int x, int y);
void reshape(int w, int h);

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
   glutReshapeFunc(reshape);

   glutMainLoop();
   return 0;
}


void mouseFunc(int button, int state, int x, int y) {
   AppState *stateVars = (AppState *)glutGetWindowData();
   if (state == GLUT_DOWN) {
      printf("Mouse button %d pressed at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 1;
      if(stateVars->list.points != NULL){
	 free(stateVars->list.points);
	 stateVars->list = (PointList){NULL, 0};
      }
   } else if (state == GLUT_UP) {
      printf("Mouse button %d released at (%d, %d)\n", button, x, y);
      stateVars->mouseDown = 0;
      printPointList(stateVars->list);
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

void display(void) {
   glClear(GL_COLOR_BUFFER_BIT);
   
   AppState *state = (AppState *)glutGetWindowData();
   

   if (state->list.points != NULL && state->list.size > 0) {
      glColor3f(LINE_COLOR);
      glPointSize(2.f);
      glBegin(GL_LINE_STRIP);
      for (size_t i = 0; i < state->list.size; i++) {
	 glVertex2f(state->list.points[i].x, state->list.points[i].y);
      }
      glEnd();

      glColor3f(POINT_COLOR);
      glPointSize(5.f);
      glBegin(GL_POINTS);
      for (size_t i = 0; i < state->list.size; i++) {
	 glVertex2f(state->list.points[i].x, state->list.points[i].y);
      }
      glEnd();
   }

   glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
