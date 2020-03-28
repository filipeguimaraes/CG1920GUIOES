

#include<stdio.h>
#include<stdlib.h>

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>

#include <IL/il.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else

#include <GL/glew.h>
#include <GL/glut.h>

#endif


float camX = 00, camY = 30, camZ = 40;
int startX, startY, tracking = 0;

int alpha = 0, beta = 45, r = 50;

unsigned int t, tw, th;
unsigned char *imageData;

int color = 0;
float *vertexB;
GLuint buffers[1];

void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if (h == 0)
        h = 1;

    // compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective
    gluPerspective(45, ratio, 1, 1000);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

float h(int x, int z) {
    float height = 0;
    float escala = 30.0 / 255;
    int xx = x + 128;
    int zz = z + 128;
    height = (float) imageData[xx * zz] * escala;

    return height;
}

void prepareData() {
    glewInit();
    glEnableClientState(GL_VERTEX_ARRAY);
    glGenBuffers(1, buffers);

    vertexB = (float *) malloc(256 * 256 * 9 * sizeof(float));
    int indice = 0;
    float posicaox = -128;
    float posicaoz = -128;


    for (int i = 0; i < th; ++i) {

        vertexB[indice] = posicaox;
        vertexB[indice + 1] = h(posicaox, posicaoz);
        vertexB[indice + 2] = posicaoz;
        indice += 3;

        for (int j = 0; j < tw; ++j) {
            vertexB[indice] = posicaox;
            vertexB[indice + 1] = h(posicaox, posicaoz + 1);
            vertexB[indice + 2] = posicaoz + 1;
            indice += 3;

            vertexB[indice] = posicaox + 1;
            vertexB[indice + 1] = h(posicaox + 1, posicaoz);
            vertexB[indice + 2] = posicaoz;
            indice += 3;

            posicaox++;
        }

        vertexB[indice] = posicaox;
        vertexB[indice + 1] = h(posicaox, posicaoz + 1);
        vertexB[indice + 2] = posicaoz + 1;
        indice += 3;

        posicaox = -128;
        posicaoz++;
    }


    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, indice * sizeof(float), vertexB, GL_STATIC_DRAW);

    free(vertexB);
}

void drawTerrain() {
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glColor3f(0.42, 0.57, 0.6);
    for (int i = 0; i < th - 1; i++) {
        glDrawArrays(GL_TRIANGLE_STRIP, (tw) * 2 * i, (tw) * 2);
    }
}


void renderScene(void) {

    float pos[4] = {-1.0, 1.0, 1.0, 0.0};

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
              0.0, 0.0, 0.0,
              0.0f, 1.0f, 0.0f);

    drawTerrain();

    // just so that it renders something before the terrain is built
    // to erase when the terrain is ready
    //glutWireTeapot(2.0);

// End of frame
    glutSwapBuffers();
}


void processKeys(unsigned char key, int xx, int yy) {

// put code to process regular keys in here
}


void processMouseButtons(int button, int state, int xx, int yy) {

    if (state == GLUT_DOWN) {
        startX = xx;
        startY = yy;
        if (button == GLUT_LEFT_BUTTON)
            tracking = 1;
        else if (button == GLUT_RIGHT_BUTTON)
            tracking = 2;
        else
            tracking = 0;
    } else if (state == GLUT_UP) {
        if (tracking == 1) {
            alpha += (xx - startX);
            beta += (yy - startY);
        } else if (tracking == 2) {

            r -= yy - startY;
            if (r < 3)
                r = 3.0;
        }
        tracking = 0;
    }
    glutPostRedisplay();
}


void processMouseMotion(int xx, int yy) {

    int deltaX, deltaY;
    int alphaAux, betaAux;
    int rAux;

    if (!tracking)
        return;

    deltaX = xx - startX;
    deltaY = yy - startY;

    if (tracking == 1) {


        alphaAux = alpha + deltaX;
        betaAux = beta + deltaY;

        if (betaAux > 85.0)
            betaAux = 85.0;
        else if (betaAux < -85.0)
            betaAux = -85.0;

        rAux = r;
    } else if (tracking == 2) {

        alphaAux = alpha;
        betaAux = beta;
        rAux = r - deltaY;
        if (rAux < 3)
            rAux = 3;
    }
    camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    camY = rAux * sin(betaAux * 3.14 / 180.0);
}


void loadImage() {
    ilGenImages(1, &t);
    ilBindImage(t);

    // terreno.jpg is the image containing our height map
    ilLoadImage((ILstring) "terreno.jpg");

    // convert the image to single channel per pixel
    // with values ranging between 0 and 255
    ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

    // important: check tw and th values
    // both should be equal to 256
    // if not there was an error loading the image
    // most likely the image could not be found
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    if (tw != 256 || th != 256) printf("Erro ao carregar a imagem!\n");

    // imageData is a LINEAR array with the pixel values
    imageData = ilGetData();
}

void init() {
    ilInit();

// 	Load the height map "terreno.jpg"
    loadImage();

// 	Build the vertex arrays
    prepareData();

// 	OpenGL settings
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
}


int main(int argc, char **argv) {

// init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(320, 320);
    glutCreateWindow("CG@DI-UM");


// Required callback registry 
    glutDisplayFunc(renderScene);
    //glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);


    init();

// enter GLUT's main cycle
    glutMainLoop();

    return 0;
}

