/*
 * CASO 3 - Diseno e ingenieria: Brazo robotico articulado
 * Computacion Grafica I (ISW-411) - UCE
 *
 * Brazo 2D de dos segmentos (brazo y antebrazo) mas una pinza
 * (reto opcional), construido como una JERARQUIA con la pila de matrices.
 *
 * Controles:
 *   Q / A : girar el brazo      (+ / -)
 *   W / S : girar el antebrazo  (+ / -)
 *   E / D : girar la pinza      (+ / -)   <- reto opcional
 *   ESC   : salir
 *
 * Compilar (Windows MSYS2):
 *   gcc caso3.c -o caso3 -Wall -lfreeglut -lglu32 -lopengl32
 */

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>   /* exit */
#include <math.h>     /* cos, sin (solo para dibujar las articulaciones) */

/* Medidas de cada segmento. El alcance maximo es 40 + 30 + 18 = 88 y el
   mundo va de -100 a 100 con la base en el centro: el brazo estirado en
   cualquier direccion siempre cabe en la ventana. */
#define LARGO_BRAZO      40.0f
#define LARGO_ANTEBRAZO  30.0f
#define LARGO_PINZA      12.0f
#define LARGO_DEDO        6.0f
#define GROSOR           8.0f
#define PASO_ANGULO       5.0f   /* grados que gira cada tecla */
#define PI               3.14159265f

/* Unico estado del programa: el angulo de cada articulacion (grados).
   Cada angulo es RELATIVO al segmento anterior, no a la pantalla. */
float anguloBrazo     = 45.0f;
float anguloAntebrazo = -30.0f;
float anguloPinza     = -20.0f;

/* Dibuja un segmento como rectangulo desde x = 0 hasta x = largo,
   centrado en y = 0. Que empiece en el ORIGEN es la clave: glRotatef gira
   alrededor del origen, asi que el segmento gira desde su base (la
   articulacion) y no desde su centro. */
void dibujarSegmento(float largo, float grosor)
{
    glBegin(GL_QUADS);
        glVertex2f(0,     -grosor / 2);
        glVertex2f(largo, -grosor / 2);
        glVertex2f(largo,  grosor / 2);
        glVertex2f(0,      grosor / 2);
    glEnd();
}

/* Circulo para marcar cada articulacion (solo decoracion). */
void dibujarArticulacion(float r)
{
    int k;
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (k = 0; k <= 20; k++) {
            float a = 2.0f * PI * k / 20;
            glVertex2f(r * cosf(a), r * sinf(a));
        }
    glEnd();
}

