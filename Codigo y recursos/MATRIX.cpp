

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <iomanip>
#include <ctime>
#include <sstream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <string>
#include <fstream>


ALLEGRO_DISPLAY* display;
ALLEGRO_FONT* fuente;
ALLEGRO_FONT* fuente2;
ALLEGRO_SAMPLE* intro;
ALLEGRO_SAMPLE* fondo;
ALLEGRO_TIMER* timer;

using namespace std;
#define FPS 60.0

#pragma warning(disable:4996)
int coordenadas[50];
int agrupacionPista[50] = { 0 };  // total de pistas en pantalla

// Estadisticas
unsigned t0, t1;  // valores de conteo del tiempo
int elementosPistas = 0; // cantidad de letras pintadas por pista
double tiempoTotal = 0; // timepo de caidad de caracteres
int cantidadLetras = 0;  // cantidad de letras pintadas en pantalla

typedef struct cadena {
	char letra[1];
	cadena* siguiente;
}*PtrCadena;

typedef struct hilera {
	int longitud;
	int X;
	int Y;
	int menos;
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

//Genera un n�mero random entre 8 y 20
int longitud()
{
	return (int)(10 + fastrand() % 16);
}

/*A�ade un caracter a la lista enlazada de caracteres*/
void añadirCaracter(PtrCadena& caracter, PtrCadena& nuevo)
{
	PtrCadena Aux;
	Aux = caracter;
	if (Aux != NULL)
	{
		while (Aux->siguiente != NULL)
		{
			Aux = Aux->siguiente;
		}

		Aux->siguiente = nuevo;
	}
	else
	{
		caracter = nuevo;
	}
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
	//if (arrayHilera != NULL)
	nuevo->siguiente = arrayHilera;
	arrayHilera = nuevo;
}

//Genera una coordenanda x multiplo de 13 y diferente a las coordenas almacenadas en el array de coordenadas
int generarX()
{
	int x = rand() % 884;
	bool estado = false;
	if (x % 13 != 0)
	{
		x = (int)(x / 13);
		x = x * 13;
	}
	for (int i = 0; i < 50 && !estado; i++)
	{
		if (coordenadas[i] == x)
		{
			x += 13;
			i = 0;
		}
		if (x > 884)
			x = 0;
		if (coordenadas[i] == 0)
		{
			agrupacionPista[(x / 13) -1] += 1;
			coordenadas[i] = x;
			estado = true;
		}
	}
	return x;
}

//Crea una hilera e inicializa los atributos de la hilera
void crearHilera(PtrHilera& Aux)
{
	Aux->longitud = longitud();
	Aux->X = generarX();
	Aux->Y = 5 + rand() % 300;
	Aux->siguiente = NULL;
	Aux->contador = 1;
	Aux->menos = 0;
	Aux->caracter = NULL;
	Aux->caracter = new (cadena);
	iniciarCaracter(Aux->caracter);
}

//Crea una cantidad de hileras segun el valor de la variable total
//Son las hileras que se a�adiran a la lista enlazada
void InicializarArray(PtrHilera& ArrayHileras, int total)
{
	ArrayHileras = NULL;
	for (int i = 0; i < total; i++) {
		PtrHilera Aux = new (hilera);
		crearHilera(Aux);
		añadirHilera(ArrayHileras, Aux);
	}
}


//Elimina la coordenada x del array de coordenadas y desplaza todos los elementos a la izquierda
//despues de la posici�n de donde fue borrada la coordenada
void eliminarCoordenada(int X)
{
	bool encontrado = false;
	for (int i = 0; i < 50; i++)
	{
		if (coordenadas[i] == X)
			encontrado = true;
		if (encontrado)
			coordenadas[i] = coordenadas[i + 1];
		if (i == 49)
			coordenadas[i] = 0;
	}
}

//Elimina el caracter de la cadena
void eliminarCaracter(PtrCadena& cadena, int X)
{
	PtrCadena Aux = NULL;
	Aux = cadena;
	cadena = cadena->siguiente;
	delete(Aux);

	if (cadena == NULL)
		eliminarCoordenada(X);
}

//Elimina la hilera del Array de Hileras
void EliminarHilera(PtrHilera& ArrayHilera)
{
	PtrHilera Aux = NULL;
	PtrHilera Aux2 = NULL;
	Aux = ArrayHilera;
	if (Aux->caracter != NULL)
	{
		Aux2 = Aux;
		while (Aux != NULL)
		{
			if (Aux->caracter == NULL)
			{
				if (Aux->siguiente == NULL)
				{
					Aux2->siguiente = NULL;
					delete(Aux);
					break;
				}
				else
				{
					Aux2->siguiente = Aux->siguiente;
					delete(Aux);
					break;
				}
			}
			Aux2 = Aux;
			Aux = Aux->siguiente;
		}
	}
	else
	{
		ArrayHilera = Aux->siguiente;
		delete(Aux);
	}
}

void destruirArrayHileras(PtrHilera& ArrayHileras) {
	PtrHilera Aux;
	Aux = ArrayHileras;
	while (Aux != NULL)
	{
		while (Aux->caracter != NULL)
		{
			PtrCadena Aux2 = NULL;
			Aux2 = Aux->caracter;
			Aux->caracter = Aux->caracter->siguiente;
			delete(Aux2);
		}
		ArrayHileras = ArrayHileras->siguiente;
		delete(Aux);
		Aux = ArrayHileras;
	}
}

//Revisa si las hileras ya cumplieron su maximo de caracteres, y si es as�, comienza a
//borrar los caracteres, manda a llamar la funcion de borrar hilera en caso de que la
//hilera ya no tenga caracteres y crea nuevas hileras seg�n la cantidad de hileras eliminadas
void verificar(PtrHilera& ArrayHilera)
{
	bool eliminado = false;
	int cont = 0;
	PtrHilera Aux = NULL;
	Aux = ArrayHilera;
	while (Aux != NULL)
	{
		if (Aux->contador <= Aux->longitud)
		{
			PtrCadena nuevo = new (cadena);
			iniciarCaracter(nuevo);
			añadirCaracter(Aux->caracter, nuevo);
			Aux->contador++;
		}
		if (Aux->contador > Aux->longitud)
		{
			if (Aux->caracter == NULL)
			{
				Aux = Aux->siguiente;
				EliminarHilera(ArrayHilera);
				eliminado = true;
				cont++;
				continue;
			}
			else
			{
				eliminarCaracter(Aux->caracter, Aux->X);
				Aux->menos++;
				Aux->Y += 15;
			}
		}
		Aux = Aux->siguiente;
	}
	if (eliminado)
	{
		for (int i = 0; i < cont; i++)
		{
			PtrHilera Aux = new (hilera);
			crearHilera(Aux);
			añadirHilera(ArrayHilera, Aux);
		}
	}
}


//Imprime en pantalla los caracteres de cada array
void dibujar(PtrHilera& ArrayHileras)
{

	PtrHilera Aux = NULL;
	Aux = ArrayHileras;
	while (Aux != NULL)
	{
		PtrCadena Aux2 = Aux->caracter;
		int cont = 1;
		int Y = Aux->Y;
		int X = Aux->X;

		while (Aux2 != NULL)
		{
			if (Aux->contador <= (Aux->longitud) - 5)
			{
				if (cont == Aux->contador)
				{
					al_draw_text(fuente, al_map_rgb(255, 255, 255), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
				}
				else
					al_draw_text(fuente, al_map_rgb(1, 252, 26), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
			}
			else
			{
				if (cont == (Aux->contador - Aux->menos))
				{
					al_draw_text(fuente, al_map_rgb(255, 255, 255), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
				}
				else if (cont == 6)
					al_draw_text(fuente, al_map_rgb(0, 190, 0), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
				else if (cont == 5)
					al_draw_text(fuente, al_map_rgb(0, 160, 0), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
				else if (cont == 4)
					al_draw_text(fuente, al_map_rgb(0, 130, 0), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
				else if (cont == 3)
					al_draw_text(fuente, al_map_rgb(0, 100, 0), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
				else if (cont == 2)
					al_draw_text(fuente, al_map_rgb(0, 80, 0), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
				else if (cont == 1)
					al_draw_text(fuente, al_map_rgb(0, 50, 0), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
				else
					al_draw_text(fuente, al_map_rgb(1, 252, 26), X, Y, ALLEGRO_ALIGN_LEFT, Aux2->letra);
			}

			cont++;
			Y += 15;
			Aux2 = Aux2->siguiente;
		}

		cantidadLetras++;
		Aux = Aux->siguiente;

	}
}

//Almacena cada uno de los datos de las estad�sticas en un archivo de texto
//Calcula el tiempo de ejecuci�n final el terminar la ejecuci�n de las funciones principales
//Obtiene la fecha y hora actual para ser guardado los datos de tal forma que se muestre un orden temporal
//Abre el archivo de texto y almacena cada dato por l�nea en formato de string
void guardarEstadisticas(int total) {
	string TotalPistas, agrupacionesPista, tiempoSimulacion, letrasPintadas;
	string nombreArchivo = "datos.txt";

	// Determinar tiempo de ejecuci�n
	t1 = clock();
	double tiempoTotal = (double(t1 - t0) / CLOCKS_PER_SEC);
	cout << "Tiempo tota de ejecucion: " << tiempoTotal << endl;
	tiempoSimulacion = to_string(tiempoTotal);

	// Obtiene la fecha y hora actual
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
	auto str = oss.str();

	// Guardar de datos en el archivo
	ofstream archivo;
	archivo.open(nombreArchivo.c_str(), fstream::app);

	TotalPistas = to_string(total);
	letrasPintadas = to_string(cantidadLetras);

	archivo << "Fecha y Hora: " + str << endl;
	archivo << "Cantidad de pistas: " + TotalPistas << endl;
	archivo << "Agrupaciones por pista: " << endl;
	for (int i = 0; i < total - 1; i++) {
		if (agrupacionPista[i] != 0)
			archivo << agrupacionPista[i] << ", ";
	}archivo << endl;
	archivo << "Tiempo de simulacion: " + tiempoSimulacion << endl;
	archivo << "Cantidad de letras pintadas: " + letrasPintadas << endl;
	archivo << endl;

	archivo.close();
	cout << "Guardado correctamente" << endl;
	cout << endl;

}

//Se carga el archivo a memoria y se imprime en pantalla las lineas que estan en el
//rango que abarca las variables inicio y fin.
void CargarArchivo(ALLEGRO_FONT* fuente, int inicio, int fin) {
	string nombreArchivo = "datos.txt";
	ifstream archivo(nombreArchivo.c_str());
	string linea;

	int i = 30;
	int cont = 0;
	constexpr int CHAR_LENGTH = 1;
	// Obtener linea de archivo, y almacenar contenido en cada "linea" para ser mostrada en pantalla
	while (getline(archivo, linea)) {
		char* char_arr;
		string str_obj(linea);
		char_arr = &str_obj[0];
		//Si el contador se encuentra entre las variables inicio y fin, se imprime
		//el contenido de la línea de texto
		if (cont >= inicio && cont < fin) {
			al_draw_text(fuente, al_map_rgb(255, 255, 255), 5, i, 0, char_arr);
			i += 15;
		}
		cont++;
	}
}

//Programa principal
int main()
{
	/*srand(time(NULL));
	const int total = 30 + rand() % 21;*/  //Total de cadenas de caracteres que caen en pantalla
	const int total = 50;
	printf("Cantidad de hileras: %d", total);
	cout << endl;
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
	al_init_image_addon();

	display = al_create_display(900, 600);
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	al_set_window_title(display, "MATRIX");

	if (!display)
	{
		al_show_native_message_box(display, "ERROR", "Display Settings", "Display Window was not created successfully", NULL, NULL);
	}

	fuente = al_load_font("Montserrat-Regular.ttf", 12, NULL); //Fuente utilizada para la simulacion
	intro = al_load_sample("MUSICA/intro.WAV"); //sonido utilizado para la pantalla inical
	fuente2 = al_load_font("Chiken.otf", 20, NULL); //Fuente para la pantalla inicial
	fondo = al_load_sample("MUSICA/sound.wav"); //Sonido de fondo durante la simulaci�n


	ALLEGRO_EVENT_QUEUE* Cola_eventos = al_create_event_queue(); //Se cre la cola de eventos
	ALLEGRO_TIMER* timer = al_create_timer(5.0 / FPS); //Timer para la simulacion 
	ALLEGRO_TIMER* timer2 = al_create_timer(2.0 / 10); //Timer para la pantalla inicial
	ALLEGRO_TIMER* timer3 = al_create_timer(1.0 / FPS); //Timer para la pantalla de estadisticas
	ALLEGRO_BITMAP* Flecha = al_load_bitmap("Imagenes/Flecha.png");
	
	al_register_event_source(Cola_eventos, al_get_timer_event_source(timer));
	al_register_event_source(Cola_eventos, al_get_timer_event_source(timer2));
	al_register_event_source(Cola_eventos, al_get_timer_event_source(timer3));
	al_register_event_source(Cola_eventos, al_get_keyboard_event_source());
	


	al_start_timer(timer2);
	int cont = 0;
	const char* matrix[] = { 
		"T", "TH", "THE", "THE_", 
		"THE M", "THE MA", "THE MAT", 
		"THE MATR", "THE MATRI", "THE MATRIX" };
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
	al_stop_timer(timer2);


	//Crea el array de hileras
	PtrHilera ArrayHileras;
	InicializarArray(ArrayHileras, total);

	bool done = false;
	al_start_timer(timer); //Timer para la simulacion
	t0 = clock();
	//Sonido de fondo durante la simulacion
	al_play_sample(fondo, 0.8, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL); 

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
				guardarEstadisticas(total);
				done = true;
				break;
			}
		}
		if (eventos.type == ALLEGRO_EVENT_TIMER) {
			if (eventos.timer.source == timer)
			{
				al_clear_to_color(al_map_rgb(0, 0, 0));
				dibujar(ArrayHileras);
				verificar(ArrayHileras);
				al_flip_display();
			}
		}
	}
	al_stop_timer(timer);


	al_destroy_sample(fondo);
	al_install_mouse();
	al_register_event_source(Cola_eventos, al_get_mouse_event_source());

	al_start_timer(timer3);
	bool cond = false;
	int inicio = 0, fin = 28;
	int mousex = 0, mousey = 0;
	al_clear_to_color(al_map_rgb(0, 0, 0));
	//PANTALLA ESTADISTICAS
	while (!cond)
	{
		ALLEGRO_EVENT eventos;
		al_wait_for_event(Cola_eventos, &eventos);
		if (eventos.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (eventos.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				cond = true;
				break;
			}
		}
		if (eventos.type == ALLEGRO_EVENT_TIMER)
		{
			//if (eventos.timer.source == timer){
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, "ESTADISTICAS");
			al_draw_bitmap(Flecha, 100, 500, NULL);
			al_draw_bitmap(Flecha, 800, 500, ALLEGRO_FLIP_HORIZONTAL);
			CargarArchivo(fuente, inicio, fin);
			//}
		}
		if (eventos.type == ALLEGRO_EVENT_MOUSE_AXES)
		{
			mousex = eventos.mouse.x;
			mousey = eventos.mouse.y;
		}
		//Si el mouse se posiciona en las flechas se pinta de amarillo la flecha y 
		//si se presiona se vanza arriba o abajo en los resultados
		if ((mousex >= 100 && mousex <= 150) && (mousey >= 500 && mousey <= 550)) {
			al_draw_tinted_bitmap(Flecha, al_map_rgb(25, 250, 250), 100, 500, NULL);
			if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (eventos.mouse.button & 1) {
					if (inicio > 0) {
						inicio -= 29;
						fin -= 29;
					}
				}
			}
		}
		if ((mousex >= 800 && mousex <= 850) && (mousey >= 500 && mousey <= 550)) {
			al_draw_tinted_bitmap(Flecha, al_map_rgb(25, 250, 250), 800, 500, ALLEGRO_FLIP_HORIZONTAL);
			if (eventos.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				if (eventos.mouse.button & 1) {
					inicio += 29;
					fin += 29;
				}
			}
		}
		al_flip_display();
	}

	destruirArrayHileras(ArrayHileras);
	al_stop_timer(timer3);
	al_destroy_font(fuente);
	al_destroy_display(display);

	return 0;
}


