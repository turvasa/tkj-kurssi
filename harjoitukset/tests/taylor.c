#include <math.h>
#include <stdint.h>
#include <time.h>
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>



double perm_product(int n) {
    if (n == 0) return 1;
    double result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Function to calculate Taylor series for e^x
double taylor_exp(double x, int terms) {
    double sum = 1.0; 
    for (int n = 1; n <= terms; n++) {
        sum += pow(x, n) / perm_product(n); 
    }
    return sum;
}

int main() {

    uint8_t i = 1; 
    
    srand(time(NULL));

    // Choose a random value for x in the Taylor series
    double x = ((double)rand() / RAND_MAX) * 10.0; // Random x between 0 and 10
    printf("Calculating e^%f using Taylor series:\n\n", x);

    // Random value for j between 0 and SCHAR_MAX
    uint8_t j = rand() % (SCHAR_MAX + 1); 
    printf("Number of terms for the Taylor series: %d (random between 0 and SCHAR_MAX)\n", j);

    // Start timing after j is calculated
    clock_t start, end;
    start = clock();

    // Calculate the Taylor series with up to j terms
    do {
        // Calculate the Taylor series with i terms
        double taylor_result = taylor_exp(x, i);
        // Exact value of e^x
        double exact_result = exp(x);

        // Print the number of terms and the comparison
        printf("Using %d term(s): Taylor series = %f, Exact = %f, Difference = %f\n", 
               i, taylor_result, exact_result, fabs(exact_result - taylor_result));

        i++; 

    } while (i <= j);  

    // End timing
    end = clock();    

    printf("\nFinal number of terms: %d\n", j);
    printf("Loop took %f seconds to run\n", (double)(end - start) / CLOCKS_PER_SEC);
    
    return 0;
}