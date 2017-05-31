#ifndef T2_LIB_RENDERER
#define T2_LIB_RENDERER

#include "../solution/manager.h"
#include <stdint.h>
#include <cairo.h>

#define STACK_LIMIT 8

/** Indica que la ventana está abierta */
bool window_open;

/** Factor de antialiasing. Determina la cantidad de muestras por píxel */
uint8_t antialiasing_factor;

/** Establece los parámetros de rendereo, retorna false en caso de fallar */
bool check_parameters(int argc, char** argv);

/** Renderea los triángulos sobre la imagen usando el manager */
void render(Scene* scene, Manager* manager, cairo_surface_t* image);

#endif /* end of include guard: T2_LIB_RENDERER */
