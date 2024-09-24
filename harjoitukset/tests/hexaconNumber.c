#include <stdio.h>
#include <inttypes.h>


uint8_t kuusikulmio(uint32_t luku);
void hexaconNumberPrint(int num, uint8_t isHexaconNumber);


int main() {
    int num = 15;
    int num_1 = 27;

    hexaconNumberPrint(num, kuusikulmio(num));
    hexaconNumberPrint(num_1, kuusikulmio(num_1));

}


uint8_t kuusikulmio(uint32_t luku) {
    for (int i = 1; i < luku; i++) {
        if ((i * (2 * i -1)) == luku) {
            return 1;
        }
    }

    return 0;
}


void hexaconNumberPrint(int num, uint8_t isHexaconNumber) {
    if (isHexaconNumber) {
        printf("Luku %i on kuusikulmioluku\n", num);
    } else {
        printf("Luku %i ei ole kuusikulmioluku\n", num);
    }
}
