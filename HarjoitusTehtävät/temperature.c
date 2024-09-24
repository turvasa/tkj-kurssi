#include <stdio.h>
#include <inttypes.h>
#include <math.h>

// Prototyyppi tähän
float lampotila(uint16_t rekisteri);


int main() {
    float tulos;
    uint16_t argumentti = 0b0011001000000000;

    tulos = lampotila(argumentti);
    printf("{%f}",tulos);

    return 0;
}

// Funktion toteutus tänne..
// Prototyyppi tähän
float lampotila(uint16_t rekisteri) {
    uint16_t mask = fff101;

    return (rekisteri | mask >> 2) * 0.03125;
}