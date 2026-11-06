#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

// Dimensões dos objetos da cena:
#define COMPRIMENTO_MESA 6.0f     // 2,54 m.
#define LARGURA_MESA     3.0f     // 1,27 m.

#define ALTURA_TAMPO     1.80f    // 0,76 m.
#define ESPESSURA_TAMPO  0.12f    // 0,048 m.

#define LARGURA_BORDA    0.3000f     // 0,12 m.
#define ALTURA_BORDA     0.2500f     // 0,10 m.
#define RAIO_BOLA           0.0675f  // 0,028575 m.
#define RAIO_CACAPA_CANTO   0.135f   // 0,05715 m.
#define RAIO_CACAPA_LATERAL 0.150f   // 0,0635 m.
#define COMPRIMENTO_TACO     3.47f   // 1,47 m.
#define ESPESSURA_TACO       0.035f  // 0,015 m.
#define RAIO_SELECAO          0.095f

// Parâmetros da mira e da força da tacada.
#define DISTANCIA_MIN_TACO   0.02f // Quase encostado na bola branca.
#define DISTANCIA_MAX_TACO   0.90f
#define PASSO_IMPACTO         0.01f
#define DESVIO_MAXIMO_IMPACTO (0.65f * RAIO_BOLA)
#define DURACAO_CARGA_TACADA 1500
#define CURVA_CARGA_TACADA 2.3f
#define PASSO_CONTINUO_TACO 1.0f

// Parâmetros da animação e colisão.
#define NUM_BOLAS            16
#define BOLA_BRANCA          0
#define VELOCIDADE_MINIMA    0.30f
#define VELOCIDADE_MAXIMA    10.00f
#define ATRITO               0.988f
#define ELASTICIDADE_BOLAS   0.92f
#define PERDA_NA_BORDA       0.70f
#define VELOCIDADE_PARADA    0.03f
#define INTERVALO_ANIMACAO   16

// Parâmetros da câmera virtual (ajustando a sensibilidade da movimentação da câmera):
#define PASSO_CONTINUO_CAMERA 1.2f
#define PI           3.14159265f
// Posição inicial da câmera virtual pensando na primeira tacada!
float anguloHorizontal = -90.0f; 
float anguloVertical = 45.0f;   
float distanciaCamera = 9.0f;

// Posição, velocidade e cor de cada bola da mesa.
float posicaoX[NUM_BOLAS];
float posicaoZ[NUM_BOLAS];
float velocidadeX[NUM_BOLAS];
float velocidadeZ[NUM_BOLAS];
float corR[NUM_BOLAS];
float corG[NUM_BOLAS];
float corB[NUM_BOLAS];
int bolaAtiva[NUM_BOLAS];
int pontos = 0;

// Estado inicial do taco e da animação.
float anguloTaco = 0.0f;
float distanciaTaco = 0.20f;
int bolaEmMovimento = 0;
int bolaBrancaSelecionada = 0;
float desvioImpacto = 0.0f;
int carregandoTacada = 0;
int inicioCargaTacada = 0;
float forcaTacada = 0.0f;
int larguraJanela = 900;
int alturaJanela = 600;
int giraTacoEsquerda = 0;
int giraTacoDireita = 0;
int cameraEsquerda = 0;
int cameraDireita = 0;
int cameraCima = 0;
int cameraBaixo = 0;
int cameraAproxima = 0;
int cameraAfasta = 0;

// Uma maneira prática de desenhar uma peça retangular centralizada.
void Caixa(float x, float y, float z, float comprimento, float altura, float largura, float r, float g, float b) 
{
    glPushMatrix();
        glTranslatef(x, y, z); // Centro da caixa.
        glScalef(comprimento, altura, largura); // Escala para o tamanho desejado.
        glColor3f(r, g, b); // Cor da caixa.
        glutSolidCube(1.0); // Desenha um cubo unitário primeiro, que será escalado para o tamanho desejado.
    glPopMatrix();
}

