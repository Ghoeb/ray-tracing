#include "scene.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
// #define DEBUG_KDTREE

//#define TEST_KDTREE

// #define DEBUG_TRAVERSE
#define RENDER_WITH_KD

enum k_dim { XD, YD, ZD };

/** Representa uno de los ejes coordenados */
typedef enum k_dim Axis;

// Cantidad de tris en un nodo hoja
#define MAX_VERT_COUNT 2
#define MAX_TRI_COUNT 16

// Avanza al siguiente eje
#define get_next_axis(axis) ((axis + 1) % 3)

struct kd_tree
{
  /** Hijo #1 del KD Tree */
  struct kd_tree *leftSon;
  /** Hijo #2 del KD Tree */
  struct kd_tree *rightSon;

  /** Eje del plano divisor del KD Tree */
  Axis plane_axis;
  /** Posición del plano divisor en el eje correspondiente */
  float plane_position;

  /** Número de triángulos bajo esta rama / hoja */
  size_t tris_count;

  /** Triángulos de la hoja. Es NULL si el nodo es una rama. */
  Triangle **leaf_tris;

  /** Señala si el nodo es una hoja del árbol */
  bool is_leaf;
};

/** Estructura que particiona el espacio y permite recorrerlo inteligentemente */
typedef struct kd_tree KDTree;

/** Compara 2 puntos en un eje coordenado */
bool point_is_less_than(Vector p1, Vector p2, Axis comparison_axis);

/** Revisa si un triángulo está completamente bajo un plano.
    Retorna -1 si es verdad, 1 si es lo contrario, y 0 si está arriba
    y abajo (lo cruza). */
int tri_compare_to_plane(Triangle* tri, Axis plane_axis, float plane_position);

KDTree *kd_tree_make_root(Scene *scene);

KDTree *kd_tree_make(
  Vector *scene_points, size_t first_point, size_t last_point, Axis current_axis);

KDTree *kd_tree_make_wTris(
  Vector *points, size_t first_point, size_t last_point,
  Axis current_axis, Triangle **tris_in_node, size_t number_of_tris);

void kd_root_destroy(KDTree *kdtree);
void kd_tree_destroy(KDTree *kdtree);

/////////////////////////////////////////////////////////////////////////
/////////////// Parte que sirve para recorrer el KD Tree ////////////////
/////////////////////////////////////////////////////////////////////////

// Axis alligned Bounding Box
struct axis_alligned_BB
{
  float minX;
  float minY;
  float minZ;
  float maxX;
  float maxY;
  float maxZ;
};

typedef struct axis_alligned_BB BoundingBox;

bool kd_node_intersect(
  Ray *ray, KDTree *kdtree, Vector current_position, BoundingBox bounding_box);

bool is_inside_BB(Vector position, BoundingBox bounding_box);

bool is_behind_plane(Vector position, Axis plane_axis, float plane_position);

void divide_BB(
  BoundingBox *leftBB, BoundingBox *rightBB,
  Axis dividing_axis, float plane_position);

// Retorna la distancia del rayo al plano. Puede ser negativa.
float intersect_plane(Axis plane_axis, Ray *ray, float plane_position);
