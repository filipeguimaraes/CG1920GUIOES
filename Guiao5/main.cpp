#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>


float alfa = 0.0f, beta = 0.5f, radius = 100.0f;
float camX, camY, camZ;

int numberTrees = 600;
float *xTreePosicion;
float *zTreePosicion;


float randomNumber(){
    return ((float) rand()) / (float) RAND_MAX * 194.0 - 97.0 ;
}

void generateTranslationsTrees(){
    xTreePosicion = (float*) malloc(numberTrees*sizeof(float));
    zTreePosicion = (float*) malloc(numberTrees*sizeof(float));

    float xrandom ;
    float zrandom ;

    for(int i=numberTrees;i>=0;i--) {
        xrandom = randomNumber();
        zrandom = randomNumber();

        while ((pow(xrandom,2)+pow(zrandom,2)-pow(50,2))<0) {
            xrandom = randomNumber();
            zrandom = randomNumber();
        }

        xTreePosicion[i] = xrandom;
        zTreePosicion[i] = zrandom;
    }
}

void trees(int numberOfTrees){
    for(int i=0; i<numberOfTrees; i++){
        glPushMatrix();

        glTranslatef(xTreePosicion[i],0,zTreePosicion[i]);

        glRotatef(-90,1,0,0);

        glColor3f(0.76, 0.58, 0.40);
        glutSolidCone(1,6,4,4);

        glTranslatef(0, 0, 3);

        glColor3f(0.04, 0.4, 0.14);
        glutSolidCone(3,8,6,8);

        glPopMatrix();
    }
}


void spherical2Cartesian() {

    camX = radius * cos(beta) * sin(alfa);
    camY = radius * sin(beta);
    camZ = radius * cos(beta) * cos(alfa);
}


void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;

    // compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}




void teapots(){
    clock_t atual = clock();

    double sec =  clock() / 250000.0 / 60.0;
    double sec_rad = sec * 2.0 * M_PI;

    double rotate = ( 360.0 / 8.0 ) ;
    double trans_rot = ( 1.0 / 8.0 ) * 2 * M_PI;
    double raio = 15.0;

    glPushMatrix();
    glTranslatef(0, 2, 0);
    glColor3f(0, 0, 1);

    for (int i = 0; i < 8; i++){
        glPushMatrix();

        glTranslatef(raio * cos((double) i * trans_rot + sec_rad), 0,
                     raio * sin((double) i * trans_rot + sec_rad));
        glRotatef((double) -i * rotate - sec * 360.0 , 0, 1.0f, 0);

        glutSolidTeapot(2);
        glPopMatrix();
    }


    rotate = ( 360.0 / 16.0 );
    trans_rot = ( 1.0 / 16.0 ) * 2 * M_PI;
    raio = 35.0;

    glColor3f(1, 0, 0);

    for (int i = 0; i < 16; i++){
        glPushMatrix();

        glTranslatef(raio * cos((double) i * trans_rot - sec_rad), 0,
                     raio * sin((double) i * trans_rot - sec_rad));
        glRotatef((double) -i * rotate + sec * 360.0 , 0, 1.0f, 0);

        glutSolidTeapot(2);
        glPopMatrix();
    }
    glPopMatrix();


}


void renderScene(void) {

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
              0.0, 0.0, 0.0,
              0.0f, 1.0f, 0.0f);

    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex3f(100.0f, 0, -100.0f);
    glVertex3f(-100.0f, 0, -100.0f);
    glVertex3f(-100.0f, 0, 100.0f);

    glVertex3f(100.0f, 0, -100.0f);
    glVertex3f(-100.0f, 0, 100.0f);
    glVertex3f(100.0f, 0, 100.0f);

    glEnd();
    glColor3f(1,0,1);
    glutSolidTorus(1,4,30,30);
    trees(numberTrees);
    teapots();

    // End of frame
    glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

// put code to process regular keys in here

}


void processSpecialKeys(int key, int xx, int yy) {

    switch (key) {

        case GLUT_KEY_RIGHT:
            alfa -= 0.1; break;

        case GLUT_KEY_LEFT:
            alfa += 0.1; break;

        case GLUT_KEY_UP:
            beta += 0.1f;
            if (beta > 1.5f)
                beta = 1.5f;
            break;

        case GLUT_KEY_DOWN:
            beta -= 0.1f;
            if (beta < -1.5f)
                beta = -1.5f;
            break;

        case GLUT_KEY_PAGE_DOWN: radius -= 5.0f;
            if (radius < 1.0f)
                radius = 1.0f;
            break;

        case GLUT_KEY_PAGE_UP: radius += 5.0f; break;
    }
    spherical2Cartesian();
    glutPostRedisplay();

}


void printInfo() {

    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));

    printf("\nUse Arrows to move the camera up/down and left/right\n");
    printf("Home and End control the distance from the camera to the origin");
}


int main(int argc, char **argv) {

// init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow("CG@DI-UM");

// Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
// Callback registration for keyboard processing
//	glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

//  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    spherical2Cartesian();

    generateTranslationsTrees();

    printInfo();

// enter GLUT's main cycle
    glutMainLoop();


    return 1;
}
