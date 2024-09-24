#include <stdio.h>
#include <inttypes.h>
#include <math.h>

float valoisuus(uint16_t rekisteri);


int main() {
    float tulos;
    uint16_t argumentti = 0b0110110000110101;

    tulos = valoisuus(argumentti);
    printf("Tulos on: %f \n",tulos);

    return 0;
}


float valoisuus(uint16_t rekisteri) {
    int mask_1 = 0xf000; //f000 = 1111 0000 0000 0000
    int mask_2 = 0x0fff; //0ffc = 0000 1111 1111 1111

    float light_1 = (rekisteri & mask_1) >> 12;
    float light_2 = rekisteri & mask_2;

    return (0.01 * pow(2.0, light_1) * light_2);
}