// Desenha a mesa de sinuca com tampo verde e quinas de madeira.
void DesenhaMesa(void)
{
    float ALTURA_QUINA = ALTURA_TAMPO + ESPESSURA_TAMPO / 2.0f; // Altura do centro das bordas de madeira (quinas).

    // Tampo verde da mesa de sinuca.
    Caixa(0.0f, ALTURA_TAMPO - ESPESSURA_TAMPO / 2.0f, 0.0f, 
          COMPRIMENTO_MESA, ESPESSURA_TAMPO, LARGURA_MESA,
          0.0f, 0.35f, 0.08f); // Verde escuro.

    // Quinas
    Caixa(0.0f, ALTURA_QUINA,  LARGURA_MESA / 2.0f + LARGURA_BORDA / 2.0f, // Quina frontal.
          COMPRIMENTO_MESA + 2.0f * LARGURA_BORDA, ALTURA_BORDA, LARGURA_BORDA,
          0.30f, 0.13f, 0.04f); // Marrom escuro.

    Caixa(0.0f, ALTURA_QUINA, -LARGURA_MESA / 2.0f - LARGURA_BORDA / 2.0f, // Quina traseira.
          COMPRIMENTO_MESA + 2.0f * LARGURA_BORDA, ALTURA_BORDA, LARGURA_BORDA,
          0.30f, 0.13f, 0.04f); // Marrom escuro.

    Caixa( COMPRIMENTO_MESA / 2.0f + LARGURA_BORDA / 2.0f, ALTURA_QUINA, 0.0f, // Quina lateral direita.
          LARGURA_BORDA, ALTURA_BORDA, LARGURA_MESA,
          0.30f, 0.13f, 0.04f); // Marrom escuro.

    Caixa(-COMPRIMENTO_MESA / 2.0f - LARGURA_BORDA / 2.0f, ALTURA_QUINA, 0.0f, // Quina lateral esquerda.
          LARGURA_BORDA, ALTURA_BORDA, LARGURA_MESA,
          0.30f, 0.13f, 0.04f); // Marrom escuro.

}

// Desenha uma bola colorida na posição x e z da mesa.
void DesenhaBola(float x, float z, float r, float g, float b)
{
    glPushMatrix();
        glTranslatef(x, ALTURA_TAMPO + RAIO_BOLA, z); // Coloca a bola sobre o tampo.
        glColor3f(r, g, b); // Cor da bola.
        glutSolidSphere(RAIO_BOLA, 24, 24); // Desenha a esfera sólida.
    glPopMatrix();
}

// Define a posição e a cor de uma bola, deixando-a inicialmente parada.
void DefineBola(int indice, float x, float z, float r, float g, float b)
{
    // Inicializa a posição, velocidade e cor da bola.
    posicaoX[indice] = x;
    posicaoZ[indice] = z;
    velocidadeX[indice] = 0.0f;
    velocidadeZ[indice] = 0.0f;
    corR[indice] = r;
    corG[indice] = g;
    corB[indice] = b;
    bolaAtiva[indice] = 1;
}

// Define a bola branca e o triângulo de bolas coloridas na posição inicial.
void InicializaBolas(void)
{
    float inicioX = 1.20f; // Posição da primeira bola do triângulo (estimado).
    float avancoX = 1.732f * RAIO_BOLA; // Distância entre as fileiras do triângulo (1.732 = raiz quadrada de 3).
    float avancoZ = 2.0f * RAIO_BOLA; // Distância entre bolas da mesma fileira (ou seja, estão encostadas).

    DefineBola(0, -1.5f, 0.0f, 0.95f, 0.95f, 0.95f);                              // Bola branca.

    // Triângulo de bolas com cores tradicionais:
    DefineBola(1, inicioX, 0.0f, 1.0f, 1.0f, 0.0f);                               // Bola 1: Amarela.

    DefineBola(2, inicioX + avancoX, -0.5f * avancoZ, 1.0f, 1.0f, 0.0f);          // Bola 9: amarela.
    DefineBola(3, inicioX + avancoX,  0.5f * avancoZ, 0.0f, 0.0f, 1.0f);          // Bola 2: azul.

    DefineBola(4, inicioX + 2.0f * avancoX, -avancoZ, 1.0f, 0.0f, 0.0f);          // Bola 3: vermelha.
    DefineBola(5, inicioX + 2.0f * avancoX, 0.0f, 0.05f, 0.05f, 0.05f);           // Bola 8: preta.
    DefineBola(6, inicioX + 2.0f * avancoX,  avancoZ, 0.0f, 0.0f, 1.0f);          // Bola 10: azul.

    DefineBola(7, inicioX + 3.0f * avancoX, -1.5f * avancoZ, 1.0f, 0.0f, 0.0f);   // Bola 11: vermelha.
    DefineBola(8, inicioX + 3.0f * avancoX, -0.5f * avancoZ, 0.50f, 0.0f, 0.50f); // Bola 4: roxa.
    DefineBola(9, inicioX + 3.0f * avancoX,  0.5f * avancoZ, 0.50f, 0.0f, 0.50f); // Bola 12: roxa.
    DefineBola(10, inicioX + 3.0f * avancoX, 1.5f * avancoZ, 1.0f, 0.50f, 0.0f);  // Bola 5: laranja.

    DefineBola(11, inicioX + 4.0f * avancoX, -2.0f * avancoZ, 0.0f, 0.50f, 0.0f); // Bola 6: verde.
    DefineBola(12, inicioX + 4.0f * avancoX, -1.0f * avancoZ, 1.0f, 0.50f, 0.0f); // Bola 13: laranja.
    DefineBola(13, inicioX + 4.0f * avancoX, 0.0f, 0.50f, 0.0f, 0.0f);            // Bola 7: vinho.
    DefineBola(14, inicioX + 4.0f * avancoX, 1.0f * avancoZ, 0.0f, 0.50f, 0.0f);  // Bola 14: verde.
    DefineBola(15, inicioX + 4.0f * avancoX, 2.0f * avancoZ, 0.50f, 0.0f, 0.0f);  // Bola 15: vinho.
}

