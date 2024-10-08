#include <stdio.h>
#include <stdint.h>
#include <math.h>


struct piste {
  int koordinaatit[3];
  struct piste *seuraava;
};

struct polku {
    double matka;
    struct piste *pisteet;
};


void laske_kuljettu_matka(struct polku *polku);



int main() {
    return 0;
}



void laske_kuljettu_matka(struct polku *polku) {
    struct piste *point = polku->pisteet;
    struct piste *next_point;
    double trail = 0.0; 

    while (point->seuraava != NULL) {
        next_point = point->seuraava;

        float length = sqrt(
            pow(point->koordinaatit[0] - next_point->koordinaatit[0], 2) +
            pow(point->koordinaatit[1] - next_point->koordinaatit[1], 2) +
            pow(point->koordinaatit[2] - next_point->koordinaatit[2], 2)
        );

        trail += length;
        point = next_point;
        next_point = polku->pisteet->seuraava;
    }

    polku->matka = trail;
}
