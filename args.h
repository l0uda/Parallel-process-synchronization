// author Ludek Burda
// login  xburda13

#ifndef ERR
#include "error.h"
#endif

#ifndef FUN
#include "fun.h"
#endif

void parseArguments(int argc, char** argv, Arguments *args){
    Check(argc==6,'g')
    for( int i=1; i<argc; i++){
        Check(isInt(argv[i]),'g')
        int setValue = atoi(argv[i]);
        switch (i) {
            case 1:
                Check(setValue >= 1,'g')
                args->immigrantNumber = setValue;
                break;
            case 2:
                Check(setValue >= 0 && setValue <= 2000,'g')
                args->immigrantWait = setValue;
                break;
            case 3:
                Check(setValue >= 0 && setValue <= 2000,'g')
                args->judgeWait = setValue;
                break;
            case 4:
                Check(setValue >= 0 && setValue <= 2000,'g')
                args->getCertificate = setValue;
                break;
            case 5:
                Check(setValue >= 0 && setValue <= 2000,'g')
                args->giveCertificate = setValue;
                break;
            default:
                break;
        }
    }
}

Arguments *getArgs(int argc, char** argv){
    Arguments *args = malloc(sizeof(Arguments));
    Check(args,'a')
    parseArguments(argc,argv,args);
    return args;
}
