#include "sinuca.h"

// Desenha o piso com quadrados claros e escuros alternados.
static void DesenhaPiso(void)
{
    int i;
    int j;
    float metadeX = NUM_QUADRADOS_X * LADO_QUADRADO / 2.0f;
    float metadeZ = NUM_QUADRADOS_Z * LADO_QUADRADO / 2.0f;

    glDisable(GL_LIGHTING); // Mantém as cores do piso sem alteração pela luz.
    glBegin(GL_QUADS);
    for (i = 0; i < NUM_QUADRADOS_X; i++)
    {
        for (j = 0; j < NUM_QUADRADOS_Z; j++)
        {
            float x = -metadeX + i * LADO_QUADRADO;
            float z = -metadeZ + j * LADO_QUADRADO;

            // A soma alterna entre par e ímpar em quadrados vizinhos.
            if ((i + j) % 2 == 0)
                glColor3f(0.68f, 0.66f, 0.61f); // Cinza claro envelhecido.
            else
                glColor3f(0.20f, 0.21f, 0.21f); // Cinza escuro.

            glVertex3f(x, 0.0f, z);
            glVertex3f(x, 0.0f, z + LADO_QUADRADO);
            glVertex3f(x + LADO_QUADRADO, 0.0f, z + LADO_QUADRADO);
            glVertex3f(x + LADO_QUADRADO, 0.0f, z);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// Desenha as quatro paredes altas ao redor do salão.
static void DesenhaParedes(void)
{
    glColor3f(0.70f, 0.61f, 0.40f); // Amarelo-bege.
    glBegin(GL_QUADS);
    // As normais apontam para dentro do salão, onde fica a luminária.
    glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(-POSICAO_PAREDE_X, 0.0f, POSICAO_PAREDE_Z);
        glVertex3f(-POSICAO_PAREDE_X, ALTURA_PAREDE, POSICAO_PAREDE_Z);
        glVertex3f( POSICAO_PAREDE_X, ALTURA_PAREDE, POSICAO_PAREDE_Z);
        glVertex3f( POSICAO_PAREDE_X, 0.0f, POSICAO_PAREDE_Z);

    glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f( POSICAO_PAREDE_X, 0.0f, -POSICAO_PAREDE_Z);
        glVertex3f( POSICAO_PAREDE_X, ALTURA_PAREDE, -POSICAO_PAREDE_Z);
        glVertex3f(-POSICAO_PAREDE_X, ALTURA_PAREDE, -POSICAO_PAREDE_Z);
        glVertex3f(-POSICAO_PAREDE_X, 0.0f, -POSICAO_PAREDE_Z);

    glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f( POSICAO_PAREDE_X, 0.0f, POSICAO_PAREDE_Z);
        glVertex3f( POSICAO_PAREDE_X, ALTURA_PAREDE, POSICAO_PAREDE_Z);
        glVertex3f( POSICAO_PAREDE_X, ALTURA_PAREDE, -POSICAO_PAREDE_Z);
        glVertex3f( POSICAO_PAREDE_X, 0.0f, -POSICAO_PAREDE_Z);

    glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-POSICAO_PAREDE_X, 0.0f, -POSICAO_PAREDE_Z);
        glVertex3f(-POSICAO_PAREDE_X, ALTURA_PAREDE, -POSICAO_PAREDE_Z);
        glVertex3f(-POSICAO_PAREDE_X, ALTURA_PAREDE, POSICAO_PAREDE_Z);
        glVertex3f(-POSICAO_PAREDE_X, 0.0f, POSICAO_PAREDE_Z);
    glEnd();
}

// Desenha uma luminária pendurada com fio, abajur e lâmpada acesa.
static void DesenhaLampada(float x, float z)
{
    GLUquadric *quadric = gluNewQuadric();
    float baseAbajur = ALTURA_LAMPADA + 0.05f;
    float topoAbajur = baseAbajur + ALTURA_ABAJUR;

    Caixa(x, (topoAbajur + ALTURA_TRILHO) / 2.0f, z,
          0.03f, ALTURA_TRILHO - topoAbajur, 0.03f,
          0.08f, 0.08f, 0.08f); // Fio.

    gluQuadricNormals(quadric, GLU_SMOOTH);
    glColor3f(0.08f, 0.30f, 0.14f); // Abajur verde.
    glPushMatrix();
        glTranslatef(x, baseAbajur, z);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(quadric, RAIO_ABAJUR, 0.12f, ALTURA_ABAJUR, 24, 1);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.94f, 0.67f); // Boca iluminada do abajur.
    glPushMatrix();
        glTranslatef(x, baseAbajur, z);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluDisk(quadric, 0.0f, RAIO_ABAJUR, 24, 1);
    glPopMatrix();

    glColor3f(1.0f, 0.98f, 0.82f); // Bulbo.
    glPushMatrix();
        glTranslatef(x, ALTURA_LAMPADA, z);
        glutSolidSphere(0.14f, 16, 16);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    gluDeleteQuadric(quadric);
}

// Configura a fonte de luz amarelada da luminária central.
void ConfiguraLuzesCenario(void)
{
    GLfloat difusa[] = { 0.23f, 0.20f, 0.14f, 1.0f };
    GLfloat especular[] = { 0.12f, 0.10f, 0.07f, 1.0f };
    GLfloat posicao[] = { 0.0f, ALTURA_LAMPADA, 0.0f, 1.0f };

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, posicao);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, difusa);
    glLightfv(GL_LIGHT1, GL_SPECULAR, especular);
}

// Reúne os objetos estáticos que formam o salão de sinuca.
void DesenhaCenario(void)
{
    DesenhaPiso();
    DesenhaParedes();
    DesenhaMobiliarioImportado();
    DesenhaLampada(0.0f, 0.0f); // Luminária central sobre a sinuca.
}
