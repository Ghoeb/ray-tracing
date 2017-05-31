#include "kdtrees.h"

#ifdef DEBUG_KDTREE

size_t current_depth = 0;

#endif

int tri_compare_to_plane(Triangle* tri, Axis plane_axis, float plane_position)
{
  Vector v1,v2,v3;
  triangle_get_vertices(tri, &v1, &v2, &v3);

  bool
  is_behind_p1 = is_behind_plane(v1, plane_axis, plane_position),

  is_behind_p2 = is_behind_plane(v2, plane_axis, plane_position),

  is_behind_p3 = is_behind_plane(v3, plane_axis, plane_position);

  if(is_behind_p1 && is_behind_p2 && is_behind_p3)
    return -1;
  else if(is_behind_p1 || is_behind_p2 || is_behind_p3)
    return 0;
  else
    return 1;
}

bool point_is_less_than(Vector p1, Vector p2, Axis comparison_axis)
{
  switch (comparison_axis) {
    case XD:
    return p1.X < p2.X;

    case YD:
    return p1.Y < p2.Y;

    case ZD:
    return p1.Z < p2.Z;

    default:

    printf("%s\n", "WHAT IS HAPPENING. CANNOT COMPARE POINTS IN THIS WEIRD PLANE.");
    printf("%s%i\n", "CURRENT PLANE IS: ", comparison_axis);

    return (bool)"wololo";
  }

}

size_t points_medianPartition(Vector *points, size_t start, size_t end, size_t pivotIdx, Axis comparison_axis)
{
  Vector pivot = points[pivotIdx];

  points[pivotIdx] = points[end];
  points[end] = pivot;

  size_t storeIdx = start;

  for(size_t i = start; i < end; i++)
  {
    if(point_is_less_than(points[i], pivot, comparison_axis))
    {
      Vector aux = points[storeIdx];
      points[storeIdx] = points[i];
      points[i] = aux;

      storeIdx++;
    }
  }

  Vector aux = points[end];
  points[end] = points[storeIdx];
  points[storeIdx] = aux;

  return storeIdx;
}

// Obtenido de https://en.wikipedia.org/wiki/Quickselect
size_t points_goodMedian(Vector *points, size_t start, size_t end, size_t n, Axis comparison_axis)
{
  if(start == end)
    return start;

  size_t pivot_index = points_medianPartition
  (points, start, end, start + 1, comparison_axis);

  if(n == pivot_index)
    return n;
  else if(n < pivot_index)
    return
    points_goodMedian(points, start, pivot_index - 1, n, comparison_axis);
  else
    return
    points_goodMedian(points, pivot_index + 1, end, n, comparison_axis);
}

KDTree *kd_tree_make_wTris(
  Vector *points, size_t first_point, size_t last_point,
  Axis current_axis, Triangle **tris_in_node, size_t number_of_tris)
{
    KDTree *new_tree = malloc(sizeof(KDTree));

    // El nodo es hoja y debiera encontrar sus triángulos
    if(number_of_tris <= MAX_TRI_COUNT || last_point + 1 - first_point <= MAX_VERT_COUNT)
    {
      // printf(
      //   "Making leaf! #Tris: %zu, #points: %zu\n",
      //   number_of_tris,
      //   last_point - first_point + 1);

      KDTree *leaf = new_tree;

      leaf -> leaf_tris = tris_in_node;

      leaf -> is_leaf = true;

      leaf -> tris_count = number_of_tris;

      return leaf;
    }
    // El nodo es rama.
    // Debiera buscar su mediana, luego armar sus hijos,
    // luego actualizar sus valores y finalmente retornarse.
    else
    {
      KDTree *branch = new_tree;

      Axis next_axis = get_next_axis(current_axis);

      size_t median_pos = (last_point - first_point) / 2 + first_point;

      size_t median_idx = points_goodMedian
      (points, first_point, last_point, median_pos, current_axis);

      // Para dejar algo a la derecha
      if(median_idx == last_point)
      {
        //median_idx = last_point - 1;
      }

      float plane_position;

      switch (current_axis) {
        case XD:
        plane_position = points[median_idx].X;
        break;

        case YD:
        plane_position = points[median_idx].Y;
        break;

        case ZD:
        plane_position = points[median_idx].Z;
        break;

        default:
        plane_position = INFINITY;
        printf("%s\n",
        "WHY GOD WHY. PLANE IS NOT X, NOR Y NOR Z. THE 4TH DIMENSION HAS OVERRUN US!");
      }

      branch -> plane_axis = current_axis;
      branch -> plane_position = plane_position;

      // Preparamos los arreglos de triángulos de los hijos
      Triangle
      **leftTris = malloc(sizeof(Triangle*) * number_of_tris),
      **rightTris = malloc(sizeof(Triangle*) * number_of_tris);

      size_t number_of_lefties = 0, number_of_righties = 0;

      size_t number_of_bothies = 0;

      for(size_t i = 0; i < number_of_tris; i++)
      {
        Triangle *current_tri = tris_in_node[i];

        int position_of_tri =
        tri_compare_to_plane(current_tri, current_axis, plane_position);

        if(position_of_tri <= 0)
        {
          leftTris[number_of_lefties] = current_tri;
          number_of_lefties++;
        }
        if(position_of_tri >= 0)
        {
          rightTris[number_of_righties] = current_tri;
          number_of_righties++;
        }
        if(position_of_tri == 0) number_of_bothies++;
      }

      branch -> leaf_tris = tris_in_node;

      branch -> leftSon = kd_tree_make_wTris(
        points, first_point, median_idx,
        next_axis, leftTris, number_of_lefties);

      branch -> rightSon = kd_tree_make_wTris(
        points, median_idx + 1, last_point,
        next_axis, rightTris, number_of_righties);

      branch -> tris_count =
      branch -> leftSon -> tris_count +
      branch -> rightSon -> tris_count -
      number_of_bothies;

      branch -> is_leaf = false;

      // printf(
      //   "Tris entering: %zu, Tris saved: %zu\n",
      //   number_of_tris, branch -> tris_count);

      return branch;
    }
  }

