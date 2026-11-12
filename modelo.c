#include "sinuca.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Guarda a lista de exibição criada a partir de um arquivo OBJ.
typedef struct
{
    GLuint lista;
    int carregado;
} Modelo;

static Modelo modeloMesaPlastico;
static Modelo modeloCadeiraPlastico;
static Modelo modeloLata;
static Modelo modeloCaixaSomCorpo;
static Modelo modeloCaixaSomBase;
static Modelo modeloCaixaSomGrade;
static Modelo modeloCaixaSomDetalhes;
static Modelo modeloCerveja;
static Modelo modeloLataVermelha;
static Modelo modeloDamas;

// Lê um índice de uma face OBJ nos formatos v, v/vt, v//vn ou v/vt/vn.
static int LeIndiceFace(char **texto, int numVertices, int numNormais, int *vertice, int *normal)
{
    char *inicio = *texto;
    char *fim;
    long indiceVertice;
    long indiceNormal = 0;

    while (*inicio == ' ' || *inicio == '\t')
        inicio++;
    if (*inicio == '\0' || *inicio == '\n' || *inicio == '\r')
        return 0;

    indiceVertice = strtol(inicio, &fim, 10);
    if (fim == inicio)
        return 0;
    inicio = fim;

    if (*inicio == '/')
    {
        inicio++;
        if (*inicio != '/')
        {
            strtol(inicio, &fim, 10); // Ignora a coordenada de textura, se existir.
            inicio = fim;
        }
        if (*inicio == '/')
        {
            inicio++;
            indiceNormal = strtol(inicio, &fim, 10);
            if (fim != inicio)
                inicio = fim;
            else
                indiceNormal = 0;
        }
    }

    if (indiceVertice < 0)
        indiceVertice = numVertices + indiceVertice + 1;
    if (indiceNormal < 0)
        indiceNormal = numNormais + indiceNormal + 1;

    *vertice = (int)indiceVertice - 1;
    *normal = (indiceNormal > 0) ? (int)indiceNormal - 1 : -1;
    *texto = inicio;
    return 1;
}

