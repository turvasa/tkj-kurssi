
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
// #include <tehtävässä halutut>

// Prototyyppi tähän
float kosteus(uint16_t rekisteri);

/*
int main() {
    float tulos;
    uint16_t argumentti = 0x8000;

    tulos = kosteus(argumentti);
    printf("{%f}",tulos);

    return 0;
}
*/
// Funktion toteutus tänne..
// Prototyyppi tähän
float kosteus(uint16_t rekisteri) {
    return rekisteri / pow(2.0, 16.0) * 100.0;
}