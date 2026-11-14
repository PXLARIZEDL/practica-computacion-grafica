/*
 * CASO 2 - Simulacion cientifica: Particulas en un contenedor
 * Computacion Grafica I (ISW-411) - UCE
 *
 * 25 particulas de gas que se mueven en linea recta y rebotan en las
 * cuatro paredes del recipiente conservando su rapidez.
 * Reto opcional hecho: el color depende de la rapidez
 * (azul = lenta / "fria", rojo = rapida / "caliente").
 *
 * Compilar (Windows MSYS2):
 *   gcc caso2.c -o caso2 -Wall -lfreeglut -lglu32 -lopengl32
 */

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>   /* rand, srand */
#include <time.h>     /* time, para que cada corrida sea distinta */
#include <math.h>     /* cos, sin, sqrt */

#define NUM_PARTICULAS 25
#define RADIO          2.0f     /* radio de cada particula (unidades del mundo) */
#define TAM_CAJA       100.0f   /* el contenedor va de 0 a 100 en X y en Y      */
#define LADOS_CIRCULO  20       /* triangulos con que se aproxima cada circulo  */
#define PI             3.14159265f

/* Un arreglo por cada dato: la particula i tiene su posicion (px[i], py[i])
   y su velocidad (vx[i], vy[i]). Un for recorre todas a la vez. */
float px[NUM_PARTICULAS], py[NUM_PARTICULAS];
float vx[NUM_PARTICULAS], vy[NUM_PARTICULAS];

/* Rapidez minima y maxima posibles; se usan para escalar el color. */
float rapidezMin, rapidezMax;

/* Numero al azar entre min y max (rand() solo da enteros). */
float aleatorio(float min, float max)
{
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

void inicializarParticulas(void)
{
    int i;
    srand((unsigned)time(NULL));

    for (i = 0; i < NUM_PARTICULAS; i++) {
        /* La posicion inicial deja libre un RADIO en cada borde para que
           ninguna particula nazca "metida" en la pared. */
        px[i] = aleatorio(RADIO, TAM_CAJA - RADIO);
        py[i] = aleatorio(RADIO, TAM_CAJA - RADIO);

        /* Velocidad de 0.2 a 1.0 unidades por frame en cada eje, con
           signo al azar. Asi ninguna queda quieta y van en todas las
           direcciones. */
        vx[i] = aleatorio(0.2f, 1.0f);
        vy[i] = aleatorio(0.2f, 1.0f);
        if (rand() % 2) vx[i] = -vx[i];
        if (rand() % 2) vy[i] = -vy[i];
    }

    /* Con |vx| y |vy| entre 0.2 y 1.0, la rapidez (Pitagoras) queda
       entre sqrt(0.2^2+0.2^2) y sqrt(1^2+1^2). */
    rapidezMin = sqrtf(0.2f * 0.2f + 0.2f * 0.2f);
    rapidezMax = sqrtf(1.0f + 1.0f);
}

/* Reto: color segun la rapidez. t va de 0 (la mas lenta) a 1 (la mas
   rapida); mezclamos azul y rojo con ese valor. */
void colorPorRapidez(int i)
{
    float rapidez = sqrtf(vx[i] * vx[i] + vy[i] * vy[i]);
    float t = (rapidez - rapidezMin) / (rapidezMax - rapidezMin);
    glColor3f(t, 0.2f, 1.0f - t);
}

/* Circulo relleno con GL_TRIANGLE_FAN: el primer vertice es el centro y
   los demas recorren el borde; OpenGL arma un "abanico" de triangulos que
   comparten el centro. Mas lados = circulo mas redondo. */
void dibujarCirculo(float cx, float cy, float r)
{
    int k;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);   /* centro */
        for (k = 0; k <= LADOS_CIRCULO; k++) {   /* <= para cerrar el circulo */
            float angulo = 2.0f * PI * k / LADOS_CIRCULO;
            glVertex2f(cx + r * cosf(angulo), cy + r * sinf(angulo));
        }
    glEnd();
}

