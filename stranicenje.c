#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>



int main(int argc, char *argv[]){

    srand((unsigned)time(NULL));

    int br_procesa = atoi(argv[1]);
    int br_okvira = atoi(argv[2]);
    
    int t = 0;

    char okvir[br_okvira][64];
    int punOkvir[br_okvira];

    for(int i = 0; i < br_okvira; i++){
        punOkvir[i] = 0;
        for(int j = 0; j < 64; j++){
            okvir[i][j] = 0;
        }
    }

    char disk[br_procesa][16][64];
    short tablica[br_procesa][16];

    for(int i = 0; i < br_procesa; i++){
        for(int j = 0; j < 16; j++){
            tablica[i][j] = 0;
            for(int k = 0; k < 64; k++){
                disk[i][j][k] = 0;
            }
        }
    }

    while(1){
        for(int i = 0; i < br_procesa; i++){
            short j = 0;
            printf("---------------\n");
            printf("proces: %d\n", i);
            printf("           t = %d\n", t);
            short logAdr = (short)rand() & 0x3FE;
            printf("log. adresa: 0x%x\n", logAdr);
            short stranica = logAdr & 0b0000001111000000;
            stranica = stranica >> 6;
            short zapis = tablica[i][stranica];
            short bitPrisutnosti = zapis & 0b0000000000100000;

            if(bitPrisutnosti == 0){
                printf("           Promasaj!\n");
                
                for(j = 0; j < br_okvira; j++){
                    if(punOkvir[j] == 0)
                        break;
                }
                if(j < br_okvira){
                    printf("           dodijeljen okvir: 0x%x\n", j);
                    if(t < 32){
                        zapis = j << 6;
                        zapis = zapis | 32;
                        zapis = zapis | t;
                    } else {
                        t = 0;
                        for(int i = 0; i < br_procesa; i++){
                            for(int j = 0; j < 16; j++){
                                tablica[i][j] = tablica[i][j] & 0b1111111111100000;
                            }
                        }
                        zapis = j << 6;
                        zapis = zapis | 0b0000000000100000;
                        zapis = zapis | 0b0000000000000001;
                    }
                    for(int k = 0; k < 64; k++){
                        okvir[j][k] = disk[i][stranica][k];
                    }
                    tablica[i][stranica] = zapis;
                } else {
                    int pr, str;
                    int min = 31;
                    short lruZap;
                    short noviOkvir;
                    for(int i = 0; i < br_procesa; i++){
                        for(int j = 0; j < 16; j++){
                            short bitPrisutnosti = tablica[i][j];
                            bitPrisutnosti = bitPrisutnosti & 0b0000000000100000;
                            if(bitPrisutnosti != 0){
                                short lru = tablica[i][j];
                                lru = lru & 0b0000000000011111;
                                if(lru < min){
                                    min = lru;
                                    pr = i;
                                    str = j;
                                    lruZap = tablica[pr][str];
                                }
                            }
                        }
                    }
                    
                    noviOkvir = lruZap & 0b1111111111000000;
                    noviOkvir = noviOkvir >> 6;
                    
                    for(int k = 0; k < 64; k++){
                        disk[pr][str][k] = okvir[noviOkvir][k];
                        okvir[noviOkvir][k] = disk[i][stranica][k];
                    }
                    
            
                    lruZap = lruZap & 0b1111111111100000;
                    tablica[pr][str] = 0;
                    tablica[i][stranica] = lruZap;

                    str = str << 6;
                    printf("           Izbacujem stranicu 0x%x iz procesa %d\n", str, pr);
                    printf("           lru izbacene stranice: 0x%x\n", min);
                    printf("           dodijeljen okvir: 0x%x\n", noviOkvir);

                    if(t < 32){
                        tablica[i][stranica] = tablica[i][stranica] | t;
                    } else {
                        t = 0;
                        tablica[i][stranica] = tablica[i][stranica] | 0b0000000000000001;
                        for(int i = 0; i < br_procesa; i++){
                            for(int j = 0; j < 16; j++){
                                tablica[i][j] = tablica[i][j] & 0b1111111111100000;
                            }
                        }
                    }
                }
            }
            printf("fiz. adresa: 0x%x\n", (j << 6) | (logAdr & 0b0000000000011111));
            printf("zapis tablice: 0x%x\n", tablica[i][stranica]);
            printf("sadrzaj adrese: %d\n", okvir[j][logAdr & 0b0000000000011111]);

            short trenOkv = okvir[j][logAdr & 0b0000000000011111];
            trenOkv++;
            okvir[j][logAdr & 0b0000000000011111] = trenOkv;
            short pomak = logAdr & 0b0000000000011111;
            okvir[j][pomak];
            punOkvir[j] = 1;

            t++;
            sleep(1);
            
        }
    }
	return 0;
}