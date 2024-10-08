#include <stdio.h>
#include <stdint.h>


#define DATA_MASK 0x7F8
#define RW_BIT 0x002
#define RESET_BIT 0x001
#define CLEAR(bit, mask) (bit &= ~mask)
#define SET(bit, mask) (bit |= mask)



void write_data_to_LCD(uint16_t* lcd_register, uint8_t data);
void reset_LCD(uint16_t* lcd_register);

// Kirjoita dataa LCD:lle
void write_data_to_LCD(uint16_t* lcd_register, uint8_t data) {
    // Tyhjennä aiemmat tiedot
    *lcd_register = *lcd_register & ~DATA_MASK; // 0b0000011111111000
    // Aseta uudet tiedot
    *lcd_register = *lcd_register | ((uint16_t)data << 3); 
    // Nollaa R/W-bitti merkiksi, että kyseessä on kirjoitustoiminto
    CLEAR(*lcd_register,RW_BIT);
}

// Nollaa LCD. Tämä asettaa Reset-bittiin arvon 1
void reset_LCD(uint16_t* lcd_register) {
    SET(*lcd_register,RESET_BIT);
}

// Kirjoita dataa LCD:lle ja nollaa se
int main() {
    uint16_t lcd_register = 0x00C8;
    write_data_to_LCD(&lcd_register,0x19);
    printf("Kirjoitettu data LCD:lle. LCD-rekisterin sisältö: %#06X \n",lcd_register);
    reset_LCD(&lcd_register);
    printf("Nollataan rekisteri. LCD-rekisterin sisältö: %#06X \n",lcd_register);

    return 0;
}