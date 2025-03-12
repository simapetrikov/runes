#include <GL/freeglut.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"

#define POINT_COLOR 1.0f, 0.0f, 0.0f
#define LINE_COLOR 0.0f, 0.0f, 1.0f
#define POINT_COLOR_B 0.0f, 1.0f, 0.0f
#define LINE_COLOR_B 0.5f, 0.0f, 0.5f
#define POINT_COLOR_C 0.1f, 0.1f, 0.1f
#define LINE_COLOR_C 1.0f, 1.0f, 1.0
#define WIDTH 640
#define HEIGHT 480
#define PATERN_SCALE 20
#define DEBUG 0
#define FILENAME "patterns"
#define MAX_PATTERNS 100

typedef struct {
  int appMode;
  int namingMode;
  PointList list;
  int mouseDown;
  Point newList[MAX_POINT_NUM];
  Point normalizedList[MAX_POINT_NUM];
  int isNewListEmpty;
  Point resolution;
  int check;
  Point patern[MAX_POINT_NUM];
  char patternName[256];
  Point patterns[MAX_PATTERNS][MAX_POINT_NUM];
  char patternNames[MAX_PATTERNS][256];
  int patternsCount;
} AppState;

void display(void);
void mouseFunc(int button, int state, int x, int y);
void motionFunc(int x, int y);
void reshape(int w, int h);
void keyboardfunc(unsigned char key, int x, int y);

void appStateInit(AppState *state);
void normalizeNewList(Point array[MAX_POINT_NUM],
                      Point normalizedArray[MAX_POINT_NUM]);

void drawPointList(PointList list);
void drawPointArray(Point array[MAX_POINT_NUM]);
void drawPointArrayFromCenter(Point array[MAX_POINT_NUM], Point center);
void drawPatternName(const char *name);
void savePattern(const char *patternName, Point array[MAX_POINT_NUM]);
void loadPatterns(Point patterns[MAX_PATTERNS][MAX_POINT_NUM], 
    char patternNames[MAX_PATTERNS][256], int *patternsCount);

int main(int argc, char **argv) {
  if (argc != 2 || !(argv[1][0] == '0' || argv[1][0] == '1')) return -1;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("runes");

  AppState *state = (AppState *)calloc(1, sizeof(AppState));
  appStateInit(state);
  glutSetWindowData(state);

  glutDisplayFunc(display);
  glutMouseFunc(mouseFunc);
  glutKeyboardFunc(keyboardfunc);
  glutMotionFunc(motionFunc);
  glutReshapeFunc(reshape);

  if (argv[1][0] == '0')
    state->appMode = 0;
  else
    state->appMode = 1;

  if (state->appMode) {
    loadPatterns(state->patterns, state->patternNames, &state->patternsCount);
  }

  glutMainLoop();
  return 0;
}

void mouseFunc(int button, int state, int x, int y) {
  AppState *stateVars = (AppState *)glutGetWindowData();
  if (state == GLUT_DOWN) {
    if (DEBUG) printf("Mouse button %d pressed at (%d, %d)\n", button, x, y);
    stateVars->mouseDown = 1;
    stateVars->isNewListEmpty = 1;
    if (stateVars->list.points != NULL) {
      free(stateVars->list.points);
      stateVars->list = (PointList){NULL, 0};
    }
  } else if (state == GLUT_UP) {
    if (DEBUG) printf("Mouse button %d released at (%d, %d)\n", button, x, y);

    stateVars->mouseDown = 0;

    if (DEBUG) printPointList(stateVars->list);

    resample(stateVars->list, stateVars->newList);
    stateVars->isNewListEmpty = 0;

    normalizeNewList(stateVars->newList, stateVars->normalizedList);


    if (DEBUG) {
      printf("\n");
      printPointArray(stateVars->normalizedList);
    }

    glutPostRedisplay();
  }
}

void keyboardfunc(unsigned char key, int x, int y) {
  AppState *state = (AppState *)glutGetWindowData();
  if (state == NULL) return;

  if (key == 'q' || key == 'Q' || key == 27) glutLeaveMainLoop();

  if (state->namingMode && !state->mouseDown && !state->appMode) {
    int len = strlen(state->patternName);
    if (key == 13) {
      state->namingMode = 0;
      if (DEBUG) printf("%s\n", state->patternName);
      savePattern(state->patternName, state->normalizedList);
    } else if (key == 8 || key == 127) {
      if (len > 0) state->patternName[len - 1] = '\0';
    } else {
      if (len < (int)sizeof(state->patternName) - 1) {
        state->patternName[len] = key;
        state->patternName[len + 1] = '\0';
      }
    }
  }

  if (!state->namingMode && !state->mouseDown && (key == 'S' || key == 's') &&
      !state->isNewListEmpty && !state->appMode) {
    state->namingMode = 1;
    state->patternName[0] = '\0';
  }

  glutPostRedisplay();
}

