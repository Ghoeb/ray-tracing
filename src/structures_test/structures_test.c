#include <stdlib.h>
#include "../solution/manager.h"
#include "../modules/scene.h"
#include <stdio.h>
#include <time.h>

char* axis_name(Axis ax)
{
  switch (ax)
  {
    case XD: return "X";
    case YD: return "Y";
    case ZD: return "Z";
  }
  return NULL;
}

#ifdef TEST_KDTREE
/** Entrega el conjunto de puntos que contiene este sub-arbol, guardando el largo en count */
/** Si en count se guarda -1 significa que hubo un error */
Vector* kd_tree_test(KDTree* kdtree, int* count)
{
  /* Si estamos en una hoja simplemente */
  if(kdtree -> is_leaf)
  {
    /* Entregamos los puntos de esta hoja a su padre */
    *count = kdtree -> point_count;
    return kdtree -> points;
  }
  /** En caso de que sea un arbol, hay que revisar que sus hijos cumplan la condicion de kdtree */
  else
  {
    /* Tomamos los puntos a la izquierda */
    int left_count = 0;
    Vector* left_points = kd_tree_test(kdtree -> leftSon, &left_count);

    /* Si  marcó error, entonces lo notificamos */
    if(left_count < 0)
    {
      printf("Error\n");
      exit(1);
    }

    /** Revisamos que todos los puntos del arreglo sean menores que el eje */
    for(int i = 0; i < left_count; i++)
    {
      Vector lesser_point = left_points[i];

      /* Indica que pasó la prueba*/
      bool pass = false;

      switch (kdtree -> plane_axis)
      {
        case XD:
          if(lesser_point.X <= kdtree -> plane_position) pass = true;
        break;
        case YD:
          if(lesser_point.Y <= kdtree -> plane_position) pass = true;
        break;
        case ZD:
          if(lesser_point.Z <= kdtree -> plane_position) pass = true;
        break;
      }

      if(!pass)
      {
        printf("Error! El punto %f \t%f \t%f ",lesser_point.X, lesser_point.Y,lesser_point.Z );
        printf("\tdice estar a la izquierda del plano %s %f\n", axis_name(kdtree -> plane_axis), kdtree ->plane_position);
        *count = -1;
        return NULL;
      }
      else
      {
        printf("El punto %f \t%f \t%f ",lesser_point.X, lesser_point.Y,lesser_point.Z );
        printf("\tefectivamente esta a la izquierda del plano %s %f\n", axis_name(kdtree -> plane_axis), kdtree ->plane_position);;
      }
    }

    /* Y los puntos a la derecha */
    int right_count = 0;
    Vector* right_points = kd_tree_test(kdtree -> rightSon,&right_count);

    /* Si marcó error, entonces lo notificamos */
    if(right_count < 0)
    {
      printf("Error\n");
      exit(1);
    }

    for(int i = 0; i < right_count; i++)
    {
      Vector greater_point = right_points[i];

      bool pass = false;

      switch (kdtree -> plane_axis)
      {
        case XD:
          if(greater_point.X >= kdtree -> plane_position) pass = true;
        break;
        case YD:
          if(greater_point.Y >= kdtree -> plane_position) pass = true;
        break;
        case ZD:
          if(greater_point.Z >= kdtree -> plane_position) pass = true;
        break;
      }
      if(!pass)
      {
        printf("Error!\n El punto %f \t%f \t%f ",greater_point.X, greater_point.Y,greater_point.Z );
        printf("\tdice estar a la derecha del plano %s %f\n", axis_name(kdtree -> plane_axis), kdtree ->plane_position);
        *count = -1;
        return NULL;
      }
      else
      {
        printf("El punto %f \t%f \t%f ",greater_point.X, greater_point.Y,greater_point.Z );
        printf("\tefectivamente esta a la derecha del plano %s %f\n", axis_name(kdtree -> plane_axis), kdtree ->plane_position);;
      }
    }

    /* Ya que no hubo errores, combinamos los arreglos para que se comparen con la siguiente division */

    /* Si los dos arreglos tienen 0 elementos, significa que hay un nodo con dos hijos sin puntos */
    if(!left_count && !right_count)
    {
      printf("Hermanos nulos\n");
      *count = 0;
      return NULL;
    }
    /* Si no hay elementos a la izquierda */
    else if(!left_count)
    {
      *count = right_count;
      return right_points;
    }
    /* Si no hay elementos a la derecha */
    else if(!right_count)
    {
      *count = left_count;
      return left_points;
    }
    else
    {
      Vector* mypoints = malloc(sizeof(Vector)* (left_count + right_count));
      for(int i = 0; i < left_count; i++)
      {
        mypoints[i] = left_points[i];
      }

      for(int i = 0; i < right_count; i++)
      {
        mypoints[left_count+i] = right_points[i];
      }

      *count = left_count+right_count;
      return mypoints;
    }
  }

}

#endif

int main(int argc, char *argv[])
{
    /** Cargamos la escena */
    Scene* scene = scene_load(argv[1]);

    if(!scene) return 1;

    KDTree *kd_we_will_traverse = kd_tree_make_root(scene);

    Ray aRay;

    aRay.closestDistance = 1000000000;
    aRay.closestObject = NULL;
    aRay.direction = (Vector){.X = 0.f, .Y = 0.f, .Z = -1.f};
    aRay.position = (Vector){.X = 0.f, .Y = 0.f, .Z = 10.f};

    BoundingBox initialBB =
    (BoundingBox)
    {
      .minX = -1000000000,
      .maxX = 1000000000,
      .minY = -1000000000,
      .maxY = 1000000000,
      .minZ = -1000000000,
      .maxZ = 1000000000
    };

    kd_node_intersect(
      &aRay,
      kd_we_will_traverse,
      aRay.position,
      initialBB);

    kd_tree_destroy(kd_we_will_traverse);

    return 0;

    int numTests = 1000;

    time_t t = clock();

    for(int i = 0; i < numTests; i++)
    {
      KDTree *kdtree = kd_tree_make_root(scene);
      kd_tree_destroy(kdtree);
    }

    t = clock() - t;
    printf ("It took me %zu clicks (%f seconds).\n", t, ((float)t)/CLOCKS_PER_SEC);
    return 0;

    /** Testeando el KD Tree... */
    printf("%s\n", "Creando el KD Tree...");
    KDTree *kdtree = kd_tree_make_root(scene);
    printf("%s\n", "KD Tree creado!");

    #ifdef TEST_KDTREE
    int count = 0;
    Vector* result = kd_tree_test(kdtree, &count);

    if(result)
    {
      printf("COOL!\n");
    }
    #endif

    printf("%s\n", "Destruyendo el KD Tree...");
    kd_tree_destroy(kdtree);
    printf("%s\n", "KD Tree destruido!");


    /** Inicializamos la estructura encargada de manejarlos */
    // Manager* man = manager_init(scene);

    /** Liberamos la memoria */
    // manager_destroy(man);
    scene_destroy(scene);

    return 0;
}
