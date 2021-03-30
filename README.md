# Shader Programming Exercises

assignments for SPG (shader programming) @FHTechnikumWien

## Key Controls

| key        | action |
| :------------- | :----- |
| p      | toggle wireframe mode |
| r      | rotate displacement tile |
| Cursor UP, DOWN      | change offset for 3D texture and generate new geometry |
| +, -      | |
| Q, E      | |
| W, A, S, D      | controls to move the camera, look around with mouse |

## MarchingCubes

![Screenshot](marchingCubes.gif "marchingcubes")

### References

- [MARCHING CUBES: A HIGH RESOLUTION
3D SURFACE CONSTRUCTION ALGORITHM paper Siggraph 1987 [.pdf]](https://people.eecs.berkeley.edu/~jrs/meshpapers/LorensenCline.pdf)
- [Polygonising a scalar field by Paul Bourke](http://paulbourke.net/geometry/polygonise/)

## Pixel Shader Displacement Mapping

![displacement tile](displacementTile.png "displacement tile")

### References

- [LearnOpenGL Parallax Mapping](https://learnopengl.com/Advanced-Lighting/Parallax-Mapping)

## Dependencies

- glad (glad.c need to be in source folder and compiled)
- glew
- glfw
- glm
- KHR
- stb (only stb_image.h needed)
