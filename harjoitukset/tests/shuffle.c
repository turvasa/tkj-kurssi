#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>


void print_list(uint8_t *list, uint16_t list_size);
void shuffle(uint8_t *list, uint16_t list_size);
void reorganize_list(uint8_t *list, uint16_t first_index, uint16_t list_size);



int main() {
    uint8_t list[30] = {
        67, 6, 73, 244, 188, 
        99, 63, 245, 46, 242, 
        56, 147, 98, 208, 74, 
        163, 163, 169, 24, 43, 
        234, 117, 124, 127, 55, 
        192, 188, 73, 200, 12
    };

    uint8_t list_size = sizeof(list);


    srand(time(NULL));

    print_list(list, list_size);
    shuffle(list, list_size);
    print_list(list, list_size);
}


void print_list(uint8_t *list, uint16_t list_size) {
    printf("[");

    for (int i = 0; i < list_size; i++) {
        if (list[i] != 0) {
            printf("%u", list[i]);

            if (i != list_size-1)
                printf(", ");
        }
    }
    printf("]\n");
}


void shuffle(uint8_t *list, uint16_t list_size) {
    uint16_t shuffled_list[list_size];

    for (int i = list_size - 1; i >= 0; i--) {
        if (i == 0) {
            shuffled_list[i] = list[i];
        } else {
            int random_num = rand()%(i);
            shuffled_list[i] = list[random_num];
            reorganize_list(list, random_num, list_size);
        }
    }

    for (int j = 0; j < list_size; j++) {
        list[j] = shuffled_list[j];
    }
}


void reorganize_list(uint8_t *list, uint16_t first_index, uint16_t list_size) {
    for (int i = first_index; i < list_size; i++) {
        if (i == list_size)
            list[i] = 0;
        else
            list[i] = list[i+1];
    }
}