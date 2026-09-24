// proj2.c
// Reseni IOS-DU2, 11.4.2020
// Autor: Ludek Burda, FIT

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "fun.h"
#include "args.h"

pid_t wpid;
int status = 0;

int main (int argc, char *argv[]) {
    FILE* filos = fopen("proj2.out","w");
    srand(time(NULL));                          // nastaveni seedu pro funkci rand zalozenou na aktualnim case systemu
    setbuf(filos,NULL);                                // zakazani nacitani dat k vytisku do souboru do bufferu
    setbuf(stderr,NULL);                               // zakazani nacitani dat k vytisku na stderr do bufferu
    Arguments *args = getArgs(argc, argv);                  // inicializace struktury s informacemi o zadanych argumentech
    Shared *shared = getShared();                           // inicializace struktury se sdilenymi promennymi
    Semaphores *sems = getSemaphores();                     // inicializace struktury se semafory
    pid_t mainSplit, immGenerator;
    mainSplit = fork();
    if (mainSplit == 0) { // proces pro generovani imigrantu
        for (int i = 0; i < args->immigrantNumber; i++) {
            usleep(1000*(rand()%(1 + args->immigrantWait)));
            immGenerator = fork();
            if (immGenerator == 0) { // jednotlivi imigranti

                /// pomyslny zacatek procesu
                sem_wait(sems->outputMutex);
                int I = ++shared->immigrantID;
                fprintf(filos,"%d\t: IMM %d\t\t: starts\n",++shared->actOrd, I);
                sem_post(sems->outputMutex);


                /// vstup do budovy
                sem_wait(sems->Judge);

                sem_wait(sems->outputMutex);
                fprintf(filos,"%d\t: IMM %d\t\t: enters\t\t: %d\t: %d\t: %d\n",++shared->actOrd, I, ++shared->immNot, shared->immReg, ++shared->immAll);
                sem_post(sems->outputMutex);

                sem_post(sems->Judge);


                /// registrace procesu imigranta
                sem_wait(sems->Mutex);

                sem_wait(sems->outputMutex);
                fprintf(filos,"%d\t: IMM %d\t\t: checks\t\t: %d\t: %d\t: %d\n",++shared->actOrd, I, shared->immNot, ++shared->immReg, shared->immAll);
                sem_post(sems->outputMutex);

                if(shared->judge == 1 && shared->immNot == shared->immReg)
                    sem_post(sems->allSignedIn);
                else
                    sem_post(sems->Mutex);


                /// zadani o certifikat procesem imigranta
                sem_wait(sems->Confirmed);

                sem_wait(sems->outputMutex);
                fprintf(filos,"%d\t: IMM %d\t\t: wants certificate\t: %d\t: %d\t: %d\n",++shared->actOrd, I, shared->immNot, shared->immReg, shared->immAll);
                sem_post(sems->outputMutex);

                usleep(1000*(rand()%(1 + args->getCertificate)));

                /// vyzvedavani certifikatu procesem imigranta
                sem_wait(sems->outputMutex);
                fprintf(filos,"%d\t: IMM %d\t\t: got certificate\t: %d\t: %d\t: %d\n",++shared->actOrd, I, shared->immNot, shared->immReg, shared->immAll);
                sem_post(sems->outputMutex);


                /// opusteni budovy
                sem_wait(sems->Judge);

                sem_wait(sems->outputMutex);
                fprintf(filos,"%d\t: IMM %d\t\t: leaves\t\t: %d\t: %d\t: %d\n",++shared->actOrd, I, shared->immNot, shared->immReg, --shared->immAll);
                sem_post(sems->outputMutex);

                sem_post(sems->Judge);

                exit(0);
            }
            else if (immGenerator < 0){
                freeSemaphores(sems);
                freeShared(shared);
                free(args);
                fclose(filos);
            }
        }
        while ((wpid = wait(&status)) > 0); // cekani na ukonceni ostatnich procesu
    }
    else if (mainSplit > 0){ // soudce
        while(1) {
            /// oznameni soudce o chystani se vejit do budovy
            sem_wait(sems->outputMutex);
            fprintf(filos,"%d\t: JUDGE\t\t: wants to enter\n", ++shared->actOrd);
            sem_post(sems->outputMutex);


            /// oznameni soudce o vchazeni do budovy
            sem_wait(sems->Judge);

            sem_wait(sems->Mutex);
            sem_wait(sems->outputMutex);
            fprintf(filos,"%d\t: JUDGE\t\t: enters\t\t: %d\t: %d\t: %d\n", ++shared->actOrd, shared->immNot, shared->immReg, shared->immAll);
            shared->judge=1;
            sem_post(sems->outputMutex);


            /// pripadne cekani na registraci vsech imigrantu v budove
            sem_wait(sems->outputMutex);
            if (shared->immReg != shared->immNot) {
                fprintf(filos,"%d\t: JUDGE\t\t: waits for imm\t\t: %d\t: %d\t: %d\n", ++shared->actOrd, shared->immNot, shared->immReg, shared->immAll);
                sem_post(sems->outputMutex);

                sem_post(sems->Mutex);
                sem_wait(sems->allSignedIn);
            } else
                sem_post(sems->outputMutex);


            /// oznameni o zacatku procesu vydavani certifikatu
            sem_wait(sems->outputMutex);
            fprintf(filos,"%d\t: JUDGE\t\t: starts confirmation\t: %d\t: %d\t: %d\n", ++shared->actOrd, shared->immNot, shared->immReg, shared->immAll);
            sem_post(sems->outputMutex);

            usleep(1000 * (rand() % (1 + args->giveCertificate)));

            /// cyklus pro vydej certifikatu
            sem_wait(sems->outputMutex);
            for (int i = 0; i < shared->immReg; i++) {
                sem_post(sems->Confirmed);
                shared->doneImm++;
            }
            shared->immNot = shared->immReg = 0;
            fprintf(filos,"%d\t: JUDGE\t\t: ends confirmation\t: %d\t: %d\t: %d\n", ++shared->actOrd, shared->immNot, shared->immReg, shared->immAll);
            sem_post(sems->outputMutex);

            usleep(1000 * (rand() % (1 + args->giveCertificate)));

            /// odchod soudce z budovy
            sem_wait(sems->outputMutex);
            shared->judge=0;
            fprintf(filos,"%d\t: JUDGE\t\t: leaves\t\t: %d\t: %d\t: %d\n", ++shared->actOrd, shared->immNot, shared->immReg, shared->immAll);
            sem_post(sems->outputMutex);

            sem_post(sems->Mutex);
            sem_post(sems->Judge);

            if (shared->doneImm == args->immigrantNumber)
                break;

            usleep(1000 * (rand() % (1 + args->judgeWait)));
        }
        /// ukonceni procesu soudce
        sem_wait(sems->outputMutex);
        fprintf(filos,"%d\t: JUDGE\t\t: finishes\n", ++shared->actOrd);
        sem_post(sems->outputMutex);

        while ((wpid = wait(&status)) > 0); // cekani na ukonceni ostatnich procesu
    }
    else {
        freeSemaphores(sems);
        freeShared(shared);
        free(args);
        fclose(filos);
    }

    freeSemaphores(sems);
    freeShared(shared);
    free(args);
    fclose(filos);
    return 0;
}
