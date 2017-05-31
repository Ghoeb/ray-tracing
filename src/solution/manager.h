#ifndef T2_LIB_STRUCTURE
#define T2_LIB_STRUCTURE

#include "scene.h"
#include "kdtrees.h"

/* TODO Debes modificar este struct para implementar tu estructura */
struct manager
{
  /** Actualmente la estructura simplemente copia el arreglo de triangulos */
  Triangle** faces;
  size_t face_count;

  #ifdef RENDER_WITH_KD
  KDTree *myTree;
  #endif
};
/** Representa la estructura encargada de administrar los triángulos */
typedef struct manager Manager;

/** Inicializa y configura el administrador de triángulos de la escena */
Manager* manager_init(Scene* scene);

/** Encuentra el triangulo más cercano que intersecte con el rayo */
bool manager_get_closest_intersection(Manager* manager, Ray* ray);

/** Libera todos los recursos asociados al administrador de triángulos */
void manager_destroy(Manager* manager);

#endif /* end of include guard: T2_LIB_STRUCTURE */
