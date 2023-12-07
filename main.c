#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <string.h>
#include <stdlib.h>
#include "src/utils.h"


const float FPS = 60;  
const int SCREEN_W = 960;
const int SCREEN_H = 540;
const int AIRCRAFT_W = 60;
const int AIRCRAFT_H = 30;
const int SHOT_RADIUS = 3;
const float SHOT_ACEL = 0.051;

const int N_ROW_GRID = 2;
const int N_COL_GRID = 1;


typedef struct Shot {
	float x, y;
	int active;
	int hit;
	float speed_y;
} Shot;


typedef struct Aircraft {
	int nave_id, dir;
	int hp, score;
	float x, y;
	float speed;
	ALLEGRO_COLOR color;
	Shot shot;
	ALLEGRO_BITMAP *image;
} Aircraft;


typedef struct Target {
	float x1, y1, x2, y2;
	int value, active;
	ALLEGRO_COLOR color;
} Target;


int inicialize_allegro_variables(
	ALLEGRO_TIMER **timer, ALLEGRO_DISPLAY **display, ALLEGRO_EVENT_QUEUE **event_queue, ALLEGRO_FONT **font_font_24
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

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}

	// Carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    *font_font_24 = al_load_font("images/arial.ttf", 24, 1);   
	if(!font_font_24) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
		return -1;
	}

	*display = al_create_display(SCREEN_W, SCREEN_H);
    if (!*display) {
        fprintf(stderr, "Failed to create display.\n");
		al_destroy_timer(*timer);
        return -1;
    }

	al_set_window_title(*display, "Canyon Bomber - André");


	// Cria a fila de eventos
	*event_queue = al_create_event_queue();
	if(!*event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(*display);
		return -1;
	}

	al_init_image_addon();

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
}

void draw_aircraft(Aircraft nave) {
	al_draw_tinted_scaled_rotated_bitmap(
		nave.image,
		nave.color,
		0, 0,              // Source bitmap region (entire bitmap)
		nave.x, nave.y,    // Target bitmap position
		nave.dir*(float)AIRCRAFT_W / al_get_bitmap_width(nave.image),   // Scaling factor for width
        (float)AIRCRAFT_H / al_get_bitmap_height(nave.image),  // Scaling factor for height
		0,          // Rotation angle in radians
		0                  // Flags (usually 0)
	);

	//desenha tiro:
	if(nave.shot.active)
		al_draw_filled_circle(
			nave.shot.x,
			nave.shot.y,
			SHOT_RADIUS, 
			nave.color
		);	
}

void create_aircraft(Aircraft *nave, int x, int y, int dir, int *hex_color, int id){
	nave->nave_id = id;
	nave->x = x;
	nave->y = y;
	nave->hp = 3;
	nave->score = 0;
	nave->dir = 1;
	nave->color = al_map_rgb(hex_color[0], hex_color[1], hex_color[2]);
	nave->speed = 5;
	nave->shot.x = 0;
	nave->shot.y = 3;
	nave->shot.active = 0;
	nave->shot.hit = 0;
	nave->shot.speed_y = 0;

	nave->image = al_load_bitmap(get_random_aircraft_image());	

	printf("\nAircraft created sucessuful!\n");
}

void run_shot(Aircraft *nave){
	nave->shot.active = 1;
	nave->shot.hit = 0;
	nave->shot.x = nave->x;
	nave->shot.y = nave->y;
	nave->shot.speed_y = 0;
}

void update_shot(Aircraft *nave){
	if(nave->shot.active) {
		nave->shot.x = nave->x;
		nave->shot.speed_y += SHOT_ACEL;
		nave->shot.y += nave->shot.speed_y;

		/* Irá desativar o tiro caso:
		- A pos x do tiro + raio do tiro ultrapassar os limites laterais da tela
			OU
		- A pos y do tiro + raio ultrapassar o limite inferior
		*/
		if (
			nave->shot.x < 0-SHOT_RADIUS ||
			nave->shot.x > SCREEN_W + SHOT_RADIUS ||
			nave->shot.y > SCREEN_H + SHOT_RADIUS)
		{
			nave->shot.active = 0;

			// A nave perderá vida caso o tiro não tenha colidido com nenhum alvo!
			if(!(nave->shot.hit))
				nave->hp--;
		}
	}
}

