#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "jugador.h"

#define VELOCIDAD_MOVIMIENTO 200.0f /* pixeles por segundo */
#define VELOCIDAD_SALTO 400.0f      /* pixeles por segundo */
#define GRAVEDAD 800.0f             /* aceleracion hacia abajo */
#define ALTURA_SUELO 510.0f         /* Y donde esta el suelo */
#define WIDTH 800
#define HEIGHT 600

/* ENEMIGOS - spaghetti code incoming */
#define MAX_ENEMIGOS 10
#define ENEMIGO_RADIUS 15.0f
#define ENEMIGO_VELOCIDAD 50.0f
#define ENEMIGO_DAMAGE_JUMP 200.0f
#define VIDA_MAX 3
#define ENEMIGO_DAMAGE 1

typedef struct
{
    float x, y;
    float velocidad;
    bool vivo;
} Enemigo;

Enemigo enemigos[MAX_ENEMIGOS];

int main(void)
{
    if (!al_init() || !al_init_primitives_addon() || !al_install_keyboard() || !al_init_image_addon())
    {
        fprintf(stderr, "No se pudo iniciar Allegro o sus addons.\n");
        return 1;
    }

    if (!al_init_font_addon() || !al_init_ttf_addon())
    {
        fprintf(stderr, "No se pudo cargar el addon de texto");
        return 1;
    }

    ALLEGRO_DISPLAY *ventana = al_create_display(WIDTH, HEIGHT);
    if (!ventana)
    {
        fprintf(stderr, "No se pudo crear la ventana.\n");
        return 1;
    }
    al_set_window_title(ventana, "Snoopylote");

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

    ALLEGRO_FONT *letra = al_load_ttf_font("src/fonts/monogram.ttf", 48, 0);
    if (!letra)
    {
        fprintf(stderr, "No se pudo cargar la fuente TTF.\n");
        al_destroy_event_queue(colaeventos);
        al_destroy_display(ventana);
        return 1;
    }

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
        al_destroy_font(letra);
        al_destroy_event_queue(colaeventos);
        al_destroy_display(ventana);
        return 1;
    }

    /* Variables de fisica */
    float velocidad_y = 0.0f;
    bool en_suelo = true;
    bool saltando = false;
    int vida = VIDA_MAX;

    /* Inicializar enemigos - spaghetti style */
    enemigos[0].x = 200.0f;
    enemigos[0].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[0].velocidad = ENEMIGO_VELOCIDAD;
    enemigos[0].vivo = true;
    enemigos[1].x = 400.0f;
    enemigos[1].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[1].velocidad = -ENEMIGO_VELOCIDAD;
    enemigos[1].vivo = true;
    enemigos[2].x = 600.0f;
    enemigos[2].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[2].velocidad = ENEMIGO_VELOCIDAD;
    enemigos[2].vivo = true;
    enemigos[3].x = 300.0f;
    enemigos[3].y = ALTURA_SUELO - ENEMIGO_RADIUS - 50.0f;
    enemigos[3].velocidad = -ENEMIGO_VELOCIDAD * 0.7f;
    enemigos[3].vivo = true;
    enemigos[4].x = 700.0f;
    enemigos[4].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[4].velocidad = -ENEMIGO_VELOCIDAD;
    enemigos[4].vivo = true;
    enemigos[5].x = 150.0f;
    enemigos[5].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[5].velocidad = ENEMIGO_VELOCIDAD * 0.8f;
    enemigos[5].vivo = true;
    enemigos[6].x = 500.0f;
    enemigos[6].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[6].velocidad = -ENEMIGO_VELOCIDAD * 1.2f;
    enemigos[6].vivo = true;
    enemigos[7].x = 650.0f;
    enemigos[7].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[7].velocidad = ENEMIGO_VELOCIDAD;
    enemigos[7].vivo = true;
    enemigos[8].x = 100.0f;
    enemigos[8].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[8].velocidad = -ENEMIGO_VELOCIDAD * 0.6f;
    enemigos[8].vivo = true;
    enemigos[9].x = 750.0f;
    enemigos[9].y = ALTURA_SUELO - ENEMIGO_RADIUS;
    enemigos[9].velocidad = -ENEMIGO_VELOCIDAD * 1.5f;
    enemigos[9].vivo = true;

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

        /* Mover enemigos - spaghetti logic */
        for (int i = 0; i < MAX_ENEMIGOS; i++)
        {
            if (enemigos[i].vivo)
            {
                enemigos[i].x += enemigos[i].velocidad * delta_tiempo;
                /* Bounce off walls */
                if (enemigos[i].x < ENEMIGO_RADIUS || enemigos[i].x > WIDTH - ENEMIGO_RADIUS)
                {
                    enemigos[i].velocidad *= -1;
                }
            }
        }

        /* Colisiones jugador-enemigo - box check spaghetti */
        for (int i = 0; i < MAX_ENEMIGOS; i++)
        {
            if (!enemigos[i].vivo)
                continue;

            /* Box check simple */
            float player_left = snoopy1.pos_x;
            float player_right = snoopy1.pos_x + 32.0f;
            float player_top = snoopy1.pos_y;
            float player_bottom = snoopy1.pos_y + 32.0f;
            float enemy_left = enemigos[i].x - ENEMIGO_RADIUS;
            float enemy_right = enemigos[i].x + ENEMIGO_RADIUS;
            float enemy_top = enemigos[i].y - ENEMIGO_RADIUS;
            float enemy_bottom = enemigos[i].y + ENEMIGO_RADIUS;

            bool colision = (player_left < enemy_right && player_right > enemy_left &&
                             player_top < enemy_bottom && player_bottom > enemy_top);

            if (colision)
            {
                /* Mario style: jump on head = enemy dies + player bounces */
                if (velocidad_y > 0 && player_bottom < enemigos[i].y + 10) /* Cayendo y arriba del enemigo */
                {
                    enemigos[i].vivo = false;
                    velocidad_y = -ENEMIGO_DAMAGE_JUMP;
                }
                else
                {
                    /* Player takes damage - push back */
                    if (vida > 0)
                    {
                        vida -= ENEMIGO_DAMAGE;
                        if (snoopy1.pos_x < enemigos[i].x)
                        {
                            snoopy1.pos_x -= 30.0f;
                        }
                        else
                        {
                            snoopy1.pos_x += 30.0f;
                        }
                        velocidad_y = -100.0f;
                    }
                }
            }
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

        /* Dibujar vida - spaghetti hearts */

        al_draw_textf(letra, al_map_rgb(255, 255, 255), 25, 25, 0, "VIDA: %d/%d", vida, VIDA_MAX);

        /* Dibujar jugador */
        jugador_dibujar(&snoopy1);

        /* Dibujar enemigos - circles spaghetti */
        for (int i = 0; i < MAX_ENEMIGOS; i++)
        {
            if (enemigos[i].vivo)
            {
                al_draw_filled_circle(enemigos[i].x, enemigos[i].y, ENEMIGO_RADIUS, al_map_rgb(32, 216, 64));
                al_draw_circle(enemigos[i].x, enemigos[i].y, ENEMIGO_RADIUS, al_map_rgb(128, 0, 0), 2);
            }
        }

        al_flip_display();
    }

    /* Limpieza */
    al_stop_timer(timer);
    al_destroy_timer(timer);
    jugador_destruir(&snoopy1);
    al_destroy_font(letra);
    al_destroy_event_queue(colaeventos);
    al_destroy_display(ventana);
    return 0;
}
