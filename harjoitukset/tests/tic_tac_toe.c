#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>


int8_t tictactoe_check(int8_t* gameboard, int win_len);
bool row(int8_t* items, int win_len, int items_count, int side_len);
bool column(int8_t* items, int win_len, int items_count, int side_len);
bool diagonal(int8_t* items, int win_len, int items_count, int side_len);
int8_t winner(int win_len, int8_t* x_indexs, int x_count, int8_t* zero_indexs, int zero_count, int side_len);
void print_winner(int winner_int);



int main() {
    int8_t game_one[25] = {
        1, 1, 1, 1, 0,
        2, 1, 1, 2, 1,
        2, 1, 1, 0, 0, 
        0, 1, 2, 1, 2,
        0, 0, 0, 1, 1
    };

    int winner_one = tictactoe_check(game_one, 4);

    print_winner(winner_one);    

    return 0;
}



void print_winner(int winner_int) {
    if (winner_int == 0) {
        printf("Kumpikaan ei voittanut!\n");
    } else if (winner_int == 1) {
        printf("Risti voitti!\n");
    } else if (winner_int == 2) {
        printf("Nolla voitti!\n");
    } else {
        printf("Something went wrong\n");
    }
}



int8_t tictactoe_check(int8_t* gameboard, int win_len) {
    uint8_t gameboard_size = 100;
    uint8_t gameboard_side = (uint8_t) sqrt((double) gameboard_size) ;

    int8_t x_indexs[gameboard_size / 2];
    uint8_t x_count = 0;

    int8_t zero_indexs[gameboard_size / 2];
    uint8_t zero_count = 0;

    for (int i = 0; i < gameboard_size; i++) {
        char box = gameboard[i];

        if (box == 1) { // Is box = x
            x_indexs[x_count] = i;
            x_count ++;
        }  else if (box == 2) { // Is box = 0
            zero_indexs[zero_count] = i;
            zero_count ++;
        }
    }

    return winner(win_len, x_indexs, x_count, zero_indexs, zero_count, gameboard_side);
}



bool row(int8_t* items, int win_len, int items_count, int side_len) {
    for (int i = 0; i < (items_count - win_len); i++) {
        int8_t matchingItem_count = 0;
        for (int j = 1; j <= win_len; j++) {

            //Does i+j go to next row
            if ((i + j) % side_len == 0) break;

            //Is the next item 1 bigger and next of next 2 bigger than the item[i] and so on
            else if (items[i] == items[i + j] - j) {
                matchingItem_count ++;

            } else break;
        }

        if (matchingItem_count >= win_len) return 1;
    }

    return 0;
}



bool column(int8_t* items, int win_len, int items_count, int side_len) {
    //No need to check last n (n = win_len - 1) rows in first for -loop, because
    //there can't found any more winnable combos
    for (int i = 0; i < (items_count - (((win_len - 1)) * side_len)); i++) {
        int8_t matchingItem_count = 0;

        for (int j = (i + 1); j < items_count; j++) {
            uint8_t factor = 1;

            if ((items[i] == items[j] - (side_len * factor))) {
                factor ++;
                matchingItem_count ++;
            }
        }

        if (matchingItem_count >= 4) {
            return 1;
        }
    }

    return 0; 
}



bool diagonal(int8_t* items, int win_len, int items_count, int side_len) {
    //No need to check last n (n = win_len - 1) rows in first for -loop, because
    //there can't found any more winnable combos
    for (int i = 0; i < (items_count - (((win_len - 1)) * side_len)); i++) {
        int8_t matchingItem_count = 0;

        for (int j = (i + 1); j <= items_count; j++) {
            uint8_t factor = 1;

            //Downward diagonal
            if (items[i] == items[j] - ((side_len + 1) * factor)) {
                matchingItem_count ++;

            //Upward diagonal
            } else if (items[i] == items[j] - ((side_len - 1) * factor)) {
                matchingItem_count ++;
            }
        }

        if (matchingItem_count >= 4) {
            return 1;
        }
    }

    return 0; 
}



int8_t winner(int win_len, int8_t* x_indexs, int x_count, int8_t* zero_indexs, int zero_count, int side_len) {
    //x check
    bool x_wins = (
        row(x_indexs, win_len, x_count, side_len) || 
        column(x_indexs, win_len, x_count, side_len) ||
        diagonal(x_indexs, win_len, x_count, side_len)
    );

    //0 check
    bool zero_wins = (
        row(zero_indexs, win_len, zero_count, side_len) || 
        column(zero_indexs, win_len, zero_count, side_len) || 
        diagonal(zero_indexs, win_len, zero_count, side_len)
    );

    if (x_wins && zero_wins) {
        return 0;
    } else if (x_wins) {
        return 1;
    } else if (zero_wins) {
        return 2;
    }

    return 0;
}
