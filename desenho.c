#include "sinuca.h"

#include <math.h>
#include <stdio.h>

// Desenha uma caixa centralizada a partir das dimensões informadas.
void Caixa(float x, float y, float z, float comprimento, float altura, float largura, float r, float g, float b)
{
    glPushMatrix();
        glTranslatef(x, y, z); // Centro da caixa.
        glScalef(comprimento, altura, largura); // Escala para o tamanho desejado.
        glColor3f(r, g, b); // Cor da caixa.
        glutSolidCube(1.0); // Desenha um cubo unitário primeiro, que será escalado para o tamanho desejado.
    glPopMatrix();
}

// Desenha o tampo e as quatro bordas de madeira da mesa de sinuca.
static void DesenhaMesa(void)
{
    float alturaQuina = ALTURA_TAMPO; // Altura do centro das bordas de madeira.

    // Tampo verde da mesa de sinuca.
    Caixa(0.0f, ALTURA_TAMPO - ESPESSURA_TAMPO / 2.0f, 0.0f,
          COMPRIMENTO_MESA, ESPESSURA_TAMPO, LARGURA_MESA,
          0.0f, 0.35f, 0.08f); // Verde escuro.

    // Bordas de madeira ao redor da área jogável.
    Caixa(0.0f, alturaQuina,  LARGURA_MESA / 2.0f + LARGURA_BORDA / 2.0f, // Quina frontal.
          COMPRIMENTO_MESA + 2.0f * LARGURA_BORDA, ALTURA_BORDA, LARGURA_BORDA,
          0.30f, 0.13f, 0.04f); // Marrom escuro.

    Caixa(0.0f, alturaQuina, -LARGURA_MESA / 2.0f - LARGURA_BORDA / 2.0f, // Quina traseira.
          COMPRIMENTO_MESA + 2.0f * LARGURA_BORDA, ALTURA_BORDA, LARGURA_BORDA,
          0.30f, 0.13f, 0.04f); // Marrom escuro.

    Caixa( COMPRIMENTO_MESA / 2.0f + LARGURA_BORDA / 2.0f, alturaQuina, 0.0f, // Quina lateral direita.
          LARGURA_BORDA, ALTURA_BORDA, LARGURA_MESA,
          0.30f, 0.13f, 0.04f); // Marrom escuro.

    Caixa(-COMPRIMENTO_MESA / 2.0f - LARGURA_BORDA / 2.0f, alturaQuina, 0.0f, // Quina lateral esquerda.
          LARGURA_BORDA, ALTURA_BORDA, LARGURA_MESA,
          0.30f, 0.13f, 0.04f); // Marrom escuro.
}

// Desenha os quatro pés da mesa de sinuca, apoiados no chão.
static void DesenhaPesMesa(void)
{
    float alturaPe = ALTURA_TAMPO - ALTURA_BORDA / 2.0f;
    float xPe = COMPRIMENTO_MESA / 2.0f + LARGURA_BORDA - LARGURA_PE / 2.0f;
    float zPe = LARGURA_MESA / 2.0f + LARGURA_BORDA - LARGURA_PE / 2.0f;
    int i;
    int j;

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            float sinalX = (i == 0) ? -1.0f : 1.0f;
            float sinalZ = (j == 0) ? -1.0f : 1.0f;

            Caixa(sinalX * xPe, alturaPe / 2.0f, sinalZ * zPe,
                  LARGURA_PE, alturaPe, LARGURA_PE,
                  0.30f, 0.13f, 0.04f); // Marrom escuro.

            Caixa(sinalX * xPe, 0.05f, sinalZ * zPe,
                  LARGURA_PE + 0.10f, 0.10f, LARGURA_PE + 0.10f,
                  0.15f, 0.07f, 0.02f); // Sapata mais escura.
        }
    }
}

