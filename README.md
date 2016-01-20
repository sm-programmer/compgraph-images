# compgraph-images
Computer Graphics program that draws 3D objects over a PPM canvas.

# Usage
`./main obj-file sx sy sz tx ty tz rx ry rz focal-dist hiding`

Where:

* **obj-file** is the name of a simplified Wavefront OBJ file used as image input.
* **sx**, **sy** and **sz** are scaling factors (one for each axis) represented as real numbers.
* **tx**, **ty** and **tz** are translation factors (one for each axis), also represented as real numbers.
* **rx**, **ry** and **rz** are rotation factors (one for each axis), representing degrees.
* **focal-dist** is the focal distance used to project the object over the canvas, represented as a real number.
* **hiding** is an integer that indicates the hiding mode in use:
  * *0* doesn't apply any face hiding
  * *1* hides faces using the *vector normals* approach.

# Example
This example uses a processed OBJ file kindly received from professor Rafael Norman Saucedo Delgado.

`./main who.is.that.pokemon.obj 4.0 4.0 4.0 0.0 0.0 400.0 0.0 180.0 0.0 300.0 1 > image.ppm`

Compare the output of the above command to the image "anout.ppm". If both are identical, the program is working perfectly!

# TODO
1. Add support for *z-buffering* as another option in face hiding.
2. Allow to enable/disable face filling.
3. Add light support (Gouraud, Phong).
4. *Clean the code up!*
5. Conform all the sources to GNU GPL v3.
6. Provide a better input procedure.