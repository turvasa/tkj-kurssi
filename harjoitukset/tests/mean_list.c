#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


float keskiarvo(char *lista);



int main() {
    char int_str[] = "201,53,12,31,5";

    float str_mean = keskiarvo(int_str);

    printf("Mean: %f.1", str_mean);

    return 0;
}



float keskiarvo(char *lista) {
    char *numbers_str = strtok(lista, ",");
    float sum = 0;
    float num_count = 0;

    while (numbers_str != NULL) {
        sum += atoi(numbers_str);
        num_count ++;

        numbers_str = strtok(NULL, ",");
    }

    if (num_count != 0) {
        return (sum / num_count);
    }

    return 0;
}
