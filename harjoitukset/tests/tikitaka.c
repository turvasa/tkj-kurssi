#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>


int8_t tictactoe_check(int8_t* gameboard, int win_len);
bool row(int8_t* items, int win_len, int items_count, int side_len, uint8_t item_type);
bool column(int8_t* items, int win_len, int items_count, int side_len, uint8_t item_type);
bool diagonal(int8_t* items, int win_len, int items_count, int side_len, uint8_t item_type);
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
    uint8_t x = 1;
    uint8_t zero = 2;

    //x check
    bool x_wins = (
        row(gameboard, win_len, gameboard_size, gameboard_side, x) || 
        column(gameboard, win_len, gameboard_size, gameboard_side, x) ||
        diagonal(gameboard, win_len, gameboard_size, gameboard_side, x)
    );

    //0 check
    bool zero_wins = (
        row(gameboard, win_len, gameboard_size, gameboard_side, zero) || 
        column(gameboard, win_len, gameboard_size, gameboard_side, zero) || 
        diagonal(gameboard, win_len, gameboard_size, gameboard_side, zero)
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


bool row(int8_t* items, int win_len, int items_count, int side_len, uint8_t item_type) {
    for (int i = 0; i < (items_count - win_len); i++) {
        int8_t matchingItem_count = 1;

        for (int j = 1; j <= win_len; j++) {

            //Does i+j go to next row
            if ((i + j) % side_len == 0) break;

            else if (items[i] == items[i + j]) {
                matchingItem_count ++;

            } else break;
        }

        if (matchingItem_count >= win_len) return 1;
    }

    return 0;
}



bool column(int8_t* items, int win_len, int items_count, int side_len, uint8_t item_type) {
    //No need to check last n (n = win_len - 1) rows in first for -loop, because
    //there can't found any more winnable combos
    for (int i = 0; i < (items_count - (((win_len - 1)) * side_len)); i++) {
        int8_t matchingItem_count = 1;

        if (items[i] == item_type) {
            for (int factor = 1; factor <= win_len; factor++) {

                if (items[i] == items[i + (side_len * factor)]) {
                    matchingItem_count ++;
                } else break;
            }

            if (matchingItem_count >= 4) {
                return 1;
            }
        }
    }

    return 0; 
}



bool diagonal(int8_t* items, int win_len, int items_count, int side_len, uint8_t item_type) {
    //No need to check last n (n = win_len - 1) rows in first for -loop, because
    //there can't found any more winnable combos
    for (int i = 0; i < (items_count - (((win_len - 1)) * side_len)); i++) {
        int8_t matchingItem_count = 1;

        if (items[i] == item_type) {

            //Downward diagonal
            if ((i % side_len) < win_len) {
                for (int factor = 1; factor <= win_len; factor++) {

                    if (items[i] == items[i + (side_len + 1) * factor]) {
                        matchingItem_count ++;

                    } else break;
                }
            }

            if (matchingItem_count >= 4) {
                return 1;
            } else matchingItem_count = 0;

            //Upward diagonal
            if ((i % side_len) > (side_len - win_len)) {
                for (int factor = 1; factor <= win_len; factor++) {

                    if (items[i] == items[i + (side_len - 1) * factor]) {
                        matchingItem_count ++;

                    } else break;
                }
            }

            if (matchingItem_count >= 4) {
                return 1;
            }
        }
    }

    return 0; 
}
