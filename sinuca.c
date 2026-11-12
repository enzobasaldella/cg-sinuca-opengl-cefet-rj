#include "sinuca.h"
#include "audio.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Posição inicial da câmera voltada para a primeira tacada.
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
int jogadorAtual = 1;
int pontosJogador1 = 0;
int pontosJogador2 = 0;
int encaçapouNaTacada = 0;
int brancaNaCacapa = 0;

// Estado inicial do taco e da animação.
float anguloTaco = 0.0f;
float distanciaTaco = DISTANCIA_DESCANSO_TACO;
int bolaEmMovimento = 0;
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
static float velocidadeGiroTaco = 0.0f;

// Define a posição e a cor de uma bola, deixando-a inicialmente parada.
static void DefineBola(int indice, float x, float z, float r, float g, float b)
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
static void InicializaBolas(void)
{
    float inicioX = 1.20f; // Posição da primeira bola do triângulo.
    float avancoX = 1.732f * RAIO_BOLA; // Distância entre as fileiras do triângulo (1.732 = raiz quadrada de 3).
    float avancoZ = 2.0f * RAIO_BOLA; // Distância entre bolas da mesma fileira (ou seja, estão encostadas).

    DefineBola(0, -1.5f, 0.0f, 0.95f, 0.95f, 0.95f);                              // Bola branca.

    // Triângulo de bolas com cores tradicionais.
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

// Verifica se o centro de uma bola entrou na abertura de uma caçapa.
static int BolaNaCacapa(int indice, float xCacapa, float zCacapa, float raioCacapa)
{
    float dx = posicaoX[indice] - xCacapa;
    float dz = posicaoZ[indice] - zCacapa;

    // Compara as distâncias ao quadrado para não precisar calcular raiz quadrada.
    return dx * dx + dz * dz < raioCacapa * raioCacapa;
}

// Remove uma bola colorida da mesa ou reposiciona a bola branca.
static void EncaçapaBola(int indice)
{
    velocidadeX[indice] = 0.0f;
    velocidadeZ[indice] = 0.0f;

    if (indice == BOLA_BRANCA)
    {
        // A branca não vale ponto e encerra a vez quando as bolas pararem.
        posicaoX[indice] = -1.5f;
        posicaoZ[indice] = 0.0f;
        brancaNaCacapa = 1;
    }
    else
    {
        // Bola inativa não é mais desenhada, atualizada nem testada em colisões.
        bolaAtiva[indice] = 0;
        encaçapouNaTacada = 1;
        if (jogadorAtual == 1)
            pontosJogador1++;
        else
            pontosJogador2++;
    }
}

// Testa cada bola ativa contra as seis caçapas da mesa.
static void VerificaCacapas(void)
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

// Restaura bolas, pontuação e mira para uma nova partida.
static void ReiniciaPartida(void)
{
    InicializaBolas();
    jogadorAtual = 1;
    pontosJogador1 = 0;
    pontosJogador2 = 0;
    encaçapouNaTacada = 0;
    brancaNaCacapa = 0;
    anguloTaco = 0.0f;
    distanciaTaco = DISTANCIA_DESCANSO_TACO;
    velocidadeGiroTaco = 0.0f;
    bolaEmMovimento = 0;
    carregandoTacada = 0;
    forcaTacada = 0.0f;
}

// Atira a bola branca na direção do taco e com a força escolhida.
static void AtiraBolaBranca(void)
{
    float forca;
    float velocidade;
    float anguloRad;

    if (bolaEmMovimento == 1)
        return;

    encaçapouNaTacada = 0;
    brancaNaCacapa = 0;
    forca = (distanciaTaco - DISTANCIA_MIN_TACO) /
            (DISTANCIA_MAX_TACO - DISTANCIA_MIN_TACO);
    velocidade = VELOCIDADE_MINIMA + forca * (VELOCIDADE_MAXIMA - VELOCIDADE_MINIMA);
    anguloRad = anguloTaco * PI / 180.0f;

    velocidadeX[BOLA_BRANCA] = velocidade * cosf(anguloRad);
    velocidadeZ[BOLA_BRANCA] = velocidade * sinf(anguloRad);
    bolaEmMovimento = 1;
}

// Troca a vez somente quando a tacada não encaçapou uma colorida ou a branca caiu.
static void FinalizaTacada(void)
{
    if (encaçapouNaTacada == 0 || brancaNaCacapa == 1)
    {
        if (jogadorAtual == 1)
            jogadorAtual = 2;
        else
            jogadorAtual = 1;
    }

    distanciaTaco = DISTANCIA_DESCANSO_TACO;
}

// Trata a colisão de duas bolas com a mesma massa.
static void ColideBolas(int primeira, int segunda)
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
static void ColideBorda(int indice)
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
static void AtualizaForcaTacada(void)
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

// Calcula até onde a câmera pode se afastar sem atravessar as paredes ou passar por cima delas.
static float LimiteDistanciaCamera(void)
{
    float horizontalRad = anguloHorizontal * PI / 180.0f;
    float verticalRad = anguloVertical * PI / 180.0f;
    float componenteX = fabsf(cosf(verticalRad) * sinf(horizontalRad));
    float componenteZ = fabsf(cosf(verticalRad) * cosf(horizontalRad));
    float limite = (ALTURA_PAREDE - 1.0f - MARGEM_CAMERA_PAREDE) / sinf(verticalRad);
    float limiteParede;

    // A câmera parte do centro: cada componente indica quanto ela avança em X ou Z.
    if (componenteX > 0.001f)
    {
        limiteParede = (POSICAO_PAREDE_X - MARGEM_CAMERA_PAREDE) / componenteX;
        if (limiteParede < limite)
            limite = limiteParede;
    }
    if (componenteZ > 0.001f)
    {
        limiteParede = (POSICAO_PAREDE_Z - MARGEM_CAMERA_PAREDE) / componenteZ;
        if (limiteParede < limite)
            limite = limiteParede;
    }

    return limite;
}

// Move a câmera e a mira enquanto suas teclas correspondentes permanecem pressionadas.
static void AtualizaControlesContinuos(void)
{
    if (cameraEsquerda == 1)
        anguloHorizontal -= PASSO_CONTINUO_CAMERA;
    if (cameraDireita == 1)
        anguloHorizontal += PASSO_CONTINUO_CAMERA;

    if (cameraCima == 1 && anguloVertical < 75.0f)
        anguloVertical += PASSO_CONTINUO_CAMERA;
    if (cameraBaixo == 1 && anguloVertical > 5.0f)
        anguloVertical -= PASSO_CONTINUO_CAMERA;
    if (anguloVertical > 75.0f)
        anguloVertical = 75.0f;
    if (anguloVertical < 5.0f)
        anguloVertical = 5.0f;
    if (cameraAproxima == 1 && distanciaCamera > 4.5f)
        distanciaCamera -= PASSO_CONTINUO_CAMERA * 0.10f;
    if (cameraAfasta == 1)
        distanciaCamera += PASSO_CONTINUO_CAMERA * 0.10f;
    if (distanciaCamera < 4.5f)
        distanciaCamera = 4.5f;
    // Também limita quando a câmera gira, pois a parede mais próxima pode mudar.
    if (distanciaCamera > LimiteDistanciaCamera())
        distanciaCamera = LimiteDistanciaCamera();

    if (bolaEmMovimento == 0 && carregandoTacada == 0 && giraTacoEsquerda != giraTacoDireita)
    {
        // Segurar a seta acelera o giro até uma velocidade máxima confortável.
        velocidadeGiroTaco += ACELERACAO_TACO;
        if (velocidadeGiroTaco < PASSO_CONTINUO_TACO)
            velocidadeGiroTaco = PASSO_CONTINUO_TACO;
        if (velocidadeGiroTaco > VELOCIDADE_MAX_TACO)
            velocidadeGiroTaco = VELOCIDADE_MAX_TACO;

        if (giraTacoEsquerda == 1)
            anguloTaco += velocidadeGiroTaco;
        else
            anguloTaco -= velocidadeGiroTaco;

        if (anguloTaco < 0.0f)
            anguloTaco += 360.0f;
        if (anguloTaco >= 360.0f)
            anguloTaco -= 360.0f;
    }
    else
        velocidadeGiroTaco = 0.0f;
}

// Atualiza posição, atrito, colisões e a barra da tacada a cada 16 milissegundos.
static void AtualizaBolas(int valor)
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

        // Quando todas param, decide se o jogador continua ou se a vez troca.
        bolaEmMovimento = algumaBolaEmMovimento;
        if (bolaEmMovimento == 0)
            FinalizaTacada();
    }

    glutPostRedisplay(); // Pede que a janela desenhe as novas posições.
    glutTimerFunc(INTERVALO_ANIMACAO, AtualizaBolas, 0); // Agenda a próxima atualização.
}

