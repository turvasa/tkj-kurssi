#include <stdio.h>
#include <inttypes.h>

int ilmanpaine(uint16_t xlsb, uint16_t lsb, uint16_t msb);

/*
int main() {
    int tulos;
    uint16_t xlsb = -34;
    uint16_t lsb = -68;
    uint16_t msb = 24;

    tulos = ilmanpaine(xlsb, lsb, msb);
    printf("Tulos on: %i \n",tulos);

    return 0;
}
*/


int ilmanpaine(uint16_t xlsb, uint16_t lsb, uint16_t msb) {
    int pressure_msb = msb << 12;
    int pressure_lsb = lsb << 4;
    int pressure_xlsb = xlsb >> 4;

    return pressure_msb | pressure_lsb | pressure_xlsb;
}