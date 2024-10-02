#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void movavg(float *array, uint8_t array_size, uint8_t window_size);


int main() {
    float data[5] = { 1.0, 2.0, 4.0, 6.0, 9.0 };
    uint8_t window_size = 3;

    movavg(data, sizeof(data), window_size);

    return 0;
}


void movavg(float *array, uint8_t array_size, uint8_t window_size) {
    int loop_iterations = abs(array_size - window_size);

    for (int i = 0; i <= loop_iterations; i++) {
        float window_float = 0;

        for (int j = i; j < (i + window_size); j++) {
            window_float += array[j];
        }

        float mean = window_float / window_size;
        printf("%.2f", mean);

        if (i != loop_iterations) {
            printf(",");
        } else {
            printf("\n");
        }
    }
}
