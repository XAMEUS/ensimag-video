#include <pthread.h>
#include "synchro.h"
#include "ensitheora.h"

bool fini;

/* les variables pour la synchro, ici */
pthread_cond_t fen_ok = PTHREAD_COND_INITIALIZER;
pthread_cond_t text_fen_ok = PTHREAD_COND_INITIALIZER;
pthread_mutex_t fen = PTHREAD_MUTEX_INITIALIZER;
int fen_envoyee = 0;
int text_fen_prete = 0;

int taille_buffer = 0;
pthread_cond_t veut_deposer_texture = PTHREAD_COND_INITIALIZER;
pthread_cond_t veut_prendre_texture = PTHREAD_COND_INITIALIZER;
pthread_mutex_t verrou_texture = PTHREAD_MUTEX_INITIALIZER;


/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
    pthread_mutex_lock(&fen);
    windowsx = buffer[0].width;
    windowsy = buffer[0].height;
    fen_envoyee = 1;
    pthread_cond_broadcast(&fen_ok);
    pthread_mutex_unlock(&fen);
}

void attendreTailleFenetre() {
    pthread_mutex_lock(&fen);
    while(!fen_envoyee) pthread_cond_wait(&fen_ok, &fen);
    pthread_mutex_unlock(&fen);
}

void signalerFenetreEtTexturePrete() {
    pthread_mutex_lock(&fen);
    text_fen_prete = 1;
    pthread_cond_broadcast(&text_fen_ok);
    pthread_mutex_unlock(&fen);
}

void attendreFenetreTexture() {
    pthread_mutex_lock(&fen);
    while(!text_fen_prete) pthread_cond_wait(&text_fen_ok, &fen);
    pthread_mutex_unlock(&fen);
}




void debutConsommerTexture() {
    pthread_mutex_lock(&verrou_texture);
    while(taille_buffer < 1) pthread_cond_wait(&veut_prendre_texture, &verrou_texture);
}

void finConsommerTexture() {
    taille_buffer--;
    pthread_cond_broadcast(&veut_deposer_texture);
    pthread_mutex_unlock(&verrou_texture);
}


void debutDeposerTexture() {
    pthread_mutex_lock(&verrou_texture);
    while(taille_buffer >= NBTEX) pthread_cond_wait(&veut_deposer_texture, &verrou_texture);
}

void finDeposerTexture() {
    taille_buffer++;
    pthread_cond_broadcast(&veut_prendre_texture);
    pthread_mutex_unlock(&verrou_texture);
}