KDTree *kd_tree_make_dummy(Scene *scene)
{
  KDTree *dummy = malloc(sizeof(KDTree));

  dummy -> is_leaf = true;

  dummy -> leaf_tris = scene_get_triangles(scene);

  dummy -> tris_count = scene_get_triangle_count(scene);

  return dummy;
}


KDTree *kd_tree_make_root(Scene *scene)
{
  //return kd_tree_make_dummy(scene);

  return kd_tree_make_wTris(
    scene_get_points(scene),
    0,
    scene_get_point_count(scene) - 1,
    XD,
    scene_get_triangles(scene),
    scene_get_triangle_count(scene));
}

void kd_root_destroy(KDTree *kdtree)
{
  if(!(kdtree -> is_leaf))
  {
    kd_tree_destroy(kdtree -> leftSon);
    kd_tree_destroy(kdtree -> rightSon);
  }

  free(kdtree);
}

void kd_tree_destroy(KDTree *kdtree)
{
  if(kdtree -> is_leaf)
  {
    //printf("Destroying a leaf... #tris: %zu\n", kdtree -> tris_count);
    if(kdtree -> leaf_tris)
    {
      //printf("Freeing the leaf triangles...\n");
      free(kdtree -> leaf_tris);
    }
    else
      printf("Found a leaf with null tris... what!\n");
    //printf("Freeing the tree...\n");
    free(kdtree);
  }
  else
  {
    free(kdtree -> leaf_tris);
    // printf("Destroying a branch...\n");
    kd_tree_destroy(kdtree -> leftSon);
    kd_tree_destroy(kdtree -> rightSon);

    free(kdtree);
  }

}

/////////////////////////////////////////////////////////////////////////
/////////////// Parte que sirve para recorrer el KD Tree ////////////////
/////////////////////////////////////////////////////////////////////////

// Para evitar errores de comparación entre plano y vector
#define COMP_ERROR 0.0001f
#define ANTI_ERROR_EXPANSION_OF_BB

bool is_inside_BB(Vector position, BoundingBox bounding_box)
{
  #ifdef ANTI_ERROR_EXPANSION_OF_BB
  return
  (
    position.X + COMP_ERROR >= bounding_box.minX &&
    position.X - COMP_ERROR <= bounding_box.maxX &&
    position.Y + COMP_ERROR >= bounding_box.minY &&
    position.Y - COMP_ERROR <= bounding_box.maxY &&
    position.Z + COMP_ERROR >= bounding_box.minZ &&
    position.Z - COMP_ERROR <= bounding_box.maxZ
  );
  #else
  return
  (
    position.X >= bounding_box.minX &&
    position.X <= bounding_box.maxX &&
    position.Y >= bounding_box.minY &&
    position.Y <= bounding_box.maxY &&
    position.Z >= bounding_box.minZ &&
    position.Z <= bounding_box.maxZ
  );
  #endif
}

bool is_behind_plane(Vector position, Axis plane_axis, float plane_position)
{
  switch (plane_axis) {
    case XD:
    return position.X <= plane_position;
    break;
    case YD:
    return position.Y <= plane_position;
    break;
    case ZD:
    return position.Z <= plane_position;
    break;

    default:
    printf("ERROR: plane_axis no es X, Y ni Z. Es: %i\n", plane_axis);
    return true;
  }
}

