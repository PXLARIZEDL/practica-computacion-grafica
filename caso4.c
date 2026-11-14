/*
 * CASO 4 - Visualizacion cientifica 3D: Modelo molecular
 * Computacion Grafica I (ISW-411) - UCE
 *
 * Molecula tipo metano (CH4): un atomo central grande y 4 atomos satelite
 * en forma de tetraedro, unidos por enlaces, girando lentamente.
 *
 * Compilar (Windows MSYS2):
 *   gcc caso4.c -o caso4 -Wall -lfreeglut -lglu32 -lopengl32
 */

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define NUM_SATELITES 4

/* Posiciones (x, y, z) de los 4 satelites. Son los vertices de un
   tetraedro (como el metano), a una distancia de ~2.5 del centro:
   2.5 / raiz(3) = 1.44 en cada coordenada. */
float satelites[NUM_SATELITES][3] = {
    { 1.44f,  1.44f,  1.44f},
    {-1.44f, -1.44f,  1.44f},
    {-1.44f,  1.44f, -1.44f},
    { 1.44f, -1.44f, -1.44f}
};

float angulo = 0.0f;   /* giro actual de la molecula (grados) */

/* Enlaces: una linea desde el centro (0,0,0) hasta cada satelite. */
void dibujarEnlaces(void)
{
    int i;

    /* Las lineas no tienen "superficie" que iluminar; apagamos la luz
       mientras las dibujamos para que salgan con su color plano. */
    glDisable(GL_LIGHTING);
    glLineWidth(4.0f);
    glColor3f(0.85f, 0.85f, 0.85f);
    glBegin(GL_LINES);
        for (i = 0; i < NUM_SATELITES; i++) {
            glVertex3f(0, 0, 0);
            glVertex3f(satelites[i][0], satelites[i][1], satelites[i][2]);
        }
    glEnd();
    glEnable(GL_LIGHTING);
}

void dibujarMolecula(void)
{
    int i;

    dibujarEnlaces();

    /* Atomo central: como esta en el origen, no hace falta trasladarlo. */
    glColor3f(0.20f, 0.20f, 0.20f);   /* gris oscuro (carbono) */
    glutSolidSphere(1.0, 20, 20);     /* radio 1, 20x20 de detalle */

    /* Satelites: glutSolidSphere SIEMPRE dibuja en el origen actual, asi
       que para cada uno movemos el origen con glTranslatef. El push/pop
       guarda y restaura la matriz: sin el, el segundo traslado se sumaria
       al primero y los atomos quedarian en posiciones equivocadas. */
    glColor3f(0.90f, 0.95f, 1.00f);   /* blanco azulado (hidrogeno) */
    for (i = 0; i < NUM_SATELITES; i++) {
        glPushMatrix();
            glTranslatef(satelites[i][0], satelites[i][1], satelites[i][2]);
            glutSolidSphere(0.5, 20, 20);   /* mas pequenos que el central */
        glPopMatrix();
    }
}

void display(void)
{
    /* Pieza 3 del Z-buffer: limpiar tambien el buffer de profundidad en
       cada frame. Si no, quedarian las profundidades del frame anterior y
       partes de la molecula desaparecerian. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* La camara va al inicio, justo despues de glLoadIdentity: asi se
       aplica a todo lo que se dibuja despues. Esta arriba (y = 5) y
       hacia un lado (x = 6, z = 8), mirando al centro (0,0,0), con el eje
       Y como "arriba". Desde ahi se ve la molecula en angulo. */
    glLoadIdentity();
    gluLookAt(6.0, 5.0, 8.0,    /* ojo: donde esta la camara */
              0.0, 0.0, 0.0,    /* centro: hacia donde mira  */
              0.0, 1.0, 0.0);   /* vector "arriba"           */

    /* Girar TODA la molecula alrededor del eje Y: va despues de la camara
       y antes de dibujar, por eso afecta a todos los atomos por igual. */
    glRotatef(angulo, 0, 1, 0);

    dibujarMolecula();

    glutSwapBuffers();
}

/* Animacion lenta: 0.4 grados cada 16 ms (~24 grados por segundo). */
void timer(int valor)
{
    (void)valor;
    angulo += 0.4f;
    if (angulo >= 360) angulo -= 360;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;   /* evita dividir entre cero al minimizar */
    glViewport(0, 0, w, h);

    /* La proyeccion va en reshape porque depende de la proporcion de la
       ventana (ancho / alto). gluPerspective = proyeccion en perspectiva:
       lo lejano se ve mas pequeno, dando profundidad real.
       45 grados de vision; cerca = 1 y lejos = 50 delimitan lo visible. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 1.0, 50.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    /* Pieza 1 del Z-buffer: pedir a GLUT una ventana con buffer de
       profundidad (GLUT_DEPTH). */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Caso 4 - Modelo molecular 3D");

    glClearColor(0.08f, 0.10f, 0.16f, 1);

    /* Pieza 2 del Z-buffer: activar la prueba de profundidad. Asi un
       atomo que esta detras no se pinta encima de uno que esta delante,
       sin importar en que orden los dibujemos. */
    glEnable(GL_DEPTH_TEST);

    /* EXTRA OPCIONAL: iluminacion basica. Sin luz, una esfera se ve como
       un circulo plano; con luz se nota su volumen.
       GL_LIGHT0 es una luz por defecto que alumbra desde la camara, y
       GL_COLOR_MATERIAL hace que glColor3f siga funcionando con la luz
       encendida (si no, todo saldria gris). */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