// Desenha as dezesseis bolas nas posições atuais.
void DesenhaBolas(void)
{
    int i;

    for (i = 0; i < NUM_BOLAS; i++)
        if (bolaAtiva[i] == 1)
            DesenhaBola(posicaoX[i], posicaoZ[i], corR[i], corG[i], corB[i]);
}

// Desenha o taco atrás da bola branca, na direção escolhida.
void DesenhaTaco(void)
{
    // Cilindro representando o taco, posicionado atrás da bola branca.
    float anguloRad = anguloTaco * PI / 180.0f;
    float direcaoX = cosf(anguloRad);
    float direcaoZ = sinf(anguloRad);
    float lateralX = -direcaoZ;
    float lateralZ = direcaoX;
    float profundidade = sqrtf(RAIO_BOLA * RAIO_BOLA - desvioImpacto * desvioImpacto);
    // Calcula a posição da ponta do taco, considerando a distância da bola branca.
    // Isso vai permiti simular a força da tacada de acordo com a distância da ponta do taco até a bola branca.
    float pontaX = posicaoX[BOLA_BRANCA] - direcaoX * (profundidade + distanciaTaco) + lateralX * desvioImpacto;
    float pontaZ = posicaoZ[BOLA_BRANCA] - direcaoZ * (profundidade + distanciaTaco) + lateralZ * desvioImpacto;

    glPushMatrix();
        glTranslatef(pontaX, ALTURA_TAMPO + RAIO_BOLA, pontaZ); // Coloca a ponta atrás da bola.
        glRotatef(-anguloTaco, 0.0f, 1.0f, 0.0f); // Gira o taco em torno do eixo vertical.
        glTranslatef(-COMPRIMENTO_TACO / 2.0f, 0.0f, 0.0f); // Centraliza o taco a partir da ponta.
        glScalef(COMPRIMENTO_TACO, ESPESSURA_TACO, ESPESSURA_TACO);
        glColor3f(0.65f, 0.38f, 0.15f); // Cor de madeira.
        glutSolidCube(1.0); // Representa o taco com uma caixa fina.
    glPopMatrix();
}

// Desenha um anel amarelo para indicar que a bola branca foi selecionada.
void DesenhaSelecaoBranca(void)
{
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);

    glPushMatrix();
        glTranslatef(posicaoX[BOLA_BRANCA], ALTURA_TAMPO + 0.008f, posicaoZ[BOLA_BRANCA]);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glutWireTorus(0.006f, RAIO_SELECAO, 8, 24);
    glPopMatrix();

    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

// Desenha a barra 2D que mostra a força escolhida enquanto o espaço está pressionado.
void DesenhaBarraForca(void)
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

// Desenha uma caçapa como um círculo escuro sobre o tampo.
void DesenhaCacapa(float x, float z, float raio)
{
    // Cria um objeto quadric (uma superfície curva)para desenhar o disco da caçapa.
    GLUquadric *quadric = gluNewQuadric();

    glPushMatrix();
        glTranslatef(x, ALTURA_TAMPO + 0.003f, z); // Posição da caçapa sobre o tampo.
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Coloca o círculo na posição horizontal.
        glColor3f(0.03f, 0.03f, 0.03f); // Cor escura para representar o buraco.
        gluDisk(quadric, 0.0, raio, 24, 1); // Desenha um círculo preenchido.
    glPopMatrix();

    gluDeleteQuadric(quadric);
}

// Desenha quatro caçapas nos cantos e duas nas laterais.
void DesenhaCacapas(void)
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

// Atualiza o título da janela com a quantidade de bolas coloridas encaçapadas.
void AtualizaTitulo(void)
{
    char titulo[50];

    snprintf(titulo, sizeof(titulo), "Sinuca 3D - Pontos: %d", pontos);
    glutSetWindowTitle(titulo);
}

