#include "jugador.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>

bool jugador_inicializar(struct Jugador *jugador, const char *ruta_sprite)
{
    jugador->sprite = al_load_bitmap(ruta_sprite);
    if (!jugador->sprite) {
        fprintf(stderr, "No se pudo cargar la imagen: %s\n", ruta_sprite);
        return false;
    }
    return true;
}

void jugador_mover(struct Jugador *jugador, struct Vec2 velocidad)
{
    jugador->pos_x += velocidad.x;
    jugador->pos_y += velocidad.y;
}

void jugador_dibujar(const struct Jugador *jugador)
{
    if (jugador->sprite) {
        al_draw_bitmap(jugador->sprite, jugador->pos_x, jugador->pos_y, 0);
    }
}

void jugador_destruir(struct Jugador *jugador)
{
    if (jugador->sprite) {
        al_destroy_bitmap(jugador->sprite);
        jugador->sprite = NULL;
    }
}
