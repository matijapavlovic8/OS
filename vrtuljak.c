#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#define N 5

void obradi_sigint(int sig);


sem_t *sem;

sem_t *slobodno_mjesto;
sem_t *vrtuljak_pun;
sem_t *smiju_izaci;
sem_t *vrtuljak_prazan;

void posjetitelj(int id){
    while(1){
        
        sem_wait(slobodno_mjesto);
        printf("Posjetitelj %d sjeo\n", id);
        sem_post(vrtuljak_pun);
        sem_wait(smiju_izaci);
        printf("Posjetitelj %d izasao\n", id);
        sem_post(vrtuljak_prazan);
    }
}

void vrtuljak(){
    while(1){
        printf("Vrtuljak prazan\n");
        for(int i = 0; i < N; i++){
            sem_post(slobodno_mjesto);
        }
        for(int i = 0; i < N; i++){
            sem_wait(vrtuljak_pun);
        }
        printf("Vrtuljak pun\n");
        printf("Vrtuljak se vrti...\n");
        sleep(2);
        printf("Vrtuljak se zaustavio\n");

        for(int i = 0; i < N; i++){
            sem_post(smiju_izaci);
        }
        for(int i = 0; i < N; i++){
            sem_wait(vrtuljak_prazan);
        }
    }
}
    

int main(void){

    struct sigaction act;

    act.sa_handler = obradi_sigint;
    sigaction(SIGINT, &act, NULL);

    int id;

    id = shmget (IPC_PRIVATE, sizeof(sem_t), 0600);
    sem = (sem_t*)shmat(id, NULL, 0);
    shmctl (id, IPC_RMID, NULL);

    slobodno_mjesto = sem;
    vrtuljak_pun = sem + 1;
    smiju_izaci = sem + 2;
    vrtuljak_prazan = sem + 3;
    
    sem_init(slobodno_mjesto, 1, 0);
    sem_init(vrtuljak_pun, 1, 0);
    sem_init(smiju_izaci, 1, 0);
    sem_init(vrtuljak_prazan, 1, 0);

    if(fork() == 0){
        vrtuljak();
    }
    
    for(int i = 0; i < N; i++){
        if(fork() == 0){
            posjetitelj(i + 1);
        }
    }

    for (int i = 0; i < N; i++ )
		wait(NULL);

	sem_destroy(slobodno_mjesto);
    sem_destroy(vrtuljak_pun);
    sem_destroy(smiju_izaci);
    sem_destroy(vrtuljak_prazan);
	shmdt(sem);

    return 0;
    
}

void obradi_sigint(int sig){
    for (int i = 0; i < N; i++ )
		wait(NULL);

	sem_destroy(slobodno_mjesto);
    sem_destroy(vrtuljak_pun);
    sem_destroy(smiju_izaci);
    sem_destroy(vrtuljak_prazan);
	shmdt(sem);
    exit(1);
}