// Desenha uma caçapa como um círculo escuro sobre o tampo.
static void DesenhaCacapa(float x, float z, float raio)
{
    GLUquadric *quadric = gluNewQuadric();

    glPushMatrix();
        glTranslatef(x, ALTURA_TAMPO + 0.003f, z); // Posição da caçapa sobre o tampo.
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Coloca o disco na horizontal.
        glColor3f(0.03f, 0.03f, 0.03f); // Cor escura para representar o buraco.
        gluDisk(quadric, 0.0, raio, 24, 1);
    glPopMatrix();

    gluDeleteQuadric(quadric);
}

// Desenha quatro caçapas nos cantos e duas nas laterais.
static void DesenhaCacapas(void)
{
    float xCanto = COMPRIMENTO_MESA / 2.0f - RAIO_CACAPA_CANTO / 2.0f;
    float zCanto = LARGURA_MESA / 2.0f - RAIO_CACAPA_CANTO / 2.0f;
    float zLateral = LARGURA_MESA / 2.0f - RAIO_CACAPA_LATERAL / 2.0f;

    DesenhaCacapa( xCanto,  zCanto, RAIO_CACAPA_CANTO); // Canto direito frontal.
    DesenhaCacapa( xCanto, -zCanto, RAIO_CACAPA_CANTO); // Canto direito traseiro.
    DesenhaCacapa(-xCanto,  zCanto, RAIO_CACAPA_CANTO); // Canto esquerdo frontal.
    DesenhaCacapa(-xCanto, -zCanto, RAIO_CACAPA_CANTO); // Canto esquerdo traseiro.
    DesenhaCacapa(0.0f,  zLateral, RAIO_CACAPA_LATERAL); // Lateral frontal.
    DesenhaCacapa(0.0f, -zLateral, RAIO_CACAPA_LATERAL); // Lateral traseira.
}

// Desenha uma bola colorida na posição x e z da mesa.
static void DesenhaBola(float x, float z, float r, float g, float b)
{
    glPushMatrix();
        glTranslatef(x, ALTURA_TAMPO + RAIO_BOLA, z); // Coloca a bola sobre o tampo.
        glColor3f(r, g, b); // Cor da bola.
        glutSolidSphere(RAIO_BOLA, 24, 24); // Desenha a esfera sólida.
    glPopMatrix();
}

// Percorre as bolas ativas e desenha cada uma na posição atual.
static void DesenhaBolas(void)
{
    int i;

    for (i = 0; i < NUM_BOLAS; i++)
        if (bolaAtiva[i] == 1)
            DesenhaBola(posicaoX[i], posicaoZ[i], corR[i], corG[i], corB[i]);
}

// Desenha o taco atrás da bola branca, na direção escolhida.
static void DesenhaTaco(void)
{
    // Direção do taco, apontado para o centro da bola branca.
    float anguloRad = anguloTaco * PI / 180.0f;
    float direcaoX = cosf(anguloRad);
    float direcaoZ = sinf(anguloRad);
    // A ponta fica atrás da bola; a distância determina a força da tacada.
    float pontaX = posicaoX[BOLA_BRANCA] - direcaoX * (RAIO_BOLA + distanciaTaco);
    float pontaZ = posicaoZ[BOLA_BRANCA] - direcaoZ * (RAIO_BOLA + distanciaTaco);

    glPushMatrix();
        glTranslatef(pontaX, ALTURA_TAMPO + RAIO_BOLA, pontaZ); // Coloca a ponta atrás da bola.
        glRotatef(-anguloTaco, 0.0f, 1.0f, 0.0f); // Gira o taco em torno do eixo vertical.
        glTranslatef(-COMPRIMENTO_TACO / 2.0f, 0.0f, 0.0f); // Centraliza o taco a partir da ponta.
        glScalef(COMPRIMENTO_TACO, ESPESSURA_TACO, ESPESSURA_TACO);
        glColor3f(0.65f, 0.38f, 0.15f); // Cor de madeira.
        glutSolidCube(1.0); // Representa o taco com uma caixa fina.

        // Listra no taco indica a cor do jogador que está na vez.
        glPushMatrix();
            glTranslatef(0.0f, 0.55f, 0.0f);
            glScalef(0.75f, 0.14f, 1.08f);
            if (jogadorAtual == 1)
                glColor3f(0.85f, 0.04f, 0.04f); // Jogador 1: vermelho.
            else
                glColor3f(0.04f, 0.18f, 0.90f); // Jogador 2: azul.
            glutSolidCube(1.0);
        glPopMatrix();
    glPopMatrix();
}

