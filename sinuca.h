#ifndef SINUCA_H
#define SINUCA_H

#include <GL/glut.h>

// Dimensões da mesa de sinuca, das bolas e do taco.
#define COMPRIMENTO_MESA      6.0f    // 2,54 m.
#define LARGURA_MESA          3.0f    // 1,27 m.
#define ALTURA_TAMPO          1.80f   // 0,76 m.
#define ESPESSURA_TAMPO       0.12f   // 0,048 m.
#define LARGURA_BORDA         0.3000f // 0,12 m.
#define ALTURA_BORDA          0.2500f // 0,10 m.
#define LARGURA_PE            0.40f   // 0,17 m.
#define RAIO_BOLA             0.0675f // 0,028575 m.
#define RAIO_CACAPA_CANTO     0.135f  // 0,05715 m.
#define RAIO_CACAPA_LATERAL   0.150f  // 0,0635 m.
#define COMPRIMENTO_TACO      3.47f   // 1,47 m.
#define ESPESSURA_TACO        0.035f  // 0,015 m.

// Dimensões do piso e da luminária do cenário.
#define LADO_QUADRADO         1.00f   // 0,42 m.
#define NUM_QUADRADOS_X       18
#define NUM_QUADRADOS_Z       14
#define ALTURA_LAMPADA        5.20f
#define ALTURA_ABAJUR         0.45f
#define RAIO_ABAJUR           0.55f
#define ALTURA_TRILHO         6.80f
#define POSICAO_PAREDE_X      8.96f
#define POSICAO_PAREDE_Z      6.96f
#define ALTURA_PAREDE         12.00f
#define ALTURA_BOTAO_SOM      2.85f
#define RAIO_BOTAO_SOM        0.25f
#define Z_BOTAO_SOM           (POSICAO_PAREDE_Z - 0.18f)

// Parâmetros da mira e da força da tacada.
#define DISTANCIA_DESCANSO_TACO 0.08f // Distância normal do taco parado.
#define DISTANCIA_MIN_TACO   0.02f // Distância mínima da ponta até a bola branca.
#define DISTANCIA_MAX_TACO   0.90f
#define FOLGA_TACO_BORDA     0.03f // Folga vertical entre o taco e o topo da borda.
#define FOLGA_TACO_BOLA      0.005f // Folga entre o taco e as outras bolas.
#define DURACAO_CARGA_TACADA 1000
#define CURVA_CARGA_TACADA 3.5f
#define PASSO_CONTINUO_TACO 1.0f
#define ACELERACAO_TACO      0.08f
#define VELOCIDADE_MAX_TACO  3.5f

// Parâmetros da animação e colisão.
#define NUM_BOLAS            16
#define BOLA_BRANCA          0
#define VELOCIDADE_MINIMA    0.30f
#define VELOCIDADE_MAXIMA    13.00f
#define ATRITO               0.988f
#define ELASTICIDADE_BOLAS   0.92f
#define PERDA_NA_BORDA       0.70f
#define VELOCIDADE_PARADA    0.03f
#define INTERVALO_ANIMACAO   16

// Parâmetros da câmera virtual e conversão de ângulos.
#define PASSO_CONTINUO_CAMERA 1.2f
#define MARGEM_CAMERA_PAREDE 0.40f
#define PI 3.14159265f

// Estado da câmera.
extern float anguloHorizontal;
extern float anguloVertical;
extern float distanciaCamera;

// Estado das bolas e da pontuação.
extern float posicaoX[NUM_BOLAS];
extern float posicaoZ[NUM_BOLAS];
extern float velocidadeX[NUM_BOLAS];
extern float velocidadeZ[NUM_BOLAS];
extern float corR[NUM_BOLAS];
extern float corG[NUM_BOLAS];
extern float corB[NUM_BOLAS];
extern int bolaAtiva[NUM_BOLAS];
extern int jogadorAtual;
extern int pontosJogador1;
extern int pontosJogador2;
extern int encaçapouNaTacada;
extern int brancaNaCacapa;

// Estado do taco, da tacada e dos controles.
extern float anguloTaco;
extern float distanciaTaco;
extern int bolaEmMovimento;
extern int carregandoTacada;
extern int inicioCargaTacada;
extern float forcaTacada;
extern int larguraJanela;
extern int alturaJanela;
extern int giraTacoEsquerda;
extern int giraTacoDireita;
extern int cameraEsquerda;
extern int cameraDireita;
extern int cameraCima;
extern int cameraBaixo;
extern int cameraAproxima;
extern int cameraAfasta;

// Funções compartilhadas entre os arquivos do projeto.
void Desenha(void);
void Redimensiona(int largura, int altura);
void Caixa(float x, float y, float z, float comprimento, float altura, float largura,
           float r, float g, float b);
void DesenhaCenario(void);
void ConfiguraLuzesCenario(void);
void CarregaModelos(void);
void DesenhaMobiliarioImportado(void);
int ClicouBotaoSom(int x, int y);

#endif
