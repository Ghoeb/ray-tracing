# Raytracing
Repository for experimenting the effect of hierarchical data structures on raytracing

Prepared as the base code for an assignment on the Data Structures and Algorithms course at PUC

The raytracer engine consists of the following:

* .obj & .mtl compatibility, much like the ones used by blender
* Real-time visualization of the rendering using GTK+3 & Cairo
* Texture mapping
* Normal mapping
* Reflection

Vector library and KD-Tree implemented by [Félix Fischer](https://github.com/felix91gr)

## Dependencies

This project depends on GTK+ 3.0

## Running the program

Compile with

```
make
```
and run the program with

```
./raytracer <scene> <antialiasing factor> [-f]
```

where 

`<scene>` is the scene file. Sample scenes can be found at the scenes folder

`<antialiasing factor>` if given a number n it wil cast n^2 rays per pixel

`[-f]` forces flat faces on all meshes
