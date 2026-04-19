#include "jugador.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <string.h>

/* Ruta base de las imagenes */
#define RUTA_BASE "images/"

/* Nombres de los archivos de sprites */
static const char *NOMBRES_IDLE[] = {
    "cam.png"};

static const char *NOMBRES_CAMINAR[] = {
    "cam1.png",
    "cam2.png"};

/* Inicializa el jugador con valores por defecto */
bool jugador_inicializar(struct Jugador *jugador)
{
    if (!jugador)
    {
        return false;
    }

    /* Inicializar posicion */
    jugador->pos_x = 100.0f;
    jugador->pos_y = 510.0f;

    /* Inicializar arrays de sprites a NULL */
    for (int i = 0; i < JUGADOR_ANIMACIONES; i++)
    {
        for (int j = 0; j < JUGADOR_MAX_SPRITES; j++)
        {
            jugador->sprites[i][j] = NULL;
            jugador->num_sprites[i] = 0;
        }
    }

    /* Estado inicial */
    jugador->animacion_actual = ANIMACION_IDLE;
    jugador->frame_actual = 0;
    jugador->tiempo_acumulado = 0.0;
    jugador->intervalo_animacion = 0.15; /* 6-7 FPS para animacion */
    jugador->esta_caminando = false;
    jugador->esta_saltando = false;
    jugador->direccion = 1; /* Mira a la derecha por defecto */

    // El sprite de Snoopy mide 32 pero lo queremos a 64
    jugador->tam_x = 32.0;
    jugador->tam_y = 32.0;
    jugador->esc_x = 2.0;
    jugador->esc_y = 2.0;

    /* Cargar sprites */
    if (!jugador_cargar_sprites(jugador))
    {
        fprintf(stderr, "No se pudieron cargar los sprites.\n");
        return false;
    }

    return true;
}

/* Carga todos los sprites desde el disco */
bool jugador_cargar_sprites(struct Jugador *jugador)
{
    if (!jugador)
    {
        return false;
    }

    char ruta[256];
    bool exito = true;

    /* Cargar sprites de idle */
    jugador->num_sprites[ANIMACION_IDLE] = 1;
    snprintf(ruta, sizeof(ruta), "%s%s", RUTA_BASE, NOMBRES_IDLE[0]);
    jugador->sprites[ANIMACION_IDLE][0] = al_load_bitmap(ruta);
    if (!jugador->sprites[ANIMACION_IDLE][0])
    {
        fprintf(stderr, "Error cargando sprite idle: %s\n", ruta);
        exito = false;
    }

    /* Cargar sprites de caminar */
    int num_frames = sizeof(NOMBRES_CAMINAR) / sizeof(NOMBRES_CAMINAR[0]);
    jugador->num_sprites[ANIMACION_CAMINAR_DERECHA] = num_frames;
    jugador->num_sprites[ANIMACION_CAMINAR_IZQUIERDA] = num_frames;

    for (int i = 0; i < num_frames; i++)
    {
        snprintf(ruta, sizeof(ruta), "%s%s", RUTA_BASE, NOMBRES_CAMINAR[i]);
        jugador->sprites[ANIMACION_CAMINAR_DERECHA][i] = al_load_bitmap(ruta);
        if (!jugador->sprites[ANIMACION_CAMINAR_DERECHA][i])
        {
            fprintf(stderr, "Error cargando sprite caminar derecha frame %d: %s\n", i, ruta);
            exito = false;
        }

        /* Para izquierda, cargamos el mismo sprite (se puede reflejar despues) */
        jugador->sprites[ANIMACION_CAMINAR_IZQUIERDA][i] = al_load_bitmap(ruta);
    }

    /* Cargar sprite de salto (usamos cam.png por ahora) */
    jugador->num_sprites[ANIMACION_SALTAR] = 1;
    snprintf(ruta, sizeof(ruta), "%s%s", RUTA_BASE, NOMBRES_IDLE[0]);
    jugador->sprites[ANIMACION_SALTAR][0] = al_load_bitmap(ruta);
    if (!jugador->sprites[ANIMACION_SALTAR][0])
    {
        fprintf(stderr, "Error cargando sprite saltar: %s\n", ruta);
        exito = false;
    }

    return exito;
}

/* Actualiza la animacion del jugador basado en el tiempo transcurrido */
void jugador_actualizar(struct Jugador *jugador, double delta_tiempo)
{
    if (!jugador || jugador->num_sprites[jugador->animacion_actual] == 0)
    {
        return;
    }

    /* Actualizar estado de animacion basado en movimiento */
    if (jugador->esta_saltando)
    {
        jugador->animacion_actual = ANIMACION_SALTAR;
    }
    else if (jugador->esta_caminando)
    {
        if (jugador->direccion == 1)
        {
            jugador->animacion_actual = ANIMACION_CAMINAR_DERECHA;
        }
        else
        {
            jugador->animacion_actual = ANIMACION_CAMINAR_IZQUIERDA;
        }
    }
    else
    {
        jugador->animacion_actual = ANIMACION_IDLE;
    }

    /* Avanzar animacion si es necesario */
    if (jugador->num_sprites[jugador->animacion_actual] > 1)
    {
        jugador->tiempo_acumulado += delta_tiempo;

        if (jugador->tiempo_acumulado >= jugador->intervalo_animacion)
        {
            jugador->tiempo_acumulado -= jugador->intervalo_animacion;
            jugador->frame_actual++;

            if (jugador->frame_actual >= jugador->num_sprites[jugador->animacion_actual])
            {
                jugador->frame_actual = 0;
            }
        }
    }
    else
    {
        /* Animaciones de un solo frame siempre muestran frame 0 */
        jugador->frame_actual = 0;
    }
}

/* Configura si el jugador esta caminando y su direccion */
void jugador_set_estado_caminando(struct Jugador *jugador, bool caminando, int direccion)
{
    if (jugador)
    {
        jugador->esta_caminando = caminando;
        if (caminando)
        {
            jugador->direccion = (direccion > 0) ? 1 : -1;
        }
    }
}

/* Configura si el jugador esta saltando */
void jugador_set_estado_saltando(struct Jugador *jugador, bool saltando)
{
    if (jugador)
    {
        jugador->esta_saltando = saltando;
    }
}

/* Dibuja el jugador en la pantalla */
void jugador_dibujar(const struct Jugador *jugador)
{
    if (!jugador)
    {
        return;
    }

    ALLEGRO_BITMAP *sprite = jugador->sprites[jugador->animacion_actual][jugador->frame_actual];
    if (sprite)
    {
        /* Configurar transformacion para voltear sprite si mira a la izquierda */
        int flag = 0;
        if (jugador->direccion == -1)
        {
            flag = ALLEGRO_FLIP_HORIZONTAL;
        }

        al_draw_scaled_bitmap(sprite, 0, 0, jugador->tam_x, jugador->tam_y, jugador->pos_x, jugador->pos_y, jugador->tam_x * 2, jugador->tam_y * 2, flag);
    }
}

/* Libera todos los recursos del jugador */
void jugador_destruir(struct Jugador *jugador)
{
    if (!jugador)
    {
        return;
    }

    /* Destruir todos los sprites */
    for (int i = 0; i < JUGADOR_ANIMACIONES; i++)
    {
        for (int j = 0; j < JUGADOR_MAX_SPRITES; j++)
        {
            if (jugador->sprites[i][j])
            {
                al_destroy_bitmap(jugador->sprites[i][j]);
                jugador->sprites[i][j] = NULL;
            }
        }
    }

    memset(jugador, 0, sizeof(struct Jugador));
}
