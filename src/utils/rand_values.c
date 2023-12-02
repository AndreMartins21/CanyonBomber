#include <stdlib.h>
#include <stdio.h>
#include <time.h>


int rand_int(int min, int max){
    return min + rand()%(max-min+1);
}

float rand_float(float min, float max){
   return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}


// void main(){
//     srand(time(NULL));
//     printf("\nRand int: %d\n", rand_int(1, 90));
//     printf("\nRand float: %f\n", rand_float(1.0, 90.23));
// }