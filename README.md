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

Compare the output of the above command to the image "anout.ppm". If both are identical in shape, the program is working perfectly!

# TO DO
1. **Z-Buffering**
  * Add support for it (as another option in face hiding). Requires changes in the raster and line drawing functions, in order to take the additional buffer into account at pixel level.
2. **Face filling**
  * Allow enable/disable. Receive the user's choice as another argument.
  * Break the `fill_face` function into several sub-functions.
3. **Illumination**
  * Add light sources support: *ambient*, *diffuse*, and *specular*.
  * Add illumination procedures: *at pixel level*, *Gouraud*, and *Phong*.
4. **Code cleaning and optimisations**
  * *Clean the code!* (Modular, self-contained design).
  * Provide a better input procedure (aside from command-line arguments).
  * Use a better OBJ parser (one made with `flex` and `bison` is on the way).
5. **Testing**
  * Test on low-memory conditions (heap allocation).
6. **Open source availability**
  * Adapt sources and executable to adhere with GNU GPL v3.