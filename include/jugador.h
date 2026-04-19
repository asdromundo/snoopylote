#ifndef JUGADOR_H
#define JUGADOR_H

#include <allegro5/allegro.h>
#include <stdbool.h>

#define JUGADOR_MAX_SPRITES 8
#define JUGADOR_ANIMACIONES 4

enum EstadoAnimacion
{
    ANIMACION_IDLE,
    ANIMACION_CAMINAR_DERECHA,
    ANIMACION_CAMINAR_IZQUIERDA,
    ANIMACION_SALTAR
};

struct Vec2
{
    float x;
    float y;
};

struct Jugador
{
    /* Posicion */
    float pos_x;
    float pos_y;

    /* Animaciones: sprites[ANIMACION][frame] */
    ALLEGRO_BITMAP *sprites[JUGADOR_ANIMACIONES][JUGADOR_MAX_SPRITES];
    int num_sprites[JUGADOR_ANIMACIONES]; /* Cantidad de frames por animacion */

    /* Estado actual */
    enum EstadoAnimacion animacion_actual;
    int frame_actual;
    double tiempo_acumulado;
    double intervalo_animacion; /* segundos entre frames */

    /* Estado de movimiento */
    bool esta_caminando;
    bool esta_saltando;
    int direccion; /* 1 = derecha, -1 = izquierda */
};

/* Inicializacion y limpieza */
bool jugador_inicializar(struct Jugador *jugador);
void jugador_destruir(struct Jugador *jugador);

/* Carga de sprites */
bool jugador_cargar_sprites(struct Jugador *jugador);

/* Actualizacion */
void jugador_actualizar(struct Jugador *jugador, double delta_tiempo);
void jugador_set_estado_caminando(struct Jugador *jugador, bool caminando, int direccion);
void jugador_set_estado_saltando(struct Jugador *jugador, bool saltando);

/* Renderizado */
void jugador_dibujar(const struct Jugador *jugador);

#endif /* JUGADOR_H */
