

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <string>


ALLEGRO_DISPLAY* display;
ALLEGRO_FONT* fuente;
ALLEGRO_FONT* fuente2;
ALLEGRO_SAMPLE* intro;
ALLEGRO_SAMPLE* fondo;
ALLEGRO_TIMER* timer;

using namespace std;
#define FPS 60.0

#pragma warning(disable:4996);




//int total = (int)(30 + rand() % 21); //Total de cadenas de caracteres que caen en pantalla
int total = 30;


typedef struct cadena {
	char letra[1];
	cadena* siguiente;
}*PtrCadena;

typedef struct hilera {
	int longitud;
	int X;
	int Y;
	//bool estado;
	int contador;
	PtrCadena caracter;
	hilera* siguiente;
}*PtrHilera;

long g_seed = 1;
inline int fastrand() {
	g_seed = (214013 * g_seed + 2531011);
	return (g_seed >> 16) & 0x7FFF;
}
//Genera un caracter random
char GenerarRandom()
{
	char c = 'A' + fastrand() % 24;
	return c;
}

//Genera un número random entre 5 y 20
int longitud()
{
	return (int)(5 + fastrand() % 20);
}

/*Añade un caracter a la lista enlazada de caracteres*/
void añadirCaracter(PtrCadena& caracter, PtrCadena& nuevo)
{
	if (caracter != NULL)
		nuevo->siguiente = caracter;
	caracter = nuevo;
}

//Asigna un caracter random y NULL al puntero siguiente
void iniciarCaracter(cadena*& caracter)
{
	caracter->letra[0] = GenerarRandom();
	caracter->siguiente = NULL;
}

//Añade una hilera a la lista enlazada de hileras
void añadirHilera(PtrHilera& arrayHilera, PtrHilera& nuevo)
{
	if (arrayHilera != NULL)
	nuevo->siguiente = arrayHilera;
	arrayHilera = nuevo;
}

//Crea una hilera e inicializa los atributos de la hilera
void crearHilera(PtrHilera& Aux)
{
	Aux->longitud = longitud();
	Aux->X = 5 + rand() % 890;
	Aux->Y = 5 + rand() % 300;
	Aux->siguiente = NULL;
	Aux->contador = 1;
	Aux->caracter = new (cadena);
	iniciarCaracter(Aux->caracter);
}

//Crea una cantidad de hileras segun el valor de la variable total
//Son las hileras que se añadiran a la lista enlazada
void InicializarArray(PtrHilera& ArrayHileras, int total)
{
	ArrayHileras = NULL;
	for (int i = 0; i < total; i++) {
		PtrHilera Aux = new (hilera);
		crearHilera(Aux);
		añadirHilera(ArrayHileras, Aux);
	}
}