// Desenha a barra 2D que mostra a força escolhida enquanto o espaço está pressionado.
static void DesenhaBarraForca(void)
{
    float larguraBarra = 360.0f;
    float alturaBarra = 26.0f;
    float x = (larguraJanela - larguraBarra) / 2.0f;
    float y = 45.0f;
    float marcadorX = x + forcaTacada * larguraBarra;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, larguraJanela, 0.0, alturaJanela);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Azul no início, amarelo no meio e vermelho escuro no final da barra.
    glBegin(GL_QUADS);
        glColor3f(0.25f, 0.75f, 1.0f);
        glVertex2f(x, y);
        glColor3f(1.0f, 0.72f, 0.08f);
        glVertex2f(x + larguraBarra * 0.65f, y);
        glVertex2f(x + larguraBarra * 0.65f, y + alturaBarra);
        glColor3f(0.25f, 0.75f, 1.0f);
        glVertex2f(x, y + alturaBarra);

        glColor3f(1.0f, 0.72f, 0.08f);
        glVertex2f(x + larguraBarra * 0.65f, y);
        glColor3f(0.48f, 0.02f, 0.02f);
        glVertex2f(x + larguraBarra, y);
        glVertex2f(x + larguraBarra, y + alturaBarra);
        glColor3f(1.0f, 0.72f, 0.08f);
        glVertex2f(x + larguraBarra * 0.65f, y + alturaBarra);
    glEnd();

    // Contorno e marcador branco indicam a força no instante atual.
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + larguraBarra, y);
        glVertex2f(x + larguraBarra, y + alturaBarra);
        glVertex2f(x, y + alturaBarra);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(marcadorX, y - 5.0f);
        glVertex2f(marcadorX, y + alturaBarra + 5.0f);
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

// Desenha um texto dentro da projeção 2D usada pelos placares.
static void DesenhaTexto2D(float x, float y, const char *texto)
{
    int i;

    glRasterPos2f(x, y);
    for (i = 0; texto[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, texto[i]);
}

// Desenha os placares dos dois jogadores e destaca quem está na vez.
static void DesenhaPlacar(void)
{
    int jogador;
    float larguraPlaca = 120.0f;
    float alturaPlaca = 30.0f;
    float y = alturaJanela - alturaPlaca - 12.0f;
    char texto[40];

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, larguraJanela, 0.0, alturaJanela);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    for (jogador = 1; jogador <= 2; jogador++)
    {
        float x = (jogador == 1) ? 12.0f : larguraJanela - larguraPlaca - 12.0f;
        int pontos = (jogador == 1) ? pontosJogador1 : pontosJogador2;
        float intensidade = (jogador == jogadorAtual) ? 1.0f : 0.60f;

        if (jogador == 1)
            glColor3f(0.72f * intensidade, 0.05f * intensidade, 0.05f * intensidade);
        else
            glColor3f(0.05f * intensidade, 0.18f * intensidade, 0.78f * intensidade);

        glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + larguraPlaca, y);
            glVertex2f(x + larguraPlaca, y + alturaPlaca);
            glVertex2f(x, y + alturaPlaca);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x, y);
            glVertex2f(x + larguraPlaca, y);
            glVertex2f(x + larguraPlaca, y + alturaPlaca);
            glVertex2f(x, y + alturaPlaca);
        glEnd();

        snprintf(texto, sizeof(texto), "JOGADOR %d: %d", jogador, pontos);

        DesenhaTexto2D(x + 10.0f, y + 10.0f, texto);
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

