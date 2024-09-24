#include <stdio.h>


int main() {
    unsigned long n = 4000;
    unsigned short laskuri = 0;
    char luokka = 'C';
    float keskiarvo = 0.0;

    printf("%lu\n"
           "%hu\n"
           "%c\n"
           "%f",
           n, 
           laskuri, 
           luokka, 
           keskiarvo
    );

    return 0;
};

