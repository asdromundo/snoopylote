#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "jugador.h"

#define VELOCIDAD_MOVIMIENTO 200.0f /* pixeles por segundo */
#define VELOCIDAD_SALTO 300.0f      /* pixeles por segundo */
#define GRAVEDAD 800.0f             /* aceleracion hacia abajo */
#define ALTURA_SUELO 510.0f         /* Y donde esta el suelo */
#define WIDTH 800
#define HEIGHT 600

int main(void)
{
    if (!al_init() || !al_init_primitives_addon() || !al_install_keyboard() || !al_init_image_addon())
    {
        fprintf(stderr, "No se pudo iniciar Allegro o sus addons.\n");
        return 1;
    }

    ALLEGRO_DISPLAY *ventana = al_create_display(WIDTH, HEIGHT);
    if (!ventana)
    {
        fprintf(stderr, "No se pudo crear la ventana.\n");
        return 1;
    }
    al_set_window_title(ventana, "Avanzar");

    ALLEGRO_COLOR colorfondo = al_map_rgb(255, 255, 255);
    al_clear_to_color(colorfondo);
    al_flip_display();

    ALLEGRO_EVENT_QUEUE *colaeventos = al_create_event_queue();
    if (!colaeventos)
    {
        fprintf(stderr, "No se pudo crear la cola de eventos.\n");
        al_destroy_display(ventana);
        return 1;
    }

    al_register_event_source(colaeventos, al_get_display_event_source(ventana));
    al_register_event_source(colaeventos, al_get_keyboard_event_source());

    // Nivel

    ALLEGRO_BITMAP *fondo;
    fondo = al_load_bitmap("./images/fondo.jpeg");
    if (!fondo)
    {
        fprintf(stderr, "No se pudo cargar el fondo.\n");
    }

    /* Inicializar jugador */
    struct Jugador snoopy1;
    if (!jugador_inicializar(&snoopy1))
    {
        fprintf(stderr, "No se pudo inicializar el jugador.\n");
        jugador_destruir(&snoopy1);
        al_destroy_event_queue(colaeventos);
        al_destroy_display(ventana);
        return 1;
    }

    /* Variables de fisica */
    float velocidad_y = 0.0f;
    bool en_suelo = true;
    bool saltando = false;

    /* Control de tiempo para delta */
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0); /* 60 FPS */
    al_start_timer(timer);
    al_register_event_source(colaeventos, al_get_timer_event_source(timer));

    bool terminar = false;
    ALLEGRO_EVENT evento;
    double delta_tiempo = 0.0;
    double tiempo_anterior = al_get_time();

    while (!terminar)
    {
        double tiempo_actual = al_get_time();
        delta_tiempo = tiempo_actual - tiempo_anterior;
        tiempo_anterior = tiempo_actual;

        /* Procesar eventos */
        while (al_get_next_event(colaeventos, &evento))
        {
            switch (evento.type)
            {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                terminar = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                switch (evento.keyboard.keycode)
                {
                case ALLEGRO_KEY_ESCAPE:
                    terminar = true;
                    break;
                case ALLEGRO_KEY_SPACE:
                case ALLEGRO_KEY_UP:
                    // Salta
                    if (en_suelo)
                    {
                        velocidad_y = -VELOCIDAD_SALTO;
                        en_suelo = false;
                        saltando = true;
                        jugador_set_estado_saltando(&snoopy1, true);
                    }
                    break;
                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        /* Actualizar estado basado en teclas presionadas */
        ALLEGRO_KEYBOARD_STATE teclado;
        al_get_keyboard_state(&teclado);

        bool moviendo = false;
        int direccion = 0;

        if (al_key_down(&teclado, ALLEGRO_KEY_RIGHT))
        {
            snoopy1.pos_x += VELOCIDAD_MOVIMIENTO * delta_tiempo;
            moviendo = true;
            direccion = 1;
        }
        else if (al_key_down(&teclado, ALLEGRO_KEY_LEFT))
        {
            snoopy1.pos_x -= VELOCIDAD_MOVIMIENTO * delta_tiempo;
            moviendo = true;
            direccion = -1;
        }

        /* Clamp position to screen bounds */
        if (snoopy1.pos_x >= WIDTH - 32)
        {
            snoopy1.pos_x = WIDTH - 32;
        }
        if (snoopy1.pos_x <= 0)
        {
            snoopy1.pos_x = 0;
        }

        /* Actualizar estado de caminar */
        jugador_set_estado_caminando(&snoopy1, moviendo, direccion);

        /* Aplicar fisica de salto */
        if (!en_suelo)
        {
            velocidad_y += GRAVEDAD * delta_tiempo;
            snoopy1.pos_y += velocidad_y * delta_tiempo;

            /* Colision con suelo */
            if (snoopy1.pos_y >= ALTURA_SUELO)
            {
                snoopy1.pos_y = ALTURA_SUELO;
                velocidad_y = 0;
                en_suelo = true;
                saltando = false;
                jugador_set_estado_saltando(&snoopy1, false);
            }
        }

        /* Actualizar animacion */
        jugador_actualizar(&snoopy1, delta_tiempo);

        /* Dibujar */
        al_clear_to_color(colorfondo);

        al_draw_scaled_bitmap(fondo, 0, 0, 500, 400, 0, 0, WIDTH, HEIGHT, 0);

        /* Dibujar linea de suelo */
        al_draw_line(0, ALTURA_SUELO, WIDTH, ALTURA_SUELO, al_map_rgb(0, 0, 0), 2);

        /* Dibujar jugador */
        jugador_dibujar(&snoopy1);

        al_flip_display();
    }

    /* Limpieza */
    al_stop_timer(timer);
    al_destroy_timer(timer);
    jugador_destruir(&snoopy1);
    al_destroy_event_queue(colaeventos);
    al_destroy_display(ventana);
    return 0;
}
