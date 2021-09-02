

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


ALLEGRO_DISPLAY* display;
ALLEGRO_FONT* fuente = NULL;
ALLEGRO_FONT* fuente2 = NULL;
ALLEGRO_SAMPLE* intro = NULL;
ALLEGRO_SAMPLE* fondo = NULL;
ALLEGRO_TIMER* timer;

using namespace std;
#define FPS 60.0

#pragma warning(disable:4996);

int main()
{

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

	display = al_create_display(720, 480);
	al_set_window_position(display, 200, 200);
	al_set_window_title(display, "MATRIX");

	if (!display)
	{
		al_show_native_message_box(display, "ERROR", "Display Settings", "Display Window was not created successfully", NULL, NULL);
	}

	//fuente = al_load_font("Chiken.ttf", 12, NULL);
	//fuente2 = al_load_font("Chiken.ttf", 18, NULL);
	//intro = al_load_sample("MUSICA/intro.wav");
	//fondo = al_load_sample("MUSICA/sound.wav");


	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

	al_install_keyboard();
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	bool done = false, draw = true;
	int x = 20, y = 400, x2 = 600, y2 = 100;
	int moveSpeed = 5;

	ALLEGRO_TIMER* timer = al_create_timer(2.0 / FPS);
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_start_timer(timer);



	while (!done)
	{


		ALLEGRO_EVENT events;
		al_wait_for_event(event_queue, &events);

		if (events.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (events.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			}
		}
	}




	al_destroy_display(display);


	return 0;
}