// Controla a câmera virtual e a força da tacada.
static void Teclado(unsigned char tecla, int x, int y)
{
    (void)x; // A GLUT informa a posição do mouse, mas este controle não a utiliza.
    (void)y;

    // As teclas controlam tacada, câmera, áudio e reinício da partida.
    switch (tecla)
    {
        case ' ':
            if (bolaEmMovimento == 0 && carregandoTacada == 0)
            {
                carregandoTacada = 1;
                inicioCargaTacada = glutGet(GLUT_ELAPSED_TIME);
                forcaTacada = 0.0f;
                distanciaTaco = DISTANCIA_MIN_TACO;
            }
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
            anguloVertical = 45.0f;
            distanciaCamera = 9.0f;
            break;

        case 'm':
        case 'M':
            AlternaMudo(); // Liga ou desliga a música de fundo.
            break;

    }

    glutPostRedisplay(); // Pede para a janela desenhar a cena novamente.
}

// Trata a soltura das teclas comuns e executa a tacada ao soltar espaço.
static void TeclaSolta(unsigned char tecla, int x, int y)
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

// Registra quando uma seta lateral começa a girar o taco.
static void TeclasEspeciais(int tecla, int x, int y)
{
    (void)x;
    (void)y;

    if (bolaEmMovimento == 1 || carregandoTacada == 1)
        return;

    if (tecla == GLUT_KEY_LEFT)
        giraTacoEsquerda = 1;
    else if (tecla == GLUT_KEY_RIGHT)
        giraTacoDireita = 1;

    glutPostRedisplay();
}