// Verifica se o centro de uma bola entrou na abertura de uma caçapa.
int BolaNaCacapa(int indice, float xCacapa, float zCacapa, float raioCacapa)
{
    float dx = posicaoX[indice] - xCacapa;
    float dz = posicaoZ[indice] - zCacapa;

    // Compara as distâncias ao quadrado para não precisar calcular raiz quadrada.
    return dx * dx + dz * dz < raioCacapa * raioCacapa;
}

// Remove uma bola colorida da mesa ou reposiciona a bola branca.
void EncaçapaBola(int indice)
{
    velocidadeX[indice] = 0.0f;
    velocidadeZ[indice] = 0.0f;

    if (indice == BOLA_BRANCA)
    {
        // A branca não vale ponto: ela volta para sua posição inicial.
        posicaoX[indice] = -1.5f;
        posicaoZ[indice] = 0.0f;
    }
    else
    {
        // Bola inativa não é mais desenhada, atualizada nem testada em colisões.
        bolaAtiva[indice] = 0;
        pontos++; // Cada bola colorida encaçapada vale um ponto.
        printf("Ponto! Total: %d\n", pontos);
        AtualizaTitulo();
    }
}

// Testa cada bola ativa contra as seis caçapas da mesa.
void VerificaCacapas(void)
{
    int i;
    float xCanto = COMPRIMENTO_MESA / 2.0f - RAIO_CACAPA_CANTO / 2.0f;
    float zCanto = LARGURA_MESA / 2.0f - RAIO_CACAPA_CANTO / 2.0f;
    float zLateral = LARGURA_MESA / 2.0f - RAIO_CACAPA_LATERAL / 2.0f;

    for (i = 0; i < NUM_BOLAS; i++)
    {
        // Não é necessário testar bolas que já foram encaçapadas.
        if (bolaAtiva[i] == 0)
            continue;

        // A bola é encaçapada ao entrar em qualquer uma das seis aberturas.
        if (BolaNaCacapa(i,  xCanto,  zCanto, RAIO_CACAPA_CANTO) ||
            BolaNaCacapa(i,  xCanto, -zCanto, RAIO_CACAPA_CANTO) ||
            BolaNaCacapa(i, -xCanto,  zCanto, RAIO_CACAPA_CANTO) ||
            BolaNaCacapa(i, -xCanto, -zCanto, RAIO_CACAPA_CANTO) ||
            BolaNaCacapa(i, 0.0f,  zLateral, RAIO_CACAPA_LATERAL) ||
            BolaNaCacapa(i, 0.0f, -zLateral, RAIO_CACAPA_LATERAL))
        {
            EncaçapaBola(i);
        }
    }
}

// Configura a iluminação da cena.
void ConfiguraIluminacao(void)
{
    // Testar diferentes valores para a luz e o material, para ver o que fica melhor!
    GLfloat posicao[] = { 0.0f, 6.0f, 0.0f, 1.0f };      // Em cima da mesa, iluminando de cima para baixo.
    GLfloat ambiente[] = { 0.20f, 0.20f, 0.20f, 1.0f };  // Luz fraca que chega em todos os lados.
    GLfloat difusa[] = { 0.80f, 0.80f, 0.80f, 1.0f };    // Iluminação principal dos objetos.
    GLfloat especular[] = { 0.35f, 0.35f, 0.35f, 1.0f }; // Brilho refletido pela luz.
    GLfloat materialEspecular[] = { 0.20f, 0.20f, 0.20f, 1.0f }; // Reflexo discreto do material.
    GLfloat brilho[] = { 20.0f }; // Concentração do brilho.

    // Ativando a iluminação e configurando os parâmetros da luz e do material.
    glEnable(GL_LIGHTING); 
    glEnable(GL_LIGHT0); 
    glEnable(GL_COLOR_MATERIAL); 
    glEnable(GL_NORMALIZE); 
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Configurando os parâmetros da luz e do material.
    glLightfv(GL_LIGHT0, GL_POSITION, posicao); 
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa); 
    glLightfv(GL_LIGHT0, GL_SPECULAR, especular);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialEspecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, brilho);
}

// Configura a projeção da cena, definindo o campo de visão e a proporção da janela.
void ConfiguraProjecao(int largura, int altura)
{
    if (altura == 0)
        altura = 1; // Evita divisão por zero.

    glViewport(0, 0, largura, altura); // Define a região da janela onde a cena será exibida.
    glMatrixMode(GL_PROJECTION); // Seleciona a matriz de projeção.
    glLoadIdentity(); // Reinicia a matriz de projeção.
    gluPerspective(45.0, (float)largura / (float)altura, 0.1, 30.0); // Campo de visão, proporção, perto e longe.
}

