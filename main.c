#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "src/utils/rand_values.h"


/*
1. Desenhar o cenário.
2. Criar a estrutura da nave.
3. Desenhar uma nave na tela.
4. Implementar o movimento dessa nave.
*/

const float FPS = 60;  
const int SCREEN_W = 960;
const int SCREEN_H = 540;
const int SPACECRAFT_W = 70;
const int SPACECRAFT_H = 30;
const int SHOOT_RADIUS = 3;
const float SHOOT_ACEL = 0.05;


typedef struct Shoot {
	float x, y;
	int active;
	int hit;
	float speed_y;
} Shoot;


typedef struct Spacecraft {
	float x,y;
	ALLEGRO_COLOR color;
	int dir;
	float speed;
	Shoot shoot;
} Spacecraft;


int inicialize_allegro_variables(
	ALLEGRO_TIMER **timer, ALLEGRO_DISPLAY **display, ALLEGRO_EVENT_QUEUE **event_queue
){
	// Inicializa Allegro
	if (!al_init()) {
        fprintf(stderr, "Failed to initialize Allegro.\n");
        return -1;
    }

	// Inicializa o módulo de primitivas do Allegro
	if (!al_init_primitives_addon()) {
		fprintf(stderr, "Failed to initialize Allegro primitives addon.\n");
		return -1;
	}

	// Inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}

	// instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	// Cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    *timer = al_create_timer(1.0 / FPS);
    if(!*timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	*display = al_create_display(SCREEN_W, SCREEN_H);
    if (!*display) {
        fprintf(stderr, "Failed to create display.\n");
		al_destroy_timer(*timer);
        return -1;
    }

	// Cria a fila de eventos
	*event_queue = al_create_event_queue();
	if(!*event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(*display);
		return -1;
	}

	// // inicializa o modulo allegro que carrega as fontes
	// al_init_font_addon();

	// // Inicializa o modulo allegro que entende arquivos tff de fontes
	// if(!al_init_ttf_addon()) {
	// 	fprintf(stderr, "failed to load tff font module!\n");
	// 	return -1;
	// }

	// //carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    // ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	// if(size_32 == NULL) {
	// 	fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	// }

	return 1;
}

void register_events(ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer){
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
}

void draw_scenario(){
	ALLEGRO_COLOR color = al_map_rgb(127, 3, 252);
	al_clear_to_color(color);

	// Draw the canyon
    // al_draw_filled_rectangle(0, SCREEN_H / 2, SCREEN_W, SCREEN_H, al_map_rgb(153, 153, 153));  //
}

void draw_spacecraft(Spacecraft nave) {
	al_draw_filled_triangle(
		nave.x, nave.y,
		nave.x - (nave.dir*SPACECRAFT_W),
		nave.y - SPACECRAFT_H/2,
		nave.x - (nave.dir*SPACECRAFT_W),
		nave.y + SPACECRAFT_H/2,
		nave.color
	);							
							
	//desenha tiro:
	if(nave.shoot.active)
		al_draw_filled_circle(
			nave.shoot.x,
			nave.shoot.y,
			SHOOT_RADIUS, 
			nave.color
		);	
}

void create_spacecraft(Spacecraft *nave){
	nave->x = -1;
	nave->y = SPACECRAFT_H;
	nave->color = al_map_rgb(255, 0, 0);
	nave->dir = 1;
	nave->speed = 3;
	nave->shoot.x = 0;
	nave->shoot.y = 0;
	nave->shoot.active = 0;
	nave->shoot.hit = 0;
	nave->shoot.speed_y = 0;

	printf("\nSpacecraft created sucessuful!\n");
}

void run_shoot(Spacecraft *nave){
	nave->shoot.active = 1;
	nave->shoot.hit = 0;
	nave->shoot.x = nave->x;
	nave->shoot.y = nave->y;
	nave->shoot.speed_y = 0;
}


void update_spacecraft(Spacecraft *nave) {
	nave->x += (nave->dir) * nave->speed;

	/* Irá trocar a direção da nave caso:
	- O x da nave for maior do que o X do cenário + largura da nave
		OU 
	- A posição de X for menor que a posição 0 - largura da nave
	*/
	if(nave->x > SCREEN_W + SPACECRAFT_W ||
	   nave->x < 0-SPACECRAFT_W
	) {
		nave->dir *= -1;
	
	}
	
	// Atualiza tiro
	if(nave->shoot.active) {
		nave->shoot.x = nave->x;
		nave->shoot.speed_y += SHOOT_ACEL;
		nave->shoot.y += nave->shoot.speed_y;
	}
	
	/* Irá desativar o tiro caso:
	- A pos x do tiro + raio do tiro ultrapassar os limites laterais da tela
		OU
	- A pos y do tiro + raio ultrapassar o limite inferior
	*/
	if(nave->shoot.x < 0-SHOOT_RADIUS ||
	   nave->shoot.x > SCREEN_W + SHOOT_RADIUS ||
	   nave->shoot.y > SCREEN_H + SHOOT_RADIUS
	) {
		   nave->shoot.active = 0;
	}
}


int main() {
	ALLEGRO_TIMER *timer = NULL; 
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;

	if (
		inicialize_allegro_variables(&timer, &display, &event_queue) == -1
	) return -1;

	register_events(event_queue, display, timer);
	

	// 2. Criar a estrutura da nave
	Spacecraft p1, p2;
	create_spacecraft(&p1);
	create_spacecraft(&p2);


	// Inicializa o temporizador
	al_start_timer(timer);

	int playing = 1;

	while(playing) {
		ALLEGRO_EVENT ev;

		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {

			draw_scenario();
			
			update_spacecraft(&p1);
			draw_spacecraft(p1);

			// Atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			
			switch(ev.keyboard.keycode) {				
				case ALLEGRO_KEY_SPACE:
					run_shoot(&p1);
				break;
				
			}
			
			//imprime qual tecla foi
			printf("\nTecla digitada: %d", ev.keyboard.keycode);
		}

	} 
     
	// Fim de jogo (Fechar janela, limpar display, fila de eventos)
	
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

    return 0;
}