/* Base fija del robot: un bloque gris que no se mueve. */
void dibujarBase(void)
{
    glColor3f(0.45f, 0.45f, 0.50f);
    glBegin(GL_QUADS);
        glVertex2f(-12, -12);
        glVertex2f( 12, -12);
        glVertex2f( 12,   0);
        glVertex2f(-12,   0);
    glEnd();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    dibujarBase();

    /* Guardamos la matriz "limpia" del mundo: todo lo que pase dentro de
       este bloque (trasladar, rotar) se deshace con el glPopMatrix final y
       no afecta a nada que se dibuje despues. */
    glPushMatrix();

        /* 1) Trasladar a la base (aqui la base esta en el origen 0,0). */
        glTranslatef(0, 0, 0);

        /* 2) Rotar el BRAZO y dibujarlo. */
        glRotatef(anguloBrazo, 0, 0, 1);       /* eje Z = giro en 2D */
        glColor3f(0.20f, 0.45f, 0.85f);        /* azul */
        dibujarSegmento(LARGO_BRAZO, GROSOR);
        dibujarArticulacion(5);                /* hombro */

        /* 3) Trasladar a la PUNTA del brazo. Como ya estamos rotados, este
              traslado en X "sigue" la direccion del brazo y nos deja justo
              en su punta, sin calcular senos ni cosenos a mano. */
        glTranslatef(LARGO_BRAZO, 0, 0);

        /* Push anidado: el antebrazo vive DENTRO del bloque del brazo, por
           eso hereda su rotacion. Si el brazo gira, todo lo de aqui adentro
           gira con el automaticamente. */
        glPushMatrix();

            /* 4) Rotar el ANTEBRAZO (relativo al brazo) y dibujarlo. */
            glRotatef(anguloAntebrazo, 0, 0, 1);
            glColor3f(0.95f, 0.55f, 0.10f);    /* naranja */
            dibujarSegmento(LARGO_ANTEBRAZO, GROSOR * 0.8f);
            dibujarArticulacion(4);            /* codo */

            /* 5) Ir a la punta del antebrazo para montar la pinza. */
            glTranslatef(LARGO_ANTEBRAZO, 0, 0);

            /* RETO: tercer segmento (pinza) con el mismo patron:
               rotar -> dibujar -> trasladar a su punta. */
            glPushMatrix();
                glRotatef(anguloPinza, 0, 0, 1);
                glColor3f(0.25f, 0.70f, 0.30f);    /* verde */
                dibujarSegmento(LARGO_PINZA, GROSOR * 0.6f);
                dibujarArticulacion(3);            /* muneca */

                glTranslatef(LARGO_PINZA, 0, 0);

                /* Los dos dedos son "hermanos": salen del mismo punto
                   pero cada uno gira distinto. Por eso cada dedo va en su
                   propio push/pop: el giro del dedo de arriba no debe
                   afectar al de abajo. */
                glPushMatrix();
                    glRotatef(35, 0, 0, 1);
                    dibujarSegmento(LARGO_DEDO, 2.0f);
                glPopMatrix();

                glPushMatrix();
                    glRotatef(-35, 0, 0, 1);
                    dibujarSegmento(LARGO_DEDO, 2.0f);
                glPopMatrix();
            glPopMatrix();   /* fin pinza */

        glPopMatrix();       /* fin antebrazo */

    glPopMatrix();           /* volvemos a la matriz limpia del mundo */

    glutSwapBuffers();
}

/* Mantiene el angulo entre -360 y 360 para que el numero no crezca sin
   limite si el usuario deja la tecla apretada mucho tiempo. */
float normalizar(float angulo)
{
    if (angulo >= 360) angulo -= 360;
    if (angulo <= -360) angulo += 360;
    return angulo;
}

void teclado(unsigned char tecla, int x, int y)
{
    (void)x; (void)y;   /* posicion del raton: no la necesitamos */

    switch (tecla) {
        case 'q': case 'Q': anguloBrazo     += PASO_ANGULO; break;
        case 'a': case 'A': anguloBrazo     -= PASO_ANGULO; break;
        case 'w': case 'W': anguloAntebrazo += PASO_ANGULO; break;
        case 's': case 'S': anguloAntebrazo -= PASO_ANGULO; break;
        case 'e': case 'E': anguloPinza     += PASO_ANGULO; break;
        case 'd': case 'D': anguloPinza     -= PASO_ANGULO; break;
        case 27: exit(0);   /* 27 = codigo ASCII de ESC */
    }

    anguloBrazo     = normalizar(anguloBrazo);
    anguloAntebrazo = normalizar(anguloAntebrazo);
    anguloPinza     = normalizar(anguloPinza);

    /* Solo cambiamos numeros; hay que pedirle a GLUT que vuelva a
       llamar a display para ver el brazo en su nueva posicion. */
    glutPostRedisplay();
}

void reshape(int w, int h)
{
    /* Viewport cuadrado y centrado para que el brazo no se deforme. */
    int lado = (w < h) ? w : h;
    glViewport((w - lado) / 2, (h - lado) / 2, lado, lado);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-100, 100, -100, 100);   /* base del robot en el centro */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Caso 3 - Brazo robotico (Q/A W/S E/D, ESC sale)");

    glClearColor(0.93f, 0.93f, 0.90f, 1);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutMainLoop();
    return 0;
}