// Calcula a posição da câmera em volta do centro da mesa.
void ConfiguraCamera(void)
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
    DesenhaMesa(); 
    DesenhaCacapas();
    DesenhaBolas(); 
    if (bolaBrancaSelecionada == 1)
        DesenhaSelecaoBranca();
    if (bolaEmMovimento == 0 && bolaBrancaSelecionada == 1)
    {
        DesenhaTaco();
    }
    if (carregandoTacada == 1)
        DesenhaBarraForca();

    glutSwapBuffers(); // Troca o buffer desenhado pelo que será exibido.
}

// Função chamada quando a janela é redimensionada.
void Redimensiona(int largura, int altura)
{
    larguraJanela = largura;
    alturaJanela = altura;
    ConfiguraProjecao(largura, altura); // Atualiza a projeção quando a janela muda de tamanho.
}

// Seleciona a bola branca quando o clique do mouse cai dentro de seu raio.
void SelecionaBolaBranca(int botao, int estado, int x, int y)
{
    GLint viewport[4];
    GLdouble matrizModelo[16];
    GLdouble matrizProjecao[16];
    GLdouble pertoX, pertoY, pertoZ;
    GLdouble longeX, longeY, longeZ;
    GLdouble fator;
    GLdouble pontoX, pontoZ;
    float dx, dz;

    if (botao != GLUT_LEFT_BUTTON || estado != GLUT_DOWN || bolaEmMovimento == 1 || carregandoTacada == 1)
        return;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, matrizModelo);
    glGetDoublev(GL_PROJECTION_MATRIX, matrizProjecao);

    // Converte o clique da janela em uma reta no espaço 3D da cena.
    gluUnProject(x, viewport[3] - y, 0.0, matrizModelo, matrizProjecao, viewport,
                 &pertoX, &pertoY, &pertoZ);
    gluUnProject(x, viewport[3] - y, 1.0, matrizModelo, matrizProjecao, viewport,
                 &longeX, &longeY, &longeZ);

    if (fabs(longeY - pertoY) < 0.0001)
        return;

    // Encontra onde essa reta cruza o plano horizontal do centro da bola branca.
    fator = (ALTURA_TAMPO + RAIO_BOLA - pertoY) / (longeY - pertoY);
    pontoX = pertoX + fator * (longeX - pertoX);
    pontoZ = pertoZ + fator * (longeZ - pertoZ);
    dx = (float)pontoX - posicaoX[BOLA_BRANCA];
    dz = (float)pontoZ - posicaoZ[BOLA_BRANCA];

    bolaBrancaSelecionada = (dx * dx + dz * dz <= RAIO_BOLA * RAIO_BOLA);
    glutPostRedisplay();
}

// Restaura bolas, pontuação, mira e seleção para uma nova partida.
void ReiniciaPartida(void)
{
    InicializaBolas();
    pontos = 0;
    anguloTaco = 0.0f;
    distanciaTaco = 0.20f;
    desvioImpacto = 0.0f;
    bolaEmMovimento = 0;
    bolaBrancaSelecionada = 0;
    carregandoTacada = 0;
    forcaTacada = 0.0f;
    AtualizaTitulo();
}

// Registra quando uma seta lateral começa a controlar a direção do taco.
void TeclasEspeciais(int tecla, int x, int y)
{
    (void)x;
    (void)y;

    if (bolaEmMovimento == 1 || bolaBrancaSelecionada == 0 || carregandoTacada == 1)
        return;

    switch (tecla)
    {
        case GLUT_KEY_LEFT:
            giraTacoEsquerda = 1;
            break;

        case GLUT_KEY_RIGHT:
            giraTacoDireita = 1;
            break;

    }

    glutPostRedisplay(); // Atualiza a posição do taco na janela.
}

// Para o movimento do taco quando uma seta lateral é solta.
void TeclaEspecialSolta(int tecla, int x, int y)
{
    (void)x;
    (void)y;

    if (tecla == GLUT_KEY_LEFT)
        giraTacoEsquerda = 0;
    if (tecla == GLUT_KEY_RIGHT)
        giraTacoDireita = 0;
}

// Atira a bola branca na direção do taco e com a força escolhida.
void AtiraBolaBranca(void)
{
    float forca;
    float velocidade;
    float anguloRad;

    if (bolaEmMovimento == 1 || bolaBrancaSelecionada == 0)
        return;

    forca = (distanciaTaco - DISTANCIA_MIN_TACO) /
            (DISTANCIA_MAX_TACO - DISTANCIA_MIN_TACO);
    velocidade = VELOCIDADE_MINIMA + forca * (VELOCIDADE_MAXIMA - VELOCIDADE_MINIMA);
    anguloRad = anguloTaco * PI / 180.0f;

    velocidadeX[BOLA_BRANCA] = velocidade * cosf(anguloRad);
    velocidadeZ[BOLA_BRANCA] = velocidade * sinf(anguloRad);
    velocidadeX[BOLA_BRANCA] += velocidade * (desvioImpacto / RAIO_BOLA) * 0.12f * -sinf(anguloRad);
    velocidadeZ[BOLA_BRANCA] += velocidade * (desvioImpacto / RAIO_BOLA) * 0.12f * cosf(anguloRad);
    bolaEmMovimento = 1;
}

