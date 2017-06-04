### Intruduction
- This is a an implementation of the article [*Fast Fluid Dynamics Simulation on the GPU*](https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch38.html) on GPU Gems.
- There is a [YouTube video](https://youtu.be/YikGESU-z6c) demonstrating the use of this program.

### Compile
- This program has been tested with VS 2015 on Win10 64bit. To compile it, you need the following libraries:
    - GLM(https://github.com/g-truc/glm)
    - glew(http://glew.sourceforge.net/)
    - glfw(http://www.glfw.org/)

### Issues
At Section Force Application in the article, the equation used to calculate the additive velocity due to external forces is wrong. Using the original equation will result in divergence of velocities, so I simply added a minus sign within exp which then becomes:

c = F * exp{ - [(x-xp)^2 + (y-yp)^2] / r}