void dibujarContenedor(void)
{
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
        glVertex2f(0, 0);               glVertex2f(TAM_CAJA, 0);        /* piso  */
        glVertex2f(TAM_CAJA, 0);        glVertex2f(TAM_CAJA, TAM_CAJA); /* der.  */
        glVertex2f(TAM_CAJA, TAM_CAJA); glVertex2f(0, TAM_CAJA);        /* techo */
        glVertex2f(0, TAM_CAJA);        glVertex2f(0, 0);               /* izq.  */
    glEnd();
}

void display(void)
{
    int i;
    glClear(GL_COLOR_BUFFER_BIT);

    dibujarContenedor();
    for (i = 0; i < NUM_PARTICULAS; i++) {
        colorPorRapidez(i);                  /* color antes de dibujar */
        dibujarCirculo(px[i], py[i], RADIO);
    }

    /* Doble buffer: dibujamos en el buffer de atras y aqui lo
       intercambiamos con el de adelante. Asi nunca se ve un frame a medio
       dibujar y la animacion no parpadea. */
    glutSwapBuffers();
}

/* Un paso de la simulacion: mover y luego revisar choques con paredes. */
void actualizar(void)
{
    int i;
    for (i = 0; i < NUM_PARTICULAS; i++) {
        /* Movimiento rectilineo: la posicion avanza segun la velocidad. */
        px[i] += vx[i];
        py[i] += vy[i];

        /* Rebote en X. Comparamos el BORDE (centro - radio / centro + radio)
           y no el centro, para que la particula toque la pared con su borde.
           Ademas la recolocamos justo dentro: si se paso un poco, en el
           siguiente frame seguiria "afuera" y volveria a invertir la
           velocidad, quedando atrapada temblando en la pared. */
        if (px[i] - RADIO < 0) {
            px[i] = RADIO;
            vx[i] = -vx[i];
        } else if (px[i] + RADIO > TAM_CAJA) {
            px[i] = TAM_CAJA - RADIO;
            vx[i] = -vx[i];
        }

        /* Rebote en Y: mismo razonamiento con el piso y el techo. */
        if (py[i] - RADIO < 0) {
            py[i] = RADIO;
            vy[i] = -vy[i];
        } else if (py[i] + RADIO > TAM_CAJA) {
            py[i] = TAM_CAJA - RADIO;
            vy[i] = -vy[i];
        }
        /* Al solo cambiar el signo, |vx| y |vy| no cambian: la rapidez se
           conserva, como pide el enunciado (choque elastico con la pared). */
    }
}

/* El "ciclo del juego": actualizar -> pedir redibujo -> volver a
   programar el timer. 16 ms ~ 60 cuadros por segundo. */
void timer(int valor)
{
    (void)valor;   /* GLUT exige este parametro aunque no lo usemos */
    actualizar();
    glutPostRedisplay();          /* sin esto la pantalla no se refresca */
    glutTimerFunc(16, timer, 0);  /* glutTimerFunc dispara una sola vez */
}

void reshape(int w, int h)
{
    /* Usamos un viewport cuadrado y centrado: el mundo es cuadrado
       (0..100 x 0..100) y asi los circulos no se estiran como ovalos
       si el usuario cambia la forma de la ventana. */
    int lado = (w < h) ? w : h;
    glViewport((w - lado) / 2, (h - lado) / 2, lado, lado);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /* Margen de 2 unidades para que las paredes (en 0 y 100) se vean. */
    gluOrtho2D(-2, TAM_CAJA + 2, -2, TAM_CAJA + 2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);   /* doble buffer */
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Caso 2 - Particulas en un contenedor");

    glClearColor(0.05f, 0.05f, 0.10f, 1);   /* fondo oscuro para resaltar */
    inicializarParticulas();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);   /* arranca la animacion */
    glutMainLoop();
    return 0;
}