float intersect_plane(Axis plane_axis, Ray *ray, float plane_position)
{
  float ray_position, ray_direction;

  switch (plane_axis) {
    case XD:
    ray_position = ray_get_origin(ray).X;
    ray_direction = ray_get_direction(ray).X;
    break;

    case YD:
    ray_position = ray_get_origin(ray).Y;
    ray_direction = ray_get_direction(ray).Y;
    break;

    case ZD:
    ray_position = ray_get_origin(ray).Z;
    ray_direction = ray_get_direction(ray).Z;
    break;

    default:
    printf("ERROR: plane_axis no es X, Y ni Z. Es: %i\n", plane_axis);
    return INFINITY;
  }

  if(ray_direction == 0)
    return INFINITY;

  return (plane_position - ray_position) / ray_direction;
}

void divide_BB(
  BoundingBox *leftBB, BoundingBox *rightBB,
  Axis dividing_axis, float plane_position)
{
  switch (dividing_axis) {
    case XD:
    leftBB -> maxX = plane_position;
    rightBB -> minX = plane_position;
    break;

    case YD:
    leftBB -> maxY = plane_position;
    rightBB -> minY = plane_position;
    break;

    case ZD:
    leftBB -> maxZ = plane_position;
    rightBB -> minZ = plane_position;
    break;
  }
}

bool kd_node_intersect(
  Ray *ray, KDTree *kdtree, Vector current_position, BoundingBox bounding_box)
{
  if(kdtree -> is_leaf)
  {
    #ifdef DEBUG_TRAVERSE
    printf("Trying to intersect a leaf! Tri count: %zu\n", kdtree -> tris_count);
    #endif

    /* Inicialmente el triangulo guardado en el rayo es NULL */
    for(int i = 0; i < kdtree -> tris_count; i++)
    {
      ray_intersect(ray, kdtree -> leaf_tris[i]);
    }

    // Position of intersection with closest triangle
    Vector inter_pos = ray_get_intersection_point(ray);

    // If the point of intersection lies within the node's region, then
    // we know we've found the closest triangle that intersects the ray
    return is_inside_BB(inter_pos, bounding_box);
  }

  #ifdef DEBUG_TRAVERSE
  printf("Opening a branch! Tri count: %zu\n", kdtree -> tris_count);
  #endif

  Axis plane_axis = kdtree -> plane_axis;
  float plane_position = kdtree -> plane_position;

  bool isBehind = is_behind_plane(
    current_position, plane_axis, plane_position);

  KDTree *near, *far;

  BoundingBox near_bb = bounding_box, far_bb = bounding_box;

  if(isBehind)
  {
    near = kdtree -> leftSon;
    far = kdtree -> rightSon;

    divide_BB(&near_bb, &far_bb, plane_axis, plane_position);
  }
  else
  {
    near = kdtree -> rightSon;
    far = kdtree -> leftSon;

    divide_BB(&far_bb, &near_bb, plane_axis, plane_position);
  }

  #ifdef DEBUG_TRAVERSE
  printf("Near and far were set-up!");
  if(near != NULL)
  {
    printf("Near count: %zu.", near -> tris_count);
  }
  if(far != NULL)
  {
    printf("Far count: %zu.\n", far -> tris_count);
  }
  printf("Intersecting the branches!\n");
  #endif

  // Intersectamos con el near primero.
  // El near siempre estará a nuestro alcance.
  if(near && kd_node_intersect(ray, near, current_position, near_bb)) return true;

  // Si el near nos falla quizás podemos intersectar con el far.
  // El far no siempre estará a nuestro alcance (puede que al tocar
  // el plano ya nos salgamos del nodo), así que primero probamos.
  float distance_to_far = intersect_plane(plane_axis, ray, plane_position);

  // Puede que el far esté detrás del origen del rayo.
  // En ese caso, es inalcanzable.
  //if(distance_to_far <= 0) return false;

  // Puede que el far sea alcanzable sólo en el infinito.
  // Con esto sabremos qué pasa
  if(distance_to_far == INFINITY) {printf("!!! Far está en el infinito\n");}

  // Original. En torus esto tiene 2 puntos de artefactos
  // float distanceDelta = 0.001;

  // En torus esto aumenta los artefactos
  // float distanceDelta = 0.004;

  // Esto ya no genera artefactos en torus
  // Ni en ningún easy
  // Ni en billiards, cornellbox, ni infinicandy
  float distanceDelta = 0.0005;

  // Primer punto que visitaríamos dentro del far.
  // Estará dentro de los límites?
  Vector far_start = vector_added_v(
    ray_get_origin(ray), vector_multiplied_f(
      ray_get_direction(ray), distance_to_far + distanceDelta ));

  // Si avanzando hasta allá nos encontramos fuera de los
  // límites del nodo Far, entonces no lo nos sirve visitarlo.
  if(!is_inside_BB(far_start, far_bb)) return false;

  //printf("Trying to intersect far!\n");

  return far && kd_node_intersect(ray, far, far_start, far_bb);
}
