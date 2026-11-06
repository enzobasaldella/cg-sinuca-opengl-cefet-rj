# Mesa de Sinuca 3D — Computação Gráfica (CEFET/RJ)

Aplicação interativa em C que representa uma mesa de sinuca 3D com OpenGL. O
projeto foi desenvolvido para o trabalho final da disciplina de Computação Gráfica
do CEFET/RJ, utilizando apenas conceitos apresentados ao longo das aulas.

> [!NOTE]
> Este é um projeto acadêmico. A física foi simplificada de propósito para priorizar
> os conceitos de computação gráfica trabalhados na disciplina.

A cena contém a mesa, dezesseis bolas, seis caçapas, taco, iluminação e câmera
virtual. O jogador seleciona a bola branca com o mouse, define a direção e a força
da tacada e acompanha a animação das colisões.

## Funcionalidades

- Projeção em perspectiva e câmera virtual controlada pelo teclado;
- Mesa, taco, bolas e caçapas modelados com primitivas do OpenGL/GLUT;
- Iluminação, normais e teste de profundidade (Z-Buffer);
- Seleção da bola branca com o mouse, indicada por um círculo amarelo;
- Direção, força por timing e ponto lateral de impacto configuráveis para a tacada;
- Colisão entre bolas e colisão com as quatro bordas da mesa;
- Atrito para reduzir a velocidade até as bolas pararem;
- Detecção de bolas encaçapadas e pontuação exibida no título da janela;
- Reinicialização das bolas e da pontuação para começar uma nova partida.

## Tecnologias

- C;
- OpenGL no modo fixo;
- GLUT;
- GLU;
- GCC e a biblioteca matemática (`libm`).

## Compilação e execução

Em Debian, Ubuntu ou Linux Mint, instale as bibliotecas necessárias:

```bash
sudo apt install build-essential freeglut3-dev libglu1-mesa-dev
```

Compile e execute a partir da raiz do repositório:

```bash
gcc -Wall -Wextra sinuca.c -o sinuca -lglut -lGLU -lGL -lm
./sinuca
```

## Controles

Primeiro, clique na bola branca. O círculo amarelo confirma que ela foi selecionada.

| Controle | Ação |
| --- | --- |
| Clique esquerdo | Seleciona a bola branca. |
| Seta esquerda / direita | Gira o taco ao redor da bola branca. |
| Segurar Espaço | Faz o taco oscilar e mostra a barra de força. |
| Soltar Espaço | Executa a tacada com a força indicada naquele instante. |
| J / L | Move o ponto de impacto para a esquerda ou direita da bola. |
| K | Retorna o ponto de impacto ao centro da bola. |
| W / A / S / D | Move a câmera virtual ao redor da mesa. |
| Q / E | Afasta ou aproxima a câmera. |
| R | Retorna a câmera à posição inicial. |
| T | Reinicia as bolas e a pontuação. |

## Implementação

A mesa é montada a partir de cubos escalados. As bolas são esferas fornecidas pela
GLUT e as caçapas são discos da GLU. A câmera é posicionada com `gluLookAt` e a
cena utiliza projeção em perspectiva.

Ao segurar Espaço, o taco percorre continuamente a distância mínima e máxima. A
barra de força acompanha esse movimento: ela acelera perto da região vermelha, para
que uma tacada forte dependa de timing. Ao soltar a tecla, a distância atual é
convertida na velocidade da bola branca.

A cada intervalo de animação, o programa atualiza as posições a partir das
velocidades das bolas, verifica colisões, aplica atrito e solicita um novo desenho
da janela. Nas colisões entre bolas, a velocidade é transferida na direção do
impacto. Nas bordas, a componente correspondente é invertida e perde energia.

## Estrutura

```text
.
├── sinuca.c    # Implementação da cena e das interações
├── README.md   # Instruções do projeto
└── .gitignore  # Arquivos locais que não devem ser versionados
```

## Limitações acadêmicas

- As colisões são uma aproximação bidimensional sobre o tampo da mesa.
- Não há regras completas de uma partida oficial de sinuca.
- A rotação visual e o giro físico das bolas não são simulados; o foco é a
  aplicação dos conceitos gráficos estudados na disciplina.
