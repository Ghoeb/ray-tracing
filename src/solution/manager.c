#include "manager.h"
#include <math.h>
#include <stdio.h>

/** Inicializa y configura el administrador de triángulos de la escena */
Manager* manager_init(Scene* scene)
{
  Manager* manager = malloc(sizeof(Manager));

  #ifdef RENDER_WITH_KD
  manager -> myTree = kd_tree_make_root(scene);
  printf("Tree made!\n");
  #endif

  manager -> face_count = scene_get_triangle_count(scene);
  manager -> faces = scene_get_triangles(scene);

  return manager;
}

/** Encuentra el triangulo más cercano que intersecte con el rayo */
bool manager_get_closest_intersection(Manager* manager, Ray* ray)
{
  #ifdef RENDER_WITH_KD

  BoundingBox world_BB =
  (BoundingBox)
  {
    .minX = -1000000000,
    .maxX = 1000000000,
    .minY = -1000000000,
    .maxY = 1000000000,
    .minZ = -1000000000,
    .maxZ = 1000000000
  };

  return kd_node_intersect(
    ray, manager -> myTree, ray_get_origin(ray), world_BB);

  #else

  /* Inicialmente el triangulo guardado en el rayo es NULL */
  for(int i = 0; i < manager -> face_count; i++)
  {
    ray_intersect(ray, manager -> faces[i]);
  }

  /* Si luego de todo eso hay un triangulo guardado en el rayo*/
  if(ray_get_intersected_object(ray))
  {
    return true;
  }
  else
  {
    return false;
  }

  #endif
}

/** Libera todos los recursos asociados al administrador de triángulos */
void manager_destroy(Manager* manager)
{
  #ifdef RENDER_WITH_KD
    kd_root_destroy(manager -> myTree);
  #endif
  free(manager);
}
