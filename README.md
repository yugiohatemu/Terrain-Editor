Terrain-Editor
==============

CS488 final project
Environment: gtkmm and openGL on Linux 

Manual

Application --  
    Randomize -- regenrate a map with a random max height limit.
	Quit -- quit the appliction

Mode -- 
	God mode -- press and move left mouse button to rotate on z axis
			 -- press and movemiddle mouse button to rotate on y axis
			 -- press and moveright mouse button to zoom in or mout
	Normal mode
			 -- Keyboard input w,s,a,d correspondent to move forward, backward, left and right
			 -- mouse x motion can rotate the scene

Light --
	Light Position -- Keyboard input w,s,a,d correspondent to move light position forward, backward, left and right
	Water Reflection -- Show water reflection effect without the terrain
	Water Shadow -- Show water shadow effect without the terrain

Environment-- 
	Select differnt kinds of texture mapping for terrain

Water Speed--
	Adjust the speed of water

Weather -- 
	Noter, weather effect is only shown in normal mode
	Select rain, snow or no particle system effect
  
Implementation Detail

To generate a terrain, I started from generating a plain grid graph with size 250 x 250. The data structure is a height map ie. a multidimensional array.. I also implanted a normal map which store the normal for each grid(quad), to reduce the computation of normal (for light).

The algorithm I choose to generate a terrain is Circle algorithm. The algorithm will choose a random point to be the circle center, and select a random radius (I have set the maximum radius to be 80). Then

for each point (tx,ty,tz) do
  pd = distance from circle center * 2 / circleRadius
	if fabs(pd) <= 1.0
		tz +=  disp/2 + cos(pd*3.14)*increaseHeight/2;
Repeat this operation for a several times. The number of times depends on the size of the terrain , circle radius and maximum increaseHeight.

The increaseHeight is decreased after each iteration to reduce the height difference between bumps. It will stop decreasing after it reaches the minimum increaseHeight. 
To make the terrain appears smoother, I defined a smoothing filter k, which is a constant between 0 and 1. For any point in the terrain, its new height will be computed taking into account the height of one of its neighbours.

height(x,y) = (1-k)* height(neighbour of (x,y)) + k*(height(x,y))

Since every grid has at most 4 neighbour, apply the smoothing function from left to right, right to left, up to bottom and bottom to up for each point in the terrain.

Manual generation mode:
The user can select a point in the terrain by clicking and select a circle radius to generate a bump.
However, this feature is failed to be implanted due to the difficulty of calculating the exact point being clicked on the terrain map.

Texture Mapping - Bump Mapping

To implant the bump mapping on terrain surface, I choose the idea of using multitexturing to create a bump map. The idea is to mix a colour map and a normal map together to create a normal texture map. Read colours from both map, for each point in the texture, computed of the resulted color of a texture map and a normal map.i.e

R on texture[i][j] =( R on color[i][j] )*(R on normal[i][j]) //Do the same for G and B
Normalize the (R,G,B) of texture[i][j]

Use the resulted texture for bump mapping, use GL_MOULATE for  GL_TEXTURE_ENV_MODE.
Transparent Water 

Transparency of water is done using multi texture mapping and blending.
The texture for water is special RGBA texture map. To create a texture map, read RGB from a water texture and read alpha from an alpha texture. Merge the two texture data based on RGBA order, we now have a texture which contains RGBA. 
In addition,  I generate the texture coordinates based on a sphere surface instead of a plane surface, ie.

glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

glTexParameteri setting is similar to normal texture mapping. Finally, use use GL_BLEND for  GL_TEXTURE_ENV_MODE. In this way, the texture mapping on the surface has a nice wave foam effect.

To implanted blending, normally we need to sort based on alpha values of each object on the scene. However, since the water is the only object that is transparent, we can just draw it in the last, with blending
function glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA) 

Special effect: 
Using glBlendFunc(GL_ONE, GL_ONE), we create a snow terrain. 

Wave

The wave effect is done using cosine function and perlin noise. The data structure is also a height map, for each frame, update the height based on the time and recompute the normal for lighting.

Reflection and Shadow

The reflection and shadow is done using the stencil buffer. The idea take the water surface as the drawing area, disable the depth buffer and  use stencil buffer to draw the terrain on the surface. Apply the material if this is a reflection, set the material to be black if we are drawing shadow.
The reflection/shadow has to be drawing before the depth buffer. The order is

stencil buffer -> depth buffer -> alpha blend

However, due to the material of the terrain, it is really hard to see the reflection/shadow effect. I have to exclude the terrain in order to show these effects.

SkyBoxes

Skyboxes is a relatively easy texture mapping. The idea is to build a box around the outer edge of terrain, which is left, right, front, back and top. We do not need the bottom since the terrain covers it. 
Disable lighting when drawing the skyboxes and manually map the texture coordinate with the box coordinate.

Particle system

The particle system I have implanted are snow and water, both are implanted using the same particle system but with different primitives, lines and quads respectively.
To generate a random particle system, in initialization, for each particle, pick a random coordinate and update the height based on time frame. When the particle is below the visible height, I recycle the particle by setting its height to be the maximum.
The particle system has only a size of 20X20X10. However, if the particle system is enabled on exploration mode, it will give user an illusion of having particle effect on the whole terrain. This can be achieved by applying certain graphic transformation. 
First, when user enters to normal mode, we can place user in the center of the particle system. Then based on user’s movement, we can apply the same transformation to the particle system, ie, the user move 1 step forward, we move the article system 1 step forward. In this way, the particle system is always inside the viewing frustum.  I think this is a reasonable twist to save memory space and reduce computation.
 
Third Person Exploration Mode

Third person exploration is all about calculating the height of the terrain based on the height map and 
3D collision.  We can get the height of the current point on terrain based on the height map of the terrain.

Besides heightmap, I use bounding volume for 3D collision. The idea of 3D collision is similar to ray tracing. Cast a ray from the center of the sphere, based on the players fov and angle, we can get an intersection point. Calculate the distance between the intersection and the player’s position. As a result, we can find out whether the player will collide with the terrain. 


 