// Lê vértices, normais e faces de um OBJ e cria uma lista de exibição OpenGL.
static int CarregaModelo(Modelo *modelo, const char *arquivo)
{
    FILE *arquivoObj = fopen(arquivo, "r");
    char linha[512];
    float *vertices;
    float *normais;
    int quantidadeVertices = 0;
    int quantidadeNormais = 0;
    int verticesLidos = 0;
    int normaisLidas = 0;
    int indicesVertices[16];
    int indicesNormais[16];
    int quantidadeIndices;
    int i;
    char *texto;

    modelo->carregado = 0;
    if (arquivoObj == NULL)
    {
        printf("Aviso: não foi possível abrir o modelo '%s'.\n", arquivo);
        return 0;
    }

    // Conta os dados antes de reservar a memória necessária.
    while (fgets(linha, sizeof(linha), arquivoObj) != NULL)
    {
        if (strncmp(linha, "v ", 2) == 0)
            quantidadeVertices++;
        else if (strncmp(linha, "vn ", 3) == 0)
            quantidadeNormais++;
    }
    rewind(arquivoObj);

    vertices = malloc(3 * (size_t)(quantidadeVertices > 0 ? quantidadeVertices : 1) * sizeof(float));
    normais = malloc(3 * (size_t)(quantidadeNormais > 0 ? quantidadeNormais : 1) * sizeof(float));
    if (vertices == NULL || normais == NULL)
    {
        free(vertices);
        free(normais);
        fclose(arquivoObj);
        return 0;
    }

    modelo->lista = glGenLists(1);
    if (modelo->lista == 0)
    {
        free(vertices);
        free(normais);
        fclose(arquivoObj);
        return 0;
    }

    // Lê o OBJ novamente e guarda seus triângulos em uma lista do OpenGL.
    glNewList(modelo->lista, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    while (fgets(linha, sizeof(linha), arquivoObj) != NULL)
    {
        if (strncmp(linha, "v ", 2) == 0)
        {
            sscanf(linha + 2, "%f %f %f", &vertices[3 * verticesLidos],
                   &vertices[3 * verticesLidos + 1], &vertices[3 * verticesLidos + 2]);
            verticesLidos++;
        }
        else if (strncmp(linha, "vn ", 3) == 0)
        {
            sscanf(linha + 3, "%f %f %f", &normais[3 * normaisLidas],
                   &normais[3 * normaisLidas + 1], &normais[3 * normaisLidas + 2]);
            normaisLidas++;
        }
        else if (strncmp(linha, "f ", 2) == 0)
        {
            texto = linha + 2;
            quantidadeIndices = 0;
            while (quantidadeIndices < 16 &&
                   LeIndiceFace(&texto, verticesLidos, normaisLidas,
                                &indicesVertices[quantidadeIndices], &indicesNormais[quantidadeIndices]))
            {
                quantidadeIndices++;
            }

            // Divide faces com mais de três vértices em triângulos.
            for (i = 1; i + 1 < quantidadeIndices; i++)
            {
                int a = indicesVertices[0];
                int b = indicesVertices[i];
                int c = indicesVertices[i + 1];
                int usaNormais = indicesNormais[0] >= 0 && indicesNormais[i] >= 0 &&
                                  indicesNormais[i + 1] >= 0 && indicesNormais[0] < normaisLidas &&
                                  indicesNormais[i] < normaisLidas && indicesNormais[i + 1] < normaisLidas;

                if (a < 0 || b < 0 || c < 0 || a >= verticesLidos || b >= verticesLidos || c >= verticesLidos)
                    continue;

                if (usaNormais)
                {
                    glNormal3fv(&normais[3 * indicesNormais[0]]);
                    glVertex3fv(&vertices[3 * a]);
                    glNormal3fv(&normais[3 * indicesNormais[i]]);
                    glVertex3fv(&vertices[3 * b]);
                    glNormal3fv(&normais[3 * indicesNormais[i + 1]]);
                    glVertex3fv(&vertices[3 * c]);
                }
                else
                {
                    float ux = vertices[3 * b] - vertices[3 * a];
                    float uy = vertices[3 * b + 1] - vertices[3 * a + 1];
                    float uz = vertices[3 * b + 2] - vertices[3 * a + 2];
                    float vx = vertices[3 * c] - vertices[3 * a];
                    float vy = vertices[3 * c + 1] - vertices[3 * a + 1];
                    float vz = vertices[3 * c + 2] - vertices[3 * a + 2];
                    float nx = uy * vz - uz * vy;
                    float ny = uz * vx - ux * vz;
                    float nz = ux * vy - uy * vx;
                    float tamanho = sqrtf(nx * nx + ny * ny + nz * nz);

                    if (tamanho > 0.0f)
                        glNormal3f(nx / tamanho, ny / tamanho, nz / tamanho);
                    glVertex3fv(&vertices[3 * a]);
                    glVertex3fv(&vertices[3 * b]);
                    glVertex3fv(&vertices[3 * c]);
                }
            }
        }
    }
    glEnd();
    glEndList();

    free(vertices);
    free(normais);
    fclose(arquivoObj);
    modelo->carregado = 1;
    return 1;
}

// Carrega os OBJ usados no mobiliário do boteco.
void CarregaModelos(void)
{
    CarregaModelo(&modeloMesaPlastico, "assets/modelos/mesa_plastico.obj");
    CarregaModelo(&modeloCadeiraPlastico, "assets/modelos/cadeira_plastico.obj");
    CarregaModelo(&modeloLata, "assets/modelos/lata_colorida.obj");
    CarregaModelo(&modeloLataVermelha, "assets/modelos/lata_vermelha.obj");
    CarregaModelo(&modeloCerveja, "assets/modelos/cerveja.obj");
    CarregaModelo(&modeloDamas, "assets/modelos/tabuleiro_damas.obj");
    CarregaModelo(&modeloCaixaSomCorpo, "assets/modelos/caixa_som/corpo.obj");
    CarregaModelo(&modeloCaixaSomBase, "assets/modelos/caixa_som/base.obj");
    CarregaModelo(&modeloCaixaSomGrade, "assets/modelos/caixa_som/grade.obj");
    CarregaModelo(&modeloCaixaSomDetalhes, "assets/modelos/caixa_som/detalhes.obj");
}

// Desenha um OBJ em uma altura escolhida, usando uma cor definida para o conjunto inteiro.
static void DesenhaModelo(Modelo *modelo, float x, float y, float z, float angulo, float r, float g, float b)
{
    if (modelo->carregado == 0)
        return;

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(angulo, 0.0f, 1.0f, 0.0f);
        glColor3f(r, g, b);
        glCallList(modelo->lista);
    glPopMatrix();
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

// Desenha uma mesa plástica amarela ou vermelha em uma posição do salão.
static void DesenhaMesaPlastica(float x, float z, float angulo, int vermelha)
{
    if (vermelha)
        DesenhaModelo(&modeloMesaPlastico, x, 0.0f, z, angulo,
                       0.62f, 0.12f, 0.10f); // Vermelho de plástico gasto.
    else
        DesenhaModelo(&modeloMesaPlastico, x, 0.0f, z, angulo,
                       0.78f, 0.66f, 0.18f); // Amarelo de plástico gasto.
}

// Acrescenta uma tampa metálica simples à lata importada.
static void DesenhaTampaLata(float x, float y, float z, float raio)
{
    GLUquadric *quadric = gluNewQuadric();

    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glColor3f(0.72f, 0.72f, 0.70f); // Alumínio fosco.
        gluDisk(quadric, 0.0f, raio, 20, 1);
    glPopMatrix();

    gluDeleteQuadric(quadric);
}

// Desenha uma lata importada sobre uma das mesas do boteco.
static void DesenhaLata(float x, float z, float r, float g, float b)
{
    DesenhaModelo(&modeloLata, x, 1.72f, z, 0.0f, r, g, b);
    DesenhaTampaLata(x, 2.051f, z, 0.082f);
}

// Desenha uma garrafa de cerveja importada sobre uma mesa do boteco.
static void DesenhaCerveja(float x, float z)
{
    glPushMatrix();
        glTranslatef(x, 1.92f, z);
        glScalef(1.40f, 1.40f, 1.40f);
        DesenhaModelo(&modeloCerveja, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.50f, 0.29f, 0.10f); // Garrafa âmbar.
    glPopMatrix();
}

// Desenha uma lata vermelha importada sobre uma mesa do boteco.
static void DesenhaLataVermelha(float x, float z)
{
    glPushMatrix();
        glTranslatef(x, 1.97f, z);
        glScalef(2.00f, 2.00f, 2.00f);
        DesenhaModelo(&modeloLataVermelha, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.68f, 0.11f, 0.09f); // Lata vermelha.
        DesenhaTampaLata(0.0f, 0.035f, 0.0f, 0.040f);
    glPopMatrix();
}

// Colore as casas do tabuleiro sobre a moldura de madeira importada.
static void DesenhaCasasTabuleiro(void)
{
    int i;
    int j;
    float lado = 0.15f;

    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            float x = -0.60f + i * lado;
            float z = -0.60f + j * lado;

            if ((i + j) % 2 == 0)
                glColor3f(0.76f, 0.69f, 0.54f); // Madeira clara.
            else
                glColor3f(0.28f, 0.17f, 0.09f); // Madeira escura.

            glVertex3f(x, 0.020f, z);
            glVertex3f(x, 0.020f, z + lado);
            glVertex3f(x + lado, 0.020f, z + lado);
            glVertex3f(x + lado, 0.020f, z);
        }
    }
    glEnd();
}

