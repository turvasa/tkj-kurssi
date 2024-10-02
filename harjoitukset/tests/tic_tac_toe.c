#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


int8_t tictactoe_check(int8_t* gameboard, int win_len);
bool row(int8_t* items, int win_len, int items_count, int side_len);
bool line(int8_t* items, int win_len, int items_count, int side_len);
bool diagonal(int8_t* items, int win_len, int items_count, int side_len);
int8_t winner(int win_len, int8_t* x_indexs, int x_count, int8_t* zero_indexs, int zero_count, int side_len);
void print_winner(int winner_int);



int main() {
    int8_t game_one[25] = {
        1, 1, 1, 1, 0,
        2, 1, 1, 2, 1,
        2, 1, 0, 0, 0, 
        0, 1, 2, 2, 2,
        0, 0, 0, 1, 2
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
    uint8_t gameboard_size = sizeof(gameboard);
    uint8_t gameboard_side = gameboard_size / 2;

    int8_t x_indexs[gameboard_side];
    uint8_t x_count = 0;

    int8_t zero_indexs[gameboard_side];
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
        for (int j = 1; j <= win_len; j++) {
            if ((items[i] != items[i + j] - j) || (j % side_len == 0)) {
                break;
            } else if (j == win_len) {
                return 1;
            }
        }
    }

    return 0;
}



bool line(int8_t* items, int win_len, int items_count, int side_len) {
    for (int i = 0; i < (items_count - win_len); i++) {
        int8_t matchingItem_count = 0;

        for (int j = i+1; j <= (items_count - win_len); j++) {

            for (int k = 1; k <= win_len; k++) {
                if ((items[i] == items[j] - (side_len * k)) && (matchingItem_count = (k-1))) {
                    matchingItem_count ++;
                }
            }
        }

        if (matchingItem_count >= 4) {
            return 1;
        }
    }

    return 0; 
}



bool diagonal(int8_t* items, int win_len, int items_count, int side_len) {
    for (int i = 0; i < (items_count - win_len); i++) {
        int8_t matchingItem_count = 0;

        for (int j = i+1; j <= (items_count - win_len); j++) {

            for (int k = 1; k <= win_len; k++) {
                if (items[i] == items[j] - ((side_len + 1) * k)) {
                    matchingItem_count ++;
                } else if (items[i] == items[j] - ((side_len - 1) * k)) {
                    matchingItem_count ++;
                }
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
    bool x_wins =
        row(x_indexs, win_len, x_count, side_len) || 
        line(x_indexs, win_len, x_count, side_len) ||
        diagonal(x_indexs, win_len, x_count, side_len)
    ;

    //0 check
    bool zero_wins = 
        row(zero_indexs, win_len, zero_count, side_len) || 
        line(zero_indexs, win_len, zero_count, side_len) || 
        diagonal(zero_indexs, win_len, zero_count, side_len)
    ;

    if (x_wins && zero_wins) {
        return 0;
    } else if (x_wins) {
        return 1;
    } else if (zero_wins) {
        return 2;
    }

    return 0;
}