int update_aircraft(Aircraft *nave) {
	// Retorna se a nave trocou a direção ou não
	int aircraft_changed_direction = 0;

	nave->x += (nave->dir) * nave->speed;

	/* Irá trocar a direção da nave caso:
	- O x da nave for maior do que o X do cenário + largura da nave
		OU 
	- A posição de X for menor que a posição 0 - largura da nave
	*/
	if(nave->x > SCREEN_W + AIRCRAFT_W || nave->x < 0-AIRCRAFT_W) {
		nave->dir *= -1;
		nave->image = al_load_bitmap(get_random_aircraft_image());

		aircraft_changed_direction = 1;
	}

	return aircraft_changed_direction;
}


void update_aircrafts_and_shots(Aircraft *nave1, Aircraft *nave2){
	// Se a nave1 trocar a direção horizontal, terá 50% de probabiliade de inverter a posição vertical com a nave2
	int changed_direction = update_aircraft(nave1);
	update_shot(nave1);
	
	if (changed_direction){
		if (rand_int(0, 1)){
			float y_aux = nave2->y;
			nave2->y = nave1->y;
			nave1->y = y_aux;
		}
	};

	update_aircraft(nave2);
	update_shot(nave2);
}


void create_target_grid(Target GRID[][N_COL_GRID]){
	int row, col;
	float target_width = SCREEN_W / N_COL_GRID;
	float target_height = SCREEN_H / (N_ROW_GRID*2.5); 
	
	for(row=0; row < N_ROW_GRID; row++){
		ALLEGRO_COLOR row_color = al_map_rgb(rand_int(0, 255), rand_int(0, 255), rand_int(0, 255));				

		for(col=0; col < N_COL_GRID; col++){
			GRID[row][col].active = 1;
			GRID[row][col].value = N_ROW_GRID-row;
			GRID[row][col].color = row_color;
			GRID[row][col].x1 = col * target_width;
			GRID[row][col].x2 = GRID[row][col].x1 + target_width;
			GRID[row][col].y1 = SCREEN_H - (row*target_height);
			GRID[row][col].y2 = GRID[row][col].y1 - target_height;
		}
	}
}

void draw_target_grid(Target GRID[][N_COL_GRID]){
	int i, j;

	for(i=0; i<N_ROW_GRID; i++){
		for(j=0; j<N_COL_GRID; j++) {
			if(GRID[i][j].active == 1) {
				al_draw_filled_rectangle(
					GRID[i][j].x1, GRID[i][j].y1, 
					GRID[i][j].x2, GRID[i][j].y2, GRID[i][j].color
				);
			}
		}
	}
}

int check_collision_on_target(Shot shot, Target target){
	if (
		shot.active ==1 && target.active == 1 
		&& shot.x >= target.x1 && shot.x <= target.x2   // Se o tiro.x estiver dentro do intervalo x do alvo
		&& shot.y <= target.y1 && shot.y >= target.y2   // Se o tiro.y estiver dentro do intervalo y do alvo
	) return 1;
	return 0;
}

int check_collision(Target GRID[][N_COL_GRID], Aircraft *p1, Aircraft *p2){
	int i, j, has_target_on =0;
	// Itera em todos os alvos, e checa se algum contém a posição (x, y) do tiro
	for (i=0; i<N_ROW_GRID; i++) {
		for(j=0; j<N_COL_GRID; j++) {
			if (check_collision_on_target(p1->shot, GRID[i][j])) {
				p1->score += GRID[i][j].value;
				p1->shot.hit = 1;
				GRID[i][j].active = 0;
			} else if (check_collision_on_target(p2->shot, GRID[i][j])) {
				p2->score += GRID[i][j].value;
				p2->shot.hit = 1;
				GRID[i][j].active = 0;
			}
			if (GRID[i][j].active == 1) {
				has_target_on = 1;
			} 
		}
	}
	return has_target_on;
}