// Desenha um tabuleiro de damas importado sobre uma mesa do boteco.
static void DesenhaTabuleiroDamas(float x, float z)
{
    glPushMatrix();
        glTranslatef(x, 1.76f, z);
        glScalef(0.62f, 0.62f, 0.62f);
        DesenhaModelo(&modeloDamas, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.40f, 0.26f, 0.12f); // Moldura de madeira.
        DesenhaCasasTabuleiro();
    glPopMatrix();
}

// Desenha uma cadeira solta no salão.
static void DesenhaCadeiraSolta(float x, float z, float angulo, float r, float g, float b)
{
    DesenhaModelo(&modeloCadeiraPlastico, x, 0.0f, z, angulo, r, g, b);
}

// Desenha uma cadeira em um dos lados da mesa, virada para o tampo.
static void DesenhaCadeiraDaMesa(int lado, float desvio, float r, float g, float b)
{
    if (lado == 0)
        DesenhaCadeiraSolta(0.00f, -1.90f, 180.0f + desvio, r, g, b);
    else if (lado == 1)
        DesenhaCadeiraSolta(0.00f, 1.90f, desvio, r, g, b);
    else if (lado == 2)
        DesenhaCadeiraSolta(-1.90f, 0.00f, -90.0f + desvio, r, g, b);
    else
        DesenhaCadeiraSolta(1.90f, 0.00f, 90.0f + desvio, r, g, b);
}

