CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lglut -lGLU -lGL -lm
FONTES = sinuca.c desenho.c cenario.c modelo.c audio.c
MODELOS = assets/modelos/mesa_plastico.obj assets/modelos/cadeira_plastico.obj \
          assets/modelos/lata_colorida.obj assets/modelos/lata_vermelha.obj \
          assets/modelos/cerveja.obj assets/modelos/tabuleiro_damas.obj \
          assets/modelos/caixa_som/corpo.obj assets/modelos/caixa_som/base.obj \
          assets/modelos/caixa_som/grade.obj assets/modelos/caixa_som/detalhes.obj
AUDIO_HABILITADO = $(shell test -f /usr/include/AL/al.h -a -f /usr/include/sndfile.h && echo 1)

ifeq ($(AUDIO_HABILITADO),1)
CFLAGS += -DTEM_AUDIO
LIBS += -lopenal -lsndfile
endif

sinuca: $(FONTES) sinuca.h audio.h $(MODELOS) assets/audio/som_bar.wav
	$(CC) $(CFLAGS) $(FONTES) -o sinuca $(LIBS)

clean:
	rm -f sinuca

.PHONY: clean