void dibujar(PtrHilera& ArrayHileras)
{
	PtrHilera Aux = NULL;
	Aux = ArrayHileras;
	while (Aux != NULL)
	{
		PtrCadena Aux2 = Aux->caracter;
		int cont = 0;
		int Y = Aux->Y;
		int X = Aux->X;
		while (Aux2 != NULL)
		{
			if (cont == 0)
			{
				al_draw_text(fuente, al_map_rgb(255, 255, 255), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
			}
			else
				al_draw_text(fuente, al_map_rgb(1, 252, 26), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
			cont++;
			Y += 20;
			Aux2 = Aux2->siguiente;
		}
		Aux = Aux->siguiente;
	}
}

void eliminarCaracter(PtrCadena& cadena)
{
	PtrCadena Aux = NULL;
	Aux = cadena;
	PtrCadena Aux2 = NULL;
	if (Aux->siguiente != NULL)
	{
		while (Aux->siguiente != NULL)
		{
			Aux2 = Aux;
			Aux = Aux->siguiente;
		}
		delete(Aux);
		Aux2->siguiente = NULL;
	}
	delete (Aux);
	cadena = NULL;
}

//void EliminarHilera(PtrHilera ArrayHilera);

//void verificar(PtrHilera& ArrayHilera)
//{
//	PtrHilera Aux = NULL;
//	Aux = ArrayHilera;
//	while (Aux != NULL)
//	{
//		if (Aux->contador <= Aux->longitud)
//		{
//			PtrCadena nuevo = new (cadena);
//			iniciarCaracter(nuevo);
//			añadirCaracter(Aux->caracter, nuevo);
//			Aux->contador++;
//		}
//		if (Aux->contador > Aux->longitud)
//		{
//			if (Aux->caracter == NULL)
//			{
//				EliminarHilera(ArrayHilera);
//			}
//			eliminarCaracter(Aux->caracter);
//		}
//
//		Aux = Aux->siguiente;
//	}
//}

//Programa principal
int main()
{
	printf("Cantidad de hileras: %d", total);
	if (!al_init())
	{
		al_show_native_message_box(NULL, NULL, NULL, "Could not initialize Allegro 5 ", NULL, NULL);
		return -1;
	}

	al_init_font_addon();
	al_init_ttf_addon();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1);
	al_install_keyboard();

	display = al_create_display(900, 600);
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	al_set_window_title(display, "MATRIX");

	if (!display)
	{
		al_show_native_message_box(display, "ERROR", "Display Settings", "Display Window was not created successfully", NULL, NULL);
	}

	//fuente = al_load_font("Chiken.otf", 12, NULL);
	fuente = al_load_font("Montserrat-Regular.ttf", 12, NULL);
	intro = al_load_sample("MUSICA/intro.WAV"); //sonido utilizado para la pantalla inical
	fuente2 = al_load_font("Chiken.otf", 20, NULL); //Fuente para la pantalla inicial
	fondo = al_load_sample("MUSICA/sound.wav"); //Sonido de fondo durante la simulación

	ALLEGRO_EVENT_QUEUE* Cola_eventos = al_create_event_queue(); //Se cre la cola de eventos

	al_install_keyboard();
	al_register_event_source(Cola_eventos, al_get_keyboard_event_source());


	ALLEGRO_TIMER* timer = al_create_timer(10.0 / FPS); //Timer para la simulacion 
	al_register_event_source(Cola_eventos, al_get_timer_event_source(timer));

	ALLEGRO_TIMER* timer2 = al_create_timer(2.0 / 10); //Timer para la pantalla inicial
	al_register_event_source(Cola_eventos, al_get_timer_event_source(timer2));

	
	al_start_timer(timer2);
	int cont = 0;
	const char* matrix[] = { "T", "TH", "THE", "THE_", "THE M", "THE MA", "THE MAT", "THE MATR", "THE MATRI", "THE MATRIX" };
	//PANTALLA INTRODUCTORIA
	while (cont < 16)
	{
		ALLEGRO_EVENT eventos;
		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_wait_for_event(Cola_eventos, &eventos);
		if (eventos.type == ALLEGRO_EVENT_TIMER) {
			if (eventos.timer.source == timer2) {
				if (cont < 10) {
					al_play_sample(intro, 1.0, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					al_draw_text(fuente2, al_map_rgb(1, 252, 26), 400, 300, ALLEGRO_ALIGN_LEFT, matrix[cont]);
					cont++;
				}
				else if (cont == 15) {
					al_clear_to_color(al_map_rgb(0, 0, 0));
					cont++;
				}
				else {
					al_draw_text(fuente2, al_map_rgb(1, 252, 26), 400, 300, ALLEGRO_ALIGN_LEFT, "THE MATRIX");
					cont++; 
				}
			}
		}
		al_flip_display();
	}
	al_destroy_font(fuente2);
	al_destroy_sample(intro);
	
	
	
	 
	PtrHilera ArrayHileras;
	InicializarArray(ArrayHileras, total);	
	

	bool done = false;
	al_start_timer(timer); //Timer para la simulacion
	al_play_sample(fondo, 0.8, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL); //Sonido de fondo durante la simulacion
	
	//INICIO DE LA SIMULACION
	//La simulación termina si se presicona la tecla ESCAPE
	while (!done)
	{
		

		ALLEGRO_EVENT eventos;
		al_wait_for_event(Cola_eventos, &eventos);
		if (eventos.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (eventos.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			}
		}
		if (eventos.type == ALLEGRO_EVENT_TIMER) {
			if (eventos.timer.source == timer) 
			{
				dibujar(ArrayHileras);
				//verificar(ArrayHileras);
				al_flip_display();
			}
		}
	}
	al_destroy_font(fuente);
	al_destroy_sample(fondo);
	al_destroy_display(display);


	return 0;
}