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

```math

c = F*exp(-\frac{(x - x_p)^2 + (y - y_p)^2}{r})

```