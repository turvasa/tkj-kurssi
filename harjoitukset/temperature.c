#include <stdio.h>
#include <inttypes.h>


float lampotila(uint16_t rekisteri);


int main() {
    float tulos;
    uint16_t argumentti = 0b0011001000000000;

    tulos = lampotila(argumentti);
    printf("Tulos on: %f \n",tulos);

    return 0;
}


float lampotila(uint16_t rekisteri) {
    int mask = 0xfffc; //fffc = 1111 1111 1111 1100
    int temperature_bit = rekisteri & mask;

    return (temperature_bit >> 2) * 0.03125;
}