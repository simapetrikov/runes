#include <GL/glut.h>
#include <stdio.h>
#include "vector.h" 

void display(void);
void mouseFunc(int button, int state, int x, int y);
void motionFunc(int x, int y);

int main(int argc, char **argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize(640, 480);
   glutCreateWindow("GLUT Input Example");

   glutDisplayFunc(display);

   glutMouseFunc(mouseFunc);
   glutMotionFunc(motionFunc);


   PointList list = (PointList){NULL, 1};

   initPointList(&list);

   list.points[0] = (Point){.1f, .1f};

   resizePointList(&list, list.size + 1);

   list.points[1] = (Point){.2f, .1f};

   appendPointList(&list, (Point){.3f, .1f});
   appendPointList(&list, (Point){.4f, .1f});
   

   removePoint(&list, 2);

   appendPointList(&list, (Point){.5f, .1f});

   replacePoint(&list, 3, (Point){.6f, .1f});
   
   printPointList(list);

   free(list.points);

   //glutMainLoop();
   return 0;
}



void display(void) {
   glClear(GL_COLOR_BUFFER_BIT);
   glutSwapBuffers();
}

void mouseFunc(int button, int state, int x, int y) {
   if (state == GLUT_DOWN) {
      printf("Mouse button %d pressed at (%d, %d)\n", button, x, y);
   } else if (state == GLUT_UP) {
      printf("Mouse button %d released at (%d, %d)\n", button, x, y);
   }
}

void motionFunc(int x, int y) {
   printf("Mouse dragged to (%d, %d)\n", x, y);
}

