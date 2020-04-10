
#include<stdio.h>
#include<stdlib.h>

#define _USE_MATH_DEFINES

#include <math.h>
#include <time.h>
#include <vector>

#include "IL/il.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else

#include <GL/glew.h>
#include <GL/glut.h>

#endif

float normalizar = 0;
float camX = 0, camY = 5, camZ = 0;
float angle = 0;
float l1 = camX + sin(angle),
        l2 = camY,
        l3 = camZ + cos(angle);

int startX, startY, tracking = 0;

int alpha = 0, beta = 45, r = 50;

unsigned int t, tw, th;
unsigned char *imageData;

float alturaMaxima = 100.0;
int color = 0;
float *vertexB;
GLuint buffers[1];

int numberTrees = 600;
float *xTreePosicion;
float *yTreePosicion;
float *zTreePosicion;


struct Vetor {
    float x;
    float y;
    float z;
}u,v,k;

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
    float escala = alturaMaxima / 255;
    int xx = x + 128;
    int zz = z + 128;
    if (xx <= 255 && xx >= 0 && zz <= 255 && zz >= 0)
        height = (float) imageData[xx + (zz * tw)] * escala;
    else printf("Excedeu os valores máximos!\n");
    return height;
}

float hf(float x, float z) {
    float height = 0;
    int x1, x2, z1, z2;

    x1 = floor(x);
    x2 = x1 + 1;
    float fx = x - x1;

    z1 = floor(z);
    z2 = z1 + 1;
    float fz = z - z1;

    float h_x1_z = h(x1, z1) * (1 - fz) + h(x1, z2) * fz;
    float h_x2_z = h(x2, z1) * (1 - fz) + h(x2, z2) * fz;

    height = h_x1_z * (1 - fx) + h_x2_z * fx;

    return height;
}

