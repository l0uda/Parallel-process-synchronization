// author Ludek Burda
// login  xburda13

#ifndef FUN
#define FUN
#include <sys/mman.h>

#ifndef ERR
#include "error.h"
#endif

//////////////////////////////
/// STRUCTURES DEFINITIONS ///
//////////////////////////////

/// structure for variables shared between individual processes
typedef struct {
    int actOrd;      // A
    int immigrantID; // I
    int immNot;      // NE
    int immReg;      // NC
    int immAll;      // NB

    int judge;       // variable indicating whether judge is inside of building or not
    int doneImm;     // counter for number of certificates given by judge
} Shared;

/// structure for semaphores
typedef struct {
    sem_t *Mutex;       // mutex for synchronizing judge in matter of waiting for unchecked immigrants
    sem_t *Judge;       // semaphore preventing immigrants from entering or escaping building while judge is inside
    sem_t *allSignedIn; // semaphore used by judge to wait for all immigrants to be checked before starting to give certificates
    sem_t *outputMutex; // mutex for isolating access to shared variables
    sem_t *Confirmed;   // semaphore that allows immigrants to start receiving certificates after judge signalizes it
} Semaphores;

/// structure to preserve informations given by user through arguments
typedef struct {
    int immigrantNumber; // PI
    int immigrantWait;   // IG
    int judgeWait;       // JG
    int getCertificate;  // IT
    int giveCertificate; // JT
} Arguments;


//////////////////////////////
/// FUNCTIONS DECLARATIONS ///
//////////////////////////////

/// functions from args.h file
void parseArguments(int argc, char** argv, Arguments *args); // function used to parse arguments
Arguments* getArgs(int argc, char** argv);                   // function used to initialize structure preserving arguments

/// functions from actual header file
Shared* getShared();                                         // function used to initialize structure with shared variables
void freeShared(Shared *shared);                             // function to cleanup structure with shared variables
Semaphores* getSemaphores();                                 // function used to initialize structure with semaphores
sem_t* initSem(int val);                                     // function used to intialize a single semaphore to given value
void freeSemaphores(Semaphores *sems);                       // function to cleanup semaphores
int isInt(char *string);                                     // function to determine whetver given string is composed of integers only


/////////////////////////////
/// FUNCTIONS DEFINITIONS ///
/////////////////////////////


int isInt(char *string) {
    for(size_t i=1; i<strlen(string);i++){
        if(!isdigit(string[i]))
            return 0;
    }
    return 1;
}

sem_t* initSem(int val){ // special function to allocate memory shared between processes (-1 argument)
    sem_t* sem = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(sem,1,val);
    return sem;
}

void freeSemaphores(Semaphores *sems){
    sem_destroy(sems->Mutex);         // unlinking unnamed semaphores
    sem_destroy(sems->outputMutex);   //
    sem_destroy(sems->Judge);         //
    sem_destroy(sems->Confirmed);     //
    sem_destroy(sems->allSignedIn);   //
    munmap(sems, sizeof(Semaphores)); // deallocating structure shared between processes
}

void freeShared(Shared *shared){
    munmap(shared, sizeof(Shared));   // deallocating structure shared between processes
}

Shared *getShared(){
    Shared *share = mmap(NULL,sizeof(Shared),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    Check(share,'a')
    share->immigrantID = share->actOrd = share->immNot = share->immReg = share->immAll = share->doneImm = share->judge = 0;
    return share;
}

Semaphores* getSemaphores(){
    Semaphores *semaphores = mmap(NULL,sizeof(Semaphores),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    Check(semaphores,'a')
    semaphores->Mutex = initSem(1);
    semaphores->outputMutex = initSem(1);
    semaphores->Judge = initSem(1);
    semaphores->Confirmed = initSem(0);
    semaphores->allSignedIn = initSem(0);
    return semaphores;
}

#endif
