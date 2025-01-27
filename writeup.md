# Write up
Place to record development activity/thoughts/resources

### 11/19 - 7:34 pm
* Mesh files used were obtained from [models-resource](https://www.models-resource.com/nintendo_64/starfox64/)
* Had to spend more time than I'd like to admit figuring how mtl files work and extending *Shape* to load in material files as well.
* Not sure how to render environment. I can't seem to find any meshes online. I'd imagine its **not** a skybox. I might start off creating some elongated surface in maya and just tile them as the game moves forward. I could also just create a water floor and clouds. It looks like the water environment has reflection which might be tricky (or not, just rendering the scene reflected over the horizon into the fbo for the water)
* Ended up chaning A LOT of the base code. Going pretty well so far.

### 11/20 - 12:12am
* Finished fixing up Shape
* Added simple ground texture which scrolls over time (texCoord + offset.y). Just uses a generic water texture on a quad that wraps
* Added camera controls (mostly pulling from asgn4). I had intended for wasd to be ship controls but I moved those to arrow keys. U key unlocks the camera (ship remains fixed in XY plane) but the camera can move around the scene. I figured this would help with debugging.
* Right now alot of the game state is in the Application class which is pretty disorganized. Should probably start factoring code into distinct entity classes.
* This ship pitch/yaw/planar-movement is pretty clunky but it works properly; eventually that will need to be cleaned up.
* Feeling pretty good about the project so far


### 11/20 2:49pm
* Tried to get phong working with textures (material/texture info loaded from mtl files). The best I could do was get some strange coloration -- not sure why :(. Gave up on it for now and reverted to using just textures with no lighting.
* Frustrated with ^^^, skipping for now. Next thing is to structure the existing code and implement some basic collision detection with the ground.

### 11/20 10:44pm
* Factored out arwing into its own class. Surprisingly went without a hitch.
* Factored out environment code into its own class.
* Reworked ship movement and now it looks GREAT! Smooth as it could be.


### 11/22 1:00am
* Fixed ship controls and made camera follow ship.
* Reworked environment and added backdrop.
* Cleaned up a lot of code.

### 11/22 3:00pm
* Added random-spawn enemy arwings that linear-interp towards the camera
* Added basic collision detection between the Arwing and ground/enemies

### 11/22 8:48pm
* Created obj/mtl file for projectiles which is consistent with original ship projectiles
* Added ship projectiles which look and feel close the the original game
* Added projectile-enemy collision detection and eenemy despawn
* Refactored camera/ship stuff, still not perfect but good enough

### 11/22 11:48pm
* Added crosshairs (not sure how to do without geom. shaders)

### 11/23 9:00pm
* Fixed enemy, crosshair, and ship rotations
* Added rolling and cleaned up ship control code
* Began adding interface (really unsure of how to render text/semi-transparent textures; same problem as crosshairs -- geom shader?)


## Update
* Since coming back to school, been working on this on & off. Forgot to update this doc
* By monday I plan to implement:
  * Ship exhaust lighting
  * Enemy Explosion animations when hit by projectile
  * Enemies following splines
  * Some kind of Interface
  * Arwing health and explosion animation
  * Enemy projectiles
* If I have time, I'd also like to implement:
  * Simple random (can't even call it procedural) terrain generation
  * Rings
  * Water reflection of scene
  * Audio
  * Part of the land portion of the level


### 11/28 11:00pm
* Fixed lighting
* Fixed crosshairs
* Added simple exhaust lighting
* Added groundwork for the interface
* Tried rushing through some enemy spline paths, but reverted -- will do soon


### 11/30 12:00pm
* Tried to render interface with ortho perspective, almost working but following camera pitch/yaw is difficult
* Tried to add flicker effect to exhaust but I think its looks better without. Cleaned up a bit though.
* Added barrel roll!

### 12/1 11:00pm
* Added simple explosion effects to enemies when hit (they burst into many brown particles with relatively correct velocities and subject to gravity)

### 12/2 12:00pm
* Added splines to enemies, can have any number of interior (between start and end) points
* Wierd bug with splines where they pause for awhile at vertices

## Update
I'm going to stop updating this doc because my life is now a tornado of misery and despair, its ok tho :). See commit messages for updates.