void prepareData() {
    //glewInit();
    glEnableClientState(GL_VERTEX_ARRAY);
    glGenBuffers(1, buffers);

    vertexB = (float *) malloc(256 * 256 * 6 * sizeof(float));
    int indice = 0;
    int posicaox = -128;
    int posicaoz = -128;


    for (int i = 0; i < th - 1; ++i) {

        vertexB[indice] = (float) posicaox;
        vertexB[indice + 1] = h(posicaox, posicaoz);
        vertexB[indice + 2] = (float) posicaoz;
        indice += 3;

        for (int j = 0; j < tw - 1; ++j) {
            vertexB[indice] = (float) posicaox;
            vertexB[indice + 1] = h(posicaox, posicaoz + 1);
            vertexB[indice + 2] = (float) posicaoz + 1;
            indice += 3;

            vertexB[indice] = (float) posicaox + 1;
            vertexB[indice + 1] = h(posicaox + 1, posicaoz);
            vertexB[indice + 2] = (float) posicaoz;
            indice += 3;

            posicaox++;
        }

        vertexB[indice] = (float) posicaox;
        vertexB[indice + 1] = h(posicaox, posicaoz + 1);
        vertexB[indice + 2] = (float) posicaoz + 1;
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


float randomNumber(float lado) {
    return ((float) rand()) / (float) RAND_MAX * (2 * lado) - lado;
}

void generateTranslationsTrees() {
    xTreePosicion = (float *) malloc(numberTrees * sizeof(float));
    zTreePosicion = (float *) malloc(numberTrees * sizeof(float));
    yTreePosicion = (float *) malloc(numberTrees * sizeof(float));

    float xrandom;
    float zrandom;

    for (int i = numberTrees; i >= 0; i--) {
        xrandom = randomNumber(124);
        zrandom = randomNumber(124);

        while ((pow(xrandom, 2) + pow(zrandom, 2) - pow(50, 2)) < 0) {
            xrandom = randomNumber(124);
            zrandom = randomNumber(124);
        }

        xTreePosicion[i] = xrandom;
        zTreePosicion[i] = zrandom;
        yTreePosicion[i] = hf(xrandom, zrandom);
    }
}

void trees() {
    for (int i = 0; i < numberTrees; i++) {
        glPushMatrix();

        glTranslatef(xTreePosicion[i], yTreePosicion[i], zTreePosicion[i]);

        glRotatef(-90, 1, 0, 0);

        glColor3f(0.76, 0.58, 0.40);
        glutSolidCone(1, 6, 4, 4);

        glTranslatef(0, 0, 3);

        glColor3f(0.04, 0.4, 0.14);
        glutSolidCone(3, 8, 6, 8);

        glPopMatrix();
    }
}


void torus() {
    glPushMatrix();
    glColor3f(1, 0, 1);
    glutSolidTorus(1, 4, 30, 30);
    glTranslatef(0, 0, hf(0, 0));
    glPopMatrix();
}

void teapots() {
    clock_t atual = clock();

    double sec = clock() / 250000.0 / 60.0;
    double sec_rad = sec * 2.0 * M_PI;

    double rotate = (360.0 / 8.0);
    double raio = 15.0;

    glPushMatrix();
    glTranslatef(0, 2, 0);
    glColor3f(0, 0, 1);

    for (int i = 0; i < 8; i++) {
        glPushMatrix();

        glRotatef((double) -i * rotate - sec * 360.0, 0, 1.0f, 0);
        glTranslatef(raio, 0, 0);

        glutSolidTeapot(2);
        glPopMatrix();
    }


    rotate = (360.0 / 16.0);
    raio = 35.0;

    glColor3f(1, 0, 0);

    for (int i = 0; i < 16; i++) {
        glPushMatrix();

        glRotatef((double) -i * rotate + sec * 360.0, 0, 1.0f, 0);
        glTranslatef(raio, 0, 0);

        glutSolidTeapot(2);
        glPopMatrix();
    }
    glPopMatrix();
}


void renderScene(void) {


    float pos[4] = {-1.0, 1.0, 1.0, 0.0};

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
              l1, camY, l3,
              0.0f, 1.0f, 0.0f);

    drawTerrain();
    torus();
    trees();
    teapots();

// End of frame
    glutSwapBuffers();
}


struct Vetor crossProduct(struct Vetor u, struct Vetor v){
    struct Vetor k;
    k.x = u.y * v.z - v.y * u.z;
    k.y = v.x * u.z - u.x * v.z;
    k.z = u.x * v.y - v.x * u.y;
    return k;
}


void processKeys(unsigned char key, int xx, int yy) {
    switch (key) {
        case 'w':
            camX = camX + (l1 - camX);
            camZ = camZ + (l3 - camZ);
            camY = 5 + hf(camX, camZ);

            l1 = camX + sin(angle);
            l3 = camZ + cos(angle);
//            l1 = l1 + (l1 - camX);
//            l3 = l3 + (l3 - camZ);
            break;

        case 's':
            camX = camX - (l1 - camX);
            camZ = camZ - (l3 - camZ);
            camY = 5 + hf(camX, camZ);

            l1 = camX + sin(angle);
            l3 = camZ + cos(angle);
//            l1 = l1 + (l1 - camX);
//            l3 = l3 + (l3 - camZ);
            break;

        case 'e':
            angle -= 0.1;
            l1 = camX + sin(angle);
            l3 = camZ + cos(angle);
            break;

        case 'q':
            angle += 0.1;
            l1 = camX + sin(angle);
            l3 = camZ + cos(angle);
            break;

        case 'd':

            u.x = l1 - camX; u.y = l2-camY; u.z = l3 - camZ;
            v.x = 0.0f; v.y = 1.0f; v.z = 0.0f;
            k = crossProduct(u, v);

            //normalizar vetor k
            normalizar = k.x; k.x /= normalizar; k.y /= normalizar; k.z /= normalizar;

            l1 -= k.x; l2 -= k.y;l3 -= k.z;
            camX -= k.x; camZ -= k.z; camY = 5 + hf(camX, camZ);

            break;

        case 'a':
            u.x = l1 - camX; u.y = l2-camY; u.z = l3 - camZ;
            v.x = 0.0f; v.y = 1.0f; v.z = 0.0f;
            k = crossProduct(u, v);

            //normalizar vetor k
            normalizar = k.x; k.x /= normalizar; k.y /= normalizar; k.z /= normalizar;

            l1 += k.x; l2 += k.y;l3 += k.z;
            camX += k.x; camZ += k.z; camY = 5 + hf(camX, camZ);

            break;

        default:
            break;
    }


    glutPostRedisplay();
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

void printInfo() {
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));
    puts("Foward: W\n"
         "Backward: S\n"
         "Rotate Right: E\n"
         "Rotate Left: Q\n"
         "Left: A\n"
         "Right: D\n");
}

void init() {
//  Helper for random numbers
    srand(time(NULL));

//  Init for Devil
    ilInit();

// 	Load the height map "terreno.jpg"
    loadImage();

// 	Build the vertex arrays
    prepareData();

//  Build vectors for trees
    generateTranslationsTrees();

//  Info
    printInfo();

// 	OpenGL settings
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
    glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    //glutMouseFunc(processMouseButtons);
    //glutMotionFunc(processMouseMotion);


    init();

// enter GLUT's main cycle
    glutMainLoop();

    return 0;
}

