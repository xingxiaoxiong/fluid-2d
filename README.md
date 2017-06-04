### Intruduction
- This is a an implementation of the article [*Fast Fluid Dynamics Simulation on the GPU*](https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch38.html) on GPU Gems.
- There is a [YouTube video](https://youtu.be/TiUO4fzy4fo) demonstrating the use of this program.

### Compile
- This program has been tested with VS 2015 on Win10 64bit. To compile it, you need the following libraries:
    - GLM(https://github.com/g-truc/glm)
    - glew(http://glew.sourceforge.net/)
    - glfw(http://www.glfw.org/)

### Issues
At Section Force Application in the article, the equation used to calculate the additive velocity due to external forces is wrong. Using the original equation will result in divergence of velocities, so I modified a little which then becomes:

![equation](http://www.sciweavers.org/tex2img.php?eq=c%20%3D%20F%20%2A%20exp%28-%5Cfrac%7B%7B%28x%20-%20x_p%29%7D%5E2%20%2B%20%7B%28y%20-%20y_p%29%7D%5E2%7D%7Br%7D%29%0A&bc=White&fc=Black&im=jpg&fs=12&ff=arev&edit=0)  