// Para o giro do taco quando a seta lateral é solta.
static void TeclaEspecialSolta(int tecla, int x, int y)
{
    (void)x;
    (void)y;

    if (tecla == GLUT_KEY_LEFT)
        giraTacoEsquerda = 0;
    else if (tecla == GLUT_KEY_RIGHT)
        giraTacoDireita = 0;
}

// Alterna o som quando o botão da parede recebe um clique.
static void Mouse(int botao, int estado, int x, int y)
{
    if (botao != GLUT_LEFT_BUTTON || estado != GLUT_DOWN)
        return;

    if (ClicouBotaoSom(x, y))
        AlternaMudo();
    glutPostRedisplay();
}

// Função principal do programa.
int main(int argc, char **argv)
{
    glutInit(&argc, argv); // Inicializa a GLUT.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Buffer duplo, cores RGB e Z-Buffer.
    glutInitWindowSize(larguraJanela, alturaJanela);
    glutCreateWindow("Sinuca 3D");
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF); // Evita que a repetição do teclado pare a carga ao segurar espaço.

    glClearColor(0.76f, 0.69f, 0.45f, 1.0f); // Fundo amarelo-bege.
    glEnable(GL_DEPTH_TEST); // Ativa o teste de profundidade.

    CarregaModelos(); // Lê os OBJ do mobiliário depois de criar a janela OpenGL.
    IniciaAudio("assets/audio/som_bar.wav"); // Inicia a música se as bibliotecas de áudio estiverem instaladas.
    atexit(ParaAudio); // Libera o dispositivo de áudio ao fechar o programa.

    glutDisplayFunc(Desenha); // Desenha a cena.
    glutReshapeFunc(Redimensiona); // Ajusta a projeção à janela.
    glutKeyboardFunc(Teclado); // Lê as teclas comuns.
    glutKeyboardUpFunc(TeclaSolta); // Executar a tacada quando o espaço é solto.
    glutSpecialFunc(TeclasEspeciais); // Controlar o taco com as setas.
    glutSpecialUpFunc(TeclaEspecialSolta); // Parar o taco quando a seta é solta.
    glutMouseFunc(Mouse); // Clicar no botão de som na parede.
    InicializaBolas(); // Define as posições iniciais das bolas.
    glutTimerFunc(INTERVALO_ANIMACAO, AtualizaBolas, 0); // Inicia a atualização das colisões.
    glutMainLoop(); // Processa os eventos enquanto a janela estiver aberta.

    return 0;
}