// Trata a colisão de duas bolas com a mesma massa.
void ColideBolas(int primeira, int segunda)
{
    // Vetor que aponta do centro da primeira bola para a segunda.
    float dx = posicaoX[segunda] - posicaoX[primeira];
    float dz = posicaoZ[segunda] - posicaoZ[primeira];
    float distancia = sqrtf(dx * dx + dz * dz);
    float distanciaMinima = 2.0f * RAIO_BOLA; // Centros de bolas encostadas.

    // Só existe colisão quando os centros estão a menos de dois raios de distância.
    if (distancia > 0.0f && distancia < distanciaMinima)
    {
        // Vetor normal: direção exata em que uma bola empurra a outra.
        float normalX = dx / distancia;
        float normalZ = dz / distancia;
        float sobreposicao = distanciaMinima - distancia;
        float velocidadeRelativa;
        float impulso;

        // Separa as bolas para que elas não fiquem uma dentro da outra.
        posicaoX[primeira] -= normalX * sobreposicao / 2.0f;
        posicaoZ[primeira] -= normalZ * sobreposicao / 2.0f;
        posicaoX[segunda] += normalX * sobreposicao / 2.0f;
        posicaoZ[segunda] += normalZ * sobreposicao / 2.0f;

        // Mede se elas se aproximam ou se afastam na direção do impacto.
        velocidadeRelativa = (velocidadeX[segunda] - velocidadeX[primeira]) * normalX +
                              (velocidadeZ[segunda] - velocidadeZ[primeira]) * normalZ;

        // Só troca velocidade se as bolas estiverem se aproximando.
        if (velocidadeRelativa < 0.0f)
        {
            // Impulso transfere a velocidade entre bolas de mesma massa.
            impulso = -(1.0f + ELASTICIDADE_BOLAS) * velocidadeRelativa / 2.0f;
            velocidadeX[primeira] -= impulso * normalX;
            velocidadeZ[primeira] -= impulso * normalZ;
            velocidadeX[segunda] += impulso * normalX;
            velocidadeZ[segunda] += impulso * normalZ;
        }
    }
}

// Faz uma bola quicar nas quatro bordas da área jogável.
void ColideBorda(int indice)
{
    // O centro da bola deve ficar dentro da mesa, deixando um raio até a borda.
    float limiteX = COMPRIMENTO_MESA / 2.0f - RAIO_BOLA;
    float limiteZ = LARGURA_MESA / 2.0f - RAIO_BOLA;

    if (posicaoX[indice] > limiteX || posicaoX[indice] < -limiteX)
    {
        if (posicaoX[indice] > limiteX)
            posicaoX[indice] = limiteX;
        else
            posicaoX[indice] = -limiteX;

        velocidadeX[indice] = -velocidadeX[indice] * PERDA_NA_BORDA; // Inverte e perde energia.
    }

    if (posicaoZ[indice] > limiteZ || posicaoZ[indice] < -limiteZ)
    {
        if (posicaoZ[indice] > limiteZ)
            posicaoZ[indice] = limiteZ;
        else
            posicaoZ[indice] = -limiteZ;

        velocidadeZ[indice] = -velocidadeZ[indice] * PERDA_NA_BORDA; // Inverte e perde energia.
    }
}

// Atualiza a força, fazendo o taco ir do mínimo ao máximo e retornar continuamente.
void AtualizaForcaTacada(void)
{
    int tempoDecorrido = glutGet(GLUT_ELAPSED_TIME) - inicioCargaTacada;
    int tempoNoCiclo = tempoDecorrido % (2 * DURACAO_CARGA_TACADA);
    float progresso;

    if (tempoNoCiclo <= DURACAO_CARGA_TACADA)
        progresso = (float)tempoNoCiclo / DURACAO_CARGA_TACADA;
    else
        progresso = 1.0f - (float)(tempoNoCiclo - DURACAO_CARGA_TACADA) / DURACAO_CARGA_TACADA;

    // A potência acelera perto do máximo, deixando o trecho vermelho mais difícil.
    forcaTacada = powf(progresso, CURVA_CARGA_TACADA);
    distanciaTaco = DISTANCIA_MIN_TACO + forcaTacada * (DISTANCIA_MAX_TACO - DISTANCIA_MIN_TACO);
}