void normalizeNewList(Point array[MAX_POINT_NUM],
                      Point normalizedArray[MAX_POINT_NUM]) {
  Point min = getMinPoint(array);
  Point max = getMaxPoint(array);

  Point range = getRange(min, max);

  float aspectRatio = (max.x - min.x) / (max.y - min.y);

  for (size_t i = 0; i < MAX_POINT_NUM; i++) {
    normalizedArray[i].x =
        (2 * (array[i].x - min.x) / range.x - 1) * PATERN_SCALE;
    normalizedArray[i].y =
        (2 * (array[i].y - min.y) / range.y - 1) * PATERN_SCALE / aspectRatio;
  }
}

void motionFunc(int x, int y) {
  if (DEBUG) printf("Mouse dragged to (%d, %d)\n", x, y);
  AppState *state = (AppState *)glutGetWindowData();
  if (state->mouseDown) {
    appendPointList(&state->list, (Point){(float)x, (float)y});
  }
  glutPostRedisplay();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  AppState *state = (AppState *)glutGetWindowData();

  if (state->namingMode) drawPatternName(state->patternName);

  if (state->list.points != NULL && state->list.size > 0)
    drawPointList(state->list);

  if (DEBUG) {
    if (!state->isNewListEmpty) drawPointArray(state->newList);

    Point center =
        (Point){(int)state->resolution.x / 2, (int)state->resolution.y / 2};

    if (!state->isNewListEmpty)
      drawPointArrayFromCenter(state->normalizedList, center);
  }

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

void appStateInit(AppState *state) {
  state->list = (PointList){NULL, 0};
  state->mouseDown = 0;
  state->resolution = (Point){WIDTH, HEIGHT};
  state->check = 0;

  state->isNewListEmpty = 1;
}

void drawPointList(PointList list) {
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

void drawPointArrayFromCenter(Point array[MAX_POINT_NUM], Point center) {
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

void drawPointArray(Point array[MAX_POINT_NUM]) {
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

void drawPatternName(const char *name) {
  glColor3f(1.0f, 1.0f, 1.0f);
  glRasterPos2i(10, 20);
  char prefix[] = "Pattern Name: ";
  for (const char *c = prefix; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }
  for (const char *c = name; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }
}

void savePattern(const char *patternName, Point array[MAX_POINT_NUM]) {
  FILE *file = fopen(FILENAME, "a");
  if (!file) return;

  fprintf(file, "%s\n", patternName);

  for (size_t i = 0; i < MAX_POINT_NUM; i++) {
    fprintf(file, "%f, %f\n", array[i].x, array[i].y);
  }
  fclose(file);
  if (DEBUG) printf("Pattern '%s' saved\n", patternName);
}

void loadPatterns(Point patterns[MAX_PATTERNS][MAX_POINT_NUM], 
    char patternNames[MAX_PATTERNS][256], int *patternsCount){

  FILE *file = fopen(FILENAME, "r");
  if (!file) return;

  *patternsCount = 0;
  char line[256];
  while (fgets(line, sizeof(line), file) && *patternsCount < MAX_PATTERNS) {
    line[strcspn(line, "\r\n")] = 0;
    strcpy(patternNames[*patternsCount], line);
    for (size_t i = 0; i < MAX_POINT_NUM; i++) {
      if (!fgets(line, sizeof(line), file)) break;
      Point a;
      if (sscanf(line, "%f, %f", &a.x, &a.y) == 2) {
        patterns[*patternsCount][i] = a;
      } else {
        patterns[*patternsCount][i] = (Point){0, 0};
      }
    }
    (*patternsCount)++;
  }
  fclose(file);

  if (DEBUG) {
    printf("Loaded %d patterns from file.\n", *patternsCount);

    for (size_t i = 0; i < *patternsCount; i++) {
      printf("%s: \n", patternNames[i]);
      printPointArray(patterns[i]);
    }
  }
}