// Configura a iluminação da cena.
static void ConfiguraIluminacao(void)
{
    GLfloat posicao[] = { 0.0f, 6.0f, 0.0f, 1.0f };      // Em cima da mesa, iluminando de cima para baixo.
    GLfloat ambiente[] = { 0.20f, 0.20f, 0.20f, 1.0f };  // Luz fraca que chega em todos os lados.
    GLfloat difusa[] = { 0.35f, 0.35f, 0.35f, 1.0f };    // Iluminação geral do salão.
    GLfloat especular[] = { 0.35f, 0.35f, 0.35f, 1.0f }; // Brilho refletido pela luz.
    GLfloat materialEspecular[] = { 0.20f, 0.20f, 0.20f, 1.0f }; // Reflexo discreto do material.
    GLfloat brilho[] = { 20.0f }; // Concentração do brilho.

    // Ativa a iluminação, o uso das cores dos objetos e as normais corrigidas.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); // Aplica a cor também no lado interno dos modelos.

    // Define a luz geral e o brilho discreto dos materiais.
    glLightfv(GL_LIGHT0, GL_POSITION, posicao);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialEspecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, brilho);

    ConfiguraLuzesCenario(); // Acende a luminária central sobre a mesa.
}

// Configura a projeção da cena, definindo o campo de visão e a proporção da janela.
static void ConfiguraProjecao(int largura, int altura)
{
    if (altura == 0)
        altura = 1; // Evita divisão por zero.

    glViewport(0, 0, largura, altura); // Define a região da janela onde a cena será exibida.
    glMatrixMode(GL_PROJECTION); // Seleciona a matriz de projeção.
    glLoadIdentity(); // Reinicia a matriz de projeção.
    gluPerspective(45.0, (float)largura / (float)altura, 0.1, 30.0); // Campo de visão, proporção, perto e longe.
}

// Calcula a posição da câmera em volta do centro da mesa.
static void ConfiguraCamera(void)
{
    // Converte os ângulos de graus para radianos para movimentação suave.
    float horizontalRad = anguloHorizontal * PI / 180.0f;
    float verticalRad = anguloVertical * PI / 180.0f;
    float cameraX;
    float cameraY;
    float cameraZ;

    cameraX = distanciaCamera * cosf(verticalRad) * sinf(horizontalRad);
    cameraY = 1.0f + distanciaCamera * sinf(verticalRad);
    cameraZ = distanciaCamera * cosf(verticalRad) * cosf(horizontalRad);

    gluLookAt(cameraX, cameraY, cameraZ,
              0.0, 1.0, 0.0,
              0.0, 1.0, 0.0);
}

// Função de desenho da cena.
void Desenha(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa a imagem e as profundidades anteriores.

    glMatrixMode(GL_MODELVIEW); // Seleciona a matriz de visualização.
    glLoadIdentity(); // Reinicia a matriz de visualização.

    // Posição do observador, ponto observado e direção vertical.
    ConfiguraCamera();

    // Configura a iluminação da cena.
    ConfiguraIluminacao();

    // Desenha os objetos da cena.
    DesenhaCenario();
    DesenhaMesa();
    DesenhaPesMesa();
    DesenhaCacapas();
    DesenhaBolas();
    if (bolaEmMovimento == 0)
        DesenhaTaco();
    if (carregandoTacada == 1)
        DesenhaBarraForca();
    DesenhaPlacar();

    glutSwapBuffers(); // Troca o buffer desenhado pelo que será exibido.
}

// Função chamada quando a janela é redimensionada.
void Redimensiona(int largura, int altura)
{
    larguraJanela = largura;
    alturaJanela = (altura > 0) ? altura : 1;
    ConfiguraProjecao(largura, alturaJanela); // Atualiza a projeção quando a janela muda de tamanho.
}
