#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <time.h>
#define N 3
#define M 20

pthread_mutex_t m;
pthread_cond_t red[2];
int u_restoranu[2];
int ceka_red[2];
int posluzeno[2];
pthread_t programeri[M];
int br_dr = 0;

void obradi_sigint(int sig);

void udji_u_restoran(int tvrtka){
    pthread_mutex_lock(&m);
    while(u_restoranu[1 - tvrtka] > 0 || (ceka_red[1-tvrtka] > 0 && posluzeno[tvrtka]>= N)){
        ceka_red[tvrtka]++;
        pthread_cond_wait(&red[tvrtka], &m);
        ceka_red[tvrtka]--;
    }
    u_restoranu[tvrtka]++;

    if(tvrtka == 0)
        printf("Usao MS programer\n");
    else 
        printf("Usao Linux programer\n");
    
    posluzeno[1 - tvrtka] = 0;
   
    posluzeno[tvrtka]++;

    pthread_mutex_unlock(&m);
}

void izadji_iz_restorana(int tvrtka){
    pthread_mutex_lock(&m);
    u_restoranu[tvrtka]--;
    if(tvrtka == 0)
        printf("Izasao MS programer\n");
    else 
        printf("Izasao Linux programer\n");
    if(u_restoranu[tvrtka] == 0){
        pthread_cond_broadcast(&red[1 - tvrtka]);
    }
    pthread_mutex_unlock(&m);
}

void *programer(void *x){
    int tvrtka = *((int*)x);
    udji_u_restoran(tvrtka);
    sleep(1);
    izadji_iz_restorana(tvrtka);
    br_dr--;
}

int main(void){
    pthread_attr_t attr;
    pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


    pthread_mutex_init(&m, NULL);

    pthread_cond_init(&red[0], NULL);
    pthread_cond_init(&red[1], NULL);


    srand((unsigned)time(NULL));
    br_dr = M;

    for(int i = 0; i < M; i++){
        int tvrtka = rand() % 2;
        pthread_create(&programeri[i], &attr, programer, &tvrtka);
    }

    while(br_dr > 0){}
    
    pthread_mutex_destroy (&m);
	pthread_cond_destroy (&red[0]);
	pthread_cond_destroy (&red[1]);

    return 0;

}
