#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>


int rand_int(int min, int max){
    return min + rand()%(max-min+1);
}

int get_len_string(char *str){
    if (str[0] == '\0'){
        return 0;
    }

    return 1 + get_len_string(str+1);
}


char* concat_strings(char *str1, char *str2){
    int i;
    int n1 = get_len_string(str1);
    int n2 = get_len_string(str1);

    char *result = (char*) malloc((n1+n2) * sizeof(char));

    for (i=0; i < n1; i++){
        result[i] = str1[i];
    }

    for (i=n1; i<(n1+n2); i++){
        result[i] = str2[i-n1];
    }

    return result;
}


char* convert_int_to_str(int i){
    char *str_value; 

    str_value = (char*) malloc(i*sizeof(char));

    // Use sprintf to convert the integer to a string
    sprintf(str_value, "%d", i);

    return str_value;
}


char* get_random_aircraft_image(){
    srand(time(NULL));

    char *random_number = convert_int_to_str(
        rand_int(1, 3)
    );

	char *image_spacecraft = concat_strings("images/spacecraft", random_number);
	return concat_strings(image_spacecraft, ".png");
}



// void main(){
//     // Rand numbers:
//     srand(time(NULL));
//     int r_int = rand_int(1, 90);

//     printf("\nRand int: %d\n", r_int);

//     // Concat strings:
//     char str[100] = "olaaa";
//     printf("Tamanho: %d\n", get_len_string(str));

//     char str1[10] = "bom ";

//     printf("\nStrings concatenadas: %s\n", concat_strings(str1, convert_int_to_str(r_int)));

//     // Random images:
//     printf("\nRandom spacecraft: %s\n", get_random_spacecraft_image());
// }