void draw_scoreboard(Aircraft p1, Aircraft p2, ALLEGRO_FONT *font_24){
	float y_scoreboard = 6;
	al_draw_textf(font_24, p1.color, 125, y_scoreboard, 1, "HP: %d | SCORE: %d", p1.hp, p1.score);
	al_draw_textf(font_24, p1.color, 110, y_scoreboard+28, 1, "(P1)");
	al_draw_textf(font_24, p2.color, SCREEN_W-125, y_scoreboard, 1, "HP: %d | SCORE: %d", p2.hp, p2.score);
	al_draw_textf(font_24, p2.color, SCREEN_W-110, y_scoreboard+28, 1, "(P2)");
}

int check_if_player_won(Aircraft p1, Aircraft p2, Aircraft **winner, int has_target_active){
	// Caso algum player estoure a quantidade de vidas, perderá automaticamente
	if(
		(p1.hp > 0 && p2.hp == 0) || ((p1.score > p2.score) && p1.hp>0 && !has_target_active) 
	){
		*winner = &p1;
		return 1;
	} else if ((p2.hp > 0 && p1.hp == 0) || ((p2.score > p1.score) && p2.hp>0 && !has_target_active)){
		*winner = &p2;
		return 1;
	}
	return 0;
}

void print_winner(Aircraft *winner, ALLEGRO_FONT *font){
	// Vai printar no meio da tela!
	int border_size = 0.5;
	float y = SCREEN_H/2 - 10;
	float x = SCREEN_W/2 -15;
	ALLEGRO_COLOR cor = winner->nave_id == 1? al_map_rgb(255, 255, 255) : al_map_rgb(0, 0, 0);

	al_draw_textf(font, al_map_rgb(0, 0, 0), x - border_size, y - border_size, 1, "PLAYER %d venceu!", winner->nave_id);
    al_draw_textf(font, al_map_rgb(0, 0, 0), x + border_size, y + border_size, 1, "PLAYER %d venceu!", winner->nave_id);
    al_draw_textf(font, al_map_rgb(0, 0, 0), x - border_size, y - border_size, 1, "PLAYER %d venceu!", winner->nave_id);
    al_draw_textf(font, al_map_rgb(0, 0, 0), x + border_size, y + border_size, 1, "PLAYER %d venceu!", winner->nave_id);
	al_draw_textf(font, cor, x, y, 1, "PLAYER %d venceu!", winner->nave_id);

	al_draw_textf(font, cor, x, y+40, 1, "[HP: %d | SCORE: %d]", winner->hp, winner->score);
	al_flip_display();
}

