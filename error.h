// author Ludek Burda
// login  xburda13

#ifndef ERR
#define ERR
#include <ctype.h>
#define Check(Expr,Type) if(!(Expr)) Error(Type);


/// error function mainaining various error types
int Error(char type) {
    switch(type){
        case 'a': // alloc
            printf("Allocation error occured\n");
            break;
        case 'g': // arguments
            printf("Wrong arguments\n");
        default:
            break;
    }
    exit(1);
}

#endif
