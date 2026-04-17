#ifndef JUGADOR_H
#define JUGADOR_H

#include <allegro5/allegro.h>
#include <stdbool.h>

struct Vec2 {
    float x;
    float y;
};

struct Jugador {
    ALLEGRO_BITMAP *sprite;
    float pos_x;
    float pos_y;
};

bool jugador_inicializar(struct Jugador *jugador, const char *ruta_sprite);
void jugador_mover(struct Jugador *jugador, struct Vec2 velocidad);
void jugador_dibujar(const struct Jugador *jugador);
void jugador_destruir(struct Jugador *jugador);

#endif /* JUGADOR_H */
