#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>


void merkkilaskuri(char *str, uint8_t *tulos);
bool is_vocal(char character);
bool is_letter(char character);



int main() {
    char str[] = "ahfioäaewhfalöqwjfäoka";
    uint8_t result[2];

    merkkilaskuri(str, result);

    printf("%i, ", result[0]);
    printf("%i\n", result[1]);

    return 0;
}



void merkkilaskuri(char *str, uint8_t *tulos) {
    int vocal_count = 0;
    int consonant_count = 0;

    //Make whole str to lower case
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);

        if (is_letter(str[i])) {

            if (is_vocal(str[i])) {
                vocal_count ++;
            } else {
                consonant_count ++;
            }
        }
    }


    tulos[0] = vocal_count;
    tulos[1] = consonant_count;
}



bool is_vocal(char character) {
    if (
        character == 'a' ||
        character == 'e' ||
        character == 'i' ||
        character == 'o' ||
        character == 'u'
    ) {
        return 1;
    }

    return 0;
}



bool is_letter(char character) {
    if (
        character == ' ' ||
        character == '.' ||
        character == ',' ||
        character == '_' ||
        character == '-' ||
        character == '/' ||
        character == '\\' ||
        character == '(' ||
        character == ')' ||
        character == '[' ||
        character == ']' ||
        character == '{' ||
        character == '}' ||
        character == '&' ||
        character == '\"' ||
        character == '#' ||
        character == '!' ||
        character == '?' ||
        character == '&' ||
        character == '=' ||
        character == '`' ||
        character == '+' ||
        character == '@' ||
        character == '$' ||
        character == '*' ||
        character == '\''
    ) {
        return 0;
    }

    return 1;
}