// Move a câmera e a mira enquanto suas teclas correspondentes permanecem pressionadas.
void AtualizaControlesContinuos(void)
{
    if (cameraEsquerda == 1)
        anguloHorizontal -= PASSO_CONTINUO_CAMERA;
    if (cameraDireita == 1)
        anguloHorizontal += PASSO_CONTINUO_CAMERA;

    if (cameraCima == 1 && anguloVertical < 75.0f)
        anguloVertical += PASSO_CONTINUO_CAMERA;
    if (cameraBaixo == 1 && anguloVertical > 5.0f)
        anguloVertical -= PASSO_CONTINUO_CAMERA;
    if (cameraAproxima == 1 && distanciaCamera > 4.5f)
        distanciaCamera -= PASSO_CONTINUO_CAMERA * 0.10f;
    if (cameraAfasta == 1 && distanciaCamera < 16.0f)
        distanciaCamera += PASSO_CONTINUO_CAMERA * 0.10f;

    if (bolaEmMovimento == 0 && bolaBrancaSelecionada == 1 && carregandoTacada == 0)
    {
        if (giraTacoEsquerda == 1)
            anguloTaco -= PASSO_CONTINUO_TACO;
        if (giraTacoDireita == 1)
            anguloTaco += PASSO_CONTINUO_TACO;

        if (anguloTaco < 0.0f)
            anguloTaco += 360.0f;
        if (anguloTaco >= 360.0f)
            anguloTaco -= 360.0f;
    }
}

// Atualiza posição, atrito, colisões e a barra da tacada a cada 16 milissegundos.
void AtualizaBolas(int valor)
{
    int i;
    int j;
    int algumaBolaEmMovimento = 0;

    (void)valor; // A GLUT exige esse parâmetro, mas não precisamos dele.

    if (carregandoTacada == 1)
        AtualizaForcaTacada();
    AtualizaControlesContinuos();

    // Só calculamos física enquanto ao menos uma bola estiver em movimento.
    if (bolaEmMovimento == 1)
    {
        for (i = 0; i < NUM_BOLAS; i++)
        {
            if (bolaAtiva[i] == 0)
                continue;

            // Nova posição = posição atual + velocidade x tempo.
            posicaoX[i] += velocidadeX[i] * 0.016f;
            posicaoZ[i] += velocidadeZ[i] * 0.016f;
            ColideBorda(i); // Corrige a posição se a bola bateu na borda.
        }

        // Compara todos os pares de bolas ativas uma única vez.
        for (i = 0; i < NUM_BOLAS; i++)
            for (j = i + 1; j < NUM_BOLAS; j++)
                if (bolaAtiva[i] == 1 && bolaAtiva[j] == 1)
                    ColideBolas(i, j);

        // Depois dos movimentos e choques, verifica se alguma bola entrou em caçapa.
        VerificaCacapas();

        for (i = 0; i < NUM_BOLAS; i++)
        {
            if (bolaAtiva[i] == 0)
                continue;

            velocidadeX[i] *= ATRITO; // Atrito reduz a velocidade horizontal.
            velocidadeZ[i] *= ATRITO; // Atrito reduz a velocidade no eixo Z da mesa.

            // Velocidades muito pequenas são consideradas paradas.
            if (fabsf(velocidadeX[i]) < VELOCIDADE_PARADA)
                velocidadeX[i] = 0.0f;
            if (fabsf(velocidadeZ[i]) < VELOCIDADE_PARADA)
                velocidadeZ[i] = 0.0f;

            if (velocidadeX[i] != 0.0f || velocidadeZ[i] != 0.0f)
                algumaBolaEmMovimento = 1;
        }

        // Quando todas param, o taco volta a aparecer.
        bolaEmMovimento = algumaBolaEmMovimento;
    }

    glutPostRedisplay(); // Pede que a janela desenhe as novas posições.
    glutTimerFunc(INTERVALO_ANIMACAO, AtualizaBolas, 0); // Agenda a próxima atualização.
}

