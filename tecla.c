#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

ALLEGRO_DISPLAY *ventana;
ALLEGRO_COLOR colorcirculo;
ALLEGRO_COLOR colorfondo;
ALLEGRO_EVENT_QUEUE *colaeventos;
ALLEGRO_EVENT evento;

const char JUGADOR_SPRITE[3][30] = {"images/cam.png", "images/cam1.png", "images/cam2.png"};

struct Jugador
{
    ALLEGRO_BITMAP *sprite;
    float pos_x;
    float pos_y;
};

struct Vec2
{
    float x;
    float y;
};

void jugador_mover(struct Jugador *jugador, struct Vec2 velocidad)
{
    jugador->pos_x += velocidad.x;
    jugador->pos_y += velocidad.y;
}

void jugador_dibujar(struct Jugador *jugador)
{
    al_draw_bitmap(jugador->sprite, jugador->pos_x, jugador->pos_y, 0);
}

void jugador_cargar_sprites(struct Jugador *jugador)
{
    jugador->sprite = al_load_bitmap(JUGADOR_SPRITE[0]);
    if (!jugador->sprite)
        printf("No se pudo cargar la imagen.\n");
}

int main(void)
{
    // Inicialización
    bool terminar = false;
    int fin = 0, cambio = 0, coordx, coordy;
    if (!al_init() || !al_init_primitives_addon() || !al_install_keyboard() || !al_init_image_addon())
        terminar = true;

    if (terminar)
        return 0;

    ventana = al_create_display(700, 600);
    al_set_window_title(ventana, "Avanzar");

    // Renderizado
    colorfondo = al_map_rgb(255, 255, 255);
    al_clear_to_color(colorfondo);
    al_flip_display();

    // Inicializacion del jugador
    // colorcirculo = al_map_rgb(70, 155, 199);
    struct Jugador snoopy1 = {NULL, 50, 50};
    snoopy1.sprite = al_load_bitmap(JUGADOR_SPRITE[0]);
    if (!snoopy1.sprite)
        printf("No se pudo cargar la imagen.\n");
    jugador_cargar_sprites(&snoopy1);

    // Eventos
    colaeventos = al_create_event_queue();
    al_register_event_source(colaeventos, al_get_display_event_source(ventana));
    al_register_event_source(colaeventos, al_get_keyboard_event_source());

    // Game Loop
    while (!terminar)
    {
        // Manejar eventos
        if (al_is_event_queue_empty(colaeventos) && cambio == 1)
        {
            // movimiento(coordx, coordy);
        }
        al_wait_for_event(colaeventos, &evento);
        switch (evento.type)
        {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            printf("Ventana cerrada");
            fin = 1;
            al_destroy_display(ventana);
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            if (evento.keyboard.keycode == 83)
            {
                cambio = 1;
                al_clear_to_color(colorfondo);
                coordx += 10;
            }
            if (evento.keyboard.keycode == 82)
            {
                cambio = 1;
                al_clear_to_color(colorfondo);
                coordx -= 10;
            }
            if (evento.keyboard.keycode == 84)
            {
                cambio = 1;
                al_clear_to_color(colorfondo);
                coordy -= 10;
            }
            if (evento.keyboard.keycode == 85)
            {
                cambio = 1;
                al_clear_to_color(colorfondo);
                coordy += 10;
            }
            break;
        }

        struct Vec2 vel_snoopy1 = {1, 1};
        // Loop jugador

        // Fisicas
        jugador_mover(&snoopy1, vel_snoopy1);

        // Renderizado
        jugador_dibujar(&snoopy1);

        al_flip_display();
    }

    // Terminación
    al_destroy_event_queue(colaeventos);
    return 0;
}
