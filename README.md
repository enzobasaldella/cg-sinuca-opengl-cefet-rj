# Mesa de Sinuca 3D — Computação Gráfica

Aplicação interativa em C que representa uma mesa de sinuca 3D com OpenGL.
Desenvolvida como trabalho final de Computação Gráfica, com áudio opcional.

A cena contém mesa, dezesseis bolas, seis caçapas, taco, cenário, iluminação e
câmera virtual. Com as bolas paradas, o jogador escolhe a direção e a força da
tacada. A pontuação dos dois jogadores aparece nos cards da tela.

## Funcionalidades

- Projeção em perspectiva e câmera virtual controlada pelo teclado;
- Mesa, taco, bolas e caçapas modelados com primitivas do OpenGL/GLUT;
- Cenário de boteco com piso quadriculado em tons envelhecidos, quatro paredes altas, luminária central, mesas plásticas, cadeiras, bebidas, tabuleiro de damas e duas caixas de som importadas;
- Iluminação, normais e teste de profundidade (Z-Buffer);
- Música de fundo em loop com OpenAL, quando as bibliotecas de áudio estão disponíveis;
- Direção e força por timing configuráveis para a tacada;
- Colisão entre bolas e colisão com as quatro bordas da mesa;
- Atrito para reduzir a velocidade até as bolas pararem;
- Detecção de bolas encaçapadas e pontuação exibida nos cards da tela;
- Reinicialização das bolas e da pontuação para começar uma nova partida.

## Tecnologias

- C;
- OpenGL no modo fixo;
- GLUT;
- GLU;
- GCC e a biblioteca matemática (`libm`);
- OpenAL e libsndfile para o áudio opcional.

## Compilação e execução

Em Debian, Ubuntu ou Linux Mint, instale as bibliotecas gráficas:

```bash
sudo apt install build-essential freeglut3-dev libglu1-mesa-dev
```

Para ouvir a música, instale também `libopenal-dev` e `libsndfile1-dev`. Sem
essas bibliotecas, o jogo compila e funciona sem áudio.

Compile e execute **dentro da pasta do projeto**, para que os arquivos de
`assets/` sejam encontrados:

```bash
make
./sinuca
```

## Controles

| Tecla | Ação |
| --- | --- |
| Setas esquerda e direita | Giram o taco ao redor da bola branca; o giro acelera enquanto a seta é segurada. |
| Segurar Espaço | Faz a força oscilar e mostra a barra na tela. |
| Soltar Espaço | Dá a tacada com a força indicada naquele instante. |
| A / D | Giram a câmera ao redor da mesa. |
| W / S | Sobem ou descem a câmera. |
| Q / E | Afastam a câmera até o limite das paredes ou a aproximam. |
| R | Restaura a posição inicial da câmera. |
| M | Liga ou desliga a música, quando o áudio está disponível. |
| T | Reinicia as bolas, a mira e a pontuação. |

## Como o jogo é construído

1. `sinuca.h` reúne as medidas da mesa, os parâmetros da tacada e o estado
   compartilhado. `sinuca.c` posiciona as 16 bolas e inicia a janela da GLUT.
2. `desenho.c` monta a mesa com caixas, desenha bolas com esferas e caçapas com
   discos. Depois configura a câmera com `gluLookAt`, a perspectiva e as luzes.
3. `cenario.c` desenha o piso, as paredes e a luminária. `modelo.c` lê os OBJ do
   mobiliário, guarda seus triângulos em listas de exibição e distribui os
   objetos no salão.
4. As teclas em `sinuca.c` giram a câmera e o taco. Ao segurar Espaço, a força
   percorre a barra; ao soltar, ela vira a velocidade inicial da bola branca.
5. O temporizador atualiza posição, colisões, caçapas e atrito. Quando as bolas
   param, o jogo decide de quem é a vez. `audio.c` toca a música, se disponível.

O arquivo [CREDITOS.md](assets/CREDITOS.md) registra a origem e a licença dos
modelos importados.

## Estrutura

```text
.
├── sinuca.c       # Estado, regras, física, teclado e main
├── sinuca.h       # Medidas e dados compartilhados
├── desenho.c      # Mesa, bolas, câmera, luz e interface
├── cenario.c      # Piso, paredes e luminária
├── modelo.c       # Leitura de OBJ e mobiliário do salão
├── audio.c        # Reprodução opcional da música
├── audio.h        # Funções públicas do áudio
├── assets/
│   ├── modelos/    # OBJ da mobília e das caixas de som
│   ├── audio/      # som_bar.wav
│   └── CREDITOS.md
├── Makefile       # Compilação
└── README.md      # Instruções e ordem de leitura
```

## Limitações acadêmicas

- As colisões são uma aproximação bidimensional sobre o tampo da mesa.
- Não há regras completas de uma partida oficial de sinuca.
- A rotação visual e o giro físico das bolas não são simulados; o foco é a
  aplicação dos conceitos gráficos estudados na disciplina.
