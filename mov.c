#include <stdio.h>
#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>

ALLEGRO_DISPLAY *ventana;
ALLEGRO_EVENT_QUEUE *colEve;
ALLEGRO_EVENT eve;
ALLEGRO_COLOR fondo;
ALLEGRO_BITMAP *cam, *cam1, *cam2;
ALLEGRO_TIMER *temporizador;
bool fin = 0, cambio = 0;
int x;
int coordx, coordy;

void caminar(void)
{
    if (eve.keyboard.keycode == 83)
    {
        cambio = 1;
        al_destroy_bitmap(cam);
        al_draw_scaled_bitmap(cam1, 0, 0, 32, 32, 0, 0, 145, 145, 0);
        al_flip_display();
        coordx += 10;
        if (eve.keyboard.keycode == 83)
        {
            cambio = 1;
            al_destroy_bitmap(cam1);
            al_draw_scaled_bitmap(cam2, 0, 0, 32, 32, 0, 0, 145, 145, 0);
            coordx -= 10;
        }
        if (eve.keyboard.keycode == 82)
        {
            cambio = 1;
            al_destroy_bitmap(cam1);
            al_draw_scaled_bitmap(cam2, 0, 0, 32, 32, 0, 0, 145, 145, 0);
            coordx -= 10;
        }
        if (eve.keyboard.keycode == 84)
        {
            cambio = 1;
            al_destroy_bitmap(cam1);
            al_draw_scaled_bitmap(cam2, 0, 0, 32, 32, 0, 0, 145, 145, 0);
            coordy -= 10;
        }
        if (eve.keyboard.keycode == 85)
        {
            cambio = 1;
            al_destroy_bitmap(cam1);
            al_draw_scaled_bitmap(cam2, 0, 0, 32, 32, 0, 0, 145, 145, 0);
            coordy += 10;
        }
    }
    return;
}

int main(void)
{

    int fin = 0, cambio = 0, coordx, coordy;
    if (!al_init() || !al_install_keyboard())
    {
        printf("No se puede inciar");
        fin = 1;
    }
    if (!al_init_image_addon())
    {
        printf("No se pudo cargar la imagen");
        fin = 1;
    }
    cam = al_load_bitmap("images\\cam.png");
    cam1 = al_load_bitmap("images\\cam1.png");
    cam2 = al_load_bitmap("images\\cam2.png");
    if (!cam || !cam1 || !cam2)
    {
        printf("No se pudo visualizar la imagen");
        fin = 1;
    }
    if (!fin)
    {
        ventana = al_create_display(800, 600);
        al_set_window_title(ventana, "Proyecto");
        colEve = al_create_event_queue();
        // temporizador=al_create_timer(5);
        al_register_event_source(colEve, al_get_display_event_source(ventana));
        al_register_event_source(colEve, al_get_keyboard_event_source());
        // al_register_event_source(colEve,al_get_timer_event_source(temporizador));
        fondo = al_map_rgb(255, 255, 255);
        al_clear_to_color(fondo);
        while (!fin)
        {
            al_draw_scaled_bitmap(cam, 0, 0, 32, 32, 0, 0, 145, 145, 0);
            al_flip_display();
            al_wait_for_event(colEve, &eve);
            switch (eve.type)
            {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                fin = 1;
                printf("ventana cerrada");
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                if (eve.keyboard.keycode == 83)
                {
                    printf("Tecla abajo");
                    cambio = 1;
                    al_destroy_bitmap(cam);
                    al_draw_scaled_bitmap(cam1, 0, 0, 32, 32, 0, 0, 145, 145, 0);
                    al_flip_display();
                    coordx += 10;
                }
                break;
            }
        }
        al_destroy_display(ventana);
        al_destroy_event_queue(colEve);
        al_destroy_bitmap(cam);
        // al_destroy_timer(temporizador);
    }
    return 0;
}