// Controla a câmera virtual, a força da tacada e o ponto de impacto.
void Teclado(unsigned char tecla, int x, int y)
{
    (void)x; // Posição horizontal do mouse, não usada neste momento.
    (void)y; // Posição vertical do mouse, não usada neste momento.

    // Controles de câmera com as teclas WASD para girar e inclinar, QE para zoom e R para resetar.
    switch (tecla)
    {
        case ' ':
            if (bolaEmMovimento == 0 && bolaBrancaSelecionada == 1 && carregandoTacada == 0)
            {
                carregandoTacada = 1;
                inicioCargaTacada = glutGet(GLUT_ELAPSED_TIME);
                forcaTacada = 0.0f;
                distanciaTaco = DISTANCIA_MIN_TACO;
            }
            break;

        case 'j':
        case 'J':
            if (bolaEmMovimento == 0 && bolaBrancaSelecionada == 1 && carregandoTacada == 0 && desvioImpacto > -DESVIO_MAXIMO_IMPACTO)
            {
                desvioImpacto -= PASSO_IMPACTO; // Move o impacto para a esquerda do taco.
                if (desvioImpacto < -DESVIO_MAXIMO_IMPACTO)
                    desvioImpacto = -DESVIO_MAXIMO_IMPACTO;
            }
            break;

        case 'l':
        case 'L':
            if (bolaEmMovimento == 0 && bolaBrancaSelecionada == 1 && carregandoTacada == 0 && desvioImpacto < DESVIO_MAXIMO_IMPACTO)
            {
                desvioImpacto += PASSO_IMPACTO; // Move o impacto para a direita do taco.
                if (desvioImpacto > DESVIO_MAXIMO_IMPACTO)
                    desvioImpacto = DESVIO_MAXIMO_IMPACTO;
            }
            break;

        case 'k':
        case 'K':
            if (carregandoTacada == 0)
                desvioImpacto = 0.0f; // Retorna o impacto para o centro da bola.
            break;

        case 't':
        case 'T':
            ReiniciaPartida(); // Reinicia a mesa e a pontuação.
            break;

        case 'a':
        case 'A':
            cameraEsquerda = 1;
            break;

        case 'd':
        case 'D':
            cameraDireita = 1;
            break;

        case 'w':
        case 'W':
            cameraCima = 1;
            break;

        case 's':
        case 'S':
            cameraBaixo = 1;
            break;

        case 'e':
        case 'E':
            cameraAproxima = 1;
            break;

        case 'q':
        case 'Q':
            cameraAfasta = 1;
            break;

        case 'r':
        case 'R':
            anguloHorizontal = -90.0f; // Retorna para a visão inicial.
            anguloVertical = 25.0f;
            distanciaCamera = 8.0f;
            break;

    }

    glutPostRedisplay(); // Pede para a janela desenhar a cena novamente.
}

// Trata a soltura das teclas comuns e executa a tacada ao soltar espaço.
void TeclaSolta(unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    switch (tecla)
    {
        case 'a':
        case 'A':
            cameraEsquerda = 0;
            break;

        case 'd':
        case 'D':
            cameraDireita = 0;
            break;

        case 'w':
        case 'W':
            cameraCima = 0;
            break;

        case 's':
        case 'S':
            cameraBaixo = 0;
            break;

        case 'e':
        case 'E':
            cameraAproxima = 0;
            break;

        case 'q':
        case 'Q':
            cameraAfasta = 0;
            break;

        case ' ':
            if (carregandoTacada == 1)
            {
                carregandoTacada = 0;
                AtiraBolaBranca();
            }
            break;
    }

    glutPostRedisplay();
}

// Função principal do programa.
int main(int argc, char **argv)
{
    glutInit(&argc, argv); // Inicializa a GLUT.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Buffer duplo, cores RGB e Z-Buffer.
    glutInitWindowSize(900, 600); 
    glutCreateWindow("Sinuca 3D"); 
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF); // Evita que a repetição do teclado pare a carga ao segurar espaço.

    glClearColor(0.55f, 0.55f, 0.55f, 1.0f); // Cor de fundo meio cinza.
    glEnable(GL_DEPTH_TEST); // Ativa o teste de profundidade.

    glutDisplayFunc(Desenha); // Desenhar!
    glutReshapeFunc(Redimensiona); // Redimensionar a janela.
    glutKeyboardFunc(Teclado); // Controlar a câmera com o teclado.
    glutKeyboardUpFunc(TeclaSolta); // Executar a tacada quando o espaço é solto.
    glutSpecialFunc(TeclasEspeciais); // Controlar o taco com as setas.
    glutSpecialUpFunc(TeclaEspecialSolta); // Parar o taco quando a seta é solta.
    glutMouseFunc(SelecionaBolaBranca); // Selecionar a bola branca com o mouse.
    InicializaBolas(); // Define as posições iniciais das bolas.
    AtualizaTitulo(); // Mostra a pontuação inicial no título da janela.
    glutTimerFunc(INTERVALO_ANIMACAO, AtualizaBolas, 0); // Inicia a atualização das colisões.
    glutMainLoop(); // Mantém a janela aberta e processa eventos.

    return 0;
}