void fill_and_print_match_history(Aircraft *winner, ALLEGRO_FONT *font){
	FILE *f, *f_write;
	ALLEGRO_COLOR cor_text = winner->nave_id == 1? al_map_rgb(255, 255, 255) : al_map_rgb(0, 0, 0);
	char f_name[] = "match_history.txt";
	int id_ultimo_vencedor, total_vitorias_p1, total_vitorias_p2, ultimo_score, ultimo_hp;

	al_clear_to_color(winner->color);

	// Lendo o arquivo
	f = fopen(f_name, "r");

	if (f == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", f_name);
        return;
    }
	
	// Ler cada linha do arquivo
    char linha[100]; 
    while (fgets(linha, 100, f) != NULL) {
        // Extrair os valores dos atributos
        sscanf(linha, "total_vitorias_p1 = %d", &total_vitorias_p1);
        sscanf(linha, "total_vitorias_p2 = %d", &total_vitorias_p2);
        sscanf(linha, "id_ultimo_vencedor = %d", &id_ultimo_vencedor);
        sscanf(linha, "ultimo_score = %d", &ultimo_score);
        sscanf(linha, "ultimo_hp = %d", &ultimo_hp);
    }

	fclose(f);

	al_draw_textf(font, cor_text, SCREEN_W/2+10, SCREEN_H*0.2, 1, "<RESULTADO JOGO ANTERIOR>:");
	al_draw_textf(font, cor_text, SCREEN_W/2+10, SCREEN_H*0.46, 1, "Vencedor: Player %d  |  Score: %d  | HP: %d ", id_ultimo_vencedor, ultimo_score, ultimo_hp);

	f_write = fopen(f_name, "w");

	id_ultimo_vencedor = winner->nave_id;
	ultimo_score = winner->score;
	ultimo_hp = winner->hp;
	total_vitorias_p1 = id_ultimo_vencedor == 1 ? total_vitorias_p1+1 : total_vitorias_p1;
	total_vitorias_p2 = id_ultimo_vencedor == 2 ? total_vitorias_p2+1 : total_vitorias_p2;	

	fprintf(f_write, "total_vitorias_p1 = %d \n", total_vitorias_p1);
    fprintf(f_write, "total_vitorias_p2 = %d \n", total_vitorias_p2);
    fprintf(f_write, "id_ultimo_vencedor = %d\n", id_ultimo_vencedor);
    fprintf(f_write, "ultimo_score = %d\n", ultimo_score);
    fprintf(f_write, "ultimo_hp = %d\n", ultimo_hp);
	
	fclose(f_write);

	al_draw_textf(font, cor_text, SCREEN_W/2, SCREEN_H*0.64, 1, "<TOTAL VITÓRIAS>:");
	al_draw_textf(font, cor_text, SCREEN_W/2, SCREEN_H*0.74, 1, "Player1: %d vitórias || Player2: %d vitórias", total_vitorias_p1, total_vitorias_p2);

	al_flip_display();
}


int main() {
	ALLEGRO_TIMER *timer = NULL; 
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_FONT *font_24 = NULL;
	Aircraft *winner;  // Ponteiro para uma struct Aircraft

	if (
		inicialize_allegro_variables(&timer, &display, &event_queue, &font_24) == -1
	) return -1;

	register_events(event_queue, display, timer);
	

	// Criando as naves
	Aircraft p1, p2;
	float x_p1 = -AIRCRAFT_W;
	float y_p1 = 2.8*AIRCRAFT_H;  
	int hex_color_p1[3] = {255, 0, 0};  // Vermelho
	create_aircraft(&p1, x_p1, y_p1, 1, hex_color_p1, 1);  // id = 1

	float y_p2 = y_p1*2;
	float x_p2 = SCREEN_W+AIRCRAFT_W;
	int hex_color_p2[3] = {255, 251, 0};  // Amarelo
	create_aircraft(&p2, x_p2, y_p2, -1, hex_color_p2, 2);  // id = 2


	Target GRID[N_ROW_GRID][N_COL_GRID];
	int has_target = 1;

	create_target_grid(GRID);

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
			draw_target_grid(GRID);
			
			update_aircrafts_and_shots(&p1, &p2);	
			
			draw_aircraft(p1);
			draw_aircraft(p2);

			draw_scoreboard(p1, p2, font_24);

			al_flip_display();

			int has_target_active = check_collision(GRID, &p1, &p2);

			if (check_if_player_won(p1, p2, &winner, has_target_active)){
				playing = 0;
			}
			
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
					run_shot(&p1);
					break;
				case ALLEGRO_KEY_A:
					run_shot(&p2);
					break;
				break;
				
			}
			
			//imprime qual tecla foi
			printf("\nTecla digitada: %d", ev.keyboard.keycode);
		}

	}      
	// Fim de jogo 
	al_clear_to_color(winner->color);
	print_winner(winner, font_24);
	al_rest(0.4);

	fill_and_print_match_history(winner, font_24);
	al_rest(3);

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_bitmap(p1.image);
	al_destroy_bitmap(p2.image);


    return 0;
}