// Agrupa uma mesa, objetos sobre o tampo e cadeiras nos lados livres.
static void DesenhaMesaBar(float x, float z, float angulo, int tipo, int vermelha)
{
    float rCadeira = vermelha ? 0.62f : 0.78f;
    float gCadeira = vermelha ? 0.12f : 0.66f;
    float bCadeira = vermelha ? 0.10f : 0.18f;

    glPushMatrix();
        glTranslatef(x, 0.0f, z);
        glRotatef(angulo, 0.0f, 1.0f, 0.0f);
        DesenhaMesaPlastica(0.0f, 0.0f, 0.0f, vermelha);

        if (tipo == 0)
        {
            DesenhaCerveja(-0.20f, 0.22f);
            DesenhaLataVermelha(0.38f, 0.26f);
        }
        else if (tipo == 1)
        {
            DesenhaLata(0.32f, 0.20f, 0.12f, 0.30f, 0.58f); // Lata azul.
            DesenhaCerveja(-0.30f, -0.18f);
        }
        else
        {
            DesenhaTabuleiroDamas(0.00f, 0.02f);
        }

        // Nas mesas centrais, a cadeira lateral fica do lado de fora do salão.
        if (z > -1.0f && z < 1.0f)
        {
            if (x < 0.0f)
                DesenhaCadeiraDaMesa(2, -5.0f, rCadeira, gCadeira, bCadeira);
            else
                DesenhaCadeiraDaMesa(3, 5.0f, rCadeira, gCadeira, bCadeira);
        }
        else if (x < 0.0f)
            DesenhaCadeiraDaMesa(3, 5.0f, rCadeira, gCadeira, bCadeira);
        else
            DesenhaCadeiraDaMesa(2, -5.0f, rCadeira, gCadeira, bCadeira);

        // Mesas das pontas têm uma cadeira para fora; as do meio têm duas.
        if (z < 1.0f)
            DesenhaCadeiraDaMesa(0, -6.0f, rCadeira, gCadeira, bCadeira);
        if (z > -1.0f)
            DesenhaCadeiraDaMesa(1, 7.0f, rCadeira, gCadeira, bCadeira);
    glPopMatrix();
}

// Desenha uma caixa de som importada, alta e voltada para o salão.
static void DesenhaCaixaSom(float x)
{
    glPushMatrix();
        glTranslatef(x, 2.85f, POSICAO_PAREDE_Z - 0.54f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glScalef(2.80f, 2.80f, 2.80f);
        DesenhaModelo(&modeloCaixaSomCorpo, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.13f, 0.13f, 0.13f); // Gabinete escuro.
        DesenhaModelo(&modeloCaixaSomBase, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.48f, 0.48f, 0.48f); // Base cinza.
        DesenhaModelo(&modeloCaixaSomGrade, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.06f, 0.06f, 0.06f); // Grade preta.
        DesenhaModelo(&modeloCaixaSomDetalhes, 0.0f, 0.0f, 0.0f, 0.0f,
                       0.77f, 0.77f, 0.77f); // Detalhes claros.
    glPopMatrix();
}

// Distribui as mesas pelas laterais com pequenos desvios de posição e rotação.
void DesenhaMobiliarioImportado(void)
{
    DesenhaMesaBar(-6.25f, -4.15f,  6.0f, 0, 0);
    DesenhaMesaBar(-6.15f, -0.18f, -7.0f, 1, 1);
    DesenhaMesaBar(-6.35f,  4.15f,  3.0f, 2, 0);

    DesenhaMesaBar( 6.30f, -4.20f, -5.0f, 1, 1);
    DesenhaMesaBar( 6.15f,  0.22f,  8.0f, 2, 0);
    DesenhaMesaBar( 6.30f,  4.20f, -4.0f, 0, 0);

    // Duas cadeiras afastadas da sinuca ocupam os espaços vazios do salão.
    DesenhaCadeiraSolta(-0.85f,  4.65f,  35.0f, 0.62f, 0.12f, 0.10f);
    DesenhaCadeiraSolta( 1.10f, -4.55f, -25.0f, 0.78f, 0.66f, 0.18f);

    DesenhaCaixaSom(-4.60f);
    DesenhaCaixaSom( 4.60f);
}
