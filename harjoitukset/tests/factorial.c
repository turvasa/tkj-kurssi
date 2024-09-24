#include <stdio.h>
#include <stdint.h>

int64_t laske_kertoma(int8_t n);

int main() {
    int num = 7;
    long long int factorial = laske_kertoma(num);

    printf("Luvun %i kertoma on: %lld", num, factorial);

    return 0;
}


int64_t laske_kertoma(int8_t n) {
    if (n <= 20) {
        long long int factorrial = 1;

        for (int i = n; i > 0; i--) {
            factorrial *= i;
        }

        return factorrial;
    } else return -1;
}
