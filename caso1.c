/*
 * CASO 1 - Visualizacion de datos: Panel de ocupacion hotelera
 * Computacion Grafica I (ISW-411) - UCE
 *
 * Grafico de barras con la ocupacion (%) de lunes a domingo.
 * Cada barra se pinta con un semaforo:
 *   verde  -> ocupacion menor a 70 %
 *   ambar  -> ocupacion entre 70 % y 89 %
 *   rojo   -> ocupacion de 90 % o mas
 *
 * Compilar (Windows MSYS2):
 *   gcc caso1.c -o caso1 -Wall -lfreeglut -lglu32 -lopengl32
 */

/* En Mac GLUT vive en otra carpeta y Apple marca OpenGL como "obsoleto";
   GL_SILENCE_DEPRECATION evita esos avisos para que compile limpio. */
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define NUM_DIAS 7

/* Datos de la semana (lunes a domingo). Son porcentajes de 0 a 100,
   por eso el "mundo" del grafico va de 0 a 100 en ambos ejes. */
float ocupacion[NUM_DIAS] = {65, 72, 80, 95, 88, 60, 55};

/* Elige el color de la barra segun el semaforo.
   Se llama ANTES de dibujar cada barra porque OpenGL es una maquina de
   estado: el color que fijemos queda activo para todos los vertices que
   vengan despues, hasta que lo cambiemos. */
void colorSemaforo(float valor)
{
    if (valor < 70) {
        glColor3f(0.20f, 0.70f, 0.25f);   /* verde: ocupacion baja  */
    } else if (valor < 90) {
        glColor3f(1.00f, 0.70f, 0.00f);   /* ambar: ocupacion media */
    } else {
        glColor3f(0.85f, 0.10f, 0.10f);   /* rojo: casi lleno       */
    }
}

/* Dibuja los ejes X e Y y unas marcas cada 10 % en el eje Y. */
void dibujarEjes(void)
{
    int i;

    /* Lineas guia suaves en 70 y 90: son los limites del semaforo y
       ayudan a la gerencia a ver por que una barra cambio de color. */
    glColor3f(0.80f, 0.80f, 0.80f);
    glBegin(GL_LINES);
        glVertex2f(0, 70);  glVertex2f(100, 70);
        glVertex2f(0, 90);  glVertex2f(100, 90);
    glEnd();

    /* Ejes en negro. GL_LINES toma los vertices de dos en dos:
       cada par es un segmento independiente. */
    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(0, 0);  glVertex2f(100, 0);   /* eje X (dias)       */
        glVertex2f(0, 0);  glVertex2f(0, 100);   /* eje Y (porcentaje) */

        /* Marquitas cada 10 % para poder leer la altura de las barras. */
        for (i = 10; i <= 100; i += 10) {
            glVertex2f(-2, (float)i);
            glVertex2f(0, (float)i);
        }
    glEnd();
}

/* Dibuja una barra por dia. */
void dibujarBarras(void)
{
    int i;

    /* Repartimos el ancho del grafico (100 unidades) en 7 "casillas"
       iguales; asi las barras quedan separadas de forma pareja sin
       importar cuantos dias haya. */
    float anchoCasilla = 100.0f / NUM_DIAS;
    float anchoBarra   = anchoCasilla * 0.6f;              /* 60 % barra    */
    float hueco        = (anchoCasilla - anchoBarra) / 2;  /* 20 % por lado */

    for (i = 0; i < NUM_DIAS; i++) {
        float x1 = i * anchoCasilla + hueco;   /* borde izquierdo */
        float x2 = x1 + anchoBarra;            /* borde derecho   */
        float alto = ocupacion[i];             /* tope = porcentaje */

        /* Primero el color (maquina de estado), luego la figura. */
        colorSemaforo(alto);

        /* Una barra es un rectangulo: 4 vertices con GL_QUADS.
           La base esta en y = 0 y el tope en y = porcentaje. */
        glBegin(GL_QUADS);
            glVertex2f(x1, 0);
            glVertex2f(x2, 0);
            glVertex2f(x2, alto);
            glVertex2f(x1, alto);
        glEnd();
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    dibujarEjes();
    dibujarBarras();
    glFlush();   /* un solo buffer: con glFlush basta para mostrar */
}

/* Se llama al abrir la ventana y cada vez que cambia de tamano. */
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /* El mundo va de -10 a 110: los datos usan 0..100 y dejamos 10
       unidades de margen por cada lado para que los ejes, las marcas y
       la barra de 95 % no queden pegados al borde de la ventana. */
    gluOrtho2D(-10, 110, -10, 110);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Caso 1 - Ocupacion hotelera (Lun a Dom)");

    glClearColor(1, 1, 1, 1);   /* fondo blanco, como un reporte */

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
