#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "jugador.h"

static const char *JUGADOR_SPRITE = "images/cam.png";

int main(void)
{
    if (!al_init() || !al_init_primitives_addon() || !al_install_keyboard() || !al_init_image_addon()) {
        fprintf(stderr, "No se pudo iniciar Allegro o sus addons.\n");
        return 1;
    }

    ALLEGRO_DISPLAY *ventana = al_create_display(700, 600);
    if (!ventana) {
        fprintf(stderr, "No se pudo crear la ventana.\n");
        return 1;
    }
    al_set_window_title(ventana, "Avanzar");

    ALLEGRO_COLOR colorfondo = al_map_rgb(255, 255, 255);
    al_clear_to_color(colorfondo);
    al_flip_display();

    ALLEGRO_EVENT_QUEUE *colaeventos = al_create_event_queue();
    if (!colaeventos) {
        fprintf(stderr, "No se pudo crear la cola de eventos.\n");
        al_destroy_display(ventana);
        return 1;
    }

    al_register_event_source(colaeventos, al_get_display_event_source(ventana));
    al_register_event_source(colaeventos, al_get_keyboard_event_source());

    struct Jugador snoopy1 = { NULL, 50.0f, 50.0f };
    if (!jugador_inicializar(&snoopy1, JUGADOR_SPRITE)) {
        al_destroy_event_queue(colaeventos);
        al_destroy_display(ventana);
        return 1;
    }

    bool terminar = false;
    ALLEGRO_EVENT evento;
    while (!terminar) {
        if (al_get_next_event(colaeventos, &evento)) {
            switch (evento.type) {
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    terminar = true;
                    break;

                case ALLEGRO_EVENT_KEY_DOWN:
                    switch (evento.keyboard.keycode) {
                        case ALLEGRO_KEY_RIGHT:
                            jugador_mover(&snoopy1, (struct Vec2){10.0f, 0.0f});
                            break;
                        case ALLEGRO_KEY_LEFT:
                            jugador_mover(&snoopy1, (struct Vec2){-10.0f, 0.0f});
                            break;
                        case ALLEGRO_KEY_UP:
                            jugador_mover(&snoopy1, (struct Vec2){0.0f, -10.0f});
                            break;
                        case ALLEGRO_KEY_DOWN:
                            jugador_mover(&snoopy1, (struct Vec2){0.0f, 10.0f});
                            break;
                        case ALLEGRO_KEY_ESCAPE:
                            terminar = true;
                            break;
                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        al_clear_to_color(colorfondo);
        jugador_dibujar(&snoopy1);
        al_flip_display();
    }

    jugador_destruir(&snoopy1);
    al_destroy_event_queue(colaeventos);
    al_destroy_display(ventana);
    return 0;
}
