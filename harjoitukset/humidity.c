#include <stdio.h>
#include <inttypes.h>
#include <math.h>


float kosteus(uint16_t rekisteri);


int main() {
    float tulos;
    uint16_t argumentti = 0x8000;

    tulos = kosteus(argumentti);
    printf("Tulos os: %f\n",tulos);

    return 0;
}


float kosteus(uint16_t rekisteri) {
    return rekisteri / pow(2.0, 16.0) * 100.0;
}