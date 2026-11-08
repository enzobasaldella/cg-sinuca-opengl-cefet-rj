#include "audio.h"

#ifdef TEM_AUDIO

#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

static ALCdevice *dispositivo = NULL;
static ALCcontext *contexto = NULL;
static ALuint buffer = 0;
static ALuint fonte = 0;
static int mudo = 0;

// Abre um arquivo WAV e inicia sua reprodução contínua em 50% do volume.
void IniciaAudio(const char *arquivo)
{
    SF_INFO info = { 0 };
    SNDFILE *arquivoAudio;
    short *amostras;
    sf_count_t quadrosLidos;
    ALenum formato;

    dispositivo = alcOpenDevice(NULL);
    if (dispositivo == NULL)
    {
        printf("Aviso: não foi possível iniciar o dispositivo de áudio.\n");
        return;
    }

    contexto = alcCreateContext(dispositivo, NULL);
    if (contexto == NULL || !alcMakeContextCurrent(contexto))
    {
        ParaAudio();
        return;
    }

    arquivoAudio = sf_open(arquivo, SFM_READ, &info);
    if (arquivoAudio == NULL || (info.channels != 1 && info.channels != 2))
    {
        if (arquivoAudio != NULL)
            sf_close(arquivoAudio);
        ParaAudio();
        return;
    }

    amostras = malloc((size_t)info.frames * info.channels * sizeof(short));
    if (amostras == NULL)
    {
        sf_close(arquivoAudio);
        ParaAudio();
        return;
    }

    quadrosLidos = sf_readf_short(arquivoAudio, amostras, info.frames);
    sf_close(arquivoAudio);
    if (quadrosLidos <= 0)
    {
        free(amostras);
        ParaAudio();
        return;
    }

    formato = (info.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, formato, amostras,
                 (ALsizei)(quadrosLidos * info.channels * sizeof(short)), info.samplerate);
    free(amostras);

    alGenSources(1, &fonte);
    alSourcei(fonte, AL_BUFFER, buffer);
    alSourcei(fonte, AL_LOOPING, AL_TRUE);
    alSourcef(fonte, AL_GAIN, 0.50f);
    alSourcePlay(fonte);
}

// Alterna entre o volume normal e o silêncio sem interromper a música.
void AlternaMudo(void)
{
    if (fonte == 0)
        return;

    mudo = !mudo;
    if (mudo == 1)
        alSourcef(fonte, AL_GAIN, 0.0f);
    else
        alSourcef(fonte, AL_GAIN, 0.50f);
}

// Libera a fonte, o buffer e o contexto criados pelo OpenAL.
void ParaAudio(void)
{
    if (fonte != 0)
    {
        alSourceStop(fonte);
        alDeleteSources(1, &fonte);
        fonte = 0;
    }

    if (buffer != 0)
    {
        alDeleteBuffers(1, &buffer);
        buffer = 0;
    }

    if (contexto != NULL)
    {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(contexto);
        contexto = NULL;
    }

    if (dispositivo != NULL)
    {
        alcCloseDevice(dispositivo);
        dispositivo = NULL;
    }
}

#else

// Mantém a compilação funcionando em computadores sem OpenAL e libsndfile.
void IniciaAudio(const char *arquivo)
{
    (void)arquivo;
}

void ParaAudio(void)
{
}

void AlternaMudo(void)
{
}

#endif
