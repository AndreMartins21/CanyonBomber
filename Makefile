FLAGS = -std=c99 -O3
PATH_ALLEGRO=/usr/lib/x86_64-linux-gnu
LIB_ALLEGRO=-lallegro -lallegro_acodec -lallegro_audio -lallegro_color -lallegro_dialog -lallegro_font -lallegro_image -lallegro_primitives -lallegro_ttf
FILE_UTILS = src/utils/rand_values

all: main

main.o: main.c
	echo "Compilando arquivo main"
	gcc $(FLAGS) -c main.c

main: main.o $(FILE_UTILS).o
	gcc $(FLAGS) -o main main.o $(FILE_UTILS).o -L $(PATH_ALLEGRO) $(LIB_ALLEGRO) -lm 


clean:
	rm main
	rm main.o
	rm $(FILE_UTILS).o